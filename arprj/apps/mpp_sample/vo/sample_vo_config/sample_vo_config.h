#ifndef SAMPLE_VO_CONFIG_H
#define SMPLE_VO_CONFIG_H

#include "sample_comm.h"
// #include "sample_vo.h"


typedef enum arSAMPLE_VO_CONFIG_CMD_E{
    CMD_VO_CONFIG_NONE = 0,
    CMD_VO_CONFIG_SWITCH = 1,
    CMD_VO_CONFIG_BUTT,
} SAMPLE_VO_CONFIG_CMD_E;


typedef struct arSAMPLE_VO_CONTROL_S{
    SAMPLE_VO_CONFIG_CMD_E    cmd;
    AR_BOOL bVoEnable;
    AR_S32 VoDev;
    AR_S32 VoLayer;
    AR_S32 VoChnNum;
    VO_PUB_ATTR_S stPubAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    SIZE_S stDevSize;
} SAMPLE_VO_CONTROL_S;

typedef struct arSAMPLE_VO_REPLY_S{
    char result[16];
    AR_S32 s32Ret;
} SAMPLE_VO_REPLY_S;

#endif