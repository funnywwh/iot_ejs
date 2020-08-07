#ifndef __EVAL_FILE_H__
#define __EVAL_FILE_H__
#include "duktape.h"
int eval_file(duk_context *ctx,const char* filename);
#endif //__EVAL_FILE_H__