#ifndef __SAMPLE_COMM_H__
#define __SAMPLE_COMM_H__

#include <pthread.h>

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "ar_common.h"
#include "ar_buffer.h"
#include "ar_comm_sys.h"
#include "ar_comm_vb.h"
#include "ar_comm_isp.h"
#include "ar_comm_vi.h"
#include "ar_comm_vo.h"
#include "ar_comm_venc.h"
#include "ar_comm_vdec.h"
#include "ar_comm_vpss.h"
#include "ar_comm_region.h"
#include "ar_comm_adec.h"
#include "ar_comm_aenc.h"
#include "ar_comm_ai.h"
#include "ar_comm_ao.h"
#include "ar_comm_aio.h"
#include "ar_defines.h"
#include "ar_comm_hdmi.h"
#include "ar_combo_dev.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "mpi_ifc_api.h"
#include "ar_math.h"
#include "ar_sns_ctrl.h"
#include "mpi_hdmi.h"
#include "sample_comm_log.h"
#include "hal_vcodec_type_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define tsline() \
do {\
    long ms; \
    time_t s; \
    struct timespec spec; \
    clock_gettime(CLOCK_REALTIME, &spec); \
    s = spec.tv_sec; \
    ms=round(spec.tv_nsec / 1.0e6); \
    if (ms > 999) { \
        s++; \
        ms = 0; \
    } \
    printf("%7ld.%3lds [%s:%d] \n", s, ms, __FUNCTION__, __LINE__); \
} while (0);

//#define logline(fmt, ...) printf("%6ld ms [%s:%d] " fmt "\n", getTimeNowMs(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define logline(fmt, ...) AR_LOG_INFO(APP_TAG_ID, " " fmt, ##__VA_ARGS__)
#define logwarn(fmt, ...) AR_LOG_WARN(APP_TAG_ID, " " fmt, ##__VA_ARGS__)
#define logerr(fmt, ...)  AR_LOG_ERR(APP_TAG_ID, " " fmt, ##__VA_ARGS__)

/*******************************************************
    macro define
*******************************************************/
#define FILE_NAME_LEN               128

#define CHECK_CHN_RET(express,Chn,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s chn %d failed at %s: LINE: %d with %#x!\033[0;39m\n", name, Chn, __FUNCTION__, __LINE__, Ret);\
            fflush(stdout);\
            return Ret;\
        }\
    }while(0)

#define CHECK_RET(express,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)
#define SAMPLE_PIXEL_FORMAT         PIXEL_FORMAT_YVU_SEMIPLANAR_420

#define TLV320_FILE "/dev/tlv320aic31"
#define COLOR_RGB_RED      0xFF0000
#define COLOR_RGB_GREEN    0x00FF00
#define COLOR_RGB_BLUE     0x0000FF
#define COLOR_RGB_BLACK    0x000000
#define COLOR_RGB_YELLOW   0xFFFF00
#define COLOR_RGB_CYN      0x00ffff
#define COLOR_RGB_WHITE    0xffffff

#define SAMPLE_VO_DEV_DHD0 0                  /* VO's device HD0 */
#define SAMPLE_VO_DEV_DHD1 1                  /* VO's device HD1 */
#define SAMPLE_VO_DEV_UHD  SAMPLE_VO_DEV_DHD0 /* VO's ultra HD device:HD0 */
#define SAMPLE_VO_DEV_HD   SAMPLE_VO_DEV_DHD1 /* VO's HD device:HD1 */
#define SAMPLE_VO_LAYER_VHD0 0
#define SAMPLE_VO_LAYER_VHD1 1
#define SAMPLE_VO_LAYER_VHD2 2
#define SAMPLE_VO_LAYER_PIP  SAMPLE_VO_LAYER_VHD2

#define SAMPLE_AUDIO_EXTERN_AI_DEV 0
#define SAMPLE_AUDIO_EXTERN_AO_DEV 0
#define SAMPLE_AUDIO_INNER_AI_DEV 0
#define SAMPLE_AUDIO_INNER_AO_DEV 0
#define SAMPLE_AUDIO_INNER_HDMI_AO_DEV 1

#define SAMPLE_AUDIO_PTNUMPERFRM   480

#define WDR_MAX_PIPE_NUM        4


#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)


#define SAMPLE_PRT(fmt, ...)                     \
    do {\
        ar_always(fmt, ##__VA_ARGS__);                  \
    }while(0)

#define CHECK_NULL_PTR(ptr)\
    do{\
        if(NULL == ptr)\
        {\
            printf("func:%s,line:%d, NULL pointer\n",__FUNCTION__,__LINE__);\
            return AR_FAILURE;\
        }\
    }while(0)


/*******************************************************
    enum define
*******************************************************/

typedef enum arPIC_SIZE_E
{
    PIC_CIF,
    PIC_D1_PAL,    /* 720 * 576 */
    PIC_D1_NTSC,   /* 720 * 480 */
    PIC_720P,	   /* 1280 * 720  */
    PIC_1080P,	   /* 1920 * 1080 */
    PIC_2592x1520,
    PIC_2688x1520,
    PIC_2592x1944,
    PIC_3840x2160,
    PIC_4096x2160,
    PIC_3000x3000,
    PIC_4000x3000,
    PIC_7680x4320,
    PIC_3840x8640,
    PIC_1080P_BIG_PIC,	   /* 992 * 1080 */
    PIC_8K_BIG_PIC,	   /* 4032 * 6048 */
	PIC_2712x1538,
	PIC_1920x2880,
    PIC_BUTT
} PIC_SIZE_E;

typedef enum arSAMPLE_SNS_TYPE_E
{
    SONY_IMX290_MIPI_2M_30FPS_12BIT,
    ISP_VIN_0_MEM_2M_30FPS_12BIT,
    SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1,
    SONY_IMX377_MIPI_8M_30FPS_10BIT,
    SONY_IMX415_MIPI_8M_30FPS_12BIT,
    SONY_IMX415_MIPI_8M_60FPS_12BIT,
    NVP6324_MIPI_2M_30FPS_8BIT,
    TP9930_DVP_SINGLE_CH,
    TP9930_DVP_MULTIPLEX_2CH,
    TP9930_DVP_MULTIPLEX_4CH,
    SONY_IMX307_MIPI_2M_25FPS_12BIT,
    SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1,
    SONY_IMX307_MIPI_2M_25FPS_12BIT_YUV420_SP,
    OV_OS04A10_MIPI_4M_30FPS_12BIT,
    OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1,
    OV_OS04A10_MIPI_4M_30FPS_10BIT,
    ISP_VIN_COMMON_INF,//19
    ISP_VIN_COMMON_INF_120x90,
    ISP_VIN_COMMON_INF_384x288,
    ISP_VIN_COMMON_INF_400x300,
    ISP_VIN_COMMON_INF_640x512,
    ISP_VIN_COMMON_INF_1280x1024,
    ISP_VIN_COMMON_INF_12BIT,
    ISP_VIN_COMMON_INF_120x90_12BIT,
    ISP_VIN_COMMON_INF_384x288_12BIT,
    ISP_VIN_COMMON_INF_400x300_12BIT,
    ISP_VIN_COMMON_INF_640x512_12BIT,
    ISP_VIN_COMMON_INF_1280x1024_12BIT,
    ISP_VIN_COMMON_INF1,
    ISP_VIN_COMMON_INF1_120x90,
    ISP_VIN_COMMON_INF1_384x288,
    ISP_VIN_COMMON_INF1_400x300,
    ISP_VIN_COMMON_INF1_640x512,
    ISP_VIN_COMMON_INF1_1280x1024,
    TIMO256_DVP_256_IR,
    TIMO256AF_DVP_256_IR,
    ELF1_DVP_256_IR,
    PLUG617_DVP_640x512_IR,
    TIMO120_DVP_120x90_IR,
    SAMPLE_SNS_TYPE_BUTT
} SAMPLE_SNS_TYPE_E;


typedef enum arSAMPLE_VO_MODE_E
{
    VO_MODE_1MUX  ,
    VO_MODE_2MUX  ,
    VO_MODE_4MUX  ,
    VO_MODE_8MUX  ,
    VO_MODE_9MUX  ,
    VO_MODE_16MUX ,
    VO_MODE_25MUX ,
    VO_MODE_36MUX ,
    VO_MODE_49MUX ,
    VO_MODE_64MUX ,
    VO_MODE_2X4   ,
    VO_MODE_BUTT
} SAMPLE_VO_MODE_E;

typedef enum arSAMPLE_RC_E
{
    SAMPLE_RC_CBR = 0,
    SAMPLE_RC_VBR,
    SAMPLE_RC_AVBR,
    SAMPLE_RC_QPMAP,
    SAMPLE_RC_FIXQP
} SAMPLE_RC_E;


/*******************************************************
    structure define
*******************************************************/
typedef struct arSAMPLE_VENC_GETSTREAM_PARA_S
{
    AR_BOOL bThreadStart;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    AR_S32  s32Cnt;
} SAMPLE_VENC_GETSTREAM_PARA_S;

typedef struct arSAMPLE_VENC_QPMAP_SENDFRAME_PARA_S
{
    AR_BOOL  bThreadStart;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VENC_CHN VeChn[VENC_MAX_CHN_NUM];
    AR_S32   s32Cnt;
    SIZE_S   stSize;
} SAMPLE_VENC_QPMAP_SENDFRAME_PARA_S;


typedef struct arSAMPLE_VI_DUMP_THREAD_INFO_S
{
    VI_PIPE     ViPipe;
    AR_S32      s32Cnt;
    AR_BOOL     bDump;
    AR_CHAR     aszName[128];
    pthread_t   ThreadId;
} SAMPLE_VI_DUMP_THREAD_INFO_S;

typedef struct arSAMPLE_SENSOR_INFO_S
{
    SAMPLE_SNS_TYPE_E   enSnsType;
    AR_S32              s32SnsId;
    AR_S32              s32BusId;
    combo_dev_t           MipiDev;	
	AR_S32             s32FocusModule;
	AR_S32             mipi_ipi_fre;	
	AR_S32             settle;
} SAMPLE_SENSOR_INFO_S;

typedef struct arSAMPLE_SNAP_INFO_S
{
    AR_BOOL  bSnap;
    AR_BOOL  bDoublePipe;
    VI_PIPE    VideoPipe;
    VI_PIPE    SnapPipe;
    VI_VPSS_MODE_E  enVideoPipeMode;
    VI_VPSS_MODE_E  enSnapPipeMode;
}SAMPLE_SNAP_INFO_S;

typedef struct arSAMPLE_DEV_INFO_S
{
    VI_DEV      ViDev;
    WDR_MODE_E  enWDRMode;
    AR_BOOL     bDvpHighLowByteSwap;
} SAMPLE_DEV_INFO_S;

typedef struct arSAMPLE_PIPE_INFO_S
{
    VI_PIPE         aPipe[4];
    VI_VPSS_MODE_E  enMastPipeMode;
    AR_BOOL         bMultiPipe;
    AR_BOOL         bVcNumCfged;
    AR_U32          u32VCNum[4];
    // u32Width[4] / u32Height[4] / f32Fps[4] are added parameters.
    // They are used for determing the resolution and fps of sensor virtual channel, because
    // some sensors allow that virtual channels has different resolution and fps.
    // For sensors that has no virtual channel, these parameters can be ignored.
    AR_U32          u32Width[4];
    AR_U32          u32Height[4];
	AR_U32 			u32BitWidth[4];
	AR_U32			u32VCDTNum[4];
	AR_U32 			u32enPixFormat[4];
    AR_FLOAT        f32Fps[4];
	AR_U32          dpcm;
	AR_U32          feature_mask;	
} SAMPLE_PIPE_INFO_S;

typedef struct arSAMPLE_CHN_INFO_S
{
    VI_CHN              ViChn;
    PIXEL_FORMAT_E      enPixFormat;
    DYNAMIC_RANGE_E     enDynamicRange;
    VIDEO_FORMAT_E      enVideoFormat;
    COMPRESS_MODE_E     enCompressMode;
    AR_U32              u32Width;
    AR_U32              u32Height;
} SAMPLE_CHN_INFO_S;

typedef struct arSAMPLE_VI_INFO_S
{
    SAMPLE_SENSOR_INFO_S    stSnsInfo;
    SAMPLE_DEV_INFO_S       stDevInfo;
    SAMPLE_PIPE_INFO_S      stPipeInfo;
    SAMPLE_CHN_INFO_S       stChnInfo;
    SAMPLE_SNAP_INFO_S      stSnapInfo;
} SAMPLE_VI_INFO_S;

typedef struct arSAMPLE_VI_CONFIG_S
{
    SAMPLE_VI_INFO_S    astViInfo[VI_MAX_DEV_NUM];
    AR_S32              as32WorkingViId[VI_MAX_DEV_NUM];
    AR_S32              s32WorkingViNum;
} SAMPLE_VI_CONFIG_S;

typedef struct arSAMPLE_VI_FRAME_CONFIG_S
{
    AR_U32                  u32Width;
    AR_U32                  u32Height;
    AR_U32                  u32ByteAlign;
    PIXEL_FORMAT_E          enPixelFormat;
    VIDEO_FORMAT_E          enVideoFormat;
    COMPRESS_MODE_E         enCompressMode;
    DYNAMIC_RANGE_E         enDynamicRange;
} SAMPLE_VI_FRAME_CONFIG_S;

typedef struct arSAMPLE_VI_FRAME_INFO_S
{
    VB_BLK             VbBlk;
    AR_U32             u32Size;
    VIDEO_FRAME_INFO_S stVideoFrameInfo;
} SAMPLE_VI_FRAME_INFO_S;

typedef struct arSAMPLE_VI_FPN_CALIBRATE_INFO_S
{
    AR_U32                  u32Threshold;
    AR_U32                  u32FrameNum;
    PIXEL_FORMAT_E          enPixelFormat;
    COMPRESS_MODE_E         enCompressMode;
} SAMPLE_VI_FPN_CALIBRATE_INFO_S;

typedef struct arSAMPLE_VI_FPN_CORRECTION_INFO_S
{
    ISP_OP_TYPE_E           enOpType;
    AR_U32                  u32Strength;
    PIXEL_FORMAT_E          enPixelFormat;
    COMPRESS_MODE_E         enCompressMode;
    SAMPLE_VI_FRAME_INFO_S  stViFrameInfo;
} SAMPLE_VI_FPN_CORRECTION_INFO_S;

typedef struct tag_SAMPLE_VO_WBC_CONFIG
{
    VO_WBC_SOURCE_TYPE_E    enSourceType;
    DYNAMIC_RANGE_E         enDynamicRange;
    COMPRESS_MODE_E         enCompressMode;
    AR_S32 s32Depth;

    AR_S32                  VoWbc;
    VO_WBC_ATTR_S           stWbcAttr;
    VO_WBC_SOURCE_S         stWbcSource;
    VO_WBC_MODE_E           enWbcMode;

}SAMPLE_VO_WBC_CONFIG;

typedef struct arSAMPLE_COMM_VO_LAYER_CONFIG_S
{
    /* for layer */
    VO_LAYER                VoLayer;
    VO_INTF_SYNC_E          enIntfSync;
    RECT_S                  stDispRect;
    SIZE_S                  stImageSize;
    PIXEL_FORMAT_E          enPixFormat;

    AR_U32                  u32DisBufLen;
    DYNAMIC_RANGE_E         enDstDynamicRange;

    /* for chn */
    SAMPLE_VO_MODE_E        enVoMode;
}SAMPLE_COMM_VO_LAYER_CONFIG_S;

typedef struct arSAMPLE_VO_CONFIG_S
{
    /* for device */
    VO_DEV                  VoDev;
    VO_INTF_TYPE_E          enVoIntfType;
    VO_INTF_SYNC_E          enIntfSync;
    PIC_SIZE_E              enPicSize;
    AR_U32                  u32BgColor;

    /* for layer */
    PIXEL_FORMAT_E          enPixFormat;
    RECT_S                  stDispRect;
    SIZE_S                  stImageSize;
    VO_PART_MODE_E          enVoPartMode;

    AR_U32                  u32DisBufLen;
    DYNAMIC_RANGE_E         enDstDynamicRange;

    /* for chnnel */
    SAMPLE_VO_MODE_E        enVoMode;
} SAMPLE_VO_CONFIG_S;


typedef enum arTHREAD_CONTRL_E
{
    THREAD_CTRL_START,
    THREAD_CTRL_PAUSE,
    THREAD_CTRL_STOP,
}THREAD_CONTRL_E;

typedef enum {
    AR_SAVE_YUV_NONE = 0,
    AR_SAVE_YUV_ONE_FILE,
    AR_SAVE_YUV_MULTI_FILE,
} AR_SAVE_FILE_E;

typedef struct arVDEC_THREAD_PARAM_S
{
    AR_S32 s32ChnId;
    PAYLOAD_TYPE_E enType;
    AR_SAVE_FILE_E s32SaveFile;
    AR_S32 s32ShowUserData;
    AR_CHAR cOutName[128];
    AR_CHAR cFileName[128];
    AR_S32 s32StreamMode;
    AR_S32 s32MilliSec;
    AR_S32 s32MinBufSize;
    AR_S32 s32IntervalTime;
    THREAD_CONTRL_E eThreadCtrl;
    AR_U64  u64PtsInit;
    AR_U64  u64PtsIncrease;
    AR_U32  u32Fps;
    AR_BOOL bCircleSend;
    AR_U32  discardMode;
    AR_U32  numFrameToDecode;
}VDEC_THREAD_PARAM_S;

typedef struct arSAMPLE_VDEC_BUF
{
    AR_U32  u32PicBufSize;
    AR_U32  u32TmvBufSize;
    AR_BOOL bPicBufAlloc;
    AR_BOOL bTmvBufAlloc;
}SAMPLE_VDEC_BUF;


typedef struct arSAMPLE_VDEC_VIDEO_ATTR
{
    VIDEO_DEC_MODE_E enDecMode;
    AR_U32           u32RefFrameNum;
    AR_U32           u32CmdQueueDepth;
    DATA_BITWIDTH_E  enBitWidth;
    ENUM_VIDEO_OUTPUT_ORDER enOutputOrder;
}SAMPLE_VDEC_VIDEO_ATTR;

typedef struct arSAMPLE_VDEC_PICTURE_ATTR
{
    PIXEL_FORMAT_E enPixelFormat;
    AR_U32         u32Alpha;
}SAMPLE_VDEC_PICTURE_ATTR;


typedef struct arSAMPLE_VDEC_ATTR
{
    PAYLOAD_TYPE_E enType;
    VIDEO_MODE_E   enMode;
    AR_S32 s32ChnId;
    AR_U32 u32Width;
    AR_U32 u32Height;
    AR_U32 u32ScaledWidth;
    AR_U32 u32ScaledHeight;
    AR_U32 u32FrameBufCnt;
    AR_U32 u32DisplayFrameNum;
    VB_SOURCE_E  vbSource;
    union
    {
        SAMPLE_VDEC_VIDEO_ATTR stSapmleVdecVideo;      /* structure with video ( h265/h264) */
        SAMPLE_VDEC_PICTURE_ATTR stSapmleVdecPicture; /* structure with picture (jpeg/mjpeg )*/
    };
}SAMPLE_VDEC_ATTR;

typedef struct arSAMPLE_VB_BASE_INFO_S
{
    PIXEL_FORMAT_E      enPixelFormat;
    AR_U32              u32Width;
    AR_U32              u32Height;
    AR_U32              u32Align;
    COMPRESS_MODE_E     enCompressMode;
}SAMPLE_VB_BASE_INFO_S;

typedef struct arSAMPLE_VB_CAL_CONFIG_S
{
    AR_U32 u32VBSize;

    AR_U32 u32HeadStride;
    AR_U32 u32HeadSize;
    AR_U32 u32HeadYSize;

    AR_U32 u32MainStride;
    AR_U32 u32MainSize;
    AR_U32 u32MainYSize;

    AR_U32 u32ExtStride;
    AR_U32 u32ExtYSize;
}SAMPLE_VB_CAL_CONFIG_S;

/*******************************************************
    function announce
*******************************************************/

AR_VOID * SAMPLE_SYS_IOMmap(AR_U64 u64PhyAddr, AR_U32 u32Size);
AR_S32 SAMPLE_SYS_Munmap(AR_VOID* pVirAddr, AR_U32 u32Size);
AR_S32 SAMPLE_SYS_SetReg(AR_U64 u64Addr, AR_U32 u32Value);
AR_S32 SAMPLE_SYS_GetReg(AR_U64 u64Addr, AR_U32 *pu32Value);

AR_S32 SAMPLE_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize);
AR_S32 SAMPLE_COMM_SYS_MemConfig(AR_VOID);
AR_VOID SAMPLE_COMM_SYS_Exit(void);
AR_S32 SAMPLE_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig);
AR_S32 SAMPLE_COMM_SYS_InitWithVbSupplement(VB_CONFIG_S* pstVbConf, AR_U32 u32SupplementConfig);

AR_S32 SAMPLE_COMM_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VI_UnBind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_VI_UnBind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_VPSS_Bind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
AR_S32 SAMPLE_COMM_VPSS_UnBind_AVS(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AVS_GRP AvsGrp, AVS_PIPE AvsPipe);
AR_S32 SAMPLE_COMM_VPSS_Bind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VPSS_UnBind_VO(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VPSS_Bind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_VPSS_UnBind_VENC(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_AVS_Bind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
AR_S32 SAMPLE_COMM_AVS_UnBind_AVS(AVS_GRP AvsSrcGrp, AVS_CHN AvsSrcChn, AVS_GRP AvsDestGrp, AVS_CHN AvsDestChn);
AR_S32 SAMPLE_COMM_AVS_Bind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_AVS_UnBind_VPSS(AVS_GRP AvsGrp, AVS_CHN AvsChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_AVS_Bind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_AVS_UnBind_VENC(AVS_GRP AvsGrp, AVS_CHN AvsChn, VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_AVS_Bind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_AVS_UnBind_VO(AVS_GRP AvsGrp, AVS_CHN AvsChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VDEC_Bind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_VDEC_UnBind_VPSS(VDEC_CHN VdecChn, VPSS_GRP VpssGrp);
AR_S32 SAMPLE_COMM_VO_Bind_VO(VO_LAYER  SrcVoLayer, VO_CHN SrcVoChn, VO_LAYER DstVoLayer, VO_CHN DstVoChn);
AR_S32 SAMPLE_COMM_VO_UnBind_VO(VO_LAYER DstVoLayer, VO_CHN DstVoChn);
AR_S32 SAMPLE_COMM_VDEC_Bind_VO(VDEC_CHN VdecChn,VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VDEC_UnBind_VO(VDEC_CHN VdecChn,VO_LAYER VoLayer, VO_CHN VoChn);

AR_VOID SAMPLE_COMM_ISP_Stop(ISP_DEV IspDev);
AR_VOID SAMPLE_COMM_All_ISP_Stop(AR_VOID);
AR_S32 SAMPLE_COMM_ISP_Run(ISP_DEV IspDev);
AR_S32 SAMPLE_COMM_ISP_BindSns(ISP_DEV IspDev, AR_U32 u32SnsId, SAMPLE_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev);
AR_S32 SAMPLE_COMM_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, AR_U32 u32SnsId);
AR_S32 SAMPLE_COMM_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev);
AR_S32 SAMPLE_COMM_ISP_GetIspAttrBySns(SAMPLE_SNS_TYPE_E enSnsType, AR_U32 u32Width,
    AR_U32 u32Height, AR_FLOAT f32Fps, ISP_PUB_ATTR_S* pstPubAttr);
AR_S32 SAMPLE_COMM_VI_BindSensor(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx);
AR_VOID SAMPLE_COMM_VI_UnbindSensor(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx);

AR_S32 SAMPLE_COMM_VI_GetWDRModeBySensor(SAMPLE_SNS_TYPE_E enMode, WDR_MODE_E* penWDRMode);
AR_S32 SAMPLE_COMM_VI_GetPipeBySensor(SAMPLE_SNS_TYPE_E enMode, SAMPLE_PIPE_INFO_S* pstPipeInfo);
AR_S32 SAMPLE_COMM_VI_GetSizeBySensor(SAMPLE_SNS_TYPE_E enMode, PIC_SIZE_E* penSize);
AR_S32 SAMPLE_COMM_VI_GetFrameRateBySensor(SAMPLE_SNS_TYPE_E enMode, AR_U32* pu32FrameRate);
AR_S32 SAMPLE_COMM_VI_StartDev(SAMPLE_VI_INFO_S* pstViInfo);
AR_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S* pstCapRect, SIZE_S* pstTarSize, SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_StartVi_Step1(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_StartVi_Step2(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32 SAMPLE_COMM_VI_GetDevAttrBySns(VI_DEV ViDev, SAMPLE_SNS_TYPE_E enSnsType, AR_BOOL bDvpByteSwap, VI_DEV_ATTR_S *pstViDevAttr);
AR_VOID SAMPLE_COMM_VI_GetSensorInfo(SAMPLE_VI_CONFIG_S* pstViConfig);


combo_dev_t SAMPLE_COMM_VI_GetComboDevBySensor(SAMPLE_SNS_TYPE_E enMode, AR_S32 s32SnsIdx);
AR_S32 SAMPLE_COMM_VI_SaveRaw(VIDEO_FRAME_S* pVBuf, AR_U32 u32Nbit, FILE* pfd);
AR_VOID* SAMPLE_COMM_VI_DumpRaw(AR_VOID *arg);
AR_S32 SAMPLE_COMM_VI_StartDumpRawThread(VI_PIPE ViPipe, AR_S32 s32Cnt, const AR_CHAR *pzsName);
AR_S32 SAMPLE_COMM_VI_StopDumpRawThread(AR_VOID);
AR_S32 SAMPLE_COMM_VI_SetParam(SAMPLE_VI_CONFIG_S* pstViConfig);
AR_S32  SAMPLE_COMM_VI_SwitchMode_StopVI(SAMPLE_VI_CONFIG_S* pstViConfigSrc);
AR_S32  SAMPLE_COMM_VI_SwitchMode(SAMPLE_VI_CONFIG_S* pstViConfigDes);

AR_S32 SAMPLE_COMM_VI_FpnCalibrateConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CALIBRATE_INFO_S *pstViFpnCalibrateInfo);
AR_S32 SAMPLE_COMM_VI_FpnCorrectionConfig(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);
AR_S32 SAMPLE_COMM_VI_DisableFpnCorrection(VI_PIPE ViPipe, SAMPLE_VI_FPN_CORRECTION_INFO_S *pstViFpnCorrectionInfo);

AR_S32 SAMPLE_COMM_VI_Load_UserPic(const char *pszYuvFile, VI_USERPIC_ATTR_S *pstUsrPic, SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo);
AR_VOID SAMPLE_COMM_VI_Release_UserPic(SAMPLE_VI_FRAME_INFO_S *pstViFrameInfo);

AR_S32 SAMPLE_COMM_VI_StartSingleViPipe(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx);
AR_S32 SAMPLE_COMM_VI_StopSingleViPipe(VI_PIPE ViPipe);
AR_S32 SAMPLE_COMM_VI_StartSingleIsp(SAMPLE_VI_INFO_S* pstViInfo, AR_S32 s32PipeIdx);
AR_S32 SAMPLE_COMM_VI_StopDev(SAMPLE_VI_INFO_S* pstViInfo);
AR_S32 SAMPLE_COMM_VI_BindPipeDev(SAMPLE_VI_INFO_S* pstViInfo);
AR_S32 SAMPLE_COMM_VI_StopMIPI(SAMPLE_VI_CONFIG_S* pstViConfig);

AR_S32 SAMPLE_COMM_VPSS_Start(VPSS_GRP VpssGrp, AR_BOOL* pabChnEnable, VPSS_GRP_ATTR_S* pstVpssGrpAttr, VPSS_CHN_ATTR_S* pastVpssChnAttr);
AR_S32 SAMPLE_COMM_VPSS_Stop(VPSS_GRP VpssGrp, AR_BOOL* pabChnEnable);


AR_S32 SAMPLE_COMM_VO_GetWH(VO_INTF_SYNC_E enIntfSync, AR_U32* pu32W, AR_U32* pu32H, AR_U32* pu32Frm);
AR_S32 SAMPLE_COMM_VO_MemConfig(VO_DEV VoDev, AR_CHAR* pcMmzName);
AR_S32 SAMPLE_COMM_VO_StartDev(VO_DEV VoDev, VO_PUB_ATTR_S* pstPubAttr);
AR_S32 SAMPLE_COMM_VO_StopDev(VO_DEV VoDev);
AR_S32 SAMPLE_COMM_VO_StartLayer(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S* pstLayerAttr);
AR_S32 SAMPLE_COMM_VO_StopLayer(VO_LAYER VoLayer);
AR_S32 SAMPLE_COMM_VO_StartChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
AR_S32 SAMPLE_COMM_VO_StopChn(VO_LAYER VoLayer, SAMPLE_VO_MODE_E enMode);
AR_S32 SAMPLE_COMM_VO_StartWBC(SAMPLE_VO_WBC_CONFIG * pWbcConfig);
AR_S32 SAMPLE_COMM_VO_StopWBC(SAMPLE_VO_WBC_CONFIG * pWbcConfig);
AR_S32 SAMPLE_COMM_VO_GetDefWBCConfig(SAMPLE_VO_WBC_CONFIG * pWbcConfig);
AR_S32 SAMPLE_COMM_VO_BindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
AR_S32 SAMPLE_COMM_VO_UnBindVpss(VO_LAYER VoLayer, VO_CHN VoChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
AR_S32 SAMPLE_COMM_VO_BindVi(VO_LAYER VoLayer, VO_CHN VoChn, VI_CHN ViChn);
AR_S32 SAMPLE_COMM_VO_UnBindVi(VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_COMM_VO_HdmiStart(VO_INTF_SYNC_E enIntfSync);
AR_S32 SAMPLE_COMM_VO_HdmiStartByDyRg(VO_INTF_SYNC_E enIntfSync, DYNAMIC_RANGE_E enDyRg);
AR_S32 SAMPLE_COMM_VO_HdmiStop(AR_VOID);
AR_S32 SAMPLE_COMM_VO_GetDefConfig(SAMPLE_VO_CONFIG_S *pstVoConfig);
AR_S32 SAMPLE_COMM_VO_StopVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
AR_S32 SAMPLE_COMM_VO_StartVO(SAMPLE_VO_CONFIG_S *pstVoConfig);
AR_S32 SAMPLE_COMM_VO_StopPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
AR_S32 SAMPLE_COMM_VO_StartPIP(SAMPLE_VO_CONFIG_S *pstVoConfig);
AR_S32 SAMPLE_COMM_VO_GetDefLayerConfig(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig);
AR_S32 SAMPLE_COMM_VO_StartLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig);
AR_S32 SAMPLE_COMM_VO_StopLayerChn(SAMPLE_COMM_VO_LAYER_CONFIG_S * pstVoLayerConfig);
AR_VOID SAMPLE_COMM_VO_Exit(AR_VOID);

AR_S32 SAMPLE_COMM_VENC_MemConfig(AR_VOID);
AR_S32 SAMPLE_COMM_VENC_Creat(VENC_CHN VencChn, PAYLOAD_TYPE_E enType,  PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, AR_U32  u32Profile, VENC_GOP_ATTR_S *pstGopAttr);
AR_S32 SAMPLE_COMM_VENC_Start(VENC_CHN VencChn, PAYLOAD_TYPE_E enType, PIC_SIZE_E enSize, SAMPLE_RC_E enRcMode, AR_U32 u32Profile, VENC_GOP_ATTR_S *pstGopAttr);
AR_S32 SAMPLE_COMM_VENC_Stop(VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, AR_BOOL bSupportDCF);
AR_S32 SAMPLE_COMM_VENC_SnapProcess(VENC_CHN VencChn, AR_U32 SnapCnt, AR_BOOL bSaveJpg, AR_BOOL bSaveThm);
AR_S32 SAMPLE_COMM_VENC_SaveJpeg(VENC_CHN VencChn, AR_U32 SnapCnt);
AR_S32 SAMPLE_COMM_VENC_SnapStop(VENC_CHN VencChn);
AR_S32 SAMPLE_COMM_VENC_StartGetStream(VENC_CHN VeChn[],AR_S32 s32Cnt);
AR_S32 SAMPLE_COMM_VENC_StopGetStream(void);
AR_S32 SAMPLE_COMM_VENC_StartGetStream_Svc_t(AR_S32 s32Cnt);
AR_S32 SAMPLE_COMM_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr);
AR_S32 SAMPLE_COMM_VENC_QpmapSendFrame(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,VENC_CHN VeChn[],AR_S32 s32Cnt,SIZE_S stSize);
AR_S32 SAMPLE_COMM_VENC_StopSendQpmapFrame(void);

AR_S32 SAMPLE_COMM_REGION_Create(AR_S32 HandleNum,RGN_TYPE_E enType);
AR_S32 SAMPLE_COMM_REGION_Destroy(AR_S32 HandleNum,RGN_TYPE_E enType);
AR_S32 SAMPLE_COMM_REGION_AttachToChn(AR_S32 HandleNum,RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
AR_S32 SAMPLE_COMM_REGION_DetachFrmChn(AR_S32 HandleNum,RGN_TYPE_E enType, MPP_CHN_S *pstMppChn);
AR_S32 SAMPLE_COMM_REGION_SetBitMap(RGN_HANDLE Handle);
AR_S32 SAMPLE_COMM_REGION_GetUpCanvas(RGN_HANDLE Handle);
AR_S32 SAMPLE_COMM_REGION_GetMinHandle(RGN_TYPE_E enType);

AR_S32 SAMPLE_COMM_VDEC_InitVBPool(AR_U32 ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
AR_VOID SAMPLE_COMM_VDEC_ExitVBPool(SAMPLE_VDEC_ATTR *pastSampleVdec);
AR_VOID SAMPLE_COMM_VDEC_CmdCtrl(AR_S32 s32ChnNum,VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
AR_VOID SAMPLE_COMM_VDEC_StartSendStream(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
AR_VOID SAMPLE_COMM_VDEC_StopSendStream(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecSend, pthread_t *pVdecThread);
AR_VOID* SAMPLE_COMM_VDEC_SendStream(AR_VOID *pArgs);
AR_VOID SAMPLE_COMM_VDEC_StartGetPic(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
AR_VOID SAMPLE_COMM_VDEC_StopGetPic(AR_S32 s32ChnNum, VDEC_THREAD_PARAM_S *pstVdecGet, pthread_t *pVdecThread);
AR_S32 SAMPLE_COMM_VDEC_Start(AR_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
AR_S32 SAMPLE_COMM_VDEC_Stop(AR_S32 s32ChnNum, SAMPLE_VDEC_ATTR *pastSampleVdec);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __SAMPLE_COMMON_H__ */
