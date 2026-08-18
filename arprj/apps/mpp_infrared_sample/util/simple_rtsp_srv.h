#ifndef __SIMPLE_RTSP_SERVER__
#define __SIMPLE_RTSP_SERVER__

// a simple rtsp server for mpi sdk

#ifdef __cplusplus
extern "C" {
#endif

#include "ar_comm_venc.h"

int smp_rtsp_enable_venc_chn(int index, VENC_CHN chn, const char *route);
AR_S32 smp_rtsp_start(void);
void smp_rtsp_stop(void);


#ifdef __cplusplus
};
#endif

#endif //__SIMPLE_RTSP_SERVER__

