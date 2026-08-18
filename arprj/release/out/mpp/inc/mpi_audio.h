/**
 * \file
 * \brief 描述音频相关的数据结构和接口.
 */

#ifndef __MPI_AUDIO_H__
#define __MPI_AUDIO_H__

#include "hal_type.h"
#include "ar_common.h"
#include "ar_comm_aio.h"
#include "ar_comm_aenc.h"
#include "ar_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */


/******************************* API declaration *****************************/
/**
\addtogroup MPI_AUDIO
 * @brief 包括音频输入、音频输出、音频编码、音频解码、内置Audio Codec等子模块。
 * @{
*/

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc


/* AI function api. */
/**
\brief  设置 Audio 模块参数属性
\param[in]  pstModParam     :AUDIO 模块参数属性指针
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AUDIO_SetModParam(const AUDIO_MOD_PARAM_S *pstModParam);

/**
\brief  获取 AUDIO 模块参数属性
\param[out]  pstModParam     :AUDIO 模块参数属性指针
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AUDIO_GetModParam(AUDIO_MOD_PARAM_S *pstModParam);


/* AI function api. */
/**
\brief  设置AI设备属性
        音频输入设备的属性决定了输入数据的格式，
        输入设备属性包括工作模式、采样率、采样精度、buffer大小、每帧的采样点数、扩展标志、时钟选择和通道数目。
        这些属性应与对接Codec配置的时序一致，即能成功对接。
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  pstAttr     :AI设备属性指针
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr);
/**
\brief  获取AI设备属性，获取的属性为前一次配置的属性，如果从来没有配置过属性，则返回失败
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[out] pstAttr     :AI设备属性指针
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr);
/**
\brief  启用AI设备
        必须在启用前配置AI设备属性，否则返回属性未配置错误
        如果AI设备已经处于启用状态，则直接返回成功
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_Enable(AUDIO_DEV AiDevId);
/**
\brief  禁用AI设备
        如果AI设备已经处于禁用状态，则直接返回成功
        要求在禁用AI设备之前，先禁用与之关联、使用AI的音频数据的AENC通道和AO设备，否则可能导致该接口调用失败
        禁用AI设备前必须先禁用该设备下已启用的所有AI通道
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel

\see \n
N/A
*/
AR_S32 AR_MPI_AI_Disable(AUDIO_DEV AiDevId);
/**
\brief  启用AI通道，启用AI通道前，必须先启用其所属的AI设备，否则返回设备未启动的错误码
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
/**
\brief  禁用AI通道
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
/**
\brief  获取音频帧，获取音频帧数据前，必须先使能对应的AI通道
@attention  有差异的接口，不支持回声抵消，其参考帧结构体指针，必须为NULL
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\param[out] pstFrm      :音频帧结构体指针
\param[out] pstAecFrm   :回声抵消参考帧结构体指针(不支持，必须为NULL)
\param[in]  s32MilliSec :获取数据的超时时间
*                        -1表示阻塞模式，无数据时一直等待；
*                        0表示非阻塞模式，无数据时则报错返回；
*                        >0表示阻塞s32MilliSec毫秒，超时则报错返回
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, AR_S32 s32MilliSec);
/**
\brief  释放音频帧
@attention  有差异的接口，不支持回声抵消，其参考帧结构体指针，必须为NULL
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\param[in]  pstFrm      :音频帧结构体指针
\param[in]  pstAecFrm   :回声抵消参考帧结构体指针(不支持，必须为NULL)
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);
/**
\brief  设置AI通道参数，通道参数目前只有一个成员变量，用于设置用户获取音频帧的缓存深度，默认深度为0。该成员变量的值不能大于30
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\param[in]  pstChnParam :音频通道参数
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_CHN_PARAM_S *pstChnParam);
/**
\brief  获取AI通道参数
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
\param[out] pstChnParam :音频通道参数
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam);

/*AR_S32 AR_MPI_AI_SetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_RECORDVQE_CONFIG_S *pstVqeConfig);
AR_S32 AR_MPI_AI_GetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_RECORDVQE_CONFIG_S *pstVqeConfig);

AR_S32 AR_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);
AR_S32 AR_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn);

AR_S32 AR_MPI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate);
AR_S32 AR_MPI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn);

AR_S32 AR_MPI_AI_SetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E enTrackMode);
AR_S32 AR_MPI_AI_GetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E *penTrackMode);
AR_S32 AR_MPI_AI_SaveFile(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo);
AR_S32 AR_MPI_AI_QueryFileStatus(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FILE_STATUS_S* pstFileStatus);*/
/**
\brief  清空Pub属性
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
* @return 正数值为有效返回值 , 其它 无效返回值
* @note   清除设备属性前，需要先停止设备
\note   当前每个device只支持一个channel
*/
AR_S32 AR_MPI_AI_ClrPubAttr(AUDIO_DEV AiDevId);
/**
\brief  获取音频输入通道号对应的设备文件句柄
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
* @return 正数 成功 , 其它 失败
* @note
\note   当前每个device只支持一个channel
*/
AR_S32 AR_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn);
/**
\brief  音频输入设备对应通道进入休眠
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
* @return 正数 成功 , 其它 失败
* @note
\note   当前每个device只支持一个channel
*/
AR_S32 AR_MPI_AI_SuspendChn(AUDIO_DEV AiDevId, AI_CHN AiChn);
/**
\brief  唤醒音频输入设备对应通道
\param[in]  AiDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AiChn       :音频输入通道号
* @return 正数 成功 , 其它 失败
* @note
\note   当前每个device只支持一个channel
*/
AR_S32 AR_MPI_AI_ResumeChn(AUDIO_DEV AiDevId, AI_CHN AiChn);

/*AR_S32 AR_MPI_AI_EnableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn);
AR_S32 AR_MPI_AI_DisableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn);

AR_S32 AR_MPI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig);
AR_S32 AR_MPI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_TALKVQE_CONFIG_S *pstVqeConfig);*/


/* AO function api. */
/**
\brief  设置AO设备属性
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  pstAttr     :音频输出设备属性
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_SetPubAttr(AUDIO_DEV AoDevId, const AIO_ATTR_S *pstAttr);
/**
\brief  获取AO设备属性
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[out] pstAttr     :音频输出设备属性
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_GetPubAttr(AUDIO_DEV AoDevId, AIO_ATTR_S *pstAttr);
/**
\brief  启用AO设备，要求在启用前配置AO设备属性，否则会返回属性未配置的错误;如果AO设备已经启用，则直接返回成功
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_Enable(AUDIO_DEV AoDevId);
/**
\brief  禁用AO设备，如果AO设备已经禁用，则直接返回成功;禁用AO设备前必须先禁用设备下所有AO通道
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_Disable(AUDIO_DEV AoDevId);
/**
\brief  启用AO通道，启用AO通道前，必须先启用其所属的AO设备，否则返回设备未启动的错误码
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_EnableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
\brief  禁用AO通道
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_DisableChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
\brief  发送AO音频帧；
        该接口用于用户主动发送音频帧至AO输出，
        如果AO通道已经通过系统绑定（AR_MPI_SYS_Bind）接口与AI或ADEC绑定，不需要也不建议调此接口
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\param[in]  pstData     :音频帧结构体指针
\param[in]  s32MilliSec :发送数据的超时时间(-1表示阻塞模式;0表示非阻塞模式;>0表示阻塞s32MilliSec毫秒，超时则报错返回)
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_SendFrame(AUDIO_DEV AoDevId, AO_CHN AoChn, const AUDIO_FRAME_S *pstData, AR_S32 s32MilliSec);

/*AR_S32 AR_MPI_AO_EnableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn, AUDIO_SAMPLE_RATE_E enInSampleRate);
AR_S32 AR_MPI_AO_DisableReSmp(AUDIO_DEV AoDevId, AO_CHN AoChn);*/

/**
\brief  清除AO通道中当前的音频数据缓存；
        为完全清除解码回放通路上所有缓存数据，此接口还应该与AR_MPI_ADEC_ClearChnBuf接口配合使用
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       : 音频输出通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_ClearChnBuf(AUDIO_DEV AoDevId ,AO_CHN AoChn);
/**
\brief  查询AO通道中当前的音频数据缓存状态；
        在AO通道成功启用后再调用此接口
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\param[out] pstStatus 缓存状态结构体指针
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_QueryChnStat(AUDIO_DEV AoDevId ,AO_CHN AoChn, AO_CHN_STATE_S *pstStatus);
/**
\brief  音频输出设备对应通道进入休眠
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输入通道号
* @return 正数 成功 , 其它 失败
* @note
\note   当前每个device只支持一个channel
*/
AR_S32 AR_MPI_AO_SuspendChn(AUDIO_DEV AoDevId, AI_CHN AoChn);
/**
\brief  暂停AO通道，AO通道暂停后；
        如果绑定的ADEC通道继续向此通道发送音频帧数据，发送的音频帧数据将会被阻塞；
        而如果绑定的AI通道继续向此通道发送音频帧数据，在通道缓冲未满的情况下则将音频帧放入缓冲区，在满的情况下则将音频帧丢弃。
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\retval ::  >=0         :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_PauseChn(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
\brief  恢复AO通道，AO通道暂停后可以通过调用此接口重新恢复
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\param[in]  AoChn       :音频输出通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_ResumeChn(AUDIO_DEV AoDevId, AO_CHN AoChn);

/*AR_S32 AR_MPI_AO_SetVolume(AUDIO_DEV AoDevId, AR_S32 s32VolumeDb);
AR_S32 AR_MPI_AO_GetVolume(AUDIO_DEV AoDevId, AR_S32 *ps32VolumeDb);

AR_S32 AR_MPI_AO_SetMute(AUDIO_DEV AoDevId, AR_BOOL bEnable, const AUDIO_FADE_S *pstFade);
AR_S32 AR_MPI_AO_GetMute(AUDIO_DEV AoDevId, AR_BOOL *pbEnable, AUDIO_FADE_S *pstFade);

AR_S32 AR_MPI_AO_SetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E enTrackMode);
AR_S32 AR_MPI_AO_GetTrackMode(AUDIO_DEV AoDevId, AUDIO_TRACK_MODE_E *penTrackMode);*/
/**
\brief  获取音频输出通道号对应的设备文件句柄
\param[in]  AoDevId     :音频设备号
\param[in]  AoChn       :音频输出通道号
\retval ::  >=0         :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_GetFd(AUDIO_DEV AoDevId, AO_CHN AoChn);
/**
\brief  清除AO设备属性，清除设备属性前，需要先停止设备
\param[in]  AoDevId     :音频设备号，参考AR_AIO_DEV_ID
\retval ::  0           :成功
\retval ::  other       :失败
\note   当前每个device只支持一个channel
\see \n
N/A
*/
AR_S32 AR_MPI_AO_ClrPubAttr(AUDIO_DEV AoDevId);


/*AR_S32 AR_MPI_AO_SetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, const AO_VQE_CONFIG_S *pstVqeConfig);
AR_S32 AR_MPI_AO_GetVqeAttr(AUDIO_DEV AoDevId, AO_CHN AoChn, AO_VQE_CONFIG_S *pstVqeConfig);

AR_S32 AR_MPI_AO_EnableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);
AR_S32 AR_MPI_AO_DisableVqe(AUDIO_DEV AoDevId, AO_CHN AoChn);*/


/**
\brief  创建音频编码通道
\param[in]  AeChn     :通道号
\param[in]  pstAttr   :音频编码通道属性指针
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr);

/**
\brief  销毁音频编码通道
\param[in]  AeChn     :通道号
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_DestroyChn(AENC_CHN AeChn);

/**
\brief  发送音频编码音频帧
\param[in]  AeChn      :通道号
\param[in]  pstFrm     :发送音频编码音频帧
\param[in]  pstAecFrm  :回声抵消参考帧结构体指针
\retval ::  0          :成功
\retval ::  other      :失败
\note 回声抵消功能暂未实现，参数不生效
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm);

/**
\brief  获取编码后码流
\param[in]  AeChn       :通道号
\param[out]  pstStream   :获取的音频码流
\param[in]  s32MilliSec :获取数据的超时时间：-1 表示阻塞模式，无数据时一直等待； 0 表示非阻塞模式，无数据时则报错返回；>0 表示阻塞 s32MilliSec 毫秒，超时则报错返回
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream, AR_S32 s32MilliSec);

/**
\brief  释放从音频编码通道获取的码流
\param[in]  AeChn       :通道号
\param[in]  pstStream   :获取的音频码流
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream);

/**
\brief  获取音频编码通道号对应的设备文件句柄
\param[in]  AeChn       :通道号
\retval ::  0           :成功
\retval ::  other       :失败
\note 该接口未实现
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_GetFd(AENC_CHN AeChn);

/**
\brief  注册编码器
\param[in]  ps32Handle  :注册句柄
\param[in]  pstEncoder  :编码器属性结构体指针
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_RegisterEncoder(AR_S32 *ps32Handle, const AENC_ENCODER_S *pstEncoder);

/**
\brief  注销编码器
\param[in]  ps32Handle  :注册句柄
\retval ::  0           :成功
\retval ::  other       :失败
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_UnRegisterEncoder(AR_S32 s32Handle);

/**
\brief  获取音频码流 buffer 相关信息
\param[in]  AeChn        :通道号
\param[out]  pu64PhysAddr :音频buffer物理地址
\param[out]  pu32Size     :音频buffer的长度，以byte为单位
\retval ::  0            :成功
\retval ::  other        :失败
\note 该接口未实现
\see \n
N/A
*/
AR_S32 AR_MPI_AENC_GetStreamBufInfo(AENC_CHN AeChn, AR_U64* pu64PhysAddr, AR_U32* pu32Size);


/**
\brief  创建音频解码通道
\param[in]  AeChn     :通道号
\param[in]  pstAttr   :音频编码通道属性指针
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr);

/**
\brief  销毁音频解码通道
\param[in]  AeChn     :通道号
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_DestroyChn(ADEC_CHN AdChn);

/**
\brief  向音频解码通道发送音频
\param[in]  AeChn     :通道号
\param[in]  pstStream :音频音频
\param[in]  bBlock    :阻塞标识
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, AR_BOOL bBlock);

/**
\brief  清除 ADEC 通道中当前的音频数据缓存
\param[in]  AeChn     :通道号
\retval ::  0         :成功
\retval ::  other     :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_ClearChnBuf(ADEC_CHN AdChn);

/**
\brief  注册解码器
\param[in]  ps32Handle :注册句柄
\param[in]  pstDecoder :解码属性结构体指针
\retval ::  0          :成功
\retval ::  other      :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_RegisterDecoder(AR_S32 *ps32Handle, const ADEC_DECODER_S *pstDecoder);

/**
\brief  注销解码器
\param[in]  ps32Handle :注册句柄
\retval ::  0          :成功
\retval ::  other      :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_UnRegisterDecoder(AR_S32 s32Handle);

/**
\brief  获取音频解码帧数据
\param[in]  AdChn      :音频解码通道
\param[out]  pstDecoder :音频帧数据结构体指针
\param[in]  bBlock     :阻塞标识
\retval ::  0          :成功
\retval ::  other      :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_INFO_S *pstFrmInfo, AR_BOOL bBlock);

/**
\brief  释放获取到的音频解码帧数据
\param[in]  AdChn      :音频解码通道
\param[in]  pstDecoder :音频帧数据结构体指针
\retval ::  0          :成功
\retval ::  other      :失败
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_ReleaseFrame(ADEC_CHN AdChn, const AUDIO_FRAME_INFO_S *pstFrmInfo);

/**
\brief  向解码器发送码流结束
\param[in]  AdChn      :音频解码通道
\param[in]  bInstant   :是否立即清除解码器内部的缓存数据
\retval ::  0          :成功
\retval ::  other      :失败
\note bInstant参数暂未生效
\see \n
N/A
*/
AR_S32 AR_MPI_ADEC_SendEndOfStream(ADEC_CHN AdChn, AR_BOOL bInstant);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MPI_AI_H__ */

