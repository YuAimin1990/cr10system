#ifndef _IPC_MD_H_
#define _IPC_MD_H_

#include "eventalarm.h"

#ifdef __cplusplus
extern "C" {
#endif


int IPC_MD_Start();
void IPC_MD_Stop();
void IPC_MD_Thread_Suspend();
void IPC_MD_Thread_Resume();


#ifdef __cplusplus
};
#endif
#endif

