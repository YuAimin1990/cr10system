#ifndef __HAL_VIN_RPC_H__
#define __HAL_VIN_RPC_H__

/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/
/** \addtogroup VI
 *  @{
 */

/**
 * @file hal_vin_rpc.h
 * @brief 定义vin的rpc模块的api 及数据类型
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/05/10
 * @license 2021-2025, Artosyn. Co., Ltd.
**/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_vin.h"
#include "hal_vin_sensor_lib.h"
typedef enum
{
   CAM_RPC_CALL_ar_camera_get_dev_num=CAM_RPC_CALL_BASE<<8,
   CAM_RPC_CALL_ar_camera_get_dev_infor,
   CAM_RPC_CALL_ar_camera_server_get_start_opt,
   CAM_RPC_CALL_ar_camera_server_set_start_opt,
   CAM_RPC_CALL_ar_camera_register_sensor_dev,
   CAM_RPC_CALL_ar_camera_register_actuator_dev,
   CAM_RPC_CALL_ar_camera_start_camera_server,
   CAM_RPC_CALL_ar_camera_set_global_env,
   CAM_RPC_CALL_ar_camera_get_global_env,
   CAM_RPC_CALL_ar_camera_get_common_sensor_dev_id,
   CAM_RPC_CALL_ar_camea_get_isp_in_dev_id,
   CAM_RPC_CALL_ar_camera_get_opened_camera,
   CAM_RPC_CALL_ar_camera_get_opened_dev,
   CAM_RPC_CALL_ar_camera_open_camera,
   CAM_RPC_CALL_ar_camera_is_opend,
   CAM_RPC_CALL_ar_camera_dev_is_opend,
   CAM_RPC_CALL_ar_camera_get_open_prop,
   CAM_RPC_CALL_ar_camera_set_open_prop,
   CAM_RPC_CALL_ar_camera_close_camera,
   CAM_RPC_CALL_ar_camera_control,
   CAM_RPC_CALL_ar_camera_creat_stream,
   CAM_RPC_CALL_ar_camera_get_stream_property,
   CAM_RPC_CALL_ar_camera_get_stream_data,
   CAM_RPC_CALL_ar_camera_set_stream_data,
   CAM_RPC_CALL_ar_camera_creat_stream_man_link,
   CAM_RPC_CALL_ar_camera_reshape_stream,
   CAM_RPC_CALL_ar_camera_bind_to_dev,
   CAM_RPC_CALL_ar_camera_unbind_to_dev,
   CAM_RPC_CALL_ar_camera_bind_get_dev_id_by_cam_id,
   CAM_RPC_CALL_ar_camera_bind_get_cam_id_by_dev_id,
   CAM_RPC_CALL_ar_camera_delete_stream,
   CAM_RPC_CALL_ar_camera_preprae_delete_stream,
   CAM_RPC_CALL_ar_camera_get_all_stream,
   CAM_RPC_CALL_ar_camera_start_stream,
   CAM_RPC_CALL_ar_camera_trigger,
   CAM_RPC_CALL_ar_camera_set_manual_crop,
   CAM_RPC_CALL_ar_camera_stop_stream,
   CAM_RPC_CALL_ar_camera_suspend_stream,
   CAM_RPC_CALL_ar_camera_resume_stream,
   CAM_RPC_CALL_ar_camera_return_stream_buffer,
   CAM_RPC_CALL_ar_camera_create_stream_client,
   CAM_RPC_CALL_ar_camera_delete_stream_client,
   CAM_RPC_CALL_ar_camera_get_stream_buffer,
   CAM_RPC_CALL_ar_camera_q_stream_buffer,
   CAM_RPC_CALL_ar_camera_dq_stream_buffer,
   CAM_RPC_CALL_ar_camera_requst_stream_buffer,
   CAM_RPC_CALL_ar_camera_creat_stream_buffer,
   CAM_RPC_CALL_ar_camera_delete_stream_buffer,
   CAM_RPC_CALL_ar_camera_flush_stream_buffer,
   CAM_RPC_CALL_ar_camer_show_all_log,
   CAM_RPC_CALL_ar_camera_process_tuning_req,
   CAM_RPC_CALL_ar_camera_get_tuning_req,
   CAM_RPC_CALL_ar_camera_release_tuning_req,

   CAM_RPC_CALL_ar_camera_get_event,
   CAM_RPC_CALL_ar_camera_release_event,
   CAM_RPC_CALL_ar_camera_register_event,
   CAM_RPC_CALL_ar_camera_unregister_event,
   CAM_RPC_CALL_ar_camera_create_event_client,
   CAM_RPC_CALL_ar_camera_delete_event_client,


   CAM_RPC_CALL_ar_camera_get_sensor_cb_event,
   CAM_RPC_CALL_ar_camera_release_sensor_cb_event,
   CAM_RPC_CALL_ar_camera_register_sensor_cb_event,
   CAM_RPC_CALL_ar_camera_unregister_sensor_cb_event,
   CAM_RPC_CALL_ar_camera_create_sensor_cb_event_client,
   CAM_RPC_CALL_ar_camera_delete_sensor_cb_event_client,

   CAM_RPC_CALL_ar_camera_get_aec_cb_event,
   CAM_RPC_CALL_ar_camera_release_aec_cb_event,
   CAM_RPC_CALL_ar_camera_register_aec_cb_event,
   CAM_RPC_CALL_ar_camera_unregister_aec_cb_event,
   CAM_RPC_CALL_ar_camera_create_aec_cb_event_client,
   CAM_RPC_CALL_ar_camera_delete_aec_cb_event_client,

   CAM_RPC_CALL_ar_camera_get_awb_cb_event,
   CAM_RPC_CALL_ar_camera_release_awb_cb_event,
   CAM_RPC_CALL_ar_camera_register_awb_cb_event,
   CAM_RPC_CALL_ar_camera_unregister_awb_cb_event,
   CAM_RPC_CALL_ar_camera_create_awb_cb_event_client,
   CAM_RPC_CALL_ar_camera_delete_awb_cb_event_client,

   CAM_RPC_CALL_ar_camera_get_af_cb_event,
   CAM_RPC_CALL_ar_camera_release_af_cb_event,
   CAM_RPC_CALL_ar_camera_register_af_cb_event,
   CAM_RPC_CALL_ar_camera_unregister_af_cb_event,
   CAM_RPC_CALL_ar_camera_create_af_cb_event_client,
   CAM_RPC_CALL_ar_camera_delete_af_cb_event_client,

   CAM_RPC_CALL_ar_camera_set_sensor_var_pra,
   CAM_RPC_CALL_ar_camera_get_sensor_var_pra,

   CAM_RPC_CALL_ar_camera_register_aec_algo_lib,
   CAM_RPC_CALL_ar_camera_unregister_aec_algo_lib,
   CAM_RPC_CALL_ar_camera_register_awb_algo_lib,
   CAM_RPC_CALL_ar_camera_unregister_awb_algo_lib,
   CAM_RPC_CALL_ar_camera_register_af_algo_lib,
   CAM_RPC_CALL_ar_camera_unregister_af_algo_lib,

   CAM_RPC_CALL_ar_camera_get_meta_event,
   CAM_RPC_CALL_ar_camera_release_meta_event,
   CAM_RPC_CALL_ar_camera_create_meta_event_client,
   CAM_RPC_CALL_ar_camera_delete_meta_event_client,
   CAM_RPC_CALL_ar_camera_register_meta_event,
   CAM_RPC_CALL_ar_camera_unregister_meta_event,
   CAM_RPC_CALL_ar_camera_read_reg,
   CAM_RPC_CALL_ar_camera_write_reg,

   CAM_RPC_CALL_ar_camera_nuc_set_out_clk,
   CAM_RPC_CALL_ar_camera_nuc_cfg,
   CAM_RPC_CALL_ar_camera_nuc_start,
   CAM_RPC_CALL_ar_camera_nuc_stop,
   CAM_RPC_CALL_ar_camera_nuc_get_occ,
   CAM_RPC_CALL_ar_camera_nuc_set_occ,
   CAM_RPC_CALL_ar_camera_nuc_cmd,
   CAM_RPC_CALL_ar_camera_stop_server,	
}ENUM_CAM_RPC_CALL_T;

typedef struct
{
   AR_S32 dev_num;
}CAM_RPC_CALL_ar_camera_get_dev_num_pra_t;

typedef struct
{
	AR_S32 dev_id;
	STRU_CAM_INFO_T dev_infor;
}CAM_RPC_CALL_ar_camera_get_dev_infor_pra_t;
typedef struct
{
	AR_S32 revert;
}CAM_RPC_CALL_ar_camera_start_camera_server_pra_t;

typedef struct
{
	AR_S32 stop;
}CAM_RPC_CALL_ar_camera_stop_camera_server_pra_t;

typedef struct
{
	STRU_CAM_SERVER_START_OPT_T opt;
}CAM_RPC_CALL_ar_camera_server_get_start_opt_pra_t;

typedef struct
{
	STRU_CAM_SERVER_START_OPT_T opt;
}CAM_RPC_CALL_ar_camera_server_set_start_opt_pra_t;

typedef struct
{
	STRU_SENSOR_DEV_T sensor_dev;
}CAM_RPC_CALL_ar_camera_register_sensor_dev_pra_t;

typedef struct
{
	STRU_ACTUATOR_DEV_T actuator_dev;
}CAM_RPC_CALL_ar_camera_register_actuator_dev_pra_t;


typedef struct
{
	STRU_CAM_SERVER_GLOBAL_PRA_T global_env;
}CAM_RPC_CALL_ar_camera_set_global_env_pra_t;

typedef struct
{
	STRU_CAM_SERVER_GLOBAL_PRA_T global_env;
}CAM_RPC_CALL_ar_camera_get_global_env_pra_t;


typedef struct
{
	AR_S32 dev_id;
}CAM_RPC_CALL_ar_camera_get_common_sensor_dev_id_pra_t;

typedef struct
{
  AR_S32 isp_in_id;
}CAM_RPC_CALL_ar_camea_get_isp_in_dev_id_pra_t;
typedef struct
{
   AR_S32 camera_id[16];
   AR_S32 opend_camera_num;
}CAM_RPC_CALL_ar_camera_get_opened_camera_pra_t;

typedef struct
{
   AR_S32 dev_id[16];
   AR_S32 opend_dev_num;
}CAM_RPC_CALL_ar_camera_get_opened_dev_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_OPEN_PROPERTY_T open_property;
}CAM_RPC_CALL_ar_camera_open_camera_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 is_opened;
}CAM_RPC_CALL_ar_camera_is_opend_pra_t;

typedef struct
{
   AR_S32 dev_id;
   AR_S32 is_opened;
}CAM_RPC_CALL_ar_camera_dev_is_opend_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_OPEN_PROPERTY_T open_property;
}CAM_RPC_CALL_ar_camera_get_open_prop_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_OPEN_PROPERTY_T open_property;
}CAM_RPC_CALL_ar_camera_set_open_prop_pra_t;

typedef struct
{
   AR_S32 camera_id;
}CAM_RPC_CALL_ar_camera_close_camera_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 ctl_code;
   AR_S32 size;
   AR_S32 revert;
}CAM_RPC_CALL_ar_camera_control_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_STREAM_PROPERTY_T property;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_creat_stream_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_AR_STREAM_PROPERTY_T property;
}CAM_RPC_CALL_ar_camera_get_stream_property_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_VOID *data;
   AR_S32 pos;
}CAM_RPC_CALL_ar_camera_get_stream_data_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_VOID *data;
   AR_S32 pos;
}CAM_RPC_CALL_ar_camera_set_stream_data_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_LINK_PRA_T p_link[16];
   STRU_AR_STREAM_PROPERTY_T property;
}CAM_RPC_CALL_ar_camera_creat_stream_man_link_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_AR_STREAM_PROPERTY_T property;
}CAM_RPC_CALL_ar_camera_reshape_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 dev_id;
}CAM_RPC_CALL_ar_camera_bind_to_dev_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 dev_id;
}CAM_RPC_CALL_ar_camera_unbind_to_dev_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 dev_id;
}CAM_RPC_CALL_ar_camera_bind_get_dev_id_by_cam_id_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 dev_id;
}CAM_RPC_CALL_ar_camera_bind_get_cam_id_by_dev_id_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_delete_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_preprae_delete_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id[16];
   AR_S32 num;
}CAM_RPC_CALL_ar_camera_get_all_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_start_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_CAMERA_TRIGGER_PROP_T prop;
}CAM_RPC_CALL_ar_camera_trigger_pra_t;

typedef struct
{
    int camera_id;
    STRU_AR_CAMERA_MANUAL_CROP_PROP_T prop;
}CAM_RPC_CALL_ar_camera_set_manual_crop_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_stop_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_S32 mode;	
}CAM_RPC_CALL_ar_camera_suspend_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_S32 mode;	
}CAM_RPC_CALL_ar_camera_resume_stream_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
}CAM_RPC_CALL_ar_camera_return_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_stream_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_stream_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   AR_S32 client_id;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
   AR_S32 timeout_ms;
}CAM_RPC_CALL_ar_camera_get_stream_buffer_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_type;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
}CAM_RPC_CALL_ar_camera_q_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_type;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
   AR_S32 timeout_ms;
}CAM_RPC_CALL_ar_camera_dq_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_type;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer[16];
   AR_S32 size;
}CAM_RPC_CALL_ar_camera_requst_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
}CAM_RPC_CALL_ar_camera_creat_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer;
}CAM_RPC_CALL_ar_camera_delete_stream_buffer_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 stream_id;
}CAM_RPC_CALL_ar_camera_flush_stream_buffer_pra_t;

typedef struct
{
   AR_S32 revert;
}CAM_RPC_CALL_ar_camer_show_all_log_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_CAM_SET_TUNING_REQUEST_T *req; //this vaddr always is core process, if user want to use the buffer, must map the req_phy
   STRU_CAM_SET_TUNING_REQUEST_T *req_phy;
}CAM_RPC_CALL_ar_camera_process_tuning_req_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_CAM_SET_TUNING_REQUEST_T *req;
   STRU_CAM_SET_TUNING_REQUEST_T *req_phy;
}CAM_RPC_CALL_ar_camera_get_tuning_req_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_CAM_SET_TUNING_REQUEST_T *req;
   STRU_CAM_SET_TUNING_REQUEST_T *req_phy;
}CAM_RPC_CALL_ar_camera_release_tuning_req_pra_t;

/*-----------------camera event-----------------*/
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_event_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_event_pra_t;

/*-------------------sensor cb -----------------------------*/
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_sensor_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_sensor_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_sensor_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_sensor_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_sensor_cb_event_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_sensor_cb_event_pra_t;

/*-------------------aec cb -----------------------------*/
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_aec_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_aec_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_aec_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_aec_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_aec_cb_event_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_aec_cb_event_pra_t;

/*-------------------awb cb -----------------------------*/
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_awb_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_awb_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_awb_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_awb_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_awb_cb_event_pra_t;


typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_awb_cb_event_pra_t;

/*-------------------af cb -----------------------------*/
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_af_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_af_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_af_cb_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_af_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_af_cb_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_af_cb_event_pra_t;


typedef struct
{
   AR_S32 camera_id;
   STRU_CAMERA_INFO_VAR_PRA_T var_pra;
}CAM_RPC_CALL_ar_camera_set_sensor_var_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_CAMERA_INFO_VAR_PRA_T var_pra;
}CAM_RPC_CALL_ar_camera_get_sensor_var_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_register_aec_algo_lib_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_unregister_aec_algo_lib_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_register_awb_algo_lib_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_unregister_awb_algo_lib_pra_t;
typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_register_af_algo_lib_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_CHAR lib_name[64];
}CAM_RPC_CALL_ar_camera_unregister_af_algo_lib_pra_t;


typedef struct
{
	AR_S32 camera_id;
	AR_S32 meta_type;
}CAM_RPC_CALL_ar_camera_register_meta_type_pra_t;

typedef struct
{
	AR_S32 camera_id;
	AR_S32 meta_type;
}CAM_RPC_CALL_ar_camera_unregister_meta_type_pra_t;

///--------------meta
typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_get_meta_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_create_meta_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
}CAM_RPC_CALL_ar_camera_delete_meta_event_client_pra_t;

typedef struct
{
   AR_S32 camera_id;
   STRU_AR_EVENT_INFO_T event_info;
}CAM_RPC_CALL_ar_camera_release_meta_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_register_meta_event_pra_t;

typedef struct
{
   AR_S32 camera_id;
   AR_S32 client_id;
   AR_S32 event;
}CAM_RPC_CALL_ar_camera_unregister_meta_event_pra_t;

typedef struct
{
	AR_U64 phy_addr;
	AR_U32 val;
	AR_S32 start;
	AR_S32 end;
}CAM_RPC_CALL_ar_camera_read_reg_pra_t;
typedef struct
{
    AR_U64 phy_addr;
	AR_U32 val;
	AR_S32 start;
	AR_S32 end;
}CAM_RPC_CALL_ar_camera_write_reg_pra_t;

typedef struct {
    ENUM_NUC_OUT_CLK_SRC_T clk_src;
    AR_U8 clk_div;
    ENUM_NUC_CLK_PHASE_T clk_phase;
    AR_BOOL clk_en;
} CAM_RPC_CALL_ar_camera_nuc_out_clk_pra_t;

typedef struct {
    STRU_NUC_CFG_T nuc_cfg;
} CAM_RPC_CALL_ar_camera_nuc_cfg_pra_t;

//typedef struct {
//    STRU_STREAM_BUFFER_INIFO_T occ_buf;
//} CAM_RPC_CALL_ar_camera_nuc_occ_pra_t;

typedef struct {
    ENUM_NUC_CMD_T nuc_cmd;
    AR_S32 size;
} CAM_RPC_CALL_ar_camera_nuc_cmd_pra_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
