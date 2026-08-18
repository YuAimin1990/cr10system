#include"camera.h"

static int api_test_get_sensor_dev_var_pra(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
      int dev_id=0;
	  index=camera_find_cmd_pra_index_by_name("-dev_id",p_cmd);
      if(index>=0)
      {
		  dev_id=atoi(p_cmd->args[index+1]);
      }

	  if(dev_id>=0)
	  {
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
         if(ret!=0)
         {
             ar_err("failed to ar_hal_vin_get_dev_num with %d", ret);
             return ret;
         }
		 ar_printf("dev_num=%d\n",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			//dump_dev_info(p_info);
		 }
		 free(p_info);
         ar_printf("start get sensor var pra info\n");
		 int pipe_id=dev_id;
		 int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
		 ar_hal_vin_pipe_bind_to_dev(vin_fd,pipe_id,dev_id);
		 STRU_DEV_INFO_VAR_T *p_var_pra=malloc(sizeof(STRU_DEV_INFO_VAR_T));
		 memset(p_var_pra,0,sizeof(STRU_DEV_INFO_VAR_T));
		 ar_printf("sizeof(STRU_DEV_INFO_VAR_T)=%d\n",sizeof(STRU_DEV_INFO_VAR_T));
		 p_var_pra->res_index=-1;
		 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);

         /*
      		 STRU_REGISTER_SETTING_T *reg_read_id;
      		 AR_U32 sensor_id[4];
      		 AR_U32 id_count;
      		 AR_S32 i2c_component_index;
      		 AR_U8 slave_address_7bit;
      		 AR_CHAR actuator[64];
      		 AR_CHAR eeprom[64];
      		 AR_CHAR flash[64];
      		 AR_CHAR master_sensor[64]; //when the device is slave , the name save the devices's master device
      		 AR_S32 need_master_slave_sel;// if set to 1 we need call the pfn_sensor_lib_set_slave_master to set the slave master
      		 AR_S32 device_mode;// 1: the device is slave ;0 :the device is master sensor;2 the device is master proxy, but it real is a slave sensor
      		 AR_S32 slave_num; //the master devie have how mush slave sensor;
      		 AR_CHAR slave_sensor[7][64]; //we max supported a master device have 7 slave devices
      		 AR_CHAR cfg_file[64]; //the device tuing cfg file
		 */
         ar_printf("slave info---------------------------------\n");
		 ar_printf("i2c_component_index=%d\n",p_var_pra->slave_infor.i2c_component_index);
		 ar_printf("slave_address_7bit=%d\n",p_var_pra->slave_infor.slave_address_7bit);
		 ar_printf("addr_type=%d\n",p_var_pra->slave_infor.reg_read_id->addr_type);
		 ar_printf("data_type=%d\n",p_var_pra->slave_infor.reg_read_id->data_type);		 
		 ar_printf("actuator=%s\n",p_var_pra->slave_infor.actuator);
		 ar_printf("eeprom=%s\n",p_var_pra->slave_infor.eeprom);
		 ar_printf("flash=%s\n",p_var_pra->slave_infor.flash);
		 ar_printf("master_sensor=%s\n",p_var_pra->slave_infor.master_sensor);
		 ar_printf("need_master_slave_sel=%d\n",p_var_pra->slave_infor.need_master_slave_sel);
		 ar_printf("device_mode=%d\n",p_var_pra->slave_infor.device_mode);
		 ar_printf("slave_num=%d\n",p_var_pra->slave_infor.slave_num);
		 for(int i=0;i<p_var_pra->slave_infor.slave_num;i++)
		 {
			ar_printf("slave_sensor[%d]=%s\n",i,p_var_pra->slave_infor.slave_sensor[i]);
		 }
		 ar_printf("cfg_file=%s\n",p_var_pra->slave_infor.cfg_file);

		 /*
                      AR_S32 out_format;
                      AR_S32   link_mod;
                      AR_S32 bit_width;
                      AR_S32 bayer;
                      AR_S32 yuv;
                      AR_S32 rgb;
                      STRU_MIPI_DESC_T mipi_desc;
                      STRU_DVP_DESC_T  dvp_desc;
                      STRU_HDMI_DESC_T hdmi_desc;
		 */
         ar_printf("sensor out info---------------------------------\n");
		 ar_printf("out_format=%d\n",p_var_pra->out_infor.out_format);
		 ar_printf("link_mod=%d\n",p_var_pra->out_infor.link_mod);
		 ar_printf("bit_width=%d\n",p_var_pra->out_infor.bit_width);
		 ar_printf("bayer=%d\n",p_var_pra->out_infor.bayer);
		 ar_printf("yuv=%d\n",p_var_pra->out_infor.yuv);
		 ar_printf("rgb=%d\n",p_var_pra->out_infor.rgb);

		 /*
      		 AR_U32 mipi_index;
      		 AR_U32 mipi_lane_cout;
      		 AR_U32 settle_count;
		 */
		 ar_printf("mipi_index=%d\n",p_var_pra->out_infor.mipi_desc.mipi_index);
		 ar_printf("mipi_lane_cout=%d\n",p_var_pra->out_infor.mipi_desc.mipi_lane_cout);
		 ar_printf("settle_count=%d\n",p_var_pra->out_infor.mipi_desc.settle_count);
         /*
                    AR_S32      highlow_8bit_switch;
                    AR_S32      dvp_index;
                    AR_S32      dvp_bit_width;
                    AR_S32      dvp_ddr_sdr_mode;
                    AR_S32      yuv_order;
                    AR_S32      de_mode;
                    AR_S32      stable_detect_mode;
                    AR_S32      de_polority; // 0:same as input, 1:select polarity detect
                    AR_S32      hs_polority; // 0:same as input, 1:select polarity detect
                    AR_S32      vs_polority; // 0:same as input, 1:select polarity detect
                    AR_S32      field_polority;
                    AR_S32      filed_select_mode;
                    AR_S32      dvp_type;
                    AR_S32      is_interlace;
                    AR_S32      h_start;
                    AR_S32      h_end;
                    AR_S32      v_start;
                    AR_S32      v_end;
                    AR_S32      de_sel;
                    AR_S32      vc_id[8];
                    AR_S32      vc_num;
		 */
		 ar_printf("highlow_8bit_switch=%d\n",p_var_pra->out_infor.dvp_desc.highlow_8bit_switch);
		 ar_printf("dvp_index=%d\n",p_var_pra->out_infor.dvp_desc.dvp_index);
		 ar_printf("dvp_bit_width=%d\n",p_var_pra->out_infor.dvp_desc.dvp_bit_width);
		 ar_printf("dvp_ddr_sdr_mode=%d\n",p_var_pra->out_infor.dvp_desc.dvp_ddr_sdr_mode);
		 ar_printf("yuv_order=%d\n",p_var_pra->out_infor.dvp_desc.yuv_order);
		 ar_printf("de_mode=%d\n",p_var_pra->out_infor.dvp_desc.de_mode);
		 ar_printf("stable_detect_mode=%d\n",p_var_pra->out_infor.dvp_desc.stable_detect_mode);
		 ar_printf("de_polority=%d\n",p_var_pra->out_infor.dvp_desc.de_polority);
		 ar_printf("hs_polority=%d\n",p_var_pra->out_infor.dvp_desc.hs_polority);
		 ar_printf("vs_polority=%d\n",p_var_pra->out_infor.dvp_desc.vs_polority);
		 ar_printf("field_polority=%d\n",p_var_pra->out_infor.dvp_desc.field_polority);
		 ar_printf("filed_select_mode=%d\n",p_var_pra->out_infor.dvp_desc.filed_select_mode);
		 ar_printf("dvp_type=%d\n",p_var_pra->out_infor.dvp_desc.dvp_type);
		 ar_printf("is_interlace=%d\n",p_var_pra->out_infor.dvp_desc.is_interlace);
		 ar_printf("h_start=%d\n",p_var_pra->out_infor.dvp_desc.h_start);
		 ar_printf("h_end=%d\n",p_var_pra->out_infor.dvp_desc.h_end);
		 ar_printf("v_start=%d\n",p_var_pra->out_infor.dvp_desc.v_start);
		 ar_printf("v_end=%d\n",p_var_pra->out_infor.dvp_desc.v_end);
		 ar_printf("vc_num=%d\n",p_var_pra->out_infor.dvp_desc.vc_num);
		 for(int i=0;i<p_var_pra->out_infor.dvp_desc.vc_num;i++)
		 {
			ar_printf("vc_id[%d]=%d\n",i,p_var_pra->out_infor.dvp_desc.vc_id[i]);
		 }
		 ar_printf("sensor have %d res -------------start res info dump-----------\n",p_var_pra->res_count);

         int start_index=0;
		 int end_index=start_index+1;
         index=camera_find_cmd_pra_index_by_name("-res_index",p_cmd);
	     if(index>=0)
	     {
              int res_index=atoi(p_cmd->args[index+1]);
			  if(res_index < 0)
			  {
			     start_index=p_var_pra->res_count;
				 end_index=p_var_pra->res_count;
			  }else if(res_index >=p_var_pra->res_count)
			  {
			     start_index=0;
				 end_index=p_var_pra->res_count;
			  }else
			  {
			     start_index=res_index;
			     end_index=start_index+1;
			  }
	     }

		 for(int i=start_index;i<end_index;i++)
		 {
			 p_var_pra->res_index=i;
			 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
			 ar_printf("res %d......................\n",p_var_pra->res_index);
			 /*
			 AR_U32 crop_en;
			 AR_U32 width;
			 AR_U32 height;
			 AR_U32 line_length;
			 AR_U32 frame_length;
			 AR_U32 frame_length_short; //the shortest
			 AR_U32 frame_length_short1;//the short1
			 AR_U32 clk_per_lane;
			 AR_U32 mipi_pix_clk;
			 AR_S32 	 mipi_lanes;
			 AR_FLOAT	 fps;

			 AR_U32 x_start;
			 AR_U32 y_start;
			 AR_U32 x_width;
			 AR_U32 y_height;

			 AR_U32 vc_mask;
			 AR_S32 vc_count;
			 STRU_VC_CHANNEL_T vc_info[4];
			 */
			 ar_printf("crop_en=%d\n",p_var_pra->res_info.crop_en);
			 ar_printf("width=%d\n",p_var_pra->res_info.width);
			 ar_printf("height=%d\n",p_var_pra->res_info.height);
			 ar_printf("line_length=%d\n",p_var_pra->res_info.line_length);
			 ar_printf("frame_length=%d\n",p_var_pra->res_info.frame_length);
			 ar_printf("frame_length_short=%d\n",p_var_pra->res_info.frame_length_short);
			 ar_printf("frame_length_short1=%d\n",p_var_pra->res_info.frame_length_short1);
			 ar_printf("clk_per_lane=%d\n",p_var_pra->res_info.clk_per_lane);
			 ar_printf("mipi_pix_clk=%d\n",p_var_pra->res_info.mipi_pix_clk);
			 ar_printf("mipi_lanes=%d\n",p_var_pra->res_info.mipi_lanes);
			 ar_printf("fps=%f\n",p_var_pra->res_info.fps);
			 ar_printf("x_start=%d\n",p_var_pra->res_info.x_start);
			 ar_printf("y_start=%d\n",p_var_pra->res_info.y_start);
			 ar_printf("x_width=%d\n",p_var_pra->res_info.x_width);
			 ar_printf("y_height=%d\n",p_var_pra->res_info.y_height);
			 ar_printf("vc_mask=%d\n",p_var_pra->res_info.vc_mask);
			 ar_printf("vc_count=%d\n",p_var_pra->res_info.vc_count);
			 /*
			      AR_U32 vc;//which virtual channel will be used this sensor, vc dt need according mipi spec to set
                      AR_S32 dt;
                      AR_U32 crop_en;
                      AR_S32 out_format;
                      AR_S32 bit_width;
                      AR_S32 bayer;
                      AR_S32 yuv;
                      AR_S32 rgb;
                      AR_S32 width;
                      AR_S32 heigh;
                      AR_S32 x;
                      AR_S32 y;
                      AR_S32 x_width;
                      AR_S32 y_heigh;
                      AR_U32 line_length;
                      AR_U32 frame_length;
                      AR_U32 clk_per_lane;
                      AR_FLOAT fps;
                      AR_S32 manual_ipi_timing;
                      AR_S32 hsa;
                      AR_S32 hbp;
                      AR_S32 hsd;
			 */
			 for(int j=0;j<p_var_pra->res_info.vc_count;j++)
			 {
				 ar_printf("vc=%d vc_info ..............\n",j);
				 ar_printf("vc=%d\n",p_var_pra->res_info.vc_info[j].vc);
				 ar_printf("dt=%d\n",p_var_pra->res_info.vc_info[j].dt);
				 ar_printf("crop_en=%d\n",p_var_pra->res_info.vc_info[j].crop_en);
				 ar_printf("out_format=%d\n",p_var_pra->res_info.vc_info[j].out_format);
				 ar_printf("bit_width=%d\n",p_var_pra->res_info.vc_info[j].bit_width);
				 ar_printf("bayer=%d\n",p_var_pra->res_info.vc_info[j].bayer);
				 ar_printf("yuv=%d\n",p_var_pra->res_info.vc_info[j].yuv);
				 ar_printf("rgb=%d\n",p_var_pra->res_info.vc_info[j].rgb);
				 ar_printf("width=%d\n",p_var_pra->res_info.vc_info[j].width);
				 ar_printf("heigh=%d\n",p_var_pra->res_info.vc_info[j].heigh);
				 ar_printf("x=%d\n",p_var_pra->res_info.vc_info[j].x);
				 ar_printf("y=%d\n",p_var_pra->res_info.vc_info[j].y);
				 ar_printf("x_width=%d\n",p_var_pra->res_info.vc_info[j].x_width);
				 ar_printf("y_heigh=%d\n",p_var_pra->res_info.vc_info[j].y_heigh);
				 ar_printf("line_length=%d\n",p_var_pra->res_info.vc_info[j].line_length);
				 ar_printf("frame_length=%d\n",p_var_pra->res_info.vc_info[j].frame_length);
				 ar_printf("clk_per_lane=%d\n",p_var_pra->res_info.vc_info[j].clk_per_lane);
				 ar_printf("fps=%f\n",p_var_pra->res_info.vc_info[j].fps);
				 ar_printf("manual_ipi_timing=%d\n",p_var_pra->res_info.vc_info[j].manual_ipi_timing);
				 ar_printf("hsa=%d\n",p_var_pra->res_info.vc_info[j].hsa);
			     ar_printf("hbp=%d\n",p_var_pra->res_info.vc_info[j].hbp);
			     ar_printf("hsd=%d\n",p_var_pra->res_info.vc_info[j].hsd);
			 }
             /*
       			 AR_S32 manual_ipi_timing;
       			 AR_S32 hsa;
       			 AR_S32 hbp;
       			 AR_S32 hsd;

       			 AR_S32 enable_bayer;
       			 AR_S32 bayer_format;
       			 //hdr config
       			 AR_U32 sensor_hdr;
       			 AR_U32 hdr_width;
       			 AR_U32 hdr_height;
       			 AR_U32 hdr_frame_count;
       			 AR_U32 bit_depth;
       			 AR_S32 	  long_offset;
       			 AR_S32 	  mid_offset;
       			 AR_S32 	  short_offset;

       			 AR_S32 	  hdr_x_start;
       			 AR_S32 	  hdr_y_start_l;
       			 AR_S32 	  hdr_y_start_m;
       			 AR_S32 	  hdr_y_start_s;
       			 AR_S32 	  hdr_ebd_word_pos;
       			 AR_S32 	  hdr_ebd_byte_pos;
       			 AR_S32 	  hdr_fid_pos;
       			 AR_S32 	  hdr_set_pos;
       			 AR_S32 	  hdr_ebd_type;
       			 AR_S32 	  hdr_fid0_pos;
       			 AR_S32 	  hdr_fid1_pos;
       			 AR_S32 	  hdr_fid2_pos;
       			 AR_S32 	  hdr_obid_active_pos;
       			 AR_S32 	  hdr_sid_pos;

       			 AR_U32 vif_line_buffer_manual_enable; // enable/disable manual mode of vif line buffering
       			 AR_U32 vif_line_buffer_de_delay; //rise edge of vsync to rise edge of first HRef;
       			 AR_U32 vif_line_buffer_htotal; //the length between two consecutive hsync(0~0xFFFF)
       			 AR_U32 vif_line_buffer_hstart; //the length between hysnc received to send-start;

       			 AR_S32 aec_factor;
       			 AR_S32 awb_factor;
       			 AR_S32 ltm_factor;
       			 //when 4k, the vif can not wrtite encoder frame to ddr, so need vif bypass mode to do a buffer, when in this case ,enable the cfg
       			 //but the vif bypass have only support one hdr, when have two hdr, another hdr can not set    it.
       			 // this is a 9301 hw limit bug, but hw designer not will fix it duce to schdule.
       			 // when a hdr sensor mipi fre is higher than vif_fre/3, the feature must be opened.
       			 // if we have two hdr sensor which mipi more than vif_fre/3 , the project must reject. please tell your pm, this is not supported
       			 AR_S32 use_vif_hdr_bypass;
       			 AR_S32 use_encoder_dpcm;
       			 AR_S32 dpcm_out_bits;
			 */
			 ar_printf("manual_ipi_timing=%d\n",p_var_pra->res_info.manual_ipi_timing);
			 ar_printf("bayer_format=%d\n",p_var_pra->res_info.bayer_format);
			 ar_printf("sensor_hdr=%d\n",p_var_pra->res_info.sensor_hdr);
			 ar_printf("hdr_width=%d\n",p_var_pra->res_info.hdr_width);
			 ar_printf("hdr_height=%d\n",p_var_pra->res_info.hdr_height);
			 ar_printf("hdr_frame_count=%d\n",p_var_pra->res_info.hdr_frame_count);
			 ar_printf("bit_depth=%d\n",p_var_pra->res_info.bit_depth);
			 ar_printf("long_offset=%d\n",p_var_pra->res_info.long_offset);
			 ar_printf("mid_offset=%d\n",p_var_pra->res_info.mid_offset);
			 ar_printf("short_offset=%d\n",p_var_pra->res_info.short_offset);
			 ar_printf("vif_x_start=%d\n",p_var_pra->res_info.vif_x_start);
			 ar_printf("vif_y_start_l=%d\n",p_var_pra->res_info.vif_y_start_l);
			 ar_printf("vif_y_start_m=%d\n",p_var_pra->res_info.vif_y_start_m);
			 ar_printf("vif_y_start_s=%d\n",p_var_pra->res_info.vif_y_start_s);
			 ar_printf("hdr_ebd_word_pos=%d\n",p_var_pra->res_info.hdr_ebd_word_pos);
			 ar_printf("hdr_ebd_byte_pos=%d\n",p_var_pra->res_info.hdr_ebd_byte_pos);
			 ar_printf("hdr_fid_pos=%d\n",p_var_pra->res_info.hdr_fid_pos);
			 ar_printf("hdr_set_pos=%d\n",p_var_pra->res_info.hdr_set_pos);
			 ar_printf("hdr_ebd_type=%d\n",p_var_pra->res_info.hdr_ebd_type);
			 ar_printf("hdr_fid0_pos=%d\n",p_var_pra->res_info.hdr_fid0_pos);
			 ar_printf("hdr_fid1_pos=%d\n",p_var_pra->res_info.hdr_fid1_pos);
			 ar_printf("hdr_fid2_pos=%d\n",p_var_pra->res_info.hdr_fid2_pos);
			 ar_printf("hdr_obid_active_pos=%d\n",p_var_pra->res_info.hdr_obid_active_pos);
			 ar_printf("hdr_sid_pos=%d\n",p_var_pra->res_info.hdr_sid_pos);
			 ar_printf("vif_line_buffer_manual_enable=%d\n",p_var_pra->res_info.vif_line_buffer_manual_enable);
			 ar_printf("vif_line_buffer_de_delay=%d\n",p_var_pra->res_info.vif_line_buffer_de_delay);
			 ar_printf("vif_line_buffer_htotal=%d\n",p_var_pra->res_info.vif_line_buffer_htotal);
			 ar_printf("vif_line_buffer_hstart=%d\n",p_var_pra->res_info.vif_line_buffer_hstart);
			 ar_printf("aec_factor=%d\n",p_var_pra->res_info.aec_factor);
			 ar_printf("awb_factor=%d\n",p_var_pra->res_info.awb_factor);
			 ar_printf("ltm_factor=%d\n",p_var_pra->res_info.ltm_factor);
			 ar_printf("use_vif_hdr_bypass=%d\n",p_var_pra->res_info.use_vif_hdr_bypass);
			 ar_printf("use_encoder_dpcm=%d\n",p_var_pra->res_info.use_encoder_dpcm);
			 ar_printf("dpcm_out_bits=%d\n",p_var_pra->res_info.dpcm_out_bits);			 
			 ar_printf("force_dng=%d\n",p_var_pra->res_info.force_dng);
			 ar_printf("tuning_name=%s\n",p_var_pra->res_info.tuning_name);
		     ar_printf("use_nr3d_encoder=%d\n",p_var_pra->res_info.use_nr3d_encoder);
		     ar_printf("ir_sensor=%d\n",p_var_pra->res_info.ir_sensor);			 
		 }
		 free(p_var_pra);
		 ar_hal_vin_close_pipe_dev(fd_pipe);
		 ar_hal_vin_close_vin_dev(vin_fd);

	  }else
	  {
	      ar_err("please set the dev_id");
	  }
   }
   return 0;
}

__test_camera_case__(get_sensor_dev_var_pra)=
{
   "--get_sensor_dev_var_pra",
   api_test_get_sensor_dev_var_pra,
   {
      "test the api ar_hal_vin_get_sensor_dev_var_pra, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [dev_id]",
	  "-res_index [index]: if index<0, not show res info, only common info and res counnt the sensor have, if index > res_count, we show all the res info ",
	  NULL,
   },
};
