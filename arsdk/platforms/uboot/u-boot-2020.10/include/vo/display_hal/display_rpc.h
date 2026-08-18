/**
 * @file display_rpc.h
 * @brief  display rpc include file
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/07/19
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef __DISPLAY_RPC_H__
#define __DISPLAY_RPC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_vo.h"
#include "vo/ar_display.h"

int ar_vo_rpc_dev_enable(ENUM_AR_HAL_VO_DEV_ID        e_dev_id);
int ar_vo_rpc_dev_disable(ENUM_AR_HAL_VO_DEV_ID        e_dev_id);
int ar_vo_rpc_dev_set_attr(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, STRU_AR_HAL_VO_DEV_ATTR *para);
int ar_vo_rpc_dev_get_attr(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, STRU_AR_HAL_VO_DEV_ATTR *attr);
int ar_vo_rpc_dev_set_csc(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, ENUM_AR_HAL_VO_CSC *para);
int ar_vo_rpc_dev_get_csc(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, ENUM_AR_HAL_VO_CSC *para);
int ar_vo_rpc_layer_enable(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id);
int ar_vo_rpc_layer_disable(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id);
int ar_vo_rpc_layer_set_attr(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_LAYER_ATTR *para);
int ar_vo_rpc_layer_get_attr(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                           STRU_AR_HAL_VO_LAYER_ATTR *para);
int ar_vo_rpc_layer_set_position(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                           STRU_AR_HAL_VO_POS *para);
int ar_vo_rpc_layer_enqueue(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                                       display_buffer_t *buffer);
int ar_vo_rpc_layer_set_csc(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                       STRU_AR_HAL_VO_LAYER_CSC   *csc);

#endif

