#include"camera.h"


static int api_test_get_pipe_raw(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
		 ar_printf("dev_num=%d\n",dev_num);
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
					  //if(index>=0)
					  {
					  	  //to dump stream buffer
						  int num=1;
						  index=camera_find_cmd_pra_index_by_name("-num",p_cmd);
						  if(index>=0)
						  {
						     num=atoi(p_cmd->args[index+1]);
						  }
						  //open the stream fd
						  STRU_DUMP_SERVER_T *p_dump_server=creat_dump_server();
						  char *path="/local/pig";
						  index=camera_find_cmd_pra_index_by_name("-name",p_cmd);
						  if(index>0)
						  {
						     path=p_cmd->args[index+1];
						  }

						  time_t t = time(NULL);
						  char date[64] = "\0";
						  strftime(date, 63, "%Y-%m-%d-%H-%M-%S", localtime(&t));

						  char *file_name=malloc(1024);

						  STRU_CAM_PRA_GET_PIPE_BUFFER_T raw={0};

                          ar_hal_vin_get_pipe_raw(fd_pipe,pipe_id,&raw);

						  sprintf(file_name,"%s_%dx%d_%s.raw",path,raw.stream_buffer.width,raw.stream_buffer.height,date);
						  ar_printf("dump to file %s\n",file_name);
						  p_dump_server->dump_req(p_dump_server,file_name,num);
						  int loop_num=num+1;
						  ar_printf("dump_raw pipe_id=%d\n",pipe_id);
						  while(loop_num>0)
						  {
							  STRU_CAM_PRA_GET_PIPE_BUFFER_T raw_get=raw;

                              //malloc buffer
							  AR_VOID *p_vaddr=NULL;
							  AR_U64 phy_addr=0;

                              for(int k=0;k<raw_get.stream_buffer.panel_count;k++)
							  {
							      ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,raw_get.stream_buffer.pannel[k].length);
    							  if(p_vaddr==NULL || phy_addr==0)
    							  {
    								ar_err("ar_hal_sys_mmz_alloc_cached err");
    								return -1;
    							  }
								  raw_get.stream_buffer.pannel[k].buffer=raw_get.stream_buffer.pannel[k].buffer_orign=p_vaddr;
								  raw_get.stream_buffer.pannel[k].buffer_phy_addr=raw_get.stream_buffer.pannel[k].buffer_phy_orign=phy_addr;

                              }

							  ar_hal_vin_get_pipe_raw(fd_pipe,pipe_id,&raw_get);
							   if(raw_get.stream_buffer.panel_count==3)
							   {
							       p_dump_server->dump_segment(p_dump_server,\
								   	raw_get.stream_buffer.pannel[0].buffer_phy_addr,\
								   	raw_get.stream_buffer.pannel[1].buffer_phy_addr,\
								   	raw_get.stream_buffer.pannel[2].buffer_phy_addr,\
								   	NULL,\
								   	raw_get.stream_buffer.pannel[0].used,\
								   	raw_get.stream_buffer.pannel[1].used,\
								   	raw_get.stream_buffer.pannel[2].used,\
								   	0);
							   }else if(raw_get.stream_buffer.panel_count==1)
							   {
							       p_dump_server->dump_segment(p_dump_server,\
								   	raw_get.stream_buffer.pannel[0].buffer_phy_addr,\
								   	NULL,\
								   	NULL,\
								   	NULL,\
								   	raw_get.stream_buffer.pannel[0].used,\
								   	0,\
								   	0,\
								   	0);
							   }else if(raw_get.stream_buffer.panel_count==2)
							   {
							       	p_dump_server->dump_segment(p_dump_server,\
								   	raw_get.stream_buffer.pannel[0].buffer_phy_addr,\
								   	raw_get.stream_buffer.pannel[1].buffer_phy_addr,\
								   	NULL,\
								   	NULL,\
								   	raw_get.stream_buffer.pannel[0].used,\
								   	raw_get.stream_buffer.pannel[1].used,\
								   	0,\
								   	0);
							   }
							   for(int k=0;k<raw_get.stream_buffer.panel_count;k++)
							   {
							       ar_hal_sys_mmz_free(raw_get.stream_buffer.pannel[k].buffer_phy_addr,raw_get.stream_buffer.pannel[k].buffer);
							   }
							   loop_num--;
						  }
						  delete_dump_server(p_dump_server);
                          free(file_name);
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

__test_camera_case__(get_pipe_raw)=
{
   "--get_pipe_raw",
   api_test_get_pipe_raw,
   {
      "test the api ar_hal_vin_get_dev_infor, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-num [num]: how much frame will be dumped",
	  "-name [name]: the dump file name. such  as isp_out, not suggust isp_out.yuv",
	  NULL,
   },
};
