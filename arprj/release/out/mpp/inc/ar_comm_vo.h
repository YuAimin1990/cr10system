/**
 * \file
 * \brief 描述视频输出相关的通用数据结构
 */

#ifndef __AR_COMM_VO_H__
#define __AR_COMM_VO_H__

#include "hal_vin_log.h"
#include "hal_type.h"
#include "ar_common.h"
#include "ar_comm_video.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/********************************Macro Definition********************************/
/** \addtogroup      MPI_VO */
/** @{ */  /** <!-- [MPI_VO] */

/** dsi最大lane数量 */
#define   MAX_DSI_LANE_NUM           4

/* VO video output interface type */
#define VO_INTF_CVBS        (0x01L<<0)
#define VO_INTF_YPBPR       (0x01L<<1)
#define VO_INTF_VGA         (0x01L<<2)
#define VO_INTF_BT656       (0x01L<<3)
#define VO_INTF_BT1120      (0x01L<<4)
#define VO_INTF_HDMI        (0x01L<<5)
#define VO_INTF_LCD         (0x01L<<6)
#define VO_INTF_BT656_H     (0x01L<<7)
#define VO_INTF_BT656_L     (0x01L<<8)
#define VO_INTF_LCD_6BIT    (0x01L<<9)
#define VO_INTF_LCD_8BIT    (0x01L<<10)
#define VO_INTF_LCD_16BIT   (0x01L<<11)
#define VO_INTF_LCD_18BIT   (0x01L<<12)
#define VO_INTF_LCD_24BIT   (0x01L<<13)
#define VO_INTF_MIPI        (0x01L<<14)
#define VO_INTF_MIPI_SLAVE  (0x01L<<15)
/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_VO */
/** @{ */  /** <!-- [MPI_VO] */
typedef enum arEN_VOU_ERR_CODE_E
{
    EN_ERR_VO_DEV_NOT_CONFIG      = 0x40,
    EN_ERR_VO_DEV_NOT_ENABLE      = 0x41,
    EN_ERR_VO_DEV_HAS_ENABLED     = 0x42,
    EN_ERR_VO_DEV_HAS_BINDED      = 0x43,
    EN_ERR_VO_DEV_NOT_BINDED      = 0x44,

    ERR_VO_NOT_ENABLE             = 0x45,
    ERR_VO_NOT_DISABLE            = 0x46,
    ERR_VO_NOT_CONFIG             = 0x47,

    ERR_VO_CHN_NOT_DISABLE        = 0x48,
    ERR_VO_CHN_NOT_ENABLE         = 0x49,
    ERR_VO_CHN_NOT_CONFIG         = 0x4a,
    ERR_VO_CHN_NOT_ALLOC          = 0x4b,

    ERR_VO_CCD_INVALID_PAT        = 0x4c,
    ERR_VO_CCD_INVALID_POS        = 0x4d,

    ERR_VO_WAIT_TIMEOUT           = 0x4e,
    ERR_VO_INVALID_VFRAME         = 0x4f,
    ERR_VO_INVALID_RECT_PARA      = 0x50,
    ERR_VO_SETBEGIN_ALREADY       = 0x51,
    ERR_VO_SETBEGIN_NOTYET        = 0x52,
    ERR_VO_SETEND_ALREADY         = 0x53,
    ERR_VO_SETEND_NOTYET          = 0x54,

    ERR_VO_GRP_INVALID_ID         = 0x55,
    ERR_VO_GRP_NOT_CREATE         = 0x56,
    ERR_VO_GRP_HAS_CREATED        = 0x57,
    ERR_VO_GRP_NOT_DESTROY        = 0x58,
    ERR_VO_GRP_CHN_FULL           = 0x59,
    ERR_VO_GRP_CHN_EMPTY          = 0x5a,
    ERR_VO_GRP_CHN_NOT_EMPTY      = 0x5b,
    ERR_VO_GRP_INVALID_SYN_MODE   = 0x5c,
    ERR_VO_GRP_INVALID_BASE_PTS   = 0x5d,
    ERR_VO_GRP_NOT_START          = 0x5e,
    ERR_VO_GRP_NOT_STOP           = 0x5f,
    ERR_VO_GRP_INVALID_FRMRATE    = 0x60,
    ERR_VO_GRP_CHN_HAS_REG        = 0x61,
    ERR_VO_GRP_CHN_NOT_REG        = 0x62,
    ERR_VO_GRP_CHN_NOT_UNREG      = 0x63,
    ERR_VO_GRP_BASE_NOT_CFG       = 0x64,

    ERR_GFX_NOT_DISABLE           = 0x65,
    ERR_GFX_NOT_BIND              = 0x66,
    ERR_GFX_NOT_UNBIND            = 0x67,
    ERR_GFX_INVALID_ID            = 0x68,

    ERR_VO_WBC_NOT_DISABLE        = 0x69,
    ERR_VO_WBC_NOT_CONFIG         = 0x6a,

    ERR_VO_CHN_AREA_OVERLAP       = 0x6b,

    EN_ERR_INVALID_WBCID          = 0x6c,
    EN_ERR_INVALID_LAYERID        = 0x6d,
    EN_ERR_VO_VIDEO_HAS_BINDED    = 0x6e,
    EN_ERR_VO_VIDEO_NOT_BINDED    = 0x6f,
    ERR_VO_WBC_HAS_BIND           = 0x70,
    ERR_VO_WBC_HAS_CONFIG         = 0x71,
    ERR_VO_WBC_NOT_BIND           = 0x72,

    /* new added */
    ERR_VO_BUTT

}EN_VOU_ERR_CODE_E;

/** VO输出接口bitmap */
typedef AR_U32 VO_INTF_TYPE_E;

/** VO输出时序 @note暂只支持接口配置为VO_INTF_HDMI才生效 */
typedef enum arVO_INTF_SYNC_E
{
    VO_OUTPUT_PAL = 0,               /* PAL standard*/
    VO_OUTPUT_NTSC,                  /* NTSC standard */

    VO_OUTPUT_1080P24,               /* 1920 x 1080 at 24 Hz. */
    VO_OUTPUT_1080P25,               /* 1920 x 1080 at 25 Hz. */
    VO_OUTPUT_1080P30,               /* 1920 x 1080 at 30 Hz. */

    VO_OUTPUT_720P50,                /* 1280 x  720 at 50 Hz. */
    VO_OUTPUT_720P60,                /* 1280 x  720 at 60 Hz. */
    VO_OUTPUT_1080I50,               /* 1920 x 1080 at 50 Hz, interlace. */
    VO_OUTPUT_1080I60,               /* 1920 x 1080 at 60 Hz, interlace. */
    VO_OUTPUT_1080P50,               /* 1920 x 1080 at 50 Hz. */
    VO_OUTPUT_1080P60,               /* 1920 x 1080 at 60 Hz. */

    VO_OUTPUT_576P50,                /* 720  x  576 at 50 Hz. */
    VO_OUTPUT_480P60,                /* 720  x  480 at 60 Hz. */

    VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
    VO_OUTPUT_1280x800_60,           /* 1280*800@60Hz VGA@60Hz*/
    VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) */
    VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking)*/
    VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) CVT */
    VO_OUTPUT_960H_PAL,              /* ITU-R BT.1302 960 x 576 at 50 Hz (interlaced)*/
    VO_OUTPUT_960H_NTSC,             /* ITU-R BT.1302 960 x 480 at 60 Hz (interlaced)*/
    VO_OUTPUT_1920x2160_30,          /* 1920x2160_30 */
    VO_OUTPUT_2560x1440_30,          /* 2560x1440_30 */
    VO_OUTPUT_2560x1440_60,          /* 2560x1440_60 */
    VO_OUTPUT_2560x1600_60,          /* 2560x1600_60 */
    VO_OUTPUT_3840x2160_24,          /* 3840x2160_24 */
    VO_OUTPUT_3840x2160_25,          /* 3840x2160_25 */
    VO_OUTPUT_3840x2160_30,          /* 3840x2160_30 */
    VO_OUTPUT_3840x2160_50,          /* 3840x2160_50 */
    VO_OUTPUT_3840x2160_60,          /* 3840x2160_60 */
    VO_OUTPUT_4096x2160_24,          /* 4096x2160_24 */
    VO_OUTPUT_4096x2160_25,          /* 4096x2160_25 */
    VO_OUTPUT_4096x2160_30,          /* 4096x2160_30 */
    VO_OUTPUT_4096x2160_50,          /* 4096x2160_50 */
    VO_OUTPUT_4096x2160_60,          /* 4096x2160_60 */
    VO_OUTPUT_320x240_60,            /* For ota5182 at 60 Hz (8bit)  */
    VO_OUTPUT_320x240_50,            /* For ili9342 at 50 Hz (6bit)  */
    VO_OUTPUT_240x320_50,            /* For ili9341 at 50 Hz (6bit), For st7789 at 50Hz(6bit) */
    VO_OUTPUT_240x320_60,            /* For ili9341 at 60 Hz (16bit) */
    VO_OUTPUT_800x600_50,            /* For LCD     at 50 Hz (24bit) */
    VO_OUTPUT_720x1280_60,           /* For MIPI DSI Tx 720 x1280 at 60 Hz */
    VO_OUTPUT_1080x1920_60,          /* For MIPI DSI Tx 1080x1920 at 60 Hz */
    VO_OUTPUT_7680x4320_30,          /* For HDMI2.1 at 30 Hz         */
    VO_OUTPUT_USER,                  /* User timing. */

    VO_OUTPUT_BUTT

} VO_INTF_SYNC_E;

/** hdmi外设模板 @attention 新增参数 */
typedef enum
{
    VO_HDMI_PERPH_MODE_DEFAULT  =  0,  /* NOTICE!!! this option would use default peripheral confiuration:
                                          i2c no=3，gpio_group=1, gpio_port=1,gpio_pin=2;*/
    VO_HDMI_PERPH_MODE_USER,           /* user defined */
}VO_HDMI_PERPH_MODE_E;

/** 缩放  模式 */
typedef enum arVO_ZOOM_IN_E
{
    VO_ZOOM_IN_RECT = 0,              /* Zoom in by rect */
    VO_ZOOM_IN_RATIO,                 /* Zoom in by ratio */
    VO_ZOOM_IN_BUTT
} VO_ZOOM_IN_E;

/** 色域空间转换 */
typedef enum arVO_CSC_MATRIX_E
{
    VO_CSC_MATRIX_IDENTITY = 0,       /* Identity CSC matrix. */

    VO_CSC_MATRIX_BT601_TO_BT709,     /* BT601 to BT709 */
    VO_CSC_MATRIX_BT709_TO_BT601,     /* BT709 to BT601 */

    VO_CSC_MATRIX_BT601_TO_RGB_PC,    /* BT601 to RGB */
    VO_CSC_MATRIX_BT709_TO_RGB_PC,    /* BT709 to RGB */

    VO_CSC_MATRIX_RGB_TO_BT601_PC,    /* RGB to BT601 FULL */
    VO_CSC_MATRIX_RGB_TO_BT709_PC,    /* RGB to BT709 FULL */

    VO_CSC_MATRIX_RGB_TO_BT2020_PC,   /* RGB to BT.2020 */
    VO_CSC_MATRIX_BT2020_TO_RGB_PC,   /* BT.2020 to RGB */

    VO_CSC_MATRIX_RGB_TO_BT601_TV,    /* RGB to BT601 LIMIT */
    VO_CSC_MATRIX_RGB_TO_BT709_TV,    /* RGB to BT709 LIMIT */

    VO_CSC_MATRIX_BUTT
} VO_CSC_MATRIX_E;

/** 通道属性 */
typedef struct arVO_CHN_ATTR_S
{
    AR_U32  u32Priority;                /* Video out overlay pri sd */
    RECT_S  stRect;                     /* Rectangle of video output channel */
    AR_BOOL bDeflicker;                 /* Deflicker or not sd */
}VO_CHN_ATTR_S;

/** 通道参数 */
typedef struct arVO_CHN_PARAM_S
{
    ASPECT_RATIO_S stAspectRatio;       /* RW; aspect ratio */
}VO_CHN_PARAM_S;

/** 通道边框 */
typedef struct arVO_BORDER_S
{
    AR_BOOL     bBorderEn;              /* RW; Do frame or not*/
    BORDER_S    stBorder;               /* RW; frame's top, bottom, left, right width and color*/
}VO_BORDER_S;

/** 状态信息 */
typedef struct arVO_QUERY_STATUS_S
{
    AR_U32 u32ChnBufUsed;              /* Channel buffer that been occupied */
} VO_QUERY_STATUS_S;

/** 时序参数 */
typedef struct arVO_SYNC_INFO_S
{
    AR_BOOL  bSynm;     /* RW; sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    AR_BOOL  bIop;      /* RW; interlaced or progressive display(0:i; 1:p) */
    AR_U8    u8Intfb;   /* RW; interlace bit width while output */

    AR_U16   u16Vact ;  /* RW; vertical active area */
    AR_U16   u16Vbb;    /* RW; vertical back blank porch */
    AR_U16   u16Vfb;    /* RW; vertical front blank porch */

    AR_U16   u16Hact;   /* RW; horizontal active area */
    AR_U16   u16Hbb;    /* RW; horizontal back blank porch */
    AR_U16   u16Hfb;    /* RW; horizontal front blank porch */
    AR_U16   u16Hmid;   /* RW; bottom horizontal active area */

    AR_U16   u16Bvact;  /* RW; bottom vertical active area */
    AR_U16   u16Bvbb;   /* RW; bottom vertical back blank porch */
    AR_U16   u16Bvfb;   /* RW; bottom vertical front blank porch */

    AR_U16   u16Hpw;    /* RW; horizontal pulse width */
    AR_U16   u16Vpw;    /* RW; vertical pulse width */

    AR_BOOL  bIdv;      /* RW; inverse data valid of output */
    AR_BOOL  bIhs;      /* RW; inverse horizontal synch signal */
    AR_BOOL  bIvs;      /* RW; inverse vertical synch signal */
} VO_SYNC_INFO_S;

/** hdmi外设相关配置 @attention 新增参数 */
typedef struct
{
    VO_HDMI_PERPH_MODE_E               enMode;                    /* RW; HDMI peripheral mode */
    AR_U8                              u8I2cNo;                   /* RW; i2c no */
    AR_U8                              u8I2cAddr;                 /* RW; i2c address */
    AR_U8                              u8GpioGroup;               /* RW; gpio group no */
    AR_U8                              u8GpioPort;                /* RW; gpio port no */
    AR_U8                              u8GpioPin;                 /* RW; gpio_pin no */
    AR_U8                              u8Reserved[3];             /* RW; reservied */
}VO_HDMI_INFO_S;

/** bt1120管脚映射 @attention 新增参数*/
typedef enum arVO_PIN_MODE_BT1120
{
    VO_PIN_MODE_BT1120_DEFAULT         =  0,
    VO_PIN_MODE_BT1120_CBY_CRY         =  1,
    VO_PIN_MODE_BT1120_YCB_YCR         =  2,
    VO_PIN_MODE_BT1120_CRY_CBY         =  3,
    VO_PIN_MODE_BT1120_YCR_YCB         =  4,
}VO_PIN_MODE_BT1120;

/** bt656管脚映射 @attention 新增参数*/
typedef enum arVO_PIN_MODE_BT656
{
    VO_PIN_MODE_BT656_DEFAULT          =  0,
    VO_PIN_MODE_BT656_YCR_YCB_H        =  1,
    VO_PIN_MODE_BT656_YCB_YCR_H        =  2,
    VO_PIN_MODE_BT656_CRY_CBY_H        =  3,
    VO_PIN_MODE_BT656_CBY_CRY_H        =  4,

    VO_PIN_MODE_BT656_YCB_YCR_L        =  5,
    VO_PIN_MODE_BT656_YCR_YCB_L        =  6,
    VO_PIN_MODE_BT656_CRY_CBY_L        =  7,
    VO_PIN_MODE_BT656_CBY_CRY_L        =  8,
}VO_PIN_MODE_BT656;

/** LCD24BIT管脚映射 @attention 新增参数*/
typedef enum arVO_PIN_MODE_RGB888
{
    VO_PIN_MODE_RGB888_DEFAULT         =  0,
    VO_PIN_MODE_RGB888_GRB             =  1,
    VO_PIN_MODE_RGB888_RGB             =  2,
    VO_PIN_MODE_RGB888_BRG             =  3,
    VO_PIN_MODE_RGB888_RBG             =  4,
    VO_PIN_MODE_RGB888_BGR             =  5,
    VO_PIN_MODE_RGB888_GBR             =  6
}VO_PIN_MODE_RGB888;

/** LCD16BIT管脚映射 @attention 新增参数*/
typedef enum arVO_PIN_MODE_RGB565
{
    VO_PIN_MODE_RGB565_DEFAULT         =  0,
    VO_PIN_MODE_RGB565_GRB             =  1,
    VO_PIN_MODE_RGB565_BGR             =  2
}VO_PIN_MODE_RGB565;

/** DVP管脚映射 @attention 新增参数*/
typedef union
{
    VO_PIN_MODE_BT656   bt656;
    VO_PIN_MODE_BT1120  bt1120;
    VO_PIN_MODE_RGB888  rgb888;
    VO_PIN_MODE_RGB565  rgb565;
}VO_PIN_MODE_E;

typedef enum arVO_LOWDELAY_MODE
{
    VO_LOWDELAY_MODE_NULL              =  0,  /**< 不使能lowdelay功能 */
    VO_LOWDELAY_MODE_NORMAL            =  1,  /**< 普通lowdelay模式 */
}VO_LOWDELAY_MODE;

typedef struct arVO_FRE_ADJUST_S
{
    AR_U64       u64StepTimePs;              /**< 步长，时间单位ps */
    AR_U64       u64ThresholdUs;             /**< 阈值，时间单位us */
    AR_FLOAT     f32InitDiff;                /**< 初始差，以帧为单位，推荐相差半帧(0.5) */
}VO_FRE_ADJUST_S;

typedef struct arVO_LOWDELAY_ATTR_S
{
    VO_LOWDELAY_MODE          enMode;         /**< lowdelay模式 */
    VO_FRE_ADJUST_S           stFreAdjust;    /**< lowdelay动态调频参数 */
}VO_LOWDELAY_ATTR_S;

/** VO输出设备属性 */
typedef struct arVO_PUB_ATTR_S
{
    AR_U32                   u32BgColor;          /**<@note      背景色，当对应第一个layer enable时，会显示该纯色*/
    VO_INTF_TYPE_E           enIntfType;          /**<@note      接口类型，目前仅VO_INTF_HDMI使能时才能使用*/
    VO_INTF_SYNC_E           enIntfSync;          /**<           时序模板     */
    VO_SYNC_INFO_S           stSyncInfo;          /**<           用户自定义时序          */
    VO_HDMI_INFO_S           stHdmiInfo;          /**<@attention hdmi相关管脚配置 */
    VO_PIN_MODE_E            enPinMode;           /**<@attention dvp out输出管脚映射 */
} VO_PUB_ATTR_S;

typedef enum arVO_IRQ_TYPE
{
    IRQ_TYPE_INTERVAL_LINE      = 1 << 0,
    IRQ_TYPE_FIX_LINE           = 1 << 1,
    IRQ_TYPE_WRITE_BACK         = 1 << 2,
    IRQ_TYPE_WB_LOAD            = 1 << 3,
    IRQ_TYPE_DE_DMA_FRAME_DONE  = 1 << 4,
    IRQ_TYPE_VSYNC              = 1 << 5,
    IRQ_TYPE_FRAME_DONE         = 1 << 6,
}VO_IRQ_TYPE;
typedef struct arVO_FRAME_DONE_PARA_S
{
    AR_U32                  u32SrcFrameId;
    AR_U32                  u32SrcPTS;
}VO_FRAME_DONE_PARA_S;

typedef union
{
    VO_FRAME_DONE_PARA_S     stFrameDonePara;
}VO_SUBSCRIBE_EXT_PARA_S;

typedef struct arVO_SUBSCRIBER_INFO_S
{
    AR_U32                   u32IrqType;
    AR_U64                   u64IrqTimeNs;
    VO_SUBSCRIBE_EXT_PARA_S  stExtpara;
} VO_SUBSCRIBE_INFO_S;

typedef struct arVO_SUBSCRIBER_ATTR_S
{
    AR_U32                   u32SubscribeType;     /**<@note 可以通过|订阅多种中断类型，ar9311/ar9341仅仅支持订阅IRQ_TYPE_FRAME_DONE中断*/
    AR_S32                  (*subscribe_call_back)(VO_DEV dev_id, VO_SUBSCRIBE_INFO_S *sub_info); /**< 订阅回调函数    */
} VO_SUBSCRIBE_ATTR_S;

/** VO回写设备属性 */
typedef struct arVO_WBC_ATTR_S
{
    SIZE_S              stTargetSize;             /**<@note      暂未实现      */
    PIXEL_FORMAT_E      enPixelFormat;            /**<@note      暂未实现      */
    AR_FLOAT            u32FrameRate;             /**<@note      暂未实现      */
    DYNAMIC_RANGE_E     enDynamicRange;           /**<@note      暂未实现      */
    COMPRESS_MODE_E     enCompressMode;           /**<@note      暂未实现      */
} VO_WBC_ATTR_S;

/** 回写设备模式 */
typedef enum arVO_WBC_MODE_E
{
    VO_WBC_MODE_NORMAL = 0,                       /**<@note      暂未实现      */
    VO_WBC_MODE_DROP_REPEAT,                      /**<@note      暂未实现      */
    VO_WBC_MODE_PROG_TO_INTL,                     /**<@note      暂未实现      */
    VO_WBC_MODE_BUTT,
} VO_WBC_MODE_E;

/** 回写源类型 */
typedef enum arVO_WBC_SOURCE_TYPE_E
{
    VO_WBC_SOURCE_DEV      = 0x0,                /**<@note      暂未实现      */
    VO_WBC_SOURCE_VIDEO    = 0x1,                /**<@note      暂未实现      */
    VO_WBC_SOURCE_GRAPHIC  = 0x2,                /**<@note      暂未实现      */
    VO_WBC_SOURCE_BUTT
} VO_WBC_SOURCE_TYPE_E;

/** 回写源信息 */
typedef struct arVO_WBC_SOURCE_S
{
    VO_WBC_SOURCE_TYPE_E    enSourceType;       /**<@note      暂未实现       */
    AR_U32                  u32SourceId;        /**<@note      暂未实现       */
} VO_WBC_SOURCE_S;

/** 回写源信息 */
typedef enum arVO_PART_MODE_E
{
    VO_PART_MODE_SINGLE    = 0,                 /**<           目前使用scaler进行拼接 */
    VO_PART_MODE_MULTI    = 1,                  /**<@note      暂不使用     */
    VO_PART_MODE_BUTT
} VO_PART_MODE_E;

/* FIFO队列，用来缓存数据(帧可能有抖动)。
 * 每次送显的时候，先检查FIFO队列，如果连续多个周期FIFO队列的长度都大于阈值，
 * 则主动丢弃FIFO中的历史数据，以实现时延可以快速减小到预期值。*/
typedef struct arVO_FIFO_INFO_S
{
    AR_BOOL         bEnable;
    AR_U32          u32Depth;
    AR_U32          u32ClearCycle;
    AR_U32          u32Threshold;
} VO_FIFO_INFO_S;

/** layer属性 */
typedef struct arVO_VIDEO_LAYER_ATTR_S
{
    RECT_S          stDispRect;                 /**<@note      暂不使用，layer的宽高与其canvas大小一致，      */
    SIZE_S          stImageSize;                /**<             layer画布大小 */
    AR_FLOAT        u32DispFrmRt;               /**<            layer刷新帧率 */
    PIXEL_FORMAT_E  enPixFormat;                /**<            像素格式          */
    AR_BOOL         bDoubleFrame;               /* <@note      暂不使用 */
    AR_BOOL         bClusterMode;               /* <@note      暂不使用 */
    DYNAMIC_RANGE_E enDstDynamicRange;          /* <@note      暂不使用 */
    AR_BOOL         bMirror;                    /* <@attention 镜像显示 */
    AR_BOOL         bFlip;                      /* <@attention 翻转显示 */
    VO_FIFO_INFO_S  fifoInfo;                /* <@attention  FIFO相关信息 */
} VO_VIDEO_LAYER_ATTR_S;

/** layer参数 */
typedef struct arVO_LAYER_PARAM_S
{
    ASPECT_RATIO_S stAspectRatio;               /**<@note      暂未实现       */
}VO_LAYER_PARAM_S;

/** 缩放比例 */
typedef struct arVO_ZOOM_RATIO_S
{
    AR_U32 u32XRatio;  /**<@note      暂未实现       */
    AR_U32 u32YRatio;  /**<@note      暂未实现       */
    AR_U32 u32WRatio;  /**<@note      暂未实现       */
    AR_U32 u32HRatio;  /**<@note      暂未实现       */
} VO_ZOOM_RATIO_S;

/** 缩放属性 */
typedef struct arVO_ZOOM_ATTR_S
{
    VO_ZOOM_IN_E       enZoomType;      /**<@note      暂未实现       */
    union
    {
        RECT_S          stZoomRect;     /**<@note      暂未实现       */
        VO_ZOOM_RATIO_S stZoomRatio;    /**<@note      暂未实现       */
    };
} VO_ZOOM_ATTR_S;

/** 色域属性 */
typedef struct arVO_CSC_S
{
    VO_CSC_MATRIX_E enCscMatrix;        /**<           色域矩阵       */
    AR_U32          u32Luma;            /**<@note      暂未实现       */
    AR_U32          u32Contrast;        /**<@note      暂未实现       */
    AR_U32          u32Hue;             /**<@note      暂未实现       */
    AR_U32          u32Satuature;       /**<@note      暂未实现       */
} VO_CSC_S;

/** VO区域信息 */
typedef struct arVO_REGION_INFO_S
{
    AR_U32              u32RegionNum;       /**<@note      暂未实现       */
    RECT_S* ATTRIBUTE   pstRegion;          /**<@note      暂未实现       */
}VO_REGION_INFO_S;

/** VO边框参数 */
typedef struct arVO_LAYER_BOUNDARY_S
{
    AR_U32  u32Width;                       /**<@note      暂未实现       */
    AR_U32  u32Color[2];                    /**<@note      暂未实现       */
}VO_LAYER_BOUNDARY_S;

/** VO边框 */
typedef struct arVO_CHN_BOUNDARY_S
{
    AR_BOOL bBoundaryEn;                   /**<@note      暂未实现       */
    AR_U32  u32ColorIndex;                 /**<@note      暂未实现       */
}VO_CHN_BOUNDARY_S;

/** VO模块参数 */
typedef struct arVO_MOD_PARAM_S
{
    AR_BOOL       bTransparentTransmit;    /**<@note      暂未实现       */
    AR_BOOL       bExitDev;                /**<@note      暂未实现       */
    AR_BOOL       bWbcBgBlackEn;           /**<@note      暂未实现       */
    AR_BOOL       bDevClkExtEn;            /**<@note      暂未实现       */
} VO_MOD_PARAM_S;

/** VO时钟源 */
typedef enum arVO_CLK_SOURCE_E
{
    VO_CLK_SOURCE_PLL,                     /**<@note      VO的时钟源固定为pll       */
    VO_CLK_SOURCE_LCDMCLK,                 /**<@note      暂不使用       */
    VO_CLK_SOURCE_BUTT
} VO_CLK_SOURCE_E;

/** VO时钟配置方式 */
typedef enum arVO_CLK_CONTROL_E
{
    VO_CLK_AUTO,                           /**<@note     VO的时钟源自动选择*/
    VO_CLK_MANUAL,                         /**<@note     用户使用hal层接口进行配置*/
} VO_CLK_CONTROL_E;

/** VO时钟相位信息选择 */
typedef enum arVO_CLK_PHASE_E
{
    VO_CLK_PHASE_DEFAULT,                  /**<@note     default值*/
    VO_CLK_PHASE0,                         /**<@note     VO的时钟相位为0度*/
    VO_CLK_PHASE90,                        /**<@note     VO的时钟相位为90度，ar9341不支持*/
    VO_CLK_PHASE180,                       /**<@note     VO的时钟相位为180度*/
    VO_CLK_PHASE270,                       /**<@note     VO的时钟相位为270度, ar9341不支持*/
} VO_CLK_PHASE_E;

/** 用户自定义pll */
typedef struct arVO_USER_INTFSYNC_PLL_S
{
    AR_U32  u32Fbdiv;                      /**<@note      暂不使用       */
    AR_U32  u32Frac;                       /**<@note      暂不使用       */
    AR_U32  u32Refdiv;                     /**<@note      暂不使用       */
    AR_U32  u32Postdiv1;                   /**<@note      暂不使用       */
    AR_U32  u32Postdiv2;                   /**<@note      暂不使用       */
}VO_USER_INTFSYNC_PLL_S;

/** 用户自定义接口属性 */
typedef struct arVO_USER_INTFSYNC_ATTR_S
{
    VO_CLK_SOURCE_E enClkSource;           /**<@note      暂不使用       */

    union
    {
        VO_USER_INTFSYNC_PLL_S stUserSyncPll;  /**<@note      暂不使用       */
        AR_U32                 u32LcdMClkDiv;  /**<@note      暂不使用       */
    };
} VO_USER_INTFSYNC_ATTR_S;

/** 用户自定义接口信息 */
typedef struct arVO_USER_INTFSYNC_INFO_S
{
    VO_USER_INTFSYNC_ATTR_S stUserIntfSyncAttr;   /**<@note       暂不使用       */
    AR_U32                  u32PreDiv;            /**<@note       暂不使用       */
    AR_U32                  u32DevDiv;            /**<@note       暂不使用       */
    AR_BOOL                 bClkReverse;          /**<@attention  clk是否翻转     */
    VO_CLK_PHASE_E          enClkPhase;           /**<@attention  clk相位，在bClkReverse为0时生效     */
    VO_CLK_CONTROL_E        enClkControl;         /**<@attention  clk控制      */
} VO_USER_INTFSYNC_INFO_S;

/** dsi时序   */
typedef struct arVO_DSI_SYNC_INFO_S
{
    AR_U32  u32Hsa;                              /**<@attention      水平同步脉宽       */
    AR_U32  u32Hbp;                              /**<@attention      水平后肩         */
    AR_U32  u32Hact;                             /**<@attention      水平有效数据       */
    AR_U32  u32Hfp;                              /**<@attention      水平前肩         */

    AR_U32  u32Vsa;                              /**<@attention      垂直同步脉宽       */
    AR_U32  u32Vbp;                              /**<@attention      垂直后肩         */
    AR_U32  u32Vact;                             /**<@attention      垂直有效数据       */
    AR_U32  u32Vfp;                              /**<@attention      垂直前肩         */
}VO_DSI_SYNC_INFO_S;

/** dsi接口属性   */
typedef struct arVO_DSI_ATTR_S
{
    AR_U32         u32Lane[MAX_DSI_LANE_NUM];    /**<@attention      lane数量       */
    AR_U32         u32BitsPerPixel;              /**<@attention      像素位宽         */
    VO_DSI_SYNC_INFO_S   stSyncInfo;             /**<@attention      时序           */
    AR_FLOAT       fDphyClkMhz;                  /**<@attention      dphy频率       */
    AR_U32         u32PllFreqReg2c0;             /**<@attention      pll寄存器       */
    AR_U32         u32PllFreqReg38c;             /**<@attention      pll寄存器       */
}VO_DSI_ATTR_S;

/** dsi命令参数 */
typedef struct arVO_DSI_CMD_S
{
    AR_U16         u16DataType;                  /**<@attention      dsi数据类型       */
    AR_U16         u16CmdSize;                   /**<@attention      dsi命令大小       */
    AR_U8         *pu8Cmd;                       /**<@attention      dsi命令内容       */
}VO_DSI_CMD_S;

/** dsi读操作，通常用于读取屏幕ID */
typedef struct arVO_DSI_READ_S
{
    AR_U16         u16DataType;                  /**<@attention      dsi数据类型        */
    AR_U16         u16DataSize;                  /**<@attention      dsi命令大小        */
    AR_U32         u32Para;                      /**<@attention      dsi读取参数        */
    AR_U8         *pu8Data;                      /**<@attention      dsi读取返回值       */
}VO_DSI_READ_S;

    /** @} */  /** <!-- ==== Structure Definition End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __AR_COMM_VO_H__ */

