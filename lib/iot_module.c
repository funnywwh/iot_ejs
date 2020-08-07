#include "duktape.h"
#include "iot_module.h"
#include "utils.h"
static duk_function_list_entry module_funcs[] = {
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};

duk_ret_t register_iot_module(duk_context* ctx){
    return register_module_funcs(ctx,"iot",module_funcs);
}