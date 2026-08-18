#include"camera.h"

static int api_test_set_sensor_dev_var_pra(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
		 ar_always("dev_num=%d",dev_num);
		 STRU_SENSOR_DEV_INFO_T *p_info=malloc(sizeof(STRU_SENSOR_DEV_INFO_T));
		 for(int i=0;i<dev_num;i++)
		 {
		    ar_hal_vin_get_dev_infor(vin_fd,i,p_info);
			//dump_dev_info(p_info);
		 }
		 free(p_info);
         ar_always("start get sensor var pra info");
		 int pipe_id=dev_id;
		 int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
		 ar_hal_vin_pipe_bind_to_dev(vin_fd,pipe_id,dev_id);
		 STRU_DEV_INFO_VAR_T *p_var_pra=malloc(sizeof(STRU_DEV_INFO_VAR_T));
		 p_var_pra->res_index=-1;
		 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
		 ar_always("res_count=%d",p_var_pra->res_count);

  		 p_var_pra->res_index=0;
  		 index=camera_find_cmd_pra_index_by_name("-res_index",p_cmd);
  		 if(index>0)
  		 {
  		     p_var_pra->res_index=atoi(p_cmd->args[index+1]);
  		 }
  		 ar_always("set var sensor pra p_var_pra->res_index=%d",p_var_pra->res_index);

		 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
		 ar_always("sensor w h = (%d %d) before set res_index=%d",p_var_pra->res_info.width,p_var_pra->res_info.height, p_var_pra->res_index);
		 index=camera_find_cmd_pra_index_by_name("-tuning",p_cmd);
         if(index>0)
		 {
		     ar_always("set var sensor pra tuning file to %s",p_cmd->args[index+1]);
		     strcpy(p_var_pra->res_info.tuning_name,p_cmd->args[index+1]);
		 }
		 index=camera_find_cmd_pra_index_by_name("-size",p_cmd);
		 if(index > 0){
     		 p_var_pra->res_info.crop_en=atoi(p_cmd->args[index+1]);
     		 p_var_pra->res_info.x_start=atoi(p_cmd->args[index+2]);
     		 p_var_pra->res_info.y_start=atoi(p_cmd->args[index+3]);
     		 p_var_pra->res_info.width=atoi(p_cmd->args[index+4]);
     		 p_var_pra->res_info.height=atoi(p_cmd->args[index+5]);
     		 p_var_pra->res_info.x_width=atoi(p_cmd->args[index+6]);
     		 p_var_pra->res_info.y_height=atoi(p_cmd->args[index+7]);
			 ar_always("%d %d %d %d %d %d %d",p_var_pra->res_info.crop_en,p_var_pra->res_info.x_start,p_var_pra->res_info.y_start,\
			 	p_var_pra->res_info.width,p_var_pra->res_info.height,p_var_pra->res_info.x_width,p_var_pra->res_info.y_height);
		 }
		 ar_always("sensor set w h = (%d %d)",p_var_pra->res_info.width,p_var_pra->res_info.height);
		 ar_hal_vin_set_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
		 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
		 ar_always("sensor w h = (%d %d) after set",p_var_pra->res_info.width,p_var_pra->res_info.height);
         free(p_var_pra);
		 ar_hal_vin_close_pipe_dev(fd_pipe);
		 ar_hal_vin_close_vin_dev(vin_fd);
	  }
   }
   return 0;
}

__test_camera_case__(set_sensor_dev_var_pra)=
{
   "--set_sensor_dev_var_pra",
   api_test_set_sensor_dev_var_pra,
   {
      "test the api ar_hal_vin_set_sensor_dev_var_pra, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [dev_id] -res_index [index]",
	  "-tuning [name]:eg :-tuning /usrdata/local/factory/tunning/cam_imx307/imx307_tuning_night_dwt_80.bin ",
	  "-size [crop 0/1] [x] [y] [w] [h] [x_width] [y_height] eg: -size 1 100 100 1920 1080 640 480",
	  NULL,
   },
};

