/**
 * \file
 * \brief 描述视频解码相关的数据结构和接口.
 */

#ifndef  __MPI_VDEC_H__
#define  __MPI_VDEC_H__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "ar_comm_vb.h"
#include "ar_comm_vdec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
/********************************API Definition********************************/
/**
\addtogroup MPI_VDEC
 * @brief 本模块提供驱动视频解码硬件工作的 MPI 接口，实现视频解码功能。
 * @{
*/

/**
\brief VDEC设备初始化，初始化基本的数据结构。
\attention 新添加接口
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_Init(AR_VOID);

/**
\brief VDEC设备去初始化去初始化基本的数据结构。
\attention 新添加接口
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_DeInit(AR_VOID);

/**
\brief 创建VDEC通道。
\attention VDEC_CHN_ATTR_S部分参数暂未实现或使用, 详见ar_comm_vdec.h
\param[in] VdChn        :   VDEC通道号，创建之后不能重复。
\param[in] pstAttr      :   VDEC通道属性指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see test_mpi_vdec/test_vdec.c + test_mpi_vdec_client/mian.c \n
N/A
*/
AR_S32 AR_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);

/**
\brief 销毁VDEC通道，创建后才能销毁。
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_DestroyChn(VDEC_CHN VdChn);

/**
\brief 获取VDEC通道的属性，创建通道后才能获取。
\attention VDEC_CHN_ATTR_S部分参数暂未实现或使用
\param[in] VdChn        :   VDEC通道号。
\param[in] pstAttr      :   VDEC通道属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr);

/**
\brief 设置VDEC通道的属性，创建通道后才能设置，建议先获取再设置。
\attention VDEC_CHN_ATTR_S部分参数暂未实现或使用
\param[in] VdChn        :   VDEC通道号。
\param[in] pstAttr      :   VDEC通道属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_SetChnAttr(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr);

/**
\brief VDEC通道启动接收数据，创建后才能启动。
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn);

/**
\brief VDEC通道停止接收数据，启动后才能停止。
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn);

/**
\brief 查询VDEC通道的状态，启动后才能查询。
\attention VDEC_CHN_STATUS_S 部分参数暂未实现或使用
\param[in] VdChn        :   VDEC通道号。
\param[in] pstStatus    :   VDEC通道状态结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_QueryStatus(VDEC_CHN VdChn, VDEC_CHN_STATUS_S *pstStatus);

/**
\brief 获取VDEC通道对应的文件描述符。
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetFd(VDEC_CHN VdChn);

/**
\brief 关闭VDEC通道对应的文件描述符。
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_CloseFd(VDEC_CHN VdChn);

/**
\brief 重置VDEC通道，停止通道后才能重置。
\attention 新增加hard参数，用于软复位或者硬复位解码器驱动
\param[in] hard         :   0软复位，只清VPU解码framebuffer；1硬复位，关闭VPU解码通道
\param[in] VdChn        :   VDEC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_ResetChn(VDEC_CHN VdChn, AR_BOOL hard);

/**
\brief 设置VDEC通道的参数，创建通道后才能设置，建议先获取再设置。
\attention VDEC_CHN_PARAM_S 部分参数暂未实现或使用
\param[in] VdChn        :   VDEC通道号。
\param[in] pstParam     :   VDEC通道参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, const VDEC_CHN_PARAM_S* pstParam);

/**
\brief 获取VDEC通道的参数，创建通道后才能获取。
\attention VDEC_CHN_PARAM_S 部分参数暂未实现或使用
\param[in] VdChn        :   VDEC通道号。
\param[in] pstParam     :   VDEC通道参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S* pstParam);

AR_S32 AR_MPI_VDEC_SetProtocolParam(VDEC_CHN VdChn, const VDEC_PRTCL_PARAM_S *pstParam)__attribute__((unavailable("todo")));
AR_S32 AR_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn,VDEC_PRTCL_PARAM_S *pstParam)__attribute__((unavailable("todo")));

/**
\brief VDEC通道发送一段或者一帧视频流数据进行解码。
\param[in] VdChn        :   VDEC通道号。
\param[in] pstStream    :   视频流信息结构体指针。
\param[in] s32MilliSec  :   等待时间，-1代表阻塞。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_SendStream(VDEC_CHN VdChn, const VDEC_STREAM_S *pstStream, AR_S32 s32MilliSec);

/**
\brief 获取VDEC通道的解码帧。
\attention VIDEO_FRAME_INFO_S 部分参数暂未实现或使用
\param[in] VdChn          :   VDEC通道号。
\param[in] pstFrameInfo   :   VDEC帧结构体指针。
\param[in] s32MilliSec    :   获取帧等待时间，-1代表阻塞。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetFrame(VDEC_CHN VdChn, VIDEO_FRAME_INFO_S *pstFrameInfo, AR_S32 s32MilliSec);

/**
\brief 释放VDEC通道的解码帧。
\param[in] VdChn          :   VDEC通道号。
\param[in] pstFrameInfo   :   VDEC帧结构体指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_ReleaseFrame(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

/**
\brief 获取VDEC通道的userdata。
\param[in] VdChn          :   VDEC通道号。
\param[in] pstUserData    :   VDEC userdata结构体指针。
\param[in] s32MilliSec    :   获取帧等待时间，-1代表阻塞。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetUserData(VDEC_CHN VdChn, VDEC_USERDATA_S *pstUserData, AR_S32 s32MilliSec);

/**
\brief 释放VDEC通道的userdata。
\param[in] VdChn          :   VDEC通道号。
\param[in] pstUserData    :   VDEC userdata结构体指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_ReleaseUserData(VDEC_CHN VdChn, const VDEC_USERDATA_S *pstUserData);

AR_S32 AR_MPI_VDEC_SetUserPic(VDEC_CHN VdChn, const VIDEO_FRAME_INFO_S *pstUsrPic)__attribute__((unavailable("todo")));
AR_S32 AR_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, AR_BOOL bInstant)__attribute__((unavailable("todo")));
AR_S32 AR_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn)__attribute__((unavailable("todo")));

AR_S32 AR_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode)__attribute__((unavailable("todo")));
AR_S32 AR_MPI_VDEC_GetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E *penDisplayMode)__attribute__((unavailable("todo")));

AR_S32 AR_MPI_VDEC_SetRotation(VDEC_CHN VdChn, ROTATION_E enRotation)__attribute__((unavailable("todo")));
AR_S32 AR_MPI_VDEC_GetRotation(VDEC_CHN VdChn, ROTATION_E *penRotation)__attribute__((unavailable("todo")));

/**
\brief 将解码通道绑定到某个视频缓存 VB 池中。
\attention VDEC_CHN_POOL_S 部分参数暂未实现或使用
\param[in] VdChn          :   VDEC通道号。
\param[in] pstPool        :   视频缓存 VB 池信息。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_AttachVbPool(VDEC_CHN VdChn, const VDEC_CHN_POOL_S *pstPool);

/**
\brief 将解码通道从某个视频缓存 VB 池中解绑定。
\attention VDEC_CHN_POOL_S 部分参数暂未实现或使用
\param[in] VdChn          :   VDEC通道号。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_DetachVbPool(VDEC_CHN VdChn);

/**
\brief 设置VDEC模块的参数, 建议先调用AR_MPI_VDEC_GetModParam。
\attention VDEC_MOD_PARAM_S 部分参数暂未实现或使用
\param[in] pstModParam  :   VDEC模块参数的结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_SetModParam(const VDEC_MOD_PARAM_S *pstModParam);

/**
\brief 获取VDEC模块的参数。
\attention VDEC_MOD_PARAM_S 部分参数暂未实现或使用
\param[in] pstModParam  :   VDEC模块参数的结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
/** @} */

#endif /* End of #ifndef  __MPI_VDEC_H__ */

