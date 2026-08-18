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
#include "sample_lcd.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "hal_vb.h"
#include "hal_type.h"

#define MAX_HELP_STR_NUM 64
#define MAX_STREAM_SIZE 6
#define MAX_STREAM_BUFER_COUNT 5

#define BIGPIC_RAW_FILE_NUM_MAX 10

#define AR_BIGPIC_OFFSET_NUM_H 2
#define AR_BIGPIC_OFFSET_NUM_V 1
#define AR_BIGPIC_OFFSET_NUM (AR_BIGPIC_OFFSET_NUM_H*AR_BIGPIC_OFFSET_NUM_V)

#define AR_BIGPIC_STATS_QUEUE_SIZE 3



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



typedef struct
{
    AR_VOID *p_vaddr;
    AR_U64 phy_addr;
}STRU_MMZ_ADDR_T;

typedef struct __STRU_CAMERA_T__ STRU_CAMERA_T;

typedef struct
{
    int offset_id;
    STRU_CAMERA_T *p_camera;
}STRU_STAT_CTX_T;

struct __STRU_CAMERA_T__
{
    SAMPLE_VI_CONFIG_S stViConfig;
    SAMPLE_SNS_TYPE_E enSnsType[3];
    AR_S32 pipe_id[3];
    AR_S32 ch_id[3];
    AR_S32 vo_ch_id;
    pthread_t display_thread;
    int use_display;

    pthread_t msg_loop;
    ar_signal_t exit_signal;

    STRU_DISPLAY_OBJ_T display_obj;
	AR_S32 stop_flag;
    pthread_t surface_thread;

    int big_pic_mode;
    int enable_display;

    AR_VB_POOL u32_pool_id_L;
    AR_VB_POOL u32_pool_id_R;
    AR_VB_POOL u32_pool_id_C;
    AR_VB_POOL u32_pool_id_F;
    AR_U64 *u64_blk_addr_L;
    AR_VOID **p_blk_vaddr_L;
    AR_U64 *u64_blk_addr_R;
    AR_VOID **p_blk_vaddr_R;
    AR_U64 phy_addr[MAX_STREAM_BUFER_COUNT];
    AR_VOID *p_vaddr[MAX_STREAM_BUFER_COUNT];

    STRU_BASIC_3A_INFO_T basic_3a_info;
    STRU_AEC_OUT_T aec_out;
    STRU_AWB_OUT_T awb_out;

    pthread_t server_thread;
    pthread_t big_pic_thread;
    int big_save_num;
    int big_pic_stop;

    int preview_stream_width;
    int preview_stream_height;
    int preview_stream_stride;

    int bigpic_raw_stream_width;
    int bigpic_raw_stream_height;
    int bigpic_isp_in_stride;

    int bigpic_isp_in_width;
    int bigpic_isp_in_height;
    int bigpic_isp_in_overlap_h;
    int bigpic_isp_in_overlap_h_hdr_gap;
    int bigpic_isp_in_overlap_v;
    int bigpic_isp_in_offset_h;
    int bigpic_isp_in_offset_v;

    int bigpic_width;
    int bigpic_height;
    int bigpic_isp_out_stride;

    int bigpic_isp_out_width;
    int bigpic_isp_out_height;
    int bigpic_isp_out_overlap_h;
    int bigpic_isp_out_overlap_v;
    int bigpic_isp_out_offset_h;
    int bigpic_isp_out_offset_v;

    int bigpic_raw_file_num;
    char bigpic_raw_file_name[BIGPIC_RAW_FILE_NUM_MAX][256];
    AR_VOID *bigpic_raw_file_buffer[BIGPIC_RAW_FILE_NUM_MAX];
    AR_U64 *bigpic_raw_file_buffer_phy[BIGPIC_RAW_FILE_NUM_MAX];
    VB_BLK bigpic_raw_file_buffer_vb_blk[BIGPIC_RAW_FILE_NUM_MAX];

    int big_pic_3a_auto_runing;
    int big_pic_3a_auto;
    pthread_t big_pic_3a_auto_thread;
    STRU_STAT_CTX_T stAeStatCtx[AR_BIGPIC_OFFSET_NUM];
    STRU_STAT_CTX_T stAwbStatCtx[AR_BIGPIC_OFFSET_NUM];
    STRU_STAT_CTX_T stAfStatCtx[AR_BIGPIC_OFFSET_NUM];
    pthread_t big_pic_statistics_ae_thread[AR_BIGPIC_OFFSET_NUM];
    pthread_t big_pic_statistics_awb_thread[AR_BIGPIC_OFFSET_NUM];
    pthread_t big_pic_statistics_af_thread[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_ae_signal[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_awb_signal[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_af_signal[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_ae_merge_signal[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_awb_merge_signal[AR_BIGPIC_OFFSET_NUM];
    ar_signal_t big_pic_statistics_af_merge_signal[AR_BIGPIC_OFFSET_NUM];
    STRU_AEC_CB_EVENT_RUN_PRA_T stAeRunPra;
    STRU_AWB_CB_EVENT_RUN_PRA_T stAwbRunPra;
    STRU_AF_CB_EVENT_RUN_PRA_T stAfRunPra;
    STRU_MMZ_ADDR_T stAeAlgoLibOutputAddr;
    STRU_MMZ_ADDR_T stAwbAlgoLibOutputAddr;
    STRU_MMZ_ADDR_T stAfAlgoLibOutputAddr;
    pthread_t big_pic_update_ae_thread;
    pthread_t big_pic_update_awb_thread;
    pthread_t big_pic_update_af_thread;
    STRU_AR_QUEUE_T *pstAeStatQ[AR_BIGPIC_OFFSET_NUM];
    STRU_AR_QUEUE_T *pstAeStatMergeQ[AR_BIGPIC_OFFSET_NUM];
    STRU_MMZ_ADDR_T stAeStatAddr[AR_BIGPIC_OFFSET_NUM][AR_BIGPIC_STATS_QUEUE_SIZE];
    STRU_AR_QUEUE_T *pstAwbStatQ[AR_BIGPIC_OFFSET_NUM];
    STRU_AR_QUEUE_T *pstAwbStatMergeQ[AR_BIGPIC_OFFSET_NUM];
    STRU_MMZ_ADDR_T stAwbStatAddr[AR_BIGPIC_OFFSET_NUM][AR_BIGPIC_STATS_QUEUE_SIZE];
    STRU_AR_QUEUE_T *pstAfStatQ[AR_BIGPIC_OFFSET_NUM];
    STRU_AR_QUEUE_T *pstAfStatMergeQ[AR_BIGPIC_OFFSET_NUM];
    STRU_MMZ_ADDR_T stAfStatAddr[AR_BIGPIC_OFFSET_NUM][AR_BIGPIC_STATS_QUEUE_SIZE];
};

#endif
