#ifndef _HC_SR04
#define _HC_SR04

#define TRIG	(25)
#define ECHO	(24)

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (*hcsr04_callback)(double distance);


/**
 * 超声波模块注册函数(只应调用一次)
 * 
 * callback 注册回调
 * milliseconds 探测间隔(毫秒)，建议不低于100ms，低于100ms经测试会出现不再回应电平信号现象
 * accuracy 两次测量精度阀值(cm)，建议不低于0.5cm
 */
int  hcsr04_register(hcsr04_callback callback,unsigned int milliseconds,double accuracy);

/**
 * 开始进行测量
 */
int  hcsr04_start();
/**
 * 停止测量
 */
void hcsr04_stop();

#if defined(__cplusplus)
}
#endif
#endif