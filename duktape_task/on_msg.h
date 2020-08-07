#ifndef __ON_MSG_H__
#define __ON_MSG_H__
#include "duktape.h"
#include "os.h"
void task_proc_msg(duk_context* ctx);
int on_timeout_msg(duk_context* ctx,Msg* pMsg);
int on_record_pcm_file_msg(duk_context* ctx,int result);
int on_play_pcm_file_end_msg(duk_context* ctx,int result);
int on_play_pcm_stream_end_msg(duk_context* ctx,int result);


typedef int (*MSG_HANDLER)(duk_context* ctx,Msg* msg);
int register_msg_handler(int msgId,MSG_HANDLER h);

#endif //__ON_MSG_H__