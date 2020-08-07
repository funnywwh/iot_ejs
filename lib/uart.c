#include "duktape.h"
#include "utils.h"
#include "uart.h"
duk_ret_t uart_construtor(duk_context* ctx){
    int port = duk_get_int_default(ctx,0,UART1);
    int bitrate = duk_get_int_default(ctx,1,115200);
    duk_push_this(ctx);
    put_number_field(ctx,-1,"port",port);
    put_number_field(ctx,-1,"bitrate",bitrate);
    if(platform_uart_open(port,bitrate) != 0){
        //failed
        duk_error(ctx,DUK_ERR_ERROR,"open uart failed");
    }
    return 1;
}

duk_ret_t uart_close(duk_context* ctx){
    int port = get_int_field(ctx,"port");
    platform_uart_close(port);
    return 0;
}

duk_ret_t uart_write(duk_context* ctx){
    if(!duk_is_buffer(ctx,0)){
        duk_error(ctx,DUK_ERR_TYPE_ERROR,"p[0] should be buffer");
    }
    int bufsize = 0;
    const void* buf = duk_get_buffer_data(ctx,-1,&bufsize);
    if(bufsize == 0){
        duk_error(ctx,DUK_RET_RANGE_ERROR,"buf size <= 0");
    }
    int port = get_int_field(ctx,"port");
    int wsize = platform_uart_write(port,buf,bufsize); 
    duk_push_int(ctx,wsize);
    return 1;
}
duk_ret_t uart_read(duk_context* ctx){
    if(!duk_is_buffer(ctx,0)){
        duk_error(ctx,DUK_ERR_TYPE_ERROR,"p[0] should be buffer");
    }
    int bufsize = 0;
    const void* buf = duk_get_buffer_data(ctx,-1,&bufsize);
    if(bufsize == 0){
        duk_error(ctx,DUK_RET_RANGE_ERROR,"buf size <= 0");
    }
    int port = get_int_field(ctx,"port");
    int rsize = platform_uart_read(port,buf,bufsize); 
    duk_push_int(ctx,rsize);
    return 1;
}

duk_function_list_entry uart_funcs[] = {
    {"iot.uart.Uart",uart_construtor,2},
    {"close",uart_close,0},
    {"read",uart_read,1},
    {"write",uart_write,1},
    {0,0,0},
};
int uart_init(duk_context* ctx){
    register_class(ctx,uart_funcs);
    return 0;
}