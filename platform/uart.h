#ifndef __PLATFORM_UART_H__
#define __PLATFORM_UART_H__

enum UART{
    UART1,
    UART2,
    UART3,
    USB_UART,
};
//return 0 ok,<0 error code
int platform_uart_open(int port,int bitrate);
int platform_uart_close(int port);
int platform_uart_read(int port,void* buf,int size);
int platform_uart_write(int port,void* data,int size);
#endif //__PLATFORM_UART_H__