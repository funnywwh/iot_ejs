#include "uart.h"


//return 0 ok,<0 error code
int platform_uart_open(int port,int bitrate){
    return 0;
}
int platform_uart_close(int port){
    return 0;
}
int platform_uart_read(int port,void* buf,int size){
    return 0;
}
int platform_uart_write(int port,void* data,int size){
    return 0;
}