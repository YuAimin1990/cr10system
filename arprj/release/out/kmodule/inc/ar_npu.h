#ifndef __AR_NPU_H__
#define __AR_NPU_H__

#ifndef MAX_INPUT_NUM
#define MAX_INPUT_NUM         32
#endif
#ifndef MAX_OUTPUT_NUM
#define MAX_OUTPUT_NUM        32
#endif
#ifndef MAX_CB_NUM
#define MAX_CB_NUM            128
#endif

typedef struct
{
	unsigned long long u64TimeTotal; //usage = u64TimeRunning / u64TimeTotal;
	unsigned long long u64TimeRunning; //time_running = sum(time_delta);
}AR_NPU_IOCTL_USAGE_S;

typedef struct
{
    unsigned int u32Block;
    unsigned int u32Debug; //1 for debug
    unsigned int u32CbufAddrPhy; //for 9311 cbuff addr
    unsigned int u32Priority;//normal/ high
    unsigned int u32SCUAddrPhy;
    unsigned int u32SCUSize;
    unsigned int u32RuntimeAddrPhy;
    unsigned int u32WeightsAddrPhy;
    unsigned int au32InputAddrPhy[MAX_INPUT_NUM];
    unsigned int au32OutputAddrPhy[MAX_OUTPUT_NUM];
    //Sram used in runtime, set sram_size to 0 if not used.
    unsigned int u32SramAddrPhy;
    unsigned int u32SramSize;
    unsigned short u16NetworkId;
    unsigned short u16FrameId;
} AR_NPU_IOCTL_CFG_S;

typedef struct
{
    unsigned int u32CurrLayer;
    unsigned int u32RestartAddrPhy;
    unsigned int au32InputAddrPhy[MAX_INPUT_NUM];
    unsigned int au32OutputAddrPhy[MAX_OUTPUT_NUM];
} AR_NPU_IOCTL_HANG_S;

typedef struct
{
    unsigned int u32CmpltLayerNum; //1 for debug
    unsigned int u32RestartAddrPhy;
    unsigned int u32IRQStatus;
    unsigned short u16NetworkId;
    unsigned short u16FrameId;
    unsigned int au32CBIDs[4]; //callback id bitmap
} AR_NPU_STATUS_S;

typedef struct
{
    unsigned int bBlock;
    AR_NPU_STATUS_S stStatus;
} AR_NPU_IOCTL_STATUS_S;

typedef struct
{
    unsigned int u32ToArm;
    unsigned int au32CBIDs[4];
} AR_NPU_IOCTL_CB_S;

typedef struct
{
    unsigned short u16NetworkId;
    unsigned int au32CBAckIDs[4];
} AR_NPU_IOCTL_CB_DONE_S;

typedef struct
{
    unsigned short u16NetworkId;
    unsigned int u32DumpEnable;
    unsigned int u32Debug;
} AR_NPU_IOCTL_DEBUG_S;

typedef struct
{
    unsigned int u32InPhyAddr;
    unsigned int u32OutPhyAddr;
} AR_NPU_IOCTL_IO_ADDR_S;

typedef struct
{
    unsigned int u32CbufPhyAddr;
} AR_NPU_IOCTL_CBUF_ADDR_S;


#define NPU_IOC_MAGIC 'D'
#define NPU_IOC_START _IOWR(NPU_IOC_MAGIC, 0, AR_NPU_IOCTL_CFG_S)
#define NPU_IOC_HANG  _IOWR(NPU_IOC_MAGIC, 1, AR_NPU_IOCTL_HANG_S)
//#define NPU_IOC_RESUME  _IOWR(NPU_IOC_MAGIC, 2, AR_NPU_IOCTL_RESUME_S)
#define NPU_IOC_QUERY _IOWR(NPU_IOC_MAGIC, 3, AR_NPU_IOCTL_STATUS_S)
#define NPU_IOC_CB_REG _IOWR(NPU_IOC_MAGIC, 4, AR_NPU_IOCTL_CB_S)
#define NPU_IOC_CB_DONE _IOWR(NPU_IOC_MAGIC, 5, AR_NPU_IOCTL_CB_DONE_S)
#define NPU_IOC_DBG_CONTINUE _IOWR(NPU_IOC_MAGIC, 6, AR_NPU_IOCTL_DEBUG_S)
#define NPU_IOC_SW_RESET   _IO(NPU_IOC_MAGIC, 7)
#define NPU_IOC_CLEAN_RESOURCE _IOWR(NPU_IOC_MAGIC, 8, unsigned short)
#define NPU_IOC_SET_SECURITY  _IOWR(NPU_IOC_MAGIC, 9, unsigned int)
#define NPU_IOC_SET_FREQUENCY  _IOWR(NPU_IOC_MAGIC, 10, unsigned int)
#define NPU_IOC_QUERY_USAGE  _IOWR(NPU_IOC_MAGIC, 11, AR_NPU_IOCTL_USAGE_S)
#define NPU_IOC_GET_IO_ADDR  _IOWR(NPU_IOC_MAGIC, 12, AR_NPU_IOCTL_IO_ADDR_S)
#define NPU_IOC_ALLOC_NET_ID   _IOWR(NPU_IOC_MAGIC, 13, unsigned short)
#define NPU_IOC_SET_NET_ID   _IOWR(NPU_IOC_MAGIC, 14, unsigned short)
#define NPU_IOC_SET_CACHE_COHERENCY _IOWR(NPU_IOC_MAGIC, 15, unsigned int)
#define NPU_IOC_GET_CBUF_ADDR  _IOWR(NPU_IOC_MAGIC, 16, AR_NPU_IOCTL_CBUF_ADDR_S)
#define NPU_IOC_GET_SOC_VERSION  _IOWR(NPU_IOC_MAGIC, 17, int)
#define NPU_IOC_SET_TIMEOUT _IOWR(NPU_IOC_MAGIC, 18, unsigned int)
#define NPU_IOC_DSP_CB_REG _IOWR(NPU_IOC_MAGIC, 19, AR_NPU_IOCTL_CB_S)


#endif

