#include "duktape.h"
#include "network.h"
#include "utils.h"

static duk_ret_t native_wait_network_ready(duk_context *ctx) {
    duk_idx_t argn = duk_get_top(ctx);
    duk_int_t timeout_sec =  -1;
    duk_bool_t ready = 0;
    if(argn > 0){
        timeout_sec = duk_get_int_default(ctx,0,-1);
    }
    ready = platform_wait_network_ready(timeout_sec);
    duk_push_boolean(ctx,ready);
    return 1;
}

static duk_function_list_entry module_funcs[] = {
    {"wait_network_ready",native_wait_network_ready,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};


int network_init(duk_context* ctx){
    platform_network_init();
    
   return register_module_funcs(ctx,"iot.network",module_funcs);
}