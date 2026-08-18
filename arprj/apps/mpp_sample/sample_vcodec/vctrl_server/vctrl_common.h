#ifndef __VCTRL_COMMON__
#define __VCTRL_COMMON__

#include "ar_common.h"
#include "ar_comm_video.h"
#include "ar_comm_sys.h"
#include "ar_comm_vb.h"
#include "hal_vin_tuning_def.h" //why need it
#include "ar_comm_vi.h"
#include "hal_sys.h"
#include "ar_buffer.h"
#include "hal_vb.h"
#include "hal_type.h"
#include "mpi_sys.h"
#include "vctrl_msg.h"
#include "utils_dbglog.h"
#define LOG_TAG (196613)

#define logline(fmt, ...) AR_LOG_INFO(LOG_TAG, " " fmt "\n",  ##__VA_ARGS__)

#define VCTRL_PIXEL_FORMAT         PIXEL_FORMAT_YVU_SEMIPLANAR_420
typedef unsigned int combo_dev_t;

typedef enum arVCTRL_SNS_TYPE_E
{
    SONY_IMX290_MIPI_2M_30FPS_12BIT,
    ISP_VIN_0_MEM_2M_30FPS_12BIT,
    SONY_IMX290_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX290_SLAVE_MIPI_2M_60FPS_10BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1,
    SONY_IMX377_MIPI_8M_30FPS_10BIT,
    SONY_IMX415_MIPI_8M_30FPS_12BIT,
    NVP6324_MIPI_2M_30FPS_8BIT,
    TP9930_DVP_4VC_1080P_25FPS,
    VCTRL_SNS_TYPE_BUTT,
} VCTRL_SNS_TYPE_E;


typedef struct arVCTRL_SENSOR_INFO_S
{
    VCTRL_SNS_TYPE_E    enSnsType;
    AR_S32              s32SnsId;
    AR_S32              s32BusId;
    combo_dev_t         MipiDev;
} VCTRL_SENSOR_INFO_S;

typedef struct arVCTRL_DEV_INFO_S
{
    VI_DEV      ViDev;
    WDR_MODE_E  enWDRMode;
} VCTRL_DEV_INFO_S;


typedef struct arVCTRL_PIPE_INFO_S
{
    VI_PIPE         aPipe[4];
    VI_VPSS_MODE_E  enMastPipeMode;
    AR_BOOL         bMultiPipe;
    AR_BOOL         bVcNumCfged;
    AR_U32          u32VCNum[4];
} VCTRL_PIPE_INFO_S;

typedef struct arVCTRL_CHN_INFO_S
{
    VI_CHN              ViChn;
    PIXEL_FORMAT_E      enPixFormat;
    DYNAMIC_RANGE_E     enDynamicRange;
    VIDEO_FORMAT_E      enVideoFormat;
    COMPRESS_MODE_E     enCompressMode;
} VCTRL_CHN_INFO_S;

typedef struct arVCTRL_SNAP_INFO_S
{
    AR_BOOL         bSnap;
    AR_BOOL         bDoublePipe;
    VI_PIPE         VideoPipe;
    VI_PIPE         SnapPipe;
    VI_VPSS_MODE_E  enVideoPipeMode;
    VI_VPSS_MODE_E  enSnapPipeMode;
}VCTRL_SNAP_INFO_S;


typedef struct arVCTRL_VI_INFO_S
{
    VCTRL_SENSOR_INFO_S    stSnsInfo;
    VCTRL_DEV_INFO_S       stDevInfo;
    VCTRL_PIPE_INFO_S      stPipeInfo;
    VCTRL_CHN_INFO_S       stChnInfo;
    VCTRL_SNAP_INFO_S      stSnapInfo;
} VCTRL_VI_INFO_S;


typedef struct arVCTRL_VI_CONFIG_S
{
    VCTRL_VI_INFO_S     astViInfo[VI_MAX_DEV_NUM];
    AR_S32              as32WorkingViId[VI_MAX_DEV_NUM];
    AR_S32              s32WorkingViNum;
} VCTRL_VI_CONFIG_S;

typedef enum arPIC_SIZE_E
{
    PIC_CIF,
    PIC_D1_PAL,    /* 720 * 576 */
    PIC_D1_NTSC,   /* 720 * 480 */
    PIC_720P,      /* 1280 * 720  */
    PIC_1080P,     /* 1920 * 1080 */
    PIC_2592x1520,
    PIC_2592x1944,
    PIC_3840x2160,
    PIC_4096x2160,
    PIC_3000x3000,
    PIC_4000x3000,
    PIC_7680x4320,
    PIC_3840x8640,
    PIC_BUTT
} PIC_SIZE_E;

typedef enum arVCTRL_RC_E
{
    VCTRL_RC_CBR = 0,
    VCTRL_RC_VBR,
    VCTRL_RC_AVBR,
    VCTRL_RC_QPMAP,
    VCTRL_RC_FIXQP
} VCTRL_RC_E;


typedef struct arVCTRL_VI_FRAME_CONFIG_S
{
    AR_U32                  u32Width;
    AR_U32                  u32Height;
    AR_U32                  u32ByteAlign;
    PIXEL_FORMAT_E          enPixelFormat;
    VIDEO_FORMAT_E          enVideoFormat;
    COMPRESS_MODE_E         enCompressMode;
    DYNAMIC_RANGE_E         enDynamicRange;
} VCTRL_VI_FRAME_CONFIG_S;


typedef struct arVCTRL_VI_FRAME_INFO_S
{
    VB_BLK             VbBlk;
    VB_POOL            u32PoolId;
    AR_U32             u32Size;
    VIDEO_FRAME_INFO_S stVideoFrameInfo;
} VCTRL_VI_FRAME_INFO_S;


typedef struct sender_data
{
    FILE* pfd;
    VI_USERPIC_ATTR_S stUsrPic;
    VCTRL_VI_FRAME_INFO_S stViFrameInfo;
    ENMU_SYS_MOD_ID e_mod_id;
    STRU_SYS_BIND_SENDER * p_sender;
    VB_POOL u32PoolId;
}ViData;

typedef struct arVCTRL_VI_BIND_INFO_S
{
    STRU_SYS_BIND_SENDER stViBind;
    ViData * stViData;
    char * inFile;
    char * ouFile;
    AR_U32 u32Width;
    AR_U32 u32Height;
}VCTRL_VI_BIND_INFO;

AR_S32 VCTRL_COMM_SYS_GetPicSize(PIC_SIZE_E enPicSize, SIZE_S* pstSize);
AR_VOID VCTRL_COMM_VI_Release_UserPic(VCTRL_VI_FRAME_INFO_S *pstViFrameInfo);
AR_S32 VCTRL_COMM_VI_StartVi(VCTRL_VI_CONFIG_S* pstViConfig);
AR_S32 VCTRL_COMM_VI_DestroyVi(VCTRL_VI_CONFIG_S* pstViConfig);
AR_S32 VCTRL_COMM_VI_StopVi(VCTRL_VI_CONFIG_S* pstViConfig);
AR_S32 VCTRL_COMM_SYS_Init(VB_CONFIG_S* pstVbConfig);
AR_VOID VCTRL_COMM_SYS_Exit(void);
AR_S32 VCTRL_COMM_CORE_Init(PAYLOAD_TYPE_E type, VIDEO_PROCESSOR_TYPE_E processor,
    AR_U32 coreClock, AR_U32 bpuClock, AR_U32 jpuClock);
AR_S32 VCTRL_COMM_VB_Init(VB_CONFIG_S* pstVbConfig);
AR_VOID VCTRL_COMM_VB_Exit(void);

PIC_SIZE_E VCTRL_COMM_SYS_GetEnPicSize(SIZE_S* pstSize);
AR_S32 VCTRL_COMM_VI_Load_UserPic(FILE* pfd, VI_USERPIC_ATTR_S *pstUsrPic, VCTRL_VI_FRAME_INFO_S *pstViFrameInfo);
#endif//__VCTRL_COMMON__
