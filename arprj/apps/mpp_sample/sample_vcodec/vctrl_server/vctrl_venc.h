#ifndef __VCTRL_VENC__
#define __VCTRL_VENC__

#include "ar_comm_video.h"
#include "ar_comm_venc.h"
#include "vctrl_common.h"
#include "vctrl_list.h"

#define FILE_NAME_LEN 128
#define YUV_FRAME_CACHED_NUM 16
#define MAX_PRELOAD_PIC_NUM 5

typedef struct arVCTRL_VENC_GETSTREAM_PARA_S
{
    AR_BOOL bThreadStart[VENC_MAX_CHN_NUM];
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    AR_S32  s32Cnt;
} VCTRL_VENC_GETSTREAM_PARA_S;

typedef struct arVCTRL_VENC_SENDSTREAM_PARA_S
{
    AR_BOOL bThreadStart[VENC_MAX_CHN_NUM];
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    AR_S32  s32Cnt;
} VCTRL_VENC_SENDSTREAM_PARA_S;

typedef struct arVCTRL_VENC_QPMAP_SENDFRAME_PARA_S
{
    AR_BOOL  bThreadStart;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    AR_S32   s32Cnt;
    SIZE_S   stSize;
} VCTRL_VENC_QPMAP_SENDFRAME_PARA_S;

AR_S32 VCTRL_VENC_StopSendQpmapFrame(void);
AR_S32 VCTRL_VENC_StopGetStream(VENC_CHN VeChn);
AR_S32 VCTRL_VENC_Stop(VENC_CHN VencChn);
AR_S32 VCTRL_VENC_StartGetStream(VCHN_INFO* pChnInfo, AR_S32 s32ChnNum);
AR_S32 VCTRL_VENC_StartGetStream_Svc_t(AR_S32 s32Cnt);
AR_S32 VCTRL_VENC_Start(VCHN_INFO* pChnInfo, VENC_GOP_ATTR_S *pstGopAttr);
AR_S32 VCTRL_VENC_SERVICE_START(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VENC_SERVICE_STOP(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VENC_SERVICE_SET(VCHN_INFO* pChnInfo);
AR_S32 VCTRL_VENC_SERVICE_STATUS();


#endif//__VCTRL_VENC__
