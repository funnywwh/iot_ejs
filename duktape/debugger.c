#include "duktape.h"

size_t uart_read (void *udata, char *buffer, size_t length);
size_t uart_write(void *udata, const char *buffer, size_t length);
void debugger_init(duk_context *ctx){
    uart_init();

    duk_debugger_attach(
        ctx,
        uart_read,//read_cb
        uart_write,//write_cb
        0,//peek_cb
        0,//read_flush_cb
        0,//write_flush_cb
        0,//request_cb
        0,//detached_cb
        0//userdata
        );
}