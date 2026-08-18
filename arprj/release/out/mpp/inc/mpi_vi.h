/**
 * \file
 * \brief 描述视频输入相关接口
 */

#ifndef __MPI_VI_H__
#define __MPI_VI_H__

#include "ar_comm_vi.h"
#include "ar_comm_dis.h"
#include "ar_comm_gdc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif /* __cplusplus */
#endif /* __cplusplus */

/******************************* API declaration *****************************/
/**
\addtogroup MPI_VI
 * @brief 通过 MIPI Rx等接口接收视频数据。
 VI 将接收到的数据存入到指定的内存区域，在此过程中，VI 可以对接收到的原始视频图像数据进行处理，实现视频数据的采集。

    vin的启动流程api调用如下
    AR_MPI_VI_SetMipiBindDev， 把具体的接口索引,比如mipi 或者dvp 和dev 号绑定，这样dev 号就代表了这些接口，也代表了连接到接口的具体的hw，比如sensor等
    AR_MPI_VI_SetComboDevAttr  配置具体接口及和接口连接的hw的一些属性，比如mipi 属性，电源，时钟等,AR_MPI_VI_SetComboDevAttr中的devno 正是MipiDev
    AR_MPI_VI_SetDevAttr       进一步配置设备的属性，也就是配置mipi 或者dvp的属性
    AR_MPI_VI_EnableDev        使能设备
    AR_MPI_VI_SetDevBindPipe   把设备和pipe 关联起来，可以关联多个pipe,这里关联的pipe 数目和pipe 号，决定着后续pipe api操作使用的pipe 号。也决定着后续可以操作的
                               所有的pipe

    以下的操作针对每个pipe，pipe 由AR_MPI_VI_SetDevBindPipe 传入
    AR_MPI_VI_CreatePipe       把AR_MPI_VI_SetDevBindPipe 关联的所有pipe 创建出来，并用pipe属性进行配置
    AR_MPI_VI_SetPipeVCNumber AR_MPI_VI_SetPipeDTNumber 如果存在虚拟通道的情况配置每个pipe使用的虚拟通道号和数据类型
    AR_MPI_VI_StartPipe        启动每个pipe
    AR_MPI_VI_SetChnAttr AR_MPI_VI_EnableChn 配置每个pipe 下的通道并使能，每个pipe 可能不止1个通道
    AR_MPI_ISP_SensorRegCallBack AR_MPI_AE_SensorRegCallBack AR_MPI_AWB_SensorRegCallBack 向pipe 注册sensor aec awb 注册回到，关联驱动的操作函数比如sensor的分辨率设置，
    曝光等。 完成pipe 和驱动的绑定
    AR_MPI_ISP_RegisterNotifyCallBack 初始化isp 之前，可以注册一些isp通知，比如aec update的回调。
    AR_MPI_ISP_MemInit
    AR_MPI_ISP_SetPubAttr 设置公共属性，包括sensor的wh 等
    AR_MPI_VI_SetPipeExtAttr 这里可以设置一些通道的扩展属性，主要修改驱动的一些参数，比如hmax，vmax，效果参数等，这里修改后，驱动的默认值会被置换掉。
    AR_MPI_ISP_Init       初始化isp
    AR_MPI_ISP_Run        启动isp，接下来可以通过api 取帧，或通过其他api 设置效果参数等，需要注意的是，isp 相关api 必须调用，不管数据是否真正的过isp。

    AR_MPI_VI_SetChnAttr AR_MPI_VI_EnableChn，这里可以继续使能上面没有使能的通道

    通道和isp 启动到这里，一些动态设置属性的api 可以使用了。典型的AR_MPI_VI_ReShapeCh，动态调整分辨率，也可以成对的使用AR_MPI_VI_PauseChn 和 AR_MPI_VI_RecoverChn，
    AR_MPI_VI_PauseChn 和 AR_MPI_VI_RecoverChn 这两个api 提供了快速的关闭和开启通道的方法。
    
    

    退出流程
    AR_MPI_VI_DisableChn 关闭AR_MPI_ISP_Run 启动后使能的通道
    AR_MPI_ISP_Exit 退出isp
    AR_MPI_ISP_SensorUnRegCallBack AR_MPI_AE_SensorUnRegCallBack AR_MPI_AWB_SensorUnRegCallBack AR_MPI_ISP_UnRegisterNotifyCallBack
    AR_MPI_VI_DisableChn 关闭AR_MPI_ISP_Run 启动前使能的通道
    AR_MPI_VI_StopPipe
    AR_MPI_VI_DestroyPipe
    AR_MPI_VI_DisableDev
    
 * @{
*/

/**
\brief 获取vin 驱动的启动选项
@attention 新添加接口
\param[in] p_opt    :指向启动选项的结构的指针
\see \n
N/A
*/


AR_S32 AR_MPI_VIN_get_driver_opt(STRU_VIN_DRIVER_START_OPT_T *p_opt);

/**
\brief 设置vin 驱动的启动选项
@attention 新添加接口
\param[in] p_opt    :   指向启动选项的结构的指针，必须先获取到选项，然后修改，然后set
\see \n
N/A
*/

AR_S32 AR_MPI_VIN_set_driver_opt(STRU_VIN_DRIVER_START_OPT_T *p_opt);

/**
\brief 加载vin 驱动，本接口在设备启动后只能调用一次，多次调用会导致不可预期的问题
@attention 新添加接口
\param[in] 无
\see \n
N/A
*/

AR_S32 AR_MPI_VIN_load_vin_driver();

/**
\brief 卸载vin 驱动，本接口在设备启动后只能调用一次，多次调用会导致不可预期的问题
@attention 新添加接口
\param[in] 无
\see \n
N/A
*/


AR_S32 AR_MPI_VIN_unload_vin_driver();


/**
\brief 使用dev属性打开vin设备，主要配置vin的工作模式，各个模块的工作频率。
@attention 新添加接口
\param[in] pProp    :   指向vin dev属性。
\see \n
N/A
*/
AR_VOID AR_MPI_VIN_OpenDev(VI_DEV_PROP_S *pProp);

/**
\brief 关闭vin设备。
@attention 新添加接口
\see \n
N/A
*/
AR_VOID AR_MPI_VIN_CloseDev();

/**
\brief 设置VI设备属性。基本设备属性默认了部分芯片配置，满足绝大部分的sensor对接要求。
\param[in] ViDev        :   VI设备号。
\param[in] pstDevAttr   :   VI设备属性指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetDevAttr(VI_DEV ViDev, const VI_DEV_ATTR_S *pstDevAttr);

/**
\brief 获取VI设备属性。
\param[in] ViDev        :   VI设备号。
\param[out] pstDevAttr  :   VI设备属性指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetDevAttr(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);

/**
\brief 启用VI设备。
\param[in] ViDev    :   VI设备号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_EnableDev(VI_DEV ViDev);

/**
\brief 禁用VI设备。
\param[in] ViDev    :   VI设备号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_DisableDev(VI_DEV ViDev);

/**
\brief 设置VI设备与MIPI设备的绑定关系。设备号是虚拟的，bind mipi 或dvp 索引后，这个设备就代表了接入这个接口的设备
       AR_MPI_VI_SetMipiBindDev-->AR_MPI_VI_SetComboDevAttr, AR_MPI_VI_SetComboDevAttr中的devno 正是MipiDev
\param[in] ViDev    :   VI设备号。
\param[in] MipiDev  :   MIPI设备号或mipi的索引。如果是dvp设备，这个参数代表dvp的索引
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetMipiBindDev(VI_DEV ViDev, MIPI_DEV MipiDev);

/**
\brief 获取VI设备所绑定的MIPI设备。
\param[in] ViDev        :   VI设备号。
\param[out] pMipiDev    :   MIPI设备指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetMipiBindDev(VI_DEV ViDev, MIPI_DEV *pMipiDev);

/**
\brief 设置VI设备与物理PIPE的绑定关系。一个设备至少需要绑定一个管道，典型解虚拟通道的时候需要一个设备对应多个pipe
       AR_MPI_VI_SetDevAttr -> AR_MPI_VI_EnableDev -> AR_MPI_VI_SetDevBindPipe
\param[in] ViDev            :   VI设备号。
\param[in] pstDevBindPipe   :   绑定到Dev的物理PIPE信息的结构体指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetDevBindPipe(VI_DEV ViDev, const VI_DEV_BIND_PIPE_S *pstDevBindPipe);

/**
\brief 获取VI设备所绑定的物理PIPE。
\param[in] ViDev            :   VI设备号。
\param[out] pstDevBindPipe  :   绑定到Dev的物理PIPE信息的结构体指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetDevBindPipe(VI_DEV ViDev, VI_DEV_BIND_PIPE_S *pstDevBindPipe);

/**
\brief 获取VI物理PIPE的RAW压缩参数。
@attention 支持未实现接口
\param[in] ViPipe       :   物理PIPE号。
\param[out] pCmpParam   :   压缩参数的结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeCmpParam(VI_PIPE ViPipe, VI_CMP_PARAM_S *pCmpParam);

/**
\brief 设置VI物理PIPE的RAW压缩参数。
@attention 支持未实现接口
\param[in] ViPipe       :   物理PIPE号。
\param[in] pCmpParam    :   压缩参数的结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeCmpParam(VI_PIPE ViPipe, VI_CMP_PARAM_S *pCmpParam);

/**
\brief 设置用户图片，作为无视频信号时的插入图片。
\param[in] ViPipe       :   PIPE号。
\param[in] pstUsrPic    :   用户图片信息结构指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetUserPic(VI_PIPE ViPipe, const VI_USERPIC_ATTR_S *pstUsrPic);

/**
\brief 启用VI PIPE插入用户图片。
\param[in] ViPipe       :   PIPE号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_EnableUserPic(VI_PIPE ViPipe);

/**
\brief 禁用VI PIPE插入用户图片。
\param[in] ViPipe       :   PIPE号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_DisableUserPic(VI_PIPE ViPipe);

/**
\brief 创建一个VI PIPE。
\param[in] ViPipe       :   PIPE号。
\param[in] pstPipeAttr  :   PIPE的属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_CreatePipe(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);

/**
\brief 销毁一个VI PIPE。
\param[in] ViPipe       :   PIPE号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_DestroyPipe(VI_PIPE ViPipe);

/**
\brief 设置VI PIPE的属性。
\param[in] ViPipe       :   PIPE号。
\param[in] pstPipeAttr  :   PIPE的属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeAttr(VI_PIPE ViPipe, const VI_PIPE_ATTR_S *pstPipeAttr);

/**
\brief 获取VI PIPE的属性。
\param[in] ViPipe       :   PIPE号。
\param[out] pstPipeAttr :   PIPE的属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeAttr(VI_PIPE ViPipe, VI_PIPE_ATTR_S *pstPipeAttr);

/**
\brief 启用VI PIPE。
\param[in] ViPipe       :   PIPE号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_StartPipe(VI_PIPE ViPipe);

/**
\brief 禁用VI PIPE。
\param[in] ViPipe       :   PIPE号。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_StopPipe(VI_PIPE ViPipe);

/**
\brief 设置VI物理PIPE裁剪功能属性。
@attention 未实现接口
\param[in] ViPipe       :   VI物理PIPE号。
\param[in] pstCropInfo  :   裁剪功能参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeCrop(VI_PIPE ViPipe, const CROP_INFO_S *pstCropInfo);

/**
\brief 获取VI物理PIPE裁剪功能属性。
@attention 未实现接口
\param[in] ViPipe       :   VI物理PIPE号。
\param[out] pstCropInfo :   裁剪功能参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeCrop(VI_PIPE ViPipe, CROP_INFO_S *pstCropInfo);

/**
\brief 设置VI物理PIPE dump属性。
@attention 未实现接口
\param[in] ViPipe       :   物理PIPE号。
\param[in] pstDumpAttr  :   VI物理PIPE dump的属性。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeDumpAttr(VI_PIPE ViPipe, const VI_DUMP_ATTR_S *pstDumpAttr);

/**
\brief 获取VI物理PIPE dump属性。
@attention 未实现接口
\param[in] ViPipe       :   物理PIPE号。
\param[out] pstDumpAttr :   VI物理PIPE dump的属性。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeDumpAttr(VI_PIPE ViPipe, VI_DUMP_ATTR_S *pstDumpAttr);

/**
\brief 设置VI PIPE数据的来源。
@attention 未实现接口
\param[in] ViPipe   :   PIPE号。
\param[in] enSource :   PIPE的数据来源。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeFrameSource(VI_PIPE ViPipe, const VI_PIPE_FRAME_SOURCE_E enSource);

/**
\brief 获取VI PIPE数据的来源。
@attention 未实现接口
\param[in] ViPipe       :   PIPE号。
\param[out] penSource   :   PIPE的数据来源。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeFrameSource(VI_PIPE ViPipe, VI_PIPE_FRAME_SOURCE_E *penSource);

/**
\brief 获取VI物理PIPE的数据。
\param[in] ViPipe           :   物理PIPE号。
\param[out] pstVideoFrame   :   VI PIPE数据信息的指针。
\param[in] s32MilliSec      :   超时参数。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeFrame(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame, AR_S32 s32MilliSec);

/**
\brief 释放VI PIPE的数据。
\param[in] ViPipe           :   PIPE号。
\param[in] pstVideoFrame    :   VI PIPE数据信息的指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_ReleasePipeFrame(VI_PIPE ViPipe, const VIDEO_FRAME_INFO_S *pstVideoFrame);


/**
\brief 通过VI PIPE发送Raw数据。
@attention 有差异的接口，接口参数不一样。
\param[in] ViPipe           :   PIPE号。
\param[in] pstVideoFrame    :   RAW数据信息。
\param[in] s32MilliSec      :   超时参数。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SendPipeRaw(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame,AR_S32 s32MilliSec);
/**
\brief 通过VI PIPE 发送raw 给isp 处理，发送后马上返回。
@attention 新加接口
\param[in] ViPipe           :   PIPE号。
\param[in] pstVideoFrame    :   RAW数据信息。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_QPipeRaw(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame);
/**
\brief 通过AR_MPI_VI_QPipeRaw 发送raw 给isp 处理完成后，可以通过AR_MPI_VI_DqPipeRaw 取出来。
@attention 新加接口
\param[in] ViPipe           :   PIPE号。
\param[out] pstVideoFrame    :   RAW数据信息。
\param[out] s32MilliSec    :   超时ms。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_DqPipeRaw(VI_PIPE ViPipe, VIDEO_FRAME_INFO_S *pstVideoFrame,AR_S32 s32MilliSec);


/**
\brief 设置PIPE的3DNR X接口参数。
\param[in] ViPipe       :   PIPE号。
\param[in] pstNrXParam  :   3DNR的X接口参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeNRXParam(VI_PIPE ViPipe, const VI_PIPE_NRX_PARAM_S *pstNrXParam);

/**
\brief 获取PIPE的3DNR X接口参数。
\param[in] ViPipe       :   PIPE号。
\param[out] pstNrXParam :   3DNR的X接口参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeNRXParam(VI_PIPE ViPipe, VI_PIPE_NRX_PARAM_S *pstNrXParam);

/**
\brief 查询VI PIPE状态。
\param[in] ViPipe       :   PIPE号。
\param[out] pstStatus   :   PIPE状态信息。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_QueryPipeStatus(VI_PIPE ViPipe, VI_PIPE_STATUS_S *pstStatus);

/**
\brief 设置VI物理PIPE对接前端sensor或者AD的VC号。
\param[in] ViPipe       :   VI物理PIPE号。
\param[in] u32VCNumber  :   VC号,表示从mipi的那个口出来，最大为3.。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetPipeVCNumber(VI_PIPE ViPipe, AR_U32 u32VCNumber);

/**
\brief 获取VI物理PIPE对接前端sensor或者AD的VC号。
\param[in] ViPipe           :   VI物理PIPE号。
\param[out] pu32VCNumber    :   VC号指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeVCNumber(VI_PIPE ViPipe, AR_U32 *pu32VCNumber);

/**
\brief 设置VI物理PIPE对接前端sensor或者AD的数据类型，对于dvp，这个接口不起作用，dvp 不区分数据类型，对于mipi，这个接口设置实际的mipi 协议的数据类型和mipi的虚拟通道号。
这里的虚拟通道号和AR_MPI_VI_SetPipeVCNumber中的设置不同，u32VCNumber只是指定使用mipi的第几个输出口，而本api配置这个口使用mipi的那个虚拟通道那个数据类型
比如：u32VCNumber=2；u32DTNumber=0x22c；表示mipi的第二个输出口上，输出sensor虚拟通道2，数据类型0x2c（raw12）的数据
\param[in] ViPipe       :   VI物理PIPE号。
\param[in] u32DTNumber  :   mipi实际的虚拟通道号和数据类型，低8bit表示数据类型，高8bit表示虚拟通道号

数据类型如下
typedef enum
{
	CSI_2_OBDATA_8		  = 0x12,
    CSI_2_YUV420_8        = 0x18,
    CSI_2_YUV420_10       = 0x19,
    CSI_2_YUV420_8_LEG    = 0x1A,
    CSI_2_YUV420_8_SHIFT  = 0x1C,
    CSI_2_YUV420_10_SHIFT = 0x1D,
    CSI_2_YUV422_8        = 0x1E,
    CSI_2_YUV422_10       = 0x1F,
    CSI_2_RGB444          = 0x20,
    CSI_2_RGB555          = 0x21,
    CSI_2_RGB565          = 0x22,
    CSI_2_RGB666          = 0x23,
    CSI_2_RGB888          = 0x24,
    CSI_2_RAW6            = 0x28,
    CSI_2_RAW7            = 0x29,
    CSI_2_RAW8            = 0x2A,
    CSI_2_RAW10           = 0x2B,
    CSI_2_RAW12           = 0x2C,
    CSI_2_RAW14           = 0x2D,
    CSI_2_RAW16           = 0x2E
}ENUM_CSI_2_DATA_TYPE_T;

\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_SetPipeDTNumber(VI_PIPE ViPipe, AR_U32 u32DTNumber);
/**
\brief 获取VI物理PIPE对接前端sensor或者AD的数据类型，对于dvp，这个接口不起作用，dvp 不区分数据类型，对于mipi，这个接口设置实际的mipi 协议的数据类型和mipi的虚拟通道号。
这里的虚拟通道号和AR_MPI_VI_SetPipeVCNumber中的设置不同，u32VCNumber只是指定使用mipi的第几个输出口，而本api配置这个口使用mipi的那个虚拟通道那个数据类型
比如：u32VCNumber=2；u32DTNumber=0x22c；表示mipi的第二个输出口上，输出sensor虚拟通道2，数据类型0x2c（raw12）的数据
\param[in] ViPipe       :   VI物理PIPE号。
\param[out] u32DTNumber  :   mipi实际的虚拟通道号和数据类型，低8bit表示数据类型，高8bit表示虚拟通道号
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeDTNumber(VI_PIPE ViPipe, AR_U32 * pu32DTNumber);


/**
\brief 获取VI PIPE文件描述符。
\param[in] ViPipe   :   VI PIPE号。
\retval ::>=0       :   成功，VI PIPE的文件描述符。
\retval ::<0        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetPipeFd(VI_PIPE ViPipe);

/**
\brief 设置VI通道属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[in] pstChnAttr   :   VI通道属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CHN_ATTR_S *pstChnAttr);

/**
\brief 获取通道需要的内存的大小，辅助分配vin vb buffer。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[in] pstChnAttr   :   VI通道属性结构体指针。
\param[in] *s32Size     :   指向获取到的通道buffer 大小的指针
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/


AR_S32 AR_MPI_VI_GetChnBufferSize(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CHN_ATTR_S *pstChnAttr,AR_S32 *s32Size);


/**
\brief 设置VI通道属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[out] pstChnAttr  :   VI通道属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_ATTR_S *pstChnAttr);


/**
\brief 动态改变通道的大小（wh）。本api 必须在isp run 之后调用。本api只能够相对初始通道的大小降低通道的大小，而不能增加。
比如isp run之后通道的大小是 1920 1080. 那么之后只能通过本api，降低到1280 720 。而不能升高到 2560 1250 等。
本api 调用后，下一帧生效，获取的的frame 信息中的成员会指示wh。 比如调用本api的时候，isp正在写第n帧，那么参数会在n+1 frame 的vsync 中配置进去，在n+2帧中生效
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[In] stSize  :        VI通道新的大小
\param[In] stCrop  :        裁剪的大小。本api 可以做到先裁剪到何时的大小，然后按照输出放大或者缩小。全0表示使用默认的crop 策略
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_ReShapeCh(VI_PIPE ViPipe, VI_CHN ViChn, SIZE_S stSize,RECT_S stCrop);



/**
\brief 设置VI通压缩属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[out] pstChnCmpAttr  :   VI通道压缩属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_SetChnCmpAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CH_CF50_CMP_ATTR_T *pstChnCmpAttr);
/**
\brief 获取VI通压缩属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[out] pstChnCmpAttr  :   VI通道压缩属性结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnCmpAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_CH_CF50_CMP_ATTR_T *pstChnCmpAttr);



/**
\brief 启用VI通道。
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_EnableChn(VI_PIPE ViPipe, VI_CHN ViChn);

/**
\brief 禁用VI通道。
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_DisableChn(VI_PIPE ViPipe, VI_CHN ViChn);

/**
\brief 休眠VI通道。空函数，不起实际作用，DDR RENTENTIOND 的时候，在销毁了vin 后调用，起到流程一致性的作用
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SuspendChn(VI_PIPE ViPipe, VI_CHN ViChn, ISP_SUS_MODE_E mode);

/**
\brief 唤醒VI通道。 空函数，不起实际作用，DDR RENTENTIOND 的时候，在重启vin前调用，起到流程一致性的作用
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_ResumeChn(VI_PIPE ViPipe, VI_CHN ViChn, ISP_SUS_MODE_E mode);


/**
\brief 暂停VI通道。暂停vin的某个通道的出图，可以用AR_MPI_VI_RecoverChn 恢复暂停的通道。
       注意：AR_MPI_VI_PauseChn 和 AR_MPI_VI_RecoverChn 只能isp run的时候调用。尽量不要再pause 后
       对vin 进行操作。比如 disabe ch ，dev ，pipe 等。如果想销毁vin，请AR_MPI_VI_RecoverChn 后进行。
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_PauseChn(VI_PIPE ViPipe, VI_CHN ViChn, ISP_SUS_MODE_E mode);

/**
\brief 恢复VI通道。
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_RecoverChn(VI_PIPE ViPipe, VI_CHN ViChn, ISP_SUS_MODE_E mode);

/**
\brief 设置VI通道裁前功能属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[in] pstCropInfo  :   裁剪功能参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CROP_INFO_S  *pstCropInfo);

/**
\brief 获取VI通道裁前功能属性。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[out] pstCropInfo :   裁剪功能参数结构体指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnCrop(VI_PIPE ViPipe, VI_CHN ViChn, VI_CROP_INFO_S  *pstCropInfo);

/**
\brief 设置VI通道低延时属性。
@attention 不支持
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[in] pstLowDelayInfo  :   低延时功能参数结构体指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_SetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, const VI_LOW_DELAY_INFO_S *pstLowDelayInfo);

/**
\brief 设置VI通道低延时属性。
@attention 不支持
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[out] pstLowDelayInfo :   低延时功能参数结构体指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnLowDelayAttr(VI_PIPE ViPipe, VI_CHN ViChn, VI_LOW_DELAY_INFO_S *pstLowDelayInfo);

/**
\brief 从VI通道获取采集的图像。
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[out] pstFrameInfo    :   VI帧信息结构指针。
\param[in] s32MilliSec      :   超时参数。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, VIDEO_FRAME_INFO_S *pstFrameInfo, AR_S32 s32MilliSec);


/**
\brief 创建一个通道的客户，使用这个客户可以从通道的主进程获取数据
@attention 新加接口
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[in] pClientId            :VI 通道客户id的指针
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_CreatChn_Client(VI_PIPE ViPipe, VI_CHN ViChn, AR_S32 *pClientId);

/**
\brief 从VI通道获取采集的图像。要回客户进程向主进程拿通道数据
@attention 新加接口
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[in] ClientId            :VI 通道客户id
\param[out] pstFrameInfo    :   VI帧信息结构指针。
\param[in] s32MilliSec      :   超时参数。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnFrame_ByClient(VI_PIPE ViPipe, VI_CHN ViChn, AR_S32 ClientId,VIDEO_FRAME_INFO_S *pstFrameInfo, AR_S32 s32MilliSec);


/**
\brief 释放一帧从VI通道获取的图像。
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[in] pstFrameInfo :   VI帧信息结构指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_ReleaseChnFrame(VI_PIPE ViPipe, VI_CHN ViChn, const VIDEO_FRAME_INFO_S *pstFrameInfo);

/**
\brief 释放一帧从VI通道获取的图像。用于客户进程释放通道数据
\param[in] ViPipe       :   VI PIPE号。
\param[in] ViChn        :   VI通道号。
\param[in] pstFrameInfo :   VI帧信息结构指针。
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_ReleaseChnFrame_ByClient(VI_PIPE ViPipe, VI_CHN ViChn, AR_S32 ClientId,const VIDEO_FRAME_INFO_S *pstFrameInfo);

/**
\brief 释放一个已经创建好的客户
@attention 新加接口
\param[in] ViPipe           :   VI PIPE号。
\param[in] ViChn            :   VI通道号。
\param[in] ClientId            :VI 通道客户id的指针
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_DeleteChn_Client(VI_PIPE ViPipe, VI_CHN ViChn, AR_S32 ClientId);


/**
\brief 查询VI通道的状态。
\param[in] ViPipe           :   PIPE号。
\param[in] ViChn            :   VI通道号。
\param[out] pstChnStatus    :   VI通道状态的指针。
\retval ::0                 :   成功。
\retval ::non-zero          :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_QueryChnStatus(VI_PIPE ViPipe, VI_CHN ViChn, VI_CHN_STATUS_S *pstChnStatus);

/**
\brief 获取VI通道文件描述符。
\param[in] ViPipe   :   VI PIPE号。
\param[in] ViChn    :   VI通道号。
\retval ::>=0       :   成功，VI CHN的文件描述符。
\retval ::<0        :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_GetChnFd(VI_PIPE ViPipe, VI_CHN ViChn);

/**
\brief 关闭VI文件描述符。
\retval ::0         :   成功。
\retval ::non-zero  :   失败，其值为错误码。
\see \n
N/A
*/
AR_S32 AR_MPI_VI_CloseFd(AR_VOID);


/**
   \brief 设置VI通道输出YUV数据的panel的首地址。
   @attention 新加接口
   \param[in] ViPipe         :   PIPE号。
   \param[in] ViChn          :   VI通道号。
   \param[in] u32BufferIndex :   VI通道buffer索引。
   \param[in] u32PannelIndex :   VI通道buffer的panel索引。
   \param[in] u64PannelPaddr :   VI通道buffer的panel首地址的物理地址。
   \param[in] pstPannelVaddr :   VI通道buffer的panel首地址的虚拟地址。
   \retval ::0               :   成功。
   \retval ::non-zero        :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_SetChnPannelAddr(VI_PIPE ViPipe, VI_CHN ViChn, AR_U32 u32BufferIndex, AR_U32 u32PannelIndex, AR_U64 u64PannelPaddr, void *pstPannelVaddr);

/**
   \brief 获取VI通道输出YUV数据的panel的占用内存长度。
   @attention 新加接口
   \param[in] ViPipe           :   PIPE号。
   \param[in] ViChn            :   VI通道号。
   \param[in] u32PannelIndex   :   VI通道buffer的panel索引。
   \param[out] u32PannelLength :   VI通道buffer的panel的占用内存长度。
   \retval ::0                 :   成功。
   \retval ::non-zero          :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_GetChnPannelLength(VI_PIPE ViPipe, VI_CHN ViChn, AR_U32 u32PannelIndex, AR_U32 *u32PannelLength);

/**
   \brief 设置VI通道输出YUV数据的stride。
   @attention 新加接口
   \param[in] ViPipe           :   PIPE号。
   \param[in] ViChn            :   VI通道号。
   \param[in] u32Stride        :   VI通道buffer的stride。
   \retval ::0                 :   成功。
   \retval ::non-zero          :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_SetChnStride(VI_PIPE ViPipe, VI_CHN ViChn, AR_U32 u32Stride);

/**
   \brief 设置VI通道输出YUV数据使用的pool id。
   @attention 新加接口
   \param[in] ViPipe           :   PIPE号。
   \param[in] ViChn            :   VI通道号。
   \param[in] u32PoolId        :   VI通道的pool id。
   \retval ::0                 :   成功。
   \retval ::non-zero          :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_SetChnPoolId(VI_PIPE ViPipe, VI_CHN ViChn, AR_U32 u32PoolId);

/**
   \brief 设置VI通道输出YUV数据使用的crop rect。
   @attention 新加接口
   \param[in] ViPipe           :   PIPE号。
   \param[in] ViChn            :   VI通道号。
   \param[in] pstCropInfo      :   VI通道的crop rect。
   \retval ::0                 :   成功。
   \retval ::non-zero          :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_SetChnCropManual(VI_PIPE ViPipe, VI_CHN ViChn, const VI_CROP_INFO_S *pstCropInfo);
/**
\brief 触发sensor 运行指定的帧数。
@attention 新加接口
\param[in] ViPipe :   ViPipe 
\param[in]  Frames：  触发运行多少frames
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_VI_TriggerPipe(VI_PIPE ViPipe, AR_S32 Frames);
/**
\brief 刷新isp的参数，一般使用在第一次启动sensor 前，把tuning 参数flush 到isp的寄存器中。配合aec awb 的update 和 AR_MPI_VI_TriggerPipe 使用。
正常情况下，sensor的第一个vsync 会把tuning 参数flush 到寄存器，但是有时候我们希望sensor 第一个vsync 前就把tuning 参数flush 进去。
@attention 新加接口
\param[in] ViPipe :   ViPipe 
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/

AR_S32 AR_MPI_ISP_FlushTuningPra(VI_PIPE ViPipe);
/**
   \brief 获取VI通道高精度的fps
   @attention 新加接口，这个api 会关闭内核所有中断大约一帧的时间，使用不当会造成严重的后果。
   建议在系统只有vin的情况下使用。使用完成后再启动其他模块。以尽可能降低关中断对其他任务的影响.
   本api 必须在pipe 处于offline 才能获取到正确的fps。online 暂时不支持
   \param[in] ViPipe           :   PIPE号。
   \param[in] ViChn            :   VI通道号。 
   \param[in] *fps             ：   获取到的通道的fps，一般是vif的fps
   \param[in] flag             :   bit[0-3]：  1: 关闭所有irq，0：只关闭调用线程说使用的cpu的中断
                                   bit[4-7]:  cpu no. 执行本api的线程bind 到那个cpu 上
                                   bit[8-15]: 测试vsync 的个数
   \retval ::0                 :   成功。
   \retval ::non-zero          :   失败，其值为错误码。
   \see \n
   N/A
*/
AR_S32 AR_MPI_VI_GetHightPricisionPipeFPS(VI_PIPE ViPipe, AR_S32 ViChn, AR_FLOAT *fps,int flag);


/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /*__MPI_VI_H__ */


