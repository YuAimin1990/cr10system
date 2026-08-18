/**
 * \file
 * \brief 描述视频输出相关接口.
 */

#ifndef __MPI_VO_H__
#define __MPI_VO_H__

#include "ar_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
\addtogroup MPI_VO
 * @brief 视频输出模块主动从内存相应位置读取数据，并通
过相应的显示设备按照时序在指定接口输出。
 * @{
*/

/* Device Relative Settings */

/**
* @brief  配置视频输出设备属性
* @param[in]  VoDev          设备号
* @param[in]  pstPubAttr     属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetPubAttr(VO_DEV VoDev, const VO_PUB_ATTR_S *pstPubAttr);

/**
* @brief  获取视频输出设备属性
* @param[in]  VoDev          设备号
* @param[out] pstPubAttr     属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetPubAttr(VO_DEV VoDev, VO_PUB_ATTR_S *pstPubAttr);

/**
* @brief  配置Lowdelay属性
* @param[in] VoDev            设备号
* @param[in] pstLowdelayAttr  属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetLowdelayAttr(VO_DEV VoDev, const VO_LOWDELAY_ATTR_S *pstLowdelayAttr);

/**
* @brief  获取Lowdelay属性
* @param[in]  VoDev            设备号
* @param[out] pstLowdelayAttr  属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetLowdelayAttr(VO_DEV VoDev, VO_LOWDELAY_ATTR_S *pstLowdelayAttr);

/**
* @brief  配置及使能VO订阅中断信息属性(动态接口)
* @param[in]  VoDev          设备号
* @param[out] pstSubAttr     属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SubscribeEnable(VO_DEV VoDev, const VO_SUBSCRIBE_ATTR_S *pstSubAttr);

/**
* @brief 更新VO订阅中断信息类型(动态接口)
* @param[in]  VoDev              设备号
* @param[out] u32SubscribeType   更新订阅中断信息类型
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SubscribeUpdate(VO_DEV VoDev, const AR_U32 u32SubscribeType);

/**
* @brief  禁用订阅中断信息
* @param[in]  VoDev          设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SubscribeDisable(VO_DEV VoDev);

/**
* @brief  使能视频输出设备
* @param[in]  VoDev          设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_Enable (VO_DEV VoDev);

/**
* @brief  禁用视频输出设备
* @param[in]  VoDev          设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_Disable(VO_DEV VoDev);

/**
* @brief  关闭输出设备所有fd
* @param[in]  VoDev          设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_CloseFd(AR_VOID);

/**
* @brief  配置接口时序
* @param[in]  VoDev          设备号
* @param[in]  pstUserInfo    时序信息指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetUserIntfSyncInfo (VO_DEV VoDev, VO_USER_INTFSYNC_INFO_S *pstUserInfo);


/* Video Relative Settings */
/**
* @brief  配置视频层属性
* @param[in]  VoDev          设备号
* @param[in]  pstLayerAttr   属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetVideoLayerAttr(VO_LAYER VoLayer, const VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
* @brief  获取视频层属性
* @param[in]  VoDev          设备号
* @param[out] pstLayerAttr   属性指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetVideoLayerAttr(VO_LAYER VoLayer, VO_VIDEO_LAYER_ATTR_S *pstLayerAttr);

/**
* @brief  使能视频层
* @param[in]  VoLayer          视频层号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_EnableVideoLayer (VO_LAYER VoLayer);

/**
* @brief  禁用视频层
* @param[in]  VoLayer          视频层号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_DisableVideoLayer(VO_LAYER VoLayer);

/**
* @brief  绑定视频层到输出设备
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoDev            设备号
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_BindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

/**
* @brief  解除视频层绑定关系
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoDev            设备号
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_UnBindVideoLayer(VO_LAYER VoLayer, VO_DEV VoDev);

/**
* @brief  设置视频层优先级
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  u32Priority      优先级
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetVideoLayerPriority(VO_LAYER VoLayer, AR_U32 u32Priority);

/**
* @brief  获取视频层优先级
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out] pu32Priority     优先级指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetVideoLayerPriority(VO_LAYER VoLayer, AR_U32 *pu32Priority);

/**
* @brief  设置视频层图像效果配置
* @param[in]  VoLayer          视频层号
* @param[in]  pstVideoCSC      色域信息指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetVideoLayerCSC(VO_LAYER VoLayer, const VO_CSC_S *pstVideoCSC);

/**
* @brief  获取视频层图像效果配置
* @param[in]  VoLayer          视频层号
* @param[out]  pstVideoCSC      色域信息指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetVideoLayerCSC(VO_LAYER VoLayer, VO_CSC_S *pstVideoCSC);

/**
* @brief  设置视频层分区模式
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  enPartMode       分区模式
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E enPartMode);

/**
* @brief  获取视频层分区模式
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out]  penPartMode      分区模式指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetVideoLayerPartitionMode(VO_LAYER VoLayer, VO_PART_MODE_E *penPartMode);

/**
* @brief  开始批量配置视频层
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_BatchBegin(VO_LAYER VoLayer);

/**
* @brief  结束批量配置视频层
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_BatchEnd  (VO_LAYER VoLayer);

/**
* @brief  设置视频层边框参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  pstLayerBoundary 边框配置指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetVideoLayerBoundary(VO_LAYER VoLayer, const VO_LAYER_BOUNDARY_S *pstLayerBoundary);

/**
* @brief  获取视频层边框参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out]  pstLayerBoundary 边框配置指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetVideoLayerBoundary(VO_LAYER VoLayer, VO_LAYER_BOUNDARY_S *pstLayerBoundary);

/**
* @brief  设置视频层参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  pstLayerParam    视频层参数指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetVideoLayerParam(VO_LAYER VoLayer,  const VO_LAYER_PARAM_S *pstLayerParam);

/**
* @brief  获取视频层参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out] pstLayerParam    视频层参数指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetVideoLayerParam(VO_LAYER VoLayer, VO_LAYER_PARAM_S *pstLayerParam);


/* Display relative operations */
/**
* @brief  设置播放容忍时间
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  u32Toleration    容忍时间
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetPlayToleration(VO_LAYER VoLayer, AR_U32 u32Toleration);

/**
* @brief  获取播放容忍时间
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out] pu32Toleration   容忍时间指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetPlayToleration(VO_LAYER VoLayer, AR_U32 *pu32Toleration);

/**
* @brief  从视频层获取图像数据
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[out] pstVFrame        图像数据指针
* @param[in]  s32MilliSec      超时时间
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetScreenFrame(VO_LAYER VoLayer, VIDEO_FRAME_INFO_S *pstVFrame, AR_S32 s32MilliSec);

/**
* @brief  释放从视频层获取的图像数据
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  pstVFrame        图像数据指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_ReleaseScreenFrame(VO_LAYER VoLayer, const VIDEO_FRAME_INFO_S *pstVFrame);

/**
* @brief  设置视频层缓冲深度
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  u32BufLen        缓冲深度
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetDisplayBufLen(VO_LAYER VoLayer, AR_U32 u32BufLen);

/**
* @brief  获取视频层缓冲深度
* @note   暂不支持
* @param[in] VoLayer          视频层号
* @param[out] pu32BufLen       缓冲深度指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetDisplayBufLen(VO_LAYER VoLayer, AR_U32 *pu32BufLen);

/* Channel Relative Operations */
/**
* @brief  设置通道属性
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstChnAttr       通道属性
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_ATTR_S *pstChnAttr);

/**
* @brief  获取通道属性
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstChnAttr       通道属性
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetChnAttr(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_ATTR_S *pstChnAttr);

/**
* @brief  使能通道
* @param[in] VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_EnableChn (VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  禁用通道
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_DisableChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  设置通道参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstChnParam      通道参数指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_PARAM_S *pstChnParam);

/**
* @brief  获取通道参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstChnParam      通道参数指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnParam(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_PARAM_S *pstChnParam);

/**
* @brief  设置通道显示位置
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstDispPos       通道位置指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, const POINT_S *pstDispPos);

/**
* @brief  获取通道显示位置
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstDispPos       通道位置指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetChnDisplayPosition(VO_LAYER VoLayer, VO_CHN VoChn, POINT_S *pstDispPos);

/**
* @brief  设置通道显示帧率
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] s32ChnFrmRate    显示帧率
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, AR_S32 s32ChnFrmRate);

/**
* @brief  获取通道显示帧率
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] ps32ChnFrmRate   显示帧率指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetChnFrameRate(VO_LAYER VoLayer, VO_CHN VoChn, AR_S32 *ps32ChnFrmRate);

/**
* @brief  获取通道图像数据
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstFrame         图像数据指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstFrame, AR_S32 s32MilliSec);

/**
* @brief  释放通道图像数据
* @param  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstFrame         图像数据指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_ReleaseChnFrame(VO_LAYER VoLayer, VO_CHN VoChn, const VIDEO_FRAME_INFO_S *pstFrame);

/**
* @brief  通道暂停
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_PauseChn (VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  通道恢复
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_ResumeChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  通道步进
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
  @note   暂不支持
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_StepChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  通道刷新
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_RefreshChn( VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  通道显示
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_ShowChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  通道隐藏
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_HideChn(VO_LAYER VoLayer, VO_CHN VoChn);

/**
* @brief  配置局部放大通道图像参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstZoomAttr      放大属性
 * @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, const VO_ZOOM_ATTR_S *pstZoomAttr);

/**
* @brief  获取局部放大通道图像参数
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstZoomAttr      放大属性
 * @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetZoomInWindow(VO_LAYER VoLayer, VO_CHN VoChn, VO_ZOOM_ATTR_S *pstZoomAttr);

/**
* @brief  获取通道时间戳
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pu64ChnPTS       时间戳指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnPTS(VO_LAYER VoLayer, VO_CHN VoChn, AR_U64 *pu64ChnPTS);

/**
* @brief  查询通道状态
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstStatus        状态指针
 * @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_QueryChnStatus(VO_LAYER VoLayer, VO_CHN VoChn, VO_QUERY_STATUS_S *pstStatus);

/**
* @brief  推送图像数据到通道
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstVFrame        图像数据指针
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SendFrame(VO_LAYER VoLayer, VO_CHN VoChn, VIDEO_FRAME_INFO_S *pstVFrame, AR_S32 s32MilliSec);

/**
* @brief  清除通道图像数据
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  bClrAll          全清标志
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_ClearChnBuf(VO_LAYER VoLayer, VO_CHN VoChn, AR_BOOL bClrAll);

/**
* @brief  设置通道边框
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstBorder        边框属性
 * @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, const VO_BORDER_S *pstBorder);

/**
* @brief  获取通道边框
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstBorder        边框属性
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnBorder(VO_LAYER VoLayer, VO_CHN VoChn, VO_BORDER_S *pstBorder);

/**
* @brief  设置通道边框
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstChnBoundary   边框属性
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, const VO_CHN_BOUNDARY_S *pstChnBoundary);

/**
* @brief  获取通道边框
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] pstChnBoundary   边框属性
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetChnBoundary(VO_LAYER VoLayer, VO_CHN VoChn, VO_CHN_BOUNDARY_S *pstChnBoundary);

/**
* @brief  设置接收缓冲阈值
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  u32Threshold     缓冲阈值
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, AR_U32 u32Threshold);

/**
* @brief  获取接收缓冲阈值
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] u32Threshold     缓冲阈值指针
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnRecvThreshold(VO_LAYER VoLayer, VO_CHN VoChn, AR_U32 *pu32Threshold);

/**
* @brief  设置通道旋转信息
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] enRotation       旋转信息
 * @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_SetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn,  ROTATION_E  enRotation);

/**
* @brief  获取通道旋转信息
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[out] enRotation       旋转信息
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnRotation(VO_LAYER VoLayer, VO_CHN VoChn, ROTATION_E *penRotation);

/**
* @brief  获取通道区域亮度
* @note   暂不支持
* @param[in]  VoLayer          视频层号
* @param[in]  VoChn            通道号
* @param[in]  pstRegionInfo    区域信息
* @param[out] pu64LumaData     亮度数据
* @param[in]  s32MilliSec      等待时间
* @retval 0 成功 , 其它 失败.
* @todo
*/
AR_S32 AR_MPI_VO_GetChnRegionLuma(VO_LAYER VoLayer, VO_CHN VoChn, VO_REGION_INFO_S *pstRegionInfo,
                                                AR_U64 *pu64LumaData, AR_S32 s32MilliSec);

/* WBC(Write Back Control) Relative Settings */

/**
* @brief  设置设备回写源
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[in]  pstWBCSource      回写源
 * @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetWBCSource(VO_WBC VoWBC, const VO_WBC_SOURCE_S *pstWBCSource);

/**
* @brief  获取设备回写源
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[out] pstWBCSource      回写源
 * @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetWBCSource(VO_WBC VoWBC, VO_WBC_SOURCE_S *pstWBCSources);

/**
* @brief  设置回写源属性
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[in]  pstWBCAttr        回写源属性
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetWBCAttr(VO_WBC VoWBC, const VO_WBC_ATTR_S *pstWBCAttr);

/**
* @brief  获取回写源属性
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[out] pstWBCAttr        回写源属性
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetWBCAttr(VO_WBC VoWBC, VO_WBC_ATTR_S *pstWBCAttr);

/**
* @brief  使能回写源
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_EnableWBC(VO_WBC VoWBC);

/**
* @brief  禁用回写源
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_DisableWBC(VO_WBC VoWBC);

/**
* @brief  设置回写模式
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[in]  enWBCMode         回写模式
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E enWBCMode);

/**
* @brief  获取回写模式
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[out] enWBCMode         回写模式
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetWBCMode(VO_WBC VoWBC, VO_WBC_MODE_E *penWBCMode);

/**
* @brief  设置回写深度
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[in]  u32Depth          回写深度
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetWBCDepth(VO_WBC VoWBC, AR_U32 u32Depth);

/**
* @brief  获取回写深度
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[out] pu32Depth         回写深度
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetWBCDepth(VO_WBC VoWBC, AR_U32 *pu32Depth);

/**
* @brief  获取回写图像数据
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[out] pstVFrame         图像数据
* @param[in]  s32MilliSec       等待时间
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetWBCFrame(VO_WBC VoWBC, VIDEO_FRAME_INFO_S *pstVFrame, AR_S32 s32MilliSec);

/**
* @brief  释放回写图像数据
* @note   暂不支持
* @param[in]  VoWBC             视频层号
* @param[in]  pstVFrame         图像数据
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_ReleaseWBCFrame(VO_WBC VoWBC, const VIDEO_FRAME_INFO_S *pstVFrame);

/* Graphic Relative Settings */
/**
* @brief  绑定图像层
* @note   暂不支持
* @param[in]  GraphicLayer      图像层号
* @param[in]  VoDev             输出设备
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_BindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);

/**
* @brief  解绑图像层
* @note   暂不支持
* @param[in]  GraphicLayer      图像层号
* @param[in]  VoDev             输出设备
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_UnBindGraphicLayer(GRAPHIC_LAYER GraphicLayer, VO_DEV VoDev);

/**
* @brief  设置图像层图像效果
* @param[in]  GraphicLayer      图像层号
* @param[in]  pstCSC            图像效果指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, const VO_CSC_S *pstCSC);

/**
* @brief  获取图像层图像效果
* @param[in]  GraphicLayer      图像层号
* @param[out] pstCSC            图像效果指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetGraphicLayerCSC(GRAPHIC_LAYER GraphicLayer, VO_CSC_S *pstCSC);

/**
* @brief  设置在用户时序的设备帧率
* @param[in]  VoDev             输出设备
* @param[in]  u32FrameRate      帧率
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_SetDevFrameRate(VO_DEV VoDev, AR_FLOAT u32FrameRate);

/**
* @brief  获取在用户时序的设备帧率
* @param[in]  VoDev             输出设备
* @param[out] u32FrameRate      帧率
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetDevFrameRate(VO_DEV VoDev, AR_U32 *pu32FrameRate);

/* Module Parameter Settings */
/**
* @brief  设置模块参数
* @note   暂不支持
* @param[in]  pstModParam      模块参数指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetModParam(const VO_MOD_PARAM_S *pstModParam);

/**
* @brief  获取参数
* @note   暂不支持
* @param[out]  pstModParam      模块参数指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetModParam(VO_MOD_PARAM_S *pstModParam);

/**
* @brief  设置设备中断门限
* @note   暂不支持
* @param[in]  VoDev            输出设备号
* @param[in]  u32Vtth          门限阈值
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_SetVtth(VO_DEV VoDev, AR_U32 u32Vtth);

/**
* @brief  获取设备中断门限
* @note   暂不支持
* @param[in]  VoDev            输出设备号
* @param[out] pu32Vtth         门限阈值指针
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_GetVtth(VO_DEV VoDev, AR_U32* pu32Vtth);

/**
* @brief  设置DSI属性
* @attention  新增接口
* @param[in]  VoDev            输出设备号
* @param[in]  pstAttr          属性配置
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_Dsi_SetAttr(VO_DEV VoDev, VO_DSI_ATTR_S* pstAttr);

/**
* @brief      下发DSI命令
* @attention  新增接口
* @param[in]  VoDev            输出设备号
* @param[in]  pstCmd           命令内容
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_Dsi_Cmd(VO_DEV VoDev, VO_DSI_CMD_S* pstCmd);

/**
* @brief      使能DSI
* @attention  新增接口
* @param[in]  VoDev            输出设备号
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_Dsi_Enable(VO_DEV VoDev);

/**
* @brief      读取内容，常用于读取屏幕id
* @attention  新增接口
* @param[in]  VoDev            输出设备号
* @param[out] pstRead          读取内容
* @retval 0 成功 , 其它 失败.
* @deprecated
*/
AR_S32 AR_MPI_VO_Dsi_Read(VO_DEV VoDev, VO_DSI_READ_S* pstRead);

/**
* @brief      向dev发送suspend命令
* @param[in]  VoDev            输出设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_Suspend(VO_DEV VoDev);

/**
* @brief      向dev发送resume命令
* @param[in]  VoDev            输出设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_Resume(VO_DEV VoDev);

/**
* @brief      复位抖动直方图统计
* @param[in]  VoDev           设备号
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_ResetSkewingHistogram(VO_DEV VoDev);

/**
* @brief  获取抖动直方图统计
* @param[in]  VoDev           设备号
* @param[out] pstSkewingData  抖动直方图统计数据指针
* @param[in]  count           抖动直方图分组数量, 单位ms，从0ms开始
* @retval 0 成功 , 其它 失败.
*/
AR_S32 AR_MPI_VO_GetSkewingHistogram(VO_DEV VoDev, AR_U64 *pstSkewingData, int count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/** @}*/

#endif /*__MPI_VO_H__ */

