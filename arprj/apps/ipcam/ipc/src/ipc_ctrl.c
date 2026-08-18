#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "hal_gpio.h"
#include "pf_type.h"
#include "pf_video.h"
#include "pf_audio.h"
#include "pf_osd.h"
#include "pf_md.h"
#include "pf_od.h"
#include "ipc_video.h"
#include "util_rpc.h"
#include "util_api.h"

#include "pf_drv.h"
#include "recmng_rec.h"

//The definition in this file must keep same with ipctool_client_handler.h
static AR_S32 g_s32RunningStatus = 0;
#define IPC_RPC_CMD_BASE            0x76540000
typedef enum
{
    IPC_RPC_CMD_SET_FLIP = IPC_RPC_CMD_BASE,
    IPC_RPC_CMD_SET_MIRROR,               /*1*/
    IPC_RPC_CMD_SET_VI_FPS,               /*2*/
    IPC_RPC_CMD_SET_VI_RES,               /*3*/
    IPC_RPC_CMD_SET_VI_ANTIFLICKER,       /*4*/
    IPC_RPC_CMD_SET_VI_DENOISE_3D,        /*5*/
    IPC_RPC_CMD_SET_VI_WDR,               /*6*/
    IPC_RPC_CMD_SET_VENC_BRC,             /*7*/
    IPC_RPC_CMD_SET_VENC_GOP,             /*8*/
    IPC_RPC_CMD_SET_VENC_QP,              /*9*/
    IPC_RPC_CMD_SET_VENC_TYPE,            /*10*/
    IPC_RPC_CMD_SET_VENC_FPS,             /*11*/
    IPC_RPC_CMD_SET_VRECORD,              /*12*/
    IPC_RPC_CMD_SET_ARECORD,              /*13*/
    IPC_RPC_CMD_SET_PLAYBACK,             /*14*/
    IPC_RPC_CMD_SNAPSHOT,                 /*15*/
    IPC_RPC_CMD_SET_IMG_SCENE,            /*16*/
    IPC_RPC_CMD_SET_IMG_STYLE,            /*17*/
    IPC_RPC_CMD_SET_IMG_BRIGHT,           /*18*/
    IPC_RPC_CMD_SET_IMG_CONTRAST,         /*19*/
    IPC_RPC_CMD_SET_IMG_SATURATION,       /*20*/
    IPC_RPC_CMD_SET_IMG_SHARPNESS,        /*21*/
    IPC_RPC_CMD_SET_IMG_HUE,              /*22*/
    IPC_RPC_CMD_GET_IMG_BRIGHT,           /*23*/
    IPC_RPC_CMD_GET_IMG_CONTRAST,         /*24*/
    IPC_RPC_CMD_GET_IMG_SATURATION,       /*25*/
    IPC_RPC_CMD_GET_IMG_SHARPNESS,        /*26*/
    IPC_RPC_CMD_GET_IMG_HUE,              /*27*/
    IPC_RPC_CMD_SET_WDT,                  /*28*/
    IPC_RPC_CMD_SET_GPIO,                 /*29*/
    IPC_RPC_CMD_GET_GPIO,                 /*30*/
    IPC_RPC_CMD_SET_SVP,                  /*31*/
    IPC_RPC_CMD_SET_WIFI_MODE,            /*32*/
    IPC_RPC_CMD_REBOOT,                   /*33*/
    IPC_RPC_CMD_POWEROFF,                 /*34*/
    IPC_RPC_CMD_DUMP_YUV,                 /*35*/
    IPC_RPC_CMD_GET_VENC_PARAM,           /*36*/
    IPC_RPC_CMD_DRAW_VPSS_OSD,            /*37*/
    IPC_RPC_CMD_SET_STREAM_OSD,            /*38*/
    IPC_RPC_CMD_GET_VI_DENOISE_3D,         /*39*/
    IPC_RPC_CMD_GET_VI_DENOISE_2D,         /*40*/
    IPC_RPC_CMD_SET_VI_DENOISE_2D,         /*41*/
    IPC_RPC_CMD_SET_VI_EXPLIMIT,           /*42*/
    IPC_RPC_CMD_GET_VI_EXPLIMIT,           /*43*/
    IPC_RPC_CMD_GET_VI_3AINFO,             /*44*/
    IPC_RPC_CMD_START,                     /*45*/
    IPC_RPC_CMD_STOP,                      /*46*/
    IPC_RPC_CMD_CREATE,                    /*47*/
    IPC_RPC_CMD_DESTORY,                   /*48*/
    IPC_RPC_CMD_RTCMD,                     /*49*/
    IPC_RPC_CMD_SET_MD,                    /*50*/
    IPC_RPC_CMD_SET_OD,                    /*51*/
    IPC_RPC_CMD_DRAW_VPSS_GEOM_OSD,        /*52*/
    IPC_RPC_CMD_SET_VOFULLSCREEN,          /*53*/
    IPC_RPC_CMD_SET_VPSS_FLIP,             /*54*/
    IPC_RPC_CMD_SET_VPSS_MIRROR,           /*55*/
    IPC_RPC_CMD_END
} IPC_RPC_CMD_E;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Fps;
} IPC_CTRL_VI_FPS_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32W;
    AR_S32 s32H;
} IPC_CTRL_VI_RES_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Freqency; //50, 60 Hz
} IPC_CTRL_VI_ANTI_FLICKER_S;

typedef struct
{
	AR_S32 s32ChannelId;
    AR_S32 s32Strength;
} IPC_CTRL_VI_DENOISE_S;

typedef struct
{
	AR_S32 s32ChannelId;
    AR_S32 s32ExpLimitUs;
} IPC_CTRL_VI_EXPLIMIT_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Enable;
} IPC_CTRL_VI_WDR_S;

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
    AR_U32 u32AfTotalSteps;
    AR_U32 u32AfFocusValue;
}IPC_CTRL_VI_3A_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
	AR_S32 s32Mode;
    AR_S32 s32Min;
    AR_S32 s32Max;
    AR_S32 s32Average;
    AR_S32 s32StatTime;
} IPC_CTRL_VENC_BRC_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
	AR_S32 s32Value;
} IPC_CTRL_VENC_GOP_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
	AR_S32 s32Type;
} IPC_CTRL_VENC_TYPE_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
	AR_S32 s32Fps;
} IPC_CTRL_VENC_FPS_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
    AR_S32 s32MinIQp;
    AR_S32 s32MinPQp;
    AR_S32 s32MaxIQp;
    AR_S32 s32MaxPQp;
} IPC_CTRL_VENC_QP_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Stream;
	AR_S32 s32Format;
    AR_S32 s32Mode;
    AR_S32 s32StartOrStop;
} IPC_CTRL_VRECORD_S;

typedef struct
{
    AR_CHAR cPathName[32];
	AR_U32 u32StreamId;
    AR_S32 s32StartOrStop;
} IPC_CTRL_ARECORD_S;

typedef struct
{
    AR_S32 s32AudioOrVedio; //0:audio 1:vedio
    AR_S32 s32StartOrStop;
    AR_CHAR achFileName[128];
} IPC_CTRL_PLAYBACK_S;

#if 0
typedef struct
{
	AR_S32 s32AudioSampleRate;//audio sample rate(8000 44100 48000)
	AR_S32 s32AudioVolume;//audio vol(0-255)
	AR_S32 s32PcmChannel;//only for pcm s32Mode
	AR_CHAR * ps8AudioDec;//yes/no, if record pcm,add -aenc no
} IPC_CTRL_PLAYBACK_AUDIO_S;
#endif

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Enable;
} IPC_CTRL_FLIP_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Enable;
} IPC_CTRL_MIRROR_S;

typedef struct
{
    AR_S32 s32VpssGrp;
	AR_S32 s32VpssChn;
	AR_S32 s32Cnt;
    AR_CHAR strPath[128];
} IPC_CTRL_SNAPSHOT_S;

typedef struct
{
	AR_S32 s32VoChn;
	AR_S32 s32Enable;
} IPC_CTRL_VOFULLSCREEN_S;


typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Mode;
    AR_CHAR strTunningFile[128];
} IPC_CTRL_IMG_SCENE_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32Style;
} IPC_CTRL_IMG_STYLE_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32BrightValue;
} IPC_CTRL_IMG_BRIGHT_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32SaturationValue;
} IPC_CTRL_IMG_SATURATION_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32ContrastValue;
} IPC_CTRL_IMG_CONTRAST_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32SharpnessValue;
} IPC_CTRL_IMG_SHARPNESS_S;

typedef struct
{
	AR_S32 s32ChannelId;
	AR_S32 s32HueValue;
} IPC_CTRL_IMG_HUE_S;

typedef struct
{
	AR_S32 s32OnOff;
} IPC_CTRL_WDT_S;

typedef struct
{
	AR_CHAR strName[8]; //like: "C1_0"
	AR_S32 s32Direction;
	AR_U32 u32Value;
} IPC_CTRL_GPIO_S;

typedef struct
{
	AR_S32 s32OnOff;
} IPC_CTRL_SVP_S;

typedef struct
{
	AR_S32 s32RebootFlag;
} IPC_CTRL_REBOOT_S;

typedef struct
{
	AR_S32 s32PoweroffFlag;
} IPC_CTRL_POWEROFF_S;

typedef struct
{
	AR_S32 s32WifiMode;
} IPC_CTRL_WIFIMODE_S;

typedef struct
{
    AR_S32 s32ChannelId;
} IPC_CTRL_DUMP_YUV_S;

typedef struct
{
    AR_U32 u32CodecType;
    AR_U32 u32Width;
    AR_U32 u32Height;

    AR_U32 u32RcMode; /*0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR*/
    AR_U32 u32Gop;
    AR_U32 u32FrameRate; //fps
    AR_U32 u32CbrKbps; //cbr bitrate
    AR_U32 u32MinKbps;   //vbr
    AR_U32 u32MaxKbps;   //vbr
    AR_U32 u32StatTime; 
    AR_U32 u32MaxPQp;	  /* RW; Range:[0, 51]; the max P B qp */
    AR_U32 u32MinPQp;	  /* RW; Range:[0, 51]; the min P B qp,can not be larger than u32MaxQp */
    AR_U32 u32MaxIQp;	  /* RW; Range:[0, 51]; the max I qp */
    AR_U32 u32MinIQp;	  /* RW; Range:[0, 51]; the min I qp,can not be larger than u32MaxIQp */

} IPC_CTRL_VENC_PARAM_S;

typedef struct
{
    AR_S32 s32ChannelId;
    AR_S32 s32StreamId;
    IPC_CTRL_VENC_PARAM_S stParam;
} IPC_CTRL_VENC_CH_S;

typedef struct
{
    AR_U32 u32VpssGrp; //will draw osd on all streams if 'alg_osd_enable' is set on this stream(cfg_osd.json)
    AR_U32 u32StrNum;  //max 32
    IPC_OSD_STRING_S stOsdStr[IPC_ALG_OSD_MAX_NUM];
    AR_U32 u32RectNum; //max 32
    IPC_OSD_RECT_S stOsdRect[IPC_ALG_OSD_MAX_NUM];
    AR_U32 u32ImgWidth; //the reference img w, used to calculate real location on video channel.
    AR_U32 u32ImgHeight;
} IPC_CTRL_OSD_S;

typedef struct
{
    AR_U32 u32VpssGrp; //will draw osd on all streams if 'alg_osd_enable' is set on this stream(cfg_osd.json)
    AR_U32 u32RectNum; //max 32
    IPC_OSD_GEOM_S stOsdRect[IPC_ALG_OSD_MAX_NUM];
    AR_U32 u32ImgWidth; //the reference img w, used to calculate real location on video channel.
    AR_U32 u32ImgHeight;
} IPC_CTRL_GEOM_OSD_S;

typedef struct
{
	AR_S32 s32GroupId;
	AR_S32 s32ChannelId;
	AR_S32 s32Enable;
} IPC_CTRL_VPSS_FLIP_S;

typedef struct
{
	AR_S32 s32GroupId;
	AR_S32 s32ChannelId;
	AR_S32 s32Enable;
} IPC_CTRL_VPSS_MIRROR_S;


//To Ctrl the channel name/time osd
typedef struct
{
    AR_U32 u32ChannelId;
    AR_U32 u32StreamId;
    AR_U32 u32ChnNameEnable;
    AR_CHAR strChnName[32];
    AR_U32 u32DateTimeEnable;
	AR_U32 u32TimeFormat;
    AR_U32 u32DateFormat;
    AR_U32 u32DisplayWeek;
	AR_U32 u32RGB; //4Bytes: 0 R G B
    AR_U32 u32FontSize; //to do
} IPC_CTRL_STREAM_OSD_S;

typedef struct
{
    AR_S32 s32VpssGrp;
    AR_S32 s32VpssChn;
    AR_S32 s32Width;
    AR_S32 s32Height;
} IPC_CTRL_STREAM_RES_S;

typedef struct
{
	AR_S32 s32Enable;
} IPC_CTRL_MD_S;

typedef struct
{
	AR_S32 s32Enable;
} IPC_CTRL_OD_S;

typedef struct arIPCTOOL_CMD_ARGS_S
{
	 AR_S32 s32ArgsNum;
	 AR_CHAR s8Args[64][128];
} IPC_CTRL_CMD_ARGS_S;

typedef struct arIPCTOOL_CMD_STRING_S
{
	 AR_CHAR s8Cmd_String[128];
} IPC_CTRL_CMD_STRING_S;

static IPC_CTRL_CMD_ARGS_S * AR_IPCTOOL_GetArgs(const AR_CHAR *ps8Cmd)
{
    IPC_CTRL_CMD_ARGS_S * pstArgs = NULL;
    AR_CHAR *ps8CmdTemp=NULL;
    AR_CHAR *ps8Str = NULL;
    AR_CHAR *ps8SplitChar = " ";

    pstArgs = (IPC_CTRL_CMD_ARGS_S *)malloc(sizeof(IPC_CTRL_CMD_ARGS_S));
    ps8CmdTemp = (AR_CHAR *)malloc(1024);
    if((!pstArgs) || (!ps8CmdTemp)){
        printf("malloc failed \r\n");
        return NULL;
    }
    memset(pstArgs, 0, sizeof(IPC_CTRL_CMD_ARGS_S));
    strcpy(ps8CmdTemp, ps8Cmd);
    ps8Str = strtok(ps8CmdTemp, ps8SplitChar);
    if(ps8Str)
    {
        strcpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0], ps8Str);
        pstArgs->s32ArgsNum++;
    }
    while(ps8Str)
    {
        ps8Str=strtok(NULL, ps8SplitChar);
        if(ps8Str)
        {
            strcpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0],ps8Str);
            pstArgs->s32ArgsNum++;
        }
    }
    free(ps8CmdTemp);
    return pstArgs;
}

static AR_S32 AR_IPCTOOL_IndexFindByName(AR_CHAR * ps8Name, IPC_CTRL_CMD_ARGS_S *pstArgv)
{
    AR_S32 s32Index=-1;
    for(int i=0; i < pstArgv->s32ArgsNum; i++)
    {
        if(!strcmp(ps8Name, pstArgv->s8Args[i]))
        {
            s32Index = i;
            break;
        }
    }
    return s32Index;
}

static AR_S32 IPC_CTRL_SetFlip(void * args, void * reply)
{
    IPC_CTRL_FLIP_S * pstFlip = (IPC_CTRL_FLIP_S *)args;

    printf("%s: id %d enable %d\n", __FUNCTION__, pstFlip->s32ChannelId, pstFlip->s32Enable);
    return IPC_PF_VI_SetFlip(pstFlip->s32ChannelId, pstFlip->s32Enable);
}
static AR_S32 IPC_CTRL_SetMirror(void * args, void * reply)
{
    IPC_CTRL_MIRROR_S * pstMirror = (IPC_CTRL_MIRROR_S *)args;

    printf("%s: id %d enable %d\n", __FUNCTION__, pstMirror->s32ChannelId, pstMirror->s32Enable);
    return IPC_PF_VI_SetMirror(pstMirror->s32ChannelId, pstMirror->s32Enable);
}
static AR_S32 IPC_CTRL_SetViFps(void * args, void * reply)
{
    IPC_CTRL_VI_FPS_S * pstFps = (IPC_CTRL_VI_FPS_S *)args;

    printf("%s: id %d fps %d\n", __FUNCTION__, pstFps->s32ChannelId, pstFps->s32Fps);
    return IPC_PF_VI_SetFps(pstFps->s32ChannelId, pstFps->s32Fps);
}
static AR_S32 IPC_CTRL_SetViRes(void * args, void * reply)
{
    IPC_CTRL_VI_RES_S * pstRes = (IPC_CTRL_VI_RES_S *)args;

    printf("%s: id %d w %d h %d\n", __FUNCTION__, pstRes->s32ChannelId, pstRes->s32W, pstRes->s32H);

    return IPC_PF_VI_SetRes(pstRes->s32ChannelId, pstRes->s32W, pstRes->s32H);
}

static AR_S32 IPC_CTRL_SetViAntiFlicker(void * args, void * reply)
{
    IPC_CTRL_VI_ANTI_FLICKER_S * pstAnti = (IPC_CTRL_VI_ANTI_FLICKER_S *)args;

    printf("%s: id %d freq %d\n", __FUNCTION__, pstAnti->s32ChannelId, pstAnti->s32Freqency);

    return IPC_PF_VI_SetAntiFlicker(pstAnti->s32ChannelId, pstAnti->s32Freqency);
}

static AR_S32 IPC_CTRL_SetViDenoise3D(void * args, void * reply)
{
    IPC_CTRL_VI_DENOISE_S * pstDe = (IPC_CTRL_VI_DENOISE_S *)args;

    return IPC_PF_VI_SetDenoise3D(pstDe->s32ChannelId, pstDe->s32Strength);
}

static AR_S32 IPC_CTRL_GetViDenoise3D(void * args, void * reply)
{
    IPC_CTRL_VI_DENOISE_S * pstDe = (IPC_CTRL_VI_DENOISE_S *)args;
    IPC_CTRL_VI_DENOISE_S stRet;

    stRet.s32ChannelId = pstDe->s32ChannelId;
    stRet.s32Strength = IPC_PF_VI_GetDenoise3D(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_SetViDenoise2D(void * args, void * reply)
{
    IPC_CTRL_VI_DENOISE_S * pstDe = (IPC_CTRL_VI_DENOISE_S *)args;

    return IPC_PF_VI_SetDenoise2D(pstDe->s32ChannelId, pstDe->s32Strength);
}

static AR_S32 IPC_CTRL_GetViDenoise2D(void * args, void * reply)
{
    IPC_CTRL_VI_DENOISE_S * pstDe = (IPC_CTRL_VI_DENOISE_S *)args;
    IPC_CTRL_VI_DENOISE_S stRet;

    stRet.s32ChannelId = pstDe->s32ChannelId;
    stRet.s32Strength = IPC_PF_VI_GetDenoise2D(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_SetViExpLimit(void * args, void * reply)
{
    IPC_CTRL_VI_EXPLIMIT_S * pstExp = (IPC_CTRL_VI_EXPLIMIT_S *)args;

    return IPC_PF_VI_SetExpLimit(pstExp->s32ChannelId, pstExp->s32ExpLimitUs);
}

static AR_S32 IPC_CTRL_GetViExpLimit(void * args, void * reply)
{
    IPC_CTRL_VI_EXPLIMIT_S * pstExp = (IPC_CTRL_VI_EXPLIMIT_S *)args;
    IPC_CTRL_VI_EXPLIMIT_S stRet;

    stRet.s32ChannelId = pstExp->s32ChannelId;
    stRet.s32ExpLimitUs = IPC_PF_VI_GetExpLimit(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_SetViWdr(void * args, void * reply)
{
    IPC_CTRL_VI_WDR_S * pstWdr = (IPC_CTRL_VI_WDR_S *)args;

    printf("%s: id %d wdr %d\n", __FUNCTION__, pstWdr->s32ChannelId, pstWdr->s32Enable);

    return IPC_PF_VI_SetHDR(pstWdr->s32ChannelId, pstWdr->s32Enable);
}

static AR_S32 IPC_CTRL_GetVi3aInfo(void * args, void * reply)
{
    IPC_CTRL_VI_3A_S * pstExp = (IPC_CTRL_VI_3A_S *)args;
    IPC_VI_VIDEO_3A_PARAM_S stVi3aInfo = {0};

    stVi3aInfo.s32ChannelId = pstExp->s32ChannelId;

    IPC_PF_VI_Get3aInfoTidyAttr(stVi3aInfo.s32ChannelId, &stVi3aInfo);

    IPC_MID_UTIL_RPCReply(reply, &stVi3aInfo, sizeof(stVi3aInfo));

    return 0;
}

static AR_S32 IPC_CTRL_SetVencBrc(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_VENC_VIDEO_PARAM_S stVencParam = {0};
    IPC_CTRL_VENC_BRC_S * pstBrc = (IPC_CTRL_VENC_BRC_S *)args;
    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pstBrc->s32ChannelId, pstBrc->s32Stream);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    printf("%s: id %d stream %d mode %d min %d max %d avg %d, stattime %d\n", __FUNCTION__,
        pstBrc->s32ChannelId, pstBrc->s32Stream, pstBrc->s32Mode, 
        pstBrc->s32Min, pstBrc->s32Max, pstBrc->s32Average, pstBrc->s32StatTime);

    s32Ret = IPC_PF_VENC_GetVideoParam(VeChn, &stVencParam);
    if(s32Ret < 0)
    {
        printf("Get venc params error.\n");
        return s32Ret;
    }

    stVencParam.rc_mode = pstBrc->s32Mode;
    stVencParam.Kbps = pstBrc->s32Average;
    stVencParam.max_Kbps = pstBrc->s32Max;
    stVencParam.min_Kbps = pstBrc->s32Min;
    stVencParam.u32StatTime = pstBrc->s32StatTime;

    s32Ret = IPC_PF_VENC_SetVideoParam(VeChn, &stVencParam);

    if(s32Ret < 0)
    {
        printf("Set venc params error.\n");
        return s32Ret;
    }

    return 0;
}
static AR_S32 IPC_CTRL_SetVencGop(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_VENC_VIDEO_PARAM_S stVencParam = {0};
    IPC_CTRL_VENC_GOP_S * pstGop = (IPC_CTRL_VENC_GOP_S *)args;

    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pstGop->s32ChannelId, pstGop->s32Stream);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    printf("%s: id %d stream %d gop %d\n", __FUNCTION__, pstGop->s32ChannelId, pstGop->s32Stream, pstGop->s32Value);

    s32Ret = IPC_PF_VENC_GetVideoParam(VeChn, &stVencParam);
    if(s32Ret < 0)
    {
        printf("Get venc params error.\n");
        return s32Ret;
    }

    stVencParam.gop = pstGop->s32Value;

    s32Ret = IPC_PF_VENC_SetVideoParam(VeChn, &stVencParam);

    if(s32Ret < 0)
    {
        printf("Set venc params error.\n");
        return s32Ret;
    }

    return 0;
}
static AR_S32 IPC_CTRL_SetVencQp(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_VENC_RC_PARAM_S stRcParam = {0};
    IPC_CTRL_VENC_QP_S * pstQp = (IPC_CTRL_VENC_QP_S *)args;
    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pstQp->s32ChannelId, pstQp->s32Stream);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    stRcParam.u32MaxIQp = pstQp->s32MaxIQp;
    stRcParam.u32MaxPQp = pstQp->s32MaxPQp;
    stRcParam.u32MinIQp = pstQp->s32MinIQp;
    stRcParam.u32MinPQp = pstQp->s32MinPQp;

    s32Ret = IPC_PF_VENC_SetRcParam(VeChn, &stRcParam);

    if(s32Ret < 0)
    {
        printf("Set venc rc params error.\n");
        return s32Ret;
    }

    return 0;
}
//swith venc type needs to recreate the pipe.
static AR_S32 IPC_CTRL_SetVencType(void * args, void * reply)
{
    IPC_CTRL_VENC_TYPE_S * pstType = (IPC_CTRL_VENC_TYPE_S *)args;
    AR_S32 s32Ret = 0;
    IPC_VENC_VIDEO_PARAM_S stVencParam = {0};

    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pstType->s32ChannelId, pstType->s32Stream);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    printf("%s: id %d stream %d Type %d\n", __FUNCTION__, pstType->s32ChannelId, pstType->s32Stream, pstType->s32Type);

    s32Ret = IPC_PF_VENC_GetVideoParam(VeChn, &stVencParam);
    if(s32Ret < 0)
    {
        printf("Get venc params error.\n");
        return s32Ret;
    }

    stVencParam.u32CodecType = pstType->s32Type;

    s32Ret = IPC_PF_VENC_SetVideoParam(VeChn, &stVencParam);

    if(s32Ret < 0)
    {
        printf("Set venc params error.\n");
        return s32Ret;
    }

    return 0;
}
static AR_S32 IPC_CTRL_SetVencFps(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_VENC_VIDEO_PARAM_S stVencParam = {0};
    IPC_CTRL_VENC_FPS_S * pstFps = (IPC_CTRL_VENC_FPS_S *)args;

    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pstFps->s32ChannelId, pstFps->s32Stream);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    printf("%s: id %d stream %d gop %d\n", __FUNCTION__, pstFps->s32ChannelId, pstFps->s32Stream, pstFps->s32Fps);

    s32Ret = IPC_PF_VENC_GetVideoParam(VeChn, &stVencParam);
    if(s32Ret < 0)
    {
        printf("Get venc params error.\n");
        return s32Ret;
    }

    stVencParam.fps = pstFps->s32Fps;

    s32Ret = IPC_PF_VENC_SetVideoParam(VeChn, &stVencParam);

    if(s32Ret < 0)
    {
        printf("Set venc params error.\n");
        return s32Ret;
    }

    return 0;
}
static AR_S32 IPC_CTRL_SetVrecord(void * args, void * reply)
{
    IPC_CTRL_VRECORD_S * pstVrecord = (IPC_CTRL_VRECORD_S *)args;
    printf("%s: id %d stream %d format %d mode %d start %d\n", __FUNCTION__, pstVrecord->s32ChannelId, pstVrecord->s32Stream,
        pstVrecord->s32Format, pstVrecord->s32Mode, pstVrecord->s32StartOrStop);

    IPC_MID_RECMNG_VedioRecord(pstVrecord->s32ChannelId, pstVrecord->s32Stream, pstVrecord->s32Mode,pstVrecord->s32Format, pstVrecord->s32StartOrStop);
	
    return 0;
}
static AR_S32 IPC_CTRL_SetArecord(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_ARECORD_S * pstArecord = (IPC_CTRL_ARECORD_S *)args;
    printf("%s: path %s start %d, stram id = %d\n", __FUNCTION__, pstArecord->cPathName, pstArecord->s32StartOrStop, pstArecord->u32StreamId);
    if(pstArecord->s32StartOrStop)
    {
        s32Ret = IPC_PF_Audio_RecordStart(pstArecord->cPathName, pstArecord->u32StreamId);
    }
    else
    {
        s32Ret = IPC_PF_Audio_RecordStop();
    }
    
    return s32Ret;
}
static AR_S32 IPC_CTRL_SetPlayback(void * args, void * reply)
{
    IPC_CTRL_PLAYBACK_S * pstPb = (IPC_CTRL_PLAYBACK_S *)args;
    printf("%s: start %d file %s\n", __FUNCTION__, pstPb->s32StartOrStop, pstPb->achFileName);
    
    if(!pstPb->s32AudioOrVedio)
    {
        if(pstPb->s32StartOrStop)
        {
            IPC_PF_Audio_RecordPlayStart(pstPb->achFileName);
        }
        else
        {
            IPC_PF_Audio_RecordPlayStop();
        }
    }
    return 0;
}
static AR_S32 IPC_CTRL_Snapshot(void * args, void * reply)
{
    IPC_CTRL_SNAPSHOT_S * pstSnap = (IPC_CTRL_SNAPSHOT_S *)args;
    //VPSS_GRP VpssGrp = 0;
    //VPSS_CHN VpssChn = 0;

    printf("%s: grp %d, chn %d, cnt %d, path %s \n", __FUNCTION__, pstSnap->s32VpssGrp, pstSnap->s32VpssChn, pstSnap->s32Cnt, pstSnap->strPath);

    return IPC_PF_VENC_SnapProcess(pstSnap->s32VpssGrp, pstSnap->s32VpssChn, pstSnap->s32Cnt, pstSnap->strPath);
}

static AR_S32 IPC_CTRL_VoFullScreen(void * args, void * reply)
{
    IPC_CTRL_VOFULLSCREEN_S * pstVofs = (IPC_CTRL_VOFULLSCREEN_S *)args;

    printf("%s: vochn %d, enc %d \n", __FUNCTION__, pstVofs->s32VoChn, pstVofs->s32Enable);

    return IPC_PF_VO_FullScreen_Process(pstVofs->s32VoChn, pstVofs->s32Enable);
}

static AR_S32 IPC_CTRL_SetImgScene(void * args, void * reply)
{
    IPC_CTRL_IMG_SCENE_S * pstScene = (IPC_CTRL_IMG_SCENE_S *)args;

    return IPC_PF_VI_SetImgScene(pstScene->s32ChannelId, pstScene->s32Mode, pstScene->strTunningFile);
}
static AR_S32 IPC_CTRL_SetImgStyle(void * args, void * reply)
{
    IPC_CTRL_IMG_STYLE_S * pstStyle = (IPC_CTRL_IMG_STYLE_S *)args;

    printf("%s: id %d style %d\n", __FUNCTION__, pstStyle->s32ChannelId, pstStyle->s32Style);
    return 0;
}
static AR_S32 IPC_CTRL_SetImgBright(void * args, void * reply)
{
    IPC_CTRL_IMG_BRIGHT_S * pstBright = (IPC_CTRL_IMG_BRIGHT_S *)args;

    printf("%s: id %d bright %d\n", __FUNCTION__, pstBright->s32ChannelId, pstBright->s32BrightValue);

    return IPC_PF_VI_SetBrightness(pstBright->s32ChannelId, pstBright->s32BrightValue);
}

static AR_S32 IPC_CTRL_SetImgContrast(void * args, void * reply)
{
    IPC_CTRL_IMG_CONTRAST_S * pstContrast = (IPC_CTRL_IMG_CONTRAST_S *)args;

    printf("%s: id %d contrast %d\n", __FUNCTION__, pstContrast->s32ChannelId, pstContrast->s32ContrastValue);
    return IPC_PF_VI_SetContrast(pstContrast->s32ChannelId, pstContrast->s32ContrastValue);
}
static AR_S32 IPC_CTRL_SetImgSaturation(void * args, void * reply)
{
    IPC_CTRL_IMG_SATURATION_S * pstSaturation = (IPC_CTRL_IMG_SATURATION_S *)args;

    printf("%s: id %d saturation %d\n", __FUNCTION__, pstSaturation->s32ChannelId, pstSaturation->s32SaturationValue);
    return IPC_PF_VI_SetSatueration(pstSaturation->s32ChannelId, pstSaturation->s32SaturationValue);
}
static AR_S32 IPC_CTRL_SetImgSharpness(void * args, void * reply)
{
    IPC_CTRL_IMG_SHARPNESS_S * pstSharpness = (IPC_CTRL_IMG_SHARPNESS_S *)args;

    printf("%s: id %d sharpness %d\n", __FUNCTION__, pstSharpness->s32ChannelId, pstSharpness->s32SharpnessValue);
    return IPC_PF_VI_SetSharpness(pstSharpness->s32ChannelId, pstSharpness->s32SharpnessValue);
}
static AR_S32 IPC_CTRL_SetImgHue(void * args, void * reply)
{
    IPC_CTRL_IMG_HUE_S * pstHue = (IPC_CTRL_IMG_HUE_S *)args;

    printf("%s: id %d hue %d\n", __FUNCTION__, pstHue->s32ChannelId, pstHue->s32HueValue);
    return IPC_PF_VI_SetHue(pstHue->s32ChannelId, pstHue->s32HueValue);
}

static AR_S32 IPC_CTRL_GetImgBright(void * args, void * reply)
{
    IPC_CTRL_IMG_BRIGHT_S * pstArgs = (IPC_CTRL_IMG_BRIGHT_S *)args;
    IPC_CTRL_IMG_BRIGHT_S stRet;

    stRet.s32ChannelId = pstArgs->s32ChannelId;
    stRet.s32BrightValue = IPC_PF_VI_GetBrightness(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_GetImgContrast(void * args, void * reply)
{
    IPC_CTRL_IMG_CONTRAST_S * pstArgs = (IPC_CTRL_IMG_CONTRAST_S *)args;
    IPC_CTRL_IMG_CONTRAST_S stRet;

    stRet.s32ChannelId = pstArgs->s32ChannelId;
    stRet.s32ContrastValue = IPC_PF_VI_GetContrast(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_GetImgSaturation(void * args, void * reply)
{
    IPC_CTRL_IMG_SATURATION_S * pstArgs = (IPC_CTRL_IMG_SATURATION_S *)args;
    IPC_CTRL_IMG_SATURATION_S stRet;

    stRet.s32ChannelId = pstArgs->s32ChannelId;
    stRet.s32SaturationValue = IPC_PF_VI_GetSatueration(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_GetImgSharpness(void * args, void * reply)
{
    IPC_CTRL_IMG_SHARPNESS_S * pstArgs = (IPC_CTRL_IMG_SHARPNESS_S *)args;
    IPC_CTRL_IMG_SHARPNESS_S stRet;

    stRet.s32ChannelId = pstArgs->s32ChannelId;
    stRet.s32SharpnessValue = IPC_PF_VI_GetSharpness(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}
static AR_S32 IPC_CTRL_GetImgHue(void * args, void * reply)
{
    IPC_CTRL_IMG_HUE_S * pstArgs = (IPC_CTRL_IMG_HUE_S *)args;
    IPC_CTRL_IMG_HUE_S stRet;

    stRet.s32ChannelId = pstArgs->s32ChannelId;
    stRet.s32HueValue = IPC_PF_VI_GetHue(stRet.s32ChannelId);

    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_SetWdt(void * args, void * reply)
{
    IPC_CTRL_WDT_S * pstWdt = (IPC_CTRL_WDT_S *)args;

    printf("%s: wdt %d \n", __FUNCTION__, pstWdt->s32OnOff);
    return 0;
}
static AR_S32 IPC_CTRL_SetGpio(void * args, void * reply)
{
    IPC_CTRL_GPIO_S * pstGpio = (IPC_CTRL_GPIO_S *)args;

    IPC_PF_GPIO_SetByName(pstGpio->strName, pstGpio->s32Direction, pstGpio->u32Value);

    return 0;
}
static AR_S32 IPC_CTRL_GetGpio(void * args, void * reply)
{
    IPC_CTRL_GPIO_S * pstGpio = (IPC_CTRL_GPIO_S *)args;
    IPC_CTRL_GPIO_S stStatus;

    strcpy(stStatus.strName, pstGpio->strName);
    IPC_PF_GPIO_GetByName(stStatus.strName, (ENUM_GPIO_DIR *)(&stStatus.s32Direction), (AR_U32 *)(&stStatus.u32Value));

    IPC_MID_UTIL_RPCReply(reply, &stStatus, sizeof(stStatus));

    return 0;
}
static AR_S32 IPC_CTRL_SetSvp(void * args, void * reply)
{
    IPC_CTRL_SVP_S * pstSvp = (IPC_CTRL_SVP_S *)args;

    printf("%s: svp %d \n", __FUNCTION__, pstSvp->s32OnOff);
    return 0;
}
static AR_S32 IPC_CTRL_SetWifiMode(void * args, void * reply)
{
    IPC_CTRL_WIFIMODE_S * pstWifi = (IPC_CTRL_WIFIMODE_S *)args;

    printf("%s: wifimode %d \n", __FUNCTION__, pstWifi->s32WifiMode);
    return 0;
}
static AR_S32 IPC_CTRL_Reboot(void * args, void * reply)
{
    IPC_CTRL_REBOOT_S * pstReboot = (IPC_CTRL_REBOOT_S *)args;

    printf("%s: reboot %d \n", __FUNCTION__, pstReboot->s32RebootFlag);
    return 0;
}
static AR_S32 IPC_CTRL_Poweroff(void * args, void * reply)
{
    IPC_CTRL_POWEROFF_S * pstPwr = (IPC_CTRL_POWEROFF_S *)args;

    printf("%s: poweroff %d \n", __FUNCTION__, pstPwr->s32PoweroffFlag);
    return 0;
}
static AR_S32 IPC_CTRL_DumpYuv(void * args, void * reply)
{
    IPC_CTRL_DUMP_YUV_S * pstDump = (IPC_CTRL_DUMP_YUV_S *)args;
    AR_CHAR strPath[128] = {0};
    FILE * fp = NULL;

    printf("%s: dump yuv %d \n", __FUNCTION__, pstDump->s32ChannelId);

    //set a path
    IPC_SYS_TIME_INFO_S stTime;
    IPC_MID_UTIL_GetLocalTime(&stTime);
    sprintf(strPath, "/mnt/dump/ch-%1d-%4d-%2d-%2d-%2d-%2d-%2d.yuv", pstDump->s32ChannelId, stTime.Year, stTime.Mon, stTime.Day, stTime.Hour, stTime.Min, stTime.Sec);

    //Get yuv buffer and save to file
    fp = fopen(strPath, "wb");
    if(!fp)
    {
        return -1;
    }

    //fwrite(yuvbuff, 1, 1920*1080, fp);

    fclose(fp);

    return 0;
}

static AR_S32 IPC_CTRL_GetVencParam(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VENC_CH_S * pCh = (IPC_CTRL_VENC_CH_S *)args;
    IPC_CTRL_VENC_CH_S stRet = {0};
    IPC_VENC_VIDEO_PARAM_S stVencParam = {0};
    IPC_VENC_RC_PARAM_S  stRcParam = {0};

    VENC_CHN VeChn = IPC_GET_VENC_CHN_ID(pCh->s32ChannelId, pCh->s32StreamId);

    if(VeChn > IPC_TOTAL_VENC_STREAM_NUM)
    {
        printf("Invalid channel num: %d\n", VeChn);
        return -1;
    }

    s32Ret = IPC_PF_VENC_GetVideoParam(VeChn, &stVencParam);
    if(s32Ret < 0)
    {
        printf("Get venc params error.\n");
        return s32Ret;
    }

    stRet.s32ChannelId = pCh->s32ChannelId;
    stRet.s32StreamId = pCh->s32StreamId;
    stRet.stParam.u32CodecType = stVencParam.u32CodecType;
    stRet.stParam.u32Height = stVencParam.height;
    stRet.stParam.u32Width = stVencParam.width;
    stRet.stParam.u32RcMode = stVencParam.rc_mode;
    stRet.stParam.u32Gop = stVencParam.gop;
    stRet.stParam.u32FrameRate = stVencParam.fps;
    stRet.stParam.u32CbrKbps = stVencParam.Kbps;
    stRet.stParam.u32MaxKbps = stVencParam.max_Kbps;
    stRet.stParam.u32MinKbps = stVencParam.min_Kbps;
    stRet.stParam.u32StatTime = stVencParam.u32StatTime;

    s32Ret = IPC_PF_VENC_GetRcParam(VeChn, &stRcParam);
    if(s32Ret < 0)
    {
        printf("Get venc rc params error.\n");
        return s32Ret;
    } 

    stRet.stParam.u32MinIQp = stRcParam.u32MinIQp;
    stRet.stParam.u32MaxIQp = stRcParam.u32MaxIQp;
    stRet.stParam.u32MinPQp = stRcParam.u32MinPQp;
    stRet.stParam.u32MaxPQp = stRcParam.u32MaxPQp;
    
    IPC_MID_UTIL_RPCReply(reply, &stRet, sizeof(stRet));

    return 0;
}

static AR_S32 IPC_CTRL_DrawVpssOsd(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_OSD_S * pstOsd = (IPC_CTRL_OSD_S *)args;

    s32Ret = IPC_PF_OSD_DrawString(pstOsd->u32VpssGrp, pstOsd->u32StrNum, pstOsd->stOsdStr, pstOsd->u32ImgWidth, pstOsd->u32ImgHeight);
    if(s32Ret)
    {
        printf("Draw string error.\n");
        return s32Ret;
    }

    s32Ret = IPC_PF_OSD_DrawRect(pstOsd->u32VpssGrp, pstOsd->u32RectNum, pstOsd->stOsdRect, pstOsd->u32ImgWidth, pstOsd->u32ImgHeight);
    if(s32Ret)
    {
        printf("Draw rect error.\n");
        return s32Ret;
    }

    return 0;
}

static AR_S32 IPC_CTRL_DrawVpssGeomOsd(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_GEOM_OSD_S * pstOsd = (IPC_CTRL_GEOM_OSD_S *)args;

    //printf("Draw Geom VpssGrp %d, RectNum %d, Imagewidth %d, imageHeiht %d\n", pstOsd->u32VpssGrp, pstOsd->u32RectNum, pstOsd->u32ImgWidth, pstOsd->u32ImgHeight);
    s32Ret = IPC_PF_OSD_DrawGeomRect(pstOsd->u32VpssGrp, pstOsd->u32RectNum, pstOsd->stOsdRect, pstOsd->u32ImgWidth, pstOsd->u32ImgHeight);
    if(s32Ret)
    {
        printf("Draw rect error.\n");
        return s32Ret;
    }

    return 0;
}

static AR_S32 IPC_CTRL_SetVpssFlip(void * args, void * reply)
{
    IPC_CTRL_VPSS_FLIP_S * pstFlip = (IPC_CTRL_VPSS_FLIP_S *)args;

    return IPC_PF_VPSS_SetFlip(pstFlip->s32GroupId, pstFlip->s32ChannelId, pstFlip->s32Enable);
}

static AR_S32 IPC_CTRL_SetVpssMirror(void * args, void * reply)
{
    IPC_CTRL_VPSS_MIRROR_S * pstMirror = (IPC_CTRL_VPSS_MIRROR_S *)args;

    return IPC_PF_VPSS_SetMirror(pstMirror->s32GroupId, pstMirror->s32ChannelId, pstMirror->s32Enable);
}


static AR_S32 IPC_CTRL_SetStreamOsd(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_STREAM_OSD_S * pstOsd = (IPC_CTRL_STREAM_OSD_S *)args;

    s32Ret = IPC_PF_SetStreamOsd(pstOsd->u32ChannelId, pstOsd->u32StreamId,
		pstOsd->u32ChnNameEnable, pstOsd->strChnName,
		pstOsd->u32DateTimeEnable, pstOsd->u32DateFormat,
		pstOsd->u32TimeFormat, pstOsd->u32DisplayWeek,
		pstOsd->u32RGB, pstOsd->u32FontSize);
    if(s32Ret)
    {
        printf("Set stream OSD error.\n");
        return s32Ret;
    }

    return 0;
}

static AR_S32 IPC_CTRL_SetMd(void * args, void * reply)
{
    IPC_CTRL_MD_S * pstMd = (IPC_CTRL_MD_S *)args;

    printf("%s: enable %d\n", __FUNCTION__, pstMd->s32Enable);
    return IPC_PF_MD_SetMd(pstMd->s32Enable);
}

static AR_S32 IPC_CTRL_SetOd(void * args, void * reply)
{
    IPC_CTRL_OD_S * pstOd = (IPC_CTRL_OD_S *)args;

    printf("%s: enable %d\n", __FUNCTION__, pstOd->s32Enable);
    return IPC_PF_OD_SetOd(pstOd->s32Enable);
}


static AR_S32 IPC_CTRL_Start(void * args, void * reply)
{
	printf("======= IPC_CTRL_Start =======\n");
	IPC_PF_CONTROL_Start();

	return 0;
}

static AR_S32 IPC_CTRL_Stop(void * args, void * reply)
{
	printf("======= IPC_CTRL_Stop =======\n");
	IPC_PF_CONTROL_Stop();

	return 0;
}

static AR_S32 IPC_CTRL_Create(void * args, void * reply)
{
	printf("======= IPC_CTRL_Create =======\n");
    if(!g_s32RunningStatus)
    {
        IPC_AV_Init();
		g_s32RunningStatus = 1;
    }
    else
    {
        printf("======= IPC CTRL Has Create =======\n");
    }
	return 0;
}

static AR_S32 IPC_CTRL_Destroy(void * args, void * reply)
{
    printf("======= IPC_CTRL_Destroy =======\n");
    if(g_s32RunningStatus)
    {
        IPC_AV_UnInit();
		g_s32RunningStatus = 0;
    }
    else
    {
        printf("======= IPC CTRL Has Destroy =======\n");
    }
	
	return 0;
}

//this will set VPSS channel resolution, the venc should adapt accordingly.
static AR_S32 IPC_CTRL_SetStreamRes(IPC_CTRL_CMD_ARGS_S * pstCmdSting)
{
    IPC_CTRL_STREAM_RES_S stStreamRes = {0};
    AR_S32 s32Index1 = 0, s32Index2 = 0, s32Index3 = 0, s32Index4 = 0;

    s32Index1 = AR_IPCTOOL_IndexFindByName("-w", pstCmdSting);
    if(s32Index1 >= 0){
        stStreamRes.s32Width = atoi(pstCmdSting->s8Args[s32Index1 + 1]);
    }

    s32Index2 = AR_IPCTOOL_IndexFindByName("-h", pstCmdSting);
    if(s32Index2 >= 0){
        stStreamRes.s32Height = atoi(pstCmdSting->s8Args[s32Index2 + 1]);
    }
    s32Index3 = AR_IPCTOOL_IndexFindByName("-vg", pstCmdSting);
    if(s32Index3 >= 0){
        stStreamRes.s32VpssGrp = atoi(pstCmdSting->s8Args[s32Index3 + 1]);
    }
    s32Index4 = AR_IPCTOOL_IndexFindByName("-vc", pstCmdSting);
    if(s32Index3 >= 0){
        stStreamRes.s32VpssChn = atoi(pstCmdSting->s8Args[s32Index4 + 1]);
    }
    if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0) || (s32Index4 < 0)){
        printf("cmd parameters error\r\n");
        return -1;
    }

    printf("%s: id %d stream %d w %d h %d\n", __FUNCTION__, stStreamRes.s32VpssGrp, stStreamRes.s32VpssChn, stStreamRes.s32Width, stStreamRes.s32Height);

    return IPC_PF_Stream_SetRes(stStreamRes.s32VpssGrp, stStreamRes.s32VpssChn, stStreamRes.s32Width, stStreamRes.s32Height);
}

static AR_S32 IPC_CTRL_RTCMD(void * args, void * reply)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_CMD_STRING_S* pstCmdSting = (IPC_CTRL_CMD_STRING_S*)args;

    IPC_CTRL_CMD_ARGS_S * pstCmdArgs = AR_IPCTOOL_GetArgs(pstCmdSting->s8Cmd_String);
    if(!pstCmdArgs){
    	printf("ipctool get args failed\r\n");
    	return -1;
    }
    // To do; add test cmd here.
    AR_S32 s32Index = AR_IPCTOOL_IndexFindByName("help",pstCmdArgs);
    if(s32Index >= 0){
    	printf("rtcmd usage:\n");
    	printf("rtcmd setstreamres -w [width] -h [height] -vg [vpss grop] -vc [vpss channel]\n ");
    }

    s32Index = AR_IPCTOOL_IndexFindByName("setstreamres", pstCmdArgs);
    if(s32Index >= 0){
    	s32Ret = IPC_CTRL_SetStreamRes(pstCmdArgs);
    }

    if(pstCmdArgs){
        free(pstCmdArgs);
    }
    return s32Ret;
}

void IPC_CTRL_StartRPCService()
{
	g_s32RunningStatus = 1;
	
    IPC_MID_UTIL_RPCInit();

    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_FLIP, IPC_CTRL_SetFlip, sizeof(IPC_CTRL_FLIP_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_MIRROR, IPC_CTRL_SetMirror, sizeof(IPC_CTRL_MIRROR_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_FPS, IPC_CTRL_SetViFps, sizeof(IPC_CTRL_VI_FPS_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_RES, IPC_CTRL_SetViRes, sizeof(IPC_CTRL_VI_RES_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_ANTIFLICKER, IPC_CTRL_SetViAntiFlicker, sizeof(IPC_CTRL_VI_ANTI_FLICKER_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_DENOISE_3D, IPC_CTRL_SetViDenoise3D, sizeof(IPC_CTRL_VI_DENOISE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_WDR, IPC_CTRL_SetViWdr, sizeof(IPC_CTRL_VI_WDR_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VENC_BRC, IPC_CTRL_SetVencBrc, sizeof(IPC_CTRL_VENC_BRC_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VENC_GOP, IPC_CTRL_SetVencGop, sizeof(IPC_CTRL_VENC_GOP_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VENC_QP, IPC_CTRL_SetVencQp, sizeof(IPC_CTRL_VENC_QP_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VENC_TYPE, IPC_CTRL_SetVencType, sizeof(IPC_CTRL_VENC_TYPE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VENC_FPS, IPC_CTRL_SetVencFps, sizeof(IPC_CTRL_VENC_FPS_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VRECORD, IPC_CTRL_SetVrecord, sizeof(IPC_CTRL_VRECORD_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_ARECORD, IPC_CTRL_SetArecord, sizeof(IPC_CTRL_ARECORD_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_PLAYBACK, IPC_CTRL_SetPlayback, sizeof(IPC_CTRL_PLAYBACK_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VOFULLSCREEN, IPC_CTRL_VoFullScreen, sizeof(IPC_CTRL_VOFULLSCREEN_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SNAPSHOT, IPC_CTRL_Snapshot, sizeof(IPC_CTRL_SNAPSHOT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_SCENE, IPC_CTRL_SetImgScene, sizeof(IPC_CTRL_IMG_SCENE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_STYLE, IPC_CTRL_SetImgStyle, sizeof(IPC_CTRL_IMG_STYLE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_BRIGHT, IPC_CTRL_SetImgBright, sizeof(IPC_CTRL_IMG_BRIGHT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_CONTRAST, IPC_CTRL_SetImgContrast, sizeof(IPC_CTRL_IMG_CONTRAST_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_SATURATION, IPC_CTRL_SetImgSaturation, sizeof(IPC_CTRL_IMG_SATURATION_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_SHARPNESS, IPC_CTRL_SetImgSharpness, sizeof(IPC_CTRL_IMG_SHARPNESS_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_IMG_HUE, IPC_CTRL_SetImgHue, sizeof(IPC_CTRL_IMG_HUE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_IMG_BRIGHT, IPC_CTRL_GetImgBright, sizeof(IPC_CTRL_IMG_BRIGHT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_IMG_CONTRAST, IPC_CTRL_GetImgContrast, sizeof(IPC_CTRL_IMG_CONTRAST_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_IMG_SATURATION, IPC_CTRL_GetImgSaturation, sizeof(IPC_CTRL_IMG_SATURATION_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_IMG_SHARPNESS, IPC_CTRL_GetImgSharpness, sizeof(IPC_CTRL_IMG_SHARPNESS_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_IMG_HUE, IPC_CTRL_GetImgHue, sizeof(IPC_CTRL_IMG_HUE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_WDT, IPC_CTRL_SetWdt, sizeof(IPC_CTRL_WDT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_GPIO, IPC_CTRL_SetGpio, sizeof(IPC_CTRL_GPIO_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_GPIO, IPC_CTRL_GetGpio, sizeof(IPC_CTRL_GPIO_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_SVP, IPC_CTRL_SetSvp, sizeof(IPC_CTRL_SVP_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_WIFI_MODE, IPC_CTRL_SetWifiMode, sizeof(IPC_CTRL_WIFIMODE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_REBOOT, IPC_CTRL_Reboot, sizeof(IPC_CTRL_REBOOT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_POWEROFF, IPC_CTRL_Poweroff, sizeof(IPC_CTRL_POWEROFF_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_DUMP_YUV, IPC_CTRL_DumpYuv, sizeof(IPC_CTRL_DUMP_YUV_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_VENC_PARAM, IPC_CTRL_GetVencParam, sizeof(IPC_CTRL_VENC_CH_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_DRAW_VPSS_OSD, IPC_CTRL_DrawVpssOsd, sizeof(IPC_CTRL_OSD_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_DRAW_VPSS_GEOM_OSD, IPC_CTRL_DrawVpssGeomOsd, sizeof(IPC_CTRL_GEOM_OSD_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VPSS_FLIP, IPC_CTRL_SetVpssFlip, sizeof(IPC_CTRL_VPSS_FLIP_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VPSS_MIRROR, IPC_CTRL_SetVpssMirror, sizeof(IPC_CTRL_VPSS_MIRROR_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_STREAM_OSD, IPC_CTRL_SetStreamOsd, sizeof(IPC_CTRL_STREAM_OSD_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_VI_DENOISE_3D, IPC_CTRL_GetViDenoise3D, sizeof(IPC_CTRL_VI_DENOISE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_VI_DENOISE_2D, IPC_CTRL_GetViDenoise2D, sizeof(IPC_CTRL_VI_DENOISE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_DENOISE_2D, IPC_CTRL_SetViDenoise2D, sizeof(IPC_CTRL_VI_DENOISE_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_VI_EXPLIMIT, IPC_CTRL_SetViExpLimit, sizeof(IPC_CTRL_VI_EXPLIMIT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_VI_EXPLIMIT, IPC_CTRL_GetViExpLimit, sizeof(IPC_CTRL_VI_EXPLIMIT_S));
    IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_GET_VI_3AINFO, IPC_CTRL_GetVi3aInfo, sizeof(IPC_CTRL_VI_3A_S));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_MD, IPC_CTRL_SetMd, sizeof(IPC_CTRL_MD_S));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_SET_OD, IPC_CTRL_SetOd, sizeof(IPC_CTRL_OD_S));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_START, IPC_CTRL_Start, sizeof(int));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_STOP, IPC_CTRL_Stop, sizeof(int));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_CREATE, IPC_CTRL_Create, sizeof(int));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_DESTORY, IPC_CTRL_Destroy, sizeof(int));
	IPC_MID_UTIL_RPCRegister(IPC_RPC_CMD_RTCMD, IPC_CTRL_RTCMD, sizeof(IPC_CTRL_CMD_STRING_S));

    //Register all handler here.
}

AR_S32 IPC_CTRL_StopRPCService()
{
	g_s32RunningStatus = 0;
    IPC_MID_UTIL_RPCUnInit();

    return 0;
}


void IPC_CTRL_Suspend()
{
	IPC_CTRL_StopRPCService();
	printf("IPC_CTRL_Suspend \n");
}

void IPC_CTRL_Resume()
{
	IPC_CTRL_StartRPCService();
	printf("IPC_CTRL_Resume \n");
}

