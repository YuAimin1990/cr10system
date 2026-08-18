#include"camera.h"
#include <sys/stat.h>


typedef struct
{
   void *phy_addr;
   void *phy_addr_orig;
   void *addr;
   void *addr_orig;
}STRU_RAW_FILE_MEM_T;

typedef struct
{
   STRU_AR_QUEUE_T *stream_buffer_pool;
   int  fd_vin;
   int  fd_pipe;
   int  pipe_id;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer_info[16];
   int stride;
   int raw_frame_num;
   int raw_size;
   STRU_RAW_FILE_MEM_T mem[16];
   ar_os_thread_id_t dqbuffer;
   int current_mem_index;
}SRTU_RAW_OBJ_T;

static int file_size(char *file)
{
   struct stat statbuf;
   stat(file,&statbuf);
   return statbuf.st_size;
}

static void *dq_loop_thread(void* arg)
{
    SRTU_RAW_OBJ_T *p_raw_obj=(SRTU_RAW_OBJ_T *)arg;
	while(1)
	{
        STRU_STREAM_BUFFER_INIFO_T *p_info=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
		ar_hal_vin_dq_stream_buffer(p_raw_obj->fd_pipe,p_raw_obj->pipe_id,0,p_info, -1);
		p_raw_obj->stream_buffer_pool->queue_insert(p_raw_obj->stream_buffer_pool,p_info);
	}
	ar_osal_thread_exit();
	return 0;
}

static int api_test_q_dq_buffer(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
				         SRTU_RAW_OBJ_T *p_raw_obj=malloc(sizeof(SRTU_RAW_OBJ_T));
						 p_raw_obj->fd_pipe=fd_pipe;
						 p_raw_obj->fd_vin=vin_fd;
						 p_raw_obj->pipe_id=pipe_id;
						 p_raw_obj->stream_buffer_pool=ar_creat_queue(16,"stream_buffer_pool");
                         //read the raw file
                         char *raw_file=NULL;
						 int num=0,res_idx = 0;
                         index=camera_find_cmd_pra_index_by_name("-raw",p_cmd);
                		 if(index>0)
                		 {
                			 raw_file=p_cmd->args[index+1];
							 num=atoi(p_cmd->args[index+2]);
							 res_idx = atoi(p_cmd->args[index+3]);
							 
                		 }
						 STRU_DEV_INFO_VAR_T *p_var_pra=malloc(sizeof(STRU_DEV_INFO_VAR_T));
						 p_var_pra->res_index=res_idx;
						 p_var_pra->res_count=1;
						 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
						 int file_len=file_size(raw_file);
						 int raw_size=file_len/num;
						 int stride=raw_size/p_var_pra->res_info.height;
						 p_raw_obj->stride=stride;
						 p_raw_obj->raw_frame_num=num;
						 p_raw_obj->raw_size=raw_size;
						 p_raw_obj->current_mem_index=0;
						 int size=0;
						 ar_always("stride = %d, height = %d, res_index = %d", stride, p_var_pra->res_info.height, res_idx);
                         ar_hal_vin_requst_stream_buffer(fd_pipe,pipe_id,0,p_raw_obj->stream_buffer_info,&size);

						 for(int i=0;i<size;i++)
						 {
						     STRU_STREAM_BUFFER_INIFO_T *p_info=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
							 *p_info=p_raw_obj->stream_buffer_info[i];
						     p_raw_obj->stream_buffer_pool->queue_insert(p_raw_obj->stream_buffer_pool,p_info);
						 }

                         int fd=open(raw_file,O_RDONLY);

                         for(int i=0;i<num;i++)
						 {

         				    AR_VOID *p_vaddr=NULL;
         				    AR_U64 phy_addr=0;
         				    ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,p_raw_obj->raw_size+1024);

                            if(p_vaddr==NULL || phy_addr==0)
                            {
                              ar_err("ar_hal_sys_mmz_alloc_cached err raw_size=0x%x",p_raw_obj->raw_size);
                              return -1;
                            }
							p_raw_obj->mem[i].addr=p_raw_obj->mem[i].addr_orig=p_vaddr;
							p_raw_obj->mem[i].addr=(void *)CAM_ALIGNE_TO((AR_U64)p_raw_obj->mem[i].addr,256);
							p_raw_obj->mem[i].phy_addr=p_raw_obj->mem[i].phy_addr_orig=(void *)phy_addr;
							p_raw_obj->mem[i].phy_addr=(void *)CAM_ALIGNE_TO(phy_addr,256);
							//fill the mem by file
							int read_size=read(fd,p_raw_obj->mem[i].addr,p_raw_obj->raw_size);
							ar_always("fill the raw men with file read_size=0x%x,raw_size=0x%x",read_size,p_raw_obj->raw_size);

                        }
                        //start to creat a thread to dqbuffer
						ar_os_thread_attr_t ctrl_attr = {0};
						ctrl_attr.name = "dqbuffer";
						ctrl_attr.stack_size = 16 * 1024;
						p_raw_obj->dqbuffer=ar_osal_thread_new(dq_loop_thread, p_raw_obj, &ctrl_attr);

                        int fps=30;
						index=camera_find_cmd_pra_index_by_name("-fps",p_cmd);
						if(index>0)
						{
						   fps=atoi(p_cmd->args[index+1]);
						}
						int run_at_max_fps=0;
						index=camera_find_cmd_pra_index_by_name("-run_at_max_fps",p_cmd);
						if(index>0)
						{
						   run_at_max_fps=1;
						}
                        //main thread gen raw mem and send to dq loop thread
                        while(1)
                        {
                            if(p_raw_obj->current_mem_index>=p_raw_obj->raw_frame_num)
                            {
                               p_raw_obj->current_mem_index=0;
                            }
							STRU_STREAM_BUFFER_INIFO_T *p_info=NULL;
							p_raw_obj->stream_buffer_pool->queue_pop(p_raw_obj->stream_buffer_pool,(void **)&p_info);
							if(p_info)
							{
                                  //insert to the stream queue
                                  p_info->panel_count=1;
								  p_info->pannel[0].used=p_raw_obj->raw_size;
								  p_info->pannel[0].length=p_info->pannel[0].used;
								  p_info->pannel[0].offset=0;
								  p_info->pannel[0].line_buffer_len=p_raw_obj->stride;
								  p_info->pannel[0].buffer_phy_addr=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr;
								  p_info->pannel[0].buffer_phy_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr_orig;
								  p_info->pannel[0].buffer=p_raw_obj->mem[p_raw_obj->current_mem_index].addr;
								  p_info->pannel[0].buffer_phy_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].addr_orig;
                                  ar_hal_vin_q_stream_buffer(fd_pipe,pipe_id,0,p_info);
								  p_raw_obj->current_mem_index++;
                                  free(p_info);

							}
                            if(run_at_max_fps)
                            {
								usleep(1000);  //30 fps
                            }else{
							   usleep(1000000/fps);  //30 fps
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

__test_camera_case__(q_dq_buffer)=
{
   "--q_dq_buffer",
   api_test_q_dq_buffer,
   {
      "test the api qbuffer dqbufer send a raw to isp to process, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-raw [name] [num]: the raw file name and how much frames in the file",
	  "-fps [fps]",
	  "-run_at_max_fps",
	  NULL,
   },
};
