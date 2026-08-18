#ifndef __AR_AWB_COMM_H__
#define __AR_AWB_COMM_H__

#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_AWB_LIB_NAME "ar_awb_lib"


/************************** sensor's interface to awb *********************/

typedef struct arAWB_SENSOR_DEFAULT_S
{
    AR_U16  u16GoldenRgain;      /* Rgain for the golden sample */
    AR_U16  u16GoldenBgain;      /* Bgain for the golden sample */
    AR_U16  u16SampleRgain;      /* Rgain for the current sample */
    AR_U16  u16SampleBgain;      /* Bgain for the current sample */
} AWB_SENSOR_DEFAULT_S;

typedef struct arAWB_SENSOR_EXP_FUNC_S
{
    AR_S32(*pfn_cmos_get_awb_default)(VI_PIPE ViPipe, AWB_SENSOR_DEFAULT_S *pstAwbSnsDft);
} AWB_SENSOR_EXP_FUNC_S;

typedef struct arAWB_SENSOR_REGISTER_S
{
    AWB_SENSOR_EXP_FUNC_S stSnsExp;
} AWB_SENSOR_REGISTER_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
