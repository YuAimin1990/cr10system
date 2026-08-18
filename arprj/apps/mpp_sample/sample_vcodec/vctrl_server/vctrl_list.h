#ifndef __VCTRL_LIST__
#define __VCTRL_LIST__

#include "list.h"
#include "ar_common.h"
#include "vctrl_common.h"
#include "ar_comm_venc.h"
#include "vctrl_msg.h"

#define MAX_NAME_LEN 128
typedef enum
{
    CHN_VENC = 0,
    CHN_VDEC = 1,
    CHN_BUTT
}CHN_TYPE_E;

typedef enum
{
    STATE_IDLE = 0,
    STATE_START = 1,
    STATE_STOP = 2,
    STATE_UNKNOWN
}CHN_STATE_E;

typedef struct arVchInfo {
    AR_S32 s32Verbose;
    AR_U32 u32CoreClock;
    AR_U32 u32BpuClock;
    AR_U32 u32JpegCoreClock;
    AR_BOOL transcoding;
    AR_BOOL preLoadPicFlag;
    AR_CHAR inputFilePath[MAX_NAME_LEN];
    AR_CHAR outputFilePath[MAX_NAME_LEN];
    VENC_CHN chnId;
    VENC_CHN transcodingChnId;
    AR_U32 u32PicNum;
    AR_U32 u32ChnNum;
    AR_U32 preLoadPicNum;
    PAYLOAD_TYPE_E enPayLoad;
    PAYLOAD_TYPE_E enTranscodingPayLoad;
    AR_S32 s32SaveFile;
    CHN_TYPE_E chnType;
    AR_U32 u32Width;
    AR_U32 u32Height;
    VENC_GOP_MODE_E enGopMode;
    AR_U32 u32BitRate;
    AR_U32 u32Gop;
    AR_U32 u32FrameRate;
    AR_U32 u32SrcFrameRate;
    VCTRL_RC_E enRcMode;
    AR_U32 u32Profile;
    CHN_STATE_E u32State;
    CHN_UPDATE_E enUpdate;
    ROI_MAP roiMap;
    QP_CHANGE qpChange;
    VCTRL_VI_BIND_INFO stViInfo;
    VIDEO_PROCESSOR_TYPE_E processor;
    pthread_t ptVencStartTid;
    pthread_t ptVdecStartTid;
}VCHN_INFO;

typedef struct arVchnnel {
    struct listnode vlist;
    VENC_CHN chnId;
    CHN_STATE_E chState;
    VCHN_INFO vchnInfo;
    AR_S32 (*vcodec_start)(VCHN_INFO *pvchn);
    AR_S32 (*vcodec_stop)(VCHN_INFO *pvchn);
    AR_S32 (*vcodec_set)(VCHN_INFO *pvchn);
}VCHN_NODE;

void queue_vchnnel_list(VCHN_NODE* pInvchn);
void start_vchnnel_work();
void stop_vchnnel_work(AR_U32 u32ChnId);
void vchn_add_queue_tail(VCHN_NODE* pvchn);
VCHN_NODE* get_venchn_by_id(AR_U32 u32ChnId);
bool is_queue_empty();

#endif
