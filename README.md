# JDaemon
一个后台进程，使用libevent 链接静态库方便部署

x86 编译命令
make x86

arm 编译命令
make arm

# 统一安装命令
tar -zxvf JDaemon-release.tgz
./installJDaemon.sh

启动服务 
/home/jin/startJDaemon start 


# 介绍
主要支持  获取文件 执行简单的shell命令 
基于libevent,支持 get post, 支持wget 和curl (curl 里 url特殊字符 需要转义 空格 转义 %20 /转义 %2F)
# 使用示例


# 清理磁盘示例
root@jxy:~$ wget -q -O - 192.168.145.66:10000/clean
/clean ok

root@jxy:~$ curl --location --max-time 3 --request POST '192.168.145.66:10000/clean'
/clean ok

#  获取文件示例 (注意curl 需要转义 且没有" ") 空格 转义 %20 /转义 %2F
root@jxy:/$ wget -O libHttpInterface.so 192.168.145.66:10000/?getfile=/opt/AII/libHttpInterface.so
Connecting to 192.168.145.66:10000 (192.168.145.66:10000)
saving to 'libHttpInterface.so'
libHttpInterface.so  100% |*****************************| 2898k  0:00:00 ETA
'libHttpInterface.so' saved


root@jxy:/$ ls
libHttpInterface.so

root@jxy:~$ curl -o libHttpInterface.so --location --max-time 3 --request POST '192.168.145.66:10000/?getfile=%2Fopt%2FAII%2FlibHttpInterface.so'
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100 2898k  100 2898k    0     0  8064k      0 --:--:-- --:--:-- --:--:-- 8051k


root@jxy:~$ ls
libHttpInterface.so  out.txt              query_disk

# 执行shell 示例 (注意curl 需要转义 且没有" ") 空格 转义 %20 /转义 %2F
root@jxy:~$ wget -q -O - 192.168.145.66:10000/?exec_shell="cat /proc/version"
Linux version 4.19.148 (root@xxx) (gcc version 8.4.0 (Buildroot 2020.02.9-23-g81e333f)) #5 SMP PREEMPT Sat Jul 29 11:40:33 CST 2023


root@jxy:~$ wget -q -O - 192.168.145.66:10000/?exec_shell="ifconfig"
eth0      Link encap:Ethernet  HWaddr 4A:8D:67:C1:6F:3D  
          inet addr:192.168.145.66  Bcast:192.168.145.255  Mask:255.255.255.0
          inet6 addr: fe80::488d:67ff:fec1:6f3d/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:7595261 errors:0 dropped:1943 overruns:0 frame:0
          TX packets:1111948 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:849394598 (810.0 MiB)  TX bytes:18280681092 (17.0 GiB)
          Interrupt:28 

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:4495503 errors:0 dropped:0 overruns:0 frame:0
          TX packets:4495503 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:18526466067 (17.2 GiB)  TX bytes:18526466067 (17.2 GiB)


root@jxy:~$ curl --location --max-time 3 --request POST '192.168.145.66:10000/?exec_shell=cat%20%2Fproc%2Fversion'
Linux version 4.19.148 (root@xxx) (gcc version 8.4.0 (Buildroot 2020.02.9-23-g81e333f)) #5 SMP PREEMPT Sat Jul 29 11:40:33 CST 2023

root@jxy:~$ curl --location --max-time 3 --request POST '192.168.145.66:10000/?exec_shell=ifconfig'
eth0      Link encap:Ethernet  HWaddr 4A:8D:67:C1:6F:3D  
          inet addr:192.168.145.66  Bcast:192.168.145.255  Mask:255.255.255.0
          inet6 addr: fe80::488d:67ff:fec1:6f3d/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:10634374 errors:0 dropped:2733 overruns:0 frame:0
          TX packets:1583949 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:1191034352 (1.1 GiB)  TX bytes:25709259069 (23.9 GiB)
          Interrupt:28 

lo        Link encap:Local Loopback  
          inet addr:127.0.0.1  Mask:255.0.0.0
          inet6 addr: ::1/128 Scope:Host
          UP LOOPBACK RUNNING  MTU:65536  Metric:1
          RX packets:6329738 errors:0 dropped:0 overruns:0 frame:0
          TX packets:6329738 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:26120233690 (24.3 GiB)  TX bytes:26120233690 (24.3 GiB)


