#ifndef __MPI_SCALER_API_H__
#define __MPI_SCALER_API_H__

#include "hal_scaler_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_IMG_ALIGN_TO_LEFT  0x01
#define AR_IMG_ALIGN_TO_RIGHT 0x02
#define AR_IMG_ALIGN_TO_MID   0x04
#define AR_IMG_ALIGN_TO_UP    0x10
#define AR_IMG_ALIGN_TO_DOWN  0x20

/**
* @brief  图像缩放处理接口
* @param  pstSrcImgs 源图片，pstCrops 坐标参数，pstDstImgs 目的图片，u32ImgNum 图片数量，u32Mode scaler模式
* @retval retval > 0 成功，其他加载失败
* @note   In order to make it simple, please process the same type of images each time.
* @note   call this routine to process all YUV planar images in batch mode and wait done, then call it again to process RGB images.
* @note   Mode: SCALER_MODE_INTERP/SCALER_MODE_BICUBIC/SCALER_MODE_BILINEAR in linux/ar_scaler.h
*/

AR_S32 AR_MPI_SCALER_CropResize(AR_IMG_S * pstSrcImgs, AR_HAL_SCALER_CROP_S * pstCrops, AR_IMG_S * pstDstImgs, AR_U32 u32ImgNum, AR_U32 u32Mode);

/**
* @brief  图像等比例缩放处理接口
* @param  pstSrcImgs 源图片，pstCrops 坐标参数，pstDstImgs 目的图片，u32ImgNum 图片数量，u32Mode scaler模式，u32AlignMode 图片对其模式
* @retval retval > 0 成功，其他加载失败
* @note   In order to make it simple, please process the same type of images each time.
* @note   call this routine to process all YUV planar images in batch mode and wait done, then call it again to process RGB images.
* @note   Mode: SCALER_MODE_INTERP/SCALER_MODE_BICUBIC/SCALER_MODE_BILINEAR in linux/ar_scaler.h
* @note   when src_width/dst_width > src_height/dst_height, u32AlignMode should be 0x04(ALIGN_TO_MID), 0x10(ALIGN_TO_UP), 0x20(ALIGN_TO_DOWN)
* @note   when src_width/dst_width < src_height/dst_height, u32AlignMode should be 0x04(ALIGN_TO_MID), 0x01(ALIGN_TO_LEFT), 0x02(ALIGN_TO_RIGHT)
*/

AR_S32 AR_MPI_SCALER_CropResizeRatio(AR_IMG_S * pstSrcImgs, AR_HAL_SCALER_CROP_S * pstCrops, AR_IMG_S * pstDstImgs, AR_U32 u32ImgNum, AR_U32 u32Mode, AR_U32 u32AlignMode);
AR_S32 AR_MPI_SCALER_SetFrequency(AR_U32 u32FreqMHz);

/**
* @brief  将Scaler挂起，power off
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  在挂起之前请调用者自行保证没有正在运行的任务
*/
AR_S32 AR_MPI_SCALER_Suspend(void);

/**
* @brief  将Scaler唤醒，power on
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  唤醒后，原来由调用者自行设置的频率需要调用者负责恢复
*/
AR_S32 AR_MPI_SCALER_Resume(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__MPI_SCALER_H__

