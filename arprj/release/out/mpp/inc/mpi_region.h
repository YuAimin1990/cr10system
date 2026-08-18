/**
 * \file
 * \brief 描述视区域管理相关接口.
 */

#ifndef __MPI_REGION_H__
#define __MPI_REGION_H__

#include "ar_comm_region.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

/**
\addtogroup MPI_REGION
 * @brief 区域管理可以实现区域的创建，并叠加到视频中或对视频进行遮挡。
 * @{
*/

/**
* @brief      创建区域
* @param[in]  Handle           区域句柄号
* @param[in]  pstRegion        模块参数指针
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_Create(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

/**
* @brief      删除区域
* @param[in]  Handle           区域句柄号
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_Destroy(RGN_HANDLE Handle);

/**
* @brief      设置区域属性
* @param[in]  Handle           区域句柄号
* @param[in]  pstRegion        区域属性指针
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_GetAttr(RGN_HANDLE Handle, RGN_ATTR_S *pstRegion);

/**
* @brief  获取区域属性
* @param[in]  Handle           区域句柄号
* @param[in]  pstRegion        区域属性指针
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_SetAttr(RGN_HANDLE Handle, const RGN_ATTR_S *pstRegion);

/**
* @brief      对区域进行位图填充
* @note       暂不支持
* @param[in]  Handle           区域句柄号
* @param[in]  pstBitmap        位图指针
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_SetBitMap(RGN_HANDLE Handle, const BITMAP_S *pstBitmap);

/**
* @brief      将区域绑定到指定通道上
* @param[in]  Handle           区域句柄号
* @param[in]  pstChn           指定通道
* @param[in]  pstChnAttr       区域在通道上的显示属性
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_AttachToChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);

/**
* @brief      将区域从指定通道上解绑
* @param[in]  Handle           区域句柄号
* @param[in]  pstChn           指定通道
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_DetachFromChn(RGN_HANDLE Handle, const MPP_CHN_S *pstChn);

/**
* @brief      设置区域在通道上的显示属性
* @param[in]  Handle           区域句柄号
* @param[in]  pstChn           指定通道
* @param[in]  pstChnAttr       区域在通道上的显示属性
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_SetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, const RGN_CHN_ATTR_S *pstChnAttr);

/**
* @brief      获取区域在通道上的显示属性
* @param[in]  Handle           区域句柄号
* @param[in]  pstChn           指定通道
* @param[out] pstChnAttr       区域在通道上的显示属性
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_GetDisplayAttr(RGN_HANDLE Handle, const MPP_CHN_S *pstChn, RGN_CHN_ATTR_S *pstChnAttr);

/**
* @brief      获取区域画布信息
* @param[in]  Handle           区域句柄号
* @param[out] pstCanvasInfo    画布信息指针
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_GetCanvasInfo(RGN_HANDLE Handle, RGN_CANVAS_INFO_S *pstCanvasInfo);

/**
* @brief      更新区域画布
* @param[in]  Handle           区域句柄号
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_UpdateCanvas(RGN_HANDLE Handle);

/**
* @brief      开始区域批量配置
* @note       暂不支持
* @param[in]  pu32Group           区域分组号
* @param[in]  u32Num              区域个数
* @param[in]  handle              区域句柄数组
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_BatchBegin(RGN_HANDLEGROUP *pu32Group,AR_U32 u32Num,const RGN_HANDLE handle[]);

/**
* @brief      结束区域批量配置
* @note       暂不支持
* @param[in]  pu32Group           区域分组号
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_BatchEnd(RGN_HANDLEGROUP u32Group);

/**
* @brief      获取区域模块的句柄
* @note       暂不支持
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_GetFd(AR_VOID);


/**
* @brief      rgn回调注册函数
* @attention  新增接口
* @param[in]  pstCb            回调钩子
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_RegistCb(RGN_REGISTER_CB_S *pstCb);

/**
* @brief      rgn回调销毁函数
* @attention  新增接口
* @param[in]  enModId          模块号
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_UnRegistCb(MOD_ID_E enModId);

/**
* @brief      设置chn参数接口，用于设置该chn是否需要带copy属性
* @attention  新增接口
* @param[in]  pstChn          chn编号
* @param[in]  pstChnAttr      chn属性
* @retval     0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_RGN_Set_Chn_Para(const MPP_CHN_S *pstChn, const RGN_CHN_PARA_S *pstChnAttr);


/**
* @brief      使RGN模块进入睡眠状态
* @attention  新增接口
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_Suspend(void);

/**
* @brief      唤醒RGN模块
* @attention  新增接口
* @retval     0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_RGN_Resume(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

/** @}*/

#endif /* End of #ifndef __MPI_REGION_H__ */

