#include "pmd.h"
#include <string.h>
#include "iot_pmd.h"

int platform_pmd_poweron_system(int simStartUpMode,int nwStartupMode){
    // if(!iot_pmd_poweron_system(simStartUpMode,nwStartupMode)){
    //     return -1;
    // }
    return 0;
}

int platform_pmd_poweroff_system(){
    iot_pmd_poweroff_system();
    return 0;
}

int platform_pmd_enter_deepsleep(int enter){
    int ret = 0;
    if(enter){
        iot_pmd_enter_deepsleep();
    }else{
        iot_pmd_exit_deepsleep();
    }
    return ret;
}
int platform_get_battery_info(BatteryInfo* pBatteryInfo){
    if(!iot_pmd_get_chg_param(
        &pBatteryInfo->battStatus,
        &pBatteryInfo->battVolt,
        &pBatteryInfo->battLevel,
        &pBatteryInfo->chargerStatus,
        &pBatteryInfo->chargeState
    )){
        return 0;
    }
    return -1;
}