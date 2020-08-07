#include "duktape.h"
#include "fsdep.h"
#include "utils.h"
#include "log.h"
#include "iot_module.h"
int tts_init(duk_context* ctx);
int audio_init(duk_context* ctx);
int network_init(duk_context* ctx);
int os_init(duk_context* ctx);
int misc_init(duk_context* ctx);
int fs_init(duk_context* ctx);
int uart_init(duk_context* ctx);
int g711a_init(duk_context* ctx);
int udp_init(duk_context* ctx);
int gpio_init(duk_context* ctx);
int keypad_init(duk_context* ctx);
int pwd_init(duk_context* ctx);
static int init_module(duk_context* ctx){
    // debugger_init(ctx);
    pwd_init(ctx);
    os_init(ctx);
    register_iot_module(ctx);
    tts_init(ctx);
    audio_init(ctx);
    network_init(ctx);
    udp_init(ctx);   
    misc_init(ctx);
    fs_init(ctx);
    uart_init(ctx);
    g711a_init(ctx);
    gpio_init(ctx);
    keypad_init(ctx);
}
extern duk_ret_t native_print(duk_context *ctx);
extern duk_ret_t native_set_timeout(duk_context* ctx);
extern duk_ret_t native_clearTimeout(duk_context* ctx);
int eval_file(duk_context *ctx,const char* filename);
int load_init(duk_context* ctx);



int duktape_init(duk_context* ctx){
    if(load_init(ctx)){
        debugf("port load init failed");
        return -2;
    }
    // eval_file(ctx,"/js/lib/iot.js");
    duk_peval_lstring_noresult(ctx,js_lib_iot_js,sizeof(js_lib_iot_js));

    duk_push_c_function(ctx, native_print, 1 /*nargs*/);
    duk_put_global_string(ctx, "print");

    duk_push_c_function(ctx, native_set_timeout, 2 /*nargs*/);
    duk_put_global_string(ctx, "setTimeout");

    duk_push_c_function(ctx, native_clearTimeout, 2 /*nargs*/);
    duk_put_global_string(ctx, "clearTimeout");
    init_module(ctx);
    return 0;
}

void duktape_uninit(duk_context* ctx){
    if(ctx){
        duk_destroy_heap(ctx);
    }
}
