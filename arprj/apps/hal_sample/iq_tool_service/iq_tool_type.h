#ifndef __IQ_TOOL_TYPE_H__
#define __IQ_TOOL_TYPE_H__
#include "hal_vin_type_def.h"
#include "hal_vcodec_type_comm.h"

typedef enum{
    TUNING_REQ_CONNECT = TUNING_REQ_MAX + 1,
    TUNING_REQ_GET_CAM_LSIT,
    TUNING_REQ_START_PREVIEW,
    TUNING_REQ_STOP_PREVIEW,
    TUNING_REQ_PREVIEW_FRAME,
    TUNING_REQ_RAW_FRAME,
    TUNING_REQ_SNAPSHOT_YUV_FRAME,
    TUNING_REQ_SNAPSHOT_JPG_FRAME,
    TUNING_REQ_STATS_META,
    TUNING_REQ_SEND_RAW_FRAME,
    TUNING_REQ_DIS_CONNECT,
    TUNING_REQ_VIDEO_BITSTREAM,
    TUNING_REQ_HDR_RAW_FRAME,
    TUNING_REQ_GET_TUNING_BIN,
    TUNING_REQ_SET_TUNING_BIN,
    TUNING_REQ_RELOAD_TUNING_BIN,
    TUNING_SEND_RAW_FRAME,
    TUNING_TRIGGER_FETCH_RAW,    
    TUNING_REQ_AF_FRAME,
    //venc tuning req start
    VENC_REQ_GET_VENC_INFO      = 0x00000100,
    VENC_REQ_CONNECT            = 0x00000101,
    VENC_REQ_DIS_CONNECT        = 0x00000102,
    VENC_REQ_GET_DYNAMIC_PARAMS = 0x00000103,
    VENC_REQ_SET_DYNAMIC_PARAMS = 0x00000104,
    //infrared tuning req cmd
    INFRARED_REQ_CONNECT        = 0x00000200,
    INFRARED_REQ_GET_SETTING    = 0x00000201,
    INFRARED_REQ_SET_SETTING    = 0x00000202,
}ENUM_IQ_REQ_ID;

enum
{
   CONNECT_MASTER,
   CONNECT_ASSISTENT,
};

enum
{
    NORMAL_MODE,
    SIM_RAW_MODE,
};




typedef struct
{
   AR_S32 request_type;
   AR_CHAR diag[64];
   AR_S32  connect_mod;
   AR_S32  cam_id;
}STRU_TUNING_REQ_CONNECT_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_CHAR diag[64];
}STRU_TUNING_REQ_CONNECT_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 count;
   AR_CHAR sensor[8][64];
   AR_CHAR dev_node[8][64];
}STRU_TUNING_REQ_GET_CAM_LSIT_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 count;
   AR_CHAR sensor[8][64];
}STRU_TUNING_REQ_GET_CAM_LSIT_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_CHAR name[64];
   AR_S32 width;
   AR_S32 height;
   AR_S32 is_dng_raw;
   AR_S32 hdr_stream;
   AR_S32 fps;
   AR_S32 cam_mode;
}STRU_TUNING_REQ_START_PREVIEW_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_START_PREVIEW_ACK_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_STOP_PREVIEW_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_STOP_PREVIEW_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
}STRU_TUNING_REQ_PREVIEW_FRAME_PRA_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_PREVIEW_FRAME_ACK_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
}STRU_TUNING_REQ_AF_FRAME_PRA_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_AF_FRAME_ACK_T;



typedef struct
{
   AR_S32 w;
   AR_S32 h;
   AR_CHAR   format_sting[64];
   AR_S32    data_offset;
   STRU_BASIC_3A_INFO_T basic_info;
}STRU_AR_RAW_HEADER_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
   AR_S32 is_no_loss_frame_mod;
   AR_S32 with_3a;
}STRU_TUNING_REQ_RAW_FRAME_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_RAW_FRAME_ACK_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
   AR_S32 is_no_loss_frame_mod;
   AR_S32 bit_rate;//the unit is k
   AR_CHAR bit_stream_dev_name[128];
}STRU_TUNING_REQ_VIDEO_BITSTREAM_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_VIDEO_BITSTREAM_ACK_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
   AR_S32 is_no_loss_frame_mod;
}STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_SNAPSHOT_YUV_FRAME_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
}STRU_TUNING_REQ_SNAPSHOT_JPG_FRAME_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
   AR_U32 frame_id;
}STRU_TUNING_REQ_SNAPSHOT_JPG_FRAME_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 data_flag;
   //when not data
   AR_S32 panels;
   AR_S32 burst_count[3];
   AR_S32 width;
   AR_S32 height;
   AR_S32 format;
   //when is data
   AR_S32 burst_ref;
   AR_S32 yuv_panel;
   AR_S32 burst_len;
}STRU_TUNING_REQ_SEND_RAW_FRAME_PRA_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 expected_frame_count;
}STRU_TUNING_REQ_SEND_RAW_FRAME_ACK_T;
typedef struct
{
    AR_S32 trigger; //start/stop trigger raw to isp, 0: stop;1:start
    AR_S32 seq_mod; //frame sequece: 0:1-->2-->3; 1: 1-->2-->3-->2-->1
    AR_S32 start_idx; //which frame start trigger
    AR_S32 seq_count; //how many frames to be triggered
    AR_S32 times; //ow many seq count to trigger
    AR_S32 fps;
}STRU_TUNING_TRIGGER_FETCH_RAW_PRA_T;
typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_STATS_MEATA_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_STATS_MEATA_ACK_T;

/*
   获取当前使用的tuning 参数对应的文件。
   返回文件数据和文件名
*/

typedef struct
{
    AR_S32 request_type;
    AR_S32 tuning_type;
}STRU_TUNING_REQ_GET_TUNING_BIN_PRA_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 file_len;
   AR_CHAR name[128];
}STRU_TUNING_REQ_GET_TUNING_BIN_ACK_T;

/*
   把传输来的文件写在指定的位置，并切换到当前文件对应的tuning 参数上
*/
typedef struct
{
    AR_S32 request_type;
    AR_CHAR file_name[128];
    STRU_ALGO_ISP_TUNING_T  tuning_data;
}STRU_TUNING_REQ_SET_TUNING_BIN_PRA_T;

typedef struct
{
    AR_S32 request_type;
}STRU_TUNING_REQ_SET_TUNING_BIN_ACK_T;

/*
   把isp 当前的tuning 参数保存到指定的文件中，文件带路径。
   如果文件系统为只读，保存失败。
*/

typedef struct
{
    AR_S32 request_type;
}STRU_TUNING_REQ_SAVE_BIN_ACK_T;


typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
}STRU_TUNING_REQ_READ_TUNING_PRA_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_WRITE_TUNING_PRA_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_LOCK_TUNING_PRA_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_UNLOCK_TUNING_PRA_ACK_T;

typedef struct
{
   AR_S32 request_type;

}STRU_TUNING_REQ_GET_3A_INFO_PRA_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_LOCK_AE_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_SET_MANUAL_AE_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_LOCK_AWB_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_UNLOCK_AWB_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_SET_MANUAL_AWB_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_LOCK_AF_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_UNLOCK_AF_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_SET_AF_MOD_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_TRIGGER_AF_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_ENDEN_ISP_MODULE_ACK_T;
typedef struct
{
   AR_S32 request_type;
   AR_S32 mode;
}STRU_TUNING_REQ_DIS_CONNECT_PRA_T;
typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_DIS_CONNECT_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_SENSOR_WRITE_ACK_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 reg_num;
   AR_U32 address[MAX_REG_NUM];
}STRU_TUNING_REQ_SENSOR_READ_PRA_T;

typedef struct
{
    AR_S32 request_type;
    AR_S32 cam_res;
    AR_S32 cam_sensce;
    AR_CHAR sensor_name[64];
}STRU_TUNING_REQ_RELOAD_BIN_PRA_T;

typedef struct
{
   AR_S32 request_type;
   AR_S32 tuning_type;
}STRU_TUNING_REQ_READ_CUR_ISP_PRA_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_UNLOCK_AE_ACK_T;

typedef struct
{
   AR_S32 request_type;
}STRU_TUNING_REQ_SET_MANUAL_AF_ACK_T;
/********************** VCODEC  STRUCT ******************************/
typedef struct
{
   AR_S32 request_id;//VENC_REQ_GET_VENC_INFO
}STRU_VENC_REQ_GET_VENC_INFO_T;
typedef STRU_VENC_CTRL_INFO STRU_VENC_REQ_GET_VENC_INFO_ACK_T;
typedef struct
{
    AR_S32 request_id;//VENC_REQ_CONNECT
    AR_S32 len;
    AR_S32 venc_id;//venc id
}STRU_VENC_REQ_CONNECT_T;
typedef struct
{
    AR_S32 request_id;//VENC_REQ_DIS_CONNECT
    AR_S32 len;
    AR_S32 venc_id;//venc id
}STRU_VENC_REQ_DISCONNECT_T;
typedef struct
{
    AR_S32 request_id;//VENC_REQ_GET_DYNAMIC_PARAMS
    AR_S32 len;
    AR_S32 venc_id;//venc id
}STRU_VENC_REQ_GET_DYNAMIC_PARAM_T;
typedef STRU_VENC_DPARAM STRU_VENC_REQ_GET_DYNAMIC_PARAM_ACK_T;
typedef struct
{
    AR_S32 request_id;//VENC_REQ_SET_DYNAMIC_PARAMS
    AR_S32 len;
    AR_S32 venc_id;//venc id
    STRU_VENC_DPARAM stVencDparam;
}STRU_VENC_REQ_SET_DYNAMIC_PARAM_T;
#endif
