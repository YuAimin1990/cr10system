/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/

/** \addtogroup VENC
 *  @{
 */

/**
 * @file hal_vcodec_type_comm.h
 * @author  Artosyn Software Team
 * @date 4 Jun 2021
 * @brief 定义VENC和VDEC共用数据结构
 */

#ifndef _HAL_VCODEC_COMM_TYPE_H_
#define _HAL_VCODEC_COMM_TYPE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "hal_vb.h"
#include "hal_type.h"
#include "ar_comm_vb.h"
//=============================================================================

// Macro definition

//=============================================================================

/**
* @note  编码和解码的设备文件名
*/
#define AR_HAL_VENC_CTRL_DEV    "hal_venc"
#define AR_HAL_VENC_DEV         "venc"
#define AR_HAL_VDEC_CTRL_DEV    "hal_vdec"
#define AR_HAL_VDEC_DEV         "vdec"
#define AR_MAX_DEV_NUM          (255)
#define AR_MAX_DEV_NAME_LEN     (64)
#define AR_MAX_EXT_PROC_NUM     (4)
#define AR_MAX_H26X_INST_NUM    (32)
#define AR_MAX_JPEG_INST_NUM    (8)
#define AR_MAX_INST_NUM         (AR_MAX_H26X_INST_NUM+AR_MAX_JPEG_INST_NUM)
#define AR_MAX_NUM_TEMPORAL_LAYER          (7)
#define AR_MAX_GOP_NUM                     (8)
#define AR_MAX_USER_DATA_NUM    16
#define AR_VIDEO_JPEG_INPUT_MSG_QUEUE_MAX_NR   4
#define AR_VIDEO_H26X_INPUT_MSG_QUEUE_MAX_NR   10
#define AR_VIDEO_TASK_MSG_QUEUE_MAX_NR         128 //default vale of u32VencTaskQueueSize
#define AR_VIDEO_IRQ_MSG_QUEUE_MAX_NR          128 //default value of u32VencIrqQueueSize
#define AR_VIDEO_DONE_MSG_QUEUE_MAX_NR         512 //default value of u32VencDoneQueueSize
#define AR_VIDEO_OUTPUT_MSG_QUEUE_MAX_NR       128 //default vale of u32VencOutQueueSize
#define AR_VIDEO_USER_DATA_BUF_SIZE            8192

#define REPEAT_PREVIOUS_FRAME 1

/**
* @note  定义编码扩展参数比特位定义。
*/
typedef enum {
    AR_VIDEO_CODEC_PARAM_HAL_EXT_INSTANCE_PARALLESIM = 1, /**< 编码实例之间的并行编码开关, 26x有效，jpeg不支持*/
} ENUM_HAL_VENC_EXTENSION_FLAGS;
/**
* @note  定义编解码类型。
*/
typedef enum
{
    HAL_VCODEC_TYPE_INVALID = 0,
    HAL_VCODEC_TYPE_HEVC,
    HAL_VCODEC_TYPE_AVC,
    HAL_VCODEC_TYPE_JPEG,
    HAL_VCODEC_TYPE_MJPEG,
    HAL_VCODEC_TYPE_MAX
} ENUM_VCODEC_TYPE;
#define IS_VCODEC_INVAILD(eType)        ((eType <= HAL_VCODEC_TYPE_INVALID) || (eType >= HAL_VCODEC_TYPE_MAX))
/**
* @note 定义编码的帧类型
*/
typedef enum
{
    HAL_VCODEC_PIC_TYPE_I            = 0, /**< I picture */
    HAL_VCODEC_PIC_TYPE_KEY          = 0, /**< KEY frame for AV1*/
    HAL_VCODEC_PIC_TYPE_P            = 1, /**< P picture */
    HAL_VCODEC_PIC_TYPE_INTER        = 1, /**< Inter frame for AV1*/
    HAL_VCODEC_PIC_TYPE_B            = 2, /**< B picture (except VC1) */
    HAL_VCODEC_PIC_TYPE_REPEAT       = 2, /**< Repeat frame (VP9 only) */
    HAL_VCODEC_PIC_TYPE_AV1_INTRA    = 2, /**< Intra only frame (AV1 only) */
    HAL_VCODEC_PIC_TYPE_VC1_BI       = 2, /**< VC1 BI picture (VC1 only) */
    HAL_VCODEC_PIC_TYPE_VC1_B        = 3, /**< VC1 B picture (VC1 only) */
    HAL_VCODEC_PIC_TYPE_D            = 3, /**< D picture in MPEG2 that is only composed of DC coefficients (MPEG2 only) */
    HAL_VCODEC_PIC_TYPE_S            = 3, /**< S picture in MPEG4 that is an acronym of Sprite and used for GMC (MPEG4 only)*/
    HAL_VCODEC_PIC_TYPE_AVS2_F       = 3, /**< F picture in AVS2 */
    HAL_VCODEC_PIC_TYPE_AV1_SWITCH   = 3, /**< Switch frame (AV1 only) */
    HAL_VCODEC_PIC_TYPE_VC1_P_SKIP   = 4, /**< VC1 P skip picture (VC1 only) */
    HAL_VCODEC_PIC_TYPE_MP4_P_SKIP_NOT_CODED = 4, /**< Not Coded P Picture in MPEG4 packed mode */
    HAL_VCODEC_PIC_TYPE_AVS2_S       = 4, /**< S picture in AVS2 */
    HAL_VCODEC_PIC_TYPE_IDR          = 5, /**< H.264/H.265 IDR picture */
    HAL_VCODEC_PIC_TYPE_AVS2_G       = 5, /**< G picture in AVS2 */
    HAL_VCODEC_PIC_TYPE_AVS2_GB      = 6, /**< GB picture in AVS2 */
    HAL_VCODEC_PIC_TYPE_MAX               /**< No Meaning */
} ENUM_PIC_TYPE;
/**
* @note  H264和H265编码器输出的码流基础信息结构体。
*/
typedef struct
{
    AR_S32 s32DevNum;
    AR_S32 s32Idxs[AR_MAX_DEV_NUM];
} STRU_VENC_CTRL_INFO;
/**
* @note  H264和H265编码器输出的码流基础信息结构体。
*/
typedef struct
{
    ENUM_PIC_TYPE ePicType;       /**< 编码帧类型 */
    AR_S32 s32PicSkipped;         /**< 当前编码帧是否使能skip模式 */
    AR_S32 s32NumOfSlices;        /**< 当前编码图像包含多少个条带数据 */
    AR_S32 s32NumOfIntra;         /**< 当前编码图像采用Intra预测模式的CU块 */
    AR_S32 s32NumOfMerge;         /**< 当前编码图像采用merge预测模式的8x8 CU块 */
    AR_S32 s32NumOfSkipBlock;     /**< 当前编码图像采用skip预测模式的8x8 CU块 */
    AR_S32 s32AvgCtuQp;           /**< 当前编码图像的平均QP */
    AR_S32 s32EncGopPicIdx;       /**< 当前编码图像所在的GOP索引 */
    AR_S32 s32EncPicPoc;          /**< 当前编码图像的POC(picture order count)值 */
    AR_S32 s32FrameCycle;         /**< 统计当前帧编码的时间 */
    AR_S32 s32ReleaseSrcFlag;     /**< 当前编码释放的原始图像buffer索引;bit[0-15]每bit代表一个buffer */
    AR_S32 s32EncSrcIdx;          /**< 当前编码帧用的原始图像buffer索引;bit[0-15]每bit代表一个buffer */
    AR_S32 s32EncPicCnt;          /**< 统计已编码图像的帧数 */
    AR_U32 u32ChangeParamCnt;     /**< 统计通道属性或参数被设置的次数 */
    AR_U32 s32StartQp;            /**< 编码当前帧的startQp值 */
    //AR_U32 u32BitstreamSize;      /**< 统计已编码图像的总大小 */
} STRU_VENC_H26X_STREAM_INFO;
/**
* @note  JPEG和MJPEG编码器输出的码流基础信息结构体。
*/
typedef struct
{
    AR_S32 s32QpCoef;           /**< 当前编码帧的量化参数 */
    AR_U32 u32ChangeParamCnt;     /**< 统计通道属性或参数被设置的次数 */
} STRU_VENC_MJPEG_JPEG_STREAM_INFO;

/**
* @note  HEVC编码器输出的码流高级信息结构体。
*/
typedef struct
{
    AR_U32  u32PicDistortionLow;  /**< The 32bit lowest difference value(SSD) between an original block and a reconstructed block in the encoded picture. It can be an indicator for image quality. Host cannot tune quality by using this value, because this value is not an input parameter, but a reporting value. */
    AR_U32  u32PicDistortionHigh; /**< The 32bit highest difference value(SSD) between an original block and a reconstructed block in the encoded picture. It can be an indicator for image quality. Host cannot tune quality by using this value, because this value is not an input parameter, but a reporting value. */
} STRU_VENC_H26X_STREAM_ADVANCE_INFO;

/**
* @note  编码器输出的码流信息结构体。
*/
#define HAL_MAX_SEGMENT_NUM    (5)
typedef struct
{
    ENUM_VCODEC_TYPE eStreamType; /**< 码流类型 */
    AR_S32 s32FrameIndex;          /**< 原始图像数据序号 */
    AR_S32 s32FrameSegmentNum;     /**< 当前帧包含的码流段数 */
    AR_U64 u64BitstreamBuffers[HAL_MAX_SEGMENT_NUM]; /**< 当前帧编码输出码流段的物理地址,ringbuffer到尾巴会增加一段地址,需要手动插头会增加一个段 */
    AR_U64 u64BitstreamBuffersVA[HAL_MAX_SEGMENT_NUM]; /**< 当前帧编码输出码流段的物理地址,ringbuffer到尾巴会增加一段地址,需要手动插头会增加一个段 */
    AR_S32 s32EncPicBytes[HAL_MAX_SEGMENT_NUM];      /**< 当前帧编码码流段的大小,ringbuffer到码流结尾会增加一个段长度 */
    AR_U64 u64Pts;                /**< 当前帧的显示时间戳，从数据buffer拿到的值 */
    AR_U64 u64EncodingTimeUs;     /**< 当前帧的编码时间，根据BPU Clock 计算,tick*cycle*256/BPUMhz */
    AR_S32 s32ErrorReason;        /**< 编码错误的原因 */
    AR_S32 s32WarnInfo;           /**< 编码预警信息 */
    AR_S32 s32MsTimeout;          /**< 获取解码帧方式;-1:阻塞 0:非阻塞 正值:超时时间,以ms为单位 */
    union {
        STRU_VENC_H26X_STREAM_INFO stH26xStreamInfo;
        STRU_VENC_MJPEG_JPEG_STREAM_INFO stJpegMjpegInfo;
    };
    STRU_VENC_H26X_STREAM_ADVANCE_INFO stH26xAdvanceStreamInfo;
} STRU_VENC_STREAM;

/**
 * @note 定义编码输入和解码输出的图像数据格式和像素数据排列格式
 */
 typedef enum
 {
    HAL_VCODEC_FORMAT_ERR           = -1,
    HAL_VCODEC_FORMAT_420           = 0 ,    /* 8bit */
    HAL_VCODEC_FORMAT_422           = 1 ,    /* 8bit */
    HAL_VCODEC_FORMAT_224           = 2 ,    /* 8bit */
    HAL_VCODEC_FORMAT_444           = 3 ,    /* 8bit */
    HAL_VCODEC_FORMAT_400           = 4 ,    /* 8bit */

                                  /* Little Endian Perspective    */
                                  /*     | addr 0 | addr 1  |    */
    HAL_VCODEC_FORMAT_420_P10_16BIT_MSB = 5, /* lsb |000000xx|xxxxxxxx | msb */
    HAL_VCODEC_FORMAT_420_P10_16BIT_LSB = 6,    /* lsb |xxxxxxx |xx000000 | msb */
    HAL_VCODEC_FORMAT_420_P10_32BIT_MSB = 7,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */
    HAL_VCODEC_FORMAT_420_P10_32BIT_LSB = 8,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */

                                  /* 4:2:2 packed format */
                                  /* Little Endian Perspective     */
                                  /*     | addr 0  | addr 1  |     */
    HAL_VCODEC_FORMAT_422_P10_16BIT_MSB = 9,    /* lsb |000000xx |xxxxxxxx | msb */
    HAL_VCODEC_FORMAT_422_P10_16BIT_LSB = 10,    /* lsb |xxxxxxxx |xx000000 | msb */
    HAL_VCODEC_FORMAT_422_P10_32BIT_MSB = 11,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */
    HAL_VCODEC_FORMAT_422_P10_32BIT_LSB = 12,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */

    HAL_VCODEC_FORMAT_YUYV               = 13,    /**< 8bit packed format : Y0U0Y1V0 Y2U1Y3V1 ... */
    HAL_VCODEC_FORMAT_YUYV_P10_16BIT_MSB = 14,    /* lsb |000000xxxxxxxxxx | msb */ /**< 10bit packed(YUYV) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_YUYV_P10_16BIT_LSB = 15,    /* lsb |xxxxxxxxxx000000 | msb */ /**< 10bit packed(YUYV) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_YUYV_P10_32BIT_MSB = 16,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */ /**< 10bit packed(YUYV) format(3Pixel=4Byte) */
    HAL_VCODEC_FORMAT_YUYV_P10_32BIT_LSB = 17,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */ /**< 10bit packed(YUYV) format(3Pixel=4Byte) */

    HAL_VCODEC_FORMAT_YVYU               = 18,    /**< 8bit packed format : Y0V0Y1U0 Y2V1Y3U1 ... */
    HAL_VCODEC_FORMAT_YVYU_P10_16BIT_MSB = 19,    /* lsb |000000xxxxxxxxxx | msb */ /**< 10bit packed(YVYU) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_YVYU_P10_16BIT_LSB = 20,    /* lsb |xxxxxxxxxx000000 | msb */ /**< 10bit packed(YVYU) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_YVYU_P10_32BIT_MSB = 21,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */ /**< 10bit packed(YVYU) format(3Pixel=4Byte) */
    HAL_VCODEC_FORMAT_YVYU_P10_32BIT_LSB = 22,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */ /**< 10bit packed(YVYU) format(3Pixel=4Byte) */

    HAL_VCODEC_FORMAT_UYVY               = 23,    /**< 8bit packed format : U0Y0V0Y1 U1Y2V1Y3 ... */
    HAL_VCODEC_FORMAT_UYVY_P10_16BIT_MSB = 24,    /* lsb |000000xxxxxxxxxx | msb */ /**< 10bit packed(UYVY) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_UYVY_P10_16BIT_LSB = 25,    /* lsb |xxxxxxxxxx000000 | msb */ /**< 10bit packed(UYVY) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_UYVY_P10_32BIT_MSB = 26,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */ /**< 10bit packed(UYVY) format(3Pixel=4Byte) */
    HAL_VCODEC_FORMAT_UYVY_P10_32BIT_LSB = 27,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */ /**< 10bit packed(UYVY) format(3Pixel=4Byte) */

    HAL_VCODEC_FORMAT_VYUY               = 28,    /**< 8bit packed format : V0Y0U0Y1 V1Y2U1Y3 ... */
    HAL_VCODEC_FORMAT_VYUY_P10_16BIT_MSB = 29,    /* lsb |000000xxxxxxxxxx | msb */ /**< 10bit packed(VYUY) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_VYUY_P10_16BIT_LSB = 30,    /* lsb |xxxxxxxxxx000000 | msb */ /**< 10bit packed(VYUY) format(1Pixel=2Byte) */
    HAL_VCODEC_FORMAT_VYUY_P10_32BIT_MSB = 31,    /* lsb |00xxxxxxxxxxxxxxxxxxxxxxxxxxx| msb */ /**< 10bit packed(VYUY) format(3Pixel=4Byte) */
    HAL_VCODEC_FORMAT_VYUY_P10_32BIT_LSB = 32,    /* lsb |xxxxxxxxxxxxxxxxxxxxxxxxxxx00| msb */ /**< 10bit packed(VYUY) format(3Pixel=4Byte) */
    HAL_VCODEC_FORMAT_SEMIPLANAR_420     = 33,
    HAL_VCODEC_FORMAT_SEMIPLANAR_422     = 34,

    HAL_VCODEC_FORMAT_MAX,
} ENUM_FB_FORMAT;

/**
 * @note  定义图像数据的打包格式
 */
typedef enum {
    HAL_VCODEC_NOT_PACKED = 0,
    HAL_VCODEC_PACKED_YUYV,
    HAL_VCODEC_PACKED_YVYU,
    HAL_VCODEC_PACKED_UYVY,
    HAL_VCODEC_PACKED_VYUY,
} ENUM_PACKED_FORMAT;
/**
 * @note  定义图像数据压缩格式
 */
typedef enum {
    HAL_VCODEC_LINEAR_FRAME_MAP                            = 0,  /**< Linear frame map type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSLESS_8BIT      = 11, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed framebuffer type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSLESS_10BIT     = 12, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed framebuffer type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSY              = 13, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed framebuffer type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSLESS_422_8BIT  = 14, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed 4:2:2 framebuffer type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSLESS_422_10BIT = 15, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed 4:2:2 framebuffer type */
    HAL_VCODEC_COMPRESSED_FRAME_MAP_V50_LOSSY_422          = 16, /**< CFRAME50(Chips&Media Frame Buffer Compression) compressed 4:2:2 framebuffer type */
    HAL_VCODEC_TILED_MAP_TYPE_MAX
} ENUM_TILEDMAP_TYPE;
/**
 * @note  定义图像色域空间
 */
typedef enum
{
    HAL_VCODEC_MATRIX_BT601_FULL,
    HAL_VCODEC_MATRIX_BT601_LIMIT,
    HAL_VCODEC_MATRIX_BT709_FULL,
    HAL_VCODEC_MATRIX_BT709_LIMIT,
    HAL_VCODEC_MATRIX_NODATA_FULL,
    HAL_VCODEC_MATRIX_NODATA_LIMIT,
    HAL_VCODEC_MATRIX_MAX,
} ENUM_VIDEO_MATRIX;
/**
* @note  编码器的状态信息结构体
*/
typedef struct
{
    AR_U32 u32LeftPicsInQeueu;  /**< 缓存在编码队列中待编码的图像数 */
    AR_U32 u32LeftStreamBytes;  /**< 剩余待取走的码流字节数 */
    AR_U32 u32LeftStreamFrames; /**< 剩余待取走的码流帧数 */
    AR_U32 u32CurPacks;         /**< 当前帧包含的slice数,如果按帧就是1 */
    AR_U32 u32LeftRecvPics;     /**< 剩余需要接收的图像数 */
    AR_U32 u32LeftEncPics;      /**< 剩余待接收并编码的图像数 */
    AR_U32 u32CalcBitRate;      /**< 实时统计的码率 */
} STRU_VENC_STATUS;
/**
* @note 定义原始图像数据的地址，大小，格式，stride等信息。
*/
typedef struct {
    AR_U32 eUid;           /**< vb's Uid */
    AR_U64 u64AddrBs;      /**< 存放码流的物理地址 */
    AR_U64 u64VirAddrBs;   /**< 存放码流的虚拟地址 */
    AR_U32 u32BsSize;      /**< 码流buffer的大小 note:确保足够保存一帧编码数据，不然数据会被回踩 */
    AR_U64 u64AddrY;       /**< Y分量的物理地址 note:必须按16对齐 */
    AR_U64 u64AddrCb;      /**< Cb分量的物理地址 note:必须按16对齐 */
    AR_U64 u64AddrCr;      /**< Cr分量的物理地址 note:必须按16对齐 */
    AR_U64 u64OffsetTblAddrY;  /**< Cframe50格式Y分量的物理地址 note:必须按16对齐 */
    AR_U64 u64OffsetTblAddrCb; /**< Cframe50格式Cb分量的物理地址 note:必须按16对齐 */
    AR_U64 u64OffsetTblAddrCr; /**< Cframe50格式Cr分量的物理地址 note:必须按16对齐 */
    //AR_U32 u32YSize;   /**< Y分量的大小 */
    //AR_U32 u32CbSize;  /**< Cb分量的大小 */
    //AR_U32 u32CrSize;  /**< Cr分量的大小 */
    AR_S32 s32CbCrInterleave; /**< CbCr分量是否交织存放 */
    //AR_S32 s32NV21; /**< 0: NV12 1: NV21. */
    AR_U64 u64Pts;       /**< The presentation Timestamp (PTS) of input source */
    AR_S32 s32Endian;    /**< 0：小端对齐 1：大端对齐 2:32bit小端 3:32bit大端 16~31:128bit对齐方式 */
    AR_S32 s32FrameIndex;          /**< 原始图像数据序号 */
    ENUM_TILEDMAP_TYPE eMapType;  /**< 原始图像数据摆放方式 */
    AR_S32 s32Stride;           /**< 原始图像数据水平方向stride */
    AR_S32 s32ChromaStride;     /**< 原始图像数据水平方向chroma分量的stride */
    AR_S32 s32Width;            /**< 原始图像数据宽度 note:JPEG支持每帧分辨率不一样 */
    AR_S32 s32Height;           /**< 原始图像数据的高度 note:JPEG支持每帧分辨率不一样*/
    //AR_S32 s32LumaBitDepth;     /**< 亮度的位深 */
    //AR_S32 s32ChromaBitDepth;   /**< 色度的位深 */
    ENUM_FB_FORMAT  eFormat;    /**< YUV格式信息 */
    //AR_S32 s32EequenceNo;       /**< 编码序列号 */
    AR_S32 s32SrcEndFlag;  /**< 是否是最后帧 */
    AR_S32 s32MsTimeout;          /**< 发送图像超时时间MS。范围[-1, +∞], -1：阻塞 0：非阻塞 大于0：超时时间 */
    AR_S32 s32RetryCnt;    /**当前帧retry的次数*/
}STRU_VIDEO_FRAME_INFO;
/**
* @note  定义h264的用户自定义map每个mb的相关参数说明1byte每个MB。
*/
typedef union {
    struct {
      AR_U8  mb_force_mode  : 2;/**< 每个MB的mode;范围[0,2]; 0:不做force 1: force skip(mv=0,coef=0) 2:force intra */
      AR_U8  mb_qp          : 6;/**< 每个MB的qp;范围[0,51],如果rc开的时候码控会根据roi与非roi区域的平均qp算一个最终的qp */
  } field;
} STRU_AVC_CUSTOM_MAP_ITEM;
/**
* @note  定义h265的用户自定义map每个ctu的相关参数说明8byte每个CTU。
*/
typedef union {
    struct {
        AR_U32  ctu_force_mode  : 2;/**< 每个CTU的mode;范围[0,2]; 0:不做force 1: force skip(mv=0,coef=0) 2:force intra */
        AR_U32  ctu_coeff_drop  : 1;/**< 每个MB的mode;范围[0,1]; 0:不drop,画质好 1:drop,节省码率画质差 */
        AR_U32  reserved        : 5; //[ 7: 3]
        AR_U32  sub_ctu_qp_0    : 6;/**< 每个CTU的第一个CU的qp;范围[0,51],如果rc开的时候码控会根据roi与非roi区域的平均qp算一个最终的qp */
        AR_U32  sub_ctu_qp_1    : 6;/**< 每个CTU的第二个CU的qp;范围[0,51],如果rc开的时候码控会根据roi与非roi区域的平均qp算一个最终的qp */
        AR_U32  sub_ctu_qp_2    : 6;/**< 每个CTU的第三个CU的qp;范围[0,51],如果rc开的时候码控会根据roi与非roi区域的平均qp算一个最终的qp */
        AR_U32  sub_ctu_qp_3    : 6;/**< 每个CTU的第四个CU的qp;范围[0,51],如果rc开的时候码控会根据roi与非roi区域的平均qp算一个最终的qp */

        AR_U32  lambda_sad_0    : 8;/**< 每个CTU的第一个CU计算sad的lambda;范围[0,127], RD cost = distortion + lambda * (rate + delta rate) */
        AR_U32  lambda_sad_1    : 8;/**< 每个CTU的第二个CU计算sad的lambda;范围[0,127], RD cost = distortion + lambda * (rate + delta rate) */
        AR_U32  lambda_sad_2    : 8;/**< 每个CTU的第三个CU计算sad的lambda;范围[0,127], RD cost = distortion + lambda * (rate + delta rate) */
        AR_U32  lambda_sad_3    : 8;/**< 每个CTU的第四个CU计算sad的lambda;范围[0,127], RD cost = distortion + lambda * (rate + delta rate) */
    } field;
} STRU_HEVC_CUSTOM_MAP_ITEM;

/**
* @note  定义h264&h265的用户自定义map的相关参数。
*/
typedef struct {
    AR_S32 s32RoiAvgQp;  /**< 设置感兴趣区域的平均QP,范围:[0,51] */
    AR_S32 s32CustomRoiMapEnable; /**< 是否使能自定义感兴趣区域QP表 */
    AR_S32 s32CustomLambdaMapEnable; /**< 是否使能自定义Lambda表 */
    AR_S32 s32CustomModeMapEnable;   /**< 是否使能自定义mode表 */
    AR_S32 s32CustomCoefDropEnable;  /**< 是否使能TQ后清除所有系数 */
    AR_U64 u64AddrCustomMap; /**< 映射表物理地址; note:每个CTU占用8个byte */
} STRU_CUSTOM_MAP_PARAM;
/**
* @note 定义SEI数据结构体
*/
typedef struct {
    AR_U32 u32PrefixSeiNalEnable;  /**< 是否使能前置SEI NAL */
    AR_U32 u32PrefixSeiDataSize;   /**< 前置SEI的总长度 */
    AR_U64 u64PrefixSeiNalAddr;    /**< 前置SEI NAL的物理地址 */
    AR_U32 u32SuffixSeiNalEnable;  /**< 是否使能后置SEI NAL */
    AR_U32 u32SuffixSeiDataSize;   /**< 后置SEI的总长度 */
    AR_U64 u64SuffixSeiNalAddr;    /**< 前置SEI NAL的物理地址 */
} STRU_SEI_DATA;
/**
* @note 定义编码帧的高级参数结构体
*/
typedef struct {
    AR_S32 s32SkipPicture;          /**< 配置当前帧是否skip */
    AR_S32 s32ForcePicQpEnable;     /**< 是否使能强制量化参数 */
    AR_S32 s32ForcePicQpI;          /**< I帧的强制量化参数;范围[0-51] */
    AR_S32 s32ForcePicQpP;          /**< P帧的强制量化参数;范围[0-51] */
    AR_S32 s32ForcePicQpB;          /**< B帧的强制量化参数;范围[0-51] */
    AR_S32 s32ForcePicTypeEnable;   /**< 强制配置帧类型 */
    AR_S32 s32ForcePicType;         /**< 配置当前帧的类型；0:I帧 1:P帧 2:B帧 3:IDR帧 */
    AR_S32 s32ImplicitHeaderEncode; /**< 是否使能在IDR帧前默认编头信息 */
    AR_U32 u32UseCurSrcAsLongtermPic; /**< 是否使能当前帧作为后续帧的长期参考帧 */
    AR_U32 u32UseLongtermRef;         /**< 是否使能当前帧参考长期参考帧 */
    AR_U32 u32ForceAllCtuCoefDropEnable;  /**< 量化编码后reset所有的参数到0 */
} STRU_PIC_ADV_PARAM;
/**
 * @note 配置编码器的帧级动态参数和图像数据信息。
*/
 typedef struct {
    STRU_VIDEO_FRAME_INFO  stSourceFrame; /**< 原始图像数据结构体 */
    STRU_PIC_ADV_PARAM stPicAdvParam; /**< 编码帧的高级参数结构体 */
    STRU_CUSTOM_MAP_PARAM stCustomMapOpt; /**< 自定义map参数结构体 */
    STRU_SEI_DATA stSeiData;        /**< SEI数据结构体 */
} STRU_USER_FRAME_INFO;
/*
* @note 定义PPS中可以动态修改的参数
*/
typedef struct
{
    AR_S32 s32ConstIntraPredFlag;            /**< 是否使能帧内预测参考帧间块;note:请参阅H.265&H.264规范 */
    AR_S32 s32DisableDeblk;                  /**< 是否不使能环路去块滤波 */
    AR_S32 s32LfCrossSliceBoundaryEnable;    /**< 是否使能跨越条带边界的滤波 */
    AR_S32 s32BetaOffsetDiv2;                /**< 去块滤波参数BetaOffsetDiv2,范围:[-6,6] */
    AR_S32 s32TcOffsetDiv2;                  /**< 去块滤波参数TcOffsetDiv3,范围:[-6,6] */
    AR_S32 s32ChromaCbQpOffset;              /**< Cb相对于亮度QP的偏移,范围:[-11,11] */
    AR_S32 s32ChromaCrQpOffset;              /**< Cr相对于亮度QP的偏移,范围:[-11,11] */
    AR_S32 s32Transform8x8Enable;            /**< 是否使能8x8变换;note:只适用于H.264 */
    AR_S32 s32EntropyCodingMode;             /**< 熵编码方式：0:CAVLC 1:CABAC;note:只适用于H.264 */
    AR_S32 s32WeightPredEnable;              /**< 是否使能加权平均预测 */
} STRU_VENC_PPS_DPARAM;
/**
* @note 定义条带分割相关的参数。
*/
typedef struct
{
    AR_S32 s32IndependSliceMode;      /**< 是否使能非独立条带划分; note:只对H.265有效 */
    AR_S32 s32IndependSliceModeArg;   /**< 每个非独立条带包含多少MB; note:只对H.265有效 */
    AR_S32 s32DependSliceMode;        /**< 是否使能独立条带划分; note:只对H.265有效 */
    AR_S32 s32DependSliceModeArg;     /**< 每个非独立条带包含多少CTU; note:只对H.265有效 */
    AR_S32 s32AvcSliceMode;        /**< 是否使能条带划分; note:只对H.264有效 */
    AR_S32 s32AvcSliceArg;         /**< 每个条带包含多少MB; note:只对H.264有效 */
} STRU_SLICE_SPLIT_SPARAM;
typedef STRU_SLICE_SPLIT_SPARAM STRU_SLICE_SPLIT_DPARAM;
/**
* @note 定义RDO相关的动态参数。
*/
typedef struct
{
    AR_S32 s32RdoSkip;             /**< 是否使能跳过RDO */
    AR_S32 s32MaxNumMerge;         /**< RDO中merge mode的候选个数,范围:[0,2] */
    AR_S32 s32IntraNxNEnable;      /**< 是否使能 intra NxN PUs */
    AR_S32 s32CoefClearDisable;    /**< 是否不使能丢弃P和B帧的变换系数,1:所有0系数块不参与RDO */
    AR_U32 u32CustomLambdaEnable;  /**< 是否使能自定义lambda表 */
    AR_U32 u32CustomMDEnable;      /**< 是否使能自定义mode decision */
    AR_S32 s32LambdaScalingEnable; /**< 是否使能自定义lambda变换表 */
} STRU_RDO_DPARAM;
/**
* @note 定义RDO相关的静态参数。
*/
typedef struct
{
    AR_S32 s32UseRecommendEncParam;        /**< 范围：0：用户自定义编码器参数 1：画质优先的编码器默认参数 2：编码速度和画质都适中的编码器参数 3：编码速度优先的参数 */
    AR_S32 s32CuSizeMode;                  /**< 编码Cu size模式；bit[0]:8x8 bit[1]:16x16 bit[2]:32x32 */
    AR_U32 u32MonochromeEnable;            /**< 是否使能黑白编码 */
    STRU_RDO_DPARAM stRdoDParam;
} STRU_RDO_SPARAM;
/**
* @note 定义RC相关的一部分动态参数。
*/
typedef struct
{
    // (rcEnable, cuLevelRc, bitAllocMode, RoiEnable, RcInitQp can't be changed while encoding)
    AR_S32 s32HvsQPEnable;                    /**< 是否使能CU级QP调整 */
    AR_S32 s32HvsQpScale;                     /**< CU级QP调整的系数,范围:[0, 15] */
    AR_S32 s32MbLevelRcEnable;                /**< 是否使能MB级的码控;只针对H.264 */
} STRU_RC_CU_DPARAM;
/**
* @note 定义支持动态修改的intra编码参数。
*/
typedef struct
{
    AR_S32 s32MinQpI;            /**< I帧最小QP,范围:[0, 51] */
    AR_S32 s32MaxQpI;            /**< I帧最大QP,范围:[0, 51] */
    AR_S32 s32HvsMaxDeltaQp;     /**< HVS的最大的QP变化,范围:[0, 51] */
    AR_S32 s32IntraQpOffset;     /**< intra帧相对于其他帧的QP偏移量 范围:[-9, 9] */
} STRU_RC_INTRA_DPARAM;
/**
* @note 定义支持动态修改的inter编码参数。
*/
typedef struct
{
    AR_S32 s32MinQpP;            /**< P帧最小QP,范围: [0, 51] */
    AR_S32 s32MaxQpP;            /**< P帧最大QP,范围: [0, 51] */
    AR_S32 s32MinQpB;            /**< B帧最小QP,范围: [0, 51] */
    AR_S32 s32MaxQpB;            /**< B帧最大QP,范围: [0, 51] */
} STRU_RC_INTER_DPARAM;
/**
* @note 定义码控相关的支持动态修改的相关参数
*/
typedef struct
{
    AR_S32 s32CVBR;                     /**< 是否使能CVBR, 主要在静止场景主动降低码率 */
    AR_S32 s32SsdThresholdByPixel;      /**< 静止判断的阈值基于每个pixel的SSD,范围:[0, 10000]; note:动态参数 */
    AR_S32 s32DeltaSsdThresholdByPixel; /**< 静止判断的阈值基于每个pixel的SSD,范围:[0, 10000]; note:动态参数 */
    AR_S32 s32StillBitRateRatio;        /**< 静止场景后码率切换到原来的百分比,范围:[10, 100]; note:动态参数 */
    AR_S32 s32StillCalcWindow;          /**< 判断是否进入静止场景的滑动窗口大小,范围:[1, 100]; note:静态参数 */
} STRU_RC_CVBR_DPARAM;
/**
* @note 定义支持动态修改的智能背景检测参数。
*/
typedef struct
{
    AR_U32 u32BgThrDiff;           /**< 静止到运动的最大门限值,范围:[0,512] */
    AR_U32 u32BgThrMeanDiff;       /**< 静止到运动的平均门限值,范围:[0,256] */
    AR_U32 u32BgLambdaQp;          /**< 对背景使用的最小 lambda QP,范围:[0,51] */
    AR_S32 s32BgDeltaQp;           /**< 背景和前景lambda QP差值,范围:[0,32] */
} STRU_BG_DPARAM;
/**
* @note 定义静态的智能背景检测参数。
*/
typedef struct
{
    AR_U32 u32BgDetectEnable;          /**< 是否使能背景检测 */
    STRU_BG_DPARAM stBgDParam;
} STRU_BG_SPARAM;
/**
* @note 定义支持动态修改的降噪参数。
*/
typedef struct
{
    AR_U32 u32NrYEnable;     /**< 是否使能Y分量的降噪 */
    AR_U32 u32NrCbEnable;    /**< 是否使能Cb分量的降噪 */
    AR_U32 u32NrCrEnable;    /**< 是否使能Cr分量的降噪 */
    AR_U32 u32NrNoiseEstEnable;  /**< 是否使能自动降噪 */
    AR_U32 u32NrNoiseSigmaY;  /**< Y分量的噪声强度 note:u32NrNoiseEstEnable为0时有效,范围:[0,255] */
    AR_U32 u32NrNoiseSigmaCb; /**< Cb分量的噪声强度 note:u32NrNoiseEstEnable为0时有效,范围:[0,255] */
    AR_U32 u32NrNoiseSigmaCr; /**< Cr分量的噪声强度 note:u32NrNoiseEstEnable为0时有效,范围:[0,255] */

    AR_U32 u32NrIntraWeightY;  /**< 帧内参考Y分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
    AR_U32 u32NrIntraWeightCb; /**< 帧内参考Cb分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
    AR_U32 u32NrIntraWeightCr; /**< 帧内参考Cr分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
    AR_U32 u32NrInterWeightY;  /**< 帧间参考Y分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
    AR_U32 u32NrInterWeightCb; /**< 帧间参考Cb分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
    AR_U32 u32NrInterWeightCr; /**< 帧间参考Cr分量的降噪强度;范围(0 ~ 31), 降噪强度=自动强度*nrIntraWeight/4 */
} STRU_NR_DPARAM;
typedef STRU_NR_DPARAM STRU_NR_SPARAM;
/**
* @note 定义支持动态修改的用户自定义的HEVC模式判别编码参数。
*/
typedef struct
{
    AR_S32 pu04DeltaRate;       /**< A value which is added to the total cost of 4x4 blocks,范围:[0 ~ 255] */
    AR_S32 pu08DeltaRate;       /**< A value which is added to the total cost of 8x8 blocks,范围:[0 ~ 255] */
    AR_S32 pu16DeltaRate;       /**< A value which is added to the total cost of 16x16 blocks,范围:[0 ~ 255] */
    AR_S32 pu32DeltaRate;       /**< A value which is added to the total cost of 32x32 blocks,范围:[0 ~ 255] */
    AR_S32 pu04IntraPlanarDeltaRate; /**< A value which is added to rate when calculating cost(=distortion + rate) in 4x4 Planar intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu04IntraDcDeltaRate;     /**< A value which is added to rate when calculating cost (=distortion + rate) in 4x4 DC intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu04IntraAngleDeltaRate;  /**< A value which is added to rate when calculating cost (=distortion + rate) in 4x4 Angular intra prediction mode,范围:[0 ~ 255]  */
    AR_S32 pu08IntraPlanarDeltaRate; /**< A value which is added to rate when calculating cost(=distortion + rate) in 8x8 Planar intra prediction mode,范围:[0 ~ 255]*/
    AR_S32 pu08IntraDcDeltaRate;     /**< A value which is added to rate when calculating cost(=distortion + rate) in 8x8 DC intra prediction mode,范围:[0 ~ 255]*/
    AR_S32 pu08IntraAngleDeltaRate;  /**< A value which is added to  rate when calculating cost(=distortion + rate) in 8x8 Angular intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu16IntraPlanarDeltaRate; /**< A value which is added to rate when calculating cost(=distortion + rate) in 16x16 Planar intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu16IntraDcDeltaRate;     /**< A value which is added to rate when calculating cost(=distortion + rate) in 16x16 DC intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu16IntraAngleDeltaRate;  /**< A value which is added to rate when calculating cost(=distortion + rate) in 16x16 Angular intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu32IntraPlanarDeltaRate; /**< A value which is added to rate when calculating cost(=distortion + rate) in 32x32 Planar intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu32IntraDcDeltaRate;     /**< A value which is added to rate when calculating cost(=distortion + rate) in 32x32 DC intra prediction mode,范围:[0 ~ 255] */
    AR_S32 pu32IntraAngleDeltaRate;  /**< A value which is added to rate when calculating cost(=distortion + rate) in 32x32 Angular intra prediction mode,范围:[0 ~ 255] */
    AR_S32 cu08IntraDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU8x8,范围:[0 ~ 255] */
    AR_S32 cu08InterDeltaRate;       /**< A value which is added to rate when calculating cost for inter CU8x8,范围:[0 ~ 255] */
    AR_S32 cu08MergeDeltaRate;       /**< A value which is added to rate when calculating cost for merge CU8x8,范围:[0 ~ 255] */
    AR_S32 cu16IntraDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU16x16,范围:[0 ~ 255] */
    AR_S32 cu16InterDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU16x16,范围:[0 ~ 255] */
    AR_S32 cu16MergeDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU16x16,范围:[0 ~ 255] */
    AR_S32 cu32IntraDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU32x32,范围:[0 ~ 255] */
    AR_S32 cu32InterDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU32x32,范围:[0 ~ 255] */
    AR_S32 cu32MergeDeltaRate;       /**< A value which is added to rate when calculating cost for intra CU32x32,范围:[0 ~ 255] */
} STRU_CUSTOM_MD_DPARAM;
typedef STRU_CUSTOM_MD_DPARAM STRU_CUSTOM_MD_SPARAM;
/**
* @note 定义支持动态修改的intra相关编码参数。
*/
typedef struct
{
    AR_S32 s32ForcedIdrHeaderEnable;       /**< 范围：0：不使能强制编header 1：在IDR帧前强制编header 2：在关键帧前强制编header */
    AR_S32 s32IntraQP;             /**< I帧的量化参数,范围[0, 51] */
    AR_S32 s32IntraPeriod;         /**< GOP周期 范围:[0,1024] */
    AR_S32 s32AvcIdrPeriod;        /**< IDR间隔,范围:[0,1024] 0:代表无穷大间隔;note:只对H.264生效 */
} STRU_INTRA_DPARAM;
/**
* @note 定义静态的intra相关编码参数。
*/
typedef struct
{
  AR_S32 s32DecodingRefreshType;         /**< 每个Intra周期插入I帧的类型，建议IDR；范围0，1，2。0:Non-IRAP,1:CRA,2:IDR */
  STRU_INTRA_DPARAM stIntraDParam;
} STRU_INTRA_SPARAM;
/**
* @note 定义支持动态修改的码控weight编码参数。
*/
typedef struct
{
    AR_U32 u32RcWeightBuf;   /**< 码控评估的一个平滑参数. 范围[1,255], 值越大调整越快. */
    AR_U32 u32RcWeightParam; /**< 调整给RC模型更新参数的速度. 范围[1,31], 值越大速度越慢. */
} STRU_RC_WEIGHT_DPARAM;
typedef STRU_RC_WEIGHT_DPARAM STRU_RC_WEIGHT_SPARAM;
/**
* @note 定义支持动态修改的VUI和HRD编码参数。
*/
typedef struct
{
    AR_U32 u32EncodeHrdRbspInVPS;  /**< 是否使能VPS的HRD信息. */
    AR_U32 u32HrdRbspDataSize;     /**< HRD的长度bits */
    AR_U64 u64HrdRbspDataAddr;     /**< HRD数据的物理地址 */

    AR_U32 u32EncodeVuiRbsp;       /**< 是否使能VUI编码 */
    AR_U32 u32VuiRbspDataSize;     /**< VUI的长度bits */
    AR_U64 u64VuiRbspDataAddr;     /**< VUI数据的物理地址 */
} STRU_VUI_HRD_DPARAM;
/**
* @note 定义支持动态修改的参数的枚举类型,与vpuapi.h定义一致.
*/
typedef enum {
    HAL_ENC_CHANGE_PPS                 = (1<<0),
    HAL_ENC_CHANGE_INTRA_PARAM         = (1<<1),
    HAL_ENC_CHANGE_RC_FRAME_RATE       = (1<<6),
    HAL_ENC_CHANGE_RC_TARGET_RATE      = (1<<8),
    HAL_ENC_CHANGE_RC                  = (1<<9),
    HAL_ENC_CHANGE_RC_MIN_MAX_QP       = (1<<10),
    HAL_ENC_CHANGE_RC_BIT_RATIO_LAYER  = (1<<11),
    HAL_ENC_CHANGE_RC_INTER_MIN_MAX_QP = (1<<12),
    HAL_ENC_CHANGE_RC_WEIGHT           = (1<<13),
    HAL_ENC_CHANGE_INDEPEND_SLICE      = (1<<16),
    HAL_ENC_CHANGE_DEPEND_SLICE        = (1<<17),
    HAL_ENC_CHANGE_RDO                 = (1<<18),
    HAL_ENC_CHANGE_NR                  = (1<<19),
    HAL_ENC_CHANGE_BG                  = (1<<20),
    HAL_ENC_CHANGE_CUSTOM_MD           = (1<<21),
    HAL_ENC_CHANGE_CUSTOM_LAMBDA_ADDR  = (1<<22),
    HAL_ENC_CHANGE_VUI_HRD_PARAM       = (1<<23),
} ENUM_CHANGE_OPTION;

/**
* @note 定义支持动态修改的编码参数。
*/
typedef struct {
    AR_S32 s32EnableOption;                   /**< 修改参数的选项集,参考ENUM_CHANGE_OPTION */
    //HAL_ENC_CHANGE_PPS
    STRU_VENC_PPS_DPARAM stPpsParam;          /**< 支持动态修改图片参数集结构体 */
    //HAL_ENC_CHANGE_INDEPEND_SLICE
    //HAL_ENC_CHANGE_DEPEND_SLICE
    STRU_SLICE_SPLIT_DPARAM stSliceSplit;     /**< 支持动态修改条带分割参数结构体 */
    //HAL_ENC_CHANGE_RDO
    STRU_RDO_DPARAM stRdoParam;               /**< 支持动态修改RDO参数结构体 */
    // HAL_ENC_CHANGE_RC_TARGET_RATE
    AR_S32 s32BitRate;                        /**< 设定目标码率,范围HEVC[128, 160000],avc[128, 240000]单位k    bps */
    // HAL_ENC_CHANGE_RC_FRAME_RATE
    AR_S32 s32FrameRate;                      /**< 图像帧率,会用来做码控;等于framerate X 1024 */
    // HAL_ENC_CHANGE_RC
    AR_S32 s32VbvBufferSize;                  /**< vbv缓冲区的大小单位ms 范围:[10, 3000] */
    STRU_RC_CU_DPARAM stRcParam;              /**< 码控相关参数结构体 */
    // HAL_ENC_CHANGE_RC_MIN_MAX_QP
    STRU_RC_INTRA_DPARAM stRcIntra;           /**< I帧码控参数 */
    // HAL_ENC_CHANGE_RC_INTER_MIN_MAX_QP
    STRU_RC_INTER_DPARAM stRcInter;           /**< P和B帧码控参数 */
    // HAL_ENC_CHANGE_RC_BIT_RATIO_LAYER
    AR_S32 fixedBitRatio[AR_MAX_GOP_NUM];        /**< 自定义码流分配策略 */
    // HAL_ENC_CHANGE_BG (bgDetectEnable can't be changed while encoding)
    STRU_BG_DPARAM stBgParam;                 /**< 背景检测相关参数结构体 */
    // HAL_ENC_CHANGE_NR
    STRU_NR_DPARAM stNrParam;                 /**< 降噪相关参数结构体 */
    // HAL_ENC_CHANGE_CUSTOM_MD
    STRU_CUSTOM_MD_DPARAM stCustomMdParam;     /**< HEVC模式判别相关参数结构体 */
    // HAL_ENC_CHANGE_CUSTOM_LAMBDA_ADDR
    AR_U64 u64CustomLambdaAddr;               /**< 用户自定义lambda表的物理地址 note:必须按16对齐 */
    // HAL_ENC_CHANGE_INTRA_PARAM
    STRU_INTRA_DPARAM stIntraParam;
    // HAL_ENC_CHANGE_RC_WEIGHT
    STRU_RC_WEIGHT_DPARAM stRcWeight;         /**< 码控weight的相关参数结构体 */
    // HAL_ENC_CHANGE_VUI_HRD_PARAM
    STRU_VUI_HRD_DPARAM stVuiHrdParam;        /**< HRD和VUI的相关参数结构体 */
} STRU_VENC_DPARAM;
/**
* @note  定义用户自定义GOP里每帧的参数结构体。
*/
typedef struct
{
    AR_S32 s32PicType;                        /**< 当前帧类型，参考ENUM_PIC_TYPE */
    AR_S32 s32PocOffset;                      /**< 当前帧在自定义GOP的POC */
    AR_S32 s32PicQp;                          /**< 当前帧的量化参数 */
    AR_S32 s32UseMultiRefP;                   /**< 是否使能当前P帧的双向参考 */
    AR_S32 s32RefPocL0;                       /**< 当前帧参考L0的POC */
    AR_S32 s32RefPocL1;                       /**< 当前帧参考的L1的POC */
    AR_S32 s32TemporalId;                     /**< 当前帧的时域ID */
} STRU_CUSTOM_GOP_PARAM;

/**
* @brief  定义用户自定义的GOP参数结构体。
*/
typedef struct {
    AR_S32 s32CustomGopSize;                      /**< 自定义的GOP长度.范围(0~8) */
    STRU_CUSTOM_GOP_PARAM picParam[AR_MAX_GOP_NUM];  /**< 自定义GOP的每帧参数 */
} STRU_CUSTOM_GOP_PARAMS;
/**
* @note 定义编码器的剪裁输出参数，需要在start之前设置进去
*/
typedef struct
{
    AR_S32 s32ConfWinTop;                    /**< 顶部的偏移 */
    AR_S32 s32ConfWinBot;                    /**< 底部的偏移 */
    AR_S32 s32ConfWinLeft;                   /**< 左边的偏移 */
    AR_S32 s32ConfWinRight;                  /**< 右边的偏移 */
} STRU_CONF_WIN_SPS;
/**
* @note 定义编码器的intra刷新参数
*/
typedef struct
{
    AR_S32 s32IntraMbRefreshMode; /**< 0：关闭intra   refresh 1：按行刷intra 2：按列刷intra 3：按MB个数刷 note:只对H264有效 */
    AR_S32 s32IntraMbRefreshArg;   /**< 范围：0-无穷大；根据mode设置对应每帧刷的连续intra参数 note:只对H264有效 */
    AR_S32 s32IntraRefreshMode; /**< 0：关闭intra   refresh 1：按行刷intra 2：按列刷intra 3：按CTU个数刷 4：自适应刷intra note:只对H265有效 */
    AR_S32 s32IntraRefreshArg;  /**< 范围：0-无穷大；根据mode设置对应每帧刷的intra参数 note:只对H265有效 */
} STRU_INTRA_REFRESH_SPARAM;
/**
* @note 定义编码器的SPS的一些静态配置参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct
{
    AR_S32 s32profile;          /**< 范围：0-3；0：自动检测 1：Main 2：Main10 3：Main still */
    AR_S32 s32EnStillPicture;   /**< 范围：0,1；0：不使能still profile，1：使能still profile */
    AR_S32 s32Level;            /**< 范围：无限制；真实level等于 level * 10 */
    AR_S32 s32Tier;             /**< 范围：0,1；0：Main 1：High */
    AR_S32 s32InternalBitDepth; /**< 范围:8,10; 8:编码器内部按8bit处理，如果输入是10bit数据会自动丢掉高2bit 10:被阉割掉 */
    AR_U32 u32UseLongTerm;      /**< 是否使能长期参考功能 */
    AR_S32 s32TmvpEnable;       /**< 是否使能时域mv预测 */
    AR_S32 s32SaoEnable;        /**< 是否使能样点自适应补偿 */
    AR_S32 s32SkipIntraTrans;   /**< 是否使能PPS中的transform_skip_enabled_flag */
    AR_U32 u32StrongIntraSmoothEnable;     /**< 是否使能帧内预测强滤波;SPS中的strong_intra_smoothing_enable_flag */
    AR_S32 s32ScalingListEnable;   /**< 范围：0：不使能用户量化表 1：使能用户量化表 2：使能默认量化表 */
} STRU_SPS_SPARAM;
/**
* @note 定义编码器的PPS的一些静态配置参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct
{
    AR_S32 s32LosslessEnable;              /**< 范围0，1。0：不使能无损编码，1：使能无损编码 */
    AR_S32 s32ConstIntraPredFlag;          /**< 范围0，1。0：不使强制帧内预测，1：使能强制帧内预测 */
    AR_S32 s32LfCrossSliceBoundaryEnable;  /**< 是否使能跨越条带边界的滤波 */
    AR_U32 u32WeightPredEnable;            /**< 是否使能加权预测;PPS中的weighted_pred_flag和weighted_bipred_flag */
    AR_S32 s32WppEnable;                   /**< 是否使能WPP;note:没有实现 */
    AR_S32 s32DisableDeblk;                /**< 是否不使能环路去块滤波 */
    AR_S32 s32BetaOffsetDiv2;              /**< 去方块滤波参数BetaOffsetDiv2,范围:[-6,6] */
    AR_S32 s32TcOffsetDiv2;                /**< 去方块滤波参数TcOffsetDiv3,范围:[-6,6] */
    AR_S32 s32ChromaCbQpOffset;            /**< Cb相对于亮度QP的偏移 */
    AR_S32 s32ChromaCrQpOffset;            /**< Cr相对于亮度QP的偏移 */
    AR_S32 s32Transform8x8Enable;          /**< 是否使能8x8帧内预测和变换 */
    AR_S32 s32EntropyCodingMode;           /**< 熵编码方式：0:CAVLC 1:CABAC;note:只适用于H.264 */
} STRU_PPS_SPARAM;
/**
* @note 定义编码器预设的GOP模式与vpuapi.h中的定义一致。
*/
typedef enum {
    GOP_IDX_CUSTOM_GOP       = 0,    /**< 用户自定义GOP结构 */
    GOP_IDX_ALL_I            = 1,    /**< 全部是I帧, gopsize = 1 */
    GOP_IDX_IPP              = 2,    /**< 连续P帧, cyclic gopsize = 1 P帧是双向参考 */
    GOP_IDX_IBBB             = 3,    /**< 连续B帧, cyclic gopsize = 1  */
    GOP_IDX_IBPBP            = 4,    /**< gopsize = 2 P帧是单向参考 */
    GOP_IDX_IBBBP            = 5,    /**< gopsize = 4 P帧是单向参考 */
    GOP_IDX_IPPPP            = 6,    /**< 连续P帧, cyclic gopsize = 4 P帧是双向参考 */
    GOP_IDX_IBBBB            = 7,    /**< 连续B帧, cyclic gopsize = 4 */
    GOP_IDX_RA_IB            = 8,    /**< 随机访问, cyclic gopsize = 8 */
    GOP_IDX_IPP_SINGLE       = 9,    /**< 连续P帧, cyclic gopsize = 1, P帧是单向参考 */
} ENUM_PRESET_GOP;
/**
* @note 定义编码器的码流分配的静态参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct {
    AR_S32 s32BitAllocMode;            /**< 分配模式, 0：多参考多码流 1：按预定参数分配 */
    AR_S32 fixedBitRatio[AR_MAX_GOP_NUM]; /**<固定码流分配比例，定义GOP内每帧图片的码流分配;只针对s32BitAllocMode=2有效 */
} STRU_BIT_ALLOCATE_PARAM;
/**
* @note 定义HEVC和AVC编码器基础的静态参数，创建实例的时候必须的一些参数。
*/
typedef struct
{
    AR_U32  u32CmdQueueDepth;       /**< RW; 范围:[2,4]; 设置基于cmd级的并行模式的queue深度 */
    AR_S32  s32SubFrameSyncEnable;  /**< RW; 范围:[0,1]; 是否使能编码器与vif或isp的帧同步编码 note:需要lowdelay时使能 */
    AR_S32  s32SubFrameSyncSrc;     /**< RW; 范围:[0,2]; 0:关闭 1:ISP 2:VIF note:需要VIN模块同步配置好 */
    AR_S32  s32Cframe50Enable;         /**<RW; 源图像是否是cf50格式 */
    AR_S32  s32Cframe50LosslessEnable; /**<RW; 是否使能cf50的无损压缩 */
    AR_S32  s32Cframe50Tx16Y;          /**< 亮度的压缩比例 */
    AR_S32  s32Cframe50Tx16C;          /**< 色度的压缩比例 */
    AR_S32  s32Cframe50_422;           /**< 是否使能422到420的转化 */
    AR_U8   u8KeyFrameSizeMultiplier;   /**<@attention 新添加参数; 关键帧buffer的size与码率的乘子; 静态属性 */
    AR_U8   u8NonKeyFrameSizeMultiplier;    /**<@attention 新添加参数; 非关键帧buffer的size与码率的乘子; 静态属性 */
} STRU_VENC_ATTR_H26X;

/**
* @note 定义编码器基础的静态参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct
{
    ENUM_VCODEC_TYPE  eEncType;        /* RW; 编码类型 */
    ENUM_FB_FORMAT  eformat;           /* RW; 参考ENUM_FB_FORMAT */
    AR_S32  s32CbcrInterleave;         /* RW; Cb与Cr是否交织 */
    AR_S32  s32PicWidth;               /* RW; 范围:H26x [32,8192],mjpeg[16,32768];编码图像的宽度单位是像素 */
    AR_S32  s32PicHeight;              /* RW; 范围:H26x [32,8192],mjpeg[16,32768];编码图像的宽度单位是像素 */
    AR_S32  s32profile;                /**< 范围：0-3；0：自动检测 1：Main 2：Main10 3：Main still */
    STRU_VENC_ATTR_H26X stH26xAttr;    /**< 指向HEVC或AVC的属性结构体 */
    AR_S32 s32MaxWidth;                /**< 硬件支持的最大编码分辨率宽度*/
    AR_S32 s32MaxHeight;               /**< 硬件支持的最大编码分辨率高度*/
    AR_U32 u32BufSize;                 /**< 码流buffer的大小 */
} STRU_VENC_ATTR;
/*
* @note 定义码控类型
*/
typedef enum {
    HAL_VENC_RC_MODE_CBR = 1,
    HAL_VENC_RC_MODE_VBR,
    HAL_VENC_RC_MODE_FIXQP,
    HAL_VENC_RC_MODE_QPMAP,
} ENUM_VENC_RC_MODE;

/**
* @note 定义编码器常用的码控参数集
*/
typedef struct {
    ENUM_VENC_RC_MODE enRcMode;  /**< 码控方式 */
    AR_S32 s32BitRate;           /**< 目标码率单位kbps; 范围HEVC[128, 160000],AVC[128, 240000]单位k    bps note:动态参数 */
    AR_S32 s32GopSize;           /* RW; Range:[1, 65536]; I帧间隔; note:动态参数 */
    AR_S32 s32FPSNum;            /* RW; 帧率信息的分子; note:动态参数 */
    AR_S32 s32SrcFPSNum;         /* RW; 源帧率信息的分子; note:动态参数 */
    AR_S32 s32FPSDen;            /* RW; 帧率信息的分母; note:动态参数 */
    AR_S32 s32VbvBufferSize;     /**< vbv缓冲区的大小单位ms 范围:[10, 3000]; note:动态参数 */
    AR_S32 s32ForcePicQpI;       /**< I帧的强制量化参数;范围[0-51] */
    AR_S32 s32ForcePicQpP;       /**< P帧的强制量化参数;范围[0-51] */
    AR_S32 s32ForcePicQpB;       /**< B帧的强制量化参数;范围[0-51] */
    AR_U32 u32StatTime;
    AR_U32 u32MaxBitRate;
    AR_U32 u32MinBitRate;
    AR_U32 u32Qfactor;           /**< 可读写; 取值范围:[0,30]; JPEG/MJPEG编码的编码质量*/

    union
    {
        /*VENC_H264_CBR_S    stH264Cbr;
        VENC_H264_VBR_S    stH264Vbr;*/
        //STRU_VENC_H264_FIXQP_S  stH264FixQp;
        /*VENC_H264_QPMAP_S  stH264QpMap;

        VENC_MJPEG_CBR_S   stMjpegCbr;
        VENC_MJPEG_VBR_S   stMjpegVbr;*/
        //STRU_VENC_MJPEG_FIXQP_S stMjpegFixQp;

        /*VENC_H265_CBR_S    stH265Cbr;
        VENC_H265_VBR_S    stH265Vbr;*/
        //STRU_VENC_H265_FIXQP_S  stH265FixQp;
        /*VENC_H265_QPMAP_S  stH265QpMap;*/
    };
} STRU_VENC_RC_ATTR;
/**
* @note 定义编码器GOP的静态参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct
{
    ENUM_PRESET_GOP eGopType;
    AR_S32 s32IntraQpOffset;   /**< intra帧相对于其他帧的QP偏移量,范围:[-9, 9] note:动态参数 */
    //AR_S32 s32IntraQP;       /**< intra帧的QP note:RC关闭的时候生效; note:动态参数 */
    AR_U32 u32BaseGop;         /**< base layer的最小gop size */
} STRU_VENC_GOP_ATTR;

/**
* @note 定义编码器CU级的码控参数集
*/
typedef struct {
    AR_S32 s32MbLevelRcEnable;         /**< 是否使能MB级的码控;只针对H.264 */
    AR_S32 s32CuLevelRcEnable;         /**< 是否使能CU级的码控;只针对H.265 */
    AR_S32 s32HvsQPEnable;             /**< 是否使能CU级QP调整 */
    AR_S32 s32HvsQpScale;              /**< CU级QP调整的系数,范围:[0, 15] */
    AR_S32 s32HvsMaxDeltaQp;           /**< HVS的最大的QP变化,范围:[0, 51] */
} STRU_CU_RC_PARAM;
/**
* @note  编码实例的码控相关的参数结构体。
*/
typedef struct
{
    AR_S32 s32InitialRcQp;       /**< 码控调整的初始qp;范围[0,51]; note 63代表不设置由码控算法自动决定 */
    AR_S32 s32IntraQP;           /**< I帧的量化参数,范围: [0, 51]; note:RC关闭时有效 */
    AR_S32 s32IntraQpOffset;     /**< intra帧相对于其他帧的QP偏移量 范围:[-9, 9] */
    AR_S32 s32MinQpI;            /**< I帧最小QP,范围:[0, 51] */
    AR_S32 s32MaxQpI;            /**< I帧最大QP,范围:[0, 51] */
    AR_S32 s32MinQpP;            /**< P帧最小QP,范围: [0, 51] */
    AR_S32 s32MaxQpP;            /**< P帧最大QP,范围: [0, 51] */
    AR_S32 s32MinQpB;            /**< B帧最小QP,范围: [0, 51] */
    AR_S32 s32MaxQpB;            /**< B帧最大QP,范围: [0, 51] */
    AR_S32 s32RoiEnable;                   /**< 是否使能序列级的ROI; note:RC打开才有效 */
    STRU_BIT_ALLOCATE_PARAM stBitAllocParam;  /**< 编码器的码流分配的静态参数结构体 */
    STRU_CU_RC_PARAM stCuRcParam;             /**< 定义编码器CU级的码控参数集结构体 */

    AR_U32 u32RcMaxIpRatio;
    AR_U32 u32RcMinIpRatio;
} STRU_VENC_RC_PARAM;

/**
* @note  MJPEG编码实例的码控相关的参数结构体。
*/
typedef struct
{
    AR_S32 s32MinQpCoef;            /**< 帧最小QP系数,范围:[0, 30] */
    AR_S32 s32MaxQpCoef;            /**< 帧最大QP系数,范围:[min, 30] */
} STRU_MJPEG_RC_PARAM;

/**
* @note 定义编码器的静态参数，需要在start之前设置进去，默认会从cjson加载一组初始化参数。
*/
typedef struct
{
    STRU_VENC_ATTR  stVencAttr;
    STRU_VENC_GOP_ATTR stGopAttr;
    STRU_VENC_RC_ATTR stRcAttr;
    STRU_SPS_SPARAM stSPSParam;
    STRU_PPS_SPARAM stPPSParam;
    STRU_RDO_SPARAM stRdoParam;               /**< 静态RDO参数结构体 */
    STRU_INTRA_REFRESH_SPARAM stIntraRefresh; /**< intrarefresh的参数结构体 */
    STRU_SLICE_SPLIT_SPARAM stSliceSplit;     /**< slice split的参数结构体 */
    // CUSTOM_GOP
    STRU_CUSTOM_GOP_PARAMS stCustGop;       /**< 自定义GOP参数结构体 */
    STRU_NR_SPARAM stNrParam;               /**< 降噪参数结构体 */
    STRU_RC_WEIGHT_SPARAM stRcWeight;  /**< 码控权重参数 */
    STRU_BG_SPARAM  stBgParam;         /**< 背景检测参数结构体 */
    STRU_VENC_RC_PARAM stRcParam;      /**< 码控相关参数结构体 */
    STRU_MJPEG_RC_PARAM stMjpegRcParam;/**< MJPEG码控相关参数结构体 */
    AR_U64 u64CustomLambdaAddr;        /**< 用户自定义lambda表的物理地址 note:必须按16对齐 */
    AR_U64 u64UserScalingListAddr;     /**< 用户自定义变换表的物理地址 note:必须按16对齐 */
    STRU_CUSTOM_MD_SPARAM stCustomMdParam; /**< 用户自定义模式判别参数结构体 */
}STRU_VENC_SPARAM;
/**
* @note 定义编码器感兴趣区域的参数结构体,每帧都可以更新,建议start编码器之前设置
*/
typedef struct
{
    AR_U32   u32RoiIndex;           /**< ROI的索引,范围:[0, +∞],后边的索引区域会覆盖前边的区域 */
    AR_BOOL  bRoiEnable;            /**< 是否使能该ROI区域 */
    AR_U32   u32RoiXStart;          /**< 该ROI区域起始点水平方向坐标 note:HEVC按64对齐,AVC按16对齐, 不对齐的话会向上取整对齐 */
    AR_U32   u32RoiYStart;          /**< 该ROI区域起始点垂直方向坐标 note:HEVC按64对齐,AVC按16对齐, 不对齐的话会向上取整对齐 */
    AR_U32   u32RoiXWidth;          /**< 该ROI区域水平宽度 note:HEVC按64对齐,AVC按16对齐, 不对齐的话会向上取整对齐 */
    AR_U32   u32RoiYHeigth;         /**< 该ROI区域垂直高度 note:HEVC按64对齐,AVC按16对齐, 不对齐的话会向上取整对齐 */
    AR_BOOL  bAbsQp;                /**< ROI区域QP模式,范围:[0, 1] note:AR_FALSE:相对qp, AR_TRUE:绝对qp; 绝对qp暂不支持 */
    AR_S32   s32Qp;                 /**< 相对qp大小,范围:[-26, 25] note:码控打开时只支持相对qp, 码控关闭只支持绝对qp */
} STRU_VENC_ROI_NODE;

typedef struct
{
    AR_U64 phyAddr;                 /**< 存放sei数据的物理地址*/
    AR_U64 virtAddr;                /**< 存放sei数据的虚拟地址*/
    AR_U32 u32FilledLen;            /**< app填充的sei数据的大小*/
    AR_U32 u32Capacity;             /**< sei buffer的容量*/
} STRU_VENC_SEI_NODE;

/**
* @note 定义H264 的HRD参数结构体，动态参数可以随时修改
*/
typedef struct
{
      AR_S32 cpb_cnt                            : 16; // [15: 0]
      AR_U32 bit_rate_scale                     :  8; // [23:16]
      AR_U32 cbp_size_scale                     :  8; // [31:24]

      AR_S32 initial_cpb_removal_delay_length  : 16; // [15: 0]
      AR_S32 cpb_removal_delay_length          : 16; // [31:16]

      AR_S32 dpb_output_delay_length           : 16; // [15: 0]
      AR_S32 time_offset_length                : 16; // [31:16]

      AR_U32 bit_rate_value[32];

      AR_U32 cpb_size_value[32];

      AR_U8 cbr_flag[32];
} STRU_AVC_HRD_PARAM;
/**
* @note 定义H264 VUI的参数结构体，动态参数可以随时修改
*/
typedef struct
{
    AR_U32 aspect_ratio_info_present_flag          :  1; // [    0]
    AR_U32 aspect_ratio_idc                        :  8; // [ 8: 1]
    AR_U32 reserved_0                              : 23; // [31: 9]

    AR_U32 sar_width                               : 16; // [15: 0]
    AR_U32 sar_height                              : 16; // [31:15]

    AR_U32 overscan_info_present_flag              :  1; // [    0]
    AR_U32 overscan_appropriate_flag               :  1; // [    1]
    AR_U32 video_signal_type_present_flag          :  1; // [    2]
    AR_U32 video_format                            :  3; // [ 5: 3]
    AR_U32 video_full_range_flag                   :  1; // [    6]
    AR_U32 colour_description_present_flag         :  1; // [    7]
    AR_U32 colour_primaries                        :  8; // [15: 8]
    AR_U32 transfer_characteristics                :  8; // [23:16]
    AR_U32 matrix_coefficients                     :  8; // [31:24]

    AR_U32 chroma_loc_info_present_flag            :  1; // [    0]
    AR_S32 chroma_sample_loc_type_top_field       :  8; // [ 8: 1]
    AR_S32 chroma_sample_loc_type_bottom_field    :  8; // [16: 9]
    AR_U32 vui_timing_info_present_flag            :  1; // [   17]
    AR_U32 reserved_1                              : 14; // [31:18]

    AR_U32 vui_num_units_in_tick                   : 32;
    AR_U32 vui_time_scale                          : 32;

    AR_U32 vui_fixed_frame_rate_flag               :  1; // [    0]
    AR_U32 vcl_hrd_parameters_present_flag         :  1; // [    1]
    AR_U32 nal_hrd_parameters_present_flag         :  1; // [    2]
    AR_U32 low_delay_hrd_flag                      :  1; // [    3]
    AR_U32 pic_struct_present_flag                 :  1; // [    4]
    AR_U32 bitstream_restriction_flag              :  1; // [    5]
    AR_U32 motion_vectors_over_pic_boundaries_flag :  1; // [    6]
    AR_S32 max_bytes_per_pic_denom                :  8; // [14: 7]
    AR_S32 max_bits_per_mincu_denom               :  8; // [22:15]
    AR_U32 reserved_2                              :  9; // [31:23]

    AR_S32 log2_max_mv_length_horizontal          :  8; // [ 7: 0]
    AR_S32 log2_max_mv_length_vertical            :  8; // [15: 8]
    AR_S32 max_num_reorder_frames                 :  8; // [23:16]
    AR_S32 max_dec_frame_buffering                :  8; // [31:24]

    STRU_AVC_HRD_PARAM vcl_hrd;
    STRU_AVC_HRD_PARAM nal_hrd;
    AR_S32 s32LengthInBits;
} STRU_AVC_VUI_INFO;
/**
* @note 定义H265的HRD参数结构体，动态参数可以随时修改
*/
#define H265_NUM_SUB_LAYER 16
#define H265_MAX_CPB_COUNT 16
typedef struct
{
    AR_U32 nal_hrd_param_present_flag                 : 1; // [    0]
    AR_U32 vcl_hrd_param_present_flag                 : 1; // [    1]
    AR_U32 sub_pic_hrd_params_present_flag            : 1; // [    2]
    AR_U32 tick_divisor_minus2                        : 8; // [10: 3]
    AR_U32 du_cpb_removal_delay_inc_length_minus1     : 5; // [15:11]
    AR_U32 sub_pic_cpb_params_in_pic_timing_sei_flag  : 1; // [   16]
    AR_U32 dpb_output_delay_du_length_minus1          : 5; // [21:17]
    AR_U32 bit_rate_scale                             : 4; // [25:22]
    AR_U32 cpb_size_scale                             : 4; // [29:26]
    AR_U32 reserved_0                                 : 2; // [31:30]

    AR_U32 initial_cpb_removal_delay_length_minus1    : 10; // [ 9: 0]
    AR_U32 cpb_removal_delay_length_minus1            :  5; // [14:10]
    AR_U32 dpb_output_delay_length_minus1             :  5; // [19:15]
    AR_U32 reserved_1                                 : 12; // [31:20]

    AR_U32 fixed_pic_rate_gen_flag[H265_NUM_SUB_LAYER];
    AR_U32 fixed_pic_rate_within_cvs_flag[H265_NUM_SUB_LAYER];
    AR_U32 low_delay_hrd_flag[H265_NUM_SUB_LAYER];
    AR_S32 cpb_cnt_minus1[H265_NUM_SUB_LAYER];
    AR_S32 elemental_duration_in_tc_minus1[H265_NUM_SUB_LAYER];

    AR_U32 nal_bit_rate_value_minus1[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];
    AR_U32 nal_cpb_size_value_minus1[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];
    AR_U32 nal_cpb_size_du_value_minus1[H265_NUM_SUB_LAYER];
    AR_U32 nal_bit_rate_du_value_minus1[H265_NUM_SUB_LAYER];
    AR_U32 nal_cbr_flag[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];

    AR_U32 vcl_bit_rate_value_minus1[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];
    AR_U32 vcl_cpb_size_value_minus1[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];
    AR_U32 vcl_cpb_size_du_value_minus1[H265_NUM_SUB_LAYER];
    AR_U32 vcl_bit_rate_du_value_minus1[H265_NUM_SUB_LAYER];
    AR_U32 vcl_cbr_flag[H265_NUM_SUB_LAYER][H265_MAX_CPB_COUNT];
} STRU_HEVC_HRD_PARAM;
/**
* @note 定义H265 VUI的参数结构体，动态参数可以随时修改
*/
typedef struct
{
    AR_U32 aspect_ratio_info_present_flag             :  1; // [    0]
    AR_U32 aspect_ratio_idc                           :  8; // [ 8: 1]
    AR_U32 reserved_0                                 : 23; // [31: 9]

    AR_U32 sar_width                                  : 16; // [15: 0]
    AR_U32 sar_height                                 : 16; // [31:16]

    AR_U32 overscan_info_present_flag                 :  1; // [    0]
    AR_U32 overscan_appropriate_flag                  :  1; // [    1]
    AR_U32 video_signal_type_present_flag             :  1; // [    2]
    AR_U32 video_format                               :  3; // [ 5: 3]
    AR_U32 video_full_range_flag                      :  1; // [    6]
    AR_U32 colour_description_present_flag            :  1; // [    7]
    AR_U32 colour_primaries                           :  8; // [15: 8]
    AR_U32 transfer_characteristics                   :  8; // [23:16]
    AR_U32 matrix_coefficients                        :  8; // [31:24]

    AR_U32 chroma_loc_info_present_flag               :  1; // [    0]
    AR_S32 chroma_sample_loc_type_top_field           :  8; // [ 8: 1]
    AR_S32 chroma_sample_loc_type_bottom_field        :  8; // [16: 9]
    AR_U32 neutral_chroma_indication_flag             :  1; // [   17]
    AR_U32 field_seq_flag                             :  1; // [   18]
    AR_U32 frame_field_info_present_flag              :  1; // [   19]
    AR_U32 default_display_window_flag                :  1; // [   20]
    AR_U32 vui_timing_info_present_flag               :  1; // [   21]
    AR_U32 vui_poc_proportional_to_timing_flag        :  1; // [   22]
    AR_U32 vui_hrd_parameters_present_flag            :  1; // [   23]
    AR_U32 bitstream_restriction_flag                 :  1; // [   24]
    AR_U32 tiles_fixed_structure_flag                 :  1; // [   25]
    AR_U32 motion_vectors_over_pic_boundaries_flag    :  1; // [   26]
    AR_U32 restricted_ref_pic_lists_flag              :  1; // [   27]
    AR_U32 reserved_1                                 :  4; // [31:28]

    AR_U32 vui_num_units_in_tick                       : 32; // [31: 0]
    AR_U32 vui_time_scale                              : 32; // [31: 0]

    AR_U32 min_spatial_segmentation_idc               : 12; // [11: 0]
    AR_U32 max_bytes_per_pic_denom                    :  5; // [16:12]
    AR_U32 max_bits_per_mincu_denom                   :  5; // [21:17]
    AR_U32 log2_max_mv_length_horizontal              :  5; // [26:22]
    AR_U32 log2_max_mv_length_vertical                :  5; // [31:27]

    AR_S32 vui_num_ticks_poc_diff_one_minus1          : 32; // [31: 0]

    STRU_CONF_WIN_SPS   def_disp_win;
    STRU_HEVC_HRD_PARAM hrd_param;
    AR_S32 s32LengthInBits;
} STRU_HEVC_VUI_INFO;
/**
* @note 定义编码器VUI的参数结构体，动态参数可以随时修改
*/
typedef union
{
    STRU_AVC_VUI_INFO stAvcVui;
    STRU_HEVC_VUI_INFO stHevcVui;
} UNION_VUI_INFO;
/**
* @note 定义jpeg编码器的动态参数。
*/
typedef struct
{
    AR_U32 qp_coef;
    AR_U8  u8YQt[64];                      /**< Y量化表 */
    AR_U8  u8CbQt[64];                     /**< Cb量化表 */
    AR_U8  u8CrQt[64];                     /**< Cr量化表 */
    AR_U32 u32MCUPerECS;                   /**< reserved */
} STRU_VENC_JPEG_PARAM;
/**
* @note 定义mjpeg编码器的动态参数。
*/
typedef struct
{
    AR_U32 qp_coef;
    AR_U8  u8YQt[64];                      /**< Y量化表 */
    AR_U8  u8CbQt[64];                     /**< Cb量化表 */
    AR_U8  u8CrQt[64];                     /**< Cr量化表 */
    AR_U32 u32MCUPerECS;                   /**<@note 不支持 */
} STRU_VENC_MJPEG_PARAM;
/**
* @note 定义编码器旋转相关的静态参数
*/
typedef struct
{
    AR_S32 s32RotationEnable;
    AR_S32 s32RotationAngle;
} STRU_VENC_ROTATION;
/**
* @note 定义编码器镜像相关的静态参数
*/
typedef struct
{
    AR_S32 s32MirrorEnable;
    AR_S32 s32MirrorDirection;
} STRU_VENC_MIRROR;
/**
* @note 定义编码器去块滤波相关的静态参数
*/
typedef struct
{
    AR_S32 s32DisableDeblk;                  /**< 是否不使能环路去块滤波 */
    AR_S32 s32LfCrossSliceBoundaryEnable;    /**< 是否使能跨越条带边界的滤波 */
    AR_S32 s32BetaOffsetDiv2;                /**< 去块滤波参数BetaOffsetDiv2 */
    AR_S32 s32TcOffsetDiv2;                  /**< 去块滤波参数TcOffsetDiv3 */
} STRU_VENC_DEBLK_H265;
/**
* @note  编码实例的属性结构体。
*/
typedef struct
{
    STRU_VENC_ATTR stVencAttr;
    STRU_VENC_RC_ATTR stRcAttr;
    STRU_VENC_GOP_ATTR stGopAttr;
} STRU_VENC_INST_ATTR;

/**
* @note  编码实例的属性结构体。
*/
typedef struct
{
    AR_S32 VeChn;
    AR_U64 *phyAddr;
    AR_U64 *virtAddr;
    AR_U32 u32FilledLen;
    AR_U32 u32Capacity;
} STRU_VENC_USERDATA;

typedef struct {
    AR_U64  u64PhyAddr;      /**< 只读; 码流buffer的起始物理地址。*/
    AR_U64  u64UserAddr;     /**< 只读; 码流buffer的虚拟地址。 */
    AR_U64  u64BufSize;      /**< 只读; 码流buffer的大小。 */
} STRU_VENC_STREAM_BUF_INFO;


/**
* @note  编码实例的接收原始图像的参数结构体。
*/
typedef struct
{
    AR_S32 s32RecvPicNum;            /*W; 范围：[-1, +∞]；-1：代表不限制接收数量；非-1：代表接收多少图像；*/
} STRU_VENC_PIC_PARAM;
/**
* @note 编码器高级参考参数。
*/
typedef struct
{
    AR_U32       u32UseLongPeriod; /* RW; 范围:[0,255]; 参考长期参考帧的间隔 */
    AR_U32       u32LongPeriod;    /* RW; 范围:[0,4294967295]; 长期参考帧的间隔 */
    AR_U32       u32BasePeriod;    /* RW; 范围:[0,4294967295]; base层的周期，记录API传递的参数 */
    AR_BOOL      bEnablePred;      /* RW; 范围:[0, 1]; base层的长期参考帧是否相互参考 */
} STRU_VENC_REF_PARAM;

typedef enum
{
    HAL_MODTYPE_VENC = 1,       /* VENC */
    HAL_MODTYPE_H264E,          /* H264e */
    HAL_MODTYPE_H265E,          /* H264e */
    HAL_MODTYPE_JPEGE,          /* Jpege */
    HAL_MODTYPE_RC,             /* Rc */
    HAL_MODTYPE_BUTT
} ENUM_MOD_TYPE;

#define IS_MOD_TYPE_INVAILD(eType)        ((eType < HAL_MODTYPE_VENC) || (eType > HAL_MODTYPE_RC))
/**
* @note 编码器模块共用参数
*/
typedef struct
{
    AR_U32 u32VencBufferCache;   /* RW; Range:[0,1]; 码流buffer是否是cached */
} STRU_VENC_MOD_PARAM;
/**
* @note 编码器码控模块共用参数
*/
typedef struct
{
    AR_U32 u32ClrStatAfterSetBr; /* RW; Range:[0,1]; 改变码率后是否清除帧率和码率的统计信息 */
} STRU_RC_MOD_PARAM;
/**
* @note H264&H265编码器模块级参数。
*/
typedef struct
{
    AR_U32 u32MiniBufMode;       /* RW; Range:[0,1]; 是否开启最小码流buffer模式 note:todo */
    AR_U32 u32CoreClock;         /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note:H264与H265的编码和解码共用 */
    AR_U32 u32BpuClock;          /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note:H264与H265的编码和解码共用 */
    AR_U64 u64Flags;             /* RW; h26x扩展参数标志位，各bit位含义参见ENUM_HAL_VENC_EXTENSION_FLAGS*/
} STRU_H26X_MOD_PARAM;
/**
* @note JPEG编码器模块级参数。
*/
typedef struct
{
    AR_U32 u32MiniBufMode;       /* RW; Range:[0,1]; 是否开启最小码流buffer模式 note:todo */
    AR_U32 u32CoreClock;         /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz */
    AR_U32 u32JpegClearStreamBuf; /* RW; Range:[0,1]; Jpeg编码设置属性时候是否清空码流buffer note: todo */
    AR_U64 u64Flags;             /* RW; jpeg扩展参数标志位，各bit位含义参见ENUM_HAL_VENC_EXTENSION_FLAGS*/
} STRU_JPEG_MOD_PARAM;

/**
* @note 编码器模块级参数。
*/
typedef struct
{
    AR_U32 u32IrqQueueSize;    /* RW; Range:[2,16]; 编解码器硬件中断队列大小, 存放中断指令 */
    AR_U32 u32TaskQueueSize;   /* RW; Range:[2,16]; 编解码器命令队列大小，存放app端的调用，如start/stop */
    AR_U32 u32DoneQueueSize;   /* RW; Range:[2,16]; 编解码器编码完成队列大小，存放所有通道编解码后的码流 */
    AR_U32 u32OutQueueSize;    /* RW; Range:[2,16]; 编解码器编码输出队列大小，存放某一通道编码解后的码流 */
    AR_U32 u32EventQueueSize;  /* RW; Range:[6,16]; 编解码器总的串行队列的大小，等于irq + task */
} STRU_EVENT_MOD_PARAM;

/**
* @note 编码器模块级参数。
*/
typedef struct
{
    ENUM_MOD_TYPE           eModType;        /* RW; 模块类型 */
    STRU_EVENT_MOD_PARAM    stEventModParam;
    union {
        STRU_VENC_MOD_PARAM stVencModParam;
        STRU_H26X_MOD_PARAM stH26xModParam;
        STRU_JPEG_MOD_PARAM stJpegModParam;
        STRU_RC_MOD_PARAM   stRcModParam;
    };
} STRU_VENC_MOD_PARAMS;

/**
* @note crop相关参数结构体
*/
typedef struct
{
    AR_BOOL bEnable;                     /* RW; 范围:[0, 1]; 是否使能crop功能 */
    AR_S32 s32X;                         /* RW; 范围:[0, pic_width], note: s32X 必须能按16对齐 */
    AR_S32 s32Y;                         /* RW; 范围:[0, pic_height] note: s32Y 必须能按2对齐 */
    AR_U32 u32Width;                     /* RW; 范围:[0, pic_width] */
    AR_U32 u32Height;                    /* RW; 范围:[0, pic_height] */
    AR_U32 u32YstartOffset;              /* RW: 计算亮度缓冲区地址的起始偏移 */
    AR_U32 u32UVstartOffset;             /* RW: 计算色度缓冲区地址的起始偏移 */
} STRU_VENC_CROP_INFO;

/**
* @note 帧率转换相关参数结构体
*/
typedef struct
{
    AR_S32 s32SrcFrmRate;                  /* RW; 范围:[0, 240]; 编码通道的输入帧率 */
    AR_S32 s32DstFrmRate;                  /* RW; 范围:[0, 240]; 编码通道的输出帧率 */
} STRU_VENC_FRAME_RATE;

/**
* @note 编码通道的相关参数结构体
*/
typedef struct
{
    AR_BOOL bColor2Grey;                  /* RW; 范围:[0, 1]; 是否使能彩转灰 */
    AR_U32  u32Priority;                  /* RW; 范围:[0, 1]; 编码通道的优先级 */
//    AR_U32  u32MaxStrmCnt;              /* RW: 范围:[0,4294967295]; Maximum number of frames in a stream buffer*/
    AR_U32 u32CmdQueueDepth;              /* RW; Range:[2,4]; 设置基于cmd级的并行模式的queue深度 note:仅对HEVC和AVC有效 */
    AR_U32  u32PollWakeUpFrmCnt;          /* RW: 范围:(0,4294967295];  阻塞方式获取码流时，多少帧唤醒一次 */
    STRU_VENC_CROP_INFO    stCropInfo;    /* crop参数结构体 */
//    STRU_VENC_FRAME_RATE   stFrameRate;   /* not support */
} STRU_VENC_CHN_PARAM;

/**
 * @note 编码通道的vb pool相关参数结构体
 * */
typedef struct
{
    VB_POOL picVbPool;             /**< 可读写; 用于存储Picture的VB池PoolId。*/
    VB_POOL picInfoVbPool;         /**< 可读写; 用于存储Picture的信息（pme、tmv、pmeinfo）的VB池PoolId。*/
} STRU_VENC_CHN_POOL_S;

/********************** VDEC DEFINE ******************************/
/**
 * @note 定义解码器的错误忽略类型,与vpuapi.h定义一致, H264&H265
 */
typedef enum {
    HAL_ERRCONCEAL_MODE_OFF         = 0, /**< 不使能 */
    HAL_ERRCONCEALMODE_INTRA_ONLY   = 1, /**< 对Intra和Inter都使能Intra纠错 */
    HAL_ERRCONCEAL_MODE_INTRA_INTER = 2  /**< 对Intra使能Intra纠错, 对Inter使能Inter纠错 */
} ENUM_ERRCONCEAL_MODE;

/**
 * @note 定义解码器的错误忽略单元,H264&H265
 */
typedef enum {
    HAL_ERRCONCEAL_UNIT_PICTURE    = 0, /**< Picture-level 纠错 */
    HAL_ERRCONCEAL_UNIT_SLICE_TILE = 1, /**< Slice/Tile-level 纠错 */
    HAL_ERRCONCEAL_UNIT_BLOCK_ROW  = 2, /**< Block-Row-level 纠错 */
    HAL_ERRCONCEAL_UNIT_BLOCK      = 3  /**< Block-level 纠错 */
} ENUM_ERRCONCEAL_UNIT;
#define   HAL_USERDATA_MAX_INDEX                 (32)
/**
 * @note 定义H265解码器,用户数据报告标志
 */
typedef enum {
    HAL_H265_USERDATA_FLAG_RESERVED_0                      = 0,
    HAL_H265_USERDATA_FLAG_RESERVED_1                      = 1,
    HAL_H265_USERDATA_FLAG_VUI                             = 2,
    HAL_H265_USERDATA_FLAG_RESERVED_3                      = 3,
    HAL_H265_USERDATA_FLAG_PIC_TIMING                      = 4,
    HAL_H265_USERDATA_FLAG_ITU_T_T35_PRE                   = 5,  /* SEI Prefix: user_data_registered_itu_t_t35 */
    HAL_H265_USERDATA_FLAG_UNREGISTERED_PRE                = 6,  /* SEI Prefix: user_data_unregistered */
    HAL_H265_USERDATA_FLAG_ITU_T_T35_SUF                   = 7,  /* SEI Suffix: user_data_registered_itu_t_t35 */
    HAL_H265_USERDATA_FLAG_UNREGISTERED_SUF                = 8,  /* SEI Suffix: user_data_unregistered */
    HAL_H265_USERDATA_FLAG_RECOVERY_POINT                  = 9,  /* SEI RESERVED */
    HAL_H265_USERDATA_FLAG_MASTERING_COLOR_VOL             = 10, /* SEI Prefix: mastering_display_color_volume */
    HAL_H265_USERDATA_FLAG_CHROMA_RESAMPLING_FILTER_HINT   = 11, /* SEI Prefix: chroma_resampling_filter_hint */
    HAL_H265_USERDATA_FLAG_KNEE_FUNCTION_INFO              = 12, /* SEI Prefix: knee_function_info */
    HAL_H265_USERDATA_FLAG_TONE_MAPPING_INFO               = 13, /* SEI Prefix: tone_mapping_info */
    HAL_H265_USERDATA_FLAG_FILM_GRAIN_CHARACTERISTICS_INFO = 14, /* SEI Prefix: film_grain_characteristics_info */
    HAL_H265_USERDATA_FLAG_CONTENT_LIGHT_LEVEL_INFO        = 15, /* SEI Prefix: content_light_level_info */
    HAL_H265_USERDATA_FLAG_COLOUR_REMAPPING_INFO           = 16, /* SEI Prefix: content_light_level_info */
    HAL_H265_USERDATA_FLAG_ITU_T_T35_PRE_1                 = 28, /* SEI Prefix: additional user_data_registered_itu_t_t35 */
    HAL_H265_USERDATA_FLAG_ITU_T_T35_PRE_2                 = 29, /* SEI Prefix: additional user_data_registered_itu_t_t35 */
    HAL_H265_USERDATA_FLAG_ITU_T_T35_SUF_1                 = 30, /* SEI Suffix: additional user_data_registered_itu_t_t35 */
    HAL_H265_USERDATA_FLAG_ITU_T_T35_SUF_2                 = 31, /* SEI Suffix: additional user_data_registered_itu_t_t35 */
} ENUM_H265_USERDATA_FLAG;
/**
 * @note 定义H264解码器,用户数据报告标志
 */
typedef enum {
    HAL_H264_USERDATA_FLAG_RESERVED_0                      = 0,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_1                      = 1,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_VUI                             = 2,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_3                      = 3,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_PIC_TIMING                      = 4,  /* SEI : pic_timing */
    HAL_H264_USERDATA_FLAG_ITU_T_T35                       = 5,  /* SEI : user_data_registered_itu_t_t35 */
    HAL_H264_USERDATA_FLAG_UNREGISTERED                    = 6,  /* SEI Prefix: user_data_unregistered */
    HAL_H264_USERDATA_FLAG_RESERVED_7                      = 7,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_8                      = 8,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_9                      = 9,  /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_10                     = 10, /* SEI : mastering_display_color_volume */
    HAL_H264_USERDATA_FLAG_RESERVED_11                     = 11, /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_RESERVED_12                     = 12, /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_TONE_MAPPING_INFO               = 13, /* SEI : tone_mapping_info */
    HAL_H264_USERDATA_FLAG_FILM_GRAIN_CHARACTERISTICS_INFO = 14, /* SEI : film_grain_characteristics_info */
    HAL_H264_USERDATA_FLAG_RESERVED_15                     = 15, /* SEI RESERVED */
    HAL_H264_USERDATA_FLAG_COLOUR_REMAPPING_INFO           = 16, /* SEI : colour_remapping_info */
    HAL_H264_USERDATA_FLAG_ITU_T_T35_1                     = 28, /* SEI : user_data_registered_itu_t_t35_1 */
    HAL_H264_USERDATA_FLAG_ITU_T_T35_2                     = 29, /* SEI : user_data_registered_itu_t_t35_2 */
} ENUM_H264_USERDATA_FLAG;
/**
 * @note 定义H26x解码器检测序列参数集变化的标志
 */
typedef enum {
    HAL_H26X_SPS_PROFILE    = (1<<5),
    HAL_H26X_SPS_CHROMA_FORMAT_IDC = (1<<15),
    HAL_H26X_SPS_SIZE       = (1<<16),
    HAL_H26X_SPS_BITDEPTH   = (1<<18),
    HAL_H265_SPS_DPB_COUNT  = (1<<19),
} ENUM_H26X_SPS_CHANGE_FLAG;

/**
* @note 定义解码器相关的静态参数
*/
typedef struct
{
    AR_BOOL              bSemiPlanner;    /**< RW; 是否使能CbCr交织存放,也就是semi planner */
    AR_BOOL              bTemporalMvpEnable; /**< RW; 范围:[0, 1]; 是否支持时域运动矢量预测 */
    AR_U32               u32CmdQueueDepth;/**< RW; 范围:[1, 4]; 设置基于cmd级的并行模式的queue深度 */
    AR_U32               u32DisplayFrameNum;/**< RW; 范围:[0, 16]; 专门用于显示的linear buffer个数 */
    AR_U32               u32RefFrameNum;  /**< RW, Range: [0, 16]; reference frame num */
    AR_U32               u32TmvBufSize;   /**< RW, 视频解码图像Tmv Buffer大小，仅PrivateVB 模式且bTemporalMvpEnable为1时有效 */
    AR_S32               s32UserdataFlag; /**< RW; 报告用户数据的标志1<<flag,flag参考ENUM_H265_USERDATA_FLAG和ENUM_H264_USERDATA_FLAG */
    AR_S32               s32TargetTemporalId; /**< RW; 范围:[0, SPS_MAX_SUB_LAYER], 0:不使能 非0:使能只解码TemporalId小于TargetID的功能 */
    AR_S32               s32SpsChangeFlag;    /**< RW; 参考ENUM_H26X_SPS_CHANGE_FLAG */
    ENUM_VCODEC_TYPE     enType;          /**< 待解码码流类型;参考ENUM_VCODEC_TYPE */
    AR_U64               u64BitStreamPa;
    AR_U64*              u64BitStreamVa;
    AR_U32               u32BitStreamLen;
} STRU_VDEC_ATTR_VIDEO;

/**
* @note 定义解码器实例相关的静态参数
*/
typedef struct
{
    ENUM_VCODEC_TYPE enType;              /**< 待解码码流类型;参考ENUM_VCODEC_TYPE */
    AR_U32           u32MaxWidth;         /**< RW; 范围:[32, 4096];解码码流最大水平分辨率,决定了参考帧内存的大小 */
    AR_U32           u32MaxHeight;        /**< RW; 范围:[32, 2160];解码码流最大垂直分辨率,决定了参考帧内存的大小 */
    AR_U32           u32FrameBufSize;     /**< RW; 解码图像帧存大小,0:由解码器自动决定大小; note:用户必须保证配置的帧存大小满足解码码流内存要求,否则不能解码.*/
    AR_U32           u32FrameBufCnt;      /**< RW; 解码图像帧存buffer大小 */
    union
    {
        STRU_VDEC_ATTR_VIDEO stVdecVideoAttr;    /**< H26x的解码器参数结构体 */
    };
} STRU_VDEC_INST_ATTR;

/**
* @note 定义视频解码模式枚举
*/
typedef enum
{
    HAL_VIDEO_DEC_MODE_IPB = 0,
    HAL_VIDEO_DEC_MODE_IP = 1,
    HAL_VIDEO_DEC_MODE_I = 2,
    HAL_VIDEO_DEC_MODE_MAX
} ENUM_VIDEO_DEC_MODE;

/**
* @note 定义视频解码输出顺序枚举
*/
typedef enum
{
    HAL_VIDEO_OUTPUT_ORDER_DISP = 0,
    HAL_VIDEO_OUTPUT_ORDER_DEC,
    HAL_VIDEO_OUTPUT_ORDER_MAX
} ENUM_VIDEO_OUTPUT_ORDER;

/**
* @note 定义解码通道高级参数
*/
typedef struct
{
    ENUM_VIDEO_DEC_MODE enDecMode; /**< 跳帧解码模式：Default：HAL_VIDEO_DEC_MODE_IP */
    ENUM_VIDEO_OUTPUT_ORDER enOutputOrder; /**< 解码图像输出顺序, Default：HAL_VIDEO_OUTPUT_ORDER_DEC */
    ENUM_FB_FORMAT          enOutputFormat;  /**< RW; 参考ENUM_FB_FORMAT,只支持YUV420和YUV422 */
    ENUM_ERRCONCEAL_MODE    enErrConcealMode;
    ENUM_ERRCONCEAL_UNIT    enErrConcealUnit;
    // TODO, add more
} STRU_VDEC_PARAM_VIDEO;

/**
* @note 定义解码通道高级参数
*/
typedef struct
{
    ENUM_VCODEC_TYPE enType;              /**< 待解码码流类型;参考ENUM_VCODEC_TYPE */
    AR_BOOL          bEnableUserData;     /* RW, Range: [0, 1]; enable userdata or not */
    AR_U32           u32DisplayFrameNum;  /**< 解码缓存图像的最小帧数;取值范围：[0, 16];Default：2 */
    AR_U32           u32ScaledWidth;      /* RW; scaled pic width, 按8对齐,最小为gen8; 0:不使能缩小 */
    AR_U32           u32ScaledHeight;     /* RW; scaled pic height, 8对齐,最小为8; 0:不使能缩小 */
    AR_U32           u32MiniBufMode;      /* RW, Range: [0, 1]; mini MMZ & VB Buffer of current channel */

    union
    {
        STRU_VDEC_PARAM_VIDEO stVdecVideoParam;  /**< 视频(H.264/H.265)解码高级参数 */
    };
} STRU_VDEC_CHN_PARAM;


/**
* @note 定义解码帧VB来源
*/
typedef enum
{
    HAL_VB_SOURCE_COMMON  = 0,
    HAL_VB_SOURCE_MODULE  = 1,
    HAL_VB_SOURCE_PRIVATE = 2,
    HAL_VB_SOURCE_USER    = 3,
    HAL_VB_SOURCE_MAX
} ENUM_VB_SOURCE;

/**
* @note 解码器模块级参数。
*/
typedef struct
{
    ENUM_VB_SOURCE         enVdecVBSource; /* RW, Range: [1, 3]; frame buffer mode */
    STRU_EVENT_MOD_PARAM   stEventModParam;
    AR_U32 u32MiniBufMode; /* RW, Range: [0, 1]; stream buffer mode */
    AR_U32 u32ParallelMode; /* RW, Range: [0, 1]; VDH working mode */
    AR_U32 u32H26xCoreClock;/* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note: share with h265, 根据解码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 */
    AR_U32 u32H26xBpuClock; /* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666, 700} Mhz note: share with h265, 比core低一档或者相等 */
    AR_U32 u32JpegCoreClock;/* RW; Range:{75, 150, 200, 250, 300, 360, 400, 450, 500, 600, 666} Mhz note: 根据解码的总pixel clock计算，比如1080p30=1920x1080x30=62M，选75M即可 */
} STRU_VDEC_MOD_PARAMS;

/**
* @note 定义解码通道绑定的VB池结构体。
*/
typedef struct
{
    AR_U32 hPicVbPool; /* RW; vb pool id for pic buffer */
    AR_U32 hTmvVbPool; /* RW; vb pool id for tmv buffer */
} STRU_VDEC_CHN_POOLS;

/**
* @note 定义解码器实例相关的错误信息
*/
typedef struct
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
} STRU_VDEC_ERROR;

/**
* @note 定义解码器实例相关的状态信息
*/
typedef struct
{
    ENUM_VCODEC_TYPE enType;          /**<只读; 解码码流类型;参考ENUM_VCODEC_TYPE */
    AR_S32  s32FrameBufferDelay;      /**<只读; 码流的frame delay */
    AR_S32  s32FrameDisplayFlag;      /**<只读; linear framebuffer各帧的使用情况 */
    AR_U32  u32LeftStreamFrames;      /**<只读; 剩余待解码的码流帧数 */
    AR_U32  u32LeftPics;              /**<只读; 剩余待输出的yuv图像 */
    AR_BOOL bStartRecvStream;         /**<只读; 解码器是否开始接收码流 */
    AR_U32  u32RecvStreamFrames;      /**<只读; 已经输入的码流帧数 */
    AR_U32  u32DecodeStreamFrames;    /**<只读; 已经解码的码流帧数 */
    AR_U32  u32ReleaseFrames;         /**<只读; 已经display完成的帧数 */
    AR_U32  u32SkipFrames;            /**<只读; 已经skip完成的帧数 */
    AR_U32  u32ErrorStreamFrames;     /**<只读; 已经解码错误的码流帧数 */
    AR_U32  u32WarnStreamFrames;      /**<只读; 已经解码报警的码流帧数 */
    AR_U32  u32Avail;                 /**<只读; 解码bit stream的剩余buffer*/
    AR_U64  u64RdPtr;                 /**<只读; 解码bit stream的读指针*/
    AR_U64  u64WrPtr;                 /**<只读; 解码bit stream的写指针*/
    STRU_VDEC_ERROR stVdecDecErr;     /**<只读; information about decode error */
} STRU_VDEC_STATUS;

/**
* @note 定义解码器解码码流数据相关的参数结构体
*/
typedef struct
{
    AR_U8     *pu8Addr;         /**< 码流起始虚拟地址; note:必须按8对齐 */
    AR_U64    u64PhyAddr;       /**< 码流起始物理地址 */
    AR_U32    u32Len;           /**< 码流长度, 0:代表EOF */
    AR_U64    u64PTS;           /**< pts  */
    AR_BOOL   bEndOfStream;     /**< 当前序列是否结束 */
    AR_S32    s32MsTimeout;     /**< 发送码流方式;-1:阻塞 0:非阻塞 正值:超时时间,以ms为单位 */
    //AR_BOOL   craAsBlaFlag;   /**< It handles CRA picture as BLA picture not to use reference from the previous decoded pictures (H.265/HEVC only) */
} STRU_VDEC_STREAM_FRAME;
/**
* @note 定义矩形显示区域结构体
*/
typedef struct {
    AR_U32 left;    /**< 距离(0,0)的水平方向左边界的偏移量 */
    AR_U32 top;     /**< 距离(0,0)的垂直方向上边界的偏移量 */
    AR_U32 right;   /**< 距离(0,0)的水平方向右边界的偏移量 */
    AR_U32 bottom;  /**< 距离(0,0)的水平方向下边界的偏移量 */
} STRU_DISPLAY_RECT;
/**
* @note 定义HEVC解码错误类型
*/
typedef enum {
    HAL_HEVC_SPSERR_SEQ_PARAMETER_SET_ID = 0x1000,/**< seq_parameter_set_id golomb decode error */
    HAL_HEVC_SPSERR_CHROMA_FORMAT_IDC = 0x1001,   /**< chroma_format_idc golomb decode error */
    HAL_HEVC_SPSERR_PIC_WIDTH_IN_LUMA_SAMPLES = 0x1002, /**< pic_width_in_luma_samples golomb decode error */
    HAL_HEVC_SPSERR_PIC_HEIGHT_IN_LUMA_SAMPLES = 0x1003, /**< pic_height_in_luma_samples golomb decode error */
    HAL_HEVC_SPSERR_CONF_WIN_LEFT_OFFSET = 0x1004, /**< conf_win_left_offset golomb decode error */
    HAL_HEVC_SPSERR_CONF_WIN_RIGHT_OFFSET = 0x1005, /**< conf_win_right_offset golomb decode error */
    HAL_HEVC_SPSERR_CONF_WIN_TOP_OFFSET = 0x1006, /**< conf_win_top_offset golomb decode error */
    HAL_HEVC_SPSERR_CONF_WIN_BOTTOM_OFFSET = 0x1007, /**< conf_win_top_offset golomb decode error */
    HAL_HEVC_SPSERR_BIT_DEPTH_LUMA_MINUS8 = 0x1008, /**< bit_depth_luma_minus8 golomb decode error */
    HAL_HEVC_SPSERR_BIT_DEPTH_CHROMA_MINUS8 = 0x1009, /**< bit_depth_chroma_minus8 golomb decode error */
    HAL_HEVC_SPSERR_LOG2_MAX_PIC_ORDER_CNT_LSB_MINUS4 =  0x100A, /**< log2_max_pic_order_cnt_lsb_minus4 golomb decode error */
    HAL_HEVC_SPSERR_SPS_MAX_DEC_PIC_BUFFERING = 0x100B, /**< sps_max_dec_pic_buffering[i] golomb decode error */
    HAL_HEVC_SPSERR_SPS_MAX_NUM_REORDER_PICS = 0x100C, /**< sps_max_num_reorder_pics[i] golomb decode error */
    HAL_HEVC_SPSERR_SPS_MAX_LATENCY_INCREASE = 0x100D, /**< sps_sps_max_latency_increase[i] golomb decode error */
    HAL_HEVC_SPSERR_LOG2_MIN_LUMA_CODING_BLOCK_SIZE_MINUS3 = 0x100E, /**<  log2_min_luma_coding_block_size_minus3 golomb decode error */
    HAL_HEVC_SPSERR_LOG2_DIFF_MAX_MIN_LUMA_CODING_BLOCK_SIZE = 0x100F, /**< log2_diff_max_min_luma_coding_block_size golomb decode error */
    HAL_HEVC_SPSERR_LOG2_MIN_TRANSFORM_BLOCK_SIZE_MINUS2 = 0x1010, /**< log2_min_transform_block_size_minus2 golomb decode error */
    HAL_HEVC_SPSERR_LOG2_DIFF_MAX_MIN_TRANSFORM_BLOCK_SIZE = 0x1011, /**< log2_diff_max_min_transform_block_size golomb decode error */
    HAL_HEVC_SPSERR_MAX_TRANSFORM_HIERARCHY_DEPTH_INTER = 0x1012, /**< max_transform_hierarchy_depth_inter golomb decode errro */
    HAL_HEVC_SPSERR_MAX_TRANSFORM_HIERARCHY_DEPTH_INTRA = 0x1013, /**< max_transform_hierarchy_depth_intra golomb decode error */
    HAL_HEVC_SPSERR_SCALING_LIST = 0x1014, /**< Scaling list parsing error */
    HAL_HEVC_SPSERR_LOG2_DIFF_MIN_PCM_LUMA_CODING_BLOCK_SIZE_MINUS3 = 0x1015, /**< log2_diff_min_pcm_luma_coding_block_size_minus3 golomb decode error */
    HAL_HEVC_SPSERR_LOG2_DIFF_MAX_MIN_PCM_LUMA_CODING_BLOCK_SIZE = 0x1016, /**< log2_diff_max_min_pcm_luma_coding_block_size golomb decode error */
    HAL_HEVC_SPSERR_NUM_SHORT_TERM_REF_PIC_SETS = 0x1017, /**< num_short_term_ref_pic_sets golomb decode error */
    HAL_HEVC_SPSERR_NUM_LONG_TERM_REF_PICS_SPS = 0x1018, /**< num_long_term_ref_pics_sps golomb decode error */
    HAL_HEVC_SPSERR_GBU_PARSING_ERROR = 0x1019, /**< Lack of stream forces decode error */
    HAL_HEVC_SPSERR_RANGE_EXTENSION_FLAG = 0x101A, /**< range_extension parsing error */
    HAL_HEVC_SPSERR_VUI_ERROR = 0x101B, /**< VUI parameter decode error */
    HAL_HEVC_SPSERR_ACTIVATE_SPS = 0x101C, /**< activate_sps decode error */

    HAL_HEVC_PPSERR_PPS_PIC_PARAMETER_SET_ID = 0x2000, /**< PPS ID parsing error */
    HAL_HEVC_PPSERR_PPS_SEQ_PARAMETER_SET_ID = 0x2001, /**< SPS ID parsing error */
    HAL_HEVC_PPSERR_NUM_REF_IDX_L0_DEFAULT_ACTIVE_MINUS1 = 0x2002, /**< num_ref_idx_l0_active_minus1 parsing error */
    HAL_HEVC_PPSERR_NUM_REF_IDX_L1_DEFAULT_ACTIVE_MINUS1 = 0x2003, /**< num_ref_idx_l1_active_minus1 parsing error */
    HAL_HEVC_PPSERR_INIT_QP_MINUS26 = 0x2004, /**< Initial qp minus26 parsing error */
    HAL_HEVC_PPSERR_DIFF_CU_QP_DELTA_DEPTH = 0x2005, /**< diff_cu_qp_delta_depth parsing error */
    HAL_HAL_HEVC_PPSERR_PPS_CB_QP_OFFSET = 0x2006, /**< cb_qp_offset parsing error */
    HAL_HEVC_PPSERR_PPS_CR_QP_OFFSET = 0x2007, /**< cr_qp_offset parsing error */
    HAL_HEVC_PPSERR_NUM_TILE_COLUMNS_MINUS1 = 0x2008, /**< num_tile_columns_minus1 parsing error */
    HAL_HEVC_PPSERR_NUM_TILE_ROWS_MINUS1 = 0x2009, /**< num_tile_rows_minus1 parsing error */
    HAL_HEVC_PPSERR_COLUMN_WIDTH_MINUS1 = 0x200A, /**< column_width_minus1 parsing error */
    HAL_HEVC_PPSERR_ROW_HEIGHT_MINUS1 = 0x200B, /**< row_height_minus1 parsing error */
    HAL_HEVC_PPSERR_PPS_BETA_OFFSET_DIV2 = 0x200C, /**< pps_beta_offset_div2 parsing error */
    HAL_HEVC_PPSERR_PPS_TC_OFFSET_DIV2 = 0x200D, /**< pps_tc_offset_div2 parsing error */
    HAL_HEVC_PPSERR_SCALING_LIST = 0x200E, /**< Scaling list parsing error */
    HAL_HEVC_PPSERR_LOG2_PARALLEL_MERGE_LEVEL_MINUS2 = 0x200F, /**< log2_parallel_merge_level_minus2 parsing error */
    HAL_HEVC_PPSERR_NUM_TILE_COLUMNS_RANGE_OUT = 0x2010, /**< num_tile_columns range out error */
    HAL_HEVC_PPSERR_NUM_TILE_ROWS_RANGE_OUT = 0x2011, /**< num_tile_rows range out error */
    HAL_HEVC_PPSERR_MORE_RBSP_DATA_ERROR = 0x2012, /**< more_rbsp_data parsing error */
    HAL_HEVC_PPSERR_PPS_PIC_PARAMETER_SET_ID_RANGE_OUT = 0x2013, /**< PPS ID range out */
    HAL_HEVC_PPSERR_PPS_SEQ_PARAMETER_SET_ID_RANGE_OUT = 0x2014, /**< SPS ID range out */
    HAL_HEVC_PPSERR_NUM_REF_IDX_L0_DEFAULT_ACTIVE_MINUS1_RANGE_OUT = 0x2015, /**< num_ref_idx_l0_default_active_minus1 range out */
    HAL_HEVC_PPSERR_NUM_REF_IDX_L1_DEFAULT_ACTIVE_MINUS1_RANGE_OUT = 0x2016, /**< num_ref_idx_l1_default_active_minus1 range out */
    HAL_HEVC_PPSERR_PPS_CB_QP_OFFSET_RANGE_OUT = 0x2017, /**< cb_qp_offset range out */
    HAL_HEVC_PPSERR_PPS_CR_QP_OFFSET_RANGE_OUT = 0x2018, /**< cr_qp_offset range out */
    HAL_HEVC_PPSERR_COLUMN_WIDTH_MINUS1_RANGE_OUT = 0x2019, /**< column_width_minus1 range out */
    HAL_HEVC_PPSERR_ROW_HEIGHT_MINUS1_RANGE_OUT = 0x2020, /**< row_height_minus1 range out */
    HAL_HEVC_PPSERR_PPS_BETA_OFFSET_DIV2_RANGE_OUT = 0x2021, /**< pps_beta_offset_div2 range out error */
    HAL_HEVC_PPSERR_PPS_TC_OFFSET_DIV2_RANGE_OUT = 0x2022, /**< pps_tc_offset_div2 range out error */

    HAL_HEVC_SHERR_SLICE_PIC_PARAMETER_SET_ID = 0x3000, /**< slice_pic_parameter_set_id decode error */
    HAL_HEVC_SHERR_ACTIVATE_PPS = 0x3001, /**< activate_pps decode error */
    HAL_HEVC_SHERR_ACTIVATE_SPS = 0x3002, /**< activate_sps decode error */
    HAL_HEVC_SHERR_SLICE_TYPE = 0x3003, /**< slice_type decode error */
    HAL_HEVC_SHERR_FIRST_SLICE_IS_DEPENDENT_SLICE = 0x3004, /**< first_slice must be independent slice */
    HAL_HEVC_SHERR_SHORT_TERM_REF_PIC_SET_SPS_FLAG = 0x3005, /**< short_term_ref_pic_set_sps_flag shall be equal to the first slice */
    HAL_HEVC_SHERR_SHORT_TERM_REF_PIC_SET = 0x3006, /**< short_term_ref_pic_set decode error*/
    HAL_HEVC_SHERR_SHORT_TERM_REF_PIC_SET_IDX = 0x3007, /**< short_term_ref_pic_set_idx shall be equal to the first slice */
    HAL_HEVC_SHERR_NUM_LONG_TERM_SPS = 0x3008, /**< num_long_term_sps decode error */
    HAL_HEVC_SHERR_NUM_LONG_TERM_PICS = 0x3009, /**< num_long_term_pics decode error */
    HAL_HEVC_SHERR_LT_IDX_SPS_IS_OUT_OF_RANGE = 0x300A, /**< lt_idx_sps is out of range */
    HAL_HEVC_SHERR_DELTA_POC_MSB_CYCLE_LT = 0x300B, /**< delta_poc_msb_cycle_lt decode error */
    HAL_HEVC_SHERR_NUM_REF_IDX_L0_ACTIVE_MINUS1 = 0x300C, /**< num_ref_idx_l0_active_minus1 decode error */
    HAL_HEVC_SHERR_NUM_REF_IDX_L1_ACTIVE_MINUS1 = 0x300D, /**< num_ref_idx_l1_active_minus1 decode error */
    HAL_HEVC_SHERR_COLLOCATED_REF_IDX = 0x300E, /**< collocated_ref_idx decode error */
    HAL_HEVC_SHERR_PRED_WEIGHT_TABLE = 0x300F, /**< pred_weight_table decode error */
    HAL_HEVC_SHERR_FIVE_MINUS_MAX_NUM_MERGE_CAND = 0x3010, /**< five_minus_max_num_merge_cand decode error */
    HAL_HEVC_SHERR_SLICE_QP_DELTA = 0x3011, /**< slice_qp_delta decode error */
    HAL_HEVC_SHERR_SLICE_QP_DELTA_IS_OUT_OF_RANGE = 0x3012, /**< slice_qp_delta is out of range */
    HAL_HEVC_SHERR_SLICE_CB_QP_OFFSET = 0x3013, /**< slice_cb_qp_offset decode error */
    HAL_HEVC_SHERR_SLICE_CR_QP_OFFSET = 0x3014, /**< slice_cr_qp_offset decode error */
    HAL_HEVC_SHERR_SLICE_BETA_OFFSET_DIV2 = 0x3015, /**< slice_beta_offset_div2 decode error */
    HAL_HEVC_SHERR_SLICE_TC_OFFSET_DIV2 = 0x3016, /**< slice_tc_offset_div2 decode error */
    HAL_HEVC_SHERR_NUM_ENTRY_POINT_OFFSETS = 0x3017, /**< num_entry_point_offsets decode error */
    HAL_HEVC_SHERR_OFFSET_LEN_MINUS1 = 0x3018, /**< offset_len_minus1 decode error */
    HAL_HEVC_SHERR_SLICE_SEGMENT_HEADER_EXTENSION_LENGTH = 0x3019, /**< slice_segment_header_extension_length decode error */

    HAL_HEVC_SPEC_OVER_PICTURE_WIDTH_SIZE = 0x4000, /**< pic_width_in_luma_samples or pic_height_in_luma_samples was out of spec */
    HAL_HEVC_SPEC_OVER_PICTURE_HEIGHT_SIZE = 0x4001, /**< pic_width_in_luma_samples or pic_height_in_luma_samples was out of spec */
    HAL_HEVC_SPEC_OVER_CHROMA_FORMAT = 0x4002, /**< chroma_format_idc was out of spec */
    HAL_HEVC_SPEC_OVER_BIT_DEPTH = 0x4003, /**< bit_depth_luma_minus8 or bit_depth_chroma_minus was out of spec */

    HAL_HEVC_ETC_INIT_SEQ_SPS_NOT_FOUND = 0x5000, /**< SPS was not found */
    HAL_HEVC_ETC_DEC_PIC_VCL_NOT_FOUND = 0x5001, /**< VCL was not found */
    HAL_HEVC_ETC_NO_VALID_SLICE_IN_AU = 0x5002,  /**< The First VCL of the next AU was detected while parsing the slice header */
} ENUM_HEVC_DECODE_ERR;
/**
* @note 定义HEVC解码报警类型
*/
typedef enum {
    HAL_HEVC_SPSWARN_MAX_SUB_LAYERS_MINUS1 = 0x1, /**< sps_max_sub_layer_minus1 shall be 0 to 6 */
    HAL_HEVC_SPSWARN_GENERAL_RESERVED_ZERO_44BITS = 0x2, /**< general_reserved_zero_44bits shall be 0 */
    HAL_HEVC_SPSWARN_RESERVED_ZERO_2BITS = 0x4, /**< reserved_zero_2bits shall be 0 */
    HAL_HEVC_SPSWARN_SUB_LAYER_RESERVED_ZERO_44BITS = 0x8, /**< sub_layer_reserved_zero_44bits shall be 0 */
    HAL_HEVC_SPSWARN_GENERAL_LEVEL_IDC = 0x10, /**< general_level_idc shall have one of level of Table A.1 */
    HAL_HEVC_SPSWARN_SPS_MAX_DEC_PIC_BUFFERING_VALUE_OVER = 0x20, /**< sps_max_dec_pic_buffering[i] <= MaxDpbSize */
    HAL_HEVC_SPSWARN_RBSP_TRAILING_BITS = 0x40, /**< Trailing bits shall be 1000… pattern, 7.3.2.1 */
    HAL_HEVC_SPSWARN_ST_RPS_UE_ERROR = 0x80, /**< Error happened while parsing RPS (Reference Picture Set syntax) into ue (unsigned exponential golomb code) */

    HAL_HEVC_PPSWARN_RBSP_TRAILING_BITS = 0x100, /**< Trailing bits shall be 1000… pattern, 7.3.2.11. */
    HAL_HEVC_PPSWARN_REPLACED_WITH_PREV_PPS = 0x200,

    HAL_HEVC_SHWARN_FIRST_SLICE_SEGMENT_IN_PIC_FLAG = 0x1000, /**< first_slice_segment_in_pic_flag shall be 1 at first slice */
    HAL_HEVC_SHWARN_NO_OUTPUT_OF_PRIOR_PICS_FLAG = 0x2000, /**< no_output_of_prior_pics_flag has different value at same AU */
    HAL_HEVC_SHWARN_PIC_OUTPUT_FLAG = 0x4000, /**< pic_output_flag has a different value at the same AU. */
    HAL_HEVC_SHWARN_DUPLICATED_SLICE_SEGMENT = 0x8000, /**< an slice segment is duplicated and abandoned */

    HAL_HEVC_ETC_INIT_SEQ_VCL_NOT_FOUND = 0x10000, /**< VCL not found */
    HAL_HEVC_ETC_MISSING_REFERENCE_PICTURE = 0x20000, /**< The reference picture was missing */
    HAL_HEVC_ETC_WRONG_TEMPORAL_ID = 0x40000, /**< The picture has a wrong temporal ID */
    HAL_HEVC_ETC_ERROR_PICTURE_IS_REFERENCED = 0x80000, /**< The picture was decoded with error reference picture */

    HAL_HEVC_SPEC_OVER_PROFILE = 0x100000, /**<  general_profile_idc and general_profile_compatibility_flag are out of specification */
    HAL_HEVC_SPEC_OVER_LEVEL = 0x200000, /**< general_level_idc is out of specification. The level is exceeded than the supported one in VPU.
                                     It can cause performance degradation or handup during decoding due to the size of buffer */
    HAL_HEVC_SPSWARN_EXTENSION_FLAG = 0x1000000,
    HAL_HEVC_SPSWARN_REPLACED_WITH_PREV_SPS = 0x2000000,
} ENUM_HEVC_DECODE_WARN;

/**
* @note 定义AVC解码错误类型
*/
typedef enum {
    HAL_AVC_SPSERR_SEQ_PARAMETER_SET_ID = 0x1000,/**< seq_parameter_set_id golomb decode error */
    HAL_AVC_SPSERR_CHROMA_FORMAT_IDC = 0x1001,   /**< chroma_format_idc golomb decode error */
    HAL_AVC_SPSERR_PIC_WIDTH_IN_LUMA_SAMPLES = 0x1002, /**< pic_width_in_luma_samples golomb decode error */
    HAL_AVC_SPSERR_PIC_HEIGHT_IN_LUMA_SAMPLES = 0x1003, /**< pic_height_in_luma_samples golomb decode error */
    HAL_AVC_SPSERR_CONF_WIN_LEFT_OFFSET = 0x1004, /**< conf_win_left_offset golomb decode error */
    HAL_AVC_SPSERR_CONF_WIN_RIGHT_OFFSET = 0x1005, /**< conf_win_right_offset golomb decode error */
    HAL_AVC_SPSERR_CONF_WIN_TOP_OFFSET = 0x1006, /**< conf_win_top_offset golomb decode error */
    HAL_AVC_SPSERR_CONF_WIN_BOTTOM_OFFSET = 0x1007, /**< conf_win_top_offset golomb decode error */
    HAL_AVC_SPSERR_BIT_DEPTH_LUMA_MINUS8 = 0x1008, /**< bit_depth_luma_minus8 golomb decode error */
    HAL_AVC_SPSERR_BIT_DEPTH_CHROMA_MINUS8 = 0x1009, /**< bit_depth_chroma_minus8 golomb decode error */
    HAL_AVC_SPSERR_SPS_MAX_DEC_PIC_BUFFERING = 0x100B, /**< sps_max_dec_pic_buffering[i] golomb decode error */
    HAL_AVC_SPSERR_SPS_MAX_NUM_REORDER_PICS = 0x100C, /**< sps_max_num_reorder_pics[i] golomb decode error */
    HAL_AVC_SPSERR_SCALING_LIST = 0x1014, /**< Scaling list parsing error */
    HAL_AVC_SPSERR_GBU_PARSING_ERROR = 0x1019, /**< Lack of stream forces decode error */
    HAL_AVC_SPSERR_VUI_ERROR = 0x101B, /**< VUI parameter decode error */
    HAL_AVC_SPSERR_ACTIVATE_SPS = 0x101C, /**< activate_sps decode error */

    HAL_AVC_PPSERR_PPS_PIC_PARAMETER_SET_ID = 0x2000, /**< PPS ID parsing error */
    HAL_AVC_PPSERR_PPS_SEQ_PARAMETER_SET_ID = 0x2001, /**< SPS ID parsing error */
    HAL_AVC_PPSERR_NUM_REF_IDX_L0_DEFAULT_ACTIVE_MINUS1 = 0x2002, /**< num_ref_idx_l0_active_minus1 parsing error */
    HAL_AVC_PPSERR_NUM_REF_IDX_L1_DEFAULT_ACTIVE_MINUS1 = 0x2003, /**< num_ref_idx_l1_active_minus1 parsing error */
    HAL_AVC_PPSERR_INIT_QP_MINUS26 = 0x2004, /**< Initial qp minus26 parsing error */
    HAL_AVC_PPSERR_PPS_CB_QP_OFFSET = 0x2006, /**< cb_qp_offset parsing error */
    HAL_AVC_PPSERR_PPS_CR_QP_OFFSET = 0x2007, /**< cr_qp_offset parsing error */
    HAL_AVC_PPSERR_SCALING_LIST = 0x200E, /**< Scaling list parsing error */
    HAL_AVC_PPSERR_MORE_RBSP_DATA_ERROR = 0x2012, /**< more_rbsp_data parsing error */
    HAL_AVC_PPSERR_PPS_PIC_PARAMETER_SET_ID_RANGE_OUT = 0x2013, /**< PPS ID range out */
    HAL_AVC_PPSERR_PPS_SEQ_PARAMETER_SET_ID_RANGE_OUT = 0x2014, /**< SPS ID range out */
    HAL_AVC_PPSERR_NUM_REF_IDX_L0_DEFAULT_ACTIVE_MINUS1_RANGE_OUT = 0x2015, /**< num_ref_idx_l0_default_active_minus1 range out */
    HAL_AVC_PPSERR_NUM_REF_IDX_L1_DEFAULT_ACTIVE_MINUS1_RANGE_OUT = 0x2016, /**< num_ref_idx_l1_default_active_minus1 range out */
    HAL_AVC_PPSERR_PPS_CB_QP_OFFSET_RANGE_OUT = 0x2017, /**< cb_qp_offset range out */
    HAL_AVC_PPSERR_PPS_CR_QP_OFFSET_RANGE_OUT = 0x2018, /**< cr_qp_offset range out */

    HAL_AVC_SHERR_SLICE_PIC_PARAMETER_SET_ID = 0x3000, /**< slice_pic_parameter_set_id decode error */
    HAL_AVC_SHERR_ACTIVATE_PPS = 0x3001, /**< activate_pps decode error */
    HAL_AVC_SHERR_ACTIVATE_SPS = 0x3002, /**< activate_sps decode error */
    HAL_AVC_SHERR_SLICE_TYPE = 0x3003, /**< slice_type decode error */
    HAL_AVC_SHERR_SHORT_TERM_REF_PIC_SET = 0x3006, /**< short_term_ref_pic_set decode error*/
    HAL_AVC_SHERR_LT_IDX_SPS_IS_OUT_OF_RANGE = 0x300A, /**< lt_idx_sps is out of range */
    HAL_AVC_SHERR_NUM_REF_IDX_L0_ACTIVE_MINUS1 = 0x300C, /**< num_ref_idx_l0_active_minus1 decode error */
    HAL_AVC_SHERR_NUM_REF_IDX_L1_ACTIVE_MINUS1 = 0x300D, /**< num_ref_idx_l1_active_minus1 decode error */
    HAL_AVC_SHERR_PRED_WEIGHT_TABLE = 0x300F, /**< pred_weight_table decode error */
    HAL_AVC_SHERR_SLICE_QP_DELTA = 0x3011, /**< slice_qp_delta decode error */
    HAL_AVC_SHERR_SLICE_BETA_OFFSET_DIV2 = 0x3015, /**< slice_beta_offset_div2 decode error */
    HAL_AVC_SHERR_SLICE_TC_OFFSET_DIV2 = 0x3016, /**< slice_tc_offset_div2 decode error */

    HAL_AVC_SPEC_OVER_PICTURE_WIDTH_SIZE = 0x4000, /**< pic_width_in_luma_samples or pic_height_in_luma_samples was out of spec */
    HAL_AVC_SPEC_OVER_PICTURE_HEIGHT_SIZE = 0x4001, /**< pic_width_in_luma_samples or pic_height_in_luma_samples was out of spec */
    HAL_AVC_SPEC_OVER_CHROMA_FORMAT = 0x4002, /**< chroma_format_idc was out of spec */
    HAL_AVC_SPEC_OVER_BIT_DEPTH = 0x4003, /**< bit_depth_luma_minus8 or bit_depth_chroma_minus was out of spec */

    HAL_AVC_ETC_INIT_SEQ_SPS_NOT_FOUND = 0x5000, /**< SPS was not found */
    HAL_AVC_ETC_DEC_PIC_VCL_NOT_FOUND = 0x5001, /**< VCL was not found */
    HAL_AVC_ETC_NO_VALID_SLICE_IN_AU = 0x5002,  /**< The First VCL of the next AU was detected while parsing the slice header */
} ENUM_AVC_DECODE_ERR;
/**
* @note 定义AVC解码报警类型
*/
typedef enum {
    HAL_AVC_SPSWARN_RESERVED_ZERO_2BITS = 0x4, /**< reserved_zero_2bits shall be 0 */
    HAL_AVC_SPSWARN_GENERAL_LEVEL_IDC = 0x10, /**< general_level_idc shall have one of level of Table A.1 */
    HAL_AVC_SPSWARN_RBSP_TRAILING_BITS = 0x40, /**< Trailing bits shall be 1000… pattern, 7.3.2.1 */

    HAL_AVC_PPSWARN_RBSP_TRAILING_BITS = 0x100, /**< Trailing bits shall be 1000… pattern, 7.3.2.11. */

    HAL_AVC_SHWARN_NO_OUTPUT_OF_PRIOR_PICS_FLAG = 0x2000, /**< no_output_of_prior_pics_flag has different value at same AU */

    HAL_AVC_ETC_INIT_SEQ_VCL_NOT_FOUND = 0x10000, /**< VCL not found */
    HAL_AVC_ETC_MISSING_REFERENCE_PICTURE = 0x20000, /**< The reference picture was missing */
    HAL_AVC_ETC_ERROR_PICTURE_IS_REFERENCED = 0x80000, /**< The picture was decoded with error reference picture */

    HAL_AVC_SPEC_OVER_PROFILE = 0x100000, /**<  general_profile_idc and general_profile_compatibility_flag are out of specification */
    HAL_AVC_SPEC_OVER_LEVEL = 0x200000, /**< general_level_idc is out of specification. The level is exceeded than the supported one in VPU.
                                     It can cause performance degradation or handup during decoding due to the size of buffer */
} ENUM_AVC_DECODE_WARN;
/**
* @note 定义解码器解码输出图像数据相关的结构体
*/
typedef struct
{
    AR_BOOL              bSemiPlanner;                 /**< 是否使能CbCr交织存放,也就是semi planner */
    AR_S32               s32Stride;                    /**< Y分量的水平stride */
    AR_S32               s32indexFrameNext;            /**< 下帧的帧存的索引, -1代表帧存没有分配,-2代表没有帧存可用需要尽快释放占用帧存*/
    AR_S32               s32IndexFrameDisplay;         /**< 显示帧的ID -3或-2代表因为reorder或者skip没有帧显示,-1代表序列结束*/
    AR_S32               s32IndexCompressFrameDisplay; /**< 显示帧对应压缩帧的ID */
    AR_S32               s32IndexFrameDecoded;         /**< 解码帧的索引; -2:因为到码流结束或者SKIP帧所以没有解码输出 -1:没有可以用的解码帧缓存 */
    AR_S32               s32IndexCompressFrameDecoded; /**< 解码帧对应的压缩帧的ID */
    AR_S32               s32NalType;                   /**< 解码帧的NAL类型,参考AVC或者HEVC的spec定义 */
    AR_S32               s32PicType;                   /**< This is the picture type of decoded picture. It reports the picture type of bottom field for interlaced stream */
    AR_S32               s32picTypeFirst;              /**< This is only valid in interlaced mode and indicates the picture type of the top field. */
    AR_S32               s32NumOfErrMBs;               /**< 解码帧的错误单元(MB/CTU)总数 */
    AR_S32               s32NumOfTotMBs;               /**< 解码帧的总单元(MB/CTU)数 */
    AR_S32               s32NumOfErrMBsInDisplay;      /**< 显示帧的错误单元(MB/CTU)总数 */
    AR_S32               s32NumOfTotMBsInDisplay;      /**< 显示帧的总单元(MB/CTU)数 */
    AR_S32               s32DispPicWidth;              /**< 显示帧的宽度 */
    AR_S32               s32DispPicHeight;             /**< 显示帧的高度 */
    AR_S32               s32DecPicWidth;               /**< 解码帧的宽度 */
    AR_S32               s32DecPicHeight;              /**< 解码帧的高度 */
    AR_S32               s32AspectRateInfo;            /**< 宽高比,参考Table E-1 of Annex E.2 of VUI of H.265 specification */
    AR_S32               s32InterlacedFrame;           /**< 0: progressive frame which consists of one picture;1 : interlaced frame which consists of two fields */
    AR_S32               s32PictureStructure;          /**< This variable indicates that the decoded picture is progressive or interlaced picture. */
    AR_S32               s32TopFieldFirst;             /**< For decoded picture consisting of two fields, 0 : VPU decodes the bottom field and then top field;
                                                                                                          1 : VPU decodes the top field and then bottom field */
    AR_S32               s32RateNumerator;             /**< 帧率的分子 */
    AR_S32               s32RateDenominator;           /**< 帧率的分母 */
    AR_S32               s32DecodedPOC;                /**< 解码帧的POC */
    AR_S32               s32DisplayPOC;                /**< 显示帧的POC */
    AR_S32               s32TemporalId;                /**< 解码帧的temporal ID */
    AR_S32               s32ConsumedByte;      /**< 当前解码帧消耗的码流byte */
    AR_S32               s32FrameDisplayFlag;  /**< 当前所有显示帧被占用的状态  */
    AR_S32               s32SequenceChanged;   /**< 序列参数变化标志,bit[5]: H.265 profile变化 bit [16]:分辨率变化 bit [19]:DPB个数变化 */
    AR_S32               s32FrameCycle;        /**< 解码帧的处理时间 */
    AR_S32               s32ErrorReason;       /**< 解码错误的原因,参考ENUM_HEVC_DECODE_WARN */
    AR_S32               s32WarnInfo;          /**< 解码的报警信息,参考ENUM_HEVC_DECODE_WARN */
    AR_S32               s32CtuSize;           /**< CTU size, 0:16x16 1:32x32 2:64x64 */
    AR_S32               s32OutputFlag;        /**< slice头信息中pic_output_flag语法 */
    AR_S32               s32MsTimeout;         /**< 获取解码帧方式;-1:阻塞 0:非阻塞 正值:超时时间,以ms为单位 */
    AR_U32               u32PoolId;            /**< 用于存储Picture的VB池Poold */
    AR_U32               s32SequenceNo;        /**< 初始值是0,序列参数变化一次加1 */
    AR_U32               u32DecodingSuccess;   /**< 解码是否完成; 0:没有解码完成 1:解码完成 */
    AR_U64               u64AddrY;             /**< Y分量的物理地址 note:必须按16对齐 */
    AR_U64               u64AddrCb;            /**< Cb分量的物理地址 note:必须按16对齐 */
    AR_U64               u64AddrCr;            /**< Cr分量的物理地址 note:必须按16对齐 */
    AR_U64               u8BytePosFrameStart;  /**< 解码当前帧的起始位置 */
    AR_U64               u8BytePosFrameEnd;    /**< 解码当前帧的结束位置 */
    AR_U64               u64ScheduleTimeUs;    /**< 解码帧的处理时间 */
    AR_U64               u64RdPtr;             /**< 码流读指针的位置 */
    AR_U64               u64PTS;               /**< 解码帧pts */
    ENUM_FB_FORMAT       enOutputFormat;       /**< 参考ENUM_FB_FORMAT,只支持YUV420和YUV422 */
    ENUM_PIC_TYPE        ePicType;             /**< 解码帧的类型, 参考ENUM_PIC_TYPE */
    STRU_DISPLAY_RECT    stDisplayRect;        /**< 显示帧的有效显示区域 */
    STRU_DISPLAY_RECT    stDecodedRect;        /**< 解码帧的有效显示区域 */
    //FrameBuffer        stDispFrame;          /**< 显示帧的信息结构体 */
} STRU_VDEC_VIDEO_FRAME;
/**
* @note 定义解码器HEVC&AVC解码输出头信息的相关的结构体
*/
typedef struct
{
      AR_S32          s32PicWidth;         /**< 水平分辨率 */
      AR_S32          s32PicHeight;        /**< 垂直分辨率 */
      AR_S32          s32FRateNumerator;   /**< 帧率的分子 */
      AR_S32          s32FRateDenominator; /**< 帧率的分母 */
      AR_S32          s32minFrameBufferCount;  /**< 解码需要的最少YUV buffer个数 */
      AR_S32          s32FrameBufDelay;        /**< 因为重新排序引起的最大显示延时buffer个数 */
      STRU_DISPLAY_RECT stPicCropRect;         /**< 窗口一致性信息 */

      AR_S32          s32MaxTemporalLayers;    /**< 最多 temporal sub-layers数 */
      AR_S32          s32Profile;              /**< H.265/H.264:profile_idc */
      AR_S32          s32Level;                /**< H.265/H.264:level_idc */
      AR_S32          s32Tier;                 /**< 0:Main 1:High */
      AR_S32          s32Interlace;            /**< 0:逐行 1: 隔行 */
      AR_S32          s32Constraint_set_flag[4]; /**< constraint_set0_flag ~ constraint_set3_flag; note: 只支持AVC */
      AR_S32          s32Direct8x8Flag;          /**< direct_8x8_inference_flag; note: 只支持AVC */
      AR_S32          s32IsExtSAR;               /**< 宽高比是否是扩展模式 */
      AR_S32          s32AspectRateInfo;       /**< 如果extsar是0 [7:0]代表宽高比IDC, 如果extsar是1 [31:16]代表宽, [15:0]代表高 */
      AR_S32          s32BitRate;              /**< -1:头信息没有码率 其他:头信息里的码率值 */
      AR_U32          u32UserDataHeader;       /**< 参考ENUM_H265_USERDATA_FLAG或者ENUM_H264_USERDATA_FLAG */
      AR_S32          s32UserDataNum;          /**< 用户数据个数 */
      AR_S32          s32UserDataSize;         /**< 用户数据大小 */
      AR_S32          s32UserDataBufFull;      /**< 用户数据缓冲区是否满 */

      AR_S32          s32ChromaFormatIDC;      /**< VUI信息中的色度格式 */
      AR_S32          s32LumaBitdepth;         /**< VUI信息中的亮度位深 */
      AR_S32          s32ChromaBitdepth;       /**< VUI信息中的色度位深 */
      AR_U32          u32SequenceNo;           /**< 序列号从0开始,改变一次加1 */
      AR_U32          u32VlcBufSize;           /**< VLC缓冲区大小 */
      AR_U32          u32ParamBufSize;         /**< 参数缓冲区大小 */
} STRU_VDEC_HEADER_INFO;
/**
* @note 定义解码器解码输出用户数据相关的结构体
*/
typedef struct
{
    AR_U64     u64UserDataPhyAddr;         /**< 保存用户数据缓冲区的物理地址 */
    AR_U8     *pu8UserDataVirAddr;         /**< 保存用户数据缓冲区的虚拟地址 */
    AR_U32     u32UserDataHeader;          /**< 报告用户数据的标志1<<flag,flag参考ENUM_H265_USERDATA_FLAG和ENUM_H264_USERDATA_FLAG */
    AR_U32     u32UserDataNum;             /**< 用户数据个数 */
    AR_U32     u32UserDataSize;            /**< 解码的用户数据占用的空间 */
    AR_U32     u32UserDataBufFull;         /**< 用户数据空间是否满 */
    AR_U32     u32UserDataOffset;          /**< 用户数据和unregisted之间的偏移 */
} STRU_VDEC_USERDATA;

/**
* @note 定义获取或者释放解码器解码输出用户数据相关的结构体
*/
typedef struct
{
    STRU_VDEC_USERDATA stUserData;         /**< 参考STRU_VDEC_USERDATA */
    AR_S32    s32TimeoutMs;                /**< -1:阻塞方式 非-1:等待时间 */
} STRU_VDEC_USERDATA_INFO;
/********************** HAL  MSG ******************************/
#define AR_COMMAND_VIDEO_CODEC_BASE              0x04000000
typedef enum {
    AR_VIDEO_CODEC_CMD_INVALID = AR_COMMAND_VIDEO_CODEC_BASE,
    AR_VENC_CMD_REQUEST_IDR,               /* 1 Request next frame as IDR */
    AR_VENC_CMD_ENABLE_IDR,                /* 2 Enable/Disable IDR frame in IDR interval */
    AR_VDEC_CMD_SET_FB_PARAM,              /* 3 */
    AR_VENC_CMD_SET_INTRAREFRSH,           /* 4 Set Intra refresh */
    AR_VENC_CMD_SET_GOP,                   /* 5 Set gop and idr interval */
    AR_VENC_CMD_ENABLE_ROI,                /* 6 Enable ROI */
    AR_VENC_CMD_SET_ROI,                   /* 7 Set ROI */
    AR_VENC_CMD_SET_ENTROPY,               /* 8 Set entropy type, H264 Only */
    AR_VENC_CMD_SET_DEBLOCK,               /* 9 Set deblock type */
    AR_VENC_CMD_SET_SAO,                   /* A Set sample adaptive offset   enable/disable flag */
    AR_VENC_CMD_SET_PARAM,                 /* B */
    AR_VENC_CMD_SEND_VIDEO_FRAME,          /* C */
    AR_VENC_CMD_SEND_VIDEO_FRAME_EXT,      /* D */
    AR_VENC_CMD_SET_STATIC_PARAM,          /* E */
    AR_VENC_CMD_SET_DYNAMIC_PARAM,         /* F */
    AR_VENC_CMD_SET_ACTIVE_PPS,            /* 10 */
    AR_VENC_CMD_SET_SLICE_SPLIT,           /* 11 */
    AR_VENC_CMD_SET_INTRAPRED,             /* 12 */
    AR_VENC_CMD_SET_SLICE_MODE,            /* 13 Set slice mode, H264 only */
    AR_VENC_CMD_SET_TRANS,                 /* 14 */
    AR_VENC_CMD_SET_QP_COEF,               /* 15 for jpeg only 0-31 */
    AR_VENC_CMD_SET_FPS,                   /* 16 */
    AR_VENC_CMD_SET_BITRATE,               /* 17 */
    AR_VENC_CMD_SET_IDR_INTERVAL,          /* 18 */
    AR_VENC_CMD_SET_ROTATION,              /* 19 */
    AR_VENC_CMD_SET_MIRROR,                /* 1A */
    AR_VENC_CMD_SET_VUI_INFO,              /* 1B */
    AR_VENC_CMD_SET_JPEG_PARAM,            /* 1C */
    AR_VENC_CMD_SET_MJPEG_PARAM,           /* 1D */
    AR_VENC_CMD_SET_REF_PARAM,             /* 1E */
    AR_VENC_CMD_SET_CHN_PARAM,             /* 1F */
    /* Dec */
    AR_VDEC_CMD_SET_ATTR,                  /* 20 */
    AR_VDEC_CMD_GET_ATTR,                  /* 21 */
    AR_VDEC_CMD_SET_PARAM,                 /* 22 */
    AR_VDEC_CMD_GET_PARAM,                 /* 23 */
    AR_VDEC_CMD_ATTACH_VB_POOL,            /* 24 */
    AR_VDEC_CMD_DETACH_VB_POOL,            /* 25 */
    AR_VDEC_CMD_SEND_STREAM_FRAME,         /* 26 */
    AR_VDEC_CMD_RELEASE_VIDEO_FRAME,       /* 27 */
    AR_VDEC_CMD_RELEASE_USERDATA_INFO,     /* 28 */
    AR_VCODEC_CMD_SET_MAX,                 /* 29 */

    /* Enc */
    AR_VCODEC_CMD_GET_INVALID = (AR_COMMAND_VIDEO_CODEC_BASE+0x8000),
    AR_VENC_CMD_GET_PARAM,                 /* 1 */
    AR_VENC_CMD_GET_INTRAREFRSH,           /* 2 Get Intra refresh */
    AR_VENC_CMD_GET_GOP,                   /* 3 Set gop and idr interval */
    AR_VENC_CMD_GET_ROI,                   /* 4 Get ROI */
    AR_VENC_CMD_GET_ENTROPY,               /* 5 Get entropy type, H264 Only */
    AR_VENC_CMD_GET_DEBLOCK,               /* 6 Get deblock type */
    AR_VENC_CMD_GET_SAO,                   /* 7 Get sample adaptive offset   enable/disable flag */
    AR_VENC_CMD_GET_SLICE_MODE,            /* 8 Get slice mode, H264 only */
    AR_VENC_CMD_GET_ACTIVE_PPS,            /* 9 */
    AR_VENC_CMD_GET_SLICE_SPLIT,           /* A */
    AR_VENC_CMD_GET_INTRAPRED,             /* B */
    AR_VENC_CMD_GET_STATIC_PARAM,          /* C */
    AR_VENC_CMD_GET_DYNAMIC_PARAM,         /* D */
    AR_VENC_CMD_GET_TRANS,                 /* E */
    AR_VENC_CMD_GET_QP_COEF,               /* F */
    AR_VENC_CMD_GET_FPS,                   /* 10 */
    AR_VENC_CMD_GET_FRAME_INFO,            /* 11 */
    AR_VENC_CMD_GET_IDR_INTERVAL,          /* 12 */
    AR_VENC_CMD_GET_ROTATION,              /* 13 */
    AR_VENC_CMD_GET_MIRROR,                /* 14 */
    AR_VENC_CMD_GET_VUI_INFO,              /* 15 */
    AR_VENC_CMD_GET_JPEG_PARAM,            /* 16 */
    AR_VENC_CMD_GET_MJPEG_PARAM,           /* 17 */
    AR_VENC_CMD_QUERY_STATUS,              /* 18 */
    AR_VENC_CMD_GET_STREAM,                /* 19 */
    AR_VENC_CMD_RELEASE_STREAM,            /* 1A */
    AR_VENC_CMD_GET_REF_PARAM,             /* 1B */
    AR_VENC_CMD_GET_CHN_PARAM,             /* 1C */
    AR_VENC_CMD_SHOW_DEBUG_INFO,           /* 1D */
    /* Dec */
    AR_VDEC_CMD_GET_VIDEO_FRAME,           /* 1E */
    AR_VDEC_CMD_QUERY_STATUS,              /* 1F */
    AR_VDEC_CMD_GET_USERDATA_INFO,         /* 20 */
    /* Common */
    AR_VENC_CMD_INSERT_USERDATA,           /* 21 */
    AR_VENC_CMD_GET_USERDATA_BUF,          /* 22 */
    AR_VENC_CMD_GET_STREAMBUF_INFO,        /* 23 */

    AR_VIDEO_CODEC_SERVICE_INVALID = (AR_COMMAND_VIDEO_CODEC_BASE+0xF000),
    AR_VIDEO_CODEC_CREATE_DEV,          // 1
    AR_VIDEO_CODEC_DESTORY_DEV,         // 2
    AR_VIDEO_CODEC_SET_MOD_PARAM,       // 3
    AR_VIDEO_CODEC_GET_MOD_PARAM,       // 4
    AR_VIDEO_CODEC_GET_CTRL_INFO,       // 5
    AR_VIDEO_CODEC_CREATE_INSTANCE,     // 6
    AR_VIDEO_CODEC_DESTORY_INSTANCE,    // 7
    AR_VIDEO_CODEC_RESET_INSTANCE,      // 8
    AR_VIDEO_CODEC_START,               // 9
    AR_VIDEO_CODEC_STOP,                // A
    AR_VIDEO_CODEC_ATTACH_VB_POOL,
    AR_VIDEO_CODEC_DETACH_VB_POOL,
    AR_VIDEO_CODEC_EXIT,
    AR_VIDEO_CODEC_SUSPEND,

    AR_VIDEO_CODEC_CMD_MAX
} ENUM_VCODEC_CTRL_CMD;
/**
* @note 定义编码器控制设备参数相关的结构体
*/
typedef struct
{
    AR_S32  dev_id;
}STRU_VENC_CTRL_PARAM;
/**
* @note 定义解码器控制设备参数相关的结构体
*/
typedef struct
{
    AR_S32  dev_id;
}STRU_VDEC_CTRL_PARAM;

#ifdef __cplusplus
#if __cplusplus
  }
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _HAL_VCODEC_COMM_TYPE_H_
/** @}*/
