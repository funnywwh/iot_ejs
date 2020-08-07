#include "duktape.h"
#include "utils.h"
#include "fs.h"
#include "log.h"
int eval_file(duk_context *ctx,const char* filename){
    int len = platform_fs_size_by_name(filename);
    if(len == 0){
        debugf("%s is empty or not exsit.",filename);
        return -999;
    }
    void* fd = platform_fs_read_open(filename);
    if(fd <= 0){
        debugf("can't open %s",filename);
        return fd;
    }
    char* buf = 0;
    int ret = 0;
    while(1){
        buf = malloc(len);
        if(buf == 0){
            debugf("malloc 0");
            break;
        }
        len = platform_fs_read(buf,len,fd);
        if(len == 0){
            debugf("read js file failed");
            break;
        }
        duk_push_string(ctx,filename);
        ret = duk_pcompile_lstring_filename(ctx,0,buf,len);
        if(ret){
            free(buf);
            buf = 0;
            debugf("compile file error");
            break;
        }
		free(buf);
		buf = 0;
        duk_push_global_object(ctx);  /* 'this' binding */
        duk_pcall_method(ctx,0);
        break;
    }
    if(buf){
        free(buf);
    }
    platform_fs_close(fd);
}