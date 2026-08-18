#include <asm/io.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/uaccess.h>
#include <linux/pagemap.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <asm/memory.h>
#include <linux/of.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/printk.h>
#include <linux/ar_icc.h>
#include <linux/mutex.h>

#include "dsp_proc_seq.h"
#include "ar_dsp_platform.h"

//#define AR_DSP_FPGA_TEST

struct ar_dsp_dev_s 		ar_dsp_dev;
struct task_struct*	g_icc_receive_thread;
struct ar_dsp_msg_queue 	g_dsp_rcv_msg;
struct ar_dsp_task_list     g_dsp_task;
AR_DSP_MSG_DUMP	gst_msg_dump;
AR_DSP_RUNTIME_S	gst_network_runtime[AR_DSP_MAX_MSG_NUM];
extern struct ar_dsp_struct *shmem_virt_addr;

static struct file_operations status_ops = {
	.owner 	= THIS_MODULE,
	.open 	= dsp_status_open,
	.read 	= seq_read,
	.release 	= single_release,
};

static struct file_operations ack_ops = {
	.owner 	= THIS_MODULE,
	.open 	= dsp_ack_msg_open,
	.read 	= seq_read,
	.release 	= single_release,
};

static struct file_operations send_ops = {
	.owner 	= THIS_MODULE,
	.open 	= dsp_send_msg_open,
	.read 	= seq_read,
	.release 	= single_release,
};

static struct file_operations runtime_ops = {
	.owner 	= THIS_MODULE,
	.open 	= dsp_runtime_msg_open,
	.read 	= seq_read,
	.release 	= single_release,
};

static int dsp_proc_create(void)
{
	int ret = 0;
	struct proc_dir_entry * stStatusEntry;
	struct proc_dir_entry * stAckEntry;
	struct proc_dir_entry * stSendEntry;
	struct proc_dir_entry * stRuntimeEntry;

	ar_dsp_dev.proc_dir = proc_mkdir("ardsp", NULL);
	if(!ar_dsp_dev.proc_dir)
	{
		printk("Create DSP Proc Dir Failed !\n");
		return -ENOMEM;
	}

	stStatusEntry = proc_create(AR_DSP_STATUS_FILE_NAME, 0, ar_dsp_dev.proc_dir, &status_ops);
	if(!stStatusEntry){
		printk("Create stStatusEntry Failed !\n");
		ret = -ENOMEM;
	}

	stAckEntry = proc_create(AR_DSP_ACK_FILE_NAME, 0, ar_dsp_dev.proc_dir, &ack_ops);
	if(!stAckEntry){
		printk("Create stAckEntry Failed !\n");
		ret = -ENOMEM;
	}

	stSendEntry = proc_create(AR_DSP_SEND_FILE_NAME, 0, ar_dsp_dev.proc_dir, &send_ops);
	if(!stSendEntry){
		printk("Create stSendEntry Failed !\n");
		ret = -ENOMEM;
	}

	stRuntimeEntry = proc_create(AR_DSP_RUNTIME_FILE_NAME, 0, ar_dsp_dev.proc_dir, &runtime_ops);
	if(!stRuntimeEntry){
		printk("Create stRuntimeEntry Failed !\n");
		ret = -ENOMEM;
	}
	return ret;
}

static int dsp_proc_destory(void)
{
	if(ar_dsp_dev.proc_dir)
	{
		proc_remove(ar_dsp_dev.proc_dir);
	}
	return 0;
}

static int ar_dsp_dev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ar_dsp_dev_close(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t ar_dsp_dev_read(struct file *file, char __user *user, size_t len, loff_t *loff)
{
	return 0;
}

static ssize_t ar_dsp_dev_write(struct file *file, const char __user *user, size_t len, loff_t *loff)
{
	return 0;
}

static int ar_dsp_task_list_add(unsigned int task_pid)
{
    struct ar_dsp_task * dsp_task;
    dsp_task = (struct ar_dsp_task *)kmalloc(sizeof(struct ar_dsp_task), GFP_KERNEL);
    if(!dsp_task){
        printk("Malloc dsp_task List Failed!\n");
        return -ENOMEM;
    }
    memset(dsp_task, 0, sizeof(struct ar_dsp_task));
    init_completion(&dsp_task->idle_wait);
    dsp_task->dsp_pid = task_pid;

    mutex_lock(&g_dsp_task.task_mutex);
    g_dsp_task.task_num ++;
    list_add_tail(&dsp_task->list,&g_dsp_task.task_list);
    mutex_unlock(&g_dsp_task.task_mutex);

    return 0;
}

static int ar_dsp_task_list_delete(unsigned int task_pid)
{
    struct ar_dsp_task *dsp_task, *tmp;
	
	mutex_lock(&g_dsp_task.task_mutex);
    list_for_each_entry_safe(dsp_task, tmp, &g_dsp_task.task_list, list){
        if(dsp_task){
            if(dsp_task->dsp_pid == task_pid){
                list_del(&dsp_task->list);
                g_dsp_task.task_num --;
                kfree(dsp_task);
                break;
            }
        }
    }
	mutex_unlock(&g_dsp_task.task_mutex);

    return 0;
}

static int ar_dsp_task_list_find(unsigned int task_pid)
{
    struct ar_dsp_task *dsp_task, *tmp;

	mutex_lock(&g_dsp_task.task_mutex);
    list_for_each_entry_safe(dsp_task, tmp, &g_dsp_task.task_list, list){
        if(dsp_task){
            if(dsp_task->dsp_pid == task_pid){
                complete(&dsp_task->idle_wait);
                break;
            }
        }
    }
	mutex_unlock(&g_dsp_task.task_mutex);

    return 0;
}


static int ar_dsp_receive_msg(struct ar_dsp_msg_queue RcvIccMsgQ, AR_DSP_TASK_ACK_IOCTL_S *RcvAckMsg)
{
	struct ar_dsp_ack_icc_msg_list *ack_msg;

	if(RcvIccMsgQ.icc_msg_num > AR_DSP_MAX_MSG_NUM){
		printk("Recv MSG From DSP Store In Queue Up To The Max Allowed !\n");
		return -EINVAL;
	}
	ack_msg = (struct ar_dsp_ack_icc_msg_list *)kmalloc(sizeof(struct ar_dsp_ack_icc_msg_list), GFP_KERNEL);
	if(!ack_msg){
		printk("Malloc ack_msg List Failed!\n");
		return -ENOMEM;
	}
	ack_msg->AckMsg = (AR_DSP_TASK_ACK_IOCTL_S *)kmalloc(sizeof(AR_DSP_TASK_ACK_IOCTL_S), GFP_KERNEL);
	if(!(ack_msg->AckMsg)){
		printk("Malloc AckMsg in List Failed !\n");
		return -ENOMEM;
	}

	memcpy(ack_msg->AckMsg, RcvAckMsg, sizeof(AR_DSP_TASK_ACK_IOCTL_S));
	mutex_lock(&g_dsp_rcv_msg.icc_mutex);
	g_dsp_rcv_msg.icc_msg_id = ar_dsp_dev.icc_msg_id;
	g_dsp_rcv_msg.icc_msg_num++;
	list_add_tail(&ack_msg->list,&g_dsp_rcv_msg.icc_msg_list);
	mutex_unlock(&g_dsp_rcv_msg.icc_mutex);

	//printk("Recv DSP MSG And Add To List Success!\n");
	//add ack msg info for dump debug
	if(gst_msg_dump.u32AckCnt == AR_DSP_MAX_MSG_NUM)
	{
		gst_msg_dump.u32AckCnt = 0;
		//printk("ACK MSG DUMP Cnt Up To The Max, Reset Zero !\n");
	}
	memcpy(&gst_msg_dump.stAckMsg[gst_msg_dump.u32AckCnt], RcvAckMsg, sizeof(AR_DSP_TASK_ACK_IOCTL_S));
	gst_msg_dump.u32AckCnt++;

	return 0;
}

static int ar_dsp_receive_thread(void * args)
{
	int ret;
	AR_DSP_TASK_ACK_IOCTL_S 	stRcvAckMsg;

	while(!kthread_should_stop()){
		ret = ar_icc_client_receive(ar_dsp_dev.icc_client, (unsigned char *)&stRcvAckMsg, sizeof(AR_DSP_TASK_ACK_IOCTL_S), ar_dsp_dev.icc_msg_id);
		if(ret >= 0){

			ret = ar_dsp_receive_msg(g_dsp_rcv_msg, &stRcvAckMsg);
			if(ret < 0){
				printk("Recv MSG From DSP Add To The List Error!!\n");
			}

                     ar_dsp_task_list_find(stRcvAckMsg.u32PID);
		}
	}
	printk("DSP Recv Thread Stop!\n");
	return 0;
}

static int ar_dsp_receive_msg_cmp(AR_DSP_TASK_ACK_IOCTL_S *src,  AR_DSP_TASK_IOCTL_S dst)
{
        if((src->u32CoreID == dst.u32CoreID) && (src->u32IsBlocked == dst.u32IsBlocked)
			&& (src->u32Priority == dst.u32Priority) && (src->u64MsgBodyAddr == dst.u64MsgBodyAddr)
				&& (src->u32MsgLen == dst.u32MsgLen) && (src->u32PID == dst.u32PID) && (!strcmp(src->s8Name,dst.s8Name))){
		return 1;
        }
        return 0;
}

static int ar_dsp_receive_msg_query_cmp(AR_DSP_TASK_ACK_IOCTL_S *src,  AR_DSP_TASK_ACK_IOCTL_S *dst)
{
        if((src->u32CoreID == dst->u32CoreID) && (src->u32IsBlocked == dst->u32IsBlocked)
			&& (src->u32Priority == dst->u32Priority) && (src->u64MsgBodyAddr == dst->u64MsgBodyAddr)
				&& (src->u32MsgLen == dst->u32MsgLen) && (src->u32PID == dst->u32PID) && (!strcmp(src->s8Name,dst->s8Name))){
		return 1;
        }
        return 0;
}


static int ar_dsp_receive_block_check(AR_DSP_TASK_IOCTL_S pstTask)
{
    struct ar_dsp_ack_icc_msg_list *ack_msg,*tmp;
    struct ar_dsp_task *dsp_task, *task_tmp;
    int flag = 0, ret = 0;

	mutex_lock(&g_dsp_task.task_mutex);
    list_for_each_entry_safe(dsp_task, task_tmp, &g_dsp_task.task_list, list){
        if(dsp_task){
            if(dsp_task->dsp_pid == pstTask.u32PID){
                flag = 1;
                break;
            }
        }
    }
	mutex_unlock(&g_dsp_task.task_mutex);

    if(!flag)
    {
        printk(KERN_ERR"DSP not register task list!\n");
        return -EINVAL;
    }

    ret = wait_for_completion_timeout(&dsp_task->idle_wait, HZ);
    if(ret <= 0)
    {
        printk(KERN_ERR"Wait for DSP receive task timeout.\n");
        ar_dsp_task_list_delete(pstTask.u32PID);
        return -ETIME;
    }

	mutex_lock(&g_dsp_rcv_msg.icc_mutex);
    list_for_each_entry_safe(ack_msg, tmp, &g_dsp_rcv_msg.icc_msg_list,list){
    	if(ack_msg){
    		if(ar_dsp_receive_msg_cmp(ack_msg->AckMsg, pstTask)){
    			list_del(&ack_msg->list);
    			g_dsp_rcv_msg.icc_msg_num--;
    			if(ack_msg->AckMsg)
    				kfree(ack_msg->AckMsg);
    			kfree(ack_msg);
                    break;
    		}
    	}
    }
	mutex_unlock(&g_dsp_rcv_msg.icc_mutex);


    ar_dsp_task_list_delete(pstTask.u32PID);
    return 0;
}

static int ar_dsp_receive_query_check(AR_DSP_TASK_ACK_IOCTL_S *pstTask)
{
	struct ar_dsp_ack_icc_msg_list *ack_msg,*tmp;

	if(list_empty(&g_dsp_rcv_msg.icc_msg_list)){
		printk("Recv Ack MSG List is Empty!\n");
		return -EINVAL;
	}

	mutex_lock(&g_dsp_rcv_msg.icc_mutex);
	list_for_each_entry_safe(ack_msg, tmp, &(g_dsp_rcv_msg.icc_msg_list),list){
		if(ack_msg){
			if(ar_dsp_receive_msg_query_cmp(ack_msg->AckMsg, pstTask)){
                           pstTask->s32ErrCode = ack_msg->AckMsg->s32ErrCode;
                           pstTask->u32Time = ack_msg->AckMsg->u32Time;
			    
				list_del(&ack_msg->list);
				g_dsp_rcv_msg.icc_msg_num--;
				if(ack_msg->AckMsg)
					kfree(ack_msg->AckMsg);
				kfree(ack_msg);
				printk("Recv DSP Ack OK!\n");
				mutex_unlock(&g_dsp_rcv_msg.icc_mutex);
				return 0;
			}
		}
	}
	mutex_unlock(&g_dsp_rcv_msg.icc_mutex);
	printk("No Query MSG Recv In The List !\n");
	return -EINVAL;
}

static long ar_dsp_dev_ioctl(struct file *file,unsigned int cmd ,unsigned long args)
{
	int ret = 0;
	char *icc_buffer = NULL;
	unsigned long proc_addr_phy = 0;
	unsigned int send_size = 0;
	unsigned int cmd_size = 0;
	AR_DSP_TASK_IOCTL_S pstTask;
       AR_DSP_TASK_ACK_IOCTL_S pstAckTask;

	cmd_size = _IOC_SIZE(cmd);

	mutex_lock(&ar_dsp_dev.dev_mutex);
	switch(cmd){
		case AR_DSP_IOC_RPC:
			if(cmd_size != sizeof(AR_DSP_TASK_IOCTL_S))
			{
				printk("CMD Size Error in %d\n",cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EINVAL;
			}
			ret = copy_from_user(&pstTask, (char __user *)args, cmd_size);
			if(ret){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EFAULT;
			}

			send_size = cmd_size + pstTask.u32MsgLen;
			if(send_size > AR_DSP_MAX_ALLOWED_SIZE){
				printk("THE ARGs Size is Larger Than Allowed\n");
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EINVAL;
			}
			icc_buffer = kmalloc(send_size, GFP_KERNEL);
			if(!icc_buffer)
			{
				printk("IccBuff kmalloc failed!\n");
				mutex_unlock(&ar_dsp_dev.dev_mutex);
                          	return -EINVAL;
			}
			memcpy(icc_buffer, &pstTask, cmd_size);
			ret = copy_from_user(icc_buffer + cmd_size, (char __user *)pstTask.u64MsgBodyAddr, pstTask.u32MsgLen);
			if(ret)
			{
				printk("BodyMsg Copy From User ERROR\n");
				kfree(icc_buffer);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EFAULT;
			}

                     ar_dsp_task_list_add(pstTask.u32PID);
					
			ret = ar_icc_client_send(ar_dsp_dev.icc_client, icc_buffer, send_size, ar_dsp_dev.icc_msg_id, ICC_CORE_CEVA_0);//pstTask.u32CoreID);
			kfree(icc_buffer);
			if(ret < 0){
                printk("The MSG Send To DSP Failed ! %d\n", ret);
                ar_dsp_task_list_delete(pstTask.u32PID);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
                return -EINVAL;
			}

			//add send msg info for dump debug
			mutex_lock(&gst_msg_dump.dump_mutex);
			if(gst_msg_dump.u32SendCnt == AR_DSP_MAX_MSG_NUM)
			{
				gst_msg_dump.u32SendCnt = 0;
				//printk("SEND MSG DUMP Cnt Up To The Max, Reset Zero !\n");
			}
			memcpy(&gst_msg_dump.stSendMsg[gst_msg_dump.u32SendCnt], &pstTask, sizeof(AR_DSP_TASK_IOCTL_S));
			gst_msg_dump.u32SendCnt++;
			mutex_unlock(&gst_msg_dump.dump_mutex);

			if(pstTask.u32IsBlocked)
			{
				ret = ar_dsp_receive_block_check(pstTask);
			}
			else
			{
				ar_dsp_task_list_delete(pstTask.u32PID);
			}
			break;
		case AR_DSP_IOC_QUERY:
			if(cmd_size != sizeof(AR_DSP_TASK_ACK_IOCTL_S))
			{
				printk("CMD Size Error in %d\n",cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EINVAL;
			}
			ret = copy_from_user(&pstAckTask, (char __user *)args, cmd_size);
			if(ret){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EFAULT;
			}
			ret = ar_dsp_receive_query_check(&pstAckTask);
            if(!ret){
                ret = copy_to_user((char __user *)args, &pstAckTask, cmd_size);
                if(ret)
                {
                    printk("ARGS Copy To User Failed in %d\n", cmd);
					mutex_unlock(&ar_dsp_dev.dev_mutex);
                    return -EFAULT;
                }
            }
			break;
		case AR_DSP_IOC_SET_ADDR:
			if(cmd_size != sizeof(unsigned int))
			{
				printk("CMD Size Error in %d\n",cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EINVAL;
			}
			ret = copy_from_user(&proc_addr_phy, (char __user *)args, cmd_size);
			if(ret){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				mutex_unlock(&ar_dsp_dev.dev_mutex);
				return -EFAULT;
			}
			shmem_virt_addr =(struct ar_dsp_struct *) phys_to_virt(proc_addr_phy);
			printk("proc_addr_phy = 0x%lx, shmem_virt_addr = %p\n", proc_addr_phy, shmem_virt_addr);
			break;
		default:
			printk("default!!!");
			break;
	}
	mutex_unlock(&ar_dsp_dev.dev_mutex);
	return ret;
}

#if 0
void ar_dsp_task_ioctl_convert(AR_DSP_TASK_IOCTL_S * dst, AR_DSP_TASK_IOCTL_S_32 * src)
{
	memcpy(dst, src, sizeof(AR_DSP_TASK_IOCTL_S_32));
	dst->u64MsgBodyAddr = src->u64MsgBodyAddr;
	dst->u32MsgLen = dst->u32MsgLen;
}

void ar_dsp_task_ack_ioctl_convert(AR_DSP_TASK_ACK_IOCTL_S * dst, AR_DSP_TASK_ACK_IOCTL_S_32 * src)
{
	memcpy(dst, src, sizeof(AR_DSP_TASK_ACK_IOCTL_S_32));
	dst->u64MsgBodyAddr = src->u64MsgBodyAddr;
	dst->u32MsgLen = dst->u32MsgLen;
	dst->s32ErrCode = dst->s32ErrCode;
	dst->u32Time = dst->u32Time;
}

void ar_dsp_task_ack_ioctl_convert_r(AR_DSP_TASK_ACK_IOCTL_S_32 * dst, AR_DSP_TASK_ACK_IOCTL_S * src)
{
	memcpy(dst, src, sizeof(AR_DSP_TASK_ACK_IOCTL_S_32));
	dst->u64MsgBodyAddr = (AR_U32)src->u64MsgBodyAddr;
	dst->u32MsgLen = dst->u32MsgLen;
	dst->s32ErrCode = dst->s32ErrCode;
	dst->u32Time = dst->u32Time;
}

static long ar_dsp_dev_ioctl_compat(struct file *file,unsigned int cmd ,unsigned long args)
{
	int ret = 0;
	char *icc_buffer = NULL;
	unsigned long proc_addr_phy = 0;
	unsigned int send_size = 0;
	unsigned int cmd_size = 0;
	AR_DSP_TASK_IOCTL_S_32 pstTask_32;
	AR_DSP_TASK_ACK_IOCTL_S_32 pstAckTask_32;
	AR_DSP_TASK_IOCTL_S pstTask;
	AR_DSP_TASK_ACK_IOCTL_S pstAckTask;

	cmd_size = _IOC_SIZE(cmd);

	switch(cmd){
		
		case AR_DSP_IOC_RPC_32:
			if(cmd_size != sizeof(AR_DSP_TASK_IOCTL_S_32))
			{
				printk("CMD Size Error in %d\n",cmd);
				return -EINVAL;
			}
			ret = copy_from_user(&pstTask_32, (char __user *)args, cmd_size);
			if(ret < 0){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				return -EINVAL;
			}

			ar_dsp_task_ioctl_convert(&pstTask, &pstTask_32);
			cmd_size = sizeof(AR_DSP_TASK_IOCTL_S);

			send_size = cmd_size + pstTask.u32MsgLen;
			if(send_size > AR_DSP_MAX_ALLOWED_SIZE){
				printk("THE ARGs Size is Larger Than Allowed\n");
				return -EINVAL;
			}
			icc_buffer = kmalloc(send_size, GFP_KERNEL);
			if(!icc_buffer)
			{
				printk("IccBuff kmalloc failed!\n");
                          	return -EINVAL;
			}
			memcpy(icc_buffer, &pstTask, cmd_size);
			ret = copy_from_user(icc_buffer + cmd_size, (char __user *)pstTask.u64MsgBodyAddr, pstTask.u32MsgLen);
			if(ret < 0)
			{
				printk("BodyMsg Copy From User ERROR\n");
				kfree(icc_buffer);
				return -EINVAL;
			}

			ar_dsp_task_list_add(pstTask.u32PID);
			ret = ar_icc_client_send(ar_dsp_dev.icc_client, icc_buffer, send_size, ar_dsp_dev.icc_msg_id, ICC_CORE_CEVA_0);//pstTask.u32CoreID);
			kfree(icc_buffer);
			if(ret < 0){
				printk("The MSG Send To DSP Failed ! %d\n", ret);
                            ar_dsp_task_list_delete(pstTask.u32PID);
				return -EINVAL;
			}

			//add send msg info for dump debug
			if(gst_msg_dump.u32SendCnt == AR_DSP_MAX_MSG_NUM)
			{
				gst_msg_dump.u32SendCnt = 0;
				//printk("SEND MSG DUMP Cnt Up To The Max, Reset Zero !\n");
			}
			memcpy(&gst_msg_dump.stSendMsg[gst_msg_dump.u32SendCnt], &pstTask, sizeof(AR_DSP_TASK_IOCTL_S));
			gst_msg_dump.u32SendCnt++;

			if(pstTask.u32IsBlocked)
			{
				ret = ar_dsp_receive_block_check(pstTask);
			}
			else
			{
				ar_dsp_task_list_delete(pstTask.u32PID);
			}
			break;
		case AR_DSP_IOC_QUERY_32:
			if(cmd_size != sizeof(AR_DSP_TASK_ACK_IOCTL_S_32))
			{
				printk("CMD Size Error in %d\n",cmd);
				return -EINVAL;
			}
			ret = copy_from_user(&pstAckTask_32, (char __user *)args, cmd_size);
			if(ret < 0){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				return -EINVAL;
			}

			ar_dsp_task_ack_ioctl_convert(&pstAckTask, &pstAckTask_32);
			cmd_size = sizeof(AR_DSP_TASK_ACK_IOCTL_S);
			
			ret = ar_dsp_receive_query_check(&pstAckTask);
			if(!ret){
				ar_dsp_task_ack_ioctl_convert_r(&pstAckTask_32, &pstAckTask);
				cmd_size = sizeof(AR_DSP_TASK_ACK_IOCTL_S_32);	
				copy_to_user((char __user *)args, &pstAckTask_32, cmd_size);
			}
			break;
		case AR_DSP_IOC_SET_ADDR_32:
			if(cmd_size != sizeof(unsigned int))
			{
				printk("CMD Size Error in %d\n",cmd);
				return -EINVAL;
			}
			ret = copy_from_user(&proc_addr_phy, (char __user *)args, cmd_size);
			if(ret < 0){
				printk("ARGS Copy Form User Failed in %d\n", cmd);
				return -EINVAL;
			}
			shmem_virt_addr =(struct ar_dsp_struct *) phys_to_virt(proc_addr_phy);
			printk("proc_addr_phy = 0x%lx, shmem_virt_addr = %p\n", proc_addr_phy, shmem_virt_addr);
			break;
		default:
			printk("default!\n");	

			break;
	}
	return ret;
}
#endif

static struct file_operations ar_dsp_fops={
	.owner		= THIS_MODULE,
	.open		= ar_dsp_dev_open,
	.release		= ar_dsp_dev_close,
	.read		= ar_dsp_dev_read,
	.write		= ar_dsp_dev_write,
	.unlocked_ioctl = ar_dsp_dev_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_dsp_dev_ioctl,
#endif
};

static int ar_dsp_probe(struct platform_device *pdev)
{
	int ret = 0;
  	struct resource *res = NULL;

	#ifdef AR_DSP_FPGA_TEST
	void * __iomem cci_base = NULL;
	void * __iomem sec_base = NULL;
	unsigned int val = 0;
	#endif

    #ifdef PROXIMA
        printk("9311 do not need dsp!\n");
        return 0;
    #endif

	if(!pdev)
	{
		printk("pdev is NULL!\n");
		return -EINVAL;
	}
	memset(&ar_dsp_dev, 0, sizeof(struct ar_dsp_dev_s));
	memset(&gst_msg_dump, 0, sizeof(AR_DSP_MSG_DUMP));
	memset(gst_network_runtime, 0, sizeof(gst_network_runtime));
	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!res)
	{
		printk("get platform resource failed!\n");
		return -EINVAL;
	}

	ar_dsp_dev.misc_dev.minor = MISC_DYNAMIC_MINOR;
	ar_dsp_dev.misc_dev.name = "dsp";
	ar_dsp_dev.misc_dev.fops = &ar_dsp_fops;
	ret = misc_register(&ar_dsp_dev.misc_dev);
	if(ret < 0)
	{
		printk("misc register fail\n");
		return -EINVAL;
	}

	memset(&g_dsp_rcv_msg, 0, sizeof(struct ar_dsp_msg_queue));
	INIT_LIST_HEAD(&g_dsp_rcv_msg.icc_msg_list);
	mutex_init(&g_dsp_rcv_msg.icc_mutex);

       memset(&g_dsp_task, 0, sizeof(struct ar_dsp_task_list));
	INIT_LIST_HEAD(&g_dsp_task.task_list);
	mutex_init(&g_dsp_task.task_mutex);

	mutex_init(&gst_msg_dump.dump_mutex);

	mutex_init(&ar_dsp_dev.dev_mutex);

	ar_dsp_dev.icc_client = ar_icc_client_create();
	ar_dsp_dev.icc_msg_id = AR_DSP_CEVA_MSG_ID;
	ret = ar_icc_client_register_msgid(ar_dsp_dev.icc_client, ar_dsp_dev.icc_msg_id);
	if(ret != 0)
	{
    	       printk("Register AR_DSP_CEVA_MSG_ID To ICC Failed!\n");
		return -EINVAL;
       }

	ar_dsp_dev.runtime_client = ar_icc_client_create();
	ar_dsp_dev.runtime_msg_id = AR_DSP_RUNTIME_MSG_ID;
	ret = ar_icc_client_register_msgid(ar_dsp_dev.runtime_client, ar_dsp_dev.runtime_msg_id);
	if(ret != 0)
	{
        	printk("Register AR_DSP_RUNTIME_MSG_ID To ICC Failed!\n");
		return -EINVAL;
    	}

	dsp_proc_create();

	//For FPGA test only!!!
	#ifdef AR_DSP_FPGA_TEST
	cci_base = ioremap_nocache(0x0D090000, 0x10000);
	sec_base =ioremap_nocache(0x01050000, 0x10000);
	val = readl(cci_base + 0x1000); //S0 Snoop Ctrl Reg
	writel(val | 0x1, cci_base + 0x1000);
	val = readl(cci_base + 0x5000); //S4 Snoop Ctrl Reg
	writel(val | 0x1, cci_base + 0x5000);

	writel(0x03ffc3, sec_base + 0x74); //Secure Ctrl Reg [5:2] for ceva arprot/awprot
	#endif

	 g_icc_receive_thread = kthread_run(ar_dsp_receive_thread, &ar_dsp_dev, "dsp_icc_receive_thread");

	return 0;
}

static int ar_dsp_remove(struct platform_device *pdev)
{
    #ifdef PROXIMA
        printk("9311 do not need dsp!\n");
        return 0;
    #endif

	if(g_icc_receive_thread){
		kthread_stop(g_icc_receive_thread);
	}
	if(ar_dsp_dev.proc_dir){
		dsp_proc_destory();
	}
	ar_icc_client_unregister_msgid(ar_dsp_dev.icc_client, ar_dsp_dev.icc_msg_id);
	ar_icc_client_destroy(ar_dsp_dev.icc_client);

	ar_icc_client_unregister_msgid(ar_dsp_dev.runtime_client, ar_dsp_dev.runtime_msg_id);
	ar_icc_client_destroy(ar_dsp_dev.runtime_client);

	misc_deregister(&ar_dsp_dev.misc_dev);

	return 0;
}

static const struct of_device_id ar_dsp_match[] = {
	{.compatible = "artosyn,dsp"},

	{},
};

MODULE_DEVICE_TABLE(of, ar_dsp_match);

static struct platform_driver ar_dsp_platform_driver = {
	.driver = {
		.name = "artosyn dsp",
		.of_match_table = ar_dsp_match,
	},

	.probe = ar_dsp_probe,
	.remove = ar_dsp_remove,
};


static int __init ar_dsp_dev_init(void)
{
	return platform_driver_register(&ar_dsp_platform_driver);
}

static void __exit ar_dsp_dev_exit(void)
{
	platform_driver_unregister(&ar_dsp_platform_driver);
}

module_init(ar_dsp_dev_init);
module_exit(ar_dsp_dev_exit);

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("ar DSP driver");
MODULE_LICENSE("GPL v2");
