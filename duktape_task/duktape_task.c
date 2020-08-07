#include "duktape.h"
#include "duktape_task.h"
#include "os.h"
#include "eval_file.h"
#include "on_msg.h"
#include "fsdep.h"
int debugf(const char *fmt, ...);
int duktape_init(duk_context* ctx);
void duktape_uninit(duk_context* ctx);
duk_context * app_init(){
    duk_context *ctx = 0;
    ctx = duk_create_heap_default();
    if (!ctx) {
        debugf("port duktape create heap failed.");
        return -1;
    }

    if(duktape_init(ctx) != 0){
        debugf("port duktape_init failed");
        return -1;
    }
    // eval_file(ctx,"/js/lib/test.js");
    duk_peval_lstring_noresult(ctx,js_lib_test_js,sizeof(js_lib_test_js));

    eval_file(ctx,"/js/app/app.js");
    return ctx;
}
void app_uninit(duk_context *ctx){
    duktape_uninit(ctx);
}

//time_base_object on the top
static duk_ret_t get_timeout_base_object(duk_context* ctx){
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_get_prop_string(ctx,-1,"timeout");//[hash_stash,timeout]
    if(!duk_is_object(ctx,-1)){
        duk_pop(ctx);//[hash_stash]
        duk_push_object(ctx);//[hash_stash,timeout]
        duk_dup(ctx,-1);//[hash_stash,timeout,timeout]
        duk_put_prop_string(ctx,-3,"timeout");//[hash_stash,timeout]
    }
    duk_remove(ctx,-2);//[timeout]
    return 1;
}
//param:[function]
static duk_ret_t save_timeout_callback(duk_context* ctx,void* timerid){
    duk_push_pointer(ctx,timerid);//[function,timerid]
    duk_swap_top(ctx,-2);//[timerid,function]
    get_timeout_base_object(ctx);//[timerid,function,timeout_base_object]
    duk_insert(ctx,-3);//[timeout_base_object,timerid,function]
    duk_put_prop(ctx,-3);//[timeout_base_object];
    duk_pop(ctx);//[]
    return 0;
}
static duk_ret_t get_timeout_callback(duk_context* ctx,void* timerid){
    get_timeout_base_object(ctx);//[timeout_base_object];
    duk_push_pointer(ctx,timerid);//[timeout_base_object,timerid]
    duk_get_prop(ctx,-2);//[timeout_base_object,callback]
    duk_remove(ctx,-2);//[callback]
    return 1;
}

static duk_ret_t del_timeout_callback(duk_context* ctx,void* timerid){
    get_timeout_base_object(ctx);//[timeout_base_object];
    duk_push_pointer(ctx,timerid);//[timeout_base_object,timerid]
    duk_del_prop(ctx,-2);//[timeout_base_object]
    duk_pop(ctx);//[]
    return 0;
}

typedef struct TimerCallbackParam_t{
    duk_context* ctx;
    void* to_task;
    void* timer;    
}TimerCallbackParam;
static void timer_callback(Msg* pMsg){
    TimerCallbackParam* param = (TimerCallbackParam*)pMsg->args;
    platform_stop_timer(param->timer);
    platform_del_timer(param->timer);
    if(platform_send_msg(param->to_task,pMsg)){
        debugf("timer_callback send msg failed.task:%p timer:%p",param->to_task,param->timer);
        del_timeout_callback(param->ctx,pMsg);
        free(param);
        free(pMsg);
    }    
}

int on_timeout_msg(duk_context* ctx,Msg* pMsg){
    if(!pMsg){
        debugf("timer_callback pMsg null");
        return;
    }
    TimerCallbackParam* param = (TimerCallbackParam*)pMsg->args;
    //call js timeout callback
    get_timeout_callback(ctx,pMsg);//[callback]
    duk_pcall(ctx,0);//[ret]
    duk_pop(ctx);//[]
    del_timeout_callback(ctx,pMsg);

    free(param);
    return 0;
}
//[callback,timeout ms]
duk_ret_t native_set_timeout(duk_context* ctx){
    TimerCallbackParam* param =(TimerCallbackParam*)malloc(sizeof(TimerCallbackParam));
    if(!param){
        duk_error(ctx,DUK_ERR_EVAL_ERROR,"malloc timer parameter failed");
    }
    Msg* pMsg = (Msg*)malloc(sizeof(Msg));
    if(!pMsg){
        free(pMsg);
        duk_error(ctx,DUK_ERR_EVAL_ERROR,"malloc timer msg failed");
    }
    pMsg->id = MSG_TIMEOUT;

    int ms = duk_get_int_default(ctx,1,0);
    void* timer = platform_create_timer(timer_callback,pMsg);
    param->timer = timer;
    param->ctx = ctx;
    param->to_task = platform_current_task();    
    pMsg->args = param; 
    //[callback,timeout ms]
    duk_dup(ctx,-2);//[callback,timeout ms,callback]
    save_timeout_callback(ctx,pMsg);//[callback,timeout ms]
    if(ms > 0){
        platform_start_timer(timer,ms);
    }else{
        timer_callback(pMsg);
    }
    duk_push_pointer(ctx,pMsg);//[callback,timeout ms,timerid]
    return 1;
}

//[timerid]
duk_ret_t native_clearTimeout(duk_context* ctx){
    Msg* pMsg = (Msg*)duk_get_pointer(ctx,0);
    if(pMsg){
        TimerCallbackParam* param = (TimerCallbackParam*)pMsg->args;
        if(param->timer){
            platform_stop_timer(param->timer);
            platform_del_timer(param->timer);
        }
        del_timeout_callback(ctx,pMsg);
        free(param);
        free(pMsg);
    }
    return 0;
}
int on_play_pcm_file_end_msg(duk_context* ctx,int result);

void duktape_task(){
    debugf("duktape_task ++");
    duk_context* ctx = app_init();
    if(!ctx){
        debugf("app init failed");
        return ;
    }
    task_proc_msg(ctx);
    debugf("duktape_task --");
    app_uninit(ctx);
}