/**
 * \file
 * \brief 描述rc相关的通用数据结构
 */

#ifndef __AR_COMM_RC_H__
#define __AR_COMM_RC_H__

#include "ar_defines.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
/********************************Macro Definition********************************/

typedef AR_U32 AR_FR32;

/**<@note 参数有更新; 定义编码通道码率控制器模式。*/
typedef enum arVENC_RC_MODE_E
{
    VENC_RC_MODE_H264CBR = 1,       /**< H.264 CBR模式。*/
    VENC_RC_MODE_H264VBR,           /**< H.264 VBR模式。*/
    VENC_RC_MODE_H264AVBR,          /**<@note 暂不支持; H.264 AVBR模式。*/
    VENC_RC_MODE_H264QVBR,          /**<@note 暂不支持; H.264 QVBR模式。*/
    VENC_RC_MODE_H264FIXQP,         /**< H.264 FixQp模式。*/
    VENC_RC_MODE_H264QPMAP,         /**< H.264 QPMAP模式。*/

    VENC_RC_MODE_MJPEGCBR,          /**< MJPEG CBR模式。*/
    VENC_RC_MODE_MJPEGVBR,          /**< MJPEG VBR模式。*/
    VENC_RC_MODE_MJPEGFIXQP,        /**<@note 暂不支持; MJPEG FixQp模式。*/

    VENC_RC_MODE_H265CBR,           /**< H.265 CBR模式。*/
    VENC_RC_MODE_H265VBR,           /**< H.265 VBR模式。*/
    VENC_RC_MODE_H265AVBR,          /**<@note 暂不支持; H.265 AVBR模式。*/
    VENC_RC_MODE_H265QVBR,          /**<@note 暂不支持; H.265 QVBR模式。*/
    VENC_RC_MODE_H265FIXQP,         /**< H.265 FixQp模式。*/
    VENC_RC_MODE_H265QPMAP,         /**< H.265 QPMAP模式。*/

    VENC_RC_MODE_BUTT,

}VENC_RC_MODE_E;

/**< 定义编码通道QPMAP模式下CU32、CU64QP值的取值方式。*/
typedef enum arVENC_RC_QPMAP_MODE_E
{
    VENC_RC_QPMAP_MODE_MEANQP= 0,       /**< CU32 QP值取4个16*16块Qp值的平均值; CU64 QP值取16个16*16块Qp值的平均值。*/
    VENC_RC_QPMAP_MODE_MINQP ,          /**< CU32 QP值取4个16*16块Qp值的最小值; CU64 QP值取16个16*16块Qp值的最小值。*/
    VENC_RC_QPMAP_MODE_MAXQP,           /**< CU32 QP值取4个16*16块Qp值的最大值; CU64 QP值取16个16*16块Qp值的最大值。*/

    VENC_RC_QPMAP_MODE_BUTT,
}VENC_RC_QPMAP_MODE_E;

/**< 定义H.264编码通道Fixqp属性结构。*/
typedef struct arVENC_H264_FIXQP_S
{
    AR_U32      u32Gop;                                 /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32SrcFrameRate;                        /**< 可读写; 取值范围:[1, 240]; VI输入帧率, 以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;                       /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]，分数帧率时坟墓不大于64; 编码器输出帧率, 以fps为单位; 动态属性 */
    AR_U32      u32IQp;                                 /**< 可读写; 取值范围:[0, 51]; I帧所有宏块Qp值。*/
    AR_U32      u32PQp;                                 /**< 可读写; 取值范围:[0, 51]; P帧所有宏块Qp值。*/
    AR_U32      u32BQp;                                 /**< 可读写; 取值范围:[0, 51]; B帧所有宏块Qp值。*/
} VENC_H264_FIXQP_S;

/**< 定义H.264编码通道CBR属性结构。*/
typedef struct arVENC_H264_CBR_S
{
    AR_U32      u32Gop;                                 /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32StatTime;                            /**< 可读写; 取值范围:[1, 60]; CBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;                        /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;                       /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32BitRate;                             /**< 可读写; 取值范围:H265[128, 160000],H264[128, 240000]kbps; 平均帧率，以fps为单位; 动态属性 */
} VENC_H264_CBR_S;

/**< 定义H.264编码通道VBR属性结构。*/
typedef struct arVENC_H264_VBR_S
{
    AR_U32      u32Gop;                     /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32StatTime;                /**< 可读写; 取值范围:[1, 60]; VBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;            /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;           /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32MaxBitRate;              /**< 可读写; 取值范围:H265[128, 160000],H264[128, 240000]kbps; 编码器输出最大码率，以kbps为单位; 动态属性 */
    AR_U32      u32MinBitRate;              /**< 可读写; 取值范围:H265[128, 160000],H264[128, 240000]kbps; 编码器输出最小码率，以kbps为单位; 动态属性 */
}VENC_H264_VBR_S;

/**< 定义H.264编码通道AVBR属性结构。*/
typedef struct arVENC_H264_AVBR_S
{
    AR_U32      u32Gop;                  /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32StatTime;             /**< 可读写; 取值范围:[1, 60]; AVBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;         /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;        /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32MaxBitRate;           /**< 可读写; 取值范围:H265[128, 160000],H264[128, 240000]kbps; 编码器输出最大码率，以kbps为单位; 动态属性 */
} VENC_H264_AVBR_S;

/**< 定义H.264编码通道QPMAP属性结构。*/
typedef struct arVENC_H264_QPMAP_S
{
    AR_U32      u32Gop;                   /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32StatTime;              /**< 可读写; 取值范围:[1, 60]; QMAP码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate ;        /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
}VENC_H264_QPMAP_S;

/**< 定义H.264编码通道QVBR属性结构。*/
typedef struct arVENC_H264_QVBR_S
{
    AR_U32      u32Gop;                   /**< 可读写; 取值范围:[1, 65536]; H.264 gop值; 动态属性 */
    AR_U32      u32StatTime;              /**< 可读写; 取值范围:[1, 60]; QVBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate ;        /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32TargetBitRate;         /**< 可读写; 编码器输出最大码率，以kbps为单位 */
}VENC_H264_QVBR_S;

/**< 定义H.265编码通道QPMAP属性结构。*/
typedef struct arVENC_H265_QPMAP_S
{
    AR_U32      u32Gop;                   /**< 可读写; 取值范围:[1, 65536]; H.265 gop值; 动态属性 */
    AR_U32      u32StatTime;              /**< 可读写; 取值范围:[1, 60]; QMAP码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate ;        /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    VENC_RC_QPMAP_MODE_E enQpMapMode;     /**< 可读写; CU32或CU64的QP值的取值方式。*/
}VENC_H265_QPMAP_S;

/**< 定义H.265编码通道CBR属性结构。*/
typedef struct arVENC_H264_CBR_S   VENC_H265_CBR_S;
/**< 定义H.265编码通道VBR属性结构。*/
typedef struct arVENC_H264_VBR_S   VENC_H265_VBR_S;
/**< 定义H.265编码通道AVBR属性结构。*/
typedef struct arVENC_H264_AVBR_S  VENC_H265_AVBR_S;
/**< 定义H.265编码通道FixQp属性结构。*/
typedef struct arVENC_H264_FIXQP_S VENC_H265_FIXQP_S;
/**< 定义H.265编码通道QVBR属性结构。*/
typedef struct arVENC_H264_QVBR_S  VENC_H265_QVBR_S;

/**< 定义MJPEG编码通道Fixqp属性结构。*/
typedef struct arVENC_MJPEG_FIXQP_S
{
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; VI输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;         /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32Qfactor;               /**< 可读写; 取值范围:[1,99]; MJPEG编码的编码质量*/
}VENC_MJPEG_FIXQP_S;

/**< 定义MJPEG编码通道CBR属性结构。*/
typedef struct arVENC_MJPEG_CBR_S
{
    AR_U32      u32StatTime;              /**< 可读写; 取值范围:[1, 60]; CBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; 编码器输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate ;        /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32BitRate;               /**< 可读写; 取值范围:[2, 409600]; 平均bitrate，以kbps为单位; 动态属性 */
} VENC_MJPEG_CBR_S;

/**< 定义MJPEG编码通道VBR属性结构。*/
typedef struct arVENC_MJPEG_VBR_S
{
    AR_U32      u32StatTime;              /**< 可读写; 取值范围:[1, 60]; VBR码率统计时间，以秒为单位; 动态属性 */
    AR_U32      u32SrcFrameRate;          /**< 可读写; 取值范围:[1, 240]; 编码器输入帧率，以fps为单位; 动态属性 */
    AR_FR32     fr32DstFrameRate;         /**< 可读写; 取值范围:[0.015625, u32SrcFrmRate]; 编码器输出帧率，以fps为单位; 动态属性 */
    AR_U32      u32MaxBitRate;            /**< 可读写; 取值范围:[2, 409600]; 最大bitrate，以kbps为单位; 动态属性 */
}VENC_MJPEG_VBR_S;

/**< 定义编码通道码率控制器属性。*/
typedef struct arVENC_RC_ATTR_S
{
    VENC_RC_MODE_E enRcMode;                    /**< 可读写; RC模式。*/
    union
    {
        VENC_H264_CBR_S    stH264Cbr;           /**< H.264 CBR模式。*/
        VENC_H264_VBR_S    stH264Vbr;           /**< H.264 VBR模式。*/
        VENC_H264_FIXQP_S  stH264FixQp;         /**<@note 暂不支持; 可读写; H.264 FixQp模式。*/
        VENC_H264_QPMAP_S  stH264QpMap;         /**< H.264 QPMAP模式。*/

        VENC_MJPEG_CBR_S   stMjpegCbr;          /**< MJPEG CBR模式。*/
        VENC_MJPEG_VBR_S   stMjpegVbr;          /**< MJPEG VBR模式。*/
        VENC_MJPEG_FIXQP_S stMjpegFixQp;        /**< MJPEG FixQp模式。*/

        VENC_H265_CBR_S    stH265Cbr;           /**< H.265 CBR模式。*/
        VENC_H265_VBR_S    stH265Vbr;           /**< H.265 VBR模式。*/
        VENC_H265_FIXQP_S  stH265FixQp;         /**<@note 暂不支持; 可读写; H.265 FixQp模式。*/
        VENC_H265_QPMAP_S  stH265QpMap;         /**< H.265 QPMAP模式。*/
    };
}VENC_RC_ATTR_S;

/**< 定义码率控制中超大帧处理模式。*/
typedef enum arRC_SUPERFRM_MODE_E
{
    SUPERFRM_NONE=0,                                    /**< 无特殊策略。*/
    SUPERFRM_DISCARD,                                   /**< 丢弃超大帧。*/
    SUPERFRM_REENCODE,                                  /**< 重编差大帧。*/
    SUPERFRM_BUTT
}VENC_SUPERFRM_MODE_E;

/**<@note 参数有更新; 定义H264协议编码通道CBR码率控制模式高级参数配置*/
typedef struct arVENC_PARAM_H264_CBR_S
{
    AR_U32  u32MinIprop;                /**<@note 暂不支持; 可读写; 取值范围: [1, 100]; 最小IP帧比例。*/
    AR_U32  u32MaxIprop;                /**<@note 暂不支持; 可读写; 取值范围: (MinIprop, 100]; 最大IP帧比列。*/
    AR_BOOL bQpMapEn;                   /**< 可读写; 取值范围:[0, 1]; CBR 码控模式下，使能QpMap功能。*/
    AR_S32  s32MaxReEncodeTimes;        /**<@note 暂不支持; 可读写; 取值范围: [0, 3]; 取值范围: 每帧重编测试。*/

    AR_U32  u32MaxBQp;                  /**<@note 新加参数; 可读写; 取值范围:(MinQp, 51]; 最大B帧码率的比值; 动态属性 */
    AR_U32  u32MinBQp;                  /**<@note 新加参数;  可读写; 取值范围:[0, 51]; 最小B帧码率的比值; 动态属性 */
    AR_U32  u32MaxPQp;                  /**<@note 新加参数;  可读写; 取值范围:(MinQp, 51]; 最大P帧码率的比值; 动态属性 */
    AR_U32  u32MinPQp;                  /**<@note 新加参数;  可读写; 取值范围:[0, 51]; 最小P帧码率的比值; 动态属性 */
    AR_U32  u32MaxIQp;                  /**< 可读写; 取值范围:(MinQp, 51]; I帧的最大QP，用于控制I帧的最大bits数; 动态属性 */
    AR_U32  u32MinIQp;                  /**< 可读写; 取值范围:[0, 51]; I帧的最小QP，用于控制I帧的最小bits数; 动态属性 */
} VENC_PARAM_H264_CBR_S;

/**<@note 参数有更新; 定义H264协议编码通道VBR码率控制模式高级参数配置。*/
typedef struct arVENC_PARAM_H264_VBR_S
{
    AR_BOOL bQpMapEn;                   /**< 可读写; 取值范围:[0, 1]; VBR模式下是否使能QpMap功能。*/
    AR_U32  u32MinIprop;                /**< 可读写; 取值范围:[1, 100]; 最小IP帧码率的比值。*/
    AR_U32  u32MaxIprop;                /**< 可读写; 取值范围:(MinIprop, 100]; 最小IP帧码率的比值。*/
    AR_S32  s32MaxReEncodeTimes;        /**<@note 暂不支持; 可读写; 取值范围:[0, 3]; 每帧重编码次数。*/

    AR_U32  u32MaxBQp;                  /* 可读写; 取值范围:(MinQp, 51]; 最大B帧码率的比值; 动态属性 */
    AR_U32  u32MinBQp;                  /* 可读写; 取值范围:[0, 51]; 最小B帧码率的比值; 动态属性 */
    AR_U32  u32MaxPQp;                  /* 可读写; 取值范围:(MinQp, 51]; 最大P帧码率的比值; 动态属性 */
    AR_U32  u32MinPQp;                  /* 可读写; 取值范围:[0, 51]; 最小P帧码率的比值; 动态属性 */
    AR_U32  u32MaxIQp;                  /* 可读写; 取值范围:(MinIQp, 51]; 最大I帧码率的比值; 动态属性 */
    AR_U32  u32MinIQp;                  /* 可读写; 取值范围:[0, 51]; 最小I帧码率的比值; 动态属性 */
} VENC_PARAM_H264_VBR_S;

/**< 定义MJPEG协议编码通道CBR码率控制模式高级参数配置。*/
typedef struct arVENC_PARAM_MJPEG_CBR_S
{
    AR_U32  u32MaxQfactor;              /* 可读写; 取值范围:[MinQfactor, 99]; 帧最大编码质量，用于钳位质量，值越大质量越好。*/
    AR_U32  u32MinQfactor;              /* 可读写; 取值范围:[1, 99]; 最小质量因子，值越小质量越差，压缩率越高。*/
} VENC_PARAM_MJPEG_CBR_S;

/**< 定义MJPEG协议编码通道VBR码率控制模式高级参数配置。*/
typedef struct arVENC_PARAM_MJPEG_VBR_S
{
    AR_S32 s32ChangePos;                    /**<@note 暂不支持; 可读写; 取值范围:[50, 100]; VBR开始调整编码质量时的码率相对于最大码率的比例。*/
    AR_U32 u32MaxQfactor;                   /**< 可读写; 取值范围:[MinQfactor, 99]; 最大质量因子，值越大质量越好。*/
    AR_U32 u32MinQfactor;                   /**< 可读写; 取值范围:[1, 99]; 最小质量因子，值越小质量越差，压缩率越高。*/
} VENC_PARAM_MJPEG_VBR_S;

/**< 定义H265协议编码通道CBR新版码率控制模式高级参数配置。*/
typedef struct arVENC_PARAM_H265_CBR_S
{
    AR_U32  u32MinIprop;                    /**< 可读写; 取值范围: [u32MinIprop, 100]; 最小IP帧比例。*/
    AR_U32  u32MaxIprop;                    /**< 可读写; 取值范围: [1, 100]; 最大IP帧比例。*/
    AR_S32  s32MaxReEncodeTimes;            /**<@note 暂不支持; 可读写; 取值范围:[0, 3]; 每帧重编码次数。*/
    AR_U32  u32MaxBQp;                      /**<@note 新加参数;  可读写; 取值范围:(MinQp, 51]; 最大B帧码率的比值; 动态属性 */
    AR_U32  u32MinBQp;                      /**<@note 新加参数;  可读写; 取值范围:[0, 51]; 最小B帧码率的比值; 动态属性 */
    AR_U32  u32MaxPQp;                      /**<@note 新加参数;  可读写; 取值范围:(MinQp, 51]; 最大P帧码率的比值; 动态属性 */
    AR_U32  u32MinPQp;                      /**<@note 新加参数;  可读写; 取值范围:[0, 51]; 最小P帧码率的比值; 动态属性 */
    AR_U32  u32MaxIQp;                      /**< 可读写; 取值范围:(MinIQp, 51]; I帧的最大QP; 动态属性。*/
    AR_U32  u32MinIQp;                      /**< 可读写; 取值范围:[1, 51]; I帧的最小QP; 动态属性。*/
    AR_BOOL bQpMapEn;                       /**< 可读写; 取值范围:[0, 1]; CBR码控模式下，使能/关闭 QpMap 功能。*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;       /**< @note 暂不支持; 可读写; 使能 QpMap 表的情况，CU32和CU64的Qp值的取值方式 */
} VENC_PARAM_H265_CBR_S;

/**<@note 参数有更新; 定义H265协议编码通道VBR码率控制模式高级参数配置。*/
typedef struct arVENC_PARAM_H265_VBR_S
{
    AR_U32  u32MinIprop;                /**< 可读写; 取值范围:[1, 100]; 最小IP帧码率比值。*/
    AR_U32  u32MaxIprop;                /**< 可读写; 取值范围:[MinIprop, 100]; 最大IP帧码率比值。*/
    AR_S32  s32MaxReEncodeTimes;        /**<@note 暂不支持; 可读写; 取值范围:[0, 3]; 每帧重编码次数。*/

    AR_U32  u32MaxBQp;                  /**< 可读写; 取值范围:(MinQp, 51]; 最大B帧QP; 动态属性 */
    AR_U32  u32MinBQp;                  /**< 可读写; 取值范围:[0, 51]; 最小B帧QP; 动态属性 */
    AR_U32  u32MaxPQp;                  /**< 可读写; 取值范围:(MinQp, 51]; 最大P帧QP; 动态属性 */
    AR_U32  u32MinPQp;                  /**< 可读写; 取值范围:[0, 51]; 最小P帧QP; 动态属性 */
    AR_U32  u32MaxIQp;                  /**< 可读写; 取值范围:(MinIQp, 51]; 最大I帧QP; 动态属性*/
    AR_U32  u32MinIQp;                  /**< 可读写; 取值范围:[0, 51]; 最小I帧QP; 动态属性 */
    AR_BOOL bQpMapEn;                   /**< 可读写; 取值范围:[0, 1]; VBR模式下是否使能QpMap功能。*/
    VENC_RC_QPMAP_MODE_E enQpMapMode;   /**< @note 暂不支持; 可读写; QPMAP模式下CU32、CU64 QP值的取值方式。*/
} VENC_PARAM_H265_VBR_S;

/**<@note 参数有更新; 定义编码通道的码率控制高级参数。*/
typedef struct arVENC_RC_PARAM_S
{
    AR_S32 s32CuOrMbLevelRcEnable;                          /**<@note 新添加参数: 是否使能CU(H.265)或MB(H.264)级的码控; 动态属性 */
    AR_S32 s32HvsQPEnable;                                  /**<@note 新添加参数: 是否使能CU级QP调整; 动态属性 */
    AR_S32 s32HvsQpScale;                                   /**<@note 新添加参数: CU级QP调整的系数; 取值范围:[0, 15]; 动态属性 */
    AR_S32 s32HvsMaxDeltaQp;                                /**<@note 新添加参数: HVS的最大的QP变化; 取值范围:[0, 51]; 动态属性 */
    AR_S32 s32FirstFrameStartQp;                            /**<@note 有更新; 可读写; 取值范围:[-1, 51]; MJPEG的话范围是[1, 99]; 静态属性 */
    union
    {
        VENC_PARAM_H264_CBR_S     stParamH264Cbr;           /**< H.264通道CBR（Constant Bit Rate）码率控制模式高级参数。*/
        VENC_PARAM_H264_VBR_S     stParamH264Vbr;           /**< H.264通道VBR（Variable Bit Rate）码率控制模式高级参数。*/
        VENC_H264_FIXQP_S         stParamH264Fixqp;         /**< H.264通道FixQp码率控制模式高级参数。*/
        VENC_PARAM_H265_CBR_S     stParamH265Cbr;           /**< H.265通道CBR（Constant Bit Rate）码率控制模式高级参数。*/
        VENC_PARAM_H265_VBR_S     stParamH265Vbr;           /**< H.265通道VBR（Variable Bit Rate）码率控制模式高级参数。*/
        VENC_H265_FIXQP_S         stParamH265Fixqp;         /**< H.265通道FixQp码率控制模式高级参数。*/
        VENC_PARAM_MJPEG_CBR_S    stParamMjpegCbr;          /**< MJPEG通道CBR（Constant Bit Rate）码率控制模式高级参数。*/
        VENC_PARAM_MJPEG_VBR_S    stParamMjpegVbr;          /**< MJPEG通道VBR（Variable Bit Rate）码率控制模式高级参数。*/
    };
} VENC_RC_PARAM_S;

/**< 瞬时码率超过阈值时的丢帧模式。*/
typedef enum arVENC_FRAMELOST_MODE_E
{
    FRMLOST_NORMAL=0,                               /**< 瞬时码率超过阈值时正常丢帧。*/
    FRMLOST_PSKIP,                                  /**< 瞬时码率超过阈值时编码pskip帧。*/
    FRMLOST_BUTT,
} VENC_FRAMELOST_MODE_E;

/**< 瞬时码率超过阈值时的丢帧策略参数。*/
typedef struct arVENC_FRAMELOST_S
{
    AR_BOOL                bFrmLostOpen;          /**< 可读写; 取值范围:[0,1]; 瞬时码率超过阈值时丢帧开关。*/
    AR_U32                 u32FrmLostBpsThr;      /**< 可读写; 取值范围:[64k, 163840k]; 丢帧阈值（单位为 bit/s）。*/
    VENC_FRAMELOST_MODE_E  enFrmLostMode;         /**< 瞬时码率超过阈值时丢帧模式。*/
    AR_U32                 u32EncFrmGaps;         /**< 可读写; 取值范围:[0,65535]; 最大允许连续丢帧帧数。*/
} VENC_FRAMELOST_S;

/**< 超大帧重编优先级枚举。*/
typedef enum arVENC_RC_PRIORITY_E
{
    VENC_RC_PRIORITY_BITRATE_FIRST = 1,         /**< 目标码率优先。*/
    VENC_RC_PRIORITY_FRAMEBITS_FIRST,           /**< 超大帧阈值优先。*/

    VENC_RC_PRIORITY_BUTT,
} VENC_RC_PRIORITY_E;

/**< 超大帧处理策略参数。*/
typedef struct arVENC_SUPERFRAME_CFG_S
{
    VENC_SUPERFRM_MODE_E  enSuperFrmMode;               /**< 可读写; 超大帧处理模式，默认为SUPERFRM_NONE。*/
    AR_U32                u32SuperIFrmBitsThr;          /**< 可读写; 取值范围:[0, 4294967295]; I帧超大阈值。*/
    AR_U32                u32SuperPFrmBitsThr;          /**< 可读写; 取值范围:[0, 4294967295]; P帧超大阈值。*/
    AR_U32                u32SuperBFrmBitsThr;          /**< 可读写; 取值范围:[0, 4294967295]; B帧超大阈值。*/
    VENC_RC_PRIORITY_E    enRcPriority;                 /**<@note 暂不支持; 可读写; P帧超大阈值。*/
} VENC_SUPERFRAME_CFG_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_COMM_RC_H__ */
