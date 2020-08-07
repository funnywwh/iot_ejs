#include "iot_tts.h"
void platform_tts_iot_init(){
    iot_tts_init(0);
    iot_tts_set_param(OPENAT_TTS_PARAM_CODEPAGE,OPENAT_CODEPAGE_UTF8);
}

int platform_tts_say(const char* txt,int size){
    iot_tts_play((char*)txt,size);
    return 0;
}