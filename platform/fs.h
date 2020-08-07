#ifndef __FS_H__
#define __FS_H__
void* platform_fs_read_open(const char* name);
int platform_fs_size(void* f);
int platform_fs_read(void* buf,int size,void* f);
int platform_fs_write(void* buf,int size,void*f);
void platform_fs_close(void* f);
int platform_fs_size_by_name(const char* name);
int plaltform_fs_pos(void* f);
void* platform_fs_open(const char* name ,int read,int write);
int platform_fs_seek(int pos,int from,void* f);

typedef void (*LS_CALLBACK)(const char* name,int isdir,unsigned int size,unsigned int ctime,unsigned int mtime,unsigned int atime,void* udata);
void platform_ls(const char* dirname,LS_CALLBACK cb,void* udata);
void platform_fs_del_file(const char* filename);
#endif //__FS_H__