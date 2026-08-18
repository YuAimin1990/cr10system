/*
 * Artosyn icc driver, share memory operation code
 */
#include <stdio.h>
#include <string.h>
#include "IccApi.h"
#include "IccShm.h"
#include "IccList.h"
#include "IccPeer.h"
#include "IccPort.h"

static void vIccShmDumpMgmt(struct icc_shm_mgmt *mgmt)
{
	struct icc_shm_desc_core *desc_core;
	struct icc_shm_desc_blks *desc_blks = &mgmt->blk_desc;
	int i, j;
	xDbgPrintf(PRN_UART, "[RTOS ICC] init field=%x\n", mgmt->init_field);
	xDbgPrintf(PRN_UART, "[RTOS ICC] init lock =%x\n", mgmt->init_lock);

	for (i = 0; i < ICC_SHM_NR_NODES; i++)
	{
		desc_core = &mgmt->core_desc[i];
		xDbgPrintf(PRN_UART, "[RTOS ICC] core desc %d\n", i);
		xDbgPrintf(PRN_UART, "[RTOS ICC] buffer cnt=%d, tx=%d, rx=%d\n",
				   desc_core->count, desc_core->tx_idx, desc_core->rx_idx);
		for (j = 0; j < ICC_SHM_BUFF_DESC_Q_SIZE; j++)
		{
			xDbgPrintf(PRN_UART, "[RTOS ICC] ringq %02d, buff idx=%04d, msg id=%02d, payload=%08x, src core=%d\n", j,
					   desc_core->pkt_desc_q[j].buf_idx, desc_core->pkt_desc_q[j].msg_id,
					   desc_core->pkt_desc_q[j].payload, desc_core->pkt_desc_q[j].src_id);
		}
	}

	xDbgPrintf(PRN_UART, "[RTOS ICC] block size 0x%x\n", desc_blks->blk_sz);
	xDbgPrintf(PRN_UART, "[RTOS ICC] block count %d\n", desc_blks->count);
	xDbgPrintf(PRN_UART, "[RTOS ICC] block used %d\n", desc_blks->used);
}

static unsigned char *pxIccIdxToBuf(struct icc_shm_mgmt *mgmt, uint32_t idx)
{
	unsigned char *buf;

	if (idx > mgmt->blk_desc.count)
		return NULL;

	buf = (unsigned char *)mgmt +
		  (mgmt->blk_desc.offset + mgmt->blk_desc.blk_sz * idx);

	return buf;
}

static uint32_t pxIccBufToIdx(struct icc_shm_mgmt *mgmt, unsigned char *buf)
{
	uint32_t idx;

	if ((uint32_t)buf > mgmt->blk_desc.offset +
							mgmt->blk_desc.count * mgmt->blk_desc.blk_sz)
		return ICC_SHM_INVALID_IDX;

	idx = (uint32_t)(buf - mgmt->blk_desc.offset) / mgmt->blk_desc.blk_sz;

	return idx;
}

/* get first zero bit */
static uint32_t pxIccGetFirstZeroBit(uint32_t value, uint32_t start)
{
	uint32_t idx;
	uint32_t tmp32;

	value |= (1 << start) - 1;

	/* Invert value */
	value = ~value;

	/* (~value) & (2's complement of value) */
	value = (value & (-value)) - 1;

	/* log2(value) */

	tmp32 = value - ((value >> 1) & 033333333333) - ((value >> 2) & 011111111111);

	idx = ((tmp32 + (tmp32 >> 3)) & 030707070707) % 63;

	/* Obtain index (compiler optimized ) */
	//GET_IDX(idx,value);

	return idx;
}

/* get first bit '1' */
static uint32_t pxIccGetFirstBit(uint32_t value, uint32_t start)
{
	return pxIccGetFirstZeroBit(~value, start);
}

static void vIccSetBitmap(struct icc_shm_mgmt *mgmt, uint32_t idx, uint32_t nr)
{
	int i;
	uint32_t *word;
	uint32_t bit_msk_idx = idx / 32;
	uint8_t bit_idx = idx % 32;

	/* First unaligned word */
	word = &mgmt->blk_desc.bit_mask[bit_msk_idx];
	for (i = bit_idx; i < 32; i++)
	{
		*word |= (1 << i);
		if (--nr == 0)
			return;
	}
	word++;

	while (nr > 32)
	{
		*word++ = ~0;
		nr -= 32;
	}

	for (i = 0; i < nr; i++)
		*word |= (1 << i);
}

static void vIccClearBitmap(struct icc_shm_mgmt *mgmt, uint32_t idx, uint32_t nr)
{
	int i;
	uint32_t *word;
	uint32_t bit_msk_idx = idx / 32;
	uint8_t bit_idx = idx % 32;

	/* First unaligned word */
	word = &mgmt->blk_desc.bit_mask[bit_msk_idx];
	for (i = bit_idx; i < 32; i++)
	{
		*word ^= (1 << i);
		if (--nr == 0)
			break;
	}
	word++;

	while (nr > 32)
	{
		*word++ = 0;
		nr -= 32;
	}

	for (i = 0; i < nr; i++)
		*word ^= (1 << i);
}

static int xIccGetBitmap(struct icc_shm_mgmt *mgmt, uint32_t *p_idx, uint32_t nr)
{
	uint32_t i, start = 0;
	uint32_t bit_tmp = 0, bit_start = 0, bit_end = 0;
	int status = -EINVAL;

refind:
	for (i = start; i < mgmt->blk_desc.count / 32; i++)
	{
		/* Find first available buffer */
		bit_start = pxIccGetFirstZeroBit(mgmt->blk_desc.bit_mask[i], bit_start);
		if (bit_start < 32)
		{
			start = i;
			break;
		}
		bit_start = 0;
	}
	if (bit_start == 32)
	{
		return status;
	}

	bit_end = bit_start + 1;
	bit_start += start * 32;

	for (i = start; i < mgmt->blk_desc.count / 32; i++)
	{
		bit_end = pxIccGetFirstBit(mgmt->blk_desc.bit_mask[i], bit_end);

		if (bit_end < 32)
		{
			start = i;
			break;
		}
		if (bit_end == 32)
		{
			bit_end += i * 32;
			if (bit_end - bit_start >= nr)
				break;
		}
		bit_end = 0;
	}

	if (bit_end - bit_start < nr)
	{
		/* reseach from bit end */
		bit_start = bit_end - i * 32 + 1;
		goto refind;
	}
	else
	{
		*p_idx = bit_start;
		status = 0;
	}

	return status;
}

unsigned char *pxIccShmGetBuffer(struct icc_shm_mgmt *mgmt, uint32_t *idx, uint32_t nr)
{
	unsigned char *buff = NULL;
	int status = 0;

	/* Acquire lock of SM buffer management block */
	vIccShmAcquireLock(&mgmt->blk_desc.lock);

	/* Check if obtained buff index is less than the buff count */
	if (mgmt->blk_desc.used + nr <= mgmt->blk_desc.count)
	{
		/* Obtain the index of the first available SM buffer */
		status = xIccGetBitmap(mgmt, idx, nr);
		if (status == 0)
		{
			vIccSetBitmap(mgmt, *idx, nr);
			/* Obtain the address of the SM buffer for the index */
			buff = pxIccIdxToBuf(mgmt, *idx);
			if (!buff)
			{
				vIccClearBitmap(mgmt, *idx, nr);
				return NULL;
			}
			/* increment used buffer count */
			mgmt->blk_desc.used += nr;
		}
	}

	/* Release lock of SM buffer management block */
	vIccShmReleaseLock(&mgmt->blk_desc.lock);

	/* Return a MCAPI buffer to the caller */
	return buff;
}

void vIccShmFreeBuffer(struct icc_shm_mgmt *mgmt, uint32_t idx, uint32_t nr)
{
	vIccShmAcquireLock(&mgmt->blk_desc.lock);
	/* Mark the buffer available */
	vIccClearBitmap(mgmt, idx, nr);
	mgmt->blk_desc.used -= nr; /* Decrement used buffer count */
	vIccShmReleaseLock(&mgmt->blk_desc.lock);
}

static struct icc_shm_desc_core *pxIccGetSmRingQ(struct icc_shm_mgmt *mgmt,
												   uint32_t core_id)
{
	struct icc_shm_desc_core *p_sm_ring_queue = NULL;

	if (core_id < ICC_SHM_NR_NODES)
	{
		p_sm_ring_queue = &mgmt->core_desc[core_id];
	}

	/* Return pointer to SM ring queue for the unit ID identified */
	return p_sm_ring_queue;
}

static int xIccEnqueueSmRingQ(struct icc_shm_desc_core *shm_des_q,
								 uint32_t buf_idx, uint32_t buf_nr,
								 uint32_t msgid, uint32_t priority,
								 size_t buff_size)
{
	uint32_t tx_idx;
	int status = 0;
	struct icc_shm_buff_desc *shm_desc;

	/* Acquire lock of the SM packet descriptor queue */
	vIccShmAcquireLock(&shm_des_q->lock);

	/* Obtain put index into the queue */
	tx_idx = shm_des_q->tx_idx;

	if (shm_des_q->count == ICC_SHM_BUFF_DESC_Q_SIZE)
	{
		/* Queue is full fail denqueue operation, Try later */
		status = -EAGAIN;
	}
	else
	{
		/* Load packet descriptor */
		shm_desc = &shm_des_q->pkt_desc_q[tx_idx];
		shm_desc->priority = priority;
		shm_desc->buf_idx = buf_idx;
		shm_desc->buf_nr = buf_nr;
		shm_desc->src_id = xIccGetCoreId();
		shm_desc->payload = buff_size;
		shm_desc->msg_id = msgid;

		shm_des_q->tx_idx = (shm_des_q->tx_idx + 1) % ICC_SHM_BUFF_DESC_Q_SIZE;
		shm_des_q->count++;

		/* Enqueue operation successfully completed */
		status = 0;
	}

	/* Release lock of the SM packet descriptor queue */
	vIccShmReleaseLock(&shm_des_q->lock);
	return status;
}

int xIccShmTx(struct icc_shm_mgmt *mgmt, int target_coreid, int buf_idx, int buf_nr,
			   size_t buffer_size, unsigned int msgid,
			   unsigned int coreid, unsigned int priority)
{
	struct icc_shm_desc_core *shm_q;
	int status = 0;

	/* Obtain SM ring queue for the destination core ID */
	//debug_print("core_id:%d, buffer_size:%d\r\n", target_coreid, buffer_size);
	shm_q = pxIccGetSmRingQ(mgmt, target_coreid);
	if (shm_q)
	{
		/* Enqueue request to transmit data */
		//debug_print("buf_nr:%d,buffer_size:%d\r\n", buf_nr, buffer_size);
		status = xIccEnqueueSmRingQ(shm_q, buf_idx, buf_nr, msgid, priority, buffer_size);
	}
	else
	{
		/* TX request to unrecognized node ID */
		status = -EINVAL;
	}

	/* Send fail, free shm buffer */
	if (status)
		vIccShmFreeBuffer(mgmt, buf_idx, buf_nr);

	//	icc_shm_dump_mgmt(mgmt);

	return status;
}

/* Return the first pending descriptor, or NULL. */
static struct icc_shm_buff_desc *pxIccShmDescGetNext(struct icc_shm_desc_core *shm_des_q)
{
	//debug_print("count:%d,rx_idx:%d,count:%x\r\n", shm_des_q->count, shm_des_q->rx_idx, &shm_des_q->count);
	if (shm_des_q->count)
	{
	    //xDbgPrintf(PRN_UART, "shm_des_q->msgid %x \r\n",shm_des_q->pkt_desc_q[shm_des_q->rx_idx].msg_id);
		return &shm_des_q->pkt_desc_q[shm_des_q->rx_idx];
	}

	return NULL;
}

/* Make the first pending descriptor available to producers again. */
static void vIccShmDescConsume(struct icc_shm_desc_core *shm_des_q)
{
	vIccShmAcquireLock(&shm_des_q->lock);

	/* Update index and count */
	shm_des_q->rx_idx = (shm_des_q->rx_idx + 1) % ICC_SHM_BUFF_DESC_Q_SIZE;
	shm_des_q->count--;

	vIccShmReleaseLock(&shm_des_q->lock);
}

#if _FREERTOS_VERSION_ON_
//pIccSignal defined in iccCore.c
extern IccSignal_t pIccSignal;
void vIccShmPollRxThread(void *arg)
{
	struct icc_shm_mgmt *mgmt = arg;
	struct icc_shm_desc_core *shm_des_q = &mgmt->core_desc[xIccGetCoreId()];
	struct icc_shm_buff_desc *shm_des;
	unsigned char *buf;
	uint32_t timeout;
	int ret;

	timeout = ICC_WAIT_PEROID / portTICK_PERIOD_MS;
	xDebugPrint(" start vIccShmPollRxThread!shm.mgmt:0x%x\r\n",mgmt);

	for (;;)
	{
		vIccShmAcquireLock(&shm_des_q->lock);
		shm_des = pxIccShmDescGetNext(shm_des_q);
		vIccShmReleaseLock(&shm_des_q->lock);

		/* No pending msg, sleep */
		if(!shm_des) {
			xIccwait(pIccSignal,timeout);
			continue;
		}
		buf = pxIccIdxToBuf(mgmt, shm_des->buf_idx);
		AR_ASSERT(NULL != buf);
		ret = xIccClientsHandleMsg(buf, shm_des->payload,
									 shm_des->msg_id, shm_des->src_id);
		if (ret < 0)
		{
			xDbgPrintf(PRN_UART, "[RTOS ICC] %s %d, shm receive error, hang\n", __func__, __LINE__);
			while (1);
		}

		/* Consume shm queue and free shm buffer */
		vIccShmDescConsume(shm_des_q);
		vIccShmFreeBuffer(mgmt, shm_des->buf_idx, shm_des->buf_nr);
	}
}
#else
void vIccShmPollRxThread(void *arg)
{
	struct icc_shm_mgmt *mgmt = arg;
	struct icc_shm_desc_core *shm_des_q = &mgmt->core_desc[xIccGetCoreId()];
	struct icc_shm_buff_desc *shm_des;
	unsigned char *buf;
	uint32_t timeout;
	int ret;

	timeout = ICC_WAIT_PEROID / portTICK_PERIOD_MS;
	//xDebugPrint(" start vIccShmPollRxThread!shm.mgmt:0x%x\r\n",mgmt);

	for (;;)
	{
		vIccShmAcquireLock(&shm_des_q->lock);
		shm_des = pxIccShmDescGetNext(shm_des_q);
		vIccShmReleaseLock(&shm_des_q->lock);

		/* No pending msg, sleep */
		if(!shm_des) {
			//xIccwait(pIccSignal,timeout);
			//continue;
			return ;
		}
		buf = pxIccIdxToBuf(mgmt, shm_des->buf_idx);
		AR_ASSERT(NULL != buf);
		ret = xIccClientsHandleMsg(buf, shm_des->payload,
									 shm_des->msg_id, shm_des->src_id);
		if (ret < 0)
		{
			xDbgPrintf(PRN_UART, "[RTOS ICC] %s %d, shm receive error, hang\n", __func__, __LINE__);
			while (1);
		}

		/* Consume shm queue and free shm buffer */
		vIccShmDescConsume(shm_des_q);
		vIccShmFreeBuffer(mgmt, shm_des->buf_idx, shm_des->buf_nr);
	}
}


#endif
#if 0
int icc_shm_init(struct icc_core *icc, void *buff)
{
	struct icc_shm_mgmt *mgmt = (struct icc_shm_mgmt *)buff;

	if (!mgmt)
	{
		return -EINVAL;
	}

	/* Has another node completed SM driver initialization */
	if (mgmt->init_field != SHM_INIT_COMPLETE_KEY)
	{
		dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm not initialized\n", __func__, __LINE__);
		return -EINVAL;
	}

#if 0
	/* Create rx thread */
	icc->rx_thread = osThreadNew(icc_shm_poll_rx_thread, mgmt, NULL);
	if (!icc->rx_thread)
	{
		dbg_printf(PRN_UART, "[RTOS ICC] %s %d, rx thread create fail\n", __func__, __LINE__);
		return -EINVAL;
	}

	dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm init success\n", __func__, __LINE__);
	dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm block size 0x%x, data offset 0x%x\n",
			   __func__, __LINE__, mgmt->blk_desc.blk_sz, mgmt->blk_desc.offset);
#endif
	return 0;
}
#else
static int xIccShmSlaveNodeInit(struct icc_shm_mgmt *mgmt)
{
	return 0;
}

static int xIccShmMasterNodeInit(struct icc_shm_mgmt *mgmt)
{
	int i, buf_blk_count = mgmt->blk_desc.count;

	/*
	 * The current node is the first node executing in the system.
	 * Initialize SM driver as master node.
	 */

	vIccShmLockInit(&mgmt->blk_desc.lock);

	for (i = 0; i < ICC_CORE_TOTAL; i++)
	{
		vIccShmLockInit(&mgmt->core_desc[i].lock);
		//debug_print("lock[%d]:%d\r\n", i, mgmt->core_desc[i].lock);
	}

	/* Make all SM buffers available */
	for (i = 0; i < (buf_blk_count + 31) / 32; i++)
	{
		mgmt->blk_desc.bit_mask[i] = 0;
	}

	/* Initialize used buff count */
	mgmt->blk_desc.used = 0;
	/* Load shared memory initialization complete key */
	mgmt->init_field = SHM_INIT_COMPLETE_KEY;

	/* Return master node initialization status */
	return 0;
}

static size_t xIccCalcMgmtSize(size_t size)
{
	size_t mgmt_size = 0;

	/* Smallest mgmt size */
	mgmt_size = ALIGN(sizeof(struct icc_shm_mgmt), ICC_SHM_BUF_LEN - 1);
	if (mgmt_size >= size)
		return 0;

	while (mgmt_size < size)
	{
		int bit_cnt = (mgmt_size - sizeof(struct icc_shm_mgmt)) * 8;
		int buf_cnt = (size - mgmt_size) / ICC_SHM_BUF_LEN;

		if (buf_cnt == 0)
			return 0;

		if (bit_cnt >= buf_cnt)
			break;

		mgmt_size += ICC_SHM_BUF_LEN;
	}

	return mgmt_size;
}

#if 1
int xIccShmInit(struct icc_core *icc, void *buff, unsigned int buff_len)
{
	int ret = 0, shm_blks, buf_blks;
	//size_t mgmt_size, shm_size = icc->mem_size;
	size_t mgmt_size, shm_size = 0;
	/* Obtain Shared memory base address */
	//struct icc_shm_mgmt *mgmt = icc->virt_addr;
	struct icc_shm_mgmt *mgmt = (struct icc_shm_mgmt *)buff;

	if (mgmt->init_field == SHM_INIT_COMPLETE_KEY)
	{
		return 0;
	}

	shm_size = buff_len;

	memset(buff, 0, shm_size); //shm already clear by uboot.
	mgmt_size = xIccCalcMgmtSize(shm_size);
	if (!mgmt_size)
	{
		return -EINVAL;
	}

	buf_blks = (shm_size - mgmt_size) / ICC_SHM_BUF_LEN;

	mgmt->blk_desc.used = 0;
	mgmt->blk_desc.count = buf_blks;
	/* Obtain the offset of the SM buffer space */
	mgmt->blk_desc.offset = mgmt_size;
	mgmt->blk_desc.blk_sz = ICC_SHM_BUF_LEN;

	xDbgPrintf(PRN_UART,"shm size=%x, mgmt size=%x, shm buf size=%lx\n",
				shm_size, mgmt_size, ICC_SHM_BUF_LEN);
	xDbgPrintf(PRN_UART,"buffer block cnt=%d\n", buf_blks);

	if (mgmt != NULL)
	{
		/* Has another node completed SM driver initialization */
		if (mgmt->init_field != SHM_INIT_COMPLETE_KEY)
		{
			mgmt->init_field = SHM_INIT_COMPLETE_KEY;

			vIccShmLockInit(&mgmt->init_lock);

			/* Obtain SM driver initialization lock */
			vIccShmAcquireLock(&mgmt->init_lock);

			/* Initialize SM driver as the Master node */
			ret = xIccShmMasterNodeInit(mgmt);

			/* Release SM driver initialization lock */
			vIccShmReleaseLock(&mgmt->init_lock);
		}
		else
		{
			/* Obtain SM driver initialization lock */
			vIccShmAcquireLock(&mgmt->init_lock);

			/* Initialize SM driver as the Slave node */
			ret = xIccShmSlaveNodeInit(mgmt);

			/* Release SM driver initialization lock */
			vIccShmReleaseLock(&mgmt->init_lock);
		}
	}
	else
	{
		ret = -EINVAL;
	}

	return ret;
}
#else
int icc_shm_init(struct icc_core *icc, void *buff)
{
	struct icc_shm_mgmt *mgmt = (struct icc_shm_mgmt *)buff;

	if(!mgmt) {
		return -EINVAL;
	}

	/* Has another node completed SM driver initialization */
	debug_print("before shm init\r\n");
#if 0
	if (mgmt->init_field != SHM_INIT_COMPLETE_KEY) {
		dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm not initialized\n", __func__, __LINE__);
		return -EINVAL;
	}
#endif
	while(mgmt->init_field != SHM_INIT_COMPLETE_KEY);
	debug_print("after shm init\r\n");
#if 0
	/* Create rx thread */
	icc->rx_thread = osThreadNew(icc_shm_poll_rx_thread, mgmt, NULL);
	if(!icc->rx_thread) {
		dbg_printf(PRN_UART, "[RTOS ICC] %s %d, rx thread create fail\n", __func__, __LINE__);
		return -EINVAL;
	}

	dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm init success\n", __func__, __LINE__);
	dbg_printf(PRN_UART, "[RTOS ICC] %s %d, shm block size 0x%x, data offset 0x%x\n",
			__func__, __LINE__, mgmt->blk_desc.blk_sz, mgmt->blk_desc.offset);
#endif
	return 0;
}
#endif
#endif
