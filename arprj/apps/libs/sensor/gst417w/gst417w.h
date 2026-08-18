#ifndef _GST417W_H_
#define _GST417W_H_


#include "ar_common.h"
#include "ar_comm_video.h"


#define GST417W_SNS_ID              417
#define GST417W_I2C_SLAVE_ADDR      (0x82 >> 1)


typedef enum {
    GST417W_CALIB_NONE = 0,
    GST417W_CALIB_RA_ADJ,
    GST417W_CALIB_HSSD,
} GST417W_CALIB_STATE_E;


typedef struct {
    unsigned int gNmiddle_first;
    unsigned int gNmin_first;
    int g17_Vtemp_V;
    int Dvtemp0;
    int gTemp_first;
} gst417w_niossysctl_t;

typedef struct {
    AR_S8 s8I2cDevId;
    AR_S32 s32I2cFd;
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;
    gst417w_niossysctl_t niossysctl;

    AR_U8 au8SnsIrParam[6];

    GST417W_CALIB_STATE_E enCalibState;
    AR_S32 s32LowPos;
    AR_S32 s32HighPos;
    AR_S32 s32MidPos;
    AR_U16 u16MinMeanDelta;
    AR_U64 u64FinalRaAdj;
    AR_U8 u8FinalHssd;
} gst417w_pipe_ctx_t;

typedef struct {
	uint8_t addr;
	uint8_t val;
} gst417w_reg_t;


#endif // _GST417W_H_