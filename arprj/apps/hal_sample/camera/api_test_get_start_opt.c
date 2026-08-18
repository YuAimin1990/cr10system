#include"camera.h"

static int api_test_get_start_opt(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 STRU_VIN_DRIVER_START_OPT_T *p_opt=malloc(sizeof(STRU_VIN_DRIVER_START_OPT_T));

         if(!p_opt)
         {
             ar_hal_vin_close_vin_dev(vin_fd);
             return -1;
         }
		 
         ar_hal_vin_driver_get_start_opt(vin_fd,p_opt);

         ar_printf("is_init=%d\n",p_opt->is_init);
         ar_printf("vif_buf_count=%d\n",p_opt->vif_buf_count);
         ar_printf("isp_buf_count=%d\n",p_opt->isp_buf_count);
         ar_printf("vfe_buf_count=%d\n",p_opt->vfe_buf_count);
         ar_printf("aec_lib_load_start=%d\n",p_opt->aec_lib_load_start);
         ar_printf("awb_lib_load_start=%d\n",p_opt->awb_lib_load_start);
         ar_printf("af_lib_load_start=%d\n",p_opt->af_lib_load_start);
         ar_printf("max_dev_num; =%d\n",p_opt->max_dev_num);
		 ar_printf("max_stream_per_dev; =%d\n",p_opt->max_stream_per_dev);		 
         ar_printf("visual_isp_count; =%d\n",p_opt->visual_isp_count);
         ar_printf("en_invalide_hw_addr=%d\n",p_opt->en_invalide_hw_addr);
         ar_printf("max_w=%d\n",p_opt->max_w);		 
         ar_printf("max_h=%d\n",p_opt->max_h);
         ar_printf("sensor_dev_count=%d\n",p_opt->sensor_dev_count);

		 for(int i=0;i<p_opt->sensor_dev_count;i++)
		 {
		     ar_printf("i=%d ...........................................:\n",i);
			 ar_printf("flag=%d\n",p_opt->sensor_dev[i].flag);

			 ar_printf("dev_name=%s\n",p_opt->sensor_dev[i].sensor_dev.dev_name);
			 ar_printf("driver_name=%s\n",p_opt->sensor_dev[i].sensor_dev.driver_name);
			 ar_printf("actuator_name=%s\n",p_opt->sensor_dev[i].sensor_dev.actuator_name);
			 ar_printf("sensor_slave_addr_7bits=%d\n",p_opt->sensor_dev[i].sensor_dev.sensor_slave_addr_7bits);
			 ar_printf("mipi_lane_count=%d\n",p_opt->sensor_dev[i].sensor_dev.mipi_lane_count);
			 ar_printf("need_master_slave_sel=%d\n",p_opt->sensor_dev[i].sensor_dev.need_master_slave_sel);
			 ar_printf("device_mode=%d\n",p_opt->sensor_dev[i].sensor_dev.device_mode);
			 ar_printf("device_type=%d\n",p_opt->sensor_dev[i].sensor_dev.device_type);
			 ar_printf("slave_num=%d\n",p_opt->sensor_dev[i].sensor_dev.slave_num);
			 ar_printf("master_sensor=%s\n",p_opt->sensor_dev[i].sensor_dev.master_sensor);
			 for(int j=0;j<p_opt->sensor_dev[i].sensor_dev.slave_num;j++)
			 {
				 ar_printf("slave_sensor[%d]=%s\n",j,p_opt->sensor_dev[i].sensor_dev.slave_sensor[j]);
			 }
			 ar_printf("slot_name=%s\n",p_opt->sensor_dev[i].sensor_dev.slot.slot_name);
			 ar_printf("slot_type=%d\n",p_opt->sensor_dev[i].sensor_dev.slot.slot_type);
			 ar_printf("mipi_index=%d\n",p_opt->sensor_dev[i].sensor_dev.slot.mipi_index);
			 ar_printf("i2c_index=%d\n",p_opt->sensor_dev[i].sensor_dev.slot.i2c_index);
			 for(int j=0;j<6;j++)
			 {
				 ar_printf("reset_gpio[%d]=%d\n",j,p_opt->sensor_dev[i].sensor_dev.slot.reset_gpio[j]);
			 }
			 for(int j=0;j<6;j++)
			 {
				 ar_printf("power_gpio[%d]=%d\n",j,p_opt->sensor_dev[i].sensor_dev.slot.power_gpio[j]);
			 }
		 }

		 for(int i=0;i<p_opt->actuator_dev_count;i++)
		 {
		     ar_printf("i=%d ...........................................:\n",i);
			 ar_printf("flag=%d\n",p_opt->actuator_dev[i].flag);
			 ar_printf("dev_name=%s\n",p_opt->actuator_dev[i].actuator_dev.dev_name);
			 ar_printf("driver_name=%s\n",p_opt->actuator_dev[i].actuator_dev.driver_name);
			 ar_printf("i2c_index=%s\n",p_opt->actuator_dev[i].actuator_dev.i2c_index);
		 }

		 ar_printf("event_inf_count=%d\n",p_opt->event_inf_count);
		 ar_printf("event_data_pool_count=%d\n",p_opt->event_data_pool_count);
		 ar_printf("event_data_max_size=%d\n",p_opt->event_data_max_size);
		 ar_printf("tuning_req_pool_count=%d\n",p_opt->tuning_req_pool_count);
		 ar_printf("mem_over_debug_en=%d\n",p_opt->mem_over_debug_en);
		 ar_printf("mem_over_debug_en_bug_on=%d\n",p_opt->mem_over_debug_en_bug_on);
		 ar_printf("ddr_rw_burst_len=%d\n",p_opt->ddr_rw_burst_len);

		 ar_printf("aec_algo_name=%s\n",p_opt->aec_algo_name);
		 ar_printf("af_algo_name=%s\n",p_opt->af_algo_name);
		 ar_printf("awb_algo_name=%s\n",p_opt->awb_algo_name);
		 ar_printf("eis_algo_name=%s\n",p_opt->eis_algo_name);

		 ar_printf("display_ui_mode=%d\n",p_opt->display_ui_mode);
		 ar_printf("enable_register_mipi_irq=%d\n",p_opt->enable_register_mipi_irq);
		 ar_printf("enable_mipi_irq_only_once=%d\n",p_opt->enable_mipi_irq_only_once);
		 ar_printf("enable_mipi_reg_dump_when_mipi_err=%d\n",p_opt->enable_mipi_reg_dump_when_mipi_err);
		 ar_printf("dvp_rx_out_mask=%d\n",p_opt->dvp_rx_out_mask);
		 ar_printf("dvp_scaler_out_mask=%d\n",p_opt->dvp_scaler_out_mask);
		 ar_printf("vif_out_mask=%d\n",p_opt->vif_out_mask);


		 ar_printf("isp_out_hw_burst=%d\n",p_opt->isp_out_hw_burst);
		 ar_printf("use_remote_ltm_algo=%d\n",p_opt->use_remote_ltm_algo);
		 ar_printf("share_stats_buffer=%d\n",p_opt->share_stats_buffer);
		 ar_printf("enable_hardware_irq_print=%d\n",p_opt->enable_hardware_irq_print);
		 ar_printf("max_stream_buffer_count=%d\n",p_opt->max_stream_buffer_count);
		 ar_printf("nr3d_use_ping_pong_buf=%d\n",p_opt->nr3d_use_ping_pong_buf);
		 ar_printf("nr3d_md_use_ping_pong_buf=%d\n",p_opt->nr3d_md_use_ping_pong_buf);
		 ar_printf("visual_isp_count=%d\n",p_opt->visual_isp_count);
		 ar_printf("ltm_max_mesh_w=%d\n",p_opt->ltm_max_mesh_w);
		 ar_printf("ltm_max_mesh_h=%d\n",p_opt->ltm_max_mesh_h);
		 ar_printf("ltm_lut_pool_count=%d\n",p_opt->ltm_lut_pool_count);

		 ar_printf("vsync_update_reg=%d\n",p_opt->vsync_update_reg);
		 ar_printf("wait_all_isp_done=%d\n",p_opt->wait_all_isp_done);
		 ar_printf("use_stats_pingpong=%d\n",p_opt->use_stats_pingpong);
		 ar_printf("check_ltm_stats=%d\n",p_opt->check_ltm_stats);
		 ar_printf("hardware_mask=0x%x\n",p_opt->hardware_mask);
		 ar_printf("filter_mask=0x%x\n",p_opt->filter_mask);
		 ar_printf("isp_use_loop_mem=%d\n",p_opt->isp_use_loop_mem);
		 ar_printf("isp_use_done_cfg_buffer=%d\n",p_opt->isp_use_done_cfg_buffer);		 
		 ar_printf("max_pipe_num=%d\n",p_opt->max_pipe_num);
		 ar_printf("max_ch_num=%d\n",p_opt->max_ch_num);
		 ar_printf("max_event_client=%d\n",p_opt->max_event_client);
		 ar_printf("use_isp_irq_optimization=%d\n",p_opt->use_isp_irq_optimization);
		 ar_printf("use_vsync_3a_clc=%d\n",p_opt->use_vsync_3a_clc);
		 ar_printf("use_vsync_trigger_aec=%d\n",p_opt->use_vsync_trigger_aec);
		 ar_printf("mem_256_pool_size=%d\n",p_opt->mem_256_pool_size);		 
		 ar_printf("disable_seq_queue=%d\n",p_opt->disable_seq_queue);			 
		 ar_printf("use_dpcm_bits_clc_stride=%d\n",p_opt->use_dpcm_bits_clc_stride);		 
		 ar_printf("use_dma_upate_reg=%d\n",p_opt->use_dma_upate_reg);		 
		 ar_printf("ir_algo_split_thread=%d\n",p_opt->ir_algo_split_thread);		 
		 ar_printf("mipi_out_mask=%d\n",p_opt->mipi_out_mask);		 
		 ar_printf("vif_use_ddr_done_irq=%d\n",p_opt->vif_use_ddr_done_irq);
		 ar_printf("use_mipi_only_skew_int=%d\n",p_opt->use_mipi_only_skew_int);
         free(p_opt);
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(get_start_opt)=
{
   "--get_start_opt",
   api_test_get_start_opt,
   {
      "test the api ar_hal_vin_driver_get_start_opt, -remote [remote binder server name]: this indicate the api run on remote app,",
	  NULL,
   },
};
