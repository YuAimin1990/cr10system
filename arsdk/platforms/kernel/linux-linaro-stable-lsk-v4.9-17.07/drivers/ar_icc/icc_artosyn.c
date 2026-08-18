/*
 * Artosyn Icc driver
 */
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/io.h>
#include "icc_priv.h"

#define ICC_TOKEN_STR_MAXLEN 32
#define ICC_CLIENT_MSG_Q_NUM 32

struct icc_token_id
{
	char token[ICC_TOKEN_STR_MAXLEN];
	unsigned short msgid;
};

struct artosyn_icc_dev {
	struct mutex mutex;
	struct icc_core *icc;
};

static struct icc_token_id g_icc_token_id[ICC_CLIENT_MSG_Q_NUM] = {0};

struct artosyn_icc_dev *artosyn_icc_device = NULL;

void artosyn_wakeup_rx_thread(void)
{
	if (artosyn_icc_device && artosyn_icc_device->icc) {
		/*
	    //spin_lock(&artosyn_icc_device->icc->lock);
	    if(artosyn_icc_device->icc->rcv_tsk)
		    wake_up_process(artosyn_icc_device->icc->rcv_tsk);
	    //spin_unlock(&artosyn_icc_device->icc->lock);
	    */
	    wakeup_icc_shm_poll_rx_thread();
	}
}

static int artosyn_icc_probe(struct platform_device *pdev)
{
	struct artosyn_icc_dev *icc_dev;
 	int ret;

	icc_dev = kzalloc(sizeof(*icc_dev), GFP_KERNEL);
	if(IS_ERR(icc_dev)) {
		return PTR_ERR(icc_dev);
	}

	icc_dev->icc = icc_core_create();
	if(IS_ERR(icc_dev->icc)) {
		return PTR_ERR(icc_dev->icc);
	}

	pdev->dev.platform_data = icc_dev->icc;
	ret = icc_of_mem_init(pdev);
	if(ret)
		goto out;

	ret = icc_vmap_mem(icc_dev->icc);
	if(ret)
		goto out;

	ret = icc_shm_init(icc_dev->icc);
	if(ret)
		goto out;

	ret = icc_of_peer_init(pdev);
	if(ret)
		goto out;

	/* Store global ptr */
	artosyn_icc_device = icc_dev;
	mutex_init(&artosyn_icc_device->mutex);
	return ret;
out:
	kfree(icc_dev);
	return ret;
}

static int artosyn_icc_remove(struct platform_device *pdev)
{
	struct icc_core *icc = pdev->dev.platform_data;
	struct artosyn_icc_dev *icc_dev =
		container_of(&icc, struct artosyn_icc_dev, icc);

	/* unmap shm */
	vunmap(icc->virt_addr);

	icc_core_destroy(icc);

	kfree(icc_dev);
	return 0;
}

static const struct of_device_id artosyn_ion_match_table[] = {
	{.compatible = "artosyn,icc"},
	{},
};

static struct platform_driver artosyn_icc_driver = {
	.probe = artosyn_icc_probe,
	.remove = artosyn_icc_remove,
	.driver = {
		.name = "icc-artosyn",
		.of_match_table = artosyn_ion_match_table,
	},
};

static int __init artosyn_icc_init(void)
{
	return platform_driver_register(&artosyn_icc_driver);
}

static void __exit artosyn_icc_exit(void)
{
	platform_driver_unregister(&artosyn_icc_driver);
}

MODULE_AUTHOR("artosyn");
MODULE_DESCRIPTION("artosyn icc inter-core communication driver");
MODULE_LICENSE("GPL");
subsys_initcall(artosyn_icc_init);
module_exit(artosyn_icc_exit);

/* artosyn icc kernel api */
void *ar_icc_client_create(void)
{
	struct icc_client *client;
	unsigned char name[32] = {0};

	sprintf(name, "%s-%d-%d", current->comm, current->group_leader->pid, current->pid);

	return icc_client_create(artosyn_icc_device->icc, name);
}

void ar_icc_client_destroy(void *client)
{
	int found;

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return;

	icc_client_destroy(client);
}

static unsigned short ar_icc_name_hash(const char * pName)
{
    unsigned short Hash = 0;
    const char *pChar = pName;
    while ((*pChar) != 0 )
    {
        Hash = ( (Hash&1)?0x8000:0)+(Hash>>1)+(*pChar);
        pChar++;
    }
    //debug_print("pName=%s, Hash=0x%x\r\n",pName,Hash);
    return Hash;
}

//robin: here we use a array to save all the token and id so that they can be dumped easily.
unsigned short ar_icc_ftok(const char *path)
{
    unsigned int i;

    for (i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
    {
    	//if this token existed, return the id
    	if (g_icc_token_id[i].token[0])
    	{
    		if(strcmp(g_icc_token_id[i].token, path) == 0)
    		{
    			if(g_icc_token_id[i].msgid)
    			{
    				return g_icc_token_id[i].msgid;
    			}
    			else
    			{
    				printk(KERN_DEBUG"Find duplicated token, but no msgid, will try to create a msgid.\r\n");
    				break;
    			}
    		}
    	}
    	else
        {
            strncpy(g_icc_token_id[i].token, path, ICC_TOKEN_STR_MAXLEN);
            g_icc_token_id[i].token[ICC_TOKEN_STR_MAXLEN - 1] = 0;

            break;
        }
    }

    if(i >= ICC_CLIENT_MSG_Q_NUM)
    {
    	printk(KERN_ERR"ICC token reaches MAX: %d!\r\n", ICC_CLIENT_MSG_Q_NUM);
    	return -1;
    }

    g_icc_token_id[i].msgid = ar_icc_name_hash(path);

    return g_icc_token_id[i].msgid;
}

void ar_icc_dump_token_id(void)
{
	int i = 0;

	for(i = 0; i < ICC_CLIENT_MSG_Q_NUM; i++)
	{
		if(g_icc_token_id[i].token[0])
		{
			printk(KERN_DEBUG"token: %s, msgid: %d\r\n", g_icc_token_id[i].token, g_icc_token_id[i].msgid);
		}
	}
}

int ar_icc_client_register_msgid(void *client, uint32_t msg_id)
{
	int ret, found = 0;

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return -EINVAL;

	return icc_register_msg(client, msg_id);
}

int ar_icc_client_unregister_msgid(void *client, uint32_t msg_id)
{
	int ret, found = 0;

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return -EINVAL;

	return icc_cancle_msg(client, msg_id);
}

int ar_icc_client_set_timeout(void *client, uint32_t timeout)
{
	int found = 0;

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return -EINVAL;

	return icc_set_timeout(client, timeout);
}

int ar_icc_client_send(void *client,
			unsigned char *buf, uint32_t size,
			uint32_t msg_id, uint32_t core_id)
{
	int ret, found = 0;
	struct icc_request request = {0};

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return -EINVAL;

	request.client  = client;
	request.task    = current;
	request.core_id = core_id;
	request.kbuf    = buf;
	request.msg_id  = msg_id;
	request.res     = 0;
	request.size    = size;
	request.flag    |= ICC_REQ_FLAG_TX;

	ret = icc_send(client, &request);
	if(ret < 0)
		return ret;

	return request.res;
}

int ar_icc_client_receive(void *client, unsigned char *buf, uint32_t size, uint32_t msg_id)
{
	int ret, found = 0;
	struct icc_request request = {0};

	found = icc_client_check(artosyn_icc_device->icc, client);
	if(!found)
		return -EINVAL;

	request.client	= client;
	request.task    = current;
	request.msg_id	= msg_id;
	request.size    = size;

	ret = icc_receive(client, &request);
	if(ret < 0)
		return ret;

	if(!request.kbuf)
		return -EINVAL;

	memcpy(buf, request.kbuf, request.size);

	kfree(request.kbuf);

	return request.size;
}

EXPORT_SYMBOL(ar_icc_ftok);
EXPORT_SYMBOL(ar_icc_dump_token_id);
EXPORT_SYMBOL(ar_icc_client_create);
EXPORT_SYMBOL(ar_icc_client_destroy);
EXPORT_SYMBOL(ar_icc_client_register_msgid);
EXPORT_SYMBOL(ar_icc_client_unregister_msgid);
EXPORT_SYMBOL(ar_icc_client_set_timeout);
EXPORT_SYMBOL(ar_icc_client_send);
EXPORT_SYMBOL(ar_icc_client_receive);
