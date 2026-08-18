/*
 * Artosyn linux mpp driver
 */
#undef DEBUG
#define pr_fmt(fmt) "ar-mpp-irq: " fmt

#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/bitmap.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/freezer.h>

#include "ar_mpp_irq.h"

#define MAX_DEV_CNT				(64)

/*
 * max_irq_event_cnt need to be (max_irq_event_cnt > 0 && max_irq_event_cnt = 2 ^ n)
 */
static unsigned int max_irq_event_cnt = 0x20;
module_param(max_irq_event_cnt, uint, 0644);

//from original irq to virq (mapped irq table)
struct map_of_irq {
	unsigned int			hwirq;
	unsigned int			virq;
};

/*
 * structure use for create mpp driver
 */
struct mpp_irq_drv {
	unsigned int			major;
	struct class			*drv_class;
	struct cdev				cdev;
	struct device			*pdev;

	char					*irq_name[MAX_IRQ_CNT];

	struct map_of_irq		irq_map[MAX_IRQ_CNT];
	unsigned int			map_cnt;

	spinlock_t				drv_lock;

	//all devices enabled irq set, protect by drv drv_lock
	unsigned char			irq_is_enable[MAX_IRQ_CNT];
	//all devices registered irqs set, protect by drv mutex
	unsigned long			drv_hwirq_bitmap[MAX_IRQ_CNT / (sizeof(long) * 8)];
};

/*
 * inforamtion for the open ar_mpp device infomation
 */
struct mpp_irq_dev {

	atomic_t				rd_ptr; //not need to protect, only one thread write dev rd_ptr
	atomic_t				wr_ptr; //wr_ptr need be protected by dev drv_lock in irq handler
	spinlock_t				dev_lock;

	struct query_irq_event	*irq_event;

	//specified device registered irq set, protect by drv drv_lock because couple with drv_oriq_bitmap
	unsigned long			dev_hwirq_bitmap[MAX_IRQ_CNT / (sizeof(long) * 8)];

	wait_queue_head_t		waitq;
	struct mpp_irq_drv		*p_irq_drv;
};

static struct mpp_irq_drv g_mpp_irq_drv;

#define IS_VALID_IRQ_ID(id)				(id > 0 && id < MAX_IRQ_CNT)
#define IS_IRQ_REGISTERED(bitmap, id)	(((bitmap)[(id) / (sizeof(bitmap[0])*8)] >> ((id) % (sizeof((bitmap)[0]) *8))) & 0x01)

static int ar_mpp_open(struct inode *inode, struct file *filp);
static long ar_mpp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int ar_mpp_close(struct inode *inode, struct file *filp);
static ssize_t ar_mpp_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);


static struct file_operations ar_mpp_fops = {
	.owner			= THIS_MODULE,
	.open			= ar_mpp_open,
	.release		= ar_mpp_close,
	.write			= ar_mpp_write,
	.unlocked_ioctl	= ar_mpp_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl	= ar_mpp_ioctl,
#endif
};


static int _hwirq_to_virq(struct mpp_irq_drv *p_irq_drv, unsigned int oriq)
{
	unsigned int nr;

	for (nr = 0; nr < p_irq_drv->map_cnt; nr ++) {
		if (p_irq_drv->irq_map[nr].hwirq == oriq)
			return p_irq_drv->irq_map[nr].virq;
	}

	pr_err("hwirq %d not found!\n", oriq);
	BUG();
	return -1;
}

static int _virq_to_hwirq(struct mpp_irq_drv *p_irq_drv, unsigned int virq)
{
	unsigned int nr;

	for (nr = 0; nr < p_irq_drv->map_cnt; nr ++) {
		if (p_irq_drv->irq_map[nr].virq == virq)
			return p_irq_drv->irq_map[nr].hwirq;
	}

	BUG();
	return -1;
}

static int ar_mpp_open(struct inode *inode, struct file *filp)
{
	struct mpp_irq_drv *p_irq_drv = container_of(inode->i_cdev, struct mpp_irq_drv, cdev);
	struct mpp_irq_dev *p_irq_dev = devm_kzalloc(p_irq_drv->pdev, sizeof(*p_irq_dev), GFP_KERNEL);

	if (IS_ERR(p_irq_dev))
		return -ENOMEM;

	p_irq_dev->irq_event = devm_kzalloc(p_irq_drv->pdev,
									sizeof(struct query_irq_event) * max_irq_event_cnt,
									GFP_KERNEL);
	if (IS_ERR(p_irq_dev->irq_event))
		return -ENOMEM;

	p_irq_dev->p_irq_drv = p_irq_drv;

	atomic_set(&p_irq_dev->rd_ptr, 0);
	atomic_set(&p_irq_dev->wr_ptr, 0);

	init_waitqueue_head(&p_irq_dev->waitq);

	filp->private_data = p_irq_dev;

	spin_lock_init(&p_irq_dev->dev_lock);

	return 0;
}


/*
 * critical case: in closing, the irq happened; or irq happening, close the dev
 * not critical case: close, ioctl
 */

static int ar_mpp_close(struct inode *inode, struct file *filp)
{
	int hwirq, virq;
	struct mpp_irq_dev * p_irq_dev = filp->private_data;
	struct mpp_irq_drv * p_irq_drv = p_irq_dev->p_irq_drv;
	unsigned long tmp_bitmap[MAX_IRQ_CNT / (sizeof(long) * 8)];

	//close vif if the caller is owner of vif irq

#define VIF_HW_IRQ (106)
	if (IS_IRQ_REGISTERED(p_irq_dev->dev_hwirq_bitmap, VIF_HW_IRQ)) {
		//for arm64, ioremap prot is PROT_DEVICE_nGnRnE, same with ioremap_nocache
		void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_CTL_OFFSET, 4);
		writel(0x01, tmp_addr);
		iounmap(tmp_addr);

		pr_warn("closed vif! \n");
	}

	pr_debug("irq p_irq_drv bitmap: 0x%lx 0x%lx p_irq_dev bitmap: 0x%lx 0x%lx\n",
					p_irq_drv->drv_hwirq_bitmap[0], p_irq_drv->drv_hwirq_bitmap[1],
					p_irq_dev->dev_hwirq_bitmap[0], p_irq_dev->dev_hwirq_bitmap[1]);

	//protect bitmap operation
	spin_lock(&p_irq_drv->drv_lock);
	bitmap_copy(tmp_bitmap, p_irq_dev->dev_hwirq_bitmap, MAX_IRQ_CNT);
	spin_unlock(&p_irq_drv->drv_lock);

	//free all irq which is registered by the dev synchronously
	while ((hwirq = find_first_bit(tmp_bitmap, MAX_IRQ_CNT)) < MAX_IRQ_CNT) {
		virq = _hwirq_to_virq(p_irq_drv, hwirq);

		//free_irq sync to the irq handler finish
		free_irq(virq, p_irq_dev);
		p_irq_drv->irq_is_enable[hwirq] = 0;
        bitmap_clear(tmp_bitmap, hwirq, 1);
		pr_warn("%s:%d: freeing irq: %d %d \n", __func__, __LINE__, hwirq, virq);
	}

	//not protect dev_hwirq_bitmap, protect drv_hwirq_bitmap
	//concurrent function: irq handler
	//not concurrent function: ioctl and other function
	spin_lock(&p_irq_drv->drv_lock);
	bitmap_xor(tmp_bitmap, p_irq_drv->drv_hwirq_bitmap, p_irq_dev->dev_hwirq_bitmap, MAX_IRQ_CNT);
	bitmap_copy(p_irq_drv->drv_hwirq_bitmap, tmp_bitmap, MAX_IRQ_CNT);
	filp->private_data = NULL;
	wake_up(&p_irq_dev->waitq);
	devm_kfree(p_irq_drv->pdev, p_irq_dev->irq_event);
	devm_kfree(p_irq_drv->pdev, p_irq_dev);
	spin_unlock(&p_irq_drv->drv_lock);

	return 0;
}

static ssize_t ar_mpp_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int i = 0;
	
	for(i = 0; i < MAX_IRQ_CNT; ++i)
	{
		if(g_mpp_irq_drv.irq_name[i])
		{
			struct irq_desc *desc = irq_to_desc(_hwirq_to_virq(&g_mpp_irq_drv, i));
			printk(KERN_INFO "index %d name %s virq %d hwirq %d enable %d irq depth %d\n",
				i, g_mpp_irq_drv.irq_name[i],
				_hwirq_to_virq(&g_mpp_irq_drv, i),
				i,
				g_mpp_irq_drv.irq_is_enable[i],
				(NULL == desc) ? 0xff : desc->depth);
		}
	}

	return count;
}

static void mpp_irq_drv_destroy(struct mpp_irq_drv *p_irq_drv)
{
	device_destroy(p_irq_drv->drv_class, MKDEV(p_irq_drv->major, 0));

	cdev_del(&p_irq_drv->cdev);

	class_destroy(p_irq_drv->drv_class);

	unregister_chrdev_region(MKDEV(p_irq_drv->major, 0), MAX_DEV_CNT);
}


static int mpp_irq_drv_create(struct platform_device *pdev)
{
	int rc = 0;
	dev_t dev_id;
	struct device *drv_device = NULL;
	struct mpp_irq_drv *p_irq_drv = &g_mpp_irq_drv;

	if ((max_irq_event_cnt == 0) || ((max_irq_event_cnt & (max_irq_event_cnt - 1)) != 0)) {
		pr_err("error max_irq_event_cnt value: %x %x\n",
				max_irq_event_cnt, (max_irq_event_cnt & (max_irq_event_cnt - 1)));
		return -EFAULT;
	}

	/*
	 * drvier init
	*/
	if (alloc_chrdev_region(&dev_id, 0, MAX_DEV_CNT, AR_MPP_DEVICE_NAME)) {
		pr_err("unable to allocate chrdev region\n");
		return -EFAULT;
	}

	p_irq_drv->drv_class = class_create(THIS_MODULE, AR_MPP_DEVICE_NAME);
	if (IS_ERR(p_irq_drv->drv_class)) {
		pr_err("class_create failed for adf_ctl\n");
		rc = PTR_ERR(p_irq_drv->drv_class);
		goto err_chrdev_unreg;
	}

	p_irq_drv->major = MAJOR(dev_id);
	p_irq_drv->pdev  = &pdev->dev;

	/*build hwirq virq map table*/
	{
		unsigned int i;
		struct device_node *np = pdev->dev.of_node;
		unsigned int num = of_irq_count(np);
		struct device_node * child = NULL;

		//p_irq_drv->map_cnt = num;

		for (i = 0 ; i < num; i++) {
			struct of_phandle_args hwirq;
			rc = of_irq_parse_one(np, i, &hwirq);
			if (rc) {
				pr_err("of_irq_parser_one rc = %d\n", rc);
				goto err_chrdev_unreg;
			}

			p_irq_drv->irq_map[i].hwirq = hwirq.args[1];
			pr_debug("==========>%d %d %d\n", hwirq.args[0], hwirq.args[1], hwirq.args[2]);
			p_irq_drv->irq_map[i].virq = irq_create_of_mapping(&hwirq);
			pr_debug("mapping irq from->to (%d %d) \n", p_irq_drv->irq_map[i].hwirq, p_irq_drv->irq_map[i].virq);
		}

		for_each_child_of_node(pdev->dev.of_node, child) {
			struct of_phandle_args hwirq;
			int j = 0;

			for(j = 0; j < of_irq_count(child); ++j)
			{
				rc = of_irq_parse_one(child, j, &hwirq);
				if (rc) {
					pr_err("of_irq_parser_one rc = %d\n", rc);
					goto err_chrdev_unreg;
				}

				p_irq_drv->irq_map[i].hwirq = hwirq.args[1];
				pr_debug("==========>%d %d %d\n", hwirq.args[0], hwirq.args[1], hwirq.args[2]);
				p_irq_drv->irq_map[i].virq = irq_create_of_mapping(&hwirq);
				pr_debug("mapping irq from->to (%d %d) \n", p_irq_drv->irq_map[i].hwirq, p_irq_drv->irq_map[i].virq);
				++i;
			}
		}

		p_irq_drv->map_cnt = i;

		spin_lock_init(&p_irq_drv->drv_lock);
	}

	cdev_init(&p_irq_drv->cdev, &ar_mpp_fops);
	p_irq_drv->cdev.owner = THIS_MODULE;
	if ((rc = cdev_add(&p_irq_drv->cdev, dev_id, 1)) != 0) {
		pr_err("cdev add failed\n");
		goto err_class_destr;
	}

	drv_device = device_create(p_irq_drv->drv_class, NULL,
								MKDEV(p_irq_drv->major, 0),
								NULL, AR_MPP_DEVICE_NAME);
	if (IS_ERR(drv_device)) {
		pr_err("failed to create device\n");
		rc = PTR_ERR(drv_device);
		goto err_cdev_del;
	}

	platform_set_drvdata(pdev, p_irq_drv);

	return 0;

err_cdev_del:
	cdev_del(&p_irq_drv->cdev);

err_class_destr:
	class_destroy(p_irq_drv->drv_class);

err_chrdev_unreg:
	unregister_chrdev_region(dev_id, MAX_DEV_CNT);

	return rc;
}


/*
 * save the irq info to queue list, and wakeup the receiver
 * FORCE_IRQ_NO_THEAD flag: to run the irq handler in time
*/
irqreturn_t ar_mpp_irq_handler(int virq, void * dev_id)
{
	irqreturn_t rc = IRQ_HANDLED;
	unsigned int rd, wr, wr_next;

	struct mpp_irq_dev *p_irq_dev = (struct mpp_irq_dev *)dev_id;
	struct mpp_irq_drv *p_irq_drv = p_irq_dev->p_irq_drv;

	int hwirq = _virq_to_hwirq(p_irq_drv, virq);
	if (hwirq < 0) {
		pr_err("not found irq %d %p\n", virq, &p_irq_dev->waitq);
		BUG();
	}

	rd = atomic_read(&p_irq_dev->rd_ptr);

	spin_lock(&p_irq_dev->dev_lock);


	if(p_irq_drv->irq_is_enable[hwirq]) {
		disable_irq_nosync(virq);
		p_irq_drv->irq_is_enable[hwirq] = 0;
	} else {
		pr_err("%d irq %d not enable %d\n", __LINE__, virq, p_irq_drv->irq_is_enable[hwirq]);
	}

	mb();//barrier();
	
	//protect more than one irq handler concurrent, not need to protect ioctl
	wr = atomic_read(&p_irq_dev->wr_ptr);

	wr_next = ((wr + 1) & (max_irq_event_cnt -1));
	if (wr_next == rd) {
		pr_err("p_irq_dev(%p) irq_event irq = %d full waiton: %p\n", p_irq_dev, hwirq, &p_irq_dev->waitq);
	} else {
		//struct timeval tv;
		//do_gettimeofday(&tv);
		//p_irq_dev->irq_event[wr].occur_time = timeval_to_ktime(tv).tv64;
		p_irq_dev->irq_event[wr].occur_time = ktime_get_raw_ns();
		p_irq_dev->irq_event[wr].irq_id = hwirq;

		mb();//barrier();

		atomic_set(&p_irq_dev->wr_ptr, wr_next);
	}

	spin_unlock(&p_irq_dev->dev_lock);

	

	wake_up(&p_irq_dev->waitq);

	pr_debug(" === waking up irq: %p %d %d irq: %d %d\n", &p_irq_dev->waitq, wr_next, rd, virq, hwirq);

	return rc;
}

static inline unsigned int cpumask_last(const struct cpumask *srcp)
{
	return find_last_bit(cpumask_bits(srcp), nr_cpumask_bits);
}

/**
 * ar_mpp_ioctl() - IO control on ar_mpp driver
 * return: 0 success
*/

static unsigned long long vsync[100]={0};
static unsigned long long sample[100]={0};

//#define debug_fps

static inline unsigned long long get_time_ns(void)
{
   #if 0
   return ktime_get_raw_ns();
   #else
   unsigned long long val=0;
   __asm__ volatile("mrs %0,cntvct_el0":"=r"(val)::);
   return val<<2;   
   #endif
}

static long ar_mpp_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	struct mpp_irq_dev *p_irq_dev = (struct mpp_irq_dev *)filp->private_data;
	struct mpp_irq_drv *p_irq_drv = p_irq_dev->p_irq_drv;

	if (_IOC_TYPE(cmd) != AR_MPP_IOCTL_MAGIC) {
		pr_err("EINVAL _IOC_TYPE cmd 0x%x:_IOC_TYPE(cmd):0x%x app magic:0x%x\n", \
				cmd, _IOC_TYPE(cmd), AR_MPP_IOCTL_MAGIC);
		rc = -EINVAL;
		goto __out;
	}

	switch (cmd) {
		case AR_MPP_REG_IRQ: {
			int hwirq, virq;
			struct request_irq_desc request;

			if ((rc = copy_from_user(&request, (struct request_irq_desc __user *)arg, sizeof(request))) != 0)  {
				pr_err("copy_from_user rc = %d \n", rc);
				break;
			}

			hwirq = request.irq_id;
			if (!IS_VALID_IRQ_ID(hwirq)) {
				pr_err("irq id=%d out range \n", hwirq);
				rc = -EINVAL;
				break;
			}

			virq = _hwirq_to_virq(p_irq_drv, hwirq);
			if (virq < 0) {
				pr_err("irq %d not in dts \n", hwirq);
				rc = -EINVAL;
				break;
			}
			spin_lock(&p_irq_drv->drv_lock);

			//check drv bitmap, this is full set of devs irqs
			if (IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq)) {
				pr_err("irq id=%d already regisgered \n", hwirq);
				rc = -EINVAL;
				spin_unlock(&p_irq_drv->drv_lock);
				break;
			}

			//when REG_IRQ, set drv_hwirq_bitmap = 1, then start registering
			//at this moment drv_hwirq_bitmap = 1 && dev_hwirq_bitmap == 0
			bitmap_set(p_irq_drv->drv_hwirq_bitmap, hwirq, 1);
			spin_unlock(&p_irq_drv->drv_lock);

			p_irq_drv->irq_name[hwirq] = devm_kzalloc(p_irq_drv->pdev, MAX_NAME_LEN + 1, GFP_KERNEL);
			if(NULL == p_irq_drv->irq_name[hwirq]) {
				pr_err("irq name kzalloc failed!\n");
				rc = -ENOMEM;
				break;
			} else {
				if (strnlen(request.name, MAX_NAME_LEN) == 0)
				{
					snprintf(p_irq_drv->irq_name[hwirq], MAX_NAME_LEN, "irq-%d", hwirq);
				}
				else
				{
					strncpy(p_irq_drv->irq_name[hwirq], request.name, MAX_NAME_LEN);
				}

				p_irq_drv->irq_name[hwirq][MAX_NAME_LEN] = '\0';
			}

			spin_lock(&p_irq_drv->drv_lock);
			//when REG_IRQ, set dev_hwirq_bitmap = 1 after registered
			//at this moment: drv_hwirq_bitmap = 1 and dev_hwirq_bitmap == 0
			bitmap_set(p_irq_dev->dev_hwirq_bitmap, hwirq, 1);
			p_irq_drv->irq_is_enable[hwirq] = 1;
			spin_unlock(&p_irq_drv->drv_lock);

			//pr_info("bind irq to cpu %d\n", cpumask_last(cpu_online_mask));
			irq_set_affinity_hint(virq, cpumask_of(cpumask_last(cpu_online_mask)));
			rc = request_irq(virq, ar_mpp_irq_handler, IRQF_NO_THREAD, p_irq_drv->irq_name[hwirq], (void *)p_irq_dev);
			if (rc != 0) {
				spin_lock(&p_irq_drv->drv_lock);
				bitmap_set(p_irq_drv->drv_hwirq_bitmap, hwirq, 0);
				p_irq_drv->irq_is_enable[hwirq] = 0;
				spin_unlock(&p_irq_drv->drv_lock);
			}

			break;
		}

		case AR_MPP_UNREG_IRQ: {

			unsigned int hwirq = (unsigned int)arg;
			int virq = _hwirq_to_virq(p_irq_drv, hwirq);
			if (virq < 0) {
				pr_err("irq %d not in dts \n", hwirq);
				rc = -EINVAL;
				break;
			}

			spin_lock(&p_irq_drv->drv_lock);

			//check the hwirq alrady registered: drv_hwirq_bitmap == 1 && dev_hwirq_bitmap == 1

			if (!IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq)) {
				pr_err("irq % is not valid or is already registered: 0x%lx 0x%lx\n",
						hwirq, p_irq_drv->drv_hwirq_bitmap[0], p_irq_drv->drv_hwirq_bitmap[1]);
				rc = -EINVAL;

				spin_unlock(&p_irq_drv->drv_lock);
				break;
			}

			//the caller need to be the one registered the irq;
			if (!IS_IRQ_REGISTERED(p_irq_dev->dev_hwirq_bitmap, hwirq)) {
				pr_err("irq % is not registered by the caller: 0x%lx 0x%lx\n",
						hwirq, p_irq_dev->dev_hwirq_bitmap[0], p_irq_dev->dev_hwirq_bitmap[1]);
				rc = -EINVAL;
				spin_unlock(&p_irq_drv->drv_lock);
				break;
			}
			irq_set_affinity_hint(virq, NULL);
			spin_unlock(&p_irq_drv->drv_lock);

			//wait the irq finish and free
			free_irq(virq, (void *)p_irq_dev);

			spin_lock(&p_irq_drv->drv_lock);
			bitmap_clear(p_irq_dev->dev_hwirq_bitmap, hwirq, 1);
			bitmap_clear(p_irq_drv->drv_hwirq_bitmap, hwirq, 1);

			if(p_irq_drv->irq_name[hwirq])
			{
				devm_kfree(p_irq_drv->pdev, p_irq_drv->irq_name[hwirq]);
			}

			p_irq_drv->irq_name[hwirq] = NULL;
			spin_unlock(&p_irq_drv->drv_lock);

			break;
		}

		case AR_MPP_DISABLE_IRQ: {
			unsigned int hwirq = (unsigned int)arg;
			int virq = _hwirq_to_virq(p_irq_drv, hwirq);
			unsigned long flags;

			if (virq < 0) {
				pr_err("irq %d not in dts \n", hwirq);
				rc = -EINVAL;
				break;
			}

			//check the hwirq alrady registered: drv_hwirq_bitmap == 1 && dev_hwirq_bitmap == 1 && irq_is_enable = 1
			spin_lock_irqsave(&p_irq_drv->drv_lock, flags);

			if ((!IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq)) || (!p_irq_drv->irq_is_enable[hwirq])) {
				pr_err("irq %d not registered(%ld) or enabled(%d) \n", hwirq,
							IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq),
							p_irq_drv->irq_is_enable[hwirq]);
				spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);
				rc = -EINVAL;
				break;
			}

			//the caller need to be the one registered the irq;
			if (!IS_IRQ_REGISTERED(p_irq_dev->dev_hwirq_bitmap, hwirq)) {
				pr_err("irq % is not registered by the caller: 0x%lx 0x%lx\n",
						hwirq, p_irq_dev->dev_hwirq_bitmap[0], p_irq_dev->dev_hwirq_bitmap[1]);
				rc = -EINVAL;
				spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);
				break;
			}

			if(p_irq_drv->irq_is_enable[hwirq]) {
				disable_irq(virq); //disable interrupt and wait for irq thread
				p_irq_drv->irq_is_enable[hwirq] = 0;
				mb();
			} else {
				pr_err("%d irq %d not enable %d\n", __LINE__, virq, p_irq_drv->irq_is_enable[hwirq]);
			}

			spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);

			rc = 0;

			break;
		}

		case AR_MPP_ENABLE_IRQ: {
			unsigned int hwirq = (unsigned int)arg;
			unsigned long flags;
		
			int virq = _hwirq_to_virq(p_irq_drv, hwirq);
			if (virq < 0) {
				pr_err("irq %d not in dts \n", hwirq);
				rc = -EINVAL;
				break;
			}

			pr_debug(" === ENABLE_IRQ 1: %d %d\n",hwirq, virq);

			//check the hwirq alrady registered: drv_hwirq_bitmap == 1 && dev_hwirq_bitmap == 1

			
			spin_lock_irqsave(&p_irq_drv->drv_lock, flags);
			rmb();

			if ((!IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq)) || (p_irq_drv->irq_is_enable[hwirq])) {
				pr_err("irq %d not registered(%ld) or enabled(%d) \n", hwirq,
							IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq),
							p_irq_drv->irq_is_enable[hwirq]);
				rc = -EINVAL;
				spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);
				break;
			}

			if (!IS_IRQ_REGISTERED(p_irq_dev->dev_hwirq_bitmap, hwirq)) {
				pr_err("irq % is not registered by the caller: 0x%lx 0x%lx\n",
						hwirq, p_irq_dev->dev_hwirq_bitmap[0], p_irq_dev->dev_hwirq_bitmap[1]);
				rc = -EINVAL;
				spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);;
				break;
			}

			if(0 == p_irq_drv->irq_is_enable[hwirq]) {
				p_irq_drv->irq_is_enable[hwirq] = 1;
				mb();
				enable_irq(virq);
			} else {
				pr_err("%d irq %d not disable %d\n", __LINE__, virq, p_irq_drv->irq_is_enable[hwirq]);
			}

			spin_unlock_irqrestore(&p_irq_drv->drv_lock, flags);

			rc = 0;

			pr_debug(" === ENABLE_IRQ 2: %d %d\n",hwirq, virq);

			break;
		}

		case AR_MPP_QUERY_IRQ: {

			if (filp->f_flags & O_NONBLOCK) { //O_NOBLOCK mode
				if ((atomic_read(&p_irq_dev->rd_ptr) == atomic_read(&p_irq_dev->wr_ptr))) {
					rc = -EAGAIN;
					break;
				}
			} else {
				// not need to protect rd_ptr and wr_ptr, because check whether rd_ptr, wr_ptr are equal

				pr_debug(" === waiting irq: %p %d %d\n", &p_irq_dev->waitq, atomic_read(&p_irq_dev->wr_ptr), atomic_read(&p_irq_dev->rd_ptr));
				if (wait_event_freezable(p_irq_dev->waitq,
						atomic_read(&p_irq_dev->rd_ptr) != atomic_read(&p_irq_dev->wr_ptr))) {
					rc = -ERESTARTSYS;
					pr_err("wait_event_freezable rc = %d \n", rc);

					break;
				}
			}

			if(!filp->private_data) {
				rc = -ERESTARTSYS;
				break;
			}

			//mb();//barrier();
			pr_debug(" === get event: %p %d %d\n", &p_irq_dev->waitq, atomic_read(&p_irq_dev->wr_ptr), atomic_read(&p_irq_dev->rd_ptr));

			//copy one event and return
			{
				struct query_irq_event * __user query = (struct query_irq_event __user *)arg;
				unsigned int rd = atomic_read(&p_irq_dev->rd_ptr);

				if (p_irq_dev->irq_event[rd].irq_id > MAX_IRQ_CNT)
					pr_err("%d id = %d rd = %d wr = %d", __LINE__, p_irq_dev->irq_event[rd].irq_id, rd, atomic_read(&p_irq_dev->wr_ptr));
				else
					pr_debug("event: irq id %d\n", p_irq_dev->irq_event[rd].irq_id);

				rc = copy_to_user(query, p_irq_dev->irq_event + rd, sizeof(*query));

				//memset(p_irq_dev->irq_event + rd, 0x7f, sizeof(*query));

				rd = ((rd + 1) & (max_irq_event_cnt -1));
				mb();//barrier();
				atomic_set(&p_irq_dev->rd_ptr, rd);
			}

			pr_debug(" === get done: %p %d %d\n", &p_irq_dev->waitq, atomic_read(&p_irq_dev->wr_ptr), atomic_read(&p_irq_dev->rd_ptr));

			break;
		}

		case AR_MPP_QUERY_IRQ_ONESHOT: {

			struct request_irq_desc request;
			unsigned int hwirq;
			int virq;

			if (copy_from_user(&request, (struct request_irq_desc __user *)arg, sizeof(request))) {
				rc = -EFAULT;
				pr_err("copy_from_user = %d \n", rc);
				break;
			}

			hwirq = request.irq_id;
			virq = _hwirq_to_virq(p_irq_drv, hwirq);
			if (virq < 0) {
				pr_err("irq %d not in dts \n", hwirq);
				rc = -EINVAL;
				break;
			}

			if (IS_IRQ_REGISTERED(p_irq_drv->drv_hwirq_bitmap, hwirq)) {
				pr_err("irq %d already registered in drv, conflict with oneshot \n", hwirq);
				//spin_unlock(&p_irq_drv->drv_lock);
				rc = -EINVAL;
				break;
			}

			if (filp->f_flags & O_NONBLOCK) { //O_NOBLOCK mode
				pr_err("IRQF_ONESHOT (%d) not support non-blocking mode \n", hwirq);
				rc = -EINVAL;
				break;
			}

			p_irq_drv->irq_is_enable[hwirq] = 1;

			p_irq_drv->irq_name[hwirq] = devm_kzalloc(p_irq_drv->pdev, MAX_NAME_LEN, GFP_KERNEL);
			if(NULL == p_irq_drv->irq_name[hwirq]) {
				pr_err("irq name kzalloc failed!\n");
				rc = -ENOMEM;
				break;
			} else {
				if (strlen(request.name) == 0)
					sprintf(p_irq_drv->irq_name[hwirq], "irq-oneshot-%d", hwirq);
				else
					strcpy(p_irq_drv->irq_name[hwirq], request.name);
			}

			rc = request_irq(virq, ar_mpp_irq_handler, IRQF_ONESHOT, p_irq_drv->irq_name[hwirq], (void *)p_irq_dev);
			if (rc != 0) {
				pr_err("request IRQF_ONESHOT (%d %d) rc=%d\n", hwirq, virq, rc);
				p_irq_drv->irq_is_enable[hwirq] = 0;
				break;
			}

			if (wait_event_interruptible(p_irq_dev->waitq,
					(atomic_read(&p_irq_dev->rd_ptr) != atomic_read(&p_irq_dev->wr_ptr)))) {
				rc = -ERESTARTSYS;
				pr_err("wait_event_interruptible rc = %d \n", rc);
				break;
			}

			//copy one event and return
			{
				struct query_irq_event * __user query = (struct query_irq_event __user *)arg;
				unsigned int rd = atomic_read(&p_irq_dev->rd_ptr);

				BUG_ON(p_irq_dev->irq_event[rd].irq_id > MAX_IRQ_CNT);

				//return copy to user return value
				rc = copy_to_user(query, p_irq_dev->irq_event + rd, sizeof(*query));
				rd = ((rd + 1) & (max_irq_event_cnt -1));
				atomic_set(&p_irq_dev->rd_ptr, rd);
			}

			free_irq(virq, (void *)p_irq_dev); //disable irq and wait irq thread end
			p_irq_drv->irq_is_enable[hwirq] = 0;

			break;
		}

		case AR_MPP_QUERY_DRV_INFO: {
			struct ar_drv_info info;
			struct device_node *np = p_irq_drv->pdev->of_node;
			struct device_node * child = NULL;
			rc = -EFAULT;

			if (copy_from_user(&info, (struct ar_drv_info __user *)arg, sizeof(info))) {
				rc = -EFAULT;
				pr_err("copy_from_user = %d \n", rc);
				break;
			}

			for_each_child_of_node(np, child) {
				if(of_device_is_compatible(child, info.compatible_name)) {
					int i = 0;
					int num_reg = 0;
					struct resource temp_res;

					info.total_irq_num = of_irq_count(child);
					if(info.total_irq_num > MAX_SUB_NODE_IRQ_CNT) {
						pr_err("total_irq_num %d bigger than max sub mode irq num %d!\n", 
							info.total_irq_num, MAX_SUB_NODE_IRQ_CNT);
						info.total_irq_num = MAX_SUB_NODE_IRQ_CNT;
					}

					for(i = 0; i < info.total_irq_num; ++i) {
						struct of_phandle_args hwirq;

						rc = of_irq_parse_one(child, i, &hwirq);
						if (rc) {
							pr_err("of_irq_parser_one rc = %d\n", rc);
							break;
						}
						info.irq_num[i] = hwirq.args[1];
					}

					while (of_address_to_resource(child, num_reg, &temp_res) == 0)
						num_reg++;
					if(1 == num_reg) {
						info.reg_addr = of_translate_address(child, of_get_address(child, 0, &info.reg_size, NULL));
						rc = copy_to_user((struct ar_drv_info __user *)arg, &info, sizeof(info));
						if (rc) {
							pr_err("of_irq_parser_one rc = %d\n", rc);
							break;
						}
					} else if (num_reg > 1 && num_reg <= MAX_SUB_NODE_IRQ_CNT) {
						num_reg = 0;
						while (of_address_to_resource(child, num_reg, &temp_res) == 0) {
							info.reg_n_addr[num_reg] = of_translate_address(child, of_get_address(child, num_reg, &info.reg_n_size[num_reg], NULL));
							//add for compatible
							if(0 == num_reg) {
								info.reg_addr = info.reg_n_addr[num_reg];
								info.reg_size = info.reg_n_size[num_reg];
							}
							//printk(KERN_INFO "info.reg_n_addr[%d]:%llu\n", num_reg, info.reg_n_addr[num_reg]);
							num_reg++;
						}
						info.total_addr_num = num_reg;
						rc = copy_to_user((struct ar_drv_info __user *)arg, &info, sizeof(info));
						if (rc) {
								pr_err("of_irq_parser_one rc = %d\n", rc);
								break;
						}
					}
					break;
				}
			}
			break;
		}
#if 1                            
        case AR_MPP_QUERY_VIF_FPS:
        {
            struct ar_drv_vif_fps_info info;
			rc = -EFAULT;
            int rc1=0;
			int i=0;
			if (copy_from_user(&info, (struct ar_drv_info __user *)arg, sizeof(info))) {
				rc = -EFAULT;
				pr_err("copy_from_user = %d \n", rc);
				break;
			}			
			void __iomem *irq_addr = ioremap(IRQ_REG_BASE, 4);
			void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_INT_OFFSET, 4);
			uint32_t reg_value=0;
			unsigned long flags;
			int b_first_vsnc=0;
			int frame_num=info.frame_num;
			if(frame_num<2)
			{
			   frame_num=2;
			}
			
			//diable all the irq
			if(info.local_irq==0)
			{
			   local_irq_save(flags);
			}else{
			   iowrite32(0,irq_addr);
			}
			//clear the current vif irq, if we have 
			reg_value=ioread32(tmp_addr);			
			iowrite32(reg_value,tmp_addr);
			//wait the first vsync and record the time
			unsigned long long start_time=get_time_ns();
			while(1)
			{
			   reg_value=ioread32(tmp_addr);
			   if(reg_value&(1<<info.view_no))
			   {
			       info.deta_ns_two_vsync= get_time_ns();				   
			       iowrite32(reg_value,tmp_addr);
				   b_first_vsnc=1;
				   break;
			   }
			   else
			   {
			       uint32_t deata=get_time_ns()-start_time;				   
			       if(deata>100000000)
			       {			       
					   iowrite32(reg_value,tmp_addr);
			           rc1 = -EFAULT;
			           pr_err("wait first vsync timeout %d\n",rc1);					   
				       break;
			       }
			   }
			   iowrite32(reg_value,tmp_addr);
			}

			
			//wait the next vsync and record the time
			if(b_first_vsnc)
			{
				//wait 9 vsync
				for(i=0;i<frame_num-2;i++)
				{
				   while(1)
				   {
					   reg_value=ioread32(tmp_addr);
					   if(reg_value&(1<<info.view_no))
					   {
					       #ifdef debug_fps
					       vsync[i]=get_time_ns();
						   #endif
						   iowrite32(reg_value,tmp_addr);
						   break;
					   }
					   iowrite32(reg_value,tmp_addr);
				   }
				}
                int k=0;
			    //wait the 10 vsync and get the deta
    			while(1)
    			{
    			   #ifdef debug_fps
    			   sample[k]=get_time_ns();
				   k++;
				   k=k%100;
				   #endif
    			   reg_value=ioread32(tmp_addr);
    			   if(reg_value&(1<<info.view_no))
    			   {
    			       info.deta_ns_two_vsync= get_time_ns()-info.deta_ns_two_vsync;
					   info.deta_ns_two_vsync=info.deta_ns_two_vsync/(frame_num-1);
    			       iowrite32(reg_value,tmp_addr);
    				   break;
    			   }
    			   iowrite32(reg_value,tmp_addr);
    			}
			}
			//enable the irq
			if(info.local_irq==0)
			{
			    local_irq_restore(flags);
			}else{
			    iowrite32(1,irq_addr);
			}
		    iounmap(tmp_addr);			
		    iounmap(irq_addr);
			rc = copy_to_user((struct ar_drv_info __user *)arg, &info, sizeof(info));
			if (rc) {
					pr_err("of_irq_parser_one rc = %d\n", rc);
					break;
			}
			if(rc1 || rc)
			{
				rc=-EFAULT;
			}
			
            #ifdef debug_fps
            //print the ppm
            int deta=vsync[1]-vsync[0];
			int deta_new=0;
            for(i=1;i<frame_num-3;i++){
				deta_new=vsync[i+1]-vsync[i];
                pr_err("ppm %d vsync:%d \n",(int)((deta_new-deta)*1000000/deta),deta);
			    deta=deta_new;
            }
			int deta_sam=sample[1]-sample[0];
			int deta_new_sam=0;
            for(i=1;i<99;i++){
				deta_new_sam=sample[i+1]-sample[i];
                pr_err("ppm %d sample:%d \n",(int)((deta_new_sam-deta_sam)*1000000/deta_sam),deta_sam);
			    deta_sam=deta_new_sam;
            }
			#endif
        }
		break;
#else       
        case AR_MPP_QUERY_VIF_FPS:
        {
            struct ar_drv_vif_fps_info info;
			rc = -EFAULT;
            int rc1=0;
			int i=0;
			if (copy_from_user(&info, (struct ar_drv_info __user *)arg, sizeof(info))) {
				rc = -EFAULT;
				pr_err("copy_from_user = %d \n", rc);
				break;
			}			
			void __iomem *irq_addr = ioremap(IRQ_REG_BASE, 4);
			void __iomem *tmp_addr = ioremap(0x010A0000 + 0x147c, 4);
			uint32_t reg_value=0;
			unsigned long flags;
			int b_first_vsnc=0;
			int frame_num=info.frame_num;
			if(frame_num<2)
			{
			   frame_num=2;
			}
			
			//diable all the irq
			if(info.local_irq==0)
			{
			   local_irq_save(flags);
			}else{
			   iowrite32(0,irq_addr);
			}
			//clear the current vif irq, if we have 
			reg_value=ioread32(tmp_addr);			
			reg_value=ioread32(tmp_addr);	
			//wait the first vsync and record the time
			unsigned long long start_time=get_time_ns();
			while(1)
			{
			   reg_value=ioread32(tmp_addr);		   
			   if(reg_value&(1<<info.view_no))
			   {
			       info.deta_ns_two_vsync= get_time_ns();				   
				   reg_value=ioread32(tmp_addr);   
				   b_first_vsnc=1;
				   break;
			   }
			   else
			   {
			       uint32_t deata=get_time_ns()-start_time;				   
			       if(deata>100000000)
			       {			       
					   //iowrite32(reg_value,tmp_addr);
			           rc1 = -EFAULT;
			           pr_err("wait first vsync timeout %d\n",rc1);					   
				       break;
			       }
			   }
			   //iowrite32(reg_value,tmp_addr);
			}

			int k=0;
			//wait the next vsync and record the time
			if(b_first_vsnc)
			{
				//wait 9 vsync
				for(i=0;i<frame_num-2;i++)
				{
				   while(1)
				   {
				       
					   sample[k]=get_time_ns();
					   k++;
					   k=k%100;
					   reg_value=ioread32(tmp_addr);		   
					   if(reg_value&(1<<info.view_no))
					   {
					       vsync[i]=get_time_ns();
						   reg_value=ioread32(tmp_addr);   
						   break;
					   }
					   //iowrite32(reg_value,tmp_addr);
				   }
				}

			    //wait the 10 vsync and get the deta
    			while(1)
    			{
				   reg_value=ioread32(tmp_addr);		   
    			   if(reg_value&(1<<info.view_no))
    			   {
    			       info.deta_ns_two_vsync= get_time_ns()-info.deta_ns_two_vsync;
					   info.deta_ns_two_vsync=info.deta_ns_two_vsync/(frame_num-1);
					   reg_value=ioread32(tmp_addr);   
    				   break;
    			   }
    			   //iowrite32(reg_value,tmp_addr);
    			}
			}
			//enable the irq
			if(info.local_irq==0)
			{
			    local_irq_restore(flags);
			}else{
			    iowrite32(1,irq_addr);
			}
		    iounmap(tmp_addr);			
		    iounmap(irq_addr);
			rc = copy_to_user((struct ar_drv_info __user *)arg, &info, sizeof(info));
			if (rc) {
					pr_err("of_irq_parser_one rc = %d\n", rc);
					break;
			}
			if(rc1 || rc)
			{
				rc=-EFAULT;
			}
            //print the ppm
            int deta=vsync[1]-vsync[0];
			int deta_new=0;
            for(i=1;i<frame_num-3;i++){
				deta_new=vsync[i+1]-vsync[i];
                pr_err("ppm %d vsync:%d \n",(int)((deta_new-deta)*1000000/deta),deta);
			    deta=deta_new;
            }
			int deta_sam=sample[1]-sample[0];
			int deta_new_sam=0;
            for(i=1;i<99;i++){
				deta_new_sam=sample[i+1]-sample[i];
                pr_err("ppm %d sample:%d \n",(int)((deta_new_sam-deta_sam)*1000000/deta_sam),deta_sam);
			    deta_sam=deta_new_sam;
            }
        }
		break;
#endif

		case AR_MPP_QUERY_H26X_INFO: {
			int vpu_timeout_us;
			struct ar_drv_h26x_vpu_info info;
			const char *out_string;
			struct device_node *np = p_irq_drv->pdev->of_node;
			struct device_node * child = NULL;
			rc = -EFAULT;

			if (copy_from_user(&info, (struct ar_drv_h26x_vpu_info __user *)arg, sizeof(info))) {
				rc = -EFAULT;
				pr_err("copy_from_user = %d \n", rc);
				break;
			}

			for_each_child_of_node(np, child) {
				if (of_device_is_compatible(child, info.compatible_name)) {
					if (!of_property_read_u32(child, "vpu_timeout_us", &vpu_timeout_us)) {
						info.vpu_timeout_us = vpu_timeout_us;
					}

					if (!of_property_read_string(child, "conf_file_path", &out_string)) {
						strncpy(info.conf_file_path, out_string, strlen(out_string));
					}

					rc = copy_to_user((struct ar_drv_h26x_vpu_info __user *)arg, &info, sizeof(info));
					if (rc) {
						pr_err("of_irq_parser_one rc = %d\n", rc);
						break;
					}

					break;
				}
			}

			break;
		}

		default:
			rc = -EOPNOTSUPP;
			break;
	}

__out:

	return rc;
}


/***********************************************************************
 * platform driver implement:
 ************************************************************************/
static const struct of_device_id ar_mpp_match[] = {
	{.compatible = "artosyn,ar_mpp"},
	{},
};
MODULE_DEVICE_TABLE(of, ar_mpp_match);


static int ar_mpp_irq_probe(struct platform_device *pdev)
{
	return mpp_irq_drv_create(pdev);
}

static int ar_mpp_irq_remove(struct platform_device *pdev)
{
	mpp_irq_drv_destroy(platform_get_drvdata(pdev));

	return 0;
}


static struct platform_driver ar_mpp_irq_platform_driver = {
	.probe    = ar_mpp_irq_probe,
	.remove   = ar_mpp_irq_remove,
	.driver   = {
		.name = "ar mpp",
		.of_match_table = ar_mpp_match,
	},
};

static int __init ar_mpp_irq_init(void)
{
	pr_debug("ar_mpp_init \n");

	return platform_driver_register(&ar_mpp_irq_platform_driver);
}

static void __exit ar_mpp_irq_exit(void)
{
	pr_debug("ar_mpp_exit \n");

	platform_driver_unregister(&ar_mpp_irq_platform_driver);
}

module_init(ar_mpp_irq_init);
module_exit(ar_mpp_irq_exit);

MODULE_AUTHOR("Artosyn");
MODULE_DESCRIPTION("Artosyn linux ");
MODULE_LICENSE("GPL v2");

