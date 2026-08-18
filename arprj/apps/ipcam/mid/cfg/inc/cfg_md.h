#ifndef _CFG_MD_H__
#define _CFG_MD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"
#include "cfg_alarm.h"


/***********************************/
/***    motion detect            ***/
/***********************************/

typedef struct
{
    AR_BOOL bEnable;
	AR_U32  u32GrpId;
	AR_U32  u32ChnId;
	AR_U16  u16MatchingThr;
	AR_U32  u32AreaPercentage;
	AR_U32  u32PostMethod;

} IPC_CFG_MD_S;


AR_S32 IPC_CFG_MD_Save();
AR_S32 IPC_CFG_MD_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_MD_LoadDefault();
void IPC_CFG_MD_Print();
IPC_CFG_MD_S * IPC_CFG_MD_GetParam();

#define MD_CFG_FILE "cfg_md.json"

#ifdef __cplusplus
}
#endif
#endif

