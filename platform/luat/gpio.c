#include "gpio.h"
#include "iot_gpio.h"
static GPIO_INT_CALLBACK s_gpio_int_cb = 0;

typedef struct PinUserData_t{
    void* udata;
    GPIO_INT_CALLBACK callback;
}PinUserData;
static void* pin_udatas[MAX_GPIO] = {0};
static void gpio_int_cb(E_OPENAT_DRV_EVT evt,E_AMOPENAT_GPIO_PORT gpioPort,unsigned char state){
    if(evt == OPENAT_DRV_EVT_GPIO_INT_IND){
        PinUserData* udata = &pin_udatas[gpioPort];
        if(udata->callback){
            udata->callback(gpioPort,state,udata->udata);
        }
    }
}
//TODO: 线程保护

int platform_gpio_setup(int pin,int mode,int pullUpOrDown,int intType,int debounce/*millseconds*/,GPIO_INT_CALLBACK cb,void* udata){
    T_AMOPENAT_GPIO_CFG  cfg;
    BOOL err;
    PinUserData* pudata = &pin_udatas[pin];
    if(pudata->callback){
        return -1;
    }
    memset(&cfg, 0, sizeof(T_AMOPENAT_GPIO_CFG));
    
    cfg.mode = mode;
    cfg.param.intCfg.intType = intType;
    cfg.param.intCfg.intCb = gpio_int_cb;
    cfg.param.intCfg.debounce = debounce;
    cfg.param.defaultState = pullUpOrDown;

    pudata->udata = udata;
    pudata->callback = cb;
    err = iot_gpio_open(pin, &cfg);

    if (!err){
        pudata->udata = 0;
        pudata->callback = 0;
        return -2;
    }
    return 0;
}
int platform_gpio_output(int pin,int val){
    iot_gpio_set(pin,val);
    return 0;
}
//读gpio管脚
//返回 0：低电平 1:高电平
int platform_gpio_input(int pin){
    int ret = 0;
    iot_gpio_read(pin,&ret);    
    return ret;
}

int platform_gpio_close(int pin){
    PinUserData* pudata = &pin_udatas[pin];
    pudata->udata = 0;
    pudata->callback = 0;
    iot_gpio_close(pin);
}
