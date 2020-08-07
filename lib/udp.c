#include "duktape.h"
#include "log.h"
#include "udp.h"
#include "utils.h"
#include "os.h"
#include "on_msg.h"
typedef struct UdpData_t{
    int sock;
    int async_sock;
    void* async_task;
    void* async_sem;
    void* read_sem;
    void* udp_task;
}UdpData;
static int native_sync(UdpData* h,int isSync);
static UdpData* get_udpdata(duk_context *ctx){
    int sock = 0;
    duk_push_this(ctx);
    if(!duk_get_prop_string(ctx,-1,"sock")){
        return 0;
    }
    UdpData* h = duk_get_pointer(ctx,-1);    
    duk_pop_2(ctx);
    return h;
}

static int get_sock(duk_context *ctx){
    int sock = 0;
    duk_push_this(ctx);
    if(!duk_get_prop_string(ctx,-1,"sock")){
        return 0;
    }
    UdpData* h = duk_get_pointer(ctx,-1);    
    duk_pop_2(ctx);
    return h->sock;
}
static duk_ret_t udp_fin(duk_context *ctx){
    UdpData* h = get_udpdata(ctx);
    if(h){
        if(h->sock > 0 ){
            platform_udp_close(h->sock);
            free(h);
        }
    }
    return 0;
}

static duk_ret_t native_udp_constructor(duk_context *ctx) {
    if (!duk_is_constructor_call(ctx)) {
        return DUK_RET_TYPE_ERROR;
    } 
    duk_push_this(ctx);//[udp]
    int sock = platform_udp_open();
    
    UdpData* h = (UdpData*)malloc(sizeof(UdpData));
    if(!h){
        duk_error(ctx,DUK_ERR_REFERENCE_ERROR,"out of memory");
    }
    memset(h,0,sizeof(UdpData));

    h->async_sem = platform_create_semaphore(1);
    if(!h->async_sem){
        free(h);
        duk_error(ctx,DUK_ERR_ERROR,"platform_create_semaphore async_sem");
    }
    h->read_sem = platform_create_semaphore(1);
    if(!h->read_sem){
        free(h);
        duk_error(ctx,DUK_ERR_ERROR,"platform_create_semaphore read_sem");
    }
    h->udp_task = platform_current_task();
    h->sock = sock;
    
    duk_push_pointer(ctx,h);//[udp,h]
    duk_put_prop_string(ctx, -2, "sock");//[udp]
    duk_push_c_function(ctx,udp_fin,1);//[udp,fin]
    duk_set_finalizer(ctx,-2);//[udp]

    vsave_head_stash_object(ctx,0,"udp:%d",sock);

    native_sync(h,0);
    return 1;
}




static duk_ret_t native_wait_recv(duk_context *ctx) {
    duk_int_t timeout_ms = duk_get_int(ctx,0);
    int sock = get_sock(ctx);
    duk_bool_t ok = 1;
    // debugf("waiting sock:%d",sock);

    ok = platform_socket_wait_recv(sock,timeout_ms);
    if(ok){
        duk_push_true(ctx);
    }else{
        duk_push_false(ctx);
    }
    return 1;
}



static int on_udp_has_data(duk_context *ctx,Msg* pMsg){
    UdpData *h = (UdpData*)pMsg->args;
    int sock = h->sock;
    vget_head_stash_object(ctx,"udp:%d",sock);//[udp]
    duk_get_prop_string(ctx,-1,"onData");//[udp,onData]
    if(duk_is_function(ctx,-1)){
        duk_dup(ctx,-1);//[udp,onData,udp]
        duk_pcall_method(ctx,0);//[udp,result]        
    }    
    duk_pop_2(ctx);
    platform_release_semaphore(h->read_sem);
    return 0;
}
static void sync_task(UdpData* h){    
    TRACE("sync_task++");
    while(1){
        int res = platform_socket_wait_recv_for_async(h->sock,h->async_sock,3600*1000);
        if(res == 1){         
            //有数据接收
            Msg* pMsg = malloc(sizeof(Msg));
            if(pMsg){
                pMsg->id = MSG_UDP_HAS_DATA;
                pMsg->args = h;
                platform_send_msg(h->udp_task,pMsg);
                platform_wait_semaphore(h->read_sem,3000);
            }else{
                ERROR("malloc msg error");
            }
        }else if(res == 2){
            TRACE("async closed");
            break;
        }else {
            continue;
        }
    }
    TRACE("sync_task--");
    platform_release_semaphore(h->async_sem);
}
static int native_sync(UdpData* h,int isSync){
    if(!isSync){
        if(!h->async_task){            
            h->async_sock = platform_udp_open();
            if(h->async_sock <= 0){
                h->async_sock = 0;
                ERROR("create async sock failed");     
                return -1;
            }
            h->async_task = platform_go(sync_task,h,1024*2,0);
        }
    }else{
        if(h->async_task){
            platform_udp_close(h->async_sock);
            platform_wait_semaphore(h->async_sem,0);            
            h->async_task = 0;
        }
    }
    return 0;
}
//[isSync]
static duk_ret_t sync(duk_context *ctx){
    UdpData* h = get_udpdata(ctx);
    if(!h){
        return DUK_RET_REFERENCE_ERROR;
    }
    duk_bool_t isSync = duk_get_boolean_default(ctx,0,0);
    native_sync(h,isSync);
    return 0;
}
static duk_function_list_entry module_funcs[] = {
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};

//接收
//params:
//buf:UInt8Array
//address:string
//port:number
//return > 0 recved size,<= 0 error
static duk_ret_t native_recvfrom(duk_context *ctx){
    int sock = get_sock(ctx);
    if(sock <= 0){
        debugf("socket not open");
        return DUK_RET_ERROR;
    }
    if(!duk_is_buffer(ctx,0)){
        debugf("arg0 is not buffer");
        return DUK_RET_TYPE_ERROR;
    }
    if(!duk_is_string(ctx,1)){
        debugf("arg1 is no string");
        return DUK_RET_TYPE_ERROR;
    }
    if(!duk_is_number(ctx,2)){
        debugf("arg1 is no number");
        return DUK_RET_TYPE_ERROR;
    }
    duk_size_t bufsize = 0;
    void* buf = duk_get_buffer_data(ctx,0,&bufsize);
    int recvedSize = 0;
    if(bufsize > 0 ){
        duk_push_this(ctx);
        const char* address = duk_get_string_default(ctx,1,0);
        int port = duk_get_int_default(ctx,2,0);
        if(address && port){
            recvedSize = platform_udp_recvfrom(sock,buf,bufsize,address,port);
        }
    }
    duk_push_int(ctx,recvedSize);
    return 1;//
}
//发送
//params:
//data:UInt8Array
//address:string
//port:number
//return > 0 recved size,<= 0 error
static duk_ret_t native_sendto(duk_context *ctx){
    int sock = get_sock(ctx);
    if(sock <= 0){
        return DUK_RET_ERROR;
    }
    if(!duk_is_buffer(ctx,0)){
        debugf("arg0 is not buffer");
        return DUK_RET_TYPE_ERROR;
    }
    if(!duk_is_string(ctx,1)){
        debugf("arg1 is not string");
        return DUK_RET_TYPE_ERROR;
    }
    if(!duk_is_number(ctx,2)){
        debugf("arg2 is not number");
        return DUK_RET_TYPE_ERROR;
    }    
    duk_size_t bufsize = 0;
    void* data = duk_get_buffer_data(ctx,0,&bufsize);
    int sentSize = 0;
    if(bufsize > 0 ){
        duk_push_this(ctx);
        const char* address = duk_get_string_default(ctx,1,0);
        int port = duk_get_int_default(ctx,2,0);
        if(address && port){
            sentSize = platform_udp_sendto(sock,data,bufsize,address,port);
        }
    }
    duk_push_int(ctx,sentSize);    
    return 1;
}
static duk_ret_t native_close(duk_context *ctx){
    UdpData* h = get_udpdata(ctx);
    if(h <= 0){
        return DUK_RET_ERROR;
    }
    platform_udp_close(h->sock);

    native_sync(h,0);

    if(h->async_sem){
        platform_delete_semaphore(h->async_sem);
        h->async_sem = 0;
    }    
    if(h->read_sem){
        platform_delete_semaphore(h->read_sem);
        h->read_sem = 0;
    }
    vdel_head_stash_object(ctx,"udp:%d",h->sock);
    free(h);

    duk_push_this(ctx);
    duk_push_int(ctx,0);
    duk_put_prop_string(ctx,-2,"sock");
    duk_pop(ctx);//this
    return 0;
}

static duk_function_list_entry udp_funcs[] = {
    {"iot.socket.Udp",native_udp_constructor,0},
    {"wait_recv",native_wait_recv,1},
    {"sendto",native_sendto,3},
    {"recvfrom",native_recvfrom,3},
    {"close",native_close,0},
    {"sync",sync,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int udp_init(duk_context* ctx){
    if(register_module_funcs(ctx,"iot.socket",module_funcs)){
        return 0;
    }
    register_msg_handler(MSG_UDP_HAS_DATA,on_udp_has_data);
    return register_class(ctx,udp_funcs);
}