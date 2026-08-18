#ifndef __PF_AUDIO_H__
#define __PF_AUDIO_H__

#include "pf_type.h"
#include "mpi_audio.h"
#include "mpi_vb.h"
#include "hal_acodec.h"
#include "acodec.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    AR_U32 u32Id;
    AR_U32 u32Stop;
} IPC_AUDIO_THREAD_STATUS;

typedef struct
{
    AUDIO_SOUND_MODE_E  enSoundmode;
    AR_U32 u32PerFrameLen;
    AR_U32 u32PerFrameBuffLen;
    AR_U8 * pu8AudioData[2];
} IPC_AUDIO_FRAME_S;

typedef struct
{
} IPC_AEC_FRAME_S;

AR_S32 IPC_PF_Audio_Init();
AR_S32 IPC_PF_Audio_UnInit();
void IPC_PF_AENC_StartMbufThread(void);
void IPC_PF_AENC_StopMbufThread(void);
void IPC_PF_AI_StartMbufThread(void);
void IPC_PF_AI_StopMbufThread(void);
AR_S32 IPC_PF_AI_SetVolume(AR_S32 s32AiInputVol);
AR_S32 IPC_PF_AO_SetVolume(AR_S32 s32AoOutputVol);

AR_S32 IPC_PF_Audio_RecordStart(AR_CHAR * pcPath, AR_U32 u32StreamId);
AR_S32 IPC_PF_Audio_RecordStop();
AR_S32 IPC_PF_Audio_RecordPlayStart(AR_CHAR * pcPath);
AR_S32 IPC_PF_Audio_RecordPlayStop();

AR_S32 IPC_PF_Audio_Suspend();
AR_S32 IPC_PF_Audio_Resume();
void IPC_PF_AENC_MbufThread_Suspend();
void IPC_PF_AENC_MbufThread_Resume();
void IPC_PF_AI_MbufThread_Suspend();
void IPC_PF_AI_MbufThread_Resume();

#ifdef __cplusplus
}
#endif
#endif