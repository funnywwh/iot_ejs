#include "duktape.h"
#include "utils.h"
#include "misc.h"
const char* platform_get_imei();
static duk_ret_t native_get_imei(duk_context *ctx) {
    
    duk_push_string(ctx,platform_get_imei());
    return 1;
}

static duk_function_list_entry module_funcs[] = {
    {"getImei",native_get_imei,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};


int misc_init(duk_context* ctx){
   platform_misc_init();
   return register_module_funcs(ctx,"iot.misc",module_funcs);
}