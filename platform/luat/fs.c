#include "iot_fs.h"
#include "fs.h"
void* platform_fs_read_open(const char* name){
    INT32 fd;
    fd = iot_fs_open_file(name, FS_O_RDONLY);
    return fd;
}
int platform_fs_size(void* f){
    INT32 fd = (INT32)f;
    int pos = iot_fs_seek_file(fd,0,FS_SEEK_CUR);
    int offset = iot_fs_seek_file(fd,0,FS_SEEK_END);
    iot_fs_seek_file(fd,pos,FS_SEEK_SET);
    return offset;
}
int platform_fs_read(void* buf,int size,void* f){
    INT32 fd = (INT32)f;
    int readed_size = iot_fs_read_file(fd, buf, size);
    return readed_size;
}
void platform_fs_close(void* f){
    INT32 fd = (INT32)f;
    iot_fs_close_file(fd);
}

int platform_fs_write(void* buf,int size,void*f){
    return iot_fs_write_file(f,buf,size);
}

int platform_fs_size_by_name(const char* name){
    return iot_fs_file_size(name);
}

int plaltform_fs_pos(void* f){
    return iot_fs_seek_file(f,0,1);
}

void* platform_fs_open(const char* name ,int read,int write){
    void* fd = 0;
    UINT32 flags = 0;
    if(read){
        flags |= FS_O_RDONLY;
    }
    if(write){
        flags |= FS_O_WRONLY|FS_O_CREAT;
    }
    fd = iot_fs_open_file(name, flags);
    return fd;
}

int platform_fs_seek(int pos,int from,void* f){
    return iot_fs_seek_file(f,pos,from);
}

void platform_ls(const char* dirname,LS_CALLBACK cb,void* udata){
    AMOPENAT_FS_FIND_DATA findResult;
    INT32 iFd = -1;
    iFd = iot_fs_find_first(dirname, &findResult);

    if(iFd>=0){
        cb(findResult.st_name,findResult.st_mode&E_FS_ATTR_ARCHIVE?0:1,findResult.st_size,findResult.ctime,findResult.atime,findResult.mtime,udata);
        while(iot_fs_find_next(iFd, &findResult) == 0)
        {
            cb(findResult.st_name,findResult.st_mode&E_FS_ATTR_ARCHIVE?0:1,findResult.st_size,findResult.ctime,findResult.atime,findResult.mtime,udata);
        }

        if(iFd >= 0)
        {
            iot_fs_find_close(iFd);
        }
    }
}

void platform_fs_del_file(const char* filename){
    iot_fs_delete_file(filename);
}