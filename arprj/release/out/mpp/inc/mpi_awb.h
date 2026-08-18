#ifndef __MPI_AWB_H__
#define __MPI_AWB_H__

#include "ar_comm_isp.h"
#include "ar_comm_3a.h"
#include "ar_awb_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/* The callback function of sensor register to awb lib. */
AR_S32 AR_MPI_AWB_SensorRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, ISP_SNS_ATTR_INFO_S *pstSnsAttrInfo,
                                    AWB_SENSOR_REGISTER_S *pstRegister);
AR_S32 AR_MPI_AWB_SensorUnRegCallBack(VI_PIPE ViPipe, ALG_LIB_S *pstAwbLib, SENSOR_ID SensorId);

AR_S32 AR_MPI_ISP_SetWBAttr(VI_PIPE ViPipe, const ISP_WB_ATTR_S *pstWBAttr);
AR_S32 AR_MPI_ISP_GetWBAttr(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr);

AR_S32 AR_MPI_ISP_SetCCMAttr(VI_PIPE ViPipe, const ISP_COLORMATRIX_ATTR_S *pstCCMAttr);
AR_S32 AR_MPI_ISP_GetCCMAttr(VI_PIPE ViPipe, ISP_COLORMATRIX_ATTR_S *pstCCMAttr);

AR_S32 AR_MPI_ISP_QueryWBInfo(VI_PIPE ViPipe, ISP_WB_INFO_S *pstWBInfo);
AR_S32 AR_MPI_ISP_CalGainByTemp(VI_PIPE ViPipe, ISP_WB_ATTR_S *pstWBAttr, AR_U16 u16ColorTemp, AR_S16 s16Shift, AR_U16 *pu16AWBGain);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
