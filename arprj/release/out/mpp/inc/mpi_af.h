#ifndef __MPI_AF_H__
#define __MPI_AF_H__

#include "ar_comm_isp.h"
#include "ar_comm_3a.h"
#include "ar_ae_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* The callback function of sensor register to ae lib. */
AR_S32 AR_MPI_AF_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                   AE_SENSOR_REGISTER_S *pstRegister);
AR_S32 AR_MPI_AF_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAeLib, SENSOR_ID SensorId);

AR_S32 AR_MPI_ISP_SetAFAttr(VI_PIPE ViPipe, const ISP_FOCUS_ATTR_S *pstFocusAttr);
AR_S32 AR_MPI_ISP_GetAFAttr(VI_PIPE ViPipe, ISP_FOCUS_ATTR_S *pstFocusAttr);

AR_S32 AR_MPI_ISP_QueryAfInfo(VI_PIPE ViPipe, ISP_FOCUS_INFO_S *pstExpInfo);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

