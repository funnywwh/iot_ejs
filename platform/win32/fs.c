#include <windows.h>
#include "fs.h"
#include <stdio.h>
#include "log.h"

static char newname[1024] = {0};
static const char *check_fs_name(const char *name)
{
    if (name[0] == '/')
    {
        snprintf(newname, sizeof(newname), ".%s", name);
    }
    else
    {
        return name;
    }
    return newname;
}
void *platform_fs_read_open(const char *name)
{
    FILE *f = fopen(check_fs_name(name), "rb");
    return f;
}
int platform_fs_size(void *f)
{
    int pos = ftell(f);
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, pos, SEEK_SET);
    return len;
}
int platform_fs_read(void *buf, int size, void *f)
{
    return fread(buf, 1, size, f);
}
void platform_fs_close(void *f)
{
    fclose(f);
}

int platform_fs_write(void *buf, int size, void *f)
{
    return fwrite(buf, 1, size, f);
}

int platform_fs_size_by_name(const char *name)
{
    const char *n = check_fs_name(name);
    FILE *f = fopen(n, "rb");
    if (!f)
    {
        debugf("fopen failed");
        return 0;
    }
    int len = platform_fs_size(f);
    fclose(f);
    return len;
}

int plaltform_fs_pos(void *f)
{
    return ftell(f);
}

void *platform_fs_open(const char *name, int read, int write)
{
    const char *n = check_fs_name(name);
    if (read && !write)
    {
        return fopen(n, "rb");
    }
    else if (!read && write)
    {
        return fopen(n, "wb+");
    }
    else if (read && write)
    {
        return fopen(n, "rw+");
    }
    return 0;
}

int platform_fs_seek(int pos, int from, void *f)
{
    fseek(f, pos, from);
    return ftell(f);
}

void platform_ls(const char *dirname, LS_CALLBACK cb, void *udata)
{
    char* ndirname = check_fs_name(dirname);
    char _dirname[256] = {0};
    snprintf(_dirname,sizeof(_dirname),"%s/*",ndirname);
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    hFind = FindFirstFileA(_dirname, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
    {
        TRACE("dirname:%s not found", _dirname);
        return;
    }
    do
    {
        char *name = 0;
        int isdir = 0;
        int size = 0;
        int ctime = 0;
        int mtime = 0;
        int atime = 0;
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            isdir = 1;
        }
        size = ffd.nFileSizeLow;
        name = ffd.cFileName;
        ctime = ffd.ftCreationTime.dwLowDateTime;
        mtime = ffd.ftLastWriteTime.dwLowDateTime;
        atime = ffd.ftLastAccessTime.dwLowDateTime;
        TRACE("%s",name);
        cb(name, isdir, size, ctime, mtime, atime, udata);
    } while (FindNextFileA(hFind, &ffd) != 0);
    FindClose(hFind);
}

void port_os__del_file(const char *);
void platform_fs_del_file(const char *filename)
{
    port_os__del_file(check_fs_name(filename));
}