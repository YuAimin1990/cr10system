
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Uart.h"
#include "IccApi.h"
#include "ArArmRmtCallDeclare.h"
#include "ArCnnOperatorDeclare.h"
#include "ArTasksSchedule.h"
#include "ArDspRuntimeCalc.h"

//********************** Globals ***************************
//**********************************************************
/* Static heap reources for tasks */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
 volatile uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]  PRAGMA_DSECT_NO_LOAD("int_sect")={0};
#endif

//record arm call dsp runtime
unsigned long long run_cycle = 0;

/* Static stack for tasks resources */
void *pIccClient = NULL;
QueueHandle_t ARMAlgxQueue = NULL;
QueueHandle_t NPUIntxQueue = NULL;
NpuSignal_t NPUAlgSignal = NULL;
AR_NETPARAM_LIST_st xGNetParamLists = {0};

static void prvARMAlgRecvTask( void *pvParameters )
{
	xDebugPrint("Starting prvARMAlgRecvTask!\r\n");

	int ret = 0;
	unsigned int xArmAlgMsgId = 0xff00ff00;//(unsigned int) xArIccFtok("CEVA_ARM_ALG");

	ret = xArIccRegisterMsgid(pIccClient, xArmAlgMsgId);
	AR_ASSERT(0 == ret);
	xDebugPrint("RECV TASK Icc Register Succeedd  ID=%x!\r\n",xArmAlgMsgId);

    AR_U32 xRecvMaxLen =ICC_MSG_LEN_MAX;
    unsigned char * xRecvMaxBuf = (unsigned char *)malloc(xRecvMaxLen);
	AR_ASSERT(NULL != xRecvMaxBuf);

	for( ;; )
	{
		memset(xRecvMaxBuf, 0, ICC_MSG_LEN_MAX);

		int u32DtcmLen = ArDTCMGetAvailableLen();
		xDebugPrint("recv u32DtcmLen = %d\n", u32DtcmLen);
		

		ret = xArIccReceive(pIccClient, (unsigned char *)xRecvMaxBuf, xRecvMaxLen, xArmAlgMsgId);
		if (ret < 0)
		{
			xDebugPrint("prvARMAlgRecvTask Receive Timeout,Continue ret=%d\r\n",ret);
			continue;
		}

		xDebugPrint("Receive Succeed CmdName = %s\r\n",((AR_DSP_TASK_IOCTL_S *)xRecvMaxBuf)->u32CmdName);

		if(errQUEUE_FULL == xQueueSend(ARMAlgxQueue, xRecvMaxBuf, 0U))
		{
			xDebugPrint("ARMAlgxQueue FULL ERR\r\n");
		}
	}

	if(xRecvMaxBuf)
	{
		free(xRecvMaxBuf);
	}
}


ArARMRemoteCallDeclare_t * xArArmRemoteCallLookUpName(const char* pName)
{
	ArARMRemoteCallDeclare_t *entry = NULL;
	ArARMRemoteCallDeclare_t *start = xArArmRemoteCallEntryStart(ArARMRemoteCallDeclare_t);
	const int32_t n_entry = xArArmRemoteCallEntryCount(ArARMRemoteCallDeclare_t);
	for (entry = start; entry != start + n_entry; entry++)
	{
		if (!strcmp(pName, entry->pName))
			return entry;
	}
	xDebugPrint("RmtCall %s not register!\r\n",pName);
	return NULL;
}


static void prvARMAlgProcTask( void *pvParameters )
{
	xDebugPrint("Starting prvARMAlgProcTask\n");

	int ret = 0;
	unsigned int xArmAlgMsgId = 0xff00ff00;//(unsigned int) xArIccFtok("CEVA_ARM_ALG");
	ret = xArIccRegisterMsgid(pIccClient, xArmAlgMsgId);
	AR_ASSERT(0 == ret);
	xDebugPrint("PROC TASK Icc Register Succeedd ID =%x!\r\n",xArmAlgMsgId);

	AR_U32 xRecvMaxLen =ICC_MSG_LEN_MAX;
	unsigned char * ARMAlgTaskBuf = (unsigned char *)malloc(xRecvMaxLen);
	AR_ASSERT(NULL != ARMAlgTaskBuf);

	for( ;; )
	{
		memset(ARMAlgTaskBuf, 0, ICC_MSG_LEN_MAX);
		xQueueReceive(ARMAlgxQueue, (unsigned char *)ARMAlgTaskBuf, portMAX_DELAY );
        if(ARMAlgTaskBuf == NULL)
        {
        	continue;
		}

		AR_DSP_TASK_IOCTL_S *xARMAlgTaskHeader=NULL;
		xARMAlgTaskHeader=(AR_DSP_TASK_IOCTL_S *)ARMAlgTaskBuf;
		unsigned char * xARMAlgTaskBody = NULL;
		xARMAlgTaskBody = (unsigned char*)ARMAlgTaskBuf + sizeof(AR_DSP_TASK_IOCTL_S);

		ArARMRemoteCallDeclare_t* entry=NULL;
		entry=xArArmRemoteCallLookUpName(xARMAlgTaskHeader->u32CmdName);
		if(entry)
		{
			ret=entry->pRmtCallHandler((void*)xARMAlgTaskBody);
		}
		xDebugPrint("%s Process Done RET = %d\r\n",xARMAlgTaskHeader->u32CmdName,ret);


		AR_DSP_TASK_ACK_IOCTL_S ARMAlgTaskAck={0};
		memcpy(&ARMAlgTaskAck, xARMAlgTaskHeader, sizeof(AR_DSP_TASK_IOCTL_S));
		ARMAlgTaskAck.s32ErrCode = ret;
		ARMAlgTaskAck.u32Time= run_cycle;
		ret = xArIccSend(pIccClient, (unsigned char *)&ARMAlgTaskAck, sizeof(AR_DSP_TASK_ACK_IOCTL_S), xArmAlgMsgId, ICC_CORE_A_0);
		if (ret < 0)
		{
		   xDebugPrint("ICC Send error\r\n");
		}

	}
	if(ARMAlgTaskBuf)
	{
		free(ARMAlgTaskBuf);
	}
}

AR_U32 xArNpuCalcCallbackID(AR_NPU_IRQ_PARAMS_st xNpuIrqParams)
{
    AR_U32 u32CBId = 0xFFFFFFFF;
	AR_U32 * pu32Bitmap = xNpuIrqParams.au32CBAckIDs;
    //if a callback need to process
    for(int i = 0; i < 4; i++)
    {
        if(pu32Bitmap[i])
        {
            for(int j = 0; j < 32; j++)
            {
                if(pu32Bitmap[i] & (1<<j))
                {
                    u32CBId = j + i*32;
                    return u32CBId;
                }
            }
        }
    }
    xDebugPrint("No callback id found!\n");
    return u32CBId;
}


AR_NPU_LIST_ITEMS_st* xArNpuCallBackLookUpParameters(AR_U16 xNetworkId)
{

		AR_NETPARAM_LIST_st* node =pxArNetParamListGetByNetWorkId(&xGNetParamLists.list, xNetworkId);
		if(node ==NULL)
		{
			xDebugPrint("listnode is NULL!\n");
		}
		return node->xNetParam;
}

ArCnnOperatorDeclare_t *xArNpuOperatorLookUpName(AR_NPU_CB_PARAM_S *xNpuCallBackParams)
{
	ArCnnOperatorDeclare_t *entry = NULL;
	ArCnnOperatorDeclare_t *start = xArCnnOperatorEntryStart(ArCnnOperatorDeclare_t);
	const int32_t n_entry = xArCnnOperatorEntryCount(ArCnnOperatorDeclare_t);
	for (entry = start; entry != start + n_entry; entry++)
	{
		if (!strcmp(xNpuCallBackParams->achOperatorType, entry->pName))
			return entry;
	}
	xDebugPrint("CB Operator %s not register!\r\n",xNpuCallBackParams->achOperatorName);
	return NULL;

}


static void prvNPUAlgProcTask( void *pvParameters )
{
	xDebugPrint("Starting prvNPUAlgProcTask\r\n");
    int i = 0, j = 0;
	int ret = 0;
	unsigned long long u64TimeStart = 0;
	unsigned long long u64TimeEnd = 0;
	AR_U32	u32CBId =0;
	unsigned int u32OutputAddr;
	unsigned int u32InputAddr; 

	xArArmRuntimeInit();
	for( ;; )
	{
		xDebugPrint("Before NPUAlgSignal Wakeup \r\n");
		xNpuwait(NPUAlgSignal,portTICK_PERIOD_MS);

		AR_NPU_IRQ_PARAMS_st xNpuIrqParams = {0};
		xQueueReceive(NPUIntxQueue, &xNpuIrqParams, portMAX_DELAY );


		AR_U32 NetWorkId = xNpuIrqParams.u32NetworkId;
		xDebugPrint("NPU NetWorkId %x\r\n",NetWorkId);
		AR_NPU_LIST_ITEMS_st *xNetParam=xArNpuCallBackLookUpParameters(NetWorkId);

		for(i = 0; i < 4; i++)
		{
		   if(xNpuIrqParams.au32CBAckIDs[i])
		   {
			   for(j = 0; j < 32; j++)
			   {
				   if(xNpuIrqParams.au32CBAckIDs[i] & (1<<j))
				   {
					   u32CBId = j + i*32;
					   xDebugPrint("CallBackId %x\r\n",u32CBId);	   
					   xDebugPrint("NPU opName %s opType %s\r\n",xNetParam->astCBParam[u32CBId].achOperatorName,
					   											 xNetParam->astCBParam[u32CBId].achOperatorType);

					   if( (xNetParam->astCBParam[u32CBId].u32InputTensorNum ==1) &&
						   (xNetParam->astCBParam[u32CBId].u32OutputTensorNum ==1))
					   {
						   if((0 == strcmp(xNetParam->astCBParam[u32CBId].astInputTensor[0].achMemoryType, "input")) &&
						      (0 == strcmp(xNetParam->astCBParam[u32CBId].astOutputTensor[0].achMemoryType, "input")) )
							{
								u32OutputAddr = *(unsigned int *)0x0114003c;
								u32InputAddr = *(unsigned int *)0x0114003c;
							}
							else if((0 == strcmp(xNetParam->astCBParam[u32CBId].astInputTensor[0].achMemoryType, "output")) &&
							        (0 == strcmp(xNetParam->astCBParam[u32CBId].astOutputTensor[0].achMemoryType, "output")) )
							{
								u32OutputAddr = *(unsigned int *)0x011400bc;
								u32InputAddr = *(unsigned int *)0x011400bc;
							}
							else if((0 == strcmp(xNetParam->astCBParam[u32CBId].astInputTensor[0].achMemoryType, "input")) &&
							        (0 == strcmp(xNetParam->astCBParam[u32CBId].astOutputTensor[0].achMemoryType, "output")) )
							{
								u32OutputAddr = *(unsigned int *)0x0114003c;
								u32InputAddr = *(unsigned int *)0x011400bc;
							}
							else
							{
								u32OutputAddr = *(unsigned int *)0x011400bc;
								u32InputAddr = *(unsigned int *)0x0114003c;
							}

					   	    ArCnnOperatorDeclare_t * entry=NULL;
						    entry=xArNpuOperatorLookUpName(&xNetParam->astCBParam[u32CBId]);
						    if(entry)
						    {
							    u64TimeStart = mss_get_clock();
							    ret=entry->pCnnCbHandler(u32OutputAddr, u32InputAddr,&xNetParam->astCBParam[u32CBId], (void*)xNetParam->astCBParam[u32CBId].pOpParams);
							    u64TimeEnd = mss_get_clock();

							    xArArmRuntimeAdd(xNetParam->astCBParam[u32CBId].achOperatorType, xNetParam->astCBParam[u32CBId].u32Id, u64TimeEnd - u64TimeStart);
						    }
						    xDebugPrint("NPU CB Process Done RET=%d\n",ret);
				   	   }
				   	   else
				   	   {
				   	        u32InputAddr = *(unsigned int *)0x0114003c;
							u32OutputAddr = *(unsigned int *)0x011400bc;

							ArCnnOperatorDeclare_t * entry=NULL;
						    entry=xArNpuOperatorLookUpName(&xNetParam->astCBParam[u32CBId]);
						    if(entry)
						    {
							    u64TimeStart = mss_get_clock();
							    ret=entry->pCnnCbHandler(u32InputAddr, u32OutputAddr,&xNetParam->astCBParam[u32CBId], (void*)xNetParam->astCBParam[u32CBId].pOpParams);
							    u64TimeEnd = mss_get_clock();

							    xArArmRuntimeAdd(xNetParam->astCBParam[u32CBId].achOperatorType, xNetParam->astCBParam[u32CBId].u32Id, u64TimeEnd - u64TimeStart);
						    }
						    xDebugPrint("NPU CB Process Done RET=%d\n",ret);

					   }
				   }
			   }
		   }
		}

	    //write back to indicate callback completement and this will clear the irq
	    vWriteReg32(NPU_CALLBACK_ACK_ID_0_REG, xNpuIrqParams.au32CBAckIDs[0]);
	    vWriteReg32(NPU_CALLBACK_ACK_ID_1_REG, xNpuIrqParams.au32CBAckIDs[1]);
	    vWriteReg32(NPU_CALLBACK_ACK_ID_2_REG, xNpuIrqParams.au32CBAckIDs[2]);
	    vWriteReg32(NPU_CALLBACK_ACK_ID_3_REG, xNpuIrqParams.au32CBAckIDs[3]);

	}
}

void xArTasksScheduleinit(void)
{
		pIccClient = pvArIccClientCreate();
		AR_ASSERT(NULL != pIccClient);
		xDebugPrint("pIccClient %x\r\n",pIccClient);

		unsigned int xItemMaxLen = 1024;
		ARMAlgxQueue = xQueueCreate(ARM_TASK_QUEUE_LENGTH, xItemMaxLen);
		AR_ASSERT( NULL != ARMAlgxQueue);
		xDebugPrint("ARMAlgxQueue %x\r\n",ARMAlgxQueue);

		NPUIntxQueue = xQueueCreate(NPU_TASK_QUEUE_LENGTH, sizeof(AR_NPU_IRQ_PARAMS_st));
		AR_ASSERT( NULL != NPUIntxQueue);
		xDebugPrint("NPUIntxQueue %x\r\n",NPUIntxQueue);

		NPUAlgSignal = xNpuCreatSignal();
		AR_ASSERT( NULL != NPUAlgSignal);
		xDebugPrint("NPUAlgSignal %x\r\n",NPUAlgSignal);

		xArNetParamListInit(&xGNetParamLists.list);

}

int xArTasksSchedule( )
{
	TaskHandle_t prvARMAlgRecvTaskhandler = NULL;
	TaskHandle_t prvARMAlgProcTaskhandler = NULL;
	TaskHandle_t prvNPUAlgProcTaskhandler = NULL;

	xDebugPrint("Enter xArTasksSchedule\r\n");

	xArTasksScheduleinit();

	/* Start the two tasks as described in the comments at the top of this file. */
	xTaskCreate( prvARMAlgRecvTask,                   /* The function that implements the task. */
               	 "ARMAlgRecvTask",                 /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,            /* The size of the stack to allocate to the task. */
				 ( void * ) NULL,                  	  /* The parameter passed to the task. */
				 ARM_ALG_RECV_TASK_PRIORITY,          /* The priority assigned to the task. */
				 &prvARMAlgRecvTaskhandler); 		  /* Task data structure. */



	/* Start the two tasks as described in the comments at the top of this file. */
	xTaskCreate( prvARMAlgProcTask,                   /* The function that implements the task. */
                 "ARMAlgProcTask",                 	  /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 2*configMINIMAL_STACK_SIZE,            /* The size of the stack to allocate to the task. */
				 ( void * ) NULL,  				      /* The parameter passed to the task. */
				 ARM_ALG_PROC_TASK_PRIORITY,          /* The priority assigned to the task. */
				 &prvARMAlgProcTaskhandler ); 		  /* Task data structure. */



	xTaskCreate( prvNPUAlgProcTask,						/* The function that implements the task. */
				 "NPUAlgProcTask",					 	/* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 2*configMINIMAL_STACK_SIZE, 				/* The size of the stack to allocate to the task. */
				 ( void * ) NULL,  						/* The parameter passed to the task.  */
				 NPU_ALG_PROC_TASK_PRIORITY, 				/* The priority assigned to the task. */
				 &prvNPUAlgProcTaskhandler );					/* Task data structure. */

	prvDSPInfoTaskCreate();

	/* Start the timer and the tasks running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details. */
	xDebugPrint("finished schedule\r\n");

	for( ;; );

	vDmaDeInit();
	
	return 0;
}



int xArBaremetalLoop()
{
    xDebugPrint("Starting xArBaremetalLoop\r\n");
	
	pIccClient = pvArIccClientCreate();
	AR_ASSERT(NULL != pIccClient);
	xDebugPrint("pIccClient %x\r\n",pIccClient);

	int ret = 0;
	unsigned int xArmAlgMsgId = 0xff00ff00;
	ret = xArIccRegisterMsgid(pIccClient, xArmAlgMsgId);
	AR_ASSERT(0 == ret);
	xDebugPrint("baremetal Icc Register Succeedd  ID=%x!\r\n",xArmAlgMsgId);

    AR_U32 xRecvMaxLen =ICC_MSG_LEN_MAX;
    unsigned char * xRecvMaxBuf = (unsigned char *)malloc(xRecvMaxLen);
	AR_ASSERT(NULL != xRecvMaxBuf);
	

	for( ;; )
	{
		memset(xRecvMaxBuf, 0, ICC_MSG_LEN_MAX);
		//xDebugPrint("BareMetal Receive start\r\n");

		ret = xArIccReceive(pIccClient, (unsigned char *)xRecvMaxBuf, xRecvMaxLen, xArmAlgMsgId);
		if (ret < 0)
		{
			xDebugPrint("BareMetal Receive Timeout or Error!\r\n");
			continue;
		}

		xDebugPrint("BareMetal Receive CmdName = %s\r\n",((AR_DSP_TASK_IOCTL_S *)xRecvMaxBuf)->u32CmdName);

		AR_DSP_TASK_IOCTL_S *xARMAlgTaskHeader=NULL;
		xARMAlgTaskHeader=(AR_DSP_TASK_IOCTL_S *)xRecvMaxBuf;
		unsigned char * xARMAlgTaskBody = NULL;
		xARMAlgTaskBody = (unsigned char*)xRecvMaxBuf + sizeof(AR_DSP_TASK_IOCTL_S);

		ArARMRemoteCallDeclare_t* entry=NULL;
		entry=xArArmRemoteCallLookUpName(xARMAlgTaskHeader->u32CmdName);
		if(entry)
		{
			ret=entry->pRmtCallHandler((void*)xARMAlgTaskBody);
		}
		xDebugPrint("BareMetal  %s Process Done RET = %d\r\n",xARMAlgTaskHeader->u32CmdName,ret);


		AR_DSP_TASK_ACK_IOCTL_S ARMAlgTaskAck={0};
		memcpy(&ARMAlgTaskAck, xARMAlgTaskHeader, sizeof(AR_DSP_TASK_IOCTL_S));
		ARMAlgTaskAck.s32ErrCode = ret;
		ARMAlgTaskAck.u32Time= run_cycle;
		ret = xArIccSend(pIccClient, (unsigned char *)&ARMAlgTaskAck, sizeof(AR_DSP_TASK_ACK_IOCTL_S), xArmAlgMsgId, ICC_CORE_A_0);
		if (ret < 0)
		{
		   xDebugPrint("BareMetal ICC Send error\r\n");
		}
		//xDebugPrint("BareMetal Send end\r\n");
	}

    xDebugPrint("finished baremetal\r\n");
    return 0;

}


