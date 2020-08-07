#include "iot_os.h"
#include "os.h"
void platform_sleep(int ms){
    iot_os_sleep(ms);
}
void* platform_current_task(){
    return iot_os_current_task();
}
int platform_send_msg(void* task,Msg* pMsg){
    BOOL ok = iot_os_send_message(task,pMsg);
    if(ok){
        return 0;
    }
    return -1;
}
int platform_wait_msg(void* task,Msg** ppMsg){
    BOOL ok = iot_os_wait_message(task,ppMsg);
    if(ok){
        return 0;
    }
    return -1;
}

void* platform_go(GO_CALLBACK cb,void* p,int stacksize,int pri){
    void* task = iot_os_create_task(cb,p,stacksize,pri,OPENAT_OS_CREATE_DEFAULT,"jsgo");
    return task;
}


void* platform_create_timer(TIMEOUT_CALLBACK cb,void* p){
    void* timer = iot_os_create_timer(cb,p);
    return timer;
}
void platform_start_timer(void* timer,int ms){
    iot_os_start_timer(timer,ms);
}

void platform_stop_timer(void* timer){
    iot_os_stop_timer(timer);
}
void platform_del_timer(void* timer){
    iot_os_delete_timer(timer);
}

double platform_get_now(){
    return iot_os_get_system_tick();
}

void* platform_create_semaphore(int count){
    return iot_os_create_semaphore(count);
}
int platform_delete_semaphore(void* h){
    if(!iot_os_delete_semaphore(h)){
        return -1;
    }
    return 0;
}
int platform_wait_semaphore(void* h ,int ms){
    if(!iot_os_wait_semaphore(h,ms)){
        return -1;
    }
    return 0;
}
int platform_release_semaphore(void* h){
    if(!iot_os_release_semaphore(h)){
        return -1;
    }
    return 0;
}