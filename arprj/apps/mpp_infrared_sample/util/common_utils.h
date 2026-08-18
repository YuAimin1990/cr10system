#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#include <stdint.h>
#include "hal_type.h"
#include "osal_def.h"
#include "ar_comm_vb.h"

typedef  struct __STRU_AR_QUEUE_T STRU_AR_QUEUE_T;

struct __STRU_AR_QUEUE_T
{
 char * name;
 uint32_t queue_size;
 uint32_t valid_data_num;
 //queue data
 void **data;
 uint32_t header;
 uint32_t tail;
 //lock
 ar_lock_t lock;
 int (*queue_pop)(STRU_AR_QUEUE_T * queue, void **item);
 int (*queue_insert)(STRU_AR_QUEUE_T * queue,void *item);
 int (*get_queue_size)(STRU_AR_QUEUE_T * queue);
 int (*look_up_head)(STRU_AR_QUEUE_T * queue, void **item);
};

typedef struct
{
    int64_t begin;
    int64_t end;
    int64_t interval;
} STRU_TIME_STATISTICS_T;

STRU_TIME_STATISTICS_T gTSTmp;

int64_t getTickCount();
int64_t getTickInterval(int64_t begin);
void time_statistics_begin(STRU_TIME_STATISTICS_T *pTs);
void time_statistics_end(STRU_TIME_STATISTICS_T *pTs, int print_log, const char *msg);

void dump_to_file(const char* file_path, const void* data, int len);

int doSystem(const char *fmt, ...);

//buffer utils
typedef struct
{
    AR_U64      u64PhyAddr;
    AR_VOID     *pViraddr;
    AR_U32      u32Size;
}COMMON_MMZ_BUFFER_T;                 //cached mmz buffer

typedef struct
{
    VB_BLK      vb_blk;
    AR_U64      u64PhyAddr;
    AR_U64      u64PhyAddrAlign;        //address align
    AR_VOID     *pViraddr;
    AR_VOID     *pViraddrAlign;         //address align
    AR_U32      u32Size;
}COMMON_VB_BUFFER_T;

AR_S32 init_common_vb_buffer(COMMON_VB_BUFFER_T* vb_buff);
AR_S32 init_common_vb_buffer_with_size(COMMON_VB_BUFFER_T* vb_buff, AR_U32 size);
AR_S32 release_common_vb_buffer(COMMON_VB_BUFFER_T* vb_buff);
AR_S32 init_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer);
AR_S32 release_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer);
AR_S32 flush_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer);
AR_S32 invalid_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer);

#endif /* _COMMON_UTILS_H_ */