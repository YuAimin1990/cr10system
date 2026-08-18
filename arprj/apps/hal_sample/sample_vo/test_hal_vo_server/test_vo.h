#ifndef __TEST_VO_H__
#define __TEST_VO_H__
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include "hal_type.h"
#include "hal_region.h"
#include "hal_vo.h"
#include "hal_vb.h"

#define ALIGN_UP(x, a)     ( ( ( (x) + ((a) - 1) ) / a ) * a )

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

typedef struct
{
    AR_VOID  *y_vrt;
    AR_VOID  *u_vrt;
    AR_VOID  *v_vrt;

    AR_VOID  *y_phy;
    AR_VOID  *u_phy;
    AR_VOID  *v_phy;

    AR_VB_BLK vb_blk;
}STRU_ADDR;

typedef struct
{
    unsigned char type[2];
    unsigned char size[4];
    unsigned char retain[4];
    unsigned char offset[4];
    unsigned char head_len[4];
    unsigned char width[4];
    unsigned char height[4];
    unsigned char planes[2];
    unsigned char bitCount[2];
}bmp_head_t;

typedef struct
{
    AR_U32     id;
    AR_U32     buf_num;
    STRU_ADDR  buf[MAX_REGION_BUFFER];
}STRU_REGION_OBJ;

typedef struct
{
    AR_S32                  layer_id;
    AR_S32                  chn_id;
    AR_S32                  chn_fd;
}STRU_VO_CHN_THREAD_PARA;

typedef struct
{
    int        dev_fd;

	int        layer_fd;
    int        chan_fd[MAX_VO_CHN_NUM];
	ar_thread_id_t thread_id_send[MAX_VO_CHN_NUM*AR_HAL_VO_LAYER_ID_MAX];
	ar_thread_id_t thread_id_recycle[MAX_VO_CHN_NUM*AR_HAL_VO_LAYER_ID_MAX];

	int        overlayer_fd[2];
    int        overlay_chan_fd[2][MAX_VO_CHN_NUM];

	AR_VB_POOL src_pool_id;

	ar_queue_id_t video_frame_queue[MAX_VO_CHN_NUM];

	ar_queue_id_t overlay_video_frame_queue[2][MAX_VO_CHN_NUM];
}STRU_VO_OBJ_T;

void recyle_channel_buffer(int layer_id, int chn_id);

#endif

