#ifndef _CFG_VENDOR_H__
#define _CFG_VENDOR_H__

#include "cfg_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    AR_U8    name[MAX_STR_LEN_128];
} IPC_CFG_VENDOR_S;

AR_S32 IPC_CFG_VENDOR_Save();
AR_S32 IPC_CFG_VENDOR_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_VENDOR_LoadDefault();
void IPC_CFG_VENDOR_Print();

IPC_CFG_VENDOR_S * IPC_CFG_VENDOR_GetParam();

#define VENDOR_CFG_FILE "cfg_vendor.json"

#ifdef __cplusplus
}
#endif
#endif

