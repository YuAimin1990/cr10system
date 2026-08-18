#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util_profile.h"
#include "cfg_all.h"
#include "pf_mbuf.h"
#include "media_fifo.h"
#include "util_profile.h"

static AR_U32 s_u32MbufInit = 0;

#define IPC_MBUF_DEFAULT_SIZE   (1 * 1024 * 1024) //ipcam typical 8Mbps bitrate

static MEDIABUF_HANDLE s_hVideoWriterId[IPC_TOTAL_VENC_STREAM_NUM] = {0};
static MEDIABUF_HANDLE s_hAudioWriterId[IPC_TOTAL_VENC_STREAM_NUM] = {0};

AR_S32 IPC_PF_MBUF_Init()
{
    ProfileStart(__FUNCTION__);

    AR_S32 i = 0, s32Ret = 0;
    AR_U32 u32Chn = 0, u32Stream = 0;

    IPC_CFG_VENC_S * pstVencCfg = IPC_CFG_VENC_GetParam();

    if (s_u32MbufInit)
        return 0;

    for(i = 0; i < IPC_TOTAL_VENC_STREAM_NUM; i++)
    {
        u32Chn = i / IPC_MAX_VENC_STREAM_PER_CHN;
        u32Stream = i % IPC_MAX_VENC_STREAM_PER_CHN;

        if(pstVencCfg->vencStream[u32Chn][u32Stream].enable)
        {
            s32Ret = IPC_MID_MBUF_Init(i, IPC_MBUF_DEFAULT_SIZE);
            if(s32Ret)
            {
                PRINT_ERR("Create mbuf %d failed.\n", i);
                return -1;
            }

            s_hVideoWriterId[i] = IPC_MID_MBUF_AddWriter(i);
            if(s_hVideoWriterId[i] == NULL)
            {
                PRINT_ERR("Create mbuf writer %d error\n", i);
        		return -1;
            }
            s_hAudioWriterId[i] = IPC_MID_MBUF_AddWriter(i);
            if(s_hAudioWriterId[i] == NULL)
            {
                PRINT_ERR("Create mbuf writer %d error\n", i);
        		return -1;
            }

        }
    }

    s_u32MbufInit = 1;

    ProfileEnd(__FUNCTION__);

    return 0;
}

void IPC_PF_MBUF_UnInit()
{
	AR_S32 i = 0;
	
    if (!s_u32MbufInit)
    {
        return;
    }

	for(i = 0; i < IPC_TOTAL_VENC_STREAM_NUM; ++i)
    {
        if(s_hVideoWriterId[i])
        {
            IPC_MID_MBUF_DelWriter(s_hVideoWriterId[i]);
        }
        if(s_hAudioWriterId[i])
        {
            IPC_MID_MBUF_DelWriter(s_hAudioWriterId[i]);
        }

        IPC_MID_MBUF_UnInit(i);
    }

    s_u32MbufInit = 0;

}

MEDIABUF_HANDLE IPC_PF_MBUF_GetVideoWriterId(AR_U32 u32FifoId)
{
    return s_hVideoWriterId[u32FifoId];
}

MEDIABUF_HANDLE IPC_PF_MBUF_GetAudioWriterId(AR_U32 u32FifoId)
{
    return s_hAudioWriterId[u32FifoId];
}

