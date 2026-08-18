
#ifndef __RTP_H__
#define __RTP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define MAX_FRAME_SIZE (1024 * 1024)

enum {
    RTP_PT_VIDEO_H264 = 96,
    RTP_PT_VIDEO_H265 = 98, //Temp defined by hbbai
    RTP_PT_AUDIO_PCMU = 0,
    RTP_PT_AUDIO_PCMA = 8,
};

/* NALU payload types in RTP packets */
enum {
    NALU_PT_FU_A = 28,
    NALU_PT_FU_B = 29,
};

/* NALU types */
enum {
    NALU_TYPE_SPS = 7,          /* sequence parameter set */
    NALU_TYPE_PPS = 8,          /* picture parameter set */
};

/* Frame types. */
enum {
    RTSP_I_FRAME = 1,
    RTSP_P_FRAME = 2,
    RTSP_A_FRAME = 3,
};

/* Frame head . */
typedef struct frame_head_s {
    unsigned int device_type;
    unsigned int frame_size;
    unsigned int frame_no;
	unsigned int width;
	unsigned int height;
    unsigned char res_video_reso;   /* Video resolution. not use */
    unsigned char frame_type;
    unsigned char frame_rate;
    unsigned char video_standard;
    unsigned int sec;
    unsigned long long usec;
    int64_t pts;
} frame_head_t;

void *rtsp_stream_thread(void *arg);
void *rtsp_replay_thread(void *arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __RTPSERV_H__ */
