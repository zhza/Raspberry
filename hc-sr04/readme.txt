#编译测试
sudo gcc hc-sr04.c -o test -I./ -lwiringPi -lpthread -D__DEBUG__
#测试
./test 


#编译成动态库
sudo gcc hc-sr04.c -shared -fPIC -o libhc-sr04.so  -I./ -lwiringPi -lpthread

