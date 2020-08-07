#include "audio.h"
#include "log.h"

void platform_tone(int type,int duration){
}


void platform_audio_init(){

}
int record_pcm_iot(int ms,REC_DATA_CB datacb,REC_END_CB endcb){
    return 0;
}


int platform_play_pcm_file(const char* filename,PLAY_END_CB cb){
	cb(0);
	return 0;
}

void platform_play_stop(){
}

int platform_play_pcm_stream(void* buffer,int size,PLAY_END_CB cb){
	return -1;
}

int platform_record_pcm_file(const char* filename,int millseconds,REC_END_CB cb){
	return 0;
}

int platform_record_stop(){
	return 0;
}

int platform_audio_set_channel(int chan){
	return 0;
}

int platform_audio_set_speaker_vol(int vol){
	return 0;
}