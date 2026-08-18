/**
 * \file
 * \brief 描述视频处理子系统相关的通用数据结构
 */

#ifndef __AR_COMM_VPSS_H__
#define __AR_COMM_VPSS_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "hal_type.h"
#include "ar_common.h"
#include "hal_errno.h"
#include "ar_comm_video.h"

#define VPSS_SHARPEN_GAIN_NUM		 (32)
#define VPSS_AUTO_ISO_STRENGTH_NUM	 (16)
#define VPSS_YUV_SHPLUMA_NUM		 (32)
#define VPSS_3DNR_MAX_AUTO_PARAM_NUM (16)

#define AR_ERR_VPSS_NULL_PTR		AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_VPSS_NOTREADY		AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_VPSS_INVALID_DEVID	AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
#define AR_ERR_VPSS_INVALID_CHNID	AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
#define AR_ERR_VPSS_EXIST			AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_EXIST)
#define AR_ERR_VPSS_UNEXIST			AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
#define AR_ERR_VPSS_NOT_SUPPORT		AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_SUPPORT)
#define AR_ERR_VPSS_NOT_PERM		AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define AR_ERR_VPSS_NOMEM			AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define AR_ERR_VPSS_NOBUF			AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define AR_ERR_VPSS_ILLEGAL_PARAM	AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_VPSS_BUSY			AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define AR_ERR_VPSS_BUF_EMPTY		AR_MPP_DEF_ERR(AR_ID_VPSS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUF_EMPTY)

#define VPSS_INVALID_FRMRATE	-1
#define VPSS_CHN0				0
#define VPSS_CHN1				1
#define VPSS_CHN2				2
#define VPSS_CHN3				3
#define VPSS_INVALID_CHN		-1

/**
* @brief  vpss channel mode enum
*/
typedef enum arVPSS_CHN_MODE_E
{
	VPSS_CHN_MODE_USER	= 0,	   /* User mode. */
	VPSS_CHN_MODE_AUTO	= 1		   /* Auto mode. */
} VPSS_CHN_MODE_E;

/**
* @brief  vpss ldc attribute structure
*/
typedef struct arVPSS_GDC_LDC_ATTR_S
{
	AR_FLOAT k[9];			   /**< @note RW; coeff */
	AR_FLOAT ldc_k0 ;		   /**< @note RW; coeff */
	AR_FLOAT ldc_k1 ;		   /**< @note RW; coeff */
	AR_FLOAT ldc_k2 ;		   /**< @note RW; coeff */
} VPSS_GDC_LDC_ATTR_S;

/**
* @brief  vpss ldc attribute structure
*/
typedef struct arVPSS_LDC_ATTR_S
{
	AR_BOOL		bEnable;							/**< @note RW;Whether LDC is enbale */
	VPSS_GDC_LDC_ATTR_S stAttr;						/**< @note RW; LDC attribute */
} VPSS_LDC_ATTR_S;

/**
* @brief  vpss spread attribute structure
*/
typedef VPSS_LDC_ATTR_S VPSS_SPREAD_ATTR_S;			/**< @note RW; spread attribute */

/**
* @brief  vpss fisheye attribute structure
*/
typedef VPSS_LDC_ATTR_S VPSS_FISHEYE_ATTR_S;		/**< @note RW; fisheye attribute */

/**
* @brief  vpss rotation_ex attribute structure
*/
typedef struct arVPSS_ROTATION_EX_ATTR_S
{
	AR_BOOL		  bEnable;							/**< @note Whether ROTATE_EX_S is enbale */
	ROTATION_EX_S stRotationEx;						/**< @note Rotate Attribute */
}VPSS_ROTATION_EX_ATTR_S;

#define AR_VPSS_ATTR \
	VPSS_CHN_MODE_E		enChnMode;			/**< RW; Vpss channel's work mode. */				\
	AR_U32				u32Width;			/**< RW; Width of target image. */					\
	AR_U32				u32Height;			/**< RW; Height of target image. */					\
	VIDEO_FORMAT_E		enVideoFormat;		/**< RW; Video format of target image. */			\
	PIXEL_FORMAT_E		enPixelFormat;		/**< RW; Pixel format of target image. */			\
	DYNAMIC_RANGE_E		enDynamicRange;		/**< RW; DynamicRange of target image. */			\
	COMPRESS_MODE_E		enCompressMode;		/**< RW; Compression mode of the output. */			\
	FRAME_RATE_CTRL_S	stFrameRate;		/**< Frame rate control info */						\
	AR_BOOL				bMirror;			/**< RW; Mirror enable. */							\
	AR_BOOL				bFlip;				/**< RW; Flip enable. */							\
	AR_U32				u32Depth;			/**< RW; Range: [0, 8]; User get list depth. */		\
	ASPECT_RATIO_S		stAspectRatio;		/**< Aspect Ratio info. */							\
	AR_U32				u32ScaleMode;		/**< @attention 新增加参数 set of the sacle mode */	\
	ROTATION_E			enRotation;			/**< rotation angle enum */							\
	VPSS_LDC_ATTR_S		stLdcAttr;			/**< @note LDC info */								\
	VPSS_SPREAD_ATTR_S	stSpreadAttr;		/**< @note Spread info */							\
	VPSS_FISHEYE_ATTR_S  stFisheyeAttr;		/**< @note Fisheye info */							\
	VPSS_ROTATION_EX_ATTR_S stRotationEx;	/**< @note rotation_ex */


/**
* @brief  vpss group attribute enum
*/
typedef struct arVPSS_GRP_ATTR_S
{
	AR_VPSS_ATTR
} VPSS_GRP_ATTR_S;

typedef struct arVPSS_GRP_ATTR_S VPSS_CHN_ATTR_S;

/**
* @brief  vpss crop coordinate enum
*/
typedef enum arVPSS_CROP_COORDINATE_E
{
	VPSS_CROP_RATIO_COOR = 0,	/**< Ratio coordinate. */
	VPSS_CROP_ABS_COOR			/**< Absolute coordinate. */
} VPSS_CROP_COORDINATE_E;

/**
* @brief  vpss crop information structure
*/
typedef struct arVPSS_CROP_INFO_S
{
	AR_BOOL					bEnable;			/**< RW; CROP enable. */
	VPSS_CROP_COORDINATE_E	enCropCoordinate;	/**< RW; Coordinate mode of the crop start point. */
	RECT_S					stCropRect;			/**< CROP rectangular. */
} VPSS_CROP_INFO_S;

/**
* @brief  vpss mpu information structure
*/
typedef struct arVPSS_MPU_INFO_S
{
	AR_BOOL					bEnable;			/**< RW; CROP enable. */
} VPSS_MPU_INFO_S;


/**
* @brief  vpss external channel attribute structure
*/
typedef struct arVPSS_EXT_CHN_ATTR_S
{
	VPSS_CHN		   s32BindChn;				/**< @note RW; Range: [0, 3]; Channel bind to. */
	AR_VPSS_ATTR
} VPSS_EXT_CHN_ATTR_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __AR_COMM_VPSS_H__ */

