#ifndef __MPI_IFC_API_H__
#define __MPI_IFC_API_H__

#include "hal_npu_types.h"
#include "hal_ifc_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define ALIGNED_BYTES 256
#define IS_ALIGN_WITH_256_BYTE(stride) (stride%ALIGNED_BYTES == 0 ? 1:0)

typedef struct
{
    AR_S32 s32RC0;
    AR_S32 s32RC1;
    AR_S32 s32RC2;
    AR_S32 s32RC3;
    AR_S32 s32GC0;
    AR_S32 s32GC1;
    AR_S32 s32GC2;
    AR_S32 s32GC3;
    AR_S32 s32BC0;
    AR_S32 s32BC1;
    AR_S32 s32BC2;
    AR_S32 s32BC3;
}AR_MPI_IFC_MATRIX_S;

typedef struct
{
    AR_S64 s64SumR;
    AR_S64 s64SumG;
    AR_S64 s64SumB;
    AR_DOUBLE dAvgR;
    AR_DOUBLE dAvgG;
    AR_DOUBLE dAvgB;
    AR_DOUBLE dSSumR;
    AR_DOUBLE dSSumG;
    AR_DOUBLE dSSumB;
} AR_MPI_IFC_RESULT_S;

/**
* @brief  图像转换标准枚举类型
* @note   支持BT601 FULL，BT601 VIDEO，BT709 FULL，BT709 VIDEO
*/
typedef enum
{
    AR_MPI_IFC_BT601_FULL = 0,
    AR_MPI_IFC_BT601_VIDEO = 1,
    AR_MPI_IFC_BT709_FULL = 2,
    AR_MPI_IFC_BT709_VIDEO = 3,
    AR_MPI_IFC_BT_NULL
}AR_MPI_IFC_STAN_E;

/**
* @brief  图片转换接口
* @param  pstImgIn YUV 输入图像参数，pstImgOut RGB 输出图像参数，enIfcStandard 转换标准
* @retval retval > 0 成功，其他加载失败
* @note   支持8bit RGB转换
* @note	  支持YUV444P/YUV444SP/YUV422P/YUV422SP/YUV420P/YUV420SP/YUVI420/YV12/NV12/NV21 -> RGB/RGB_INTLV转换
* @note   支持RGB -> YUV444P/GRAY转换
*/
AR_S32 AR_MPI_IFC_CvtColor(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut, AR_MPI_IFC_STAN_E enIfcStandard);
AR_S32 AR_MPI_IFC_CvtColor_Ext(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut, AR_MPI_IFC_RESULT_S * pstResult, AR_MPI_IFC_STAN_E enIfcStandard);
AR_S32 AR_MPI_IFC_SetFrequency(AR_U32 u32FreqMHz);

/**
* @brief  将IFC挂起，power off
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  在挂起之前请调用者自行保证没有正在运行的任务
*/
AR_S32 AR_MPI_IFC_Suspend(void);

/**
* @brief  将IFC唤醒，power on
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  唤醒后，原来由调用者自行设置的频率需要调用者负责恢复
*/
AR_S32 AR_MPI_IFC_Resume(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif//__MPI_IFC_API_H__

