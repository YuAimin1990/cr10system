#include"camera.h"

static int api_test_set_aec_manu(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
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
         int dev_id=0;
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
    			      STRU_AEC_SET_INFO_T value={0};
                      STRU_BASIC_3A_INFO_T basic_3a_info={0};
                      ar_hal_vin_get_3a_info(fd_pipe,pipe_id,&basic_3a_info);
                      value = basic_3a_info.aec_info;
                      ar_printf("current aec_mode=%d, gain=%f, exp_time_us=%f, luma_target=%d, gain_short=%f, exp_time_us_short=%f \n", value.aec_mode, value.gain, value.exp_time_us, value.luma_target, value.gain_short, value.exp_time_us_short);

					  index=camera_find_cmd_pra_index_by_name("-aec_mode",p_cmd);
					  if(index>=0){
					  	  value.aec_mode=atoi(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-gain",p_cmd);
					  if(index>=0){
					  	  value.gain=atof(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-exp_time_us",p_cmd);
					  if(index>=0){
					  	  value.exp_time_us=atof(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-luma_target",p_cmd);
					  if(index>=0){
					  	  value.luma_target=atoi(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-gain_short",p_cmd);
					  if(index>=0){
					  	  value.gain_short=atof(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-exp_time_us_short",p_cmd);
					  if(index>=0){
					  	  value.exp_time_us_short=atof(p_cmd->args[index +1]);
					  }

                      ar_printf("switch to aec_mode=%d, gain=%f, exp_time_us=%f, luma_target=%d, gain_short=%f, exp_time_us_short=%f\n", value.aec_mode, value.gain, value.exp_time_us, value.luma_target, value.gain_short, value.exp_time_us_short);
                      ar_hal_vin_aec_manu_set(fd_pipe,pipe_id,&value);

                      ar_delay(100);
                      ar_hal_vin_get_3a_info(fd_pipe,pipe_id,&basic_3a_info);
                      value = basic_3a_info.aec_info;
                      ar_printf("current aec_mode=%d, gain=%f, exp_time_us=%f, luma_target=%d, gain_short=%f, exp_time_us_short=%f after switch\n", value.aec_mode, value.gain, value.exp_time_us, value.luma_target, value.gain_short, value.exp_time_us_short);
				  }else
				  {
				      ar_err("open fd pipe failed");
				  }
				  ar_hal_vin_close_pipe_dev(fd_pipe);
			   }
			   else
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

__test_camera_case__(set_aec_manu)=
{
   "--set_aec_manu",
   api_test_set_aec_manu,
   {
      "test -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-aec_mode [0 -- 6, if missed, be not changed], 0: AEC_AUTO, 1: AEC_FULL_SWEEP, 2: AEC_AVERGATE, 3: AEC_CENTER_WEIGHT, 4: AEC_SPOT, 5: AEC_MANUAL, 6: AEC_LOCKED",
      "-gain [if missed, be not changed]",
      "-exp_time_us [if missed, be not changed]",
      "-luma_target [if missed, be not changed]",
      "-gain_short [if missed, be not changed]",
      "-exp_time_us_short [if missed, be not changed]",
	  NULL,
   },
};
