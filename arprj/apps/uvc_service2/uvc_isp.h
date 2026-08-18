#ifndef __UVC_ISP_H__
#define __UVC_ISP_H__

#include <ar_comm_isp.h>

AR_S32 UVC_ISP_GetIspAttrBySns(UVC_SNS_TYPE_E enSnsType, ISP_PUB_ATTR_S* pstPubAttr);
AR_S32 UVC_ISP_Aelib_Callback(ISP_DEV IspDev);
AR_S32 UVC_ISP_Aelib_UnCallback(ISP_DEV IspDev);
AR_S32 UVC_ISP_Awblib_Callback(ISP_DEV IspDev);
AR_S32 UVC_ISP_Awblib_UnCallback(ISP_DEV IspDev);
AR_S32 UVC_ISP_Sensor_Regiter_callback(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType);
AR_S32 UVC_ISP_Sensor_UnRegiter_callback(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType);
AR_S32 UVC_ISP_Run(ISP_DEV IspDev);
AR_VOID UVC_ISP_Stop(ISP_DEV IspDev);
AR_S32 UVC_ISP_BindSns(ISP_DEV IspDev, UVC_SNS_TYPE_E enSnsType, AR_S8 s8SnsDev);

#endif
