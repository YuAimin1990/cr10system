#ifndef __UTIL_HIST_H__
#define __UTIL_HIST_H__
#include "hal_type.h"
#include "osal.h"
#include "binder_ipc.h"

#define HIST_INSTANCE_NAME_LEN        32
#define HIST_PROBE_NAME_LEN           32
#define HIST_FRAME_BUFFER_MAX_NUM     1024
#define HIST_FRAME_STAT_MAX_NUM       32
#define HIST_FLUSH_THREAD_STACK_SIZE  128*1024
#define HIST_INVALID_FRAME_ID         0xFFFFFFFFFFFFFFFF

typedef enum
{
    AR_UTIL_HIST_TIMESTAMP,
    AR_UTIL_HIST_FRAMESIZE,
}AR_UTIL_HIST_TYPE_E;

typedef struct
{
    AR_U64 u64FrameSeq;
    AR_U64 u64FrameId;
    AR_U64 u64UsTimeStamp;
}AR_UTIL_HIST_FRAME_S;

typedef struct
{
    AR_CHAR u8Name[HIST_PROBE_NAME_LEN];
    AR_U32  u32ThrLow;
    AR_U32  u32ThrHight;
    AR_U16  u16Group;
}AR_UTIL_HIST_PROBE_INFO_S;

typedef struct
{
    AR_CHAR u8Name[HIST_PROBE_NAME_LEN];
    AR_U32  u32ThrLow;
    AR_U32  u32ThrHight;
    AR_U16  u16Group;
    AR_U32  u32Distance;
    AR_U32  *pau32Stat;
    AR_U64  *pau64StatSumPerGrp;
    AR_U64  u64StatTotal;
    AR_U64  u64StatTotalSum;
    AR_U64  u64EmptyPos;
    AR_U64  u64FlushPos;
    AR_U32  u32StatError;
    AR_U64  u64UsTimeStamp[HIST_FRAME_STAT_MAX_NUM];
    AR_UTIL_HIST_FRAME_S *pstFrameBuffer;
}AR_UTIL_HIST_PROBE_S;

typedef struct
{
    FILE *pCsvFileFp;
    ar_os_thread_id_t flush_thread_tid;
    ar_signal_t signal;
    AR_BOOL bEnable;

    AR_CHAR u8Name[HIST_INSTANCE_NAME_LEN];
    AR_U32  u32ProbeNum;
    AR_U64  u64FrameId[HIST_FRAME_STAT_MAX_NUM];
    AR_U64  u64FrameSeq;
    AR_U64  u64MaxRecord;
    AR_U64  u64RecordCnt;
    AR_U64  u64ThrExtreme;  /*Extreme thresh to determine abnormal frame*/
    AR_BOOL bExtreme[HIST_FRAME_STAT_MAX_NUM]; /*Make the frame as extreme. By default is True*/
    BINDER_CMD_T *stBinderCmd;
    AR_UTIL_HIST_PROBE_S *pstProbe;
    AR_UTIL_HIST_TYPE_E enType;
}AR_UTIL_HIST_INST_S;

AR_VOID ar_hist_stat_reset(AR_UTIL_HIST_INST_S *pstHistInst);
AR_UTIL_HIST_INST_S *ar_hist_create_instance(AR_CHAR *puInst8Name, AR_UTIL_HIST_PROBE_INFO_S astHistProbe[], AR_U32 u32ProbeNum,
                                                        AR_CHAR *puFilePath, AR_U64 u64MaxRecord, AR_UTIL_HIST_TYPE_E enType);
AR_VOID ar_hist_destroy_instance(AR_UTIL_HIST_INST_S *pstHistInst);
AR_S32 ar_hist_binder_cmd(AR_CHAR *cmd_str, AR_S32 len, void *data);
AR_VOID ar_hist_stat_probe(AR_UTIL_HIST_INST_S *pstHistInst, AR_CHAR *pu8ProbeName, AR_U64 u64FrameId, AR_U64 *pu64UsStart);
AR_VOID ar_hist_stat_summary(AR_UTIL_HIST_INST_S *pstHistInst, AR_U64 u64FrameId);
AR_VOID ar_hist_stat_view(AR_UTIL_HIST_INST_S *pstHistInst, AR_U64 u64FrameId);
AR_VOID ar_hist_set_extreme(AR_UTIL_HIST_INST_S *pstHistInst, AR_U64 u64Extreme);
AR_VOID ar_hist_stat_dump(AR_UTIL_HIST_INST_S *pstHistInst);
AR_VOID ar_hist_stat_enable(AR_UTIL_HIST_INST_S *pstHistInst);
AR_VOID ar_hist_stat_disable(AR_UTIL_HIST_INST_S *pstHistInst);

#endif

