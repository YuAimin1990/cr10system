/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VENC
 *  @{
 */

/**
 * @file hal_venc.h
 * @author  Artosyn Software Team
 * @date 2021/04/21
 * @brief 定义VENC 控制API;
 */

#ifndef _HAL_VENC_H_
#define _HAL_VENC_H_

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
AR_S32 ar_hal_venc_init(void);
/**
* @brief  去初始化一些基本的额数据结构
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 无.
*/
AR_S32 ar_hal_venc_deinit(void);
/**
* @brief  创建跟core通信的编码设备文件
* @param  设备文件id，生成设备文件的后缀，如/dev/venc1
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 无.
*/
AR_S32 ar_hal_venc_create_dev(AR_S32 id);
/**
* @brief  销毁跟core通信的编码设备文件
* @param  设备文件后缀，比如/dev/venc1, id就是1
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须销毁设备文件绑定的实例才能销毁设备文件，不然会造成实例变成僵尸实例
*/
AR_S32 ar_hal_venc_destory_dev(AR_S32 id);
/**
* @brief  获取已经创建的编码设备文件信息
* @param  控制设备信息指针
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须在创建完通信设备后才能调用.
*/
AR_S32 ar_hal_venc_get_dev_info(STRU_VENC_CTRL_INFO *pVencCtrlInfo);
/**
* @brief  创建编码实例绑定在以id为后缀的编码设备文件上
* @param  id 设备文件的id
* @param  pInstAttr 编码实例的属性结构体
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须在创建完编码设备文件才能创建对应的实例.
*/
AR_S32 ar_hal_venc_create_inst(AR_S32 id, STRU_VENC_INST_ATTR *pInstAttr);
/**
* @brief  销毁绑定在以id为后缀的编码设备文件对应的编码实例
* @param  id 设备文件的id
* @return  integer.
*              0 成功,
*              非0 失败.
* @note 必须在创建完编码设备文件才能创建对应的实例
*/
AR_S32 ar_hal_venc_destory_inst(AR_S32 id);

/**
* @brief  打开id对应的encoder设备返回fd
* @param  id 设备文件id
* @return  integer
*              大于0 打开成功
*             -1 打开失败
* @note 必须在创建完编码设备文件才能打开设备
*/
AR_S32 ar_hal_venc_dev_open(AR_S32 id);
/**
* @brief  关闭句柄对应的设备文件
* @param  fd 文件描述符
* @return  integer
*              0 关闭成功
*              -1 关闭失败
* @note close必须在open之后调用
*/
AR_S32 ar_hal_venc_dev_close(AR_S32 fd);
/**
* @brief  重新初始化id对应的实例
* @param  id 实例对应的id
* @return  integer
*              0 成功
*              非0 失败
* @note 必须在start之前或者stop之后才能重新初始化
*/
AR_S32 ar_hal_venc_reset(AR_S32 id);
/**
* @brief  做完开始编码之前的所有初始化比如分配编码实例配置参数，分配buffer，编头等
* @param  id  编码实例的id
* @param  p_pic_param 设置编码器的接收数据信息
* @return  integer.
*              0 成功,
*              非0代表失败
* @note 必须在实例创建成功才可以调用
*/
AR_S32 ar_hal_venc_start(AR_S32 id, STRU_VENC_PIC_PARAM *p_pic_param);
/**
* @brief  停止编码器接收原始yuv数据
* @param  id  编码实例的id
* @return  integer.
*              0 停止成功,
*              非0 停止失败.
* @note 必须在start之后才可以stop.
*/
AR_S32 ar_hal_venc_stop(AR_S32 id);
/**
* @brief  设置编码实例的静态参数如源数据格式,分辨率,码率和码控类型等
* @param  id  编码实例的id
* @p_venc_attr  指向STRU_VENC_INST_ATTR的指针
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 静态的参数,静态参数只能在通道start之前设置,或者stop之后重新配置,动态参数部分设置之后下一帧生效
*/
AR_S32 ar_hal_venc_set_attr(AR_S32 id, STRU_VENC_INST_ATTR *p_venc_attr);
/**
* @brief  获取编码实例的静态属性
* @param  id  编码实例的id
* @p_enc_attr  指向STRU_VENC_INST_ATTR的指针
* @return  integer.
*              0 获取成功
*              非0 获取失败
* @note 创建编码示例后才可以获取.
*/
AR_S32 ar_hal_venc_get_attr(AR_S32 id,  STRU_VENC_INST_ATTR *p_venc_attr);
/**
* @brief  设置编码实例的静态参数如源数据格式,分辨率,码率和码控类型等
* @param  id  编码实例的id
* @p_venc_attr  指向STRU_VENC_INST_ATTR的指针
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 只能设置支持静态的参数,静态参数只能在通道start之前设置,或者stop之后重新配置
*/
AR_S32 ar_hal_venc_set_mode_param(AR_S32 id, STRU_VENC_INST_ATTR *p_venc_attr);
/**
* @brief  获取编码实例的静态属性
* @param  id  编码实例的id
* @p_enc_attr  指向STRU_VENC_INST_ATTR的指针
* @return  integer.
*              0 获取成功
*              非0 获取失败
* @note 创建编码示例后才可以获取.
*/
AR_S32 ar_hal_venc_get_mode_param(AR_S32 id,  STRU_VENC_INST_ATTR *p_venc_attr);

/**
* @brief  设置编码实例的静态参数如分辨率，缓冲区大小等最大集合的静态参数。
* @param  id  编码实例的id
* @param  p_venc_sparam  编码实例初始化参数结构体
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 只能设置静态参数,静态参数只能在创建通道的时候设置,或者stop之后重新配置,属于高级参数集。
*/
AR_S32 ar_hal_venc_set_sparam(AR_S32 id, STRU_VENC_SPARAM *p_venc_sparam);
/**
* @brief  获取编码实例的静态参数如分辨率，缓冲区大小等最大集合的静态参数。
* @param  id  编码实例的id
* @param  p_venc_sparam  编码实例初始化参数结构体
* @return  integer.
*              0 设置成功,
*              非0 设置失败.
* @note 创建完编码实例才可以获取,属于高级参数集。
*/
AR_S32 ar_hal_venc_get_sparam(AR_S32 id, STRU_VENC_SPARAM *p_venc_sparam);

/**
* @brief  获取编码器的状态.
* @param  id  编码实例的id
* @param  p_venc_status  指向STRU_VENC_STATUS的指针
* @return  integer.
*              0 获取成功,
*              非0 获取状态失败.
* @note 在start之后查询状态才有实际意义
*/
AR_S32 ar_hal_venc_query_status(AR_S32 id, STRU_VENC_STATUS *p_venc_status);
/**
* @brief  获取一帧编码后的码流.
* @param  id  编码实例的id
* @param  p_Stream  指向STRU_VENC_STREAM的指针
* @return  integer.
*              0 获取成功,
*              非0 获取码流失败.
* @note 在查询到已经有数据的时候在调用可以执行效率更高.
*/
AR_S32 ar_hal_venc_get_stream(AR_S32 id, STRU_VENC_STREAM *p_Stream);
/**
* @brief  释放码流帧.
* @param  id  编码实例的id
* @param  p_Stream  指向STRU_VENC_STREAM的指针
* @return  integer.
*              0 释放成功,
*              非0 释放失败.
* @note 只能释放通过get接口获取的码流帧.
*/
AR_S32 ar_hal_venc_release_stream(AR_S32 id, STRU_VENC_STREAM *p_Stream);
/**
* @brief  获取用于传递sei数据的buffer
* @param  id  编码实例的id
* @param  stUserData: sei buffer的信息
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 无
*/
AR_S32 ar_hal_venc_get_userdata_buf(AR_S32 id, STRU_VENC_USERDATA *stUserData);
/**
* @brief  向core发送sei数据
* @param  id  编码实例的id
* @param  stUserData: sei buffer的信息
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 无
*/
AR_S32 ar_hal_venc_insert_userData(AR_S32 id, STRU_VENC_USERDATA *stUserData);
/**
* @brief  用户发送原始图像给编码器的接口.
* @param  id  编码实例的id
* @param  p_frame  原始图像信息指针
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 调用此接口需要用原始图像的物理地址，根据s32MsTimeout支持阻塞和非阻塞方式发送.
*/
AR_S32 ar_hal_venc_send_frame(AR_S32 id, STRU_VIDEO_FRAME_INFO *p_frame);
/**
* @brief  用户发送原始图像给编码器的高级接口.
* @param  id  编码实例的id
* @param  p_frame  原始图像信息指针
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 调用此接口需要用原始图像的物理地址，根据s32MsTimeout支持阻塞和非阻塞方式发送.
*/
AR_S32 ar_hal_venc_send_frame_ext(AR_S32 id, STRU_USER_FRAME_INFO *p_frame);
/**
* @brief  设置编码器的静态参数.
* @param  id  编码实例的id
* @param  p_sparam 动态参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，静态参数需要在start之前配置。
*/
AR_S32 ar_hal_venc_set_sparam(AR_S32 id, STRU_VENC_SPARAM *p_sparam);
/**
* @brief  获取编码器相关静态参数.
* @param  id  编码实例的id
* @param  p_sparam 静态参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，任意时间获取当前的静态参数.
*/
AR_S32 ar_hal_venc_get_sparam(AR_S32 id, STRU_VENC_SPARAM *p_sparam);
/**
* @brief  设置编码器的动态参数.
* @param  id  编码实例的id
* @param  p_dparam 动态参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，动态参数在start之后设置,下一帧生效。
*/
AR_S32 ar_hal_venc_set_dparam(AR_S32 id, STRU_VENC_DPARAM *p_dparam);
/**
* @brief  获取编码器相关动态参数.
* @param  id  编码实例的id
* @param  p_dparam 动态参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，任意时间获取当前的动态参数.
*/
AR_S32 ar_hal_venc_get_dparam(AR_S32 id, STRU_VENC_DPARAM *p_dparam);
/**
* @brief  请求IDR帧.
* @param  id  编码实例的id
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，会对码率的稳定有影响，会受到GOP模式的限制.
*/
AR_S32 ar_hal_venc_request_idr(AR_S32 id);
/**
* @brief  是否使能IDR帧，如果关闭则从下帧开始就没有IDR或者I帧.
* @param  id  编码实例的id
* @param  enable  使能或者关闭
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，会对码率的稳定有影响，会受到GOP模式的限制.
*/
AR_S32 ar_hal_venc_enable_idr(AR_S32 id, AR_BOOL enable);
/**
* @brief  使能QP map，设置map的物理起始地址.
* @param  id  编码实例的id
* @param  p_custom_map 客户自定义的map结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，码控打开会对参数的意义有影响，支持每帧动态设置.
*/
AR_S32 ar_hal_venc_set_qp_map(AR_S32 id, STRU_CUSTOM_MAP_PARAM *p_custom_map);
/**
* @brief  获取custom map的结构体信息.
* @param  id  编码实例的id
* @param  p_custom_map 客户自定义的map结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码.
*/
AR_S32 ar_hal_venc_get_qp_map(AR_S32 id, STRU_CUSTOM_MAP_PARAM *p_custom_map);
/**
* @brief  设置PPS相关动态参数.
* @param  id  编码实例的id
* @param  p_pps_param PPS参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，动态参数在任意时间设置下一帧生效.
*/
AR_S32 ar_hal_venc_set_pps_param(AR_S32 id, STRU_VENC_PPS_DPARAM *p_pps_param);
/**
* @brief  获取PPS相关动态参数.
* @param  id  编码实例的id
* @param  p_pps_param PPS参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，任意时间获取当前的PPS动态参数。
*/
AR_S32 ar_hal_venc_get_pps_param(AR_S32 id, STRU_VENC_PPS_DPARAM *p_pps_param);
/**
* @brief  使能或关闭条带分割.
* @param  id  编码实例的id
* @param  p_slice_split 条带分割参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，任意时刻可以设置的动态参数。
*/
AR_S32 ar_hal_venc_set_slice_split(AR_S32 id, STRU_SLICE_SPLIT_DPARAM *p_slice_split);
/**
* @brief  获取条带分割参数.
* @param  id  编码实例的id
* @param  p_slice_split 条带分割参数结构
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码.
*/
AR_S32 ar_hal_venc_get_slice_split(AR_S32 id, STRU_SLICE_SPLIT_DPARAM *p_slice_split);
/**
* @brief  设置帧内预测参数.
* @param  id  编码实例的id
* @param  enable 是否开启
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264编码.
*/
AR_S32 ar_hal_venc_set_h264_intrapred(AR_S32 id, AR_BOOL enable);
/**
* @brief  获取帧内预测参数.
* @param  id  编码实例的id
* @param  p_enable 是否开启
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264编码.
*/
AR_S32 ar_hal_venc_get_h264_intrapred(AR_S32 id, AR_BOOL *p_enable);
/**
* @brief  设置是否开启去块滤波.
* @param  id  编码实例的id
* @param  enable 是否开启
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.265编码.
*/
AR_S32 ar_hal_venc_set_h265_deblk(AR_S32 id, STRU_VENC_DEBLK_H265 *p_deblk);
/**
* @brief  获取是否开启去块滤波.
* @param  id  编码实例的id
* @param  p_enable 是否开启
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.265编码.
*/
AR_S32 ar_hal_venc_get_h265_deblk(AR_S32 id, STRU_VENC_DEBLK_H265 *p_deblk);
/**
* @brief  设置熵编码模式.
* @param  id  编码实例的id
* @param  mode 熵编码模式 0:CAVLC 1:CABAC
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264编码.
*/
AR_S32 ar_hal_venc_set_h264_entropy(AR_S32 id, AR_S32 mode);
/**
* @brief  获取熵编码模式.
* @param  id  编码实例的id
* @param  p_mode 编码模式 0:CAVLC 1:CABAC
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264编码.
*/
AR_S32 ar_hal_venc_get_h264_entropy(AR_S32 id, AR_S32 *p_mode);
/**
* @brief  设置VUI的结构体信息.
* @param  id  编码实例的id
* @param  p_vui 用户自定义编码的VUI结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码.
*/
AR_S32 ar_hal_venc_set_vui_info(AR_S32 id, UNION_VUI_INFO *p_vui);
/**
* @brief  获取VUI的结构体信息.
* @param  id  编码实例的id
* @param  p_vui 编码的VUI结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码.
*/
AR_S32 ar_hal_venc_get_vui_info(AR_S32 id, UNION_VUI_INFO *p_vui);
/**
* @brief  设置ROI的结构体信息.
* @param  id  编码实例的id
* @param  p_roi_item 用户指定的ROI区域结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，建议在start编码器之前设置.
*/
AR_S32 ar_hal_venc_set_roi(AR_S32 id, STRU_VENC_ROI_NODE *p_roi_item);
/**
* @brief  获取ROI的结构体信息.
* @param  id  编码实例的id
* @param  index  指定ROI区域的索引
* @param  p_roi_item 用户指定要获取的ROI区域结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持H.264/H.265编码，建议在设置之前先获取.
*/
AR_S32 ar_hal_venc_get_roi(AR_S32 id, AR_S32 index, STRU_VENC_ROI_NODE *p_roi_item);
/**
* @brief  设置JPEG的参数结构体信息.
* @param  id  编码实例的id
* @param  p_jpeg_param JPEG参数结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持设置JPEG编码的动态参数.
*/
AR_S32 ar_hal_venc_set_jpeg_param(AR_S32 id, STRU_VENC_JPEG_PARAM *p_jpeg_param);
/**
* @brief  获取JPEG的参数结构体信息.
* @param  id  编码实例的id
* @param  p_jpeg_param JPEG参数结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持获取JPEG编码的动态参数.
*/
AR_S32 ar_hal_venc_get_jpeg_param(AR_S32 id, STRU_VENC_JPEG_PARAM *p_jpeg_param);
/**
* @brief  设置MJPEG的参数结构体信息.
* @param  id  编码实例的id
* @param  p_mjpeg_param MJPEG参数结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持设置MJPEG编码的动态参数.
*/
AR_S32 ar_hal_venc_set_mjpeg_param(AR_S32 id, STRU_VENC_MJPEG_PARAM *p_mjpeg_param);
/**
* @brief  获取MJPEG的参数结构体信息.
* @param  id  编码实例的id
* @param  p_mjpeg_param JPEG参数结构体信息
* @return  integer.
*              0 成功,
*              非 0 失败.
* @note 只支持获取MJPEG编码的动态参数.
*/
AR_S32 ar_hal_venc_get_mjpeg_param(AR_S32 id, STRU_VENC_MJPEG_PARAM *p_mjpeg_param);
/**
* @brief  设置用户自定义参考关系。
* @param  id  编码实例的id
* @param  p_custom_gop 自定义参考关系结构体。
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 只支持设置H.264&H.265码控相关的静态参数，需要在start之前设置。
*/
AR_S32 ar_hal_venc_set_custom_gop(AR_S32 id, STRU_CUSTOM_GOP_PARAMS* p_custom_gop);
/**
* @brief  获取用户自定义参考关系。
* @param  id  编码实例的id
* @param  p_custom_gop 自定义参考关系结构体。
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 只支持获取H.264&H.265码控相关的静态参数。
*/
AR_S32 ar_hal_venc_get_custom_gop(AR_S32 id, STRU_CUSTOM_GOP_PARAM *p_custom_gop);
/**
* @brief  配置H.265&H.264编码背景检测相关参数。
* @param  id  编码实例的id
* @param  p_bg_param  背景检测参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 只支持配置H.265&H.264的背景检测相关静动态参数。
*/
AR_S32 ar_hal_venc_set_bg_param(AR_S32 id, STRU_BG_DPARAM* p_bg_param);
/**
* @brief  获取H.265&H.264编码背景检测相关参数。
* @param  id  编码实例的id
* @param  p_bg_param  背景检测参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 只支持获取H.265&H.264的背景检测相关静动态参数。
*/
AR_S32 ar_hal_venc_get_bg_param(AR_S32 id, STRU_BG_DPARAM* p_bg_param);
/**
* @brief  设置编码器旋转相关静态参数
* @param  id  编码实例的id
* @param  p_rotation 旋转参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 设置编码器旋转相关的静态参数。
*/
AR_S32 ar_hal_venc_set_rotation(AR_S32 id, STRU_VENC_ROTATION *p_rotation);
/**
* @brief  获取编码器旋转相关静态参数
* @param  id  编码实例的id
* @param  p_rotation 旋转参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 获取编码器旋转相关的静态参数。
*/
AR_S32 ar_hal_venc_get_rotation(AR_S32 id, STRU_VENC_ROTATION *p_rotation);
/**
* @brief  设置编码器镜像相关静态参数
* @param  id  编码实例的id
* @param  p_mirror 镜像参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 设置编码器镜像相关的静态参数。
*/
AR_S32 ar_hal_venc_set_mirror(AR_S32 id, STRU_VENC_MIRROR *p_mirror);
/**
* @brief  获取编码器镜像相关静态参数
* @param  id  编码实例的id
* @param  p_mirror 旋转参数的结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 获取编码器镜像相关的静态参数。
*/
AR_S32 ar_hal_venc_get_mirror(AR_S32 id, STRU_VENC_MIRROR *p_mirror);
/**
* @brief  打印编码器相关的参数打印信息
* @param  id  编码实例的id
* @param  u32CmdId 参考ENUM_VCODEC_CTRL_CMD
* @param  pCmdParam 指向参数的指针
* @param  s32CmdParamLength 发送参数的长度
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 主要用于mpi层可以直接与mpp service通信
*/
AR_S32 ar_hal_venc_send_cmd(AR_S32 id, AR_U32 u32CmdId, void *pCmdParam, AR_S32 s32CmdParamLength);
/**
* @brief  设置编码器相关的模块参数
* @param  pVencModParam  编码模块级参数结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 模块加载时候的参数，作用范围是全局
*/
AR_S32 ar_hal_venc_set_mod_param(STRU_VENC_MOD_PARAMS *pVencModParam);
/**
* @brief  获取编码器相关的模块参数
* @param  pVencModParam  编码模块级参数结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 无
*/
AR_S32 ar_hal_venc_get_mod_param(STRU_VENC_MOD_PARAMS *pVencModParam);

/**
* @brief  video encoder的模块退出函数，回收资源
* @param  void
* @return  integer.
*              0 成功,
*              非0 失败.
* @note   必须在/dev/hal_venc存在的前提下.
*/
AR_S32 ar_hal_venc_exit();

/**
* @brief  打印编码器相关的参数打印信息
* @param  id  编码实例的id
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 主要用于参数debug
*/
AR_S32 ar_hal_venc_show_debug_info(AR_S32 id);
/**
* @brief   获取编码器bitstream buffer信息
* @param  id  编码实例的id
* @param  stStreamBufInfo 存储码流buffer信息的结构体的指针
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 无
*/
AR_S32 ar_hal_venc_get_streambuf_info(AR_S32 id, STRU_VENC_STREAM_BUF_INFO *stStreamBufInfo);

/**
* @brief  清除codec的clock/power的操作状态
* @param  pVencModParam编码模块级参数结构体
* @return  integer.
*              0 成功，
*              非 0 失败。
* @note 无
*/
AR_S32 ar_hal_venc_suspend(STRU_VENC_MOD_PARAMS *pVencModParam);
#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _HAL_VENC_H_
/** @}*/

