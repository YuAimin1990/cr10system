#ifndef __PF_ISP_H__
#define __PF_ISP_H__

#include "pf_type.h"
#include "ar_comm_isp.h"
#include "ar_sns_ctrl.h"
#include "ar_ae_comm.h"
#include "ar_awb_comm.h"

#include "util_common.h"
#include "pf_sensor.h"

#ifdef __cplusplus
extern "C"
{
#endif


AR_S32 IPC_PF_ISP_GetIspAttrBySns(IPC_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S* pstPubAttr);
ISP_SNS_OBJ_S* IPC_PF_ISP_GetSnsObj(VI_PIPE ViPipe, IPC_SNS_TYPE_E enSnsType);
AR_S32 IPC_PF_ISP_Aelib_Callback(ISP_DEV IspDev);
AR_S32 IPC_PF_ISP_Aelib_UnCallback(ISP_DEV IspDev);
AR_S32 IPC_PF_ISP_Awblib_Callback(ISP_DEV IspDev);
AR_S32 IPC_PF_ISP_Awblib_UnCallback(ISP_DEV IspDev);
AR_S32 IPC_PF_ISP_Run(ISP_DEV IspDev);
AR_VOID IPC_PF_ISP_Stop(ISP_DEV IspDev);
AR_S32 IPC_PF_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType);
AR_S32 IPC_PF_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType);
AR_S32 IPC_PF_ISP_BindSns(ISP_DEV IspDev, IPC_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev);


#ifdef __cplusplus
}
#endif
#endif

