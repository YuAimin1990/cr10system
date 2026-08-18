
#ifndef _AR_DSP_PLATFORM_H_
#define _AR_DSP_PLATFORM_H_
#include <linux/miscdevice.h>

#define AR_DSP_CEVA_MSG_ID	0xff00ff00
#define AR_DSP_RUNTIME_MSG_ID 0xff00ffff
#define AR_DSP_MAX_ALLOWED_SIZE	(1024)
#define AR_DSP_MAX_MSG_NUM	(1024)

typedef uint32_t   AR_U32;
typedef unsigned long	AR_U64;
typedef int32_t    AR_S32;
typedef uint16_t   AR_U16;
typedef int16_t    AR_S16;
typedef uint8_t    AR_UCHAR;
typedef int8_t     AR_CHAR;
typedef uint32_t   AR_BOOL;
typedef float      AR_FLOAT;

#pragma pack (4)
typedef struct
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U64 		u64MsgBodyAddr;
	AR_U32          u32MsgLen;
}AR_DSP_TASK_IOCTL_S;

typedef struct
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U32 		u64MsgBodyAddr;
	AR_U32          u32MsgLen;
}AR_DSP_TASK_IOCTL_S_32;

typedef struct 
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U64 		u64MsgBodyAddr;
	AR_U32		u32MsgLen;
	AR_S32		s32ErrCode;
	AR_U32		u32Time;
}AR_DSP_TASK_ACK_IOCTL_S;

typedef struct 
{
	AR_CHAR 	s8Name[32];
	AR_U32 		u32Priority;
	AR_BOOL 	u32IsBlocked;
	AR_U32 		u32PID;
	AR_U32		u32CoreID;
	AR_U32 		u64MsgBodyAddr;
	AR_U32		u32MsgLen;
	AR_S32		s32ErrCode;
	AR_U32		u32Time;
}AR_DSP_TASK_ACK_IOCTL_S_32;

#pragma pack ()

#pragma pack (4)
typedef struct 
{
	AR_CHAR s8NetName[32];
	AR_U32	u32NetworkId;
	AR_U64	u64CycleTime;
} AR_DSP_RUNTIME_S;
#pragma pack ()

typedef struct 
{
	AR_U32 u32SendCnt;
	AR_DSP_TASK_IOCTL_S stSendMsg[AR_DSP_MAX_MSG_NUM];
	AR_U32 u32AckCnt;
	AR_DSP_TASK_ACK_IOCTL_S stAckMsg[AR_DSP_MAX_MSG_NUM];
	struct mutex       dump_mutex;
}AR_DSP_MSG_DUMP;

struct ar_dsp_ack_icc_msg_list{
	struct list_head list;
	AR_DSP_TASK_ACK_IOCTL_S *AckMsg;
};

struct ar_dsp_msg_queue {
	struct list_head   icc_msg_list;
	unsigned int       icc_msg_id;
	unsigned int       icc_msg_num;
	struct mutex       icc_mutex;
};

struct ar_dsp_task{
    struct list_head list;
    struct completion idle_wait;
    unsigned int dsp_pid;
};

struct ar_dsp_task_list{
    struct list_head task_list;
    unsigned int task_num;
    struct mutex task_mutex;
};

struct ar_dsp_dev_s{
	unsigned int icc_msg_id;
	struct icc_client *icc_client;
	unsigned int runtime_msg_id;
	struct icc_client *runtime_client;
	struct miscdevice misc_dev;
	struct proc_dir_entry  *proc_dir;
	struct mutex       dev_mutex;
};

#define AR_DSP_IOC_MAGIC         'X'
#define AR_DSP_IOC_SET_ADDR         _IOWR(AR_DSP_IOC_MAGIC, 0, unsigned int)
#define AR_DSP_IOC_RPC          		_IOWR(AR_DSP_IOC_MAGIC, 1, AR_DSP_TASK_IOCTL_S)
#define AR_DSP_IOC_QUERY          	_IOWR(AR_DSP_IOC_MAGIC, 2, AR_DSP_TASK_ACK_IOCTL_S)

#define AR_DSP_IOC_SET_ADDR_32      _IOWR(AR_DSP_IOC_MAGIC, 0, unsigned int)
#define AR_DSP_IOC_RPC_32          		_IOWR(AR_DSP_IOC_MAGIC, 1, AR_DSP_TASK_IOCTL_S_32)
#define AR_DSP_IOC_QUERY_32        	_IOWR(AR_DSP_IOC_MAGIC, 2, AR_DSP_TASK_ACK_IOCTL_S_32)


#endif
