#ifndef _H3812C1SG_H_
#define _H3812C1SG_H_


#include "ar_common.h"
#include "ar_comm_sns.h"
#include "hal_vin_type_def.h"


#define H3812C1SG_SNS_ID            3812
#define H3812C1SG_CFG_DATA_BITS     384


typedef struct {
    AR_S32 s32NucFd;
    ISP_CMOS_SENSOR_IMAGE_MODE_S stImageMode;
    STRU_NUC_CFG_T stNucCfg;
    STRU_NUC_CMD_CFG_DATA_T stCfgData;
    IR_SNS_ATTR_S stIrSnsAttr;

    AR_S32 s32LowPos;
    AR_S32 s32HighPos;
    AR_S32 s32MidPos;
    AR_U32 u32FinalRdm;

    AR_U8 *pu8OccFlag;
    AR_U8 *pu8OccLow;
    AR_U8 *pu8OccHigh;
    AR_U16 *pu16MinPixDelta;
    AR_U8 *pu8FinalOcc;
    AR_U32 u32OccDonePixs;
    AR_U32 u32OccSuccessPixs;
} H3812C1SG_PIPE_CTX_S;


#endif // #ifndef _H3812C1SG_H_