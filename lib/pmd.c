#include "duktape.h"
#include "pmd.h"
#include "utils.h"
//[simStartUpMode,nwStartupMode]
static duk_ret_t powerOn(duk_context *ctx) {
    int simStartUpMode = duk_get_int_default(ctx,0,0);
    int nwStartupMode =  duk_get_int_default(ctx,1,0);
    if(!platform_pmd_poweron_system(simStartUpMode,nwStartupMode)){
        duk_push_true(ctx);
    }else{
        duk_push_false(ctx);
    }
    return 1;
}
static duk_ret_t powerOff(duk_context *ctx) {
    if(!platform_pmd_poweroff_system()){
        duk_push_true(ctx);
    }else{
        duk_push_false(ctx);
    }
    return 1;
}
static duk_ret_t enterDeepSleep(duk_context *ctx) {
    duk_bool_t enter = duk_get_boolean_default(ctx,0,0);
    if(!platform_pmd_enter_deepsleep(enter)){
        duk_push_true(ctx);
    }else{
        duk_push_false(ctx);
    }
    return 1;
}
static duk_ret_t getBatteryInfo(duk_context *ctx) {
    BatteryInfo batteryinfo = {0};
    if(platform_get_battery_info(&batteryinfo)){
        duk_push_undefined(ctx);
    }else{
        duk_push_bare_object(ctx);
        duk_push_int(ctx,batteryinfo.battStatus);
        duk_put_prop_string(ctx,-2,"battStatus");
        duk_push_int(ctx,batteryinfo.battVolt);
        duk_put_prop_string(ctx,-2,"battVolt");
        duk_push_int(ctx,batteryinfo.battLevel);
        duk_put_prop_string(ctx,-2,"battLevel");
        duk_push_int(ctx,batteryinfo.chargerStatus);
        duk_put_prop_string(ctx,-2,"chargerStatus");
        duk_push_int(ctx,batteryinfo.chargeState);
        duk_put_prop_string(ctx,-2,"chargeState");
    }
    return 1;
}
static duk_function_list_entry module_funcs[] = {
    {"powerOn",powerOn,2},
    {"powerOff",powerOff,1},
    {"enterDeepSleep",enterDeepSleep,1},
    {"getBatteryInfo",getBatteryInfo,0},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int pwd_init(duk_context* ctx){
    register_module_funcs(ctx,"iot.pmd",module_funcs);
    return 0;
}