/**
 * \file
 * \brief 描述视频解码相关的通用数据结构
 */

#ifndef  __AR_COMM_VDEC_H__
#define  __AR_COMM_VDEC_H__
#include "hal_type.h"
#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"
#include "ar_comm_vb.h"
#include "ar_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
/********************************Macro Definition********************************/

#define AR_IO_BLOCK   AR_TRUE
#define AR_IO_NOBLOCK AR_FALSE


typedef enum arVIDEO_MODE_E
{
    VIDEO_MODE_STREAM = 0, /* send by stream */
    VIDEO_MODE_FRAME     , /* send by frame  */
    VIDEO_MODE_COMPAT    , /* One frame supports multiple packets sending. The current frame is considered to end when bEndOfFrame is equal to AR_TRUE */
    VIDEO_MODE_BUTT
}VIDEO_MODE_E;


typedef struct arVDEC_ATTR_VIDEO_S
{
    AR_U32       u32RefFrameNum;       /* RW, Range: [0, 16]; reference frame num. */
    AR_BOOL      bTemporalMvpEnable;   /* RW; specifies whether temporal motion vector predictors can be used for inter prediction */
    AR_U32       u32TmvBufSize;        /* RW; tmv buffer size(Byte) */
    AR_U32       u32CmdQueueDepth;     /* RW; Range:[1, 4]; 设置基于帧级的并行，编码器最多可以同时编码的帧数 note:新加参数,仅对HEVC和AVC有效*/
}VDEC_ATTR_VIDEO_S;


typedef struct arVDEC_CHN_ATTR_S
{
    PAYLOAD_TYPE_E enType;              /* RW; video type to be decoded   */
    // attention, enMode currently only support frame mode
    VIDEO_MODE_E   enMode;              /* RW; send by stream or by frame */
    AR_U32         u32PicWidth;         /* RW; max pic width */
    AR_U32         u32PicHeight;        /* RW; max pic height */
    AR_U32         u32StreamBufSize;    /* RW; stream buffer size(Byte) */
    AR_U32         u32FrameBufSize;     /* RW; frame buffer size(Byte) */
    AR_U32         u32FrameBufCnt;
    union
    {
        VDEC_ATTR_VIDEO_S stVdecVideoAttr;    /* structure with video ( h264/h265) */
    };
}VDEC_CHN_ATTR_S;


typedef struct arVDEC_STREAM_S
{
    AR_U32  u32Len;              /* W; stream len */
    AR_U64  u64PTS;              /* W; time stamp */
    AR_BOOL bEndOfFrame;         /* W; is the end of a frame */
    AR_BOOL bEndOfStream;        /* W; is the end of all stream */
    AR_BOOL bDisplay;            /* W; is the current frame displayed. only valid by VIDEO_MODE_FRAME */
    AR_U8* ATTRIBUTE pu8Addr;    /* W; stream address */
}VDEC_STREAM_S;

typedef struct arVDEC_USERDATA_S
{
    AR_U64           u64PhyAddr;  /* R; userdata data phy address */
    AR_U32           u32Len;        /* R; userdata data len */
    AR_BOOL          bValid;        /* R; is valid? */
    AR_U8* ATTRIBUTE pu8Addr;       /* R; userdata data vir address */
}VDEC_USERDATA_S;


typedef struct ar_VDEC_DECODE_ERROR_S
{
    AR_S32 s32FormatErr;              /* R; format error. eg: do not support filed */
    AR_S32 s32PicSizeErrSet;          /* R; picture width or height is larger than chnnel width or height*/
    AR_S32 s32StreamUnsprt;           /* R; unsupport the stream specification */
    AR_S32 s32PackErr;                /* R; stream package error */
    AR_S32 s32PrtclNumErrSet;         /* R; protocol num is not enough. eg: slice, pps, sps */
    AR_S32 s32RefErrSet;              /* R; refrence num is not enough */
    AR_S32 s32PicBufSizeErrSet;       /* R; the buffer size of picture is not enough */
    AR_S32 s32StreamSizeOver;         /* R; the stream size is too big and and force discard stream */
    AR_S32 s32VdecStreamNotRelease;   /* R; the stream not released for too long time */
}VDEC_DECODE_ERROR_S;


typedef struct arVDEC_CHN_STATUS_S
{
    PAYLOAD_TYPE_E enType;               /* R; video type to be decoded */
    // attention, u32LeftStreamBytes currently not used
    AR_U32  u32LeftStreamBytes;          /* R; left stream bytes waiting for decode */
    AR_U32  u32LeftStreamFrames;         /* R; left frames waiting for decode,only valid for VIDEO_MODE_FRAME*/
    AR_U32  u32LeftPics;                 /* R; pics waiting for output */
    AR_BOOL bStartRecvStream;            /* R; had started recv stream? */
    AR_U32  u32RecvStreamFrames;         /* R; how many frames of stream has been received. valid when send by frame. */
    AR_U32  u32DecodeStreamFrames;       /* R; how many frames of stream has been decoded. valid when send by frame. */
    // attention, stVdecDecErr currently not used
    VDEC_DECODE_ERROR_S stVdecDecErr;    /* R; information about decode error */
}VDEC_CHN_STATUS_S;



typedef enum arVIDEO_DEC_MODE_E
{
    VIDEO_DEC_MODE_IPB = 0,
    VIDEO_DEC_MODE_IP,
    VIDEO_DEC_MODE_I,
    VIDEO_DEC_MODE_BUTT
}VIDEO_DEC_MODE_E;


typedef enum arVIDEO_OUTPUT_ORDER_E
{
    VIDEO_OUTPUT_ORDER_DISP = 0,
    VIDEO_OUTPUT_ORDER_DEC,
    VIDEO_OUTPUT_ORDER_BUTT
}VIDEO_OUTPUT_ORDER_E;

/**
 * @note It sets the mode of error conceal.
 */
typedef enum {
    VIDEO_DEC_ERRCONCEAL_MODE_OFF         = 0, /**< Conceal off */
    VIDEO_DEC_ERRCONCEALMODE_INTRA_ONLY   = 1, /**< Intra conceal in intra-picture, inter-picture */
    VIDEO_DEC_ERRCONCEAL_MODE_INTRA_INTER = 2  /**< Intra conceal in intra-picture, Inter conceal in inter-picture */
} VIDEO_DEC_ERRCONCEAL_MODE;

/**
 * @note It sets the unit of error conceal
 */
typedef enum {
    VIDEO_DEC_ERRCONCEAL_UNIT_PICTURE    = 0, /**< Picture-level error conceal */
    VIDEO_DEC_ERRCONCEAL_UNIT_SLICE_TILE = 1, /**< Slice/Tile-level error conceal */
    VIDEO_DEC_ERRCONCEAL_UNIT_BLOCK_ROW  = 2, /**< Block-Row-level error conceal */
    VIDEO_DEC_ERRCONCEAL_UNIT_BLOCK      = 3  /**< Block-level error conceal */
} VIDEO_DEC_ERRCONCEAL_UNIT;

typedef struct arVDEC_PARAM_VIDEO_S
{
    // attention, s32ErrThreshold currently not used
    AR_S32               s32ErrThreshold;      /* RW, Range: [0, 100]; threshold for stream error process, 0: discard with any error, 100 : keep data with any error */
    VIDEO_DEC_MODE_E     enDecMode;            /* RW; decode mode , 0: deocde IPB frames, 1: only decode I frame & P frame , 2: only decode I frame */
    VIDEO_OUTPUT_ORDER_E enOutputOrder;        /* RW; frames output order ,0: the same with display order , 1: the same width decoder order */
    // attention, enCompressMode currently not used
    COMPRESS_MODE_E      enCompressMode;       /* RW; compress mode */
    // attention, enVideoFormat currently not used
    VIDEO_FORMAT_E       enVideoFormat;        /* RW; video format */
    // attention, new added for video decoder used
    VIDEO_DEC_ERRCONCEAL_MODE    enErrConcealMode; /* RW; error conceal mode */
    // attention, new added for video decoder used
    VIDEO_DEC_ERRCONCEAL_UNIT    enErrConcealUnit; /* RW; error conceal mode */
}VDEC_PARAM_VIDEO_S;


typedef struct arVDEC_PARAM_PICTURE_S
{
    PIXEL_FORMAT_E    enPixelFormat;    /* RW; out put pixel format */
    AR_U32            u32Alpha;         /* RW, Range: [0, 255]; value 0 is transparent.
                                                      [0 ,127]   is deemed to transparent when enPixelFormat is ARGB1555 or ABGR1555
                                                      [128 ,256] is deemed to non-transparent when enPixelFormat is ARGB1555 or ABGR1555 */
}VDEC_PARAM_PICTURE_S;


typedef struct arVDEC_CHN_PARAM_S
{
    PAYLOAD_TYPE_E enType;              /* RW; video type to be decoded   */
    AR_BOOL        bEnableUserData;     /* RW, Range: [0, 1]; enable userdata or not */
    AR_BOOL        bEnableVbCache;      /* RW, Range: [0, 1]; enable video frame vb cache or not */
    AR_U32         u32DisplayFrameNum;  /* RW, Range: [0, 16]; display frame num */
    AR_U32         u32ScaledWidth;      /* RW; scaled pic width */
    AR_U32         u32ScaledHeight;     /* RW; scaled pic height */
    AR_U32         u32MiniBufMode;      /* RW, Range: [0, 1]; Mini MMZ & VB Buffer of current channel, VB Source should be Private mode */

    union
    {
        VDEC_PARAM_VIDEO_S   stVdecVideoParam;   /* structure with video ( h265/h264) */
        VDEC_PARAM_PICTURE_S stVdecPictureParam; /* structure with picture (jpeg/mjpeg )*/
    };
}VDEC_CHN_PARAM_S;



typedef struct arH264_PRTCL_PARAM_S
{
    AR_S32  s32MaxSliceNum;            /* RW; max slice num support */
    AR_S32  s32MaxSpsNum;              /* RW; max sps num support */
    AR_S32  s32MaxPpsNum;              /* RW; max pps num support */
}H264_PRTCL_PARAM_S;

typedef struct arH265_PRTCL_PARAM_S
{
    AR_S32  s32MaxSliceSegmentNum;     /* RW; max slice segmnet num support */
    AR_S32  s32MaxVpsNum;              /* RW; max vps num support */
    AR_S32  s32MaxSpsNum;              /* RW; max sps num support */
    AR_S32  s32MaxPpsNum;              /* RW; max pps num support */
}H265_PRTCL_PARAM_S;

typedef struct arVDEC_PRTCL_PARAM_S
{
    PAYLOAD_TYPE_E enType;                      /* RW; video type to be decoded, only h264 and h265 supported*/
    union
    {
        H264_PRTCL_PARAM_S stH264PrtclParam;    /* protocol param structure for h264 */
        H265_PRTCL_PARAM_S stH265PrtclParam;    /* protocol param structure for h265 */
    };
}VDEC_PRTCL_PARAM_S;


typedef struct arVDEC_CHN_POOL_S
{
    VB_POOL hPicVbPool;         /* RW;  vb pool id for pic buffer */
    VB_POOL hTmvVbPool;         /* RW;  vb pool id for tmv buffer */
}VDEC_CHN_POOL_S;


typedef enum arVDEC_EVNT_E
{
    VDEC_EVNT_STREAM_ERR = 1,
    VDEC_EVNT_UNSUPPORT,
    VDEC_EVNT_OVER_REFTHR,
    VDEC_EVNT_REF_NUM_OVER,
    VDEC_EVNT_SLICE_NUM_OVER,
    VDEC_EVNT_SPS_NUM_OVER,
    VDEC_EVNT_PPS_NUM_OVER,
    VDEC_EVNT_PICBUF_SIZE_ERR,
    VDEC_EVNT_SIZE_OVER,
    VDEC_EVNT_IMG_SIZE_CHANGE,
    VDEC_EVNT_VPS_NUM_OVER,
    VDEC_EVNT_BUTT
} VDEC_EVNT_E;

typedef struct arVDEC_VIDEO_MOD_PARAM_S
{
    AR_U32  u32MaxPicWidth;
    AR_U32  u32MaxPicHeight;
    AR_U32  u32MaxSliceNum;
    AR_U32  u32VdhMsgNum;
    AR_U32  u32VdhBinSize;
    AR_U32  u32VdhExtMemLevel;
    AR_U32  u32CoreClock;           /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note: share with h265, 根据解码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 */
    AR_U32  u32BpuClock;            /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note: share with h265, 比core低一档或者相等 */
} VDEC_VIDEO_MOD_PARAM_S;

typedef enum arVDEC_CAPACITY_STRATEGY_E {
    VDEC_CAPACITY_STRATEGY_BY_MOD = 0,
    VDEC_CAPACITY_STRATEGY_BY_CHN = 1,
    VDEC_CAPACITY_STRATEGY_BUTT
} VDEC_CAPACITY_STRATEGY_E;

typedef struct arVDEC_PICTURE_MOD_PARAM_S {
    AR_U32 u32MaxPicWidth;
    AR_U32 u32MaxPicHeight;
    AR_BOOL bSupportProgressive;
    AR_BOOL bDynamicAllocate;
    VDEC_CAPACITY_STRATEGY_E enCapStrategy;
    AR_U32  u32CoreClock;               /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666} Mhz note: 根据解码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 */
} VDEC_PICTURE_MOD_PARAM_S;

/* param of each queue size in vdec module*/
typedef struct arVDEC_MOD_EVENT_S
{
    AR_U32 u32VdecIrqQueueSize;    /* RW; Range:[2,16]; size of vdec irq message queue size, store hardware irq command */
    AR_U32 u32VdecTaskQueueSize;   /* RW; Range:[2,16]; size of vdec cmd message queue size, store app command, such as start/stop */
    AR_U32 u32VdecDoneQueueSize;   /* RW; Range:[2,16]; size of vdec done message queue size, store decoded frame of all channel */
    AR_U32 u32VdecOutQueueSize;    /* RW; Range:[2,16]; size of vdec output message queue size, store decoded frame of one channel */
    AR_U32 u32VdecEventQueueSize;  /* RW; Range:[6,16]; size of vdec total message queue size, irq + task */
} VDEC_MOD_EVENT_S;

typedef struct arVDEC_MOD_PARAM_S
{
    VB_SOURCE_E              enVdecVBSource;    /* RW, Range: [1, 3];  frame buffer mode  */
    AR_U32                   u32MiniBufMode;    /* RW, Range: [0, 1];  stream buffer mode */
    AR_U32                   u32ParallelMode;   /* RW, Range: [0, 1];  VDH working mode   */
    VDEC_MOD_EVENT_S         stEventModParam;
    VDEC_VIDEO_MOD_PARAM_S   stVideoModParam;
    VDEC_PICTURE_MOD_PARAM_S stPictureModParam;
} VDEC_MOD_PARAM_S;

/************************************************************************************************************************/

/* invlalid channel ID */
#define AR_ERR_VDEC_INVALID_CHNID     AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AR_ERR_VDEC_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
/* channel exists */
#define AR_ERR_VDEC_EXIST             AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
/* using a NULL point */
#define AR_ERR_VDEC_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AR_ERR_VDEC_NOT_CONFIG        AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AR_ERR_VDEC_NOT_SUPPORT      AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change stati attribute */
#define AR_ERR_VDEC_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
/* the channle is not existed  */
#define AR_ERR_VDEC_UNEXIST           AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
/* failure caused by malloc memory */
#define AR_ERR_VDEC_NOMEM             AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
/* failure caused by malloc buffer */
#define AR_ERR_VDEC_NOBUF             AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
/* no data in buffer */
#define AR_ERR_VDEC_BUF_EMPTY         AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)
/* no buffer for new data */
#define AR_ERR_VDEC_BUF_FULL          AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)
/* system is not ready,had not initialed or loaded*/
#define AR_ERR_VDEC_SYS_NOTREADY      AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
/*system busy*/
#define AR_ERR_VDEC_BUSY              AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)

/* bad address,  eg. used for copy_from_user & copy_to_user   */
#define AR_ERR_VDEC_BADADDR              AR_MPP_DEF_ERR(AR_ID_VDEC, HAL_ERR_LEVEL_ERROR, HAL_ERR_BADADDR)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef  __AR_COMM_VDEC_H__ */

