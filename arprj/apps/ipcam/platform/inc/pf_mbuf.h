#ifndef __PF_MBUF_H__
#define __PF_MBUF_H__

#include "pf_type.h"
#include "media_fifo.h"

#ifdef __cplusplus
extern "C"
{
#endif

AR_S32 IPC_PF_MBUF_Init();
void IPC_PF_MBUF_UnInit();
MEDIABUF_HANDLE IPC_PF_MBUF_GetVideoWriterId(AR_U32 u32FifoId);
MEDIABUF_HANDLE IPC_PF_MBUF_GetAudioWriterId(AR_U32 u32FifoId);

#ifdef __cplusplus
}
#endif
#endif





