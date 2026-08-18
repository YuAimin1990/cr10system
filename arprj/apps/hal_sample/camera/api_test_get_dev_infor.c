#include"camera.h"

static int api_test_get_dev_infor(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
   int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
   if(index>=0)
   {
       strcpy(p_cmd->args[index],"-local");
	   camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
   }else
   {
	     // int dev_id=atoi(p_cmd->args[index+1]);
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
			dump_dev_info(p_info);
		 }
		 free(p_info);
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(get_dev_infor)=
{
   "--get_dev_infor",
   api_test_get_dev_infor,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  NULL,
   },
};
