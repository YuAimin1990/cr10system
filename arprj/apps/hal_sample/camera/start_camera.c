#include"camera.h"
#include "sensor.h"


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
	   ar_loop("get stream frame_id %d stream_id=%d index=%d",p_stream_buffer->frame_id,p_stream->stream_id,p_stream_buffer->buffer_index);
	   count=0;
	}
	count++;

	ar_hal_vin_return_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream_buffer);
	free(p_stream_buffer);
	free(p_frame);
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
		   ar_always("get client_id=%d frame_id %d buffer_index=%d stream_id=%d index=%d",client_id,p_stream_buffer->frame_id,p_stream_buffer->buffer_index,\
		   	p_stream->stream_id,p_stream_buffer->buffer_index);
		   count=0;
		}
		count++;


        if(p_stream_buffer->buffer_index==CAM_ERR_EXIT_MSG)
        {
            ar_always("receive exit msg , exit the stream loop thread");
            break;
        }

		#ifdef USE_SIMPLE_ELEMET
		if(ret>=0)
		{
     		SRTU_PAD_FRAME_T *p_frame=malloc(sizeof(SRTU_PAD_FRAME_T));
			p_frame->frameid = p_stream_buffer->frame_id;
     		p_frame->pannel_cout=p_stream_buffer->panel_count;
            p_frame->w = p_stream->stream_prop.total_w;
            p_frame->h = p_stream->stream_prop.total_h;
     		for(int i=0;i<p_frame->pannel_cout;i++){
     		    p_frame->pannel[i].len=p_stream_buffer->pannel[i].length;
     		    p_frame->pannel[i].used=p_stream_buffer->pannel[i].used;
     		    p_frame->pannel[i].stride=p_stream_buffer->pannel[i].line_buffer_len;
     		    p_frame->pannel[i].pa=p_stream_buffer->pannel[i].buffer_phy_addr;
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
			   is_first=0;
			}
     		push_to_next(p_camera->p_element,p_stream->p_pad,p_frame);
		}
		#else
		if(ret>=0){
		    ar_hal_vin_return_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream_buffer);
			free(p_stream_buffer);
		}
		#endif
	}
	//will be called by stop stream first
	//ar_hal_vin_delete_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_stream->stream_id,client_id);
	ar_osal_thread_exit();
	return NULL;
}

static SRTU_PAD_FRAME_T * buffer_sink_buffer_chain(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad,SRTU_PAD_FRAME_T *p_frame)
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
   mem_free(p_frame);
   return NULL;
}


static SRTU_PAD_FRAME_T *send_to_fw_display(STRU_SURFACE_OBJ_T *p_display,SRTU_PAD_FRAME_T *p_frame)
{
    STRU_AR_HAL_VO_DISP_BUF    buffer;
    int                        ret;
   // static uint32_t            frame_id = 0;

    buffer.frame_id = p_frame->frameid;
    buffer.format = AR_HAL_VO_FMT_YV12;
    buffer.frame_width = p_frame->w;
    buffer.frame_height = p_frame->h;
    buffer.roi.w = p_frame->w;
    buffer.roi.h = p_frame->h;
    buffer.roi.x = 0;
    buffer.roi.y = 0;
    buffer.luma_stride = p_frame->pannel[0].stride;
    buffer.chroma_stride = p_frame->pannel[1].stride;
    buffer.pannel_num =3; //p_frame->pannel_cout;
    for (int i=0; i<buffer.pannel_num; i++)
    {
        buffer.pannel[i].buffer = p_frame->pannel[i].va;
        buffer.pannel[i].buffer_orign = p_frame->pannel[i].va_orig;
        buffer.pannel[i].buffer_pa = p_frame->pannel[i].pa;
        buffer.pannel[i].buffer_pa_orign = p_frame->pannel[i].pa_orig;
        buffer.pannel[i].length = p_frame->pannel[i].len;
    }
    buffer.interlace_filed_flag = 0;
    buffer.usr_data = (void *)p_frame;
	#if 0
	ar_always("chan_%d send buffer frameid:%d (%p,%p,%p)", p_display->chan_id, buffer.frame_id,
		                                                   buffer.pannel[0].buffer_pa, buffer.pannel[1].buffer_pa,
		                                                   buffer.pannel[2].buffer_pa);
	#endif

  //  frame_id++;
    ret = ar_hal_vo_chn_send_frame(p_display->chan_fd, &buffer, 0);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

   End:
	   if (ret != AR_HAL_VO_SUCCESS)
	   {
		   ar_err("ret = 0x%x", ret);
		   return NULL;
	   }
    return NULL;
}

static SRTU_PAD_FRAME_T *surface_sink_buffer_chain(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad,SRTU_PAD_FRAME_T *p_frame)
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

static void *surface_loop_thread(void* arg)
{
	STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)arg;
    STRU_AR_HAL_VO_DISP_BUF    buffer={0};
    /* */
	while(!p_display->stop_flag)
	{
        int ret = ar_hal_vo_chn_get_frame(p_display->chan_fd, &buffer, p_display->wait_times);

        if(ret<0)
        {
           if(ret!=HAL_ERR_VO_CHN_TIMEOUT)
		   {
		     ar_err("err, exit the loop ret=%x %x",ret,HAL_ERR_VO_CHN_TIMEOUT);
		     ar_osal_thread_exit();
           }else
           {
              ar_err("time out .... go to end");
              goto End;
           }
        }

#if 0
		ar_always("chan_%d get buffer frameid:%d (%p,%p,%p)", p_display->chan_id, buffer.frame_id,
															   buffer.pannel[0].buffer_pa, buffer.pannel[1].buffer_pa,
															   buffer.pannel[2].buffer_pa);
#endif
        ret = ar_hal_vo_chn_release_frame(p_display->chan_fd, &buffer);
        if(ret<0)
        {
           if(ret!=HAL_ERR_VO_CHN_TIMEOUT)
		   {
		     ar_err("err, exit the loop");
		     ar_osal_thread_exit();
           }else
           {
              ar_err("time out .... go to end");
              goto End;
           }
        }
        End:
    	if(ret == AR_HAL_VO_SUCCESS)
    	{
            if(buffer.usr_data)
            {
                mem_free((SRTU_PAD_FRAME_T *)buffer.usr_data);
            }
    	}
	}
	ar_osal_thread_exit();
    return NULL;
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
    char cmd_pra_name[16];
    int ret = AR_HAL_VO_SUCCESS;

	p_display->dev_fd=p_camera->display_obj.dev_fd;
	p_display->e_dev_id=p_camera->display_obj.e_dev_id;
	p_display->e_layer_id=p_camera->display_obj.e_layer_id;
	p_display->layer_fd=p_camera->display_obj.layer_fd;

    sprintf(cmd_pra_name, "-surface%d", surface_num+1);
    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);

    if(index>0)
    {
        x=atoi(p_cmd->args[index+1]);
        y=atoi(p_cmd->args[index+2]);
        w=atoi(p_cmd->args[index+3]);
        h=atoi(p_cmd->args[index+4]);
    }
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

   p_display->wait_times=200;
   index=camera_find_cmd_pra_index_by_name("-surface_loop_times",p_cmd);
   if(index>=0)
   {
      p_display->wait_times=atoi(p_cmd->args[index+1]);
   }

    index = camera_find_cmd_pra_index_by_name("-vc", p_cmd);
    if (index >= 0)
        p_display->chan_id = p_camera->dev_id*6 + surface_num;
    else
        p_display->chan_id = p_camera->dev_id*2 + surface_num;

    ar_always("chan_id=%d (x,y,w,h)==>(%d %d %d %d)",p_display->chan_id,x,y,w,h);


    ret = ar_vo_surface_init(p_display,&rect);
    //start a thread to get display frames
	//create thread for stream buffer process
	ar_os_thread_attr_t ctrl_attr = {0};
	ctrl_attr.name = "stream";
	ctrl_attr.stack_size = 16 * 1024;
	p_display->surface_thread=ar_osal_thread_new(surface_loop_thread,p_display, &ctrl_attr);

    return ret;
}

static int camera_pipeline_init(STRU_CAMERA_T *p_camera, STRU_CAMERA_CMD_T *p_cmd)
{
    int index = 0;
    STRU_STREAM_T *p_strm = NULL;
    // STRU_ELMENT_T *p_dest_element = NULL;
    char cmd_pra_name[16];

    // creat element for buffer loop
    p_camera->p_element = creat_element("cam_src", p_camera);
    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        p_strm = &p_camera->stream[i];
        p_strm->p_pad = creat_pad("cam_src_pad", p_strm);
        add_out_pad(p_camera->p_element, p_strm->p_pad);
        p_strm->p_pad->buffer_chain = NULL;
    }

    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        STRU_SURFACE_OBJ_T *p_display = malloc(sizeof(STRU_SURFACE_OBJ_T));
        memset(p_display, 0, sizeof(STRU_SURFACE_OBJ_T));
        p_display->chan_fd = -1;
        p_display->stop_flag = 0;
        p_camera->p_surface_element[i] = creat_element(
            "surface_element", p_display);
        p_display->p_element = p_camera->p_surface_element[i];
        p_display->pad = creat_pad("surface_pad_0", NULL);
        add_in_pad(p_display->p_element, p_display->pad);
        p_display->pad->buffer_chain = surface_sink_buffer_chain;

        // buffer element
        STRU_BUFFER_OBJ_T *p_buffer_obj = malloc(sizeof(STRU_BUFFER_OBJ_T));
        p_camera->p_buffer_element[i] = creat_element(
            "buffer_element", p_buffer_obj);
        p_buffer_obj->p_element = p_camera->p_buffer_element[i];
        p_buffer_obj->pad = creat_pad("buffer_pad_0", NULL);
        add_in_pad(p_buffer_obj->p_element, p_buffer_obj->pad);
        p_buffer_obj->pad->buffer_chain = buffer_sink_buffer_chain;
    }

    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        p_strm = &p_camera->stream[i];
        if (!p_strm->enable)
            continue;

        if (p_strm->stream_prop.stream_type != STREAM_TYPE_RAW_BAYER) {
            sprintf(cmd_pra_name, "-scaler%d_to", i+1);
            index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
            if (index >= 0) {
                if (!strcmp("surface", p_cmd->args[index+1])) {
                    ar_always("scaler%d_to link to p_surface_element %d",
                        i+1, i);
                    camera_surface_init(p_camera, p_cmd, i);
					link_pad(p_camera->p_element, i, p_camera->p_surface_element[i], 0);
                } else {
                    ar_always("scaler%d_to link to p_buffer_element %d",
                        i+1, i);
					link_pad(p_camera->p_element, i, p_camera->p_buffer_element[i], 0);
                }
            } else {
                ar_always("scaler%d_to link to p_buffer_element %d", i+1, i);
				link_pad(p_camera->p_element, i, p_camera->p_buffer_element[i], 0);
            }
        } else {
            ar_always("scaler%d_to link to p_buffer_element %d", i+1, i);
			link_pad(p_camera->p_element, i, p_camera->p_buffer_element[i], 0);
        }

    }

    return 0;
}

static int surface_deinit_with_ch(STRU_CAMERA_T *p_camera,int ch)
{
	STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[ch]->obj;
    STRU_AR_HAL_VO_DISP_BUF buffer;
	int ret=0;
	if(p_display->chan_fd>=0)
	{
		ar_always("wait the surface thread exit");
		p_display->stop_flag=1;
		ar_osal_thread_join(p_display->surface_thread);

		ar_hal_vo_chn_pause(p_display->chan_fd);

        ar_always("wait clean all the buffer");

        ar_hal_vo_chn_clear_frame(p_display->chan_fd, AR_TRUE);
		while(1)
		{
		    memset(&buffer,0,sizeof(buffer));
			ret = ar_hal_vo_chn_get_frame(p_display->chan_fd, &buffer, 0);
			if (ret != AR_HAL_VO_SUCCESS){
				break;
			}

			ret = ar_hal_vo_chn_release_frame(p_display->chan_fd, &buffer);
			if (ret != AR_HAL_VO_SUCCESS){
				break;
			}
			ar_always("recycle frame");
		}
		ar_hal_vo_chn_disable(p_display->chan_fd);
		ar_hal_vo_fd_close(p_display->chan_fd);
		ar_always("surface thread exit");
	}
    return 0;
}

static int surface_deinit(STRU_CAMERA_T *p_camera)
{
    int ret=0;
    //stop display first
    //disable all the surface
    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
		surface_deinit_with_ch(p_camera,i);
    }
	return ret;
}

static int camera_pipeline_deinit(STRU_CAMERA_T *p_camera)
{
    // int ret;
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


	   index=camera_find_cmd_pra_index_by_name("-format1",p_cmd);
	   if(index>=0)
	   {
	        int out_format=atoi(p_cmd->args[index+1]);
			ar_always("out_format1=%d",out_format);
			switch(out_format)
			{
			   case 0:
			   p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			   break;
			   case 1:
			   p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_semiPlannar;
			   break;
			   case 2:
			   p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_CF50_8BIT_Plannar;
			   break;
			   default:
			   ar_always("not supported use yuv420");
			   p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			   break;
			}
	   }


	   int sub_index=camera_find_cmd_pra_index_by_name("-scaler1_ext",p_cmd);
       if(sub_index>0)
       {
          sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
		  if(sub_index>0)
		  {
		      p_camera->stream[0].stream_prop.hw_out_index|=(1<<31);
			  int hw_out=atoi(p_cmd->args[sub_index+1]);
			  ar_always("hw_index=%d",hw_out);
			  p_camera->stream[0].stream_prop.hw_out_index |=(hw_out&0xf);
		  }
       }

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


	   index=camera_find_cmd_pra_index_by_name("-format2",p_cmd);
	   if(index>=0)
	   {
	        int out_format=atoi(p_cmd->args[index+1]);
			ar_always("out_format2=%d",out_format);
			switch(out_format)
			{
			   case 0:
			   p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			   break;
			   case 1:
			   p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_semiPlannar;
			   break;
			   default:
			   ar_always("not supported use yuv420");
			   p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			   break;
			}
	   }

	   int sub_index=camera_find_cmd_pra_index_by_name("-scaler2_ext",p_cmd);
       if(sub_index>0)
       {
          sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
		  if(sub_index>0)
		  {
		      p_camera->stream[0].stream_prop.hw_out_index|=(1<<31);
			  int hw_out=atoi(p_cmd->args[sub_index+1]);
			  ar_always("hw_index=%d",hw_out);
			  p_camera->stream[0].stream_prop.hw_out_index |=(hw_out&0xf);
		  }
       }

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

	   int sub_index=camera_find_cmd_pra_index_by_name("-raw_ext",p_cmd);
       if(sub_index>0)
       {
          sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
		  if(sub_index>0)
		  {
		      p_camera->stream[0].stream_prop.hw_out_index|=(1<<31);
			  int hw_out=atoi(p_cmd->args[sub_index+1]);
			  ar_always("hw_index=%d",hw_out);
			  p_camera->stream[0].stream_prop.hw_out_index |=(hw_out&0xf);
		  }
       }

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

static int need_vo(STRU_CAMERA_T *p_camera, STRU_CAMERA_CMD_T *p_cmd)
{
    char cmd_pra_name[16];

    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        sprintf(cmd_pra_name, "-scaler%d_to", i+1);
        int index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
        if (index >= 0) {
            if(!strcmp("surface", p_cmd->args[index+1]))
                return 1;
        }
    }

    return 0;
}

static int camera_test_run_start_camera_main(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    int ret=0;
	char *name=NULL;
	int need_server=0;
	int need_deskew = 0;

	int index=camera_find_cmd_pra_index_by_name("-remote",p_cmd);
	if(index>=0)
	{
		strcpy(p_cmd->args[index],"-local");
		ar_always("start camera with remote");
		camera_send_cmd_to_remote(p_camera,p_cmd->args[index+1],p_cmd);
		return 0;
	}
	index=camera_find_cmd_pra_index_by_name("-server",p_cmd);
	if(index>=0)
	{
		need_server=1;
	}
	
	p_camera->need_server=need_server;
	name=p_cmd->args[index+1];

	index=camera_find_cmd_pra_index_by_name("-deskew",p_cmd);
	if(index>=0)
	{
		need_deskew=1;
	}
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

	index=camera_find_cmd_pra_index_by_name("-isp_in",p_cmd);
	if(index > 0)
	{
	   preview_stream_type=STREAM_TYPE_SIM_PREVIEW;
	   video_stream_type=STREAM_TYPE_SIM_VIDEO;
	}

	if (strcmp(p_camera->dev_info.sensor_name, "hdmi_video_dvp_in") == 0
		|| strcmp(p_camera->dev_info.sensor_name, "cam_tp9930_0") == 0
		|| strcmp(p_camera->dev_info.sensor_name, "cam_tp9930_1") == 0)
	{
		preview_stream_type = STREAM_TYPE_RAW_YUV;
		video_stream_type = STREAM_TYPE_RAW_YUV;
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
	}else
	{
	    p_camera->pipe_prop.max_w[0]=1920;
	    p_camera->pipe_prop.max_h[0]=1080;
	    p_camera->pipe_prop.max_fps[0]=25;
	}
    ar_always("sensor sel: (w h fps)==>(%d %d %f)",p_camera->pipe_prop.max_w[0],p_camera->pipe_prop.max_h[0],p_camera->pipe_prop.max_fps[0]);

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
		if(need_vo(p_camera,p_cmd))
		{
            AR_BOOL  mipi = AR_FALSE;
            AR_BOOL  interlace = AR_FALSE;

           index=camera_find_cmd_pra_index_by_name("-mipi_tx",p_cmd);
           if(index>0)
    		   mipi =  AR_TRUE;
           else
    		   mipi =  AR_FALSE;

            index=camera_find_cmd_pra_index_by_name("-vo_interlace",p_cmd);
            if(index>0)
                interlace =  AR_TRUE;
            else
                interlace =  AR_FALSE;

            ar_vo_dev_init(&p_camera->display_obj,1920,1080,-1,-1,p_camera->pipe_prop.max_fps[0], mipi, interlace);
		}
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
	p_camera->stream[0].stream_prop.queue_depth=3;
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

       int sub_index=camera_find_cmd_pra_index_by_name("-scaler1_ext",p_cmd);
       if(sub_index>0)
       {
    	   sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
    	   if(sub_index>0)
    	   {
    		   p_camera->stream[0].stream_prop.hw_out_index|=(1<<31);
    		   int hw_out=atoi(p_cmd->args[sub_index+1]);
    		   ar_always("hw_index=%d",hw_out);
    		   p_camera->stream[0].stream_prop.hw_out_index |=(hw_out&0xf);
    	   }
        }


	}

	p_camera->stream[0].stream_prop.fps=p_camera->pipe_prop.max_fps[0];

	index=camera_find_cmd_pra_index_by_name("-fps1",p_cmd);
	if(index>=0)
	{
	    p_camera->stream[0].stream_prop.fps=atoi(p_cmd->args[index+1]);
	}

	p_camera->stream[0].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	p_camera->stream[0].stream_prop.stream_type=preview_stream_type;
	p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;

	index=camera_find_cmd_pra_index_by_name("-format1",p_cmd);
	if(index>=0)
	{
		 int out_format=atoi(p_cmd->args[index+1]);
		 ar_always("out_format1=%d",out_format);
		 switch(out_format)
		 {
			case 0:
			p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			break;
			case 1:
			p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_semiPlannar;
			break;
			case 2:
			p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_CF50_8BIT_Plannar;
			break;
			default:
			ar_always("not supported use yuv420");
			p_camera->stream[0].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			break;
		 }
	}

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
	p_camera->stream[1].stream_prop.queue_depth=3;
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
	   int sub_index=camera_find_cmd_pra_index_by_name("-scaler2_ext",p_cmd);
       if(sub_index>0)
       {
    	   sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
    	   if(sub_index>0)
    	   {
    		   p_camera->stream[1].stream_prop.hw_out_index|=(1<<31);
    		   int hw_out=atoi(p_cmd->args[sub_index+1]);
    		   ar_always("hw_index=%d",hw_out);
    		   p_camera->stream[1].stream_prop.hw_out_index |=(hw_out&0xf);
    	   }
        }
	}
	p_camera->stream[1].stream_prop.fps=p_camera->pipe_prop.max_fps[0];;
	index=camera_find_cmd_pra_index_by_name("-fps2",p_cmd);
	if(index>=0)
	{
	    p_camera->stream[1].stream_prop.fps=atoi(p_cmd->args[index+1]);
	}

	p_camera->stream[1].stream_prop.mode=1<<STREAM_MODE_NORMAL;
	p_camera->stream[1].stream_prop.stream_type=video_stream_type;
	p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;

	index=camera_find_cmd_pra_index_by_name("-format2",p_cmd);
	if(index>=0)
	{
		 int out_format=atoi(p_cmd->args[index+1]);
		 ar_always("out_format2=%d",out_format);
		 switch(out_format)
		 {
			case 0:
			p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			break;
			case 1:
			p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_semiPlannar;
			break;
			default:
			ar_always("not supported use yuv420");
			p_camera->stream[1].stream_prop.format=STREAM_FORMAT_YUV420_8BIT_Plannar;
			break;
		 }
	}

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
	p_camera->stream[2].stream_prop.queue_depth=3;
	p_camera->stream[2].stream_prop.fps=p_camera->pipe_prop.max_fps[0];;
	index=camera_find_cmd_pra_index_by_name("-fps3",p_cmd);
	if(index>=0)
	{
	    p_camera->stream[2].stream_prop.fps=atoi(p_cmd->args[index+1]);
	}

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
	   int sub_index=camera_find_cmd_pra_index_by_name("-raw_ext",p_cmd);
       if(sub_index>0)
       {
    	   sub_index=camera_find_cmd_pra_index_by_name("-hw_index",p_cmd);
    	   if(sub_index>0)
    	   {
    		   p_camera->stream[2].stream_prop.hw_out_index|=(1<<31);
    		   int hw_out=atoi(p_cmd->args[sub_index+1]);
    		   ar_always("hw_index=%d",hw_out);
    		   p_camera->stream[2].stream_prop.hw_out_index |=(hw_out&0xf);
    	   }
        }
	}
	p_camera->stream[2].stream_prop.fps=p_camera->pipe_prop.max_fps[0];;
	index=camera_find_cmd_pra_index_by_name("-fps3",p_cmd);
	if(index>=0)
	{
	    p_camera->stream[2].stream_prop.fps=atoi(p_cmd->args[index+1]);
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

    //here we set the fre
    p_camera->public_prop.vif_fre_mod=0;
	index=camera_find_cmd_pra_index_by_name("-vif_fre",p_cmd);
	if(index>=0)
	{
		p_camera->public_prop.vif_fre_mod=1;
        p_camera->public_prop.vif_fre_hz=atoi(p_cmd->args[index+1]);
	}

	p_camera->public_prop.isp_fre_mod=0;
	index=camera_find_cmd_pra_index_by_name("-isp_fre",p_cmd);
	if(index>=0)
	{
		p_camera->public_prop.isp_fre_mod=1;
        p_camera->public_prop.isp_fre_hz=atoi(p_cmd->args[index+1]);
	}

    p_camera->public_prop.hdr_fre_mod=0;
	index=camera_find_cmd_pra_index_by_name("-hdr_fre",p_cmd);
	if(index>=0)
	{
		p_camera->public_prop.hdr_fre_mod=1;
        p_camera->public_prop.hdr_fre_hz=atoi(p_cmd->args[index+1]);
	}

	
    p_camera->public_prop.mipi_fre_mod=0;
	index=camera_find_cmd_pra_index_by_name("-all_mpi_fre",p_cmd);
	if(index>=0)
	{
		p_camera->public_prop.mipi_fre_mod=1;
        p_camera->public_prop.mipi_fre_hz=atoi(p_cmd->args[index+1]);
	}

    ar_always("vif isp hdr ==> mode=(%d %d %d) frehz=(%d %d %d)",\
		p_camera->public_prop.vif_fre_mod,p_camera->public_prop.isp_fre_mod,p_camera->public_prop.hdr_fre_mod,\
		p_camera->public_prop.vif_fre_hz,p_camera->public_prop.isp_fre_hz,p_camera->public_prop.hdr_fre_hz);


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


    //set the man res
    int man_res=0;
	index=camera_find_cmd_pra_index_by_name("-man_res",p_cmd);
	if(index>0)
	{
	    man_res=0x10;
		int res_index=atoi(p_cmd->args[index+1]);
		man_res|=res_index;
	}

	ar_always("man_res=0x%x",man_res);
	p_camera->pipe_prop.man_res_mask=man_res;

	p_camera->pipe_prop.vfe_mode=0;
	index=camera_find_cmd_pra_index_by_name("-vfe_mode",p_cmd);
	if(index>0)
	{
		p_camera->pipe_prop.vfe_mode=atoi(p_cmd->args[index+1]);
	}

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

    if(man_res)
	{
	    p_camera->sensor_var_pra.res_index=(man_res&0xf);
		ar_always("res_index=%d",p_camera->sensor_var_pra.res_index);
		ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);
     	index=camera_find_cmd_pra_index_by_name("-mipi_fre",p_cmd);
        if(index>0)
     	{
     	      int mipi_fre=atoi(p_cmd->args[index+1]);
     		  mipi_fre=mipi_fre/1000000;
           	  p_camera->sensor_var_pra.res_info.mipi_pix_clk=mipi_fre;
         }
     	index=camera_find_cmd_pra_index_by_name("-dpcm",p_cmd);
        if(index>0)
     	{
     	      int dpcm_en=atoi(p_cmd->args[index+1]);
     	      int dpcm=atoi(p_cmd->args[index+2]);
     		  p_camera->sensor_var_pra.res_info.use_encoder_dpcm=dpcm_en;
           	  p_camera->sensor_var_pra.res_info.dpcm_out_bits=dpcm;
         }
     	index=camera_find_cmd_pra_index_by_name("-dpcm_nr3d",p_cmd);
        if(index>0)
     	{
     	      int dpcm_en=atoi(p_cmd->args[index+1]);
           	  p_camera->sensor_var_pra.res_info.use_nr3d_encoder=dpcm_en;
         }

     	index=camera_find_cmd_pra_index_by_name("-skip_ration",p_cmd);
        if(index>0)
        {
		    p_camera->sensor_var_pra.res_info.aec_factor=atoi(p_cmd->args[index+1]);
            p_camera->sensor_var_pra.res_info.awb_factor=atoi(p_cmd->args[index+2]);
            p_camera->sensor_var_pra.res_info.ltm_factor=atoi(p_cmd->args[index+3]);
			ar_always("aec awb ltm raion==>(%d %d %d)",\
				p_camera->sensor_var_pra.res_info.aec_factor,\
				p_camera->sensor_var_pra.res_info.awb_factor,\
				p_camera->sensor_var_pra.res_info.ltm_factor);
        }
     	index=camera_find_cmd_pra_index_by_name("-settle_count",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.out_infor.mipi_desc.settle_count=atoi(p_cmd->args[index+1]);
			ar_always("settle count =%d",p_camera->sensor_var_pra.out_infor.mipi_desc.settle_count);
		}
     	index=camera_find_cmd_pra_index_by_name("-dng",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.force_dng=atoi(p_cmd->args[index+1]);
			ar_always("force_dng =%d",p_camera->sensor_var_pra.res_info.force_dng);
		}
		index=camera_find_cmd_pra_index_by_name("-bit_width",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.bit_depth=atoi(p_cmd->args[index+1]);
			ar_always("bit_width =%d",p_camera->sensor_var_pra.res_info.bit_depth);
		}

		index=camera_find_cmd_pra_index_by_name("-bayer",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.out_infor.bayer=atoi(p_cmd->args[index+1]);
			ar_always("bayer =%d",p_camera->sensor_var_pra.out_infor.bayer);
		}

        index=camera_find_cmd_pra_index_by_name("-crop_en",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.crop_en=atoi(p_cmd->args[index+1]);
			ar_always("crop_en =%d",p_camera->sensor_var_pra.res_info.crop_en);
		}
		index=camera_find_cmd_pra_index_by_name("-sensor_res",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.width=atoi(p_cmd->args[index+1]);
            p_camera->sensor_var_pra.res_info.height=atoi(p_cmd->args[index+2]);
			ar_always("sensor width =%d", p_camera->sensor_var_pra.res_info.width);
			ar_always("sensor height =%d", p_camera->sensor_var_pra.res_info.height);
		}
		index=camera_find_cmd_pra_index_by_name("-total_res",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.line_length=atoi(p_cmd->args[index+1]);
            p_camera->sensor_var_pra.res_info.frame_length=atoi(p_cmd->args[index+2]);
			ar_always("sensor width =%d", p_camera->sensor_var_pra.res_info.line_length);
			ar_always("sensor height =%d", p_camera->sensor_var_pra.res_info.frame_length);
		}
		index=camera_find_cmd_pra_index_by_name("-crop_info",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.x_start=atoi(p_cmd->args[index+1]);
            p_camera->sensor_var_pra.res_info.y_start=atoi(p_cmd->args[index+2]);
            p_camera->sensor_var_pra.res_info.x_width=atoi(p_cmd->args[index+3]);
            p_camera->sensor_var_pra.res_info.y_height=atoi(p_cmd->args[index+4]);
			ar_always("x_start =%d", p_camera->sensor_var_pra.res_info.x_start);
			ar_always("y_start =%d", p_camera->sensor_var_pra.res_info.y_start);
			ar_always("x_width =%d", p_camera->sensor_var_pra.res_info.x_width);
			ar_always("y_height =%d", p_camera->sensor_var_pra.res_info.y_height);
		}
		index=camera_find_cmd_pra_index_by_name("-hdr_crop",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.hdr_x_start=atoi(p_cmd->args[index+1]);
            p_camera->sensor_var_pra.res_info.hdr_y_start=atoi(p_cmd->args[index+2]);
            p_camera->sensor_var_pra.res_info.hdr_width=atoi(p_cmd->args[index+3]);
            p_camera->sensor_var_pra.res_info.hdr_height=atoi(p_cmd->args[index+4]);
			ar_always("hdr_x_start =%d", p_camera->sensor_var_pra.res_info.hdr_x_start);
			ar_always("hdr_y_start =%d", p_camera->sensor_var_pra.res_info.hdr_y_start);
			ar_always("hdr_width =%d",  p_camera->sensor_var_pra.res_info.hdr_width);
			ar_always("hdr_height =%d", p_camera->sensor_var_pra.res_info.hdr_height);
		}
		index=camera_find_cmd_pra_index_by_name("-long_offset",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.long_offset=atoi(p_cmd->args[index+1]);
			ar_always("long_offset =%d", p_camera->sensor_var_pra.res_info.long_offset);
		}
		index=camera_find_cmd_pra_index_by_name("-mid_offset",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.mid_offset=atoi(p_cmd->args[index+1]);
			ar_always("mid_offset =%d", p_camera->sensor_var_pra.res_info.mid_offset);
		}
		index=camera_find_cmd_pra_index_by_name("-short_offset",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.short_offset=atoi(p_cmd->args[index+1]);
			ar_always("short_offset =%d", p_camera->sensor_var_pra.res_info.short_offset);
		}
		index=camera_find_cmd_pra_index_by_name("-hdr_frame_count",p_cmd);
		if(index>0)
		{
            p_camera->sensor_var_pra.res_info.hdr_frame_count=atoi(p_cmd->args[index+1]);
			ar_always("hdr_frame_count =%d", p_camera->sensor_var_pra.res_info.hdr_frame_count);
		}
		p_camera->sensor_var_pra.out_infor.cam_infor.face=CAM_FACE_BACK;
		index=camera_find_cmd_pra_index_by_name("-isp_in",p_cmd);
		if(index>0)
		{
		   p_camera->sensor_var_pra.out_infor.cam_infor.face=CAM_FACE_EMULATOT;
		   p_camera->sensor_var_pra.out_infor.link_mod = LINK_MODE_MEM;
		   ar_always("isp in dev =%d", p_camera->sensor_var_pra.out_infor.cam_infor.face);
		}
		ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe,p_camera->pipe_id,&p_camera->sensor_var_pra);
    }

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
                   memset(p_vaddr,0x5c,p_camera->stream[i].stream_buffer[j].pannel[k].length);

				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[k].buffer=p_vaddr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer=(void *)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_orign,256);
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign=(void *)phy_addr;
				   p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_addr=(void *)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[k].buffer_phy_orign,256);
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
    if (need_deskew)
    {
        ar_hal_vin_set_deskew(p_camera->fd_pipe, p_camera->pipe_id, 0, 10);
        ar_hal_vin_set_deskew(p_camera->fd_pipe, p_camera->pipe_id, 1, 12);
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


	if(need_server)
	{
	    ar_signal_wait(p_camera->exit_signal);
		stop_camera(p_camera);

        //close vo dev

		if(need_vo(p_camera,p_cmd)){
		   ar_vo_dev_deinit(&p_camera->display_obj);
		}

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

static void camera_set_dvp_stream_property(STRU_CAMERA_T *p_camera, STRU_CAMERA_CMD_T *p_cmd)
{
	 int i = 0, index = 0;
	 char cmd_pra_name[16];
	 for (i = 0; i < MAX_STREAM_SIZE; i++) {
	    p_camera->stream[i].enable = 0;
	    p_camera->stream[i].p_camera = p_camera;

	    STRU_AR_STREAM_PROPERTY_T *p_strm_porp =
	        &(p_camera->stream[i].stream_prop);
	    p_strm_porp->skip = 0;
	    p_strm_porp->queue_depth = 3;
	    p_strm_porp->mode = (1 << STREAM_MODE_NORMAL);

	    if ((i % 3) == 0)
	        p_strm_porp->stream_type = STREAM_TYPE_RAW_YUV_VC0 + i/3;
	    else if ((i % 3) == 1)
	        p_strm_porp->stream_type = STREAM_TYPE_RAW_YUV_SCALER_VC0 + i/3;
	    else
	        p_strm_porp->stream_type = STREAM_TYPE_RAW_YUV_SUB_SCALER_VC0 + i/3;

	    p_strm_porp->width = 1280;
	    p_strm_porp->height = 720;
	    p_strm_porp->x = 0;
	    p_strm_porp->y = 0;
	    p_strm_porp->total_w = 1280;
	    p_strm_porp->total_h = 720;
	    p_strm_porp->stride = -1;

	    sprintf(cmd_pra_name, "-scaler%d", i+1);
	    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
	    if (index > 0) {
	        p_camera->stream[i].enable = 1;
	        p_strm_porp->width = atoi(p_cmd->args[index+1]);
	        p_strm_porp->height = atoi(p_cmd->args[index+2]);
	        p_strm_porp->x = atoi(p_cmd->args[index+3]);
	        p_strm_porp->y = atoi(p_cmd->args[index+4]);
	        p_strm_porp->total_w = atoi(p_cmd->args[index+5]);
	        p_strm_porp->total_h = atoi(p_cmd->args[index+6]);
	        p_strm_porp->stride = atoi(p_cmd->args[index+7]);

	        sprintf(cmd_pra_name, "-scaler%d_ext", i+1);
	        int sub_index = camera_find_cmd_pra_index_by_name(
	            cmd_pra_name, p_cmd);
	        if(sub_index > 0) {
	            sub_index = camera_find_cmd_pra_index_by_name(
	                "-hw_index", p_cmd);
	            if(sub_index > 0) {
	                p_strm_porp->hw_out_index |= (1<<31);
	                int hw_out = atoi(p_cmd->args[sub_index+1]);
	                ar_always("hw_index=%d", hw_out);
	                p_strm_porp->hw_out_index |= (hw_out & 0xf);
	            }
	        }
	    }

	    p_strm_porp->fps = 25;
	    sprintf(cmd_pra_name, "-fps%d", i+1);
	    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
	    if(index >= 0)
	        p_strm_porp->fps = atoi(p_cmd->args[index+1]);

	    p_strm_porp->format = STREAM_FORMAT_YUV420_8BIT_Plannar;
	    sprintf(cmd_pra_name, "-format%d", i+1);
	    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
	    if(index >= 0) {
	        int out_format = atoi(p_cmd->args[index+1]);
	        ar_always("out_format%d = %d", i+1, out_format);
	        switch(out_format) {
	        case 0:
	            p_strm_porp->format = STREAM_FORMAT_YUV420_8BIT_Plannar;
	            break;
	        case 1:
	            p_strm_porp->format = STREAM_FORMAT_YUV420_8BIT_semiPlannar;
	            break;
	        case 2:
	            p_strm_porp->format = STREAM_FORMAT_YUV420_CF50_8BIT_Plannar;
	            break;
	        default:
	            ar_always("not supported use yuv420");
	            p_strm_porp->format = STREAM_FORMAT_YUV420_8BIT_Plannar;
	            break;
	        }
	    }
	}
	return;
}

static int camera_test_run_start_camera_vc(STRU_CAMERA_T *p_camera,
    STRU_CAMERA_CMD_T *p_cmd, int vc_type)
{
    int ret = 0;
    char *name = NULL;
    int need_server = 0;
    int i = 0;
    char cmd_pra_name[16];

    int index = camera_find_cmd_pra_index_by_name("-remote", p_cmd);
    if (index >= 0) {
        strcpy(p_cmd->args[index], "-local");
        ar_always("start camera with remote");
        camera_send_cmd_to_remote(p_camera, p_cmd->args[index+1], p_cmd);
        return 0;
    }

    index = camera_find_cmd_pra_index_by_name("-server", p_cmd);
    if (index >= 0) {
        need_server = 1;
        name = p_cmd->args[index+1];
    }
    p_camera->need_server = need_server;

    p_camera->dev_id = 0;
    index = camera_find_cmd_pra_index_by_name("-dev_id", p_cmd);
    p_camera->dev_id = (index >= 0 ? atoi(p_cmd->args[index+1]) : 0);

    // open the vin dev
    p_camera->fd_vin_dev = ar_hal_vin_open_vin_dev();
    if (p_camera->fd_vin_dev < 0) {
        ar_err("open vin dev err");
        return -1;
    }

    //first to creat a msg queue
    if (need_server) {
        char server_name[64];
        sprintf(server_name, "/%s_%d", name, p_camera->dev_id);
        ar_always("server name is %s", server_name);

        struct mq_attr attr = {0};
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(STRU_CAMERA_CMD_T);
        p_camera->mqid = mq_open(server_name, O_CREAT | O_RDWR, 0666, &attr);
        if(p_camera->mqid == (mqd_t)-1) {
            ar_err("start server failed, %d", p_camera->mqid);
            perror("open mq failed, the error code is :");
            return -1;
        }

        mq_getattr(p_camera->mqid, &attr);
        ar_always("mq_maxmsg=%d mq_msgsize=%d sizeof(STRU_CAMERA_CMD_T)=%d",
            attr.mq_maxmsg, attr.mq_msgsize, sizeof(STRU_CAMERA_CMD_T));

        //creat a loop to receive msg
        ar_os_thread_attr_t ctrl_attr = {0};
        ctrl_attr.name = "msg";
        ctrl_attr.stack_size = 16 * 1024;
        p_camera->msg_loop = ar_osal_thread_new(msg_loop_thread,
            p_camera, &ctrl_attr);
        p_camera->exit_signal = ar_create_signal();

        //init vo dev
        if(need_vo(p_camera, p_cmd))
            ar_vo_dev_init(&p_camera->display_obj, 1024, 600, -1, -1,30, AR_FALSE, AR_FALSE);
    }
    if (!vc_type)
	{
		//dvp
		camera_set_dvp_stream_property(p_camera, p_cmd);
	}
	else
	{	
		//mipi
		
	 	for (i = 0; i < MAX_STREAM_SIZE; i++)
		{
		    p_camera->stream[i].enable = 0;
		    p_camera->stream[i].p_camera = p_camera;

		    STRU_AR_STREAM_PROPERTY_T *p_strm_porp =
		        &(p_camera->stream[i].stream_prop);
		    p_strm_porp->skip = 0;
		    p_strm_porp->queue_depth = 3;
		    p_strm_porp->mode = (1 << STREAM_MODE_NORMAL);

		    p_strm_porp->stream_type = STREAM_TYPE_RAW_BAYER_VC0 + i;

		    p_strm_porp->width = 2712;
		    p_strm_porp->height = 1538;
		    p_strm_porp->x = 0;
		    p_strm_porp->y = 0;
		    p_strm_porp->total_w = 2712;
		    p_strm_porp->total_h = 1538;
		    p_strm_porp->stride = -1;

		    sprintf(cmd_pra_name, "-buffer%d", i);
		    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
		    if (index > 0) {
		        p_camera->stream[i].enable = 1;
		        p_strm_porp->width = atoi(p_cmd->args[index+1]);
		        p_strm_porp->height = atoi(p_cmd->args[index+2]);
		        p_strm_porp->x = atoi(p_cmd->args[index+3]);
		        p_strm_porp->y = atoi(p_cmd->args[index+4]);
		        p_strm_porp->total_w = atoi(p_cmd->args[index+5]);
		        p_strm_porp->total_h = atoi(p_cmd->args[index+6]);
		        p_strm_porp->stride = atoi(p_cmd->args[index+7]);
		        
		    }

		    p_strm_porp->fps = 30;
		    sprintf(cmd_pra_name, "-fps%d", i);
		    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
		    if(index >= 0)
		        p_strm_porp->fps = atoi(p_cmd->args[index+1]);

		    p_strm_porp->format = STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;
		    sprintf(cmd_pra_name, "-format%d", i);
		    index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
		    if(index >= 0) {
		        int out_format = atoi(p_cmd->args[index+1]);
		        ar_always("out_format%d = %d", i, out_format);
		        switch(out_format) {
		        case 0:
		            p_strm_porp->format = STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;
		            break;
		        case 1:
		            p_strm_porp->format = STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;
		            break;
		        case 2:
		            p_strm_porp->format = STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;
		            break;
		        default:
		            ar_always("not supported raw");
		            p_strm_porp->format = STREAM_FORMAT_RAW_UNPACKED_12BIT_BGGR;
		            break;
		        }
		    }
		}
	}	
    // add element init to it
    camera_pipeline_init(p_camera, p_cmd);

    // if the dev have been opend, exit
    int opend = 0;
    ret = ar_hal_vin_dev_is_opend(p_camera->fd_vin_dev,
        p_camera->dev_id, &opend);
    if (ret < 0 || opend) {
        ar_err("the device have started ret=%d opend=%d", ret, opend);
        ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
        p_camera->fd_vin_dev = -1;
        return -1;
    }

    //first to set public prop
    STRU_VIN_DRIVER_PUBLIC_PROPERTY_T *p_pub_prop = &p_camera->public_prop;
    ar_hal_vin_get_public_property(p_camera->fd_vin_dev, p_pub_prop);

    if (p_camera->dev_info.device_type == DEVICE_TYPE_ISPIN) {
        p_pub_prop->cam_mode = SING_CAMERA_OFFLINE;
        p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DDR;
    } else {
        p_pub_prop->cam_mode = SING_CAMERA_NORMAL;
        p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DIRECT;
    }

    index = camera_find_cmd_pra_index_by_name("-cam_mode", p_cmd);
    if (index >= 0) {
        if (!strcmp("online", p_cmd->args[index+1])) {
            if (p_camera->dev_info.device_type == DEVICE_TYPE_ISPIN) {
                p_pub_prop->cam_mode = SING_CAMERA_OFFLINE;
                p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DDR;
            } else {
                p_pub_prop->cam_mode = SING_CAMERA_NORMAL;
                p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DIRECT;
            }
        } else if(!strcmp("offline", p_cmd->args[index+1])) {
            p_pub_prop->cam_mode = SING_CAMERA_OFFLINE;
            p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DDR;
        } else if(!strcmp("multi", p_cmd->args[index+1])) {
            p_pub_prop->cam_mode = MUILTI_CMAERA_MODE;
            p_pub_prop->vif_isp_mode = VIF2ISP_TRANSFER_DDR;
        }
    }

    p_pub_prop->isp_used = 1;

    // here we set the fre
    p_pub_prop->vif_fre_mod = 0;
    index = camera_find_cmd_pra_index_by_name("-vif_fre", p_cmd);
    if (index >= 0) {
        p_pub_prop->vif_fre_mod = 1;
        p_pub_prop->vif_fre_hz = atoi(p_cmd->args[index+1]);
    }

    p_pub_prop->isp_fre_mod = 0;
    index = camera_find_cmd_pra_index_by_name("-isp_fre", p_cmd);
    if (index >= 0) {
        p_pub_prop->isp_fre_mod = 1;
        p_pub_prop->isp_fre_hz = atoi(p_cmd->args[index+1]);
    }

    p_pub_prop->hdr_fre_mod = 0;
    index = camera_find_cmd_pra_index_by_name("-hdr_fre", p_cmd);
    if (index >= 0) {
        p_pub_prop->hdr_fre_mod = 1;
        p_pub_prop->hdr_fre_hz = atoi(p_cmd->args[index+1]);
    }

    ar_always("vif isp hdr ==> mode=(%d %d %d) frehz=(%d %d %d)",
        p_pub_prop->vif_fre_mod, p_pub_prop->isp_fre_mod,
        p_pub_prop->hdr_fre_mod, p_pub_prop->vif_fre_hz,
        p_pub_prop->isp_fre_hz, p_pub_prop->hdr_fre_hz);

    ar_hal_vin_set_public_property(p_camera->fd_vin_dev, p_pub_prop);

    // dump the info of the dev
    ar_hal_vin_get_dev_infor(p_camera->fd_vin_dev,
        p_camera->dev_id, &p_camera->dev_info);
    dump_dev_info(&p_camera->dev_info);

    // we set the pipe id with same dev id
    p_camera->pipe_id = p_camera->dev_id;
    // bind the dev id with pipe id
    ar_hal_vin_pipe_bind_to_dev(p_camera->fd_vin_dev,
        p_camera->pipe_id, p_camera->dev_id);

    // here we can open the pipe, set the pipeprop
    STRU_VIN_PIPE_PROPERTY_T *p_pipe_prop = &p_camera->pipe_prop;

    p_pipe_prop->sensor_hdr = 0;
    index = camera_find_cmd_pra_index_by_name("-hdr", p_cmd);
    if (index >= 0)
        p_pipe_prop->sensor_hdr = 1;

    index = camera_find_cmd_pra_index_by_name("-man_aec_update", p_cmd);
    if (index >= 0)
        p_pipe_prop->enable_man_aec_update = 1;

    index = camera_find_cmd_pra_index_by_name("-man_awb_update", p_cmd);
    if(index >= 0)
        p_pipe_prop->enable_man_awb_update = 1;

    index = camera_find_cmd_pra_index_by_name("-man_af_update", p_cmd);
    if (index >= 0)
        p_pipe_prop->enable_man_af_update = 1;

    p_pipe_prop->get_buffer_mode = 1;    // we must set to 1, hal use api mode, but not callback mode
    p_pipe_prop->trigger_mode = 0;       // always trigger mode disable
    index = camera_find_cmd_pra_index_by_name("-trigger", p_cmd);
    if (index >= 0)
        p_pipe_prop->trigger_mode = 1;

    p_pipe_prop->max_w[0] = 1920;
    p_pipe_prop->max_h[0] = 1080;
    p_pipe_prop->max_fps[0] = 25;
    index = camera_find_cmd_pra_index_by_name("-max_wh", p_cmd);
    if (index > 0) {
        p_pipe_prop->max_w[0] = atoi(p_cmd->args[index+1]);
        p_pipe_prop->max_h[0] = atoi(p_cmd->args[index+2]);
        p_pipe_prop->max_fps[0] = atoi(p_cmd->args[index+3]);
    }
    ar_always("sensor sel: (w h fps)==>(%d %d %f)", 
        p_pipe_prop->max_w[0], p_pipe_prop->max_h[0], p_pipe_prop->max_fps[0]);

    // set the man res
    int man_res=0;
    index = camera_find_cmd_pra_index_by_name("-man_res", p_cmd);
    if (index > 0) {
        man_res = (0x10 | atoi(p_cmd->args[index+1]));
    } else {
        // For vc stream, it is necessary that the user tells sensor driver
        // each virtual channel's resulotion and fps.
        // So set manual resolution here by force. If the user does not give a
        // man_res_idx, just set man_res_idx 0 by default;
        man_res = 0x10;
    }
    ar_always("man_res=0x%x", man_res);
    p_pipe_prop->man_res_mask = man_res;

    ar_hal_vin_open_pipe(p_camera->fd_vin_dev,
        p_camera->pipe_id, p_pipe_prop);

    // the pipe have opened,we can open the pipe fd
    p_camera->fd_pipe = ar_hal_vin_open_pipe_dev(p_camera->pipe_id);
    if (p_camera->fd_pipe < 0) {
        ar_err("open the pipe dev err");
        ar_hal_vin_close_pipe(p_camera->fd_vin_dev, p_camera->pipe_id);
        ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
        p_camera->fd_vin_dev = -1;
        return -1;
    }

    STRU_DEV_INFO_VAR_T *p_snr_var_pra = &p_camera->sensor_var_pra;
    int vc_num = -1;
    int snr_var_changed = 0;

    // get sensor var dev info
    p_snr_var_pra->res_index = (man_res ? (man_res&0xf) : -1);
    ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe,
        p_camera->pipe_id, p_snr_var_pra);

    index = camera_find_cmd_pra_index_by_name("-vc_cnt", p_cmd);
    if (index > 0) {
        vc_num = atoi(p_cmd->args[index+1]);
        if (vc_num > 0) {
            if (p_snr_var_pra->out_infor.link_mod == LINK_MODE_DVP) {
                p_snr_var_pra->out_infor.dvp_desc.vc_num = vc_num;
                index = camera_find_cmd_pra_index_by_name("-vc_id", p_cmd);
                if (index > 0) {
                    for (i = 0; i < vc_num; i++)
                        p_snr_var_pra->out_infor.dvp_desc.vc_id[i] = atoi(p_cmd->args[index+i+1]);
                }

                snr_var_changed = 1;
            }
        }
    }

    if (man_res) {
        index = camera_find_cmd_pra_index_by_name("-mipi_fre", p_cmd);
        if (index > 0) {
            int mipi_fre = atoi(p_cmd->args[index+1]);
            mipi_fre = mipi_fre/1000000;
            p_snr_var_pra->res_info.mipi_pix_clk = mipi_fre;
        }

        index = camera_find_cmd_pra_index_by_name("-dpcm", p_cmd);
        if (index > 0) {
            int dpcm_en = atoi(p_cmd->args[index+1]);
            int dpcm = atoi(p_cmd->args[index+2]);
            p_snr_var_pra->res_info.use_encoder_dpcm = dpcm_en;
            p_snr_var_pra->res_info.dpcm_out_bits = dpcm;
        }

        index = camera_find_cmd_pra_index_by_name("-skip_ration", p_cmd);
        if (index > 0) {
            p_snr_var_pra->res_info.aec_factor = atoi(p_cmd->args[index+1]);
            p_snr_var_pra->res_info.awb_factor = atoi(p_cmd->args[index+2]);
            p_snr_var_pra->res_info.ltm_factor = atoi(p_cmd->args[index+3]);
            ar_always("aec awb ltm raion==>(%d %d %d)",
                p_snr_var_pra->res_info.aec_factor,
                p_snr_var_pra->res_info.awb_factor,
                p_snr_var_pra->res_info.ltm_factor);
        }

        index = camera_find_cmd_pra_index_by_name("-settle_count", p_cmd);
        if (index > 0) {
            p_snr_var_pra->out_infor.mipi_desc.settle_count =
                atoi(p_cmd->args[index+1]);
            ar_always("settle count =%d",
                p_snr_var_pra->out_infor.mipi_desc.settle_count);
        }

        if (p_snr_var_pra->out_infor.link_mod == LINK_MODE_DVP) {
            p_snr_var_pra->res_info.vc_count = p_snr_var_pra->out_infor.dvp_desc.vc_num;

            for (i = 0; i < MAX_STREAM_SIZE; i++) {
                sprintf(cmd_pra_name, "-vc_res%d", i+1);
                index = camera_find_cmd_pra_index_by_name(cmd_pra_name, p_cmd);
                if (index > 0) {
                    p_snr_var_pra->res_info.vc_info[i].crop_en = 1;
                    p_snr_var_pra->res_info.vc_info[i].width = atoi(p_cmd->args[index+1]);
                    p_snr_var_pra->res_info.vc_info[i].heigh = atoi(p_cmd->args[index+2]);
                    p_snr_var_pra->res_info.vc_info[i].x = 0;
                    p_snr_var_pra->res_info.vc_info[i].y = 0;
                    p_snr_var_pra->res_info.vc_info[i].x_width = atoi(p_cmd->args[index+1]);
                    p_snr_var_pra->res_info.vc_info[i].y_heigh = atoi(p_cmd->args[index+2]);
                    p_snr_var_pra->res_info.vc_info[i].fps = atoi(p_cmd->args[index+3]);
                }
            }
        }

        snr_var_changed = 1;
    }
	
    if (snr_var_changed) {
        ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe,
            p_camera->pipe_id, p_snr_var_pra);
    }

    // here we register the 3a algo
    index = camera_find_cmd_pra_index_by_name("-aec", p_cmd);
    if (index > 0)
        ar_hal_vin_register_aec_algo_lib(p_camera->fd_pipe,
            p_camera->pipe_id, p_cmd->args[index+1]);

    index = camera_find_cmd_pra_index_by_name("-awb", p_cmd);
    if (index > 0)
        ar_hal_vin_register_awb_algo_lib(p_camera->fd_pipe,
            p_camera->pipe_id, p_cmd->args[index+1]);

    index = camera_find_cmd_pra_index_by_name("-af", p_cmd);
    if (index > 0)
        ar_hal_vin_register_af_algo_lib(p_camera->fd_pipe,
            p_camera->pipe_id, p_cmd->args[index+1]);

    index = camera_find_cmd_pra_index_by_name("-sensor", p_cmd);
    if (index >= 0) {
        ar_always("start the sensor %s", p_cmd->args[index+1]);
        STRU_SENSOR_OBJ_T *p_obj = get_sensor_obj(p_cmd->args[index+1]);
        p_obj->init(p_obj, p_camera->fd_vin_dev,
            p_camera->fd_pipe, p_camera->pipe_id, 0);
    }

    // start to creat stream
    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        STRU_STREAM_T *p_strm = &p_camera->stream[i];
        if (!p_strm->enable)
            continue;

        p_strm->stream_id = -1;
        ar_hal_vin_creat_stream(p_camera->fd_pipe, p_camera->pipe_id,
            &p_strm->stream_prop, &p_strm->stream_id);

        p_strm->fd_stream = ar_hal_vin_open_stream_dev(
            p_camera->pipe_id, p_strm->stream_id);
        ar_always("p_camera->stream[%d]=%d fd_stream=%d", i,
            p_strm->stream_id, p_strm->fd_stream);

        // prepare stream buffer for every stream
        for (int j = 0; j < MAX_STREAM_BUFER_COUNT; j++) {
            STRU_STREAM_BUFFER_INIFO_T *p_strm_buf =
                &p_strm->stream_buffer[j];
            ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe,
                p_camera->pipe_id, p_strm->stream_id, p_strm_buf);

            for (int k = 0; k < p_strm_buf->panel_count; k++) {
                // malloc later to instead with vb policy
                STRU_PANNEL_T *p_pannel = &p_strm_buf->pannel[k];
                AR_VOID *p_vaddr = NULL;
                AR_U64 phy_addr = 0;

                ar_hal_sys_mmz_alloc_cached(&phy_addr, &p_vaddr,
                    NULL, NULL, p_pannel->length);
                if (p_vaddr == NULL || phy_addr == 0) {
                    ar_err("ar_hal_sys_mmz_alloc_cached err");
                    return -1;
                }

                memset(p_vaddr, 0x5c, p_pannel->length);

                p_pannel->buffer_orign = p_pannel->buffer = p_vaddr;
                p_pannel->buffer = (AR_VOID *)CAM_ALIGNE_TO(
                    (AR_U64)p_pannel->buffer_orign, 256);
                p_pannel->buffer_phy_orign = (void *)phy_addr;
                p_pannel->buffer_phy_addr = (AR_VOID *)CAM_ALIGNE_TO(
                    (AR_U64)p_pannel->buffer_phy_orign, 256);

                ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",
                p_pannel->buffer_orign, p_pannel->buffer,
                p_pannel->buffer_phy_orign, p_pannel->buffer_phy_addr,
                p_pannel->length, p_pannel->used);
            }

            ar_hal_vin_return_stream_buffer(p_strm->fd_stream,
                p_camera->pipe_id, p_strm->stream_id, p_strm_buf);
        }
    }

    for (int i = 0; i < MAX_STREAM_SIZE; i++) {
        STRU_STREAM_T *p_strm = &p_camera->stream[i];

        if (p_strm->enable) {
            ar_hal_vin_start_stream(p_camera->fd_pipe,
                p_camera->pipe_id, p_strm->stream_id);

            // create thread for stream buffer process
            ar_os_thread_attr_t ctrl_attr = {0};
            ctrl_attr.name = "stream";
            ctrl_attr.stack_size = 16 * 1024;
            p_strm->stream_thread = ar_osal_thread_new(
                stream_loop_thread, p_strm, &ctrl_attr);
        }
    }

    // here we send a manula sof to update tuning pra
    STRU_AR_CAMERA_SOF_PRO_T sof_prop;
    sof_prop.sof_mode = 1;
    sof_prop.to_trigger = 1;
    ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);

    // then to trigger run
    STRU_AR_CAMERA_TRIGGER_PROP_T trigger_prop;
    trigger_prop.need_sync = 0;
    trigger_prop.trigger_mode = 1;
    trigger_prop.trigger_num = 0xffffffff;

    if (p_camera->pipe_prop.trigger_mode)
       ar_hal_vin_trigger(p_camera->fd_pipe, p_camera->pipe_id, &trigger_prop);

    sof_prop.sof_mode = 0;
    sof_prop.to_trigger = 0;
    ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);

    p_camera->frames = 0;
    index = camera_find_cmd_pra_index_by_name("-frames", p_cmd);
    if (index > 0) {
        p_camera->frames = atoi(p_cmd->args[index+1]);
        // start vsync event
        ar_hal_vin_create_event_client(p_camera->fd_pipe,
            p_camera->pipe_id, &p_camera->event_client_id);
        ar_always("event_client_id=%d", p_camera->event_client_id);

        ar_os_thread_attr_t ctrl_attr = {0};
        ctrl_attr.name = "stream";
        ctrl_attr.stack_size = 16 * 1024;
        p_camera->event_thread = ar_osal_thread_new(event_loop_thread,
            p_camera, &ctrl_attr);

        // after wait on event, we can register all the event, if user use event ,please register your intrest event, not all
        for(int i = NOTIFY_EVENT_SOF; i <= NOTIFY_EVENT_SOF; i++)
            ar_hal_vin_register_event(p_camera->fd_pipe,
                p_camera->pipe_id, p_camera->event_client_id, i);
    }

    if(need_server) {
        ar_signal_wait(p_camera->exit_signal);
        stop_camera(p_camera);

        //close vo dev
        if(need_vo(p_camera,p_cmd))
            ar_vo_dev_deinit(&p_camera->display_obj);

        ar_always("terminate the msg_loop");
        STRU_CAMERA_CMD_T *p_cmd = malloc(sizeof(STRU_CAMERA_CMD_T));
        memset(p_cmd, 0, sizeof(STRU_CAMERA_CMD_T));
        strcpy(&p_cmd->args[0][0], "--exit_msg_loop");
        mq_send(p_camera->mqid, (const char *)p_cmd,
            sizeof(STRU_CAMERA_CMD_T), 1);
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
    int index = camera_find_cmd_pra_index_by_name("-start_stream", p_cmd);

    if (index > 0)
    {
        camera_test_run_start_camera_stream(p_camera, p_cmd);
    }
	else 
	{
        index = camera_find_cmd_pra_index_by_name("-vc", p_cmd);
        if (index > 0)
		{
			int vc_type = 0;
			if (!strcmp(p_cmd->args[index + 1], "mipi"))
				vc_type = 1;
			else
				vc_type = 0;

			ar_always("start camera vc_type = %d", vc_type);
            camera_test_run_start_camera_vc(p_camera, p_cmd, vc_type);
			
		}
        else
            camera_test_run_start_camera_main(p_camera, p_cmd);
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
    //stop surfaec
    surface_deinit(p_camera);

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

static int stop_stream_single(STRU_CAMERA_T *p_camera,int index)
{
	ar_always("delete all the stream index %d client and wait stream loop exit",index);
	int i=index;
    if(p_camera->stream[i].enable)
    {
 	  ar_hal_vin_delete_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,p_camera->stream[i].client_id);
 	  ar_osal_thread_join(p_camera->stream[i].stream_thread);
    }

	surface_deinit_with_ch(p_camera,i);

	ar_always("stop stream");
	if(p_camera->stream[i].enable)
	{
		ar_hal_vin_stop_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
	}
	ar_always("delete stream buffer mem and delete stream buffer");
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
	if(p_camera->stream[i].enable)
	 {
		ar_always("delete stream %d",p_camera->stream[i].stream_id);
		ar_hal_vin_delete_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
		//close stream fd
		ar_hal_vin_close_stream_dev(p_camera->stream[i].fd_stream);
		p_camera->stream[i].enable=0;
	}
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
	   index=camera_find_cmd_pra_index_by_name("-stop_stream",p_cmd);
	   if(index >=0 && p_camera->need_server)
       {
           int stream_index=atoi(p_cmd->args[index+1]);
		   stop_stream_single(p_camera,stream_index);
       }
	}
	return 0;
}


__test_camera_case__(start_camera)=
{
   "--start_camera",
   camera_test_run_start_camera,
   {
      "start the camera with pra,the cmd will block the process until user to do stop/exit camera test",
	  "-server [name] -dev_id [id] -scaler1 [w] [h] [x] [y] [total_w] [total_h] [stride]",
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
	  "-scaler1_ext -hw_index [index]",
	  "-scaler2_ext -hw_index [index]",
	  "-raw_ext -hw_index [index]",
	  "-format1 [0:yuv420 1:yuv420semi 2:yuv422 3:cf50]",
	  "-format2 [0:yuv420 1:yuv420semi 2:yuv422 3:cf50]",
	  "-vif_fre [frehz] -isp_fre [frehz] -hdr_fre [hdr_fre] -mipi_fre [mipi_khz] -all_mpi_fre [all_mpi_fre_khz]",
	  "-man_res [res]",
	  "-dpcm [en] [bits]",
	  "-fps1 [fps] -fps2 [fps] -fps3 [fps] ",
	  "-surface_loop_times [ms]",
	  "-skip_ration [aec awb ltm]",
	  "-settle_count",
	  "-dng",
	  "-dpcm_nr3d [0/1]",
	  "-vfe_mode: [VFE_MODE_NULL:0 /VFE_MODE_CEVA_HDR:1 VFE_MODE_CEVA_LDC:2 VFE_MODE_AI_ISP_NR:3 VFE_MODE_DMA_CPY:4]",
	  "-vc: virtual channel: -scaler1/4/7... are raw yuv streams, -scaler2/5/8... are raw yuv scaler streams, -scaler3/6/9... are raw yuv sub scaler streams,",
	  "-vc_cnt: [0/1/2/4] virtual channel count",
	  "-vc_id: [vc_id_1] [vc_id_2] ... ...",
	  "-vc_res1: [w] [h] [fps] sensor resolution and fps of virtual channel 1",
	  "-vc_res2: [w] [h] [fps] sensor resolution and fps of virtual channel 2",
	  "-vc_res3: [w] [h] [fps] sensor resolution and fps of virtual channel 3",
	  "-vc_res4: [w] [h] [fps] sensor resolution and fps of virtual channel 4",
	  "-isp_in",
	  "-mipi_tx",
	  "-crop_en [0/1]",
	  "-crop_info [x,y,w,h]",
	  NULL,
   },
};
__test_camera_case__(stop_camera)=
{
	"--stop",
	camera_test_run_stop_camera,
	{
		"stop the camera ,-remote [remote binder server name]: this indicate the api run on remote app",
		"eg: camera --stop -remote camera_0 -exit",
		"-stop_stream [0:scaler1/1:scaler2/2:raw]",
		"-exit",
	},
};

