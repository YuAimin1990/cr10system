#ifndef __CFG_CHANNEL_H__
#define __CFG_CHANNEL_H__
#include "pf_type.h"
#include "cfg_common.h"

//For the ids, -1, 0xffffffff, means invalid or not bind any channels.
typedef struct
{
    AR_U32 u32Enable;
    AR_CHAR chType[32];
} IPC_CHANNEL_S;

typedef struct cfg_channel
{
    IPC_CHANNEL_S stChannel[IPC_MAX_PIPELINE_NUM];
} IPC_CFG_CHANNEL_S;

AR_S32 IPC_CFG_CHANNEL_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_CHANNEL_LoadDefault(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_CHANNEL_Save();

AR_S32 IPC_CFG_CHANNEL_Print();
IPC_CFG_CHANNEL_S * IPC_CFG_CHANNEL_GetParam();

#define CHANNEL_CFG_FILE "cfg_channel.json"
#endif
