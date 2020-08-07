#include "iot_vat.h"

static char g_imei[16] = "012345678912345";
static BOOL g_got_imei = FALSE;

static AtCmdRsp getimei_cb(u8* pRspStr)
{
	iot_debug_print("[ota]demo_ota_getimei");
    AtCmdRsp  rspValue = AT_RSP_WAIT;
    u8 *rspStrTable[ ] = {"+CME ERROR","+WIMEI: ", "OK"};
    s16  rspType = -1;
    u8 imei[16] = {0};
    u8  i = 0;
    u8  *p = pRspStr + 2;
    for (i = 0; i < sizeof(rspStrTable) / sizeof(rspStrTable[0]); i++)
    {
        if (!strncmp(rspStrTable[i], p, strlen(rspStrTable[i])))
        {
            rspType = i;
            if (rspType == 1){
				strncpy(imei,p+strlen(rspStrTable[i]),15);
				strncpy(g_imei, imei, 15);
                debugf("imei:%s",g_imei);
            }
            break;
        }
    }
    switch (rspType)
    {
        case 0:  /* ERROR */
        rspValue = AT_RSP_ERROR;
        break;

        case 1:  /* +wimei */
        rspValue  = AT_RSP_WAIT;
        break;

		case 2:  /* OK */
        rspValue  = AT_RSP_CONTINUE;
        break;

        default:
        break;
    }
    return rspValue;
}
VOID platform_misc_init(VOID)
{
    if(g_got_imei){
        return ;
    }
    g_got_imei = TRUE;
	BOOL result = FALSE;
	AtCmdEntity atCmdInit[]={
		{AT_CMD_DELAY"2000",10,NULL},
		{"AT+WIMEI?"AT_CMD_END,11,getimei_cb},
	};
	result = iot_vat_push_cmd(atCmdInit,sizeof(atCmdInit) / sizeof(atCmdInit[0]));
    return result;
}

const char* platform_get_imei(){
    return g_imei;
}