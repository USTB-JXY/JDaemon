#!/bin/sh
killall startJDaemon
killall startJDSafe
killall JDaemon


mv startJDaemon /home/jin
mv JDaemon /home/jin/
mv startJDSafe  /home/jin 

#/etc/rc.local  文件中是否存在该字符串
if ! grep "/home/jin/startJDaemon" /etc/rc.local  >/dev/null
then
#不存在，添加
   sed -i '/^exit 0/i /home/jin/startJDaemon start' /etc/rc.local 
else
#存在，不做处理
   echo "startJDaemon command already exists /etc/rc.local "
fi
/bin/sh -c "/home/jin/startJDaemon start"
sleep 1
ps -ef | grep J

echo "install finsh !"

