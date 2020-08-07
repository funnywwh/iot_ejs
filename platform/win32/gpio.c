#include "gpio.h"
static GPIO_INT_CALLBACK s_gpio_int_cb = 0;


//TODO: 线程保护

int platform_gpio_setup(int pin,int mode,int pullUpOrDown,int intType,int debounce/*millseconds*/,GPIO_INT_CALLBACK cb,void* udata){
 
    return 0;
}
int platform_gpio_output(int pin,int val){
    return 0;
}
//读gpio管脚
//返回 0：低电平 1:高电平
int platform_gpio_input(int pin){
    return 0;
}

int platform_gpio_close(int pin){
    return 0;
}
