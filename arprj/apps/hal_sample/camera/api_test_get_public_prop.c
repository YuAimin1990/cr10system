#include"camera.h"

static int api_test_get_public_property(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
		 STRU_VIN_DRIVER_PUBLIC_PROPERTY_T public_prop;
         ar_hal_vin_get_public_property(vin_fd,&public_prop);
		 ar_printf("\n");
		 ar_printf("cam_mode=%d\n",public_prop.cam_mode);
		 ar_printf("isp_used=%d\n",public_prop.isp_used);
		 ar_printf("vif_isp_mode=%d\n",public_prop.vif_isp_mode);
		 ar_printf("eis_ldc_mode=%d\n",public_prop.eis_ldc_mode);
		 ar_printf("vif_fre_mod=%d\n",public_prop.vif_fre_mod);
		 ar_printf("vif_fre_hz=%d\n",public_prop.vif_fre_hz);
		 ar_printf("isp_fre_mod=%d\n",public_prop.isp_fre_mod);
		 ar_printf("isp_fre_hz=%d\n",public_prop.isp_fre_hz);
		 ar_printf("hdr_fre_mod=%d\n",public_prop.hdr_fre_mod);
		 ar_printf("hdr_fre_hz=%d\n",public_prop.hdr_fre_hz);
		 
		 ar_hal_vin_close_vin_dev(vin_fd);
   }
   return 0;
}

__test_camera_case__(get_public_prop)=
{
   "--get_public_prop",
   api_test_get_public_property,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  NULL,
   },
};
