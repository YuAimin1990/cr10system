#ifndef __HAL_SCALER_API_H__
#define __HAL_SCALER_API_H__
#include "ar_scaler.h"
#include "hal_npu_types.h"

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



typedef struct
{
    AR_U32 u32X;
    AR_U32 u32Y;
    AR_U32 u32W;
    AR_U32 u32H;
} AR_HAL_SCALER_CROP_S;

/**
* @brief  YUV图像处理接口
* @param  pstSrcImgs 源图片，pstCrops 坐标参数，pstDstImgs 目的图片，u32ImgNum 图片数量，u32Mode scaler模式
* @retval retval > 0 成功，其他加载失败
* @note   In order to make it simple, please process the same type of images each time.
* @note   call this routine to process all YUV planar images in batch mode and wait done, then call it again to process RGB images.
* @note   Mode: SCALER_MODE_INTERP/SCALER_MODE_BICUBIC/SCALER_MODE_BILINEAR in linux/ar_scaler.h
*/

AR_S32 ar_hal_scaler_crop_resize(AR_IMG_S * pstSrcImgs, AR_HAL_SCALER_CROP_S * pstCrops, AR_IMG_S * pstDstImgs, AR_U32 u32ImgNum, AR_U32 u32Mode);

AR_S32 ar_hal_scaler_set_frequency(AR_U32 u32FreqMHz);

/**
* @brief  将Scaler挂起，power off
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  在挂起之前请调用者自行保证没有正在运行的任务
*/
AR_S32 ar_hal_scaler_suspend(void);

/**
* @brief  将Scaler唤醒，power on
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  唤醒后，原来由调用者自行设置的频率需要调用者负责恢复
*/
AR_S32 ar_hal_scaler_resume(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__HAL_SCALER_H__

