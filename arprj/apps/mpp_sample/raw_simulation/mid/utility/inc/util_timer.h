#ifndef __MID_UTIL_TIMER_H__
#define __MID_UTIL_TIMER_H__

#include "util_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TIMEER_NUMBER  10

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array)   (sizeof(array) / sizeof(array[0]))
#endif

typedef enum
{
    TIMER_FALSE = 0,  /*!< Logical false. */
    TIMER_TRUE  = 1   /*!< Logical true. */
} TIMER_BOOL;

//timer
typedef void (*TIMER_HANDLER)(void);
typedef void (*TIMER_HANDLER2)(void *lPara, void *pPara);


typedef struct tagAppTimer
{
    unsigned int       stamp;              /* the start timestamp*/
    unsigned int       interval;           /* the interval of the timer */
    TIMER_BOOL      oneshot;            /* one shot flag */
    unsigned int       id;                 /* Timer id */
    TIMER_HANDLER      fpcallback;    /* Timer id */
    TIMER_HANDLER2     fpcallback2;  /* Timer id */
    unsigned char          argNum;
    void *         arg1;           /* argument */
    void *         arg2;
}TIMER_INFO_T;

int  IPC_MID_UTIL_TIMER_Init();
void IPC_MID_UTIL_TIMER_UnInit();

/*
interl: 间隔时间, interl*250ms
immediate: 立即执行
oneshot: 只执行一次
*/
int IPC_MID_UTIL_TIMER_AddTask(TIMER_HANDLER const task_handler, int interl, TIMER_BOOL immediate,TIMER_BOOL oneshot);
int IPC_MID_UTIL_TIMER_AddTask2(TIMER_HANDLER2 const task_handler, int interl,TIMER_BOOL immediate,TIMER_BOOL oneshot, void *lPara, void *pPara);
void IPC_MID_UTIL_TIMER_DelTask(TIMER_HANDLER const task_handler);
void IPC_MID_UTIL_TIMER_DelTask2(TIMER_HANDLER2 const task_handler);
unsigned int timer_get_ms();

#ifdef __cplusplus
}
#endif
#endif

