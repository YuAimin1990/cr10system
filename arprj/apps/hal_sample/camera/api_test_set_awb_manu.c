#include"camera.h"

static int api_test_set_awb_manu(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
    			      STRU_AWB_SET_INFO_T value={0};
                      STRU_BASIC_3A_INFO_T basic_3a_info={0};
                      ar_hal_vin_get_3a_info(fd_pipe,pipe_id,&basic_3a_info);
                      value = basic_3a_info.awb_info;
                      ar_printf("current awb_mode=%d, cct=%d, r_gain=%f, g_gain=%f, b_gain=%f\n", value.awb_mode, value.cct, value.r_gain, value.g_gain, value.b_gain);

					  index=camera_find_cmd_pra_index_by_name("-awb_mode",p_cmd);
					  if(index>=0){
					  	  value.awb_mode=atoi(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-cct",p_cmd);
					  if(index>=0){
					  	  value.cct=atoi(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-r_gain",p_cmd);
					  if(index>=0){
					  	  value.r_gain=atof(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-g_gain",p_cmd);
					  if(index>=0){
					  	  value.g_gain=atof(p_cmd->args[index +1]);
					  }
					  index=camera_find_cmd_pra_index_by_name("-b_gain",p_cmd);
					  if(index>=0){
					  	  value.b_gain=atof(p_cmd->args[index +1]);
					  }

                      ar_printf("switch to awb_mode=%d, cct=%d, r_gain=%f, g_gain=%f, b_gain=%f\n", value.awb_mode, value.cct, value.r_gain, value.g_gain, value.b_gain);
                      ar_hal_vin_awb_manu_set(fd_pipe,pipe_id,&value);

                      ar_delay(100);
                      ar_hal_vin_get_3a_info(fd_pipe,pipe_id,&basic_3a_info);
                      value = basic_3a_info.awb_info;
                      ar_printf("current awb_mode=%d, cct=%d, r_gain=%f, g_gain=%f, b_gain=%f after switch\n", value.awb_mode, value.cct, value.r_gain, value.g_gain, value.b_gain);
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

__test_camera_case__(set_awb_manu)=
{
   "--set_awb_manu",
   api_test_set_awb_manu,
   {
      "test -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-awb_mode [0 -- 11, if missed, be not changed], 0: AWB_D75=0, 1: AWB_D65, 2: AWB_D50, 3: AWB_NOON, 4: AWB_CW, 5: AWB_TL84, 6: AWB_A, 7: AWB_H, 8: AWB_CUST1, 9: AWB_CUST2, 10: AWB_AUTO, 11: AWB_MANUAL",
      "-cct [if missed, be not changed]",
      "-r_gain [if missed, be not changed]",
      "-g_gain [if missed, be not changed]",
      "-b_gain [if missed, be not changed]",
	  NULL,
   },
};
