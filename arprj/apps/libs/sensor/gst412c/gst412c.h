#ifndef _GST412C_H_
#define _GST412C_H_


#include "ar_common.h"
#include "ar_comm_video.h"


#define GST412C_SNS_ID              412
#define GST412C_I2C_SLAVE_ADDR      (0x42)


typedef enum {
    GST412C_CALIB_NONE = 0,
    GST412C_CALIB_RA_ADJ,
    GST412C_CALIB_HSSD,
} GST412C_CALIB_STATE_E;


typedef struct {
    unsigned int gNmiddle_first;
    unsigned int gNmin_first;
    int g17_Vtemp_V;
    int Dvtemp0;
    int gTemp_first;
} gst412c_niossysctl_t;

typedef struct {
    AR_S8 s8I2cDevId;
    AR_S32 s32I2cFd;
    AR_S32 s32VinFd;
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;
    gst412c_niossysctl_t niossysctl;

    GST412C_CALIB_STATE_E enCalibState;
    AR_S32 s32RaAdjCalibFrmIdx;
    AR_S32 s32HssdCalibFrmIdx;
    AR_U16 u16MinMeanDelta;
    AR_U8 au8FinalRasel;
    AR_U8 u8FinalHssd;
} gst412c_pipe_ctx_t;

typedef struct {
	uint8_t addr;
	uint8_t val;
} gst412c_reg_t;


#endif // _GST412C_H_