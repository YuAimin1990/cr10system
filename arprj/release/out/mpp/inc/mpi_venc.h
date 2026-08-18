/**
 * \file
 * \brief 描述视频编码相关的数据结构和接口.
 */

#ifndef __MPI_VENC_H__
#define __MPI_VENC_H__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "ar_comm_venc.h"
#include "ar_comm_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/******************************* API declaration *****************************/
/**
\addtogroup MPI_VENC
 * @brief 本模块支持多路实时编码，且每路编码独立，编码协议和编码 profile 可以不同。
 本模块支持视频编码同时，调度 Region 模块对编码图像内容进行叠加和遮挡。
 * @{
*/

/**
\brief VENC设备初始化，初始化基本的数据结构。
\attention 新添加接口
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_Init(AR_VOID);

/**
\brief VENC设备去初始化去初始化基本的数据结构。
\attention 新添加接口
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_Exit(AR_VOID);

/**
\brief 创建VENC通道。
\param[in] VeChn        :   VENC通道号，创建之后不能重复。
\param[in] pstAttr      :   VENC通道属性指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr);

/**
\brief 销毁VENC通道，创建后才能销毁。
\param[in] VeChn        :   VENC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_DestroyChn(VENC_CHN VeChn);

/**
\brief 重置VENC通道，停止通道后才能重置。
\param[in] VeChn        :   VENC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_ResetChn(VENC_CHN VeChn);

/**
\brief VENC通道启动接收数据，创建后才能启动。
\param[in] VeChn        :   VENC通道号。
\param[in] pstRecvParam :   VENC接收图片参数指针
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_StartRecvFrame(VENC_CHN VeChn, const VENC_RECV_PIC_PARAM_S *pstRecvParam);

/**
\brief VENC通道停止接收数据，启动后才能停止。
\param[in] VeChn        :   VENC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_StopRecvFrame(VENC_CHN VeChn);

/**
\brief 查询VENC通道的状态，启动后才能查询。
\param[in] VeChn        :   VENC通道号。
\param[in] pstStatus    :   VENC通道状态结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_QueryStatus(VENC_CHN VeChn, VENC_CHN_STATUS_S *pstStatus);

/**
\brief 设置VENC通道的属性，创建通道后才能设置，建议先获取再设置。
\param[in] VeChn        :   VENC通道号。
\param[in] pstChnAttr   :   VENC通道属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstChnAttr);

/**
\brief 获取VENC通道的属性，创建通道后才能获取。
\param[in] VeChn         :   VENC通道号。
\param[out] pstChnAttr   :   VENC通道属性结构体指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstChnAttr);

/**
\brief 获取VENC通道的码流。
\param[in] VeChn        :   VENC通道号。
\param[out] pstStream   :   VENC码流结构体指针。
\param[in] s32MilliSec  :   获取码流等待时间，-1代表阻塞。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, AR_S32 s32MilliSec);

/**
\brief 释放VENC通道的码流。
\param[in] VeChn        :   VENC通道号。
\param[in] pstStream    :   VENC码流结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream);

/**
\brief 插入VENC通道的用户数据。
\note 暂不支持。
\param[in] VeChn        :   VENC通道号。
\param[in] pu8Data      :   用户数据地址。
\param[in] u32Len       :   用户数据长度。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_InsertUserData(VENC_CHN VeChn, AR_U8 *pu8Data, AR_U32 u32Len);

/**
\brief VENC通道发送一帧视频数据进行编码。
\param[in] VeChn        :   VENC通道号。
\param[in] pstFrame     :   视频帧信息结构体指针。
\param[in] s32MilliSec  :   等待时间，-1代表阻塞。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame ,AR_S32 s32MilliSec);

/**
\brief VENC通道发送一帧用户视频数据进行编码。
\note 接口参数变化。
\param[in] VeChn        :   VENC通道号。
\param[in] pstFrame     :   用户视频帧信息结构体指针。
\param[in] s32MilliSec  :   等待时间，-1代表阻塞。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SendFrameEx(VENC_CHN VeChn, const USER_FRAME_INFO_S *pstFrame, AR_S32 s32MilliSec);

/**
\brief VENC通道立即编码一帧IDR帧。
\note 接口参数变化
\param[in] VeChn        :   VENC通道号。
\param[in] bInstant     :   立即编码IDR，只支持true。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_RequestIDR(VENC_CHN VeChn, AR_BOOL bInstant);

/**
\brief 获取VENC通道对应的文件描述符。
\param[in] VeChn        :   VENC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetFd(VENC_CHN VeChn);

/**
\brief 关闭VENC通道对应的文件描述符,建议在销毁通道后调用。
\param[in] VeChn        :   VENC通道号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_CloseFd(VENC_CHN VeChn);

/**
\brief 设置VENC通道的ROI属性。
\note 接口变化。支持无限设置。
\param[in] VeChn        :   VENC通道号。
\param[in] pstRoiAttr   :   VENC ROI属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetRoiAttr(VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr);

/**
\brief 获取VENC通道的ROI属性。
\note 接口参数变化。需要先AR_MPI_VENC_SetRoiAttr，才能AR_MPI_VENC_GetRoiAttr，否则返回失败。
\param[in] VeChn        :   VENC通道号。
\param[in] u32Index     :   ROI区域的索引
\param[out] pstRoiAttr  :   VENC ROI属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetRoiAttr(VENC_CHN VeChn, AR_U32 u32Index, VENC_ROI_ATTR_S *pstRoiAttr);

/**
\brief 获取VENC通道的ROI扩展属性。
\note 暂不支持。
\param[in] VeChn          :   VENC通道号。
\param[in] u32Index       :   ROI区域的索引
\param[out] pstRoiAttrEx  :   对应ROI区域的配置参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetRoiAttrEx(VENC_CHN VeChn, AR_U32 u32Index, VENC_ROI_ATTR_EX_S *pstRoiAttrEx)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的ROI扩展属性。
\note 暂不支持。
\param[in] VeChn          :   VENC通道号。
\param[in] pstRoiAttrEx   :   对应ROI区域的配置参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetRoiAttrEx(VENC_CHN VeChn, const VENC_ROI_ATTR_EX_S *pstRoiAttrEx)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的ROI扩展属性。
\note 暂不支持。
\param[in] VeChn             :   VENC通道号。
\param[in] pstRoiBgFrmRate   :   VENC ROI属性结构体指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的ROI扩展属性。
\note 暂不支持。
\param[in] VeChn              :   VENC通道号。
\param[out] pstRoiBgFrmRate   :   VENC ROI属性结构体指针。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道H.264的条带分割参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstSliceSplit :   H.264的条带分割参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_H264_SLICE_SPLIT_S *pstSliceSplit);

/**
\brief 获取VENC通道H.264的条带分割参数。
\param[in] VeChn          :   VENC通道号。
\param[out] pstSliceSplit :   H.264的条带分割参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_H264_SLICE_SPLIT_S *pstSliceSplit);

/**
\brief 设置VENC通道H.264的帧内预测参数。
\param[in] VeChn            :   VENC通道号。
\param[in] pstH264IntraPred :   H.264的帧内预测参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_H264_INTRA_PRED_S *pstH264IntraPred);

/**
\brief 获取VENC通道H.264的帧内预测参数。
\param[in] VeChn             :   VENC通道号。
\param[out] pstH264IntraPred :   H.264的帧内预测参数指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_H264_INTRA_PRED_S *pstH264IntraPred);

/**
\brief 设置H.264协议编码通道的变换、量化属性。
\note 暂不支持。
\param[in] VeChn             :   VENC通道号。
\param[in] pstH264Trans      :   H.264协议编码通道的变换、量化参数指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_H264_TRANS_S *pstH264Trans)__attribute__((unavailable("todo")));

/**
\brief 获取H.264协议编码通道的变换、量化属性。
\note 暂不支持。
\param[in] VeChn             :   VENC通道号。
\param[out] pstH264Trans     :   H.264协议编码通道的变换、量化参数指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_H264_TRANS_S *pstH264Trans)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道H.264的熵编码参数。
\param[in] VeChn             :   VENC通道号。
\param[in] pstH264EntropyEnc :   H.264的熵编码参数指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_H264_ENTROPY_S *pstH264EntropyEnc);

/**
\brief 获取VENC通道H.264的熵编码参数。
\param[in] VeChn              :   VENC通道号。
\param[out] pstH264EntropyEnc :   H.264的熵编码参数指针。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_H264_ENTROPY_S *pstH264EntropyEnc);

/**
\brief 设置VENC通道H.264的deblock参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstH264Dblk   :   H.264的deblock的参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_H264_DBLK_S *pstH264Dblk);

/**
\brief 获取VENC通道H.264的deblock参数。
\param[in] VeChn         :   VENC通道号。
\param[out] pstH264Dblk   :   H.264的deblock的参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_H264_DBLK_S *pstH264Dblk);

/**
\brief 设置H.264编码的VUI配置。
\param[in] VeChn         :   编码通道号。
\param[in] pstH264Vui    :   H.264协议编码通道的Vui参数。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_H264_VUI_S *pstH264Vui);

/**
\brief 获取H.264编码的VUI配置。
\param[in] VeChn         :   编码通道号。
\param[out] pstH264Vui   :   H.264协议编码通道的Vui参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_H264_VUI_S *pstH264Vui);

/**
\brief 设置H.265编码的VUI配置。
\param[in] VeChn         :   编码通道号。
\param[in] pstH264Vui    :   H.265协议编码通道的Vui参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265Vui(VENC_CHN VeChn, const VENC_H265_VUI_S *pstH265Vui);

/**
\brief 设置H.265编码的VUI配置。
\param[in] VeChn         :   编码通道号。
\param[out] pstH264Vui   :   H.265协议编码通道的Vui参数。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265Vui(VENC_CHN VeChn, VENC_H265_VUI_S *pstH265Vui);

/**
\brief 设置VENC通道Jpeg的编码参数。
\note 参数有更新。
\param[in] VeChn         :   VENC通道号。
\param[in] pstJpegParam  :   Jpeg的编码参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_JPEG_PARAM_S *pstJpegParam);

/**
\brief 获取VENC通道Jpeg的编码参数。
\note 参数有更新。
\param[in] VeChn          :   VENC通道号。
\param[out] pstJpegParam  :   Jpeg的编码参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_JPEG_PARAM_S *pstJpegParam);

/**
\brief 设置VENC通道MJpeg的编码参数。
\note 参数有更新。
\param[in] VeChn         :   VENC通道号。
\param[in] pstMjpegParam :   MJpeg的编码参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_MJPEG_PARAM_S *pstMjpegParam);

/**
\brief 获取VENC通道MJpeg的编码参数。
\note 参数有更新。
\param[in] VeChn          :   VENC通道号。
\param[out] pstMjpegParam :   MJpeg的编码参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_MJPEG_PARAM_S *pstMjpegParam);

/**
\brief 获取VENC通道码控的参数。
\note 参数有更新。
\param[in] VeChn         :   VENC通道号。
\param[out] pstRcParam    :   码控参数的指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam);

/**
\brief 设置VENC通道码控的参数。
\note 参数有更新。
\param[in] VeChn         :   VENC通道号。
\param[in] pstRcParam    :   码控参数的指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam);

/**
\brief 设置VENC通道H.264和H.264的高级跳帧参考参数。
\note 参数有更新。
\param[in] VeChn         :   VENC通道号。
\param[in] pstRefParam   :   H.264和H.265的预测参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_REF_PARAM_S *pstRefParam);

/**
\brief 获取VENC通道H.264和H.264的高级跳帧参考参数。
\note 参数有更新。
\param[in] VeChn          :   VENC通道号。
\param[out] pstRefParam   :   H.264和H.265的预测参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_REF_PARAM_S *pstRefParam);

/**
\brief 设置JPEG抓拍通道的抓拍模式。
\note 暂不支持。
\param[in] VeChn              :   VENC通道号。
\param[in] enJpegEncodeMode   :   通道抓拍模式。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetJpegEncodeMode(VENC_CHN VeChn, const VENC_JPEG_ENCODE_MODE_E enJpegEncodeMode)__attribute__((unavailable("todo")));

/**
\brief 设置JPEG抓拍通道的抓拍模式。
\note 暂不支持。
\param[in] VeChn              :   VENC通道号。
\param[out] enJpegEncodeMode  :   通道抓拍模式。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetJpegEncodeMode(VENC_CHN VeChn, VENC_JPEG_ENCODE_MODE_E *penJpegEncodeMode)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道码控的参数。
\param[in] VeChn         :   VENC通道号。
\param[in] bEnableIDR    :   是否打开IDR。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_EnableIDR(VENC_CHN VeChn, AR_BOOL bEnableIDR);

/**
\brief 获取码流buffer的物理地址和大小。
\note 暂不支持。
\param[in] VeChn                :   VENC通道号。
\param[out] pstStreamBufInfo    :   码流结构体参数指针。
\retval ::0                     :   成功。
\retval ::non-zero              :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pstStreamBufInfo);

/**
\brief 设置VENC通道H.265的条带分割参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstSliceSplit :   H.265的条带分割参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_H265_SLICE_SPLIT_S *pstSliceSplit);

/**
\brief 获取VENC通道H.265的条带分割参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstSliceSplit :   H.265的条带分割参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_H265_SLICE_SPLIT_S *pstSliceSplit);

/**
\brief 设置VENC通道H.265的预测参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstePredUnit  :   H.265的预测参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_H265_PU_S *pstePredUnit);

/**
\brief 获取VENC通道H.265的预测参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstPredUnit   :   H.265的预测参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_H265_PU_S *pstPredUnit);


/**
\brief 设置H.265协议编码通道的变换、量化的属性。
\note 暂不支持。
\param[in] VeChn         :   VENC通道号。
\param[in] pstH265Trans  :   H.265协议编码通道变换、量化结构体参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_H265_TRANS_S *pstH265Trans)__attribute__((unavailable("todo")));

/**
\brief 获取H.265协议编码通道的变换、量化的属性。
\note 暂不支持。
\param[in] VeChn         :   VENC通道号。
\param[out] pstH265Trans :   H.265协议编码通道变换、量化结构体参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_H265_TRANS_S *pstH265Trans)__attribute__((unavailable("todo")));

/**
\brief 设置H.265通道的熵编码属性。
\note 暂不支持。
\param[in] VeChn          :   VENC通道号。
\param[in] pstH265Entropy :   H.265协议编码通道熵编码的结构体参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_H265_ENTROPY_S *pstH265Entropy)__attribute__((unavailable("todo")));

/**
\brief 设置H.265通道的熵编码属性。
\note 暂不支持。
\param[in] VeChn           :   VENC通道号。
\param[out] pstH265Entropy :   H.265协议编码通道熵编码的结构体参数指针。
\retval ::0                :   成功。
\retval ::non-zero         :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_H265_ENTROPY_S *pstH265Entropy)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道H.265的deblock参数。
\param[in] VeChn         :   VENC通道号。
\param[in] pstH265Dblk   :   H.265的deblock的参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_H265_DBLK_S *pstH265Dblk);

/**
\brief 获取VENC通道H.265的deblock参数。
\param[in] VeChn          :   VENC通道号。
\param[out] pstH265Dblk   :   H.265的deblock的参数指针。
\retval ::0               :   成功。
\retval ::non-zero        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_H265_DBLK_S *pstH265Dblk);

/**
\brief 设置VENC通道H.265的SAO参数。
\param[in] pstH265Sao    :   H.265的SAO的参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_H265_SAO_S *pstH265Sao);

/**
\brief 获取VENC通道H.265的SAO参数。
\param[in] VeChn         :   VENC通道号。
\param[out] pstH265Sao    :   H.265的SAO的参数指针。
\retval ::0              :   成功。
\retval ::non-zero       :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_H265_SAO_S *pstH265Sao);

/**
\brief 设置VENC通道的帧内刷新参数。
\note 参数更新。
\param[in] VeChn            :   VENC通道号。
\param[in] pstIntraRefresh  :   帧内刷新参数结构体指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetIntraRefresh(VENC_CHN VeChn, const VENC_INTRA_REFRESH_S *pstIntraRefresh);

/**
\brief 获取VENC通道的帧内刷新参数。
\note 参数更新。
\param[in] VeChn             :   VENC通道号。
\param[out] pstIntraRefresh  :   帧内刷新参数结构体指针。
\retval ::0                  :   成功。
\retval ::non-zero           :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_INTRA_REFRESH_S *pstIntraRefresh);

/**
\brief 获取H.264/H.265通道的SSE属性。
\note 暂不支持。
\param[in] VeChn            :   VENC通道号。
\param[out] pstSSECfg       :   SSE区域参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetSSERegion(VENC_CHN VeChn, AR_U32 u32Index,VENC_SSE_CFG_S *pstSSECfg)__attribute__((unavailable("todo")));

/**
\brief 设置H.264/H.265 通道的SSE属性。
\note 暂不支持。
\param[in] VeChn            :   VENC通道号。
\param[in] pstSSECfg        :   SSE区域参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S * pstSSECfg)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的帧内刷新参数。
\note 参数更新。
\param[in] VeChn            :   VENC通道号。
\param[in] pstChnParam      :   编码通道的参数结构体参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetChnParam(VENC_CHN VeChn, const VENC_CHN_PARAM_S *pstChnParam);

/**
\brief 获取VENC通道的帧内刷新参数。
\note 参数更新。
\param[in] VeChn            :   VENC通道号。
\param[out] pstChnParam     :   编码通道的参数结构体参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetChnParam(VENC_CHN VeChn, VENC_CHN_PARAM_S *pstChnParam);

/**
\brief 设置VENC通道的模块参数。
\note 参数有更新。
\param[in] pstModParam      :   模块的参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetModParam(const VENC_PARAM_MOD_S *pstModParam);

/**
\brief 获取VENC通道的模块参数。
\note 参数有更新。
\param[out] pstModParam     :   模块的参数指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetModParam(VENC_PARAM_MOD_S *pstModParam);

/**
\brief 将编码通道绑定到某个视频缓存VB池中。
\note 目前只支持使用VENC_CHN_POOL_S中的hPicVbPool，此vb pool中最小的buffer数量为2，
\note 每块buffer的size计算方法为：align32(width)*align32(height) + align16(align32(width)/2)*align32(height)
\param[in] VeChn            :   通道号
\param[in] pstPool          :   视频缓存VB池的Id号。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_AttachVbPool(VENC_CHN VeChn, const VENC_CHN_POOL_S *pstPool);

/**
\brief 将编码通道从某个视频缓存VB池中解绑定。
\note
\param[in] VeChn            :   通道号
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_DetachVbPool(VENC_CHN VeChn);

/**
\brief 设置 CU 模式选择的倾向性。
\note 暂不支持。
\param[in] VeChn            :   通道号
\param[in] pstCuPrediction  :   CU模式选择的倾向性参数指针
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetCuPrediction(VENC_CHN VeChn, const  VENC_CU_PREDICTION_S * pstCuPrediction)__attribute__((unavailable("todo")));

/**
\brief 获取 CU 模式选择的倾向性。
\note 暂不支持。
\param[in] VeChn            :   通道号
\param[out] pstCuPrediction :   CU模式选择的倾向性参数指针
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetCuPrediction(VENC_CHN VeChn, VENC_CU_PREDICTION_S * pstCuPrediction)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的去呼吸效应强度。
\note 参数更新。
\param[in] VeChn              :   VENC通道号。
\param[in] pstDeBreathEffect  :   帧内刷新参数结构体参数指针。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetDeBreathEffect(VENC_CHN VeChn, const VENC_DEBREATHEFFECT_S * pstDeBreathEffect);

/**
\brief 获取VENC通道的去呼吸效应强度。
\note 参数更新。
\param[in] VeChn               :   VENC通道号。
\param[out] pstDeBreathEffect  :   帧内刷新参数结构体参数指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetDeBreathEffect(VENC_CHN VeChn, VENC_DEBREATHEFFECT_S * pstDeBreathEffect);

/**
\brief 获取VENC通道的去呼吸效应强度。
\note 暂不支持。
\param[in] VeChn              :   VENC通道号。
\param[in] pstDeBreathEffect  :   帧内刷新参数结构体指针。
\retval ::0                   :   成功。
\retval ::non-zero            :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetHierarchicalQp(VENC_CHN VeChn, const VENC_HIERARCHICAL_QP_S * pstHierarchicalQp)__attribute__((unavailable("todo")));

/**
\brief 获取VENC通道的去呼吸效应强度。
\note 暂不支持。
\param[in] VeChn               :   VENC通道号。
\param[out] pstHierarchicalQp  :   帧内刷新参数结构体指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetHierarchicalQp(VENC_CHN VeChn, VENC_HIERARCHICAL_QP_S * pstHierarchicalQp)__attribute__((unavailable("todo")));

/**
\brief 设置VENC通道的逆时针旋转角度参数, 必须在AR_MPI_VENC_StartRecvFrame调用之前设置才能生效
\param[in] VeChn               :   VENC通道号。
\param[out] pstHierarchicalQp  :   逆时针旋转角度参数结构体指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetChnRotationParam(VENC_CHN VeChn, VENC_ROTATION_PARAM_S* pstRotationParam);

/**
\brief 获取VENC通道的逆时针旋转角度参数
\param[in] VeChn               :   VENC通道号。
\param[out] pstRotationParam   :   逆时针旋转角度参数结构体指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetChnRotationParam(VENC_CHN VeChn, VENC_ROTATION_PARAM_S* pstRotationParam);

/**
\brief 设置VENC通道的镜像方向参数, 必须在AR_MPI_VENC_StartRecvFrame调用之前设置才能生效
\param[in] VeChn               :   VENC通道号。
\param[out] pstRotationParam   :   镜像方向参数结构体指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_SetChnMirroParam(VENC_CHN VeChn, VENC_MIRROR_PARAM_S* pstMirrorParam);

/**
\brief 获取VENC通道的镜像方向参数
\param[in] VeChn               :   VENC通道号。
\param[out] pstHierarchicalQp  :   镜像方向参数结构体指针。
\retval ::0                    :   成功。
\retval ::non-zero             :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VENC_GetChnMirrorParam(VENC_CHN VeChn, VENC_MIRROR_PARAM_S* pstMirrorParam);
/*
AR_S32 AR_MPI_VENC_SysSuspend();
AR_S32 AR_MPI_VENC_SysResume();
*/
AR_S32 AR_MPI_VENC_Suspend(VENC_CHN VeChn);
AR_S32 AR_MPI_VENC_Resume(VENC_CHN VeChn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

/** @} */

#endif /* __MPI_VENC_H__ */

