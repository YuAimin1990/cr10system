#include"camera.h"

static int api_test_get_3a_infor_ext(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int dev_id=atoi(p_cmd->args[index+1]);
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
		 ar_printf("dev_num=%d\n",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_dev_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_dev_info);
			//dump_dev_info(p_dev_info);
		 }
		 free(p_dev_info);


		  dev_id=0;
    	  index=camera_find_cmd_pra_index_by_name("-dev_id",p_cmd);
    	  if(index>0)
    	  {
    		  dev_id=atoi(p_cmd->args[index+1]);
    	  }
    	  int opend=0;
    	  ret=ar_hal_vin_dev_is_opend(vin_fd,dev_id,&opend);
    	  if(ret<0 || opend)
    	  {
    		 ar_always("the device have started ret=%d opend=%d",ret,opend);
    		 if(opend)
    		 {
    			int pipe_id=0;
    			ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(vin_fd,dev_id,&pipe_id);
    			ar_printf("dev id =%d pipe_id=%d \n",dev_id,pipe_id);
    			if(ret>=0 && pipe_id>=0)
				{
    				int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
    				if(fd_pipe>=0)
    				{
						 STRU_ALGO_3A_GET_INFO_T  *p_info=malloc(sizeof(STRU_ALGO_3A_GET_INFO_T));
    					 ar_hal_vin_get_3a_info_ext(fd_pipe,pipe_id,p_info);
    					 ar_hal_vin_close_pipe_dev(fd_pipe);

    					 ar_printf("basic aec info ...................................\n");
    					 ar_printf("aec_mode=%d\n",p_info->aec_info.aec_mode);
						 ar_printf("luma_target=%d\n",p_info->aec_info.luma_target);
    					 ar_printf("locked=%d\n",p_info->aec_info.locked);
						 ar_printf("gain=%f\n",p_info->aec_info.gain);
						 ar_printf("sensor_gain=%f\n",p_info->aec_info.sensor_gain);
						 ar_printf("isp_gain1=%f\n",p_info->aec_info.isp_gain1);
						 ar_printf("isp_gain2=%f\n",p_info->aec_info.isp_gain2);
    					 ar_printf("exp_time_us=%f\n",p_info->aec_info.exp_time_us);
    					 ar_printf("cur_real_gain_short=%f\n",p_info->aec_info.cur_real_gain_short);
    					 ar_printf("exp_time_us_short=%f\n",p_info->aec_info.exp_time_us_short);
						 ar_printf("line_count=%d\n",p_info->aec_info.line_count);
						 ar_printf("cur_line_cnt_short=%d\n",p_info->aec_info.cur_line_cnt_short);
						 ar_printf("exp_index=%d\n",p_info->aec_info.exp_index);
						 ar_printf("short_exp_index=%d\n",p_info->aec_info.short_exp_index);
						 ar_printf("lux_index=%d\n",p_info->aec_info.lux_index);
						 ar_printf("current_luma=%d\n",p_info->aec_info.current_luma);
						 ar_printf("short_luma=%d\n",p_info->aec_info.short_luma);
						 ar_printf("ae_settled=%d\n",p_info->aec_info.ae_settled);
						 ar_printf("ration_total=%f\n",p_info->aec_info.ration_total);
						 ar_printf("ration_time=%f\n",p_info->aec_info.ration_time);
						 ar_printf("ration_gain=%f\n",p_info->aec_info.ration_gain);

    					 ar_printf("basic awb info ...................................\n");
    					 ar_printf("awb_mode=%d\n",p_info->awb_info.awb_mode);
    					 ar_printf("cct=%d\n",p_info->awb_info.cct);
    					 ar_printf("r_gain=%f\n",p_info->awb_info.r_gain);
    					 ar_printf("g_gain=%f\n",p_info->awb_info.g_gain);
    					 ar_printf("b_gain=%f\n",p_info->awb_info.b_gain);
						 ar_printf("current_lux_index=%d\n",p_info->awb_info.current_lux_index);
						 ar_printf("is_extreme_blue=%d\n",p_info->awb_info.is_extreme_blue);
						 ar_printf("is_extreme_green=%d\n",p_info->awb_info.is_extreme_green);
						 ar_printf("ExtremeB_pec=%f\n",p_info->awb_info.ExtremeB_pec);
						 ar_printf("GreenZone_pec=%f\n",p_info->awb_info.GreenZone_pec);
						 ar_printf("last_R_gain=%f\n",p_info->awb_info.last_R_gain);
						 ar_printf("last_G_gain=%f\n",p_info->awb_info.last_G_gain);
						 ar_printf("last_B_gain=%f\n",p_info->awb_info.last_B_gain);
						 ar_printf("smooth_weight=%f\n",p_info->awb_info.smooth_weight);
						 ar_printf("r_gain_adj_coef=%f\n",p_info->awb_info.r_gain_adj_coef);
						 ar_printf("b_gain_adj_coef=%f\n",p_info->awb_info.b_gain_adj_coef);
						 ar_printf("GreenZone_num=%f\n",p_info->awb_info.GreenZone_num);
						 ar_printf("GreenZone_r_ave=%f\n",p_info->awb_info.GreenZone_r_ave);
						 ar_printf("GreenZone_g_ave=%f\n",p_info->awb_info.GreenZone_g_ave);
						 ar_printf("GreenZone_g_ave=%f\n",p_info->awb_info.GreenZone_g_ave);
						 ar_printf("ExtremeB_num=%d\n",p_info->awb_info.ExtremeB_num);
						 ar_printf("ExtremeB_r_ave=%f\n",p_info->awb_info.ExtremeB_r_ave);
						 ar_printf("ExtremeB_g_ave=%f\n",p_info->awb_info.ExtremeB_g_ave);
						 ar_printf("ExtremeB_b_ave=%f\n",p_info->awb_info.ExtremeB_b_ave);
						 ar_printf("green_rg=%f\n",p_info->awb_info.green_rg);
						 ar_printf("green_bg=%f\n",p_info->awb_info.green_bg);
						 ar_printf("percent_cnt=%d\n",p_info->awb_info.percent_cnt);
						 ar_printf("sgw_cnt=%d\n",p_info->awb_info.sgw_cnt);
						 ar_printf("rg_bg_as0_num=%d\n",p_info->awb_info.rg_bg_as0_num);
						 ar_printf("grey_point_num=%d\n",p_info->awb_info.grey_point_num);
						 ar_printf("out_line_num=%d\n",p_info->awb_info.out_line_num);
						 ar_printf("block_percent_low_num=%d\n",p_info->awb_info.block_percent_low_num);
						 ar_printf("sgw_rg_ratio=%f\n",p_info->awb_info.sgw_rg_ratio);
						 ar_printf("sgw_bg_ratio=%f\n",p_info->awb_info.sgw_bg_ratio);
						 ar_printf("a_cluster=%d\n",p_info->awb_info.a_cluster);
						 ar_printf("a_rg_ratio=%f\n",p_info->awb_info.a_rg_ratio);
						 ar_printf("a_bg_ratio=%f\n",p_info->awb_info.a_bg_ratio);
						 ar_printf("unsat_y_a_ave=%f\n",p_info->awb_info.unsat_y_a_ave);
						 ar_printf("h_cluster=%d\n",p_info->awb_info.h_cluster);
						 ar_printf("h_rg_ratio=%f\n",p_info->awb_info.h_rg_ratio);
						 ar_printf("h_bg_ratio=%f\n",p_info->awb_info.h_bg_ratio);
						 ar_printf("unsat_y_h_ave=%f\n",p_info->awb_info.unsat_y_h_ave);
						 ar_printf("f_cluster=%d\n",p_info->awb_info.f_cluster);
						 ar_printf("f_rg_ratio=%f\n",p_info->awb_info.f_rg_ratio);
						 ar_printf("f_bg_ratio=%f\n",p_info->awb_info.f_bg_ratio);
						 ar_printf("unsat_y_f_ave=%f\n",p_info->awb_info.unsat_y_f_ave);
						 ar_printf("day_cluster=%d\n",p_info->awb_info.day_cluster);
						 ar_printf("day_rg_ratio=%f\n",p_info->awb_info.day_rg_ratio);
						 ar_printf("day_bg_ratio=%f\n",p_info->awb_info.day_bg_ratio);
						 ar_printf("unsat_y_day_ave=%f\n",p_info->awb_info.unsat_y_day_ave);
						 ar_printf("w_d=%f\n",p_info->awb_info.w_d);
						 ar_printf("w_f=%f\n",p_info->awb_info.w_f);
						 ar_printf("w_a=%f\n",p_info->awb_info.w_a);
						 ar_printf("w_h=%f\n",p_info->awb_info.w_h);
						 ar_printf("wrg_sum=%f\n",p_info->awb_info.wrg_sum);
						 ar_printf("wbg_sum=%f\n",p_info->awb_info.wbg_sum);
						 ar_printf("ave_rg_ratio=%f\n",p_info->awb_info.ave_rg_ratio);
						 ar_printf("ave_bg_ratio=%f\n",p_info->awb_info.ave_bg_ratio);
						 ar_printf("unsat_y_mid=%f\n",p_info->awb_info.unsat_y_mid);
						 ar_printf("unsat_y_max=%f\n",p_info->awb_info.unsat_y_max);
						 ar_printf("unsat_y_min_threshold=%f\n",p_info->awb_info.unsat_y_min_threshold);
						 ar_printf("valid_sample_cnt=%d\n",p_info->awb_info.valid_sample_cnt);
						 ar_printf("gw_init_decision=%d\n",p_info->awb_info.gw_init_decision);
						 ar_printf("gw_init_decision_rg=%f\n",p_info->awb_info.gw_init_decision_rg);
						 ar_printf("gw_init_decision_bg=%f\n",p_info->awb_info.gw_init_decision_bg);
						 ar_printf("gw_init_decision_cct=%d\n",p_info->awb_info.gw_init_decision_cct);
						 ar_printf("gw_init_domain_cluster=%d\n",p_info->awb_info.gw_init_domain_cluster);
						 ar_printf("awb_sgw_cluster_dist2_max_compact_cluster=%f\n",p_info->awb_info.awb_sgw_cluster_dist2_max_compact_cluster);
						 ar_printf("awb_sgw_cluster_temp_min_dist=%f\n",p_info->awb_info.awb_sgw_cluster_temp_min_dist);
						 ar_printf("sw_policy=%d\n",p_info->awb_info.sw_policy);
						 ar_printf("sw_policy_rg=%f\n",p_info->awb_info.sw_policy_rg);
						 ar_printf("sw_policy_bg=%f\n",p_info->awb_info.sw_policy_bg);
						 ar_printf("sw_policy_decision=%d\n",p_info->awb_info.sw_policy_decision);
						 ar_printf("run_times=%d\n",p_info->awb_info.run_times);

						 AR_U32 af_mode;
						 AR_U32 algo_type;
						 AR_U32 len_position;
						 AR_U32 focus_value;
						 AR_U32 history_pos;
						 AR_U32 history_focus_value;
						 AR_U32 stats_type;
						 AR_U32 point_num;
						 STRU_AF_SEARCH_POINT_T point[AF_INFO_SEARCH_POINT_NUM];


    					 ar_printf("basic awb info ...................................\n");
						 ar_printf("af_mode=%d\n",p_info->af_info.af_mode);
						 ar_printf("algo_type=%d\n",p_info->af_info.algo_type);
						 ar_printf("len_position=%d\n",p_info->af_info.len_position);
						 ar_printf("focus_value=%d\n",p_info->af_info.focus_value);
						 ar_printf("history_pos=%d\n",p_info->af_info.history_pos);
						 ar_printf("history_focus_value=%d\n",p_info->af_info.history_focus_value);
						 ar_printf("stats_type=%d\n",p_info->af_info.stats_type);
						 ar_printf("point_num=%d\n",p_info->af_info.point_num);
						 ar_printf("%-10s %-10s\n","pos","fv");
						 for(int k=0;k<p_info->af_info.point_num;k++)
						 {
						    ar_printf("%-10d %-10d \n",p_info->af_info.point[k].pos,p_info->af_info.point[k].fv);
						 }

						 free(p_info);
    				}else
    				{
    					 ar_err("can not opened the pipe fd");
    				}
    			}else
    			{
    			   ar_err("can not get bind pipid by devid");
    			}
    		 }
    	   }else
    	   {
    		   ar_err("the dev not opend, can not do this test, so open the dev, to do later");
    	   }
		   ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(get_3a_infor_ext)=
{
   "--get_3a_infor_ext",
   api_test_get_3a_infor_ext,
   {
      "test the api ar_hal_vin_get_3a_infor_ext, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  NULL,
   },
};
