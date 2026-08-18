#ifndef SAMPLE_COMM_MUXER_H
#define SAMPLE_COMM_MUXER_H

#include "ar_common.h"
#include "ar_comm_aio.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/imgutils.h"
#include "libavutil/avassert.h"
#include "libavutil/error.h"

#ifdef __cplusplus
}
#endif

#include <pthread.h>

#define MUXER_MAX_AVSTREAM 8

typedef struct MUXER_CONTEXT_S MUXER_CONTEXT_T;

typedef struct MUXER_AVSTREAM_S{
    AR_S32 s32Valid;
    AR_S32 s32EncChn;
    enum AVMediaType eMediaType;
    AR_U64 u64Pts;
    AVStream* stStream;
    MUXER_CONTEXT_T* stContext;
} MUXER_AVSTREAM_T;


struct MUXER_CONTEXT_S {
    AR_BOOL           boolRunFlag;
    AENC_CHN        audioEncChn;
    VENC_CHN        videoEncChn;
    AR_CHAR         szFullName[128];
    AVFormatContext* stMuxerFormat;

    pthread_mutex_t muxerMutex;
    pthread_t muxerThreadIds[MUXER_MAX_AVSTREAM];
    AR_S32 s32AvStreamCount;
    MUXER_AVSTREAM_T stAvStreams[MUXER_MAX_AVSTREAM];
};

AR_S32 SAMPLE_COMM_MUXER_InitMuxer(MUXER_CONTEXT_T* context, const AR_CHAR* szFullName);

AR_S32 SAMPLE_COMM_MUXER_AddVideoStream(MUXER_CONTEXT_T* context, AR_S32 s32EncChn, PAYLOAD_TYPE_E enPayload, AR_S32 s32VideoWidth, AR_S32 s32VideoHeight, AR_S32 s32VideoBitrate, AR_U8* u8ExtraData, AR_S32 s32ExtraSize);

AR_S32 SAMPLE_COMM_MUXER_AddAudioStream(MUXER_CONTEXT_T* context, AR_S32 s32EncChn, PAYLOAD_TYPE_E enPayload, AUDIO_SAMPLE_RATE_E enSampleRate, AUDIO_SOUND_MODE_E enSoundMode, AUDIO_BIT_WIDTH_E enBitWidth, AR_U8* u8ExtraData, AR_S32 s32ExtraSize);

AR_S32 SAMPLE_COMM_MUXER_StartPack(MUXER_CONTEXT_T* context);

AR_S32 SAMPLE_COMM_MUXER_StopPack(MUXER_CONTEXT_T* context);

#endif
