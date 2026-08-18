#ifndef __MID_UTIL_COMMON_H__
#define __MID_UTIL_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <libgen.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <ctype.h>

#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <sys/mount.h>

#include <linux/unistd.h>
#include <linux/if.h>
#include <linux/wireless.h>

//#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>

#endif

//提示信息可根据需要是否打印
#define IPC_SD_LOG_ENABLE 0

//int init_sd_log_fp();
//int sd_printf_info(const char *format, ...);
//int sd_printf_err(const char *format, ...);
int sd_printf(char *buf, int buf_size);


/* PRINT_INFO */
#if IPC_SD_LOG_ENABLE
#define PRINT_INFO(fmt, args...)  \
    do{\
        char buf[200] = {0};\
		snprintf(buf, sizeof(buf), "[APP INFO] [ %s, Line: %d ]  " fmt "\n", __FILE__, __LINE__, ##args);\
        sd_printf(buf, strlen(buf));\
    }while(0)

#else
#define PRINT_INFO(fmt, args...)  \
    do{\
        printf("[APP INFO] [ %s, Line: %d ]  " fmt "\n", __FILE__, __LINE__,  ##args); \
    }while(0)
#endif

/* PRINT_ERR */
#if IPC_SD_LOG_ENABLE
#define PRINT_ERR(fmt, args...)  \
    do{\
        char buf[200] = {0};\
		snprintf(buf, sizeof(buf), "[APP ERR] [ %s, Line: %d ]  " fmt "\n", __FILE__, __LINE__, ##args);\
        sd_printf(buf, strlen(buf));\
    }while(0)

#define PRINT_ERR_MSG(fmt, args...)  \
    do{\
        char buf[200] = {0};\
		snprintf(buf, sizeof(buf), "[APP ERR] [ %s, Line: %d ]  " fmt "\n", __FILE__, __LINE__, ##args);\
        sd_printf(buf, strlen(buf));\
    }while(0)

#else
#define PRINT_ERR(fmt, args...)  \
    do{\
        printf("\033[0;31m[APP ERR] [ %s, Line:%d ]  " fmt "\033[0;0m\n", __FILE__, __LINE__,  ##args); \
    }while(0)

#define PRINT_ERR_MSG(fmt, args...)  \
    do{\
        printf("\033[0;31m[APP ERR] [ %s, Line:%d ]  " fmt "\033[0;0m\n", __FILE__, __LINE__,  ##args); \
    }while(0)
#endif

#define IPC_SET_THREADNAME(name) \
{\
    prctl(PR_SET_NAME, (unsigned long)name, 0,0,0); \
    pid_t tid;\
    tid = syscall(SYS_gettid);\
    printf("set pthread name:%d, %s, %s pid:%d tid:%d\n", __LINE__, __func__, name, getpid(), tid);\
}


#ifdef __cplusplus
}
#endif
#endif
