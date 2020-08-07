#include "string.h"
#include "iot_debug.h"
#include "iot_uart.h"
#include "iot_os.h"

#define UART_PORT OPENAT_UART_1

//中断方式读串口1数据
//注: 中断中有复杂的逻辑,要发送消息到task中处理
void uart_recv_handle(T_AMOPENAT_UART_MESSAGE* evt)
{
	INT8 *recv_buff = NULL;
    int32 recv_len;
    int32 dataLen = evt->param.dataLen;
	if(dataLen)
	{
		recv_buff = iot_os_malloc(dataLen);
		if(recv_buff == NULL)
		{
			iot_debug_print("uart_recv_handle_0 recv_buff malloc fail %d", dataLen);
		}	
		switch(evt->evtId)
		{
		    case OPENAT_DRV_EVT_UART_RX_DATA_IND:

		        // recv_len = iot_uart_read(UART_PORT2, recv_buff, dataLen , UART_RECV_TIMEOUT);
		        // iot_debug_print("uart_recv_handle_1:recv_len %d", recv_len);
				// uart_msg_send(uart_task_handle, UART_RECV_MSG, recv_buff, recv_len);
		        break;

		    case OPENAT_DRV_EVT_UART_TX_DONE_IND:
		        iot_debug_print("uart_recv_handle_2 OPENAT_DRV_EVT_UART_TX_DONE_IND");
		        break;
		    default:
		        break;
		}
	}
}


BOOL uart_init(VOID)
{
    BOOL err;
    T_AMOPENAT_UART_PARAM uartCfg;
    
    memset(&uartCfg, 0, sizeof(T_AMOPENAT_UART_PARAM));
    uartCfg.baud = OPENAT_UART_BAUD_115200; //波特率
    uartCfg.dataBits = 8;   //数据位
    uartCfg.stopBits = 1; // 停止位
    uartCfg.parity = OPENAT_UART_NO_PARITY; // 无校验
    uartCfg.flowControl = OPENAT_UART_FLOWCONTROL_NONE; //无流控
    uartCfg.txDoneReport = TRUE; // 设置TURE可以在回调函数中收到OPENAT_DRV_EVT_UART_TX_DONE_IND
    uartCfg.uartMsgHande = 0 ;//uart_recv_handle; //回调函数

    // 配置uart1 使用中断方式读数据
    err = iot_uart_open(UART_PORT, &uartCfg);

    
    return err;
}

const char* hexchars = "01234567890abcdef";
int from_hex(char* buf,int size){
	unsigned char b = 0;
	int z = 0;
	for(int i = 0;i<size;i++){
		for(int j = 0;j<sizeof(hexchars);j++){
			if(buf[i] == hexchars[j]){
				if(z == 0){
					z = 1;
					b = j;
				}else{
					z = 0;
					b = (b << 4) | j;
					buf[i-1] = b;
				}
				break;
			}
		}
	}
	return size/2;
}

int to_hex(char* inbuf,int size,char* outbuf){
	for(int i = 0;i<size;i++){
		unsigned char b = inbuf[i];
		int j = i*2;
		outbuf[j] = hexchars[b&0xf];
		outbuf[j+1] = hexchars[(b>>4)&0xf];
	}
	return size*2;
}

int base64_encode(const char *indata, int inlen, char *outdata, int *outlen);
int base64_decode(const char *indata, int inlen, char *outdata, int *outlen);

size_t uart_read (void *udata, char *buffer, size_t length){
	size_t r = iot_uart_read(UART_PORT,buffer,length,0);
	debugf("uart recv data:%.*X",r,buffer);
	return r;
}
size_t uart_write(void *udata, const char *buffer, size_t length){
	length = iot_uart_write(UART_PORT,buffer,length);
	return length;
}