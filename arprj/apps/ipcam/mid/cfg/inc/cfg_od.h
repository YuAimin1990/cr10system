#ifndef _CFG_OD_H__
#define _CFG_OD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"
#include "cfg_alarm.h"


/***********************************/
/***    occlusion detect            ***/
/***********************************/

typedef struct
{
    AR_BOOL bEnable;
	AR_U32  u32GrpId;
	AR_U32  u32ChnId;
	AR_U32  u32AlarmBlockNumThred;

} IPC_CFG_OD_S;


AR_S32 IPC_CFG_OD_Save();
AR_S32 IPC_CFG_OD_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_OD_LoadDefault();
void IPC_CFG_OD_Print();
IPC_CFG_OD_S * IPC_CFG_OD_GetParam();

#define OD_CFG_FILE "cfg_od.json"

#ifdef __cplusplus
}
#endif
#endif

