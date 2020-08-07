#include <stdio.h>
#include <string.h>
#include "duktape.h"
#include "duk_module_node.h"
#include "log.h"

duk_ret_t cb_resolve_module(duk_context *ctx) {
	const char *module_id;
	const char *parent_id;

	module_id = duk_require_string(ctx, 0);
	parent_id = duk_require_string(ctx, 1);

	duk_push_sprintf(ctx, "js/%s.js", module_id);
	// debugf("resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n",
	// 	module_id, parent_id, duk_get_string(ctx, -1));

	return 1;
}

extern void* platform_fs_read_open(const char* name);
extern int platform_fs_size(void* f);
extern int platform_fs_read(void* buf,int size,void* f);
extern void platform_fs_close(void* f);

duk_ret_t cb_load_module(duk_context *ctx) {
	const char *filename;
	const char *module_id;

	module_id = duk_require_string(ctx, 0);
	duk_get_prop_string(ctx, 2, "filename");
	filename = duk_require_string(ctx, -1);

	// debugf("load_cb: id:'%s', filename:'%s'\n", module_id, filename);

	void* f = platform_fs_read_open(filename);
	if(!f){
		debugf("can't open file:%s\n",filename);
		(void) duk_type_error(ctx, "cannot find module: %s", module_id);
		return -1;
	}
	int size = platform_fs_size(f);
	while(size > 0 ){
		char* src = (char*)malloc(size);
		while(src){			
			int readed_size = platform_fs_read(src,size,f);
			if(readed_size != size){
				debugf("read file size err.readed size:%d want:%d\n",readed_size,size);
				(void) duk_type_error(ctx, "read file size err.readed size:%d want:%d\n",readed_size,size);
				break;
			}
			// debugf("mod context:%s\n",src);
			duk_push_lstring(ctx,src,readed_size);
			break;		
		}
		if(src){
			free((void*)src);
		}
		break;
	}
	platform_fs_close(f);
	// debugf("load_cb end\n");
	return 1;
}

static duk_ret_t handle_print(duk_context *ctx) {
	debugf("%s\n", duk_safe_to_string(ctx, 0));
	return 0;
}

static duk_ret_t handle_assert(duk_context *ctx) {
	if (duk_to_boolean(ctx, 0)) {
		return 0;
	}
	(void) duk_generic_error(ctx, "assertion failed: %s", duk_safe_to_string(ctx, 1));
	return 0;
}

int load_init(duk_context* ctx) {
	int i;
	int exitcode = 0;

	duk_push_c_function(ctx, handle_assert, 2);
	duk_put_global_string(ctx, "assert");

	duk_push_object(ctx);
	duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "resolve");
	duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "load");
	duk_module_node_init(ctx);

	return exitcode;
}
