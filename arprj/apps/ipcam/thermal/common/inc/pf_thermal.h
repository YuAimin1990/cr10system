#ifndef __PF_THERMAL_H__
#define __PF_THERMAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "ar_combo_dev.h"
#include "ar_comm_vb.h"
#include "ar_common.h"
//#include "cfg_vio.h"
#include "pf_sensor.h"
//#include "fifo.h"
#include "ar_comm_vpss.h"
#include "hal_sys.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define  THERMAL_TYPE_IRAY_ELF  1
#define  THERMAL_TYPE_GD_TIMO256 2
#define  THERMAL_TYPE_GD_NK1221A 3

#ifdef THERMAL_TYPE_TIMO256
#define THERMAL_TYPE THERMAL_TYPE_GD_TIMO256
#endif
#ifdef THERMAL_TYPE_ELF1
#define THERMAL_TYPE THERMAL_TYPE_IRAY_ELF
#endif

typedef struct
{
    VB_BLK vb_blk;
    AR_U64 phy_addr;
    void *p_vaddr;
    void *p_vaddr_align;
    AR_U64 phy_addr_align;
    size_t size;
}STRU_VB_BUFFER_T;

typedef struct
{
    AR_U64   u64PhyAddr;
    AR_VOID* pVirAddr;
    AR_VOID* pVirAddr_u;
    AR_VOID* pVirAddr_v;
    AR_U32   u32BufLen;
}STRU_MMZ_YUV_BUFFER_T;

typedef struct
{
    int ViPipe_isp;
    int ViChn_isp;
    int ViPipe_raw;
    int ViChn_raw;
    int RawWidth;
    int RawHeight;
    int ImgWidth;
    int ImgHeight;
    STRU_VB_BUFFER_T astIspRawBuffer[3];
    STRU_AR_FIFO_T *stream_buffer_pool;
}STRU_IR_RAW_PROCESS_PARAM_T;

typedef struct
{
    int ViPipe_isp;
    int ViChn_isp;
    int ViPipe_vis;
    int ViChn_vis;
    int VpssGrp_vis;
    int VpssChn_vis;
    int VpssGrp_inf;
    int VpssChn_inf;
}STRU_YUV_PROCESS_PARAM_T;

typedef enum
{
    SENSOR_TYPE_NORMAL,
    SENSOR_TYPE_THERMAL,
    SENSOR_DATA_TYPE_BUTT,
} IPC_SENSOR_TYPE_E;

typedef enum {
    IPC_IR_DYNAMIC_CALIB_TYPE_NONE = 0,
    IPC_IR_DYNAMIC_CALIB_TYPE_B,
    IPC_IR_DYNAMIC_CALIB_TYPE_OCC_B,
    IPC_IR_DYNAMIC_CALIB_TYPE_SNS_OCC_B,
} IPC_IR_DYNAMIC_CALIB_TYPE_E;

/*This is for AR9311/ARS31 chips*/
#define IR_OCC_BUF_ALIGN        256
#define IR_KB_BUF_ALIGN         16
#define IR_AUTO_GTM_HIST_BINS   (1 << 14)

typedef struct AR_VI_WRAPPER
{
    VI_DEV ViDev;
    VI_PIPE ViPipe;
    VI_CHN ViChn;
    IPC_SNS_TYPE_E enSnsType;
    PIXEL_FORMAT_E enInPixFmt;
    PIXEL_FORMAT_E enOutPixFmt;
    SIZE_S stInSize;
    SIZE_S stOutSize;
    RECT_S stRoi;
    AR_S32 s32BusId;
    AR_S32 s32MipiDev;
    AR_S32 s32Fps;

    AR_CHAR achPowerGPIO[8];
    AR_CHAR achResetGPIO[8];
    AR_CHAR achCommGPIO[8];    //common purpose

    AR_CHAR * strInitSnsParamCalibFname;
    AR_CHAR * strInitOccCalibFname;
    AR_CHAR * strInitKbCalibFname;
    AR_CHAR * strSnsParamCalibFname;
    AR_CHAR * strOccCalibFname;

    //tmp cached buffer for cpu to use in case, malloced by mmz.
    AR_U64 u64TmpCachedBuffPhyAddr;
    AR_VOID * pTmpCachedBuffVirtAddr;
    
    //Sensor specific handler
    AR_S32 (*pfnSnsHandler)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID * pPrivData);
    //Each vi has its own process, in which will call customer registered pfnSnsHandler
    AR_S32 (*pfnFrameProcess)(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID * pPrivData);

    AR_S32 (*pfnThreadFunc)(AR_VOID *pPrivData);
    pthread_t threadPid;
	volatile AR_BOOL bIrfnThreadStart;
    AR_S32 (*pfnThreadDeqFunc)(AR_VOID *pPrivData); //if send frame in a non-block way, we need to dequeue it explicitly
    pthread_t threadDeqPid;
	volatile AR_BOOL bIrDeThreadStart;

    struct AR_VI_WRAPPER * pstNextVi;//the frame will be sent to which vi after pfnFrameProcess.

    IR_SNS_CALIB_CFG_S stIrSnsCalibCfg;
    IR_OCC_CALIB_CFG_S stIrOccCalibCfg;
    IR_B_CALIB_CFG_S stIrBCalibCfg;
    IPC_IR_DYNAMIC_CALIB_TYPE_E enFirstIrDynamicCalibType;
    IPC_IR_DYNAMIC_CALIB_TYPE_E enIrDynamicCalibType;
    AR_U32 u32DynamicCalibIntervalMs;
    AR_VOID *(*pfnIrDynamicCalibFunc)(AR_VOID *arg);
    pthread_t irDynamicCalibThread;
    volatile AR_BOOL bIrDynamicCalibThreadStart;
} VI_WRAPPER_S;

typedef struct
{
    VI_WRAPPER_S stViSns;
    VI_WRAPPER_S stViThermal;
    VI_WRAPPER_S stViIsp0;
    VI_WRAPPER_S stViIsp1;
} VI_IR_GROUP_S;

int IPC_PF_IsThermalSensor(IPC_SNS_TYPE_E sns);
void setRawProcessParam(IPC_SNS_TYPE_E sns);
void setRawProcessParamISP(int pipe_isp, int ch_isp);
void setRawProcessParamRaw(int pipe_raw, int ch_raw);
void setYuvProcessParamVis(int pipe_vis, int ch_vis, int grp_vis, int grpch_vis);
int IPC_PF_THERMAL_Start(void);

AR_S32 IrKbCalib(VIDEO_FRAME_INFO_S *pstFrmInfo, AR_VOID * pPrivData);
AR_S32 IrOpenShutter();
AR_S32 IrCloseShutter();
AR_S32 IrLoadAndApplyAllParam(VI_IR_GROUP_S * pstViIRGrp);
AR_S32 IrRawFrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);
AR_S32 IrThermalFrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);
AR_S32 IrIsp0FrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);
AR_S32 IrIsp1FrameProcess(VIDEO_FRAME_INFO_S *pstFrameInfo, AR_VOID *pPrivData);
AR_VOID * IrFrameProcessThreadFunc(AR_VOID *arg);
AR_VOID * IrDeqThreadFunc(AR_VOID *arg);
AR_VOID *IrDynamicCalibFunc(AR_VOID *arg);
AR_VOID * IrFussionThreadFunc(AR_VOID *arg);

AR_S32 IPC_PF_CreateViIRGroup(IPC_SNS_TYPE_E enSnsType, AR_S32 s32DevStartId);
AR_S32 IPC_PF_DestroyViIRGroup();
VI_IR_GROUP_S * IPC_PF_GetViIRGroup();
AR_S32 IPC_PF_StartIrFussion();
AR_S32 IPC_PF_StopIrFussion();

#ifdef __cplusplus
}
#endif

#endif

