#include "iot_audio.h"
#include "audio.h"
#include "log.h"

void platform_tone(int type,int duration){
    iot_audio_play_dtmf(type,duration,OPENAT_AUD_SPK_GAIN_18dB);
}


void platform_audio_init(){

}
int record_pcm_iot(int ms,REC_DATA_CB datacb,REC_END_CB endcb){
	E_AMOPENAT_RECORD_PARAM param;
	
	param.fileName = "/record.amr";
	param.record_mode = OPENAT_RECORD_FILE;
	param.quality = OPENAT_RECORD_QUALITY_MEDIUM;
	param.type = OPENAT_RECORD_TYPE_MIC;
	param.format = OPENAT_AUD_FORMAT_PCM;
	param.time_sec = 20;
    BOOL ret = iot_audio_rec_start(&param,endcb);
    debugf("rec ret:%d",ret);
    return 0;
}


int platform_play_pcm_file(const char* filename,PLAY_END_CB cb){
    T_AMOPENAT_PLAY_PARAM playParam;
    BOOL err;

    playParam.playBuffer = FALSE;
    playParam.playFileParam.callback = cb;
    playParam.playFileParam.fileFormat = OPENAT_AUD_FORMAT_PCM;
    playParam.playFileParam.fileName = filename;
    err = iot_audio_play_music(&playParam);
	if(err){
		return 0;
	}
	return -1;
}

void platform_play_stop(){
	iot_audio_stop_music();
}

int platform_play_pcm_stream(void* buffer,int size,PLAY_END_CB cb){
    T_AMOPENAT_PLAY_PARAM playParam;
    BOOL err;

    playParam.playBuffer = TRUE;
    playParam.playBufferParam.callback = cb;
    playParam.playBufferParam.format = OPENAT_AUD_FORMAT_PCM;
	playParam.playBufferParam.pBuffer = buffer;
	playParam.playBufferParam.loop = FALSE;
    playParam.playBufferParam.len = size;
    err = iot_audio_play_music(&playParam);
	if(err){
		return 0;
	}	
	return -1;
}

int platform_record_pcm_file(const char* filename,int millseconds,REC_END_CB cb){
    BOOL err = FALSE;
	E_AMOPENAT_RECORD_PARAM param;
	
	param.fileName = filename;
	param.record_mode = OPENAT_RECORD_FILE;
	param.quality = OPENAT_RECORD_QUALITY_BEST;
	param.type = OPENAT_RECORD_TYPE_MIC;
	param.format = OPENAT_AUD_FORMAT_PCM;
	param.time_sec = millseconds/1000;
    err = iot_audio_rec_start(&param, cb);
	if(err){		
		return -1;
	}
	return 0;
}

int platform_record_stop(){
	if(!iot_audio_rec_stop()){
		return -1;
	}
	return 0;
}

int platform_audio_set_channel(int chan){
	if(!iot_audio_set_channel(chan)){
		return -1;
	}
	return 0;
}

int platform_audio_set_speaker_vol(int vol){
	if(!iot_audio_set_speaker_vol(vol)){
		return -1;
	}
	return 0;
}