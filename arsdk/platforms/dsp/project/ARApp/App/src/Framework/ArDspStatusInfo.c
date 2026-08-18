
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Uart.h"
#include "IccApi.h"
#include "ArDspStatusInfo.h"


volatile uint64_t ulCpu_RunTime;
static DspInfo_t *pDspInfoAddrStart;
static IrqInfo_t xIrqInfo[MAX_IRQ_NUM];
uint64_t ulCpuLastRunTime = 1;
extern uint64_t ulGetArNpuIrqCnt(void);


//#define FPGA_TEST

void vApplicationTickHook( void )
{
	ulCpu_RunTime++;
}
/**********************************************************************************************************************
 **********************************************************************************************************************/

int vDspInfoAddrInit(char *cBufferAddr)
{
	pDspInfoAddrStart = (DspInfo_t *)cBufferAddr;
	if(sizeof(DspInfo_t) > 1024)
	{
		xDebugPrint("vDspInfoAddrInit failed, size of DspInfo_t too large\n");
		return -1;
	}

	memset(pDspInfoAddrStart,0,sizeof(DspInfo_t));
	xDebugPrint("vDspInfoAddrInit.pDspInfoAddrStart = %x \r\n",pDspInfoAddrStart);
	return 0;
}

DspInfo_t* xDspGetInfoAddr(void)
{
	return (DspInfo_t*)pDspInfoAddrStart;
}

void vIrqSetInfo(eIrqInfo xIrqId)	//get for every isr func
{
	xIrqInfo[xIrqId].ulIrqId = xIrqId;
	xIrqInfo[xIrqId].ulIrqCount++;
}

#ifndef FPGA_TEST
void vDspGetInfo(void)
{
	UBaseType_t xTaskNum = 0;
	TaskStatus_t* pxStatusArry;
	uint32_t ulTotalRunTime;
	UBaseType_t xArrySize;
	DspInfo_t* pxDspInfoAddr;
	int i = 0;

	xTaskNum = uxTaskGetNumberOfTasks();
	pxStatusArry = malloc(xTaskNum * sizeof(TaskStatus_t));

	pxDspInfoAddr = xDspGetInfoAddr();

	pxDspInfoAddr->ulDspCoreId = 0;
	if(ulCpuLastRunTime != ulCpu_RunTime)	//adjust to dsp status
	{
		strcpy(pxDspInfoAddr->cDspCoreStatus, "Active");
	}
	else
	{
		strcpy(pxDspInfoAddr->cDspCoreStatus, "Dumped");
	}
	ulCpuLastRunTime = ulCpu_RunTime;
	pxDspInfoAddr->ulDspKeep_Alive = ulCpu_RunTime;
	pxDspInfoAddr->ulTask_Num = xTaskNum;
	if(pxStatusArry != NULL)
	{
		xArrySize = uxTaskGetSystemState((TaskStatus_t *)pxStatusArry,(UBaseType_t)xTaskNum,(uint32_t *)&ulTotalRunTime);
		ulTotalRunTime /= 100UL;

		for(i = 0;i < xTaskNum; i++)
		{
			
			strcpy(pxDspInfoAddr->xTaskInfo[i].cTaskName, pxStatusArry[i].pcTaskName);
			pxDspInfoAddr->xTaskInfo[i].ulTaskId = pxStatusArry[i].xTaskNumber;

			pxDspInfoAddr->xTaskInfo[i].ulHeap = pxStatusArry[i].ulHeapUsed;
			pxDspInfoAddr->xTaskInfo[i].ulMallocCnt = pxStatusArry[i].ulMallocCnt;

			if(ulTotalRunTime != 0)
			{
				pxDspInfoAddr->xTaskInfo[i].ulCpuRate = pxStatusArry[i].ulRunTimeCounter/ulTotalRunTime;
			}
			//xDebugPrint("ulRunTimeCounter=%lu,TotalRunTime = %lu\r\n",pxStatusArry[i].ulRunTimeCounter,ulTotalRunTime);
			switch(pxStatusArry[i].eCurrentState)
			{
				case eRunning:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'r';
					break;
				case eReady:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'R';
					break;
				case eBlocked:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'B';
					break;
				case eSuspended:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'S';
					break;
				case eDeleted:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'D';
					break;
				default:
					pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'I';
					break;
			}
			
			pxDspInfoAddr->xTaskInfo[i].ulPriority = pxStatusArry[i].uxCurrentPriority;
		}
	}
	free(pxStatusArry);

	pxDspInfoAddr->ulIrq_Num = 3;
	pxDspInfoAddr->xIrqInfo[eNpuIrq].ulIrqId = eNpuIrq;
	pxDspInfoAddr->xIrqInfo[eNpuIrq].ulIrqCount = ulGetArNpuIrqCnt();
	pxDspInfoAddr->xIrqInfo[eDspIrq].ulIrqId = eDspIrq;
	pxDspInfoAddr->xIrqInfo[eDspIrq].ulIrqCount = xTaskGetTickCount();
	pxDspInfoAddr->xIrqInfo[eIccIrq].ulIrqId = eIccIrq;
	pxDspInfoAddr->xIrqInfo[eIccIrq].ulIrqCount = xArIccIsrCnt();
	//xDebugPrint("\r\n");
}
#else

int DspGetInfo_fpga(void)
{
	UBaseType_t xTaskNum = 0;
	TaskStatus_t* pxStatusArry;
	uint32_t ulTotalRunTime = 0;
	uint64_t ulCpuLastRunTime = 0;
	UBaseType_t xArrySize;
	DspInfo_t* pxDspInfoAddr;
	int i = 0;


	xTaskNum = uxTaskGetNumberOfTasks();
	xDebugPrint("xTaskNum = %d\r\n",xTaskNum);

	pxStatusArry = malloc(xTaskNum * sizeof(TaskStatus_t));
	if(!pxStatusArry)
	{
		xDebugPrint("malloc pxStatusArry failed!\r\n");
		return -1;
	}


	xDebugPrint("cDspCoreStatus = active");


	if(pxStatusArry != NULL)
	{
		xArrySize = uxTaskGetSystemState((TaskStatus_t *)pxStatusArry,(UBaseType_t)xTaskNum,(uint32_t *)&ulTotalRunTime);

		xDebugPrint("ulTotalRunTime = %d;xArrySize = %d\r\n",ulTotalRunTime,xArrySize);

				/* For percentage calculations. */
				ulTotalRunTime /= 100UL;

				for(i = 0;i < xTaskNum; i++)
				{
					//pxDspInfoAddr->ulTask_Num = xTaskNum;
					//strcpy(pxDspInfoAddr->xTaskInfo[i].cTaskName,pxStatusArry[i].pcTaskName);
					//pxDspInfoAddr->xTaskInfo[i].ulTaskId = pxStatusArry[i].xTaskNumber;

					//pxDspInfoAddr->xTaskInfo[i].ulHeap = pxStatusArry[i].ulHeapUsed;
					//pxDspInfoAddr->xTaskInfo[i].ulMallocCnt = pxStatusArry[i].ulMallocCnt;

					xDebugPrint("xTaskNum=%d,pcTaskName=%s,ulHeapUsed=%x,ulMallocCnt=%d\r\n",pxStatusArry[i].xTaskNumber,pxStatusArry[i].pcTaskName,pxStatusArry[i].ulHeapUsed,pxStatusArry[i].ulMallocCnt);
					if(ulTotalRunTime != 0)
					{
						//pxDspInfoAddr->xTaskInfo[i].ulCpuRate = pxStatusArry[i].ulRunTimeCounter/ulTotalRunTime;

						xDebugPrint("ulCpuRate = %lu\r\n",pxStatusArry[i].ulRunTimeCounter/ulTotalRunTime);
					}
					xDebugPrint("ulRunTimeCounter=%lu,TotalRunTime = %lu\r\n",pxStatusArry[i].ulRunTimeCounter,ulTotalRunTime);

					if(pxStatusArry[i].eCurrentState == eRunning)
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'r';
						xDebugPrint("eCurrentState = eRunning\r\n");
					}
					else if(pxStatusArry[i].eCurrentState == eReady)
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'R';
						xDebugPrint("eCurrentState = eReady\r\n");
					}
					else if(pxStatusArry[i].eCurrentState == eBlocked)
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'B';
						xDebugPrint("eCurrentState = eBlocked\r\n");
					}
					else if(pxStatusArry[i].eCurrentState == eSuspended)
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'S';
						xDebugPrint("eCurrentState = eSuspended\r\n");
					}
					else if(pxStatusArry[i].eCurrentState == eDeleted)
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'D';
						xDebugPrint("eCurrentState = eDeleted\r\n");
					}
					else
					{
						//pxDspInfoAddr->xTaskInfo[i].cTaskStatus = 'I';
						xDebugPrint("eCurrentState = invalid\r\n");
					}

					//pxDspInfoAddr->xTaskInfo[i].ulPriority = pxStatusArry[i].uxCurrentPriority;
					xDebugPrint("uxCurrentPriority = %d\r\n",pxStatusArry[i].uxCurrentPriority);
				}

			}

			free(pxStatusArry);


	return 0;
}
#endif
void prvDSPInfoTask( void *pvParameters )
{
	xDebugPrint("start prvDSPInfoTask task!\r\n");

	for(;;)
	{
		vDspGetInfo();
		//xDebugPrint("run prvDSPInfoTask task!\r\n");
		vTaskDelay(60000/portTICK_RATE_MS);
	}
}

void prvDSPInfoTaskCreate(void)
{
	TaskHandle_t prvDSPInfoTaskhandler = NULL;

	extern char __shm_status_start;
	vDspInfoAddrInit((char *)&__shm_status_start);
	printf("vDspInfoAddrInit start addr = %p\n",&__shm_status_start);

	xTaskCreate( prvDSPInfoTask,					/* The function that implements the task. */
			     "DSPInfoTask",					 	/* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE, 								/* The size of the stack to allocate to the task. */
				 ( void * ) NULL,  					/* The parameter passed to the task.  */
				 DSP_INFO_TASK_PRIORITY, 			/* The priority assigned to the task. */
				 &prvDSPInfoTaskhandler );			/* Task data structure. */

	xDebugPrint("prvDSPInfoTask has been create!\r\n");
}
