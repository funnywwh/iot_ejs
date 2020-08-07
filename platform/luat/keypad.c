#include "keypad.h"
#include "iot_keypad.h"
#include "log.h"
static KEYPAD_CALLBACK s_keys_upper_cb = 0;
static VOID gpio_key_cb(T_AMOPENAT_KEYPAD_MESSAGE *pKeypadMessage){
    if(s_keys_upper_cb){
        s_keys_upper_cb(pKeypadMessage->nType,pKeypadMessage->data.gpio.r,pKeypadMessage->data.gpio.c,pKeypadMessage->bPressed);
    }else{
        ERROR("s_keys_upper_cb is null.");
    }
}
int platform_setup_m_keys(int rowMark,int colMark,KEYPAD_CALLBACK cb){
    T_AMOPENAT_KEYPAD_CONFIG cfg;
    cfg.type = OPENAT_KEYPAD_TYPE_MATRIX;
    cfg.pKeypadMessageCallback = gpio_key_cb;
    cfg.config.matrix.keyInMask = rowMark;
    cfg.config.matrix.keyOutMask = colMark;
    BOOL err = iot_keypad_init(&cfg);   
    if(!err){
        ERROR("iot_keypad_init");
        return -1;
    }
    s_keys_upper_cb = cb;
    return 0;
}


int platform_setup_gpio_keys(int rowMark,int colMark,E_GPIOKEY_MODE mode,KEYPAD_CALLBACK cb){
    T_AMOPENAT_KEYPAD_CONFIG cfg;
    cfg.type = OPENAT_KEYPAD_TYPE_GPIO;
    cfg.pKeypadMessageCallback = gpio_key_cb;
    cfg.config.gpio.mode = mode;
    cfg.config.gpio.gpiofirstcfg = TRUE;
    cfg.config.gpio.gpioInMask = rowMark;
    cfg.config.gpio.gpioOutMask = colMark;
    BOOL err = iot_keypad_init(&cfg);   
    if(!err){
        ERROR("iot_keypad_init");
        return -1;
    }  
    s_keys_upper_cb = cb;
    return 0;
}
