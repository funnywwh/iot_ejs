#include "duktape.h"
#include "os.h"
#include "utils.h"
#include "on_msg.h"
#include "log.h"
static MSG_HANDLER s_msg_handler[MSG_ID_COUNT] = {0};

static int on_go_msg(duk_context* ctx,GoMsg* msg){
    return 0;
}
static int on_to_parent_msg(duk_context* ctx,GoMsg* msg){   
    // debugf("on_go_msg++");
    // get go_object from head_stash[from_task] 
    get_head_stash_task_object(ctx,msg->from_task);//[go_object]
    // debugf("got go object");
    //get go_object.onMsg
    duk_get_prop_string(ctx,-1,"onMsg");//[go_object,onMsg]
    if(duk_is_function(ctx,-1)){
        // debugf("calling onMsg");
        duk_swap_top(ctx,-2);//[onMsg,go_object]
        //call go_object.onMsg(msg)
        duk_push_string(ctx,msg->body);//[onMsg,go_object(this),msg]
        duk_pcall_method(ctx,1);//[]
    }else{
        duk_pop_2(ctx);
    }
    free(msg->body);
    free(msg);
    // debugf("on_go_msg--");
    return 0;
}
static int on_to_child_msg(duk_context* ctx,GoMsg* pMsg){
    // debugf("go on_go_msg");
    //call iot.parent.onMsg function
    do{
        duk_eval_string(ctx,"iot.os.parent.onMsg");//[onMsg]
        if(!duk_is_function(ctx,-1)){
            duk_pop(ctx);
            break;
        }
        duk_eval_string(ctx,"iot.os.parent");//[onMsg,this]
        duk_push_string(ctx,pMsg->body);//[onMsg,this,msg]
        duk_pcall_method(ctx,1);
    }while(0);
    free(pMsg->body);
    free(pMsg);
    return 0;
}

duk_ret_t get_gpio_irq_callback(duk_context* ctx,int pin);
duk_ret_t del_gpio_irq_callback(duk_context* ctx,int pin);
static int on_gpio_msg(duk_context* ctx,GpioMsg* pGpioMsg){
    get_gpio_irq_callback(ctx,pGpioMsg->pin);//[callback]
    if(duk_is_function(ctx,-1)){
        duk_push_int(ctx,pGpioMsg->pin);//[callback,pin]
        duk_push_int(ctx,pGpioMsg->val);//[callback,pin,val]
        duk_pcall(ctx,2);//[ret]
    }else{
        debugf("callback is not function");
    }
    duk_pop(ctx);//[]
    free(pGpioMsg);  
    return 0;
}


void task_proc_msg(duk_context* ctx){
    Msg* pMsg = 0;
    void* curtask = platform_current_task();
    int quit = 0;
    while(!quit){        
        // debugf("go wait msg");
        int ret = platform_wait_msg(curtask,&pMsg);
        if(ret != 0 || pMsg == 0){
            debugf("wait msg err:%d",ret);
            break;
        }
        // debugf("go msg:%d",pMsg->id);
        
        switch (pMsg->id)
        {
        case MSG_QUIT/* constant-expression */:{
            debugf("task quit");
            quit = 1;
        }break;
        case MSG_GO_MSG:{
            ret = on_go_msg(ctx,(GoMsg*)pMsg->args);
        }break;
        case MSG_TO_CHILD_MSG:{
            ret = on_to_child_msg(ctx,(GoMsg*)pMsg->args);
        }break;
        case MSG_TO_PARENT_MSG:{
            ret = on_to_parent_msg(ctx,(GoMsg*)pMsg->args);
        }break;
        case MSG_TIMEOUT:{
            ret = on_timeout_msg(ctx,pMsg);
        }break;
        case MSG_PLAY_PCM_FILE_CB:{
            ret = on_play_pcm_file_end_msg(ctx,pMsg->args);
        }break;
        case MSG_PLAY_PCM_STREAM_CB:{
            ret = on_play_pcm_stream_end_msg(ctx,pMsg->args);
        }break;
        case MSG_RECORD_PCM_FILE_CB:{
            ret = on_record_pcm_file_msg(ctx,pMsg->args);
        }break;
        case MSG_GPIO_CB:{
            if(pMsg->args){
                ret = on_gpio_msg(ctx,pMsg->args);
            }
        }break;
        default:
            if(pMsg->id >= 0 && pMsg->id < MSG_ID_COUNT){
                if(s_msg_handler[pMsg->id]){
                    ret = s_msg_handler[pMsg->id](ctx,pMsg);
                }else{
                    debugf("msgid:%d not register handler.",pMsg->id);
                }
            }else{
                debugf("msgid:%d out of range",pMsg->id);
            }
            break;
        }
        free(pMsg);
        if(ret != 0){
            debugf("proc msg code:%d",ret);
            break;
        }
    }    
}




int on_play_pcm_file_end_msg(duk_context* ctx,int result){
    get_head_stash_object(ctx,"play_pcm_file_cb");//[play_pcm_file_cb]
    del_head_stash_object(ctx,"play_pcm_file_cb");//[play_pcm_file_cb]
    if(duk_is_function(ctx,-1)){
        duk_push_int(ctx,result);
        duk_pcall(ctx,1);//[ret]
        duk_pop(ctx);//[]
    }else{
        duk_pop(ctx);//[]
    }
    
    return 0;
}

int on_play_pcm_stream_end_msg(duk_context* ctx,int result){
    get_head_stash_object(ctx,"play_pcm_s_cb");//[play_pcm_s_cb]
    del_head_stash_object(ctx,"play_pcm_s_cb");//[play_pcm_s_cb]
    if(duk_is_function(ctx,-1)){
        duk_push_int(ctx,result);
        duk_pcall(ctx,1);//[ret]
        duk_pop(ctx);//[]
    }else{
        duk_pop(ctx);//[]
    }
        
    return 0;
}
int on_record_pcm_file_msg(duk_context* ctx,int result){
    get_head_stash_object(ctx,"record_pcm_file_cb");//[record_pcm_file_cb]
    del_head_stash_object(ctx,"record_pcm_file_cb");//[record_pcm_file_cb]
    if(duk_is_function(ctx,-1)){
        duk_push_int(ctx,result);
        duk_pcall(ctx,1);//[ret]
        duk_pop(ctx);//[]
    }else{
        duk_pop(ctx);//[]
    }
            
    return 0;
}

int register_msg_handler(int msgId,MSG_HANDLER h){
    if(msgId >= 0 && msgId < MSG_ID_COUNT){
        s_msg_handler[msgId] = h;
    }
}