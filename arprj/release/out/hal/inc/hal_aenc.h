/**
 * @file hal_aenc.h
 * @brief  hal 音频解码API
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_AENC_H__
#define __HAL_AENC_H__

#include "hal_type.h"
#include "hal_comm_aio.h"
#include "hal_comm_aenc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/**
* @brief  初始化音频编码模块
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_aenc_init();

/**
* @brief  创建音频编码通道
* @param  ae_chn              通道号
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_aenc_create_chn(AR_S32 ae_chn, const STRU_AENC_CHN_ATTR *p_attr);
/**
* @brief  销毁音频编码通道
* @param  ae_chn              通道号
* @return 0 成功 , 其它 失败
* @note   通道未创建的情况下调用此接口会返回成功;如果正在获取/释放码流或者发送帧时销毁该通道，则会返回失败，用户同步处理时需要注意
*/
AR_S32 ar_hal_aenc_destroy_chn(AR_S32 ae_chn);
/**
* @brief  发送音频编码音频帧
* @param  ae_chn              通道号
* @param  p_frm               音频帧结构体指针
* @param  p_aec_frm           回声抵消参考帧结构体指针
* @return 0 成功 , 其它 失败
* @note  音频编码发送数据帧是非阻塞接口，如果音频码流缓存满，则直接返回失败
		 该接口用于用户主动发送音频帧进行编码，如果 AENC 通道已经通过系统绑定（ar_hal_sys_bind）接口与 AI 绑定，不需要也不建议调此接口。
*/
AR_S32 ar_hal_aenc_send_frame(AR_S32 ae_chn, const STRU_AUDIO_FRAME *p_frm, const STRU_AEC_FRAME *p_aec_frm);
/**
* @brief  获取编码后码流
* @param  ae_chn              通道号
* @param  p_stream            获取的码流指针
* @param  s32_milli_sec       获取数据的超时时间
* @return 0 成功 , 其它 失败
* @note   ....
*/
AR_S32 ar_hal_aenc_get_stream(AR_S32 ae_chn, STRU_AUDIO_STREAM *p_stream, AR_S32 s32_milli_sec);
/**
* @brief  释放从音频编码通道获取的码流
* @param  ae_chn              通道号
* @param  p_stream            获取的码流指针
* @return 0 成功 , 其它 失败
* @note   ....
*/
AR_S32 ar_hal_aenc_release_stream(AR_S32 ae_chn, const STRU_AUDIO_STREAM *p_stream);
/**
* @brief  获取音频编码通道号对应的设备文件句柄
* @param  ae_chn              通道号
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_aenc_get_fd(AR_S32 ae_chn);
/**
* @brief  注册解码器
* @param  p_s32_handle        注册句柄
* @param  p_encoder           解码器属性结构体
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_aenc_register_encoder(AR_S32 *p_s32_handle, const STRU_AENC_ENCODER *p_encoder);
/**
* @brief  注销解码器
* @param  p_s32_handle        注册句柄
* @return 0 成功 , 其它 失败
* @note   通常不需要注销解码器;注销解码器前，需要先销毁通过该解码器创建的所有解码通道，未销毁或者销毁过程中调用此接口将返回报错
*/
AR_S32 ar_hal_aenc_unRegister_encoder(AR_S32 s32Handle);
/**
* @brief  获取音频码流buffer相关信息
* @param  ae_chn              通道号
* @param  p_u64_phys_addr     音频码流buffer的物理地址
* @param  p_u32_size          音频码流buffer的长度，以byte为单位
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_aenc_get_stream_buf_info(AR_S32 ae_chn, AR_U64* p_u64_phys_addr, AR_U32* p_u32_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_AENC_H__ */
