#ifndef __VCTRL_VDEC__
#define __VCTRL_VDEC__

#include "ar_comm_video.h"
#include "ar_comm_vdec.h"
#include "vctrl_list.h"

typedef struct arSAMPLE_VDEC_PICTURE_ATTR
{
    PIXEL_FORMAT_E enPixelFormat;
    AR_U32         u32Alpha;
}SAMPLE_VDEC_PICTURE_ATTR;

typedef struct arSAMPLE_VDEC_VIDEO_ATTR
{
    VIDEO_DEC_MODE_E enDecMode;
    AR_U32              u32RefFrameNum;
    DATA_BITWIDTH_E  enBitWidth;
}SAMPLE_VDEC_VIDEO_ATTR;

typedef struct arSAMPLE_VDEC_ATTR
{
    PAYLOAD_TYPE_E enType;
    VIDEO_MODE_E   enMode;
    AR_U32 u32Width;
    AR_U32 u32Height;
    AR_S32 s32ChnId;
    AR_U32 u32FrameBufCnt;
    AR_U32 u32DisplayFrameNum;
    union
    {
        SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;      /* structure with video ( h265/h264) */
        SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
    };
}SAMPLE_VDEC_ATTR;

typedef enum arTHREAD_CONTRL_E
{
    THREAD_CTRL_START,
    THREAD_CTRL_PAUSE,
    THREAD_CTRL_STOP,
}THREAD_CONTRL_E;

typedef struct arVDEC_THREAD_PARAM_S
{
    AR_BOOL transcoding;
    AR_BOOL bCircleSend;
    AR_CHAR cInFileName[128];
    AR_CHAR cOutFileName[128];
    AR_S32 s32ChnId;
    AR_S32 s32TranscodingChnId;
    AR_U32 u32PicNum;
    AR_U32 u32ChnNum;
    PAYLOAD_TYPE_E enType;
    PAYLOAD_TYPE_E enTranscodingType;
    AR_S32 s32SaveFile;
    AR_S32 s32StreamMode;
    AR_S32 s32MilliSec;
    AR_S32 s32MinBufSize;
    AR_S32 s32IntervalTime;
    THREAD_CONTRL_E eThreadCtrl;
    AR_U64  u64PtsInit;
    AR_U64  u64PtsIncrease;
    AR_U32  u32Fps;
}VDEC_THREAD_PARAM_S;

typedef struct arSAMPLE_VDEC_BUF
{
    AR_U32  u32PicBufSize;
    AR_U32  u32TmvBufSize;
    AR_BOOL bPicBufAlloc;
    AR_BOOL bTmvBufAlloc;
}SAMPLE_VDEC_BUF;

AR_S32 VCTRL_VDEC_Stop(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VDEC_Start(VCHN_INFO* pChnInfo, SAMPLE_VDEC_ATTR *pastSampleVdec);
AR_S32 VCTRL_VDEC_SERVICE_START(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VDEC_SERVICE_STOP(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VDEC_SERVICE_SET(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VDEC_SERVICE_STATUS(VCHN_INFO* pChnInfo);

#endif
