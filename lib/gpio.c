#include "duktape.h"
#include "gpio.h"
#include "utils.h"
#include "os.h"
#include "log.h"

//[...,callback,...]
static duk_ret_t save_gpio_irq_callback(duk_context* ctx,int pin,duk_idx_t fidx){   
    return vsave_head_stash_object(ctx,fidx,"gpiocb:%d",pin);
}
duk_ret_t get_gpio_irq_callback(duk_context* ctx,int pin){
    return vget_head_stash_object(ctx,"gpiocb:%d",pin);    
}
duk_ret_t del_gpio_irq_callback(duk_context* ctx,int pin){
    return vdel_head_stash_object(ctx,"gpiocb:%d",pin);    
}

typedef struct GpioIntCallbackParam_t{
    void* to_task;
}GpioIntCallbackParam;
static void gpio_int_cb(int pin,int val,void* to_task){
    if(!to_task){
        debugf("to task is null");
        return ;
    }
    Msg* pMsg = 0;
    GpioMsg* pGpioMsg = 0;
    pMsg = (Msg*)malloc(sizeof(Msg));
    if(!pMsg){
        debugf("gpio int malloc Msg failed.");
        return ;
    }
    pGpioMsg = (GpioMsg*)malloc(sizeof(GpioMsg));
    if(!pGpioMsg){
        free(pMsg);
        debugf("gpio int malloc GpioMsg failed.");
        return ;
    }
    pGpioMsg->pin = pin;
    pGpioMsg->val = val;
    pMsg->id = MSG_GPIO_CB;
    pMsg->args = pGpioMsg;    
    if(platform_send_msg(to_task,pMsg)){
        debugf("gpio int send msg failed. task:%p",to_task);
        free(pGpioMsg);
        free(pMsg);
       return ; 
    }
    return ;

}
//[pin,mode,pullUpOrDown,intType,debounce,int_callback]
static duk_ret_t setup(duk_context *ctx){
    if(!duk_is_number(ctx,0)){//pin
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg0 is not number");
    }
    if(!duk_is_number(ctx,1)){//mode
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg1 is not number");
    }

    int pin = duk_get_int_default(ctx,0,0);
    if(pin < 0 || pin > MAX_GPIO){
        duk_error(ctx,DUK_ERR_RANGE_ERROR,"gpio index out of range.");
    }    
    int mode = duk_get_int_default(ctx,1,0);
    int pullup = duk_get_boolean_default(ctx,2,0);        
    int intType = duk_get_int_default(ctx,3,0);
    int debounce = duk_get_int_default(ctx,4,0);
    void* cur_task = platform_current_task();
    
    if(mode == GPIO_INPUT || mode == GPIO_INPUT_INT){
        if(!duk_is_boolean(ctx,2)){//pullUpOrDown
            duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not boolean");
        }
        if(mode == GPIO_INPUT_INT){
            if(!duk_is_number(ctx,3)){//intType
                duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg3 is not number");
            }        
            if(!duk_is_number(ctx,4)){//debounce
                duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg4 is not number");
            }
            if(!duk_is_function(ctx,5)){//calback
                duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg5 is not function");
            }
            save_gpio_irq_callback(ctx,pin,5);
        }
    } 
    int ret = platform_gpio_setup(pin,mode,pullup,intType,debounce,gpio_int_cb,cur_task);
    duk_push_int(ctx,ret);
    return 0;
}
//[pin]
static duk_ret_t close(duk_context *ctx){
    int pin = duk_get_int_default(ctx,0,-1);
    if(pin < 0 || pin > MAX_GPIO){
        duk_error(ctx,DUK_ERR_RANGE_ERROR,"gpio index out of range.");
    }
    int ret = platform_gpio_close(pin);
    del_gpio_irq_callback(ctx,pin);
    return 1;
}
static duk_ret_t output(duk_context *ctx){
    int pin = duk_get_int_default(ctx,0,0);
    if(pin < 0 || pin > MAX_GPIO){
        duk_error(ctx,DUK_ERR_RANGE_ERROR,"gpio index out of range.");
    }    
    int val = duk_get_int_default(ctx,1,0);
    platform_gpio_output(pin,val);    
    return 0;
}
static duk_ret_t input(duk_context *ctx){
    int pin = duk_get_int_default(ctx,0,0);
    if(pin < 0 || pin > MAX_GPIO){
        duk_error(ctx,DUK_ERR_RANGE_ERROR,"gpio index out of range.");
    }    
    int val = platform_gpio_input(pin);
    duk_push_int(ctx,val ? 1 : 0);
    return 1;
}
static duk_function_list_entry module_funcs[] = {
    {"setup",setup,6},
    {"close",close,1},
    {"output",output,2},
    {"input",input,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int gpio_init(duk_context* ctx){
    register_module_funcs(ctx,"iot.gpio",module_funcs);
    return 0;
}