#include"camera.h"
#include "sensor.h"
#include "hal_sys.h"
#include "hal_vb.h"
#include "hal_vo.h"
#include "hal_dbglog.h"


enum{
    AR_DISPLAY_CURSOR_CFG=0X1024,
    AR_DISPLAY_CURSOR_DIS,
    AR_DISPLAY_CURSOR_EN,
    AR_DISPLAY_CURSOR_GET,
    AR_DISPLAY_CURSOR_SET,
    AR_DISPLAY_OVERLAY_SET,
    AR_DISPLAY_WAIT_FOR_VSYNC,
    AR_DISPLAY_SET_OUT_SIZE,
};

typedef struct
{
  int w;
  int h;
}display_out_size_t;


static AR_S32 give_bind_call_back(AR_S32 s32_dev_id, AR_S32 s32_chn_id, STRU_SYS_BIND_DEST *pst_bind_send)
{
   return 0;
}

static int  hal_vin_format_2_sys_format(int format)
{
   return format;
}

static AR_S32 get_frame_call_back(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_VOID *pv_data,void *priv)
{
   STRU_CAMERA_T *p_camera=(STRU_CAMERA_T *)priv;
   STRU_STREAM_T *p_stream=&p_camera->stream[s32_chn_id];
   //ar_always("get frame");
   STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
   int ret=ar_hal_vin_get_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream->client_id,\
	   p_stream_buffer,-1);
   	if(ret<0){
     		ar_err("err ar_hal_vin_get_stream_buffer,exit the loop thread ");
			free(p_stream_buffer);
			return -1;
	}
	STRU_SYS_VIDEO_FRAME_INFO *pstFrameInfo=(STRU_SYS_VIDEO_FRAME_INFO *)pv_data;

	pstFrameInfo->stVFrame.u64_phy_addr[0]=(AR_U64)p_stream_buffer->pannel[0].buffer_phy_addr;
	pstFrameInfo->stVFrame.u64_phy_addr[1]=(AR_U64)p_stream_buffer->pannel[1].buffer_phy_addr;
	pstFrameInfo->stVFrame.u64_phy_addr[2]=(AR_U64)p_stream_buffer->pannel[2].buffer_phy_addr;
	pstFrameInfo->stVFrame.u64_vir_addr[0]=(AR_U64)p_stream_buffer->pannel[0].buffer;
	pstFrameInfo->stVFrame.u64_vir_addr[1]=(AR_U64)p_stream_buffer->pannel[1].buffer;
	pstFrameInfo->stVFrame.u64_vir_addr[2]=(AR_U64)p_stream_buffer->pannel[2].buffer;
	pstFrameInfo->stVFrame.u32_stride[0]=p_stream_buffer->pannel[0].line_buffer_len;
	pstFrameInfo->stVFrame.u32_stride[1]=p_stream_buffer->pannel[1].line_buffer_len;
	pstFrameInfo->stVFrame.u32_stride[2]=p_stream_buffer->pannel[2].line_buffer_len;

	pstFrameInfo->stVFrame.u32_width=p_stream->stream_prop.width;
	pstFrameInfo->stVFrame.u32_height=p_stream->stream_prop.height;
	pstFrameInfo->stVFrame.e_pixel_format=hal_vin_format_2_sys_format(p_stream->stream_prop.format);
	pstFrameInfo->stVFrame.u64_PTS=p_stream_buffer->timestamps;
    pstFrameInfo->stVFrame.u64_private_data=(AR_U64)p_stream_buffer;
	pstFrameInfo->u32PoolId=AR_VB_INVALID_POOLID;
    return 0;
}
static AR_S32 release_frame_call_back(AR_S32 s32_dev_id, AR_S32 s32_chn_id, const AR_VOID *pv_data,void *priv)
{
	//ar_always("release frame");

	STRU_CAMERA_T *p_camera=(STRU_CAMERA_T *)priv;
	STRU_STREAM_T *p_stream=&p_camera->stream[s32_chn_id];
	STRU_SYS_VIDEO_FRAME_INFO *p_frame=(STRU_SYS_VIDEO_FRAME_INFO *)pv_data;
	STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer=(STRU_STREAM_BUFFER_INIFO_T *)p_frame->stVFrame.u64_private_data;
	ar_hal_vin_return_stream_buffer(p_stream->fd_stream,p_camera->pipe_id,p_stream->stream_id,p_stream_buffer);
	free(p_stream_buffer);
    return 0;
}


static AR_S32 sys_init(STRU_VB_CONFIG_S * st_vb_config)
{
    //STRU_VB_SUPPLEMENT_CONFIG_S st_supplement_conf = {0};
    AR_U32 s32_ret = AR_FAILURE;

    ar_hal_vb_exit();

    if (NULL == st_vb_config)
    {
        printf("input parameter is null, it is invaild!\n");
        return AR_FAILURE;
    }

    s32_ret = ar_hal_vb_set_config(st_vb_config);

    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_VB_SetConf failed!\n");
        return AR_FAILURE;
    }

    s32_ret = ar_hal_vb_init();

    if (AR_SUCCESS != s32_ret)
    {
        printf("AR_MPI_VB_Init failed!\n");
        return AR_FAILURE;
    }

    return AR_SUCCESS;
}

static int send_to_fw_display(STRU_SURFACE_OBJ_T *p_display,STRU_SYS_VIDEO_FRAME_INFO *p_frame)
{
#if 0
    STRU_AR_HAL_VO_DISP_BUF    buffer;
    int                        ret;
    static uint32_t            frame_id = 0;
    STRU_SYS_VIDEO_FRAME_INFO  *release_frame;

    buffer.frame_id =frame_id;
    buffer.format = AR_HAL_VO_FMT_YV12;
    buffer.frame_width = p_frame->stVFrame.u32_width;
    buffer.frame_height = p_frame->stVFrame.u32_height;
    buffer.roi.w = p_frame->stVFrame.u32_width;
    buffer.roi.h = p_frame->stVFrame.u32_height;
    buffer.roi.x = 0;
    buffer.roi.y = 0;
    buffer.luma_stride = p_frame->stVFrame.u32_stride[0];
    buffer.chroma_stride = p_frame->stVFrame.u32_stride[1];
    buffer.pannel_num = 3;
    for (int i=0; i<buffer.pannel_num; i++)
    {
        buffer.pannel[i].buffer = p_frame->stVFrame.u64_vir_addr[i];
        buffer.pannel[i].buffer_orign = p_frame->stVFrame.u64_vir_addr[i];
        buffer.pannel[i].buffer_pa = p_frame->stVFrame.u64_phy_addr[i];
        buffer.pannel[i].buffer_pa_orign =  p_frame->stVFrame.u64_phy_addr[i];
        buffer.pannel[i].length = 0;
        #if 0
        ar_always("pannel%d, buffer=%p, buffer_orign=%p, buffer_pa=%p, buffer_pa_orign=%p\r\n",
                    i,
                    buffer.pannel[i].buffer,
                    buffer.pannel[i].buffer_orign,
                    buffer.pannel[i].buffer_pa,
                    buffer.pannel[i].buffer_pa_orign
                    );
        #endif
    }
    buffer.interlace_filed_flag = 0;
    buffer.usr_data = (void *)p_frame;

//    ar_always("frame_id=%d, format=%d, frame_width=%d, frame_height=%d, luma_stride=%d, chroma_stride=%d\r\n",
//                 buffer.frame_id, buffer.format, buffer.frame_width,
//                 buffer.frame_height, buffer.luma_stride, buffer.chroma_stride);

//    ar_always("roi.w=%d, roi.h=%d, roi.x=%d, roi.y=%d\r\n",
//                 buffer.roi.w, buffer.roi.h, buffer.roi.x, buffer.roi.y);

//    ar_always("send buffer_%d %p", frame_id, p_frame);
    //ar_always("send buffer_%d", frame_id);
    frame_id++;
    ret = ar_hal_vb_user_add_by_frameinfo(p_frame, AR_VB_UID_VO);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_always("ref buffer_%d error %d", buffer.frame_id, ret);
    }

    ret = ar_hal_vo_chn_send_frame(p_display->chan_fd,
                                   p_display->e_layer_id,
                                   p_display->chan_id,
                                   &buffer,
                                   0);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    /* */
    memset(&buffer, 0, sizeof(STRU_AR_HAL_VO_DISP_BUF));
    //ar_always("get buffer!");
    ret = ar_hal_vo_chn_get_frame(p_display->chan_fd,
                                   p_display->e_layer_id,
                                   p_display->chan_id,
                                   &buffer,
                                   -1);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_always("can't get buffer");
        goto End;
    }

//    ar_always("release buffer_%d %p", buffer.frame_id, buffer.usr_data);
    //ar_always("release buffer_%d", buffer.frame_id);

    release_frame = (STRU_SYS_VIDEO_FRAME_INFO *)buffer.usr_data;
    ret = ar_hal_vb_user_sub_by_frameinfo(release_frame, AR_VB_UID_VO);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_always("unref buffer_%d error %d", buffer.frame_id, ret);
    }

    ret = ar_hal_vo_chn_release_frame(p_display->chan_fd,
                                  p_display->e_layer_id,
                                  p_display->chan_id,
                                  &buffer);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_always("can't get buffer");
        goto End;
    }

   End:
	   if (ret != AR_HAL_VO_SUCCESS)
		   ar_always("ret = 0x%x", ret);
    return ret;
#else
    return 0;
#endif
}

static int call_back(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_BOOL b_block, ENMU_SYS_DATA_TYPE e_data_type, AR_VOID *pv_data,AR_VOID *priv)
{
	STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)priv;
	STRU_SYS_VIDEO_FRAME_INFO *p_frame=(STRU_SYS_VIDEO_FRAME_INFO *)pv_data;

    #if 0
	v4ar_buffer_t buffer_disp;

	buffer_disp.buffer_type=V4AR_BUFFER_NORMA;
	buffer_disp.mem_malloc_type=V4AR_MEM_MALLOC_USR;
	buffer_disp.pannle_num=3;
	buffer_disp.mem.pannel[0].pa=(void *)p_frame->stVFrame.u64_phy_addr[0];
	buffer_disp.mem.pannel[1].pa=(void *)p_frame->stVFrame.u64_phy_addr[1];
	buffer_disp.mem.pannel[2].pa=(void *)p_frame->stVFrame.u64_phy_addr[2];
    //ar_always("send to display");
	c_ioctl(p_display->fd,VIDIOC_V4AR_DEV_QBUF, &buffer_disp,sizeof(buffer_disp));
	//dq the last display buffer
	c_ioctl(p_display->fd,VIDIOC_V4AR_DEV_DQBUF, &buffer_disp,sizeof(buffer_disp));
	return 0;
    #else
    return send_to_fw_display(p_display, p_frame);
    #endif
}

int ar_vo_surface_init_bind(STRU_SURFACE_OBJ_T *vo_obj,
                          int width,
                          int height,
                          int y_stride,
                          int uv_stride,
                          STRU_AR_HAL_VO_RECT *chan_pos)
{
    STRU_AR_HAL_VO_DEV_ATTR    dev_attr;
    STRU_AR_HAL_VO_LAYER_ATTR  layer_attr;
    STRU_AR_HAL_VO_CHN_ATTR    chan_attr;
    int                        ret;

    ar_hal_log_init();

    ar_always("dev_id = %d, layer_id = %d, chan_id = %d",
                   vo_obj->e_dev_id, vo_obj->e_layer_id, vo_obj->chan_id);
    ar_always("width = %d, height = %d, y_stride = %d, uv_stride = %d",
                   width, height, y_stride, uv_stride);

    /* init vo dev */
    vo_obj->dev_fd = ar_hal_vo_dev_open(vo_obj->e_dev_id);
    if (vo_obj->dev_fd < 0)
    {
        ret = HAL_ERR_VO_DEV_OPEN;
        goto End;
    }
    dev_attr.bg_color = 0x808080;
    dev_attr.e_interface = AR_HAL_VO_DEV_INTF_DVP;
    dev_attr.e_sub_interface = AR_HAL_VO_DEV_SUB_INTF_HDMI;
    dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_1080P60;
    dev_attr.bit_count_per_channel = 8;
    ret = ar_hal_vo_dev_set_attr(vo_obj->dev_fd,  &dev_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_dev_enable(vo_obj->dev_fd);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    /* init vo layer */
    vo_obj->layer_fd = ar_hal_vo_layer_open(vo_obj->e_layer_id);
    if (vo_obj->layer_fd < 0)
    {
        ret = HAL_ERR_VO_LAYER_OPEN;
        goto End;
    }
    layer_attr.width = width;
    layer_attr.height = height;
    layer_attr.luma_stride = y_stride;
    layer_attr.chroma_stride = uv_stride;
    layer_attr.format = AR_HAL_VO_FMT_YV12;
    layer_attr.fps = 30;
    ret = ar_hal_vo_layer_set_attr(vo_obj->layer_fd,  &layer_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_layer_enable(vo_obj->layer_fd);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    /* init vo channel */
    vo_obj->chan_fd = ar_hal_vo_chn_open(vo_obj->e_layer_id, vo_obj->chan_id);
    if (vo_obj->chan_fd < 0)
    {
        ret = HAL_ERR_VO_CHN_OPEN;
        goto End;
    }

    chan_attr.priority = 0;
    chan_attr.rect = *chan_pos;

    ret = ar_hal_vo_chn_set_attr(vo_obj->chan_fd, &chan_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_chn_enable(vo_obj->chan_fd);
    if (ret != AR_HAL_VO_SUCCESS)
       goto End;

   End:
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);

    return ret;
}

static int camera_surface_1_init(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
    int index=0;
    //STRU_SURFACE_OBJ_T *p_display=(STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[0]->obj;
	STRU_SURFACE_OBJ_T *p_display=malloc(sizeof(STRU_SURFACE_OBJ_T));
    int x=0;
    int y=0;
    int w=p_camera->stream[0].stream_prop.total_w;
    int h=p_camera->stream[0].stream_prop.total_h;
    STRU_AR_HAL_VO_RECT rect;
    int ret = AR_HAL_VO_SUCCESS;

    index=camera_find_cmd_pra_index_by_name("-surface1",p_cmd);
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

    p_display->e_dev_id = AR_HAL_VO_DEV_ID_HD0;
    p_display->e_layer_id = AR_HAL_VO_LAYER_ID_VIDEO_0;
    p_display->chan_id = 0;

    ret = ar_vo_surface_init_bind(p_display, p_camera->stream[0].stream_prop.total_w, p_camera->stream[0].stream_prop.total_h, -1, -1, &rect);


	STRU_SYS_BIND_RECEIVER reciver;

	reciver.u32_max_dev_cnt=1;
	reciver.u32_max_chn_cnt=1;
	reciver.e_mod_id=AR_SYS_ID_VO;
	reciver.call_back=call_back;
	reciver.pv_priv_data=p_display;

    ar_always("register receiver %d %d %d",reciver.e_mod_id,reciver.u32_max_dev_cnt,reciver.u32_max_chn_cnt);
	ar_hal_sys_bind_register_receiver(&reciver);
    return ret;
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
	index=camera_find_cmd_pra_index_by_name("-server",p_cmd);
	if(index>=0)
	{
		need_server=1;
	}
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
         //register the bind sender
		 ar_hal_sys_bind_init();

		 STRU_SYS_BIND_SENDER *p_bind=malloc(sizeof(STRU_SYS_BIND_SENDER));

		 p_bind->e_mod_id=AR_SYS_ID_VI;
		 p_bind->u32_max_dev_cnt=1;
		 p_bind->u32_max_chn_cnt=3;
		 p_bind->s32_flag=0;
		 p_bind->e_data_type=AR_SYS_DATA_VI_FRAME;
		 p_bind->give_bind_call_back=give_bind_call_back;
		 p_bind->get_frame_call_back=get_frame_call_back;
		 p_bind->release_frame_call_back=release_frame_call_back;
		 p_bind->pv_priv_data=p_camera;

		 ar_always("register sender %d %d %d",p_bind->e_mod_id,p_bind->u32_max_dev_cnt,p_bind->u32_max_chn_cnt);
		 ar_hal_sys_bind_register_sender(p_bind);

		 free(p_bind);

         STRU_VB_CONFIG_S st_vb_config;

		 memset(&st_vb_config, 0 , sizeof(st_vb_config));

		 st_vb_config.st_comm_pool[0].u64_blk_size = (2048 * 1080+8192)+(2048 * 1080/4+8192)+(2048 * 1080/4+8192);
		 st_vb_config.st_comm_pool[0].u32_blk_cnt = MAX_STREAM_BUFER_COUNT*2;

		 st_vb_config.st_comm_pool[1].u64_blk_size = (2048 * 1080*2+8192);
		 st_vb_config.st_comm_pool[1].u32_blk_cnt = MAX_STREAM_BUFER_COUNT;

		 st_vb_config.u32_max_pool_cnt=2;

		sys_init(&st_vb_config);

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

	   //set up a pool for stream

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

	camera_surface_1_init(p_camera,p_cmd);

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
		   ar_hal_vin_create_stream_client(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].client_id);
		   ar_always("p_camera->stream[%d]=%d fd_stream=%d client_id=%d",i,p_camera->stream[i].stream_id,p_camera->stream[i].fd_stream,p_camera->stream[i].client_id);
		   //prepare stream buffer for every stream
		   for(int j=0;j<MAX_STREAM_BUFER_COUNT;j++)
		   {
                ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
				AR_VB_BLK vb_blk=0;

				//for(int k=0;k<p_camera->stream[i].stream_buffer[j].panel_count;k++)
				{
				   //malloc later to instead with vb policy
				   vb_blk=ar_hal_vb_get_block_ex(AR_VB_INVALID_POOLID,p_camera->stream[i].stream_buffer[j].pannel[0].length,NULL,AR_VB_UID_VI);
				   ar_hal_vb_mmap_pool(ar_hal_vb_handle2poolid(vb_blk));
				   AR_U64 phy_addr=ar_hal_vb_handle2physaddr(vb_blk);
				   void *p_vaddr=NULL;
				   ar_hal_vb_get_blockviraddr(ar_hal_vb_handle2poolid(vb_blk),ar_hal_vb_handle2physaddr(vb_blk),&p_vaddr);

                   ar_always("vb_blk=%d phy_addr=%p  p_vaddr=%p",vb_blk,(void *)phy_addr,p_vaddr);
//yyyy
                   if(p_camera->stream[i].stream_buffer[j].panel_count>=1)
				   {
                       memset(p_vaddr,0,p_camera->stream[i].stream_buffer[j].pannel[0].length);

    				   p_camera->stream[i].stream_buffer[j].pannel[0].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[0].buffer=p_vaddr;
    				   p_camera->stream[i].stream_buffer[j].pannel[0].buffer=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[0].buffer_orign,256);
    				   p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_orign=(void *)phy_addr;
    				   p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_addr=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_orign,256);
    				   ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
    				   	p_camera->stream[i].stream_buffer[j].pannel[0].buffer_orign,p_camera->stream[i].stream_buffer[j].pannel[0].buffer,\
    				   	p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_addr,\
    				   	p_camera->stream[i].stream_buffer[j].pannel[0].length,\
    				   	p_camera->stream[i].stream_buffer[j].pannel[0].used);
                   }
//uuuuuu
				if(p_camera->stream[i].stream_buffer[j].panel_count>=2){

                     p_vaddr=p_camera->stream[i].stream_buffer[j].pannel[0].buffer+p_camera->stream[i].stream_buffer[j].pannel[0].length;
                     phy_addr=((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[0].buffer_phy_addr)+p_camera->stream[i].stream_buffer[j].pannel[0].length;

     				memset(p_vaddr,0,p_camera->stream[i].stream_buffer[j].pannel[1].length);


     				p_camera->stream[i].stream_buffer[j].pannel[1].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[1].buffer=p_vaddr;
     				p_camera->stream[i].stream_buffer[j].pannel[1].buffer=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[1].buffer_orign,256);
     				p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_orign=(void *)phy_addr;
     				p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_addr=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_orign,256);
     				ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
     				 p_camera->stream[i].stream_buffer[j].pannel[1].buffer_orign,p_camera->stream[i].stream_buffer[j].pannel[1].buffer,\
     				 p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_addr,\
     				 p_camera->stream[i].stream_buffer[j].pannel[1].length,\
     				 p_camera->stream[i].stream_buffer[j].pannel[1].used);
				}

////vvvv
				 if(p_camera->stream[i].stream_buffer[j].panel_count>=3){

      				 p_vaddr=p_camera->stream[i].stream_buffer[j].pannel[1].buffer+p_camera->stream[i].stream_buffer[j].pannel[1].length;
      				 phy_addr=((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[1].buffer_phy_addr)+p_camera->stream[i].stream_buffer[j].pannel[1].length;

      				 memset(p_vaddr,0,p_camera->stream[i].stream_buffer[j].pannel[2].length);

      				   p_camera->stream[i].stream_buffer[j].pannel[2].buffer_orign=p_camera->stream[i].stream_buffer[j].pannel[2].buffer=p_vaddr;
      				   p_camera->stream[i].stream_buffer[j].pannel[2].buffer=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[2].buffer_orign,256);
      				   p_camera->stream[i].stream_buffer[j].pannel[2].buffer_phy_orign=(void *)phy_addr;
      				   p_camera->stream[i].stream_buffer[j].pannel[2].buffer_phy_addr=(void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[i].stream_buffer[j].pannel[2].buffer_phy_orign,256);
      				   ar_always("buffer_orign=%p,buffer=%p,buffer_phy_orign=%p,buffer_phy_addr=%p len=0x%x used=0x%x",\
      				   	p_camera->stream[i].stream_buffer[j].pannel[2].buffer_orign,p_camera->stream[i].stream_buffer[j].pannel[2].buffer,\
      				   	p_camera->stream[i].stream_buffer[j].pannel[2].buffer_phy_orign,p_camera->stream[i].stream_buffer[j].pannel[2].buffer_phy_addr,\
      				   	p_camera->stream[i].stream_buffer[j].pannel[2].length,\
      				   	p_camera->stream[i].stream_buffer[j].pannel[2].used);
				   }
				}
				ar_hal_vin_return_stream_buffer(p_camera->stream[i].fd_stream,p_camera->pipe_id,\
					p_camera->stream[i].stream_id,&p_camera->stream[i].stream_buffer[j]);
		   }
       }
    }

    for(int i=0;i<MAX_STREAM_SIZE;i++)
    {
       if(p_camera->stream[i].enable)
       {
          ar_hal_vin_start_stream(p_camera->fd_pipe,p_camera->pipe_id,p_camera->stream[i].stream_id);
       }
    }

	//bind the vin to vo
	STRU_SYS_CHN_S pst_src_chn;
	pst_src_chn.s32_dev_id=0;
	pst_src_chn.s32_chn_id=0;
	pst_src_chn.e_mod_id=AR_SYS_ID_VI;

	STRU_SYS_CHN_S pst_dest_chn;

	pst_dest_chn.e_mod_id=AR_SYS_ID_VO;
	pst_dest_chn.s32_dev_id=0;
	pst_dest_chn.s32_chn_id=0;

    ar_hal_sys_bind(&pst_src_chn,&pst_dest_chn);

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

	if(need_server)
	{
    	ar_osal_thread_join(p_camera->msg_loop);
    	mq_close(p_camera->mqid);
	}
	return 0;
}
static int camera_test_vin_bind_vo(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd)
{
	camera_test_run_start_camera_main(p_camera,p_cmd);
	return 0;
}

__test_camera_case__(vin_bind_vo)=
{
   "--vin_bind_vo",
   camera_test_vin_bind_vo,
   {
      "start the camera with pra,the cmd will block the process until user to do stop/exit camera test",
	  "-server [name] -dev_id [id] -scaler1 [w] [h] [x] [y] [total_w] [total_h] [stride] -fps [fps ] ",
	  "-scaler2 [w] [h] [x] [y] [total_w] [total_h] [stride] -raw [w] [h] [x] [y] [total_w] [total_h] [stride] -hdr -cam_mode [online/offline/multi]",
	  "-scaler1_to [surface/buffer] [num]",
	  "-scaler2_to [surface/buffer] [num]",
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
	  NULL,
   },
};
