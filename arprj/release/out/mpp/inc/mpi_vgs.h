/**
 * \file
 * \brief 描述视频图形处理系统.
 */

#ifndef __MPI_VGS_H__
#define __MPI_VGS_H__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "ar_comm_vgs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /*__cplusplus*/

/**
\addtogroup MPI_VGS
 * @brief VGS对输入图像进行处理，如添加COVER，打OSD, 画线，旋转等处理
 * @{
*/

/**
* @brief      创建vgs job
* @param[in]  phHandle         VGS句柄
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_BeginJob(VGS_HANDLE *phHandle);

/**
* @brief      结束vgs job，此接口为同步接口，vgs完成所有task后返回
* @param[in]  phHandle         VGS句柄
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_EndJob(VGS_HANDLE hHandle);

/**
* @brief      取消vgs job，vgs中所有task将取消操作
* @param[in]  phHandle         VGS句柄
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_CancelJob(VGS_HANDLE hHandle);

/**
* @brief      添加缩放task到指定job中
* @note       暂不支持enScaleCoefMode
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          缩放task属性
* @param[in]  enScaleCoefMode  缩放模式
* @retval 0   成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddScaleTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, VGS_SCLCOEF_MODE_E enScaleCoefMode);

/**
* @brief      添加画线task到指定job中
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          画线task属性
* @param[in]  pstVgsDrawLine   画线参数
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddDrawLineTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_DRAW_LINE_S *pstVgsDrawLine);

/**
* @brief      添加cover task到指定job中
* @note       暂不支持实心四边形，不支持凸四边形
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          cover task属性
* @param[in]  pstVgsAddCover   cover参数
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddCoverTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_COVER_S *pstVgsAddCover);

/**
* @brief      添加osd task到指定job中
* @note       仅gray1支持u32BgAlpha, u32FgAlpha
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          osd task属性
* @param[in]  pstVgsAddOsd   cover参数
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddOsdTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_OSD_S *pstVgsAddOsd);

/**
* @brief      添加画线task数组到指定job中
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          画线task属性
* @param[in]  astVgsDrawLine   画线参数
* @param[in]  u32ArraySize     数组大小
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddDrawLineTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_DRAW_LINE_S astVgsDrawLine[], AR_U32 u32ArraySize);


/**
* @brief      添加cover task到指定job中
* @note       暂不支持实心四边形，不支持凸四边形
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          cover task属性
* @param[in]  astVgsAddCover   cover参数
* @param[in]  u32ArraySize     数组大小
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddCoverTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_COVER_S astVgsAddCover[], AR_U32 u32ArraySize);

/**
* @brief      添加osd task到指定job中
* @note       仅gray1支持u32BgAlpha, u32FgAlpha
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask     osd task属性
* @param[in]  astVgsAddOsd     osd参数
* @param[in]  u32ArraySize     数组大小
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddOsdTaskArray(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, const VGS_ADD_OSD_S astVgsAddOsd[], AR_U32 u32ArraySize);

/**
* @brief      添加旋转task到指定job中
* @param[in]  phHandle         VGS句柄
* @param[in]  pstTask          osd task属性
* @param[in]  enRotationAngle  旋转角度
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_AddRotationTask(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, ROTATION_E enRotationAngle);

/**
* @brief      使VGS模块进入睡眠状态
* @attention  新增接口
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_Suspend(void);

/**
* @brief      唤醒VGS模块
* @attention  新增接口
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VGS_Resume(void);

AR_S32 AR_MPI_VGS_AddRotationTaskExt(VGS_HANDLE hHandle, const VGS_TASK_ATTR_S *pstTask, VGS_ADD_ROTATION_S *pstRotation);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /*__cplusplus*/

#endif /*end of __MPI_VGS_H__*/

