#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <wiringPi.h>
#include <pthread.h>
#include <semaphore.h>

#include "hc-sr04.h"

pthread_t p;
unsigned int btime,etime;
unsigned int split_time=10;
int near_state=LOW;
int isRunning=0;
sem_t		sig;
hcsr04_callback _callback;
double last_distance=0;
double _accuracy=0.5;

void echo_edge_sigle();
void send_wave_pulse();
void *do_hcsr04_service(void *args);


inline void echo_edge_sigle(){
	int ret=0; 
	double distance=0,diff_dis=0;
	ret=digitalRead(ECHO);
	if(ret==LOW && near_state==HIGH){
		etime=micros();
		distance=34029*(etime - btime)/2000000.0;
		near_state=ret;
		
		if(_callback){
			diff_dis=distance - last_distance;
			//1cm 差距时返回
			if(diff_dis>_accuracy|| diff_dis<-_accuracy){
				printf("echo=%d,dis=%4.3f,last=%4.3f,diff=%4.3f\n",ret,distance,last_distance,diff_dis);
				last_distance=distance;
				_callback(distance);
			}
		}
		//发信完成信号
		sem_post(&sig);
	}else if(ret==HIGH){
		near_state=HIGH;
		btime=micros();
		//printf("echo=%d\n",ret);
	}else{
		//可能会连续收到两个相同的电平信号
		near_state=ret;
	}
}

inline void send_wave_pulse(){
	digitalWrite(TRIG,HIGH);
	usleep(10);
	digitalWrite(TRIG,LOW);
}

int hcsr04_register(hcsr04_callback callback,unsigned int milliseconds,double accuracy){
	//针角初始化
	pinMode(TRIG,OUTPUT);
	pinMode(ECHO,INPUT);
	
	//回调注册及电平监听
	_callback=callback;
	wiringPiISR (ECHO,INT_EDGE_BOTH, &echo_edge_sigle);
	
	//多线程交互信号量注册
	sem_init(&sig,0,0);
	//
	split_time=milliseconds;
	_accuracy=accuracy;
	
	//注意最小阀值
	if(milliseconds<=10) split_time=10;
	if(accuracy<0.5) _accuracy=0.5;
	
	return 1;
}

//启动测距
int hcsr04_start(){
	isRunning=1;
	//直接启动
	pthread_create(&p,NULL,do_hcsr04_service,NULL);
	pthread_join(p,NULL); 
}

//停止测距离
void hcsr04_stop(){
	isRunning=0;
	sem_post(&sig);
}

void *do_hcsr04_service(void *args){
	//第一次需要触发
	send_wave_pulse();
	while(1){
		//等待信号
		sem_wait(&sig);
		if(0==isRunning) break;
		delay(split_time);
		send_wave_pulse();
	}
	return NULL;
}

#ifdef __DEBUG__ 
//外部注册函数
void event_on_distance(double distance){
	printf("Dis=%4.3f\n",distance);
}

int main(){
	wiringPiSetup(); 
	
	//注册回调
	hcsr04_register(event_on_distance,100,0);
	
	//启动测距
	hcsr04_start();
	
	getchar();
	
	return 0;
}
#endif