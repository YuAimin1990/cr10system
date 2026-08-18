#include"camera.h"

typedef struct
{
   int event_client_id;
   ar_os_thread_id_t event_thread;
   AR_S32 fd_pipe;
   AR_S32 pipe_id;
   AR_S32 fd_vin_dev;
}STRU_TEST_EVENT_T;

static void *event_loop_thread(void* arg)
{
   STRU_TEST_EVENT_T *p_event_pbj=(STRU_TEST_EVENT_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->fd_vin_dev,p_opt);
   int event_data_size=sizeof(STRU_META_DATA_TYPE_T);
   int fd_mem=ar_hal_vin_open_dev_mem();
   while(1)
   {
        ar_hal_vin_get_meta_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_meta_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{

			case META_TYPE_AEC_GROUP:
			break;
			case META_TYPE_AWB_GROUP:
			break;
			case META_TYPE_AF_GROUP:
			break;
		    default:
		    break;
		}
		ar_hal_vin_release_meta_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}
static int api_test_pipe_meta_event(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
				      STRU_TEST_EVENT_T event_obj;
                      ar_hal_vin_create_meta_event_client(fd_pipe,pipe_id,&event_obj.event_client_id);
					  event_obj.fd_pipe=fd_pipe;
					  event_obj.fd_vin_dev=vin_fd;
					  event_obj.pipe_id=pipe_id;
					  ar_always("event_client_id=%d",event_obj.event_client_id);
					  ar_os_thread_attr_t ctrl_attr = {0};
					  ctrl_attr.name = "stream";
					  ctrl_attr.stack_size = 16 * 1024;
					  event_obj.event_thread=ar_osal_thread_new(event_loop_thread, &event_obj, &ctrl_attr);
					  //after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
					  for(int i=META_TYPE_AF_STATS;i<META_TYPE_MAX;i++)
					  {
					      ar_hal_vin_register_meta_event(fd_pipe,pipe_id,event_obj.event_client_id,i);
					  }
					  int delay_ms=5000;
					  index=camera_find_cmd_pra_index_by_name("-time",p_cmd);
					  if(index>0)
					  {
						  delay_ms=atoi(p_cmd->args[index+1]);
					  }
					  ar_always("after %d ms, the get event will exit waiting .....",delay_ms);
                      ar_delay(delay_ms);
                      //unregister all the event
                      ar_always("unregister all the event");
					   for(int i=META_TYPE_AF_STATS;i<META_TYPE_MAX;i++)
					  {
					      ar_hal_vin_unregister_meta_event(fd_pipe,pipe_id,event_obj.event_client_id,i);
					  }
					  ar_osal_thread_join(event_obj.event_thread);
					  ar_hal_vin_delete_meta_event_client(fd_pipe,pipe_id,event_obj.event_client_id);

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

__test_camera_case__(meta_event)=
{
   "--meta_event",
   api_test_pipe_meta_event,
   {
      "test meta event relate api, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-time [ms] hw many ms to loop get event",
	  NULL,
   },
};
