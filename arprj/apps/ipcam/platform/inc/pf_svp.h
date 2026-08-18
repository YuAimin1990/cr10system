#ifndef __PF_SVP_H__
#define __PF_SVP_H__

#include "util_common.h"
#include "util_api.h"
#include "cfg_all.h"
#include "hal_npu_types.h"
#include "mpi_npu_api.h"
#include "ar_comm_video.h"

#ifdef __cplusplus
extern "C"
{
#endif



AR_S32 IPC_PF_SVP_Init();
AR_S32 IPC_PF_SVP_UnInit();

void IPC_PF_SVP_Suspend();
void IPC_PF_SVP_Resume();

#ifdef __cplusplus
}
#endif
#endif
