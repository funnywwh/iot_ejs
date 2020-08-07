#include "duktape.h"
#ifndef __UTILS_H__
#define __UTILS_H__
duk_ret_t native_print(duk_context *ctx);
int duktape_init(duk_context* ctx);
void duktape_uninit(duk_context* ctx);
duk_idx_t register_module_single(duk_context* ctx,duk_idx_t obj_idx,const char* name);
duk_idx_t register_module(duk_context* ctx,const char* name);
duk_ret_t register_module_funcs(duk_context* ctx,const char* name,duk_function_list_entry* funcs);
duk_ret_t register_module_func(duk_context* ctx,const char* name,duk_c_function f,int nargs);
duk_ret_t register_class(duk_context* ctx,duk_function_list_entry* funcs);
duk_ret_t register_module_numbers(duk_context* ctx,const char* name,duk_number_list_entry* numbers);
duk_ret_t put_string_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname,const char* value);
duk_ret_t put_number_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname,duk_double_t value);
duk_ret_t put_true_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname);
duk_ret_t put_false_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname);
duk_ret_t get_array_length(duk_context* ctx,duk_idx_t aidx);
//stack -1 is array element
duk_ret_t array_push(duk_context* ctx,duk_idx_t aidx);

int get_int_field(duk_context* ctx,const char* fieldname);

//创建iot.os.TaskObject
duk_ret_t create_task_object(duk_context* ctx,void* task,int msgId);
//heap_stash[task] = task_object(stack[-1])
duk_ret_t save_head_stash_task_object(duk_context* ctx,void* task);
//get heap_stash[task]
duk_ret_t get_head_stash_task_object(duk_context* ctx,void* task);


duk_ret_t save_head_stash_object(duk_context* ctx,const char* key,duk_idx_t objIdx);
//stack top is object
duk_ret_t get_head_stash_object(duk_context* ctx,const char* key);
duk_ret_t del_head_stash_object(duk_context* ctx,const char* key);

//格式化key
duk_ret_t vsave_head_stash_object(duk_context* ctx,duk_idx_t objIdx,const char* keyFmt,...);
//stack top is object
duk_ret_t vget_head_stash_object(duk_context* ctx,const char* keyFmt,...);
duk_ret_t vdel_head_stash_object(duk_context* ctx,const char* keyFmt,...);
#endif //__UTILS_H__