_XUE_COMPILE_TIME = $(shell date +"%Y%m%d%H%M%S")
x86:
	g++  -fsanitize=leak -I/usr/local/libevent2.1/include/ -DSSIZE_T=size_t -DX86 -DXUE_COMPILE_TIME=$(_XUE_COMPILE_TIME)   JDaemon.cpp -o JDaemon -lpthread  ./x86/libevent.a
	cp x86/x86-installJDaemon.sh  ./installJDaemon.sh 
	make release
single:
	g++  -fsanitize=leak -I/usr/local/libevent2.1/include/ -DSSIZE_T=size_t -DX86 -DXUE_COMPILE_TIME=$(_XUE_COMPILE_TIME)   JDaemon.cpp -o JDaemonS -lpthread  ./x86/libevent.a
test:
	g++  -fsanitize=leak -fpermissive -I/usr/local/libevent2.1/include/ -DSSIZE_T=size_t -DX86 -DXUE_COMPILE_TIME=$(_XUE_COMPILE_TIME)   JEvent-test.cpp -o JEvent-test -lpthread  ./x86/libevent.a
clang:
	clang++  -v -fsanitize=memory -I/usr/local/libevent2.1/include/ -DSSIZE_T=size_t -DX86 -DXUE_COMPILE_TIME=$(_XUE_COMPILE_TIME)   JDaemon.cpp -o JDaemon -lpthread  ./x86/libevent.a
	cp x86/x86-installJDaemon.sh  ./installJDaemon.sh 
	make release
arm:
	g++ -I/usr/local/libevent2.1/include/ -DSSIZE_T=size_t  -DARM -DXUE_COMPILE_TIME=$(_XUE_COMPILE_TIME)  JDaemon.cpp -o JDaemon -lpthread  ./arm/libevent.a
	cp arm/arm-installJDaemon.sh  ./installJDaemon.sh 
	make release

release:
	rm -rf JDaemon-release.tgz
	chmod +x startJDaemon
	chmod +x startJDSafe
	chmod +x JDaemon
	chmod +x installJDaemon.sh
	tar -zcvf JDaemon-release.tgz JDaemon  installJDaemon.sh  startJDaemon startJDSafe
remove:
	@if [ -e /home/jrun/startJDaemon ]; then /home/jrun/startJDaemon stop; fi
	rm -rf 	/home/jrun/JDaemon
	rm -rf 	/home/jrun/startJDaemon
	rm -rf 	/home/jrun/startJDSafe
install:
	@if [ -e /home/jrun/startJDaemon ]; then /home/jrun/startJDaemon stop; fi
	chmod +x startJDaemon
	chmod +x startJDSafe
	cp JDaemon /home/jrun/
	cp startJDaemon /home/jrun
	cp startJDSafe /home/jrun
start:
	/home/jrun/startJDaemon start
stop:
	/home/jrun/startJDaemon stop
clean:
	rm -rf installJDaemon.sh
	rm -rf JDaemon-release.tgz
	rm -rf JDaemon
push:
	git add ./*
	git commit -m 'noraml update'
	git push -u origin main

.PHONY: x86 arm single install clean remove release  start stop clang test push


