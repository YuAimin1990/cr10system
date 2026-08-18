#ifndef __AR_COMM_VB_H__
#define __AR_COMM_VB_H__

#include "hal_type.h"
#include "hal_errno.h"
#include "ar_debug.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */

#define VB_INVALID_POOLID (-1U)
#define VB_INVALID_HANDLE (-1U)


#define VB_MAX_COMM_POOLS 16
#define VB_MAX_MOD_COMM_POOLS 16


/* user ID for VB */
#define VB_MAX_USER   VB_UID_BUTT

typedef enum arVB_UID_E
{
    VB_UID_VI        = 0,
    VB_UID_VO        = 1,
    VB_UID_VGS       = 2,
    VB_UID_VENC      = 3,
    VB_UID_VDEC      = 4,
    VB_UID_H265E     = 5,
    VB_UID_H264E     = 6,
    VB_UID_JPEGE     = 7,
    VB_UID_H264D     = 8,
    VB_UID_JPEGD     = 9,
    VB_UID_VPSS      = 10,
    VB_UID_DIS       = 11,
    VB_UID_USER      = 12,
    VB_UID_PCIV      = 13,
    VB_UID_AI        = 14,
    VB_UID_AENC      = 15,
    VB_UID_RC        = 16,
    VB_UID_VFMW      = 17,
    VB_UID_GDC       = 18,
    VB_UID_AVS       = 19,
    VB_UID_DPU_RECT  = 20,
    VB_UID_DPU_MATCH = 21,
    VB_UID_MCF       = 22,
    VB_UID_BUTT      = 23,

} VB_UID_E;



/* Generall common pool use this owner id, module common pool use VB_UID as owner id */
#define POOL_OWNER_COMMON    -1

/* Private pool use this owner id */
#define POOL_OWNER_PRIVATE    -2



typedef AR_U32 VB_POOL;
typedef AR_U32 VB_BLK;

#define RESERVE_MMZ_NAME "window"

typedef enum arVB_REMAP_MODE_E
{
    VB_REMAP_MODE_NONE      = 0,        /* no remap */
    VB_REMAP_MODE_NOCACHE   = 1,        /* no cache remap */
    VB_REMAP_MODE_CACHED    = 2,        /* cache remap, if you use this mode, you should flush cache by yourself */
    VB_REMAP_MODE_BUTT
} VB_REMAP_MODE_E;


typedef struct arVB_POOL_CONFIG_S
{
    AR_U64 u64BlkSize;
    AR_U32 u32BlkCnt;
    VB_REMAP_MODE_E enRemapMode;
    AR_CHAR acMmzName[MAX_MMZ_NAME_LEN + 1];
}VB_POOL_CONFIG_S;

typedef struct arVB_CONFIG_S
{
    AR_U32 u32MaxPoolCnt;
    VB_POOL_CONFIG_S astCommPool[VB_MAX_COMM_POOLS];
} VB_CONFIG_S;


typedef struct arVB_POOL_STATUS_S
{
    AR_U32 bIsCommPool;
    AR_U32 u32BlkCnt;
    AR_U32 u32FreeBlkCnt;
}VB_POOL_STATUS_S;

#define VB_SUPPLEMENT_JPEG_MASK         0x1
#define VB_SUPPLEMENT_ISPINFO_MASK      0x2
#define VB_SUPPLEMENT_MOTION_DATA_MASK  0x4
#define VB_SUPPLEMENT_DNG_MASK          0x8

typedef struct arVB_SUPPLEMENT_CONFIG_S
{
    AR_U32 u32SupplementConfig;
}VB_SUPPLEMENT_CONFIG_S;


#define AR_ERR_VB_NULL_PTR          AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define AR_ERR_VB_NOMEM             AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define AR_ERR_VB_NOBUF             AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define AR_ERR_VB_UNEXIST           AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
#define AR_ERR_VB_ILLEGAL_PARAM     AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define AR_ERR_VB_NOTREADY          AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define AR_ERR_VB_BUSY              AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define AR_ERR_VB_NOT_PERM          AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define AR_ERR_VB_SIZE_NOT_ENOUGH   AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_SIZE_NOT_ENOUGH)

#define AR_ERR_VB_2MPOOLS AR_MPP_DEF_ERR(AR_ID_VB, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUTT + 1)

#define AR_TRACE_VB(level, fmt,...)\
do{ \
    AR_TRACE(level, AR_ID_VB,"[Func]:%s [Line]:%d [Info]:"fmt,__FUNCTION__, __LINE__,##__VA_ARGS__);\
}while(0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __AR_COMM_VB_H_ */

