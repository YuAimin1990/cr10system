#include"camera.h"

static int api_test_get_3a_update(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
			   ar_printf("dev id =%d pipe_id=%d\n",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
				  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				  if(fd_pipe>=0)
				  {
    			      STRU_AEC_OUT_T aec_out;
                      ar_hal_vin_get_aec_update(fd_pipe,pipe_id,&aec_out);
					  ar_printf("aec update .....................................................\n");
					  ar_printf("exp_time_us=%d\n",aec_out.exp_time_us);
					  ar_printf("line_count=%d\n",aec_out.line_count);
					  ar_printf("real_gain=%f\n",aec_out.real_gain);
					  ar_printf("current_luma=%f\n",aec_out.current_luma);
    			      STRU_AWB_OUT_T awb_out;
                      ar_hal_vin_get_awb_update(fd_pipe,pipe_id,&awb_out);

					  ar_always("awb update .....................................................\n");
					  ar_always("cct=%f\n",awb_out.cct);
					  ar_always("r_gain=%f\n",awb_out.r_gain);
					  ar_always("g_gain=%f\n",awb_out.g_gain);
					  ar_always("b_gain=%f\n",awb_out.b_gain);
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

__test_camera_case__(get_3a_update)=
{
   "--get_3a_update",
   api_test_get_3a_update,
   {
      "-remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  NULL,
   },
};
