#ifndef __PF_MD_H__
#define __PF_MD_H__

#include "pf_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

void IPC_PF_MD_UnInit(void);
AR_S32 IPC_PF_MD_Init();
AR_S32 IPC_PF_MD_SetMd(AR_BOOL bMdEn);

void IPC_PF_MD_Suspend();
void IPC_PF_MD_Resume();

#ifdef __cplusplus
}
#endif
#endif