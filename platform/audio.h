#ifndef __AUDIO_H__
#define __AUDIO_H__
typedef void (*REC_DATA_CB)(void* data,int size);
typedef void (*REC_END_CB)(int result);

typedef void (*PLAY_END_CB)(int result);

int record_pcm_iot(int ms,REC_DATA_CB datacb,REC_END_CB endcb);
int platform_play_pcm_file(const char* filename,PLAY_END_CB cb);
void platform_play_stop();
int platform_play_pcm_stream(void* buffer,int size,PLAY_END_CB cb);
int platform_record_pcm_file(const char* filename,int millseconds,REC_END_CB cb);
int platform_record_stop();

void platform_tone(int type,int duration);
int platform_audio_set_channel(int chan);
int platform_audio_set_speaker_vol(int vol);
void platform_audio_init();
#endif //__AUDIO_H__