#ifndef __OS_H__
#define __OS_H__
void platform_sleep(int ms);

void* platform_current_task();


enum MsgId{
    MSG_QUIT = 1,
    MSG_UART_ON_DATA = 2,
    MSG_GO_MSG = 3,//go 子线程给父线程发信息
    MSG_TIMEOUT = 4,//setTimeout 回调事件
    MSG_PLAY_PCM_FILE_CB = 5,//play pcm file结束的消息
    MSG_PLAY_PCM_STREAM_CB = 6,//play pcm stream结束的消息
    MSG_RECORD_PCM_FILE_CB = 7,//record_pcm_file callback 事件
    MSG_TO_CHILD_MSG = 8,//parent send msg to child 
    MSG_TO_PARENT_MSG = 9,//child task send msg to parent
    MSG_GPIO_CB = 10,//gpio irq callback
    MSG_KEYPAD_CB = 11,//keypad irq callback
    MSG_UDP_HAS_DATA = 12,
    MSG_ID_COUNT,
};

typedef struct QuitMsg_t{
    int code;
}QuitMsg;

typedef struct UartOnDataMsg_t{
    void* data;
    int size;
}UartOnDataMsg;

typedef struct GoMsg_t{
    void* from_task;
    char* body;
}GoMsg;

typedef struct GpioMsg_t{
    int pin;
    int val;
}GpioMsg;

typedef struct Msg_t{
    int id;
    void* args;//此参数是一块内存,接收者负责释放,内容根据id来判断
}Msg;
int platform_wait_msg(void* task,Msg** pMsg);
int platform_send_msg(void* task,Msg* pMsg);

typedef void (*GO_CALLBACK)(void* p);
//cb 执行函数
//p 参数
//stacksize 堆栈大小
//pri 优先级>=0,越大越高
//return thread id
void* platform_go(GO_CALLBACK cb,void* p,int stacksize,int pri);

typedef void (*TIMEOUT_CALLBACK)(void* p);
void* platform_create_timer(TIMEOUT_CALLBACK cb,void* p);
void platform_start_timer(void* timer,int ms);
void platform_stop_timer(void* timer);
void platform_del_timer(void* timer);

void* platform_create_semaphore(int count);
int platform_delete_semaphore(void* h);
int platform_wait_semaphore(void* h ,int ms);
int platform_release_semaphore(void* h);

double platform_get_now();

#endif //__OS_H__