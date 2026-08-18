#ifndef __ISPIN_VI_BUFFER_LOOP_H__
#define __ISPIN_VI_BUFFER_LOOP_H__

#include "common_utils.h"
#include "ar_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef struct
{
    AR_U32      u32DualIspinEnable;
    VI_PIPE     ViPipe_isp0;
    VI_CHN      ViChn_isp0;
    VI_PIPE     ViPipe_isp1;
    VI_CHN      ViChn_isp1;
    SIZE_S      stSize;
    AR_U32      u32RawAlign;
    AR_U32      u32YuvAlign;
}STRU_ISPIN_VI_LOOP_CONFIG_T;

AR_S32 ISPIN_VI_Buf_Loop_Start(STRU_ISPIN_VI_LOOP_CONFIG_T *psConfig);
AR_S32 ISPIN_VI_Buf_Loop_Stop();
AR_S32 ISPIN_VI_Buf_Loop_Dq_RawFrame(VIDEO_FRAME_INFO_S *pFrame);
AR_S32 ISPIN_VI_Buf_Loop_Q_RawFrame(VIDEO_FRAME_INFO_S *pFrame);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* End of #ifndef __ISPIN_VI_BUFFER_LOOP_H__ */
