#include "iot_os.h"
#include "iot_network.h"

static E_OPENAT_NETWORK_STATE s_network_state = 0;
static void networkIndCallBack(E_OPENAT_NETWORK_STATE state){
    debugf("networkIndCallBack:%d",state);
    s_network_state = state;
}

E_OPENAT_NETWORK_STATE get_network_state(){
    return s_network_state;
}

int platform_wait_network_ready(int timeout_sec){
    int ready = 0;
    while(timeout_sec < 0 || timeout_sec > 0){
        if(s_network_state == OPENAT_NETWORK_READY){
            debugf("network ready");
            ready = 1;
            break;
        }
        iot_os_sleep(1000);
        timeout_sec --;
    }
    return ready;
}

void platform_network_init(){
    iot_network_set_cb(networkIndCallBack);
}