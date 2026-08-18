
#ifndef SAMPLE_COMM_AUDIO_H
#define SAMPLE_COMM_AUDIO_H

#include "ar_common.h"
#include "ar_comm_aio.h"


//AR_S32 SAMPLE_STREAM_AI_SetVolume(AUDIO_SAMPLE_RATE_E enSample,AR_S32 s32Volume);


AR_S32 SAMPLE_COMM_AUDIO_StartAi(AUDIO_DEV audDevId, AR_S32 s32ChnNum, AIO_ATTR_S* pstAioAttr);

 AR_S32 SAMPLE_COMM_AUDIO_StopAi(AUDIO_DEV audioDevId, AR_S32 s32ChnNum);

AR_S32 SAMPLE_COMM_AUDIO_StartAenc(AENC_CHN audioEncChn, AIO_ATTR_S* pstAioAttr, PAYLOAD_TYPE_E enType);

AR_S32 SAMPLE_COMM_AUDIO_StopAenc(AENC_CHN audioEncChn);

AR_S32 SAMPLE_COMM_AUDIO_BindAiAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn);

AR_S32 SAMPLE_COMM_AUDIO_UnbindAiAenc(AR_S32 AiDev, AR_S32 AiChn, AR_S32 AencDev, AR_S32 AencChn);

#endif
