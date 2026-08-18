/**
 * \file
 * \brief 描述视频输入相关的通用数据结构
 */

#ifndef __AR_COMM_VI_H__
#define __AR_COMM_VI_H__

#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"
#include "ar_comm_gdc.h"
#include "ar_comm_isp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */
#endif /* __cplusplus */

/********************************Macro Definition********************************/
/** \addtogroup      MPI_VI */
/** @{ */  /** <!-- [MPI_VI] */

#define VI_MAX_ADCHN_NUM            (4UL)       /**<VI设备属性中s32AdChnId的个数*/

#define VI_PMFCOEF_NUM              (9UL)       /**<@note 新添加参数。*/
#define VI_COMPMASK_NUM             (2UL)       /**<VI设备属性中Component mask的个数*/
#define VI_PRO_MAX_FRAME_NUM        (8UL)       /**<@note 新添加参数。*/
#define VI_SHARPEN_GAIN_NUM         32          /**<@note 新添加参数。*/
#define VI_AUTO_ISO_STRENGTH_NUM    16          /**<@note 新添加参数。*/

#define VI_INVALID_FRMRATE  (-1)                /**<VI帧率控制的宏，当原始帧率和目标帧率都为该宏时，不做帧率控制*/
#define VI_CHN0               0                 /**<@note 新添加参数。*/
#define VI_CHN1               1                 /**<@note 新添加参数。*/
#define VI_CHN2               2                 /**<@note 新添加参数。*/
#define VI_CHN3               3                 /**<@note 新添加参数。*/
#define VI_INVALID_CHN       -1                 /**<@note 新添加参数。*/

#define VI_MAX_VC_NUM         4                 /**<@note 新添加参数。*/

/** @} */  /** <!-- ==== Macro Definition end ==== */

/*************************** Structure Definition ****************************/
/** \addtogroup      MPI_VI */
/** @{ */  /** <!-- [MPI_VI] */

/**定义VI的工作模式*/
typedef enum
{
 VIN_CAMERA_NORMAL,                 /**<@note 新添加参数。*/
 VIN_CAMERA_OFFLINE,                /**<@note 新添加参数。*/
 VIN_CMAERA_MULTI_MODE,             /**<@note 新添加参数。*/
 VIN_CMAERA_TOOL_SIMULATION,             /**<@note 新添加参数。sensor 工作在可以由工具发送raw 图仿真*/
}ENUM_VIN_WORK_MODE_T;

/**VI的公共属性，包括VI的工作模式，各个模块的工作频率*/
typedef struct
{
	ENUM_VIN_WORK_MODE_T cam_mode;                /**<@note 新添加参数。VI的工作模式。*/
	AR_S32 vif_fre_mod;             /**<@note 新添加参数。vif时钟频率模式。0：自动模式；1：频率由vif_fre_hz指定，范围请参考soc 手册。*/
	AR_S32 vif_fre_hz;              /**<@note 新添加参数。vif时钟频率，单位为HZ。*/	
	AR_S32 mipi_fre_mod;             /**<@note 新添加参数。mipi时钟频率模式。0：自动模式；1：频率由vif_fre_hz指定，范围请参考soc 手册。*/
	AR_S32 mipi_fre_hz;              /**<@note 新添加参数。pcs时钟频率，单位为HZ。9341 只有200M和333M，不是这两个值，默认为333M*/
	AR_S32 isp_fre_mod;             /**<@note 新添加参数。isp时钟频率模式。0：自动模式；1：频率由isp_fre_hz指定，范围请参考soc 手册。*/
	AR_S32 isp_fre_hz;              /**<@note 新添加参数。isp时钟频率，单位为HZ。*/
	AR_S32 hdr_fre_mod;             /**<@note 新添加参数。hdr时钟频率模式。0：自动模式；1：频率由hdr_fre_hz指定，范围请参考soc 手册。*/
	AR_S32 hdr_fre_hz;              /**<@note 新添加参数。hdr时钟频率，单位为HZ。*/
	AR_S32 eis_fre_mod;             /**<@note 新添加参数。eis时钟频率模式。0：自动模式；1：频率由eis_fre_hz指定，范围请参考soc 手册。*/
	AR_S32 eis_fre_hz;              /**<@note 新添加参数。eis时钟频率，单位为HZ。*/
	ENUM_VIN_HW_BURST_T hw_burst;   /**<@note 新添加参数。主要指vif的hw burst */
	ENUM_VIN_HW_BURST_T isp_burst; /**<@note 新添加参数。主要指isp的hw burst */
}VI_DEV_PROP_S;

/**定义VI通道低延时属性*/
typedef struct arVI_LOW_DELAY_INFO_S
{
    AR_BOOL bEnable;                /**<低延时使能开关。AR_FALSE：不使能；AR_TRUE：使能。*/
    AR_U32 u32LineCnt;              /**<@note 不使用。*/
}VI_LOW_DELAY_INFO_S;

/**定义VI PIPE的RAW数据压缩参数*/
typedef struct arVI_CMP_PARAM_S
{
	AR_U8  bEnable;                             /**<压缩使能。*/
    AR_U8  au8CmpParam[VI_CMP_PARAM_SIZE];      /**<@note 转义参数 后续实现可能会重定义。*/
} VI_CMP_PARAM_S;

/**用户图片类型*/
typedef enum ar_VI_USERPIC_MODE_E
{
    VI_USERPIC_MODE_PIC = 0,        /**<YUV图像模式。*/
    VI_USERPIC_MODE_BGC,            /**<纯色背景图像模式。*/
    VI_USERPIC_MODE_BUTT,
} VI_USERPIC_MODE_E;

/**纯色背景模式下的用户图片相关信息*/
typedef struct arVI_USERPIC_BGC_S
{
    AR_U32          u32BgColor;     /**<填充数据，与颜色的RGB值对应。取值范围：[0, 0xFFFFFF]。*/
} VI_USERPIC_BGC_S;

/**用户图片信息*/
typedef struct arVI_USERPIC_ATTR_S
{
    VI_USERPIC_MODE_E       enUsrPicMode;       /**<用户图片模式。*/
    union
    {
        VIDEO_FRAME_INFO_S  stUsrPicFrm;        /**<YUV图像模式下的用户图片信息。*/
        VI_USERPIC_BGC_S    stUsrPicBg;         /**<纯色背景模式下的用户图片信息。*/
    } unUsrPic;
} VI_USERPIC_ATTR_S;

typedef enum arEN_VI_ERR_CODE_E
{
    ERR_VI_FAILED_NOTENABLE = 64,   /**<@note 新添加参数。device or channel not enable*/
    ERR_VI_FAILED_NOTDISABLE,       /**<@note 新添加参数。device not disable*/
    ERR_VI_FAILED_CHNOTDISABLE,     /**<@note 新添加参数。channel not disable*/
    ERR_VI_CFG_TIMEOUT,             /**<@note 新添加参数。config timeout*/
    ERR_VI_NORM_UNMATCH,            /**<@note 新添加参数。video norm of ADC and VIU is unmatch*/
    ERR_VI_INVALID_WAYID,           /**<@note 新添加参数。invlalid way ID*/
    ERR_VI_INVALID_PHYCHNID,        /**<@note 新添加参数。invalid phychn id*/
    ERR_VI_FAILED_NOTBIND,          /**<@note 新添加参数。device or channel not bind */
    ERR_VI_FAILED_BINDED,           /**<@note 新添加参数。device or channel not unbind */
    ERR_VI_DIS_PROCESS_FAIL         /**<@note 新添加参数。dis process failed */
} EN_VI_ERR_CODE_E;

#define AR_ERR_VI_INVALID_PARA          AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)        /**<错误码：视频输入参数设置无效*/
#define AR_ERR_VI_INVALID_DEVID         AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)        /**<错误码：视频输入设备号无效*/
#define AR_ERR_VI_INVALID_PIPEID        AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_PIPEID)       /**<错误码：PIPE号无效*/
#define AR_ERR_VI_INVALID_STITCHGRPID   AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_STITCHGRPID)  /**<错误码：拼接组无效*/
#define AR_ERR_VI_INVALID_CHNID         AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)        /**<错误码：视频输入通道号无效*/
#define AR_ERR_VI_INVALID_NULL_PTR      AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)             /**<错误码：输入参数空指针错误*/
#define AR_ERR_VI_FAILED_NOTCONFIG      AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_CONFIG)           /**<错误码：视频设备或通道属性未配置*/
#define AR_ERR_VI_SYS_NOTREADY          AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)         /**<错误码：视频输入系统未初始化*/
#define AR_ERR_VI_BUF_EMPTY             AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)            /**<错误码：视频输入缓存为空*/
#define AR_ERR_VI_BUF_FULL              AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_FULL)             /**<错误码：视频输入缓存为满*/
#define AR_ERR_VI_NOMEM                 AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)                /**<错误码：分配内存失败*/
#define AR_ERR_VI_NOT_SUPPORT           AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)          /**<错误码：操作不支持*/
#define AR_ERR_VI_BUSY                  AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)                 /**<错误码：视频输入系统忙*/
#define AR_ERR_VI_NOT_PERM              AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)             /**<错误码：操作不允许*/

#define AR_ERR_VI_FAILED_NOTENABLE      AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTENABLE)      /**<错误码：视频输入设备未启用*/
#define AR_ERR_VI_FAILED_NOTDISABLE     AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTDISABLE)     /**<错误码：视频输入设备未禁用*/
#define AR_ERR_VI_FAILED_CHNOTDISABLE   AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_FAILED_CHNOTDISABLE)   /**<错误码：视频输入通道未禁用*/
#define AR_ERR_VI_CFG_TIMEOUT           AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_CFG_TIMEOUT)           /**<错误码：视频配置属性超时*/
#define AR_ERR_VI_NORM_UNMATCH          AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_NORM_UNMATCH)          /**<错误码：不匹配*/
#define AR_ERR_VI_INVALID_WAYID         AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_INVALID_WAYID)         /**<错误码：视频通路号无效*/
#define AR_ERR_VI_INVALID_PHYCHNID      AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_INVALID_PHYCHNID)      /**<@note 新添加参数。错误码：物理通道号无效*/
#define AR_ERR_VI_FAILED_NOTBIND        AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_FAILED_NOTBIND)        /**<错误码：视频通道未绑定*/
#define AR_ERR_VI_FAILED_BINDED         AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, ERR_VI_FAILED_BINDED)         /**<错误码：视频通道已绑定*/

#define AR_ERR_VI_PIPE_EXIST            AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)                /**<错误码：PIPE已存在*/
#define AR_ERR_VI_PIPE_UNEXIST          AR_MPP_DEF_ERR(AR_ID_VI, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)              /**<错误码：PIPE不存在*/


/**定义视频设备的接口模式*/
typedef enum arVI_INTF_MODE_E
{
    VI_MODE_BT656 = 0,              /**<输入数据的协议符合标准BT.656协议，端口数据输入模式为亮度色度分离模式，分量模式为单分量。*/
    VI_MODE_BT601,                  /**<输入数据的协议符合标准BT.601协议，端口数据输入模式为亮度色度复合模式，分量模式为单分量。*/
    VI_MODE_DIGITAL_CAMERA,         /**<输入数据的协议为Digital camera协议，端口数据输入模式为亮度色度复合模式，分量模式为单分量。*/
    VI_MODE_BT1120_STANDARD,        /**<输入数据的协议符合标准BT.1120协议（BT656+双分量），端口数据输入模式为亮度色度分离模式，分量模式为双分量。*/
    VI_MODE_BT1120_INTERLEAVED,     /**<输入数据的协议符合BT.1120 interleave模式，端口数据输入模式为亮度色度分离模式，分量模式为双分量。*/
    VI_MODE_MIPI,                   /**<输入数据符合MIPI协议，用于传输RAW数据。*/
    VI_MODE_MIPI_YUV420_NORMAL,     /**<输入数据符合MIPI协议，用于传输YUV420 normal模式的数据。*/
    VI_MODE_MIPI_YUV420_LEGACY,     /**<输入数据符合MIPI协议，用于传输YUV420 legacy模式的数据。*/
    VI_MODE_MIPI_YUV422,            /**<输入数据符合MIPI协议，用于传输YUV422数据。*/
    VI_MODE_LVDS,                   /**<输入数据符合LVDS 协议。*/
    VI_MODE_HISPI,                  /**<输入数据符合HISPI协议。*/
    VI_MODE_SLVS,                   /**<输入数据符合SLVS-EC协议。*/
	VI_MODE_MEM,
    VI_MODE_BUTT
} VI_INTF_MODE_E;


/**定义视频设备的输入模式*/
typedef enum arVI_INPUT_MODE_E
{
    VI_INPUT_MODE_BT656 = 0,        /**<输入数据的协议符合标准BT.656协议。*/
    VI_INPUT_MODE_BT601,            /**<输入数据的协议符合标准BT.601协议。*/
    VI_INPUT_MODE_DIGITAL_CAMERA,   /**<输入数据的协议为Digital camera协议。*/
    VI_INPUT_MODE_INTERLEAVED,      /**<输入数据的协议为BT1120 interleave。*/
    VI_INPUT_MODE_MIPI,             /**<输入数据符合MIPI协议。*/
    VI_INPUT_MODE_LVDS,             /**<输入数据符合LVDS协议。*/
    VI_INPUT_MODE_HISPI,            /**<输入数据符合HISPI协议。*/
    VI_INPUT_MODE_SLVS,             /**<输入数据符合SLVS-EC协议。*/

    VI_INPUT_MODE_BUTT
} VI_INPUT_MODE_E;


/**定义视频设备的复合工作模式*/
typedef enum arVI_WORK_MODE_E
{
    VI_WORK_MODE_1Multiplex = 0,    /**<1路复合工作模式。*/
    VI_WORK_MODE_2Multiplex,        /**<2路复合工作模式，输入数据的协议必须为标准BT.656协议。*/
    VI_WORK_MODE_3Multiplex,        /**<3路复合工作模式，输入数据的协议必须为标准BT.656协议。*/
    VI_WORK_MODE_4Multiplex,        /**<4路复合工作模式，输入数据的协议必须为标准BT.656协议。*/

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;

/**定义视频设备接收的是隔行或逐行输入图像*/
typedef enum arVI_SCAN_MODE_E
{
    VI_SCAN_INTERLACED  = 0,        /**<VI输入为隔行图像。*/
    VI_SCAN_PROGRESSIVE,            /**<VI输入为逐行图像。*/

    VI_SCAN_BUTT
} VI_SCAN_MODE_E;

/**定义视频设备接收的YUV数据的数据排列顺序*/
typedef enum arVI_YUV_DATA_SEQ_E
{
    VI_DATA_SEQ_VUVU = 0,           /**<YUV 数据通过分离模式输入时，C分量的输入排列顺序为VUVU。*/
    VI_DATA_SEQ_UVUV,               /**<YUV 数据通过分离模式输入时，C分量的输入排列顺序为UVUV。*/

    VI_DATA_SEQ_UYVY,               /**<YUV 数据通过复合模式输入时，顺序为UYVY。*/
    VI_DATA_SEQ_VYUY,               /**<YUV 数据通过复合模式输入时，顺序为VYUY。*/
    VI_DATA_SEQ_YUYV,               /**<YUV 数据通过复合模式输入时，顺序为YUYV。*/
    VI_DATA_SEQ_YVYU,               /**<YUV 数据通过复合模式输入时，顺序为YVYU。*/

    VI_DATA_SEQ_BUTT
} VI_YUV_DATA_SEQ_E;

/**定义视频设备接收的时钟类型*/
typedef enum arVI_CLK_EDGE_E
{
    VI_CLK_EDGE_SINGLE_UP = 0,      /**<时钟单沿模式，且VI设备在上升沿采样。*/
    VI_CLK_EDGE_SINGLE_DOWN,        /**<时钟单沿模式，且VI设备在下降沿采样。*/

    VI_CLK_EDGE_BUTT
} VI_CLK_EDGE_E;

/**定义视频设备接收的数据的分量类型*/
typedef enum arVI_COMPONENT_MODE_E
{
    VI_COMPONENT_MODE_SINGLE = 0,   /**<输入数据为单分量。*/
    VI_COMPONENT_MODE_DOUBLE,       /**<输入数据为双分量。*/

    VI_COMPONENT_MODE_BUTT
} VI_COMPONENT_MODE_E;

/**定义视频设备接收的数据是复合还是分离模式*/
typedef enum arVI_COMBINE_MODE_E
{
    VI_COMBINE_COMPOSITE = 0,       /**<复合模式。*/
    VI_COMBINE_SEPARATE,            /**<分离模式。*/

    VI_COMBINE_BUTT
} VI_COMBINE_MODE_E;

/**定义视频设备输入数据的垂直同步信号类型*/
typedef enum arVI_VSYNC_E
{
    VI_VSYNC_FIELD = 0,             /**<垂直同步翻转模式，即翻转一次表示一场。BT.601模式下表示场号，DC模式下表示行有效信号。*/
    VI_VSYNC_PULSE,                 /**<垂直同步脉冲模式，即一个脉冲到来表示新的一帧或一场。*/

    VI_VSYNC_BUTT
} VI_VSYNC_E;

/**定义视频设备输入数据垂直同步信号的极性*/
typedef enum arVI_VSYNC_NEG_E
{
    VI_VSYNC_NEG_HIGH = 0,          /**<若VI_VSYNC_E = VI_VSYNC_FIELD，则表示偶数场的vsync信号为高电平；若VI_VSYNC_E = VI_VSYNC_PULSE，则表示vsync同步脉冲为正脉冲。*/
    VI_VSYNC_NEG_LOW,               /**<若VI_VSYNC_E = VI_VSYNC_FIELD，则表示偶数场的vsync信号为低电平；若VI_VSYNC_E = VI_VSYNC_PULSE，则表示vsync同步脉冲为负脉冲。*/

    VI_VSYNC_NEG_BUTT
} VI_VSYNC_NEG_E;

/**定义视频设备输入数据的水平同步信号类型*/
typedef enum arVI_HSYNC_E
{
    VI_HSYNC_VALID_SINGNAL = 0,     /**<水平同步数据有效信号。*/
    VI_HSYNC_PULSE,                 /**<水平同步脉冲信号。*/

    VI_HSYNC_BUTT
} VI_HSYNC_E;

/**定义视频设备输入数据水平同步信号的极性*/
typedef enum arVI_HSYNC_NEG_E
{
    VI_HSYNC_NEG_HIGH = 0,          /**<若VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL，则高电平表示有效数据；若VI_HSYNC_E = VI_HSYNC_PULSE，则正脉冲表示同步脉冲。*/
    VI_HSYNC_NEG_LOW,               /**<若VI_HSYNC_E = VI_HSYNC_VALID_SINGNAL，则低电平表示有效数据；若VI_HSYNC_E = VI_HSYNC_PULSE，则负脉冲表示同步脉冲。*/

    VI_HSYNC_NEG_BUTT
} VI_HSYNC_NEG_E;

/**定义视频设备输入数据的垂直有效同步信号类型*/
typedef enum arVI_VSYNC_VALID_E
{
    VI_VSYNC_NORM_PULSE = 0,        /**<表示垂直有效同步标识。*/
    VI_VSYNC_VALID_SINGAL,          /**<表示垂直同步时序行有效信号。*/

    VI_VSYNC_VALID_BUTT
} VI_VSYNC_VALID_E;

/**定义视频设备输入数据的垂直有效同步信号类型*/
typedef enum arVI_VSYNC_VALID_NEG_E
{
    VI_VSYNC_VALID_NEG_HIGH = 0,    /**<若VI_VSYNC_VALID_E = VI_VSYNC_NORM_SINGAL，则表示高电平为有效信号。*/
    VI_VSYNC_VALID_NEG_LOW,         /**<若 VI_VSYNC_VALID_E = VI_VSYNC_NORM_SINGAL，则表示低电平为有效信号。*/

    VI_VSYNC_VALID_NEG_BUTT
} VI_VSYNC_VALID_NEG_E;

/**定义视频设备输入数据的DE信号类型*/
typedef enum arVI_DE_E
{
    VI_DE_PIXEL_IN = 0,             /**<@note 新添加参数。*/
    VI_DE_HSYNC_IN,                 /**<@note 新添加参数。*/
    VI_DE_LOW_HVSYNC,               /**<@note 新添加参数。*/
    VI_DE_NO_DE,                    /**<@note 新添加参数。*/

    VI_DE_BUTT
} VI_DE_E;

/**定义视频设备输入数据的DE信号极性*/
typedef enum arVI_DE_NEG_E
{
    VI_DE_NEG_HIGH = 0,    /**<表示高电平为有效信号。*/
    VI_DE_NEG_LOW,         /**<表示低电平为有效信号。*/

    VI_DE_NEG_BUTT
} VI_DE_NEG_E;


/**定义视频设备输入时序的消隐信息*/
typedef struct arVI_TIMING_BLANK_S
{
    AR_U32 u32HsyncHfb ;            /**<水平前消隐区宽度。*/
    AR_U32 u32HsyncAct ;            /**<水平有效宽度。*/
    AR_U32 u32HsyncHbb ;            /**<水平后消隐区宽度*/
    AR_U32 u32VsyncVfb ;            /**<帧图像或隔行输入时奇场图像的垂直前消隐区高度。*/
    AR_U32 u32VsyncVact ;           /**<帧图像或隔行输入时奇场垂直有效高度。*/
    AR_U32 u32VsyncVbb ;            /**<帧图像或隔行输入时奇场垂直后消隐区高度。*/
    AR_U32 u32VsyncVbfb ;           /**<隔行输入时偶场垂直前消隐区高度（帧输入时无效）。*/
    AR_U32 u32VsyncVbact ;          /**<隔行输入时偶场垂直有效高度（帧输入时无效）。*/
    AR_U32 u32VsyncVbbb ;           /**<隔行输入时偶场垂直后消隐区高度（帧输入时无效）。*/
} VI_TIMING_BLANK_S;

/**定义视频设备接收BT.601、DC、BT.656和BT.1120时序的同步信息*/
typedef struct arVI_SYNC_CFG_S
{
    VI_VSYNC_E              enVsync;            /**<垂直同步信号类型。*/
    VI_VSYNC_NEG_E          enVsyncNeg;         /**<垂直同步信号的极性。*/
    VI_HSYNC_E              enHsync;            /**<水平同步信号类型。*/
    VI_HSYNC_NEG_E          enHsyncNeg;         /**<水平同步信号的极性。*/
    VI_VSYNC_VALID_E        enVsyncValid;       /**<垂直有效同步信号类型。*/
    VI_VSYNC_VALID_NEG_E    enVsyncValidNeg;    /**<垂直有效同步信号的极性。*/
    VI_TIMING_BLANK_S       stTimingBlank;      /**<输入时序的消隐信息。*/
    VI_DE_E                 enDE;               /**<@note 新添加参数。DE信号类型。*/
    VI_DE_NEG_E             enDENeg;            /**<@note 新添加参数。DE信号极性。*/
} VI_SYNC_CFG_S;

/**定义BT.656定时基准码最高bit配置*/
typedef enum arVI_BT656_FIXCODE_E
{
    VI_BT656_FIXCODE_1 = 0,         /**<BT.656协议的EAV/SAV最高bit固定为1。*/
    VI_BT656_FIXCODE_0,             /**<BT.656协议的EAV/SAV最高bit固定为0。*/

    VI_BT656_FIXCODE_BUTT
} VI_BT656_FIXCODE_E;

/**BT.656定时基准码场指示位（F）极性*/
typedef enum arVI_BT656_FIELD_POLAR_E
{
    VI_BT656_FIELD_POLAR_STD = 0,   /**<标准模式，第一场F=0，第二场F=1。*/
    VI_BT656_FIELD_POLAR_NSTD,      /**<非标准模式，第一场F=1，第二场F=0。*/

    VI_BT656_FIELD_POLAR_BUTT
} VI_BT656_FIELD_POLAR_E;

/**定义视频设备接收的BT.656时序的同步信息*/
typedef struct arVI_BT656_SYNC_CFG_S
{
    VI_BT656_FIXCODE_E     enFixCode;           /**<BT.656定时基准码最高bit配置。*/
    VI_BT656_FIELD_POLAR_E enFieldPolar;        /**<BT.656定时基准码场指示位（F）极性。*/
} VI_BT656_SYNC_CFG_S;

/**VI输入数据类型枚举*/
typedef enum arVI_DATA_TYPE_E
{
    VI_DATA_TYPE_YUV = 0,           /**<输入数据类型为YUV，VI前端一般接的是AD。*/
    VI_DATA_TYPE_RGB,               /**<输入数据类型为RGB，VI前端一般接的是Sensor。*/

    VI_DATA_TYPE_BUTT
} VI_DATA_TYPE_E;

/**定义bayer域相位调整类型*/
typedef enum arVI_REPHASE_MODE_E
{
    VI_REPHASE_MODE_NONE = 0,       /**<不进行相位调整。*/
    VI_REPHASE_MODE_SKIP_1_2,       /**<进行SKIP1/2类型的调整。*/
    VI_REPHASE_MODE_SKIP_1_3,       /**<进行SKIP1/3类型的调整。*/
    VI_REPHASE_MODE_BINNING_1_2,    /**<进行BINNING1/2类型的调整。*/
    VI_REPHASE_MODE_BINNING_1_3,    /**<进行BINNING1/3类型的调整。*/

    VI_REPHASE_MODE_BUTT
} VI_REPHASE_MODE_E;

/**定义bayer域相位调整属性*/
typedef struct arVI_BAS_REPHASE_ATTR_S
{
    VI_REPHASE_MODE_E   enHRephaseMode;         /**<水平相位调整类型。*/
    VI_REPHASE_MODE_E   enVRephaseMode;         /**<垂直相位调整类型。*/
} VI_BAS_REPHASE_ATTR_S;

/**定义bayer域缩放属性*/
typedef struct arVI_BAS_SCALE_ATTR_S
{
    SIZE_S      stBasSize;          /**<缩放之后的宽与高。*/
} VI_BAS_SCALE_ATTR_S;

/**定义bayer域属性*/
typedef struct arVI_BAS_ATTR_S
{
    VI_BAS_SCALE_ATTR_S     stSacleAttr;        /**<Bayer域缩放属性。*/
    VI_BAS_REPHASE_ATTR_S   stRephaseAttr;      /**<Bayer域相位调整属性。*/
} VI_BAS_ATTR_S;

/**定义WDR参数数据信息*/
typedef struct arVI_WDR_ATTR_S
{
    WDR_MODE_E  enWDRMode;          /**<WDR工作模式，分为帧模式、行模式、非WDR等三大类。*/
    AR_U32      u32CacheLine;       /**<在线行模式WDR，离线PIPE数据缓存的行数。取值范围为[1，PIPE的图像高度u32MaxH]。根据带宽的使用情况和sensor的曝光行差等因素来调节到合适值。*/
} VI_WDR_ATTR_S;


/**定义视频输入设备的属性*/
typedef struct arVI_DEV_ATTR_S
{
    VI_INTF_MODE_E      enIntfMode;                             /**<@note 转义参数 接口模式。*/
    VI_WORK_MODE_E      enWorkMode;                             /**<复合工作模式，使用虚拟通道进行复合。*/

    AR_U32              au32ComponentMask[VI_COMPMASK_NUM];     /**<@note 转义参数 分量掩码配置。*/
    VI_SCAN_MODE_E      enScanMode;                             /**<输入扫描模式 (逐行、隔行)。*/
    AR_S32              as32AdChnId[VI_MAX_ADCHN_NUM];          /**<@note不使用参数 取值范围[-1, 3]，推荐统一设置为默认值-1，此参数无意义。*/

    VI_YUV_DATA_SEQ_E   enDataSeq;                              /**<@note 转义参数 输入数据顺序。*/
    VI_SYNC_CFG_S       stSynCfg;                               /**<@note 转义参数 同步时序配置，请参考dvp sensor 点亮文档。*/

    VI_DATA_TYPE_E      enInputDataType;                        /**<输入数据类型，Sensor输入一般为RGB，AD输入一般为YUV。*/

    AR_BOOL             bDataReverse;                           /**<@note不使用参数 因为走线约束等硬件原因，有可能出现AD/Sensor的数据线与VI数据线连接数据高低位反接。当AD/Sensor管脚与VI管脚正向连接时，取bDataReverse = AR_FALSE；当反向连接时，取bDataReverse = AR_TRUE。*/

    SIZE_S              stSize;                                 /**<VI设备可设置要捕获图像的高宽。*/

    VI_BAS_ATTR_S       stBasAttr;                              /**<@note不使用参数 Bayer域缩放之后的宽、高，以及相位调整的类型。*/

    VI_WDR_ATTR_S       stWDRAttr;                              /**<WDR属性。*/

    DATA_RATE_E         enDataRate;                             /**<设备的速率。*/
    AR_BOOL             bDvpHighLowByteSwap;                    /**<@note新加参数 DVP数据线高低8位交换。*/

    AR_BOOL             bIrSensor;                              /**<@note新加参数 是否为IR senosr。*/
    AR_BOOL             bDvpInputFromNuc;                       /**<@note新加参数 DVP输入是否来自于NUC。该参数主要用于艾睿红外sensor，这种sensor不是直接走DVP输出，而是由NUC模块接收sensor输出，再转换为DVP信号。*/
} VI_DEV_ATTR_S;

/**定义VI DEV与PIPE的绑定关系*/
typedef struct arVI_DEV_BIND_PIPE_S
{
    AR_U32  u32Num;                             /**<该VI Dev所绑定的PIPE数目，取值范围[1, VI_MAX_PIPE_NUM]。*/
    VI_PIPE PipeId[VI_MAX_PHY_PIPE_NUM];        /**<该VI Dev绑定的PIPE号。*/
} VI_DEV_BIND_PIPE_S;

/**定义VI PIPE的3DNR参考帧来源选择*/
typedef enum arVI_NR_REF_SOURCE_E
{
    VI_NR_REF_FROM_RFR = 0,         /**<重构帧作为参考帧。*/
    VI_NR_REF_FROM_CHN0,            /**<0通道输出作为参考帧。*/

    VI_NR_REF_FROM_BUTT
} VI_NR_REF_SOURCE_E;

/**定义VI PIPE的Bypass模式*/
typedef enum arVI_PIPE_BYPASS_MODE_E
{
    VI_PIPE_BYPASS_NONE,            /**<VI的数据经过FE与BE处理。*/
    VI_PIPE_BYPASS_FE,              /**<VI的数据不经过FE处理，只经过BE处理。*/
    VI_PIPE_BYPASS_BE,              /**<VI的数据经过FE处理，不经过BE处理。*/

    VI_PIPE_BYPASS_BUTT
} VI_PIPE_BYPASS_MODE_E;

/**定义VI PIPE的3DNR属性*/
typedef struct arVI_NR_ATTR_S
{
    PIXEL_FORMAT_E      enPixFmt;               /**<@note 不使用 重构帧的像素格式。*/
    DATA_BITWIDTH_E     enBitWidth;             /**<@note 不使用 重构帧的bit位宽。*/
    VI_NR_REF_SOURCE_E  enNrRefSource;          /**<@note 不使用 参考帧来源选择。*/
    COMPRESS_MODE_E     enCompressMode;         /**<note 转义参数 重构帧是否压缩。*/
} VI_NR_ATTR_S;


typedef enum arVI_PIPE_FEATURE_E
{
    FEA_AR_MPI_ISP_GetVDTimeOut=1<<0,
	FEA_AR_MPI_ISP_GetAEStatistics=1<<1,
	FEA_AR_MPI_ISP_GetWBStatistics=1<<2,
	FEA_AR_MPI_ISP_GetFocusStatistics=1<<3,
}VI_PIPE_FEATURE_E;

typedef enum arVI_NOCPM_ENABLE_E
{
   VIN_PIPE_NOCPM_DISABLE=0,
   VIN_PIPE_NOCPM_ENABLE=0X12EDB48C,
}VI_NOCPM_ENABLE_E;

/**定义VI PIPE属性*/
typedef struct arVI_PIPE_ATTR_S
{
    VI_PIPE_BYPASS_MODE_E enPipeBypassMode;     /**<@note 不使用 VI PIPE的Bypass模式。静态属性，创建PIPE时设定，不可更改。*/
    AR_BOOL               bYuvSkip;             /**<@note 不使用 是否关闭下采样和CSC。AR_FALSE：yuv skip 不使能；AR_TRUE：yuv skip 使能。静态属性，创建PIPE时设定，不可更改。*/
    AR_BOOL               bIspBypass;           /**<@note 不使用 ISP是否bypass。AR_FALSE：ISP正常运行；AR_TRUE：ISP bypass，不运行ISP。静态属性，创建PIPE时设定，不可更改。*/
    AR_U32                u32MaxW;              /**<输入图像宽度。静态属性，创建PIPE时设定，不可更改。*/
    AR_U32                u32MaxH;              /**<输入图像高度。静态属性，创建PIPE 时设定，不可更改。*/
    PIXEL_FORMAT_E        enPixFmt;             /**<@note 不使用 像素格式。*/
    COMPRESS_MODE_E       enCompressMode;       /**<@note 转义参数     数据压缩格式。*/
    DATA_BITWIDTH_E       enBitWidth;           /**<@note 转义参数 输入图像的bit位宽。静态属性，创建PIPE时设定，不可更改*/
    AR_BOOL               bNrEn;                /**<@note 不使用 NR使能开关。AR_FALSE：不使能；AR_TRUE：使能。*/
    VI_NR_ATTR_S          stNrAttr;             /**<@note 转义参数 NR属性结构体。静态属性，创建PIPE时设定，不可更改。*/
    AR_BOOL               bSharpenEn;           /**<@note 不使用 Sharpen使能开关。*/
    FRAME_RATE_CTRL_S     stFrameRate;          /**<@note 不使用 帧率控制。*/
    AR_BOOL               bDiscardProPic;       /**<@note 不使用 视频通路是否丢弃长曝光的帧。*/
	ENUM_VFE_MODE         enVfeMode;            /**<@note 新添加参数 前处理模式，主要支持ceva hdr，ai isp 功能。*/
	AR_S32                s32Trigger;          /**<@note 新添加参数,是否需要用户调用api  触发出图，只针对实际的sensor */
	AR_S32                s32EnableManAecUpdate; /**<@note 新添加参数,是否使能手动的更新aec，一般用在大图模式下的aec手动运行的情况 */
	AR_S32                s32EnableManAwbUpdate; /**<@note 新添加参数,是否使能手动的更新awb，一般用在大图模式下的awb手动运行的情况*/
	AR_S32                s32EnableManAfUpdate; /**<@note 新添加参数,是否使能手动的更新af，一般用在大图模式下的af手动运行的情况 */	
	AR_S32                s32enableUsrAec;/**<@note 新添加参数,是否使能使用用户直接的aec，屏蔽掉系统aec */	
	AR_S32                s32enableUsrAwb;/**<@note 新添加参数,是否使能使用用户直接的awb，屏蔽掉系统awb */	
	AR_S32                s32enableUsrAf;/**<@note 新添加参数,是否使能使用用户直接的af，屏蔽掉系统af */	
	AR_S32                s32AwbRation;/**<@note 新添加参数,awb的调整率，用来降低awb 的计算频率，从而节省cpu 消耗*/	
	AR_U32                u32FeatureMask; /**<@note 新添加参数,使能某些pipe 功能, VI_PIPE_FEATURE_E*/
	VI_NOCPM_ENABLE_E     enEnableNoCmp; /**<@note 新添加参数,使能pipe的非压缩功能，如果不使能非压缩，即使enCompressMode 配置为非压缩，系统仍然使用压缩功能*/	
	AR_S32                s32AecRation;/**<@note 新添加参数,aec的调整率，用来降低aec 的计算频率，从而节省cpu 消耗,需要配合sensor驱动的特性，aec 算法调试参数保证aec 收敛速度满足需求*/	
} VI_PIPE_ATTR_S;

/**定义VI PIPE属性，本属性用来给用户机会修改驱动的一些默认配置，典型的使用本属性的场景是一个ispin的驱动适配所有灌raw，本参数一旦使能，所有的参数都要正确配置，
强烈建议用户先调用本属性的get 获取驱动的值，然后针对自己的需求修改，本api 必须放到AR_MPI_ISP_SetPubAttr 之后，AR_MPI_ISP_Init 之前
这点需要注意，AR_MPI_VI_GetPipeExtAttr，获取到的是驱动的默认值，而不是AR_MPI_VI_SetPipeExtAttr 设置进去的*/
typedef struct arVI_PIPE_EXT_ATTR_S
{
    AR_S32 s32EnableExtAttr;  /**< 使能管道的扩展属性，使能后，这些属性值将使用本设置的属性，不再使用从驱动程序获取的值*/
	
    AR_U32  u32LinesPer500ms; /**< 500ms 曝光时间的时候，sensor的曝光行数*/
	AR_U32  u32Hmax;          /**< Sensor的htotal=width+hblank， 等于sensor的宽度加上行消隐*/
    AR_U32  u32FullLinesStd;  /**< Sensor的vtotal=height+vblank， 等于sensor的高度加上帧消隐*/
    AR_U32  u32FullLinesMid;  /**< 在hdr模式的时候，sensor的中曝光的最大曝光行数*/
    AR_U32  u32FullLinesShort;  /**< 在hdr模式的时候，sensor的短曝光的最大曝光行数*/
    AR_U32  u32FullLinesMax;   /**< sensor 容许的最大曝光行数*/
	AR_CHAR TuningPraBinName[256]; /**< isp的效果参数的文件名，注意带路径*/
    AR_BOOL bDvpDownSampleEn;/**<使用dvp输入的降采样功能*/
	AR_BOOL bFoucs;/**<使用isp的对焦功能*/
    AR_U32   au32ShortOffset;/**<描述在非vc hdr模式下，短曝光开始的位置相对于帧的便宜的行数*/
    AR_U32   au32MidOffset;/**<描述在非vc hdr模式下，中曝光开始的位置相对于帧的便宜的行数*/
    AR_U32   au32LongOffset;/**<描述在非vc hdr模式下，长曝光开始的位置相对于帧的便宜的行数*/
    AR_U32   au32VcCnt;/**<描述在vc hdr模式下，虚拟通道的个数，一般为2 或者 3，非vc 模式设置为0*/
    AR_U32   au32VcMask;/**<描述在vc hdr模式下，使用的虚拟通道的情况，不同的bit，代表不同弄的vc 号，非 vchdr 设置为0*/
}VI_PIPE_EXT_ATTR_S;


/**定义VI PIPE的3DNR X接口的参数*/
typedef struct arVI_PIPE_NRX_PARAM_S
{
    AR_BOOL   bEnable;                                          /**<@note 新添加参数。Range:[0, 1]; Format:1.0;Enable/Disable NrXParam Function*/
    AR_BOOL   bManual;                                          /**<@note 新添加参数。*/
    STRU_ISP_SUB_MODULE_3D_2D_NR_TUNING_T stAutoNrXParam;       /**<@note 新添加参数。set to tuning bin, the system will select the NrXParam from tuning*/
    STRU_ISP_SUB_MODULE_3D_2D_NR_PRA stManualNrXParam;          /**<@note 新添加参数。the isp will use the NrXParam directly from this setting*/
}VI_PIPE_NRX_PARAM_S;

/**定义VI通道属性*/
typedef struct arVI_CHN_ATTR_S
{
    SIZE_S              stSize;                 /**<目标图像大小。*/
    PIXEL_FORMAT_E      enPixelFormat;          /**<目标图像像素格式。静态属性，设置CHN时设定，不可更改。*/
    DYNAMIC_RANGE_E     enDynamicRange;         /**<@note不使用 目标图像动态范围。静态属性，设置CHN时设定，不可更改。*/
    VIDEO_FORMAT_E      enVideoFormat;          /**<@note不使用 目标图像视频数据格式。*/
    COMPRESS_MODE_E     enCompressMode;         /**<@note 转义参数 目标图像压缩格式，只支持seg（cf50）或不压缩,压缩只ch0支持，且ch0必须配置和sensor大小相同的情况下支持*/
    AR_BOOL             bMirror;                /**<@note不使用 Mirror使能开关。AR_FALSE：不使能；AR_TRUE：使能。*/
    AR_BOOL             bFlip;                  /**<@note不使用 Flip 使能开关。AR_FALSE：不使能；AR_TRUE：使能。*/
    AR_U32              u32Depth;               /**<@note 转义参数，队列超过这个深度，前面的数据会被丢弃。*/
    FRAME_RATE_CTRL_S   stFrameRate;            /**<帧率控制。源帧率取值范围：(0, 240]，以及-1。目标帧率取值范围：[-1, 240]。当源帧率为-1 时，目标帧率必须为-1(不进行帧率控制)，其他情况下，目标帧率不能大于源帧率。*/
    AR_S32              u32BufCount;            /**<@note 新添加参数。 分配给本通道使用的buffer的数据，如果为0，默认设置为5. 设置的原则是，如果vin 后面bind的模块较多，则增加这个数目*/
    AR_S32              u32DepthClient[8];      /**<@note 新添加参数,多进程取流使用，每一个用户获取图像的队列深度。队列超过这个深度，前面的数据会被丢弃，最多支持8个用户独立获取*/
    AR_S32              s32Timeout;            /**<@note  新加参数，<=0 的时候默认10ms，加大这个值有利用减少系统调用，节省cpu，但是也导致需要多一个buffer。*/
	AR_S32              s32KeepRation;         /**<@note  新加参数，控制isp 缩小输出的时候是否先crop到输出图像的比例，然后压缩，0，不做同比例，1 做同比例*/	
	AR_S32				s32AntiDisable; 		/**<@note  新加参数，控制isp 输出缩放是否打开抗锯齿功能>	，0 打开抗锯齿，1： 关闭抗锯齿*/
	AR_S32              s32ContinueBuffer;      /**<@note  新加参数，控制yuv 三个pannel的地址是否连续在一起，1：连续在一起。0 非连续在一起*/
} VI_CHN_ATTR_S;


typedef struct{
	AR_S32 s32SofLowdeayEn;  /**< 是否使能软件lowdelay 功能 */
	AR_U32 u32LowdeayPattern;/**< 软件lowdeay 使用的pattern */
}VIN_SOFT_LOWDELAY_ATTR;


/**定义VI通道扩展属性，在设置完成通道属性后，isp run 之前配置。静态属性，isp run 之后不可修改*/
typedef struct arVI_CHN_EXT_ATTR_S
{
   RECT_S               stHwCrop;  /**<配置通道的crop 属性，通道对原图像进行指定区域的crop 后输出，如果ch输出的是raw图，请水平方向6像素对齐,如果kw 设置为0，将使用默认的crop 策略*/  

   /**<如下属性控制数据写到buffer的什么位置,使能这个feature 后，(total_w total_h 大于0就认为使能了本特征),驱动会分配一个total_w * total_h 的buffer。 数据会写入到这个buffer的
      x，y 开始的位置，数据的wh 由VI_CHN_ATTR_S 的stSize 指定，本feature 用来动态修改分辨率的时候使用*/
   AR_S32 x;  //
   AR_S32 y;
   AR_S32 total_w;  //x+width<=total_w
   AR_S32 total_h;  //y+total_h<=total_h

   /**<通道的lowdelay 模式*/
   STRU_LOW_DELAY_MODE_T enLowDelayMode;
   AR_S32 s32DelayLine;  /**<通道写入多少行后，输出lowdelay 信号，默认16*/   
   ENUM_LOW_DELAY_SINK_T enLowDelaySink;
   AR_S32 hw_out_index;
   RECT_S               stHwCrop1;  /**<配置通道的crop1 属性，本属性只作用于isp输出的scaler 之后，对scaler之后的图像进行cop 输出*/
   VIN_SOFT_LOWDELAY_ATTR stSoftLowdelayAttr;
} VI_CHN_EXT_ATTR_S;



/**定义VI PIPE的状态信息*/
typedef struct arVI_PIPE_STATUS_S
{
    AR_BOOL bEnable;                /**<当前PIPE是否使能。*/
    AR_U32  u32IntCnt;              /**<中断计数。*/
    AR_U32  u32FrameRate;           /**<VI PIPE的实时帧率。*/
    AR_U32  u32LostFrame;           /**<丢帧计数。*/
    AR_U32  u32VbFail;              /**<VB申请失败计数。*/
    SIZE_S  stSize;                 /**<PIPE当前图像大小。*/
} VI_PIPE_STATUS_S;

typedef struct
{
   STRU_CAM_PRA_CF50_CMP_PRA_T stChCmpAttr;  
}VI_CH_CF50_CMP_ATTR_T;

/**定义扩展通道的图像来源*/
typedef enum arVI_EXT_CHN_SOURCE_E
{
    VI_EXT_CHN_SOURCE_TAIL,         /**<扩展通道的图像来自物理通道后处理（DIS,LDC,SPREAD等）后的图像。*/
    VI_EXT_CHN_SOURCE_HEAD,         /**<扩展通道的图像来自物理通道后处理（DIS,LDC,SPREAD等）前的图像。*/

    VI_EXT_CHN_SOURCE_BUTT
} VI_EXT_CHN_SOURCE_E;

/**定义VI扩展通道属性*/
typedef struct arVI_EXT_CHN_ATTR_S
{
    VI_EXT_CHN_SOURCE_E enSource;               /**<定义扩展通道的图像来源，只支持VI_EXT_CHN_SOURCE_TAIL。*/
    VI_CHN              s32BindChn;             /**<绑定的源物理通道。*/
    SIZE_S              stSize;                 /**<目标图像大小。*/
    PIXEL_FORMAT_E      enPixFormat;            /**<目标图像像素格式。*/
    DYNAMIC_RANGE_E     enDynamicRange;         /**<目标图像动态范围。*/
    COMPRESS_MODE_E     enCompressMode;         /**<目标图像压缩格式。*/
    AR_U32              u32Depth;               /**<用户获取图像的队列深度。*/
    FRAME_RATE_CTRL_S   stFrameRate;            /**<帧率控制。源帧率取值范围：(0, 240]，以及-1。目标帧率取值范围：[-1, 240]。当源帧率为-1时，目标帧率必须为-1(不进行帧率控制)，其他情况下，目标帧率不能大于源帧率。*/
} VI_EXT_CHN_ATTR_S;

/**定义VI扩展通道属性*/
typedef enum arVI_CROP_COORDINATE_E
{
    VI_CROP_RATIO_COOR = 0,         /**<相对坐标。*/
    VI_CROP_ABS_COOR,               /**<绝对坐标。*/
    VI_CROP_BUTT
} VI_CROP_COORDINATE_E;

/**定义VI CROP信息结构体*/
typedef struct arVI_CROP_INFO_S
{
    AR_BOOL                 bEnable;            /**<CROP使能开关。*/
    VI_CROP_COORDINATE_E    enCropCoordinate;   /**<CROP起点坐标模式。*/
    RECT_S                  stCropRect;         /**<CROP的矩形区域。*/
} VI_CROP_INFO_S;


/**定义VI通道的状态信息*/
typedef struct arVI_CHN_STATUS_S
{
    AR_BOOL bEnable;                /**<当前通道是否使能。0：不使能；1：使能。*/
    AR_U32  u32FrameRate;           /**<VI通道的实时帧率。*/
    AR_U32  u32LostFrame;           /**<丢帧计数。*/
    AR_U32  u32VbFail;              /**<VB申请失败计数。*/
    SIZE_S  stSize;                 /**<通道当前图像大小。*/
	AR_U32  u32FredomBufCount;      /**<当前通道可以自由使用的buffer的个数，也就是说这个buffer 已经被其他应用者释放，但是还没有配置给isp的buffer的数量*/
} VI_CHN_STATUS_S;

typedef struct arVI_PMF_ATTR_S
{
    AR_BOOL bEnable;                            /**<@note 新添加参数。Whether PMF is enable */
    SIZE_S  stDestSize;                         /**<@note 新添加参数。Target size */
    AR_S64  as64PMFCoef[VI_PMFCOEF_NUM];        /**<@note 新添加参数。Array of PMF coefficients */
} VI_PMF_ATTR_S;

/**定义VS信号的属性*/
typedef struct arVI_DUMP_ATTR_S
{
    AR_BOOL         bEnable;        /**<是否使能dump。*/
    AR_U32          u32Depth;       /**<Dump数据的队列深度。取值范围：[0, 8]。*/
} VI_DUMP_ATTR_S;

/**定义VI PIPE数据的来源类型*/
typedef enum arVI_PIPE_FRAME_SOURCE_E
{
    VI_PIPE_FRAME_SOURCE_DEV = 0,   /**<数据来自设备。*/
    VI_PIPE_FRAME_SOURCE_USER_FE,   /**<数据来自用户从FE送进来的数据。*/
    VI_PIPE_FRAME_SOURCE_USER_BE,   /**<数据来自用户从BE送进来的数据。*/

    VI_PIPE_FRAME_SOURCE_BUTT
} VI_PIPE_FRAME_SOURCE_E;

typedef struct ar_VI_RAW_INFO_S
{
    VIDEO_FRAME_INFO_S      stVideoFrame;       /**<@note 新添加参数。*/
    ISP_CONFIG_INFO_S       stIspInfo;          /**<@note 新添加参数。*/
} VI_RAW_INFO_S;

/**定义VI通道提前上报中断属性*/
typedef struct arVI_EARLY_INTERRUPT_S
{
    AR_BOOL bEnable;                /**<提前上报中断使能开关。AR_FALSE：不使能；AR_TRUE：使能。*/
    AR_U32 u32LineCnt;              /**<提前上报中断提前的行数。该行数必须大于0，小于等于通道图像输出的高度。*/
} VI_EARLY_INTERRUPT_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* End of #ifndef__AR_COMM_VIDEO_IN_H__ */


