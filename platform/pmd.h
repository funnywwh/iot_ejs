#ifndef __PWD_H__
#define __PWD_H__

int platform_pmd_poweron_system(int simStartUpMode,int nwStartupMode);
int platform_pmd_poweroff_system();
int platform_pmd_enter_deepsleep(int enter);

typedef struct BatteryInfo_t{
    int battStatus;
    int battVolt;
    int battLevel;
    int chargerStatus;
    int chargeState;
}BatteryInfo;

int platform_get_battery_info(BatteryInfo* pBatteryInfo);
#endif //__PWD_H__