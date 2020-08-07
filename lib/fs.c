#include "duktape.h"
#include "utils.h"
#include "fs.h"
static duk_ret_t file_constructor(duk_context *ctx){
    char* filename = duk_get_string_default(ctx,0,"");
    duk_push_this(ctx);
    duk_dup(ctx,-2);
    //set filename property
    duk_put_prop_string(ctx,-2,"name");
    duk_push_pointer(ctx,0);
    duk_put_prop_string(ctx,-2,"fd");
    return 1;
}
static const char* get_filename(duk_context* ctx){
    duk_push_this(ctx);
    duk_get_prop_string(ctx,-1,"name");
    const char* name = duk_get_string_default(ctx,-1,"");
    duk_pop_2(ctx);
    return name;
}

static void* get_fd(duk_context* ctx){
    void* fd = 0;
    duk_push_this(ctx);
    duk_get_prop_string(ctx,-1,"fd");
    fd = duk_get_pointer(ctx,-1);
    duk_pop_2(ctx);
    return fd;
}
static duk_ret_t file_size(duk_context *ctx){
    const char* filename = get_filename(ctx);
    duk_uint_t size = platform_fs_size_by_name(filename);
    duk_push_uint(ctx,size);
    return 1;
}
static duk_ret_t file_open(duk_context *ctx){
    duk_bool_t read = duk_get_boolean(ctx,0);
    duk_bool_t write = duk_get_boolean(ctx,1);
    const char* filename = get_filename(ctx);
    void* fd = platform_fs_open(filename,read,write); 
    if(fd<=0){
        duk_push_false(ctx);
        return 1;
    }
    duk_push_this(ctx);
    duk_push_pointer(ctx,fd);
    duk_put_prop_string(ctx,-2,"fd");
    duk_pop(ctx);//pop this;
    duk_push_true(ctx);
    return 1;
}
static duk_ret_t file_close(duk_context *ctx){
    void* fd = get_fd(ctx);
    if(fd){
        platform_fs_close(fd);
        duk_push_this(ctx);
        duk_push_pointer(ctx,fd);
        duk_put_prop_string(ctx,-2,"fd");
        duk_pop(ctx);//pop this;
    }
    return 0;
}
static duk_ret_t file_read(duk_context *ctx){
    if(!duk_is_buffer(ctx,0)){
        duk_error(ctx,-1,"param 0 is not buffer");
    }
    duk_size_t size = 0;
    void* buf = duk_get_buffer_data(ctx,0,&size);
    if(size == 0){
        duk_error(ctx,-1,"buffer is 0");
    }
    void* fd = get_fd(ctx);
    int rsize = platform_fs_read(buf,size,fd);
    // debugf("fd:%d want size:%d readed size:%d buf:%p",fd,size,rsize,buf);
    duk_push_int(ctx,rsize);
    return 1;
}
static duk_ret_t file_write(duk_context *ctx){
    if(!duk_is_buffer(ctx,0)){
        duk_error(ctx,-1,"param 0 is not buffer");
    }
    duk_size_t size = 0;
    void* buf = duk_get_buffer_data(ctx,0,&size);
    if(size == 0){
        duk_error(ctx,-1,"buffer is 0");
    }
    void* fd = get_fd(ctx);
    int wsize = platform_fs_write(buf,size,fd);
    duk_push_int(ctx,wsize);
    return 1;
}
//移动文件指针
//param:
//p[0] is pos
//p[1] is from 0:begin 1:cur 2:end
//返回文件位置
static duk_ret_t file_seek(duk_context *ctx){
    int pos = duk_get_int_default(ctx,0,0);
    int from = duk_get_int_default(ctx,1,0);
    void* fd = get_fd(ctx);
    pos = platform_fs_seek(pos,from,fd);
    duk_push_int(ctx,pos);
    return 1;
}

static duk_ret_t file_pos(duk_context *ctx){
    void* fd = get_fd(ctx);
    int pos = platform_fs_seek(0,1,fd);
    duk_push_int(ctx,pos);
    return 1;
}
typedef struct ls_callback_param_t{
    duk_context* ctx;
    duk_idx_t arrayidx;
}ls_callback_param;


static ls_callback(const char* name,int isdir,unsigned int size,unsigned int ctime,unsigned int mtime,unsigned int atime,void* udata){
    ls_callback_param *p = (ls_callback_param*)udata;
    duk_context* ctx = p->ctx;
    duk_idx_t aidx = p->arrayidx;
    duk_idx_t oidx = duk_push_object(ctx);
    // debugf("name:%s isdir:%d ctime:%d mtime:%d atime:%d",name,isdir,ctime,mtime,atime);
    put_string_field(ctx,oidx,"name",name);
    put_number_field(ctx,oidx,"size",size);
    if(isdir){
        put_true_field(ctx,oidx,"isdir");
    }else{
        put_false_field(ctx,oidx,"isdir");
    }
    
    put_number_field(ctx,oidx,"createTime",ctime);
    put_number_field(ctx,oidx,"modifyTime",mtime);
    put_number_field(ctx,oidx,"accessTime",atime);    
    array_push(ctx,aidx);    
}
static duk_ret_t ls(duk_context *ctx){
    const char* dirname = duk_get_string_default(ctx,0,"");
    if(strlen(dirname) > 0 ){
        duk_idx_t aidx = duk_push_array(ctx);
        ls_callback_param p;
        p.ctx = ctx;
        p.arrayidx = aidx;

        platform_ls(dirname,ls_callback,&p);
        return 1;
    }    
    return 0;
}
static duk_ret_t delFile(duk_context *ctx){
    char* filename = duk_get_string(ctx,0);
    platform_fs_del_file(filename);
    return 0;
}
static duk_function_list_entry module_funcs[] = {
    {"ls",ls,1},
    {"delFile",delFile,1},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};

static duk_function_list_entry file_funcs[] = {
    {"iot.fs.File",file_constructor,1},
    {"size",file_size,0},
    {"open",file_open,2},
    {"close",file_close,0},
    {"read",file_read,1},
    {"write",file_write,1},
    {"seek",file_seek,1},
    {"pos",file_pos,0},
    {0/*name*/,0/*c function*/,0 /*nargs*/}
};
int fs_init(duk_context* ctx){
    register_module_funcs(ctx,"iot.fs",module_funcs);
    register_class(ctx,file_funcs);
    return 0;
}