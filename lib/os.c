#include "duktape.h"
#include "os.h"
#include "utils.h"
#include "log.h"
#include "eval_file.h"
#include "on_msg.h"
static duk_ret_t sleep(duk_context *ctx) {
    platform_sleep(duk_get_int_default(ctx,0,0));
    return 0;
}
//p[0] task
//p[1] msg id
//p[2] msg body string
//return true ok,false failed
static duk_ret_t send_msg(duk_context* ctx){
    void* task = duk_get_pointer_default(ctx,0,0);
    if(task == 0){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"task == 0");
    }
    int msgid = duk_get_int_default(ctx,1,0);
    char* msg = duk_get_string_default(ctx,2,"");
    Msg* pMsg = (Msg*)malloc(sizeof(Msg));
    if(!pMsg){
        duk_error(ctx,DUK_ERR_REFERENCE_ERROR,"out of memory for Msg");
    }
    GoMsg* pGoMsg = (GoMsg*)malloc(sizeof(GoMsg));
    if(!pGoMsg){
        free(pMsg);
        duk_error(ctx,DUK_ERR_REFERENCE_ERROR,"out of memory for Msg");
    }
    pGoMsg->from_task = platform_current_task();
    pGoMsg->body = strdup(msg);    
    pMsg->args = pGoMsg;
    pMsg->id = msgid;
    if(platform_send_msg(task,pMsg)){
        free(pMsg->args);
        free(pMsg);
        debugf("platform_send_msg failed");
        // duk_error(ctx,DUK_ERR_EVAL_ERROR,"platform_send_msg failed");
        duk_push_false(ctx);
    }else{
        duk_push_true(ctx);
    }
    return 1;
}
//return object
// {
//     id:0,
//     msg:"",
// }
static duk_ret_t wait_msg(duk_context* ctx){
    void* task = duk_get_pointer_default(ctx,0,0);
    Msg* pMsg = 0;
    if(platform_wait_msg(task,pMsg) != 0){
        duk_error(ctx,DUK_ERR_EVAL_ERROR,"wait msg failed");
    }
    
    if(pMsg){
        duk_idx_t oidx = duk_push_object(ctx);//for return
        duk_push_int(ctx,pMsg->id);
        duk_put_prop_string(ctx,oidx,"id");
        if(pMsg->args){
            duk_push_string(ctx,(char*)pMsg->args);
            free(pMsg->args);
        }else{
            duk_push_string(ctx,"");
        }
        duk_put_prop_string(ctx,oidx,"msg");
        free(pMsg);
    }
    return 1;
}
static duk_ret_t current_task(duk_context* ctx){
    void* task = platform_current_task();
    duk_push_pointer(ctx,task);
    return 1;
}


typedef struct go_cb_param_t{
    duk_context* ctx;
    void* parent_task;
    char* js_path;//should free
}go_cb_param;

static void go_task(void* p){
    go_cb_param* param = (go_cb_param*)p;
    duk_context* ctx = duk_create_heap_default();
    while(ctx){
        if(duktape_init(ctx) != 0){
            debugf("go duktape init faield");
            break;
        }
        duk_eval_string(ctx,"iot.os");//[iot.os]
        create_task_object(ctx,param->parent_task,MSG_TO_PARENT_MSG);//[iot.os,parent task_object]
        duk_put_prop_string(ctx,-2,"parent");        
        duk_pop(ctx);

        eval_file(ctx,param->js_path);
        task_proc_msg(ctx);
        break;
    };
    if(ctx){
        duktape_uninit(ctx);
    }else{
        debugf("create duktape failed");
    }
    free(param->js_path);
    free(param);
    debugf("go_task--");
}



//返回线程对象
//p[0] js file path
//return thread object
// {
//     task:task
// }
//
static duk_ret_t go(duk_context* ctx){
    // debugf("go++");
    const char* jspath = duk_get_string_default(ctx,0,0);
    if(jspath == 0){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"p[0] is not js path");
    }
    int stacksize = duk_get_int_default(ctx,1,1024*8);
    int pri = duk_get_uint_default(ctx,2,0);
    go_cb_param* p = (go_cb_param*)malloc(sizeof(go_cb_param));
    p->ctx = ctx;
    p->parent_task = platform_current_task();
    p->js_path = strdup(jspath);
    debugf("go js:%s",p->js_path);
    void* sub_task = platform_go(go_task,p,stacksize,pri);
    if(sub_task == 0){
        free(p->js_path);
        free(p);
        duk_error(ctx,DUK_ERR_EVAL_ERROR,"create thread failed");
    }
    //save heap_stash[sub_task] = go object;
    create_task_object(ctx,sub_task,MSG_TO_CHILD_MSG);//[taskobject]
    save_head_stash_task_object(ctx,sub_task);
    // debugf("go--");
    return 1;
}
static duk_function_list_entry module_funcs[] = {
    {"sleep",sleep,1},
    {"current_task",current_task,0},
    {"send_msg",send_msg,3},
    {"wait_msg",wait_msg,0},
    {"go",go,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};

int os_init(duk_context* ctx){
    debugf("os_init");
    return register_module_funcs(ctx,"iot.os",module_funcs);
}


duk_double_t get_now(duk_context* ctx){
    return platform_get_now();
}

