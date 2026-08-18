/**
 * \file
 * \brief 描述视频处理子系统的相关接口.
 */


#ifndef __MPI_VPSS_H__
#define __MPI_VPSS_H__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "ar_comm_vb.h"
#include "ar_comm_vpss.h"
#include "ar_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/**
\addtogroup MPI_VPSS
 * @brief 视频处理子系统，支持的具体图像处理功能包括
FRC（Frame Rate Control）、CROP、Sharpen、3DNR、Scale、像素格式转换、LDC、
Spread、固定角度旋转、任意角度旋转、鱼眼校正、Cover/Coverex、Overlayex、
Mosaic、Mirror/Flip、HDR、Aspect Ratio、压缩解压等
 * @{
*/

/**
\brief  创建一个VPSS GROUP
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  pstGrpAttr    VPSS GROUP属性指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_CreateGrp(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S* pstGrpAttr);

/**
\brief  销毁一个VPSS GROUP
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_DestroyGrp(VPSS_GRP VpssGrp);

/**
\brief  启用VPSS GROUP
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_StartGrp(VPSS_GRP VpssGrp);

/**
\brief  禁用VPSS GROUP
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_StopGrp(VPSS_GRP VpssGrp);

/**
\brief  复位VPSS GROUP
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_ResetGrp(VPSS_GRP VpssGrp);

/**
\brief  获取VPSS GROUP属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[out] pstGrpAttr    VPSS GROUP属性指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetGrpAttr(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S* pstGrpAttr);

/**
\brief  设置VPSS GROUP属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  pstGrpAttr    VPSS GROUP属性指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetGrpAttr(VPSS_GRP VpssGrp, const VPSS_GRP_ATTR_S* pstGrpAttr);

/**
\brief  设置VPSS CROP功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  pstCropInfo   CROP功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetGrpCrop(VPSS_GRP VpssGrp, const VPSS_CROP_INFO_S* pstCropInfo);

/**
\brief  设置VPSS CROP功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[out] pstCropInfo   CROP功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetGrpCrop(VPSS_GRP VpssGrp, VPSS_CROP_INFO_S* pstCropInfo);

/**
\brief  用户向VPSS发送数据
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssGrpPipe   VPSS组的管道号。取值只能为0
\param[in]  pstVideoFrame 待发送的图像信息
\param[in]  s32MilliSec   超时参数s32MilliSec设为-1时，为阻塞接口；0时为非阻塞接口；大于0时为超时等待时间，超时时间的单位为毫秒（ms）
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SendFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S* pstVideoFrame, AR_S32 s32MilliSec);

/**
\brief  用户从GROUP获取一帧原始图像。主要应用场景：高清设备解码回放，要求暂停、步进时，PIP层和普通视频层上的两个通道显示同一帧图像。通过本接口和HI_MPI_VPSS_SendFrame等接口的配合使用，可实现该功能
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssGrpPipe   VPSS组的管道号。取值只能为0
\param[out] pstVideoFrame 待发送的图像信息
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe,  VIDEO_FRAME_INFO_S* pstVideoFrame);

/**
\brief  用户释放一帧源图像
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssGrpPipe   VPSS组的管道号。取值只能为0
\param[in]  pstVideoFrame 待发送的图像信息
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_ReleaseGrpFrame(VPSS_GRP VpssGrp, VPSS_GRP_PIPE VpssGrpPipe, const VIDEO_FRAME_INFO_S* pstVideoFrame);

/**
\brief  使能Backup帧
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_EnableBackupFrame(VPSS_GRP VpssGrp);

/**
\brief  不使能Backup帧
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_DisableBackupFrame(VPSS_GRP VpssGrp);

/**
\brief  设置延迟启动VPSS处理的延时队列长度。延时处理主要由于前端进来的图像通过PCI传到主片可能会有延迟，为了保持同步，VPSS通过delay队列让主片的图像也延迟一段时间；另一应用场景是用户从VI获取图像，再通过TDE模块画框，该帧同时又送给VPSS进行处理，由于VPSS处理速度比TDE快，因此需要VPSS进行延迟处理
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  u32Delay      延时队列长度。取值范围：[0, 5]
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetGrpDelay(VPSS_GRP VpssGrp, AR_U32 u32Delay);

/**
\brief  获取延迟启动VPSS处理的延时队列长度
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[out] pu32Delay     获取延时队列长度指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetGrpDelay(VPSS_GRP VpssGrp, AR_U32 *pu32Delay);

/**
\brief  设置VPSS GROUP对应的鱼眼镜头LMF参数配置
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  pstFisheyeConfig     鱼眼镜头LMF参数结构体指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetGrpFisheyeConfig(VPSS_GRP VpssGrp, const VPSS_FISHEYE_ATTR_S *pstFisheyeConfig);

/**
\brief  设置VPSS GROUP对应的鱼眼镜头LMF参数配置
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[out] pstFisheyeConfig     鱼眼镜头LMF参数结构体指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetGrpFisheyeConfig(VPSS_GRP VpssGrp, VPSS_FISHEYE_ATTR_S *pstFisheyeConfig);

/**
\brief  VPSS以非绑定方式接收图像时，使能用户自己做帧率控制，此时VPSS通道帧率控制和VO的帧率控制不生效
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_EnableUserFrameRateCtrl(VPSS_GRP VpssGrp);

/**
\brief  VPSS以非绑定方式接收图像时，禁止用户自己做帧率控制，此时VPSS通道的帧率控制和VO的帧率控制生效
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_DisableUserFrameRateCtrl(VPSS_GRP VpssGrp);

/**
\brief  设置VPSS通道属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  pstChnAttr    VPSS通道属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CHN_ATTR_S* pstChnAttr);

/**
\brief  获取VPSS通道属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[out] pstChnAttr    VPSS通道属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CHN_ATTR_S* pstChnAttr);

/**
\brief  启用VPSS通道
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

/**
\brief  禁用VPSS通道
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

/**
\brief  设置VPSS通道CROP功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  pstCropInfo   CROP功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_CROP_INFO_S* pstCropInfo);

/**
\brief  获取VPSS通道CROP功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pstCropInfo   CROP功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnCrop(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_CROP_INFO_S* pstCropInfo);

/**
\brief  设置VPSS通道MPU功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  pstMpuInfo    MPU功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnMpuInfo(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_MPU_INFO_S* pstMpuInfo);

/**
\brief  获取VPSS通道MPU功能属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pstMpuInfo    MPU功能参数
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnMpuInfo(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_MPU_INFO_S* pstMpuInfo);

/**
\brief  设置VPSS通道图像固定角度旋转属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  enRotation    旋转属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E enRotation);

/**
\brief  获取VPSS通道图像固定角度旋转属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[out] penRotation    旋转属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnRotation(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, ROTATION_E* penRotation);

/**
\brief  设置VPSS通道CROP图像任意角度旋转属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  pstRotationExAttr    任意角度旋转属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnRotationEx(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_ROTATION_EX_ATTR_S* pstRotationExAttr);

/**
\brief  获取VPSS通道CROP图像任意角度旋转属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[out] pstRotationExAttr    任意角度旋转属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnRotationEx(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_ROTATION_EX_ATTR_S *pstRotationExAttr);

/**
\brief  设置VPSS镜头畸变校正（LDC）属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  pstLDCAttr    LDC属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_LDC_ATTR_S *pstLDCAttr);

/**
\brief  获取VPSS镜头畸变校正（LDC）属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[out] pstLDCAttr    LDC属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnLDCAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_LDC_ATTR_S *pstLDCAttr);

/**
\brief  设置VPSS通道展宽属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  pstSpreadAttr    展宽属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_SPREAD_ATTR_S *pstSpreadAttr);

/**
\brief  获取VPSS通道展宽属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[out] pstSpreadAttr    展宽属性结构体指针。动态属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnSpreadAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_SPREAD_ATTR_S *pstSpreadAttr);

/**
\brief   用户从通道获取一帧处理完成的图像
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pstVideoFrame 待发送的图像信息
\param[in]  s32MilliSec   超时参数s32MilliSec设为-1时，为阻塞接口；0时为非阻塞接口；大于0时为超时等待时间，超时时间的单位为毫秒（ms）
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S* pstVideoFrame, AR_S32 s32MilliSec);

/**
\brief   用户从通道获取一帧处理完成的图像，输出图像使用重新分配的内存
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pstVideoFrame 待发送的图像信息
\param[in]  s32MilliSec   超时参数s32MilliSec设为-1时，为阻塞接口；0时为非阻塞接口；大于0时为超时等待时间，超时时间的单位为毫秒（ms）
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnFrame_WithCopy(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VIDEO_FRAME_INFO_S* pstVideoFrame, AR_S32 s32MilliSec);

/**
\brief   用户释放一帧通道图像
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  pstVideoFrame 待发送的图像信息
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_ReleaseChnFrame(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_FRAME_INFO_S* pstVideoFrame);

/**
\brief   获取指定图像区域的亮度总和。该接口主要用于对OSD区域进行亮度和统计，根据统计的亮度，进行反色处理，使OSD区域更加明显
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  pstRegionInfo 区域信息。其中pstRegionInfo->pstRegion为统计区域的区域属性，即起始位置、宽、高；pstRegionInfo->u32RegionNum为统计区域的个数
\param[out] pu64LumaData  接收区域亮度和统计信息的内存指针，该内存大小应该大于或等于sizeof(AR_U64)×pstRegionInfo->u32RegionNum
\param[in]  s32MilliSec   超时参数s32MilliSec设为-1时，为阻塞接口；0时为非阻塞接口；大于0时为超时等待时间，超时时间的单位为毫秒（ms）
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetRegionLuma(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VIDEO_REGION_INFO_S* pstRegionInfo,
                                 AR_U64* pu64LumaData, AR_S32 s32MilliSec);

/**
\brief   将VPSS的通道绑定到某个视频缓存VB池中
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\param[in]  hVbPool       视频缓存VB池信息
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_AttachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VB_POOL hVbPool);

/**
\brief   将VPSS的通道从某个视频缓存VB池中解绑定
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_PHY_CHN_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_DetachVbPool(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

/**
\brief   设置VPSS扩展通道属性，扩展通道的主要应用是进行二次缩放和帧率控制
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)
\param[in]  pstExtChnAttr VPSS扩展通道属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_EXT_CHN_ATTR_S* pstExtChnAttr);

/**
\brief   获取VPSS扩展通道属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)
\param[out] pstExtChnAttr VPSS扩展通道属性
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetExtChnAttr(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_EXT_CHN_ATTR_S* pstExtChnAttr);

/**
\brief   设置VPSS扩展通道对应的鱼眼属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)
\param[in]  pstFishEyeAttr VPSS扩展通道的鱼眼属性结构体指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, const VPSS_FISHEYE_ATTR_S *pstFishEyeAttr);

/**
\brief   获取VPSS扩展通道对应的鱼眼属性
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)
\param[out] pstFishEyeAttr VPSS扩展通道的鱼眼属性结构体指针
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetExtChnFisheye(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, VPSS_FISHEYE_ATTR_S *pstFishEyeAttr);

/**
\brief   设置VPSS通道输出YUV数据的stride对齐, 行和列使用相同的align
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  u32Align      VPSS通道输出YUV数据的stride对齐，单位为Byte。取值范围：[0, 1024]
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 u32Align);

/**
\brief   获取VPSS通道输出YUV数据的stride对齐, 行和列使用相同的align
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pu32Align     VPSS通道输出YUV数据的stride对齐，单位为Byte
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 *pu32Align);

/**
\brief   设置VPSS通道输出YUV数据的stride对齐, 行和列使用不同的align
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  u32WAlign     VPSS通道输出YUV数据的行stride对齐，单位为Byte。取值范围：[0, 1024]
\param[in]  u32HAlign     VPSS通道输出YUV数据的列stride对齐，单位为Byte。取值范围：[0, 1024]
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnWHAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 u32WAlign, AR_U32 u32HAlign);

/**
\brief   获取VPSS通道输出YUV数据的stride对齐, 行和列使用不同的align
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pu32WAlign    VPSS通道输出YUV数据的行stride对齐，单位为Byte
\param[out] pu32HAlign    VPSS通道输出YUV数据的列stride对齐，单位为Byte
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnWHAlign(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 *pu32WAlign, AR_U32 *pu32HAlign);

/**
\brief   设置VPSS通道Scaler module
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[in]  u32ScaleModuleSel  VPSS通道Scaler module. 0 scaler, 1 gdc.
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_SetChnScaleModule(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 u32ScaleModuleSel);

/**
\brief   获取VPSS通道Scaler module
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\param[out] pu32ScaleModuleSel VPSS通道Scaler module. 0 scaler, 1 gdc.
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnScaleModule(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, AR_U32 *pu32ScaleModuleSel);


/**
\brief   获取VPSS通道对应的设备文件句柄
\param[in]  VpssGrp       VPSS GROUP号。取值范围：[0, VPSS_MAX_GRP_NUM)
\param[in]  VpssChn       VPSS 通道号。取值范围：[0, VPSS_MAX_CHN_NUM)
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_GetChnFd(VPSS_GRP VpssGrp, VPSS_CHN VpssChn);

/**
\brief   关闭设备和通道的文件描述符
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_CloseFd(AR_VOID);

/**
* @brief      向VPSS发送suspend命令
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_Suspend(AR_VOID);

/**
* @brief      向VPSS发送resume命令
\retval ::AR_SUCCESS     :   成功
\retval ::ERROR_CODE     :   失败, 返回错误码
*/
AR_S32 AR_MPI_VPSS_Resume(AR_VOID);

/** @}*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __MPI_VPSS_H__ */

