#include"camera.h"

typedef struct
{
   int event_client_id;
   ar_os_thread_id_t event_thread;
   AR_S32 fd_pipe;
   AR_S32 pipe_id;
   AR_S32 fd_vin_dev;

   AR_S32 dev_id_raw;
   AR_S32 fd_pipe_raw;
   AR_S32 pipe_id_raw;
   int fd_stream;
   int stream_id;
   ar_os_thread_id_t dqbuffer;
   ar_os_thread_id_t qbuffer;


   STRU_AR_QUEUE_T *stream_buffer_pool;
   STRU_STREAM_BUFFER_INIFO_T stream_buffer_info[16];

   STRU_AR_QUEUE_T *raw_queue;
   ar_signal_t raw_queue_singnal;
}STRU_RAW_PROCCESS_T;

static void *event_loop_thread(void* arg)
{
   STRU_RAW_PROCCESS_T *p_raw_process_obj=(STRU_RAW_PROCCESS_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_raw_process_obj->fd_vin_dev,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   while(1)
   {
        ar_hal_vin_get_event(p_raw_process_obj->fd_pipe,p_raw_process_obj->pipe_id,p_raw_process_obj->event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_event(p_raw_process_obj->fd_pipe,p_raw_process_obj->pipe_id,p_raw_process_obj->event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
           	case NOTIFY_EVENT_ERR:
			{
                ar_always("NOTIFY_EVENT_ERR");
			}
			break;
            case NOTIFY_EVENT_SHUTTER:
			{
                ar_always("NOTIFY_SHUTTER");
			}
			break;
            case NOTIFY_EVENT_FOCUS_STATUS:
			{

                ar_always("NOTIFY_FOCUS_STATUS");
			}
			break;
            case NOTIFY_EVENT_CONNECT:
			{
                ar_always("NOTIFY_EVENT_CONNECT");
			}
			break;
            case NOTIFY_EVENT_DISCONNECT:
			{
                ar_always("NOTIFY_EVENT_DISCONNECT");
			}
			break;
            case NOTIFY_EVENT_FORAMAT_CHANGE:
			{
                ar_always("NOTIFY_EVENT_FORAMAT_CHANGE");
			}
			break;
            case NOTIFY_EVENT_UNSUPPORTED:
			{
                ar_always("NOTIFY_SENSCE_CHANGED");
			}
			break;
            case NOTIFY_EVENT_STREAM_TIMEOUT:
			{
                ar_always("NOTIFY_BUFFER_SINK_STREAM_TIMEOUT");
			}
			break;
            case NOTIFY_EVENT_MIPI_ERR:
			{
                ar_always("NOTIFY_MIPI_ERR");
			}
			break;

            case NOTIFY_EVENT_SOF:
			{
#ifndef DISABLE_DBG_LOG
                STRU_NOTIFY_EVENT_SOF_PRA_T *p_sof=(STRU_NOTIFY_EVENT_SOF_PRA_T *)event_data;
                ar_debug("NOTIFY_EVENT_SOF,sof_count=%d",p_sof->sof_count);
#endif
				STRU_AR_CAMERA_SOF_PRO_T p_sof_prop;
				p_sof_prop.sof_mode=1;
				p_sof_prop.to_trigger=1;
				ar_hal_vin_vsync_update(p_raw_process_obj->fd_pipe_raw,p_raw_process_obj->pipe_id_raw,&p_sof_prop);
			}
			break;
            case NOTIFY_EVENT_AEC_UPDATE:
			{
				STRU_AEC_OUT_T *p_aec_out=(STRU_AEC_OUT_T *)event_data;
                ar_debug("luma=%f real_gain=%f exp_time_us=%d line_count=%d exp_index=%d",\
					p_aec_out->current_luma,p_aec_out->real_gain,p_aec_out->exp_time_us,p_aec_out->line_count,p_aec_out->exp_index);
				ar_hal_vin_aec_update(p_raw_process_obj->fd_pipe_raw,p_raw_process_obj->pipe_id_raw,p_aec_out);
			}
			break;
            case NOTIFY_EVENT_AWB_UPDATE:
			{
#ifndef DISABLE_DBG_LOG
				 STRU_AWB_OUT_T *p_awb_out=(STRU_AWB_OUT_T *)event_data;
				 ar_debug("cct=%f r_gain=%f b_gain=%f",p_awb_out->cct,p_awb_out->r_gain,p_awb_out->b_gain);
#endif
			}
			break;
            case NOTIFY_EVENT_AF_UPDATE:
			{
#ifndef DISABLE_DBG_LOG
				STRU_AF_OUT_T *p_af_out=(STRU_AF_OUT_T *)event_data;
				ar_debug("lens_position=%d move_step=%d dir=%d b_move_lens=%d",\
					p_af_out->lens_position,p_af_out->move_step,p_af_out->dir,p_af_out->b_move_lens);
#endif
			}
			break;
			case NOTIFY_EVENT_NOBUFFER:
			{
                ar_always("NOTIFY_MIPI_ERR");
			}
			break;
			case NOTIFY_EVENT_STREAM_BUFFER_CFG:
			{
                ar_always("NOTIFY_MIPI_ERR");
			}
			break;
		    default:
		    break;
		}
		ar_hal_vin_release_event(p_raw_process_obj->fd_pipe,p_raw_process_obj->pipe_id,p_raw_process_obj->event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_osal_thread_exit();
   return NULL;
}

static int raw_dev_init(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd,STRU_RAW_PROCCESS_T *p_raw_process_obj)
{
    int index=0;
	int dev_id=0;
	index=camera_find_cmd_pra_index_by_name("-raw_id",p_cmd);
	if(index>0)
	{
		dev_id=atoi(p_cmd->args[index+1]);
	}
	p_raw_process_obj->dev_id_raw=dev_id;
	int opend=0;
	int ret=ar_hal_vin_dev_is_opend(p_raw_process_obj->fd_vin_dev,dev_id,&opend);
	if(ret<0 || opend)
	{
	   int pipe_id=0;
	   ret=ar_hal_vin_bind_get_pipe_id_by_dev_id(p_raw_process_obj->fd_vin_dev,dev_id,&pipe_id);
	   ar_always("dev id =%d pipe_id=%d",dev_id,pipe_id);
	   if(ret)
	   {
		  ar_always("the device have started ret=%d opend=%d",ret,opend);
		  int fd_pipe=ar_hal_vin_open_pipe_dev(pipe_id);
		  p_raw_process_obj->fd_pipe_raw=fd_pipe;
		  if(fd_pipe>=0)
		  {
                ar_always("get raw stream and send to vin qbuffer thread");

				int num=0;
				int stream_id_all[8];
				ar_hal_vin_get_all_stream(fd_pipe,pipe_id,stream_id_all,&num);

                int stream_id=0;
				p_raw_process_obj->stream_id=stream_id;

				STRU_AR_STREAM_PROPERTY_T prop;
				ar_hal_vin_get_stream_property(fd_pipe,pipe_id,stream_id,&prop);

				if(prop.stream_type!=STREAM_TYPE_RAW_BAYER)
				{
				   ar_err("raw stream id err, please start raw dev only raw stream");
				   ar_hal_vin_close_pipe_dev(fd_pipe);
				   return -1;
				}
				//open the stream fd
				int fd_stream=ar_hal_vin_open_stream_dev(pipe_id,stream_id);
				p_raw_process_obj->fd_stream=fd_stream;

				int client_id=0;
				ar_hal_vin_create_stream_client(fd_pipe,pipe_id,stream_id,&client_id);
				ar_always("stream_id %d client id=%d fd_stream=%d pipe_id=%d",stream_id,client_id,fd_stream,pipe_id);
				while(1)
				{
					 STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
					 ar_hal_vin_get_stream_buffer(fd_stream,pipe_id,stream_id,client_id,p_stream_buffer,-1);
                     if(p_stream_buffer->panel_count==1)
					 {
					     //send the stream buffer to vin dq queue
					     p_raw_process_obj->raw_queue->queue_insert(p_raw_process_obj->raw_queue,p_stream_buffer);
						 ar_signal(p_raw_process_obj->raw_queue_singnal);

					 }else
					 {
						 ar_always("not supported dump pannels =%d",p_stream_buffer->panel_count);
					 }
					 //ar_hal_vin_return_stream_buffer(fd_stream,pipe_id,stream_id,p_stream_buffer);
				}
				ar_hal_vin_delete_stream_client(fd_pipe,pipe_id,stream_id,client_id);
				ar_hal_vin_close_stream_dev(fd_stream);
				ar_hal_vin_close_pipe_dev(fd_pipe);

		  }else
		  {
		     ar_err("err open pipe dev for pip id=%d",pipe_id);
		  }
	   }else
	   {
            ar_err("can no get pipe id by dev id");
	   }
	}else
	{
	    ar_err("the dev not opend, can not do this test, so open the dev, to do later");
	}

    return 0;
}

static void *dq_loop_thread(void* arg)
{
    STRU_RAW_PROCCESS_T *p_raw_obj=(STRU_RAW_PROCCESS_T *)arg;
	while(1)
	{
        STRU_STREAM_BUFFER_INIFO_T *p_info=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
		ar_hal_vin_dq_stream_buffer(p_raw_obj->fd_pipe,p_raw_obj->pipe_id,0,p_info, -1);
		p_raw_obj->stream_buffer_pool->queue_insert(p_raw_obj->stream_buffer_pool,p_info);
		if(p_info->data[0]){
			STRU_STREAM_BUFFER_INIFO_T * p_stream_buffer=p_info->data[0];
		    ar_hal_vin_return_stream_buffer(p_raw_obj->fd_stream,p_raw_obj->pipe_id_raw,p_raw_obj->stream_id,p_stream_buffer);
			free(p_stream_buffer);
		}
	}
	ar_osal_thread_exit();
	return 0;
}

static void *q_loop_thread(void* arg)
{
    STRU_RAW_PROCCESS_T *p_raw_obj=(STRU_RAW_PROCCESS_T *)arg;
	STRU_STREAM_BUFFER_INIFO_T *p_info=NULL;
	while(1)
	{
	    p_raw_obj->raw_queue->queue_pop(p_raw_obj->raw_queue,(void**)&p_info);
		if(p_info==NULL)
		{
		   ar_signal_wait(p_raw_obj->raw_queue_singnal);
		}else
		{
		    STRU_STREAM_BUFFER_INIFO_T *p_info_q=NULL;
			p_raw_obj->stream_buffer_pool->queue_pop(p_raw_obj->stream_buffer_pool,(void**)&p_info_q);
			if(p_info_q)
			{
                  //insert to the stream queue
                  p_info_q->panel_count=1;
				  p_info_q->pannel[0].used=p_info->pannel[0].used;
				  p_info_q->pannel[0].length=p_info->pannel[0].length;
				  p_info_q->pannel[0].offset=0;
				  p_info_q->pannel[0].line_buffer_len=p_info->pannel[0].line_buffer_len;
				  p_info_q->pannel[0].buffer_phy_addr=p_info->pannel[0].buffer_phy_addr;
				  p_info_q->pannel[0].buffer_phy_orign=p_info->pannel[0].buffer_phy_orign;
				  p_info_q->pannel[0].buffer=p_info->pannel[0].buffer;
				  p_info_q->pannel[0].buffer_phy_orign=p_info->pannel[0].buffer_orign;
				  p_info_q->data[0]=p_info;
                  ar_hal_vin_q_stream_buffer(p_raw_obj->fd_pipe,p_raw_obj->pipe_id,0,p_info_q);
                  free(p_info_q);

			}else
			{
			  ar_err("no buffer");
			}
		}
	}
	ar_osal_thread_exit();
	return 0;
}


static int api_test_raw_process(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
				      STRU_RAW_PROCCESS_T raw_process_obj;
                      ar_hal_vin_create_event_client(fd_pipe,pipe_id,&raw_process_obj.event_client_id);
					  raw_process_obj.fd_pipe=fd_pipe;
					  raw_process_obj.fd_vin_dev=vin_fd;
					  raw_process_obj.pipe_id=pipe_id;
					  ar_always("event_client_id=%d",raw_process_obj.event_client_id);
					  ar_os_thread_attr_t ctrl_attr = {0};
					  ctrl_attr.name = "stream";
					  ctrl_attr.stack_size = 16 * 1024;
					  raw_process_obj.event_thread=ar_osal_thread_new(event_loop_thread, &raw_process_obj, &ctrl_attr);
					  //after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
					  for(int i=NOTIFY_EVENT_ERR;i<NOTIFY_EVENT_MAX;i++)
					  {
					      ar_hal_vin_register_event(fd_pipe,pipe_id,raw_process_obj.event_client_id,i);
					  }
                      //creat a thread to qbuffer
					  raw_process_obj.stream_buffer_pool=ar_creat_queue(16,"stream_buffer_pool");
					  int size=0;
					  ar_hal_vin_requst_stream_buffer(fd_pipe,pipe_id,0,raw_process_obj.stream_buffer_info,&size);

					  for(int i=0;i<size;i++)
					  {
						  STRU_STREAM_BUFFER_INIFO_T *p_info=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
						  *p_info=raw_process_obj.stream_buffer_info[i];
						  raw_process_obj.stream_buffer_pool->queue_insert(raw_process_obj.stream_buffer_pool,p_info);
					  }


					  raw_process_obj.raw_queue=ar_creat_queue(16,"raw_queue");
					  raw_process_obj.raw_queue_singnal=ar_create_signal();

					  ctrl_attr.name = "qbuffer";
					  ctrl_attr.stack_size = 16 * 1024;
					  raw_process_obj.qbuffer=ar_osal_thread_new(q_loop_thread, &raw_process_obj, &ctrl_attr);
                      //creat a thread to qbuffer

					  ctrl_attr.name = "dqbuffer";
					  ctrl_attr.stack_size = 16 * 1024;
					  raw_process_obj.dqbuffer=ar_osal_thread_new(dq_loop_thread, &raw_process_obj, &ctrl_attr);

                      //init raw dev to get raw
					  raw_dev_init(p_camera,p_cmd,&raw_process_obj);
                      //unregister all the event
                      ar_always("unregister all the event");
					  for(int i=NOTIFY_EVENT_ERR;i<NOTIFY_EVENT_MAX;i++)
					  {
					      ar_hal_vin_unregister_event(fd_pipe,pipe_id,raw_process_obj.event_client_id,i);
					  }
					  ar_osal_thread_join(raw_process_obj.event_thread);
					  ar_hal_vin_delete_event_client(fd_pipe,pipe_id,raw_process_obj.event_client_id);

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

__test_camera_case__(raw_process)=
{
   "--raw_process",
   api_test_raw_process,
   {
      "test raw process, please start_camera first, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id] :isp in devid,  send raw to it and get aec update event",
	  "-raw_id [id] raw dev id, we will get raw and send aec update ctl to it",
	  NULL,
   },
};
