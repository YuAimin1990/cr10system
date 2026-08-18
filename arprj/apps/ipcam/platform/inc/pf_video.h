#ifndef __PF_VIDEO_H__
#define __PF_VIDEO_H__

#include "cfg_all.h"

#include "pf_type.h"
#include "ar_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    MOD_ID_SYS     = 1,
    MOD_ID_VB      = 2,
    MOD_ID_VI      = 3,
    MOD_ID_VO      = 4,
    MOD_ID_VPSS    = 5,
    MOD_ID_RGN     = 6,
    MOD_ID_VENC    = 7,
    MOD_ID_VDEC    = 8,
    MOD_ID_VGS     = 9,
    MOD_ID_AI      = 10,
    MOD_ID_AO      = 11,
    MOD_ID_AENC    = 12,
    MOD_ID_ADEC    = 13,
    MOD_ID_BUTT,
} IPC_MOD_ID_E;

/*************** BIND structures ************/
typedef struct
{
    IPC_MOD_ID_E enModId;
    AR_U32 u32DevId;
    AR_U32 u32ChnId;
} IPC_MPP_CHN_S;

/*************** VI 3A structures ************/
typedef struct
{
    AR_S32 s32ChannelId;
    //aec_info, STRU_AEC_SET_INFO_T
    AR_S32   s32AecMode;
    AR_FLOAT f32AecGain;
    AR_FLOAT f32AecExpTimeUs;
    AR_U32 u32AecLumaTarget;
    AR_FLOAT f32AecExpTimeUsShort;
    AR_U32 u32ExpIndex;
    AR_U32 u32LuxIndex;
    //awb_info STRU_AWB_SET_INFO_T
    AR_S32 s32AwbMode;
    AR_U32 u32AwbCct;
    AR_FLOAT f32AwbRGain;
    AR_FLOAT f32AwbGGain;
    AR_FLOAT f32AwbBGain;
    //af_info STRU_AF_GET_BASIC_INFO_T
    AR_U32 u32AfLenPosition;
    AR_U32 u32AfFocusValue;
}IPC_VI_VIDEO_3A_PARAM_S;

/*************** VI structures ************/
typedef struct
{
    AR_U32 u32CodecType;
    AR_U32 width;
    AR_U32 height;

    AR_U32 rc_mode; /*0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR*/
    AR_U32 gop;
    AR_S32 s32IPQpDelta;
    AR_U32 fps;
    AR_U32 Kbps;
    AR_U32 min_Kbps;
    AR_U32 max_Kbps;
    AR_U32 u32StatTime;

} IPC_VENC_VIDEO_PARAM_S;

typedef struct
{
    AR_U32	u32MaxPQp;	   /* RW; Range:[0, 51]; the max P B qp */
    AR_U32	u32MinPQp;	   /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    AR_U32	u32MaxIQp;	  /* RW; Range:[0, 51]; the max I qp */
    AR_U32	u32MinIQp;	  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */
} IPC_VENC_RC_PARAM_S;
typedef struct IPC_VENC_STREAM
{
    /* data */
    AR_U32 u32CodecType; //H264,265,mjpeg
    AR_U32 u32FrameType; //I/P/B frame
    AR_U64 u64Pts;
    AR_U64 u64FrameNo;
    AR_U64 u64AddrVirt; //stream buff addr
    AR_U64 u64Len;
    AR_U64 u64Offset;
    void * private_buffer; //platform related buffer ptr, may be used when release it, so save here.
} IPC_VENC_STREAM_S; //tmp define by hbbai

typedef struct IPC_VB_POOL_CNT
{
    AR_U32 u32PoolD1;
    AR_U32 u32Pool720P;
	AR_U32 u32Pool1080P;
	AR_U32 u32Pool5M;
	AR_U32 u32Pool4K;
} IPC_VB_POOL_CNT_S;

typedef enum {
    AR_VIDEO_CODEC_PARAM_HAL_EXT_INSTANCE_PARALLESIM = 1, /**< 编码实例之间的并行编码开关, 26x有效，jpeg不支持*/
} ENUM_HAL_VENC_EXTENSION_FLAGS;


AR_S32 IPC_PF_SYS_Init();
AR_S32 IPC_PF_SYS_UnInit();

AR_S32 IPC_PF_VI_Init();
AR_S32 IPC_PF_VI_UnInit();
AR_S32 IPC_PF_VI_SetRes(VI_PIPE ViPipe, AR_U32 u32W, AR_U32 u32H);
AR_S32 IPC_PF_VI_SetFps(VI_PIPE ViPipe, AR_U32 u32Fps);
AR_S32 IPC_PF_VI_SetFlip(VI_PIPE ViPipe, AR_BOOL bFlip);
AR_S32 IPC_PF_VI_SetMirror(VI_PIPE ViPipe, AR_BOOL bMirror);
AR_S32 IPC_PF_VI_SetHDR(VI_PIPE ViPipe, AR_U32 u32OnOff);
AR_S32 IPC_PF_VI_SetHue(VI_PIPE ViPipe, AR_U32 u32Hue);
AR_S32 IPC_PF_VI_GetHue(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_SetBrightness(VI_PIPE ViPipe, AR_U32 u32Brightness);
AR_S32 IPC_PF_VI_GetBrightness(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_SetSatueration(VI_PIPE ViPipe, AR_U32 u32Satueration);
AR_S32 IPC_PF_VI_GetSatueration(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_SetContrast(VI_PIPE ViPipe, AR_U32 u32Contrast);
AR_S32 IPC_PF_VI_GetContrast(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_SetSharpness(VI_PIPE ViPipe, AR_U32 u32Sharpness);
AR_S32 IPC_PF_VI_GetSharpness(VI_PIPE ViPipe);

AR_S32 IPC_PF_VI_SetDenoise3D(VI_PIPE ViPipe, AR_U32 u32Strength);
AR_S32 IPC_PF_VI_SetDenoise2D(VI_PIPE ViPipe, AR_U32 u32Strength);
AR_S32 IPC_PF_VI_GetDenoise3D(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_GetDenoise2D(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_GetExpLimit(VI_PIPE ViPipe);
AR_S32 IPC_PF_VI_SetExpLimit(VI_PIPE ViPipe, AR_U32 u32ExpLimitUs);

AR_S32 IPC_PF_VI_SetAntiFlicker(VI_PIPE ViPipe, AR_U32 u32Freq);
AR_S32 IPC_PF_VI_SetImgScene(VI_PIPE ViPipe, AR_U32 u32Mode, AR_CHAR * strTunningName);
AR_S32 IPC_PF_VI_Get3aInfoTidyAttr(VI_PIPE ViPipe, IPC_VI_VIDEO_3A_PARAM_S * pst3aInfo);

AR_S32 IPC_PF_VPSS_Init();
AR_S32 IPC_PF_VPSS_UnInit();
AR_S32 IPC_PF_VPSS_SetRotate(AR_U32 u32GrpId, VPSS_ROTATE_S * pstRotate);
AR_S32 IPC_PF_VPSS_SetFlip(AR_U32 u32GrpId, AR_S32 s32ChnId, AR_U32 u32FlipEnable);
AR_S32 IPC_PF_VPSS_SetMirror(AR_U32 u32GrpId, AR_S32 s32ChnId, AR_U32 u32MirrorEnable);
AR_S32 IPC_PF_VPSS_SetEis(AR_U32 u32GrpId, AR_U32 u32Enable);
AR_S32 IPC_PF_VPSS_SetLdc(AR_U32 u32GrpId, AR_U32 u32Enable);
AR_S32 IPC_PF_VPSS_SetMosaic(AR_U32 u32GrpId, VPSS_MOSAIC_S * pstMosaic, AR_U32 u32Num);
AR_S32 IPC_PF_VPSS_SetCover(AR_U32 u32GrpId, VPSS_MOSAIC_S * pstCover, AR_U32 u32Num);
AR_S32 IPC_PF_VPSS_SetCrop(AR_U32 u32GrpId, AR_U32 u32ChnId, VPSS_CROP_S * pstCrop);
AR_S32 IPC_PF_VPSS_SetRes(AR_U32 u32GrpId, AR_U32 u32ChnId, AR_U32 u32Width, AR_U32 u32Height);
AR_S32 IPC_PF_VPSS_SetYUV2RGB(AR_U32 u32GrpId, AR_U32 u32ChnId, AR_U32 u32Enable);

AR_S32 IPC_PF_VO_Init();
AR_S32 IPC_PF_VO_FullScreen_Process(AR_U32 u32VoChnId, AR_U32 u32Enable);

AR_S32 IPC_PF_VO_UnInit();

AR_S32 IPC_PF_VENC_UnInit();
AR_S32 IPC_PF_VENC_Init();
AR_S32 IPC_PF_VENC_GetStream(AR_S32 s32DevFd, AR_U32 ChannelID, IPC_VENC_STREAM_S *pstVideoStream);
AR_S32 IPC_PF_VENC_ReleaseStream(AR_U32 ChannelID, IPC_VENC_STREAM_S *pstVideoStream);
AR_S32 IPC_PF_VENC_RequestIDR(VENC_CHN VeChn);
AR_S32 IPC_PF_VENC_Color2Gray(VENC_CHN VeChn, AR_U32 bColor2Grey);
AR_S32 IPC_PF_VENC_GetVideoParam(VENC_CHN VeChn, IPC_VENC_VIDEO_PARAM_S *p);
AR_S32 IPC_PF_VENC_SetVideoParam(VENC_CHN VeChn, IPC_VENC_VIDEO_PARAM_S *p);
AR_S32 IPC_PF_VENC_SetRcParam(VENC_CHN VeChn, IPC_VENC_RC_PARAM_S * pRc);
AR_S32 IPC_PF_VENC_GetRcParam(VENC_CHN VeChn, IPC_VENC_RC_PARAM_S *pRc);
AR_S32 IPC_PF_VENC_IsInit();
AR_S32 IPC_PF_VENC_CheckAlive();

/* quality:[1,99] */
AR_S32 IPC_PF_VENC_SetJpegQuality(AR_U32 ch, AR_U32 quality);
AR_S32 IPC_PF_VENC_SnapProcess(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 SnapCnt, AR_CHAR * pSaveDir);
AR_S32 IPC_PF_VENC_StartMbufThread(void);
AR_S32 IPC_PF_VENC_StopMbufThread(void);
AR_S32 IPC_PF_VENC_UpdateParam();
AR_S32 IPC_PF_VENC_UpdateChnParam(AR_U32 ch);
AR_S32 IPC_PF_VENC_SetRes(AR_U32 pipeNum, AR_U32 u32ChnId, AR_U32 u32Width, AR_U32 u32Height);
AR_S32 IPC_PF_Stream_SetRes(AR_U32 u32VpssGrpId, AR_U32 u32VpssChnId, AR_U32 u32Width, AR_U32 u32Height);

AR_S32 IPC_PF_MbufStart();
MOD_ID_E IPC_PF_MOD_TRANS(IPC_MOD_ID_E enMod);

AR_S32 IPC_PF_Create_Channel();
AR_S32 IPC_PF_Destroy_Channel();

AR_S32 IPC_PF_CONTROL_Start();
AR_S32 IPC_PF_CONTROL_Stop();
AR_S32 IPC_PF_VI_StartCameraDetect();
void IPC_PF_VI_StopCameraDetect();

void IPC_PF_VENC_MbufThread_Suspend();
void IPC_PF_VENC_MbufThread_Resume();
void IPC_PF_VI_CameraDectect_Suspend();
void IPC_PF_VI_CameraDectect_Resume();

AR_S32 IPC_PF_VI_Suspend();
AR_S32 IPC_PF_VI_Resume();
AR_S32 IPC_PF_VO_Suspend();
AR_S32 IPC_PF_VO_Resume();

AR_S32 IPC_PF_VPSS_Suspend();
AR_S32 IPC_PF_VPSS_Resume();
AR_S32 IPC_PF_VENC_Suspend();
AR_S32 IPC_PF_VENC_Resume();

void IPC_PF_SubMoudle_Suspend();
void IPC_PF_SubMoudle_Resume();

#ifdef __cplusplus
}
#endif
#endif

