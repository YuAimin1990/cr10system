
#ifndef _DSP_PROC_SEQ_H_
#define _DSP_PROC_SEQ_H_
#include <linux/types.h>

#define AR_DSP_STATUS_FILE_NAME		"state"
#define AR_DSP_ACK_FILE_NAME			"ack_msg"
#define AR_DSP_SEND_FILE_NAME		"send_msg"
#define AR_DSP_RUNTIME_FILE_NAME		"runtime"

#define MAX_TASK_NUM 	(10)
#define MAX_IRQ_NUM	(5)

#pragma pack (4)
struct task_info_struct
{
  	unsigned int Task_Id;
  	unsigned int Heap; 
  	char TaskStatus;
  	unsigned int Priority;
  	unsigned int Malloc_Cnt;
  	char Task_Name[32];
	unsigned int CpuUsage;
};

struct irq_info_struct
{
	unsigned int IrqId;
	unsigned int IrqCount;
};

struct ar_dsp_struct 
{
	unsigned int DspCoreId;
   	char DspCoreStatus[8]; 
	unsigned int DspKeep_Alive;
   	unsigned int DspMemInfo[2];
	unsigned int Task_Num;
   	struct task_info_struct TaskInfo[MAX_TASK_NUM];
	unsigned int Irq_Num;
   	struct irq_info_struct IrqInfo[MAX_IRQ_NUM];
};
#pragma pack ()

enum irq_info_enum
{
	Dla_Irq = 0,
	Icc_Irq,
	Dsp_Irq,
	Other_Irq
} ;

int dsp_status_open(struct inode *inode, struct file *file);
int dsp_ack_msg_open(struct inode *inode, struct file *file);
int dsp_send_msg_open(struct inode *inode, struct file *file);
int dsp_runtime_msg_open(struct inode *inode, struct file *file);
#endif
