#ifndef _RTD6122C_H_
#define _RTD6122C_H_


#include "ar_common.h"
#include "ar_comm_video.h"


#define RTD6122C_SNS_ID         6122
#define RTD6122C_CFG_DATA_BITS  408


typedef enum {
    RTD6122C_CALIB_NONE = 0,
    RTD6122C_CALIB_ADC_GAIN,
    RTD6122C_CALIB_DC,
} RTD6122C_CALIB_STATE_E;


typedef struct {
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;
    STRU_NUC_CFG_T stNucCfg;
    STRU_NUC_CMD_CFG_DATA_T stCfgData;

    RTD6122C_CALIB_STATE_E enCalibState;
    AR_S32 s32AdcGainCalibFrmIdx;
    AR_S32 s32LowPos;
    AR_S32 s32HighPos;
    AR_U32 u32FinalAdcGain;
    AR_U32 u32FinalIrg;
    AR_U32 u32FinalSfbOcc;
    AR_BOOL bIrgChanged;
} RTD6122C_PIPE_CTX_S;


#endif // _RTD6122C_H_