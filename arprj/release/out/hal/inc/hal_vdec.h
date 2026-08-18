/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VDEC
 *  @{
 */

/**
 * @file hal_vdec.h
 * @author  Artosyn Software Team
 * @date 2021/04/21
 * @brief 定义VDEC 控制API;
 */

#ifndef _HAL_VDEC_H_
#define _HAL_VDEC_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_vcodec_type_comm.h"

/**
* @brief  初始化一些基本的额数据结构
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 无.
*/
AR_S32 ar_hal_vdec_init(void);

/**
* @brief  去初始化一些基本的额数据结构
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 无.
*/
AR_S32 ar_hal_vdec_deinit(void);

/**
* @brief  创建跟core通信的解码设备文件
* @param  设备文件id，生成设备文件的后缀，如/dev/venc1
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 无.
*/
AR_S32 ar_hal_vdec_create_dev(AR_S32 id);

/**
* @brief  销毁跟core通信的解码设备文件。
* @param  设备文件后缀，比如/dev/vdec1, id就是1
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须销毁设备文件绑定的实例才能销毁设备文件，不然会造成实例变成僵尸实例
*/
AR_S32 ar_hal_vdec_destory_dev(AR_S32 id);

/**
* @brief  创建解码实例绑定在以id为后缀的解码设备文件上
* @param  id 设备文件的id
* @param  p_vdec_attr STRU_VDEC_INST_ATTR
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须在创建完解码设备文件才能创建对应的实例
*/
AR_S32 ar_hal_vdec_create_inst(AR_S32 id, STRU_VDEC_INST_ATTR *p_vdec_attr);

/**
* @brief  销毁绑定在以id为后缀的解码设备文件对应的解码实例
* @param  id 设备文件的id
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须在创建完解码设备文件才能创建对应的实例
*/
AR_S32 ar_hal_vdec_destory_inst(AR_S32 id);

/**
* @brief  打开id对应的解码器返回fd
* @param  id 设备文件id
* @return  integer
*              大于0 打开成功
*             -1 打开失败
* @note 必须在创建完解码设备文件才能打开设备
*/
AR_S32 ar_hal_vdec_dev_open(AR_S32 id);

/**
* @brief  关闭id对应的设备文件
* @param  fd 文件描述符
* @return  integer
*              0 关闭成功
*              -1 关闭失败
* @note close必须在open之后调用
*/
AR_S32 ar_hal_vdec_dev_close(AR_S32 id);

/**
* @brief  解码器开始接收用户发送的码流数据。
* @param  id 解码实例的id
* @return  integer
*              0 成功
*              非0 失败
* @note 开始接收码流进行解码分配参考帧等。
*/
AR_S32 ar_hal_vdec_start(AR_S32 id);

/**
* @brief  解码器停止接收用户发送的码流数据。
* @param  id  解码实例的id
* @return  integer
*              0 成功
*              非0 失败
* @note 解码器停止解码。
*/
AR_S32 ar_hal_vdec_stop(AR_S32 id);

/**
* @brief 将解码通道绑定到某个视频缓存VB池中。
* @param  id   解码实例的id
* @param  pool 视频缓存VB 池信息
* @return  integer
*              0 成功
*              非0 失败
* @note 必须保证通道已创建，否则会返回通道未创建。
*/
AR_S32 ar_hal_vdec_attach_vb_pool(AR_S32 id, STRU_VDEC_CHN_POOLS *pool);

/**
* @brief 将解码通道从某个视频缓存VB池中解绑定。
* @param  id   解码实例的id
* @return  integer
*              0 成功
*              非0 失败
* @note 必须保证通道已创建，否则会返回通道未创建。
*/
AR_S32 ar_hal_vdec_detach_vb_pool(AR_S32 id);

/**
* @brief 设置解码通道参数。
* @param  mod  模块参数结构体指针
* @return  integer
*              0 成功
*              非0 失败
* @note 此接口必须在所有解码通道创建前调用。
*/
AR_S32 ar_hal_vdec_set_mod_param(STRU_VDEC_MOD_PARAMS *mod);

/**
* @brief 设置解码通道参数。
* @param  mod  模块参数结构体指针
* @return  integer
*              0 成功
*              非0 失败
* @note
*/
AR_S32 ar_hal_vdec_get_mod_param(STRU_VDEC_MOD_PARAMS *mod);

/**
* @brief  video decoder的模块退出函数，回收资源
* @param  void
* @return  integer.
*              0 成功,
*              非0 失败.
* @note   必须在/dev/hal_vdec存在的前提下.
*/
AR_S32 ar_hal_vdec_exit();

/**
* @brief  设置解码实例的静态属性
* @param  id  解码实例的id
* @p_vdec_attr  指向STRU_VDEC_INST_ATTR的指针
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 静态的参数,静态参数只能在通道start之前设置,或者stop之后重新配置,动态参数部分设置之后下一帧生效
*/
AR_S32 ar_hal_vdec_set_attr(AR_S32 id, STRU_VDEC_INST_ATTR *p_vdec_attr);

/**
* @brief  获取解码实例的静态属性
* @param  id  解码实例的id
* @p_enc_attr  指向STRU_VDEC_INST_ATTR的指针
* @return  integer.
*              0 获取成功
*              非0 获取失败
* @note 创建解码示例后才可以获取.
*/
AR_S32 ar_hal_vdec_get_attr(AR_S32 id, STRU_VDEC_INST_ATTR *p_vdec_attr);

/**
* @brief  设置解码实例动态属性
* @param  id  解码实例的id
* @p_vdec_attr  指向STRU_VDEC_CHN_PARAM的指针
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 创建解码示例后才可以获取.
*/
AR_S32 ar_hal_vdec_set_param(AR_S32 id, STRU_VDEC_CHN_PARAM *p_vdec_attr);

/**
* @brief  获取解码实例的动态属性
* @param  id  解码实例的id
* @p_enc_attr  指向STRU_VDEC_CHN_PARAM的指针
* @return  integer.
*              0 获取成功
*              非0 获取失败
* @note 创建解码示例后才可以获取.
*/
AR_S32 ar_hal_vdec_get_param(AR_S32 id, STRU_VDEC_CHN_PARAM *p_vdec_attr);

/**
* @brief  查询解码器状态。
* @param  id 解码实例的id
* @param  p_vdec_status 解码器状态结构体。
* @return  integer
*              0 成功
*              非0 失败
* @note start之后才能查询状态。
*/
AR_S32 ar_hal_vdec_query_status(AR_S32 id, STRU_VDEC_STATUS *p_vdec_status);

/**
* @brief  重新初始化id对应的实例
* @param  id 解码实例的id
* @param  hard 标识是否重置解码实例
* @return  integer
*              0 成功
*              非0 失败
* @note 必须在start之前或者stop之后才能重新初始化
*/
AR_S32 ar_hal_vdec_reset(AR_S32 id, AR_S32 hard);

/**
* @brief  送完整的一帧码流给解码器解码。
* @param  id 解码实例的id。
* @param  p_vdec_stream 解码器的码流帧结构体。
* @return  integer
*              0 成功
*              非0 失败
* @note 无。
*/
AR_S32 ar_hal_vdec_send_stream(AR_S32 id, STRU_VDEC_STREAM_FRAME *p_vdec_stream);

/**
* @brief  获取解码后的图像帧。
* @param  id 解码实例的id。
* @param  p_video_frame 解码器输出的图像帧结构体
* @return  integer
*              0 成功
*              非0 失败
* @note 仅支持H.264&H.265。
*/
AR_S32 ar_hal_vdec_get_frame(AR_S32 id, STRU_VDEC_VIDEO_FRAME *p_video_frame);

/**
* @brief  释放解码后的图像帧。
* @param  id 解码实例的id。
* @param  p_video_frame 解码器输出的图像帧结构体
* @return  integer
*              0 成功
*              非0 失败
* @note 仅支持H.264&H.265。
*/
AR_S32 ar_hal_vdec_release_frame(AR_S32 id, STRU_VDEC_VIDEO_FRAME *p_video_frame);

/**
* @brief  获取解码头后的信息。
* @param  id 解码实例的id。
* @param  p_header_info 解码器输出的头信息结构体
* @return  integer
*              0 成功
*              非0 失败
* @note 仅支持H.264&H.265。
*/
AR_S32 ar_hal_vdec_get_headerinfo(AR_S32 id, STRU_VDEC_HEADER_INFO *p_header_info);

/**
* @brief  获取解码后的用户数据。
* @param  id 解码实例的id。
* @param  p_user_data 解码器输出的用户数据结构体
* @return  integer
*              0 成功
*              非0 失败
* @note 仅支持H.264&H.265。
*/

AR_S32 ar_hal_vdec_get_userdata(AR_S32 id, STRU_VDEC_USERDATA_INFO *p_user_data);
/**
* @brief  释放解码后的用户数据。
* @param  id 解码实例的id。
* @param  p_video_frame 解码器输出的用户数据结构体
* @return  integer
*              0 成功
*              非0 失败
* @note 仅支持H.264&H.265。
*/
AR_S32 ar_hal_vdec_release_userdata(AR_S32 id, STRU_VDEC_USERDATA_INFO *p_user_data);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _HAL_VDEC_H_
/** @}*/
