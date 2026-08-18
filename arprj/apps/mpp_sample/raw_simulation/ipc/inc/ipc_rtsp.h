#ifndef __IPC_RTSP_H__
#define __IPC_RTSP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ar_rtsp_server.h"
#include "cfg_all.h"

#define MAX_VIDEO_ENC_CHN   IPC_TOTAL_STREAM_NUM

typedef struct video_param{
    int max_ch;//max video channle number
    int enc_type[MAX_VIDEO_ENC_CHN];
    int buffer_id[MAX_VIDEO_ENC_CHN];//video stream buffer id default 0-2
    int fps[MAX_VIDEO_ENC_CHN];
    char rtsp_route[MAX_VIDEO_ENC_CHN][512];
}VIDEO_PARAM_S;
typedef struct audio_patam{
    int enable;// 0 disable; 1 input; 2- input&output
    int enc_type;//audio enc type
    int samplerate;//audio samplerate
    int samplewidth;//audio samplewidth
    int channle_num;
}AUDIO_PATAM_S;
typedef struct rtsp_param{
    VIDEO_PARAM_S video;
    AUDIO_PATAM_S audio;
    int authenticate;
}RTSP_PARAM_S;

AR_S32 IPC_NET_RTSP_Start(void);

void IPC_NET_RTSP_Stop(void);


#ifdef __cplusplus
};
#endif

#endif

