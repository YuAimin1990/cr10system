#ifndef __AR_DSP_STATUSINFO_H__
#define __AR_DSP_STATUSINFO_H__

#ifdef __cplusplus
extern "C"{
#endif

//define the max task num and the max interrupt num
#define MAX_TASK_NUM	(10)
#define MAX_IRQ_NUM		(5)

//define dsp information task priority
#define DSP_INFO_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

#pragma pack(4)
typedef struct xTASK_INFO
{
  	unsigned int ulTaskId;
  	unsigned int ulHeap;
  	char cTaskStatus;
  	unsigned int ulPriority;
  	unsigned int ulMallocCnt;
  	char cTaskName[32];
  	unsigned int ulCpuRate;
} TaskInfo_t;

typedef struct xIRQ_INFO
{
	unsigned int ulIrqId;
	unsigned int ulIrqCount;
} IrqInfo_t;

typedef struct xDSP_INFO
{
	unsigned int ulDspCoreId;
   	char cDspCoreStatus[8];
	unsigned int ulDspKeep_Alive;
   	unsigned int ulDspMemInfo[2];
   	unsigned int ulTask_Num;
   	TaskInfo_t xTaskInfo[MAX_TASK_NUM];
   	unsigned int ulIrq_Num;
   	IrqInfo_t xIrqInfo[MAX_IRQ_NUM];
} DspInfo_t;
#pragma pack()

typedef enum
{
	eNpuIrq = 0,
	eIccIrq,
	eDspIrq,
	eOtherIrq
} eIrqInfo;

extern int vDspInfoAddrInit(char *cBufferAddr);
extern DspInfo_t* xDspGetInfoAddr(void);
extern void vIrqSetInfo(eIrqInfo xIrqId);
extern void prvDSPInfoTaskCreate(void);

#ifdef __cplusplus
}
#endif
#endif
