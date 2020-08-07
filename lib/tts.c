#include "duktape.h"
#include "tts.h"
#include "utils.h"
static duk_ret_t tts(duk_context *ctx) {
    if(!duk_is_string(ctx,0)){
        return DUK_RET_TYPE_ERROR;
    }
    const char* tts_text = duk_to_string(ctx,0);
    platform_tts_say(tts_text,strlen(tts_text));
    return 0;
}
static duk_function_list_entry module_funcs[] = {
    {"say",tts,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int tts_init(duk_context* ctx){
    platform_tts_iot_init();
    return register_module_funcs(ctx,"iot.tts",module_funcs);
}