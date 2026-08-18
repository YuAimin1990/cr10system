#ifndef __HAL_VPS_GDC_H__
#define __HAL_VPS_GDC_H__

#include "ar_gdc.h"
#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */



#define AR_VPS_GDC_DEV "/dev/argdc"

#define GDC_LUT_MATRIX_ROW 65
#define GDC_LUT_MATRIX_COL 65

#define GDC_LUT_AXIS_STRIDE  (((GDC_LUT_MATRIX_COL + 3) /4)*4)
#define VPS_GDC_ION_SIZE_MAX  (GDC_LUT_AXIS_STRIDE*GDC_LUT_MATRIX_ROW*4 +1024)

/**
* @brief 用户态实现图像:旋转,水平/垂直翻转,LDC功能
* @param pstGdcParams GDC参数
* @retval 0 成功 其他 失败
* @note  输入输出地址为有效物理地址
**/
AR_S32 ar_hal_vps_gdc_general(AR_GDC_PARAMS_S *pstGdcParams);
AR_S32 ar_hal_gdc_set_frequency(AR_U32 u32FreqMHz);
AR_S32 ar_hal_gdc_get_time(AR_FLOAT *fpTime);

/**
* @brief  将GDC挂起，power off
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  在挂起之前请调用者自行保证没有正在运行的任务
*/
AR_S32 ar_hal_gdc_suspend(void);

/**
* @brief  将GDC唤醒，power on
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  唤醒后，原来由调用者自行设置的频率需要调用者负责恢复
*/
AR_S32 ar_hal_gdc_resume(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__HAL_EIS_H__

