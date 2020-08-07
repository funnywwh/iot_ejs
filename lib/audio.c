#include "duktape.h"
#include "log.h"
#include "utils.h"
#include "os.h"
#include "audio.h"

static duk_ret_t tone(duk_context *ctx) {
    if(!duk_is_string(ctx,0)){
        return DUK_RET_TYPE_ERROR;
    }
    
    const char* tone_string = "01234567890abcd*#";
    const char* tone_txt = duk_to_string(ctx,0);
    for(int i = 0;i<16;i++){
        if(tone_string[i] == tone_txt[0]){
            platform_tone(i,1000);
            break;
        }
    }
    return 0;
}
//设置音频通道
static duk_ret_t setChannel(duk_context *ctx){
    int chan = duk_get_int(ctx,0);
    platform_audio_set_channel(chan);
    return 0;
}

static duk_ret_t setSpeakerVol(duk_context *ctx){
    int vol = duk_get_int(ctx,0);
    platform_audio_set_speaker_vol(vol);
    return 0;
}
static void rec_cb(void* data,int size){
    debugf("rec_cb(%p,%d)",data,size);
}
static void rec_result_cb(int result){
    debugf("rec_result_cb(%d)",result);
}
static duk_ret_t record_pcm(duk_context *ctx){
    int rec_time_ms = duk_get_int_default(ctx,0,1000);//default 1s
    debugf("record_pcm");
    if(record_pcm_iot(rec_time_ms,rec_cb,rec_result_cb)){
        debugf("record_pcm_iot false");
        duk_push_int(ctx,-1);
        return 1;
    }
    duk_push_int(ctx,0);
    return 1;
}
static duk_ret_t record_stop(duk_context *ctx){
    platform_record_stop();
    return 0;
}



static void* play_pcmfile_task = 0;
static play_pcmfile_cb(int result){
    // debugf("play_pcmfile_cb task:%p",play_pcmfile_task);
    platform_play_stop();
    if(play_pcmfile_task){
        Msg* pMsg = (Msg*)malloc(sizeof(Msg));
        if(pMsg){
            pMsg->id = MSG_PLAY_PCM_FILE_CB;
            pMsg->args = (void*)result;
            platform_send_msg(play_pcmfile_task,pMsg);
        }
    }
    play_pcmfile_task = 0;
}
//[filename,end_callback]
static duk_ret_t play_pcm_file(duk_context *ctx){
    if(!duk_is_string(ctx,0)){
        debugf("arg0 is not string");
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not string");
    }
    if(!duk_is_function(ctx,1)){
        debugf("arg2 is not function");
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not function");
    }    
    if(play_pcmfile_task){
        duk_error(ctx,DUK_ERR_ERROR,"playing");
    }
    char* filename = duk_get_string(ctx,0);
    play_pcmfile_task = platform_current_task();
    save_head_stash_object(ctx,"play_pcm_file_cb",1);    
    int ret = platform_play_pcm_file(filename,play_pcmfile_cb);
    duk_push_int(ctx,ret);
    return 1;
}

void* record_pcm_file_task = 0;
static void record_pcm_file_cb(int result){
    platform_record_stop();
    if(record_pcm_file_task){
        Msg* pMsg = (Msg*)malloc(sizeof(Msg));
        if(pMsg){
            pMsg->id = MSG_RECORD_PCM_FILE_CB;
            pMsg->args = (void*)result;
            platform_send_msg(record_pcm_file_task,pMsg);
        }
    }
    record_pcm_file_task = 0;
}
//[filename,millseconds,end_callback]
static duk_ret_t record_pcm_file(duk_context *ctx){
    if(!duk_is_string(ctx,0)){
        debugf("arg0 is not string");
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not string");
    }
    if(!duk_is_number(ctx,1)){
        debugf("arg0 is not int");
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not int");
    }
    if(!duk_is_function(ctx,2)){
        debugf("arg2 is not function");
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg2 is not function");
    }
    char* filename = duk_get_string(ctx,0);//default 1s
    record_pcm_file_task = platform_current_task();
    int ms = duk_get_int_default(ctx,1,0);
    save_head_stash_object(ctx,"record_pcm_file_cb",2);
    int ret = platform_record_pcm_file(filename,ms,record_pcm_file_cb);
    duk_push_int(ctx,ret);
    return 1;
}
static duk_ret_t play_stop(duk_context *ctx){
    platform_play_stop();
    return 0;
}
static play_pcm_stream_cb(int result){
    platform_play_stop();
    if(play_pcmfile_task){
        Msg* pMsg = (Msg*)malloc(sizeof(Msg));
        if(pMsg){
            pMsg->id = MSG_PLAY_PCM_STREAM_CB;
            pMsg->args = (void*)result;
            platform_send_msg(play_pcmfile_task,pMsg);
        }
    }
    play_pcmfile_task = 0;
}
//[buffer,callback]
static duk_ret_t play_pcm_strem(duk_context *ctx){
    if(play_pcmfile_task){
        duk_error(ctx,DUK_ERR_ERROR,"playing");
    }
    if(!duk_is_buffer_data(ctx,0)){
        duk_error(ctx,DUK_ERR_ERROR,"arg0 is not buffer");
    }
    if(!duk_is_function(ctx,1)){
        duk_error(ctx,DUK_ERR_SYNTAX_ERROR,"arg1 is not function");
    }
    duk_size_t pcmSize = 0;
    void* pcmBuffer = duk_get_buffer_data(ctx,0,&pcmSize);
    if(pcmSize <= 0){
        duk_error(ctx,DUK_RET_RANGE_ERROR,"arg0 buffer size <= 0");
    }
    play_pcmfile_task = platform_current_task();
    save_head_stash_object(ctx,"play_pcm_s_cb",1);
    int ret = platform_play_pcm_stream(pcmBuffer,pcmSize,play_pcm_stream_cb);
    duk_push_int(ctx,ret);
    return 0;
}
static duk_function_list_entry module_funcs[] = {
    {"tone",tone,1},
    {"setChannel",setChannel,1},
    {"setSpeakerVol",setSpeakerVol,1},
    {"record_pcm",record_pcm,2},
    {"record_stop",record_stop,0},
    {"play_pcm_file",play_pcm_file,2},
    {"play_stop",play_stop,0},
    {"play_pcm_stream",play_pcm_strem,2},
    {"record_pcm_file",record_pcm_file,3},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};

static duk_number_list_entry module_numbers[] = {
    {"OPENAT_AUDIOHAL_ITF_RECEIVER",0},
    {"OPENAT_AUDIOHAL_ITF_EARPIECE",1},
    {"OPENAT_AUDIOHAL_ITF_LOUDSPEAKER",2},
    {"OPENAT_AUD_CHANNEL_DUMMY_AUX_HANDSET",3},
    {0,0},
};

int audio_init(duk_context* ctx){
    platform_audio_init();

    register_module_funcs(ctx,"iot.audio",module_funcs);
    register_module_numbers(ctx,"iot.audio",module_numbers);
    return 0;
}