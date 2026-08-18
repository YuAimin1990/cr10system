#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "IccShm.h"
#include "SystemIO.h"
#include "IccCore.h"
#include "IccPort.h"
#include "IccShm.h"

struct icc_core ar_icc_core;
static struct icc_token_id g_icc_token_id[ICC_CLIENT_MSG_Q_NUM] = {0};

#if _FREERTOS_VERSION_ON_
//stack information defined for ICC receive task
IccSignal_t pIccSignal = NULL;
//IccRecvSignal_t pIccRecvSignal = NULL;
#endif
#if 1
int xIccGetCoreId()
{
	int dsp_id = 0, core_id = 0;

	dsp_id = xGetSelfCoreId();
	core_id = dsp_id + ICC_CORE_CV_0;

	return core_id;
}
#else
int xIccGetCoreId()
{
	return ICC_CORE_M_0;
}
#endif


struct icc_core xIccCoreInfoGet(void)
{
	return ar_icc_core;
}

#if _FREERTOS_VERSION_ON_
/*Export api*/
volatile unsigned int icc_isr_cnt = 0;
void vIccIsr(void)
{
	long woken = 0;

	/* clear the icc receive irq : 0x010C4008 */
	vClearFromArmInterrupt2(0);
	/*xSemaphoreGiveFromISR */
	xIccSignalIrq(pIccSignal, &woken);
	icc_isr_cnt ++;
}
#else

/*Export api*/
volatile unsigned int icc_isr_cnt = 0;
void vIccIsr(void)
{
	long woken = 0;

	/* clear the icc receive irq : 0x010C4008 */
	vClearFromArmInterrupt2(0);
	/*xSemaphoreGiveFromISR */

	vIccShmPollRxThread((void*)ar_icc_core.mgmt);
		
	//xIccSignalIrq(pIccSignal, &woken);
	icc_isr_cnt ++;
}


#endif
unsigned int xArIccIsrCnt(void)
{
	return icc_isr_cnt;
}

static void __IccDumpClientsInfo(void)
{
	struct icc_client *client;
	struct icc_request *request;
	struct icc_msg_queue *msg_q;
	struct icc_msg *msg;
	int i;

	osMutexAcquire(ar_icc_core.mutex, osWaitForever);
	list_for_each_entry(client, &ar_icc_core.client_list, list)
	{
		xDbgPrintf(PRN_UART, "icc client %p\n", __func__, __LINE__, client);

		osMutexAcquire(client->mutex, osWaitForever);
		for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
		{
			msg_q = &client->msg_rx_queue[i];
			if (msg_q->msg_id == ICC_INVALI_MSG_ID)
				continue;

			xDbgPrintf(PRN_UART, "	 registered msgid %08x\n", msg_q->msg_id);

			request = msg_q->cur_request;

			if (request)
				xDbgPrintf(PRN_UART, "	    thread %p is pending for msgid\n", request->owner, request->msg_id);
			else
				xDbgPrintf(PRN_UART, "	    no pending request\n");

			if (!list_empty(&msg_q->msg_list))
			{
				list_for_each_entry(msg, &msg_q->msg_list, list)
				{
					xDbgPrintf(PRN_UART, "      pending msg: payload=0x%06x, from core %d\n",
							   msg->size, msg->core_id);
				}
			}
			else
			{
				xDbgPrintf(PRN_UART, "		no pending msg\n");
			}
		}
		xDbgPrintf(PRN_UART, "\n");
		osMutexRelease(client->mutex);
	}
	osMutexRelease(ar_icc_core.mutex);
}

static portBASE_TYPE xIccDumpClientsInfo(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	__IccDumpClientsInfo();
	return pdTRUE;
}

static const CLI_Command_Definition_t icc_dump_info =
	{
		.pcCommand = "icc_dump",
		.pcHelpString = "Dump infomations of all icc clients\n",
		.pxCommandInterpreter = xIccDumpClientsInfo,
		.cExpectedNumberOfParameters = 0,
};

void IccRegisterCliCommand(void)
{
}

static int xIccRegisterMsgid(struct icc_client *client, unsigned int msg_id)
{
	struct icc_msg_queue *msg_q;
	int i, pos = -1;

	/* Lock client */
	osMutexAcquire(client->mutex, osWaitForever);
	for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		msg_q = &client->msg_rx_queue[i];

		/* Msg id alread registered */
		if (msg_q->msg_id == msg_id)
		{
			osMutexRelease(client->mutex);
			return 0;
		}

		/* Find first unsed msg_q */
		if (msg_q->msg_id == ICC_INVALI_MSG_ID && pos == -1)
			pos = i;
	}
	osMutexRelease(client->mutex);

	if (pos == -1)
	{
		return -EINVAL;
	}

	/* Mark msg queue used */
	msg_q = &client->msg_rx_queue[pos];
	msg_q->msg_id = msg_id;
	msg_q->cur_request = NULL;
	INIT_LIST_HEAD(&msg_q->msg_list);
	return 0;
}

static int xIccUnregisterMsgid(struct icc_client *client, unsigned int msg_id)
{
	struct icc_msg_queue *msg_q;
	struct icc_msg *msg, *t_msg;
	int i;

	/* Clear msg list */
	osMutexAcquire(client->mutex, osWaitForever);
	for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		msg_q = &client->msg_rx_queue[i];

		if (msg_q->msg_id != msg_id)
		{
			continue;
		}

		if (!list_empty(&msg_q->msg_list))
		{
			list_for_each_entry_safe(msg, t_msg, &msg_q->msg_list, list)
			{
				list_del(&msg->list);
				free(msg->buf);
				free(msg);
			}
		}
		/* Set msg queue unused */
		msg_q->msg_id = ICC_INVALI_MSG_ID;
		msg_q->cur_request = NULL;
		break;
	}
	osMutexRelease(client->mutex);

	return 0;
}

static int xIccAddMsgNolock(struct icc_msg_queue *msg_q, unsigned char *buf,
							  unsigned int size, unsigned int core_id)
{
	struct icc_msg *msg;

	msg = malloc(sizeof(*msg));
	if (!msg)
		return -ENOMEM;

	msg->buf = malloc(size);
	if (!msg->buf)
		goto free_msg;

	memcpy(msg->buf, buf, size);
	msg->core_id = core_id;
	msg->size = size;
	list_add_tail(&msg->list, &msg_q->msg_list);

	return 0;
free_msg:
	free(msg);
	return -EINVAL;
}

int xIccClientsHandleMsg(unsigned char *buf, unsigned int size,
						   unsigned int msg_id, unsigned int core_id)
{
	struct icc_client *client;
	struct icc_request *request;
	struct icc_msg_queue *msg_q;
	int i, ret;

	osMutexAcquire(ar_icc_core.mutex, osWaitForever);
	list_for_each_entry(client, &ar_icc_core.client_list, list)
	{
		//debug_print("client:%x\r\n", client);
		osMutexAcquire(client->mutex, osWaitForever);
		for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
		{
			msg_q = &client->msg_rx_queue[i];

			if (msg_q->msg_id != msg_id)
				continue;

			/* Find registered msgid */
			request = msg_q->cur_request;

			/* Client have pending request	*/
			if (request)
			{
				/*
				 * If request buffer size is smaller than message size,
				 * add message to msg list and return error code to request thread
				 *
				 * Then the request thread can receive message with a bigger buffer next time
				 */
				if (request->size < size)
				{
					xDbgPrintf(PRN_UART, "[ICC] %s %d, buffer overflow, msg size=%x, buffer size=%x\n",
							   __func__, __LINE__, size, request->size);

					ret = xIccAddMsgNolock(msg_q, buf, size, core_id);
					if (ret < 0)
					{
						osMutexRelease(client->mutex);
						osMutexRelease(ar_icc_core.mutex);
						return -EINVAL;
					}

					request->size = 0;
					request->res = -EOVERFLOW;
				}
				else
				{
					request->buf = malloc(size);
					if (!request->buf)
					{
						request->size = 0;
						request->res = -ENOMEM;
						msg_q->cur_request = NULL;
						osMutexRelease(client->mutex);
						osMutexRelease(ar_icc_core.mutex);
						return 0;
					}
					else
					{
						memcpy(request->buf, buf, size);
						request->dst_core = core_id;
						request->size = size;
						request->res = 0;
					}
				}

				/* Current request id finished, remove it */
				msg_q->cur_request = NULL;
				/* Wake up thread */
				//osThreadFlagsSet(request->owner, ICC_WAIT_FLAG);
				#if _FREERTOS_VERSION_ON_
				xIccRecvSignal(request->pIccRecvSignal);
				#else
                osThreadFlagsSet(request->owner, ICC_WAIT_FLAG);
				#endif
			}
			else
			{
				ret = xIccAddMsgNolock(msg_q, buf, size, core_id);
				if (ret < 0)
				{
					osMutexRelease(client->mutex);
					osMutexRelease(ar_icc_core.mutex);
					return -EINVAL;
				}
			}
			break;
		}
		osMutexRelease(client->mutex);
	}
	osMutexRelease(ar_icc_core.mutex);
	return 0;
}

static void *pvIccClientCreate(void)
{
	struct icc_client *client;
	struct icc_msg_queue *msg_q;
	int i;

	client = malloc(sizeof(*client));
	if (!client)
	{
		return NULL;
	}
	memset(client, 0, sizeof(*client));

	client->thread_id = osThreadGetId();
	client->mutex = osMutexNew(NULL);
	for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		msg_q = &client->msg_rx_queue[i];
		msg_q->msg_id = ICC_INVALI_MSG_ID;
		msg_q->cur_request = NULL;
		INIT_LIST_HEAD(&msg_q->msg_list);
	}

	osMutexAcquire(ar_icc_core.mutex, osWaitForever);
	list_add(&client->list, &ar_icc_core.client_list);
	osMutexRelease(ar_icc_core.mutex);
	return client;
}

static void vIccClientDestroy(struct icc_client *client)
{
	struct icc_request *request;
	struct icc_msg_queue *msg_q;
	struct icc_msg *msg, *t_msg;
	int i, found = 0;

	/* Find client */
	osMutexAcquire(ar_icc_core.mutex, osWaitForever);
	list_del(&client->list);
	osMutexRelease(ar_icc_core.mutex);

	osMutexAcquire(client->mutex, osWaitForever);

	/* Cancle msg list */
	for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		msg_q = &client->msg_rx_queue[i];
		if (msg_q->msg_id == ICC_INVALI_MSG_ID)
			continue;

		/* Cancle current request */
		request = msg_q->cur_request;
		if (request)
		{
			request->buf = NULL;
			request->size = 0;
			request->res = -EINVAL;
			/* Wake up thread */
			osThreadFlagsSet(client->thread_id, ICC_WAIT_FLAG);
		}

		list_for_each_entry_safe(msg, t_msg, &msg_q->msg_list, list)
		{
			list_del(&msg->list);
			free(msg->buf);
			free(msg);
		}
		msg_q->msg_id = ICC_INVALI_MSG_ID;
	}

	osMutexRelease(client->mutex);

	free(client);
}

static int xIccSend(struct icc_client *client, struct icc_request *request)
{
	unsigned char *icc_buf;
	int ret;
	uint32_t idx, nr, blksz = ar_icc_core.mgmt->blk_desc.blk_sz;
	struct icc_peer *peer = &ar_icc_core.peer[request->dst_core];


	if (request->msg_id == ICC_INVALI_MSG_ID)
		return -EINVAL;

	if (!request || !request->buf)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, no buffer\n", __func__, __LINE__, request->msg_id);
		return -EINVAL;
	}

	if (!peer->notify)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, invalid target core\n", __func__, __LINE__, request->msg_id);
		return -EINVAL;
	}

	nr = (request->size + blksz - 1) / blksz;

	/* Alloc buffer */
	icc_buf = pxIccShmGetBuffer(ar_icc_core.mgmt, &idx, nr);
	if (!icc_buf)
		return -EAGAIN;
	//debug_print("request->size:%d,%x,%x\r\n", request->size, icc_buf, request->buf);
	memcpy(icc_buf, request->buf, request->size);

	/* Send buffer */
	ret = xIccShmTx(ar_icc_core.mgmt, request->dst_core, idx, nr,
					 request->size, request->msg_id, xIccGetCoreId(), 0);
	if (ret < 0)
		return ret;
	//debug_print("before notify:%x\r\n", peer->notify);

	ret = peer->notify(peer);
	if (ret < 0)
		return ret;

	return 0;
}

static int xIccReceive(struct icc_client *client, struct icc_request *request)
{
	struct icc_request *req;
	int ret = 0, i, found = 0;
	struct icc_msg_queue *msg_q;
	struct icc_msg *msg;
	uint32_t flags;

	if (request->msg_id == ICC_INVALI_MSG_ID)
		return -EINVAL;

	osMutexAcquire(client->mutex, osWaitForever);
	for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		msg_q = &client->msg_rx_queue[i];
		if (msg_q->msg_id != request->msg_id)
			continue;

		/* No pending msg */
		if (list_empty(&msg_q->msg_list))
			break;

		msg = list_first_entry(&msg_q->msg_list, struct icc_msg, list);

		/* If buffer size is smaller than msg size, return  */
		if (request->size < msg->size)
		{
			xDbgPrintf(PRN_UART, "[ICC] %s %d, buffer overflow, msg size=%x, buffer size=%x\n",
					   __func__, __LINE__, msg->size, request->size);
			osMutexRelease(client->mutex);
			return -EOVERFLOW;
		}

		list_del(&msg->list);

		/* Re-use buffer */
		request->buf = msg->buf;
		request->msg_id = msg_q->msg_id;
		request->dst_core = msg->core_id;
		request->size = msg->size;
		request->res = 0;
		/* Don't free msg->buf */
		free(msg);
		osMutexRelease(client->mutex);
		return 0;
	}

	/* Msg id not registered */
	if (i == ICC_CLIENT_MSG_Q_NUM)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, msgid 0x%x not registered\n",
				   __func__, __LINE__, request->msg_id);
		osMutexRelease(client->mutex);
		return -EINVAL;
	}

	/* Multi-thread is allow to receive one msgid at one time  */
	if (msg_q->cur_request)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Another thread is pending for msg id %x\n",
				   __func__, __LINE__, request->msg_id);
		osMutexRelease(client->mutex);
		return -EINVAL;
	}

	/* Set pending request */
	msg_q->cur_request = request;

	osMutexRelease(client->mutex);
	//debug_print("before wait\r\n");

	#if _FREERTOS_VERSION_ON_
	flags = xIccRecvWait(request->pIccRecvSignal, ICC_WAIT_PEROID / portTICK_PERIOD_MS);
    #else
	/* Wait for flag or time out*/
	flags = osThreadFlagsWait(ICC_WAIT_FLAG, osFlagsWaitAny, ICC_TIMEOUT);
	//debug_print("after wait\r\n");
	/* Wakeup, Clear flag */
	osThreadFlagsClear(ICC_WAIT_FLAG);
	#endif
	/* flags > 0, complete normally, flags < 0, failed */
	if ((flags > 0U) && (flags < 0x80000000U))
	{
		/* Cur_request has been cancled by handle msg */
		return 0;
	}
	else
	{
		/* Request failed, cancle client's cur_request */
		osMutexAcquire(client->mutex, osWaitForever);
		msg_q->cur_request = NULL;
		osMutexRelease(client->mutex);

		switch ((int32_t)flags)
		{
		case osErrorTimeout:
			//dbg_printf(PRN_UART, "[ICC] %s %d, Receive msg id 0x%x timeout\n",
			//__func__, __LINE__, request->msg_id);
			return -ETIME;
		default:
			//xDbgPrintf(PRN_UART, "[ICC] %s %d, Receive msg id 0x%x error(0x%x)\n",
			//		   __func__, __LINE__, request->msg_id, flags);
			return -EINVAL;
		}
	}

	return request->res;
}

static int xIccCoreInit(unsigned char *shm, unsigned int shm_len)
{
	int ret;

	memset(&ar_icc_core, 0, sizeof(ar_icc_core));

	/* Init client list */
	INIT_LIST_HEAD(&ar_icc_core.client_list);
	/* Register peer interface */
	xIccPeerInitCa(&ar_icc_core.peer[ICC_CORE_A_0], ICC_CORE_A_0);
	xIccPeerInitCa(&ar_icc_core.peer[ICC_CORE_A_1], ICC_CORE_A_1);
	xIccPeerInitCa(&ar_icc_core.peer[ICC_CORE_A_2], ICC_CORE_A_2);
	xIccPeerInitCa(&ar_icc_core.peer[ICC_CORE_A_3], ICC_CORE_A_3);
	//	icc_peer_init_cm(&ar_icc_core.peer[ICC_CORE_M_0], ICC_CORE_M_0);
	xIccPeerInitCv(&ar_icc_core.peer[ICC_CORE_CV_0], ICC_CORE_CV_0);
	xIccPeerInitCv(&ar_icc_core.peer[ICC_CORE_CV_1], ICC_CORE_CV_1);
	xIccPeerInitCv(&ar_icc_core.peer[ICC_CORE_CV_2], ICC_CORE_CV_2);
	xIccPeerInitCv(&ar_icc_core.peer[ICC_CORE_CV_3], ICC_CORE_CV_3);
	//dbg_printf(PRN_UART, "after shm init:%d, %x\r\n", ret, shm);
	xDbgPrintf(PRN_UART,"after shm init:%x\r", shm);
	ret = xIccShmInit(&ar_icc_core, shm, shm_len);
	if (ret < 0)
	{
		xDbgPrintf(PRN_UART, "icc shm init error\r");

		return ret;
	}

	xDbgPrintf(PRN_UART, "shm:0x%x\r", shm);
	ar_icc_core.mgmt = (struct icc_shm_mgmt *)shm;
	/* Create mutex */
	ar_icc_core.mutex = osMutexNew(NULL);
	xDbgPrintf(PRN_UART,"after icc_core_init:%x\r", ar_icc_core.mutex);

	return 0;
}

static int xIccCheckClient(void *client)
{
	struct icc_client *client_t;
	int found = 0;

	osMutexAcquire(ar_icc_core.mutex, osWaitForever);
	list_for_each_entry(client_t, &ar_icc_core.client_list, list)
	{
		if (client == client_t)
		{
			osMutexRelease(ar_icc_core.mutex);
			return 1;
		}
	}
	osMutexRelease(ar_icc_core.mutex);
	/* Not found client */
	return 0;
}

static void vIccCoreExit(void)
{
}

/*------------------------user api------------------------ */
static unsigned short xArIccNameHash(const char * pName)
{
    unsigned short Hash = 0;
    const char *pChar = pName;
    while ((*pChar) != 0 )
    {
        Hash = ( (Hash&1)?0x8000:0)+(Hash>>1)+(*pChar);
        pChar++;
    }
    //debug_print("pName=%s, Hash=0x%x\r\n",pName,Hash);
    return Hash;
}


int xArIccInit(unsigned char * shm, unsigned int shm_len)
{
    TaskHandle_t IccShmTaskHandler;

    // init_master_core_malloc();
    memset(g_icc_token_id, 0, sizeof(g_icc_token_id));

    xArIccCoreInit(shm, shm_len);

	#if _FREERTOS_VERSION_ON_
    //Icc signal create
    pIccSignal = xIccCreateSignal();
    //pIccRecvSignal = xIccRecvCreateSignal();

    /* icc recv task create */
    xTaskCreate( vIccShmPollRxThread,                  	/* The function that implements the task. */
    	         "IccReceiveTask",                 	 	/* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 2048,           		/* The size of the stack to allocate to the task. */
    			 ( void * )ar_icc_core.mgmt,  	 		/* The parameter passed to the task. */
    			 ICC_RECEIVE_TASK_PRIORITY,          		/* The priority assigned to the task. */
    			 &IccShmTaskHandler); 		    		/* Task data structure. */
    #endif
    return 0;
}

//robin: here we use a array to save all the token and id so that they can be dumped easily.
unsigned short xArIccFtok(const char *path)
{
    unsigned int i;

    for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
    {
    	//if this token existed, return the id
    	if (g_icc_token_id[i].token[0])
    	{
    		if(strcmp(g_icc_token_id[i].token, path) == 0)
    		{
    			if(g_icc_token_id[i].msgid)
    			{
    				return g_icc_token_id[i].msgid;
    			}
    			else
    			{
    				xDebugPrint("Find duplicated token, but no msgid, will try to create a msgid.\r\n");
    				break;
    			}
    		}
    	}
    	else
        {
            strncpy(g_icc_token_id[i].token, path, ICC_TOKEN_STR_MAXLEN);
            g_icc_token_id[i].token[ICC_TOKEN_STR_MAXLEN - 1] = 0;

            break;
        }
    }

    if(i >= ICC_CLIENT_MSG_Q_NUM)
    {
    	xDebugPrint("ICC token reaches MAX: %d!\r\n", ICC_CLIENT_MSG_Q_NUM);
    	return -1;
    }

    g_icc_token_id[i].msgid = xArIccNameHash(path);

    return g_icc_token_id[i].msgid;
}

void pArIccDumpTokenId()
{
	int i = 0;

	for(i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		if(g_icc_token_id[i].token[0])
		{
			xDebugPrint("token: %s, msgid: %d\r\n", g_icc_token_id[i].token, g_icc_token_id[i].msgid);
		}
	}
}

int xArIccRegisterMsgid(void *client, unsigned int msg_id)
{
	int ret;

	ret = xIccCheckClient(client);
	if (!ret)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Invalid client\n", __func__, __LINE__);
		return -EINVAL;
	}

	return xIccRegisterMsgid(client, msg_id);
}

int xArIccUnregisterMsgid(void *client, unsigned int msg_id)
{
	int ret;

	ret = xIccCheckClient(client);
	if (!ret)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Invalid client\n", __func__, __LINE__);
		return -EINVAL;
	}

	return xIccUnregisterMsgid(client, msg_id);
}

void *pvArIccClientCreate(void)
{
	return pvIccClientCreate();
}

void xArIccClientDestroy(void *client)
{
	int ret;

	ret = xIccCheckClient(client);
	if (!ret)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Invalid client\n", __func__, __LINE__);
		return;
	}

	return vIccClientDestroy(client);
}

int xArIccSend(void *client, unsigned char *buf, uint32_t size, uint32_t msg_id, uint32_t core_id)
{
	struct icc_request request;
	int ret;
	ret = xIccCheckClient(client);
	if (!ret)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Invalid client\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (!buf || !size)
		return -EINVAL;

	request.owner = osThreadGetId();
	request.buf = buf;
	request.msg_id = msg_id;
	request.dst_core = core_id;
	request.size = size;
	request.res = 0;

	ret = xIccSend(client, &request);
	if (ret < 0)
		goto out;

	ret = request.res;
	if (ret < 0)
	{
		goto out;
	}

out:
	return ret;
}

int xArIccReceive(void *client, unsigned char *buf, uint32_t size, uint32_t msg_id)
{
	struct icc_request request = {0};
	int ret;

	ret = xIccCheckClient(client);
	if (!ret)
	{
		xDbgPrintf(PRN_UART, "[ICC] %s %d, Invalid client\n", __func__, __LINE__);
		return -EINVAL;
	}

	if (!buf || msg_id == ICC_INVALI_MSG_ID)
		return -EINVAL;

	request.owner = osThreadGetId();
	request.buf = NULL;
	request.msg_id = msg_id;
	request.dst_core = 0;
	request.size = size;
	request.res = 0;
	#if _FREERTOS_VERSION_ON_
	request.pIccRecvSignal = xIccRecvCreateSignal();
	#endif

	ret = xIccReceive(client, &request);
	if (ret < 0)
		goto out;

	ret = request.res;
	if (ret < 0)
		goto out;
	if (!request.buf)
	{
		ret = -1;
		goto out;
	}

	memcpy(buf, request.buf, request.size);

	ret = request.size;

out:
	#if _FREERTOS_VERSION_ON_
    vIccRecvDeleteSignal(request.pIccRecvSignal);
	#endif
	if (request.buf)
		free(request.buf);
	return ret;
}

int xArIccCoreInit(unsigned char *shm, unsigned int shm_len)
{
	IccRegisterCliCommand();
	return xIccCoreInit(shm, shm_len);
}

void pArIccCorExit(void)
{
	return vIccCoreExit();
}
