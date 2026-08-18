#ifndef _RTD3172C_H_
#define _RTD3172C_H_


#include "ar_common.h"
#include "ar_comm_video.h"


#define RTD3172C_SNS_ID         3172
#define RTD3172C_CFG_DATA_BITS  189
#define RTD3172C_CFG_DATA_BYTES ((RTD3172C_CFG_DATA_BITS + 7) / 8)


typedef enum {
    RTD3172C_CALIB_NONE = 0,
    RTD3172C_CALIB_ADC_GAIN,
    RTD3172C_CALIB_DC,
} RTD3172C_CALIB_STATE_E;


typedef struct {
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;
    STRU_NUC_CFG_T stNucCfg;
    STRU_NUC_CMD_CFG_DATA_T stCfgData;

    RTD3172C_CALIB_STATE_E enCalibState;
    AR_S32 s32AdcGainCalibFrmIdx;
    AR_S32 s32LowPos;
    AR_S32 s32HighPos;
    AR_U32 u32FinalAdcGain;
    AR_U32 u32FinalNtcc;
} RTD3172C_PIPE_CTX_S;


#endif // _RTD3172C_H_