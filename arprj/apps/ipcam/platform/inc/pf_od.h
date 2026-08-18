#ifndef __PF_OD_H__
#define __PF_OD_H__

#include "pf_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

void IPC_PF_OD_UnInit();
AR_S32 IPC_PF_OD_Init();
AR_S32 IPC_PF_OD_SetOd(AR_BOOL bOdEn);

void IPC_PF_OD_Suspend();
void IPC_PF_OD_Resume();

#ifdef __cplusplus
}
#endif
#endif