#include"camera.h"

static int api_test_get_3a_infor(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     int dev_id=0;
	     int vin_fd=ar_hal_vin_open_vin_dev();
		 AR_S32  dev_num=0;
		 int ret=ar_hal_vin_get_dev_num(vin_fd,&dev_num);
		 ar_always("dev_num=%d",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			//dump_dev_info(p_info);
		 }
		 free(p_info);

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
			   ar_printf("dev id =%d pipe_id=%d\n",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0){
			   	   STRU_BASIC_3A_INFO_T info;
				   int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				   if(fd_pipe>=0)
				   {
				        ar_hal_vin_get_3a_info(fd_pipe,pipe_id,&info);
						ar_hal_vin_close_pipe_dev(fd_pipe);
                        /*
      						AR_S32	 aec_mode;
      						AR_FLOAT gain;
      						AR_FLOAT exp_time_us;
      						AR_U32 luma_target;
      						AR_FLOAT gain_short;
      						AR_FLOAT exp_time_us_short;
						*/
						ar_printf("basic aec info ...................................\n");
                        ar_printf("aec_mode=%d\n",info.aec_info.aec_mode);
                        ar_printf("gain=%f\n",info.aec_info.gain);
						ar_printf("exp_time_us=%f\n",info.aec_info.exp_time_us);
						ar_printf("luma_target=%d\n",info.aec_info.luma_target);
						ar_printf("gain_short=%f\n",info.aec_info.gain_short);
						ar_printf("exp_time_us_short=%f\n",info.aec_info.exp_time_us_short);
						ar_printf("lux_index=%f\n",info.aec_info.lux_index);

						ar_printf("basic awb info ...................................\n");
						/*
      						AR_S32 awb_mode;
      						AR_U32 cct;
      						AR_FLOAT r_gain;
      						AR_FLOAT g_gain;
      						AR_FLOAT b_gain;
						*/
					    ar_printf("awb_mode=%d\n",info.awb_info.awb_mode);
						ar_printf("cct=%d\n",info.awb_info.cct);
						ar_printf("r_gain=%f\n",info.awb_info.r_gain);
						ar_printf("g_gain=%f\n",info.awb_info.g_gain);
						ar_printf("b_gain=%f\n",info.awb_info.b_gain);

						
				        ar_printf("basic af info ...................................\n");

				        ar_printf("af_mod=%d\n",info.af_info.af_mod);
						ar_printf("algo_type=%d\n",info.af_info.algo_type);
						ar_printf("len_position=%d\n",info.af_info.len_position);
						ar_printf("focus_value=%d\n",info.af_info.focus_value);
						ar_printf("stats_type=%d\n",info.af_info.stats_type);
						
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

__test_camera_case__(get_3a_infor)=
{
   "--get_3a_infor",
   api_test_get_3a_infor,
   {
      "test the api ar_hal_vin_get_3a_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  NULL,
   },
};
