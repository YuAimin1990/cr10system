
#ifndef __MPI_HDMI_H__
#define __MPI_HDMI_H__

#include "ar_comm_hdmi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */


AR_S32 AR_MPI_HDMI_Init(AR_VOID);
AR_S32 AR_MPI_HDMI_DeInit(AR_VOID);

AR_S32 AR_MPI_HDMI_Open(AR_HDMI_ID_E enHdmi);
AR_S32 AR_MPI_HDMI_Close(AR_HDMI_ID_E enHdmi);
AR_S32 AR_MPI_HDMI_GetSinkCapability(AR_HDMI_ID_E enHdmi, AR_HDMI_SINK_CAPABILITY_S *pstSinkCap);
AR_S32 AR_MPI_HDMI_SetAttr(AR_HDMI_ID_E enHdmi, const AR_HDMI_ATTR_S *pstAttr);
AR_S32 AR_MPI_HDMI_GetAttr(AR_HDMI_ID_E enHdmi, AR_HDMI_ATTR_S *pstAttr);
AR_S32 AR_MPI_HDMI_Start(AR_HDMI_ID_E enHdmi);
AR_S32 AR_MPI_HDMI_Stop(AR_HDMI_ID_E enHdmi);
AR_S32 AR_MPI_HDMI_Force_GetEDID(AR_HDMI_ID_E enHdmi, AR_HDMI_EDID_S *pstEdidData);
AR_S32 AR_MPI_HDMI_RegCallbackFunc(AR_HDMI_ID_E enHdmi, const AR_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);
AR_S32 AR_MPI_HDMI_UnRegCallbackFunc(AR_HDMI_ID_E enHdmi, const AR_HDMI_CALLBACK_FUNC_S *pstCallbackFunc);

AR_S32 AR_MPI_HDMI_RegCECCallBackFunc(AR_HDMI_ID_E enHdmi, const AR_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
AR_S32 AR_MPI_HDMI_UnRegCECCallBackFunc(AR_HDMI_ID_E enHdmi, const AR_HDMI_CECCALLBACK_FUNC_S *pstCECCallback);
AR_S32 AR_MPI_HDMI_SetCECCommand(AR_HDMI_ID_E enHdmi, const AR_HDMI_CEC_CMD_S *pCECCmd);
AR_S32 AR_MPI_HDMI_CECStatus(AR_HDMI_ID_E enHdmi, AR_HDMI_CEC_STATUS_S *pStatus);
AR_S32 AR_MPI_HDMI_CEC_Enable(AR_HDMI_ID_E enHdmi);
AR_S32 AR_MPI_HDMI_CEC_Disable(AR_HDMI_ID_E enHdmi);

/* Module Parameter */
AR_S32 AR_MPI_HDMI_SetModParam(AR_HDMI_ID_E enHdmi, const AR_HDMI_MOD_PARAM_S *pstModParam);
AR_S32 AR_MPI_HDMI_GetModParam(AR_HDMI_ID_E enHdmi, AR_HDMI_MOD_PARAM_S *pstModParam);

/* Normally, these functions are not necessary */
AR_S32 AR_MPI_HDMI_SetInfoFrame(AR_HDMI_ID_E enHdmi, const AR_HDMI_INFOFRAME_S *pstInfoFrame);
AR_S32 AR_MPI_HDMI_GetInfoFrame(AR_HDMI_ID_E enHdmi, AR_HDMI_INFOFRAME_TYPE_E enInfoFrameType, AR_HDMI_INFOFRAME_S *pstInfoFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__MPI_HDMI_H__ */

