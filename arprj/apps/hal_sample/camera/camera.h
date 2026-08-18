#ifndef __CAMERA_H__
#define __CAMERA_H__

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
#include <pthread.h>
#include <pthread.h>
#include <stdint.h>
#include "hal_vin_type_def.h"
#include "hal_vin_rpc.h"
#include "hal_vin.h"
//#include "v4ar_dev.h"
#include "hal_vin_log.h"
#include "osal.h"
#include <mqueue.h>
#include "hal_vo.h"



#define MAX_HELP_STR_NUM 64
#define MAX_STREAM_SIZE 6
#define MAX_STREAM_BUFER_COUNT 5

////////////////////fifo...............................................
typedef  struct __STRU_AR_QUEUE_T STRU_AR_QUEUE_T;

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

STRU_AR_QUEUE_T *ar_creat_queue(uint32_t queue_size,char *name);
int ar_delete_queue(STRU_AR_QUEUE_T **queue);

/////a simple multi arch to push buffer for different element
///////////////////  dump file  ///////////////////////
typedef struct __STRU_DUMP_SERVER_T STRU_DUMP_SERVER_T;

struct  __STRU_DUMP_SERVER_T
{
  int current_index;
  int req_count;
  ar_lock_t dump_lock;
  ar_lock_t req_lock;
  STRU_AR_QUEUE_T *dump_queue;
  int  remote_fd;
  int dma_fd;
  ar_os_semaphore_id_t dump_queue_signal;
  ar_os_thread_id_t dump_task;
  void (*dump)(STRU_DUMP_SERVER_T *server_l,void *y,void *u,void *v,void *meta,int y_len,int u_len,int v_len,int meta_len);
  void (*dump_segment)(STRU_DUMP_SERVER_T *server_l,void *y,void *u,void *v,void *meta,int y_len,int u_len,int v_len,int meta_len);
  void (*dump_req)(STRU_DUMP_SERVER_T *server_l,char *name,int num);
};
STRU_DUMP_SERVER_T *creat_dump_server();
void delete_dump_server(STRU_DUMP_SERVER_T *server_l);

typedef struct
{
   ar_lock_t frame_lock;

}STRU_MULTI_CORE_T;

typedef struct
{
   void *va;
   void *pa;
   void *va_orig;
   void *pa_orig;
   int len;
   int used;
   int stride;
}STRU_MEM_PANNEL_T;

typedef struct __SRTU_PAD_FRAME_T__ SRTU_PAD_FRAME_T;

struct __SRTU_PAD_FRAME_T__
{
  int frameid;
  int ref;
  int x;
  int y;
  int w;
  int h;
  int pannel_cout;
  STRU_MEM_PANNEL_T pannel[6];
  int (*mem_free)(SRTU_PAD_FRAME_T *p_frame);
  void *priv[8];
};

typedef struct __STRU_ELMENT_T__ STRU_ELMENT_T;

typedef struct __STRU_PAD_T__ STRU_PAD_T;

typedef struct
{
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    int                      dev_fd;
    int                      layer_fd;
	STRU_DUMP_SERVER_T *p_dump;
}STRU_DISPLAY_OBJ_T;


typedef struct
{
    STRU_ELMENT_T           *p_element;
    STRU_PAD_T              *pad;
    int                      buffer_ref;
    AR_S32                   chan_id;
    int                      chan_fd;
	ar_os_thread_id_t surface_thread;
	int               stop_flag;

	//dislpay_obj
    ENUM_AR_HAL_VO_DEV_ID    e_dev_id;
    ENUM_AR_HAL_VO_LAYER_ID  e_layer_id;
    int                      dev_fd;
    int                      layer_fd;
	STRU_DUMP_SERVER_T *p_dump;
	int wait_times;
}STRU_SURFACE_OBJ_T;


struct __STRU_PAD_T__
{
   STRU_ELMENT_T *p_element;
   void *obj;
   char name[128];
   SRTU_PAD_FRAME_T *(*buffer_chain)(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad,SRTU_PAD_FRAME_T *p_frame);
   void *next;
   void *pairing;
};

struct __STRU_ELMENT_T__
{
  void *obj;
  char name[128];
  int in_pad_count;
  int out_pad_count;
  STRU_PAD_T *pad_in[8];
  STRU_PAD_T *pad_out[8];
};

STRU_ELMENT_T *creat_element(char *name,void *usrdata);
STRU_PAD_T *creat_pad(char *name,void *usrdata);
int add_out_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad);
int add_in_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad);
int push_to_next(STRU_ELMENT_T *p_element,STRU_PAD_T *p_current_pad,SRTU_PAD_FRAME_T *p_frame);
int link_pad(STRU_ELMENT_T *p_src_element,int out_pad_index,STRU_ELMENT_T *p_dest_element,int in_pad_index);




typedef struct
{
  STRU_ELMENT_T *p_element;
  STRU_PAD_T *pad;
  int buffer_ref;
}STRU_BUFFER_OBJ_T;

typedef struct
{
 int args_num;
 char args[256][64];
}STRU_CAMERA_CMD_T;

typedef struct __STRU_CAMERA_T__ STRU_CAMERA_T;

typedef struct
{
   char *p_name;
   int (*pfn_camera_test_run)(STRU_CAMERA_T *p_camera,STRU_CAMERA_CMD_T *p_cmd);
   char *p_help_string[MAX_HELP_STR_NUM];
}STRU_CAMERA_TEST_CASE;

typedef struct
{
  STRU_CAMERA_T *p_camera;
  int enable;
  int fd_stream;
  int stream_id;
  int client_id;
  STRU_AR_STREAM_PROPERTY_T stream_prop;
  STRU_STREAM_BUFFER_INIFO_T stream_buffer[MAX_STREAM_BUFER_COUNT];
  STRU_STREAM_BUFFER_INIFO_T stream_buffer_right[MAX_STREAM_BUFER_COUNT];
  ar_os_thread_id_t stream_thread;
  STRU_PAD_T *p_pad;
  AR_U32					  u32PoolId;
}STRU_STREAM_T;

struct __STRU_CAMERA_T__
{
  int dev_id;
  int pipe_id;
  AR_S32 fd_vin_dev;
  int fd_pipe;
  STRU_DEV_INFO_VAR_T sensor_var_pra;
  STRU_SENSOR_DEV_INFO_T dev_info;
  STRU_VIN_DRIVER_PUBLIC_PROPERTY_T public_prop;
  STRU_VIN_PIPE_PROPERTY_T pipe_prop;
  STRU_STREAM_T stream[MAX_STREAM_SIZE];
  int use_display;
  int use_scaler2;
  int use_raw;
  int use_hdr;
  int test_case_size;
  STRU_CAMERA_TEST_CASE *p_test_case;
  STRU_ELMENT_T *p_element;

  STRU_ELMENT_T *p_surface_element[MAX_STREAM_SIZE];
  STRU_ELMENT_T *p_buffer_element[MAX_STREAM_SIZE];
  ar_os_thread_id_t msg_loop;
  mqd_t mqid;
  ar_signal_t exit_signal;
  int need_server;
  int event_client_id;
  ar_os_thread_id_t  event_thread;
  int frames;
  STRU_DISPLAY_OBJ_T display_obj;
  STRU_DUMP_SERVER_T *p_dump;
  int big_pic_mode;
  int enable_display;
  AR_S32 device_type;
  STRU_BASIC_3A_INFO_T basic_3a_info;
  STRU_AEC_OUT_T aec_out;
  STRU_AWB_OUT_T awb_out;
  AR_S32 fd_mem;
  STRU_VIN_MEM_T tuning_req_mem;
  void *tuning_req_v;
  STRU_PIPE_SET_TUNING_REQUEST_T *tuning_req_pra;
  STRU_PIPE_SET_TUNING_REQUEST_T tuning_req_pra_prop;
  STRU_PIPE_SET_TUNING_REQUEST_T tuning_req_pra_data;
  int tuning_req_pra_saved;
};
//define sensor init segment
#define __test_camera_case__(__name)		\
	__test_camera_case_define__(STRU_CAMERA_TEST_CASE, __name)

#define __test_camera_case_define__(_type, _name)		\
	_type __test_camera_case__list_2##_name __attribute__((__aligned__(4)))		\
			__attribute__((unused,			\
			section(".__test_camera_case__list_2"#_name)))

#define __test_camera_case__start(_type)					\
({									\
	static char start[0] __attribute__((__aligned__(4))) __attribute__((unused,	\
		section(".__test_camera_case__list_1")));			\
	(_type *)&start;						\
})

#define __test_camera_case__end(_type)					\
({									\
	static char end[0] __attribute__((__aligned__(4))) __attribute__((unused,		\
		section(".__test_camera_case__list_3")));			\
	(_type *)&end;							\
})

#define __test_camera_case__count(_type)					\
	({								\
		_type *start = __test_camera_case__start(_type);		\
		_type *end = __test_camera_case__end(_type);		\
		unsigned int _ll_result = end - start;			\
		_ll_result;						\
	})
int camera_find_cmd_pra_index_by_name(char *name,STRU_CAMERA_CMD_T *p_cmd);
STRU_CAMERA_CMD_T *camera_construct_cmd(int argc, char *argv[]);
int camera_send_cmd_to_remote(STRU_CAMERA_T *p_camera,char *remote_name,STRU_CAMERA_CMD_T *p_cmd);
void *msg_loop_thread(void* arg);
STRU_ELMENT_T *creat_element(char *name,void *usrdata);
int delete_element(STRU_ELMENT_T *p_element);
STRU_PAD_T *creat_pad(char *name,void *usrdata);
int delete_pad(STRU_PAD_T *p_pad);
int add_out_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad);
int add_in_pad(STRU_ELMENT_T *p_element,STRU_PAD_T *p_pad);
int push_to_next(STRU_ELMENT_T *p_element,STRU_PAD_T *p_current_pad,SRTU_PAD_FRAME_T *p_frame);
int link_pad(STRU_ELMENT_T *p_src_element,int out_pad_index,STRU_ELMENT_T *p_dest_element,int in_pad_index);
void dump_dev_info(STRU_SENSOR_DEV_INFO_T *p_info);
void ar_vo_lcd_gpio_int(void);
int ar_vo_dev_init(STRU_DISPLAY_OBJ_T *vo_obj,int width,int height,int y_stride,
                            int uv_stride,float fps, AR_BOOL is_mipi_tx, AR_BOOL is_interlace);
int ar_vo_dev_deinit(STRU_DISPLAY_OBJ_T *vo_obj);
int ar_vo_surface_init(STRU_SURFACE_OBJ_T *vo_obj, STRU_AR_HAL_VO_RECT *chan_pos);


AR_S32 ar_hal_sys_mmz_alloc_cached(AR_U64* pu64_phy_addr, AR_VOID** p_vir_addr, const AR_CHAR* pstr_mmb, const AR_CHAR* pstr_zone, AR_U32 u32_len);
AR_S32 ar_hal_sys_mmz_free(AR_U64 u64_phy_addr, AR_VOID* p_vir_addr);

#endif
