#include "iot_debug.h"
#include "iot_uart.h"
#include "iot_debug.h"
#include "iot_os.h"
#include "iot_tts.h"

#ifdef LOG_TO_UART1
#define UART_PORT OPENAT_UART_1
#elif defined(LOG_TO_UART2)
#define UART_PORT OPENAT_UART_2
#endif 

VOID log_init(VOID)
{
#if defined(UART_PORT)
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
    err = iot_uart_open(UART_PORT, &uartCfg);
#endif    
}

int debugf(const char *fmt, ...)
{
    const int size = 2048;
	char* buff = malloc(size);
	va_list args;
	va_start(args, fmt);
	int l = vsnprintf(buff, size, fmt, args);
    if(l > 0 ){
#if defined(UART_PORT)        
        iot_uart_write(UART_PORT, buff, l);
        if(buff[l-1] != '\n'){
            iot_uart_write(UART_PORT, "\n", 1);
        }
#else        
        iot_debug_print("%s",buff);
#endif        
    }
    free(buff);
	va_end (args);
    return 0;
}