#ifndef __CFG_VIO_H__
#define __CFG_VIO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"
#include "ar_comm_video.h"
#include "ar_comm_sns.h"

#if 0
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
#endif

//new define for isp raw simulation, only the most common parameters
typedef struct
{
    RECT_S stWndRect;  /**<RW; Start position of the cropping window, image width, and image height */
    SIZE_S stSnsSize;  /**<RW; Width and height of the image output from the sensor*/
    ISP_BAYER_FORMAT_E enBayer;
} VI_CFG_ISP_PUB_ATTR;

typedef struct
{
    RECT_S stRect;
    data_type_t enMipiDataType;
} VI_CFG_COMBO_ATTR;

typedef struct
{
    AR_U32 u32Width;
    AR_U32 u32Height;
    AR_U32 u32BayerScaleWidth;
    AR_U32 u32BayerScaleHeight;
} VI_CFG_DEV_ATTR;
typedef struct
{
    AR_U32 u32MaxW;  /**<输入图像宽度。静态属性，创建PIPE时设定，不可更改。*/
    AR_U32 u32MaxH;  /**<输入图像高度。静态属性，创建PIPE 时设定，不可更改。*/
    PIXEL_FORMAT_E enPixFmt;
    DATA_BITWIDTH_E enBitWidth;
    PIXEL_FORMAT_E enNrPixFmt;               /**<@note 不使用 重构帧的像素格式。*/
    DATA_BITWIDTH_E     enNrBitWidth;             /**<@note 不使用 重构帧的bit位宽。*/
    COMPRESS_MODE_E     enNrCompressMode;
} VI_CFG_PIPE_ATTR;

typedef struct
{
    SIZE_S stSize;  /**<目标图像大小。*/
    PIXEL_FORMAT_E enPixelFormat; /**<目标图像像素格式。静态属性，设置CHN时设定，不可更改。*/
} VI_CFG_CHN_ATTR;

typedef struct
{
    AR_U32  mipi_id;
    AR_CHAR sensor[32]; /* sensor, e.g. "imx307", "imx291", etc...*/
    VI_CFG_ISP_PUB_ATTR stPubAttr;
    VI_CFG_COMBO_ATTR stComboAttr;
    VI_CFG_DEV_ATTR stDevAttr;
    VI_CFG_PIPE_ATTR stPipeAttr;
    VI_CFG_CHN_ATTR stChnAttr;

    AR_U32 channel_1_enable;
    AR_U32 channel_1_width;
    AR_U32 channel_1_height;
    AR_U32 channel_1_stride;
    AR_U32 fps; //fake
    AR_U32 channel_2_raw_enable; //ch 2 is for raw data, the fps will be sensor original fps.
} VI_CFG;

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
	VI_CFG vi[IPC_MAX_PIPELINE_NUM]; //MAX sensors
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

