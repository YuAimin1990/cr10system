#ifndef __ICCCORE_H__
#define __ICCCORE_H__


#include "SystemIO.h"
#include <stdio.h>
#include <string.h>
#if _FREERTOS_VERSION_ON_
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif
#include "IccApi.h"
#include "IccList.h"
#include "IccPeer.h"

#define FROM_ARM_INTERRUPT2_BASE (0x010C4008)

#define vClearFromArmInterrupt2(id) vIoWrite(FROM_ARM_INTERRUPT2_BASE, 0)

//define the priority of ICC recv task
#define	ICC_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#define ICC_TOKEN_STR_MAXLEN 32
#define ICC_CLIENT_MSG_Q_NUM 32
#define ICC_INVALI_MSG_ID (~0)

/* rx thread period 50s */
#define ICC_WAIT_PEROID 50000

/* receive timeout 50s */
#define ICC_TIMEOUT 50000

#define ICC_WAIT_FLAG 0x1234


#if _FREERTOS_VERSION_ON_
/*********************rtos define & global********************************/
typedef xSemaphoreHandle IccRecvSignal_t;
#define xIccRecvCreateSignal() xSemaphoreCreateBinary()
#define vIccRecvDeleteSignal(signal) vSemaphoreDelete((signal))
#define xIccRecvWait(signal, ticks) xSemaphoreTake((signal), ticks)
#define xIccRecvSignal(signal) xSemaphoreGive((signal))
#define xIccRecvSignalIrq(signal,woken) xSemaphoreGiveFromISR((signal),(woken))

struct icc_request
{
	osThreadId_t owner;
	int dst_core;
	int msg_id;
	unsigned char *buf;
	int size;
	int res;
	IccRecvSignal_t pIccRecvSignal;
};

#else

struct icc_request
{
	osThreadId_t owner;
	int dst_core;
	int msg_id;
	unsigned char *buf;
	int size;
	int res;
};

#endif
struct icc_msg
{
	struct list_head list;
	unsigned int core_id;
	unsigned char *buf;
	unsigned int size;
};

struct icc_msg_queue
{
	struct list_head msg_list;
	struct icc_request *cur_request;
	unsigned int msg_id;
};

struct icc_client
{
	struct list_head list;
	osThreadId_t thread_id;
	osMutexId_t mutex;
	struct icc_msg_queue msg_rx_queue[ICC_CLIENT_MSG_Q_NUM];
};

struct icc_core
{
	struct list_head client_list;
	struct icc_shm_mgmt *mgmt;
	osThreadId_t rx_thread;
	struct icc_peer peer[ICC_CORE_TOTAL];
	osMutexId_t mutex;
};

struct icc_token_id
{
	char token[ICC_TOKEN_STR_MAXLEN];
	unsigned short msgid;
};

int xIccGetCoreId();
int xIccClientsHandleMsg(unsigned char *buf, unsigned int size,
						   unsigned int msg_id, unsigned int core_id);
#endif
