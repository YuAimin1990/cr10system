#ifndef __UTILS_LOG_H__
#define __UTILS_LOG_H__

#include <sys/syscall.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define LOG_TAG "osal"

#define log_tag_err(fmt,...) do { \
    time_t t = time(NULL); \
    char date[64] = "\0"; \
    strftime(date, 63, "%Y-%m-%d:%H:%M:%S", localtime(&t)); \
    fprintf(stderr,"[%s][%u:%ld:%s:%s:%s(%d)] "fmt"\n", LOG_TAG, getpid(), \
                   syscall(SYS_gettid), date, "ERROR", basename(__FILE__), \
                   __LINE__, ##__VA_ARGS__); \
} while(0)

#define log_tag_info(fmt,...) do { \
    time_t t = time(NULL); \
    char date[64] = "\0"; \
    strftime(date, 63, "%Y-%m-%d:%H:%M:%S", localtime(&t)); \
    fprintf(stderr,"[%s][%u:%ld:%s:%s:%s(%d)] "fmt"\n", LOG_TAG, getpid(), \
                   syscall(SYS_gettid), date, "INFO", basename(__FILE__), \
                   __LINE__, ##__VA_ARGS__); \
} while(0)

#define log_tag_warning(fmt,...) do { \
    time_t t = time(NULL); \
    char date[64] = "\0"; \
    strftime(date, 63, "%Y-%m-%d:%H:%M:%S", localtime(&t)); \
    fprintf(stderr,"[%s][%u:%ld:%s:%s:%s(%d)] "fmt"\n", LOG_TAG, getpid(), \
                   syscall(SYS_gettid), date, "WARN", basename(__FILE__), \
                   __LINE__, ##__VA_ARGS__); \
} while(0)

#define log_tag_debug(fmt,...) do { \
    time_t t = time(NULL); \
    char date[64] = "\0"; \
    strftime(date, 63, "%Y-%m-%d:%H:%M:%S", localtime(&t)); \
    fprintf(stderr,"[%s][%u:%ld:%s:%s:%s(%d)] "fmt"\n", LOG_TAG, getpid(), \
                   syscall(SYS_gettid), date, "DEBUG", basename(__FILE__), \
                   __LINE__, ##__VA_ARGS__); \
} while(0)

#define log_tag_always(fmt,...) do { \
    time_t t = time(NULL); \
    char date[64] = "\0"; \
    strftime(date, 63, "%Y-%m-%d:%H:%M:%S", localtime(&t)); \
    fprintf(stderr,"[%s][%u:%ld:%s:%s:%s(%d)] "fmt"\n", LOG_TAG, getpid(), \
                   syscall(SYS_gettid), date, "ALWAYS", basename(__FILE__), \
                   __LINE__, ##__VA_ARGS__); \
} while(0)


#define log_func_enter() do { \
} while(0)

#define log_func_exit() do { \
} while(0)

#define log_conditon(fmt, ...) do { \
}while(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //__UTILS_LOG_H__
