#ifndef __LOG_H__
#define __LOG_H__
void log_init();
int debugf(const char *fmt, ...);
#ifdef TRACE
#undef TRACE
#endif
#ifdef ERROR
#undef ERROR
#endif

#define TRACE(fmt,...) debugf("T:%s:%d "fmt,__FILE__,__LINE__,##__VA_ARGS__)
#define ERROR(fmt,...) debugf("E:%s:%d "fmt,__FILE__,__LINE__,##__VA_ARGS__)
#endif //__LOG_H__