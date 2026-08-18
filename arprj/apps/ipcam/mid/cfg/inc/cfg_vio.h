#ifndef __CFG_VIO_H__
#define __CFG_VIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

typedef struct
{
	AR_U32  mipi_id;
	AR_CHAR sensor[32]; /* sensor, e.g. "imx307", "imx291", etc...*/
    AR_U32 i2c_bus;
    AR_U32 width;    /*0:720; 1:960; 2:1080*/
    AR_U32 height;
    AR_U32 stride;
	AR_U32 fps;
	AR_U32 flip;  //new add
	AR_U32 mirror;
    AR_U32 HDR_enable;
    AR_U32 vfe_mode; //see ENUM_VFE_MODE
    AR_CHAR power_gpio[8];
    AR_CHAR reset_gpio[8];
    AR_CHAR comm_gpio[8];
    AR_U32 channel_0_enable;
    AR_U32 channel_0_fps;
    AR_U32 channel_1_enable;
    AR_U32 channel_1_width;
    AR_U32 channel_1_height;
    AR_U32 channel_1_stride;
    AR_U32 channel_1_fps;
    AR_U32 channel_2_raw_enable; //ch 2 is for raw data, the fps will be sensor original fps.
    AR_U32 raw_compress_enable; //disable it for tunning and getting original raw data.
} VI_PIPE_CFG;

//VO settings, type, parameters etc..
typedef struct
{
    AR_U32 enable;
    AR_CHAR type[32];
    AR_U32 lcd_id;
    AR_U32 width;
    AR_U32 height;
    AR_U32 stride;
    AR_U32 fps;
    AR_U32 square; // square x square display style.
} VO_CFG;

typedef struct
{
    AR_U32 cam_mode; //0: Normal(line mode), 1: DDR(offline mode), 2: multi camera mode.
    AR_U32 vb_blk_cnt;
    AR_U32 hdr_freq;
    AR_U32 isp_freq;
    AR_U32 vif_freq;
    AR_U32 mipi_freq;
}IPC_VI_DEV_CFG_S;

typedef struct
{
	VI_PIPE_CFG vi[IPC_MAX_PIPELINE_NUM]; //MAX sensors
	IPC_VI_DEV_CFG_S vi_cfg;
	VO_CFG vo;
} IPC_CFG_VIO_S;

typedef struct
{
    AR_U32 channel_0_width;
    AR_U32 channel_0_height;
    AR_U32 channel_1_width;
    AR_U32 channel_1_height;
} IPC_CFG_VIO_MAX_S;

typedef struct
{
    IPC_CFG_VIO_MAX_S stViMaxResource[IPC_MAX_PIPELINE_NUM];
} IPC_VI_MAX_RESOURCE_S;


AR_S32 IPC_CFG_VIO_Save();
AR_S32 IPC_CFG_VIO_Load(CFG_RUN_MODE_E enMode);
AR_S32 IPC_CFG_VIO_LoadDefault(CFG_RUN_MODE_E enMode);
void IPC_CFG_VIO_Print();
IPC_CFG_VIO_S * IPC_CFG_VIO_GetParam();
IPC_VI_MAX_RESOURCE_S * IPC_CFG_VIO_GetMaxResource();

#define VIO_CFG_FILE "cfg_vio.json"

#ifdef __cplusplus
}
#endif
#endif

