#ifndef _GST212W4_H_
#define _GST212W4_H_


#include "ar_common.h"
#include "ar_comm_video.h"


#define GST212W4_SNS_ID             212
#define GST212W4_I2C_SLAVE_ADDR     0x45


typedef enum {
    GST212W4_CALIB_NONE = 0,
    GST212W4_CALIB_RA_ADJ,
    GST212W4_CALIB_HSSD,
    GST212W4_CALIB_AD_STEP,
} GST212W4_CALIB_STATE_E;


typedef struct {
    AR_S8 s8I2cDevId;
    AR_S32 s32I2cFd;
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;

    AR_U8 au8SnsIrParam[7];

    GST212W4_CALIB_STATE_E enCalibState;
    AR_S32 s32LowPos;
    AR_S32 s32HighPos;
    AR_S32 s32MidPos;
    AR_U16 u16MinMeanDelta;
    AR_U64 u64FinalRaAdj;
    AR_U8 u8FinalHssd;
    AR_U8 u8FinalAdStep;
} gst212w4_pipe_ctx_t;

typedef struct {
	uint8_t addr;
	uint8_t val;
} gst212w4_reg_t;


#endif // _GST212W4_H_