#include"camera.h"
#include <sys/stat.h>
#include "sensor.h"


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


#define USE_SIMPLE_ELEMET
#define EXIT_WHEN_GET_STREAM_BUFFER_ERR
//#define USE_LOG_BUFFER_LOOP
#ifdef USE_LOG_BUFFER_LOOP
#define ar_loop ar_always
#else
#define ar_loop ar_debug
#endif
static int stop_camera(STRU_CAMERA_T *p_camera);

static int mem_free(SRTU_PAD_FRAME_T *p_frame)
{
	static int count=0;
	STRU_STREAM_T *p_stream=(STRU_STREAM_T *)p_frame->priv[1];
	STRU_CAMERA_T *p_camera=p_stream->p_camera;
	STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=p_frame->priv[0];
	if(count>=1000)
	{
	   ar_loop("get stream buffer %d stream_id=%d index=%d",p_stream_buffer->frame_id,p_stream->stream_id,p_stream_buffer->buffer_index);
	   count=0;
	}
	count++;

	ar_hal_vin_return_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream_buffer);
	free(p_stream_buffer);
	return 0;
}


static void *stream_loop_thread(void* arg)
{
	STRU_STREAM_T *p_stream=(STRU_STREAM_T *)arg;
	STRU_CAMERA_T *p_camera=p_stream->p_camera;
	int client_id;
	ar_hal_vin_create_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_stream->stream_id,&client_id);
	p_stream->client_id=client_id;
	int count=0;
	int ret=0;
	int is_first=1;
	while(1)
	{
	    STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
        ret=ar_hal_vin_get_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,client_id,\
			p_stream_buffer,-1);

        #ifdef EXIT_WHEN_GET_STREAM_BUFFER_ERR
		if(ret<0){
     		ar_err("err ar_hal_vin_get_stream_buffer,exit the loop thread ");
			break;
		}
		#endif

		if(count>=1000)
		{
		   ar_loop("get client_id=%d stream buffer %d stream_id=%d index=%d",client_id,p_stream_buffer->frame_id,p_stream->stream_id,p_stream_buffer->buffer_index);
		   count=0;
		}
		count++;


        if(p_stream_buffer->buffer_index==CAM_ERR_EXIT_MSG)
        {
            ar_always("receive exit msg , exit the stream loop thread");
            break;
        }

        if(p_stream_buffer->frame_id%2)
        {
			if(ret>=0){
				//ar_always("odd frame skip");
				ar_hal_vin_return_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream_buffer);
				free(p_stream_buffer);
			}
        }else
        {
      		if(ret>=0)
      		{
           		SRTU_PAD_FRAME_T *p_frame=malloc(sizeof(SRTU_PAD_FRAME_T));
           		p_frame->pannel_cout=p_stream_buffer->panel_count;
                  p_frame->w = p_camera->stream[0].stream_prop.total_w;
                  p_frame->h = p_camera->stream[0].stream_prop.total_h;
           		for(int i=0;i<p_frame->pannel_cout;i++){
           		    p_frame->pannel[i].len=p_stream_buffer->pannel[i].length;
           		    p_frame->pannel[i].used=p_stream_buffer->pannel[i].used;
           		    p_frame->pannel[i].stride=p_stream_buffer->pannel[i].line_buffer_len;
           		    p_frame->pannel[i].pa=p_stream_buffer->pannel[i].buffer_phy_orign;
           		    p_frame->pannel[i].pa_orig=p_stream_buffer->pannel[i].buffer_phy_orign;
           		    p_frame->pannel[i].va=p_stream_buffer->pannel[i].buffer;
           		    p_frame->pannel[i].va_orig=p_stream_buffer->pannel[i].buffer_orign;
           		}
           		p_frame->priv[0]=p_stream_buffer;
           		p_frame->priv[1]=p_stream;
           		p_frame->mem_free=mem_free;
      			if(is_first)
      			{
      			   ar_always("firt push to next");
				   ar_always("%p %p %p",p_frame->pannel[0].pa,p_frame->pannel[1].pa,p_frame->pannel[2].pa);
      			   is_first=0;
      			}
				//ar_delay(100);
           		push_to_next(p_camera->p_element,p_stream->p_pad,p_frame);
      		}
        }
	}
	//will be called by stop stream first
	//ar_hal_vin_delete_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_stream->stream_id,client_id);
	ar_osal_thread_exit();
	return NULL;
}

static SRTU_PAD_FRAME_T *buffer_sink_buffer_chain(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad,SRTU_PAD_FRAME_T *p_frame)
{
   static int count=0;
   // STRU_STREAM_T *p_stream=(STRU_STREAM_T *)p_frame->priv[1];
   // STRU_CAMERA_T *p_camera=p_stream->p_camera;
   // STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=p_frame->priv[0];
   if(count>=1000)
   {
      ar_loop("get stream buffer %d stream_id=%d index=%d",p_stream_buffer->frame_id,p_stream->stream_id,p_stream_buffer->buffer_index);
      count=0;
   }
   count++;
   return p_frame;
}


static SRTU_PAD_FRAME_T * send_to_fw_display(STRU_SURFACE_OBJ_T *p_display,SRTU_PAD_FRAME_T *p_frame)
{
    STRU_AR_HAL_VO_DISP_BUF    buffer;
    int                        ret;
    static uint32_t            frame_id = 0;

    buffer.frame_id =frame_id;
    buffer.format = AR_HAL_VO_FMT_YV12;
    buffer.frame_width = p_frame->w;
    buffer.frame_height = p_frame->h;
    buffer.roi.w = p_frame->w;
    buffer.roi.h = p_frame->h;
    buffer.roi.x = 0;
    buffer.roi.y = 0;
    buffer.luma_stride = p_frame->pannel[0].stride;
    buffer.chroma_stride = p_frame->pannel[1].stride;
    buffer.pannel_num = p_frame->pannel_cout;
    for (int i=0; i<p_frame->pannel_cout; i++)
    {
        buffer.pannel[i].buffer = p_frame->pannel[i].va;
        buffer.pannel[i].buffer_orign = p_frame->pannel[i].va_orig;
        buffer.pannel[i].buffer_pa = p_frame->pannel[i].pa;
        buffer.pannel[i].buffer_pa_orign = p_frame->pannel[i].pa_orig;
        buffer.pannel[i].length = p_frame->pannel[i].len;
    }
    buffer.interlace_filed_flag = 0;
    buffer.usr_data = (void *)p_frame;
	ar_debug("send buffer_%d", frame_id);

    frame_id++;

    int y_len=buffer.luma_stride*p_frame->h;
    p_display->p_dump->dump(p_display->p_dump,\
		buffer.pannel[0].buffer_pa,\
		buffer.pannel[1].buffer_pa,\
		buffer.pannel[2].buffer_pa,\
		NULL,y_len,y_len/4,y_len/4,0);

    ret = ar_hal_vo_chn_send_frame(p_display->chan_fd, &buffer, 0);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    /* */
	ar_debug("get buffer");
    memset(&buffer, 0, sizeof(STRU_AR_HAL_VO_DISP_BUF));
    ret = ar_hal_vo_chn_get_frame(p_display->chan_fd, &buffer, -1);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("can't get buffer");
        goto End;
    }

    ar_debug("release buffer_%d", buffer.frame_id);
    ret = ar_hal_vo_chn_release_frame(p_display->chan_fd, &buffer);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("can't get buffer");
        goto End;
    }
    ar_debug("release finish_%d", buffer.frame_id);

   End:
	   if (ret != AR_HAL_VO_SUCCESS)
		   ar_err("ret = 0x%x", ret);

    return buffer.usr_data;
}

static SRTU_PAD_FRAME_T * surface_sink_buffer_chain(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad,SRTU_PAD_FRAME_T *p_frame)
{
   static int count=0;
   static int first=1;
   // STRU_STREAM_T *p_stream=(STRU_STREAM_T *)p_frame->priv[1];
   // STRU_CAMERA_T *p_camera=p_stream->p_camera;
   // STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=p_frame->priv[0];
   STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_element->obj;
   if(count>=1000)
   {
      ar_loop("get stream buffer %d stream_id=%d index=%d",p_stream_buffer->frame_id,p_stream->stream_id,p_stream_buffer->buffer_index);
      count=0;
   }
   count++;
   if(first==1)
   {
      ar_always("first send to display");
      first=0;
   }
   return send_to_fw_display(p_display,p_frame);
}

static int camera_surface_init(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd,int surface_num)
{
    int index=0;
    STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[surface_num]->obj;
    int x=0;
    int y=0;
    int w=p_camera->stream[surface_num].stream_prop.total_w;
    int h=p_camera->stream[surface_num].stream_prop.total_h;
    STRU_AR_HAL_VO_RECT rect;
    int ret = AR_HAL_VO_SUCCESS;

	p_display->dev_fd=p_camera->display_obj.dev_fd;
	p_display->e_dev_id=p_camera->display_obj.e_dev_id;
	p_display->e_layer_id=p_camera->display_obj.e_layer_id;
	p_display->layer_fd=p_camera->display_obj.layer_fd;
	p_display->p_dump=p_camera->p_dump;


	if(surface_num==0)
	{
       index=camera_find_cmd_pra_index_by_name("-surface1",p_cmd);
	}else
	{
	   index=camera_find_cmd_pra_index_by_name("-surface2",p_cmd);
	}
    if(index>0)
    {
        x=atoi(p_cmd->args[index+1]);
        y=atoi(p_cmd->args[index+2]);
        w=atoi(p_cmd->args[index+3]);
        h=atoi(p_cmd->args[index+4]);
    }
    ar_always("(x,y,w,h)==>(%d %d %d %d)",x,y,w,h);
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    p_display->chan_id = p_camera->dev_id*2+surface_num;
    ret = ar_vo_surface_init(p_display,&rect);
    return ret;
}

static int camera_pipeline_init(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    int index=0;
    //creat element for buffer loop
    p_camera->p_element=creat_element("cam_src",p_camera);
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
	   p_camera->stream[i].p_pad=creat_pad("cam_src_pad",&p_camera->stream[i]);
	   add_out_pad(p_camera->p_element,p_camera->stream[i].p_pad);
	   p_camera->stream[i].p_pad->buffer_chain=NULL;
    }
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       STRU_SURFACE_OBJ_T *p_display=malloc(sizeof(STRU_SURFACE_OBJ_T));
	   memset(p_display,0,sizeof(STRU_SURFACE_OBJ_T));
	   p_display->chan_fd=-1;
       p_camera->p_surface_element[i]=creat_element("surface_element",p_display);
	   p_display->p_element=p_camera->p_surface_element[i];
       p_display->pad=creat_pad("surface_pad_0",NULL);
       add_in_pad(p_display->p_element,p_display->pad);
       p_display->pad->buffer_chain=surface_sink_buffer_chain;

       //buffer element
       STRU_BUFFER_OBJ_T *p_buffer_obj=malloc(sizeof(STRU_BUFFER_OBJ_T));
       p_camera->p_buffer_element[i]=creat_element("buffer_element",p_buffer_obj);
       p_buffer_obj->p_element=p_camera->p_buffer_element[i];
       p_buffer_obj->pad=creat_pad("buffer_pad_0",NULL);
       add_in_pad(p_buffer_obj->p_element,p_buffer_obj->pad);
       p_buffer_obj->pad->buffer_chain=buffer_sink_buffer_chain;
    }


    //start link pad ,preview
    if(p_camera->stream[0].enable)
    {
       STRU_ELMENT_T * p_dest_element=NULL;
       index=camera_find_cmd_pra_index_by_name("-scaler1_to",p_cmd);
       if(index>=0)
       {
    	   int num=0;
    	   if(!strcmp("surface",p_cmd->args[index+1]))
    	   {
    		   p_dest_element=p_camera->p_surface_element[num];
    		   ar_always("scaler1_to link to p_surface_element %d",num);
			   camera_surface_init(p_camera,p_cmd,num);

    	   }else
    	   {
    		  p_dest_element=p_camera->p_buffer_element[num];
    		   ar_always("scaler1_to link to p_buffer_element %d",num);
    	   }
       }else
       {
    	   ar_always("scaler1_to link to p_buffer_element 0");
    	   p_dest_element=p_camera->p_buffer_element[0];
       }
       link_pad(p_camera->p_element,0,p_dest_element,0);
    }

    if(p_camera->stream[1].enable)
    {
       STRU_ELMENT_T * p_dest_element=NULL;
       index=camera_find_cmd_pra_index_by_name("-scaler2_to",p_cmd);
       if(index>=0)
       {
           int num=1;
    	   if(!strcmp("surface",p_cmd->args[index+1]))
    	   {
    		   p_dest_element=p_camera->p_surface_element[num];
    		   ar_always("scaler2_to link to p_surface_element %d",num);
			   camera_surface_init(p_camera,p_cmd,num);

    	   }else
    	   {
    		  p_dest_element=p_camera->p_buffer_element[num];
    		   ar_always("scaler2_to link to p_buffer_element %d",num);
    	   }
       }else
       {
    	   ar_always("scaler2_to link to p_buffer_element 1");
    	   p_dest_element=p_camera->p_buffer_element[1];
       }
       link_pad(p_camera->p_element,1,p_dest_element,0);
    }

    if(p_camera->stream[2].enable)
    {
       STRU_ELMENT_T * p_dest_element=NULL;
       ar_always("scaler2_to link to p_buffer_element 2");
       p_dest_element=p_camera->p_buffer_element[2];
       link_pad(p_camera->p_element,2,p_dest_element,0);
    }
	return 0;
}


static int camera_pipeline_deinit(STRU_CAMERA_T *p_camera)
{
    //stop display first
    //disable all the surface
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
		STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[i]->obj;
		if(p_display->chan_fd>=0)
		{
		    ar_hal_vo_chn_disable(p_display->chan_fd);
			ar_hal_vo_fd_close(p_display->chan_fd);
		}
    }
	//deinit element
	//free all the elment and pad

    // int index=0;
    //creat element for buffer loop
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
	   delete_pad(p_camera->stream[i].p_pad);
	   p_camera->stream[i].p_pad=NULL;
    }
	delete_element(p_camera->p_element);
	p_camera->p_element=NULL;

    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       ar_always("i=%d %p %p",i,p_camera->p_surface_element[i],p_camera->p_buffer_element[i]);
	   STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[i]->obj;
	   delete_pad(p_display->pad);
	   delete_element(p_camera->p_surface_element[i]);
	   p_camera->p_surface_element[i]=NULL;
	   free(p_display);

	   STRU_BUFFER_OBJ_T *p_buffer_obj=(STRU_BUFFER_OBJ_T *)p_camera->p_buffer_element[i]->obj;
	   delete_pad(p_buffer_obj->pad);
	   delete_element(p_camera->p_buffer_element[i]);
	   p_camera->p_buffer_element[i]=NULL;
       free(p_buffer_obj);
    }
    return 0;
}

static int camera_test_start_stream(STRU_CAMERA_T *p_camera,int index)
{
       //start to creat stream
    int i=index;
    //for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       if(p_camera->stream[i].enable)
	   {
	       p_camera->stream[i].stream_id=-1;
           ar_hal_vin_creat_stream(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->stream[i].stream_prop,&p_camera->stream[i].stream_id);
		   p_camera->stream[i].fd_stream=ar_hal_vin_open_stream_dev(p_camera->pipe_id,p_camera->stream[i].stream_id);
		   ar_always("p_camera->stream[%d]=%d fd_stream=%d",i,p_camera->stream[i].stream_id,p_camera->stream[i].fd_stream);
		   //prepare stream buffer for every stream
		   for(int j=0;j<MAX_STREAM_BUFER_COUNT;j++)
		   {
                ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);

				for(int k=0;k<p_camera->stream[i].stream_buffer[j].panel_count;k++)
				{
				   //malloc later to instead with vb policy
				   AR_VOID *p_vaddr=NULL;
				   AR_U64 phy_addr=0;
				   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,p_camera->stream[i].stream_buffer[j].pannel[k].length);

                   if(p_vaddr==NULL || phy_addr==0)
                   {
                     ar_err("ar_hal_sys_mmz_alloc_cached err");
                     return -1;
                   }
                   memset(p_vaddr,0,p_camera->stream[i].stream_buffer[j].pannel[k].length);

				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[k].buffer=p_vaddr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign,256);
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign=(void *)phy_addr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_addr=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,256);
				   ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign,p_camera->stream[i].stream_buffer[j].pannel[k].buffer,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_addr,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].length,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].used);
				}
				ar_hal_vin_return_stream_buffer(p_camera->stream[i].fd_stream,p_camera->pipe_id,\
					p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
		   }
       }
    }
	i=index;
    //for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       if(p_camera->stream[i].enable)
       {
         ar_hal_vin_start_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);

		 //create thread for stream buffer process
		 ar_os_thread_attr_t ctrl_attr = {0};
		 ctrl_attr.name = "stream";
		 ctrl_attr.stack_size = 16 * 1024;
		 p_camera->stream[i].stream_thread=ar_osal_thread_new(stream_loop_thread, &p_camera->stream[i], &ctrl_attr);
       }
    }
	return 0;
}


static int camera_test_run_start_camera_stream(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    //set stream type by  device_type
    int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
	if(index>=0)
	{
		strcpy(p_cmd->args[index],"-local");
		ar_always("start camera with remote");
		camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
		return 0;
	}
	int preview_stream_type=STREAM_TYPE_PREVIEW;
	int video_stream_type=STREAM_TYPE_VIDEO;
	ar_hal_vin_get_dev_infor(p_camera->fd_vin_dev,p_camera->dev_id,&p_camera->dev_info);
    if(p_camera->dev_info.device_type==DEVICE_TYPE_ISPIN)
    {
		preview_stream_type=STREAM_TYPE_SIM_PREVIEW;
		video_stream_type=STREAM_TYPE_SIM_VIDEO;
    }
	index=camera_find_cmd_pra_index_by_name("-scaler1",p_cmd);
	if(index>=0 && p_camera->stream[0].enable==0)
	{
	   p_camera->stream[0].enable=1;
	   p_camera->stream[0].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[0].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[0].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[0].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[0].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[0].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[0].stream_prop.stride=atoi(p_cmd->args[index+7]);
	   p_camera->stream[0].stream_prop.fps=25;
	   p_camera->stream[0].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	   p_camera->stream[0].stream_prop.stream_type=preview_stream_type;
	   p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;

	   //link the stream
	   STRU_ELMENT_T * p_dest_element=NULL;
       index=camera_find_cmd_pra_index_by_name("-scaler1_to",p_cmd);
       if(index>=0)
       {
    	   int num=0;

    	   if(!strcmp("surface",p_cmd->args[index+1]))
    	   {
    		   p_dest_element=p_camera->p_surface_element[num];
    		   ar_always("scaler1_to link to p_surface_element %d",num);
			   camera_surface_init(p_camera,p_cmd,num);
    	   }else
    	   {
    		  p_dest_element=p_camera->p_buffer_element[num];
    		   ar_always("scaler1_to link to p_buffer_element %d",num);
    	   }
       }else
       {
    	   ar_always("scaler1_to link to p_buffer_element 0");
    	   p_dest_element=p_camera->p_buffer_element[0];
       }
       link_pad(p_camera->p_element,0,p_dest_element,0);
	   camera_test_start_stream(p_camera,0);
	}
	index=camera_find_cmd_pra_index_by_name("-scaler2",p_cmd);
	if(index>=0 && p_camera->stream[1].enable==0)
	{
	   p_camera->use_scaler2=1;
	   p_camera->stream[1].enable=1;
	   p_camera->stream[1].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[1].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[1].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[1].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[1].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[1].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[1].stream_prop.stride=atoi(p_cmd->args[index+7]);
	   p_camera->stream[1].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	   p_camera->stream[1].stream_prop.stream_type=video_stream_type;
	   p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;

       STRU_ELMENT_T * p_dest_element=NULL;
       index=camera_find_cmd_pra_index_by_name("-scaler2_to",p_cmd);
       if(index>=0)
       {
    	   int num=1;

    	   if(!strcmp("surface",p_cmd->args[index+1]))
    	   {
    		   p_dest_element=p_camera->p_surface_element[num];
    		   ar_always("scaler2_to link to p_surface_element %d",num);
			   camera_surface_init(p_camera,p_cmd,num);
    	   }else
    	   {
    		  p_dest_element=p_camera->p_buffer_element[num];
    		   ar_always("scaler2_to link to p_buffer_element %d",num);
    	   }
       }else
       {
    	   ar_always("scaler2_to link to p_buffer_element 1");
    	   p_dest_element=p_camera->p_buffer_element[1];
       }
       link_pad(p_camera->p_element,1,p_dest_element,0);
	   camera_test_start_stream(p_camera,1);
	}

	index=camera_find_cmd_pra_index_by_name("-raw",p_cmd);
	if(index>=0 && p_camera->stream[2].enable==0)
	{
	   p_camera->use_raw=1;
	   p_camera->stream[2].enable=1;
	   p_camera->stream[2].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[2].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[2].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[2].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[2].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[2].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[2].stream_prop.stride=atoi(p_cmd->args[index+7]);
	   p_camera->stream[2].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	   p_camera->stream[2].stream_prop.stream_type=STREAM_TYPE_RAW_BAYER;
	   int bayer_fomat=0;
	   ar_hal_vin_get_raw_format(&p_camera->sensor_var_pra.out_infor,&bayer_fomat);
	   ar_hal_vin_bayer_format_to_dng_format(bayer_fomat,(AR_S32 *)&p_camera->stream[2].stream_prop.format);

       STRU_ELMENT_T * p_dest_element=NULL;
       ar_always("scaler2_to link to p_buffer_element 2");
       p_dest_element=p_camera->p_buffer_element[2];
       link_pad(p_camera->p_element,2,p_dest_element,0);
	   camera_test_start_stream(p_camera,2);
	}
	return 0;
}

static void *event_loop_thread(void* arg)
{
   STRU_CAMERA_T *p_event_pbj=(STRU_CAMERA_T *)arg;
   STRU_AR_EVENT_INFO_T * p_event_info=malloc(sizeof(STRU_AR_EVENT_INFO_T));
   STRU_CAM_SERVER_START_OPT_T *p_opt=malloc(sizeof(STRU_CAM_SERVER_START_OPT_T));
   ar_hal_vin_driver_get_start_opt(p_event_pbj->fd_vin_dev,p_opt);
   int event_data_size=p_opt->event_data_max_size;
   int fd_mem=ar_hal_vin_open_dev_mem();
   int loop=p_event_pbj->frames;
   int enable_exit=loop;
   while(1)
   {
        ar_hal_vin_get_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
        if(p_event_info->event_code==-1)
        {
           ar_hal_vin_release_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		   ar_always("exit event arrvied");
		   break;
        }
        //map the event data
        AR_S32 offset_ret=0;
        void *event_data_maped=ar_hal_vin_map_phy_addr(fd_mem,p_event_info->event_phy,event_data_size,&offset_ret);
        // void *event_data=event_data_maped+offset_ret;
		switch(p_event_info->event_code)
		{
            case NOTIFY_EVENT_SOF:
			{
                // STRU_NOTIFY_EVENT_SOF_PRA_T *p_sof=(STRU_NOTIFY_EVENT_SOF_PRA_T *)event_data;
                //ar_always("NOTIFY_EVENT_SOF,sof_count=%d",p_sof->sof_count);
                if(enable_exit)
                {
                   if(loop>0)
                   {
                     loop--;
					 if(loop<=0)
					 {
					    ar_always("exit camera");
						ar_always("unregister all the event");
						for(int i=NOTIFY_EVENT_SOF;i<=NOTIFY_EVENT_SOF;i++)
						{
							ar_hal_vin_unregister_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,i);
						}
						ar_hal_vin_delete_event_client(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id);
					 }
                   }
                }
			}
			break;
		    default:
		    break;
		}
		ar_hal_vin_release_event(p_event_pbj->fd_pipe,p_event_pbj->pipe_id,p_event_pbj->event_client_id,p_event_info);
		ar_hal_vin_ummap_vaddr(event_data_maped,event_data_size);
   }
   free(p_event_info);
   free(p_opt);
   ar_hal_vin_close_dev_mem(fd_mem);
   ar_signal(p_event_pbj->exit_signal);
   ar_osal_thread_exit();
   return NULL;
}


static int camera_test_run_start_camera_main(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    int ret=0;
	char *name=NULL;
	int need_server=0;

	int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
	if(index>=0)
	{
		strcpy(p_cmd->args[index],"-local");
		ar_always("start camera with remote");
		camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
		return 0;
	}

	index=camera_find_cmd_pra_index_by_name("-dump",p_cmd);
	if(index>0)
	{
		p_camera->p_dump->dump_req(p_camera->p_dump,"/big_picture.yuv",1);
		return 0;
	}


	index=camera_find_cmd_pra_index_by_name("-server",p_cmd);
	if(index>=0)
	{
		need_server=1;
	}
	p_camera->need_server=need_server;

	name=p_cmd->args[index+1];

	p_camera->dev_id=0;
	index=camera_find_cmd_pra_index_by_name("-dev_id",p_cmd);
	if(index>=0)
	{
	   p_camera->dev_id=atoi(p_cmd->args[index+1]);
	}
    //open the vin dev
    p_camera->fd_vin_dev=ar_hal_vin_open_vin_dev();
	if(p_camera->fd_vin_dev<0)
	{
	   ar_err("open vin dev err");
	   return -1;
	}
	//set stream type by  device_type
	int preview_stream_type=STREAM_TYPE_PREVIEW;
	int video_stream_type=STREAM_TYPE_VIDEO;
	ar_hal_vin_get_dev_infor(p_camera->fd_vin_dev,p_camera->dev_id,&p_camera->dev_info);
    if(p_camera->dev_info.device_type==DEVICE_TYPE_ISPIN)
    {
		preview_stream_type=STREAM_TYPE_SIM_PREVIEW;
		video_stream_type=STREAM_TYPE_SIM_VIDEO;
    }

    //first to creat a msg queue
    if(need_server)
	{
        char server_name[64];
    	sprintf(server_name,"/%s_%d",name,p_camera->dev_id);
    	ar_always("server name is %s",server_name);
    	struct mq_attr attr={0};
    	attr.mq_maxmsg=10;
    	attr.mq_msgsize=sizeof(STRU_CAMERA_CMD_T);
    	p_camera->mqid=mq_open(server_name,O_CREAT | O_RDWR,0666,&attr);
    	if(p_camera->mqid==(mqd_t)-1)
    	{
    	   ar_err("start server failed,%d",p_camera->mqid);
    	   perror("open mq failed, the error code is :");
    	   return -1;
    	}
    	mq_getattr(p_camera->mqid,&attr);
    	ar_always("mq_maxmsg=%d mq_msgsize=%d sizeof(STRU_CAMERA_CMD_T)=%d",attr.mq_maxmsg,attr.mq_msgsize,sizeof(STRU_CAMERA_CMD_T));
    	//creat a loop to receive msg
    	ar_os_thread_attr_t ctrl_attr = {0};
    	ctrl_attr.name = "msg";
    	ctrl_attr.stack_size = 16 * 1024;
    	p_camera->msg_loop=ar_osal_thread_new(msg_loop_thread,p_camera, &ctrl_attr);
		p_camera->exit_signal=ar_create_signal();

		//init vo dev
		ar_vo_dev_init(&p_camera->display_obj,1920,1080,-1,-1,30, AR_FALSE, AR_FALSE);

		p_camera->p_dump=creat_dump_server();
		p_camera->display_obj.p_dump=p_camera->p_dump;
    }

	p_camera->stream[0].p_camera=p_camera;
	p_camera->stream[0].enable=0;
	p_camera->stream[0].stream_prop.width=1920;
	p_camera->stream[0].stream_prop.height=1080;
	p_camera->stream[0].stream_prop.x=0;
	p_camera->stream[0].stream_prop.y=0;
	p_camera->stream[0].stream_prop.total_w=1920;
	p_camera->stream[0].stream_prop.total_h=1080;
	p_camera->stream[0].stream_prop.stride=-1;
	p_camera->stream[0].stream_prop.skip=0;
	index=camera_find_cmd_pra_index_by_name("-scaler1",p_cmd);
	if(index>=0)
	{
	   p_camera->stream[0].enable=1;
	   p_camera->stream[0].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[0].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[0].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[0].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[0].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[0].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[0].stream_prop.stride=atoi(p_cmd->args[index+7]);
	}
	p_camera->stream[0].stream_prop.fps=25;
	index=camera_find_cmd_pra_index_by_name("-fps",p_cmd);
	if(index>=0)
	{
	    p_camera->stream[0].stream_prop.fps=atoi(p_cmd->args[index+1]);
	}

	p_camera->stream[0].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	p_camera->stream[0].stream_prop.stream_type=preview_stream_type;
	p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;


    //construct scaler2
    p_camera->use_scaler2=0;
	p_camera->stream[1].p_camera=p_camera;
	p_camera->stream[1].enable=0;
	p_camera->stream[1].stream_prop.width=640;
	p_camera->stream[1].stream_prop.height=480;
	p_camera->stream[1].stream_prop.x=0;
	p_camera->stream[1].stream_prop.y=0;
	p_camera->stream[1].stream_prop.total_w=640;
	p_camera->stream[1].stream_prop.total_h=480;
	p_camera->stream[1].stream_prop.stride=-1;
	p_camera->stream[1].stream_prop.skip=0;
	index=camera_find_cmd_pra_index_by_name("-scaler2",p_cmd);
	if(index>=0)
	{
	   p_camera->use_scaler2=1;
	   p_camera->stream[1].enable=1;
	   p_camera->stream[1].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[1].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[1].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[1].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[1].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[1].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[1].stream_prop.stride=atoi(p_cmd->args[index+7]);
	}
	p_camera->stream[1].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	p_camera->stream[1].stream_prop.stream_type=video_stream_type;
	p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
    //construct raw stream

    p_camera->use_raw=0;
	p_camera->stream[2].p_camera=p_camera;
	p_camera->stream[2].enable=0;
	p_camera->stream[2].stream_prop.width=1920;
	p_camera->stream[2].stream_prop.height=1080;
	p_camera->stream[2].stream_prop.x=0;
	p_camera->stream[2].stream_prop.y=0;
	p_camera->stream[2].stream_prop.total_w=1920;
	p_camera->stream[2].stream_prop.total_h=1080;
	p_camera->stream[2].stream_prop.stride=-1;
	p_camera->stream[2].stream_prop.skip=0;
	index=camera_find_cmd_pra_index_by_name("-raw",p_cmd);
	if(index>=0)
	{
	   p_camera->use_raw=1;
	   p_camera->stream[2].enable=1;
	   p_camera->stream[2].stream_prop.width=atoi(p_cmd->args[index+1]);
	   p_camera->stream[2].stream_prop.height=atoi(p_cmd->args[index+2]);
	   p_camera->stream[2].stream_prop.x=atoi(p_cmd->args[index+3]);
	   p_camera->stream[2].stream_prop.y=atoi(p_cmd->args[index+4]);
	   p_camera->stream[2].stream_prop.total_w=atoi(p_cmd->args[index+5]);
	   p_camera->stream[2].stream_prop.total_h=atoi(p_cmd->args[index+6]);
	   p_camera->stream[2].stream_prop.stride=atoi(p_cmd->args[index+7]);
	}
	p_camera->stream[2].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	p_camera->stream[2].stream_prop.stream_type=STREAM_TYPE_RAW_BAYER;
	p_camera->stream[2].stream_prop.format=STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;

    //add element init to it
	camera_pipeline_init(p_camera,p_cmd);

    // if the dev have been opend, exit
    int opend=0;
    ret=ar_hal_vin_dev_is_opend(p_camera->fd_vin_dev,p_camera->dev_id,&opend);
	if(ret<0 || opend)
	{
	   ar_err("the device have started ret=%d opend=%d",ret,opend);
	   ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
	   p_camera->fd_vin_dev=-1;
	   return -1;
	}
    //first to set public prop
	ar_hal_vin_get_public_property(p_camera->fd_vin_dev,&p_camera->public_prop);

	if(p_camera->dev_info.device_type==DEVICE_TYPE_ISPIN)
	{
		p_camera->public_prop.cam_mode=SING_CAMERA_OFFLINE;
		p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DDR;
	}else
	{
		p_camera->public_prop.cam_mode=SING_CAMERA_NORMAL;
		p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DIRECT;
	}

	index=camera_find_cmd_pra_index_by_name("-cam_mode",p_cmd);
	if(index>=0){
         if(!strcmp("online",p_cmd->args[index+1]))
         {
            if(p_camera->dev_info.device_type==DEVICE_TYPE_ISPIN)
			{
				p_camera->public_prop.cam_mode=SING_CAMERA_OFFLINE;
				p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DDR;
            }else
            {
    			p_camera->public_prop.cam_mode=SING_CAMERA_NORMAL;
    			p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DIRECT;
            }
         }else if(!strcmp("offline",p_cmd->args[index+1]))
         {
			p_camera->public_prop.cam_mode=SING_CAMERA_OFFLINE;
			p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DDR;
         }else if(!strcmp("multi",p_cmd->args[index+1]))
         {
			 p_camera->public_prop.cam_mode=MUILTI_CMAERA_MODE;
			 p_camera->public_prop.vif_isp_mode=VIF2ISP_TRANSFER_DDR;
         }
	}
    p_camera->public_prop.isp_used=1;
    ar_hal_vin_set_public_property(p_camera->fd_vin_dev,&p_camera->public_prop);

    //dump the info of the dev
	ar_hal_vin_get_dev_infor(p_camera->fd_vin_dev,p_camera->dev_id,&p_camera->dev_info);
	STRU_SENSOR_DEV_INFO_T *p_info=&p_camera->dev_info;
	dump_dev_info(p_info);

	//we set the pipe id with same dev id
	p_camera->pipe_id=p_camera->dev_id;
	//bind the dev id with pipe id
	ar_hal_vin_pipe_bind_to_dev(p_camera->fd_vin_dev,p_camera->pipe_id,p_camera->dev_id);

    //here we can open the pipe, set the pipeprop
	p_camera->pipe_prop.sensor_hdr=0;
	index=camera_find_cmd_pra_index_by_name("-hdr",p_cmd);
	if(index>=0)
	{
	   p_camera->pipe_prop.sensor_hdr=1;
	}
    index=camera_find_cmd_pra_index_by_name("-man_aec_update",p_cmd);
	if(index>=0)
	{
	   p_camera->pipe_prop.enable_man_aec_update=1;
	}
	index=camera_find_cmd_pra_index_by_name("-man_awb_update",p_cmd);
	if(index>=0)
	{
	   p_camera->pipe_prop.enable_man_awb_update=1;
	}
	index=camera_find_cmd_pra_index_by_name("-man_af_update",p_cmd);
	if(index>=0)
	{
	   p_camera->pipe_prop.enable_man_af_update=1;
	}

	p_camera->pipe_prop.get_buffer_mode=1;//we must set to 1, hal use api mode ,but not callback mode
	p_camera->pipe_prop.trigger_mode=0; //always trigger mode disable
	index=camera_find_cmd_pra_index_by_name("-trigger",p_cmd);
	if(index>=0)
	{
	   p_camera->pipe_prop.trigger_mode=1;
	}

	p_camera->pipe_prop.max_w[0]=1920;
	p_camera->pipe_prop.max_h[0]=1080;
	p_camera->pipe_prop.max_fps[0]=25;
	index=camera_find_cmd_pra_index_by_name("-max_wh",p_cmd);
	if(index>0)
	{
		p_camera->pipe_prop.max_w[0]=atoi(p_cmd->args[index+1]);
		p_camera->pipe_prop.max_h[0]=atoi(p_cmd->args[index+2]);
		p_camera->pipe_prop.max_fps[0]=atoi(p_cmd->args[index+3]);
	}
    ar_always("sensor sel: (w h fps)==>(%d %d %f)",p_camera->pipe_prop.max_w[0],p_camera->pipe_prop.max_h[0],p_camera->pipe_prop.max_fps[0]);

	ar_hal_vin_open_pipe(p_camera->fd_vin_dev,p_camera->pipe_id,&p_camera->pipe_prop);

    //the pipe have opened,we can open the pipe fd
	p_camera->fd_pipe=ar_hal_vin_open_pipe_dev(p_camera->pipe_id);
    if(p_camera->fd_pipe<0)
    {
		ar_err("open the pipe dev err");
		ar_hal_vin_close_pipe(p_camera->fd_vin_dev,p_camera->pipe_id);
		ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
		p_camera->fd_vin_dev=-1;
		return -1;
    }
    //get sensor var dev info
    p_camera->sensor_var_pra.res_index=-1;
    ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);
	int bayer_fomat=0;
	ar_hal_vin_get_raw_format(&p_camera->sensor_var_pra.out_infor,&bayer_fomat);
	ar_hal_vin_bayer_format_to_dng_format(bayer_fomat,(AR_S32 *)&p_camera->stream[2].stream_prop.format);

	p_camera->sensor_var_pra.res_index=0;
    ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);

    ar_always("imag_w imag_h x y w h",p_camera->sensor_var_pra.res_info.width,p_camera->sensor_var_pra.res_info.height,\
		p_camera->sensor_var_pra.res_info.x_start,p_camera->sensor_var_pra.res_info.y_start,\
		p_camera->sensor_var_pra.res_info.x_width,p_camera->sensor_var_pra.res_info.y_height);

	int offset=64;
	index=camera_find_cmd_pra_index_by_name("-offset",p_cmd);
	if(index>0)
	{
	   offset=atoi(p_cmd->args[index+1]);
	}

    p_camera->sensor_var_pra.res_info.width=1920/2+offset;
	p_camera->sensor_var_pra.res_info.height=1080;
    //set to bug picture mode w h
 	index=camera_find_cmd_pra_index_by_name("-big_raw_wh",p_cmd);
    if(index>0)
    {
		p_camera->sensor_var_pra.res_info.width=atoi(p_cmd->args[index+1]);
		p_camera->sensor_var_pra.res_info.height=atoi(p_cmd->args[index+2]);
		p_camera->sensor_var_pra.res_info.width=p_camera->sensor_var_pra.res_info.width/2+offset;
    }
 	index=camera_find_cmd_pra_index_by_name("-bit_depth",p_cmd);
    if(index>0)
    {
		p_camera->sensor_var_pra.out_infor.bit_width=atoi(p_cmd->args[index+1]);

    }
	/*
	  BAYER_RGGB,
       BAYER_BGGR,
       BAYER_GRBG,
       BAYER_GBRG,
     */
	index=camera_find_cmd_pra_index_by_name("-bayer",p_cmd);
    if(index>0)
    {
		p_camera->sensor_var_pra.out_infor.bayer=atoi(p_cmd->args[index+1]);
    }

    p_camera->sensor_var_pra.res_info.line_length=p_camera->sensor_var_pra.res_info.width+400;
	p_camera->sensor_var_pra.res_info.frame_length=p_camera->sensor_var_pra.res_info.height+400;


	p_camera->sensor_var_pra.res_info.force_dng=0;
	index=camera_find_cmd_pra_index_by_name("-force_dng",p_cmd);
    if(index>0)
    {
		p_camera->sensor_var_pra.res_info.force_dng=1;
    }

	p_camera->sensor_var_pra.res_info.x_start=0;
	p_camera->sensor_var_pra.res_info.y_start=0;
	p_camera->sensor_var_pra.res_info.x_width=p_camera->sensor_var_pra.res_info.width;
	p_camera->sensor_var_pra.res_info.y_height=p_camera->sensor_var_pra.res_info.height;

	int y_offset=64;
	index=camera_find_cmd_pra_index_by_name("-y_offset",p_cmd);
	if(index>0)
	{
	   y_offset=atoi(p_cmd->args[index+1]);
	}

    y_offset=p_camera->stream[0].stream_prop.width-2*y_offset;

    ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);

    ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);

    ar_always("imag_w imag_h x y w h",p_camera->sensor_var_pra.res_info.width,p_camera->sensor_var_pra.res_info.height,\
		p_camera->sensor_var_pra.res_info.x_start,p_camera->sensor_var_pra.res_info.y_start,\
		p_camera->sensor_var_pra.res_info.x_width,p_camera->sensor_var_pra.res_info.y_height);


    //here we register the 3a algo
 	index=camera_find_cmd_pra_index_by_name("-aec",p_cmd);
    if(index>0)
    {
        ar_hal_vin_register_aec_algo_lib(p_camera->fd_pipe,p_camera->pipe_id,p_cmd->args[index+1]);
    }
    index=camera_find_cmd_pra_index_by_name("-awb",p_cmd);
    if(index>0)
    {
        ar_hal_vin_register_awb_algo_lib(p_camera->fd_pipe,p_camera->pipe_id,p_cmd->args[index+1]);
    }
    index=camera_find_cmd_pra_index_by_name("-af",p_cmd);
    if(index>0)
    {
        ar_hal_vin_register_af_algo_lib(p_camera->fd_pipe,p_camera->pipe_id,p_cmd->args[index+1]);
    }

    index=camera_find_cmd_pra_index_by_name("-sensor",p_cmd);
	if(index>=0)
	{
	    ar_always("start the sensor %s",p_cmd->args[index+1]);
	    STRU_SENSOR_OBJ_T *p_obj=get_sensor_obj(p_cmd->args[index+1]);
		p_obj->init(p_obj,p_camera->fd_vin_dev,p_camera->fd_pipe,p_camera->pipe_id,0);
	}


    //start to creat stream
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       if(p_camera->stream[i].enable)
	   {
	       p_camera->stream[i].stream_id=-1;
           ar_hal_vin_creat_stream(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->stream[i].stream_prop,&p_camera->stream[i].stream_id);
		   p_camera->stream[i].fd_stream=ar_hal_vin_open_stream_dev(p_camera->pipe_id,p_camera->stream[i].stream_id);
		   ar_always("p_camera->stream[%d]=%d fd_stream=%d",i,p_camera->stream[i].stream_id,p_camera->stream[i].fd_stream);
		   //prepare stream buffer for every stream
		   for(int j=0;j<MAX_STREAM_BUFER_COUNT;j++)
		   {
                ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
                ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer_right[j]);

				for(int k=0;k<p_camera->stream[i].stream_buffer[j].panel_count;k++)
				{
				   //malloc later to instead with vb policy
				   AR_VOID *p_vaddr=NULL;
				   AR_U64 phy_addr=0;
				   ar_hal_sys_mmz_alloc_cached(&phy_addr,&p_vaddr,NULL,NULL,p_camera->stream[i].stream_buffer[j].pannel[k].length);

                   if(p_vaddr==NULL || phy_addr==0)
                   {
                     ar_err("ar_hal_sys_mmz_alloc_cached err");
                     return -1;
                   }
                   memset(p_vaddr,0,p_camera->stream[i].stream_buffer[j].pannel[k].length);

				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[k].buffer=p_vaddr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign,256);
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign=(void *)phy_addr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_addr=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,256);
				   ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign,p_camera->stream[i].stream_buffer[j].pannel[k].buffer,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_addr,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].length,\
				   	p_camera->stream[i].stream_buffer[j].pannel[k].used);

    				p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_orign=p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer=p_vaddr;
    				p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer=(void *)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_orign,256);
					p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer+=y_offset;

					p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_orign=(void *)phy_addr;
    				p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_addr=(void *)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_orign,256);
                    p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_orign=p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_addr;
                    if(k==0)
					{
					    p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_addr+=y_offset;
                    }else
                    {
                       p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_addr+=y_offset/2;
                    }
					ar_always("right::::buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
    				 p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_orign,p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer,\
    				 p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_orign,p_camera->stream[i].stream_buffer_right[j].pannel[k].buffer_phy_addr,\
    				 p_camera->stream[i].stream_buffer_right[j].pannel[k].length,\
    				 p_camera->stream[i].stream_buffer_right[j].pannel[k].used);

				}
				ar_hal_vin_return_stream_buffer(p_camera->stream[i].fd_stream,p_camera->pipe_id,\
					p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
				ar_hal_vin_return_stream_buffer(p_camera->stream[i].fd_stream,p_camera->pipe_id,\
					p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer_right[j]);
		   }
       }
    }

    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       if(p_camera->stream[i].enable)
       {
         ar_hal_vin_start_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);

		 //create thread for stream buffer process
		 ar_os_thread_attr_t ctrl_attr = {0};
		 ctrl_attr.name = "stream";
		 ctrl_attr.stack_size = 16 * 1024;
		 p_camera->stream[i].stream_thread=ar_osal_thread_new(stream_loop_thread, &p_camera->stream[i], &ctrl_attr);
       }
    }

    //here we send a manula sof to update tuning pra
    STRU_AR_CAMERA_SOF_PRO_T sof_prop;
	sof_prop.sof_mode=1;
	sof_prop.to_trigger=1;
    ar_hal_vin_vsync_update(p_camera->fd_pipe,p_camera->pipe_id,&sof_prop);

    //then to trigger run
    STRU_AR_CAMERA_TRIGGER_PROP_T trigger_prop;
	trigger_prop.need_sync=0;
	trigger_prop.trigger_mode=1;
	trigger_prop.trigger_num=0xffffffff;

    if(p_camera->pipe_prop.trigger_mode){
       ar_hal_vin_trigger(p_camera->fd_pipe,p_camera->pipe_id,&trigger_prop);
    }

	sof_prop.sof_mode=0;
	sof_prop.to_trigger=0;
    ar_hal_vin_vsync_update(p_camera->fd_pipe,p_camera->pipe_id,&sof_prop);

    p_camera->frames=0;

    index=camera_find_cmd_pra_index_by_name("-frames",p_cmd);
	if(index>0)
	{
	    p_camera->frames=atoi(p_cmd->args[index+1]);
		//start vsync event
    	ar_hal_vin_create_event_client(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->event_client_id);
    	ar_always("event_client_id=%d",p_camera->event_client_id);
    	ar_os_thread_attr_t ctrl_attr = {0};
    	ctrl_attr.name = "stream";
    	ctrl_attr.stack_size = 16 * 1024;
    	p_camera->event_thread=ar_osal_thread_new(event_loop_thread, p_camera, &ctrl_attr);
    	//after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
    	for(int i=NOTIFY_EVENT_SOF;i<=NOTIFY_EVENT_SOF;i++)
    	{
    		ar_hal_vin_register_event(p_camera->fd_pipe,p_camera->pipe_id,p_camera->event_client_id,i);
    	}
	}


	if(p_camera->need_server)
	{
	    ar_signal_wait(p_camera->exit_signal);
		stop_camera(p_camera);

        //close vo dev
		ar_vo_dev_deinit(&p_camera->display_obj);

		ar_always("terminate the msg_loop");
		STRU_CAMERA_CMD_T *p_cmd=malloc(sizeof(STRU_CAMERA_CMD_T));
		memset(p_cmd,0,sizeof(STRU_CAMERA_CMD_T));
		strcpy(&p_cmd->args[0][0],"--exit_msg_loop");
		mq_send(p_camera->mqid,(const char *)p_cmd,sizeof(STRU_CAMERA_CMD_T),1);
		free(p_cmd);
		ar_always("wait the msg loop exit");
    	ar_osal_thread_join(p_camera->msg_loop);
		ar_always("clean mqid and exit signal");
		ar_delete_signal(p_camera->exit_signal);
	}
	ar_always("exit start camera");
	return 0;
}
static int camera_test_run_start_camera(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
	int index=camera_find_cmd_pra_index_by_name("-start_stream",p_cmd);
	if(index>0)
	{
	    camera_test_run_start_camera_stream(p_camera,p_cmd);
	}else
	{
	    camera_test_run_start_camera_main(p_camera,p_cmd);
	}
	return 0;
}

static int stop_camera(STRU_CAMERA_T *p_camera)
{
	ar_always("delete all the stream client and wait stream loop exit");
	for(int i=0;i<MAX_STREAM_SIZE;i++)
	{
	   if(p_camera->stream[i].enable)
	   {
		  ar_hal_vin_delete_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,p_camera->stream[i].client_id);
		  ar_osal_thread_join(p_camera->stream[i].stream_thread);
	   }
	}
	ar_always("stop stream");
	for(int i=0;i<MAX_STREAM_SIZE;i++)
	{
		if(p_camera->stream[i].enable)
		{
			ar_hal_vin_stop_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
		}
	}
	ar_always("delete stream buffer mem and delete stream buffer");
	for(int i=0;i<MAX_STREAM_SIZE;i++)
	{
		if(p_camera->stream[i].enable)
		{
			 for(int j=0;j<MAX_STREAM_BUFER_COUNT;j++)
			 {
				 for(int k=0;k<p_camera->stream[i].stream_buffer[j].panel_count;k++)
				 {
					 if(p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign)
					 {
						 ar_hal_sys_mmz_free((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign);
						 ar_always("free p_camera->stream[%d].stream_buffer[%d].pannel[%d].buffer_orign=%p",i,j,k,p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign);
						 p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign=NULL;
					 }
				 }
				ar_always("deletge stream buffer");
				ar_hal_vin_delete_stream_buffer(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
			 }
		}
	}
	ar_always("prepare deleted stream");
	for(int i=0;i<MAX_STREAM_SIZE;i++)
	{
		 if(p_camera->stream[i].enable)
		 {
			ar_always("delete stream %d",p_camera->stream[i].stream_id);			
			ar_hal_vin_preprae_delete_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
			ar_hal_vin_delete_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
			//close stream fd
			ar_hal_vin_close_stream_dev(p_camera->stream[i].fd_stream);
			p_camera->stream[i].enable=0;
		 }
	}
	ar_always("deinit pipeline");
	camera_pipeline_deinit(p_camera);
	ar_always("close the pipe");
	ar_hal_vin_close_pipe(p_camera->fd_vin_dev,p_camera->pipe_id);
	ar_always("unbind the pipe id and devid");
	ar_hal_vin_pipe_unbind_to_dev(p_camera->fd_vin_dev,p_camera->pipe_id,p_camera->dev_id);
	ar_always("close the pipe fd");
	ar_hal_vin_close_pipe_dev(p_camera->fd_pipe);
	ar_always("close the vin fd");
	ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
    return 0;
}

static int camera_test_run_stop_camera(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
	int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
	if(index>=0)
	{
		strcpy(p_cmd->args[index],"-local");
		camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
	}else
	{
	   index=camera_find_cmd_pra_index_by_name("-exit",p_cmd);
       if(index >=0 && p_camera->need_server)
       {
           ar_signal(p_camera->exit_signal);
       }
	}
	return 0;
}


__test_camera_case__(start_camera_big_mode)=
{
   "--start_camera_big_mode",
   camera_test_run_start_camera,
   {
      "start the camera with pra,the cmd will block the process until user to do stop/exit camera test",
	  "-server [name] -dev_id [id] -scaler1 [w] [h] [x] [y] [total_w] [total_h] [stride] -fps [fps ] ",
	  "-scaler2 [w] [h] [x] [y] [total_w] [total_h] [stride] -raw [w] [h] [x] [y] [total_w] [total_h] [stride] -hdr -cam_mode [online/offline/multi]",
	  "-scaler1_to [surface/buffer]",
	  "-scaler2_to [surface/buffer]",
	  "-surface1 [x] [y] [w] [h]",
	  "-surface2 [x] [y] [w] [h]",
	  "-surface3 [x] [y] [w] [h]",
	  "-sensor [name] :eg -sensor imx307",
	  "-man_aec_update",
	  "-man_awb_update",
	  "-man_af_update",
	  "-trigger",
	  "-aec [name]",
	  "-awb [name]",
	  "-af [name]",
	  "-max_wh w h fps",
	  "-start_stream",
	  "-frames [num]: start camera and run num frames eg: -frames 100",
	  "-offset",
	  "-y_offset",
	  "-big_raw_wh [w] [h]",
	  "-bit_depth [8/10/12/14/16]",
	  "-bayer [0:BAYER_RGGB 1:BAYER_BGGR 2:BAYER_GRBG 3:BAYER_GBRG]",
	  "-force_dng",
	  "-dump : -remote camera-2",
	  NULL,
   },
};

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

static int api_test_big_picture(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
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
						 int num=0;
                         index=camera_find_cmd_pra_index_by_name("-raw",p_cmd);
                		 if(index>0)
                		 {
                			 raw_file=p_cmd->args[index+1];
							 num=atoi(p_cmd->args[index+2]);
                		 }
						 STRU_DEV_INFO_VAR_T *p_var_pra=malloc(sizeof(STRU_DEV_INFO_VAR_T));
						 p_var_pra->res_index=0;
						 p_var_pra->res_count=1;
						 ar_hal_vin_get_sensor_var_pra(fd_pipe,pipe_id,p_var_pra);
						 int file_len=file_size(raw_file);
						 int raw_size=file_len/num;
						 int stride=raw_size/p_var_pra->res_info.height;

						 index=camera_find_cmd_pra_index_by_name("-stride",p_cmd);
						 if(index>0)
						 {
							stride=atoi(p_cmd->args[index+1]);
						 }
						 ar_always("stride=%d",stride);
						 p_raw_obj->stride=stride;
						 p_raw_obj->raw_frame_num=num;
						 p_raw_obj->raw_size=raw_size;
						 p_raw_obj->current_mem_index=0;
						 int size=0;
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
							p_raw_obj->mem[i].addr=(void*)CAM_ALIGNE_TO((AR_U64)p_raw_obj->mem[i].addr,256);
							p_raw_obj->mem[i].phy_addr=p_raw_obj->mem[i].phy_addr_orig=(void *)phy_addr;
							p_raw_obj->mem[i].phy_addr=(void*)CAM_ALIGNE_TO(phy_addr,256);
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
						int offset=64;
						index=camera_find_cmd_pra_index_by_name("-offset",p_cmd);
						if(index>0)
						{
						   offset=atoi(p_cmd->args[index+1]);
						}
						int bit_depth=12;
						index=camera_find_cmd_pra_index_by_name("-bit_depth",p_cmd);
						if(index>0)
						{
						   bit_depth=atoi(p_cmd->args[index+1]);
						}

						int dng=0;
						index=camera_find_cmd_pra_index_by_name("-dng",p_cmd);
						if(index>0)
						{
						   dng=1;
						}

						int dng_raw_offset=0;
						if(dng)
						{
						   dng_raw_offset=(p_var_pra->res_info.width-2*offset)*2;
						}else
						{
						   dng_raw_offset=p_var_pra->res_info.width-2*offset;
						   dng_raw_offset=ceil(dng_raw_offset/(128/bit_depth))*16;
						}
						ar_always("dng_raw_offset=%d bit_depth=%d dng=%d",dng_raw_offset,bit_depth,dng);
                        //main thread gen raw mem and send to dq loop thread
                        while(1)
                        {
                            if(p_raw_obj->current_mem_index>=p_raw_obj->raw_frame_num)
                            {
                               p_raw_obj->current_mem_index=0;
                            }
							STRU_STREAM_BUFFER_INIFO_T *p_info=NULL;
							p_raw_obj->stream_buffer_pool->queue_pop(p_raw_obj->stream_buffer_pool,(void**)&p_info);
							if(p_info)
							{
                                  //insert to the stream queue left of the picture
                                  p_info->panel_count=1;
								  p_info->pannel[0].used=p_raw_obj->raw_size;
								  p_info->pannel[0].length=p_info->pannel[0].used;
								  p_info->pannel[0].offset=0;
								  p_info->pannel[0].line_buffer_len=p_raw_obj->stride;
								  p_info->pannel[0].buffer_phy_addr=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr;
								  p_info->pannel[0].buffer_phy_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr_orig;
								  p_info->pannel[0].buffer=p_raw_obj->mem[p_raw_obj->current_mem_index].addr;
								  p_info->pannel[0].buffer_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].addr_orig;
                                  ar_hal_vin_q_stream_buffer(fd_pipe,pipe_id,0,p_info);
                                  free(p_info);

							}else
							{
							  ar_err("no buffer");
							}
							p_raw_obj->stream_buffer_pool->queue_pop(p_raw_obj->stream_buffer_pool,(void**)&p_info);
							if(p_info)
							{
                                  //insert to the stream queue right of the picture
                                  p_info->panel_count=1;
								  p_info->pannel[0].used=p_raw_obj->raw_size;
								  p_info->pannel[0].length=p_info->pannel[0].used;
								  p_info->pannel[0].offset=0;
								  p_info->pannel[0].line_buffer_len=p_raw_obj->stride;
								  p_info->pannel[0].buffer_phy_addr=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr+dng_raw_offset;
								  p_info->pannel[0].buffer_phy_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].phy_addr_orig;
								  p_info->pannel[0].buffer=p_raw_obj->mem[p_raw_obj->current_mem_index].addr+dng_raw_offset;
								  p_info->pannel[0].buffer_orign=p_raw_obj->mem[p_raw_obj->current_mem_index].addr_orig;
                                  ar_hal_vin_q_stream_buffer(fd_pipe,pipe_id,0,p_info);
								  p_raw_obj->current_mem_index++;
                                  free(p_info);

							}else
							{
							  ar_err("no buffer");
							}
							usleep(1000000/fps);  //30 fps
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

__test_camera_case__(send_big_pic_raw)=
{
   "--send_big_pic_raw",
   api_test_big_picture,
   {
      "test the api for big picute raw process, -remote [remote binder server name]: this indicate the api run on remote app,",
	  "-dev_id [id]",
	  "-raw [name] [num] -stride [stride]: the raw file name and how much frames in the file",
	  "-fps [fps]",
	  "-offset",
	  "-bit_depth [8/10/12/14/16]",
	  "-dng",
	  NULL,
   },
};
