/*
 * Artosyn ION Driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) "Ion: " fmt

#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/scatterlist.h>
#include <linux/artosyn_ion.h>
#include <linux/sched.h>
#include <linux/compat.h>
#include <linux/sizes.h>
#include <linux/smp.h>
#include <asm/cputype.h>
#include <asm/topology.h>
#include <asm/cacheflush.h>
#include <asm/cpu.h>

#include "../ion_priv.h"
#include "../ion.h"
#include "../ion_of.h"

struct artosyn_ion_dev {
	struct ion_heap	**heaps;
	struct ion_device *idev;
	struct ion_platform_data *data;
};

static struct artosyn_ion_dev *artosyn_ion_device = NULL;

static struct ion_of_heap artosyn_heaps[] = {
	/* System heap */
	PLATFORM_HEAP("artosyn,system",
		      0,
		      ION_HEAP_TYPE_SYSTEM,
		      "system_heap"),

	/* Carveout heap */
	PLATFORM_HEAP("artosyn,carveout-dsp",
				  24,
				  ION_HEAP_TYPE_CARVEOUT,
				  "carveout-dsp"),

	PLATFORM_HEAP("artosyn,carveout-m7",
				  25,
				  ION_HEAP_TYPE_CARVEOUT,
				  "carveout-m7"),

	PLATFORM_HEAP("artosyn,carveout-icc",
				  26,
				  ION_HEAP_TYPE_CARVEOUT,
				  "carveout-icc"),

	PLATFORM_HEAP("artosyn,carveout-linux-sys-heap",
				  27,
				  ION_HEAP_TYPE_CARVEOUT,
				  "carveout-linux-sys-heap"),

	{}
};

static int artosyn_ion_probe(struct platform_device *pdev)
{
	struct artosyn_ion_dev *ipdev;
	int i;

	ipdev = devm_kzalloc(&pdev->dev, sizeof(*ipdev), GFP_KERNEL);
	if (!ipdev)
		return -ENOMEM;

	platform_set_drvdata(pdev, ipdev);

	ipdev->idev = ion_device_create(NULL);
	if (IS_ERR(ipdev->idev))
		return PTR_ERR(ipdev->idev);

	ipdev->data = ion_parse_dt(pdev, artosyn_heaps);
	if (IS_ERR(ipdev->data))
		return PTR_ERR(ipdev->data);

	ipdev->heaps = devm_kzalloc(&pdev->dev,
				sizeof(struct ion_heap) * ipdev->data->nr,
				GFP_KERNEL);
	if (!ipdev->heaps) {
		ion_destroy_platform_data(ipdev->data);
		return -ENOMEM;
	}

	for (i = 0; i < ipdev->data->nr; i++) {
		ipdev->heaps[i] = ion_heap_create(&pdev->dev, &ipdev->data->heaps[i]);
		if (!ipdev->heaps) {
			ion_destroy_platform_data(ipdev->data);
			return -ENOMEM;
		}
		ion_device_add_heap(ipdev->idev, ipdev->heaps[i]);
	}

	artosyn_ion_device = ipdev;

	return 0;
}

static int artosyn_ion_remove(struct platform_device *pdev)
{
	struct artosyn_ion_dev *ipdev;
	int i;

	ipdev = platform_get_drvdata(pdev);

	for (i = 0; i < ipdev->data->nr; i++)
		ion_heap_destroy(ipdev->heaps[i]);

	ion_destroy_platform_data(ipdev->data);
	ion_device_destroy(ipdev->idev);

	return 0;
}

static const struct of_device_id artosyn_ion_match_table[] = {
	{.compatible = "artosyn,ion"},
	{},
};

static struct platform_driver artosyn_ion_driver = {
	.probe = artosyn_ion_probe,
	.remove = artosyn_ion_remove,
	.driver = {
		.name = "ion-artosyn",
		.of_match_table = artosyn_ion_match_table,
	},
};

static int __init artosyn_ion_init(void)
{
	return platform_driver_register(&artosyn_ion_driver);
}

subsys_initcall(artosyn_ion_init);

/* Artosyn kernel interface */
struct artosyn_ion_client {
	struct list_head list;
	struct ion_client *client;
};

static LIST_HEAD(artosyn_ion_client_list);

static DEFINE_MUTEX(artosyn_ion_mutex);

int artosyn_client_create(unsigned char *client_name)
{
	struct artosyn_ion_client *arto_client;
	struct ion_client *client;

	mutex_lock(&artosyn_ion_mutex);

	/* Client already exits */
	list_for_each_entry(arto_client, &artosyn_ion_client_list, list) {
		if(strcmp(arto_client->client->name, client_name) == 0) {
			mutex_unlock(&artosyn_ion_mutex);
			return -EINVAL;
		}
	}

	arto_client = kzalloc(sizeof(*arto_client), GFP_KERNEL);
	if(!arto_client) {
		mutex_unlock(&artosyn_ion_mutex);
		return -ENOMEM;
	}

	client = ion_client_create(artosyn_ion_device->idev, client_name);
	if(IS_ERR(client)) {
		mutex_unlock(&artosyn_ion_mutex);
		return PTR_ERR(client);
	}

	arto_client->client = client;
	list_add(&arto_client->list, &artosyn_ion_client_list);

	mutex_unlock(&artosyn_ion_mutex);
	return 0;
}
EXPORT_SYMBOL(artosyn_client_create);

int artosyn_alloc_buffer(unsigned char *client_name, size_t len, size_t align,
			int heap_id_mask, unsigned int flags, void **va, void **pa)
{
	struct artosyn_ion_client *arto_client;
	struct ion_handle *handle = NULL;
	struct ion_buffer *buffer;
	void *vaddr;
	struct page *page;
	int found = 0;

	mutex_lock(&artosyn_ion_mutex);

	list_for_each_entry(arto_client, &artosyn_ion_client_list, list) {
		if(strcmp(arto_client->client->name, client_name) == 0) {
			found = 1;
			break;
		}
	}
	if(!found) {
		mutex_unlock(&artosyn_ion_mutex);
		return -EINVAL;
	}

	handle = ion_alloc(arto_client->client, len, align, heap_id_mask, flags);
	if(IS_ERR(handle)) {
		mutex_unlock(&artosyn_ion_mutex);
		return PTR_ERR(handle);
	}

	buffer = handle->buffer;

	vaddr = ion_map_kernel(arto_client->client, handle);
	if(IS_ERR(vaddr)) {
		mutex_unlock(&artosyn_ion_mutex);
		return PTR_ERR(vaddr);
	}

	buffer->vaddr = vaddr;

	*va = vaddr;
	if(buffer->sg_table->nents == 1) {
		page = sg_page(buffer->sg_table->sgl);
		*pa = (void *)page_to_phys(page);
	}
	else
		*pa = 0;

	mutex_unlock(&artosyn_ion_mutex);
	return handle->id;
}
EXPORT_SYMBOL(artosyn_alloc_buffer);

int artosyn_free_buffer(unsigned char *client_name, int handle_id)
{
	struct artosyn_ion_client *arto_client;
	struct ion_handle *handle = NULL;
	struct ion_buffer *buffer = NULL;
	int found = 0;

	mutex_lock(&artosyn_ion_mutex);

	list_for_each_entry(arto_client, &artosyn_ion_client_list, list) {
		if(strcmp(arto_client->client->name, client_name) == 0) {
			found = 1;
			break;
		}
	}
	if(!found) {
		mutex_unlock(&artosyn_ion_mutex);
		return -EINVAL;
	}

	mutex_lock(&arto_client->client->lock);

	handle = ion_handle_get_by_id_nolock(arto_client->client, handle_id);
	if (IS_ERR(handle)) {
		mutex_unlock(&arto_client->client->lock);
		mutex_unlock(&artosyn_ion_mutex);
		return PTR_ERR(handle);
	}
	buffer = handle->buffer;
	ion_free_nolock(arto_client->client, handle);

	mutex_unlock(&arto_client->client->lock);
	mutex_unlock(&artosyn_ion_mutex);
	return 0;
}
EXPORT_SYMBOL(artosyn_free_buffer);

int artosyn_destroy_client(unsigned char *client_name)
{
	struct artosyn_ion_client *arto_client;
	int found = 0;

	mutex_lock(&artosyn_ion_mutex);

	list_for_each_entry(arto_client, &artosyn_ion_client_list, list) {
		if(strcmp(arto_client->client->name, client_name) == 0) {
			found = 1;
			break;
		}
	}
	if(!found) {
		mutex_unlock(&artosyn_ion_mutex);
		return -EINVAL;
	}
	ion_client_destroy(arto_client->client);
	list_del(&arto_client->list);
	kfree(arto_client);
	mutex_unlock(&artosyn_ion_mutex);
	return 0;
}
EXPORT_SYMBOL(artosyn_destroy_client);


/*static void artosyn_ion_flush_cache_all(void *dummy)
{
	flush_cache_all();
}

void ion_flush_all_cpus_caches(void)
{
	int cpu;
	cpumask_t mask;

	preempt_disable();

	cpumask_clear(&mask);
	for_each_online_cpu(cpu) {
		cpumask_set_cpu(cpu, &mask);
	}

	on_each_cpu_mask(&mask, artosyn_ion_flush_cache_all, NULL, 1);

	preempt_enable();
}*/

