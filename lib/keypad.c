#include "duktape.h"
#include "keypad.h"
#include "os.h"
#include "log.h"
#include "utils.h"
#include "on_msg.h"
typedef struct KeypadMsg_t{
    int type;
    int row;
    int col;
    int press;//1 press,0 up
}KeypadMsg;

static void* to_task = 0;
static void keypad_cb(int type,int row,int col,int press){
    if(!to_task){
        debugf("keypad_cb to_task is null");
        return;
    }
    Msg* pMsg = (Msg*)malloc(sizeof(Msg));
    if(!pMsg){
        debugf("keypad_cb malloc Msg failed");
        return ;
    }
    KeypadMsg* pKeypadMsg = (KeypadMsg*)malloc(sizeof(KeypadMsg));
    if(!pKeypadMsg){
        free(pMsg);
        debugf("keypad_cb malloc KeypadMsg failed");
        return ;
    }
    pKeypadMsg->type = type;
    pKeypadMsg->row = row;
    pKeypadMsg->col = col;
    pKeypadMsg->press = press;
    pMsg->id = MSG_KEYPAD_CB;
    pMsg->args = pKeypadMsg;
    if(platform_send_msg(to_task,pMsg)){
        free(pKeypadMsg);
        free(pMsg);
        ERROR("platform_send_msg failed");
        return ;
    }
}
const char* get_keypad_prop_key(int type){
    char* key = "keypad_unknown";
    switch (type)
    {
    case KEYPAD_TYPE_MATRIX:
        key = "keypad_m_cb";
        break;
    case KEYPAD_TYPE_GPIO:
        key = "keypad_gpio_cb";
        break;
    case KEYPAD_TYPE_ADC:
        key = "keypad_adc_cb";
        break;
    default:
        break;
    }
    return key;
}
int on_keypad_callback_msg(duk_context* ctx,Msg* msg){
    if(msg == 0 || msg->args == 0){
        ERROR("msg == 0 || msg->args == 0");
        return 0;
    }
    KeypadMsg* pKeypadMsg = (KeypadMsg*)msg->args;

    vget_head_stash_object(ctx,get_keypad_prop_key(pKeypadMsg->type));//[callback]
    if(!duk_is_function(ctx,-1)){
        ERROR(" callback is not function");
        return 0;
    }
    duk_push_int(ctx,pKeypadMsg->row);
    duk_push_int(ctx,pKeypadMsg->col);
    duk_push_boolean(ctx,pKeypadMsg->press>0?1:0);
    duk_pcall(ctx,3);
    free(pKeypadMsg);
    return 0;
}
//[rowMark,colMark,callback]
duk_ret_t setup_m_keys(duk_context* ctx){
    int rowMark = duk_get_int_default(ctx,0,0);
    int colMark = duk_get_int_default(ctx,1,0);
    if(!duk_is_function(ctx,2)){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"callback is not function");
    }
    to_task = platform_current_task();
    if(vsave_head_stash_object(ctx,2,get_keypad_prop_key(KEYPAD_TYPE_MATRIX))){
        duk_error(ctx,DUK_ERR_ERROR,"save keypad callback failed.");
    }
    if(platform_setup_m_keys(rowMark,colMark,keypad_cb)){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"setup MATRIX keypad failed");
    }
    return 0;
}
//[rowMark,colMark,callback]
duk_ret_t setup_gpio_keys(duk_context* ctx){
    int rowMark = duk_get_int_default(ctx,0,0);
    int colMark = duk_get_int_default(ctx,1,0);
    int mode = duk_get_int_default(ctx,2,0);
    if(!duk_is_function(ctx,3)){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg3 is not function");
    }
    to_task = platform_current_task();
    if(vsave_head_stash_object(ctx,3,get_keypad_prop_key(KEYPAD_TYPE_GPIO))){
        duk_error(ctx,DUK_ERR_ERROR,"save keypad callback failed.");
    }
    if(platform_setup_gpio_keys(rowMark,colMark,mode,keypad_cb)){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"setup GPIO keypad failed");
    }    
    return 0;
}
static duk_function_list_entry module_funcs[] = {
    {"setup_m_keys",setup_m_keys,3},
    {"setup_gpio_keys",setup_gpio_keys,4},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int keypad_init(duk_context* ctx){
    register_msg_handler(MSG_KEYPAD_CB,on_keypad_callback_msg);
    register_module_funcs(ctx,"iot.keypad",module_funcs);
    return 0;
}