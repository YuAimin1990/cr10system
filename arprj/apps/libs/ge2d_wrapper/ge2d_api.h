#ifndef __GE2DE_H__
#define __GE2DE_H__
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "ar_common.h"
#include "ar_fifo.h"
#include "osal.h"

#define G2ED_FRAME_POOL_SIZE  16
#define G2ED_FRAME_MAX_SIZE   (1024*1024)
#define G2ED_MAIN_QUEUE_SIZE  (G2ED_FRAME_POOL_SIZE+8)
#define G2ED_MAX_CLIENT 128
#define MAX_POINT_NUM 68
#define MAX_MAP_TABLE_SIZE (AR_ALIGN32((MAX_POINT_NUM*sizeof(uint32_t)))*(MAX_POINT_NUM))
#define MAX_DOT_MAT_NUM 1024

#define USE_GLOBAL_BUFFER_DONE

typedef enum
{
    ARGB8888=0,
    ARGB4444,
    ARGB1555,
    YV12,
    NV12,
    MONOCHROMA,
    GRAY1,
    GRAY4,
    GRAY8
}ge2d_format_t;

//a frame is consisted by several commands
typedef struct
{
    uint32_t *phy_orgin_addr;
    uint32_t *frame_start_addr;
    int frame_len;
    uint32_t *frame_end_addr;
    ar_signal_t frame_done;
    int line_count;
    int frame_index;
    uint32_t *line_header_addr;
    uint32_t  *map_table;
    uint32_t  *map_table_phy;
    int th;
}frame_cmd_t;

typedef struct __ge2d_server_t__ ge2d_server_t;
typedef struct __ge2d_client_t__  ge2d_client_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int rgb; //line color
}line_data_t;

typedef struct
{
    int alfa_en;
    int line_w;
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    int end_line;
}line_append_data_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int format;
    int x;
    int y;
    int w;
    int h;
    int stride;
    int uv_stride;
    void *y_addr;
    void *u_addr;
    void *v_addr;
    int dest_x;
    int dest_y;
    int src_w;
    int src_h;
}bmp_data_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int x;
    int y;
    int w;
    int h;
    int stride;
    int front_color;
    int back_color;
    void *addr;
    int dest_x;
    int dest_y;
    int need_revert_color;
    int format;
    int th;
}dot_mat_data_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int x;
    int y;
    int w;
    int h;
    int stride;
    int uv_stride;
    void *y_addr;
    void *u_addr;
    void *v_addr;
    int back_color;
    int dest_x;
    int dest_y;
    float degree;
    int format;
    int src_w;
    int src_h;
}rotation_data_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int format;
    int x;
    int y;
    int w;
    int h;
    int src_w;
    int src_h;
    int stride;
    int uv_stride;
    void *y_addr;
    void *u_addr;
    void *v_addr;
    int dest_x;
    int dest_y;
    int dest_w;
    int dest_h;
}stretch_blit_data_t;

typedef struct
{
    int use_global_alfa;
    int alfa;
    int w;
    int h;
    int x;
    int y;
    int level;
}mosaic_data_t;

typedef struct
{
    int format;
    int use_global_alfa;
    int alfa;
    int x;
    int y;
    int w;
    int h;
    int stride;
    int uv_stride;
    void *y_addr;
    void *u_addr;
    void *v_addr;
    int color_key_low;
    int color_key_high;
    int dest_x;
    int dest_y;
    int src_w;
    int src_h;
}color_key_data_t;

typedef struct
{
    uint32_t cmd_offset_0;
    uint32_t cmd_offset_1;
    uint32_t cmd_offset_2;
    uint32_t yuv[3];
    uint32_t stats_addr;
}frame_start_t;

enum
{
    GE2D_CSC_MODE_BT601_TV,
    GE2D_CSC_MODE_BT601_PC,
    GE2D_CSC_MODE_BT709_TV,
    GE2D_CSC_MODE_BT709_PC,
};

struct __ge2d_client_t__
{
    char name[128];
    ge2d_server_t *server;
    ar_signal_t signal; //when client send a frame  request with the signal , then to wait the signal, when the frame process completed, the signal will be set
    //start frame structure
    frame_start_t start_frame;
    //format
    int w;
    int h;
    int format;
    int y_stride;
    int uv_stride;
    int csc_mode;
    //please user to malloc buffer to store dotmatric position
    int need_stats;
    void  *stats_buffer;
    void  *stats_buffer_orgin;
    void  *stats_buffer_phy;
    int   stats_flag;
    int frame_count;
    int current_frame_index;
    float map_table_x[MAX_POINT_NUM][MAX_POINT_NUM];
    float map_table_y[MAX_POINT_NUM][MAX_POINT_NUM];
    int (*client_set_format)(ge2d_client_t *client,int w,int h,int format,int y_stride,int uv_stride);
    int (*client_set_csc_mode)(ge2d_client_t *client,int csc_mode);
    int (*client_start)(ge2d_client_t *client);
    int (*client_stop)(ge2d_client_t *client);
    frame_cmd_t *(*malloc_frame)(ge2d_client_t *client);
    int (*free_frame)(ge2d_client_t *client,frame_cmd_t *frame);
    int (*process_frame)(ge2d_client_t *client,frame_cmd_t *frame);
    int (*frame_add_start)(ge2d_client_t *client,frame_cmd_t *frame,void *y,void *u,void *v);
    int (*frame_add_end)(ge2d_client_t *client,frame_cmd_t *frame); //always have a done int
    int (*frame_add_line)(ge2d_client_t *client,frame_cmd_t *frame,line_data_t *data);
    int (*frame_append_line)(ge2d_client_t *client,frame_cmd_t *frame,line_append_data_t *data);
    int (*frame_add_line_end)(ge2d_client_t *client,frame_cmd_t *frame);
    int (*frame_add_bmp)(ge2d_client_t *client,frame_cmd_t *frame,bmp_data_t *data);
    int (*frame_add_dot_mat)(ge2d_client_t *client,frame_cmd_t *frame,dot_mat_data_t *data);
    int (*frame_add_rotation)(ge2d_client_t *client,frame_cmd_t *frame,rotation_data_t *data);
    int (*frame_add_mosaic)(ge2d_client_t *client,frame_cmd_t *frame,mosaic_data_t *data);
    int (*frame_add_color_key)(ge2d_client_t *client,frame_cmd_t *frame,color_key_data_t *data);
    int (*frame_add_stretch_blit)(ge2d_client_t *client,frame_cmd_t *frame,stretch_blit_data_t *data);
};

struct __ge2d_server_t__
{
    ar_lock_t lock;
    int mpp_dev_fd;
    //for osd task get process frame request
    ar_queue_t *ge2d_main_queue;
    ar_signal_t ge2d_main_queue_signal;
    //frame pool for frambuffer
    ar_queue_t *ge2d_frame_pool_queue;
    ar_signal_t ge2d_frame_pool_queue_signal;
    frame_cmd_t *frame[G2ED_FRAME_POOL_SIZE];
    //client
    ge2d_client_t *client_list[G2ED_MAX_CLIENT];
    //creat client for user api
    ge2d_client_t* (*creat_client)(ge2d_server_t *server,char *name);
    int (*delete_client)(ge2d_server_t *server,ge2d_client_t *client);
};

ge2d_server_t *get_ge2d_server();

#endif
