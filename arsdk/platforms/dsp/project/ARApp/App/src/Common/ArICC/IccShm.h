#ifndef __ICC_SHM_H__
#define __ICC_SHM_H__

#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "IccCore.h"
#include "IccSpinlock.h"

#define ICC_SHM_NR_NODES 12
#define ICC_SHM_BUF_LEN  128
/* 4K aligned */
#define ICC_SHM_BUFF_DESC_Q_SIZE 16

#define ICC_SHM_INVALID_IDX (~0)

#define SHM_INIT_COMPLETE_KEY 0xEF56A55A

/*********************rtos define & global********************************/
typedef xSemaphoreHandle IccSignal_t;
#define xIccCreateSignal() xSemaphoreCreateBinary()
#define vIccDeleteSignal(signal) vSemaphoreDelete((signal))
#define xIccwait(signal, ticks) xSemaphoreTake((signal), ticks)
#define xIccSignal(signal) xSemaphoreGive((signal))
#define xIccSignalIrq(signal,woken) xSemaphoreGiveFromISR((signal),(woken))

/*********************rtos define & global********************************/
typedef xSemaphoreHandle IccRecvSignal_t;
#define xIccRecvCreateSignal() xSemaphoreCreateBinary()
#define vIccRecvDeleteSignal(signal) vSemaphoreDelete((signal))
#define xIccRecvWait(signal, ticks) xSemaphoreTake((signal), ticks)
#define xIccRecvSignal(signal) xSemaphoreGive((signal))
#define xIccRecvSignalIrq(signal,woken) xSemaphoreGiveFromISR((signal),(woken))

/* buffer descriptor */
struct icc_shm_buff_desc
{
	uint32_t buf_idx;  //buf_idx: buffer idx
	uint32_t buf_nr;   //buf_nr:  buffer number
	uint32_t payload;  //payload: data size
	uint32_t msg_id;   //msg_id:  message id of this buffer
	uint32_t src_id;   //src_id:  src core id
	uint32_t priority; //
};

/* SM buffer descriptor queue for each core*/
struct icc_shm_desc_core
{
	shm_lock lock;
	/* buffers used by this core */
	struct icc_shm_buff_desc pkt_desc_q[ICC_SHM_BUFF_DESC_Q_SIZE];
	/* buffer idx available of this core, Lock the global inter-core Lock */
	uint32_t tx_idx;
	uint32_t count;
	/* buffer idx this core is receiving, Lock the global inter-core Lock */
	uint32_t rx_idx;
};

/* ICC driver block descriptor */
struct icc_shm_desc_blks
{
	shm_lock lock;
	/* buffer block size */
	uint32_t blk_sz;
	/* total buffer count */
	uint32_t count;
	/* used buffer count */
	uint32_t used;
	/* buff data offset in shm */
	uint32_t offset;
	/* buffer usage bitmap, when get available, need Lock inter-core lock */
	uint32_t bit_mask[];
};

/* ICC driver management block */
struct icc_shm_mgmt
{
	shm_lock init_lock; /*lock shm_init_lock when init the SHM_MGMT_BLOCK, inter-core Lock*/
	uint32_t init_field;
	struct icc_shm_desc_core core_desc[ICC_SHM_NR_NODES];
	struct icc_shm_desc_blks blk_desc;
};

int xIccShmInit(struct icc_core *icc, void *buff, unsigned int buff_len);
unsigned char *pxIccShmGetBuffer(struct icc_shm_mgmt *mgmt, uint32_t *idx, uint32_t nr);
int xIccShmTx(struct icc_shm_mgmt *mgmt, int target_coreid, int buf_idx, int buf_nr,
			   size_t buffer_size, unsigned int msgid,
			   unsigned int coreid, unsigned int priority);
void vIccShmPollRxThread(void *arg);
#endif
