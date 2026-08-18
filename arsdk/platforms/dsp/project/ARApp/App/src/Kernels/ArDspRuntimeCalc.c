#include "ArArmRmtCallDeclare.h"
#include "ArDspRuntimeCalc.h"
#include "IccApi.h"

//AR_DSP_RUNTIME_STATUS_S  s8Runtime  	PRAGMA_DSECT_LOAD("ext_data_sect") ={0};
//#define FPGA_TEST

void * pvRuntimeClient = NULL;
AR_U32 ulRuntimeMsgId = 0xff00ffff;
DspRuntimeStatus_s* pxRuntime = NULL;

AR_S32 xArArmRuntimeInit(void)
{
	AR_S32 s32Ret = 0;

	pvRuntimeClient = pvArIccClientCreate();
	AR_ASSERT(NULL != pvRuntimeClient);
	xDebugPrint("xArArmRuntimeInit pvRuntimeClient %x\r\n",pvRuntimeClient);

	s32Ret = xArIccRegisterMsgid(pvRuntimeClient, ulRuntimeMsgId);
	AR_ASSERT(0 == s32Ret);
	xDebugPrint("xArArmRuntimeInit Register Succeedd  ID=%x!\r\n",ulRuntimeMsgId);

	pxRuntime = (DspRuntimeStatus_s * )malloc(sizeof(DspRuntimeStatus_s));
	if(!pxRuntime)
	{
		xDebugPrint("xArArmRuntimeInit pstRuntime malloc failed \n");
		return -1;
	}
	memset(pxRuntime, 0, sizeof(DspRuntimeStatus_s));

	pxRuntime->xMutex = osMutexNew(NULL);
	return 0;
}

AR_S32 xArArmRuntimeAdd(AR_CHAR * pcNetName, AR_U32 ulNetId, AR_U64 ulCycle)
{
	if((!pcNetName) || (!pxRuntime))
	{
		xDebugPrint("xArArmRuntimeAdd pstRuntime or pcNetName with error parameters\n");
		return -1;
	}

	osMutexAcquire(pxRuntime->xMutex, osWaitForever);
	if(pxRuntime->ulTotalCnt == AR_DSP_MAX_RUNTIME_SIZE)
	{
		pxRuntime->ulTotalCnt = 0;
		xDebugPrint("xArArmRuntimeAdd the total cnt upto the max num, reset to zero\n");
	}

	strcpy(pxRuntime->xDspRuntime[pxRuntime->ulTotalCnt].cNetName, pcNetName);
	pxRuntime->xDspRuntime[pxRuntime->ulTotalCnt].ulNetworkId = ulNetId;
	pxRuntime->xDspRuntime[pxRuntime->ulTotalCnt].ulCycleTime = ulCycle;
	pxRuntime->ulTotalCnt++;
	osMutexRelease(pxRuntime->xMutex);
	return 0;
}

AR_S32 xArArmCalcRuntime(void *pParams)
{
	AR_S32 s32Ret = 0;
	AR_U32 u32SendSize = 0;

	if((!pvRuntimeClient) || (!pxRuntime))
	{
		xDebugPrint("xArArmCalcRuntime pstRuntime or pvRuntimeClient with error parameters\n");
		return -1;
	}

#ifdef FPGA_TEST
	AR_CHAR cNetName[32] = "goognet";
	for(int i =0;i<100;i++){
		xArArmRuntimeAdd(cNetName, 1, 123456);
	}
#endif

	u32SendSize = AR_DSP_MAX_RUNTIME_SIZE * sizeof(DspRuntime_t);

	s32Ret = xArIccSend(pvRuntimeClient, (unsigned char *)pxRuntime->xDspRuntime, u32SendSize, ulRuntimeMsgId, ICC_CORE_A_0);
	if (s32Ret < 0)
	{
	   xDebugPrint("xArArmCalcRuntime Send error\r\n");
	}
	else
	{
		xDebugPrint("xArArmCalcRuntime Send success and clear record info\r\n");
		osMutexAcquire(pxRuntime->xMutex, osWaitForever);
		memset(pxRuntime->xDspRuntime, 0, sizeof(DspRuntime_t));
		pxRuntime->ulTotalCnt = 0;
		osMutexRelease(pxRuntime->xMutex);
	}

	return 0;
}

AR_ARM_REMOTECALL_DECLARE(xArArmCalcRuntime) = {
    .pName = "ArArmCalcRuntime",
    .pRmtCallHandler = xArArmCalcRuntime,
};

