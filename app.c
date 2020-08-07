#include "os.h"
#include "log.h"
int __errno = 0;
void _exit(int status){    
}

int _fini(){
    
}
extern void duktape_task(void*);

int appimg_enter(void *param)
{    
    log_init();
    debugf("appimg_enter++");
    platform_go(duktape_task,0,1024*8, 0);
    debugf("appimg_enter--");
    
    return 0;
}

void appimg_exit(void)
{
    
}

