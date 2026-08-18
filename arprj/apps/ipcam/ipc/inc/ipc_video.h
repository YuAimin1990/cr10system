#ifndef __IPC_VIDEO_H__
#define __IPC_VIDEO_H__

#ifdef __cplusplus
extern "C" {
#endif

int IPC_AV_Init();
int IPC_AV_UnInit();

AR_S32 IPC_AV_Suspend();
AR_S32 IPC_AV_Resume();


#ifdef __cplusplus
};
#endif

#endif

