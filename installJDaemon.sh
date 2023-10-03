#!/bin/sh
killall startJDaemon
killall startJDSafe
killall JDaemon


mv startJDaemon /home/jrun
mv JDaemon /home/jrun/
mv startJDSafe  /home/jrun 

if [ ! -f /etc/rc.local ]; then
    touch /etc/rc.local
    chmod 755 /etc/rc.local
    echo '''#!/bin/bash''' >> /etc/rc.local
    chmod +x /etc/rc.local
    systemctl start rc-local
    systemctl enable rc-local
fi
# /etc/rc.local  文件中是否存在该字符串
if ! grep "/home/jrun/startJDaemon" /etc/rc.local >/dev/null
then
#不存在，添加
   echo "/home/jrun/startJDaemon start" >> /etc/rc.local
   chmod +x /etc/rc.local

else
#存在，不做处理
   echo "startJDaemon command already exists /etc/rc.local "
fi

/bin/sh -c "/home/jrun/startJDaemon start"
sleep 1
ps -ef | grep J

echo "install finsh !"

