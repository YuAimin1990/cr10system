#include"camera.h"

static int api_test_3a_algo_register(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
			dump_dev_info(p_info);
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
			   ar_always("dev id =%d pipe_id=%d",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
				  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				  if(fd_pipe>=0)
				  {
					  index=camera_find_cmd_pra_index_by_name("-reg",p_cmd);
				      if(index > 0)
					  {
					      int be_register=atoi(p_cmd->args[index+1]);
						  if(be_register==1)
						  {
         					  index=camera_find_cmd_pra_index_by_name("-aec",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_register_aec_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
         					  index=camera_find_cmd_pra_index_by_name("-awb",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_register_awb_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
         					  index=camera_find_cmd_pra_index_by_name("-af",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_register_af_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
						  }else
						  {
						      index=camera_find_cmd_pra_index_by_name("-aec",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_unregister_aec_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
         					  index=camera_find_cmd_pra_index_by_name("-awb",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_unregister_awb_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
         					  index=camera_find_cmd_pra_index_by_name("-af",p_cmd);
         					  if(index>0)
         					  {
         					      ar_hal_vin_unregister_af_algo_lib(fd_pipe,pipe_id,p_cmd->args[index+1]);
         					  }
						  }
				      }
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

__test_camera_case__(algo_3a_register)=
{
   "--3a_algo_register",
   api_test_3a_algo_register,
   {
      "test register 3a algo lib, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-reg [1/0]",
	  "-aec [name]",
	  "-awb [name]",
	  "-af [name]",
	  NULL,
   },
};
