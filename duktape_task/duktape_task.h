#ifndef __PORT_H__
#define __PORT_H__
#include "duktape.h"
duk_context* app_init();
void app_uninit(duk_context * ctx);
#endif //__PORT_H__