#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <algorithm>
#include <string>
#include <mutex>
#include <future>
#include <vector>
#include <thread>

#define _MACROSTR(x) #x
#define MACROSTR(x) _MACROSTR(x)
#define BUILDTIME MACROSTR(XUE_COMPILE_TIME)

using namespace std;

// 获取时间
long GetCurSec();

// 执行shell 命令 返回结果
string GetCMDRet(string cmd);

// 解码url
string UrlDecode(string str);

// 发送文件
void send_file(struct evhttp_request *req, const char *file_path);

// 处理回调
void on_request(struct evhttp_request *req, void *arg);

// 格式化json
string GetLevelStr(int level);
string FormatJsonToDisplay(string json);

// 重启进程任务
std::future<void> kill_future;

std::future<string> shell_future;
// 上一次重启时间
long lastStartAIITime = GetCurSec();

static string env_lib = "export PATH=/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/bin:/sbin && export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/lib && ";
int main()
{
    // 初始化libevent库
    event_base *base = event_base_new();
    if (!base)
    {
        std::cerr << "Failed to initialize event base." << std::endl;
        return 1;
    }

    // 创建HTTP服务器
    evhttp *http_server = evhttp_new(base);
    if (!http_server)
    {
        std::cerr << "Failed to create HTTP server." << std::endl;
        return 1;
    }

    // 设置请求处理回调函数
    evhttp_set_gencb(http_server, on_request, nullptr);

    // 绑定监听地址和端口
    if (evhttp_bind_socket(http_server, "0.0.0.0", 10000) != 0)
    {
        std::cerr << "Failed to bind to port 10000." << std::endl;
        return 1;
    }

    std::cout << "Server started on port 10000." << std::endl;

    // 进入事件循环
    event_base_dispatch(base);

    // 清理资源
    evhttp_free(http_server);
    event_base_free(base);

    return 0;
}

void on_request(struct evhttp_request *req, void *arg)
{
    // 解析URL
    const char *url = evhttp_request_get_uri(req);
    string curl(url);

    std::cout << "Received request for URL: " << curl << "\n";
    curl = UrlDecode(curl);
    std::cout << "Decode  URL: " << curl << "\n";
    string ret = curl + " ok ";
    if (curl.find("getfile") != string::npos)
    {
        const evhttp_uri *uri = evhttp_request_get_evhttp_uri(req);
        char url[1024];
        evhttp_uri_join(const_cast<evhttp_uri *>(uri), url, sizeof(url));

        struct evkeyvalq params;
        evhttp_parse_query(url, &params);

        const char *file_param = evhttp_find_header(&params, "getfile");
        if (!file_param)
        {
            evhttp_send_error(req, HTTP_BADREQUEST, "Bad request");
            evhttp_clear_headers(&params);
            return;
        }
        printf("getfile ok \n");
        send_file(req, file_param);
        evhttp_clear_headers(&params);
        return;
    }
   

    if (curl.find("exec_shell") != string::npos)
    {
        string cmdstr = "";
        if (curl.find("=") != string::npos && curl.find("=") + 1 < curl.size())
        {
            cmdstr = curl.substr(curl.find("=") + 1);
        }

        printf("exec_shell cmd %s\n", cmdstr.c_str());
        string querystr = cmdstr;

        ret = GetCMDRet(querystr);
    }
    if (curl.find("query_buildtime") != string::npos)
    {
        string str = "AIIDaemon buildtime :";
        string cmdstr(BUILDTIME);
        ret = str + cmdstr;
    }

    if (curl.find("query_disk") != string::npos)
    {
        string querystr = R"(df -h)";
        ret = GetCMDRet(querystr);
    }
    if (curl.find("query_ipcs") != string::npos)
    {
        string querystr = R"(ipcs -m)";
        ret = GetCMDRet(querystr);
    }

   
    // 处理请求
    // 这里可以根据你的需求进行具体的处理逻辑

    ret += "\n";
    // 发送响应
    struct evbuffer *response_buffer = evbuffer_new();
    if (response_buffer)
    {
        evbuffer_add_printf(response_buffer, ret.c_str());

        evhttp_send_reply(req, HTTP_OK, "OK", response_buffer);

        evbuffer_free(response_buffer);
    }
    else
    {
        std::cerr << "Failed to create response buffer." << std::endl;
    }
}

void send_file(struct evhttp_request *req, const char *file_path)
{
    struct stat st;
    if (stat(file_path, &st) < 0)
    {
        evhttp_send_error(req, HTTP_NOTFOUND, "File not found");
        return;
    }

    int fd = open(file_path, O_RDONLY);
    if (fd < 0)
    {
        evhttp_send_error(req, HTTP_NOTFOUND, "File not found");
        return;
    }

    struct evbuffer *evb = evbuffer_new();
    evbuffer_add_file(evb, fd, 0, st.st_size);
    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

long GetCurSec()
{
    time_t seconds;

    seconds = time(NULL);
    return seconds;
}

string GetCMDRet(string cmd)
{

    auto this_future = std::async(std::launch::async, [cmd]
                        {
                        string s = "";
                        char buf[5000];
                        FILE *p_file = NULL;
                        string com = env_lib + cmd;
                        p_file = popen(com.c_str(), "r");
                        if (!p_file)
                        {
                            fprintf(stderr, "Erro to popen");
                        }
                        while (fgets(buf, 500, p_file) != NULL)
                        {
                            s += buf;
                        }
                        pclose(p_file);
                        return s; 
                        });
    string ret = "";
    std::future_status status = this_future.wait_for(std::chrono::milliseconds(700));
    if (status == std::future_status::deferred)
    {
        ret = "shell cmd  not exec";
    }
    else if (status == std::future_status::timeout)
    {
        ret = "shell cmd  exec timeout";
    }
    else if (status == std::future_status::ready)
    { 
        ret = this_future.get();
    }
    shell_future = std::move(this_future);
    return ret;
}

string UrlDecode(string str)
{
    string ret;
    char ch;
    int i, ii, len = str.length();

    for (i = 0; i < len; i++)
    {
        if (str[i] != '%')
        {
            if (str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }
        else
        {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}
string getString(char x)
{
    // string class has a constructor
    // that allows us to specify size of
    // string as first parameter and character
    // to be filled in given size as second
    // parameter.
    string s(1, x);

    return s;
}
string GetLevelStr(int level)
{
    string levelStr = "";
    for (int i = 0; i < level; i++)
    {
        levelStr += "    ";
    }
    return levelStr;
}

string FormatJsonToDisplay(string json)
{
    string result = "";
    int level = 0;
    for (string::size_type index = 0; index < json.size(); index++)
    {
        char c = json[index];

        if (level > 0 && '\n' == json[json.size() - 1])
        {
            result += GetLevelStr(level);
        }

        switch (c)
        {
        case '{':
        case '[':
            result = result + getString(c) + "\n";
            level++;
            result += GetLevelStr(level);
            break;
        case ',':
            result = result + getString(c) + "\n";
            result += GetLevelStr(level);
            break;
        case '}':
        case ']':
            result += "\n";
            level--;
            result += GetLevelStr(level);
            result += getString(c);
            break;
        default:
            result += getString(c);
            break;
        }
    }
    return result;
}