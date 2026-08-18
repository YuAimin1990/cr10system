#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/oom.h>
#include <asm/memory.h>
#include <asm/io.h>
#include <linux/kdebug.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/profile.h>


#define VIF_HW_IRQ (106)
#define VIF_REG_BASE			(0x01100000)
#define VIF_CTL_OFFSET			(0xaf << 2)
#define MPP_SERVICE_NAME		"mpp_service.app"

static int ar_panic(struct notifier_block *self, unsigned long cmd, void *ptr)
{
	//for arm64, ioremap prot is PROT_DEVICE_nGnRnE, same with ioremap_nocache
	void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_CTL_OFFSET, 4);
	writel(0x01, tmp_addr);
	iounmap(tmp_addr);

	pr_emerg("%s %d:closed vif!\n", __func__, __LINE__);

	return NOTIFY_DONE;
}

static int ar_panic_die(struct notifier_block *self, unsigned long cmd, void *ptr)
{
	//for arm64, ioremap prot is PROT_DEVICE_nGnRnE, same with ioremap_nocache
	void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_CTL_OFFSET, 4);
	writel(0x01, tmp_addr);
	iounmap(tmp_addr);

	pr_emerg("%s %d:closed vif!\n", __func__, __LINE__);

	return NOTIFY_DONE;
}

/*static int ar_panic_oom(struct notifier_block *self, unsigned long cmd, void *ptr)
{
	//for arm64, ioremap prot is PROT_DEVICE_nGnRnE, same with ioremap_nocache
	void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_CTL_OFFSET, 4);
	writel(0x01, tmp_addr);
	iounmap(tmp_addr);

	pr_emerg("%s %d:closed vif!\n", __func__, __LINE__);

	return NOTIFY_DONE;
}*/

static int ar_panic_exit(struct notifier_block *self, unsigned long val, void *data)
{
	struct task_struct * info = data;

	if(0 == strcmp(MPP_SERVICE_NAME, info->comm))
	{
		//for arm64, ioremap prot is PROT_DEVICE_nGnRnE, same with ioremap_nocache
		void __iomem *tmp_addr = ioremap(VIF_REG_BASE + VIF_CTL_OFFSET, 4);
		writel(0x01, tmp_addr);
		iounmap(tmp_addr);
		pr_emerg("%s %d:closed vif! %s\n", __func__, __LINE__, info->comm);
	}

	return 0;
}

static struct notifier_block ar_panic_blk = {
	.notifier_call = ar_panic,
};

static struct notifier_block ar_die_blk = {
	.notifier_call = ar_panic_die,
};

/*static struct notifier_block ar_oom_blk = {
	.notifier_call = ar_panic_oom,
};*/

static struct notifier_block ar_exit_blk = {
	.notifier_call = ar_panic_exit,
};

int __init ar_panic_init(void)
{
	int ret = 0;
	ret = atomic_notifier_chain_register(&panic_notifier_list, &ar_panic_blk);
	if(0 == ret) {
		printk(KERN_INFO "atomic_notifier_chain_register success!\n");
	}
	else {
		printk(KERN_ERR "atomic_notifier_chain_register failed!\n");
		return -1;
	}

	ret = register_die_notifier(&ar_die_blk);
	if(0 == ret) {
		printk(KERN_INFO "register_die_notifier success!\n");
	}
	else {
		printk(KERN_ERR "register_die_notifier failed!\n");
		goto err0;
	}

	/*ret = register_oom_notifier(&ar_oom_blk);
	if(0 == ret) {
		printk(KERN_INFO "register_oom_notifier sucess!\n");
	}
	else {
		printk(KERN_ERR "register_oom_notifier failed!\n");
		goto err1;
	}*/

	ret = profile_event_register(PROFILE_TASK_EXIT, &ar_exit_blk);
	if(0 == ret) {
		printk(KERN_INFO "profile_event_register sucess!\n");
	}
	else {
		printk(KERN_ERR "profile_event_register failed! %d\n", ret);
		goto err2;
	}

	printk(KERN_INFO "ar panic: startup!\n");

	return ret;

err0:
	atomic_notifier_chain_unregister(&panic_notifier_list, &ar_panic_blk);

//err1:
	unregister_die_notifier(&ar_die_blk);

err2:
	//unregister_oom_notifier(&ar_oom_blk);

	return -1;
}
module_init(ar_panic_init);

MODULE_AUTHOR("Artosyn");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
