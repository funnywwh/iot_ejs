#include <windows.h>
#include "log.h"
#include "os.h"
void platform_sleep(int ms){
}
void* platform_current_task(){
    return GetCurrentThreadId();
}

#define WM_JS_MSG (WM_USER+100)


int platform_send_msg(void* task,Msg* pMsg){
    DWORD dwThreadId = (DWORD)task;
    if(!PostThreadMessage(dwThreadId,WM_JS_MSG,pMsg,0)){
        return -1;
    }
    return 0;
}
int platform_wait_msg(void* task,Msg** ppMsg){
    MSG msg;
    while(1){
        if(!GetMessage(&msg,0,0,0)){
            return -1;
        }
        if(msg.message == WM_JS_MSG){
            break;
        }
        TRACE("got system msg");
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    }
    *ppMsg = 0;
    *ppMsg = (Msg*)msg.wParam;
    return 0;
}
typedef struct GoParam_t{
    GO_CALLBACK cb;
    void* p;
}GoParam;

static go_callback(GoParam* p){
    p->cb(p->p);
    free(p);
}
void* platform_go(GO_CALLBACK cb,void* p,int stacksize,int pri){
    DWORD dwThreadId = 0;
    GoParam* gop = (GoParam*)malloc(sizeof(GoParam));
    if(!gop){
        return 0;
    }
    gop->cb = cb;
    gop->p = p;
    CreateThread(0,stacksize,go_callback,gop, 0,&dwThreadId);
    return dwThreadId;
}

typedef struct TimerParam_t{
    TIMEOUT_CALLBACK cb;
    void* p;
    void* timer;
    void* timerQueue;
}TimerParam;
static void timer_callback(PVOID lpParam, BOOLEAN TimerOrWaitFired){
    TimerParam* ptimer = (TimerParam*)lpParam;
    if(!ptimer){
        return ;
    }
    ptimer->cb(ptimer->p);
}

void* platform_create_timer(TIMEOUT_CALLBACK cb,void* p){
    TimerParam* param = (TimerParam*)malloc(sizeof(TimerParam));
    if(!param){
        ERROR("platform_create_timer malloc timer failed");
        return -1;
    }
    param->cb = cb;
    param->p = p;
    param->timer = 0;
    param->timerQueue = CreateTimerQueue();
    if(!param->timerQueue){
        ERROR("CreateTimerQueue failed");
        free(param);
        return -2;
    }
    return param;
}

#define MillS_SECOND 10000
void platform_start_timer(void* timer,int ms){
    TimerParam* ptimer = (TimerParam*)timer;
    if(!ptimer){
        ERROR("platform_start_timer");
        return ;
    }
    if(!CreateTimerQueueTimer( &ptimer->timer, ptimer->timerQueue, 
            (WAITORTIMERCALLBACK)timer_callback, ptimer ,ms, 0, 0)){
                ERROR("CreateTimerQueueTimer failed");
                return ;
            }
}

void platform_stop_timer(void* timer){
    TimerParam* ptimer = (TimerParam*)timer;
    if(!ptimer){
        return ;
    }
    if(ptimer->timer){
        DeleteTimerQueueTimer(ptimer->timerQueue,ptimer->timer,0);
    }
    ptimer->timer = 0;
}
void platform_del_timer(void* timer){
    TimerParam* ptimer = (TimerParam*)timer;
    if(!ptimer){
        return ;
    }
    if(ptimer->timer){
        DeleteTimerQueueTimer(ptimer->timerQueue,ptimer->timer,0);
    }
    if(ptimer->timerQueue){
        DeleteTimerQueue(ptimer->timerQueue);
    }
    free(ptimer);
}

double platform_get_now(){
    return 0;
}

void* platform_create_semaphore(int count){
    return CreateSemaphore(0,0,count,0);
}
int platform_delete_semaphore(void* h){
    if(!h){
        return ;
    }
    CloseHandle(h);
    return 0;
}
int platform_wait_semaphore(void* h ,int ms){
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(h,ms);
    if(WAIT_OBJECT_0 == dwWaitResult){
        return 0;
    }
    return -1;
}
int platform_release_semaphore(void* h){
    LONG prevCount = 0;
    if(!ReleaseSemaphore(h,1,&prevCount)){
        return -1;
    }
    return 0;
}