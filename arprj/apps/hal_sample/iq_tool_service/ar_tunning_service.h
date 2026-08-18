#ifndef __AR_TUNING_SERVICE_H__
#define __AR_TUNING_SERVICE_H__
#define ENABALE_ALL_LOG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <pthread.h>
#include "hal_vin_type_def.h"
#include "fifo.h"
#include "hal_sys.h"
#include "iq_tool_type.h"

#define TUNING_SER_STREAM_BUF_COUNT 3
#define MAX_MEM_FIFO 4096
#define RAW_MEM_FIFO (256)
#define MAX_PIPE_NUM (8)
#define USE_AXI_DMA 1

typedef  int (*pfn_send)(void *client,uint8_t *play_load,uint32_t len,uint8_t msg_id, uint8_t ecc_flag);

enum
{
  TUNING_STREAM_PREVIEW,
  TUNING_STREAM_VIDEO,
  TUNING_STREAM_SCALER3,
  TUNING_STREAM_RAW,
  TUNING_STREAM_HDR_RAW,  
  TUNING_STREAM_META_AF,
  TUNING_SER_MAX_STREAM_COUNT,
};
typedef struct
{
 int panel_count;
 void * va[3];
 void * va_orig[3];
 void * pa[3];
 void * pa_orig[3];

 int  len[3];
 int fd_map[3];
}tuning_stream_buf_t;

enum
{
   STREAM_MODE_YUV,
   STREAM_MODE_H264,
   STREAM_MODE_H265,
   STREAM_MODE_JPG,
   STREAM_MODE_RAW,   
};

typedef struct
{
    int buffer_count;
    tuning_stream_buf_t buffer[TUNING_SER_STREAM_BUF_COUNT];
    int buffer_ref;
    int stop_flag;
    int format;
    int width;
    int height;
    int bit_depth;
    pthread_t tid;
    int req_frame;
    int send_data_mod;
    int stride;
    int stream_type;
}tuning_stream_t;

enum
{
    ONCE_RAW_ONCE_FRAME,
    ONCE_RAW_REPEAT_FRAME,
    TRIGER_NONE
};

enum
{
    ONE_FRAME_STREAM,
    MULTI_FRAME_STREAM,
};

typedef struct
{
    int pipe_fd;
    int pipe_id;
    int first_open;
    int sensor_hdr;
    tuning_stream_t stream[TUNING_SER_MAX_STREAM_COUNT];
    int nframes;
    void *p_raw;
    int len;
    int trigger_start;
    int seq_count;
    int seq_mod;
    int start_idx;
    int times;
    int hdr_mod;
    int hdr_frames;
    int frame_id;
    int raw_cmd;
    int fps;
    int stream_fd[MAX_PIPE_NUM];
    int stream_id[MAX_PIPE_NUM];
    int with_3a;
	int af_frame_count;	
	int event_client_id;
	ar_os_thread_id_t event_thread;
}cam_obj_t;

enum
{
  SERVER_DISCONECT,
  SERVER_CONNECTED,
  SERVER_STREAM_RUNING,
};

typedef struct
{
	void *pa;
	void *va;
	int len;
}v4ar_pannel_t;

typedef v4ar_pannel_t v4ar_mem_t;

typedef struct
{
  void *va[3];
  void *pa[3];
  uint32_t len[3];
}send_mem_t;

typedef struct
{
  int is_no_loss_send;
  int stream_type;
  int stream_fd;
  int write_to_file;
  send_mem_t mem;
  STRU_BASIC_3A_INFO_T basic_info;
}send_v4l2_buffer_t;


typedef struct
{
    float real_gain;
    uint32_t exp_time_us;
    int width;
    int height;
    int bayer_format;
    int bit_depth;
}tuning_raw_info_t;

typedef struct
{
  cam_obj_t *obj;
  int tool_mode;
  int is_first_frame;
  int enable_display;
  int fd_mem;
  int vin_fd;
  int fd_display;
  //int fd_rpc_mem;
  int fd_encoder_video_in;
  int fd_encoder_video_out;
  int fd_encoder_priview_in;
  int fd_encoder_priview_out;
  int fd_encoder_jpg_in;
  int fd_encoder_jpg_out;

  int fd_assist_mod_bitstram;
  pthread_t tid_video_bitstream_assist;
  volatile int video_bitstream_assist_runing;

  int probe_sensor_num;
  char sensor[MAX_PIPE_NUM][64];
  int pipe_fd[MAX_PIPE_NUM];
  int pipe_id[MAX_PIPE_NUM];
  int stream_id[MAX_PIPE_NUM][MAX_PIPE_NUM];
  int stream_fd[MAX_PIPE_NUM][MAX_PIPE_NUM];
  int stream_client[MAX_PIPE_NUM];
  int  server_status;
  int  send_thread_status;
  //for net communacation used
  void * client;
  uint8_t msg_id;
  pfn_send send;
  pfn_send send_server;
  pthread_t tid_priview;
  pthread_t tid_video;
  pthread_t tid_raw;
  pthread_t tid_hdr_raw;  
  pthread_t tid_af;
  pthread_t tid_qbuf;
  pthread_t tid_dq;
  pthread_mutex_t mutex;
  pthread_cond_t  cond_privew;
  pthread_cond_t  cond_video;
  pthread_cond_t  cond_raw;
  pthread_cond_t  cond_hdr_raw;  
  pthread_cond_t  cond_af;
  STRU_AR_QUEUE_T *queue_priview;
  STRU_AR_QUEUE_T *queue_video;
  STRU_AR_QUEUE_T *queue_raw;
  STRU_AR_QUEUE_T *queue_hdr_raw;
  STRU_AR_QUEUE_T *queue_af;


  pthread_t tid_video_bitstream;
  pthread_t tid_jpeg;

  pthread_t tid_jpeg_dq_buffer;
  pthread_t tid_h265_dq_buffer;
  volatile int recording_bitstream;
  pthread_mutex_t mutex_stop;
  int jpg_server_pipeline_fd;
  int h265_server_pipeline_fd;
  unsigned int ddr_rw_burest_len;
  int dev_mem_fd;
  unsigned int page_size;
  v4ar_mem_t mem_raw_frame;
  tuning_raw_info_t raw_info;
  STRU_STREAM_BUFFER_INIFO_T pstream_buf[8];
  v4ar_mem_t mem_raw_buf[RAW_MEM_FIFO];
  STRU_BASIC_3A_INFO_T mem_raw_3a[RAW_MEM_FIFO];
  pthread_mutex_t qlock;
  pthread_cond_t  qready;
  STRU_AR_QUEUE_T *q_dq_event;
  char stream_sensor_name[64];
}tuning_server_t;

typedef struct
{
  int request_type;
  int len;
}tuning_request_t;

typedef enum
{
  TUNING_ERR_SUCCESS,
  TUNING_ERR_TIME_OUT,
  TUNING_ERR_UNKNOW,
}error_code_t;

typedef struct
{
   int request_type;
   int error_code;
   int len;
}tuning_ack_t;

///////////////////////////////////////////////////////////////////

typedef enum {
    SYS_INFO_EVENT_EXT_NONE = 0,
    SYS_INFO_EVENT_EXT_GET_SYS_INFO,
    SYS_INFO_EVENT_EXT_MAX,
}sys_info_get_event_ext_t;

typedef struct
{
    unsigned int ddr_rw_burst_len;
} RTOS_SYS_DDR_INFO_CONTEXT_T;

typedef struct
{
    RTOS_SYS_DDR_INFO_CONTEXT_T ddr_info;
} RTOS_SYS_INFO_CONTEXT_t;

typedef struct
{
 int len;
 int nframes;
 int fps;
 int trigger_mode;
 int stream_mode;
 int width;
 int height;
 int hdr_mode;
 int hdr_frames; //how much frame for hdr merge;long middle short, or long short
 int with_3a;
}STU_CAM_SET_RAW_TO_SENSOR_SIM_PRA_T;

///////////////////////////////////////////////////////////////////

int dbg_handle_cam_tuning(void *client,uint8_t *msg,uint32_t len,int msg_id);
int register_cam_tuning_send_ops(pfn_send, void * client,uint8_t msg_id);
int unregister_cam_tuning_client(void * client,int msg_id);
#endif
