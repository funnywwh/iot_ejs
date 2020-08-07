#include "pmd.h"
#include <string.h>
#include "log.h"
#include <stdlib.h>
int platform_pmd_poweron_system(int simStartUpMode,int nwStartupMode){
    return 0;
}

int platform_pmd_poweroff_system(){
    TRACE("platform_pmd_poweroff_system");
    exit(-1);
    return 0;
}

int platform_pmd_enter_deepsleep(int enter){
    return 0;
}
int platform_get_battery_info(BatteryInfo* pBatteryInfo){
    pBatteryInfo->battLevel = 100;
    pBatteryInfo->battStatus = 1;
    pBatteryInfo->battVolt = 3700;
    pBatteryInfo->chargerStatus = 1;
    pBatteryInfo->chargeState = 2;
    return 0;
}