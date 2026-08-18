#ifndef __TASKS_SCHEDULE_H__
#define __TASKS_SCHEDULE_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ArDspPlatform.h"

//**********************************************************
//********************* Constants **************************
//**********************************************************
/* Priorities at which the tasks are created. */
#define	ARM_ALG_RECV_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define	ARM_ALG_PROC_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define NPU_ALG_PROC_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )


/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the portTICK_RATE_MS constant. */
#define mainQUEUE_TASK_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )



/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define ARM_TASK_QUEUE_LENGTH					( 4 )
#define NPU_TASK_QUEUE_LENGTH					( 4 )


enum{
       AR_DSP_TASK_ERR_JSON = -1,
       AR_DSP_TASK_ERR_NETID = -2,
       AR_DSP_TASK_ERR_QUEUE_FULL = -3,
       AR_DSP_TASK_ERR_HEAP =-4,
       AR_DSP_TASK_ERR_MAX
}AR_DSP_TASK_ERR_E;


enum{
       AR_DSP_TASK_CMD_REG_NPU_CB = 0,
       AR_DSP_TASK_CMD_REG_CV_ALG,
       AR_DSP_TASK_CMD_REG_MAX
}AR_DSP_TASK_CMD_E;

#define ICC_MSG_LEN_MAX 1024

#define CMD_NAME_LEN_MAX 32
#pragma pack(4)
typedef struct
{
	AR_CHAR 	u32CmdName[CMD_NAME_LEN_MAX];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U64 		u64MsgBodyAddr;
	AR_U32		u32MshLen;
}AR_DSP_TASK_IOCTL_S;

typedef struct
{
	AR_CHAR 	u32CmdName[CMD_NAME_LEN_MAX];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U64 		u64MsgBodyAddr;
	AR_U32		u32MshLen;
	AR_S32 		s32ErrCode;
	AR_U32		u32Time;
}AR_DSP_TASK_ACK_IOCTL_S;
#pragma pack()


//*********************rtos define & global********************************
typedef xSemaphoreHandle NpuSignal_t;
#define xNpuCreatSignal() xSemaphoreCreateBinary()
#define vNpuDeleteSignal(signal) vSemaphoreDelete((signal))
#define xNpuwait(signal, ticks) xSemaphoreTake((signal), ticks)
#define xNpuSignal(signal) xSemaphoreGive((signal))
#define xNpuSignalIrq(signal,woken) xSemaphoreGiveFromISR((signal),(woken))

extern int xArTasksSchedule();



#endif

