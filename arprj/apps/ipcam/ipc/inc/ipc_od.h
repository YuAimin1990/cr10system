#ifndef _IPC_OD_H_
#define _IPC_OD_H_

#include "eventalarm.h"

#ifdef __cplusplus
extern "C" {
#endif


int IPC_OD_Start();
void IPC_OD_Stop();
void IPC_OD_Thread_Suspend();
void IPC_OD_Thread_Resume();


#ifdef __cplusplus
};
#endif
#endif

