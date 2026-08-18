
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include <linux/seq_file.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/ar_icc.h>
#include "dsp_proc_seq.h"
#include "ar_dsp_platform.h"

struct ar_dsp_struct *shmem_virt_addr = NULL;
extern struct ar_dsp_dev_s 	ar_dsp_dev;
extern AR_DSP_MSG_DUMP	gst_msg_dump;
extern AR_DSP_RUNTIME_S	gst_network_runtime[AR_DSP_MAX_MSG_NUM];

//#define AR_DSP_DEFAULT_STATUS_ADDR 0x32400000

static int dsp_status_show(struct seq_file *s, void *v)
{
	int i;

	if(shmem_virt_addr== NULL){
		seq_printf(s,"shmem is NULL!shmem_virt_addr = %p \r\n", shmem_virt_addr);
		return 0;
	}

	seq_printf(s, "DSP_CORE_ID : %d\n"
		"DSP_STATUS : %s\n"
		"DSP_KEEP_ALIVE : %x\n",
		shmem_virt_addr->DspCoreId,
		shmem_virt_addr->DspCoreStatus,
		shmem_virt_addr->DspKeep_Alive
		);
	//		       0	 	0xf0000000	R	   1		1234	  33		     prvARMAlgRecvTask
	seq_puts(s, "\nTask_Id 	Heap 	Status Priority Malloc_Cnt Dsp_Usage Task_Name\n");	
	for (i = 0; i < shmem_virt_addr->Task_Num; i++) {
		seq_printf(s,"%d		0x%x	    %c	   %d	  %d      	    %d	%s\n",
			shmem_virt_addr->TaskInfo[i].Task_Id,
			shmem_virt_addr->TaskInfo[i].Heap,
			shmem_virt_addr->TaskInfo[i].TaskStatus,
			shmem_virt_addr->TaskInfo[i].Priority,
			shmem_virt_addr->TaskInfo[i].Malloc_Cnt,
			shmem_virt_addr->TaskInfo[i].CpuUsage,
			shmem_virt_addr->TaskInfo[i].Task_Name
			);
	}
	//		     1		        123456
	seq_puts(s, "Dsp_Irq_Id Dsp_Irq_cnt\n");
	seq_printf(s, "%d		%d\n", shmem_virt_addr->IrqInfo[0].IrqId, shmem_virt_addr->IrqInfo[0].IrqCount);	
	seq_printf(s, "%d		%d\n", shmem_virt_addr->IrqInfo[1].IrqId, shmem_virt_addr->IrqInfo[1].IrqCount);
	seq_printf(s, "%d		%d\n", shmem_virt_addr->IrqInfo[2].IrqId, shmem_virt_addr->IrqInfo[2].IrqCount);
	return 0;
}

int dsp_status_open(struct inode *inode, struct file *file)
{
        return single_open(file, dsp_status_show, inode->i_private);
}

static int dsp_ack_msg_show(struct seq_file *s, void *v)
{
	int i = 0;
	
	//		    123	     1	     	   123	   ArArmRegisterNPUCallback
	seq_puts(s, "PID    ErrorCode   RunTime  netName\r\n");
	for(i = 0; i < AR_DSP_MAX_MSG_NUM; i++)
	{
		if(strcmp(gst_msg_dump.stAckMsg[i].s8Name, "\0"))
		{
			seq_printf(s, "%d	 %d         %d       %s\n", 
				gst_msg_dump.stAckMsg[i].u32PID, 
				gst_msg_dump.stAckMsg[i].s32ErrCode, 
				gst_msg_dump.stAckMsg[i].u32Time,
				gst_msg_dump.stAckMsg[i].s8Name);
		}
	}

	return 0;
}

int dsp_ack_msg_open(struct inode *inode, struct file *file)
{
        return single_open(file, dsp_ack_msg_show, inode->i_private);
}

static int dsp_send_msg_show(struct seq_file *s, void *v)
{
	int i = 0;

	seq_puts(s, "PID  IsBlocked 	PRI 		MsgBodyAddr 		MsgLen  netName\n");
	//		     123	   1	     	 1		0x7f00000000		1234     ArArmRegisterNPUCallback            
	for(i = 0; i < AR_DSP_MAX_MSG_NUM; i++)
	{
		if(strcmp(gst_msg_dump.stSendMsg[i].s8Name, "\0"))
		{
			seq_printf(s, "%d	   %d		%d        0x%lx 	%d     %s\n", 
				gst_msg_dump.stSendMsg[i].u32PID,
				gst_msg_dump.stSendMsg[i].u32IsBlocked,
				gst_msg_dump.stSendMsg[i].u32Priority, 
				gst_msg_dump.stSendMsg[i].u64MsgBodyAddr,
				gst_msg_dump.stSendMsg[i].u32MsgLen,
				gst_msg_dump.stSendMsg[i].s8Name);
		}
	}

	return 0;
}

int dsp_send_msg_open(struct inode *inode, struct file *file)
{
        return single_open(file, dsp_send_msg_show, inode->i_private);
}

static int ar_dsp_receive_runtime_msg(void)
{
	int ret = 0;

	ret = ar_icc_client_receive(ar_dsp_dev.runtime_client, (unsigned char *)gst_network_runtime, AR_DSP_MAX_MSG_NUM * sizeof(AR_DSP_RUNTIME_S), ar_dsp_dev.runtime_msg_id);
	if(ret < 0)
	{
		printk("dsp runtime receive error, please check dsp\n");
	}

	return ret;
}


static int dsp_runtime_msg_show(struct seq_file *s, void *v)
{
	int i = 0;
	int ret = 0;
	AR_DSP_TASK_IOCTL_S * runtime_buffer = NULL;

	runtime_buffer = (AR_DSP_TASK_IOCTL_S *)kmalloc(sizeof(AR_DSP_TASK_IOCTL_S), GFP_KERNEL);
	if(!runtime_buffer)
	{
		printk("Malloc runtime_buffer Failed\n");
		return -EINVAL;
	}

	strncpy(runtime_buffer->s8Name, "ArArmCalcRuntime", 31);
	ret = ar_icc_client_send(ar_dsp_dev.icc_client, (unsigned char *)runtime_buffer, sizeof(AR_DSP_TASK_IOCTL_S), ar_dsp_dev.icc_msg_id, ICC_CORE_CEVA_0);	
	if(ret < 0){
		printk("The MSG Send To DSP Failed ! %d\n", ret);
	}
	kfree(runtime_buffer);	
	printk("after runtime_buffer send success\n");
	
	ret = ar_dsp_receive_runtime_msg();
	
	//		   "0			 0x12345f   upsample
	seq_puts(s, "NetworkId   CycleTime  netName\r\n"); 
	for(i = 0; i < AR_DSP_MAX_MSG_NUM; i++)
	{
		if(strcmp(gst_network_runtime[i].s8NetName, "\0"))
		{
			seq_printf(s, "%d      	    %ld    %s\n", 
				gst_network_runtime[i].u32NetworkId,
				gst_network_runtime[i].u64CycleTime,
				gst_network_runtime[i].s8NetName);
		}
	}

	return 0;
}


int dsp_runtime_msg_open(struct inode *inode, struct file *file)
{
        return single_open(file, dsp_runtime_msg_show, inode->i_private);
}

