#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

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
#include "hal_vb.h"
#include "osal.h"
#include "hal_gpio.h"
#include "mpi_gdc_api.h"
#ifdef AR_FEAT_EIS
#include "hal_eis_algo.h"
#include "sample_eis.h"
#endif

#include <sys/prctl.h>
#include "ar_common.h"
#include "ar_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef SAMPLE_PRT
#define SAMPLE_PRT(fmt, ...)                     \
    do {\
        ar_always(fmt, ##__VA_ARGS__);  \
    }while(0)
#endif

#ifndef PAUSE
#define PAUSE()  do {\
        printf("---------------press Enter key to exit!---------------\n");\
        getchar();\
    } while (0)
#endif

typedef struct
{
	AR_S32 s32CameraMode;
	AR_S32 s32IsFromFile;
	AR_CHAR *pstrDirName;
	AR_S32 s32SensorRes;
	AR_S32 s32Cmd;
	AR_S32 s32Num;
	AR_S32 s32Count;
	AR_S32 s32nframes;
	AR_S32 u32votype;
	AR_S32 s32IsHdrOn;
	AR_S32 cam_mode;	
	AR_S32 dpcm;
    AR_S32 argc;
	AR_S32 mipi_fre;
	AR_S32 vif_fre;
	AR_S32 isp_fre;
	AR_S32 pcs_fre;
	AR_S32 settle;
	AR_S32 feature_mask;
	AR_S32 ltm_mode;
    AR_CHAR **argv;
	AR_S32 sus_mode;
	AR_S32 en_cf50;
	AR_S32 cf50_mode;
	AR_S32 flip;
	AR_S32 mirror;
	AR_S32 s32I2cDev;
	AR_U32 u32Sensor;
	void   *p_data[4];
}SAMPLE_VIO_PRA;
//define a queue
typedef  struct __STRU_AR_QUEUE_T STRU_AR_QUEUE_T;
static STRU_AR_QUEUE_T *ar_creat_queue(uint32_t queue_size,char *name);
static int ar_delete_queue(STRU_AR_QUEUE_T **queue);

struct __STRU_AR_QUEUE_T
{
 char * name;
 uint32_t queue_size;
 uint32_t valid_data_num;
 //queue data
 void **data;
 uint32_t header;
 uint32_t tail;
 //lock
 ar_lock_t lock;
 int (*queue_pop)(STRU_AR_QUEUE_T * queue, void **item);
 int (*queue_insert)(STRU_AR_QUEUE_T * queue,void *item);
 int (*get_queue_size)(STRU_AR_QUEUE_T * queue);
 int (*look_up_head)(STRU_AR_QUEUE_T * queue, void **item);
};


typedef struct
{
    AR_S32                   chan_id;
    AR_S32                      chan_fd;
	STRU_AR_HAL_VO_RECT      pos;
}STRU_SURFACE_OBJ_T;

typedef struct
{
    STRU_SURFACE_OBJ_T       surface[24];
	//dislpay_obj
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    AR_S32                      dev_fd;
    AR_S32                      layer_fd;
    AR_S32 width;
    AR_S32 height;
    AR_S32 y_stride;
    AR_S32 uv_stride;
	AR_S32 cf50;
	AR_S32 cf50_mode;
	AR_S32 format;
}STRU_DISPLAY_OBJ_T;


typedef struct
{
   AR_S32 pipe_id;
   AR_S32 ch_id;
   AR_S32 vo_ch_id;
   volatile AR_BOOL stop;
   AR_S32 nframes;
   AR_S32 suspend;
   char *name;
   SRTU_SENSOR_DEFAULT_ATTR_T *p_attr;
   AR_S32 lane_id;
}STRU_VIO_SAMPLE_FRAME_RUN_T;


typedef struct
{
   AR_S32 pipe_id;
   AR_S32 algo_type; // 0:aec 1:awb 2:af
   AR_S32 nframes;
}STRU_VIO_SAMPLE_GET_3A_STATS_T;

typedef struct
{
	STRU_AR_QUEUE_T *p_queue;
	ar_signal_t signal;
	int pipe_id;
	int ViPipe_raw;
	int ViChn_raw;
}STRU_VIO_SAMPLE_RAW_PROCESS_RUN_T;


typedef struct
{
   char *p_name;
   int (*vio_test_case)(SAMPLE_VIO_PRA *ppra);
   char *p_help_string[64];
}SAMPLE_VIO_TEST_CASE;

typedef struct
{
  int case_index;  
  SAMPLE_VIO_TEST_CASE *test_case;
  char *test_cmd;
  char *test_cmds[64];
}SAMPLE_VIO_TEST_CASE_LIST_T;

typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
}rgb_t;

typedef struct
{
  uint8_t y;
  uint8_t u;
  uint8_t v;
}yuv_t;

typedef struct{  
	uint8_t ch[4];
}bayer_t;

typedef struct
{
	char *y_addr;
	char *u_addr;
	char *v_addr;
	int width;
	int height;
	int line_len;
	int cbcr_line_len;
	int use_color_set;
	int yuv_color[3];
	int bit_depth;
	int roi_w;
	int roi_h;
	int bayer;
}color_fill_t;



AR_VOID SAMPLE_VIO_MsgInit(AR_VOID);
AR_VOID SAMPLE_VIO_MsgExit(AR_VOID);

void SAMPLE_VIO_HandleSig(AR_S32 signo);
AR_S32 SAMPLE_VIO_ViOnlineVpssOnlineRoute(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViOnlineVpssOfflineRoute(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViDoublePipeRoute(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViDoubleChnRoute(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViWdrSwitch(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViVpssLowDelay(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViRotate(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViFPN(SAMPLE_VIO_PRA *ppra);

AR_S32 SAMPLE_VIO_ViResoSwitch(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViDoubleWdrPipe(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViDeMuxYuv(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_ViSetUsrPic(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only(SAMPLE_VIO_PRA *ppra);
AR_S32 write_yuv_data_to_file(FILE *fp, VIDEO_FRAME_INFO_S *pstFrameInfo, int format);
AR_S32 SAMPLE_VIO_Only_YUV420SP(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_run_200_frame_exit(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_mipi_2(SAMPLE_VIO_PRA *ppra);
int SAMPLE_VIO_region_vi_attach_test(char* file_name, AR_U32 rgn_id);
AR_S32 SAMPLE_VIO_Only_For_Bind(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_AR_MPI_ISP_GetVDTimeOut(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Get3aStats(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_ISP_IN(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_imx307_raw_process(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Bind_Get_Pipe_Raw(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_multi_get_ch_frame(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Dual_Sensor_With_Hdr_Nohdr(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Three_Sensor(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Two_Sensor(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Hdr(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_2Ch(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_2Ch_Plus_Raw(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Start_Stop(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_TP9930_DVP_24Ch_Randomly_Plug_UserPic(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_IMX307_MIPI_1Ch_AND_TP9930_DVP_4Ch(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_With_IMX415(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_With_IMX415_60Fps(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_With_OS04A10(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Hdr_For_Bind_With_OS04A10(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_GC2093(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_GC2093_Hdr(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_For_Bind_With_SC530AI(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Only_Two_Sensor_60fps_30fps(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_Big_Pic_Demo(SAMPLE_VIO_PRA *ppra);
AR_S32 SAMPLE_VIO_VI_DeMuxVCRaw(SAMPLE_VIO_PRA *ppra);
void* frame_run(void* param);
AR_VOID SAMPLE_AR_MPI_VIN_OpenDev(AR_S32 mode,AR_S32 hdr_freq, AR_S32 vif_freq, AR_S32 isp_freq,AR_S32 mipi_freq);
AR_S32 sample_find_pra_by_name(AR_CHAR *name, AR_S32 argc, AR_CHAR *argv[]);
AR_S32 SAMPLE_VI_Only_gn2(SAMPLE_VIO_PRA *ppra);

AR_S32 sample_pipeline_dual_imx307(SAMPLE_VIO_PRA *ppra);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
