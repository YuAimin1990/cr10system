/*****************************************************************************
Copyright: 2016-2020, Artosyn. Co., Ltd.
File name: _DISPLAY_API_H_
Description: define the cam user api, use the api, the usr can use the cmera to privew ,capture picture
Author: llli@artosyn.com
Version: v1.0
Date:2018-04-24
History:2018-04-24 : first release sdk
*****************************************************************************/
#ifndef _DISPLAY_API_H_
#define _DISPLAY_API_H_

#include <stdlib.h>
#include <stdint.h>
//#include <common.h>
#include <config.h>
#include <errno.h>
#include <malloc.h>

#include <string.h>
#include <stdio.h>
#include "vo/display_hal/hal_vo.h"

enum
{
	AR_DISP_OK   =   0,
	AR_DISP_ERR	 =  -1,
};

typedef struct
{
  void * buffer;
  void * buffer_orign;
  void * buffer_pa;
  void * buffer_pa_orign;
  uint32_t length;
  uint32_t line_buffer_len;
}disp_pannel_t;

typedef struct
{
  uint32_t frame_id;
  uint32_t lowdelay_index;
  uint32_t timestamps;
  uint64_t timestamps_us;
  int panel_count;
  disp_pannel_t pannel[3];
  int interlace_filed_flag;
  int x;
  int y;
  void *priv;
  uint64_t timestamps_enqueue;
  uint64_t timestamps_cfg;
  uint64_t timestamps_show;
  uint64_t timestamps_done;
}display_buffer_t;



/*
 * The format of the framebuffer
 */
typedef enum{
    Input_XRGB4444,
    Input_ARGB4444,
    Input_XRGB1555,
    Input_ARGB1555,
    Input_RGB565,  // 04
    Input_XRGB8888,
    Input_ARGB8888,
    Input_YUY2, // 07
    Input_UYVY,
    Input_INDEX8,
    Input_MONOCHROME, // 0A
    Input_YV12 = 0xF,
    Input_A8, // 0x10
    Input_NV12,
    Input_NV16,
    Input_RG16,
    Input_R8, // 0x14
    Input_NV12_10BIT, //0x15
    Input_ARGB2101010,
    Input_NV16_10BIT,
    Input_INDEX1,
    Input_INDEX2, //0x19
    Input_INDEX4, //0x1A
    Input_P010, //0x1B
    Input_NV12_10BIT_L1, // 0x1C  3P4B
    Input_NV16_10BIT_L1, //0x1D  3P4B
    Input_MAX
}display_format_t;


/*display cursor ops*/
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

typedef struct
{
    unsigned int x;
	unsigned int y;
}cursor_pos_t;

typedef cursor_pos_t overlay_pos_t;

typedef int (*pfn_display_cb)(void *usr_data);

typedef void* display_handle_t;
/*
brief: ar_display_enqueue ,send a buffer to display to show
param:1 display handle, which is get by ar_creat_display_layer 2 buffer, the buffer will be display
retval: 0 sucess , other failed
note:
*/

int ar_display_enqueue(display_handle_t handle,display_buffer_t *buffer);
/*
brief: ar_display_dequeue ,get a buffer from display
param:1 display handle, which is get by ar_creat_display_layer
retval: 0 success , other failed
note:
*/
display_buffer_t *ar_display_dequeue(display_handle_t handle);
/*
brief: ar_get_display_layer
param:no param
retval: NULL, failed, a display_buffer_t  will be return if success
note:
*/
display_handle_t ar_get_display_layer(int layer_id);

/*
brief: get_display_dev
param:no param
retval: NULL, failed, a display_handle_t  will be return if success
note:
*/
display_handle_t *get_display_dev(void);

/*
brief: ar_init_display
param:the handle
retval:  0 success , other failed
note:the first fuction to call when start a camera
*/

int ar_init_display(display_handle_t handle,int id);
int ar_display_set_overlay_id(display_handle_t handle,int id);
int ar_display_set_use_background(int use_background,display_buffer_t *buffer);

/*
brief: ar_cfg_display_out
param:1 handle, 2 width 3 height
retval: 0 success , other failed
note:
*/
int ar_cfg_display_out(display_handle_t handle,int width,int height);

/*
 brief: ar_cfg_display_interlace_mode
param:1 handle, 2  interlace mode
retval: 0 success , other failed
note:
*/

int ar_cfg_display_interlace_mode(display_handle_t handle,int interlace_mode);


/*
brief: ar_cfg_flush_cb
param:1 handle, 2 cb 3void* usrdata
retval: 0 success , other failed
note:
*/
int ar_cfg_flush_cb(display_handle_t handle,pfn_display_cb cb,void *usrdata);

/*
brief: ar_cfg_start_display
param:1 handle, 2 width 3 height 4 format ,ref to display_format_t
retval: 0 success , other failed
note:
*/
int ar_cfg_start_display(display_handle_t handle,int width,int height,int format,float fps, int luma_stride, int chroma_stride);
/*
brief: ar_pause_display
param:handle
retval: 0 success , other failed
note:
*/
int ar_pause_display(display_handle_t handle);
void ar_display_cfg_cursor(display_handle_t handle,char *cursor_file);
void ar_display_cursor_enable(display_handle_t handle);
void ar_display_cursor_disable(display_handle_t handle);
void ar_display_cursor_set(display_handle_t handle,unsigned int x,unsigned int y);
void ar_display_cursor_get(display_handle_t handle,unsigned int *x,unsigned int *y);
void ar_display_overlay_set(display_handle_t handle,unsigned int x,unsigned int y);
void ar_display_zoom(display_handle_t handle,float zoom);
int ar_display_interface_cfg_csc(int csc_pra);
int ar_display_set_csc(display_handle_t handle,int format,int convert_matric, int change_range);
int ar_display_set_layer_csc(display_handle_t handle, STRU_AR_HAL_VO_LAYER_CSC *csc);
int  ar_display_pannel_buffer_malloc(disp_pannel_t *pannel, int size);
int ar_display_dev_enable(void);
int ar_display_dev_disable(void);
int ar_display_layer_enable(display_handle_t handle,int width,int height,int format,
                                        float fps, int luma_stride, int chroma_stride);
int ar_display_layer_disable(display_handle_t handle);

int ar_display_layer_flush_all_buffer(display_handle_t handle, AR_BOOL clr_all);
void ar_display_init(void);

#endif
