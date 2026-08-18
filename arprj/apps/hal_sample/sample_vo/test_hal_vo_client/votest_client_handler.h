#ifndef _IPCTOOL_CALLBACK_HANDLER_H_
#define _IPCTOOL_CALLBACK_HANDLER_H_
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
//#include "binder_ipc_common.h"
#include "binder_ipc.h"
#include "mpi_type.h"
#include "hal_vo.h"
#include "test_vo_common.h"

#define MAX_CALLBACK_NUM            32
#define BINDER_VOTEST_NAME         "votest_ctrl"

typedef struct arVOTEST_CMD_ARGS_S
{
	 AR_S32    s32ArgsNum;
	 AR_CHAR   s8Args[64][128];
} VOTEST_CMD_ARGS_S;

typedef struct arVOTEST_SERVICE_S
{
	AR_CHAR *s8ServiceName;
	AR_CHAR *s8Help;
	AR_S32 (*ServiceHandler)(VOTEST_CMD_ARGS_S * pstCmdArgs);
} VOTEST_SERVICE_S;

typedef struct arBINDER_RET_S
{
	AR_CHAR s8Name[14];
	AR_U32 u32Ret;
} BINDER_RET_S;

VOTEST_CMD_ARGS_S * AR_VOTEST_GetArgs(const AR_CHAR *ps8Cmd);
AR_S32 AR_VOTEST_BinderInit(void);
AR_S32 AR_VOTEST_ClientPauseChannel(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientResumeChannel(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientShowChannel(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientHideChannel(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientStepChannel(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientDumpLayerFrame(VOTEST_CMD_ARGS_S * pstCmdArgs);

AR_S32 AR_VOTEST_ClientEnableDev(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientDisableDev(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientEnableLayer(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientDisableLayer(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientEnableChn(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientDisableChn(VOTEST_CMD_ARGS_S * pstCmdArgs);

AR_S32 AR_VOTEST_ClientSetDevAttr(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientSetLayerAttr(VOTEST_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_VOTEST_ClientSetChnAttr(VOTEST_CMD_ARGS_S * pstCmdArgs);

#endif

