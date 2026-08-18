/**
 * @file hal_aio.h
 * @brief  hal 音频输出输出数据结构及API
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_AIO_H__
#define __HAL_AIO_H__

#include "utils_dbglog.h"
#include "hal_type.h"
#include "hal_comm.h"
#include "hal_comm_aio.h"
#include "hal_sys.h"
#include "hal_vb.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define AENC_ADAPT_MAGIC 0Xfcfcfcfc

#define HAL_TAG_ID(mod_id) ( mod_id | LOG_LAYER_HAL)

typedef enum
{
    AR_AI_IOC_OPEN_DEV = 0,
    AR_AI_IOC_SET_PUB_ATTR,
    AR_AI_IOC_GET_PUB_ATTR,
    AR_AI_IOC_CLR_PUB_ATTR,
    AR_AI_IOC_AI_GET_VB_POOL_ID,
    AR_AI_IOC_AI_ENABLE,
    AR_AI_IOC_AI_DISABLE,
    AR_AI_IOC_AI_CHN_ENABLE,
    AR_AI_IOC_AI_CHN_DISABLE,
    AR_AI_IOC_AI_GET_FRAME,
    AR_AI_IOC_AI_RELEASE_FRAME,
    AR_AI_IOC_AI_SET_CHN_PARAM,
    AR_AI_IOC_AI_GET_CHN_PARAM,
    AR_AI_IOC_AI_SUSPEND_CHN,
    AR_AI_IOC_AI_RESUME_CHN,
    AR_AI_IOC_BUTT,
}ENMU_AI_IOCTL;

typedef struct
{
    AR_S32 s32_dev_id;
    AR_S32 s32_chn_id;
    STRU_AUDIO_FRAME st_audio_frame;
    AR_S32 s32_timeout;
    AR_S32 s32_is_bind;
}STRU_AI_GET_FRAME;

typedef enum
{
    AR_AO_IOC_OPEN_DEV = 0,
    AR_AO_IOC_SET_PUB_ATTR,
    AR_AO_IOC_GET_PUB_ATTR,
    AR_AO_IOC_CLR_PUB_ATTR,
    AR_AO_IOC_AO_ENABLE,
    AR_AO_IOC_AO_DISABLE,
    AR_AO_IOC_AO_CHN_ENABLE,
    AR_AO_IOC_AO_CHN_DISABLE,
    AR_AO_IOC_AO_SEND_FRAME,
    AR_AO_IOC_AO_PAUSE_CHN,
    AR_AO_IOC_AO_RESUME_CHN,
    AR_AO_IOC_AO_CLR_CHN_BUF,
    AR_AO_IOC_AO_QRY_CHN_STAT,
    AR_AO_IOC_AO_SUSPEND_CHN,
    AR_AO_IOC_BUTT,
} ENMU_AO_IOCTL;

typedef enum {
    AR_ACODEC_DEV_ID = 0,
    AR_I2S_MASTER0_ID = 1,
    AR_I2S_MASTER1_ID,
#ifdef AR9341
    AR_I2S_MASTER2_ID,
#endif
    AR_I2S_SLAVE0_ID,
    AR_I2S_SLAVE1_ID,
} AR_AUDIO_DEV_ID;

/**
* @brief  设置内置audio codec寄存器相关参数
* @param  fd Audio Codec 设备文件描述符
* @param  request_code ioctl 号
* @param  args 无符号整型指
* @param  size args的长度
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_acodec_ioctl(int fd, int request_code, void * args, unsigned int size);

/**
* @brief  初始化ai hal层
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_ai_init();

/**
* @brief  去初始化ai hal层
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_ai_deinit();

/**
* @brief  设置音频系统的基础属性
* @param  pstModParam  AUDIO模块参数属性指针
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_audio_set_mod_param(const STRU_AUDIO_MOD_PARAM *p_mod_param);
/**
* @brief  获取音频系统的基础属性
* @param  pstModParam  AUDIO模块参数属性指针
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_audio_get_mod_param(STRU_AUDIO_MOD_PARAM *p_mod_param);

/* AI function api. */
/**
* @brief  设置AI设备属性
* @param  ai_dev_id  音频设备号
* @param  p_attr     AI设备属性指针
* @return 0 成功 , 其它 失败
* @note   音频输入设备的属性决定了输入数据的格式，
*         输入设备属性包括工作模式、采样率、采样精度、buffer大小、每帧的采样点数、扩展标志、时钟选择和通道数目。
*         这些属性应与对接Codec配置的时序一致，即能成功对接。
*/
AR_S32 ar_hal_ai_set_pub_attr(AR_S32 ai_dev_id, const STRU_AIO_ATTR *p_attr);
/**
* @brief  获取AI设备属性
* @param  ai_dev_id  音频设备号
* @param  p_attr     AI设备属性指针
* @return 0 成功 , 其它 失败
* @note   获取的属性为前一次配置的属性，如果从来没有配置过属性，则返回失败
*/
AR_S32 ar_hal_ai_get_pub_attr(AR_S32 ai_dev_id, STRU_AIO_ATTR *p_attr);
/**
* @brief  启用AI设备
* @param  ai_dev_id  音频设备号
* @return 0 成功 , 其它 失败
* @note   必须在启用前配置AI设备属性，否则返回属性未配置错误
*         如果AI设备已经处于启用状态，则直接返回成功
*/
AR_S32 ar_hal_ai_enable(AR_S32 ai_dev_id);
/**
* @brief  禁用AI设备
* @param  ai_dev_id  音频设备号
* @return 0 成功 , 其它 失败
* @note   如果AI设备已经处于禁用状态，则直接返回成功
          要求在禁用AI设备之前，先禁用与之关联、使用AI的音频数据的AENC通道和AO设备，否则可能导致该接口调用失败
          禁用AI设备前必须先禁用该设备下已启用的所有AI通道
*/
AR_S32 ar_hal_ai_disable(AR_S32 ai_dev_id);
/**
* @brief  启用AI通道
* @param  ai_dev_id  音频设备号
* @param  ai_chn     音频输入通道号
* @return 0 成功 , 其它 失败
* @note   启用AI通道前，必须先启用其所属的AI设备，否则返回设备未启动的错误码
*/
AR_S32 ar_hal_ai_enable_chn(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  禁用AI通道
* @param  ai_dev_id  音频设备号
* @param  ai_chn     音频输入通道号
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_disable_chn(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  获取音频帧
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_frm          音频帧结构体指针
* @param  p_aec_frm      回声抵消参考帧结构体指针
* @param  s32_milli_sec  获取数据的超时时间
*                        -1表示阻塞模式，无数据时一直等待；
*                        0表示非阻塞模式，无数据时则报错返回；
*                        >0表示阻塞s32MilliSec毫秒，超时则报错返回
* @return 0 成功 , 其它 失败
* @note   获取音频帧数据前，必须先使能对应的AI通道
*/
AR_S32 ar_hal_ai_get_frame(AR_S32 ai_dev_id, AR_S32 ai_chn, STRU_AUDIO_FRAME *p_frm, STRU_AEC_FRAME *p_aec_frm, AR_S32 s32_milli_sec);
/**
* @brief  释放音频帧
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_frm          音频帧结构体指针
* @param  p_aec_frm      回声抵消参考帧结构体指针
* @return 0 成功 , 其它 失败
* @note   如果不需要释放回声抵消参考帧，pstAecFrm置为NULL即可
*/
AR_S32 ar_hal_ai_release_frame(AR_S32 ai_dev_id, AR_S32 ai_chn, const STRU_AUDIO_FRAME *p_frm, const STRU_AEC_FRAME *p_aec_frm);
/**
* @brief  设置AI通道参数
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_chn_param    音频通道参数
* @return 0 成功 , 其它 失败
* @note   通道参数目前只有一个成员变量，用于设置用户获取音频帧的缓存深度，默认深度为0。该成员变量的值不能大于30
*/
AR_S32 ar_hal_ai_set_chn_param(AR_S32 ai_dev_id, AR_S32 ai_chn, const STRU_AI_CHN_PARAM *p_chn_param);
/**
* @brief  获取AI通道参数
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_chn_param    音频通道参数
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_get_chn_param(AR_S32 ai_dev_id, AR_S32 ai_chn, STRU_AI_CHN_PARAM *p_chn_param);
/**
* @brief  设置AI的声音质量增强功能（Record）相关属性
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_vqe_config   音频输入声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_set_record_vqe_attr(AR_S32 ai_dev_id, AR_S32 ai_chn, const STRU_AI_RECORDVQE_CONFIG *p_vqe_config);
/**
* @brief  获取AI的声音质量增强功能（Record）相关属性
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @param  p_vqe_config   音频输入声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_get_record_vqe_attr(AR_S32 ai_dev_id, AR_S32 ai_chn, STRU_AI_RECORDVQE_CONFIG *p_vqe_config);
/**
* @brief  使能AI的声音质量增强功能
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @return 0 成功 , 其它 失败
* @note   禁用AI通道后，如果重新启用AI通道，并使用声音质量增强功能，需调用此接口重新启用声音质量增强功能
*/
AR_S32 ar_hal_ai_enable_vqe(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  禁用AI的声音质量增强功能
* @param  ai_dev_id      音频设备号
* @param  ai_chn         音频输入通道号
* @return 0 成功 , 其它 失败
* @note   不再使用AI声音质量增强功能时，应该调用此接口将其禁用
*/
AR_S32 ar_hal_ai_disable_vqe(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  启用AI重采样
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  en_out_sample_rate  音频重采样的输出采样率
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_enable_resmp(AR_S32 ai_dev_id, AR_S32 ai_chn, ENUM_AUDIO_SAMPLE_RATE en_out_sample_rate);
/**
* @brief  禁用AI重采样
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @return 0 成功 , 其它 失败
* @note   要求在调用此接口之前，先禁用使用该AI设备相应通道音频数据的AENC通道和AO通道，否则可能导致该接口调用失败
*/
AR_S32 ar_hal_ai_disable_resmp(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  设置AI声道模式
* @param  ai_dev_id           音频设备号
* @param  en_track_mode       音频输入声道模式
* @return 0 成功 , 其它 失败
* @note   在AI设备成功启用后再调用此接口;AI设备工作在I2S模式时，支持获取声道模式，PCM模式下不支持
*/
AR_S32 ar_hal_ai_set_track_mode(AR_S32 ai_dev_id, ENUM_AUDIO_TRACK_MODE en_track_mode);
/**
* @brief  获取AI声道模式
* @param  ai_dev_id           音频设备号
* @param  p_en_track_mode     音频输入声道模式指针
* @return 0 成功 , 其它 失败
* @note   在AI设备成功启用后再调用此接口;AI设备工作在I2S模式时，支持设置声道模式，PCM模式下不支持
*/
AR_S32 ar_hal_ai_get_track_mode(AR_S32 ai_dev_id, ENUM_AUDIO_TRACK_MODE *p_en_track_mode);
/**
* @brief  开启音频输入保存文件功能
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  p_save_file_info    音频保存文件属性结构体指针
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_save_file(AR_S32 ai_dev_id, AR_S32 ai_chn, const STRU_AUDIO_SAVE_FILE_INFO *p_save_file_info);
/**
* @brief  查询音频输入通道是否处于存文件的状态
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  p_file_status       状态属性结构体指针
* @return 0 成功 , 其它 失败
* @note   如果pstFileStatus的bSaving为AR_TRUE，说明还没有达到指定大小，为AR_FALSE则已经达到指定大小
*/
AR_S32 ar_hal_ai_query_file_status(AR_S32 ai_dev_id, AR_S32 ai_chn, STRU_AUDIO_FILE_STATUS* p_file_status);
/**
* @brief  清空Pub属性
* @param  ai_dev_id           音频设备号
* @return 正数值为有效返回值 , 其它 无效返回值
* @note   清除设备属性前，需要先停止设备
*/
AR_S32 ar_hal_ai_clr_pub_attr(AR_S32 ai_dev_id);
/**
* @brief  获取音频输入通道号对应的设备文件句柄
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @return 正数 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ai_get_fd(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  在AEC不打开的情况下也使用户能获取到AEC参考帧
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  ao_dev_id           用于获取AEC参考帧的AO设备
* @param  ao_chn              用于获取AEC参考帧的AO通道号
* @return 0 成功 , 其它 失败
* @note   此接口仅用于在AEC功能关闭的情况下使能获取AEC参考帧，AEC参考帧通过接口AR_MPI_AI_GetFrame返回，供用户在上层自己做AEC处理;
*         不支持立体声
*/
AR_S32 ar_hal_ai_enable_aec_ref_frame(AR_S32 ai_dev_id, AR_S32 ai_chn, AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  在AEC不打开的情况下禁止获取AEC参考帧
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @return 0 成功 , 其它 失败
* @note   重复调用本接口返回成功
*/
AR_S32 ar_hal_ai_disable_aec_ref_frame(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  设置AI的声音质量增强功能（Talk）相关属性
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  ao_dev_id           用于回声抵消的AO设备号
* @param  ao_chn              用于回声抵消的AO通道号
* @param  p_vqe_config        音频输入声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note   Talk Vqe主要在IPC场景下使用
*/
AR_S32 ar_hal_ai_set_talk_vqe_attr(AR_S32 ai_dev_id, AR_S32 ai_chn, AR_S32 AoDevId, AR_S32 ao_chn, const STRU_AI_TALKVQE_CONFIG *p_vqe_config);
/**
* @brief  获取AI的声音质量增强功能（Talk）相关属性
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @param  p_vqe_config        音频输入声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note   Talk Vqe主要在IPC场景下使用
*/
AR_S32 ar_hal_ai_get_talk_vqe_attr(AR_S32 ai_dev_id, AR_S32 ai_chn, STRU_AI_TALKVQE_CONFIG *p_vqe_config);
/**
* @brief  AI设备的通道进入休眠，降低功耗
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @return 0 成功 , 其它 失败
* @note   适用于快速开关机场景
*/
AR_S32 ar_hal_ai_suspend_chn(AR_S32 ai_dev_id, AR_S32 ai_chn);
/**
* @brief  唤醒AI设备的通道
* @param  ai_dev_id           音频设备号
* @param  ai_chn              音频输入通道号
* @return 0 成功 , 其它 失败
* @note   适用于快速开关机场景
*/
AR_S32 ar_hal_ai_resume_chn(AR_S32 ai_dev_id, AR_S32 ai_chn);
/*===================== AO function api. ============================*/
/**
* @brief  初始化ao hal层
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_ao_init();

/**
* @brief  去初始化ao hal层
* @return 0 成功 , 其它 失败
*/
AR_S32 ar_hal_ao_deinit();

/**
* @brief  设置AO设备属性
* @param  ao_dev_id           音频设备号
* @param  p_attr              音频输出设备属性
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_ao_set_pub_attr(AR_S32 ao_dev_id, const STRU_AIO_ATTR *p_attr);
/**
* @brief  获取AO设备属性
* @param  ao_dev_id           音频设备号
* @param  p_attr              音频输出设备属性
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_ao_get_pub_attr(AR_S32 ao_dev_id, STRU_AIO_ATTR *p_attr);
/**
* @brief  启用AO设备
* @param  ao_dev_id           音频设备号
* @return 0 成功 , 其它 失败
* @note   要求在启用前配置AO设备属性，否则会返回属性未配置的错误;如果AO设备已经启用，则直接返回成功
*/
AR_S32 ar_hal_ao_enable(AR_S32 ao_dev_id);
/**
* @brief  禁用AO设备
* @param  ao_dev_id           音频设备号
* @return 0 成功 , 其它 失败
* @note   如果AO设备已经禁用，则直接返回成功;禁用AO设备前必须先禁用设备下所有AO通道
*/
AR_S32 ar_hal_ao_disable(AR_S32 ao_dev_id);
/**
* @brief  启用AO通道
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   启用AO通道前，必须先启用其所属的AO设备，否则返回设备未启动的错误码
*/
AR_S32 ar_hal_ao_enable_chn(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  禁用AO通道
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ao_disable_chn(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  发送AO音频帧
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @param  p_data              音频帧结构体指针
* @param  s32_milli_sec       发送数据的超时时间(-1表示阻塞模式;0表示非阻塞模式;>0表示阻塞s32MilliSec毫秒，超时则报错返回)
* @return 0 成功 , 其它 失败
* @note   该接口用于用户主动发送音频帧至AO输出，
*         如果AO通道已经通过系统绑定（AR_MPI_SYS_Bind）接口与AI或ADEC绑定，不需要也不建议调此接口
*/
AR_S32 ar_hal_ao_send_frame(AR_S32 ao_dev_id, AR_S32 ao_chn, const STRU_AUDIO_FRAME *p_data, AR_S32 s32_milli_sec);
/**
* @brief  启用AO重采样
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @param  en_in_sample_rate   音频重采样的输入采样率
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_ao_enable_resmp(AR_S32 ao_dev_id, AR_S32 ao_chn, ENUM_AUDIO_SAMPLE_RATE en_in_sample_rate);
/**
* @brief  禁用AO重采样
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   不再使用AO重采样功能的话，应该调用此接口将其禁用
*/
AR_S32 ar_hal_ao_disable_resmp(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  清除AO通道中当前的音频数据缓存
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   为完全清除解码回放通路上所有缓存数据，此接口还应该与AR_MPI_ADEC_ClearChnBuf接口配合使用
*/
AR_S32 ar_hal_ao_clear_chn_buf(AR_S32 ao_dev_id ,AR_S32 ao_chn);
/**
* @brief  查询AO通道中当前的音频数据缓存状态
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @param  pstStatus           缓存状态结构体指针
* @return 0 成功 , 其它 失败
* @note   在AO通道成功启用后再调用此接口
*/
AR_S32 ar_hal_ao_query_chn_stat(AR_S32 ao_dev_id ,AR_S32 ao_chn, STRU_AO_CHN_STATE *pstStatus);
/**
* @brief  AO设备的通道休眠
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   适用于快速开关机，AO通道休眠后，如果绑定的ADEC通道继续向此通道发送音频帧数据，发送的音频帧数据将会被阻塞；
*         而如果绑定的AI通道继续向此通道发送音频帧数据，在通道缓冲未满的情况下则将音频帧放入缓冲区，在满的情况下则将音频帧丢弃。
*/
AR_S32 ar_hal_ao_suspend_chn(AR_S32 ao_dev_id, AR_S32 ao_chn);

/**
* @brief  暂停AO通道
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   AO通道暂停后，如果绑定的ADEC通道继续向此通道发送音频帧数据，发送的音频帧数据将会被阻塞；
*         而如果绑定的AI通道继续向此通道发送音频帧数据，在通道缓冲未满的情况下则将音频帧放入缓冲区，在满的情况下则将音频帧丢弃。
*/
AR_S32 ar_hal_ao_pause_chn(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  唤醒AO设备的通道
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   适用于快速开关机，AO通道休眠后可以通过调用此接口重新恢复
*/
AR_S32 ar_hal_ao_resume_chn(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  设置AO设备音量大小
* @param  ao_dev_id           音频设备号
* @param  s32_volume_db       音频设备音量大小（以Db为单位）取值范围：[-121,6]
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口
*/
AR_S32 ar_hal_ao_set_volume(AR_S32 ao_dev_id, AR_S32 s32_volume_db);
/**
* @brief  获取AO设备音量大小
* @param  ao_dev_id           音频设备号
* @param  p_s32_volume_db     音频设备音量大小指针
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口
*/
AR_S32 ar_hal_ao_get_volume(AR_S32 ao_dev_id, AR_S32 *p_s32_volume_db);
/**
* @brief  设置AO设备静音状态
* @param  ao_dev_id           音频设备号
* @param  b_enable            音频设备是否启用静音
* @param  p_fade             淡入淡出结构体指针
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口;调用此接口时，用户可以选择是否使用淡入淡出功能，如果不使用淡入淡出则将结构体指针赋为空即可。
*/
AR_S32 ar_hal_ao_set_mute(AR_S32 ao_dev_id, AR_BOOL b_enable, const STRU_AUDIO_FADE *p_fade);
/**
* @brief  获取AO设备静音状态
* @param  ao_dev_id           音频设备号
* @param  p_b_enable          音频设备静音状态指针
* @param  p_fade             淡入淡出结构体指针
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口
*/
AR_S32 ar_hal_ao_get_mute(AR_S32 ao_dev_id, AR_BOOL *p_b_enable, STRU_AUDIO_FADE *p_fade);
/**
* @brief  设置AO设备声道模式
* @param  ao_dev_id           音频设备号
* @param  en_track_mode       音频设备声道模式
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口;AO设备工作在I2S模式时，支持设置声道模式，PCM模式下不支持
*/
AR_S32 ar_hal_ao_set_track_mode(AR_S32 ao_dev_id, ENUM_AUDIO_TRACK_MODE en_track_mode);
/**
* @brief  获取AO设备声道模式
* @param  ao_dev_id           音频设备号
* @param  p_en_track_mode     音频设备声道模式指针
* @return 0 成功 , 其它 失败
* @note   在AO设备成功启用后再调用此接口;AO设备工作在I2S模式时，支持设置声道模式，PCM模式下不支持
*/
AR_S32 ar_hal_ao_get_track_mode(AR_S32 ao_dev_id, ENUM_AUDIO_TRACK_MODE *p_en_track_mode);
/**
* @brief  获取音频输出通道号对应的设备文件句柄
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 正数 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_ao_get_fd(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  清除AO设备属性
* @param  ao_dev_id           音频设备号
* @return 0 成功 , 其它 失败
* @note   清除设备属性前，需要先停止设备; ...
*/
AR_S32 ar_hal_ao_clr_pub_attr(AR_S32 ao_dev_id);
/**
* @brief  设置AO的声音质量增强功能相关属性
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @param  p_vqe_config        音频输出声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_ao_set_vqe_attr(AR_S32 ao_dev_id, AR_S32 ao_chn, const STRU_AO_VQE_CONFIG *p_vqe_config);
/**
* @brief  获取AO的声音质量增强功能相关属性
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @param  p_vqe_config        音频输出声音质量增强配置结构体指针
* @return 0 成功 , 其它 失败
* @note   获取声音质量增强功能相关属性前必须先设置相对应AO通道的声音质量增强功能相关属性
*/
AR_S32 ar_hal_ao_get_vqe_attr(AR_S32 ao_dev_id, AR_S32 ao_chn, STRU_AO_VQE_CONFIG *p_vqe_config);
/**
* @brief  使能AO的声音质量增强功能
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_ao_enable_vqe(AR_S32 ao_dev_id, AR_S32 ao_chn);
/**
* @brief  禁用AO的声音质量增强功能
* @param  ao_dev_id           音频设备号
* @param  ao_chn              音频输出通道号
* @return 0 成功 , 其它 失败
* @note   不再使用AO声音质量增强功能时，应该调用此接口将其禁用;多次禁用相同AO通道的声音质量增强功能，返回成功
*/
AR_S32 ar_hal_ao_disable_vqe(AR_S32 ao_dev_id, AR_S32 ao_chn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_AIO_H__ */
