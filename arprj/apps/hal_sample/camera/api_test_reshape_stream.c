#include"camera.h"

static int api_test_reshape_stream(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
			   ar_printf("dev id =%d pipe_id=%d\n",dev_id,pipe_id);
			   if(ret>=0 && pipe_id>=0)
			   {
				  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
				  if(fd_pipe>=0)
				  {
                      //resize stream
                      STRU_AR_STREAM_PROPERTY_T stream_prop;
                      AR_S32 stream_id[8];
					  AR_S32 num=0;
                      ar_hal_vin_get_all_stream(fd_pipe,pipe_id,stream_id,&num);
                      if(num > 0)
                      {
                          for(int i=0;i<num;i++)
                          {
                              STRU_AR_STREAM_PROPERTY_T prop;
                              ar_hal_vin_get_stream_property(fd_pipe,pipe_id,i,&prop);
							  ar_printf("stream id = %d =================\n",i);
							  ar_printf("camera_id=%d\n",prop.camera_id);
							  ar_printf("stream_type=%d\n",prop.stream_type);
							  ar_printf("mode=%d\n",prop.mode);
							  ar_printf("width=%d\n",prop.width);
							  ar_printf("height=%d\n",prop.height);
							  ar_printf("format=%d\n",prop.format);
							  ar_printf("fps=%f\n",prop.fps);
							  ar_printf("x=%d\n",prop.stream_type);
							  ar_printf("y=%d\n",prop.stream_type);
							  ar_printf("total_w=%d\n",prop.total_w);
							  ar_printf("total_h=%d\n",prop.total_h);
							  ar_printf("stride=%d\n",prop.stride);
                          }
                 	      int stream_id=0;
                 		  index=camera_find_cmd_pra_index_by_name("-stream_id",p_cmd);
                 		  if(index>0)
                 		  {
                 			 stream_id=atoi(p_cmd->args[index+1]);
							 ar_printf("resize stream %d\n",stream_id);
							 ar_hal_vin_get_stream_property(fd_pipe,pipe_id,stream_id,&stream_prop);
                             int reshape_2_w=640;
							 int reshape_2_h=480;
                             index=camera_find_cmd_pra_index_by_name("-reshape2",p_cmd);
                 		     if(index>0)
                 		     {
                 		         reshape_2_w=atoi(p_cmd->args[index+1]);
								 reshape_2_h=atoi(p_cmd->args[index+2]);
                 		     }

							 ar_printf("reshape to %d %d\n",reshape_2_w,reshape_2_h);
							 stream_prop.width=reshape_2_w;
							 stream_prop.height=reshape_2_h;

                             int anti=stream_prop.anti;
                             index=camera_find_cmd_pra_index_by_name("-anti",p_cmd);
							 if(index>=0)
							 {
							     anti=atoi(p_cmd->args[index+1]);
							 }
                             stream_prop.anti=anti;
                             ar_printf("anti=%d\n",anti);

                             float fps = stream_prop.fps;
                             index = camera_find_cmd_pra_index_by_name("-fps", p_cmd);
                 		     if(index>0)
                 		     {
                 		         fps=atof(p_cmd->args[index+1]);
                 		     }
                             stream_prop.fps = fps;
                             ar_printf("fps=%f\n", fps);

							 ar_hal_vin_reshape_stream(fd_pipe,pipe_id,stream_id,&stream_prop);
		                  }else
		                  {
		                     ar_printf("no stream id ,can not test\n");
		                  }
                      }else
                      {
                         ar_err("no stream created,can not do this test");
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

__test_camera_case__(reshape_stream)=
{
   "--reshape_stream",
   api_test_reshape_stream,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-stream_id [id]: if this opt not be set, this test will show all the stream",
	  "-reshape2 [w] [h]",
	  "-fps [fps]",
	  "-anti [0/1]",
	  NULL,
   },
};
