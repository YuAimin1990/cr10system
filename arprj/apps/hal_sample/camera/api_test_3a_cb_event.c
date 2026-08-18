#include"camera.h"
#include "hal_vin_aec_algo_lib.h"
#include "hal_vin_awb_algo_lib.h"
#include "hal_vin_af_algo_lib.h"

typedef struct
{
   int event_data_size;
   int fd_mem;
   int aec_event_client_id;
   int awb_event_client_id;
   int af_event_client_id;
   ar_os_thread_id_t event_aec_thread;
   ar_os_thread_id_t event_awb_thread;
   ar_os_thread_id_t event_af_thread;
   AR_S32 fd_pipe;
   AR_S32 pipe_id;
   AR_S32 fd_vin_dev;
}STRU_TEST_3A_CB_EVENT_T;

static int  aec_ctl(STRU_TEST_3A_CB_EVENT_T *p_event_pbj,STRU_AEC_CB_EVENT_CTL_PRA_T *p_pra)
{
   AR_S32 offset_ret=0;
   void *event_data_maped=ar_hal_vin_map_phy_addr(p_event_pbj->fd_mem,p_pra->pra,p_event_pbj->event_data_size,&offset_ret);
   // void *event_data=event_data_maped+offset_ret;
   switch(p_pra->clt_code)
   {
      case AEC_CTL_HW_START:
      case AEC_CTL_HW_STOP:
      case AEC_CTL_HW_PICK_RES:
      case AEC_CTL_HW_SET_IN_OUT_FORMAT:
      case AEC_CTL_GET_IMG_BRIGHTNESS:
      case AEC_CTL_SET_IMG_BRIGHTNESS:
      case AEC_CTL_SET_FACE_AEC:
      case AEC_CTL_SET_AE_MODE:
      case AEC_CTL_GET_AE_MODE:
      case AEC_CTL_SET_MANUAL_AE:
      case AEC_CTL_SET_SENCE_MODE:
      case AEC_CTL_GET_SCENE_MODE:
      case AEC_CTL_CAM_PRA_SET_SENSOR_HDR:
      case AEC_CTL_CAM_PRA_GET_SENSOR_HDR:
      case AEC_CTL_CAM_PRA_SET_HDR_AUTO_DETECT:
      case AEC_CTL_CAM_PRA_GET_HDR_AUTO_DETECT:
      case AEC_CTL_CAM_PRA_SET_HDR_PRA_MODE:
      case AEC_CTL_CAM_PRA_GET_HDR_PRA_MODE:
      case AEC_CTL_CAM_PRA_SET_FPS_RANGE:
      case AEC_CTL_CAM_PRA_SET_EXP_LIMIT:
      case AEC_CTL_CAM_PRA_SET_ANTIBANDING:
      case AEC_CTL_CAM_PRA_GET_ANTIBANDING:
      case AEC_CTL_CAM_PRA_SET_MOTION_DETECT:
      case AEC_CTL_CAM_PRA_GET_BASIC_3A_INFO:
      case AEC_CTL_EVENT_AWB_UPDATE:
	  default:
	  break;
   }
   ar_hal_vin_ummap_vaddr(event_data_maped,p_event_pbj->event_data_size);
   return 0;
}

static void *event_aec_loop_thread(void* arg)
{
   STRU_TEST_3A_CB_EVENT_T *p_event_pbj=(STRU_TEST_3A_CB_EVENT_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->fd_vin_dev,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   p_event_pbj->event_data_size=event_data_size;
   while(1)
   {
        ar_hal_vin_get_aec_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->aec_event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_aec_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->aec_event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
		   case AEC_CB_EVENT_INIT:
		   {
		   	  // STRU_AEC_CB_EVENT_INIT_PRA_T *p_pra=(STRU_AEC_CB_EVENT_INIT_PRA_T *)event_data;
			  ar_always("AEC_CB_EVENT_INIT");
		   }
		   break;
		   case AEC_CB_EVENT_EXIT:
		   {
			   // STRU_AEC_CB_EVENT_EXIT_PRA_T *p_pra=(STRU_AEC_CB_EVENT_EXIT_PRA_T *)event_data;
			   ar_always("AEC_CB_EVENT_EXIT");
		   }
		   break;
		   case AEC_CB_EVENT_RUN:
		   {
			   // STRU_AEC_CB_EVENT_RUN_PRA_T *p_pra=(STRU_AEC_CB_EVENT_RUN_PRA_T *)event_data;
			   ar_always("AEC_CB_EVENT_RUN");
		   }
		   break;
		   case AEC_CB_EVENT_CTL:
		   {
			   STRU_AEC_CB_EVENT_CTL_PRA_T *p_pra=(STRU_AEC_CB_EVENT_CTL_PRA_T *)event_data;
			   ar_always("AEC_CB_EVENT_CTL");
			   aec_ctl(p_event_pbj,p_pra);
		   }
		   break;
		   default:
		   break;
		}
		ar_hal_vin_release_aec_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->aec_event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}
static int  awb_ctl(STRU_TEST_3A_CB_EVENT_T *p_event_pbj,STRU_AWB_CB_EVENT_CTL_PRA_T *p_pra)
{
   AR_S32 offset_ret=0;
   void *event_data_maped=ar_hal_vin_map_phy_addr(p_event_pbj->fd_mem,p_pra->pra,p_event_pbj->event_data_size,&offset_ret);
   // void *event_data=event_data_maped+offset_ret;
   switch(p_pra->clt_code)
   {
	   case AWB_CTL_HW_START:
	   case AWB_CTL_HW_STOP:
	   case AWB_CTL_HW_PICK_RES:
	   case AWB_CTL_HW_SET_IN_OUT_FORMAT:
	   case AWB_CTL_EVENT_AEC_UPDATE:
	   case AWB_CTL_SET_DEBUG_INFO:
	   case AWB_CTL_CAM_PRA_SET_AWB_MODE:
	   case AWB_CTL_CAM_PRA_SET_MANUAL_AWB:
	   case AWB_CTL_CAM_PRA_GET_BASIC_3A_INFO:
	   default:
	   break;
   }
   ar_hal_vin_ummap_vaddr(event_data_maped,p_event_pbj->event_data_size);
   return 0;
}

static void *event_awb_loop_thread(void* arg)
{
   STRU_TEST_3A_CB_EVENT_T *p_event_pbj=(STRU_TEST_3A_CB_EVENT_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->fd_vin_dev,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   while(1)
   {
        ar_hal_vin_get_awb_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->awb_event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_awb_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->awb_event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
           case AWB_CB_EVENT_INIT:
		   {
			   // STRU_AWB_CB_EVENT_INIT_PRA_T *p_pra=(STRU_AWB_CB_EVENT_INIT_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_INIT");
		   }
		   break;
           case AWB_CB_EVENT_EXIT:
		   {
			   // STRU_AWB_CB_EVENT_EXIT_PRA_T *p_pra=(STRU_AWB_CB_EVENT_EXIT_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_EXIT");
		   }
		   break;
           case AWB_CB_EVENT_RUN:
		   {
			   // STRU_AWB_CB_EVENT_RUN_PRA_T *p_pra=(STRU_AWB_CB_EVENT_RUN_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_RUN");
		   }
		   break;
           case AWB_CB_EVENT_CTL:
		   {
			   STRU_AWB_CB_EVENT_CTL_PRA_T *p_pra=(STRU_AWB_CB_EVENT_CTL_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_CTL");
			   awb_ctl(p_event_pbj,p_pra);
		   }
		   break;
		   default:
		   break;
		}
		ar_hal_vin_release_awb_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->awb_event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}
static int  af_ctl(STRU_TEST_3A_CB_EVENT_T *p_event_pbj,STRU_AEC_CB_EVENT_CTL_PRA_T *p_pra)
{
   AR_S32 offset_ret=0;
   void *event_data_maped=ar_hal_vin_map_phy_addr(p_event_pbj->fd_mem,p_pra->pra,p_event_pbj->event_data_size,&offset_ret);
   // void *event_data=event_data_maped+offset_ret;
   switch(p_pra->clt_code)
   {
	   case AWB_CTL_HW_START:
	   case AWB_CTL_HW_STOP:
	   case AWB_CTL_HW_PICK_RES:
	   case AWB_CTL_HW_SET_IN_OUT_FORMAT:
	   case AWB_CTL_EVENT_AEC_UPDATE:
	   case AWB_CTL_SET_DEBUG_INFO:
	   case AWB_CTL_CAM_PRA_SET_AWB_MODE:
	   case AWB_CTL_CAM_PRA_SET_MANUAL_AWB:
	   case AWB_CTL_CAM_PRA_GET_BASIC_3A_INFO:
	   default:
	   break;
   }
   ar_hal_vin_ummap_vaddr(event_data_maped,p_event_pbj->event_data_size);
   return 0;
}

static void *event_af_loop_thread(void* arg)
{
   STRU_TEST_3A_CB_EVENT_T *p_event_pbj=(STRU_TEST_3A_CB_EVENT_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->fd_vin_dev,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   while(1)
   {
        ar_hal_vin_get_af_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->af_event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_af_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->af_event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        // void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
    	   case AF_CB_EVENT_INIT:
		   {
			   // STRU_AF_CB_EVENT_INIT_PRA_T *p_pra=(STRU_AF_CB_EVENT_INIT_PRA_T *)event_data;
			   ar_always("AF_CB_EVENT_INIT");
		   }
		   break;
    	   case	AF_CB_EVENT_EXIT:
		   {
			   // STRU_AF_CB_EVENT_EXIT_PRA_T *p_pra=(STRU_AF_CB_EVENT_EXIT_PRA_T *)event_data;
			   ar_always("AF_CB_EVENT_EXIT");
		   }
		   break;
    	   case AF_CB_EVENT_RUN:
		   {
			   // STRU_AF_CB_EVENT_RUN_PRA_T *p_pra=(STRU_AF_CB_EVENT_RUN_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_CTL");
		   }
		   break;
    	   case AF_CB_EVENT_CTL:
		   {
			   // STRU_AWB_CB_EVENT_CTL_PRA_T *p_pra=(STRU_AWB_CB_EVENT_CTL_PRA_T *)event_data;
			   ar_always("AWB_CB_EVENT_CTL");
		   }
		   break;
		   default:
		   break;
		}
		ar_hal_vin_release_af_cb_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->af_event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}


static int api_test_pipe_event(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
				      STRU_TEST_3A_CB_EVENT_T event_obj;
					  int fd_mem=ar_hal_vin_open_dev_mem();
					  event_obj.fd_mem=fd_mem;
                      ar_hal_vin_create_aec_cb_event_client(fd_pipe,pipe_id,&event_obj.aec_event_client_id);
                      ar_hal_vin_create_awb_cb_event_client(fd_pipe,pipe_id,&event_obj.awb_event_client_id);
                      ar_hal_vin_create_af_cb_event_client(fd_pipe,pipe_id,&event_obj.af_event_client_id);
					  event_obj.fd_pipe=fd_pipe;
					  event_obj.fd_vin_dev=vin_fd;
					  event_obj.pipe_id=pipe_id;
					  ar_always("event_client_id=%d %d %d",event_obj.aec_event_client_id,event_obj.awb_event_client_id,event_obj.af_event_client_id);
					  ar_os_thread_attr_t ctrl_attr = {0};
					  ctrl_attr.name = "stream";
					  ctrl_attr.stack_size = 16 * 1024;
					  event_obj.event_aec_thread=ar_osal_thread_new(event_aec_loop_thread, &event_obj, &ctrl_attr);
					  event_obj.event_awb_thread=ar_osal_thread_new(event_awb_loop_thread, &event_obj, &ctrl_attr);
					  event_obj.event_af_thread=ar_osal_thread_new(event_af_loop_thread, &event_obj, &ctrl_attr);
					  //after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
					  for(int i=AEC_CB_EVENT_INIT;i<AEC_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_register_aec_cb_event(fd_pipe,pipe_id,event_obj.aec_event_client_id,i);
					  }
					  for(int i=AWB_CB_EVENT_INIT;i<AWB_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_register_awb_cb_event(fd_pipe,pipe_id,event_obj.awb_event_client_id,i);
					  }
					  for(int i=AF_CB_EVENT_INIT;i<AF_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_register_af_cb_event(fd_pipe,pipe_id,event_obj.af_event_client_id,i);
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
					  for(int i=AEC_CB_EVENT_INIT;i<AEC_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_unregister_aec_cb_event(fd_pipe,pipe_id,event_obj.aec_event_client_id,i);
					  }
					  for(int i=AWB_CB_EVENT_INIT;i<AWB_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_unregister_awb_cb_event(fd_pipe,pipe_id,event_obj.awb_event_client_id,i);
					  }
					  for(int i=AF_CB_EVENT_INIT;i<AF_CB_EVENT_MAX;i++)
					  {
					      ar_hal_vin_unregister_af_cb_event(fd_pipe,pipe_id,event_obj.af_event_client_id,i);
					  }
					  ar_osal_thread_join(event_obj.event_aec_thread);
					  ar_osal_thread_join(event_obj.event_awb_thread);
					  ar_osal_thread_join(event_obj.event_af_thread);
					  ar_hal_vin_delete_aec_cb_event_client(fd_pipe,pipe_id,event_obj.aec_event_client_id);
					  ar_hal_vin_delete_awb_cb_event_client(fd_pipe,pipe_id,event_obj.awb_event_client_id);
					  ar_hal_vin_delete_af_cb_event_client(fd_pipe,pipe_id,event_obj.af_event_client_id);

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

__test_camera_case__(event_3a_cb)=
{
   "--3a_cb_event",
   api_test_pipe_event,
   {
      "test the 3a callback event, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-time [ms] hw many ms to loop get event",
	  NULL,
   },
};
