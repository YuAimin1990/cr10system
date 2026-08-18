#ifndef __CFG_CONTROL_H__
#define __CFG_CONTROL_H__

#include "cfg_common.h"

#define IPC_MAX_CONTRL_ELE_NEEDED 32

typedef struct
{
	AR_U32 u32ModId;;
	AR_U32 u32DevId;
	AR_U32 u32ChnId;
	AR_U32 u32Status;
} CONTROL_ELE_S;

typedef struct
{
	AR_BOOL bEnable;
	AR_U32 u32PipelineNum;
	CONTROL_ELE_S sCtrSrcEle;
	CONTROL_ELE_S sCtrDstEle;
} CONTROL_CFG_S;

typedef struct
{
	CONTROL_CFG_S stCtrCfg[IPC_MAX_CONTRL_ELE_NEEDED];
} IPC_CFG_CONTROL_S;

AR_S32 IPC_CFG_CONTROL_Save();
AR_S32 IPC_CFG_CONTROL_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_CONTROL_LoadDefault();
void IPC_CFG_CONTROL_Print();
IPC_CFG_CONTROL_S * IPC_CFG_CONTROL_GetParam();

#define CONTROL_CFG_FILE "cfg_control.json"

#endif
