
#ifndef __RECMNG_REC_H__
#define __RECMNG_REC_H__

#include "pf_type.h"
#include "ar_avi_api.h"
#include "ar_mov_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RECORD_SIZE_MB           0x1
/* record mode */
#define RECORD_FIXED_SIZE         0X1
#define RECORD_FIXED_DURATION     0X2
#define RECORD_DELETE_OLD_FILES   0X4

#define RECORD_IS_FIXED_SIZE(x)          (x&0x1)
#define RECORD_IS_FIXED_DURATION(x)      (x&0x2)
#define RECORD_IS_DELETE(x)              (x&0x4)

#define RECORD_DEFAULT_ALARM_DURATION_SEC    30 //30seconds

typedef struct tagRecInitParam {
    AR_S32 bps;
    AR_S32 fps;
    AR_S32 gop;
    AR_S32 width;
    AR_S32 height;
    AR_S32 codec_type;

    AR_S32 audio_enable;
	AR_S32 a_enc_type; // 0 a-law; 1 u-law; 2- pcm
    AR_S32 a_chans;    /* Audio channels, 0 for no audio */
    AR_S32 a_rate;     /* Rate in Hz */
    AR_S32 a_bits;     /* bits per audio sample */

    AR_S32 channel; //max 8 channels
    AR_S32 stream; //max 4 streams for each channel
    AR_S32 mode;
    AR_S32 size_m;
    AR_S32 duration; // min
    AR_S32 pre_rec_sec;
    AR_S32 record_type; //avi or mov ?
    MovWriteHandle mov_w_handle;
    AviFile avi_w_handle;
} IPC_REC_PARAM_S;

//each channel's status will be set according to the cfg json file.
//ONLY one of schedule record and manual record enabled, they can not work at same time.
typedef struct
{
    pthread_mutex_t lock;
    pthread_t tidAlarm;
    pthread_t tidManualSched; //manual/sched use one thread

    AR_U32 u32Id;

    AR_U32 u32ScheduleRecStop; //will record from start time and stop time.
    AR_U32 u32AlarmRecStop;    //will record when triggered by events, and record 30s each time.
    AR_U32 u32ManualRecStop;   //will record automatically and continuously, record 5min each time.

    AR_U32 u32AlarmRecTrigger; //trigger a record, it'll stop after recording TIME_SLICE period of time.

    AR_U32 u32AlarmThreadCreated;
    AR_U32 u32ManualSchedThreadCreated; //they are the same thread.

    AR_U32 u32AlarmThreadExit; //to exit the loop when app exits.
    AR_U32 u32ManualSchedThreadExit;
} IPC_CHANNEL_REC_CTRL_S;

typedef enum
{
	ENC_TYPE_H264 , //H264
	ENC_TYPE_H265 , //H265
	ENC_TYPE_MPEG,      //< MPEG4
	ENC_TYPE_MJPEG,    //< Motion JPEG
	ENC_TYPE_YUV422,
	ENC_TYPE_COUNT     //< do not use
}eEncType;

typedef enum emRECORD_TYPE_E
{
    RECORD_TYPE_SCHED   = 0x000001,  //定时录像 by slice
    RECORD_TYPE_MOTION  = 0x000002,  //移到侦测录像
    RECORD_TYPE_ALARM   = 0x000004,  //报警录像
    RECORD_TYPE_CMD     = 0x000008,  //命令录像
    RECORD_TYPE_MANU    = 0x000010,  //手工录像
    RECORD_TYPE_SCHED_2 = 0x000011,  //定时录像 by period
    RECORD_TYPE_ALL     = 0xff,      //全部录像
} RECORD_TYPE_E, E_RECORD_TYPE;

//Export APIs
void IPC_MID_RECMNG_StartRec(AR_U32 u32Chn, AR_U32 u32Stream);
void IPC_MID_RECMNG_StopRec(AR_U32 u32Chn, AR_U32 u32Stream);
void IPC_MID_RECMNG_StartRecAll();
void IPC_MID_RECMNG_StopRecAll();
void IPC_MID_RECMNG_VedioRecord(AR_U32 u32ChannelId, AR_U32 u32Stream, AR_S32 s32Mode, AR_S32 s32Format, AR_S32 s32Status);


void IPC_MID_RECMNG_Init();
void IPC_MID_RECMNG_UnInit();

#ifdef __cplusplus
}
#endif

#endif /* __RECMNG_REC_H__ */

