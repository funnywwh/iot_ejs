#include "iot_uart.h"
#include "uart.h"


//return 0 ok,<0 error code
int platform_uart_open(int port,int bitrate){
    BOOL err;
    T_AMOPENAT_UART_PARAM uartCfg;
    
    memset(&uartCfg, 0, sizeof(T_AMOPENAT_UART_PARAM));
    uartCfg.baud = OPENAT_UART_BAUD_115200; //波特率
    uartCfg.dataBits = 8;   //数据位
    uartCfg.stopBits = 1; // 停止位
    uartCfg.parity = OPENAT_UART_NO_PARITY; // 无校验
    uartCfg.flowControl = OPENAT_UART_FLOWCONTROL_NONE; //无流控
    uartCfg.txDoneReport = TRUE; // 设置TURE可以在回调函数中收到OPENAT_DRV_EVT_UART_TX_DONE_IND
    uartCfg.uartMsgHande = 0; //回调函数

    // 配置uart1 使用中断方式读数据
    err = iot_uart_open(port, &uartCfg);
    return 0;
}
int platform_uart_close(int port){
    iot_uart_close(port);
    return 0;
}
int platform_uart_read(int port,void* buf,int size){
    int rsize = 0;
    rsize = iot_uart_read(port,buf,size,0);
    return rsize;
}
int platform_uart_write(int port,void* data,int size){
    int wsize = 0;
    wsize = iot_uart_write(port,data,size);
    return wsize;
}