#include"camera.h"
#include <sys/stat.h>
#include "sensor.h"

#define AR_BIGPIC_NUM 200

#define AR_BIGPIC_TO_FILE_NUM 1
#define AR_BIGPIC_TO_FILE_RAW 1
#define AR_BIGPIC_TO_FILE_YUV_SMALL 1
#define AR_BIGPIC_TO_FILE_YUV_BIG 1

#define AR_BIGPIC_TO_DISPLAY 1

#define AR_BIGPIC_DISPLAY_IF_HDMI AR_FALSE
#define AR_BIGPIC_DISPLAY_IF_MIPI AR_TRUE

#define AR_BIGPIC_IMX307

#ifdef AR_BIGPIC_IMX307

#define AR_BIGPIC_DISPLAY_IF AR_BIGPIC_DISPLAY_IF_MIPI
#define AR_BIGPIC_DISPLAY_WIDTH 1920
#define AR_BIGPIC_DISPLAY_HEIGHT 1080
#define AR_BIGPIC_DISPLAY_FPS 30

#define AR_BIGPIC_PHY_DEV_ID 0
#define AR_BIGPIC_SIM_DEV_ID 3

#define AR_BIGPIC_PREVIEW_STREAM_WIDTH 1920
#define AR_BIGPIC_PREVIEW_STREAM_HEIGHT 1080
#define AR_BIGPIC_PREVIEW_STREAM_STRIDE 2048

#define AR_BIGPIC_RAW_STREAM_WIDTH 1920
#define AR_BIGPIC_RAW_STREAM_HEIGHT 1080
#define AR_BIGPIC_ISP_IN_STRIDE 3840

#define AR_BIGPIC_WIDTH 1856
#define AR_BIGPIC_HEIGHT 1080
#define AR_BIGPIC_ISP_OUT_STRIDE 2048

#define AR_BIGPIC_ISP_IN_WIDTH 992
#define AR_BIGPIC_ISP_IN_HEIGHT 1080
#define AR_BIGPIC_ISP_IN_OVERLAP_H 64
#define AR_BIGPIC_ISP_IN_OVERLAP_V 0
#define AR_BIGPIC_ISP_IN_OFFSET_H 864
#define AR_BIGPIC_ISP_IN_OFFSET_V 0

#define AR_BIGPIC_ISP_OUT_WIDTH 960
#define AR_BIGPIC_ISP_OUT_HEIGHT 1080
#define AR_BIGPIC_ISP_OUT_OVERLAP_H 32
#define AR_BIGPIC_ISP_OUT_OVERLAP_V 0
#define AR_BIGPIC_ISP_OUT_OFFSET_H 896
#define AR_BIGPIC_ISP_OUT_OFFSET_V 0

#else

#define AR_BIGPIC_DISPLAY_IF AR_BIGPIC_DISPLAY_IF_MIPI
#define AR_BIGPIC_DISPLAY_WIDTH 1920
#define AR_BIGPIC_DISPLAY_HEIGHT 1080
#define AR_BIGPIC_DISPLAY_FPS 30

#define AR_BIGPIC_PHY_DEV_ID 0
#define AR_BIGPIC_SIM_DEV_ID 3

#define AR_BIGPIC_PREVIEW_STREAM_WIDTH 1920
#define AR_BIGPIC_PREVIEW_STREAM_HEIGHT 1080
#define AR_BIGPIC_PREVIEW_STREAM_STRIDE 2048

#define AR_BIGPIC_RAW_STREAM_WIDTH 8064
#define AR_BIGPIC_RAW_STREAM_HEIGHT 6048
#define AR_BIGPIC_ISP_IN_STRIDE 16128

#define AR_BIGPIC_WIDTH 8000
#define AR_BIGPIC_HEIGHT 6048
#define AR_BIGPIC_ISP_OUT_STRIDE 8192

#define AR_BIGPIC_ISP_IN_WIDTH 4064
#define AR_BIGPIC_ISP_IN_HEIGHT 6048
#define AR_BIGPIC_ISP_IN_OVERLAP_H 64
#define AR_BIGPIC_ISP_IN_OVERLAP_V 0
#define AR_BIGPIC_ISP_IN_OFFSET_H 3936
#define AR_BIGPIC_ISP_IN_OFFSET_V 0

#define AR_BIGPIC_ISP_OUT_WIDTH 4032
#define AR_BIGPIC_ISP_OUT_HEIGHT 6048
#define AR_BIGPIC_ISP_OUT_OVERLAP_H 32
#define AR_BIGPIC_ISP_OUT_OVERLAP_V 0
#define AR_BIGPIC_ISP_OUT_OFFSET_H 3968
#define AR_BIGPIC_ISP_OUT_OFFSET_V 0

#endif

#define AR_BIGPIC_OFFSET_NUM_H 2
#define AR_BIGPIC_OFFSET_NUM_V 1
#define AR_BIGPIC_OFFSET_NUM (AR_BIGPIC_OFFSET_NUM_H*AR_BIGPIC_OFFSET_NUM_V)

#define AR_BIGPIC_TAG "bigpic"

#define AR_BIGPIC_STREAM_INDEX_PREVIEW 0
#define AR_BIGPIC_STREAM_INDEX_RAW 2

static STRU_ISP_SUB_MODULE_LSC_TUNING_T tuning_req_write_pra_lsc[AR_BIGPIC_OFFSET_NUM];

typedef struct
{
    STRU_AR_QUEUE_T *stream_buffer_pool;
    int fd_vin;
    int fd_pipe;
    int pipe_id;
    int stream_id;
    STRU_STREAM_BUFFER_INIFO_T stream_buffer_info[16];
    ar_os_thread_id_t dqbuffer_thread;
    int dqbuffer_stop;
    ar_signal_t dqbuffer_to_dq;
}SRTU_QDQ_OBJ_T;

static void* take_still_thread(void *arg);

static int mem_free(SRTU_PAD_FRAME_T *p_frame)
{
    STRU_STREAM_T *p_stream = (STRU_STREAM_T *)p_frame->priv[1];
    STRU_CAMERA_T *p_camera = p_stream->p_camera;
    STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer = p_frame->priv[0];

    static int count = 0;
    if(count >= 1000)
    {
        // ar_loop("get stream buffer %d stream_id = %d index = %d", p_stream_buffer->frame_id, p_stream->stream_id, p_stream_buffer->buffer_index);
        count = 0;
    }
    count++;

    if(p_camera->big_pic_mode)
    {
        for(int k=0; k<p_stream_buffer->panel_count; k++)
        {
            if(p_stream_buffer->pannel[k].buffer_orign)
            {
                ar_hal_sys_mmz_free(p_stream_buffer->pannel[k].buffer_phy_orign, p_stream_buffer->pannel[k].buffer_orign);
                p_stream_buffer->pannel[k].buffer_orign = NULL;
            }
        }

    }else{
        ar_hal_vin_return_stream_buffer(p_stream->fd_stream, p_camera->pipe_id, p_stream->stream_id, p_stream_buffer);
    }

    free(p_stream_buffer);

    return 0;
}

static SRTU_PAD_FRAME_T* buffer_sink_buffer_chain(STRU_ELMENT_T *p_element, STRU_PAD_T *p_pad, SRTU_PAD_FRAME_T *p_frame)
{
    // STRU_STREAM_T *p_stream = (STRU_STREAM_T *)p_frame->priv[1];
    // STRU_CAMERA_T *p_camera = p_stream->p_camera;
    // STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer = p_frame->priv[0];

    static int count = 0;
    if(count >= 1000)
    {
        // ar_loop("get stream buffer %d stream_id = %d index = %d", p_stream_buffer->frame_id, p_stream->stream_id, p_stream_buffer->buffer_index);
        count = 0;
    }
    count++;

    return p_frame;
}

static SRTU_PAD_FRAME_T* send_to_fw_display(STRU_SURFACE_OBJ_T *p_display, SRTU_PAD_FRAME_T *p_frame)
{
    int ret = 0;
    STRU_AR_HAL_VO_DISP_BUF buffer = {0};
    static uint32_t frame_id = 0;

    buffer.frame_id = frame_id;
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
    for(int i=0; i<p_frame->pannel_cout; i++)
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

    int y_len = buffer.luma_stride*p_frame->h;
    p_display->p_dump->dump(p_display->p_dump, \
                            buffer.pannel[0].buffer_pa, \
                            buffer.pannel[1].buffer_pa, \
                            buffer.pannel[2].buffer_pa, \
                            NULL, y_len, y_len/4, y_len/4, 0);

    ret = ar_hal_vo_chn_send_frame(p_display->chan_fd, &buffer, 0);
    if(ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("ret = 0x%x", ret);
        return NULL;
    }

    return buffer.usr_data;
}

static SRTU_PAD_FRAME_T* surface_sink_buffer_chain(STRU_ELMENT_T *p_element, STRU_PAD_T *p_pad, SRTU_PAD_FRAME_T *p_frame)
{
    // STRU_STREAM_T *p_stream = (STRU_STREAM_T *)p_frame->priv[1];
    // STRU_CAMERA_T *p_camera = p_stream->p_camera;
    // STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer = p_frame->priv[0];
    STRU_SURFACE_OBJ_T *p_display = (STRU_SURFACE_OBJ_T *)p_element->obj;

    static int count = 0;
    if(count >= 1000)
    {
        // ar_loop("get stream buffer %d stream_id = %d index = %d", p_stream_buffer->frame_id, p_stream->stream_id, p_stream_buffer->buffer_index);
        count = 0;
    }
    count++;

    return send_to_fw_display(p_display, p_frame);
}

static void* surface_thread(void *arg)
{
    STRU_AR_HAL_VO_DISP_BUF buffer = {0};

    STRU_SURFACE_OBJ_T *p_display = (STRU_SURFACE_OBJ_T *)arg;
    p_display->wait_times = 300;

    /* */
    while(!p_display->stop_flag)
    {
        int ret = ar_hal_vo_chn_get_frame(p_display->chan_fd, &buffer, p_display->wait_times);

        if(ret < 0)
        {
            if(ret != HAL_ERR_VO_CHN_TIMEOUT)
            {
                ar_err("err, exit the loop ret = %x(%x,%x) %x", ret, (ret&0x0fff0000)>>16, AR_GET_ERRID(ret), HAL_ERR_VO_CHN_TIMEOUT);
                ar_osal_thread_exit();

            }else
            {
                // ar_err("time out .... go to end");
                goto End;
            }
        }

#if 0
        ar_always("chan_%d get buffer frameid:%d (%p,%p,%p)", p_display->chan_id, buffer.frame_id,
                  buffer.pannel[0].buffer_pa, buffer.pannel[1].buffer_pa,
                  buffer.pannel[2].buffer_pa);
#endif
        ret = ar_hal_vo_chn_release_frame(p_display->chan_fd, &buffer);
        if(ret < 0)
        {
            if(ret != HAL_ERR_VO_CHN_TIMEOUT)
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

static int surface_init(STRU_CAMERA_T *p_camera, int index)
{
    int ret = 0;

    STRU_SURFACE_OBJ_T *p_display = (STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[index]->obj;

    STRU_AR_HAL_VO_RECT rect = {
        .x = 0,
        .y = 0,
        // .w = p_camera->stream[index].stream_prop.total_w,
        // .h = p_camera->stream[index].stream_prop.total_h,
        // NOTE: crash when bypass scaler
        .w = 1280, //AR_BIGPIC_DISPLAY_WIDTH,
        .h = 720, //AR_BIGPIC_DISPLAY_HEIGHT,
    };

    p_display->dev_fd = p_camera->display_obj.dev_fd;
    p_display->e_dev_id = p_camera->display_obj.e_dev_id;
    p_display->e_layer_id = p_camera->display_obj.e_layer_id;
    p_display->layer_fd = p_camera->display_obj.layer_fd;
    p_display->p_dump = p_camera->p_dump;

    p_display->chan_id = p_camera->dev_id*2+index;
    ret = ar_vo_surface_init(p_display, &rect);

    p_display->stop_flag = 0;

    ar_os_thread_attr_t surface_thread_ctrl_attr = {0};
    surface_thread_ctrl_attr.name = "surface_thread";
    surface_thread_ctrl_attr.stack_size = 16 * 1024;
    p_display->surface_thread = ar_osal_thread_new(surface_thread, p_display, &surface_thread_ctrl_attr);

    return ret;
}

static int surface_deinit(STRU_CAMERA_T *p_camera, int index)
{
    int ret = 0;
    STRU_SURFACE_OBJ_T *p_display = (STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[index]->obj;
    STRU_AR_HAL_VO_DISP_BUF buffer = {0};

    if(p_display->chan_fd >= 0)
    {
        ar_always("wait the surface thread exit");
        p_display->stop_flag = 1;
        ar_osal_thread_join(p_display->surface_thread);

        ar_hal_vo_chn_pause(p_display->chan_fd);

        ar_always("wait clean all the buffer");

        ar_hal_vo_chn_clear_frame(p_display->chan_fd, AR_TRUE);
        while(1)
        {
            memset(&buffer, 0, sizeof(buffer));
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

static void display_buffer(STRU_STREAM_T *p_stream, STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer)
{
    STRU_CAMERA_T *p_camera = p_stream->p_camera;

    SRTU_PAD_FRAME_T *p_frame = malloc(sizeof(SRTU_PAD_FRAME_T));
    memset(p_frame, 0, sizeof(SRTU_PAD_FRAME_T));
    p_frame->pannel_cout = p_stream_buffer->panel_count;
    if(p_camera->big_pic_mode)
    {
        p_frame->w = AR_BIGPIC_WIDTH;
        p_frame->h = AR_BIGPIC_HEIGHT;

    }else{
        p_frame->w = p_camera->stream[0].stream_prop.total_w;
        p_frame->h = p_camera->stream[0].stream_prop.total_h;
    }
    for(int i=0; i<p_frame->pannel_cout; i++)
    {
        p_frame->pannel[i].len = p_stream_buffer->pannel[i].length;
        p_frame->pannel[i].used = p_stream_buffer->pannel[i].used;
        p_frame->pannel[i].stride = p_stream_buffer->pannel[i].line_buffer_len;
        p_frame->pannel[i].pa = p_stream_buffer->pannel[i].buffer_phy_orign;
        p_frame->pannel[i].pa_orig = p_stream_buffer->pannel[i].buffer_phy_orign;
        p_frame->pannel[i].va = p_stream_buffer->pannel[i].buffer;
        p_frame->pannel[i].va_orig = p_stream_buffer->pannel[i].buffer_orign;
    }
    p_frame->priv[0] = p_stream_buffer;
    p_frame->priv[1] = p_stream;
    p_frame->mem_free = mem_free;

    if(p_stream_buffer->frame_id == 1)
    {
        ar_always("first push to next");
        ar_always("%p %p %p", p_frame->pannel[0].pa, p_frame->pannel[1].pa, p_frame->pannel[2].pa);
    }

    push_to_next(p_camera->p_element, p_stream->p_pad, p_frame);
}

static void* display_thread(void *arg)
{
    STRU_STREAM_T *p_stream = (STRU_STREAM_T *)arg;
    STRU_CAMERA_T *p_camera = p_stream->p_camera;

    int count = 0;
    int ret = 0;

    while(p_camera->use_display)
    {
        STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
        memset(p_stream_buffer, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
        ret = ar_hal_vin_get_stream_buffer(p_stream->fd_stream, p_camera->pipe_id, p_stream->stream_id, p_stream->client_id, p_stream_buffer, -1);

        if(count++ >= 1000)
        {
            ar_always("get client_id = %d stream buffer %d stream_id = %d buffer_index = %d", \
                      p_stream->client_id, p_stream_buffer->frame_id, \
                      p_stream->stream_id, p_stream_buffer->buffer_index);
            count = 0;
        }

        if(p_stream_buffer->buffer_index == CAM_ERR_EXIT_MSG)
        {
            ar_always("receive exit msg, exit the stream loop thread");
            break;
        }

        if(ret >= 0)
        {
            display_buffer(p_stream, p_stream_buffer);
        }
    }

    ar_osal_thread_exit();

    return NULL;
}

static int pipeline_init(STRU_CAMERA_T *p_camera)
{
    p_camera->p_element = creat_element("cam_src", p_camera);
    for(int i=0; i<MAX_STREAM_SIZE; i++)
    {
        p_camera->stream[i].p_pad = creat_pad("cam_src_pad", &p_camera->stream[i]);
        add_out_pad(p_camera->p_element, p_camera->stream[i].p_pad);
        p_camera->stream[i].p_pad->buffer_chain = NULL;

        STRU_SURFACE_OBJ_T *p_display = malloc(sizeof(STRU_SURFACE_OBJ_T));
        memset(p_display, 0, sizeof(STRU_SURFACE_OBJ_T));
        p_display->chan_fd = -1;
        p_camera->p_surface_element[i] = creat_element("surface_element", p_display);
        p_display->p_element = p_camera->p_surface_element[i];
        p_display->pad = creat_pad("surface_pad_0", NULL);
        add_in_pad(p_display->p_element, p_display->pad);
        p_display->pad->buffer_chain = surface_sink_buffer_chain;

        STRU_BUFFER_OBJ_T *p_buffer_obj = malloc(sizeof(STRU_BUFFER_OBJ_T));
        memset(p_buffer_obj, 0, sizeof(STRU_BUFFER_OBJ_T));
        p_camera->p_buffer_element[i] = creat_element("buffer_element", p_buffer_obj);
        p_buffer_obj->p_element = p_camera->p_buffer_element[i];
        p_buffer_obj->pad = creat_pad("buffer_pad_0", NULL);
        add_in_pad(p_buffer_obj->p_element, p_buffer_obj->pad);
        p_buffer_obj->pad->buffer_chain = buffer_sink_buffer_chain;
    }

    return 0;
}

static int pipeline_deinit(STRU_CAMERA_T *p_camera)
{
    for(int i=0; i<MAX_STREAM_SIZE; i++)
    {
        STRU_BUFFER_OBJ_T *p_buffer_obj = (STRU_BUFFER_OBJ_T *)p_camera->p_buffer_element[i]->obj;
        delete_pad(p_buffer_obj->pad);
        delete_element(p_camera->p_buffer_element[i]);
        p_camera->p_buffer_element[i] = NULL;
        free(p_buffer_obj);

        STRU_SURFACE_OBJ_T *p_display = (STRU_SURFACE_OBJ_T *)p_camera->p_surface_element[i]->obj;
        delete_pad(p_display->pad);
        delete_element(p_camera->p_surface_element[i]);
        p_camera->p_surface_element[i] = NULL;
        free(p_display);

        delete_pad(p_camera->stream[i].p_pad);
        p_camera->stream[i].p_pad = NULL;
    }
    delete_element(p_camera->p_element);
    p_camera->p_element = NULL;

    return 0;
}

static int creat_stream(STRU_CAMERA_T *p_camera, int index)
{
    if(p_camera->device_type == DEVICE_TYPE_NORMAL)
    {
        if(p_camera->stream[index].enable == 0)
        {
            p_camera->stream[index].enable = 1;
            p_camera->stream[index].p_camera = p_camera;
            if(p_camera->big_pic_mode)
            {
                p_camera->use_raw = 1;
                p_camera->stream[index].stream_prop.width = AR_BIGPIC_RAW_STREAM_WIDTH;
                p_camera->stream[index].stream_prop.height = AR_BIGPIC_RAW_STREAM_HEIGHT;
                p_camera->stream[index].stream_prop.x = 0;
                p_camera->stream[index].stream_prop.y = 0;
                p_camera->stream[index].stream_prop.total_w = AR_BIGPIC_RAW_STREAM_WIDTH;
                p_camera->stream[index].stream_prop.total_h = AR_BIGPIC_RAW_STREAM_HEIGHT;
                p_camera->stream[index].stream_prop.stride = AR_BIGPIC_ISP_IN_STRIDE;
                p_camera->stream[index].stream_prop.skip = 0;
                p_camera->stream[index].stream_prop.fps = 25;
                p_camera->stream[index].stream_prop.mode = 1<<STREAM_MODE_NORMAL;
                p_camera->stream[index].stream_prop.stream_type = STREAM_TYPE_RAW_BAYER;
                int bayer_format = 0;
                ar_hal_vin_get_raw_format(&p_camera->sensor_var_pra.out_infor, &bayer_format);
                ar_hal_vin_bayer_format_to_dng_format(bayer_format, (AR_S32 *)&p_camera->stream[index].stream_prop.format);

            }else{
                p_camera->use_raw = 0;
                p_camera->stream[index].stream_prop.width = AR_BIGPIC_PREVIEW_STREAM_WIDTH;
                p_camera->stream[index].stream_prop.height = AR_BIGPIC_PREVIEW_STREAM_HEIGHT;
                p_camera->stream[index].stream_prop.x = 0;
                p_camera->stream[index].stream_prop.y = 0;
                p_camera->stream[index].stream_prop.total_w = AR_BIGPIC_PREVIEW_STREAM_WIDTH;
                p_camera->stream[index].stream_prop.total_h = AR_BIGPIC_PREVIEW_STREAM_HEIGHT;
                p_camera->stream[index].stream_prop.stride = AR_BIGPIC_PREVIEW_STREAM_STRIDE;
                p_camera->stream[index].stream_prop.skip = 0;
                p_camera->stream[index].stream_prop.fps = 25;
                p_camera->stream[index].stream_prop.mode = 1<<STREAM_MODE_NORMAL;
                p_camera->stream[index].stream_prop.stream_type = STREAM_TYPE_PREVIEW;
                p_camera->stream[index].stream_prop.format = STREAM_FORMAT_YUV420_8BIT_Plannar;
            }

            // creat stream
            {
                p_camera->stream[index].stream_id = -1;
                ar_hal_vin_creat_stream(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->stream[index].stream_prop, &p_camera->stream[index].stream_id);
                p_camera->stream[index].fd_stream = ar_hal_vin_open_stream_dev(p_camera->pipe_id, p_camera->stream[index].stream_id);
                ar_always("p_camera->stream[%d] = %d fd_stream = %d", index, p_camera->stream[index].stream_id, p_camera->stream[index].fd_stream);

                // creat stream buffer
                for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
                {
                    ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, \
                                                   &p_camera->stream[index].stream_buffer[j]);

                    for(int k=0; k<p_camera->stream[index].stream_buffer[j].panel_count; k++)
                    {
                        AR_VOID *p_vaddr = NULL;
                        AR_U64 phy_addr = 0;
                        ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, p_camera->stream[index].stream_buffer[j].pannel[k].length);

                        if(p_vaddr == NULL || phy_addr == 0)
                        {
                            ar_err("ar_hal_sys_mmz_alloc err");
                            return -1;
                        }
                        memset(p_vaddr, 0, p_camera->stream[index].stream_buffer[j].pannel[k].length);

                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign = (void *)phy_addr;
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr = \
                            (void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, 256);
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign = p_vaddr;
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer = \
                            (void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign, 256);

                        ar_always("panel = %d, buffer_orign = %p, buffer = %p, buffer_phy_orign = %p, buffer_phy_addr = %p len = 0x%x used = 0x%x", \
                                  k, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].length, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].used);
                    }

                    ar_hal_vin_return_stream_buffer(p_camera->stream[index].fd_stream, \
                                                    p_camera->pipe_id, \
                                                    p_camera->stream[index].stream_id, \
                                                    &p_camera->stream[index].stream_buffer[j]);
                }
            }
        }

    }else if(p_camera->device_type == DEVICE_TYPE_ISPIN)
    {
        if(p_camera->stream[index].enable == 0)
        {
            p_camera->stream[index].enable = 1;
            p_camera->stream[index].p_camera = p_camera;
            p_camera->stream[index].stream_prop.width = AR_BIGPIC_ISP_OUT_WIDTH;
            p_camera->stream[index].stream_prop.height = AR_BIGPIC_ISP_OUT_HEIGHT;
            p_camera->stream[index].stream_prop.x = 0;
            p_camera->stream[index].stream_prop.y = 0;
            p_camera->stream[index].stream_prop.total_w = AR_BIGPIC_ISP_OUT_WIDTH;
            p_camera->stream[index].stream_prop.total_h = AR_BIGPIC_ISP_OUT_HEIGHT;
            p_camera->stream[index].stream_prop.stride = AR_BIGPIC_ISP_OUT_STRIDE;
            p_camera->stream[index].stream_prop.skip = 0;
            p_camera->stream[index].stream_prop.fps = 25;
            p_camera->stream[index].stream_prop.mode = 1<<STREAM_MODE_NORMAL;
            p_camera->stream[index].stream_prop.stream_type = STREAM_TYPE_SIM_PREVIEW;
            p_camera->stream[index].stream_prop.format = STREAM_FORMAT_YUV420_8BIT_Plannar;

            // creat stream
            {
                p_camera->stream[index].stream_id = -1;
                ar_hal_vin_creat_stream(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->stream[index].stream_prop, &p_camera->stream[index].stream_id);
                p_camera->stream[index].fd_stream = ar_hal_vin_open_stream_dev(p_camera->pipe_id, p_camera->stream[index].stream_id);
                ar_always("p_camera->stream[%d] = %d fd_stream = %d", index, p_camera->stream[index].stream_id, p_camera->stream[index].fd_stream);

                // creat stream buffer
                for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
                {
                    ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, \
                                                   &p_camera->stream[index].stream_buffer[j]);
                    ar_hal_vin_creat_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, \
                                                   &p_camera->stream[index].stream_buffer_right[j]);

                    for(int k=0; k<p_camera->stream[index].stream_buffer[j].panel_count; k++)
                    {
                        AR_VOID *p_vaddr = NULL;
                        AR_U64 phy_addr = 0;
                        if(k == 0)
                        {
                            ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT+256);
                            if(p_vaddr == NULL || phy_addr == 0)
                            {
                                ar_err("ar_hal_sys_mmz_alloc err");
                                return -1;
                            }
                            memset(p_vaddr, 0, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT+256);

                        }else
                        {
                            ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                            if(p_vaddr == NULL || phy_addr == 0)
                            {
                                ar_err("ar_hal_sys_mmz_alloc err");
                                return -1;
                            }
                            memset(p_vaddr, 0, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                        }

                        // left
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign = (void *)phy_addr;
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr = \
                            (void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, 256);
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign = p_vaddr;
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer = \
                            (void*)CAM_ALIGNE_TO((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign, 256);

                        ar_always("L: panel = %d, buffer_orign = %p, buffer = %p, buffer_phy_orign = %p, buffer_phy_addr = %p len = 0x%x used = 0x%x", \
                                  k, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].length, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].used);

                        // right
                        p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_phy_orign = \
                            p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign;
                        p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_phy_addr = \
                            p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr;
                        p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_orign = \
                            p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign;
                        p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer = \
                            p_camera->stream[index].stream_buffer[j].pannel[k].buffer;
                        if(k == 0)
                        {
                            p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_phy_addr += AR_BIGPIC_ISP_OUT_OFFSET_H;
                            p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer += AR_BIGPIC_ISP_OUT_OFFSET_H;

                        }else
                        {
                            p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_phy_addr += AR_BIGPIC_ISP_OUT_OFFSET_H/2;
                            p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer += AR_BIGPIC_ISP_OUT_OFFSET_H/2;
                        }

                        ar_always("R: panel = %d, buffer_orign = %p, buffer = %p, buffer_phy_orign = %p, buffer_phy_addr = %p len = 0x%x used = 0x%x", \
                                  k, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_addr, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].length, \
                                  p_camera->stream[index].stream_buffer[j].pannel[k].used);
                    }

                    ar_hal_vin_return_stream_buffer(p_camera->stream[index].fd_stream, p_camera->pipe_id, \
                                                    p_camera->stream[index].stream_id, &p_camera->stream[index].stream_buffer[j]);
                    ar_hal_vin_return_stream_buffer(p_camera->stream[index].fd_stream, p_camera->pipe_id, \
                                                    p_camera->stream[index].stream_id, &p_camera->stream[index].stream_buffer_right[j]);
                }
            }
        }
    }

    return 0;
}

static int destroy_stream(STRU_CAMERA_T *p_camera, int index)
{
    if(p_camera->device_type == DEVICE_TYPE_NORMAL)
    {
        if(p_camera->stream[index].enable == 1)
        {
            for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
            {
                for(int k=0; k<p_camera->stream[index].stream_buffer[j].panel_count; k++)
                {
                    if(p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign)
                    {
                        ar_hal_sys_mmz_free((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign);
                        ar_always("free p_camera->stream[%d].stream_buffer[%d].pannel[%d].buffer_orign=%p", index, j, k, p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign);
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign = NULL;
                    }
                }
                ar_always("delete stream buffer");
                ar_hal_vin_delete_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, &p_camera->stream[index].stream_buffer[j]);
            }

            ar_always("prepare deleted stream");
            ar_hal_vin_preprae_delete_stream(p_camera->fd_pipe, p_camera->pipe_id,p_camera->stream[index].stream_id);

            ar_always("delete stream %d", p_camera->stream[index].stream_id);
            ar_hal_vin_delete_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);
            //close stream fd
            ar_hal_vin_close_stream_dev(p_camera->stream[index].fd_stream);
            p_camera->stream[index].enable = 0;
        }

    }else if(p_camera->device_type == DEVICE_TYPE_ISPIN)
    {
        if(p_camera->stream[index].enable == 1)
        {
            for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
            {
                for(int k=0; k<p_camera->stream[index].stream_buffer[j].panel_count; k++)
                {
                    if(p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign)
                    {
                        ar_hal_sys_mmz_free((AR_U64)p_camera->stream[index].stream_buffer[j].pannel[k].buffer_phy_orign, p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign);
                        ar_always("free p_camera->stream[%d].stream_buffer[%d].pannel[%d].buffer_orign=%p", index, j, k, p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign);
                        p_camera->stream[index].stream_buffer[j].pannel[k].buffer_orign = NULL;
                        p_camera->stream[index].stream_buffer_right[j].pannel[k].buffer_orign = NULL;
                    }
                }
                ar_always("delete stream buffer");
                ar_hal_vin_delete_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, &p_camera->stream[index].stream_buffer[j]);
                ar_hal_vin_delete_stream_buffer(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, &p_camera->stream[index].stream_buffer_right[j]);
            }

            ar_always("prepare deleted stream");
            ar_hal_vin_preprae_delete_stream(p_camera->fd_pipe, p_camera->pipe_id,p_camera->stream[index].stream_id);

            ar_always("delete stream %d", p_camera->stream[index].stream_id);
            ar_hal_vin_delete_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);
            //close stream fd
            ar_hal_vin_close_stream_dev(p_camera->stream[index].fd_stream);
            p_camera->stream[index].enable = 0;
        }
    }

    return 0;
}

static int start_stream(STRU_CAMERA_T *p_camera, int index)
{
    if(p_camera->device_type == DEVICE_TYPE_NORMAL)
    {
        if(p_camera->stream[index].enable == 1)
        {
            if(p_camera->enable_display)
            {
                // init vo dev
                ar_vo_dev_init(&p_camera->display_obj, AR_BIGPIC_DISPLAY_WIDTH, AR_BIGPIC_DISPLAY_HEIGHT, -1, -1, AR_BIGPIC_DISPLAY_FPS, AR_BIGPIC_DISPLAY_IF, AR_FALSE);
                p_camera->display_obj.p_dump = p_camera->p_dump;
                STRU_ELMENT_T * p_dest_element = p_camera->p_surface_element[index];
                ar_always("scaler1_to link to p_surface_element %d", index);
                surface_init(p_camera, index);
                link_pad(p_camera->p_element, index, p_dest_element, 0);
            }

            ar_hal_vin_start_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);
            ar_hal_vin_create_stream_client(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, &p_camera->stream[index].client_id);

            if(p_camera->pipe_prop.trigger_mode)
            {
                // ar_hal_vin_aec_update(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->aec_out);

                // entry sof mode
                STRU_AR_CAMERA_SOF_PRO_T sof_prop;
                sof_prop.sof_mode = 1;
                sof_prop.to_trigger = 1;
                ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);
                // exit sof mode
                sof_prop.sof_mode = 0;
                sof_prop.to_trigger = 0;
                ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);

                // trigger stream
                STRU_AR_CAMERA_TRIGGER_PROP_T trigger_prop;
                trigger_prop.need_sync = 0;
                trigger_prop.trigger_mode = 1;
                trigger_prop.trigger_num = 0xffffffff;
                ar_hal_vin_trigger(p_camera->fd_pipe, p_camera->pipe_id, &trigger_prop);

            }else if(p_camera->tuning_req_pra_saved == 1)
            {
                p_camera->tuning_req_pra_saved = 0;

                // lock the tuning
                p_camera->tuning_req_pra->req_type = TUNING_REQ_LOCK_TUNING_PRA;
                ar_hal_vin_process_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);

                // modify tuning prop
                *p_camera->tuning_req_pra = p_camera->tuning_req_pra_prop;
                p_camera->tuning_req_pra->req_type = TUNING_REQ_SET_UPDATE_PROP;
                ar_hal_vin_process_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);

                // write the pra
                p_camera->tuning_req_pra->req_type = TUNING_REQ_WRITE_TUNING_PRA;
                p_camera->tuning_req_pra->req_pra.write_tuning_pra.tuning_type = TUNING_ISP_LSC;
                p_camera->tuning_req_pra->req_pra.write_tuning_pra.tuning.isp_sub_module_lsc_tuning = \
                    p_camera->tuning_req_pra_data.req_pra.read_tuning_pra.tuning.isp_sub_module_lsc_tuning;
                ar_hal_vin_process_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);

                // unlock the tuning
                p_camera->tuning_req_pra->req_type = TUNING_REQ_UNLOCK_TUNING_PRA;
                p_camera->tuning_req_pra->req_pra.unlock_tuning_pra.force_type = 2;
                ar_hal_vin_process_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);

                // ar_hal_vin_aec_update(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->aec_out);
                // ar_hal_vin_awb_update(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->awb_out);

                // entry sof mode
                STRU_AR_CAMERA_SOF_PRO_T sof_prop;
                sof_prop.sof_mode = 1;
                sof_prop.to_trigger = 1;
                ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);
                // exit sof mode
                sof_prop.sof_mode = 0;
                sof_prop.to_trigger = 0;
                ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);
            }


            if(p_camera->enable_display)
            {
                // create thread for stream buffer process
                ar_os_thread_attr_t display_thread_ctrl_attr = {0};
                display_thread_ctrl_attr.name = "display_thread";
                display_thread_ctrl_attr.stack_size = 16 * 1024;
                p_camera->use_display = 1;
                p_camera->stream[index].stream_thread = ar_osal_thread_new(display_thread, &p_camera->stream[index], &display_thread_ctrl_attr);
            }
        }

    }else if(p_camera->device_type == DEVICE_TYPE_ISPIN)
    {
        if(p_camera->stream[index].enable == 1)
        {
            if(AR_BIGPIC_TO_DISPLAY)
            {
                // init vo dev
                ar_vo_dev_init(&p_camera->display_obj, AR_BIGPIC_DISPLAY_WIDTH, AR_BIGPIC_DISPLAY_HEIGHT, -1, -1, AR_BIGPIC_DISPLAY_FPS, AR_BIGPIC_DISPLAY_IF, AR_FALSE);
                p_camera->display_obj.p_dump = p_camera->p_dump;
                // link stream
                STRU_ELMENT_T * p_dest_element = p_camera->p_surface_element[index];
                ar_always("scaler1_to link to p_surface_element %d", index);
                surface_init(p_camera, index);
                link_pad(p_camera->p_element, index, p_dest_element, 0);
            }

            // start stream
            ar_hal_vin_start_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);
            ar_hal_vin_create_stream_client(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, &p_camera->stream[index].client_id);

            // ar_hal_vin_aec_set_mode(p_camera->fd_pipe, p_camera->pipe_id, 7);
            // ar_hal_vin_awb_set_mode(p_camera->fd_pipe, p_camera->pipe_id, 12);
            // ar_hal_vin_aec_update(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->aec_out);
            // ar_hal_vin_awb_update(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->awb_out);

            // entry sof mode
            STRU_AR_CAMERA_SOF_PRO_T sof_prop;
            sof_prop.sof_mode = 1;
            sof_prop.to_trigger = 1;
            ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);
            // exit sof mode
            sof_prop.sof_mode = 0;
            sof_prop.to_trigger = 0;
            ar_hal_vin_vsync_update(p_camera->fd_pipe, p_camera->pipe_id, &sof_prop);
        }
    }

    return 0;
}

static int stop_stream(STRU_CAMERA_T *p_camera, int index)
{
    if(p_camera->device_type == DEVICE_TYPE_NORMAL)
    {
        if(p_camera->stream[index].enable == 1)
        {
            ar_hal_vin_delete_stream_client(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, p_camera->stream[index].client_id);

            if(p_camera->enable_display)
            {
                p_camera->use_display = 0;
                ar_osal_thread_join(p_camera->stream[index].stream_thread);
            }

            ar_hal_vin_stop_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);

            if(p_camera->enable_display)
            {
                surface_deinit(p_camera, index);
                ar_vo_dev_deinit(&p_camera->display_obj);
            }
        }

    }else if(p_camera->device_type == DEVICE_TYPE_ISPIN)
    {
        if(p_camera->stream[index].enable == 1)
        {
            // ar_hal_vin_aec_set_mode(p_camera->fd_pipe, p_camera->pipe_id, p_camera->basic_3a_info.aec_info.aec_mode);
            // ar_hal_vin_awb_set_mode(p_camera->fd_pipe, p_camera->pipe_id, p_camera->basic_3a_info.awb_info.awb_mode);

            ar_hal_vin_delete_stream_client(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id, p_camera->stream[index].client_id);
            ar_hal_vin_stop_stream(p_camera->fd_pipe, p_camera->pipe_id, p_camera->stream[index].stream_id);

            if(AR_BIGPIC_TO_DISPLAY)
            {
                surface_deinit(p_camera, index);
                ar_vo_dev_deinit(&p_camera->display_obj);
            }
        }
    }

    return 0;
}

static void* dqbuffer_thread(void *arg)
{
    SRTU_QDQ_OBJ_T *p_qdq_obj = (SRTU_QDQ_OBJ_T *)arg;

    while(1)
    {
        ar_signal_wait(p_qdq_obj->dqbuffer_to_dq);
        if(p_qdq_obj->dqbuffer_stop)
        {
            break;
        }
        STRU_STREAM_BUFFER_INIFO_T *p_info = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
        memset(p_info, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
        ar_hal_vin_dq_stream_buffer(p_qdq_obj->fd_pipe, p_qdq_obj->pipe_id, p_qdq_obj->stream_id, p_info, -1);
        p_qdq_obj->stream_buffer_pool->queue_insert(p_qdq_obj->stream_buffer_pool, p_info);
    }

    ar_osal_thread_exit();

    return 0;
}

static int open_camera(STRU_CAMERA_T *p_camera, int camera_id)
{
    int ret = 0;

    p_camera->dev_id = camera_id;

    ar_hal_vin_get_dev_infor(p_camera->fd_vin_dev, p_camera->dev_id, &p_camera->dev_info);
    STRU_SENSOR_DEV_INFO_T *p_info = &p_camera->dev_info;
    dump_dev_info(p_info);

    // if the dev have been opened, exit
    int opened = 0;
    ret = ar_hal_vin_dev_is_opend(p_camera->fd_vin_dev, p_camera->dev_id, &opened);
    if(ret < 0 || opened)
    {
        ar_err("the device have started ret = %d opened = %d", ret, opened);
        ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
        p_camera->fd_vin_dev = -1;
        return -1;
    }

    p_camera->device_type = p_camera->dev_info.device_type;
    pipeline_init(p_camera);
    p_camera->p_dump = creat_dump_server();
    p_camera->fd_mem = ar_hal_vin_open_dev_mem();

    return ret;
}

static int close_camera(STRU_CAMERA_T *p_camera)
{
    ar_hal_vin_close_dev_mem(p_camera->fd_mem);
    delete_dump_server(p_camera->p_dump);
    pipeline_deinit(p_camera);

    return 0;
}

static int creat_pipe(STRU_CAMERA_T *p_camera, int index)
{
    int ret = 0;
    int pra_group = 0;

    ar_hal_vin_get_public_property(p_camera->fd_vin_dev, &p_camera->public_prop);

    if(p_camera->dev_id == AR_BIGPIC_PHY_DEV_ID)
    {
        if(index == AR_BIGPIC_STREAM_INDEX_RAW)
        {
            p_camera->big_pic_mode = 1;
            p_camera->enable_display = 0;

            pra_group = 1;

        }else
        {
            p_camera->big_pic_mode = 0;
            p_camera->enable_display = 1;

            pra_group = 0;
        }

    }else if(p_camera->dev_id == AR_BIGPIC_SIM_DEV_ID)
    {
        if(index == AR_BIGPIC_STREAM_INDEX_PREVIEW)
        {
            p_camera->big_pic_mode = 1;
            p_camera->enable_display = AR_BIGPIC_TO_DISPLAY;

            pra_group = 2;

        }else
        {
            ar_err("do not support the stream index %d", index);
        }
    }

    if(pra_group == 1)
    {
        p_camera->public_prop.cam_mode = SING_CAMERA_OFFLINE;
        p_camera->public_prop.vif_isp_mode = VIF2ISP_TRANSFER_DDR;
        p_camera->public_prop.isp_used = 0;

        p_camera->pipe_prop.enable_man_aec_update = 1;
        p_camera->pipe_prop.enable_man_awb_update = 1;
        p_camera->pipe_prop.enable_man_af_update = 1;
        p_camera->pipe_prop.get_buffer_mode = 1; // we must set to 1, hal use api mode , but not callback mode
        p_camera->pipe_prop.trigger_mode = 1;
        p_camera->pipe_prop.max_w[0] = AR_BIGPIC_RAW_STREAM_WIDTH;
        p_camera->pipe_prop.max_h[0] = AR_BIGPIC_RAW_STREAM_HEIGHT;
        p_camera->pipe_prop.max_fps[0] = 25;

    }else if(pra_group == 0)
    {
        p_camera->public_prop.cam_mode = SING_CAMERA_OFFLINE;
        p_camera->public_prop.vif_isp_mode = VIF2ISP_TRANSFER_DDR;
        p_camera->public_prop.isp_used = 1;

        p_camera->pipe_prop.enable_man_aec_update = 1;
        p_camera->pipe_prop.enable_man_awb_update = 1;
        p_camera->pipe_prop.enable_man_af_update = 1;
        p_camera->pipe_prop.get_buffer_mode = 1; //we must set to 1, hal use api mode , but not callback mode
        p_camera->pipe_prop.trigger_mode = 0; //always trigger mode disable
        p_camera->pipe_prop.max_w[0] = AR_BIGPIC_PREVIEW_STREAM_WIDTH;
        p_camera->pipe_prop.max_h[0] = AR_BIGPIC_PREVIEW_STREAM_HEIGHT;
        p_camera->pipe_prop.max_fps[0] = 25;

    }else if(pra_group == 2)
    {
        p_camera->public_prop.cam_mode = SING_CAMERA_OFFLINE;
        p_camera->public_prop.vif_isp_mode = VIF2ISP_TRANSFER_DDR;
        p_camera->public_prop.isp_used = 1;

        p_camera->pipe_prop.sensor_hdr = 0;
        p_camera->pipe_prop.enable_man_aec_update = 1;
        p_camera->pipe_prop.enable_man_awb_update = 1;
        p_camera->pipe_prop.enable_man_af_update = 1;
        p_camera->pipe_prop.get_buffer_mode = 1; // we must set to 1,  hal use api mode , but not callback mode
        p_camera->pipe_prop.trigger_mode = 0; // always trigger mode disable
        p_camera->pipe_prop.man_res_mask = 0;
        p_camera->pipe_prop.vfe_mode = 0;
        p_camera->pipe_prop.max_w[0] = AR_BIGPIC_ISP_OUT_WIDTH;
        p_camera->pipe_prop.max_h[0] = AR_BIGPIC_ISP_OUT_HEIGHT;
        p_camera->pipe_prop.max_fps[0] = 25;
    }

    ar_always("sensor sel: (w h fps) == >(%d %d %f)", p_camera->pipe_prop.max_w[0], p_camera->pipe_prop.max_h[0], p_camera->pipe_prop.max_fps[0]);
    ar_hal_vin_set_public_property(p_camera->fd_vin_dev, &p_camera->public_prop);

    p_camera->pipe_id = p_camera->dev_id;
    ar_hal_vin_pipe_bind_to_dev(p_camera->fd_vin_dev, p_camera->pipe_id, p_camera->dev_id);
    ar_hal_vin_open_pipe(p_camera->fd_vin_dev, p_camera->pipe_id, &p_camera->pipe_prop);

    p_camera->fd_pipe = ar_hal_vin_open_pipe_dev(p_camera->pipe_id);
    if(p_camera->fd_pipe < 0)
    {
        ar_err("open the pipe dev err");
        ar_hal_vin_close_pipe(p_camera->fd_vin_dev, p_camera->pipe_id);
        ar_hal_vin_close_vin_dev(p_camera->fd_vin_dev);
        p_camera->fd_vin_dev = -1;
        return -1;
    }

    if(pra_group == 1)
    {
        p_camera->sensor_var_pra.res_index = 0;
        ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);
        p_camera->sensor_var_pra.out_infor.bit_width = 12;
        p_camera->sensor_var_pra.out_infor.bayer = BAYER_RGGB;
        p_camera->sensor_var_pra.res_info.width = AR_BIGPIC_RAW_STREAM_WIDTH;
        p_camera->sensor_var_pra.res_info.height = AR_BIGPIC_RAW_STREAM_HEIGHT;
        p_camera->sensor_var_pra.res_info.line_length = p_camera->sensor_var_pra.res_info.width+400;
        p_camera->sensor_var_pra.res_info.frame_length = p_camera->sensor_var_pra.res_info.height+400;
        p_camera->sensor_var_pra.res_info.force_dng = 1;
        p_camera->sensor_var_pra.res_info.x_start = 0;
        p_camera->sensor_var_pra.res_info.y_start = 0;
        p_camera->sensor_var_pra.res_info.x_width = p_camera->sensor_var_pra.res_info.width;
        p_camera->sensor_var_pra.res_info.y_height = p_camera->sensor_var_pra.res_info.height;
        ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);

    }else if(pra_group == 0)
    {
        p_camera->sensor_var_pra.res_index = 0;
        ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);
        p_camera->sensor_var_pra.out_infor.bit_width = 12;
        p_camera->sensor_var_pra.out_infor.bayer = BAYER_RGGB;
        p_camera->sensor_var_pra.res_info.width = AR_BIGPIC_PREVIEW_STREAM_WIDTH;
        p_camera->sensor_var_pra.res_info.height = AR_BIGPIC_PREVIEW_STREAM_HEIGHT;
        p_camera->sensor_var_pra.res_info.line_length = p_camera->sensor_var_pra.res_info.width+400;
        p_camera->sensor_var_pra.res_info.frame_length = p_camera->sensor_var_pra.res_info.height+400;
        p_camera->sensor_var_pra.res_info.force_dng = 1;
        p_camera->sensor_var_pra.res_info.x_start = 0;
        p_camera->sensor_var_pra.res_info.y_start = 0;
        p_camera->sensor_var_pra.res_info.x_width = p_camera->sensor_var_pra.res_info.width;
        p_camera->sensor_var_pra.res_info.y_height = p_camera->sensor_var_pra.res_info.height;
        ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);

    }else if(pra_group == 2)
    {
        p_camera->sensor_var_pra.res_index = 0;
        p_camera->sensor_var_pra.res_count = 1;
        ar_hal_vin_get_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);
        p_camera->sensor_var_pra.out_infor.bit_width = 12;
        p_camera->sensor_var_pra.out_infor.bayer = BAYER_RGGB;
        p_camera->sensor_var_pra.res_info.width = AR_BIGPIC_ISP_IN_WIDTH;
        p_camera->sensor_var_pra.res_info.height = AR_BIGPIC_ISP_IN_HEIGHT;
        p_camera->sensor_var_pra.res_info.force_dng = 1;
        p_camera->sensor_var_pra.res_info.x_start = 0;
        p_camera->sensor_var_pra.res_info.y_start = 0;
        p_camera->sensor_var_pra.res_info.x_width = p_camera->sensor_var_pra.res_info.width;
        p_camera->sensor_var_pra.res_info.y_height = p_camera->sensor_var_pra.res_info.height;
        ar_hal_vin_set_sensor_var_pra(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->sensor_var_pra);
    }

    ar_hal_vin_get_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);
    STRU_PIPE_SET_TUNING_REQUEST_T *tuning_req_p = p_camera->tuning_req_mem.remote_pa;
    AR_S32 tuning_req_offset = 0;
    p_camera->tuning_req_v = ar_hal_vin_map_phy_addr(p_camera->fd_mem, (void *)tuning_req_p, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T), &tuning_req_offset);
    p_camera->tuning_req_pra = (STRU_PIPE_SET_TUNING_REQUEST_T *)(p_camera->tuning_req_v+tuning_req_offset);

    return 0;
}

static int destroy_pipe(STRU_CAMERA_T *p_camera)
{
    ar_hal_vin_ummap_vaddr(p_camera->tuning_req_v, sizeof(STRU_PIPE_SET_TUNING_REQUEST_T));
    ar_hal_vin_release_tuning_req(p_camera->fd_pipe, p_camera->pipe_id, &p_camera->tuning_req_mem);

    ar_hal_vin_close_pipe_dev(p_camera->fd_pipe);
    ar_hal_vin_close_pipe(p_camera->fd_vin_dev, p_camera->pipe_id);
    ar_hal_vin_pipe_unbind_to_dev(p_camera->fd_vin_dev, p_camera->pipe_id, p_camera->dev_id);

    return 0;
}

static int take_still(STRU_CAMERA_T *p_camera_phy, STRU_CAMERA_T *p_camera_sim, SRTU_QDQ_OBJ_T *p_qdq_obj)
{
    int raw_stream_index = AR_BIGPIC_STREAM_INDEX_RAW;
    int yuv_stream_index = AR_BIGPIC_STREAM_INDEX_PREVIEW;

    int size = 0;
    ar_hal_vin_requst_stream_buffer(p_qdq_obj->fd_pipe, \
                                    p_qdq_obj->pipe_id, \
                                    p_qdq_obj->stream_id, \
                                    p_qdq_obj->stream_buffer_info, \
                                    &size);
    if(size <= 0)
    {
        ar_err("ispin stream has not started");
        return -1;
    }
    for(int i=0; i<size; i++)
    {
        STRU_STREAM_BUFFER_INIFO_T *p_info = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
        memset(p_info, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
        *p_info = p_qdq_obj->stream_buffer_info[i];
        p_qdq_obj->stream_buffer_pool->queue_insert(p_qdq_obj->stream_buffer_pool, p_info);
    }

    STRU_AR_CAMERA_MANUAL_CROP_PROP_T crop_prop = {0};
    crop_prop.x = 0;
    crop_prop.y = 0;
    crop_prop.width = AR_BIGPIC_ISP_OUT_WIDTH;
    crop_prop.height = AR_BIGPIC_ISP_OUT_HEIGHT;
    ar_hal_vin_set_manual_crop(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &crop_prop);

    // get raw buffer
    for(int raw_pic_frame_id=0; raw_pic_frame_id<AR_BIGPIC_NUM; raw_pic_frame_id++)
    {
        STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer_raw = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
        memset(p_stream_buffer_raw, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
        int ret = ar_hal_vin_get_stream_buffer(p_camera_phy->stream[raw_stream_index].fd_stream, \
                                               p_camera_phy->pipe_id, p_camera_phy->stream[raw_stream_index].stream_id, \
                                               p_camera_phy->stream[raw_stream_index].client_id, \
                                               p_stream_buffer_raw, -1);
        if(p_stream_buffer_raw->buffer_index == CAM_ERR_EXIT_MSG)
        {
            ar_err("receive exit msg");

        }else if(ret >= 0)
        {
        }

        // process raw buffer
        if(AR_BIGPIC_TO_FILE_RAW && raw_pic_frame_id<AR_BIGPIC_TO_FILE_NUM)
        {
            char raw_file_name[64] = {0};
            sprintf(raw_file_name, "/mnt/raw_save_big_%d.raw", raw_pic_frame_id);
            int raw_pic_file_fd = open(raw_file_name, O_RDWR|O_CREAT);
            if(raw_pic_file_fd < 0){
                ar_err("%s open file error\n", raw_file_name);
                return -1;
            }

            for(int i=0; i<AR_BIGPIC_RAW_STREAM_HEIGHT; i++)
            {
                write(raw_pic_file_fd, p_stream_buffer_raw->pannel[0].buffer+i*AR_BIGPIC_ISP_IN_STRIDE, AR_BIGPIC_RAW_STREAM_WIDTH*2);
            }

            close(raw_pic_file_fd);
            ar_err("%s save done\n", raw_file_name);
        }
        for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM;)
        {
            int small_pic_pixel_offset_h = 0;
            int small_pic_pixel_offset_v = 0;
            int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
            int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
            if(small_pic_num_h > 0)
            {
                small_pic_pixel_offset_h = (p_camera_sim->sensor_var_pra.res_info.width-AR_BIGPIC_ISP_IN_OVERLAP_H)*small_pic_num_h-AR_BIGPIC_ISP_IN_OVERLAP_H;
            }
            if(small_pic_num_v > 0)
            {
                small_pic_pixel_offset_v = (p_camera_sim->sensor_var_pra.res_info.height-AR_BIGPIC_ISP_IN_OVERLAP_V)*small_pic_num_v-AR_BIGPIC_ISP_IN_OVERLAP_V;
            }
            int small_pic_byte_offset_h = small_pic_pixel_offset_h*2;
            int small_pic_byte_offset_v = small_pic_pixel_offset_v*AR_BIGPIC_ISP_IN_STRIDE;
            int small_pic_byte_offset = small_pic_byte_offset_v+small_pic_byte_offset_h;

            if(small_pic_num_h > 0)
            {
                crop_prop.x = AR_BIGPIC_ISP_IN_OVERLAP_H-AR_BIGPIC_ISP_OUT_OVERLAP_H;
            }
            if(small_pic_num_v > 0)
            {
                crop_prop.y = AR_BIGPIC_ISP_IN_OVERLAP_V-AR_BIGPIC_ISP_OUT_OVERLAP_V;
            }
            ar_hal_vin_set_manual_crop(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &crop_prop);

            // lock the tuning
            p_camera_sim->tuning_req_pra->req_type = TUNING_REQ_LOCK_TUNING_PRA;
            // ar_hal_vin_process_tuning_req(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &p_camera_sim->tuning_req_mem);

            // modify tuning prop
            p_camera_sim->tuning_req_pra->req_type = TUNING_REQ_SET_UPDATE_PROP;
            memset(&p_camera_sim->tuning_req_pra->req_pra.update_prop, 0, sizeof(p_camera_sim->tuning_req_pra->req_pra.update_prop));
            p_camera_sim->tuning_req_pra->req_pra.update_prop.moudle = TUNING_ISP_LSC;
            p_camera_sim->tuning_req_pra->req_pra.update_prop.only_force_update = 1;
            p_camera_sim->tuning_req_pra->req_pra.update_prop.force_update = 1;
            p_camera_sim->tuning_req_pra->req_pra.update_prop.update_src = 1;
            p_camera_sim->tuning_req_pra->req_pra.update_prop.trigger_from = 0;
            // ar_hal_vin_process_tuning_req(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &p_camera_sim->tuning_req_mem);

            // write the pra
            p_camera_sim->tuning_req_pra->req_type = TUNING_REQ_WRITE_TUNING_PRA;
            p_camera_sim->tuning_req_pra->req_pra.write_tuning_pra.tuning_type = TUNING_ISP_LSC;
            p_camera_sim->tuning_req_pra->req_pra.write_tuning_pra.tuning.isp_sub_module_lsc_tuning = tuning_req_write_pra_lsc[small_pic_frame_id];
            // ar_hal_vin_process_tuning_req(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &p_camera_sim->tuning_req_mem);

            // unlock the tuning
            p_camera_sim->tuning_req_pra->req_type = TUNING_REQ_UNLOCK_TUNING_PRA;
            p_camera_sim->tuning_req_pra->req_pra.unlock_tuning_pra.force_type = 2;
            // ar_hal_vin_process_tuning_req(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &p_camera_sim->tuning_req_mem);

            // entry sof mode
            STRU_AR_CAMERA_SOF_PRO_T sof_prop;
            sof_prop.sof_mode = 1;
            sof_prop.to_trigger = 1;
            ar_hal_vin_vsync_update(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &sof_prop);
            // exit sof mode
            sof_prop.sof_mode = 0;
            sof_prop.to_trigger = 0;
            ar_hal_vin_vsync_update(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &sof_prop);

            // get queue size
            STRU_STREAM_BUFFER_INIFO_T *p_info = NULL;
            p_qdq_obj->stream_buffer_pool->queue_pop(p_qdq_obj->stream_buffer_pool, (void**)&p_info);
            if(!p_info)
            {
                ar_err("no buffer");
            }
            // insert to the stream queue right of the picture
            p_info->panel_count = 1;
            p_info->pannel[0].used = AR_BIGPIC_ISP_IN_STRIDE*AR_BIGPIC_ISP_IN_HEIGHT;
            p_info->pannel[0].length = p_info->pannel[0].used;
            p_info->pannel[0].offset = 0;
            p_info->pannel[0].line_buffer_len = AR_BIGPIC_ISP_IN_STRIDE;
            p_info->pannel[0].buffer_phy_addr = p_stream_buffer_raw->pannel[0].buffer_phy_addr+small_pic_byte_offset;
            p_info->pannel[0].buffer_phy_orign = p_stream_buffer_raw->pannel[0].buffer_phy_orign;
            p_info->pannel[0].buffer = p_stream_buffer_raw->pannel[0].buffer+small_pic_byte_offset;
            p_info->pannel[0].buffer_orign = p_stream_buffer_raw->pannel[0].buffer_orign;
            ar_hal_vin_q_stream_buffer(p_qdq_obj->fd_pipe, p_qdq_obj->pipe_id, p_qdq_obj->stream_id, p_info);
            free(p_info);
            ar_signal(p_qdq_obj->dqbuffer_to_dq);

            // get yuv buffer
            STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer_yuv = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
            memset(p_stream_buffer_yuv, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
            int ret = ar_hal_vin_get_stream_buffer(p_camera_sim->stream[yuv_stream_index].fd_stream, \
                                                   p_camera_sim->pipe_id, p_camera_sim->stream[yuv_stream_index].stream_id, \
                                                   p_camera_sim->stream[yuv_stream_index].client_id, \
                                                   p_stream_buffer_yuv, -1);
            if(p_stream_buffer_yuv->buffer_index == CAM_ERR_EXIT_MSG)
            {
                ar_err("receive exit msg");

            }else if(ret >= 0)
            {
            }

            int yuv_pic_pixel_offset_h = 0;
            int yuv_pic_pixel_offset_v = 0;
            if(small_pic_num_h > 0)
            {
                yuv_pic_pixel_offset_h = (AR_BIGPIC_ISP_OUT_WIDTH-AR_BIGPIC_ISP_OUT_OVERLAP_H)*small_pic_num_h-AR_BIGPIC_ISP_OUT_OVERLAP_H;
            }
            if(small_pic_num_v > 0)
            {
                yuv_pic_pixel_offset_v = (AR_BIGPIC_ISP_OUT_HEIGHT-AR_BIGPIC_ISP_OUT_OVERLAP_V)*small_pic_num_v-AR_BIGPIC_ISP_OUT_OVERLAP_V;
            }
            int yuv_pic_byte_offset_h = yuv_pic_pixel_offset_h;
            int yuv_pic_byte_offset_v = yuv_pic_pixel_offset_v*AR_BIGPIC_ISP_OUT_STRIDE;
            int yuv_pic_byte_offset_y = yuv_pic_byte_offset_v+yuv_pic_byte_offset_h;
            int yuv_pic_byte_offset_uv = yuv_pic_byte_offset_v/4+yuv_pic_byte_offset_h/2;

            AR_VOID *addr_pa_y = p_stream_buffer_yuv->pannel[0].buffer_phy_addr - yuv_pic_byte_offset_y;
            AR_VOID *addr_pa_u = p_stream_buffer_yuv->pannel[1].buffer_phy_addr - yuv_pic_byte_offset_uv;
            AR_VOID *addr_pa_v = p_stream_buffer_yuv->pannel[2].buffer_phy_addr - yuv_pic_byte_offset_uv;
            AR_VOID *addr_va_y = p_stream_buffer_yuv->pannel[0].buffer - yuv_pic_byte_offset_y;
            AR_VOID *addr_va_u = p_stream_buffer_yuv->pannel[1].buffer - yuv_pic_byte_offset_uv;
            AR_VOID *addr_va_v = p_stream_buffer_yuv->pannel[2].buffer - yuv_pic_byte_offset_uv;
            AR_VOID *addr_y = p_stream_buffer_yuv->pannel[0].buffer;
            AR_VOID *addr_u = p_stream_buffer_yuv->pannel[1].buffer;
            AR_VOID *addr_v = p_stream_buffer_yuv->pannel[2].buffer;

            if(AR_BIGPIC_TO_FILE_YUV_SMALL && raw_pic_frame_id<AR_BIGPIC_TO_FILE_NUM)
            {
                char small_file_name[64] = {0};
                sprintf(small_file_name, "/mnt/yuv_save_small_%d_%d.yuv", raw_pic_frame_id, small_pic_frame_id);
                int small_pic_file_fd = open(small_file_name, O_RDWR|O_CREAT);
                if(small_pic_file_fd < 0){
                    ar_err("%s open file error\n", small_file_name);
                    return -1;
                }

                for(int i=0; i<AR_BIGPIC_ISP_OUT_HEIGHT; i++)
                {
                    write(small_pic_file_fd, addr_y+i*AR_BIGPIC_ISP_OUT_STRIDE, AR_BIGPIC_ISP_OUT_WIDTH);
                }
                for(int i=0; i<AR_BIGPIC_ISP_OUT_HEIGHT/2; i++)
                {
                    write(small_pic_file_fd, addr_u+i*AR_BIGPIC_ISP_OUT_STRIDE/2, AR_BIGPIC_ISP_OUT_WIDTH/2);
                }
                for(int i=0; i<AR_BIGPIC_ISP_OUT_HEIGHT/2; i++)
                {
                    write(small_pic_file_fd, addr_v+i*AR_BIGPIC_ISP_OUT_STRIDE/2, AR_BIGPIC_ISP_OUT_WIDTH/2);
                }

                close(small_pic_file_fd);
                ar_err("%s save done\n", small_file_name);
            }

            small_pic_frame_id++;
            // process big yuv buffer
            if(small_pic_frame_id >= AR_BIGPIC_OFFSET_NUM)
            {
                // display big yuv
                if(AR_BIGPIC_TO_DISPLAY)
                {
                    STRU_STREAM_BUFFER_INIFO_T *p_stream_buffer_display = malloc(sizeof(STRU_STREAM_BUFFER_INIFO_T));
                    memset(p_stream_buffer_display, 0, sizeof(STRU_STREAM_BUFFER_INIFO_T));
                    memcpy(p_stream_buffer_display, p_stream_buffer_yuv, sizeof(STRU_STREAM_BUFFER_INIFO_T));
                    AR_VOID *p_vaddr_display = NULL;
                    AR_U64 phy_addr_display = 0;
                    ar_hal_sys_mmz_alloc(&phy_addr_display, &p_vaddr_display, NULL, NULL, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT+256);
                    p_stream_buffer_display->pannel[0].buffer_phy_orign = phy_addr_display;
                    p_stream_buffer_display->pannel[0].buffer_phy_addr = (void*)CAM_ALIGNE_TO((AR_U64)phy_addr_display, 256);
                    p_stream_buffer_display->pannel[0].buffer_orign = p_vaddr_display;
                    p_stream_buffer_display->pannel[0].buffer = (void*)CAM_ALIGNE_TO((AR_U64)p_vaddr_display, 256);
                    memcpy(p_stream_buffer_display->pannel[0].buffer, addr_va_y,  AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT+256);
                    p_stream_buffer_display->pannel[0].length = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT+256;
                    p_stream_buffer_display->pannel[0].used = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT;
                    p_stream_buffer_display->pannel[0].line_buffer_len = AR_BIGPIC_ISP_OUT_STRIDE;
                    p_vaddr_display = NULL;
                    phy_addr_display = 0;
                    ar_hal_sys_mmz_alloc(&phy_addr_display, &p_vaddr_display, NULL, NULL, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                    p_stream_buffer_display->pannel[1].buffer_phy_orign = phy_addr_display;
                    p_stream_buffer_display->pannel[1].buffer_phy_addr = (void*)CAM_ALIGNE_TO((AR_U64)phy_addr_display, 256);
                    p_stream_buffer_display->pannel[1].buffer_orign = p_vaddr_display;
                    p_stream_buffer_display->pannel[1].buffer = (void*)CAM_ALIGNE_TO((AR_U64)p_vaddr_display, 256);
                    memcpy(p_stream_buffer_display->pannel[1].buffer, addr_va_u,  AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                    p_stream_buffer_display->pannel[1].length = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256;
                    p_stream_buffer_display->pannel[1].used = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4;
                    p_stream_buffer_display->pannel[1].line_buffer_len = AR_BIGPIC_ISP_OUT_STRIDE/2;
                    p_vaddr_display = NULL;
                    phy_addr_display = 0;
                    ar_hal_sys_mmz_alloc(&phy_addr_display, &p_vaddr_display, NULL, NULL, AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                    p_stream_buffer_display->pannel[2].buffer_phy_orign = phy_addr_display;
                    p_stream_buffer_display->pannel[2].buffer_phy_addr = (void*)CAM_ALIGNE_TO((AR_U64)phy_addr_display, 256);
                    p_stream_buffer_display->pannel[2].buffer_orign = p_vaddr_display;
                    p_stream_buffer_display->pannel[2].buffer = (void*)CAM_ALIGNE_TO((AR_U64)p_vaddr_display, 256);
                    memcpy(p_stream_buffer_display->pannel[2].buffer, addr_va_v,  AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256);
                    p_stream_buffer_display->pannel[2].length = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4+256;
                    p_stream_buffer_display->pannel[2].used = AR_BIGPIC_ISP_OUT_STRIDE*AR_BIGPIC_HEIGHT/4;
                    p_stream_buffer_display->pannel[2].line_buffer_len = AR_BIGPIC_ISP_OUT_STRIDE/2;

                    display_buffer(&p_camera_sim->stream[yuv_stream_index], p_stream_buffer_display);
                }

                // save big yuv
                if(AR_BIGPIC_TO_FILE_YUV_BIG && raw_pic_frame_id<AR_BIGPIC_TO_FILE_NUM)
                {
                    char yuv_file_name[64] = {0};
                    sprintf(yuv_file_name, "/mnt/yuv_save_big_%d.yuv", raw_pic_frame_id);
                    int yuv_pic_file_fd = open(yuv_file_name, O_RDWR|O_CREAT);
                    if(yuv_pic_file_fd < 0){
                        ar_err("%s open file error\n", yuv_file_name);
                        return -1;
                    }

                    for(int i=0; i<AR_BIGPIC_HEIGHT; i++)
                    {
                        write(yuv_pic_file_fd, addr_va_y+i*AR_BIGPIC_ISP_OUT_STRIDE, AR_BIGPIC_WIDTH);
                    }
                    for(int i=0; i<AR_BIGPIC_HEIGHT/2; i++)
                    {
                        write(yuv_pic_file_fd,addr_va_u+i*AR_BIGPIC_ISP_OUT_STRIDE/2, AR_BIGPIC_WIDTH/2);
                    }
                    for(int i=0; i<AR_BIGPIC_HEIGHT/2; i++)
                    {
                        write(yuv_pic_file_fd, addr_va_v+i*AR_BIGPIC_ISP_OUT_STRIDE/2, AR_BIGPIC_WIDTH/2);
                    }

                    close(yuv_pic_file_fd);
                    ar_err("%s save done\n", yuv_file_name);
                }
            }

            ar_hal_vin_return_stream_buffer(p_camera_sim->stream[yuv_stream_index].fd_stream, \
                                            p_camera_sim->pipe_id, \
                                            p_camera_sim->stream[yuv_stream_index].stream_id, \
                                            p_stream_buffer_yuv);
            free(p_stream_buffer_yuv);
        }

        ar_hal_vin_return_stream_buffer(p_camera_phy->stream[raw_stream_index].fd_stream, \
                                        p_camera_phy->pipe_id, \
                                        p_camera_phy->stream[raw_stream_index].stream_id, \
                                        p_stream_buffer_raw);
        free(p_stream_buffer_raw);
    }

    // cancel manual crop
    crop_prop.x = 0;
    crop_prop.y = 0;
    crop_prop.width = 0;
    crop_prop.height = 0;
    ar_hal_vin_set_manual_crop(p_camera_sim->fd_pipe, p_camera_sim->pipe_id, &crop_prop);

    return 0;
}

static void* take_still_thread(void *arg)
{
    unsigned int prio = 0;
    STRU_CAMERA_CMD_T *p_cmd = malloc(sizeof(STRU_CAMERA_CMD_T));
    memset(p_cmd, 0, sizeof(STRU_CAMERA_CMD_T));

    STRU_CAMERA_T *p_camera_phy = (STRU_CAMERA_T *)arg;

    STRU_CAMERA_T *p_camera_sim = malloc(sizeof(STRU_CAMERA_T));
    memset(p_camera_sim, 0, sizeof(STRU_CAMERA_T));

    int raw_stream_index = AR_BIGPIC_STREAM_INDEX_RAW;
    int yuv_stream_index = AR_BIGPIC_STREAM_INDEX_PREVIEW;
    p_camera_sim->fd_vin_dev = p_camera_phy->fd_vin_dev;
    open_camera(p_camera_sim, AR_BIGPIC_SIM_DEV_ID);

    while(1)
    {
        // start preview
        creat_pipe(p_camera_phy, yuv_stream_index);
        creat_stream(p_camera_phy, yuv_stream_index);
        start_stream(p_camera_phy, yuv_stream_index);

        if(mq_receive(p_camera_phy->mqid, (char *)p_cmd, sizeof(STRU_CAMERA_CMD_T), &prio) == (mqd_t)-1)
        {
            ar_err("receive err");
            perror("mq_receive");
            break;

        }else
        {
            if(!strcmp(p_cmd->args[0], "--take_still"))
            {
                ar_err("start to take still");

                // lock 3a, tuning
                ar_hal_vin_aec_set_mode(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, 6);
                ar_hal_vin_awb_set_mode(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, 11);
                p_camera_phy->tuning_req_pra->req_type = TUNING_REQ_LOCK_TUNING_PRA;
                ar_hal_vin_process_tuning_req(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->tuning_req_mem);

                ar_hal_vin_get_3a_info(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->basic_3a_info);
                ar_hal_vin_get_aec_update(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->aec_out);
                ar_hal_vin_get_awb_update(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->awb_out);
                p_camera_sim->basic_3a_info = p_camera_phy->basic_3a_info;
                p_camera_sim->aec_out = p_camera_phy->aec_out;
                p_camera_sim->awb_out = p_camera_phy->awb_out;

                // backup tuning prop
                p_camera_phy->tuning_req_pra->req_type = TUNING_REQ_GET_UPDATE_PROP;
                p_camera_phy->tuning_req_pra->req_pra.update_prop.moudle = TUNING_ISP_LSC;
                ar_hal_vin_process_tuning_req(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->tuning_req_mem);
                p_camera_phy->tuning_req_pra_prop = *p_camera_phy->tuning_req_pra;
                // backup tuning data
                p_camera_phy->tuning_req_pra->req_type = TUNING_REQ_READ_TUNING_PRA;
                p_camera_phy->tuning_req_pra->req_pra.read_tuning_pra.tuning_type = TUNING_ISP_LSC;
                ar_hal_vin_process_tuning_req(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->tuning_req_mem);
                p_camera_phy->tuning_req_pra_data = *p_camera_phy->tuning_req_pra;
                // set flag
                p_camera_phy->tuning_req_pra_saved = 1;

                // unlock the tuning, 3a
                p_camera_phy->tuning_req_pra->req_type = TUNING_REQ_UNLOCK_TUNING_PRA;
                p_camera_phy->tuning_req_pra->req_pra.unlock_tuning_pra.force_type = 0;
                ar_hal_vin_process_tuning_req(p_camera_phy->fd_pipe, p_camera_phy->pipe_id, &p_camera_phy->tuning_req_mem);

                // stop preview
                stop_stream(p_camera_phy, yuv_stream_index);
                destroy_stream(p_camera_phy, yuv_stream_index);
                destroy_pipe(p_camera_phy);

                // start raw stream
                creat_pipe(p_camera_phy, raw_stream_index);
                creat_stream(p_camera_phy, raw_stream_index);
                start_stream(p_camera_phy, raw_stream_index);

                creat_pipe(p_camera_sim, yuv_stream_index);
                creat_stream(p_camera_sim, yuv_stream_index);

                SRTU_QDQ_OBJ_T *p_qdq_obj = malloc(sizeof(SRTU_QDQ_OBJ_T));
                memset(p_qdq_obj, 0, sizeof(SRTU_QDQ_OBJ_T));
                p_qdq_obj->fd_pipe = p_camera_sim->fd_pipe;
                p_qdq_obj->fd_vin = p_camera_sim->fd_vin_dev;
                p_qdq_obj->pipe_id = p_camera_sim->pipe_id;
                p_qdq_obj->stream_id = 0;
                p_qdq_obj->stream_buffer_pool = ar_creat_queue(16, "stream_buffer_pool");
                p_qdq_obj->dqbuffer_to_dq = ar_create_signal();
                // start to creat a thread to dqbuffer
                ar_os_thread_attr_t ctrl_attr = {0};
                ctrl_attr.name = "dqbuffer_thread";
                ctrl_attr.stack_size = 16 * 1024;
                p_qdq_obj->dqbuffer_thread = ar_osal_thread_new(dqbuffer_thread, p_qdq_obj, &ctrl_attr);

                // start yuv stream
                start_stream(p_camera_sim, yuv_stream_index);

                // take still
                take_still(p_camera_phy, p_camera_sim, p_qdq_obj);

                // stop yuv stream
                stop_stream(p_camera_sim, yuv_stream_index);

                p_qdq_obj->dqbuffer_stop = 1;
                ar_signal(p_qdq_obj->dqbuffer_to_dq);
                ar_osal_thread_join(p_qdq_obj->dqbuffer_thread);
                ar_delete_signal(p_qdq_obj->dqbuffer_to_dq);
                while(p_qdq_obj->stream_buffer_pool->get_queue_size(p_qdq_obj->stream_buffer_pool))
                {
                    STRU_STREAM_BUFFER_INIFO_T *p_info = NULL;
                    p_qdq_obj->stream_buffer_pool->queue_pop(p_qdq_obj->stream_buffer_pool, (void**)&p_info);
					if(p_info){
                       free(p_info);
					}
                }
                ar_delete_queue(&p_qdq_obj->stream_buffer_pool);
                free(p_qdq_obj);

                destroy_stream(p_camera_sim, yuv_stream_index);
                destroy_pipe(p_camera_sim);

                // stop raw stream
                stop_stream(p_camera_phy, raw_stream_index);
                destroy_stream(p_camera_phy, raw_stream_index);
                destroy_pipe(p_camera_phy);


            }else if(!strcmp(p_cmd->args[0], "--exit_demo"))
            {
                stop_stream(p_camera_phy, yuv_stream_index);
                destroy_stream(p_camera_phy, yuv_stream_index);
                destroy_pipe(p_camera_phy);

                ar_signal(p_camera_phy->exit_signal);
                break;
            }
        }
    }

    close_camera(p_camera_sim);

    free(p_camera_sim);

    free(p_cmd);

    ar_osal_thread_exit();

    return NULL;
}

static int to_take_still(STRU_CAMERA_T *p_camera, char *p_cmd)
{
    char server_name[64] = {0};
    char *server_tag = AR_BIGPIC_TAG;
    p_camera->dev_id = AR_BIGPIC_PHY_DEV_ID;
    sprintf(server_name, "/%s_%d", server_tag, p_camera->dev_id);
    ar_always("server name is %s", server_name);

    p_camera->mqid = mq_open(server_name, O_RDWR, 0666, NULL);
    if(p_camera->mqid == (mqd_t)-1)
    {
        ar_err("server %s has not started",  server_name);
        perror("open mq failed,  the error code is :");
        return -1;
    }

    STRU_CAMERA_CMD_T *p_take_cmd = malloc(sizeof(STRU_CAMERA_CMD_T));
    memset(p_take_cmd, 0, sizeof(STRU_CAMERA_CMD_T));
    strcpy(&p_take_cmd->args[0][0], p_cmd);
    mq_send(p_camera->mqid, (const char *)p_take_cmd, sizeof(STRU_CAMERA_CMD_T), 1);
    free(p_take_cmd);

    mq_close(p_camera->mqid);
}

static int api_test_big_pic(STRU_CAMERA_T *p_camera, STRU_CAMERA_CMD_T *p_cmd)
{
    int index_take = camera_find_cmd_pra_index_by_name("-take", p_cmd);
    int index_exit = camera_find_cmd_pra_index_by_name("-exit", p_cmd);
    if(index_take > 0 || index_exit > 0)
    {
        if(index_take > 0)
        {
            to_take_still(p_camera, "--take_still");
        }
        if(index_exit > 0)
        {
            to_take_still(p_camera, "--exit_demo");
        }

    }else
    {
        // open the vin dev
        AR_S32 fd_vin_dev = ar_hal_vin_open_vin_dev();
        if(fd_vin_dev < 0)
        {
            ar_err("open vin dev err");
            return -1;
        }

        p_camera->fd_vin_dev = fd_vin_dev;
        open_camera(p_camera, AR_BIGPIC_PHY_DEV_ID);

        char server_name[64] = {0};
        char *server_tag = AR_BIGPIC_TAG;
        sprintf(server_name, "/%s_%d", server_tag, p_camera->dev_id);
        ar_always("server name is %s", server_name);

        struct mq_attr attr = {0};
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(STRU_CAMERA_CMD_T);
        p_camera->mqid = mq_open(server_name, O_CREAT|O_RDWR, 0666, &attr);
        if(p_camera->mqid == (mqd_t)-1)
        {
            ar_err("start server failed, %d", p_camera->mqid);
            perror("open mq failed, the error code is :");
            return -1;
        }
        mq_getattr(p_camera->mqid, &attr);
        ar_always("mq_maxmsg = %d mq_msgsize = %d sizeof(STRU_CAMERA_CMD_T) = %d", attr.mq_maxmsg, attr.mq_msgsize, sizeof(STRU_CAMERA_CMD_T));

        // creat a loop to receive msg
        ar_os_thread_attr_t ctrl_attr = {0};
        ctrl_attr.name = "take_still_thread";
        ctrl_attr.stack_size = 16 * 1024;
        p_camera->msg_loop = ar_osal_thread_new(take_still_thread, p_camera, &ctrl_attr);

        p_camera->exit_signal = ar_create_signal();
        ar_signal_wait(p_camera->exit_signal);
        ar_osal_thread_join(p_camera->msg_loop);
        ar_delete_signal(p_camera->exit_signal);

        mq_close(p_camera->mqid);

        close_camera(p_camera);

        ar_hal_vin_close_vin_dev(fd_vin_dev);

        ar_always("exit start camera");
    }

    return 0;
}

__test_camera_case__(big_pic) =
{
    "--big_pic",
    api_test_big_pic,
    {
        "test demo for taking big picute, -remote [remote binder server name]: this indicate the api run on remote app,",
        "-dev_id [id]",
        "-take",
        "-exit",
        NULL,
    },
};


#ifdef AR_BIGPIC_IMX307
static STRU_ISP_SUB_MODULE_LSC_TUNING_T tuning_req_write_pra_lsc[AR_BIGPIC_OFFSET_NUM] =
{
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
    {
       1,//enable
       1,//interpolation_enable
       2,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             300,
          },
          /* aec trigger 1 */
          {
             330,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2700,
             2900,
          },
          /* awb trigger 1 */
          {
             3700,
             4300,
          },
          /* awb trigger 2 */
          {
             4700,
             5300,
          },
          /* awb trigger 3 */
          {
             6200,
             6800,
          },
       },
       /* isp_sub_module_lsc_pra */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
       },
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
    {
       1,//enable
       1,//interpolation_enable
       2,//count_ae
       4,//count_awb
       1,//trigger_mode : 0 gain trigger 1 lux trigger
       /* aec trigger */
       {
          /* aec trigger 0 */
          {
             0,
             300,
          },
          /* aec trigger 1 */
          {
             330,
             500,
          },
       },
       /* awb trigger */
       {
          /* awb trigger 0 */
          {
             2700,
             2900,
          },
          /* awb trigger 1 */
          {
             3700,
             4300,
          },
          /* awb trigger 2 */
          {
             4700,
             5300,
          },
          /* awb trigger 3 */
          {
             6200,
             6800,
          },
       },
       /* isp_sub_module_lsc_pra */
       {
          /* aec trigger 0 */
          {
             /* awb para 0 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0.3,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
          /* aec trigger 1 */
          {
             /* awb para 0 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.1992, 2.3433, 1.9121, 1.5298, 1.4131, 1.4233, 1.5537, 1.9023, 2.4263, 4.1050, 
                   3.1421, 2.0850, 1.6104, 1.3647, 1.2305, 1.2461, 1.3516, 1.6318, 2.1030, 3.1899, 
                   2.8296, 1.8955, 1.4897, 1.2407, 1.1377, 1.1323, 1.2539, 1.4883, 1.9395, 2.8428, 
                   2.5220, 1.7749, 1.3657, 1.1577, 1.0308, 1.0361, 1.1577, 1.3940, 1.7847, 2.6113, 
                   2.4526, 1.6831, 1.3301, 1.0957, 0.9961, 1.0024, 1.1006, 1.3301, 1.7251, 2.4331, 
                   2.4526, 1.6948, 1.3179, 1.1001, 0.9995, 0.9907, 1.1050, 1.3281, 1.7026, 2.4526, 
                   2.4761, 1.6992, 1.3330, 1.1157, 0.9922, 0.9971, 1.1094, 1.3228, 1.7305, 2.4478, 
                   2.6470, 1.7856, 1.4067, 1.1729, 1.0654, 1.0669, 1.2046, 1.4087, 1.8306, 2.6416, 
                   2.8472, 1.9229, 1.5029, 1.2622, 1.1470, 1.1445, 1.2500, 1.4946, 1.9546, 2.8765, 
                   3.3706, 2.1104, 1.6577, 1.3687, 1.2622, 1.2603, 1.4233, 1.6392, 2.1929, 3.2603, 
                   3.7437, 2.2642, 1.7798, 1.4980, 1.3921, 1.3823, 1.4985, 1.8442, 2.2637, 3.9390, 
                   3.0400, 1.9692, 1.5884, 1.3345, 1.2163, 1.2207, 1.3511, 1.5649, 2.0352, 2.9741, 
                   2.6152, 1.8320, 1.4404, 1.2212, 1.1313, 1.1309, 1.2285, 1.4644, 1.8540, 2.7119, 
                   2.3862, 1.7114, 1.3472, 1.1450, 1.0313, 1.0356, 1.1504, 1.3643, 1.7402, 2.4795, 
                   2.3403, 1.6436, 1.3008, 1.0933, 0.9976, 0.9878, 1.1060, 1.2998, 1.6680, 2.3350, 
                   2.2754, 1.6333, 1.2959, 1.0869, 0.9897, 0.9985, 1.0933, 1.3110, 1.6694, 2.3560, 
                   2.3408, 1.6431, 1.3013, 1.1055, 0.9888, 0.9937, 1.1084, 1.3154, 1.6753, 2.3394, 
                   2.4731, 1.7427, 1.3721, 1.1719, 1.0522, 1.0625, 1.1563, 1.3931, 1.7476, 2.5264, 
                   2.6890, 1.8286, 1.4668, 1.2324, 1.1372, 1.1460, 1.2505, 1.4727, 1.8833, 2.7197, 
                   3.1987, 2.0332, 1.6094, 1.3706, 1.2510, 1.2393, 1.3496, 1.6387, 2.0278, 3.1948, 
                   3.7358, 2.1665, 1.7969, 1.4893, 1.3394, 1.3931, 1.4990, 1.8223, 2.3691, 3.6719, 
                   2.8809, 1.9331, 1.5830, 1.3003, 1.2417, 1.2202, 1.3589, 1.5513, 2.0581, 2.8774, 
                   2.5972, 1.7915, 1.4209, 1.2251, 1.1172, 1.1396, 1.2104, 1.4858, 1.8511, 2.7061, 
                   2.3564, 1.6665, 1.3550, 1.1245, 1.0332, 1.0527, 1.1563, 1.3613, 1.7632, 2.4351, 
                   2.2788, 1.6338, 1.2773, 1.0908, 1.0044, 1.0063, 1.1016, 1.3271, 1.6738, 2.3555, 
                   2.2368, 1.6030, 1.3018, 1.0752, 1.0098, 0.9995, 1.1152, 1.3193, 1.6763, 2.3662, 
                   2.3076, 1.6309, 1.2998, 1.1104, 0.9790, 1.0112, 1.0996, 1.3164, 1.6553, 2.3730, 
                   2.4287, 1.7217, 1.3726, 1.1621, 1.0713, 1.0654, 1.1792, 1.3955, 1.7524, 2.5547, 
                   2.6240, 1.8091, 1.4648, 1.2363, 1.1387, 1.1479, 1.2310, 1.4722, 1.8638, 2.6611, 
                   3.0371, 2.0659, 1.5962, 1.3877, 1.2427, 1.2788, 1.3477, 1.6167, 2.0439, 3.1831, 
                },
             },
             /* awb para 1 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   3.8623, 2.3374, 1.7646, 1.5356, 1.3662, 1.4063, 1.5190, 1.8076, 2.3306, 3.7979, 
                   3.0981, 2.0151, 1.5991, 1.3687, 1.2480, 1.2549, 1.3589, 1.6108, 2.0391, 3.0825, 
                   2.6689, 1.8364, 1.4492, 1.2339, 1.1270, 1.1353, 1.2495, 1.4668, 1.8716, 2.6538, 
                   2.4233, 1.7310, 1.3574, 1.1680, 1.0444, 1.0435, 1.1567, 1.3818, 1.7476, 2.4971, 
                   2.3882, 1.6445, 1.3257, 1.1055, 0.9912, 1.0063, 1.1104, 1.3345, 1.6821, 2.3657, 
                   2.3521, 1.6289, 1.3159, 1.0845, 1.0044, 0.9961, 1.0957, 1.3281, 1.6689, 2.3042, 
                   2.3701, 1.6548, 1.3340, 1.1240, 1.0059, 1.0015, 1.1235, 1.3462, 1.6772, 2.4087, 
                   2.5737, 1.7217, 1.3936, 1.1807, 1.0552, 1.0674, 1.1748, 1.3945, 1.7603, 2.4639, 
                   2.7495, 1.8345, 1.4775, 1.2456, 1.1616, 1.1318, 1.2593, 1.4976, 1.8496, 2.7930, 
                   3.3662, 2.0322, 1.6572, 1.3882, 1.2690, 1.2773, 1.4038, 1.6206, 2.0801, 3.1636, 
                   3.7710, 2.2446, 1.7661, 1.5005, 1.3745, 1.3853, 1.5098, 1.8311, 2.2500, 3.8540, 
                   2.9961, 1.9717, 1.5732, 1.3252, 1.2246, 1.2305, 1.3296, 1.5869, 2.0078, 2.9424, 
                   2.6167, 1.8188, 1.4316, 1.2231, 1.1265, 1.1240, 1.2368, 1.4575, 1.8467, 2.7012, 
                   2.4092, 1.6934, 1.3467, 1.1431, 1.0317, 1.0381, 1.1348, 1.3672, 1.7104, 2.4346, 
                   2.3193, 1.6255, 1.3037, 1.0918, 0.9946, 0.9971, 1.1123, 1.3110, 1.6777, 2.3379, 
                   2.2983, 1.6143, 1.2935, 1.0786, 1.0010, 0.9941, 1.0942, 1.3066, 1.6558, 2.3184, 
                   2.3198, 1.6377, 1.3018, 1.1084, 0.9922, 0.9976, 1.1138, 1.3218, 1.6719, 2.3623, 
                   2.4609, 1.7061, 1.3745, 1.1489, 1.0605, 1.0596, 1.1650, 1.3818, 1.7456, 2.5054, 
                   2.6543, 1.8442, 1.4448, 1.2378, 1.1450, 1.1372, 1.2466, 1.4805, 1.8574, 2.7480, 
                   3.1655, 2.0215, 1.6367, 1.3633, 1.2671, 1.2534, 1.3643, 1.6113, 2.0620, 3.0894, 
                   3.7603, 2.1733, 1.7725, 1.4741, 1.3638, 1.3599, 1.4785, 1.7744, 2.2871, 3.6040, 
                   2.8887, 1.9268, 1.5713, 1.3184, 1.2144, 1.2246, 1.3398, 1.5654, 2.0156, 2.9600, 
                   2.6162, 1.7720, 1.4175, 1.2104, 1.1138, 1.1152, 1.2095, 1.4697, 1.8125, 2.7314, 
                   2.3384, 1.6890, 1.3140, 1.1489, 1.0317, 1.0410, 1.1577, 1.3467, 1.7358, 2.4141, 
                   2.3325, 1.6250, 1.3008, 1.1025, 0.9922, 0.9985, 1.0933, 1.3374, 1.6421, 2.4233, 
                   2.2217, 1.6152, 1.2661, 1.0898, 0.9961, 0.9951, 1.1050, 1.3042, 1.6479, 2.3296, 
                   2.2974, 1.6377, 1.2993, 1.1128, 0.9941, 1.0059, 1.0986, 1.3457, 1.6484, 2.4141, 
                   2.4243, 1.6885, 1.3564, 1.1553, 1.0566, 1.0605, 1.1729, 1.3799, 1.7515, 2.5034, 
                   2.6143, 1.8311, 1.4453, 1.2402, 1.1426, 1.1382, 1.2378, 1.4761, 1.8545, 2.6890, 
                   3.0015, 2.0210, 1.5894, 1.3853, 1.2480, 1.2739, 1.3687, 1.6567, 2.0400, 3.2544, 
                },
             },
             /* awb para 2 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0791, 2.3071, 1.8794, 1.5049, 1.3950, 1.3872, 1.5635, 1.8208, 2.4116, 4.0425, 
                   3.1987, 2.0659, 1.6440, 1.3647, 1.2593, 1.2534, 1.3638, 1.6455, 2.1035, 3.1821, 
                   2.7422, 1.8774, 1.4590, 1.2212, 1.1260, 1.1079, 1.2446, 1.4629, 1.9092, 2.8193, 
                   2.5249, 1.7588, 1.3857, 1.1650, 1.0464, 1.0562, 1.1606, 1.4067, 1.7891, 2.6040, 
                   2.4614, 1.6870, 1.3330, 1.1035, 1.0103, 0.9897, 1.1304, 1.3320, 1.7393, 2.4697, 
                   2.3799, 1.6724, 1.2988, 1.0830, 0.9878, 0.9883, 1.0928, 1.3125, 1.6870, 2.3818, 
                   2.4946, 1.7061, 1.3535, 1.1392, 1.0078, 1.0161, 1.1318, 1.3677, 1.7480, 2.5322, 
                   2.5493, 1.7900, 1.3804, 1.1616, 1.0654, 1.0586, 1.1782, 1.3984, 1.7974, 2.5869, 
                   2.8740, 1.8994, 1.5127, 1.2642, 1.1489, 1.1479, 1.2666, 1.5166, 1.9575, 2.8789, 
                   3.3486, 2.1646, 1.6880, 1.3999, 1.2983, 1.2808, 1.4082, 1.7100, 2.1621, 3.4326, 
                   3.7246, 2.2808, 1.7480, 1.5278, 1.3652, 1.3857, 1.5112, 1.8262, 2.2856, 3.7935, 
                   3.0601, 1.9766, 1.6162, 1.3364, 1.2378, 1.2466, 1.3389, 1.5986, 2.0483, 3.0044, 
                   2.6060, 1.8096, 1.4214, 1.2163, 1.1104, 1.1191, 1.2319, 1.4580, 1.8481, 2.6851, 
                   2.4604, 1.7153, 1.3770, 1.1509, 1.0483, 1.0522, 1.1533, 1.3892, 1.7456, 2.5171, 
                   2.3057, 1.6431, 1.3013, 1.0981, 0.9966, 1.0000, 1.1177, 1.3159, 1.6968, 2.3809, 
                   2.2690, 1.6211, 1.2920, 1.0757, 0.9829, 0.9976, 1.0894, 1.3145, 1.6499, 2.3667, 
                   2.3545, 1.6606, 1.3223, 1.1152, 1.0112, 1.0122, 1.1299, 1.3418, 1.7090, 2.4395, 
                   2.4199, 1.7129, 1.3667, 1.1455, 1.0503, 1.0537, 1.1636, 1.3906, 1.7383, 2.5439, 
                   2.6929, 1.8569, 1.4580, 1.2485, 1.1431, 1.1548, 1.2661, 1.4912, 1.9077, 2.7798, 
                   3.1938, 2.0444, 1.6431, 1.3701, 1.2788, 1.2588, 1.3706, 1.6611, 2.0649, 3.2065, 
                   3.7676, 2.1494, 1.7959, 1.4575, 1.3784, 1.3604, 1.4849, 1.8022, 2.2222, 3.5879, 
                   2.8955, 1.9805, 1.5562, 1.3379, 1.2290, 1.2344, 1.3447, 1.5879, 2.0161, 3.0039, 
                   2.5591, 1.7646, 1.4189, 1.1973, 1.1206, 1.1182, 1.2227, 1.4443, 1.8276, 2.6543, 
                   2.3511, 1.7134, 1.3359, 1.1577, 1.0361, 1.0483, 1.1626, 1.3647, 1.7427, 2.4536, 
                   2.2734, 1.6343, 1.2954, 1.0913, 1.0000, 0.9990, 1.1133, 1.3164, 1.6733, 2.3857, 
                   2.2153, 1.5972, 1.2783, 1.0781, 0.9951, 0.9912, 1.0894, 1.3018, 1.6392, 2.2881, 
                   2.3003, 1.6680, 1.3169, 1.1230, 1.0068, 1.0229, 1.1274, 1.3267, 1.7017, 2.4043, 
                   2.4150, 1.6978, 1.3374, 1.1655, 1.0420, 1.0562, 1.1563, 1.3745, 1.7373, 2.4756, 
                   2.6450, 1.8271, 1.4756, 1.2261, 1.1631, 1.1484, 1.2549, 1.4868, 1.8643, 2.7544, 
                   3.0923, 2.0610, 1.5869, 1.4268, 1.2461, 1.2861, 1.3608, 1.6191, 2.0957, 3.0503, 
                },
             },
             /* awb para 3 */
             {
                0,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                1,//lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                /* lsc table */
                {
                   4.0630, 2.3340, 1.8652, 1.5303, 1.3970, 1.4116, 1.5674, 1.8408, 2.4243, 4.0415, 
                   3.1001, 2.0317, 1.6133, 1.3320, 1.2319, 1.2134, 1.3462, 1.6001, 2.0596, 3.1025, 
                   2.7930, 1.8813, 1.4736, 1.2334, 1.1274, 1.1309, 1.2607, 1.4624, 1.9346, 2.7861, 
                   2.5044, 1.7524, 1.3711, 1.1455, 1.0391, 1.0278, 1.1489, 1.3799, 1.7539, 2.5664, 
                   2.4048, 1.6772, 1.3159, 1.0981, 0.9966, 0.9966, 1.1167, 1.3203, 1.7124, 2.4189, 
                   2.3779, 1.6885, 1.3145, 1.0908, 1.0049, 1.0068, 1.1006, 1.3442, 1.6987, 2.4556, 
                   2.4097, 1.6768, 1.3262, 1.1104, 1.0000, 0.9922, 1.1074, 1.3384, 1.6963, 2.4819, 
                   2.5972, 1.7876, 1.4038, 1.1777, 1.0659, 1.0864, 1.1782, 1.4307, 1.7979, 2.6611, 
                   2.7847, 1.9072, 1.4810, 1.2598, 1.1553, 1.1396, 1.2681, 1.5029, 1.9277, 2.9194, 
                   3.2676, 2.0850, 1.6733, 1.3750, 1.2549, 1.3003, 1.3608, 1.6890, 2.1235, 3.2163, 
                   3.7461, 2.2451, 1.7700, 1.4917, 1.3638, 1.3794, 1.4966, 1.8110, 2.3057, 3.7617, 
                   2.9478, 1.9429, 1.5674, 1.3184, 1.2100, 1.2153, 1.3237, 1.5664, 1.9902, 2.9966, 
                   2.6294, 1.8320, 1.4385, 1.2271, 1.1270, 1.1216, 1.2427, 1.4619, 1.8745, 2.6963, 
                   2.4106, 1.6885, 1.3511, 1.1445, 1.0220, 1.0488, 1.1318, 1.3696, 1.7217, 2.5049, 
                   2.2949, 1.6470, 1.2886, 1.0942, 0.9971, 0.9868, 1.1089, 1.3130, 1.6714, 2.3472, 
                   2.3154, 1.6304, 1.3125, 1.0850, 0.9985, 1.0093, 1.0923, 1.3149, 1.6738, 2.3672, 
                   2.2891, 1.6504, 1.2832, 1.1089, 0.9795, 0.9951, 1.1074, 1.3149, 1.6655, 2.3662, 
                   2.5103, 1.7207, 1.3818, 1.1655, 1.0566, 1.0625, 1.1694, 1.3955, 1.7598, 2.5669, 
                   2.6284, 1.8730, 1.4590, 1.2520, 1.1519, 1.1523, 1.2642, 1.4697, 1.8940, 2.7295, 
                   3.2412, 1.9746, 1.5967, 1.3696, 1.2051, 1.2505, 1.3379, 1.6270, 2.0308, 3.1763, 
                   3.6626, 2.2036, 1.7524, 1.4995, 1.3364, 1.3804, 1.4985, 1.7896, 2.3198, 3.6001, 
                   2.8735, 1.8955, 1.5376, 1.2983, 1.2036, 1.2080, 1.3091, 1.5269, 1.9756, 2.8428, 
                   2.5762, 1.7954, 1.4307, 1.2163, 1.1250, 1.1279, 1.2339, 1.4590, 1.8491, 2.6855, 
                   2.3506, 1.6670, 1.3330, 1.1313, 1.0288, 1.0454, 1.1436, 1.3599, 1.7251, 2.4292, 
                   2.2979, 1.5918, 1.2949, 1.0859, 0.9834, 0.9946, 1.1016, 1.2930, 1.6416, 2.3379, 
                   2.2417, 1.6235, 1.2881, 1.0845, 1.0112, 1.0059, 1.1143, 1.3213, 1.6694, 2.3569, 
                   2.2676, 1.6016, 1.2827, 1.0903, 0.9717, 1.0024, 1.0894, 1.3022, 1.6387, 2.3169, 
                   2.4136, 1.6929, 1.3677, 1.1523, 1.0640, 1.0674, 1.1826, 1.3862, 1.7612, 2.4966, 
                   2.6230, 1.8320, 1.4292, 1.2402, 1.1387, 1.1470, 1.2354, 1.4634, 1.8584, 2.6626, 
                   3.0088, 1.9487, 1.6074, 1.3398, 1.2295, 1.2476, 1.3828, 1.5625, 2.0811, 3.0229, 
                },
             },
          },
       },
    },
};
#else
static STRU_ISP_SUB_MODULE_LSC_TUNING_T tuning_req_write_pra_lsc[AR_BIGPIC_OFFSET_NUM] =
{
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
    {
        1,//enable
        1,//interpolation enable
        1,//ae count
        4,//awb count
        1,//tigger_mode : 0 gain ,1 lux
        {
            /*aec trigger 0*/
            {
                0,
                500,
            },
        },
        {
            /*awb trigger 1*/
            {
                2600,
                3000,
            },
            /*awb trigger 2*/
            {
                3750,
                4150,
            },
            /*awb trigger 3*/
            {
                4800,
                5200,
            },
            /*awb trigger 4*/
            {
                6300,
                6700,
            },
            /*awb trigger 5*/
            {
                0,
                0,
            },
            /*awb trigger 6*/
            {
                0,
                0,
            },
        },
        /* isp_sub_module_lsc_pra */
        {
            /* aec trigger 0 */
            {
                /* para 1 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        4.33278, 3.87821, 3.53859, 2.97114, 2.84090, 2.52895, 2.43345, 2.27490, 2.18855, 2.31656,
                        4.15697, 3.75798, 3.25063, 2.89361, 2.65129, 2.41345, 2.20927, 2.09191, 2.01283, 2.00296,
                        3.90908, 3.47613, 3.02047, 2.67813, 2.43912, 2.14735, 1.94465, 1.81731, 1.73860, 1.80199,
                        3.70264, 3.24349, 2.85428, 2.50691, 2.19744, 1.93043, 1.72925, 1.60705, 1.52777, 1.52914,
                        3.52747, 3.10599, 2.67589, 2.38354, 2.03563, 1.75174, 1.53331, 1.42074, 1.32800, 1.36386,
                        3.40109, 2.94062, 2.59504, 2.20490, 1.89018, 1.59635, 1.40724, 1.25699, 1.18744, 1.19790,
                        3.27455, 2.92563, 2.52029, 2.16133, 1.80140, 1.52688, 1.29671, 1.16384, 1.06144, 1.08916,
                        3.31890, 2.80849, 2.48392, 2.08642, 1.71399, 1.46774, 1.23137, 1.07647, 1.00276, 1.01643,
                        3.16726, 2.87265, 2.44070, 2.06871, 1.75339, 1.42891, 1.22701, 1.06813, 0.97399, 1.00688,
                        3.34604, 2.83323, 2.49934, 2.14841, 1.69869, 1.51345, 1.21931, 1.08835, 1.00014, 1.02676,
                        3.72439, 3.33942, 2.82967, 2.56187, 2.28058, 2.14119, 1.97030, 1.90640, 1.85131, 1.87286,
                        3.42379, 3.17945, 2.70656, 2.39488, 2.17750, 2.00946, 1.85839, 1.74630, 1.70438, 1.70319,
                        3.30723, 2.92402, 2.51713, 2.24074, 2.02937, 1.79886, 1.66177, 1.56433, 1.51852, 1.52441,
                        3.17301, 2.73740, 2.35767, 2.10256, 1.84188, 1.66952, 1.49939, 1.41081, 1.35966, 1.37744,
                        2.80731, 2.56269, 2.22727, 1.96200, 1.71201, 1.49560, 1.37382, 1.25951, 1.22804, 1.22046,
                        2.83818, 2.47632, 2.13909, 1.87542, 1.62328, 1.42700, 1.26413, 1.17877, 1.12057, 1.12299,
                        2.69205, 2.40836, 2.09695, 1.78929, 1.54541, 1.34768, 1.19877, 1.10255, 1.05355, 1.05156,
                        2.63156, 2.35481, 2.04066, 1.74504, 1.50283, 1.30871, 1.15595, 1.04996, 0.99167, 1.00125,
                        2.59835, 2.39509, 2.02588, 1.75000, 1.51305, 1.29014, 1.15272, 1.04227, 0.99242, 0.99356,
                        2.67779, 2.30612, 2.07168, 1.71546, 1.51343, 1.29778, 1.16300, 1.04632, 0.99611, 1.00100,
                        2.93402, 2.66260, 2.48880, 2.18773, 2.08223, 1.88054, 1.83127, 1.69818, 1.72131, 1.67510,
                        2.80117, 2.67953, 2.38069, 2.17071, 1.97286, 1.88898, 1.74052, 1.66894, 1.60717, 1.63521,
                        2.69578, 2.53656, 2.21816, 2.01938, 1.86486, 1.67489, 1.56521, 1.47010, 1.45770, 1.43340,
                        2.61088, 2.35289, 2.13536, 1.91011, 1.72337, 1.56974, 1.44204, 1.34851, 1.32014, 1.32559,
                        2.45565, 2.31912, 1.97781, 1.83761, 1.59918, 1.44399, 1.31994, 1.22383, 1.19338, 1.18472,
                        2.47898, 2.18073, 1.98383, 1.74171, 1.53176, 1.36795, 1.22424, 1.13139, 1.11594, 1.07654,
                        2.33735, 2.18444, 1.88600, 1.69916, 1.46380, 1.30897, 1.16856, 1.09119, 1.03797, 1.07383,
                        2.35028, 2.13149, 1.89119, 1.65120, 1.44807, 1.27635, 1.14685, 1.03000, 1.00770, 0.96080,
                        2.36710, 2.13062, 1.88050, 1.64557, 1.44311, 1.26623, 1.12524, 1.04019, 0.98769, 1.02326,
                        2.37680, 2.09119, 1.90704, 1.62727, 1.45516, 1.29717, 1.14759, 1.02233, 1.03724, 0.97644,
                    },
                },
                /* para 2 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        3.46096, 3.11607, 2.83506, 2.40765, 2.27183, 2.11008, 1.94953, 1.90880, 1.81319, 1.92626,
                        3.41198, 2.98095, 2.64919, 2.35822, 2.16447, 1.98349, 1.85799, 1.74427, 1.71533, 1.69513,
                        3.19925, 2.81470, 2.46550, 2.20372, 1.98904, 1.82905, 1.64207, 1.56988, 1.51237, 1.53937,
                        2.94296, 2.62021, 2.30377, 2.06737, 1.83394, 1.64843, 1.49935, 1.40847, 1.35937, 1.35746,
                        2.86443, 2.52285, 2.21105, 1.98717, 1.72215, 1.53626, 1.37838, 1.28144, 1.25090, 1.24046,
                        2.71516, 2.41336, 2.13772, 1.86897, 1.62376, 1.43135, 1.27299, 1.18769, 1.12342, 1.13920,
                        2.66023, 2.35802, 2.06654, 1.79616, 1.56637, 1.35611, 1.21830, 1.10737, 1.04618, 1.05134,
                        2.60891, 2.30225, 2.06623, 1.75342, 1.50519, 1.31677, 1.16280, 1.05434, 1.00442, 0.99956,
                        2.59205, 2.32846, 1.99731, 1.75337, 1.51002, 1.30520, 1.15210, 1.03828, 0.98474, 0.99814,
                        2.61290, 2.32168, 2.06707, 1.76223, 1.54545, 1.32375, 1.16807, 1.05647, 1.01872, 0.97628,
                        3.68399, 3.22612, 2.83341, 2.45297, 2.26830, 2.05979, 1.93171, 1.84536, 1.81081, 1.81487,
                        3.43372, 3.12345, 2.63497, 2.38566, 2.11950, 1.97474, 1.79712, 1.72531, 1.65158, 1.67404,
                        3.36499, 2.86030, 2.48771, 2.17937, 1.98910, 1.76149, 1.63643, 1.53011, 1.50406, 1.48284,
                        2.97563, 2.69731, 2.27262, 2.08612, 1.79538, 1.63328, 1.47127, 1.38896, 1.32897, 1.35378,
                        2.92889, 2.52607, 2.19427, 1.92981, 1.70085, 1.48351, 1.35497, 1.26839, 1.22588, 1.21602,
                        2.79659, 2.44494, 2.10119, 1.86329, 1.58468, 1.40324, 1.26809, 1.16054, 1.13158, 1.12881,
                        2.65460, 2.35026, 2.05488, 1.74868, 1.53036, 1.32858, 1.19329, 1.10167, 1.05103, 1.04701,
                        2.60402, 2.32158, 1.99831, 1.73333, 1.47473, 1.29438, 1.15597, 1.04252, 0.99509, 1.00313,
                        2.61128, 2.30618, 2.00975, 1.71404, 1.47462, 1.28934, 1.14449, 1.03918, 0.99639, 0.99720,
                        2.55940, 2.29130, 2.01070, 1.70263, 1.49658, 1.29246, 1.15746, 1.05533, 0.99121, 1.01141,
                        2.94006, 2.73834, 2.44355, 2.17946, 2.04335, 1.83849, 1.77150, 1.70092, 1.60268, 1.73072,
                        2.86197, 2.65713, 2.36149, 2.11879, 1.92952, 1.81474, 1.67727, 1.59661, 1.57438, 1.54122,
                        2.78259, 2.50531, 2.19180, 1.97565, 1.80698, 1.63908, 1.51740, 1.44131, 1.39779, 1.43281,
                        2.61820, 2.32828, 2.06964, 1.87954, 1.68411, 1.51694, 1.40210, 1.32501, 1.26495, 1.29266,
                        2.55572, 2.28485, 1.97482, 1.78809, 1.55418, 1.40778, 1.27198, 1.19853, 1.17660, 1.16232,
                        2.33489, 2.16899, 1.93091, 1.70822, 1.50282, 1.34265, 1.20442, 1.13831, 1.08284, 1.09835,
                        2.29023, 2.13824, 1.83357, 1.63844, 1.42087, 1.27132, 1.14862, 1.07781, 1.03290, 1.03702,
                        2.31895, 2.09825, 1.85895, 1.60530, 1.40666, 1.23701, 1.11882, 1.02536, 0.98588, 0.97922,
                        2.24793, 2.08081, 1.81855, 1.60766, 1.38357, 1.24816, 1.10004, 1.03445, 0.98150, 0.98450,
                        2.21451, 2.11269, 1.82336, 1.62060, 1.40207, 1.22578, 1.13345, 1.02647, 0.98225, 1.03514,
                    },
                },
                /* para 3 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        3.67464, 3.29896, 3.00846, 2.68891, 2.43807, 2.32209, 2.11778, 2.02774, 2.02366, 2.00146,
                        3.63950, 3.25735, 2.85713, 2.57556, 2.37582, 2.18264, 2.03676, 1.91711, 1.86782, 1.84819,
                        3.33144, 3.03465, 2.67867, 2.43455, 2.18886, 1.99624, 1.79856, 1.69694, 1.64934, 1.65665,
                        3.21940, 2.87841, 2.52353, 2.28493, 2.01827, 1.80362, 1.63546, 1.51661, 1.44659, 1.47770,
                        3.09339, 2.71560, 2.43612, 2.15449, 1.86124, 1.64898, 1.47086, 1.34577, 1.30983, 1.30123,
                        2.92964, 2.65425, 2.31235, 2.06710, 1.74981, 1.52261, 1.33928, 1.23037, 1.13773, 1.17085,
                        2.89028, 2.55024, 2.28407, 1.92287, 1.68748, 1.43533, 1.27126, 1.11710, 1.06730, 1.05377,
                        2.84145, 2.54906, 2.23103, 1.94374, 1.62132, 1.40331, 1.19359, 1.06473, 0.99074, 1.00831,
                        2.83257, 2.51644, 2.22224, 1.89182, 1.64156, 1.38596, 1.19587, 1.04807, 0.98450, 0.97886,
                        2.83992, 2.51981, 2.24614, 1.91473, 1.63563, 1.41245, 1.19225, 1.07390, 0.99668, 1.00711,
                        3.74525, 3.24205, 2.85575, 2.48990, 2.26360, 2.05416, 1.95014, 1.85225, 1.77138, 1.87183,
                        3.39847, 3.18494, 2.66421, 2.40998, 2.16387, 1.99333, 1.84708, 1.72080, 1.70365, 1.65589,
                        3.37417, 2.91922, 2.52006, 2.24483, 2.01655, 1.78939, 1.65490, 1.55023, 1.49194, 1.55531,
                        3.09222, 2.72433, 2.31349, 2.09506, 1.81963, 1.64083, 1.48924, 1.40275, 1.35096, 1.34686,
                        2.89890, 2.57424, 2.21052, 1.96203, 1.70504, 1.49139, 1.36042, 1.25453, 1.21521, 1.23050,
                        2.83085, 2.45476, 2.12940, 1.85588, 1.58781, 1.39883, 1.25893, 1.16766, 1.11526, 1.13155,
                        2.64473, 2.39025, 2.05314, 1.77532, 1.53808, 1.33111, 1.19613, 1.09175, 1.04453, 1.05507,
                        2.64549, 2.36787, 2.04482, 1.74801, 1.49840, 1.30224, 1.15094, 1.05717, 0.99082, 1.00661,
                        2.61965, 2.35309, 2.00556, 1.73998, 1.48791, 1.28530, 1.15452, 1.03525, 0.99444, 1.00087,
                        2.62124, 2.33681, 2.04674, 1.71865, 1.52800, 1.31080, 1.14421, 1.06750, 0.97996, 1.01740,
                        3.23941, 2.97123, 2.54829, 2.33952, 2.09312, 1.96442, 1.80233, 1.76933, 1.69078, 1.75514,
                        3.17594, 2.79277, 2.50780, 2.19285, 2.00378, 1.85156, 1.73840, 1.62765, 1.59992, 1.60132,
                        2.94622, 2.67543, 2.27008, 2.06887, 1.86676, 1.69345, 1.54281, 1.47650, 1.42842, 1.44690,
                        2.81696, 2.45073, 2.15960, 1.92089, 1.72458, 1.53379, 1.41591, 1.32763, 1.27621, 1.30538,
                        2.65693, 2.36283, 2.02780, 1.84310, 1.59894, 1.41921, 1.29419, 1.20143, 1.17671, 1.16753,
                        2.49042, 2.23653, 1.96021, 1.72144, 1.51223, 1.32179, 1.20527, 1.11599, 1.08019, 1.08398,
                        2.48724, 2.20375, 1.91027, 1.67596, 1.44760, 1.28167, 1.14520, 1.06814, 1.02622, 1.03328,
                        2.40764, 2.16924, 1.88790, 1.63894, 1.42400, 1.24936, 1.12696, 1.02716, 0.99692, 0.99090,
                        2.41420, 2.19178, 1.88610, 1.64774, 1.40350, 1.23931, 1.10501, 1.02661, 0.98518, 0.99653,
                        2.42015, 2.15282, 1.86701, 1.65891, 1.40905, 1.27348, 1.12469, 1.05160, 0.98969, 1.02966,
                    },
                },
                /* para 4 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        3.32630, 3.17446, 2.75632, 2.50958, 2.34061, 2.17846, 2.07624, 1.97643, 1.92972, 1.98294,
                        3.35839, 3.02765, 2.74826, 2.45282, 2.27358, 2.12030, 1.93997, 1.88343, 1.78588, 1.81315,
                        3.16400, 2.88935, 2.55576, 2.30951, 2.12058, 1.92754, 1.77201, 1.65460, 1.61841, 1.60901,
                        3.05464, 2.75621, 2.40549, 2.21969, 1.94995, 1.75240, 1.60130, 1.49521, 1.42942, 1.45342,
                        2.87619, 2.61288, 2.36270, 2.07743, 1.85411, 1.63653, 1.46732, 1.35786, 1.29848, 1.30118,
                        2.83187, 2.55560, 2.25122, 2.01147, 1.72173, 1.52221, 1.35063, 1.22727, 1.16982, 1.16073,
                        2.75142, 2.46768, 2.19214, 1.91208, 1.64319, 1.42201, 1.25356, 1.12941, 1.05634, 1.07016,
                        2.64639, 2.43716, 2.14437, 1.86124, 1.60917, 1.38205, 1.20315, 1.06787, 1.00707, 1.00870,
                        2.67729, 2.41615, 2.12321, 1.87383, 1.55890, 1.35817, 1.17829, 1.04703, 0.97388, 0.96994,
                        2.65231, 2.46483, 2.13778, 1.87489, 1.63125, 1.39997, 1.21435, 1.07642, 1.01547, 1.01765,
                        3.59726, 3.30957, 2.80972, 2.55604, 2.26461, 2.10141, 1.95497, 1.84859, 1.81488, 1.84828,
                        3.55333, 3.14496, 2.72139, 2.37534, 2.16602, 1.96288, 1.83039, 1.73527, 1.67382, 1.68342,
                        3.28417, 2.92065, 2.46948, 2.25039, 1.99009, 1.80874, 1.63943, 1.53838, 1.49504, 1.51738,
                        3.10509, 2.72057, 2.36209, 2.08267, 1.84629, 1.64193, 1.49320, 1.39745, 1.35371, 1.35628,
                        3.01514, 2.58973, 2.24553, 1.99583, 1.72442, 1.50709, 1.38301, 1.27148, 1.21941, 1.23415,
                        2.80080, 2.47727, 2.16082, 1.86264, 1.62307, 1.41920, 1.27319, 1.17725, 1.13106, 1.13707,
                        2.69653, 2.41114, 2.09427, 1.77829, 1.54851, 1.33153, 1.19384, 1.10030, 1.04545, 1.05186,
                        2.69328, 2.33976, 2.03971, 1.74874, 1.48377, 1.29038, 1.15269, 1.04777, 0.98495, 1.01001,
                        2.59457, 2.34846, 2.01100, 1.71833, 1.49236, 1.27591, 1.14282, 1.03847, 0.98631, 0.98730,
                        2.75999, 2.37483, 2.06829, 1.76210, 1.49798, 1.28728, 1.16050, 1.06043, 0.98960, 1.02934,
                        3.35424, 2.97158, 2.68024, 2.31933, 2.15659, 1.94414, 1.85127, 1.74126, 1.74505, 1.69483,
                        3.17352, 2.85969, 2.50250, 2.22424, 2.01452, 1.86046, 1.72628, 1.64223, 1.57898, 1.61230,
                        3.01091, 2.71403, 2.29583, 2.07290, 1.89441, 1.68225, 1.55434, 1.46969, 1.43158, 1.44586,
                        2.80997, 2.52933, 2.17702, 1.96449, 1.72953, 1.54312, 1.42423, 1.33862, 1.28824, 1.29656,
                        2.75844, 2.42453, 2.10099, 1.86082, 1.65036, 1.43009, 1.30767, 1.22300, 1.17918, 1.20789,
                        2.60066, 2.33957, 1.99806, 1.78949, 1.52038, 1.35833, 1.22147, 1.14778, 1.10406, 1.10572,
                        2.39080, 2.26399, 1.93482, 1.68714, 1.47672, 1.28058, 1.15033, 1.08083, 1.02492, 1.04407,
                        2.50472, 2.20498, 1.90412, 1.66210, 1.41266, 1.25561, 1.12032, 1.03471, 0.99547, 0.98445,
                        2.39785, 2.20442, 1.88581, 1.63698, 1.41343, 1.23137, 1.11245, 1.02686, 0.98162, 1.00435,
                        2.37652, 2.23857, 1.90114, 1.68393, 1.44741, 1.28201, 1.12527, 1.05646, 0.99433, 1.01036,
                    },
                },
                /* para 5 */
                {
                },
                /* para 6 */
                {
                },
            },
        },
    },
    /* isp_sub_module_lsc_tuning_t isp_sub_module_lsc_tuning */
    {
        1,//enable
        1,//interpolation enable
        1,//ae count
        4,//awb count
        1,//tigger_mode : 0 gain ,1 lux
        {
            /*aec trigger 0*/
            {
                0,
                500,
            },
        },
        {
            /*awb trigger 1*/
            {
                2600,
                3000,
            },
            /*awb trigger 2*/
            {
                3750,
                4150,
            },
            /*awb trigger 3*/
            {
                4800,
                5200,
            },
            /*awb trigger 4*/
            {
                6300,
                6700,
            },
            /*awb trigger 5*/
            {
                0,
                0,
            },
            /*awb trigger 6*/
            {
                0,
                0,
            },
        },
        /* isp_sub_module_lsc_pra */
        {
            /* aec trigger 0 */
            {
                /* para 1 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        2.33291, 2.19506, 2.34986, 2.39256, 2.58233, 2.74089, 3.06787, 3.36531, 3.91709, 4.20156,
                        2.03189, 2.01573, 2.06670, 2.23376, 2.37127, 2.65842, 2.87013, 3.28634, 3.72892, 4.23952,
                        1.78442, 1.76505, 1.83171, 1.94661, 2.18066, 2.38771, 2.73108, 2.97397, 3.48128, 3.96294,
                        1.55715, 1.52466, 1.60231, 1.75798, 1.91435, 2.23061, 2.52326, 2.85697, 3.27271, 3.71999,
                        1.33751, 1.34417, 1.40324, 1.55268, 1.75634, 2.00640, 2.38743, 2.68179, 3.06615, 3.66760,
                        1.20423, 1.18207, 1.26011, 1.40474, 1.61372, 1.89629, 2.24172, 2.62097, 2.98635, 3.45706,
                        1.07285, 1.06783, 1.14930, 1.31654, 1.52339, 1.79522, 2.14497, 2.51049, 2.88325, 3.35749,
                        1.01608, 1.00069, 1.08192, 1.23987, 1.45701, 1.73738, 2.10096, 2.45173, 2.87428, 3.32100,
                        1.00172, 0.99319, 1.05901, 1.23131, 1.45572, 1.72372, 2.04948, 2.47833, 2.79273, 3.31098,
                        1.04612, 1.00081, 1.09488, 1.24542, 1.47637, 1.73540, 2.13328, 2.44140, 2.92865, 3.28274,
                        1.80959, 1.83622, 1.89814, 1.99050, 2.09117, 2.27099, 2.51599, 2.80129, 3.32198, 3.70387,
                        1.71358, 1.70967, 1.75858, 1.85911, 2.01809, 2.19083, 2.41058, 2.75317, 3.13525, 3.72172,
                        1.50720, 1.51663, 1.56844, 1.66427, 1.80939, 2.01228, 2.24157, 2.51253, 2.94953, 3.43280,
                        1.36760, 1.35136, 1.41004, 1.49142, 1.66260, 1.83499, 2.10667, 2.35734, 2.72619, 3.24507,
                        1.22658, 1.22348, 1.27621, 1.37043, 1.50521, 1.72135, 1.96083, 2.24807, 2.56135, 3.11846,
                        1.12049, 1.12918, 1.16708, 1.26896, 1.42635, 1.60348, 1.87262, 2.16099, 2.47404, 2.95652,
                        1.05296, 1.03514, 1.10535, 1.19393, 1.35465, 1.53116, 1.80349, 2.08881, 2.38989, 2.87540,
                        0.99761, 1.00613, 1.05183, 1.16523, 1.30456, 1.51640, 1.75164, 2.06268, 2.38432, 2.78655,
                        0.99721, 0.98205, 1.04410, 1.14062, 1.31295, 1.47136, 1.74596, 2.05389, 2.32251, 2.82843,
                        1.00714, 1.01377, 1.05509, 1.17406, 1.30154, 1.52995, 1.74338, 2.08848, 2.35921, 2.79664,
                        1.77835, 1.68601, 1.76970, 1.78508, 1.91928, 2.06040, 2.23964, 2.49406, 2.72778, 3.04998,
                        1.57135, 1.62363, 1.64080, 1.74089, 1.85582, 2.00729, 2.15668, 2.40881, 2.67201, 2.95857,
                        1.49395, 1.43506, 1.51184, 1.54966, 1.70552, 1.86296, 2.05140, 2.23325, 2.54049, 2.86343,
                        1.29935, 1.31213, 1.34391, 1.43425, 1.57097, 1.72790, 1.92589, 2.13397, 2.38223, 2.69438,
                        1.18956, 1.19586, 1.22863, 1.32597, 1.42761, 1.63927, 1.82405, 2.04094, 2.31745, 2.57997,
                        1.08906, 1.10982, 1.14494, 1.22185, 1.39105, 1.51637, 1.76074, 1.97164, 2.19363, 2.55497,
                        1.04689, 1.03543, 1.07624, 1.18437, 1.30699, 1.49154, 1.68421, 1.94247, 2.15792, 2.44379,
                        1.00051, 1.00052, 1.04276, 1.14086, 1.29683, 1.44301, 1.67518, 1.90129, 2.12391, 2.45859,
                        0.99102, 0.99851, 1.03217, 1.13957, 1.27243, 1.44736, 1.65112, 1.90400, 2.10724, 2.40974,
                        1.01593, 0.99986, 1.04666, 1.12731, 1.31304, 1.44110, 1.66172, 1.88522, 2.11940, 2.44752,
                    },
                },
                /* para 2 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        1.93854, 1.82430, 1.91026, 2.02293, 2.07955, 2.31938, 2.45089, 2.89663, 3.08907, 3.61005,
                        1.70352, 1.72346, 1.76210, 1.85871, 1.99916, 2.17600, 2.34530, 2.63646, 2.99526, 3.36721,
                        1.53397, 1.52718, 1.56202, 1.67126, 1.79137, 1.99771, 2.22644, 2.46250, 2.79763, 3.24717,
                        1.37165, 1.38001, 1.42029, 1.50381, 1.67093, 1.85862, 2.09053, 2.34243, 2.64249, 3.08223,
                        1.24794, 1.23858, 1.28025, 1.38604, 1.49128, 1.73528, 1.97282, 2.19662, 2.51942, 2.90778,
                        1.14050, 1.13915, 1.19263, 1.27497, 1.45234, 1.61752, 1.90490, 2.16211, 2.43056, 2.84786,
                        1.05945, 1.04513, 1.09964, 1.21434, 1.34176, 1.55101, 1.78654, 2.04863, 2.36476, 2.66858,
                        1.00503, 1.00701, 1.06587, 1.17488, 1.32714, 1.51424, 1.76943, 2.05078, 2.32294, 2.72132,
                        1.01220, 0.98429, 1.04034, 1.15515, 1.29316, 1.50686, 1.73253, 2.02664, 2.30508, 2.60218,
                        1.01947, 1.02194, 1.06349, 1.18208, 1.31348, 1.51449, 1.76192, 2.03201, 2.30979, 2.71032,
                        1.78887, 1.83467, 1.84022, 1.96215, 2.09262, 2.22050, 2.51253, 2.83889, 3.21943, 3.80743,
                        1.69706, 1.66648, 1.71860, 1.81832, 1.95019, 2.11914, 2.35324, 2.64345, 3.11880, 3.54477,
                        1.48306, 1.49461, 1.54507, 1.62660, 1.79163, 1.96370, 2.19627, 2.47282, 2.85646, 3.36359,
                        1.35820, 1.34836, 1.38088, 1.48287, 1.61816, 1.79955, 2.06453, 2.28669, 2.70100, 3.16764,
                        1.23055, 1.21374, 1.26997, 1.35438, 1.49298, 1.69801, 1.93070, 2.19703, 2.52067, 2.97674,
                        1.12459, 1.12987, 1.16747, 1.26154, 1.41573, 1.57901, 1.84608, 2.12569, 2.40310, 2.97233,
                        1.05595, 1.04045, 1.09684, 1.19758, 1.32631, 1.52619, 1.77379, 2.03562, 2.37909, 2.74930,
                        1.00852, 0.99752, 1.05010, 1.14618, 1.30365, 1.47827, 1.71487, 2.03187, 2.29215, 2.80270,
                        1.00172, 0.98708, 1.03270, 1.15327, 1.27234, 1.47360, 1.72801, 1.98581, 2.32465, 2.69272,
                        1.02837, 1.01113, 1.05423, 1.15689, 1.29828, 1.47879, 1.73910, 2.04479, 2.31429, 2.83181,
                        1.65559, 1.66235, 1.65699, 1.77776, 1.84935, 1.98012, 2.16504, 2.44293, 2.70453, 3.06778,
                        1.55154, 1.58109, 1.60397, 1.68456, 1.80790, 1.93251, 2.14273, 2.30474, 2.74184, 2.87403,
                        1.41349, 1.41692, 1.42477, 1.52397, 1.63816, 1.79760, 1.97356, 2.19964, 2.44944, 2.86493,
                        1.26954, 1.28485, 1.30750, 1.39690, 1.51066, 1.67539, 1.90596, 2.04065, 2.43634, 2.62832,
                        1.16206, 1.18911, 1.19777, 1.29012, 1.40755, 1.56631, 1.77781, 1.99077, 2.21420, 2.63850,
                        1.09292, 1.08416, 1.12283, 1.21010, 1.31817, 1.49319, 1.70319, 1.93366, 2.18165, 2.53735,
                        1.01966, 1.02636, 1.06953, 1.14081, 1.28123, 1.43746, 1.65539, 1.86105, 2.12968, 2.42606,
                        0.99725, 0.98466, 1.03416, 1.11838, 1.24813, 1.40364, 1.60567, 1.86347, 2.06994, 2.42504,
                        0.98648, 0.97382, 1.02128, 1.09656, 1.22974, 1.38675, 1.60851, 1.82984, 2.08584, 2.43766,
                        1.04983, 0.98642, 1.05545, 1.12874, 1.27871, 1.41849, 1.59629, 1.87629, 2.04390, 2.37283,
                    },
                },
                /* para 3 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        2.02951, 1.98546, 2.13118, 2.11782, 2.34759, 2.46998, 2.68161, 3.01367, 3.23055, 3.76292, 
                        1.87309, 1.85696, 1.90924, 2.03575, 2.18285, 2.34863, 2.56969, 2.85318, 3.23707, 3.57780, 
                        1.64987, 1.64370, 1.71582, 1.79489, 2.00068, 2.18740, 2.44647, 2.67611, 3.03769, 3.44897, 
                        1.46682, 1.45469, 1.51037, 1.63231, 1.79526, 2.04191, 2.27549, 2.56104, 2.84084, 3.32931, 
                        1.30154, 1.28519, 1.35454, 1.46489, 1.64505, 1.85235, 2.17099, 2.44218, 2.71982, 3.23592, 
                        1.16016, 1.15755, 1.20806, 1.34551, 1.51751, 1.74975, 2.03973, 2.32671, 2.63098, 2.98694, 
                        1.05075, 1.05064, 1.11394, 1.26604, 1.45032, 1.68235, 1.96341, 2.29523, 2.55723, 3.02327, 
                        1.00543, 0.99165, 1.06839, 1.20160, 1.39607, 1.63528, 1.93196, 2.21287, 2.57479, 2.83594, 
                        0.99552, 0.98685, 1.04450, 1.19922, 1.40638, 1.60644, 1.90183, 2.24591, 2.48113, 2.94665, 
                        0.99270, 0.99680, 1.06225, 1.20626, 1.38224, 1.65415, 1.92513, 2.26093, 2.57719, 2.94020, 
                        1.87678, 1.81532, 1.88268, 1.96537, 2.06069, 2.25328, 2.49233, 2.81654, 3.28491, 3.60621, 
                        1.68720, 1.67898, 1.74300, 1.81480, 2.00237, 2.16254, 2.38264, 2.72969, 3.15268, 3.65031, 
                        1.53143, 1.50983, 1.55412, 1.66988, 1.78418, 2.01477, 2.21856, 2.49092, 2.93420, 3.35764, 
                        1.36422, 1.34503, 1.40456, 1.47534, 1.64150, 1.82847, 2.08925, 2.34921, 2.71515, 3.25507, 
                        1.22669, 1.22017, 1.25739, 1.37952, 1.49106, 1.72164, 1.94746, 2.23048, 2.55907, 3.04459, 
                        1.12807, 1.12200, 1.16719, 1.25790, 1.40824, 1.58506, 1.85831, 2.14326, 2.42887, 2.97107, 
                        1.05601, 1.03647, 1.08902, 1.19249, 1.33596, 1.53271, 1.77848, 2.06909, 2.38829, 2.82200, 
                        0.99630, 1.00456, 1.05639, 1.15932, 1.30502, 1.50449, 1.73830, 2.04614, 2.34590, 2.82272, 
                        1.00463, 0.99543, 1.03317, 1.14748, 1.28845, 1.48113, 1.73012, 2.04083, 2.34012, 2.77594, 
                        1.01357, 1.01402, 1.05457, 1.17403, 1.29093, 1.51871, 1.76107, 2.02969, 2.39657, 2.79185, 
                        1.73695, 1.70986, 1.75713, 1.79213, 1.90347, 2.06209, 2.26537, 2.54709, 2.94234, 3.14431, 
                        1.58172, 1.59794, 1.62435, 1.72528, 1.84634, 2.01242, 2.19915, 2.49989, 2.81536, 3.21909, 
                        1.44027, 1.42495, 1.47047, 1.54803, 1.69473, 1.85458, 2.07633, 2.26180, 2.66494, 3.00834, 
                        1.26889, 1.27205, 1.32270, 1.39565, 1.53014, 1.73437, 1.91436, 2.18659, 2.45229, 2.85707, 
                        1.19470, 1.16987, 1.20397, 1.29857, 1.42324, 1.59578, 1.85178, 2.03987, 2.38556, 2.73001, 
                        1.07361, 1.08541, 1.11559, 1.20046, 1.32997, 1.51691, 1.71232, 1.98444, 2.22647, 2.62379, 
                        1.03282, 1.01547, 1.06641, 1.14299, 1.28124, 1.45039, 1.69060, 1.93162, 2.22521, 2.52927, 
                        0.98570, 1.00368, 1.02704, 1.12044, 1.25437, 1.42686, 1.62506, 1.92200, 2.15742, 2.52525, 
                        1.00422, 0.97793, 1.02633, 1.10668, 1.24356, 1.41227, 1.65451, 1.88652, 2.17831, 2.47475, 
                        0.99502, 0.99768, 1.04930, 1.12169, 1.26983, 1.44059, 1.64930, 1.92675, 2.17415, 2.52604, 
                    },
                },
                /* para 4 */
                {
                    1.000000,//stength,register: [07:00]lsc_snr1_man_factor = floor(strength * 2^7) ,address:0x64804C40
                    1,  //lsc_man_mode: [16]lsc_snr1_man_mode = lsc_man_mode,address:0x64804C40
                    {
                        1.92794, 1.94757, 2.00775, 2.07981, 2.19978, 2.32108, 2.52438, 2.90787, 3.02627, 3.53518, 
                        1.85880, 1.80479, 1.86487, 1.95334, 2.11905, 2.25984, 2.47442, 2.69356, 3.05398, 3.39410, 
                        1.57494, 1.63148, 1.65352, 1.78943, 1.90150, 2.12957, 2.31008, 2.57302, 2.84629, 3.23136, 
                        1.46845, 1.44846, 1.49384, 1.59558, 1.77262, 1.96723, 2.20939, 2.43377, 2.73164, 3.12512, 
                        1.29313, 1.29235, 1.35311, 1.46888, 1.61284, 1.86138, 2.08908, 2.34054, 2.61926, 2.98480, 
                        1.15899, 1.17256, 1.22056, 1.34965, 1.51713, 1.74270, 1.98095, 2.29235, 2.51408, 2.88361, 
                        1.07675, 1.06401, 1.11625, 1.26014, 1.42011, 1.63887, 1.90520, 2.16872, 2.48479, 2.80968, 
                        1.00245, 0.99007, 1.06556, 1.19955, 1.37417, 1.59941, 1.86718, 2.14012, 2.43338, 2.69060, 
                        0.99111, 0.99743, 1.04202, 1.19184, 1.37441, 1.56843, 1.85450, 2.13019, 2.40203, 2.75687, 
                        1.01300, 0.98936, 1.07862, 1.18590, 1.40865, 1.58319, 1.90989, 2.10508, 2.49619, 2.70328, 
                        1.83978, 1.81546, 1.89891, 1.95416, 2.12054, 2.24534, 2.50372, 2.90710, 3.24551, 3.86426, 
                        1.69947, 1.66086, 1.73345, 1.82095, 1.97737, 2.17722, 2.37534, 2.71812, 3.14250, 3.60917, 
                        1.51191, 1.49536, 1.56200, 1.63480, 1.80594, 1.97956, 2.23162, 2.49324, 2.90708, 3.46430, 
                        1.34987, 1.35267, 1.39396, 1.48804, 1.64522, 1.85365, 2.08786, 2.36708, 2.72070, 3.21966, 
                        1.23375, 1.22495, 1.28519, 1.37002, 1.53013, 1.71223, 1.98523, 2.24237, 2.58769, 3.11771, 
                        1.13069, 1.14106, 1.17636, 1.28034, 1.42657, 1.62387, 1.87913, 2.17069, 2.46466, 2.98770, 
                        1.04543, 1.04107, 1.09796, 1.19348, 1.34809, 1.54460, 1.77258, 2.09552, 2.38509, 2.84300, 
                        1.00565, 1.00109, 1.05519, 1.14737, 1.30831, 1.48648, 1.74649, 2.03870, 2.33757, 2.81336, 
                        0.99151, 0.98727, 1.03314, 1.14577, 1.28785, 1.48344, 1.71426, 2.02172, 2.34386, 2.69343, 
                        1.03801, 1.00795, 1.06941, 1.15114, 1.33704, 1.48856, 1.76245, 2.05239, 2.35311, 2.81151, 
                        1.69251, 1.76211, 1.74995, 1.85663, 1.94126, 2.17126, 2.30621, 2.78189, 2.91837, 3.48588, 
                        1.61533, 1.58662, 1.65155, 1.72578, 1.87475, 2.00254, 2.22060, 2.51060, 2.87543, 3.28454, 
                        1.41766, 1.42110, 1.46717, 1.55291, 1.68577, 1.90872, 2.06693, 2.32881, 2.65547, 3.05466, 
                        1.31343, 1.28976, 1.34671, 1.41552, 1.56545, 1.72677, 1.97138, 2.20774, 2.52027, 2.97971, 
                        1.20554, 1.18301, 1.23227, 1.31542, 1.45014, 1.64169, 1.86784, 2.07110, 2.44051, 2.70308, 
                        1.11274, 1.10428, 1.14433, 1.22042, 1.36637, 1.52976, 1.78609, 2.03199, 2.28856, 2.78541, 
                        1.03843, 1.02796, 1.07895, 1.16063, 1.28377, 1.47385, 1.69357, 1.93934, 2.26745, 2.51094, 
                        0.99536, 0.98991, 1.03005, 1.11509, 1.25205, 1.42309, 1.65079, 1.91842, 2.18985, 2.61355, 
                        0.99611, 0.98878, 1.02481, 1.11709, 1.23750, 1.42129, 1.65053, 1.89113, 2.20080, 2.48199, 
                        1.00403, 1.00160, 1.05569, 1.11697, 1.27411, 1.45204, 1.65793, 1.96446, 2.17914, 2.63484, 
                    },
                },
                /* para 5 */
                {
                },
                /* para 6 */
                {
                },
            },
        },
    },
};
#endif
