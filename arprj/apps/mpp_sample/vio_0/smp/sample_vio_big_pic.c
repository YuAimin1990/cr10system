#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include"sample_vio_big_pic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hal_vo.h"
#include "hal_vin_log.h"
#include "hal_region.h"
#include "mpi_vb.h"
#include "osal.h"
#include "../sample_vio.h"
#include "hal_gpio.h"
#include <dirent.h>


#define AR_BIGPIC_IMX307_1080P 0
#define AR_BIGPIC_OV48C40_8K 1

#define AR_BIGPIC_PIPE_FILE "/tmp/cmd_pipe"

#define AR_BIGPIC_TAG "bigpic"

#define AR_BIGPIC_DISPLAY_IF_HDMI AR_FALSE
#define AR_BIGPIC_DISPLAY_IF_MIPI AR_TRUE
#define AR_BIGPIC_DISPLAY_API_LEVEL_HAL 0
#define AR_BIGPIC_DISPLAY_API_LEVEL_MPI 1

#define AR_BIGPIC_CAMERA_MODE_STILL 0
#define AR_BIGPIC_CAMERA_MODE_VIDEO 1

static int AR_BIGPIC_NUM = 200;
static int AR_BIGPIC_CNT = 1;

static int AR_BIGPIC_TO_FILE_NUM = 1;
static int AR_BIGPIC_TO_FILE_RAW = 1;
static int AR_BIGPIC_TO_FILE_YUV_SMALL = 1;
static int AR_BIGPIC_TO_FILE_YUV_BIG = 1;

static int AR_BIGPIC_TO_DISPLAY = 1;
static int AR_BIGPIC_DISPLAY_IF = AR_BIGPIC_DISPLAY_IF_MIPI;
static int AR_BIGPIC_DISPLAY_API_LEVEL = AR_BIGPIC_DISPLAY_API_LEVEL_HAL;
static int AR_BIGPIC_DISPLAY_WIDTH = 1920;
static int AR_BIGPIC_DISPLAY_HEIGHT = 1080;
static int AR_BIGPIC_DISPLAY_FPS = 30;

static int AR_BIGPIC_I2C_BUS_ID = 1;

static int AR_BIGPIC_SENSOR_TYPE_SIM_LINE = SENSOR16_TYPE;
static int AR_BIGPIC_SENSOR_TYPE_PHY_LINE = SENSOR19_TYPE;
static int AR_BIGPIC_SENSOR_TYPE_SIM_HDR = SENSOR21_TYPE;
static int AR_BIGPIC_SENSOR_TYPE_SIM_HDR_R = SENSOR22_TYPE;
static int AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW = SENSOR9_TYPE;
static int AR_BIGPIC_SENSOR_TYPE_PHY_HDR_RAW = SENSOR23_TYPE;

static int AR_BIGPIC_WORKING_VI_ID_PREVIEW = 0;
static int AR_BIGPIC_WORKING_VI_ID_LEFT = 0; //AR_BIGPIC_WORKING_VI_ID_PREVIEW;
static int AR_BIGPIC_WORKING_VI_ID_RIGHT = 1; //(AR_BIGPIC_WORKING_VI_ID_LEFT+1);
static int AR_BIGPIC_WORKING_VI_ID_RAW = 2; //(AR_BIGPIC_WORKING_VI_ID_LEFT+2);

static int AR_BIGPIC_PREVIEW_STREAM_WIDTH = 1920;
static int AR_BIGPIC_PREVIEW_STREAM_HEIGHT = 1080;
static int AR_BIGPIC_PREVIEW_STREAM_STRIDE = 2048;

static int AR_BIGPIC_RAW_STREAM_WIDTH = 1920;
static int AR_BIGPIC_RAW_STREAM_HEIGHT = 1080;
static int AR_BIGPIC_ISP_IN_STRIDE = 3840;

static int AR_BIGPIC_ISP_IN_WIDTH = 1056;
static int AR_BIGPIC_ISP_IN_HEIGHT = 1080;
static int AR_BIGPIC_ISP_IN_OVERLAP_H = 96;
static int AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP = 0;
static int AR_BIGPIC_ISP_IN_OVERLAP_V = 0;
static int AR_BIGPIC_ISP_IN_OFFSET_H = 864;
static int AR_BIGPIC_ISP_IN_OFFSET_V = 0;

static int AR_BIGPIC_RAW_STREAM_WIDTH_HDR = 1952;
static int AR_BIGPIC_RAW_STREAM_HEIGHT_HDR = 2678;
static int AR_BIGPIC_ISP_IN_STRIDE_HDR = 4096;

static int AR_BIGPIC_ISP_IN_WIDTH_HDR = 1184;
static int AR_BIGPIC_ISP_IN_HEIGHT_HDR = 2678;
static int AR_BIGPIC_ISP_IN_OVERLAP_H_HDR = 208;
static int AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP_HDR = 112; //(AR_BIGPIC_ISP_IN_OVERLAP_H_HDR-AR_BIGPIC_ISP_IN_OVERLAP_H);
static int AR_BIGPIC_ISP_IN_OVERLAP_V_HDR = 0;
static int AR_BIGPIC_ISP_IN_OFFSET_H_HDR = 768;
static int AR_BIGPIC_ISP_IN_OFFSET_V_HDR = 0;

static int AR_BIGPIC_WIDTH = 1920;
static int AR_BIGPIC_HEIGHT = 1080;
static int AR_BIGPIC_ISP_OUT_STRIDE = 2048;

static int AR_BIGPIC_ISP_OUT_WIDTH = 1024;
static int AR_BIGPIC_ISP_OUT_HEIGHT = 1080;
static int AR_BIGPIC_ISP_OUT_OVERLAP_H = 64;
static int AR_BIGPIC_ISP_OUT_OVERLAP_V = 0;
static int AR_BIGPIC_ISP_OUT_OFFSET_H = 896;
static int AR_BIGPIC_ISP_OUT_OFFSET_V = 0;

static int AR_BIGPIC_FREQ_PREVIEW_MIPI = 200000000;
static int AR_BIGPIC_FREQ_PREVIEW_VIF = 400000000;
static int AR_BIGPIC_FREQ_PREVIEW_HDR = 150000000;
static int AR_BIGPIC_FREQ_PREVIEW_ISP = 150000000; //AR_BIGPIC_FREQ_PREVIEW_HDR;
static int AR_BIGPIC_FREQ_BIGPIC_MIPI = 200000000;
static int AR_BIGPIC_FREQ_BIGPIC_VIF = 400000000;
static int AR_BIGPIC_FREQ_BIGPIC_HDR = 150000000;
static int AR_BIGPIC_FREQ_BIGPIC_ISP = 150000000; //AR_BIGPIC_FREQ_BIGPIC_HDR;

typedef enum
{
    AR_BIGPIC_STATE_NULL,
    AR_BIGPIC_STATE_STOP_WILLABLE,
    AR_BIGPIC_STATE_RUN,
    AR_BIGPIC_STATE_RUN_PREVIEW,
    AR_BIGPIC_STATE_RUN_PREVIEW_START,
    AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE,
    AR_BIGPIC_STATE_RUN_PREVIEW_STOP,
    AR_BIGPIC_STATE_RUN_BIG_PIC,
    AR_BIGPIC_STATE_RUN_BIG_PIC_START,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_PRE,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_TRIGGER_RAW,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_RAW,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_RAW,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_YUV,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_YUV,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_BIG,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_DISP_YUV,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_BIG,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_RAW,
    AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_POST,
    AR_BIGPIC_STATE_RUN_BIG_PIC_STOP,
};

extern SAMPLE_SNS_TYPE_E g_enSnsType[];
static int server_state_current = AR_BIGPIC_STATE_STOP_WILLABLE;
static int server_state_will = AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE;
static int server_state_will_previous = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE;
static SAMPLE_VIO_PRA pra = {0};
static SAMPLE_VIO_PRA *ppra = &pra;

static void* take_still_thread(void *arg);
static void* display_thread(void *arg);
static AR_S32 Display_Init(STRU_CAMERA_T *pstCamera, AR_S32 Ch);
static AR_S32 Display_Deinit(STRU_CAMERA_T *pstCamera, AR_S32 Ch);
static AR_S32 Display_Buffer(STRU_CAMERA_T *pstCamera, AR_S32 Ch, VIDEO_FRAME_INFO_S *pstFrame);
static void* big_pic_3a_auto_thread(void *arg);

static void big_pic_config(int res)
{
    if(res == AR_BIGPIC_IMX307_1080P)
    {
        AR_BIGPIC_NUM = 200;
        AR_BIGPIC_CNT = 1;

        AR_BIGPIC_TO_FILE_NUM = 1;
        AR_BIGPIC_TO_FILE_RAW = 1;
        AR_BIGPIC_TO_FILE_YUV_SMALL = 1;
        AR_BIGPIC_TO_FILE_YUV_BIG = 1;

        AR_BIGPIC_TO_DISPLAY = 1;
        AR_BIGPIC_DISPLAY_IF = AR_BIGPIC_DISPLAY_IF_MIPI;
        AR_BIGPIC_DISPLAY_API_LEVEL = AR_BIGPIC_DISPLAY_API_LEVEL_MPI;
        AR_BIGPIC_DISPLAY_WIDTH = 1920;
        AR_BIGPIC_DISPLAY_HEIGHT = 1080;
        AR_BIGPIC_DISPLAY_FPS = 30;

        AR_BIGPIC_I2C_BUS_ID = 1;

        AR_BIGPIC_SENSOR_TYPE_SIM_LINE = SENSOR16_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_LINE = SENSOR19_TYPE;
        AR_BIGPIC_SENSOR_TYPE_SIM_HDR = SENSOR21_TYPE;
        AR_BIGPIC_SENSOR_TYPE_SIM_HDR_R = SENSOR22_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW = SENSOR9_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_HDR_RAW = SENSOR23_TYPE;

        AR_BIGPIC_WORKING_VI_ID_PREVIEW = 0;
        AR_BIGPIC_WORKING_VI_ID_LEFT = AR_BIGPIC_WORKING_VI_ID_PREVIEW;
        AR_BIGPIC_WORKING_VI_ID_RIGHT = (AR_BIGPIC_WORKING_VI_ID_LEFT+1);
        AR_BIGPIC_WORKING_VI_ID_RAW = (AR_BIGPIC_WORKING_VI_ID_LEFT+2);

        AR_BIGPIC_PREVIEW_STREAM_WIDTH = 1920;
        AR_BIGPIC_PREVIEW_STREAM_HEIGHT = 1080;
        AR_BIGPIC_PREVIEW_STREAM_STRIDE = 2048;

        AR_BIGPIC_RAW_STREAM_WIDTH = 1920;
        AR_BIGPIC_RAW_STREAM_HEIGHT = 1080;
        AR_BIGPIC_ISP_IN_STRIDE = 3840;

        AR_BIGPIC_ISP_IN_WIDTH = 1280;
        AR_BIGPIC_ISP_IN_HEIGHT = 1080;
        AR_BIGPIC_ISP_IN_OVERLAP_H = 320;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP = 0;
        AR_BIGPIC_ISP_IN_OVERLAP_V = 0;
        AR_BIGPIC_ISP_IN_OFFSET_H = 640;
        AR_BIGPIC_ISP_IN_OFFSET_V = 0;

        AR_BIGPIC_RAW_STREAM_WIDTH_HDR = 1952;
        AR_BIGPIC_RAW_STREAM_HEIGHT_HDR = 2678;
        AR_BIGPIC_ISP_IN_STRIDE_HDR = 4096;

        AR_BIGPIC_ISP_IN_WIDTH_HDR = 1184;
        AR_BIGPIC_ISP_IN_HEIGHT_HDR = 2678;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR = 208;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP_HDR = (AR_BIGPIC_ISP_IN_OVERLAP_H_HDR-AR_BIGPIC_ISP_IN_OVERLAP_H);
        AR_BIGPIC_ISP_IN_OVERLAP_V_HDR = 0;
        AR_BIGPIC_ISP_IN_OFFSET_H_HDR = 768;
        AR_BIGPIC_ISP_IN_OFFSET_V_HDR = 0;

        AR_BIGPIC_WIDTH = 1920;
        AR_BIGPIC_HEIGHT = 1080;
        AR_BIGPIC_ISP_OUT_STRIDE = 2048;

        AR_BIGPIC_ISP_OUT_WIDTH = 1024;
        AR_BIGPIC_ISP_OUT_HEIGHT = 1080;
        AR_BIGPIC_ISP_OUT_OVERLAP_H = 64;
        AR_BIGPIC_ISP_OUT_OVERLAP_V = 0;
        AR_BIGPIC_ISP_OUT_OFFSET_H = 896;
        AR_BIGPIC_ISP_OUT_OFFSET_V = 0;

        AR_BIGPIC_FREQ_PREVIEW_MIPI = 100000000;
        AR_BIGPIC_FREQ_PREVIEW_VIF = 300000000;
        AR_BIGPIC_FREQ_PREVIEW_HDR = 300000000;
        AR_BIGPIC_FREQ_PREVIEW_ISP = AR_BIGPIC_FREQ_PREVIEW_HDR;
        AR_BIGPIC_FREQ_BIGPIC_MIPI = 100000000;
        AR_BIGPIC_FREQ_BIGPIC_VIF = 300000000;
        AR_BIGPIC_FREQ_BIGPIC_HDR = 300000000;
        AR_BIGPIC_FREQ_BIGPIC_ISP = AR_BIGPIC_FREQ_BIGPIC_HDR;

    }else if(res == AR_BIGPIC_OV48C40_8K)
    {
        AR_BIGPIC_NUM = 200;
        AR_BIGPIC_CNT = 1;

        AR_BIGPIC_TO_FILE_NUM = 1;
        AR_BIGPIC_TO_FILE_RAW = 0;
        AR_BIGPIC_TO_FILE_YUV_SMALL = 0;
        AR_BIGPIC_TO_FILE_YUV_BIG = 1;

        AR_BIGPIC_TO_DISPLAY = 0;
        AR_BIGPIC_DISPLAY_IF = AR_BIGPIC_DISPLAY_IF_MIPI;
        AR_BIGPIC_DISPLAY_API_LEVEL = AR_BIGPIC_DISPLAY_API_LEVEL_MPI;
        AR_BIGPIC_DISPLAY_WIDTH = 1920;
        AR_BIGPIC_DISPLAY_HEIGHT = 1080;
        AR_BIGPIC_DISPLAY_FPS = 30;

        AR_BIGPIC_I2C_BUS_ID = 1;

        AR_BIGPIC_SENSOR_TYPE_SIM_LINE = SENSOR17_TYPE;
        // AR_BIGPIC_SENSOR_TYPE_PHY_LINE = SENSOR18_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_LINE = SENSOR19_TYPE;
        AR_BIGPIC_SENSOR_TYPE_SIM_HDR = SENSOR17_TYPE;
        AR_BIGPIC_SENSOR_TYPE_SIM_HDR_R = SENSOR17_TYPE;
        // AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW = SENSOR18_TYPE;
        // AR_BIGPIC_SENSOR_TYPE_PHY_HDR_RAW = SENSOR18_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW = SENSOR9_TYPE;
        AR_BIGPIC_SENSOR_TYPE_PHY_HDR_RAW = SENSOR23_TYPE;

        AR_BIGPIC_WORKING_VI_ID_PREVIEW = 0;
        AR_BIGPIC_WORKING_VI_ID_LEFT = AR_BIGPIC_WORKING_VI_ID_PREVIEW;
        AR_BIGPIC_WORKING_VI_ID_RIGHT = (AR_BIGPIC_WORKING_VI_ID_LEFT+1);
        AR_BIGPIC_WORKING_VI_ID_RAW = (AR_BIGPIC_WORKING_VI_ID_LEFT+2);

        AR_BIGPIC_PREVIEW_STREAM_WIDTH = 1920;
        AR_BIGPIC_PREVIEW_STREAM_HEIGHT = 1080;
        AR_BIGPIC_PREVIEW_STREAM_STRIDE = 2048;

        AR_BIGPIC_RAW_STREAM_WIDTH = 8064;
        AR_BIGPIC_RAW_STREAM_HEIGHT = 6048;
        AR_BIGPIC_ISP_IN_STRIDE = 16128;

        AR_BIGPIC_ISP_IN_WIDTH = 4064;
        AR_BIGPIC_ISP_IN_HEIGHT = 6048;
        AR_BIGPIC_ISP_IN_OVERLAP_H = 64;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP = 0;
        AR_BIGPIC_ISP_IN_OVERLAP_V = 0;
        AR_BIGPIC_ISP_IN_OFFSET_H = 3936;
        AR_BIGPIC_ISP_IN_OFFSET_V = 0;

        AR_BIGPIC_RAW_STREAM_WIDTH_HDR = 8064;
        AR_BIGPIC_RAW_STREAM_HEIGHT_HDR = 6048;
        AR_BIGPIC_ISP_IN_STRIDE_HDR = 16128;

        AR_BIGPIC_ISP_IN_WIDTH_HDR = 4064;
        AR_BIGPIC_ISP_IN_HEIGHT_HDR = 6048;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR = 64;
        AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP_HDR = (AR_BIGPIC_ISP_IN_OVERLAP_H_HDR-AR_BIGPIC_ISP_IN_OVERLAP_H);
        AR_BIGPIC_ISP_IN_OVERLAP_V_HDR = 0;
        AR_BIGPIC_ISP_IN_OFFSET_H_HDR = 3936;
        AR_BIGPIC_ISP_IN_OFFSET_V_HDR = 0;

        AR_BIGPIC_WIDTH = 8000;
        AR_BIGPIC_HEIGHT = 6048;
        AR_BIGPIC_ISP_OUT_STRIDE = 8192;

        AR_BIGPIC_ISP_OUT_WIDTH = 4032;
        AR_BIGPIC_ISP_OUT_HEIGHT = 6048;
        AR_BIGPIC_ISP_OUT_OVERLAP_H = 32;
        AR_BIGPIC_ISP_OUT_OVERLAP_V = 0;
        AR_BIGPIC_ISP_OUT_OFFSET_H = 3968;
        AR_BIGPIC_ISP_OUT_OFFSET_V = 0;

        AR_BIGPIC_FREQ_PREVIEW_MIPI = 200000000;
        AR_BIGPIC_FREQ_PREVIEW_VIF = 400000000;
        AR_BIGPIC_FREQ_PREVIEW_HDR = 400000000;
        AR_BIGPIC_FREQ_PREVIEW_ISP = AR_BIGPIC_FREQ_PREVIEW_HDR;
        AR_BIGPIC_FREQ_BIGPIC_MIPI = 200000000;
        AR_BIGPIC_FREQ_BIGPIC_VIF = 400000000;
        AR_BIGPIC_FREQ_BIGPIC_HDR = 400000000;
        AR_BIGPIC_FREQ_BIGPIC_ISP = AR_BIGPIC_FREQ_BIGPIC_HDR;

    }else{
        ar_err("not support this res!!!");
    }
}

static int change_state_will(int state)
{
    switch(state)
    {
    case AR_BIGPIC_STATE_STOP_WILLABLE:
    case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
    case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
    case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
        server_state_will = state;

        break;
    default:
        server_state_will = AR_BIGPIC_STATE_NULL;

        break;
    }

    return server_state_will;
}

static int check_state_willed()
{
    int ret = -1;
    switch(server_state_current)
    {
    case AR_BIGPIC_STATE_STOP_WILLABLE:
    case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
    case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
    case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
        if(server_state_will == server_state_current)
        {
            server_state_will = AR_BIGPIC_STATE_NULL;
        }

        ret = 0;

        break;
    default:
        break;
    }

    return ret;
}

static int display_hal_init(STRU_CAMERA_T *p_camera, int index)
{
    int ret = 0;

    STRU_DISPLAY_OBJ_T *p_display = &p_camera->display_obj;

    p_display->dev_fd = -1;
    p_display->layer_fd = -1;
    if(ppra->u32votype == AR_BIGPIC_DISPLAY_IF_MIPI)
        ar_vo_dev_init_for_mipi(p_display, AR_BIGPIC_DISPLAY_WIDTH, AR_BIGPIC_DISPLAY_HEIGHT, -1, -1
                                , AR_BIGPIC_DISPLAY_FPS, AR_BIGPIC_DISPLAY_IF_MIPI, AR_FALSE);

    else{
        ar_vo_dev_init(p_display, AR_BIGPIC_DISPLAY_WIDTH, AR_BIGPIC_DISPLAY_HEIGHT, -1, -1);
    }

    STRU_AR_HAL_VO_RECT rect = {
        .x = 0,
        .y = 0,
        // NOTE: crash when bypass scaler
        .w = 1280, //AR_BIGPIC_DISPLAY_WIDTH,
        .h = 720, //AR_BIGPIC_DISPLAY_HEIGHT,
    };

    p_display->surface[index].chan_fd = -1;
    ret = ar_vo_surface_init(p_display, index, &rect);

    if(p_camera->big_pic_mode == 0)
    {
        p_camera->use_display = 1;
        pthread_create(&p_camera->display_thread, NULL, display_thread, p_camera);
    }

    return ret;
}

static int display_hal_deinit(STRU_CAMERA_T *p_camera, int index)
{
    int ret = 0;
    STRU_DISPLAY_OBJ_T *p_display = (STRU_DISPLAY_OBJ_T *)&p_camera->display_obj;
    STRU_AR_HAL_VO_DISP_BUF buffer = {0};

    if(p_camera->big_pic_mode == 0)
    {
        ar_always("wait the display thread exit");
        p_camera->use_display = 0;
        pthread_join(p_camera->display_thread, NULL);
        ar_always("display thread exit");
    }

    if(p_display->surface[index].chan_fd >= 0)
    {
        ar_hal_vo_chn_pause(p_display->surface[index].chan_fd);

        ar_always("wait clean all the buffer");

        ar_hal_vo_chn_clear_frame(p_display->surface[index].chan_fd, AR_TRUE);
        while(1)
        {
            memset((void*)&buffer, 0, sizeof(buffer));
            ret = ar_hal_vo_chn_get_frame(p_display->surface[index].chan_fd, &buffer, 0);
            if (ret != AR_HAL_VO_SUCCESS){
                break;
            }

            ret = ar_hal_vo_chn_release_frame(p_display->surface[index].chan_fd, &buffer);
            if (ret != AR_HAL_VO_SUCCESS){
                break;
            }
            ar_always("recycle frame");
        }

        ar_hal_vo_chn_disable(p_display->surface[index].chan_fd);
        ar_hal_vo_fd_close(p_display->surface[index].chan_fd);
        p_display->surface[index].chan_fd = -1;
    }

	if(p_display->layer_fd >= 0){
        ar_hal_vo_layer_disable(p_display->layer_fd);
        ar_hal_vo_fd_close(p_display->layer_fd);
        p_display->layer_fd = -1;
    }

    if(p_display->dev_fd >= 0){
	    ar_hal_vo_dev_disable(p_display->dev_fd);
		ar_hal_vo_fd_close(p_display->dev_fd);
        p_display->dev_fd = -1;
    }

    return 0;
}

static AR_S32 display_hal_buffer(STRU_CAMERA_T *p_camera, AR_S32 ch, VIDEO_FRAME_INFO_S *p_frame)
{
    AR_S32 ret = 0;
    STRU_AR_HAL_VO_DISP_BUF buffer = {0};
    static AR_S32 frame_id = 0;
    STRU_DISPLAY_OBJ_T *p_display = (STRU_DISPLAY_OBJ_T *)&p_camera->display_obj;

    buffer.frame_id = frame_id;
    buffer.format = AR_HAL_VO_FMT_YV12;
    buffer.frame_width = p_frame->stVFrame.u32Width;
    buffer.frame_height = p_frame->stVFrame.u32Height;
    buffer.roi.w = p_frame->stVFrame.u32Width;
    buffer.roi.h = p_frame->stVFrame.u32Height;
    buffer.roi.x = 0;
    buffer.roi.y = 0;
    buffer.luma_stride = p_frame->stVFrame.u32Stride[0];
    buffer.chroma_stride = p_frame->stVFrame.u32Stride[1];
    buffer.pannel_num = 3;
    for(AR_S32 i=0; i<buffer.pannel_num; i++)
    {
        buffer.pannel[i].buffer = (void*)p_frame->stVFrame.u64VirAddr[i];
        buffer.pannel[i].buffer_orign =  (void*)p_frame->stVFrame.u64VirAddr[i];
        buffer.pannel[i].buffer_pa = (void*)p_frame->stVFrame.u64PhyAddr[i];
        buffer.pannel[i].buffer_pa_orign =(void*)p_frame->stVFrame.u64PhyAddr[i];
        buffer.pannel[i].length = p_frame->stVFrame.u32Stride[i]*p_frame->stVFrame.u32Height;
    }
    buffer.interlace_filed_flag = 0;
    buffer.usr_data = NULL;
    ar_debug("send buffer_%d", frame_id);

    frame_id++;

    ret = ar_hal_vo_chn_send_frame(p_display->surface[ch].chan_fd, &buffer, 0);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        goto End;
    }

    memset((void*)&buffer, 0, sizeof(STRU_AR_HAL_VO_DISP_BUF));
    ret = ar_hal_vo_chn_get_frame(p_display->surface[p_camera->vo_ch_id].chan_fd, &buffer, 300);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("can't get buffer");
        goto End;
    }

    ret = ar_hal_vo_chn_release_frame(p_display->surface[p_camera->vo_ch_id].chan_fd, &buffer);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("can't get buffer");
        goto End;
    }

End:
    if (ret != AR_HAL_VO_SUCCESS)
    {
        ar_err("ret = 0x%x", ret);
    }

    return ret;
}

static void* display_thread(void *arg)
{
    VIDEO_FRAME_INFO_S FrameInfo = {0};
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_PREVIEW;
    while(p_camera->use_display)
    {
        AR_S32 ret = AR_MPI_VI_GetChnFrame(p_camera->pipe_id[yuv_stream_index], p_camera->ch_id[yuv_stream_index], &FrameInfo, -1);

        // push to display
        Display_Buffer(p_camera, p_camera->vo_ch_id, &FrameInfo);

        AR_MPI_VI_ReleaseChnFrame(p_camera->pipe_id[yuv_stream_index], p_camera->ch_id[yuv_stream_index], &FrameInfo);
    }

    return NULL;
}

static int create_stream(STRU_CAMERA_T *p_camera)
{
    if(p_camera->big_pic_mode == 0)
    {
        AR_S32 s32Ret = 0;

        // creat stream buffer
        AR_S32 s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_PREVIEW;
        g_enSnsType[s32WorkSnsId] = p_camera->enSnsType[s32WorkSnsId];

        // preview
        SIZE_S stSize = {
            .u32Width = AR_BIGPIC_PREVIEW_STREAM_WIDTH,
            .u32Height = AR_BIGPIC_PREVIEW_STREAM_HEIGHT,
        };
        ar_err("(w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

        VB_CONFIG_S stVbConf = {0};
        stVbConf.u32MaxPoolCnt = p_camera->stViConfig.s32WorkingViNum;

        AR_U32 u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height+10, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
        stVbConf.astCommPool[s32WorkSnsId].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[s32WorkSnsId].u32BlkCnt = 5*stVbConf.u32MaxPoolCnt;

        s32Ret = AR_MPI_VB_SetConfig(&stVbConf);
        if (AR_SUCCESS != s32Ret)
        {
            ar_err("AR_MPI_VB_SetConf failed!\n");
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VB_Init();
        if (AR_SUCCESS != s32Ret)
        {
            ar_err("AR_MPI_VB_Init failed!\n");
            return AR_FAILURE;
        }

    }else if(p_camera->big_pic_mode == 1)
    {
        AR_S32 s32Ret = 0;

        // creat stream buffer
        VB_CONFIG_S stVbConf = {0};
        stVbConf.u32MaxPoolCnt = 1;
        stVbConf.astCommPool[0].u64BlkSize = 4096;
        stVbConf.astCommPool[0].u32BlkCnt = 1;
        s32Ret = AR_MPI_VB_SetConfig(&stVbConf);
        if (AR_SUCCESS != s32Ret)
        {
            ar_err("AR_MPI_VB_SetConf failed!\n");
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VB_Init();
        if (AR_SUCCESS != s32Ret)
        {
            ar_err("AR_MPI_VB_Init failed!\n");
            usleep(5000000);
            return AR_FAILURE;
        }

        // yuv
        AR_S32 s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_LEFT;

        SIZE_S stSize = {
            .u32Width = p_camera->bigpic_isp_out_width,
            .u32Height = p_camera->bigpic_isp_out_height,
        };
        ar_err("small yuv (w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

        AR_U32 u32BlkSize = COMMON_GetPicBufferSize(stSize.u32Width, stSize.u32Height+10, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 512);
        AR_U64 u64BlkSize = (AR_U64)u32BlkSize*2;

        STRU_VB_POOL_CONFIG_V2_S st_vb_pool_config_v2_L = {0};
        STRU_VB_POOL_CONFIG_V2_S st_vb_pool_config_v2_R = {0};
        st_vb_pool_config_v2_L.u32_blk_cnt = MAX_STREAM_BUFER_COUNT;
        st_vb_pool_config_v2_R.u32_blk_cnt = MAX_STREAM_BUFER_COUNT;

        AR_U64 vb_blk_size = p_camera->bigpic_isp_out_stride*p_camera->bigpic_height+256;
        vb_blk_size += p_camera->bigpic_isp_out_stride*p_camera->bigpic_height/4+256;
        vb_blk_size += p_camera->bigpic_isp_out_stride*p_camera->bigpic_height/4+256;

        if(vb_blk_size < u64BlkSize)
        {
            vb_blk_size = u64BlkSize;
        }
        vb_blk_size = CAM_ALIGNE_TO(vb_blk_size, 512);
        st_vb_pool_config_v2_L.u64_blk_size = vb_blk_size;
        st_vb_pool_config_v2_R.u64_blk_size = vb_blk_size;

        st_vb_pool_config_v2_L.u64_blk_addr = (AR_U64 *)malloc(sizeof(AR_U64) * st_vb_pool_config_v2_L.u32_blk_cnt);
        st_vb_pool_config_v2_L.p_blk_vaddr = (AR_VOID *)malloc(sizeof(AR_VOID *) * st_vb_pool_config_v2_L.u32_blk_cnt);
        st_vb_pool_config_v2_R.u64_blk_addr = (AR_U64 *)malloc(sizeof(AR_U64) * st_vb_pool_config_v2_R.u32_blk_cnt);
        st_vb_pool_config_v2_R.p_blk_vaddr = (AR_VOID *)malloc(sizeof(AR_VOID *) * st_vb_pool_config_v2_R.u32_blk_cnt);

        p_camera->u64_blk_addr_L = st_vb_pool_config_v2_L.u64_blk_addr;
        p_camera->p_blk_vaddr_L = st_vb_pool_config_v2_L.p_blk_vaddr;
        p_camera->u64_blk_addr_R = st_vb_pool_config_v2_R.u64_blk_addr;
        p_camera->p_blk_vaddr_R = st_vb_pool_config_v2_R.p_blk_vaddr;

        for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
        {
            AR_VOID *p_vaddr = NULL;
            AR_U64 phy_addr = 0;
            ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, vb_blk_size);
            if(!phy_addr || !p_vaddr)
            {
                ar_err("vb too small");
                usleep(5000000);
                return -1;
            }
            memset((void*)p_vaddr, 0, vb_blk_size);

            p_camera->phy_addr[j] = phy_addr;
            p_camera->p_vaddr[j] = p_vaddr;

            void *p_vaddr_align = NULL;
            AR_U64 phy_addr_align = 0;
            p_vaddr_align = (void*)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
            phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);

            // yuv left
            st_vb_pool_config_v2_L.u64_blk_addr[j] = phy_addr_align;
            st_vb_pool_config_v2_L.p_blk_vaddr[j] = p_vaddr_align;
            ar_err("debug debug debug L Y j=%d pa=%p va=%p", j, st_vb_pool_config_v2_L.u64_blk_addr[j], st_vb_pool_config_v2_L.p_blk_vaddr[j]);
            s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_LEFT;
            // u
            AR_U64 u64PannelPaddrUL = st_vb_pool_config_v2_L.u64_blk_addr[j] + p_camera->bigpic_isp_out_stride*p_camera->bigpic_height+256;
            void *pstPannelVaddrUL = st_vb_pool_config_v2_L.p_blk_vaddr[j] + p_camera->bigpic_isp_out_stride*p_camera->bigpic_height+256;
            AR_MPI_VI_SetChnPannelAddr(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                                       p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                                       j, 1, \
                                       u64PannelPaddrUL, pstPannelVaddrUL);
            ar_err("debug debug debug L U j=%d pa=%p va=%p", j, u64PannelPaddrUL, pstPannelVaddrUL);
            // v
            AR_U64 u64PannelPaddrVL = u64PannelPaddrUL + p_camera->bigpic_isp_out_stride*p_camera->bigpic_height/4+256;
            void *pstPannelVaddrVL = pstPannelVaddrUL + p_camera->bigpic_isp_out_stride*p_camera->bigpic_height/4+256;
            AR_MPI_VI_SetChnPannelAddr(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                                       p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                                       j, 2, \
                                       u64PannelPaddrVL, pstPannelVaddrVL);
            ar_err("debug debug debug L V j=%d pa=%p va=%p", j, u64PannelPaddrVL, pstPannelVaddrVL);

            // yuv right
            st_vb_pool_config_v2_R.u64_blk_addr[j] = phy_addr_align + p_camera->bigpic_isp_out_offset_h;
            st_vb_pool_config_v2_R.p_blk_vaddr[j] = p_vaddr_align + p_camera->bigpic_isp_out_offset_h;
            ar_err("debug debug debug R Y j=%d pa=%p va=%p", j, st_vb_pool_config_v2_R.u64_blk_addr[j], st_vb_pool_config_v2_R.p_blk_vaddr[j]);
            s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_RIGHT;
            // u
            AR_U64 u64PannelPaddrUR = u64PannelPaddrUL + p_camera->bigpic_isp_out_offset_h/2;
            void *pstPannelVaddrUR = pstPannelVaddrUL + p_camera->bigpic_isp_out_offset_h/2;
            AR_MPI_VI_SetChnPannelAddr(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                                       p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                                       j, 1, \
                                       u64PannelPaddrUR, pstPannelVaddrUR);
            ar_err("debug debug debug R U j=%d pa=%p va=%p", j, u64PannelPaddrUR, pstPannelVaddrUR);
            // v
            AR_U64 u64PannelPaddrVR = u64PannelPaddrVL + p_camera->bigpic_isp_out_offset_h/2;
            void *pstPannelVaddrVR = pstPannelVaddrVL + p_camera->bigpic_isp_out_offset_h/2;
            AR_MPI_VI_SetChnPannelAddr(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                                       p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                                       j, 2, \
                                       u64PannelPaddrVR, pstPannelVaddrVR);
            ar_err("debug debug debug R V j=%d pa=%p va=%p", j, u64PannelPaddrVR, pstPannelVaddrVR);
        }

        // yuv left
        s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_LEFT;
        g_enSnsType[s32WorkSnsId] = p_camera->enSnsType[s32WorkSnsId];

        ar_err("AR_MPI_VB_CreatePool L!\n");
        AR_VB_POOL u32_pool_id_L = ar_hal_vb_create_pool_v2(&st_vb_pool_config_v2_L);
        if(AR_VB_INVALID_POOLID == u32_pool_id_L)
        {
            ar_err("AR_MPI_VB_CreatePool L failed!\n");
            usleep(5000000);
        }
        ar_err("AR_MPI_VB_CreatePool L done, id = %d\n", u32_pool_id_L);

        p_camera->u32_pool_id_L = u32_pool_id_L;

        AR_MPI_VI_SetChnPoolId(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->u32_pool_id_L);

        AR_MPI_VI_SetChnStride(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->bigpic_isp_out_stride);

        AR_MPI_VI_SetChnDisableSeqQueue(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               0);

        // yuv right
        s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_RIGHT;
        g_enSnsType[s32WorkSnsId] = p_camera->enSnsType[s32WorkSnsId];

        ar_err("AR_MPI_VB_CreatePool R!\n");
        AR_VB_POOL u32_pool_id_R = ar_hal_vb_create_pool_v2(&st_vb_pool_config_v2_R);
        if(AR_VB_INVALID_POOLID == u32_pool_id_R)
        {
            ar_err("AR_MPI_VB_CreatePool R failed!\n");
            usleep(5000000);
        }
        ar_err("AR_MPI_VB_CreatePool R done, id = %d\n", u32_pool_id_R);

        p_camera->u32_pool_id_R = u32_pool_id_R;

        AR_MPI_VI_SetChnPoolId(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->u32_pool_id_R);

        AR_MPI_VI_SetChnStride(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->bigpic_isp_out_stride);

        AR_MPI_VI_SetChnDisableSeqQueue(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               0);

        // raw
        s32WorkSnsId = AR_BIGPIC_WORKING_VI_ID_RAW;
        g_enSnsType[s32WorkSnsId] = p_camera->enSnsType[s32WorkSnsId];

        stSize.u32Width = p_camera->bigpic_raw_stream_width;
        stSize.u32Height = p_camera->bigpic_raw_stream_height;
        if(ppra->s32SensorRes == AR_BIGPIC_OV48C40_8K && ppra->s32IsFromFile == 1) // optimize mem, raw pipe is not used when loading from file
        {
            stSize.u32Width = AR_BIGPIC_PREVIEW_STREAM_WIDTH;
            stSize.u32Height = AR_BIGPIC_PREVIEW_STREAM_HEIGHT;
        }
        ar_err("sensor raw (w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

        u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height+10, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 512);

        STRU_VB_POOL_CONFIG_S st_vb_pool_config_C = {0};
        st_vb_pool_config_C.u32_blk_cnt = MAX_STREAM_BUFER_COUNT;
        ar_err("st_vb_pool_config_C.u32_blk_cnt = %d\n", st_vb_pool_config_C.u32_blk_cnt);
        st_vb_pool_config_C.u64_blk_size = u32BlkSize;
        ar_err("AR_MPI_VB_CreatePool C!\n");
        AR_VB_POOL u32_pool_id_C = ar_hal_vb_create_pool(&st_vb_pool_config_C);
        if(AR_VB_INVALID_POOLID == u32_pool_id_C)
        {
            ar_err("AR_MPI_VB_CreatePool C failed!\n");
            usleep(5000000);
        }
        ar_err("AR_MPI_VB_CreatePool C done, id = %d\n", u32_pool_id_C);

        p_camera->u32_pool_id_C = u32_pool_id_C;

        AR_MPI_VI_SetChnPoolId(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->u32_pool_id_C);

        AR_MPI_VI_SetChnStride(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                               p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                               p_camera->bigpic_isp_in_stride);
        if(ppra->s32SensorRes == AR_BIGPIC_OV48C40_8K && ppra->s32IsFromFile == 1) // optimize mem, raw pipe is not used when loading from file
        {
            AR_MPI_VI_SetChnStride(p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0], \
                                   p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn, \
                                   CAM_ALIGNE_TO(AR_BIGPIC_PREVIEW_STREAM_WIDTH*2, 256));
        }

        if(ppra->s32IsFromFile == 1)
        {
            char cur_path[256];
            getcwd(cur_path, sizeof(cur_path));
            chdir(ppra->pstrDirName);

            int file_num = 0;
            struct stat sb = {0};
            struct dirent *file = NULL;
            DIR* dir = opendir(ppra->pstrDirName);
            if(!dir)
            {
                ar_err("%s open error \n", ppra->pstrDirName);
                return -1;
            }
            while((file = readdir(dir)) != NULL)
            {
                if(strncmp(file->d_name, ".", 1) == 0)
                {
                    continue;
                }
                if(stat(file->d_name, &sb) >= 0 && S_ISDIR(sb.st_mode))
                {
                    continue;
                }

                int raw_pic_file_fd = open(file->d_name, O_RDONLY);
                if(raw_pic_file_fd < 0){
                    ar_err("%s open error \n", file->d_name);
                    continue;
                }
                close(raw_pic_file_fd);

                memcpy(p_camera->bigpic_raw_file_name[file_num], file->d_name, sizeof(file->d_name));

                ++file_num;
                if(file_num >= BIGPIC_RAW_FILE_NUM_MAX)
                {
                    ar_err("only support max %d files\n", BIGPIC_RAW_FILE_NUM_MAX);
                    break;
                }
            }
            closedir(dir);
            p_camera->bigpic_raw_file_num = file_num;
            ar_err("file raw num = %d\n", p_camera->bigpic_raw_file_num);

            if(p_camera->bigpic_raw_file_num > 0)
            {
                stSize.u32Width = p_camera->bigpic_isp_in_stride;
                stSize.u32Height = p_camera->bigpic_raw_stream_height;
                ar_err("file raw (w h)=>(%d %d)\n", stSize.u32Width, stSize.u32Height);

                u32BlkSize = VI_GetRawBufferSize(stSize.u32Width, stSize.u32Height+10, PIXEL_FORMAT_RGB_BAYER_16BPP, COMPRESS_MODE_NONE, 512);

                STRU_VB_POOL_CONFIG_S st_vb_pool_config_F = {0};
                st_vb_pool_config_F.u32_blk_cnt = p_camera->bigpic_raw_file_num;
                st_vb_pool_config_F.u64_blk_size = u32BlkSize;
                ar_err("AR_MPI_VB_CreatePool F!\n");
                AR_VB_POOL u32_pool_id_F = ar_hal_vb_create_pool(&st_vb_pool_config_F);
                if(AR_VB_INVALID_POOLID == u32_pool_id_F)
                {
                    ar_err("AR_MPI_VB_CreatePool F failed!\n");
                    usleep(5000000);
                }
                ar_err("AR_MPI_VB_CreatePool F done, id = %d\n", u32_pool_id_F);

                p_camera->u32_pool_id_F = u32_pool_id_F;

                ar_hal_vb_mmap_pool(p_camera->u32_pool_id_F);

                for(int file_num=0; file_num<p_camera->bigpic_raw_file_num; file_num++)
                {
                    int raw_pic_file_fd = open(p_camera->bigpic_raw_file_name[file_num], O_RDONLY);
                    if(raw_pic_file_fd < 0){
                        ar_err("%s open error \n", p_camera->bigpic_raw_file_name[file_num]);
                        break;
                    }

                    int vb_blk = ar_hal_vb_get_block_ex(p_camera->u32_pool_id_F, u32BlkSize, NULL, AR_VB_UID_VI);
                    AR_U64 phy_addr = ar_hal_vb_handle2physaddr(vb_blk);
                    void *p_vaddr = NULL;
                    ar_hal_vb_get_blockviraddr(p_camera->u32_pool_id_F, phy_addr, &p_vaddr);
                    ar_err("vb_blk = %d phy_addr = %p p_vaddr = %p", vb_blk, (void *)phy_addr, p_vaddr);

                    if(!phy_addr || !p_vaddr)
                    {
                        ar_err("vb too small for file_num %d ~~~~~~~~~~~~~~~~~~~~~~~", file_num);
                        usleep(5000000);
                    }
                    void *p_vaddr_align = NULL;
                    AR_U64 phy_addr_align = 0;
                    p_vaddr_align = (void*)CAM_ALIGNE_TO((AR_U64)p_vaddr, 256);
                    phy_addr_align = CAM_ALIGNE_TO(phy_addr, 256);
                    ar_err("vb_blk = %d phy_addr_align = %p p_vaddr_align = %p", vb_blk, (void *)phy_addr_align, p_vaddr_align);

                    for(int i=0; i<p_camera->bigpic_raw_stream_height; i++)
                    {
                        read(raw_pic_file_fd, p_vaddr_align+i*p_camera->bigpic_isp_in_stride, 2*p_camera->bigpic_raw_stream_width);
                    }
                    p_camera->bigpic_raw_file_buffer[file_num] = p_vaddr_align;
                    p_camera->bigpic_raw_file_buffer_phy[file_num] = phy_addr_align;
                    p_camera->bigpic_raw_file_buffer_vb_blk[file_num] = vb_blk;

                    close(raw_pic_file_fd);
                    ar_err("%s restore done\n", p_camera->bigpic_raw_file_name[file_num]);
                }
            }

            chdir(cur_path);
        }
    }

    return 0;
}

static int destroy_stream(STRU_CAMERA_T *p_camera)
{
    if(p_camera->big_pic_mode == 0)
    {

    }else if(p_camera->big_pic_mode == 1)
    {
        if(p_camera->bigpic_raw_file_num > 0)
        {
            for(int i=0; i<p_camera->bigpic_raw_file_num; i++)
            {
                VB_BLK vb_blk = p_camera->bigpic_raw_file_buffer_vb_blk[i];
                ar_hal_vb_release_block_ex(vb_blk, AR_VB_UID_VI);
            }
            p_camera->bigpic_raw_file_num = 0;

            ar_hal_vb_munmap_pool(p_camera->u32_pool_id_F);
            ar_hal_vb_destroy_pool(p_camera->u32_pool_id_F);
        }
        ar_hal_vb_destroy_pool(p_camera->u32_pool_id_C);
        ar_hal_vb_destroy_pool_v2(p_camera->u32_pool_id_R);
        ar_hal_vb_destroy_pool_v2(p_camera->u32_pool_id_L);

        for(int j=0; j<MAX_STREAM_BUFER_COUNT; j++)
        {
            ar_hal_sys_mmz_free((AR_U64)p_camera->phy_addr[j], p_camera->p_vaddr[j]);
        }

        free(p_camera->u64_blk_addr_L);
        free(p_camera->p_blk_vaddr_L);
        free(p_camera->u64_blk_addr_R);
        free(p_camera->p_blk_vaddr_R);
    }

    AR_MPI_VB_Exit();

    return 0;
}

static int start_stream(STRU_CAMERA_T *p_camera)
{
    VI_DEV_PROP_S dev_prop = {0};
    if(p_camera->big_pic_mode == 0)
    {
        dev_prop.cam_mode = VIN_CAMERA_OFFLINE;

        dev_prop.mipi_fre_mod = 1;
        dev_prop.mipi_fre_hz = AR_BIGPIC_FREQ_PREVIEW_MIPI;
        dev_prop.vif_fre_mod = 1;
        dev_prop.vif_fre_hz = AR_BIGPIC_FREQ_PREVIEW_VIF;
        dev_prop.hdr_fre_mod = 1;
        dev_prop.hdr_fre_hz = AR_BIGPIC_FREQ_PREVIEW_HDR;
        dev_prop.isp_fre_mod = 1;
        dev_prop.isp_fre_hz = AR_BIGPIC_FREQ_PREVIEW_ISP;

    }else if(p_camera->big_pic_mode == 1)
    {
        dev_prop.cam_mode=VIN_CMAERA_MULTI_MODE;

        dev_prop.mipi_fre_mod = 1;
        dev_prop.mipi_fre_hz = AR_BIGPIC_FREQ_BIGPIC_MIPI;
        dev_prop.vif_fre_mod = 1;
        dev_prop.vif_fre_hz = AR_BIGPIC_FREQ_BIGPIC_VIF;
        dev_prop.hdr_fre_mod = 1;
        dev_prop.hdr_fre_hz = AR_BIGPIC_FREQ_BIGPIC_HDR;
        dev_prop.isp_fre_mod = 1;
        dev_prop.isp_fre_hz = AR_BIGPIC_FREQ_BIGPIC_ISP;
    }
    AR_MPI_VIN_OpenDev(&dev_prop);

    /*start vi*/
    AR_S32 s32Ret = SAMPLE_COMM_VI_StartVi(&p_camera->stViConfig);
    if (AR_SUCCESS != s32Ret)
    {
        ar_err("start vi failed.s32Ret:0x%x !\n", s32Ret);
        return -1;
    }

    if(p_camera->big_pic_mode == 0)
    {
        if(p_camera->aec_out.line_count > 0)
        {
            AR_MPI_ISP_AecUpdate(p_camera->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera->aec_out);
            AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera->awb_out);
            AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW]);

        }else{
            ar_err("last p_camera_preview->aec_out.line_count <= 0");
        }

        AR_MPI_VI_TriggerPipe(p_camera->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], 1);
    }

    /*start vo*/
    if(p_camera->enable_display)
    {
        Display_Init(p_camera, p_camera->vo_ch_id);
    }

    return 0;
}

static int stop_stream(STRU_CAMERA_T *p_camera)
{
    if(p_camera->enable_display)
    {
        Display_Deinit(p_camera, p_camera->vo_ch_id);
    }

    SAMPLE_COMM_VI_StopVi(&p_camera->stViConfig);

    return 0;
}

static int open_camera(STRU_CAMERA_T *p_camera, int big_pic_mode)
{
    AR_S32 s32Ret = 0;

    // vin0, vin1, phy [N]
    // preview L, preview R, raw [Y]
    AR_S32 as32WorkingViId[3] = {AR_BIGPIC_WORKING_VI_ID_LEFT, AR_BIGPIC_WORKING_VI_ID_RIGHT, AR_BIGPIC_WORKING_VI_ID_RAW};
    VI_DEV MipiDev[3]         = {0, 0, 0};
    VI_DEV ViDev[3]           = {0, 1, 2};
    VI_PIPE ViPipe[3]         = {0, 1, 2};
    VI_CHN ViChn[3]           = {0, 0, 2};
    AR_S32 s32BusId[3]        = {AR_BIGPIC_I2C_BUS_ID, AR_BIGPIC_I2C_BUS_ID, AR_BIGPIC_I2C_BUS_ID};
    AR_S32 enPixFormat[3]     = {PIXEL_FORMAT_YVU_PLANAR_420, PIXEL_FORMAT_YVU_PLANAR_420, PIXEL_FORMAT_RGB_BAYER_12BPP};

    p_camera->big_pic_mode = big_pic_mode;

    if(p_camera->big_pic_mode == 1)
    {
        p_camera->stViConfig.s32WorkingViNum = 3;

        // p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_SIM;
        // p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_SIM;
        // p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_RAW;
        if(ppra->s32IsHdrOn)
        {
            p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_SIM_HDR;
            p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_SIM_HDR_R;
            p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_HDR_RAW;


            p_camera->bigpic_raw_stream_width = AR_BIGPIC_RAW_STREAM_WIDTH_HDR;
            p_camera->bigpic_raw_stream_height = AR_BIGPIC_RAW_STREAM_HEIGHT_HDR;
            p_camera->bigpic_isp_in_stride = AR_BIGPIC_ISP_IN_STRIDE_HDR;

            p_camera->bigpic_isp_in_width = AR_BIGPIC_ISP_IN_WIDTH_HDR;
            p_camera->bigpic_isp_in_height = AR_BIGPIC_ISP_IN_HEIGHT_HDR;
            p_camera->bigpic_isp_in_overlap_h = AR_BIGPIC_ISP_IN_OVERLAP_H_HDR;
            p_camera->bigpic_isp_in_overlap_h_hdr_gap = AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP_HDR;
            p_camera->bigpic_isp_in_overlap_v = AR_BIGPIC_ISP_IN_OVERLAP_V_HDR;
            p_camera->bigpic_isp_in_offset_h = AR_BIGPIC_ISP_IN_OFFSET_H_HDR;
            p_camera->bigpic_isp_in_offset_v = AR_BIGPIC_ISP_IN_OFFSET_V_HDR;


        }else{
            p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_SIM_LINE;
            p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_SIM_LINE;
            p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_LINE;


            p_camera->bigpic_raw_stream_width = AR_BIGPIC_RAW_STREAM_WIDTH;
            p_camera->bigpic_raw_stream_height = AR_BIGPIC_RAW_STREAM_HEIGHT;
            p_camera->bigpic_isp_in_stride = AR_BIGPIC_ISP_IN_STRIDE;

            p_camera->bigpic_isp_in_width = AR_BIGPIC_ISP_IN_WIDTH;
            p_camera->bigpic_isp_in_height = AR_BIGPIC_ISP_IN_HEIGHT;
            p_camera->bigpic_isp_in_overlap_h = AR_BIGPIC_ISP_IN_OVERLAP_H;
            p_camera->bigpic_isp_in_overlap_h_hdr_gap = AR_BIGPIC_ISP_IN_OVERLAP_H_HDR_GAP;
            p_camera->bigpic_isp_in_overlap_v = AR_BIGPIC_ISP_IN_OVERLAP_V;
            p_camera->bigpic_isp_in_offset_h = AR_BIGPIC_ISP_IN_OFFSET_H;
            p_camera->bigpic_isp_in_offset_v = AR_BIGPIC_ISP_IN_OFFSET_V;
        }

        p_camera->bigpic_width = AR_BIGPIC_WIDTH;
        p_camera->bigpic_height = AR_BIGPIC_HEIGHT;
        p_camera->bigpic_isp_out_stride = AR_BIGPIC_ISP_OUT_STRIDE;

        p_camera->bigpic_isp_out_width = AR_BIGPIC_ISP_OUT_WIDTH;
        p_camera->bigpic_isp_out_height = AR_BIGPIC_ISP_OUT_HEIGHT;
        p_camera->bigpic_isp_out_overlap_h = AR_BIGPIC_ISP_OUT_OVERLAP_H;
        p_camera->bigpic_isp_out_overlap_v = AR_BIGPIC_ISP_OUT_OVERLAP_V;
        p_camera->bigpic_isp_out_offset_h = AR_BIGPIC_ISP_OUT_OFFSET_H;
        p_camera->bigpic_isp_out_offset_v = AR_BIGPIC_ISP_OUT_OFFSET_V;


        p_camera->enable_display = AR_BIGPIC_TO_DISPLAY;

    }else
    {
        p_camera->stViConfig.s32WorkingViNum = 1;

        // p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_PHY_PREVIEW;
        // p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_PHY_PREVIEW;
        // p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_PREVIEW;
        if(ppra->s32IsHdrOn)
        {
            p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW;
            p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW;
            p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_HDR_PREVIEW;

        }else{
            p_camera->enSnsType[0] = AR_BIGPIC_SENSOR_TYPE_PHY_LINE;
            p_camera->enSnsType[1] = AR_BIGPIC_SENSOR_TYPE_PHY_LINE;
            p_camera->enSnsType[2] = AR_BIGPIC_SENSOR_TYPE_PHY_LINE;
        }

        p_camera->enable_display = 1;
    }
    ar_err("big_pic_mode = %d", p_camera->big_pic_mode);
    ar_err("enSnsType[0] = %d, enSnsType[1] = %d, enSnsType[2] = %d", p_camera->enSnsType[0], p_camera->enSnsType[1], p_camera->enSnsType[2]);

    for(AR_S32 s32WorkSnsId=0; s32WorkSnsId<p_camera->stViConfig.s32WorkingViNum; s32WorkSnsId++)
    {
        p_camera->stViConfig.as32WorkingViId[s32WorkSnsId] = as32WorkingViId[s32WorkSnsId];

        p_camera->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.enSnsType = p_camera->enSnsType[s32WorkSnsId];
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32SnsId  = s32WorkSnsId;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.s32BusId  = s32BusId[s32WorkSnsId];
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stSnsInfo.MipiDev   = MipiDev[s32WorkSnsId];

        p_camera->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.ViDev     = ViDev[s32WorkSnsId];
        if(ppra->s32IsHdrOn)
        {
            p_camera->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_2To1_LINE;

        }else{
            p_camera->stViConfig.astViInfo[s32WorkSnsId].stDevInfo.enWDRMode = WDR_MODE_NONE;
        }

        p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.enMastPipeMode = VI_OFFLINE_VPSS_OFFLINE;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[0]       = ViPipe[s32WorkSnsId];
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[1]       = -1;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[2]       = -1;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stPipeInfo.aPipe[3]       = -1;

        p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.ViChn          = ViChn[s32WorkSnsId];
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enPixFormat    = enPixFormat[s32WorkSnsId];
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enDynamicRange = DYNAMIC_RANGE_SDR8;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enVideoFormat  = VIDEO_FORMAT_LINEAR;
        p_camera->stViConfig.astViInfo[s32WorkSnsId].stChnInfo.enCompressMode = COMPRESS_MODE_NONE;

        p_camera->pipe_id[s32WorkSnsId] = ViPipe[s32WorkSnsId];
        p_camera->ch_id[s32WorkSnsId]   = ViChn[s32WorkSnsId];
    }

    return 0;
}

static int close_camera(STRU_CAMERA_T *p_camera)
{
    return 0;
}

static int take_still(STRU_CAMERA_T *p_camera)
{
    ar_always("debug debug debug take still entry");

    int raw_stream_index = AR_BIGPIC_WORKING_VI_ID_RAW;
    int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;

    if(ppra->s32CameraMode == AR_BIGPIC_CAMERA_MODE_VIDEO)
    {
        p_camera->big_pic_3a_auto = 1;

    }else
    {
        p_camera->big_pic_3a_auto = 0;
    }

    if(p_camera->big_pic_3a_auto)
    {
        p_camera->big_pic_stop = 0;
        p_camera->big_pic_3a_auto_runing = 1;
        pthread_create(&p_camera->big_pic_3a_auto_thread, NULL, big_pic_3a_auto_thread, p_camera);
    }

    if(ppra->s32IsFromFile == 0)
    {
        ar_always("debug debug debug 3a update pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
        ar_always("debug debug debug AR_MPI_ISP_AecUpdate real_gain %f exp_index %d", p_camera->aec_out.real_gain, p_camera->aec_out.exp_index);
        AR_MPI_ISP_AecUpdate(p_camera->pipe_id[raw_stream_index], &p_camera->aec_out);
        // entry sof mode
        STRU_AR_CAMERA_SOF_PRO_T sof_prop;
        sof_prop.sof_mode = 1;
        sof_prop.to_trigger = 1;
        AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
        // // exit sof mode
        // sof_prop.sof_mode = 0;
        // sof_prop.to_trigger = 0;
        // AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
        // trigger on
        AR_MPI_VI_TriggerPipe(p_camera->pipe_id[raw_stream_index], 1);
    }
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
        AR_MPI_ISP_AecUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->aec_out);
        AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->awb_out);
        AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);
    }

    VI_CROP_INFO_S stCropInfo = {0};
    stCropInfo.bEnable = 1;
    stCropInfo.stCropRect.s32X = 0;
    stCropInfo.stCropRect.s32Y = 0;
    stCropInfo.stCropRect.u32Width = p_camera->bigpic_isp_out_width;
    stCropInfo.stCropRect.u32Height = p_camera->bigpic_isp_out_height;

    // get raw buffer
    int file_num = 0;
    for(int raw_pic_frame_id=0; raw_pic_frame_id<ppra->s32Num || p_camera->big_pic_3a_auto_runing; raw_pic_frame_id++)
    {
        if(raw_pic_frame_id >= ppra->s32Num)
        {
            if(p_camera->big_pic_3a_auto)
            {
                p_camera->big_pic_stop = 1;
            }
        }

        ar_always("debug debug debug start raw_pic_frame_id=%d ========================", raw_pic_frame_id);

        VIDEO_FRAME_INFO_S FrameInfoRaw = {0};
        VIDEO_FRAME_INFO_S *p_stream_buffer_raw = &FrameInfoRaw;
        if(ppra->s32IsFromFile == 0)
        {
            ar_always("debug debug debug GetChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
            AR_MPI_VI_GetChnFrame(p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index], p_stream_buffer_raw, -1);
            ar_always("debug debug debug u32PoolId=%d, enModId=%d", p_stream_buffer_raw->u32PoolId, p_stream_buffer_raw->enModId);
            ar_always("debug debug debug u32FrameId=%d, u32Width=%d, u32Height=%d", p_stream_buffer_raw->stVFrame.u32FrameId,p_stream_buffer_raw->stVFrame.u32Width,p_stream_buffer_raw->stVFrame.u32Height);

        }else{
            if(file_num >= p_camera->bigpic_raw_file_num)
            {
                file_num = 0;
            }

            p_stream_buffer_raw->stVFrame.u32Width = p_camera->bigpic_raw_stream_width;
            p_stream_buffer_raw->stVFrame.u32Height = p_camera->bigpic_raw_stream_height;
            p_stream_buffer_raw->stVFrame.u32Stride[0] = p_camera->bigpic_isp_in_stride;
            p_stream_buffer_raw->stVFrame.u32Stride[1] = 0;
            p_stream_buffer_raw->stVFrame.u32Stride[2] = 0;
            p_stream_buffer_raw->stVFrame.u64PhyAddr[0] = p_camera->bigpic_raw_file_buffer_phy[file_num];
            p_stream_buffer_raw->stVFrame.u64PhyAddr[1] = NULL;
            p_stream_buffer_raw->stVFrame.u64PhyAddr[2] = NULL;
            p_stream_buffer_raw->stVFrame.u64VirAddr[0] = p_camera->bigpic_raw_file_buffer[file_num];
            p_stream_buffer_raw->stVFrame.u64VirAddr[1] = NULL;
            p_stream_buffer_raw->stVFrame.u64VirAddr[2] = NULL;

            file_num++;
        }

        // process raw buffer
        if(AR_BIGPIC_TO_FILE_RAW && raw_pic_frame_id<ppra->s32nframes)
        {
            char raw_file_name[64] = {0};
            sprintf(raw_file_name, "/mnt/raw_save_big_%d.raw", raw_pic_frame_id);
            int raw_pic_file_fd = open(raw_file_name, O_RDWR|O_CREAT);
            if(raw_pic_file_fd < 0){
                ar_err("%s open file error\n", raw_file_name);
                return -1;
            }

            ar_always("debug debug debug stride=%d, AR_BIGPIC_ISP_IN_STRIDE=%d",p_stream_buffer_raw->stVFrame.u32Stride[0], p_camera->bigpic_isp_in_stride);
            for(int i=0; i<p_stream_buffer_raw->stVFrame.u32Height; i++)
            {
                write(raw_pic_file_fd,
                      p_stream_buffer_raw->stVFrame.u64VirAddr[0]+i*p_stream_buffer_raw->stVFrame.u32Stride[0],
                      p_stream_buffer_raw->stVFrame.u32Width*2);
            }

            close(raw_pic_file_fd);
            ar_err("%s save done\n", raw_file_name);
        }
        VIDEO_FRAME_INFO_S FrameInfoYuv[AR_BIGPIC_OFFSET_NUM] = {0};
        for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM;)
        {
            ar_always("debug debug debug start small_pic_frame_id=%d ++++++++++++++++++++++++", small_pic_frame_id);

            int small_pic_pixel_offset_h = 0;
            int small_pic_pixel_offset_v = 0;
            int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
            int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
            int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
            if(small_pic_num_h > 0)
            {
                small_pic_pixel_offset_h = (p_camera->bigpic_isp_in_width-p_camera->bigpic_isp_in_overlap_h)*small_pic_num_h-p_camera->bigpic_isp_in_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                small_pic_pixel_offset_v = (p_camera->bigpic_isp_in_height-p_camera->bigpic_isp_in_overlap_v)*small_pic_num_v-p_camera->bigpic_isp_in_overlap_v;
            }
            int small_pic_byte_offset_h = small_pic_pixel_offset_h*2;
            int small_pic_byte_offset_v = small_pic_pixel_offset_v*p_camera->bigpic_isp_in_stride;
            int small_pic_byte_offset = small_pic_byte_offset_v+small_pic_byte_offset_h;

            stCropInfo.stCropRect.s32X = 0;
            stCropInfo.stCropRect.s32Y = 0;
            if(small_pic_num_h > 0)
            {
                stCropInfo.stCropRect.s32X = (p_camera->bigpic_isp_in_overlap_h-p_camera->bigpic_isp_in_overlap_h_hdr_gap)-p_camera->bigpic_isp_out_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                stCropInfo.stCropRect.s32Y = p_camera->bigpic_isp_in_overlap_v-p_camera->bigpic_isp_out_overlap_v;
            }
            ar_always("debug debug debug SetChnCropManual PROC pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            ar_always("debug debug debug SetChnCropManual PROC s32X=%d, s32Y=%d", stCropInfo.stCropRect.s32X, stCropInfo.stCropRect.s32Y);
            AR_MPI_VI_SetChnCropManual(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stCropInfo);

            // ar_always("debug debug debug 3a update pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            // ar_always("debug debug debug AR_MPI_ISP_AecUpdate real_gain %f exp_index %d", p_camera->aec_out.real_gain, p_camera->aec_out.exp_index);
            // ar_always("debug debug debug AR_MPI_ISP_AwbUpdate r_gain %f, g_gain %f, b_gain %f", p_camera->awb_out.r_gain, p_camera->awb_out.g_gain, p_camera->awb_out.b_gain);
            // AR_MPI_ISP_AecUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->aec_out);
            // AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->awb_out);
            // AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);

            VIDEO_FRAME_INFO_S VideoFrameQ = {0};
            VIDEO_FRAME_INFO_S *pstVideoFrameQ = &VideoFrameQ;
            memcpy(pstVideoFrameQ, p_stream_buffer_raw, sizeof(VIDEO_FRAME_INFO_S));
            pstVideoFrameQ->stVFrame.u64PhyAddr[0] = p_stream_buffer_raw->stVFrame.u64PhyAddr[0]+small_pic_byte_offset;
            pstVideoFrameQ->stVFrame.u64PhyAddr[1] = 0;
            pstVideoFrameQ->stVFrame.u64PhyAddr[2] = 0;
            pstVideoFrameQ->stVFrame.u64VirAddr[0] = p_stream_buffer_raw->stVFrame.u64VirAddr[0]+small_pic_byte_offset;
            pstVideoFrameQ->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
            pstVideoFrameQ->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
            pstVideoFrameQ->stVFrame.u32Stride[0] = p_camera->bigpic_isp_in_stride;
            pstVideoFrameQ->stVFrame.u32Stride[1] = 0;
            pstVideoFrameQ->stVFrame.u32Stride[2] = 0;
            pstVideoFrameQ->stVFrame.u32Width = p_camera->bigpic_isp_in_width;
            pstVideoFrameQ->stVFrame.u32Height = p_camera->bigpic_isp_in_height;
            pstVideoFrameQ->stVFrame.u64PTS = 0;
            pstVideoFrameQ->stVFrame.u64PrivateData = (AR_U64)(NULL);
            if(ppra->s32IsFromFile == 0)
                pstVideoFrameQ->u32PoolId = p_camera->u32_pool_id_C;
            else
                pstVideoFrameQ->u32PoolId = p_camera->u32_pool_id_F;
            ar_always("debug debug debug QueryChnStatus pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            VI_CHN_STATUS_S stChnStatus = {0};
            AR_MPI_VI_QueryChnStatus(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stChnStatus);
            while(stChnStatus.u32FredomBufCount<1)
            {
                usleep(10000);
                AR_MPI_VI_QueryChnStatus(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stChnStatus);
            }
            ar_always("debug debug debug SendPipeRaw pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            AR_MPI_VI_SendPipeRaw(p_camera->pipe_id[small_pic_num_abs], pstVideoFrameQ, -1);
            ar_always("debug debug debug u32PoolId=%d, enModId=%d", pstVideoFrameQ->u32PoolId, pstVideoFrameQ->enModId);
            ar_always("debug debug debug u32FrameId=%d, u32Width=%d, u32Height=%d", pstVideoFrameQ->stVFrame.u32FrameId,pstVideoFrameQ->stVFrame.u32Width,pstVideoFrameQ->stVFrame.u32Height);

            // get yuv buffer
            VIDEO_FRAME_INFO_S *p_stream_buffer_yuv = &FrameInfoYuv[small_pic_frame_id];
            ar_always("debug debug debug GetChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            AR_MPI_VI_GetChnFrame(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], p_stream_buffer_yuv, -1);

            int yuv_pic_pixel_offset_h = 0;
            int yuv_pic_pixel_offset_v = 0;
            if(small_pic_num_h > 0)
            {
                yuv_pic_pixel_offset_h = (p_camera->bigpic_isp_out_width-p_camera->bigpic_isp_out_overlap_h)*small_pic_num_h-p_camera->bigpic_isp_out_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                yuv_pic_pixel_offset_v = (p_camera->bigpic_isp_out_height-p_camera->bigpic_isp_out_overlap_v)*small_pic_num_v-p_camera->bigpic_isp_out_overlap_v;
            }
            int yuv_pic_byte_offset_h = yuv_pic_pixel_offset_h;
            int yuv_pic_byte_offset_v = yuv_pic_pixel_offset_v*p_stream_buffer_yuv->stVFrame.u32Stride[0];
            int yuv_pic_byte_offset_y = yuv_pic_byte_offset_v+yuv_pic_byte_offset_h;
            int yuv_pic_byte_offset_uv = yuv_pic_byte_offset_v/4+yuv_pic_byte_offset_h/2;

            AR_VOID *addr_pa_y = p_stream_buffer_yuv->stVFrame.u64PhyAddr[0] - yuv_pic_byte_offset_y;
            AR_VOID *addr_pa_u = p_stream_buffer_yuv->stVFrame.u64PhyAddr[1] - yuv_pic_byte_offset_uv;
            AR_VOID *addr_pa_v = p_stream_buffer_yuv->stVFrame.u64PhyAddr[2] - yuv_pic_byte_offset_uv;
            AR_VOID *addr_va_y = p_stream_buffer_yuv->stVFrame.u64VirAddr[0] - yuv_pic_byte_offset_y;
            AR_VOID *addr_va_u = p_stream_buffer_yuv->stVFrame.u64VirAddr[1] - yuv_pic_byte_offset_uv;
            AR_VOID *addr_va_v = p_stream_buffer_yuv->stVFrame.u64VirAddr[2] - yuv_pic_byte_offset_uv;
            AR_VOID *addr_y = addr_va_y + yuv_pic_byte_offset_y;
            AR_VOID *addr_u = addr_va_u + yuv_pic_byte_offset_uv;
            AR_VOID *addr_v = addr_va_v + yuv_pic_byte_offset_uv;
            ar_always("debug debug debug pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            ar_always("debug debug debug yuv_pic_byte_offset_y=%d, yuv_pic_byte_offset_uv=%d", yuv_pic_byte_offset_y, yuv_pic_byte_offset_uv);
            ar_always("debug debug debug big y pa=%p, va=%p", addr_pa_y, addr_va_y);
            ar_always("debug debug debug big u pa=%p, va=%p", addr_pa_u, addr_va_u);
            ar_always("debug debug debug big v pa=%p, va=%p", addr_pa_v, addr_va_v);
            ar_always("debug debug debug small y va=%p u va=%p v va=%p", addr_y, addr_u, addr_v);

            if(AR_BIGPIC_TO_FILE_YUV_SMALL && raw_pic_frame_id<ppra->s32nframes)
            {
                char small_file_name[64] = {0};
                sprintf(small_file_name, "/mnt/yuv_save_small_%d_%d.yuv", raw_pic_frame_id, small_pic_frame_id);
                int small_pic_file_fd = open(small_file_name, O_RDWR|O_CREAT);
                if(small_pic_file_fd < 0){
                    ar_err("%s open file error\n", small_file_name);
                    return -1;
                }

                for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height; i++)
                {
                    write(small_pic_file_fd, addr_y+i*p_stream_buffer_yuv->stVFrame.u32Stride[0], p_stream_buffer_yuv->stVFrame.u32Width);
                }
                for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height/2; i++)
                {
                    write(small_pic_file_fd, addr_u+i*p_stream_buffer_yuv->stVFrame.u32Stride[0]/2, p_stream_buffer_yuv->stVFrame.u32Width/2);
                }
                for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height/2; i++)
                {
                    write(small_pic_file_fd, addr_v+i*p_stream_buffer_yuv->stVFrame.u32Stride[0]/2, p_stream_buffer_yuv->stVFrame.u32Width/2);
                }

                close(small_pic_file_fd);
                ar_err("%s save done\n", small_file_name);
            }

            small_pic_frame_id++;
            // process big yuv buffer
            if(small_pic_frame_id >= AR_BIGPIC_OFFSET_NUM)
            {
                VIDEO_FRAME_INFO_S FrameInfoBig = {0};
                VIDEO_FRAME_INFO_S *p_stream_buffer_big = &FrameInfoBig;
                memcpy((void*)p_stream_buffer_big, (void*)p_stream_buffer_yuv, sizeof(VIDEO_FRAME_INFO_S));
                p_stream_buffer_big->u32PoolId = p_camera->u32_pool_id_L;
                p_stream_buffer_big->stVFrame.u32Width = p_camera->bigpic_width;
                p_stream_buffer_big->stVFrame.u32Height = p_camera->bigpic_height;
                p_stream_buffer_big->stVFrame.u32Stride[0] = p_stream_buffer_yuv->stVFrame.u32Stride[0];
                p_stream_buffer_big->stVFrame.u32Stride[1] = p_stream_buffer_yuv->stVFrame.u32Stride[0]/2;
                p_stream_buffer_big->stVFrame.u32Stride[2] = p_stream_buffer_yuv->stVFrame.u32Stride[0]/2;
                p_stream_buffer_big->stVFrame.u64PhyAddr[0] = addr_pa_y;
                p_stream_buffer_big->stVFrame.u64PhyAddr[1] = addr_pa_u;
                p_stream_buffer_big->stVFrame.u64PhyAddr[2] = addr_pa_v;
                p_stream_buffer_big->stVFrame.u64VirAddr[0] = addr_va_y;
                p_stream_buffer_big->stVFrame.u64VirAddr[1] = addr_va_u;
                p_stream_buffer_big->stVFrame.u64VirAddr[2] = addr_va_v;

                // display big yuv
                if(p_camera->enable_display)
                {
                    Display_Buffer(p_camera, p_camera->vo_ch_id, p_stream_buffer_big);
                }

                // save big yuv
                if(AR_BIGPIC_TO_FILE_YUV_BIG && raw_pic_frame_id<ppra->s32nframes)
                {
                    char yuv_file_name[64] = {0};
                    sprintf(yuv_file_name, "/mnt/yuv_save_big_%d.yuv", raw_pic_frame_id);
                    int yuv_pic_file_fd = open(yuv_file_name, O_RDWR|O_CREAT);
                    if(yuv_pic_file_fd < 0){
                        ar_err("%s open file error\n", yuv_file_name);
                        return -1;
                    }

                    for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height; i++)
                    {
                        write(yuv_pic_file_fd, addr_va_y+i*p_stream_buffer_big->stVFrame.u32Stride[0], p_stream_buffer_big->stVFrame.u32Width);
                    }
                    for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height/2; i++)
                    {
                        write(yuv_pic_file_fd, addr_va_u+i*p_stream_buffer_big->stVFrame.u32Stride[1], p_stream_buffer_big->stVFrame.u32Width/2);
                    }
                    for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height/2; i++)
                    {
                        write(yuv_pic_file_fd, addr_va_v+i*p_stream_buffer_big->stVFrame.u32Stride[2], p_stream_buffer_big->stVFrame.u32Width/2);
                    }

                    close(yuv_pic_file_fd);
                    ar_err("%s save done\n", yuv_file_name);
                }

                ar_always("debug debug debug ReleaseChnFrame all yuv");
                for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
                {
                    int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
                    ar_always("debug debug debug ReleaseChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
                    AR_MPI_VI_ReleaseChnFrame(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &FrameInfoYuv[small_pic_frame_id]);
                }
            }

            ar_always("debug debug debug end small_pic_frame_id=%d ++++++++++++++++++++++++", small_pic_frame_id-1);
        }

        ar_always("debug debug debug ReleaseChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
        AR_MPI_VI_ReleaseChnFrame(p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index], p_stream_buffer_raw);

        ar_always("debug debug debug end raw_pic_frame_id=%d ========================", raw_pic_frame_id);
    }

    // cancel manual crop
    stCropInfo.bEnable = 0;
    stCropInfo.stCropRect.s32X = 0;
    stCropInfo.stCropRect.s32Y = 0;
    stCropInfo.stCropRect.u32Width = 0;
    stCropInfo.stCropRect.u32Height = 0;
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
        ar_always("debug debug debug SetChnCropManual CANCEL pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
        AR_MPI_VI_SetChnCropManual(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stCropInfo);
    }

    if(ppra->s32IsFromFile == 0)
    {
        // exit sof mode
        STRU_AR_CAMERA_SOF_PRO_T sof_prop;
        sof_prop.sof_mode = 0;
        sof_prop.to_trigger = 0;
        AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
    }

    if(p_camera->big_pic_3a_auto)
    {
        pthread_join(p_camera->big_pic_3a_auto_thread, NULL);
    }

    ar_always("debug debug debug take still exit");

    return 0;
}

static void* take_still_thread(void *arg)
{
    char *p_cmd = "--take_still";

    STRU_CAMERA_T *p_camera_preview = (STRU_CAMERA_T *)arg;

    STRU_CAMERA_T *p_camera_big_pic = malloc(sizeof(STRU_CAMERA_T));
    memset((void*)p_camera_big_pic, 0, sizeof(STRU_CAMERA_T));

    open_camera(p_camera_big_pic, 1);

    int take_still_count = ppra->s32Count;
    while(1)
    {
        // start preview
        create_stream(p_camera_preview);
        start_stream(p_camera_preview);
        // preview 10s
        usleep(10000000);

        if(take_still_count-- <= 0)
        {
            p_cmd = "--exit_demo";
        }

        if(!strcmp(p_cmd, "--take_still"))
        {
            ar_err("entry take still");

            AR_MPI_ISP_GetAecUpdate(p_camera_preview->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera_preview->aec_out);
            AR_MPI_ISP_GetAwbUpdate(p_camera_preview->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera_preview->awb_out);
            p_camera_big_pic->aec_out = p_camera_preview->aec_out;
            p_camera_big_pic->awb_out = p_camera_preview->awb_out;
            ar_always("debug debug debug AR_MPI_ISP_GetAecUpdate real_gain %f exp_index %d", p_camera_big_pic->aec_out.real_gain, p_camera_big_pic->aec_out.exp_index);
            ar_always("debug debug debug AR_MPI_ISP_GetAwbUpdate r_gain %f, g_gain %f, b_gain %f", p_camera_big_pic->awb_out.r_gain, p_camera_big_pic->awb_out.g_gain, p_camera_big_pic->awb_out.b_gain);

            // stop preview
            stop_stream(p_camera_preview);
            destroy_stream(p_camera_preview);

            // start big pic
            create_stream(p_camera_big_pic);
            start_stream(p_camera_big_pic);

            // take still
            take_still(p_camera_big_pic);

            // stop big pic
            stop_stream(p_camera_big_pic);
            destroy_stream(p_camera_big_pic);

            ar_err("exit take still");

        }else if(!strcmp(p_cmd, "--exit_demo"))
        {
            stop_stream(p_camera_preview);
            destroy_stream(p_camera_preview);

            ar_signal(p_camera_preview->exit_signal);
            break;
        }
    }

    close_camera(p_camera_big_pic);

    free(p_camera_big_pic);

    return NULL;
}

static int big_pic_print_aec_input(ISP_AE_STATISTICS_S *pstAeStat)
{
    STRU_MESH_GRID_STATS_DATA_T *pstStatsData = &pstAeStat->stAecStats.stats_data;
    AR_U32 r_sum = 0;
    AR_U32 gr_sum = 0;
    AR_U32 gb_sum = 0;
    AR_U32 b_sum = 0;
    AR_U32 r_num = 0;
    AR_U32 gr_num = 0;
    AR_U32 gb_num = 0;
    AR_U32 b_num = 0;

    for(int m=0; m<BLOCK_COL; m++)
    {
        for(int n=0; n<BLOCK_ROW; n++)
        {
            AR_U32 index = n*BLOCK_COL+m;
            r_sum += pstStatsData->p_mesh_grid_stats_r_sum[index];
            gr_sum += pstStatsData->p_mesh_grid_stats_gr_sum[index];
            gb_sum += pstStatsData->p_mesh_grid_stats_gb_sum[index];
            b_sum += pstStatsData->p_mesh_grid_stats_b_sum[index];
            r_num += pstStatsData->p_mesh_grid_stats_r_num[index];
            gr_num += pstStatsData->p_mesh_grid_stats_gr_num[index];
            gb_num += pstStatsData->p_mesh_grid_stats_gb_num[index];
            b_num += pstStatsData->p_mesh_grid_stats_b_num[index];
        }
    }

    ar_err("++++++++++++++++++++");
    ar_err("TOTAL  r_sum=%d gr_sum=%d gb_sum=%d b_sum=%d", r_sum, gr_sum, gb_sum, b_sum);
    ar_err("TOTAL  r_num=%d gr_num=%d gb_num=%d b_num=%d", r_num, gr_num, gb_num, b_num);
    ar_err("TOTAL  r_sum/r_num=%d gr_sum/gr_num=%d gb_sum/gb_num=%d b_sum/b_num=%d", r_sum/(r_num+1), gr_sum/(gr_num+1), gb_sum/(gb_num+1), b_sum/(b_num+1));
    ar_err("++++++++++++++++++++");
}
static int big_pic_print_aec_output(STRU_AEC_OUT_T *aec_out)
{
    ar_err("--------------------");
    ar_err("updated = %d", aec_out->updated);
    ar_err("line_count = %d", aec_out->line_count);
    ar_err("exp_time_us = %d", aec_out->exp_time_us);
    ar_err("real_gain = %f", aec_out->real_gain);
    ar_err("sensor_gain = %f", aec_out->sensor_gain);
    ar_err("isp_gainl = %f", aec_out->isp_gainl);
    ar_err("isp_gain2 = %f", aec_out->isp_gain2);
    ar_err("exp_gain = %f", aec_out->exp_gain);
    ar_err("exp_index = %d", aec_out->exp_index);
    ar_err("lux_index = %d", aec_out->lux_index);
    ar_err("lux_value = %d", aec_out->lux_index);
    ar_err("ae_settle = %d", aec_out->ae_settle);
    ar_err("current_luma = %f", aec_out->current_luma);
    //ration  = %d",  long/short
    ar_err("short_exp_index = %d", aec_out->short_exp_index);
    for(int i=0; i<4; i++)
    {
        ar_err("ration_gain[%d] = %f", i, aec_out->ration_gain[i]);
        ar_err("ration_time[%d] = %f", i, aec_out->ration_time[i]);
        ar_err("total_raion[%d] = %f", i, aec_out->total_raion[i]);
    }
    ar_err("drc_gain = %f", aec_out->drc_gain);
    ar_err("drc_gain_1 = %f", aec_out->drc_gain_1);
    ar_err("drc_gain_2 = %f", aec_out->drc_gain_2);
    ar_err("fps = %f", aec_out->fps);
    ar_err("--------------------");

    return 0;
}
static int big_pic_aec_lib_output_2_aec_output(STRU_AEC_ALGO_LIB_OUTPUT_T *aec_lib_out, STRU_AEC_OUT_T *aec_out)
{
    aec_out->updated = aec_lib_out->updated;
    aec_out->line_count = aec_lib_out->line_count;
    aec_out->exp_time_us = aec_lib_out->exp_time_us;
    aec_out->real_gain = aec_lib_out->real_gain;
    aec_out->sensor_gain = aec_lib_out->sensor_gain;
    aec_out->isp_gainl = aec_lib_out->isp_gainl;
    aec_out->isp_gain2 = aec_lib_out->isp_gain2;
    aec_out->exp_gain = aec_lib_out->exp_gain;
    aec_out->exp_index = aec_lib_out->exp_index;
    aec_out->lux_index = aec_lib_out->lux_index;
    aec_out->lux_value = aec_lib_out->lux_index;
    aec_out->ae_settle = aec_lib_out->ae_settle;
    aec_out->current_luma = aec_lib_out->current_luma;
    //ration  =  long/short
    aec_out->short_exp_index = aec_lib_out->short_exp_index;
    for(int i=0; i<4; i++)
    {
        aec_out->ration_gain[i] = aec_lib_out->ration_gain[i];
        aec_out->ration_time[i] = aec_lib_out->ration_time[i];
        aec_out->total_raion[i] = aec_lib_out->total_raion[i];
    }
    aec_out->drc_gain = aec_lib_out->drc_gain;
    aec_out->drc_gain_1 = aec_lib_out->drc_gain_1;
    aec_out->drc_gain_2 = aec_lib_out->drc_gain_2;
    aec_out->fps = aec_lib_out->fps;

    return 0;
}
static void big_pic_statistics_ae_downsampling(ISP_AE_STATISTICS_S **ppstAeStat)
{
    ISP_AE_STATISTICS_S *pstAeStat = *ppstAeStat;
    STRU_MESH_GRID_STATS_DATA_T *pstStatsData = &pstAeStat->stAecStats.stats_data;

    static int enable_print = 0;
    for(int m=0; m<BLOCK_COL/AR_BIGPIC_OFFSET_NUM_H; m++)
    {
        for(int n=0; n<BLOCK_ROW/AR_BIGPIC_OFFSET_NUM_V; n++)
        {
            AR_U32 r_sum = 0;
            AR_U32 gr_sum = 0;
            AR_U32 gb_sum = 0;
            AR_U32 b_sum = 0;
            AR_U32 r_num = 0;
            AR_U32 gr_num = 0;
            AR_U32 gb_num = 0;
            AR_U32 b_num = 0;

            AR_U32 row = n*AR_BIGPIC_OFFSET_NUM_V;
            AR_U32 col = m*AR_BIGPIC_OFFSET_NUM_H;
            AR_U32 index = BLOCK_COL*row+col;

            if(enable_print)
            {
                ar_err("--------------------");
                ar_err("START  index=%3d (row=%2d * BLOCK_COL + col=%2d)", index, row, col);
                ar_err("");
            }
            for(int i=0; i<AR_BIGPIC_OFFSET_NUM_V; i++)
            {
                for(int j=0; j<AR_BIGPIC_OFFSET_NUM_H; j++)
                {
                    row = n*AR_BIGPIC_OFFSET_NUM_V+i;
                    col = m*AR_BIGPIC_OFFSET_NUM_H+j;
                    index = BLOCK_COL*row+col;
                    if(enable_print)
                    {
                        ar_err("ADD    index=%3d (row=%2d * BLOCK_COL + col=%2d) [%2d,%2d]", index, row, col, i, j);
                    }
                    r_sum += pstStatsData->p_mesh_grid_stats_r_sum[index];
                    gr_sum += pstStatsData->p_mesh_grid_stats_gr_sum[index];
                    gb_sum += pstStatsData->p_mesh_grid_stats_gb_sum[index];
                    b_sum += pstStatsData->p_mesh_grid_stats_b_sum[index];
                    r_num += pstStatsData->p_mesh_grid_stats_r_num[index];
                    gr_num += pstStatsData->p_mesh_grid_stats_gr_num[index];
                    gb_num += pstStatsData->p_mesh_grid_stats_gb_num[index];
                    b_num += pstStatsData->p_mesh_grid_stats_b_num[index];
                }
            }

            row = n;
            col = m;
            index = BLOCK_COL*row+col;
            if(enable_print)
            {
                ar_err("");
                ar_err("UPDATE index=%3d (row=%2d * BLOCK_COL + col=%2d)", index, row, col);
                ar_err("--------------------");
            }
            pstStatsData->p_mesh_grid_stats_r_sum[index] = r_sum;
            pstStatsData->p_mesh_grid_stats_gr_sum[index] = gr_sum;
            pstStatsData->p_mesh_grid_stats_gb_sum[index] = gb_sum;
            pstStatsData->p_mesh_grid_stats_b_sum[index] = b_sum;
            pstStatsData->p_mesh_grid_stats_r_num[index] = r_num;
            pstStatsData->p_mesh_grid_stats_gr_num[index] = gr_num;
            pstStatsData->p_mesh_grid_stats_gb_num[index] = gb_num;
            pstStatsData->p_mesh_grid_stats_b_num[index] = b_num;
        }
    }
    enable_print = 0;
}
static void big_pic_statistics_ae_merge(ISP_AE_STATISTICS_S **ppstAeStat)
{
    ISP_AE_STATISTICS_S **pstAeStat = ppstAeStat;
    STRU_MESH_GRID_STATS_DATA_T *pstStatsData0 = &pstAeStat[0]->stAecStats.stats_data;

    static int enable_print = 0;
    if(enable_print)
    {
        ar_err("++++++++++++++++++++");
    }

    // find small frame statistics
    for(int i=0; i<AR_BIGPIC_OFFSET_NUM_H; i++)
    {
        for(int j=0; j<AR_BIGPIC_OFFSET_NUM_V; j++)
        {
            if(i==0 && j==0)
            {
                continue;
            }
            STRU_MESH_GRID_STATS_DATA_T *pstStatsData = &pstAeStat[j*AR_BIGPIC_OFFSET_NUM_H+i]->stAecStats.stats_data;

            AR_U32 row = j*(BLOCK_ROW/AR_BIGPIC_OFFSET_NUM_V);
            AR_U32 col = (BLOCK_COL/AR_BIGPIC_OFFSET_NUM_H)*i;
            AR_U32 index = BLOCK_COL*row+col;
            if(enable_print)
            {
                ar_err("START  index=%3d (row=%2d * BLOCK_COL + col=%2d) FROM offset[%2d,%2d]", index, row, col, j, i);
                ar_err("");
            }

            // copy samll frame statistics
            for(int m=0; m<BLOCK_COL/AR_BIGPIC_OFFSET_NUM_H; m++)
            {
                for(int n=0; n<BLOCK_ROW/AR_BIGPIC_OFFSET_NUM_V; n++)
                {
                    row = j*(BLOCK_ROW/AR_BIGPIC_OFFSET_NUM_V)+n;
                    col = (BLOCK_COL/AR_BIGPIC_OFFSET_NUM_H)*i+m;
                    index = BLOCK_COL*row+col;
                    int index_offset = BLOCK_COL*n+m;
                    if(enable_print)
                    {
                        ar_err("UPDATE index=%3d (row=%2d * BLOCK_COL + col=%2d)", index, row, col);
                        ar_err("FROM   index=%3d (row=%2d * BLOCK_COL + col=%2d) offset[%2d,%2d]", index_offset, n, m, j, i);
                        ar_err("");
                    }
                    pstStatsData0->p_mesh_grid_stats_r_sum[index] = pstStatsData->p_mesh_grid_stats_r_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_gr_sum[index] = pstStatsData->p_mesh_grid_stats_gr_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_gb_sum[index] = pstStatsData->p_mesh_grid_stats_gb_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_b_sum[index] = pstStatsData->p_mesh_grid_stats_b_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_r_num[index] = pstStatsData->p_mesh_grid_stats_r_num[index_offset];
                    pstStatsData0->p_mesh_grid_stats_gr_num[index] = pstStatsData->p_mesh_grid_stats_gr_num[index_offset];
                    pstStatsData0->p_mesh_grid_stats_gb_num[index] = pstStatsData->p_mesh_grid_stats_gb_num[index_offset];
                    pstStatsData0->p_mesh_grid_stats_b_num[index] = pstStatsData->p_mesh_grid_stats_b_num[index_offset];
                }
            }
        }
    }
    if(enable_print)
    {
        ar_err("++++++++++++++++++++");
    }

    enable_print = 0;
}
static void* big_pic_statistics_ae_thread(void *arg)
{
    STRU_STAT_CTX_T *p_ctx = (STRU_STAT_CTX_T *)arg;
    STRU_CAMERA_T *p_camera = p_ctx->p_camera;

    int small_pic_frame_id = p_ctx->offset_id;
    int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;
    int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
    int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
    int small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

    while(!p_camera->big_pic_stop)
    {
        STRU_MMZ_ADDR_T *pstAeStatAddr = NULL;
        p_camera->pstAeStatQ[small_pic_frame_id]->queue_pop(p_camera->pstAeStatQ[small_pic_frame_id], (void**)&pstAeStatAddr);
        if(pstAeStatAddr != NULL)
        {
            ISP_AE_STATISTICS_S *pstAeStat = (ISP_AE_STATISTICS_S *)pstAeStatAddr->p_vaddr;
            AR_MPI_ISP_GetAEStatistics(p_camera->pipe_id[small_pic_num_abs], pstAeStat);
            // big_pic_statistics_ae_downsampling(&pstAeStat);
            p_camera->pstAeStatMergeQ[small_pic_frame_id]->queue_insert(p_camera->pstAeStatMergeQ[small_pic_frame_id], (void*)pstAeStatAddr);
            ar_signal(p_camera->big_pic_statistics_ae_merge_signal[small_pic_frame_id]);

        }else{
            ar_signal_wait(p_camera->big_pic_statistics_ae_signal[small_pic_frame_id]);
        }
    }
}
static void* big_pic_update_ae_thread(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    ISP_AE_STATISTICS_S *pstAeStat[AR_BIGPIC_OFFSET_NUM] = {0};
    STRU_MMZ_ADDR_T *pstAeStatAddr[AR_BIGPIC_OFFSET_NUM] = {0};
    while(!p_camera->big_pic_stop)
    {
        int small_pic_frame_id = 0;
        int raw_stream_index = AR_BIGPIC_WORKING_VI_ID_RAW;
        int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;
        int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
        int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
        int small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;
        for(; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM;)
        {
            small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

            p_camera->pstAeStatMergeQ[small_pic_frame_id]->queue_pop(p_camera->pstAeStatMergeQ[small_pic_frame_id], (void**)&pstAeStatAddr[small_pic_frame_id]);
            if(pstAeStatAddr[small_pic_frame_id] == NULL)
            {
                // ar_err("wait .... for big_pic_statistics_ae_merge_signal[%d]", small_pic_frame_id);
                ar_signal_wait(p_camera->big_pic_statistics_ae_merge_signal[small_pic_frame_id]);
                continue;

            }else{
                // ar_err("wait done for big_pic_statistics_ae_merge_signal[%d]", small_pic_frame_id);
                pstAeStat[small_pic_frame_id] = pstAeStatAddr[small_pic_frame_id]->p_vaddr;
                big_pic_statistics_ae_downsampling(&pstAeStat[small_pic_frame_id]);
            }

            small_pic_frame_id++;
        }
        small_pic_frame_id = 0;
        small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
        small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
        small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

        big_pic_statistics_ae_merge(&pstAeStat[small_pic_frame_id]);

        ISP_AE_STATISTICS_S *pstAeStat = (ISP_AE_STATISTICS_S *)(pstAeStatAddr[small_pic_frame_id]->p_vaddr);
        AR_U64 offset = (AR_U64)&pstAeStat->stAecStats - (AR_U64)pstAeStat;
        p_camera->stAeRunPra.p_aec_algo_lib_input = (STRU_AEC_ALOG_LIB_INPUT_T*)(pstAeStatAddr[small_pic_frame_id]->phy_addr + offset);

        AR_MPI_ISP_AecRun(p_camera->pipe_id[small_pic_num_abs], &p_camera->stAeRunPra);

        STRU_AEC_OUT_T aec_out = {0};
        static int enable_print = 0;
        if(enable_print%20 == 0)
        {
            // big_pic_print_aec_input(pstAeStat);
        }
        big_pic_aec_lib_output_2_aec_output((STRU_AEC_ALGO_LIB_OUTPUT_T*)p_camera->stAeAlgoLibOutputAddr.p_vaddr, &aec_out);
        if(enable_print%20 == 0)
        {
            // big_pic_print_aec_output(&aec_out);
        }
        enable_print++;

        if(ppra->s32IsFromFile == 0)
        {
            AR_MPI_ISP_AecUpdate(p_camera->pipe_id[raw_stream_index], &aec_out);
            // entry sof mode
            STRU_AR_CAMERA_SOF_PRO_T sof_prop;
            sof_prop.sof_mode = 1;
            sof_prop.to_trigger = 1;
            AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
        }

        for(small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
        {
            int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
            AR_MPI_ISP_AecUpdate(p_camera->pipe_id[small_pic_num_abs], &aec_out);
            // AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);

            p_camera->pstAeStatQ[small_pic_frame_id]->queue_insert(p_camera->pstAeStatQ[small_pic_frame_id], (void*)pstAeStatAddr[small_pic_frame_id]);
            ar_signal(p_camera->big_pic_statistics_ae_signal[small_pic_frame_id]);
            pstAeStatAddr[small_pic_frame_id] = NULL;
        }
    }
}

static int big_pic_print_awb_input(ISP_WB_STATISTICS_S *pstAwbStat)
{
    STRU_AWB_ALGO_LIB_INPUT_T *pstStatsData = &pstAwbStat->stAwbStats;
    AR_U32 r_sum = 0;
    AR_U32 g_sum = 0;
    AR_U32 b_sum = 0;
    AR_U32 num = 0;

    for(int m=0; m<MATRIX_COL_NUMBER; m++)
    {
        for(int n=0; n<MATRIX_ROW_NUMBER; n++)
        {
            AR_U32 index = n*MATRIX_COL_NUMBER+m;
            r_sum += pstStatsData->p_mesh_grid_stats_r_sum[index];
            g_sum += pstStatsData->p_mesh_grid_stats_g_sum[index];
            b_sum += pstStatsData->p_mesh_grid_stats_b_sum[index];
            num += pstStatsData->p_mesh_grid_stats_num[index];
        }
    }

    ar_err("++++++++++++++++++++");
    ar_err("TOTAL  r_sum=%d g_sum=%d b_sum=%d", r_sum, g_sum, b_sum);
    ar_err("TOTAL  num=%d", num);
    ar_err("TOTAL  r_sum/num=%d g_sum/num=%d b_sum/num=%d", r_sum/(num+1), g_sum/(num+1), b_sum/(num+1));
    ar_err("++++++++++++++++++++");
}
static int big_pic_print_awb_output(STRU_AWB_OUT_T *awb_out)
{
    ar_err("--------------------");
    ar_err("r_gain = %f", awb_out->r_gain);
    ar_err("g_gain = %f", awb_out->g_gain);
    ar_err("b_gain = %f", awb_out->b_gain);
    ar_err("cct = %f", awb_out->cct);
    ar_err("--------------------");

    return 0;
}
static int big_pic_awb_lib_output_2_awb_output(STRU_AWB_ALGO_LIB_OUTPUT_T *awb_lib_out, STRU_AWB_OUT_T *awb_out)
{
    awb_out->cct = awb_lib_out->cct;
    awb_out->r_gain = awb_lib_out->r_gain;
    awb_out->g_gain = awb_lib_out->g_gain;
    awb_out->b_gain = awb_lib_out->b_gain;

    return 0;
}
static void big_pic_statistics_awb_downsampling(ISP_WB_STATISTICS_S **ppstAwbStat)
{
    ISP_WB_STATISTICS_S *pstAwbStat = *ppstAwbStat;
    STRU_AWB_ALGO_LIB_INPUT_T *pstStatsData = &pstAwbStat->stAwbStats;

    static int enable_print = 0;
    for(int m=0; m<MATRIX_COL_NUMBER/AR_BIGPIC_OFFSET_NUM_H; m++)
    {
        for(int n=0; n<MATRIX_ROW_NUMBER/AR_BIGPIC_OFFSET_NUM_V; n++)
        {
            AR_U32 r_sum = 0;
            AR_U32 g_sum = 0;
            AR_U32 b_sum = 0;
            AR_U32 num = 0;

            AR_U32 row = n*AR_BIGPIC_OFFSET_NUM_V;
            AR_U32 col = m*AR_BIGPIC_OFFSET_NUM_H;
            AR_U32 index = MATRIX_COL_NUMBER*row+col;

            if(enable_print)
            {
                ar_err("--------------------");
                ar_err("START  index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d)", index, row, col);
                ar_err("");
            }
            for(int i=0; i<AR_BIGPIC_OFFSET_NUM_V; i++)
            {
                for(int j=0; j<AR_BIGPIC_OFFSET_NUM_H; j++)
                {
                    row = n*AR_BIGPIC_OFFSET_NUM_V+i;
                    col = m*AR_BIGPIC_OFFSET_NUM_H+j;
                    index = MATRIX_COL_NUMBER*row+col;
                    if(enable_print)
                    {
                        ar_err("ADD    index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d) [%2d,%2d]", index, row, col, i, j);
                    }
                    r_sum += pstStatsData->p_mesh_grid_stats_r_sum[index];
                    g_sum += pstStatsData->p_mesh_grid_stats_g_sum[index];
                    b_sum += pstStatsData->p_mesh_grid_stats_b_sum[index];
                    num += pstStatsData->p_mesh_grid_stats_num[index];
                }
            }

            row = n;
            col = m;
            index = MATRIX_COL_NUMBER*row+col;
            if(enable_print)
            {
                ar_err("");
                ar_err("UPDATE index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d)", index, row, col);
                ar_err("--------------------");
            }
            pstStatsData->p_mesh_grid_stats_r_sum[index] = r_sum;
            pstStatsData->p_mesh_grid_stats_g_sum[index] = g_sum;
            pstStatsData->p_mesh_grid_stats_b_sum[index] = b_sum;
            pstStatsData->p_mesh_grid_stats_num[index] = num;
        }
    }
    enable_print = 0;
}
static void big_pic_statistics_awb_merge(ISP_WB_STATISTICS_S **ppstAwbStat)
{
    ISP_WB_STATISTICS_S **pstAwbStat = ppstAwbStat;
    STRU_AWB_ALGO_LIB_INPUT_T *pstStatsData0 = &pstAwbStat[0]->stAwbStats;

    static int enable_print = 0;
    if(enable_print)
    {
        ar_err("++++++++++++++++++++");
    }

    // find small frame statistics
    for(int i=0; i<AR_BIGPIC_OFFSET_NUM_H; i++)
    {
        for(int j=0; j<AR_BIGPIC_OFFSET_NUM_V; j++)
        {
            if(i==0 && j==0)
            {
                continue;
            }
            STRU_AWB_ALGO_LIB_INPUT_T *pstStatsData = &pstAwbStat[j*AR_BIGPIC_OFFSET_NUM_H+i]->stAwbStats;

            AR_U32 row = j*(MATRIX_ROW_NUMBER/AR_BIGPIC_OFFSET_NUM_V);
            AR_U32 col = (MATRIX_COL_NUMBER/AR_BIGPIC_OFFSET_NUM_H)*i;
            AR_U32 index = MATRIX_COL_NUMBER*row+col;
            if(enable_print)
            {
                ar_err("START  index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d) FROM offset[%2d,%2d]", index, row, col, j, i);
                ar_err("");
            }

            // copy samll frame statistics
            for(int m=0; m<MATRIX_COL_NUMBER/AR_BIGPIC_OFFSET_NUM_H; m++)
            {
                for(int n=0; n<MATRIX_ROW_NUMBER/AR_BIGPIC_OFFSET_NUM_V; n++)
                {
                    row = j*(MATRIX_ROW_NUMBER/AR_BIGPIC_OFFSET_NUM_V)+n;
                    col = (MATRIX_COL_NUMBER/AR_BIGPIC_OFFSET_NUM_H)*i+m;
                    index = MATRIX_COL_NUMBER*row+col;
                    int index_offset = MATRIX_COL_NUMBER*n+m;
                    if(enable_print)
                    {
                        ar_err("UPDATE index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d)", index, row, col);
                        ar_err("FROM   index=%3d (row=%2d * MATRIX_COL_NUMBER + col=%2d) offset[%2d,%2d]", index_offset, n, m, j, i);
                        ar_err("");
                    }
                    pstStatsData0->p_mesh_grid_stats_r_sum[index] = pstStatsData->p_mesh_grid_stats_r_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_g_sum[index] = pstStatsData->p_mesh_grid_stats_g_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_b_sum[index] = pstStatsData->p_mesh_grid_stats_b_sum[index_offset];
                    pstStatsData0->p_mesh_grid_stats_num[index] = pstStatsData->p_mesh_grid_stats_num[index_offset];
                }
            }
        }
    }
    if(enable_print)
    {
        ar_err("++++++++++++++++++++");
    }

    enable_print = 0;
}
static void* big_pic_statistics_awb_thread(void *arg)
{
    STRU_STAT_CTX_T *p_ctx = (STRU_STAT_CTX_T *)arg;
    STRU_CAMERA_T *p_camera = p_ctx->p_camera;

    int small_pic_frame_id = p_ctx->offset_id;
    int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;
    int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
    int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
    int small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

    while(!p_camera->big_pic_stop)
    {
        STRU_MMZ_ADDR_T *pstAwbStatAddr = NULL;
        p_camera->pstAwbStatQ[small_pic_frame_id]->queue_pop(p_camera->pstAwbStatQ[small_pic_frame_id], (void**)&pstAwbStatAddr);
        if(pstAwbStatAddr != NULL)
        {
            ISP_WB_STATISTICS_S *pstAwbStat = (ISP_WB_STATISTICS_S *)pstAwbStatAddr->p_vaddr;
            AR_MPI_ISP_GetWBStatistics(p_camera->pipe_id[small_pic_num_abs], pstAwbStat);
            // big_pic_statistics_awb_downsampling(&pstAwbStat);
            p_camera->pstAwbStatMergeQ[small_pic_frame_id]->queue_insert(p_camera->pstAwbStatMergeQ[small_pic_frame_id], (void*)pstAwbStatAddr);
            ar_signal(p_camera->big_pic_statistics_awb_merge_signal[small_pic_frame_id]);

        }else{
            ar_signal_wait(p_camera->big_pic_statistics_awb_signal[small_pic_frame_id]);
        }
    }
}
static void* big_pic_update_awb_thread(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    ISP_WB_STATISTICS_S *pstAwbStat[AR_BIGPIC_OFFSET_NUM] = {0};
    STRU_MMZ_ADDR_T *pstAwbStatAddr[AR_BIGPIC_OFFSET_NUM] = {0};
    while(!p_camera->big_pic_stop)
    {
        int small_pic_frame_id = 0;
        int raw_stream_index = AR_BIGPIC_WORKING_VI_ID_RAW;
        int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;
        int small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
        int small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
        int small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;
        for(; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM;)
        {
            small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

            p_camera->pstAwbStatMergeQ[small_pic_frame_id]->queue_pop(p_camera->pstAwbStatMergeQ[small_pic_frame_id], (void**)&pstAwbStatAddr[small_pic_frame_id]);
            if(pstAwbStatAddr[small_pic_frame_id] == NULL)
            {
                // ar_err("wait .... for big_pic_statistics_awb_merge_signal[%d]", small_pic_frame_id);
                ar_signal_wait(p_camera->big_pic_statistics_awb_merge_signal[small_pic_frame_id]);
                continue;

            }else{
                // ar_err("wait done for big_pic_statistics_awb_merge_signal[%d]", small_pic_frame_id);
                pstAwbStat[small_pic_frame_id] = pstAwbStatAddr[small_pic_frame_id]->p_vaddr;
                big_pic_statistics_awb_downsampling(&pstAwbStat[small_pic_frame_id]);
            }

            small_pic_frame_id++;
        }
        small_pic_frame_id = 0;
        small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
        small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
        small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;

        big_pic_statistics_awb_merge(&pstAwbStat[small_pic_frame_id]);

        ISP_WB_STATISTICS_S *pstAwbStat = (ISP_WB_STATISTICS_S *)(pstAwbStatAddr[small_pic_frame_id]->p_vaddr);
        AR_U64 offset = (AR_U64)&pstAwbStat->stAwbStats - (AR_U64)pstAwbStat;
        p_camera->stAwbRunPra.p_awb_algo_lib_input = (STRU_AWB_ALGO_LIB_INPUT_T*)(pstAwbStatAddr[small_pic_frame_id]->phy_addr + offset);

        AR_MPI_ISP_AwbRun(p_camera->pipe_id[small_pic_num_abs], &p_camera->stAwbRunPra);

        STRU_AWB_OUT_T awb_out = {0};
        static int enable_print = 0;
        if(enable_print%20 == 0)
        {
            // big_pic_print_awb_input(pstAwbStat);
        }
        big_pic_awb_lib_output_2_awb_output((STRU_AWB_ALGO_LIB_OUTPUT_T*)p_camera->stAwbAlgoLibOutputAddr.p_vaddr, &awb_out);
        if(enable_print%20 == 0)
        {
            // big_pic_print_awb_output(&awb_out);
        }
        enable_print++;

        for(small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
        {
            int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
            AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[small_pic_num_abs], &awb_out);
            // AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);

            p_camera->pstAwbStatQ[small_pic_frame_id]->queue_insert(p_camera->pstAwbStatQ[small_pic_frame_id], (void*)pstAwbStatAddr[small_pic_frame_id]);
            ar_signal(p_camera->big_pic_statistics_awb_signal[small_pic_frame_id]);
            pstAwbStatAddr[small_pic_frame_id] = NULL;
        }
    }
}
static void big_pic_statistics_init(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    AR_VOID *p_vaddr = NULL;
    AR_U64 phy_addr = 0;

    char name[32] = {0};
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        sprintf(name, "ae stats queue &d", small_pic_frame_id);
        p_camera->pstAeStatQ[small_pic_frame_id] = ar_creat_queue(AR_BIGPIC_STATS_QUEUE_SIZE, name);
        sprintf(name, "ae stats update queue &d", small_pic_frame_id);
        p_camera->pstAeStatMergeQ[small_pic_frame_id] = ar_creat_queue(AR_BIGPIC_STATS_QUEUE_SIZE, name);

        for(int i=0; i<AR_BIGPIC_STATS_QUEUE_SIZE; i++)
        {
            p_vaddr = NULL;
            phy_addr = 0;
            ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, sizeof(ISP_AE_STATISTICS_S));
            if(!phy_addr || !p_vaddr)
            {
                ar_err("vb too small ~~~~~~~~~~~~~~~~~~~~~~~");
                usleep(5000000);
                return -1;
            }
            memset((void*)p_vaddr, 0, sizeof(ISP_AE_STATISTICS_S));

            ar_err("[AE] small_pic_frame_id=%d i=%d p_vaddr=%p phy_addr=%p", small_pic_frame_id, i, p_vaddr, phy_addr);
            p_camera->stAeStatAddr[small_pic_frame_id][i].p_vaddr = p_vaddr;
            p_camera->stAeStatAddr[small_pic_frame_id][i].phy_addr = phy_addr;

            p_camera->pstAeStatQ[small_pic_frame_id]->queue_insert(p_camera->pstAeStatQ[small_pic_frame_id], (void*)&p_camera->stAeStatAddr[small_pic_frame_id][i]);
        }
    }

    p_vaddr = NULL;
    phy_addr = 0;
    ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, sizeof(STRU_AEC_ALGO_LIB_OUTPUT_T));
    if(!phy_addr || !p_vaddr)
    {
        ar_err("vb too small ~~~~~~~~~~~~~~~~~~~~~~~");
        usleep(5000000);
        return -1;
    }
    memset((void*)p_vaddr, 0, sizeof(STRU_AEC_ALGO_LIB_OUTPUT_T));
    p_camera->stAeAlgoLibOutputAddr.p_vaddr = p_vaddr;
    p_camera->stAeAlgoLibOutputAddr.phy_addr = phy_addr;
    p_camera->stAeRunPra.p_aec_algo_lib_output = (STRU_AEC_ALGO_LIB_OUTPUT_T*)p_camera->stAeAlgoLibOutputAddr.phy_addr;

    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        p_camera->stAeStatCtx[small_pic_frame_id].offset_id = small_pic_frame_id;
        p_camera->stAeStatCtx[small_pic_frame_id].p_camera = p_camera;

        p_camera->big_pic_statistics_ae_signal[small_pic_frame_id] = ar_create_signal();
        p_camera->big_pic_statistics_ae_merge_signal[small_pic_frame_id] = ar_create_signal();

        pthread_create(&p_camera->big_pic_statistics_ae_thread[small_pic_frame_id], NULL, big_pic_statistics_ae_thread, &p_camera->stAeStatCtx[small_pic_frame_id]);
    }
    pthread_create(&p_camera->big_pic_update_ae_thread, NULL, big_pic_update_ae_thread, p_camera);

    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        sprintf(name, "awb stats queue &d", small_pic_frame_id);
        p_camera->pstAwbStatQ[small_pic_frame_id] = ar_creat_queue(AR_BIGPIC_STATS_QUEUE_SIZE, name);
        sprintf(name, "awb stats update queue &d", small_pic_frame_id);
        p_camera->pstAwbStatMergeQ[small_pic_frame_id] = ar_creat_queue(AR_BIGPIC_STATS_QUEUE_SIZE, name);

        for(int i=0; i<AR_BIGPIC_STATS_QUEUE_SIZE; i++)
        {
            p_vaddr = NULL;
            phy_addr = 0;
            ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, sizeof(ISP_WB_STATISTICS_S));
            if(!phy_addr || !p_vaddr)
            {
                ar_err("vb too small ~~~~~~~~~~~~~~~~~~~~~~~");
                usleep(5000000);
                return -1;
            }
            memset((void*)p_vaddr, 0, sizeof(ISP_WB_STATISTICS_S));

            ar_err("[AWB] small_pic_frame_id=%d i=%d p_vaddr=%p phy_addr=%p", small_pic_frame_id, i, p_vaddr, phy_addr);
            p_camera->stAwbStatAddr[small_pic_frame_id][i].p_vaddr = p_vaddr;
            p_camera->stAwbStatAddr[small_pic_frame_id][i].phy_addr = phy_addr;

            p_camera->pstAwbStatQ[small_pic_frame_id]->queue_insert(p_camera->pstAwbStatQ[small_pic_frame_id], (void*)&p_camera->stAwbStatAddr[small_pic_frame_id][i]);

        }
    }

    p_vaddr = NULL;
    phy_addr = 0;
    ar_hal_sys_mmz_alloc(&phy_addr, &p_vaddr, NULL, NULL, sizeof(STRU_AWB_ALGO_LIB_OUTPUT_T));
    if(!phy_addr || !p_vaddr)
    {
        ar_err("vb too small ~~~~~~~~~~~~~~~~~~~~~~~");
        usleep(5000000);
        return -1;
    }
    memset((void*)p_vaddr, 0, sizeof(STRU_AWB_ALGO_LIB_OUTPUT_T));
    p_camera->stAwbAlgoLibOutputAddr.p_vaddr = p_vaddr;
    p_camera->stAwbAlgoLibOutputAddr.phy_addr = phy_addr;
    p_camera->stAwbRunPra.p_awb_algo_lib_output = (STRU_AWB_ALGO_LIB_OUTPUT_T*)p_camera->stAwbAlgoLibOutputAddr.phy_addr;

    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        p_camera->stAwbStatCtx[small_pic_frame_id].offset_id = small_pic_frame_id;
        p_camera->stAwbStatCtx[small_pic_frame_id].p_camera = p_camera;

        p_camera->big_pic_statistics_awb_signal[small_pic_frame_id] = ar_create_signal();
        p_camera->big_pic_statistics_awb_merge_signal[small_pic_frame_id] = ar_create_signal();

        pthread_create(&p_camera->big_pic_statistics_awb_thread[small_pic_frame_id], NULL, big_pic_statistics_awb_thread, &p_camera->stAwbStatCtx[small_pic_frame_id]);
    }
    pthread_create(&p_camera->big_pic_update_awb_thread, NULL, big_pic_update_awb_thread, p_camera);

    return;
}
static void big_pic_statistics_deinit(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    AR_VOID *p_vaddr = NULL;
    AR_U64 phy_addr = 0;

    pthread_join(p_camera->big_pic_update_ae_thread, NULL);
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        pthread_join(p_camera->big_pic_statistics_ae_thread[small_pic_frame_id], NULL);

        ar_delete_signal(p_camera->big_pic_statistics_ae_merge_signal[small_pic_frame_id]);
        ar_delete_signal(p_camera->big_pic_statistics_ae_signal[small_pic_frame_id]);
    }

    p_vaddr = p_camera->stAeAlgoLibOutputAddr.p_vaddr;
    phy_addr = p_camera->stAeAlgoLibOutputAddr.phy_addr;
    ar_hal_sys_mmz_free(phy_addr, p_vaddr);

    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        for(int i=0; i<AR_BIGPIC_STATS_QUEUE_SIZE; i++)
        {
            p_camera->pstAeStatMergeQ[small_pic_frame_id]->queue_pop(p_camera->pstAeStatMergeQ[small_pic_frame_id], (void**)&p_vaddr);
            p_camera->pstAeStatQ[small_pic_frame_id]->queue_pop(p_camera->pstAeStatQ[small_pic_frame_id], (void**)&p_vaddr);

            p_vaddr = p_camera->stAeStatAddr[small_pic_frame_id][i].p_vaddr;
            phy_addr = p_camera->stAeStatAddr[small_pic_frame_id][i].phy_addr;
            ar_hal_sys_mmz_free(phy_addr, p_vaddr);
        }

        ar_delete_queue(&p_camera->pstAeStatMergeQ[small_pic_frame_id]);
        ar_delete_queue(&p_camera->pstAeStatQ[small_pic_frame_id]);
    }

    pthread_join(p_camera->big_pic_update_awb_thread, NULL);
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        pthread_join(p_camera->big_pic_statistics_awb_thread[small_pic_frame_id], NULL);

        ar_delete_signal(p_camera->big_pic_statistics_awb_merge_signal[small_pic_frame_id]);
        ar_delete_signal(p_camera->big_pic_statistics_awb_signal[small_pic_frame_id]);
    }

    p_vaddr = p_camera->stAwbAlgoLibOutputAddr.p_vaddr;
    phy_addr = p_camera->stAwbAlgoLibOutputAddr.phy_addr;
    ar_hal_sys_mmz_free(phy_addr, p_vaddr);

    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        for(int i=0; i<AR_BIGPIC_STATS_QUEUE_SIZE; i++)
        {
            p_camera->pstAwbStatMergeQ[small_pic_frame_id]->queue_pop(p_camera->pstAwbStatMergeQ[small_pic_frame_id], (void**)&p_vaddr);
            p_camera->pstAwbStatQ[small_pic_frame_id]->queue_pop(p_camera->pstAwbStatQ[small_pic_frame_id], (void**)&p_vaddr);

            p_vaddr = p_camera->stAwbStatAddr[small_pic_frame_id][i].p_vaddr;
            phy_addr = p_camera->stAwbStatAddr[small_pic_frame_id][i].phy_addr;
            ar_hal_sys_mmz_free(phy_addr, p_vaddr);
        }

        ar_delete_queue(&p_camera->pstAwbStatMergeQ[small_pic_frame_id]);
        ar_delete_queue(&p_camera->pstAwbStatQ[small_pic_frame_id]);
    }

    return;
}
static void* big_pic_3a_auto_thread(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    big_pic_statistics_init(p_camera);

    while(!p_camera->big_pic_stop)
    {
        usleep(30000);
    }

    big_pic_statistics_deinit(p_camera);

    p_camera->big_pic_3a_auto_runing = 0;
}
static void* big_pic_sub(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    ar_err("debug debug debug take still entry");

    int raw_stream_index = AR_BIGPIC_WORKING_VI_ID_RAW;
    int yuv_stream_index = AR_BIGPIC_WORKING_VI_ID_LEFT;

    VI_CROP_INFO_S stCropInfo = {0};
    stCropInfo.bEnable = 1;
    stCropInfo.stCropRect.s32X = 0;
    stCropInfo.stCropRect.s32Y = 0;
    stCropInfo.stCropRect.u32Width = p_camera->bigpic_isp_out_width;
    stCropInfo.stCropRect.u32Height = p_camera->bigpic_isp_out_height;

    VIDEO_FRAME_INFO_S FrameInfoRaw = {0};
    VIDEO_FRAME_INFO_S *p_stream_buffer_raw = &FrameInfoRaw;
    VIDEO_FRAME_INFO_S FrameInfoYuv[AR_BIGPIC_OFFSET_NUM] = {0};
    VIDEO_FRAME_INFO_S *p_stream_buffer_yuv = &FrameInfoYuv[0];
    VIDEO_FRAME_INFO_S FrameInfoBig = {0};
    VIDEO_FRAME_INFO_S *p_stream_buffer_big = &FrameInfoBig;

    int raw_pic_frame_id = 0;
    int small_pic_frame_id = 0;
    AR_VOID *addr_pa_y = NULL;
    AR_VOID *addr_pa_u = NULL;
    AR_VOID *addr_pa_v = NULL;
    AR_VOID *addr_va_y = NULL;
    AR_VOID *addr_va_u = NULL;
    AR_VOID *addr_va_v = NULL;
    AR_VOID *addr_y = NULL;
    AR_VOID *addr_u = NULL;
    AR_VOID *addr_v = NULL;

    int small_pic_num_h = 0;
    int small_pic_num_v = 0;
    int small_pic_num_abs = yuv_stream_index;

    int server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_TRIGGER_RAW;
    int big_save_num = ppra->s32nframes;
    int big_pic_stop = p_camera->big_pic_stop;
    int file_num = 0;
    while(!big_pic_stop)
    {
        switch(server_state_big_pic)
        {
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_TRIGGER_RAW:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_TRIGGER_RAW\n");
            if(p_camera->big_pic_3a_auto)
            {
                p_camera->big_pic_3a_auto_runing = 1;
                pthread_create(&p_camera->big_pic_3a_auto_thread, NULL, big_pic_3a_auto_thread, p_camera);
            }
            // big_pic_print_aec_output(&p_camera->aec_out);
            if(ppra->s32IsFromFile == 0)
            {
                ar_always("debug debug debug 3a update pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
                ar_always("debug debug debug AR_MPI_ISP_AecUpdate real_gain %f exp_index %d", p_camera->aec_out.real_gain, p_camera->aec_out.exp_index);
                AR_MPI_ISP_AecUpdate(p_camera->pipe_id[raw_stream_index], &p_camera->aec_out);
                // entry sof mode
                STRU_AR_CAMERA_SOF_PRO_T sof_prop;
                sof_prop.sof_mode = 1;
                sof_prop.to_trigger = 1;
                AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
                // // exit sof mode
                // sof_prop.sof_mode = 0;
                // sof_prop.to_trigger = 0;
                // AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
                // trigger on
                AR_MPI_VI_TriggerPipe(p_camera->pipe_id[raw_stream_index], 1);
            }
            for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
            {
                int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
                AR_MPI_ISP_AecUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->aec_out);
                AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->awb_out);
                AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);
            }

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_PRE;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_PRE:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_PRE\n");

            if(server_state_current == AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE)
            {
                if(ppra->s32nframes > 0)
                {
                    big_save_num = ppra->s32nframes;

                    ppra->s32nframes = 0;
                }
            }

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_RAW;
        }

        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_RAW:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_RAW\n");

            ar_always("debug debug debug start raw_pic_frame_id=%d ========================", raw_pic_frame_id);

            if(ppra->s32IsFromFile == 0)
            {
                ar_always("debug debug debug GetChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
                AR_MPI_VI_GetChnFrame(p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index], p_stream_buffer_raw, -1);
                ar_always("debug debug debug u32PoolId=%d, enModId=%d", p_stream_buffer_raw->u32PoolId, p_stream_buffer_raw->enModId);
                ar_always("debug debug debug u32FrameId=%d, u32Width=%d, u32Height=%d", p_stream_buffer_raw->stVFrame.u32FrameId,p_stream_buffer_raw->stVFrame.u32Width,p_stream_buffer_raw->stVFrame.u32Height);

            }else{
                if(file_num >= p_camera->bigpic_raw_file_num)
                {
                    file_num = 0;
                }

                p_stream_buffer_raw->stVFrame.u32Width = p_camera->bigpic_raw_stream_width;
                p_stream_buffer_raw->stVFrame.u32Height = p_camera->bigpic_raw_stream_height;
                p_stream_buffer_raw->stVFrame.u32Stride[0] = p_camera->bigpic_isp_in_stride;
                p_stream_buffer_raw->stVFrame.u32Stride[1] = 0;
                p_stream_buffer_raw->stVFrame.u32Stride[2] = 0;
                p_stream_buffer_raw->stVFrame.u64PhyAddr[0] = p_camera->bigpic_raw_file_buffer_phy[file_num];
                p_stream_buffer_raw->stVFrame.u64PhyAddr[1] = NULL;
                p_stream_buffer_raw->stVFrame.u64PhyAddr[2] = NULL;
                p_stream_buffer_raw->stVFrame.u64VirAddr[0] = p_camera->bigpic_raw_file_buffer[file_num];
                p_stream_buffer_raw->stVFrame.u64VirAddr[1] = NULL;
                p_stream_buffer_raw->stVFrame.u64VirAddr[2] = NULL;

                file_num++;
            }

            if(AR_BIGPIC_TO_FILE_RAW && big_save_num > 0)
            {
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_RAW;

            }else{
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW;
            }
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_RAW:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_RAW\n");

            char raw_file_name[64] = {0};
            sprintf(raw_file_name, "/mnt/raw_save_big_%d.raw", raw_pic_frame_id);
            int raw_pic_file_fd = open(raw_file_name, O_RDWR|O_CREAT);
            if(raw_pic_file_fd < 0){
                ar_err("%s open file error\n", raw_file_name);
                return -1;
            }

            ar_always("debug debug debug stride=%d, AR_BIGPIC_ISP_IN_STRIDE=%d",p_stream_buffer_raw->stVFrame.u32Stride[0], p_camera->bigpic_isp_in_stride);
            for(int i=0; i<p_stream_buffer_raw->stVFrame.u32Height; i++)
            {
                write(raw_pic_file_fd,
                      p_stream_buffer_raw->stVFrame.u64VirAddr[0]+i*p_stream_buffer_raw->stVFrame.u32Stride[0],
                      p_stream_buffer_raw->stVFrame.u32Width*2);
            }

            close(raw_pic_file_fd);
            ar_err("%s save done\n", raw_file_name);

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW\n");

            ar_always("debug debug debug start small_pic_frame_id=%d ++++++++++++++++++++++++", small_pic_frame_id);

            int small_pic_pixel_offset_h = 0;
            int small_pic_pixel_offset_v = 0;
            small_pic_num_h = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_v = small_pic_frame_id/AR_BIGPIC_OFFSET_NUM_H;
            small_pic_num_abs = yuv_stream_index+small_pic_num_v*AR_BIGPIC_OFFSET_NUM_H+small_pic_num_h;
            if(small_pic_num_h > 0)
            {
                small_pic_pixel_offset_h = (p_camera->bigpic_isp_in_width-p_camera->bigpic_isp_in_overlap_h)*small_pic_num_h-p_camera->bigpic_isp_in_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                small_pic_pixel_offset_v = (p_camera->bigpic_isp_in_height-p_camera->bigpic_isp_in_overlap_v)*small_pic_num_v-p_camera->bigpic_isp_in_overlap_v;
            }
            int small_pic_byte_offset_h = small_pic_pixel_offset_h*2;
            int small_pic_byte_offset_v = small_pic_pixel_offset_v*p_camera->bigpic_isp_in_stride;
            int small_pic_byte_offset = small_pic_byte_offset_v+small_pic_byte_offset_h;

            stCropInfo.stCropRect.s32X = 0;
            stCropInfo.stCropRect.s32Y = 0;
            if(small_pic_num_h > 0)
            {
                stCropInfo.stCropRect.s32X = (p_camera->bigpic_isp_in_overlap_h-p_camera->bigpic_isp_in_overlap_h_hdr_gap)-p_camera->bigpic_isp_out_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                stCropInfo.stCropRect.s32Y = p_camera->bigpic_isp_in_overlap_v-p_camera->bigpic_isp_out_overlap_v;
            }
            ar_always("debug debug debug SetChnCropManual PROC pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            ar_always("debug debug debug SetChnCropManual PROC s32X=%d, s32Y=%d", stCropInfo.stCropRect.s32X, stCropInfo.stCropRect.s32Y);
            AR_MPI_VI_SetChnCropManual(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stCropInfo);

            // ar_always("debug debug debug 3a update pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            // ar_always("debug debug debug AR_MPI_ISP_AecUpdate real_gain %f exp_index %d", p_camera->aec_out.real_gain, p_camera->aec_out.exp_index);
            // ar_always("debug debug debug AR_MPI_ISP_AwbUpdate r_gain %f, g_gain %f, b_gain %f", p_camera->awb_out.r_gain, p_camera->awb_out.g_gain, p_camera->awb_out.b_gain);
            // AR_MPI_ISP_AecUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->aec_out);
            // AR_MPI_ISP_AwbUpdate(p_camera->pipe_id[small_pic_num_abs], &p_camera->awb_out);
            // AR_MPI_ISP_FlushTuningPra(p_camera->pipe_id[small_pic_num_abs]);

            VIDEO_FRAME_INFO_S VideoFrameQ = {0};
            VIDEO_FRAME_INFO_S *pstVideoFrameQ = &VideoFrameQ;
            memcpy(pstVideoFrameQ, p_stream_buffer_raw, sizeof(VIDEO_FRAME_INFO_S));
            pstVideoFrameQ->stVFrame.u64PhyAddr[0] = p_stream_buffer_raw->stVFrame.u64PhyAddr[0]+small_pic_byte_offset;
            pstVideoFrameQ->stVFrame.u64PhyAddr[1] = 0;
            pstVideoFrameQ->stVFrame.u64PhyAddr[2] = 0;
            pstVideoFrameQ->stVFrame.u64VirAddr[0] = p_stream_buffer_raw->stVFrame.u64VirAddr[0]+small_pic_byte_offset;
            pstVideoFrameQ->stVFrame.u64VirAddr[1] = (AR_U64)(NULL);
            pstVideoFrameQ->stVFrame.u64VirAddr[2] = (AR_U64)(NULL);
            pstVideoFrameQ->stVFrame.u32Stride[0] = p_camera->bigpic_isp_in_stride;
            pstVideoFrameQ->stVFrame.u32Stride[1] = 0;
            pstVideoFrameQ->stVFrame.u32Stride[2] = 0;
            pstVideoFrameQ->stVFrame.u32Width = p_camera->bigpic_isp_in_width;
            pstVideoFrameQ->stVFrame.u32Height = p_camera->bigpic_isp_in_height;
            pstVideoFrameQ->stVFrame.u64PTS = 0;
            pstVideoFrameQ->stVFrame.u64PrivateData = (AR_U64)(NULL);
            if(ppra->s32IsFromFile == 0)
                pstVideoFrameQ->u32PoolId = p_camera->u32_pool_id_C;
            else
                pstVideoFrameQ->u32PoolId = p_camera->u32_pool_id_F;
            ar_always("debug debug debug QueryChnStatus pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            VI_CHN_STATUS_S stChnStatus = {0};
            AR_MPI_VI_QueryChnStatus(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stChnStatus);
            while(stChnStatus.u32FredomBufCount<1)
            {
                usleep(10000);
                AR_MPI_VI_QueryChnStatus(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stChnStatus);
            }
            ar_always("debug debug debug SendPipeRaw pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            AR_MPI_VI_SendPipeRaw(p_camera->pipe_id[small_pic_num_abs], pstVideoFrameQ, -1);
            ar_always("debug debug debug u32PoolId=%d, enModId=%d", pstVideoFrameQ->u32PoolId, pstVideoFrameQ->enModId);
            ar_always("debug debug debug u32FrameId=%d, u32Width=%d, u32Height=%d", pstVideoFrameQ->stVFrame.u32FrameId,pstVideoFrameQ->stVFrame.u32Width,pstVideoFrameQ->stVFrame.u32Height);
            ar_always("debug debug debug u32FrameId=%d, u64PhyAddr[0]=%p, u64VirAddr[0]=%p", pstVideoFrameQ->stVFrame.u32FrameId,pstVideoFrameQ->stVFrame.u64PhyAddr[0],pstVideoFrameQ->stVFrame.u64VirAddr[0]);

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_YUV;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_YUV:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_YUV\n");

            ar_always("debug debug debug GetChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            AR_MPI_VI_GetChnFrame(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], p_stream_buffer_yuv, -1);

            int yuv_pic_pixel_offset_h = 0;
            int yuv_pic_pixel_offset_v = 0;
            if(small_pic_num_h > 0)
            {
                yuv_pic_pixel_offset_h = (p_camera->bigpic_isp_out_width-p_camera->bigpic_isp_out_overlap_h)*small_pic_num_h-p_camera->bigpic_isp_out_overlap_h;
            }
            if(small_pic_num_v > 0)
            {
                yuv_pic_pixel_offset_v = (p_camera->bigpic_isp_out_height-p_camera->bigpic_isp_out_overlap_v)*small_pic_num_v-p_camera->bigpic_isp_out_overlap_v;
            }
            int yuv_pic_byte_offset_h = yuv_pic_pixel_offset_h;
            int yuv_pic_byte_offset_v = yuv_pic_pixel_offset_v*p_stream_buffer_yuv->stVFrame.u32Stride[0];
            int yuv_pic_byte_offset_y = yuv_pic_byte_offset_v+yuv_pic_byte_offset_h;
            int yuv_pic_byte_offset_uv = yuv_pic_byte_offset_v/4+yuv_pic_byte_offset_h/2;

            addr_pa_y = p_stream_buffer_yuv->stVFrame.u64PhyAddr[0] - yuv_pic_byte_offset_y;
            addr_pa_u = p_stream_buffer_yuv->stVFrame.u64PhyAddr[1] - yuv_pic_byte_offset_uv;
            addr_pa_v = p_stream_buffer_yuv->stVFrame.u64PhyAddr[2] - yuv_pic_byte_offset_uv;
            addr_va_y = p_stream_buffer_yuv->stVFrame.u64VirAddr[0] - yuv_pic_byte_offset_y;
            addr_va_u = p_stream_buffer_yuv->stVFrame.u64VirAddr[1] - yuv_pic_byte_offset_uv;
            addr_va_v = p_stream_buffer_yuv->stVFrame.u64VirAddr[2] - yuv_pic_byte_offset_uv;
            addr_y = addr_va_y + yuv_pic_byte_offset_y;
            addr_u = addr_va_u + yuv_pic_byte_offset_uv;
            addr_v = addr_va_v + yuv_pic_byte_offset_uv;
            ar_always("debug debug debug pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
            ar_always("debug debug debug yuv_pic_byte_offset_y=%d, yuv_pic_byte_offset_uv=%d", yuv_pic_byte_offset_y, yuv_pic_byte_offset_uv);
            ar_always("debug debug debug big y pa=%p, va=%p", addr_pa_y, addr_va_y);
            ar_always("debug debug debug big u pa=%p, va=%p", addr_pa_u, addr_va_u);
            ar_always("debug debug debug big v pa=%p, va=%p", addr_pa_v, addr_va_v);
            ar_always("debug debug debug small y va=%p u va=%p v va=%p", addr_y, addr_u, addr_v);

            if(AR_BIGPIC_TO_FILE_YUV_SMALL && big_save_num > 0)
            {
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_YUV;

            }else{
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_BIG;
            }
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_YUV:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_YUV\n");

            char small_file_name[64] = {0};
            sprintf(small_file_name, "/mnt/yuv_save_small_%d_%d.yuv", raw_pic_frame_id, small_pic_frame_id);
            int small_pic_file_fd = open(small_file_name, O_RDWR|O_CREAT);
            if(small_pic_file_fd < 0){
                ar_err("%s open file error\n", small_file_name);
                return -1;
            }

            for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height; i++)
            {
                write(small_pic_file_fd, addr_y+i*p_stream_buffer_yuv->stVFrame.u32Stride[0], p_stream_buffer_yuv->stVFrame.u32Width);
            }
            for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height/2; i++)
            {
                write(small_pic_file_fd, addr_u+i*p_stream_buffer_yuv->stVFrame.u32Stride[0]/2, p_stream_buffer_yuv->stVFrame.u32Width/2);
            }
            for(int i=0; i<p_stream_buffer_yuv->stVFrame.u32Height/2; i++)
            {
                write(small_pic_file_fd, addr_v+i*p_stream_buffer_yuv->stVFrame.u32Stride[0]/2, p_stream_buffer_yuv->stVFrame.u32Width/2);
            }

            close(small_pic_file_fd);
            ar_err("%s save done\n", small_file_name);

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_BIG;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_BIG:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_GET_BIG\n");

            if(small_pic_frame_id >= AR_BIGPIC_OFFSET_NUM-1)
            {
                memcpy((void*)p_stream_buffer_big, (void*)p_stream_buffer_yuv, sizeof(VIDEO_FRAME_INFO_S));
                p_stream_buffer_big->u32PoolId = p_camera->u32_pool_id_L;
                p_stream_buffer_big->stVFrame.u32Width = p_camera->bigpic_width;
                p_stream_buffer_big->stVFrame.u32Height = p_camera->bigpic_height;
                p_stream_buffer_big->stVFrame.u32Stride[0] = p_stream_buffer_yuv->stVFrame.u32Stride[0];
                p_stream_buffer_big->stVFrame.u32Stride[1] = p_stream_buffer_yuv->stVFrame.u32Stride[0]/2;
                p_stream_buffer_big->stVFrame.u32Stride[2] = p_stream_buffer_yuv->stVFrame.u32Stride[0]/2;
                p_stream_buffer_big->stVFrame.u64PhyAddr[0] = addr_pa_y;
                p_stream_buffer_big->stVFrame.u64PhyAddr[1] = addr_pa_u;
                p_stream_buffer_big->stVFrame.u64PhyAddr[2] = addr_pa_v;
                p_stream_buffer_big->stVFrame.u64VirAddr[0] = addr_va_y;
                p_stream_buffer_big->stVFrame.u64VirAddr[1] = addr_va_u;
                p_stream_buffer_big->stVFrame.u64VirAddr[2] = addr_va_v;

                if(p_camera->enable_display)
                {
                    server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_DISP_YUV;

                }else if(big_save_num > 0)
                {
                    server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_BIG;

                }else{
                    server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV;
                }

            }else{
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV;
            }
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_DISP_YUV:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_DISP_YUV\n");

            Display_Buffer(p_camera, p_camera->vo_ch_id, p_stream_buffer_big);

            if(big_save_num > 0)
            {
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_BIG;

            }else{
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV;
            }
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_BIG:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_BIG\n");

            char yuv_file_name[64] = {0};
            sprintf(yuv_file_name, "/mnt/yuv_save_big_%d.yuv", raw_pic_frame_id);
            int yuv_pic_file_fd = open(yuv_file_name, O_RDWR|O_CREAT);
            if(yuv_pic_file_fd < 0){
                ar_err("%s open file error\n", yuv_file_name);
                return -1;
            }

            for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height; i++)
            {
                write(yuv_pic_file_fd, addr_va_y+i*p_stream_buffer_big->stVFrame.u32Stride[0], p_stream_buffer_big->stVFrame.u32Width);
            }
            for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height/2; i++)
            {
                write(yuv_pic_file_fd, addr_va_u+i*p_stream_buffer_big->stVFrame.u32Stride[1], p_stream_buffer_big->stVFrame.u32Width/2);
            }
            for(int i=0; i<p_stream_buffer_big->stVFrame.u32Height/2; i++)
            {
                write(yuv_pic_file_fd, addr_va_v+i*p_stream_buffer_big->stVFrame.u32Stride[2], p_stream_buffer_big->stVFrame.u32Width/2);
            }

            close(yuv_pic_file_fd);
            ar_err("%s save done\n", yuv_file_name);

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_YUV\n");

            small_pic_frame_id++;
            small_pic_frame_id = small_pic_frame_id%AR_BIGPIC_OFFSET_NUM;
            p_stream_buffer_yuv = &FrameInfoYuv[small_pic_frame_id];
            if(small_pic_frame_id == 0)
            {
                ar_always("debug debug debug ReleaseChnFrame all yuv");
                for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
                {
                    int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
                    ar_always("debug debug debug ReleaseChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
                    AR_MPI_VI_ReleaseChnFrame(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &FrameInfoYuv[small_pic_frame_id]);
                    ar_always("debug debug debug end small_pic_frame_id=%d ++++++++++++++++++++++++", small_pic_frame_id);
                }

                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_RAW;

            }else{
                server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SEND_RAW;
            }
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_RAW:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_RELEASE_RAW\n");

            ar_always("debug debug debug ReleaseChnFrame pipe_id=%d, ch_id=%d", p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index]);
            AR_MPI_VI_ReleaseChnFrame(p_camera->pipe_id[raw_stream_index], p_camera->ch_id[raw_stream_index], p_stream_buffer_raw);

            ar_always("debug debug debug end raw_pic_frame_id=%d ========================", raw_pic_frame_id);

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_POST;
        }
        break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_POST:
        {
            ar_always("debug debug debug AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_POST\n");

            raw_pic_frame_id++;

            switch(server_state_current)
            {
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
                if(!p_camera->big_pic_3a_auto_runing)
                {
                    big_pic_stop = p_camera->big_pic_stop;

                    if(p_camera->big_pic_3a_auto)
                        pthread_join(p_camera->big_pic_3a_auto_thread, NULL);

                    if(ppra->s32IsFromFile == 0)
                    {
                        // exit sof mode
                        STRU_AR_CAMERA_SOF_PRO_T sof_prop;
                        sof_prop.sof_mode = 0;
                        sof_prop.to_trigger = 0;
                        AR_MPI_ISP_VsyncUpdate(p_camera->pipe_id[raw_stream_index], &sof_prop);
                    }

                }
                if(big_pic_stop)
                {
                    ar_err("debug debug debug big_pic_stop %d \n", big_pic_stop);
                }

                break;
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                big_save_num--;
                if(big_save_num == 0)
                {
                    change_state_will(server_state_will_previous);
                }

                break;
            default:
                break;
            }

            server_state_big_pic = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_PRE;
        }
        break;
        }
    }

    // cancel manual crop
    stCropInfo.bEnable = 0;
    stCropInfo.stCropRect.s32X = 0;
    stCropInfo.stCropRect.s32Y = 0;
    stCropInfo.stCropRect.u32Width = 0;
    stCropInfo.stCropRect.u32Height = 0;
    for(int small_pic_frame_id=0; small_pic_frame_id<AR_BIGPIC_OFFSET_NUM; small_pic_frame_id++)
    {
        int small_pic_num_abs = yuv_stream_index+small_pic_frame_id;
        ar_always("debug debug debug SetChnCropManual CANCEL pipe_id=%d, ch_id=%d", p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs]);
        AR_MPI_VI_SetChnCropManual(p_camera->pipe_id[small_pic_num_abs], p_camera->ch_id[small_pic_num_abs], &stCropInfo);
    }

    ar_err("debug debug debug take still exit");

    return NULL;
}

static void* big_pic_main(void *arg)
{
    STRU_CAMERA_T *p_camera_preview = (STRU_CAMERA_T *)arg;

    STRU_CAMERA_T *p_camera_big_pic = malloc(sizeof(STRU_CAMERA_T));
    memset((void*)p_camera_big_pic, 0, sizeof(STRU_CAMERA_T));

    open_camera(p_camera_big_pic, 1);

    int take_still_count = ppra->s32Count;
    int server_state_stop = 0;
    while(!server_state_stop)
    {
        switch(server_state_current)
        {
        case AR_BIGPIC_STATE_STOP_WILLABLE:
            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_STOP_WILLABLE:
                ar_signal(p_camera_preview->exit_signal);
                server_state_stop = 1;
                ar_err("debug debug debug server_state_stop %d", server_state_stop);

                break;
            case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_PREVIEW_START;

                break;
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_START;

                break;
            default:
                break;
            }
            check_state_willed();

            break;
        case AR_BIGPIC_STATE_RUN_PREVIEW_START:
            ar_err("start preview");

            create_stream(p_camera_preview);
            start_stream(p_camera_preview);

            server_state_current = AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE;

            break;
        case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_STOP_WILLABLE:
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_PREVIEW_STOP;

                break;
            default:
                break;
            }
            check_state_willed();

            break;
        case AR_BIGPIC_STATE_RUN_PREVIEW_STOP:
            ar_err("stop preview");

            AR_MPI_ISP_GetAecUpdate(p_camera_preview->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera_preview->aec_out);
            AR_MPI_ISP_GetAwbUpdate(p_camera_preview->pipe_id[AR_BIGPIC_WORKING_VI_ID_PREVIEW], &p_camera_preview->awb_out);
            p_camera_big_pic->aec_out = p_camera_preview->aec_out;
            p_camera_big_pic->awb_out = p_camera_preview->awb_out;
            ar_always("debug debug debug AR_MPI_ISP_GetAecUpdate real_gain %f exp_index %d", p_camera_big_pic->aec_out.real_gain, p_camera_big_pic->aec_out.exp_index);
            ar_always("debug debug debug AR_MPI_ISP_GetAwbUpdate r_gain %f, g_gain %f, b_gain %f", p_camera_big_pic->awb_out.r_gain, p_camera_big_pic->awb_out.g_gain, p_camera_big_pic->awb_out.b_gain);

            stop_stream(p_camera_preview);
            destroy_stream(p_camera_preview);

            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_STOP_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_STOP_WILLABLE;

                break;
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
                if(ppra->s32CameraMode == AR_BIGPIC_CAMERA_MODE_VIDEO)
                {
                    p_camera_big_pic->big_pic_3a_auto = 1;

                }else
                {
                    p_camera_big_pic->big_pic_3a_auto = 0;
                }
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_START;

                break;
            default:
                break;
            }

            break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_START:
            ar_err("start big pic");

            create_stream(p_camera_big_pic);
            start_stream(p_camera_big_pic);

            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE;

                pthread_create(&p_camera_big_pic->big_pic_thread, NULL, big_pic_sub, p_camera_big_pic);

                break;
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE;
                server_state_will_previous = AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE;

                pthread_create(&p_camera_big_pic->big_pic_thread, NULL, big_pic_sub, p_camera_big_pic);

                break;
            default:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE;
                break;
            }

            break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_STOP_WILLABLE:
            case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
                p_camera_big_pic->big_pic_stop = 1;
                pthread_join(p_camera_big_pic->big_pic_thread, NULL);
                p_camera_big_pic->big_pic_stop = 0;

                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_STOP;

                break;
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE;
                server_state_will_previous = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE;

                break;
            default:
                break;
            }
            check_state_willed();

            break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE:
            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
            case AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE;

                break;
            default:
                break;
            }
            check_state_willed();

            break;
        case AR_BIGPIC_STATE_RUN_BIG_PIC_STOP:
            ar_err("stop big pic");

            stop_stream(p_camera_big_pic);
            destroy_stream(p_camera_big_pic);

            switch(server_state_will)
            {
            case AR_BIGPIC_STATE_STOP_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_STOP_WILLABLE;

                break;
            case AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE:
                server_state_current = AR_BIGPIC_STATE_RUN_PREVIEW_START;

                break;
            default:
                break;
            }

            break;
        default:
            break;
        }

        usleep(500000);
    }

    close_camera(p_camera_big_pic);

    free(p_camera_big_pic);

    return NULL;
}

static int to_take_still(void *arg)
{
    STRU_CAMERA_T *p_camera = (STRU_CAMERA_T *)arg;

    int cmd_pipe = -1;
    mkfifo(AR_BIGPIC_PIPE_FILE, 0777);
    cmd_pipe = open(AR_BIGPIC_PIPE_FILE, O_RDWR);

    static SAMPLE_VIO_PRA pra_l = {0};
    static SAMPLE_VIO_PRA *ppra_l = &pra_l;
    int cmd_server_stop = 0;
    while(!cmd_server_stop)
    {
        int ret = read(cmd_pipe, ppra_l, sizeof(SAMPLE_VIO_PRA));
        if(ret < sizeof(SAMPLE_VIO_PRA))
        {
            continue;
        }

        printf("\nserver got cmd \n");
        ppra->s32Cmd = ppra_l->s32Cmd;
        switch(ppra->s32Cmd)
        {
        case 5:
            printf("\n\t -bigpic) switch to big pic mode \n");
            change_state_will(AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_WILLABLE);

            break;
        case 4:
            printf("\n\t -preview) switch to preview mode \n");
            change_state_will(AR_BIGPIC_STATE_RUN_PREVIEW_RUNING_WILLABLE);

            break;
        case 3:
            printf("\n\t -take num) save big pic with num \n");
            ppra->s32nframes = ppra_l->s32nframes;
            change_state_will(AR_BIGPIC_STATE_RUN_BIG_PIC_RUNING_SAVE_WILLABLE);

            break;
        case 2:
            printf("\n\t -stop) stop big pic server \n");
            change_state_will(AR_BIGPIC_STATE_STOP_WILLABLE);
            cmd_server_stop = 1;
            break;

        default:
            printf("\n");
            printf("\t -bigpic)   switch to big pic mode \n");
            printf("\t -preview)  switch to preview mode \n");
            printf("\t -take num) save big pic with num \n");
            printf("\t -stop)     stop big pic server \n");

            usleep(1000000);
            break;
        }
    }

    close(cmd_pipe);
    unlink(AR_BIGPIC_PIPE_FILE);
}

AR_S32 SAMPLE_VIO_Big_Pic_Demo(SAMPLE_VIO_PRA *ppra_l)
{
    ar_err("entry big pic demo");
    *ppra = *ppra_l;

    AR_MPI_SYS_Init();

    big_pic_config(ppra->s32SensorRes);

    if(ppra->s32Cmd > 1)
    {
        ar_err("\nentry client mode\n");
        // check pipe file exist
        int cmd_pipe = open(AR_BIGPIC_PIPE_FILE, O_WRONLY);
        if(cmd_pipe < 0)
        {
            ar_err("\nbig pic server not exist, please start server\n");
            return 0;
        }

        write(cmd_pipe, ppra, sizeof(SAMPLE_VIO_PRA));
        close(cmd_pipe);

        return 0;

    }else if(ppra->s32Cmd == 1)
    {
        ar_err("\nentry server mode\n");
        // check pipe file exist
        int cmd_pipe = open(AR_BIGPIC_PIPE_FILE, O_WRONLY);
        if(cmd_pipe > 0)
        {
            ar_err("\nbig pic server already exists\n");
            close(cmd_pipe);
            return 0;
        }

    }else{
        ar_err("\nentry normal mode\n");
        // check pipe file exist
        int cmd_pipe = open(AR_BIGPIC_PIPE_FILE, O_WRONLY);
        if(cmd_pipe > 0)
        {
            ar_err("\nbig pic server already exists, please stop server\n");
            close(cmd_pipe);
            return 0;
        }
    }

    if(ppra->s32Cmd == 0)
    {
        if(ppra->s32Num <= 0)
        {
            ppra->s32Num = AR_BIGPIC_NUM;
        }
        if(ppra->s32Count <= 0)
        {
            ppra->s32Count = AR_BIGPIC_CNT;
        }
        if(ppra->s32nframes <= 0)
        {
            ppra->s32nframes = AR_BIGPIC_TO_FILE_NUM;
        }

    }else
    {
        ppra->s32Num = 0;
        ppra->s32Count = 0;
        ppra->s32nframes = 0;
    }

    STRU_CAMERA_T *p_camera = malloc(sizeof(STRU_CAMERA_T));
    memset((void*)p_camera, 0, sizeof(STRU_CAMERA_T));

    open_camera(p_camera, 0);

    p_camera->exit_signal = ar_create_signal();

    // creat a loop to receive msg
    if(ppra->s32Cmd == 0)
    {
        pthread_create(&p_camera->msg_loop, NULL, take_still_thread, p_camera);

    }else
    {
        pthread_create(&p_camera->msg_loop, NULL, big_pic_main, p_camera);
        pthread_create(&p_camera->server_thread, NULL, to_take_still, p_camera);
    }

    ar_signal_wait(p_camera->exit_signal);

    if(ppra->s32Cmd == 1)
    {
        pthread_join(p_camera->server_thread, NULL);
    }
    pthread_join(p_camera->msg_loop, NULL);

    ar_delete_signal(p_camera->exit_signal);

    close_camera(p_camera);

    free(p_camera);

    ar_err("exit big pic demo");

    AR_MPI_SYS_Exit();

    return 0;
}
static VO_DSI_ATTR_S pstDsiCfg =
{
    .u32Lane           = {1,1,1,1},
    .u32BitsPerPixel   = 24,
    .stSyncInfo        = {
        .u32Hsa  = 72,
        .u32Hbp  = 162,
        .u32Hact = 1024,
        .u32Hfp  = 162,

        .u32Vsa  = 10,
        .u32Vbp  = 23,
        .u32Vact = 600,
        .u32Vfp  = 12
    },
    .fDphyClkMhz      = 360.3744,
    .u32PllFreqReg2c0 = 0x2c65f509,
    .u32PllFreqReg38c = 0x2800
};

#define CHECK_RET(express,name)                                  \
    do {                                                                \
    AR_S32 Ret;                                                         \
    Ret = express;                                                      \
    if (Ret != AR_SUCCESS) {                                            \
        printf("%s failed at %s : LINE: %d with %#x!\n",name, __FUNCTION__,__LINE__,Ret); \
        AR_MPI_SYS_Exit();                                              \
        AR_MPI_VB_Exit();                                               \
        return Ret;                                                     \
    }                                                                   \
    }while(0)

extern int VO_SysCall(const char *cmd, char *buff, int bufsize);
extern int VO_GPIO_SetValue(int pin, int value);
extern void VO_7INCH_LCD_Init(void);

AR_S32 VO_HDMI_1080P60(AR_VOID)
{
    AR_U32 i = 0;
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_S32 VoChnNum = 1;
    VO_PUB_ATTR_S stPubAttr = {0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    SIZE_S stDevSize = {0};

    VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM] = {0};

    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = 0;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.bIop = 1;

    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    stPubAttr.stSyncInfo.bIdv = 0;
    stPubAttr.stSyncInfo.bIhs = 0;
    stPubAttr.stSyncInfo.bIvs = 0;

    stPubAttr.enIntfType = VO_INTF_HDMI;
    stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");


    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = AR_BIGPIC_DISPLAY_WIDTH;
    stDevSize.u32Height = AR_BIGPIC_DISPLAY_HEIGHT;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width  = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;

    stLayerAttr.u32DispFrmRt = AR_BIGPIC_DISPLAY_FPS;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

    /* SET AND ENABLE VO CHN */
    {
        AR_S32  i;
        AR_U32  u32Height = stDevSize.u32Height/VoChnNum;
        AR_U32  u32Width  = stDevSize.u32Width/VoChnNum;
        AR_U32  u32Square;

        if (VoChnNum == 1)
            u32Square = 1;
        else if (VoChnNum <= 4)
            u32Square = 2;
        else if (VoChnNum <= 9)
            u32Square = 3;
        else
            u32Square = 1;

        u32Height = stDevSize.u32Height/u32Square;
        u32Width  = stDevSize.u32Width/u32Square;

        for (i = 0; i < VoChnNum; i++) {
            astChnAttr[i].bDeflicker = AR_FALSE;
            astChnAttr[i].u32Priority = 0;
            astChnAttr[i].stRect.s32X = (i%u32Square)*u32Width;
            astChnAttr[i].stRect.s32Y = (i/u32Square)*u32Height;
            astChnAttr[i].stRect.u32Height = u32Height;
            astChnAttr[i].stRect.u32Width = u32Width;
        }
    }

    for (i = 0; i < VoChnNum; i++) {
        CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

        CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
    }

    return AR_SUCCESS;
}
AR_S32 VO_MIPILCD_1024_600(AR_VOID)
{
    AR_U32 i = 0;
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;
    AR_S32 VoChnNum = 1;
    VO_PUB_ATTR_S stPubAttr={0};
    VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
    VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
    AR_U32 u32Framerate = 0;
    SIZE_S stDevSize = {0};

    VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM] = {0};

    /* SET VO PUB ATTR OF USER TYPE */
    stPubAttr.u32BgColor = COLOR_RGB_BLUE;
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = 0;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.bIop = 1;

    stPubAttr.stSyncInfo.u16Hmid = 1;
    stPubAttr.stSyncInfo.u16Bvact = 1;
    stPubAttr.stSyncInfo.u16Bvbb = 1;
    stPubAttr.stSyncInfo.u16Bvfb = 1;

    stPubAttr.stSyncInfo.bIdv = 0;
    stPubAttr.stSyncInfo.bIhs = 0;
    stPubAttr.stSyncInfo.bIvs = 0;


    /* USER SET VO FRAME RATE */
    u32Framerate = 60;
    CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");


    /* USER SET VO DEV SYNC INFO */
    stPubAttr.enIntfSync = VO_OUTPUT_USER;
    stPubAttr.stSyncInfo.bSynm = AR_TRUE;
    stPubAttr.stSyncInfo.bIop  = AR_TRUE;
    stPubAttr.stSyncInfo.u8Intfb = 0;
    stPubAttr.stSyncInfo.u16Hpw  = 96;
    stPubAttr.stSyncInfo.u16Hbb  = 216;
    stPubAttr.stSyncInfo.u16Hact = 1024;
    stPubAttr.stSyncInfo.u16Hfb  = 216;
    stPubAttr.stSyncInfo.u16Vpw  = 10;
    stPubAttr.stSyncInfo.u16Vbb  = 23;
    stPubAttr.stSyncInfo.u16Vact = 600;
    stPubAttr.stSyncInfo.u16Vfb  = 12;
    stPubAttr.stSyncInfo.u16Hmid  = 0;
    stPubAttr.stSyncInfo.u16Bvact = 0;
    stPubAttr.stSyncInfo.u16Bvbb  = 0;
    stPubAttr.stSyncInfo.u16Bvfb  = 0;
    stPubAttr.stSyncInfo.bIdv = AR_FALSE;
    stPubAttr.stSyncInfo.bIhs = AR_FALSE;
    stPubAttr.stSyncInfo.bIvs = AR_FALSE;
    stPubAttr.enIntfType = VO_INTF_MIPI;
    stPubAttr.enIntfType |= VO_INTF_LCD_24BIT;

    CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");


    /* USER SET VO SYNC INFO OF USER INTF */
    stUserInfo.bClkReverse = AR_TRUE;
    stUserInfo.u32DevDiv = 1;
    stUserInfo.u32PreDiv = 1;
    stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
    stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

    CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

    /* ENABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

    /* SET LCD */
    CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");
    CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
    VO_7INCH_LCD_Init();

    /*SET VO LAYER ATTR*/
    stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
    stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

    stLayerAttr.bClusterMode = AR_FALSE;
    stLayerAttr.bDoubleFrame = AR_FALSE;
    stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    stLayerAttr.stDispRect.s32X = 0;
    stLayerAttr.stDispRect.s32Y = 0;
    stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
    stLayerAttr.stDispRect.u32Width  = stDevSize.u32Width;

    stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
    stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;

    stLayerAttr.u32DispFrmRt = AR_BIGPIC_DISPLAY_FPS;

    CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

    /* ENABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

    /* SET AND ENABLE VO CHN */
    {
        AR_S32  i;
        AR_U32  u32Height = stDevSize.u32Height/VoChnNum;
        AR_U32  u32Width  = stDevSize.u32Width/VoChnNum;
        AR_U32  u32Square;

        if (VoChnNum == 1)
            u32Square = 1;
        else if (VoChnNum <= 4)
            u32Square = 2;
        else if (VoChnNum <= 9)
            u32Square = 3;
        else
            u32Square = 1;

        u32Height = stDevSize.u32Height/u32Square;
        u32Width  = stDevSize.u32Width/u32Square;

        for (i = 0; i < VoChnNum; i++) {
            astChnAttr[i].bDeflicker = AR_FALSE;
            astChnAttr[i].u32Priority = 0;
            astChnAttr[i].stRect.s32X = (i%u32Square)*u32Width;
            astChnAttr[i].stRect.s32Y = (i/u32Square)*u32Height;
            astChnAttr[i].stRect.u32Height = u32Height;
            astChnAttr[i].stRect.u32Width = u32Width;
        }
    }

    for (i = 0; i < VoChnNum; i++) {
        CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

        CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
    }

    return AR_SUCCESS;
}
static AR_S32 Display_MPI_Init(STRU_CAMERA_T *pstCamera, AR_S32 Ch)
{
    /* SET LCD */
    if(ppra->u32votype == AR_BIGPIC_DISPLAY_IF_HDMI)
    {
        VO_HDMI_1080P60();

    }else
    {
        VO_MIPILCD_1024_600();
    }

    if(pstCamera->big_pic_mode == 0)
    {
        pstCamera->use_display = 1;
        pthread_create(&pstCamera->display_thread, NULL, display_thread, pstCamera);
    }

    return AR_SUCCESS;
}

static AR_S32 Display_MPI_Deinit(STRU_CAMERA_T *pstCamera, AR_S32 Ch)
{
    AR_S32 VoDev = 0;
    AR_S32 VoLayer = 0;

    if(pstCamera->big_pic_mode == 0)
    {
        ar_always("wait the display thread exit");
        pstCamera->use_display = 0;
        pthread_join(pstCamera->display_thread, NULL);
        ar_always("display thread exit");
    }

    /*DISABLE VO CHN*/
    CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, Ch), "AR_MPI_VO_DisableChn");

    /* DISABLE VO LAYER */
    CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

    /* DISABLE VO DEV */
    CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

    return AR_SUCCESS;
}

static AR_S32 Display_MPI_Buffer(STRU_CAMERA_T *pstCamera, AR_S32 Ch, VIDEO_FRAME_INFO_S *pstFrame)
{
    AR_S32 s32Ret = AR_SUCCESS;
    VO_LAYER VoLayer = 0;

    pstFrame->stVFrame.u64PTS = 40000;
    pstFrame->stVFrame.u32TimeRef = 40000;

    s32Ret = AR_MPI_VO_SendFrame(VoLayer, Ch, pstFrame, 0);

    return AR_SUCCESS;
}

static AR_S32 Display_Init(STRU_CAMERA_T *pstCamera, AR_S32 Ch)
{
    if(AR_BIGPIC_DISPLAY_API_LEVEL == AR_BIGPIC_DISPLAY_API_LEVEL_HAL)
    {
        display_hal_init(pstCamera, Ch);

    }else
    {
        Display_MPI_Init(pstCamera, Ch);
    }

    return AR_SUCCESS;
}
static AR_S32 Display_Deinit(STRU_CAMERA_T *pstCamera, AR_S32 Ch)
{
    if(AR_BIGPIC_DISPLAY_API_LEVEL == AR_BIGPIC_DISPLAY_API_LEVEL_HAL)
    {
        display_hal_deinit(pstCamera, Ch);

    }else
    {
        Display_MPI_Deinit(pstCamera, Ch);
    }

    return AR_SUCCESS;
}
static AR_S32 Display_Buffer(STRU_CAMERA_T *pstCamera, AR_S32 Ch, VIDEO_FRAME_INFO_S *pstFrame)
{
    if(AR_BIGPIC_DISPLAY_API_LEVEL == AR_BIGPIC_DISPLAY_API_LEVEL_HAL)
    {
        display_hal_buffer(pstCamera, Ch, pstFrame);

    }else
    {
        Display_MPI_Buffer(pstCamera, Ch, pstFrame);
    }

    return AR_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
