#ifndef __AR_DVFS_H__
#define __AR_DVFS_H__
#include <linux/of.h>
#include <asm/io.h>

enum {
	AR_DVFS_DEV_ARMLINUX,
	AR_DVFS_DEV_ARMRTOS,
	AR_DVFS_DEV_ARMM,
	AR_DVFS_DEV_CEVA,
	DVFS_PROCESSORS,
};

struct ar_dvfs_processor {
	unsigned char *name;
	int valid;
	int (*get_load)(struct ar_dvfs_processor *processor);
	int (*get_freq)(struct ar_dvfs_processor *processor);
	int (*get_volt)(struct ar_dvfs_processor *processor);
	/*
	 * freq, 10MHz
	 */
	int (*set_freq)(struct ar_dvfs_processor *processor, unsigned int freq);
	int (*set_volt)(struct ar_dvfs_processor *processor);
	void *priv;
};

int ar_dvfs_init_ceva(struct device_node *node, struct ar_dvfs_processor *processor);

#endif
