#ifndef _CFG_RECORD_H__
#define _CFG_RECORD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cfg_common.h"

typedef enum {
    MMC_REC_AVI = 0,
    MMC_REC_MOV,
    MMC_REC_TYPE_UNKNOWN,
} MMC_REC_TYPE;


typedef enum {
    AUDIO_REC_ALAW = 0,
    AUDIO_REC_ULAW,
    AUDIO_REC_PCM,
    AUDIO_REC_ADPCM,
    AUDIO_REC_TYPE_UNKNOWN,
} AUDIO_REC_TYPE;

#define IPC_CFG_REC_MODE_MANUAL    1
#define IPC_CFG_REC_MODE_SCHEDULE  2

/***********************************/
/***         record              ***/
/***********************************/
typedef struct {
    AR_S32      enable;
    //AR_S32      channel;
    AR_S32      stream_no;
    AR_S32      recordMode;
    AR_S32      recordType;
    CFG_SCHEDTIME  scheduleTime[7][4];
    AR_U32       scheduleSlice[7][3];
    AR_S32      preRecordTime;
    AR_S32      audioRecEnable;
    AR_S32      recAudioType;     /* 0 a-law; 1 u-law; 2 pcm; 3-adpcm*/
    AR_S32      recordDuration; //time to record for every file
    AR_S32      recycleRecord;

//snap config is in cfg_snap.h, no need to keep it here.
//    AR_S32      snap_ch;
//    AR_S32      snap_qulity;
} CFG_RECORD_S;

typedef struct cfg_record
{
    /* data */
    //AR_U32 u32Num; //total channel streams
    CFG_RECORD_S stRecord[IPC_MAX_PIPELINE_NUM][IPC_MAX_VENC_STREAM_PER_CHN];
} IPC_CFG_RECORD_S;

 AR_S32 IPC_CFG_RECORD_Save();
 AR_S32 IPC_CFG_RECORD_Load(CFG_RUN_MODE_E enMode);
 AR_S32 IPC_CFG_RECORD_LoadDefault();
 void IPC_CFG_RECORD_Print();
 IPC_CFG_RECORD_S * IPC_CFG_RECORD_GetParam();

#define RECORD_CFG_FILE "cfg_record.json"

#ifdef __cplusplus
}
#endif
#endif

