#ifndef __MPI_GDC_API_H__
#define __MPI_GDC_API_H__

#include "hal_npu_types.h"
#include "hal_type.h"
#include "hal_vps_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define AR_GDC_OP_NUM_MAX 8

#define CHECK_GDC_WH_VALID(num)  			(0 < (num) && (num) <= 3840)

/* 5 action params*/
typedef enum
{
    AR_GDC_ROTATE_E = 1,
    AR_GDC_FLIP_E = 2,
    AR_GDC_MIRROR_E = 3,
    AR_GDC_LDC_E = 4,
    AR_GDC_EIS_E = 5,
    AR_GDC_USER_DEF_E = 6,
    AR_GDC_BUTT_E
} AR_GDC_OP_TYPE_E;

typedef struct
{
    AR_FLOAT fAngle; // 0 - 360; ONLY SUPPORT 0, 90, 180, 270 now.
} AR_GDC_ROTATE_ATTR_S;
typedef struct
{
    AR_BOOL bEnable;
} AR_GDC_FLIP_ATTR_S;
typedef struct
{
    AR_BOOL bEnable;
} AR_GDC_MIRROR_ATTR_S;
typedef struct
{
  AR_FLOAT k[9];
  AR_FLOAT ldc_k0 ;
  AR_FLOAT ldc_k1 ;
  AR_FLOAT ldc_k2 ;
} AR_GDC_LDC_ATTR_S;

typedef struct
{
  AR_U32 u32Reserved; //TO DO.
} AR_GDC_EIS_ATTR_S;

typedef struct
{
    AR_U64 u64LutVirtAddr;
	AR_U32 u32LutLen;
} AR_GDC_USER_ATTR_S;

typedef struct
{
    AR_GDC_OP_TYPE_E enType;
    void * pAttr; //Points to attributes.
} AR_GDC_TRANS_PARAM_S;

typedef struct
{
	AR_GDC_BUFFER_S stInBuffer;
	AR_GDC_BUFFER_S stOutBuffer;
	AR_GDC_TRANS_PARAM_S stParams[AR_GDC_OP_NUM_MAX];
} AR_GDC_TRANSFORM_S;

typedef struct
{
	AR_GDC_TRANS_PARAM_S stParams[AR_GDC_OP_NUM_MAX];
} AR_GDC_TRANSFORM_ARRAY_S;

/**
* @brief 多操作合一接口，内部会按照0-AR_GDC_OP_NUM_MAX的顺序将操作合并，充分节省硬件资源
* @param
* @retval 0 成功 其他 失败
* @note  输入输出地址为有效物理地址
**/
AR_S32 AR_MPI_GDC_Transform(AR_GDC_TRANSFORM_S * pstParam);

/**
* @brief 多操作合一接口，内部会按照0-AR_GDC_OP_NUM_MAX的顺序将操作合并，充分节省硬件资源
* @param
* @retval 0 成功 其他 失败
* @note  输入输出地址为有效物理地址。为了在调用ifc/scaler时，不用做参数转换。
**/
AR_S32 AR_MPI_GDC_Transform_General(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut, AR_GDC_TRANSFORM_ARRAY_S * pstParam);
AR_S32 AR_MPI_GDC_SetFrequency(AR_U32 u32FreqMHz);
AR_S32 AR_MPI_GDC_GetTime(AR_FLOAT *fpTime);

/**
* @brief  将GDC挂起，power off
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  在挂起之前请调用者自行保证没有正在运行的任务
*/
AR_S32 AR_MPI_GDC_Suspend(void);

/**
* @brief  将GDC唤醒，power on
* @param  NULL
* @retval 0 成功，非零值 失败.
* @note  唤醒后，原来由调用者自行设置的频率需要调用者负责恢复
*/
AR_S32 AR_MPI_GDC_Resume(void);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
