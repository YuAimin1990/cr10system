/**
 * @file hal_adec.h
 * @brief  hal 音频编码API
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/30
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifndef __HAL_ADEC_H__
#define __HAL_ADEC_H__

#include "hal_type.h"
#include "hal_comm_aio.h"
#include "hal_comm_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

AR_S32 ar_hal_adec_init();

/**
* @brief  创建音频解码通道
* @param  ad_chn              通道号
* @param  p_attr              通道属性指针
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_adec_create_chn(AR_S32 ad_chn, const STRU_ADEC_CHN_ATTR *p_attr);
/**
* @brief  销毁音频解码通道
* @param  ad_chn              通道号
* @return 0 成功 , 其它 失败
* @note   通道未创建的情况下调用此接口会返回成功;如果正在获取/释放码流或者发送帧，销毁该通道则这些操作都会立即返回失败
*/
AR_S32 ar_hal_adec_destroy_chn(AR_S32 ad_chn);
/**
* @brief  向音频解码通道发送码流
* @param  ad_chn              通道号
* @param  p_stream            音频码流
* @param  b_block             阻塞标识(AR_TRUE：阻塞 AR_FALSE：非阻塞)
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_adec_send_stream(AR_S32 ad_chn, const STRU_AUDIO_STREAM *p_stream, AR_BOOL b_block);
/**
* @brief  清除ADEC通道中当前的音频数据缓存
* @param  ad_chn              通道号
* @return 0 成功 , 其它 失败
* @note   ...
*/
AR_S32 ar_hal_adec_clear_chn_buf(AR_S32 ad_chn);
/**
* @brief  注册解码器
* @param  p_s32_handle         注册句柄
* @param  p_decoder            解码器属性结构体
* @return 0 成功 , 其它 失败
* @note   用户通过传入解码器属性结构体，向ADEC模块注册一个解码器，并返回注册句柄，用户可以最后通过注册句柄来注销该解码器。
*/
AR_S32 ar_hal_adec_register_decoder(AR_S32 *p_s32_handle, const STRU_ADEC_DECODER *p_decoder);
/**
* @brief  注销解码器
* @param  p_s32_handle         注册句柄（注册解码器时获得的句柄）
* @return 0 成功 , 其它 失败
* @note   通常不需要注销解码器;注销解码器前，需要先销毁通过该解码器创建的所有解码通道，未销毁或者销毁过程中调用此接口将返回报错。
*/
AR_S32 ar_hal_adec_unRegister_decoder(AR_S32 s32_handle);
/**
* @brief  获取音频解码帧数据
* @param  ad_chn               音频解码通道
* @param  p_frm_info           音频帧数据结构体
* @param  b_block              是否以阻塞方式获取
* @return 0 成功 , 其它 失败
* @note   ....
*/
AR_S32 ar_hal_adec_get_frame(AR_S32 ad_chn, STRU_AUDIO_FRAME_INFO *p_frm_info, AR_BOOL b_block);
/**
* @brief  释放获取到的音频解码帧数据
* @param  ad_chn               音频解码通道
* @param  p_frm_info           音频帧数据结构体
* @return 0 成功 , 其它 失败
* @note   必须在ADEC通道创建之后调用;本接口必须与接口AR_MPI_ADEC_GetFrame配合使用
*/
AR_S32 ar_hal_adec_release_frame(AR_S32 ad_chn, const STRU_AUDIO_FRAME_INFO *p_frm_info);
/**
* @brief  向解码器发送码流结束标识符，并清除码流buffer
* @param  ad_chn               音频解码通道
* @param  b_instant            是否立即清除解码器内部的缓存数据(AR_FALSE:延时清除 AR_TRUE:立即清除)
* @return 0 成功 , 其它 失败
* @note
*/
AR_S32 ar_hal_adec_send_end_of_stream(AR_S32 ad_chn);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_ADEC_H__ */
