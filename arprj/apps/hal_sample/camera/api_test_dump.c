#include"camera.h"

static int api_test_dump_frame(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
					  int num=0;
					  int stream_id[8];
                      ar_hal_vin_get_all_stream(fd_pipe,pipe_id,stream_id,&num);
					  ar_printf("stream num %d\n",num);
					  index=camera_find_cmd_pra_index_by_name("-stream_id",p_cmd);
					  if(index>=0){
					  	  //to dump stream buffer
					  	  int stream_id=atoi(p_cmd->args[index+1]);
						  int num=1;
						  index=camera_find_cmd_pra_index_by_name("-num",p_cmd);
						  if(index>=0)
						  {
						     num=atoi(p_cmd->args[index+1]);
						  }
						  //open the stream fd
						  int fd_stream=ar_hal_vin_open_stream_dev(pipe_id,stream_id);
						  STRU_DUMP_SERVER_T *p_dump_server=creat_dump_server();
						  char *path="/local/pig";
						  index=camera_find_cmd_pra_index_by_name("-name",p_cmd);
						  if(index>0)
						  {
						     path=p_cmd->args[index+1];
						  }
						  STRU_AR_STREAM_PROPERTY_T prop;
						  ar_hal_vin_get_stream_property(fd_pipe,pipe_id,stream_id,&prop);

						  time_t t = time(NULL);
						  char date[64] = "\0";
						  strftime(date, 63, "%Y-%m-%d-%H-%M-%S", localtime(&t));

						  char *file_name=malloc(1024);
						  if(prop.stream_type==STREAM_TYPE_RAW_BAYER)
						  {
						     sprintf(file_name,"%s_%dx%d_%s.raw",path,prop.stride/2,prop.total_h,date);
						  }else{
						     sprintf(file_name,"%s_%dx%d_%s.yuv",path,prop.stride,prop.total_h,date);
						  }
						  ar_printf("dump to file %s\n",file_name);
						  p_dump_server->dump_req(p_dump_server,file_name,num);
						  int client_id=0;
						  ar_hal_vin_create_stream_client(fd_pipe,pipe_id,stream_id,&client_id);
						  int loop_num=num+1;
						  ar_printf("dump_stream %d client id=%d fd_stream=%d pipe_id=%d\n",stream_id,client_id,fd_stream,pipe_id);
						  while(loop_num>0)
						  {
						       STRU_STREAM_BUFFER_INIFO_T stream_buffer;
                               ar_hal_vin_get_stream_buffer(fd_stream,pipe_id,stream_id,client_id,&stream_buffer,-1);
							   if(stream_buffer.panel_count==3)
							   {
							       p_dump_server->dump_segment(p_dump_server,\
								   	stream_buffer.pannel[0].buffer_phy_addr,\
								   	stream_buffer.pannel[1].buffer_phy_addr,\
								   	stream_buffer.pannel[2].buffer_phy_addr,\
								   	NULL,\
								   	stream_buffer.pannel[0].used,\
								   	stream_buffer.pannel[1].used,\
								   	stream_buffer.pannel[2].used,\
								   	0);
							   }else if(stream_buffer.panel_count==1)
							   {
							       p_dump_server->dump_segment(p_dump_server,\
								   	stream_buffer.pannel[0].buffer_phy_addr,\
								   	NULL,\
								   	NULL,\
								   	NULL,\
								   	stream_buffer.pannel[0].used,\
								   	0,\
								   	0,\
								   	0);
							   }else
							   {
							       ar_printf("not supported dump pannels =%d\n",stream_buffer.panel_count);
							   }
							   loop_num--;
							   ar_hal_vin_return_stream_buffer(fd_stream,pipe_id,stream_id,&stream_buffer);
						  }
						  ar_hal_vin_delete_stream_client(fd_pipe,pipe_id,stream_id,client_id);
						  delete_dump_server(p_dump_server);
						  ar_hal_vin_close_stream_dev(fd_stream);
                          free(file_name);
					  }else
					  {
					     ar_printf("the dev %d have following stream ...\n",dev_id);
						 for(int i=0;i<num;i++)
						 {
						     STRU_AR_STREAM_PROPERTY_T prop;
                             ar_hal_vin_get_stream_property(fd_pipe,pipe_id,i,&prop);
				             ar_printf("stream id = %d =================\n",i);
							 ar_printf("camera_id=%d \n",prop.camera_id);
							 ar_printf("stream_type=%d \n",prop.stream_type);
							 ar_printf("mode=%d\n",prop.mode);
							 ar_printf("width=%d\n",prop.width);
							 ar_printf("height=%d\n",prop.height);
							 ar_printf("format=%d\n",prop.format);
							 ar_printf("fps=%f\n",prop.fps);
							 ar_printf("x=%d\n",prop.x);
							 ar_printf("y=%d\n",prop.y);
							 ar_printf("total_w=%d\n",prop.total_w);
							 ar_printf("total_h=%d\n",prop.total_h);
							 ar_printf("stride=%d\n",prop.stride);
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

__test_camera_case__(dump_frame)=
{
   "--dump_frame",
   api_test_dump_frame,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-stream_id [id]:",
	  "-num [num]: how much frame will be dumped",
	  "-name [name]: the dump file name. such  as isp_out, not suggust isp_out.yuv",
	  NULL,
   },
};
