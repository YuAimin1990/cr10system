#ifndef __AR_DSP_RUNTIME_CALC_H__
#define __AR_DSP_RUNTIME_CALC_H__

#include "IccPort.h"

#define AR_DSP_MAX_RUNTIME_SIZE 1024

#pragma pack (4)
typedef struct xDSP_RUNTIME
{
	AR_CHAR cNetName[32];
	AR_U32	ulNetworkId;
	AR_U64	ulCycleTime;
} DspRuntime_t;
#pragma pack ()

typedef struct xDSP_RUNTIME_STATUS
{
	DspRuntime_t  xDspRuntime[AR_DSP_MAX_RUNTIME_SIZE];
	AR_U32 ulTotalCnt;
	osMutexId_t xMutex;
} DspRuntimeStatus_s;

AR_S32 xArArmRuntimeInit(void);
AR_S32 xArArmRuntimeAdd(AR_CHAR * pcNetName, AR_U32 ulNetId, AR_U64 ulCycle);
#endif
