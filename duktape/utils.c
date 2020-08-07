#include "utils.h"
#include "log.h"



duk_ret_t native_print(duk_context *ctx) {
    duk_push_string(ctx, " ");
	duk_insert(ctx, 0);
	duk_join(ctx, duk_get_top(ctx) - 1);
	debugf("%s", duk_safe_to_string(ctx, -1));

    return 0;
}
duk_idx_t register_module_single(duk_context* ctx,duk_idx_t obj_idx,const char* name){
    duk_bool_t chk = duk_get_prop_string(ctx,obj_idx,name);
    if(chk){
        return 0;
    }
    duk_pop(ctx);
    duk_push_object(ctx);
    duk_dup(ctx,-1);    
    duk_put_prop_string(ctx,obj_idx-2,name);
    return 1;
}

duk_idx_t register_module(duk_context* ctx,const char* name){
    duk_idx_t idx = 0;
    char* key = 0;
    char* dupname = strdup(name);
    char* pnext = dupname;
    char* prev = pnext;
    int popcount = 0;
    duk_push_global_object(ctx);
    while(1){
        
        //loop register module splited by '.'
        while((pnext = strstr(pnext,"."))){
            *pnext++ = 0;
            register_module_single(ctx,-1,prev);   
            popcount++;         
            prev = pnext;
        }
        break;
    }
    register_module_single(ctx,-1,prev);
    if(popcount > 0){
        duk_insert(ctx,-popcount);
        popcount --;
        duk_pop_n(ctx,popcount);
    }
    free(dupname);
    return 1;
}

//注册模块和其函数
//name 可以带点,e.g:a.b.c
//funcs 将注册到 c模块下面
duk_ret_t register_module_funcs(duk_context* ctx,const char* name,duk_function_list_entry* funcs){
    register_module(ctx,name);
    duk_put_function_list(ctx,-1,funcs);    
    duk_pop(ctx);
    return 0;
}
//注册模块和其函数
//name 可以带点,e.g:a.b.c
//numbers 变量列表,将注册到 c模块下面
duk_ret_t register_module_numbers(duk_context* ctx,const char* name,duk_number_list_entry* numbers){
    register_module(ctx,name);
    duk_put_number_list(ctx,-1,numbers);
    duk_pop(ctx);
    return 0;
}

//注册模块的单个函数
//name 可以带点,e.g:a.b.c
//f 将注册到 c模块下面
//nargs 函数参数
duk_ret_t register_module_func(duk_context* ctx,const char* name,duk_c_function f,int nargs){
       duk_idx_t idx = 0;
    char* key = 0;
    char* dupname = strdup(name);
    char* pnext = dupname;
    char* prev = pnext;
    int popcount = 0;
    duk_push_global_object(ctx);
    while(1){        
        //loop register module splited by '.'
        while((pnext = strstr(pnext,"."))){
            *pnext++ = 0;
            register_module_single(ctx,-1,prev);   
            popcount++;         
            prev = pnext;
        }
        break;
    }
    duk_push_c_function(ctx,f,nargs);
    duk_put_prop_string(ctx,-1,prev);
    
    if(popcount > 0){
        duk_pop_n(ctx,popcount);
    }
    duk_pop(ctx);
    free(dupname);
    return 0;
}

//注册模块下的类
//funcs的成员函数
//funcs[0] is contructor key is class name,value is contructor function
duk_ret_t register_class(duk_context* ctx,duk_function_list_entry* funcs){
    if(funcs && funcs[0].key){
        duk_function_list_entry contructor = funcs[0];
        char* dupname = strdup(contructor.key);
        char* pnext = dupname;
        char* prev = pnext;
        int popcount = 0;     
        duk_push_global_object(ctx);
        while((pnext = strstr(pnext,"."))){
            *pnext++ = 0;    
            register_module_single(ctx,-1,prev);   
            popcount++;
            prev = pnext;
        }
        if(popcount > 0){
            //left top as class module       
            duk_insert(ctx,-popcount);
            popcount --;
            duk_pop_n(ctx,popcount);
            duk_remove(ctx,-2);//remove global
        }
        //top is class module object stack [module object]
        
        duk_push_c_function(ctx, contructor.value, contructor.nargs /*nargs*/); //[module object,contructor]

        /* Push class object. */
        duk_push_object(ctx); //use for return stack [module object,contructor,class object]
        duk_dup(ctx,-1);//use for prototype will delete [module object,contructor,class object,class object]
        duk_insert(ctx,0);//[class object,module object,contructor,class object]
        funcs++;
        duk_put_function_list(ctx,-1,funcs);
        duk_put_prop_string(ctx,-2,"prototype");//[class object,module object,contructor]

        /* Finally, register class  object */
        duk_put_prop_string(ctx,-2,prev);//owner is module object[class object,module object]
        free(dupname);

        duk_pop(ctx);//remove class module object,only left class object. stack [module object]
    }
    
    return 1;
}

duk_ret_t put_string_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname,const char* value){
    duk_push_string(ctx,value);
    duk_put_prop_string(ctx,oidx,fieldname);
    return 0;
}

duk_ret_t put_number_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname,duk_double_t value){
    duk_push_number(ctx,value);
    duk_put_prop_string(ctx,oidx,fieldname);
    return 0;    
}
duk_ret_t get_array_length(duk_context* ctx,duk_idx_t aidx){
    duk_get_prop_string(ctx,aidx,"length");
    int length = duk_get_int(ctx,-1);
    duk_pop(ctx);
    return length;
}
//stack -1 is array element
duk_ret_t array_push(duk_context* ctx,duk_idx_t aidx){
    duk_get_prop_string(ctx,aidx,"length");
    int length = duk_get_int(ctx,-1);
    duk_pop(ctx);
    duk_put_prop_index(ctx,aidx,length);
    return 0;
}
duk_ret_t put_true_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname){
    duk_push_true(ctx);
    duk_put_prop_string(ctx,oidx,fieldname);
    return 0;    
}
duk_ret_t put_false_field(duk_context* ctx,duk_idx_t oidx,const char* fieldname){
    duk_push_false(ctx);
    duk_put_prop_string(ctx,oidx,fieldname);
    return 0;    
}

int get_int_field(duk_context* ctx,const char* fieldname){
    duk_push_this(ctx);
    int ret = duk_get_int_default(ctx,-1,0);
    duk_pop_2(ctx);
    return ret;
}

duk_ret_t create_task_object(duk_context* ctx,void* task,int msgId){
    duk_eval_string(ctx,"(function(task,msgId){return new iot.os.TaskObject(task,msgId);})");//[function]
    duk_push_pointer(ctx,task);//[function,task]
    duk_push_int(ctx,msgId);//[function,task,msgid]
    duk_pcall(ctx,2);//[taskobject]
    return 1;
}
//stack top is task_base_object
static duk_ret_t get_sub_task_base_object(duk_context* ctx){
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_get_prop_string(ctx,-1,"subtask");//[hash_stash,subtask]
    if(!duk_is_object(ctx,-1)){
        duk_pop(ctx);//[hash_stash]
        duk_push_object(ctx);//[hash_stash,subtask]
        duk_dup(ctx,-1);//[hash_stash,subtask,subtask]
        duk_put_prop_string(ctx,-3,"subtask");//[hash_stash,subtask]
    }
    duk_remove(ctx,-2);//[subtask]
    return 1;
}
//stack[-1] = taskobject
duk_ret_t save_head_stash_task_object(duk_context* ctx,void* task){
    get_sub_task_base_object(ctx);//[taskobject,task_base_object]
    duk_push_pointer(ctx,task);//[taskobject,task_base_object,task]
    duk_dup(ctx,-3);//[taskobject,task_base_object,task(key),taskobject(value)]
    duk_put_prop(ctx,-3);//[taskobject,task_base_object]
    duk_pop(ctx);//heap stash [taskobject]
    return 0;
}
//return [taskobject]
duk_ret_t get_head_stash_task_object(duk_context* ctx,void* task){
    get_sub_task_base_object(ctx);//[hash_stash]
    duk_push_pointer(ctx,task);//[hash_stash,task(key)]
    duk_get_prop(ctx,-2);//[hash_stash,go_object(value)]
    duk_remove(ctx,-2);
    return 1;
}


duk_ret_t save_head_stash_object(duk_context* ctx,const char* key,duk_idx_t objIdx){
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_dup(ctx,objIdx);//[hash_stash,object]
    duk_put_prop_string(ctx,-2,key);//[hash_stash]
    duk_pop(ctx);
    return 0;
}
//stack top is object
duk_ret_t get_head_stash_object(duk_context* ctx,const char* key){
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_get_prop_string(ctx,-1,key);//[hash_stash,object]
    duk_remove(ctx,-2);//[object]
    return 1;
}
duk_ret_t del_head_stash_object(duk_context* ctx,const char* key){
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_del_prop_string(ctx,-1,key);
    duk_pop(ctx);
    return 0;
}

duk_ret_t vsave_head_stash_object(duk_context* ctx,duk_idx_t objIdx,const char* keyFmt,...){
    char key[64] = {0};
    va_list args;
	va_start(args, keyFmt);
	int l = vsnprintf(key, sizeof(key), keyFmt, args);
    va_end (args);
    if(l <= 0 ){        
        return DUK_RET_REFERENCE_ERROR;
    }    
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_dup(ctx,objIdx);//[hash_stash,object]
    duk_put_prop_string(ctx,-2,key);//[hash_stash]
    duk_pop(ctx);
    return 0;
}
//stack top is object
duk_ret_t vget_head_stash_object(duk_context* ctx,const char* keyFmt,...){
    char key[64] = {0};
    va_list args;
	va_start(args, keyFmt);
	int l = vsnprintf(key, sizeof(key), keyFmt, args);
    va_end (args);
    if(l <= 0 ){        
        return DUK_RET_REFERENCE_ERROR;
    }
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_get_prop_string(ctx,-1,key);//[hash_stash,object]
    duk_remove(ctx,-2);//[object]
    return 1;
}
duk_ret_t vdel_head_stash_object(duk_context* ctx,const char* keyFmt,...){
    char key[64] = {0};
    va_list args;
	va_start(args, keyFmt);
	int l = vsnprintf(key, sizeof(key), keyFmt, args);
    va_end (args);
    if(l <= 0 ){        
        return DUK_RET_REFERENCE_ERROR;
    }    
    duk_push_heap_stash(ctx);//[hash_stash]
    duk_del_prop_string(ctx,-1,key);
    duk_pop(ctx);
    return 0;
}