
#ifndef _CFG_ALL_H_
#define _CFG_ALL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_vio.h"
#include "cfg_venc.h"

#include "cfg_record.h"
#include "cfg_snap.h"
#include "cfg_vpss.h"

extern AR_S32 IPC_CFG_SaveAll();
extern AR_S32 IPC_CFG_LoadAll();
extern AR_S32 IPC_CFG_PrintAll();
extern AR_S32 IPC_CFG_LoadDefaultAll(CFG_RUN_MODE_E enMode);
extern void IPC_CFG_ClearAll();

#ifdef __cplusplus
}
#endif
#endif
