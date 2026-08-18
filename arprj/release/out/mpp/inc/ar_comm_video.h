/**
 * \file
 * \brief 描述视频相关的通用数据结构
 */

#ifndef __AR_COMM_VIDEO_H__
#define __AR_COMM_VIDEO_H__

#include "hal_type.h"
//#include "ar_defines.h"
#include "ar_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SRC_LENS_COEF_SEG  2
#define DST_LENS_COEF_SEG  3
#define SRC_LENS_COEF_NUM  4
#define DST_LENS_COEF_NUM  4

#define ISP_BAYER_CHN   (4)


typedef enum arOPERATION_MODE_E
{
    OPERATION_MODE_AUTO   = 0,
    OPERATION_MODE_MANUAL = 1,
    OPERATION_MODE_BUTT
} OPERATION_MODE_E;


/*Angle of rotation*/
typedef enum arROTATION_E
{
    ROTATION_0   = 0,
    ROTATION_90  = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3,
    ROTATION_BUTT
} ROTATION_E;

typedef enum arVB_SOURCE_E
{
    VB_SOURCE_COMMON  = 0,
    VB_SOURCE_MODULE  = 1,
    VB_SOURCE_PRIVATE = 2,
    VB_SOURCE_USER    = 3,
    VB_SOURCE_BUTT
} VB_SOURCE_E;

typedef enum arDATA_RATE_E
{
    DATA_RATE_X1 = 0,         /* RW; output 1 pixel per clock */
    DATA_RATE_X2 = 1,         /* RW; output 2 pixel per clock */

    DATA_RATE_BUTT
} DATA_RATE_E;

typedef struct arBORDER_S
{
    AR_U32 u32TopWidth;
    AR_U32 u32BottomWidth;
    AR_U32 u32LeftWidth;
    AR_U32 u32RightWidth;
    AR_U32 u32Color;
} BORDER_S;


typedef struct arPOINT_S
{
    AR_S32 s32X;
    AR_S32 s32Y;
} POINT_S;

typedef struct arSIZE_S
{
    AR_U32 u32Width;
    AR_U32 u32Height;
} SIZE_S;
//当坐标模式是 VI_CROP_RATIO_COOR，的时候，用q12 表示，也就是4096 表示 1
typedef struct arRECT_S
{
    AR_S32 s32X;
    AR_S32 s32Y;
    AR_U32 u32Width;
    AR_U32 u32Height;
} RECT_S;

typedef struct arVIDEO_REGION_INFO_S
{
    AR_U32           u32RegionNum;       /* W; count of the region */
    RECT_S ATTRIBUTE* pstRegion;         /* W; region attribute */
} VIDEO_REGION_INFO_S;


typedef struct arCROP_INFO_S
{
    AR_BOOL bEnable;
    RECT_S  stRect;
} CROP_INFO_S;

typedef struct arFRAME_RATE_CTRL_S
{
    AR_S32  s32SrcFrameRate;        /* RW; source frame rate */
    AR_S32  s32DstFrameRate;        /* RW; dest frame rate */
} FRAME_RATE_CTRL_S;

typedef enum arASPECT_RATIO_E
{
    ASPECT_RATIO_NONE   = 0,        /* full screen */
    ASPECT_RATIO_AUTO   = 1,        /* ratio no change, 1:1*/
    ASPECT_RATIO_MANUAL = 2,        /* ratio manual set */
    ASPECT_RATIO_BUTT

}ASPECT_RATIO_E;

typedef struct arASPECT_RATIO_S
{
    ASPECT_RATIO_E enMode;          /* aspect ratio mode: none/auto/manual */
    AR_U32         u32BgColor;      /* background color, RGB 888 */
    RECT_S         stVideoRect;     /* valid in ASPECT_RATIO_MANUAL mode */

} ASPECT_RATIO_S;



/* we ONLY define picture format used, all unused will be deleted!*/
typedef enum arPIXEL_FORMAT_E
{
    PIXEL_FORMAT_RGB_444 = 0,
    PIXEL_FORMAT_RGB_555,
    PIXEL_FORMAT_RGB_565,
    PIXEL_FORMAT_RGB_888,

    PIXEL_FORMAT_BGR_444,
    PIXEL_FORMAT_BGR_555,
    PIXEL_FORMAT_BGR_565,
    PIXEL_FORMAT_BGR_888,

    PIXEL_FORMAT_ARGB_1555,
    PIXEL_FORMAT_ARGB_4444,
    PIXEL_FORMAT_ARGB_8565, // 10
    PIXEL_FORMAT_ARGB_8888,
    PIXEL_FORMAT_ARGB_2BPP,

    PIXEL_FORMAT_ABGR_1555,
    PIXEL_FORMAT_ABGR_4444,
    PIXEL_FORMAT_ABGR_8565,
    PIXEL_FORMAT_ABGR_8888,

    PIXEL_FORMAT_RGB_BAYER_8BPP,
    PIXEL_FORMAT_RGB_BAYER_10BPP,
    PIXEL_FORMAT_RGB_BAYER_12BPP,
    PIXEL_FORMAT_RGB_BAYER_14BPP, //20
    PIXEL_FORMAT_RGB_BAYER_16BPP,


    PIXEL_FORMAT_YVU_PLANAR_422,
    PIXEL_FORMAT_YVU_PLANAR_420,
    PIXEL_FORMAT_YVU_PLANAR_444,

    PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    PIXEL_FORMAT_YVU_SEMIPLANAR_420, //
    PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    PIXEL_FORMAT_UYVY_PACKAGE_422,
    PIXEL_FORMAT_YUYV_PACKAGE_422,
    PIXEL_FORMAT_VYUY_PACKAGE_422,

    PIXEL_FORMAT_YUV_400,
    PIXEL_FORMAT_UV_420,


    /* SVP data format */
    PIXEL_FORMAT_BGR_888_PLANAR,
    PIXEL_FORMAT_HSV_888_PACKAGE,
    PIXEL_FORMAT_HSV_888_PLANAR,
    PIXEL_FORMAT_LAB_888_PACKAGE,
    PIXEL_FORMAT_LAB_888_PLANAR,
    PIXEL_FORMAT_S8C1,
    PIXEL_FORMAT_S8C2_PACKAGE,
    PIXEL_FORMAT_S8C2_PLANAR,
    PIXEL_FORMAT_S16C1,
    PIXEL_FORMAT_U8C1,
    PIXEL_FORMAT_U16C1,
    PIXEL_FORMAT_S32C1,
    PIXEL_FORMAT_U32C1,
    PIXEL_FORMAT_U64C1,
    PIXEL_FORMAT_S64C1,

    PIXEL_FORMAT_GRAY1,
    PIXEL_FORMAT_GRAY4,
    PIXEL_FORMAT_GRAY8,
    PIXEL_FORMAT_CF50,

    PIXEL_FORMAT_BUTT
} PIXEL_FORMAT_E;


typedef enum arVIDEO_FIELD_E
{
    VIDEO_FIELD_TOP         = 0x1,    /* even field */
    VIDEO_FIELD_BOTTOM      = 0x2,    /* odd field */
    VIDEO_FIELD_INTERLACED  = 0x3,    /* two interlaced fields */
    VIDEO_FIELD_FRAME       = 0x4,    /* frame */

    VIDEO_FIELD_BUTT
} VIDEO_FIELD_E;

typedef enum arVIDEO_FORMAT_E
{
    VIDEO_FORMAT_LINEAR = 0,       /* nature video line */
    VIDEO_FORMAT_TILE_64x16,       /* tile cell: 64pixel x 16line */
    VIDEO_FORMAT_TILE_16x8,        /* tile cell: 16pixel x 8line */
    VIDEO_FORMAT_LINEAR_DISCRETE,  /* The data bits are aligned in bytes */
    VIDEO_FORMAT_BUTT
} VIDEO_FORMAT_E;



typedef enum arCOMPRESS_MODE_E
{
    COMPRESS_MODE_NONE = 0,   /* no compress */
    COMPRESS_MODE_SEG,        /* cf50 is used*/
    COMPRESS_MODE_DPCM_6BITS,  /*for dpcm comp*/
    COMPRESS_MODE_DPCM_8BITS,
    COMPRESS_MODE_DPCM_10BITS,
    COMPRESS_MODE_BUTT,
} COMPRESS_MODE_E;


typedef enum arVIDEO_DISPLAY_MODE_E
{
    VIDEO_DISPLAY_MODE_PREVIEW  = 0x0,
    VIDEO_DISPLAY_MODE_PLAYBACK = 0x1,

    VIDEO_DISPLAY_MODE_BUTT
} VIDEO_DISPLAY_MODE_E;

typedef struct arLUMA_INFO_S
{
    AR_U64 u64LumaPixSum;      /* Luma sum of current frame */
    AR_U32 u32LumaPixAverage;  /* Luma average of current frame */
    AR_U64 u64PTS;             /* PTS of current frame  */
} LUMA_INFO_S;

#define LDCI_STAT_WND_X             24
#define LDCI_STAT_WND_Y             16

#define DCF_DRSCRIPTION_LENGTH      32
#define DCF_CAPTURE_TIME_LENGTH     20

typedef struct arISP_DCF_CONST_INFO_S
{
    AR_U8       au8ImageDescription[DCF_DRSCRIPTION_LENGTH];        /*Describes image*/
    AR_U8       au8Make[DCF_DRSCRIPTION_LENGTH];                    /*Shows manufacturer of digital cameras*/
    AR_U8       au8Model[DCF_DRSCRIPTION_LENGTH];                   /*Shows model number of digital cameras*/
    AR_U8       au8Software[DCF_DRSCRIPTION_LENGTH];                /*Shows firmware (internal software of digital cameras) version number*/

    AR_U8       u8LightSource;                                      /*Light source, actually this means white balance setting. '0' means unknown, '1' daylight, '2'
                                                                               fluorescent, '3' tungsten, '10' flash, '17' standard light A, '18' standard light B, '19' standard light
                                                                               C, '20' D55, '21' D65, '22' D75, '255' other*/
    AR_U32      u32FocalLength;                                     /*Focal length of lens used to take image. Unit is millimeter*/
    AR_U8       u8SceneType;                                        /*Indicates the type of scene. Value '0x01' means that the image was directly photographed.*/
    AR_U8       u8CustomRendered;                                   /*Indicates the use of special processing on image data, such as rendering geared to output.
                                                                               0 = Normal process  1 = Custom process   */
    AR_U8       u8FocalLengthIn35mmFilm;                            /*Indicates the equivalent focal length assuming a 35mm film camera, in mm*/
    AR_U8       u8SceneCaptureType;                                 /*Indicates the type of scene that was shot. 0 = Standard,1 = Landscape,2 = Portrait,3 = Night scene. */
    AR_U8       u8GainControl;                                      /*Indicates the degree of overall image gain adjustment. 0 = None,1 = Low gain up,2 = High gain up,3 = Low gain down,4 = High gain down. */
    AR_U8       u8Contrast;                                         /*Indicates the direction of contrast processing applied by the camera when the image was shot.
                                                                               0 = Normal,1 = Soft,2 = Hard */
    AR_U8       u8Saturation;                                       /*Indicates the direction of saturation processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Low saturation,2 = High saturation*/
    AR_U8       u8Sharpness;                                        /*Indicates the direction of sharpness processing applied by the camera when the image was shot.
                                                                              0 = Normal,1 = Soft,2 = Hard .*/
    AR_U8       u8MeteringMode;                                     /*Exposure metering method. '0' means unknown, '1' average, '2' center weighted average, '3'
                                                                              spot, '4' multi-spot, '5' multi-segment, '6' partial, '255' other*/
} ISP_DCF_CONST_INFO_S;

typedef struct arISP_DCF_UPDATE_INFO_S
{
    AR_U32      u32ISOSpeedRatings;                                 /*CCD sensitivity equivalent to Ag-Hr film speedrate*/
    AR_U32      u32ExposureTime;                                    /*Exposure time (reciprocal of shutter speed).*/
    AR_U32      u32ExposureBiasValue;                               /*Exposure bias (compensation) value of taking picture*/
    AR_U8       u8ExposureProgram;                                  /*Exposure program that the camera used when image was taken. '1' means manual control, '2'
                                                                              program normal, '3' aperture priority, '4' shutter priority, '5' program creative (slow program),
                                                                              '6' program action(high-speed program), '7' portrait mode, '8' landscape mode*/
    AR_U32      u32FNumber;                                         /*The actual F-number (F-stop) of lens when the image was taken*/
    AR_U32      u32MaxApertureValue;                                /*Maximum aperture value of lens.*/
    AR_U8       u8ExposureMode;                                     /*Indicates the exposure mode set when the image was shot.
                                                                              0 = Auto exposure,1 = Manual exposure, 2 = Auto bracket*/
    AR_U8       u8WhiteBalance;                                     /* Indicates the white balance mode set when the image was shot.
                                                                                0 = Auto white balance ,1 = Manual white balance */
} ISP_DCF_UPDATE_INFO_S;

typedef struct arISP_DCF_INFO_S
{
    ISP_DCF_CONST_INFO_S  stIspDCFConstInfo;
    ISP_DCF_UPDATE_INFO_S stIspDCFUpdateInfo;
} ISP_DCF_INFO_S;

typedef struct arJPEG_DCF_S
{
    AR_U8           au8CaptureTime[DCF_CAPTURE_TIME_LENGTH];            /*The date and time when the picture data was generated*/
    AR_BOOL         bFlash;                                             /*whether the picture is captured when a flash lamp is on*/
    AR_U32          u32DigitalZoomRatio;                                /*Indicates the digital zoom ratio when the image was shot.
                                                                                   If the numerator of the recorded value is 0, this indicates that digital zoom was not used.*/
    ISP_DCF_INFO_S  stIspDCFInfo;

} JPEG_DCF_S;


typedef struct arISP_FRAME_INFO_S
{
    AR_U32      u32ISO;                    /**<gain=iso/100*/
    AR_U32      u32ExposureTime;           /**<@note。us*/
    AR_U32      u32IspDgain;               /**<@note。not used*/
    AR_U32      u32Again;/**<@note。not used*/
    AR_U32      u32Dgain;/**<@note。not used*/
    AR_U32      au32Ratio[3];/**<@note。not used*/
    AR_U32      u32IspNrStrength;/**<@note。not used*/
    AR_U32      u32FNumber;               /**<@note。not used*/
    AR_U32      u32SensorID;              /**<@note。not used*/
    AR_U32      u32SensorMode;/**<@note。not used*/
    AR_U32      u32HmaxTimes;             /**<行时间，单位us*/
    AR_U32      u32VcNum;                /**<@note。not used*/
  AR_U32      uCurrentLuma; /**<@note。added 当前frame的曝光luma，raw 空间值*/
  AR_U32      uLuxIndex; /**<@note。not used，当前帧的luma索引*/
  AR_U32      uFrameDirty; /**<@note，新加参数，如果这个值被设置，说明isp过去的某一个帧由错误产生，如果当前frame的值和前一帧的值不同，说明当前帧有isp 错误*/
} ISP_FRAME_INFO_S;


#define CFACOLORPLANE     (3)
#define DNG_NP_SIZE       (6)


typedef struct arDNG_RATIONAL_S
{
    AR_U32 u32Numerator;/*represents the numerator of a fraction,*/
    AR_U32 u32Denominator;/* the denominator. */
} DNG_RATIONAL_S;

/*
Defines the structure of DNG image dynamic infomation
*/
typedef struct arDNG_IMAGE_DYNAMIC_INFO_S
{
    AR_U32 au32BlackLevel[ISP_BAYER_CHN];           /* RO;Range: [0x0, 0xFFFF]; black level*/
    DNG_RATIONAL_S astAsShotNeutral[CFACOLORPLANE]; /* specifies the selected white balance at time of capture, encoded as the coordinates of a perfectly neutral color in linear reference space values.*/
    AR_DOUBLE adNoiseProfile[DNG_NP_SIZE];          /* RO;describes the amount of noise in a raw image*/
} DNG_IMAGE_DYNAMIC_INFO_S;


typedef struct arISP_HDR_INFO_S
{
    AR_U32 u32ColorTemp;
    AR_U16 au16CCM[9];
    AR_U8  u8Saturation;
} ISP_HDR_INFO_S;

typedef struct arISP_ATTACH_INFO_S
{
    ISP_HDR_INFO_S     stIspHdr;
    AR_U32 u32ISO;
    AR_U8 u8SnsWDRMode;
} ISP_ATTACH_INFO_S;

typedef enum arFRAME_FLAG_E
{
    FRAME_FLAG_SNAP_FLASH  = 0x1 << 0,
    FRAME_FLAG_SNAP_CUR      = 0x1 << 1,
    FRAME_FLAG_SNAP_REF       = 0x1 << 2,
    FRAME_FLAG_SNAP_END      = 0x1 << 31,
    FRAME_FLAG_BUTT
} FRAME_FLAG_E;


/* RGGB=4 */
#define ISP_WB_GAIN_NUM  4
/* 3*3=9 matrix */
#define ISP_CAP_CCM_NUM   9
typedef struct arISP_CONFIG_INFO_S
{
    AR_U32  u32ISO;
    AR_U32  u32IspDgain;
    AR_U32  u32ExposureTime;
    AR_U32  au32WhiteBalanceGain[ISP_WB_GAIN_NUM];
    AR_U32  u32ColorTemperature;
    AR_U16  au16CapCCM[ISP_CAP_CCM_NUM];
} ISP_CONFIG_INFO_S;

typedef struct arVIDEO_EXT_S
{
    AR_S32 s32NalType;                  /**< This is nal Type of decoded picture */
    AR_S32 s32PicType;                  /**< This is the picture type of decoded picture. It reports the picture type of bottom field for interlaced stream */
    AR_S32 s32picTypeFirst;             /**< This is only valid in interlaced mode and indicates the picture type of the top field */
    AR_S32 s32NumOfErrMBsInDisplay;     /**< This is the total number of error coded unit(MB/CTU) in the display picture of indexFrameDisplay */
    AR_S32 s32NumOfTotMBsInDisplay;     /**< This is the total number of coded unit(MB/CTU) in the display picture of indexFrameDisplay */
    AR_S32 s32TopFieldFirst;            /**< For decoded picture consisting of two fields, 0 : VPU decodes the bottom field and then top field;
                                                                                           1 : VPU decodes the top field and then bottom field */
    AR_S32 s32RateNumerator;            /**< The numerator part of frame rate fraction */
    AR_S32 s32RateDenominator;          /**< The denominator part of frame rate fraction */
} VIDEO_EXT_S;

typedef struct arVIDEO_SUPPLEMENT_S
{
    AR_U64   u64JpegDCFPhyAddr;
    AR_U64   u64IspInfoPhyAddr;
    AR_U64   u64LowDelayPhyAddr;
    AR_U64   u64MotionDataPhyAddr;
    AR_U64   u64FrameDNGPhyAddr;

    AR_VOID* ATTRIBUTE pJpegDCFVirAddr;        /* JPEG_DCF_S, used in JPEG DCF */
    AR_VOID* ATTRIBUTE pIspInfoVirAddr;        /* ISP_FRAME_INFO_S, used in ISP debug, when get raw and send raw */
    AR_VOID* ATTRIBUTE pLowDelayVirAddr;       /* used in low delay */
    AR_VOID* ATTRIBUTE pMotionDataVirAddr;     /* vpss 3dnr use: gme motion data, Filter motion data, gyro data.*/
    AR_VOID* ATTRIBUTE pFrameDNGVirAddr;
} VIDEO_SUPPLEMENT_S;

typedef enum arCOLOR_GAMUT_E
{
    COLOR_GAMUT_BT601 = 0,
    COLOR_GAMUT_BT709,
    COLOR_GAMUT_BT2020,
    COLOR_GAMUT_USER,
    COLOR_GAMUT_BUTT
} COLOR_GAMUT_E;

typedef struct arISP_COLORGAMMUT_INFO_S
{
        COLOR_GAMUT_E enColorGamut;
} ISP_COLORGAMMUT_INFO_S;

typedef enum arDYNAMIC_RANGE_E
{
    DYNAMIC_RANGE_SDR8 = 0,
    DYNAMIC_RANGE_SDR10,
    DYNAMIC_RANGE_HDR10,
    DYNAMIC_RANGE_HLG,
    DYNAMIC_RANGE_SLF,
    DYNAMIC_RANGE_XDR,
    DYNAMIC_RANGE_BUTT
} DYNAMIC_RANGE_E;


typedef enum arDATA_BITWIDTH_E
{
    DATA_BITWIDTH_8 = 8,
    DATA_BITWIDTH_10 = 10,
    DATA_BITWIDTH_12 = 12,
    DATA_BITWIDTH_14 = 14,
    DATA_BITWIDTH_16 = 16,
    DATA_BITWIDTH_OB_8 = 18,
    DATA_BITWIDTH_BUTT
} DATA_BITWIDTH_E;

typedef struct arVIDEO_FRAME_S
{
    AR_U32              u32FrameId;
    AR_U32              u32Width;
    AR_U32              u32Height;
    // attention, enField currently not used
    VIDEO_FIELD_E       enField;
    PIXEL_FORMAT_E      enPixelFormat;
    // attention, enVideoFormat currently not used
    VIDEO_FORMAT_E      enVideoFormat;
    // attention, enCompressMode currently not used
    COMPRESS_MODE_E     enCompressMode;
    // attention, enDynamicRange currently not used
    DYNAMIC_RANGE_E     enDynamicRange;     /**<note 暂不支持；*/
    // attention, enColorGamut currently not used
    COLOR_GAMUT_E       enColorGamut;
    // attention, u32HeaderStride currently not used
    AR_U32              u32HeaderStride[3];
    AR_U32              u32Stride[3];
    // attention, u32ExtStride currently not used
    AR_U32              u32ExtStride[3];

    // attention, u64HeaderPhyAddr currently not used
    AR_U64              u64HeaderPhyAddr[3];
    // attention, u64HeaderVirAddr currently not used
    AR_U64              u64HeaderVirAddr[3];
    AR_U64              u64PhyAddr[3];
    AR_U64              u64VirAddr[3];
    // attention, u64ExtPhyAddr currently not used
    AR_U64              u64ExtPhyAddr[3];
    // attention, u64ExtVirAddr currently not used
    AR_U64              u64ExtVirAddr[3];

    AR_S16              s16OffsetTop;       /**<note 暂不支持； top offset of show area */
    AR_S16              s16OffsetBottom;    /**<note 暂不支持； bottom offset of show area */
    AR_S16              s16OffsetLeft;      /**<note 暂不支持； left offset of show area */
    AR_S16              s16OffsetRight;     /**<note 暂不支持； right offset of show area */

    // attention, u32MaxLuminance currently not used
    AR_U32              u32MaxLuminance;    /**<note 暂不支持；*/
    // attention, u32MinLuminance currently not used
    AR_U32              u32MinLuminance;    /**<note 暂不支持；*/
    // attention, u32TimeRef currently not used
    AR_U32              u32TimeRef;
    AR_U64              u64PTS;
    // attention, u64PrivateData currently not used
    AR_U64              u64PrivateData;
    // attention, u32FrameFlag currently not used
    AR_U32              u32FrameFlag;     /* FRAME_FLAG_E, can be OR operation. */
    // attention, stSupplement currently not used
    VIDEO_SUPPLEMENT_S  stSupplement;     /**<note 暂不支持；*/

    AR_U32              u32HeaderLen[3];  /**<note 新加参数，cf50 ，头的使用的buffer的大小；*/
    AR_U32              u32Len[3]; /**<note 新加参数；buffer 使用的大小*/
    AR_FLOAT            fLineTime; /**<note 新加参数；sensor 采集一行的时间，单位s*/

    // NOTICE: Do not modify VIDEO_FRAME_S unless you add same changeds in STRU_SYS_VIDEO_FRAME of hal_sys.h and
    // make sure this change will not make any defactes.
    // attention, new added for video decoder used
    AR_S32              s32IndexFrameDisplay; /* This is a frame buffer index for the picture to be displayed at the moment */

    // attention, new added for video decoder used
    VIDEO_EXT_S         stExt;
} VIDEO_FRAME_S;

typedef struct arVIDEO_FRAME_INFO_S
{
    VIDEO_FRAME_S stVFrame;
    AR_U32        u32PoolId;
    MOD_ID_E      enModId;
} VIDEO_FRAME_INFO_S;



typedef struct arVB_CAL_CONFIG_S
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
}VB_CAL_CONFIG_S;


typedef struct arBITMAP_S
{
    PIXEL_FORMAT_E enPixelFormat;  /* Bitmap's pixel format */
    AR_U32 u32Width;               /* Bitmap's width */
    AR_U32 u32Height;              /* Bitmap's height */
    AR_VOID* ATTRIBUTE pData;      /* Address of Bitmap's data */
} BITMAP_S;

typedef struct arLDC_ATTR_S
{
    AR_BOOL    bAspect;                /* RW;Whether aspect ration  is keep */
    AR_S32     s32XRatio;                /* RW; Range: [0, 100], field angle ration of  horizontal,valid when bAspect=0.*/
    AR_S32     s32YRatio;                /* RW; Range: [0, 100], field angle ration of  vertical,valid when bAspect=0.*/
    AR_S32     s32XYRatio;                /* RW; Range: [0, 100], field angle ration of  all,valid when bAspect=1.*/
    AR_S32 s32CenterXOffset;        /* RW; Range: [-511, 511], horizontal offset of the image distortion center relative to image center.*/
    AR_S32 s32CenterYOffset;        /* RW; Range: [-511, 511], vertical offset of the image distortion center relative to image center.*/
    AR_S32 s32DistortionRatio;        /* RW; Range: [-300, 500], LDC Distortion ratio.When spread on,s32DistortionRatio range should be [0, 500]*/
} LDC_ATTR_S;


typedef struct arLDCV2_ATTR_S
{
    AR_S32 s32FocalLenX;                                            /*RW; focal length in horizontal direction, with 2 decimal numbers*/
    AR_S32 s32FocalLenY;                                            /*RW; focal length in vertical direction, with 2 decimal numbers*/
    AR_S32 s32CoorShiftX;                                           /*RW; coordinate of image center, with 2 decimal numbers*/
    AR_S32 s32CoorShiftY;                                           /*RW; Y coordinate of image center, with 2 decimal numbers*/
    AR_S32 as32SrcCaliRatio[SRC_LENS_COEF_SEG][SRC_LENS_COEF_NUM];  /*RW; lens distortion coefficients of the source image, with 5 decimal numbers*/
    AR_S32 s32SrcJunPt;                                             /*RW; Junction Point of the two segments*/
    AR_S32 as32DstCaliRatio[DST_LENS_COEF_SEG][DST_LENS_COEF_NUM];  /*RW; lens distortion coefficients, with 5 decimal numbers*/
    AR_S32 as32DstJunPt[DST_LENS_COEF_SEG - 1];                     /*RW; Junction Point of the three segments*/
} LDCV2_ATTR_S;

typedef enum arROTATION_VIEW_TYPE_E
{
    ROTATION_VIEW_TYPE_ALL      = 0, /* View all source Image,no lose*/
    ROTATION_VIEW_TYPE_TYPICAL  = 1, /* View from rotation Image with source size,same lose*/
    ROTATION_VIEW_TYPE_INSIDE   = 2, /* View with no black section,all  in dest Image*/
    ROTATION_VIEW_TYPE_BUTT,
} ROTATION_VIEW_TYPE_E;

typedef struct arROTATION_EX_S
{
    ROTATION_VIEW_TYPE_E enViewType;       /*Rotation mode*/
    AR_U32               u32Angle;         /*Rotation Angle:[0,360]*/
    AR_S32               s32CenterXOffset; /* Horizontal offset of the image distortion center relative to image center.*/
    AR_S32               s32CenterYOffset; /* Vertical offset of the image distortion center relative to image center.*/
    SIZE_S               stDestSize;       /*Dest size of any angle rotation*/
} ROTATION_EX_S;


typedef enum arWDR_MODE_E
{
    WDR_MODE_NONE = 0,
    WDR_MODE_BUILT_IN,
    WDR_MODE_QUDRA,

    WDR_MODE_2To1_LINE,
    WDR_MODE_2To1_FRAME,
    WDR_MODE_2To1_FRAME_FULL_RATE,

    WDR_MODE_3To1_LINE,
    WDR_MODE_3To1_FRAME,
    WDR_MODE_3To1_FRAME_FULL_RATE,

    WDR_MODE_4To1_LINE,
    WDR_MODE_4To1_FRAME,
    WDR_MODE_4To1_FRAME_FULL_RATE,

    WDR_MODE_BUTT,
} WDR_MODE_E;




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _AR_COMM_VIDEO_H_ */

