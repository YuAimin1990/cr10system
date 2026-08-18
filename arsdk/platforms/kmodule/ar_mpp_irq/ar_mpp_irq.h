/*
 *  AR mpp driver defines
 *
 */

#ifndef _UAPI_LINUX_AR_MPP_H
#define _UAPI_LINUX_AR_MPP_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define AR_MPP_DEVICE_NAME		"ar_mpp_ctl"

#define MAX_IRQ_CNT			(512)
#define MAX_SUB_NODE_IRQ_CNT		(32)


#define MAX_NAME_LEN			(20)
#define MAX_COMPT_NAME_LEN			(32)


#ifdef AR9341
#define VIF_REG_BASE			(0x01100000)
#else
#define VIF_REG_BASE			(0x08870000)
#endif
#define VIF_CTL_OFFSET			(0xaf << 2)
#define VIF_INT_OFFSET			(0X17C)

#ifdef AR9341
#define IRQ_REG_BASE			(0x01001000)
#else
#define IRQ_REG_BASE			(0x00201000)
#endif



/*
 * structure use when ioctl AR_MPP_REG_IRQ
 */
struct request_irq_desc {
	unsigned int irq_id;					/*which one irq happend*/
	unsigned char name[MAX_NAME_LEN];		/*the request irq name*/
};

/*
 * structure use when ioctl AR_MPP_QUERY_IRQ
 */
struct query_irq_event {
	unsigned int irq_id;		/*which one irq happend*/
	unsigned int reserved;
	unsigned long long occur_time;	/*time when the irq happend*/
};

struct ar_drv_info
{
	char compatible_name[MAX_COMPT_NAME_LEN];
	int total_irq_num;
	int irq_num[MAX_SUB_NODE_IRQ_CNT];
	unsigned long long reg_addr;
	unsigned long long reg_size;
	int total_addr_num;
	unsigned long long reg_n_addr[MAX_SUB_NODE_IRQ_CNT];
	unsigned long long reg_n_size[MAX_SUB_NODE_IRQ_CNT];
};

struct ar_drv_vif_fps_info
{
   int view_no;//which vif view will be queryed fps
   unsigned long long deta_ns_two_vsync;// the times of two vsync
   int local_irq;
   int frame_num;   
};

struct ar_drv_h26x_vpu_info
{
    char compatible_name[MAX_COMPT_NAME_LEN];
    char conf_file_path[64];
    int  vpu_timeout_us;
};

#define AR_MPP_IOCTL_MAGIC 	'M'

#define AR_MPP_REG_IRQ		_IOW(AR_MPP_IOCTL_MAGIC, 0, struct request_irq_desc)

#define AR_MPP_UNREG_IRQ	_IOW(AR_MPP_IOCTL_MAGIC, 1, unsigned int)

#define AR_MPP_DISABLE_IRQ	_IOW(AR_MPP_IOCTL_MAGIC, 2, unsigned int)

#define AR_MPP_ENABLE_IRQ	_IOW(AR_MPP_IOCTL_MAGIC, 3, unsigned int)

#define AR_MPP_QUERY_IRQ	_IOR(AR_MPP_IOCTL_MAGIC, 4, struct query_irq_event)

#define AR_MPP_QUERY_IRQ_ONESHOT	_IOR(AR_MPP_IOCTL_MAGIC, 5, struct query_irq_event)

#define AR_MPP_QUERY_DRV_INFO	_IOR(AR_MPP_IOCTL_MAGIC, 6, struct ar_drv_info)

#define AR_MPP_QUERY_VIF_FPS	_IOR(AR_MPP_IOCTL_MAGIC, 7, struct ar_drv_vif_fps_info)

#define AR_MPP_QUERY_H26X_INFO	_IOR(AR_MPP_IOCTL_MAGIC, 8, struct ar_drv_h26x_vpu_info)

#endif /* _UAPI_LINUX_WATCHDOG_H */


