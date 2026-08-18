#ifndef _IPCTOOL_CALLBACK_HANDLER_H_
#define _IPCTOOL_CALLBACK_HANDLER_H_
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
//#include "binder_ipc_common.h"
#include "binder_ipc.h"
#if 0
typedef uint32_t   AR_U32;
typedef int32_t    AR_S32;
typedef uint16_t   AR_U16;
typedef int16_t    AR_S16;
typedef unsigned char    AR_UCHAR;
typedef char     AR_CHAR;
typedef uint32_t   AR_BOOL;
typedef float      AR_FLOAT;
#endif

#define MAX_CALLBACK_NUM 32
#define BINDER_IPCTOOL_NAME "ipc_ctrl"
#define IPC_RPC_CMD_BASE            0x76540000

#define VENC_ENCODE_TYPE_H264 1
#define VENC_ENCODE_TYPE_H265 2
#define VENC_ENCODE_TYPE_MJPEG 3

/*0: CBR; 1: VBR; 2: CVBR, 3: AVBR, 4: QPMAP, 5: FIXQP, 6 QVBR*/
#define VENC_BRCMODE_CBR 0
#define VENC_BRCMODE_VBR 1
#define VENC_BRCMODE_CVBR 2
#define VENC_BRCMODE_AVBR 3
#define VENC_BRCMODE_QPMAP 4
#define VENC_BRCMODE_FIXQP 5
#define VENC_BRCMODE_QVBR 6

typedef struct arIPCTOOL_CMD_ARGS_S
{
	 AR_S32 s32ArgsNum;
	 AR_CHAR s8Args[64][128];
} IPCTOOL_CMD_ARGS_S;

typedef struct arIPCTOOL_SERVICE_S
{
	AR_CHAR s8ServiceName[128];
	AR_CHAR s8Help[128];
	AR_S32 (*ServiceHandler)(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
} IPCTOOL_SERVICE_S;

typedef struct arBINDER_RET_S
{
	AR_CHAR s8Name[14];
	AR_U32 u32Ret;
} BINDER_RET_S;

//The following definition must keep same as ipcam(ipc_ctrl.h)
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
    AR_S32 s32StatTime; /* 1 - 60*/
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
	AR_S32 s32AudioOrVedio;
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
	AR_S32 s32Value;
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

#define IPC_ALG_OSD_MAX_NUM   32
#define IPC_ALG_OSD_STR_MAX_LEN   32
typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32RGB; //RGB888
    AR_CHAR achStr[IPC_ALG_OSD_STR_MAX_LEN];
} IPC_OSD_STRING_S;
typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32W;
    AR_U32 u32H;
    AR_U32 u32RGB; //RGB888
} IPC_OSD_RECT_S;
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


typedef struct
{
	AR_S32 s32Enable;
} IPC_CTRL_MD_S;

typedef struct
{
	AR_S32 s32Enable;
} IPC_CTRL_OD_S;

typedef struct arIPCTOOL_CMD_STRING_S
{
	 AR_CHAR s8Cmd_String[128];
} IPCTOOL_CMD_STRING_S;

AR_S32 AR_IPCTOOL_HandlerViRes(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerViFps(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerViAntiFlicker(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetDenoise3D(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetDenoise3D(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetDenoise2D(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetDenoise2D(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetExpLimit(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetExpLimit(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerViWdr(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVi3aInfo(IPCTOOL_CMD_ARGS_S * pstCmdArgs);

AR_S32 AR_IPCTOOL_HandlerVencBrc(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVencGop(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVencType(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVencFps(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVencQp(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVrecord(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerArecord(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerPlayback(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerFlip(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerMirror(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSnapshot(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVoFullScreen(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerImageScene(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerImageStyle(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetBright(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetSaturation(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetContrast(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetSharpness(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetHue(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetBright(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetSaturation(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetContrast(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetSharpness(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetHue(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerWdt(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetGpio(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetGpio(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSvp(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerReboot(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerPowerOff(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerWifiMode(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerDumpYuv(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerGetVencParam(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerDrawOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerClearOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerSetStreamOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVpssFlip(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerVpssMirror(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerStart(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerStop(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerCreate(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerDestory(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerMd(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_HandlerOd(IPCTOOL_CMD_ARGS_S * pstCmdArgs);
AR_S32 AR_IPCTOOL_Send_Cmd(char* pstCmdString);


IPCTOOL_CMD_ARGS_S * AR_IPCTOOL_GetArgs(const AR_CHAR *ps8Cmd);
AR_S32 AR_IPCTOOL_BinderInit(void);

#endif

