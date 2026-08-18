#undef DEBUG
#define pr_fmt(fmt) "ar-mpp-proc:" fmt

#include "ar_mpp_proc.h"
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <asm/bitops.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/bitmap.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/hashtable.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/freezer.h>

#define MAX_PROC_DEV_NUM				(32)
#define PROC_HASH_BITS					(8)

struct mpp_proc_drv;

// ====================================================================
//
// ======     proc_dev: /dev/ar_mpp_proc
// ======     procfs -> /proc/file
//
// cat procfs:   print msg write from proc_dev; notify userspace update msg by write-callback;
// echo procfs:  update msg to userspace read-callback;
// ====================================================================


struct mpp_procfs
{
	char					procfs_name[MAX_PROC_NAME_LEN];
	u32						prot;
	struct proc_dir_entry	*dentry;
	struct proc_dir_entry	*parent;


	char					*pdata_write_from_procdev;		//通过 /dev/ar_mpp_proc写入的data; cat显示
	s32						buf_len_write_from_procdev;
	s32						data_len_write_from_procdev;

	char					*pdata_write_from_procfs;		//通过 procfs 写入的data, user space读取后, 返回到read callback中
	s32						buf_len_from_procfs;
	s32						data_len_from_procfs;

	struct hlist_node		hnode;
	struct proc_dev			*p_proc_dev;					//pointer to proc_dev which created the procfs

	s32						bitmap_idx;				//

	struct mutex			mutex;
};


struct proc_dev
{
	wait_queue_head_t			wait_q;
	u32							created_procfs_cnt;
	struct mpp_procfs			*created_dentry_procfs[MAX_PROC_HANDLE_NUM];

	struct mpp_procfs			*created_dir_dentry_procfs[MAX_PROC_HANDLE_NUM];

	unsigned long				avail_write_msg_bitmap[MAX_PROC_HANDLE_NUM / (sizeof(long) * 8)];
	unsigned long				avail_read_msg_bitmap[MAX_PROC_HANDLE_NUM / (sizeof(long) * 8)];

	struct mpp_proc_drv			*p_proc_drv;
	int							idx;

	struct mutex				mutex;
};


struct mpp_proc_drv
{
	struct hlist_head			proc_hashtable[1 << PROC_HASH_BITS];

	struct proc_dev				*p_proc_dev[MAX_PROC_DEV_NUM];
	int							proc_dev_cnt;

	struct cdev					cdev;
	dev_t						dev_id;
	struct class				*drv_class;
	struct device				*drv_device;

	unsigned int				major;
	struct mutex				mutex;
};

struct mpp_proc_drv g_ar_proc_drv;

static inline u32 str_hash(const char *s)
{
	u32 h = 0;

	while (*s) {
		h = h * 131313 + *s;
		s ++;
	}

	return h;
}

static int _insert_dir_entry_to_proc_dev(struct proc_dev *p_proc_dev, struct mpp_procfs *entry);

struct mpp_procfs * _find_entry_in_proc_drv(struct mpp_proc_drv *pdrv, const char *procfs_name)
{
	struct hlist_node *next  = NULL;
	struct mpp_procfs *entry = NULL;
	u32 key = str_hash(procfs_name);

	pr_debug("%s: pdrv=%p procfs_name:%s key:0x%x\n", __func__, pdrv, procfs_name, key);

	hash_for_each_possible_safe(pdrv->proc_hashtable, entry, next, hnode, key) {
		if (strcmp(entry->procfs_name, procfs_name) == 0) {
			return entry;
		}
	}

	pr_debug("%s: not exist %s \n", __func__, procfs_name);

	return NULL;
}

struct mpp_procfs * _find_descendants_in_proc_drv(struct mpp_proc_drv *pdrv, const char *procfs_name, u32* family_count)
{
	struct hlist_node *next  = NULL;
	struct mpp_procfs *entry = NULL;
	struct mpp_procfs *parent = NULL;
	u32 count = 0;
	size_t len = strlen(procfs_name);
	u32 key = str_hash(procfs_name);
	u32 bkt;

	hash_for_each_safe(pdrv->proc_hashtable, bkt, next, entry, hnode) {
		pr_debug("%s: entry=%p name:%s\n", __func__, entry, entry->procfs_name);
		if (strcmp(entry->procfs_name, procfs_name) == 0) {
			parent = entry;
			count++;
		} else if(strncmp(entry->procfs_name, procfs_name, len) == 0) {
			count++;
		}
	}

	pr_debug("%s: %s family count %d parent %p\n", __func__, procfs_name, count, parent);

	*family_count = count;
	return parent;
}


static int mpp_procfs_show(struct seq_file *f, void *ptr)
{
	struct mpp_procfs *proc_entry = (struct mpp_procfs *)f->private;
	pr_debug("%s ==>: proc_entry=%p\n", __func__, proc_entry);

	BUG_ON(!proc_entry);

	//add mutex because AR_PROC_WRITE_MSG_TO_PROC_FS may re-allocate the pdata_write_from_procdev
	mutex_lock(&proc_entry->mutex);

	if (proc_entry->pdata_write_from_procdev == NULL) {
		pr_debug("%s: empty data \n", __func__);
	} else {
		seq_printf(f, "%s\n", proc_entry->pdata_write_from_procdev);
	}

	bitmap_set(proc_entry->p_proc_dev->avail_read_msg_bitmap, proc_entry->bitmap_idx, 1);

	wake_up(&proc_entry->p_proc_dev->wait_q);

	mutex_unlock(&proc_entry->mutex);

	pr_debug("%s <==: proc_entry=%p\n", __func__, proc_entry);
	return 0;
}


static int mpp_procfs_open(struct inode *inode, struct file *filp)
{
	int rc;

	struct mpp_procfs *proc_entry = PDE_DATA(inode);

	pr_debug("%s: ==> proc_entry=%p\n", __func__, proc_entry);

	rc = single_open(filp, mpp_procfs_show, proc_entry);

	pr_debug("%s: <== proc_entry =%p rc =%d \n", __func__, proc_entry, rc);
	return rc;
}


static ssize_t mpp_procfs_write(struct file *file,
				const char __user *user_buffer,
				size_t count,
				loff_t *position)
{
	int rc = -1;
	struct mpp_procfs *proc_entry = ((struct seq_file *)file->private_data)->private;
	struct proc_dev *p_proc_dev = proc_entry->p_proc_dev;

	pr_debug("%s: ==> entry:%p user_buffer:%p count:%ld\n", __func__, proc_entry, user_buffer, count);

	BUG_ON(proc_entry == NULL);

	mutex_lock(&proc_entry->mutex);

	if (proc_entry->pdata_write_from_procfs == NULL) {
		proc_entry->pdata_write_from_procfs = kzalloc(count * 2, GFP_KERNEL);
		proc_entry->buf_len_from_procfs = count * 2;
	} else if (proc_entry->buf_len_from_procfs < count) {
		proc_entry->pdata_write_from_procfs = krealloc(proc_entry->pdata_write_from_procfs,  count * 2, GFP_KERNEL);
		proc_entry->buf_len_from_procfs = count * 2;
	}

	if (proc_entry->pdata_write_from_procfs) {
		rc = copy_from_user(proc_entry->pdata_write_from_procfs, user_buffer, count);
		proc_entry->data_len_from_procfs = count;
		if (rc != 0) {
			pr_err("%s: == copy_from_user ret=%d \n", __func__, rc);
		}

		bitmap_set(p_proc_dev->avail_write_msg_bitmap, proc_entry->bitmap_idx, 1);
		mutex_unlock(&proc_entry->mutex);

		wake_up(&p_proc_dev->wait_q);
	} else {
		pr_err("%s: -ENOMEM \n", __func__);

		mutex_unlock(&proc_entry->mutex);
		return -ENOMEM;
	}

	pr_debug("%s: <== write: mpp_procfs=%p, %ld\n", __func__, proc_entry, count);

	return count;
}


static int mpp_procfs_release(struct inode *inode, struct file *file)
{
	pr_debug("%s: \n", __func__);

	return 0;
}

static const struct file_operations mpp_procfs_fops = {
	.open			= mpp_procfs_open,
	.read			= seq_read,
	.write			= mpp_procfs_write,
	.release		= mpp_procfs_release,
};


void _new_procfs_entry(struct mpp_procfs ** entry, char *procfs_name, s32 prot, struct proc_dev *p_proc_dev)
{
	*entry = kzalloc(sizeof(struct mpp_procfs), GFP_KERNEL);
	BUG_ON(*entry == NULL);

	strncpy((*entry)->procfs_name, procfs_name, MAX_PROC_NAME_LEN-1);
	(*entry)->prot = prot;
	(*entry)->p_proc_dev = p_proc_dev;
	(*entry)->bitmap_idx = MAX_PROC_HANDLE_NUM;

	mutex_init(&(*entry)->mutex);
	INIT_HLIST_NODE(&((*entry)->hnode));
}


struct mpp_procfs *_register_create_procfs_file(struct proc_dev *p_proc_dev, char *procfs_name, int prot, void *priv)
{
	struct proc_dir_entry *dentry = NULL;
	struct proc_dir_entry *parent = NULL, *pre_parent = NULL;
	char fullpath[MAX_PROC_NAME_LEN] = {0};
	struct mpp_procfs *proc_entry = NULL;
	struct mpp_proc_drv *p_proc_drv = p_proc_dev->p_proc_drv;

	char *next = kstrdup(procfs_name, GFP_KERNEL);
	if(!next) {
		pr_debug("%s: kstrdup %s failed\n", __func__, procfs_name);
		return NULL;
	}

	char *next_start = next;
	char *cur  = strsep(&next, "/");

	mutex_lock(&p_proc_drv->mutex);

	pr_debug("%s: procfs_name:%s\n", __func__, procfs_name);
	if ((proc_entry = _find_entry_in_proc_drv(p_proc_drv, procfs_name)) != NULL) {
		mutex_unlock(&p_proc_drv->mutex);
		kfree(next_start);
		return proc_entry;
	}

	while (next != NULL) {
		pr_debug("%s %d %s %s %s %s\n", __func__, __LINE__, procfs_name, fullpath, next, cur);

		strcat(fullpath, cur);
		proc_entry = _find_entry_in_proc_drv(p_proc_drv, fullpath);

		if (proc_entry) {
			parent = proc_entry->dentry;
			pr_debug("get %s entry:%p next:%s cur:%s\n", fullpath, proc_entry, next, cur);
		} else {
			//create dir
			parent = proc_mkdir(cur, parent);
			BUG_ON(!parent);

			_new_procfs_entry(&proc_entry, fullpath, 0, p_proc_dev);
			proc_entry->dentry = parent;
			proc_entry->parent = pre_parent;

			_insert_dir_entry_to_proc_dev(p_proc_dev, proc_entry);

			//mutex_lock(&p_proc_drv->mutex);
			hash_add(p_proc_drv->proc_hashtable, &proc_entry->hnode, str_hash(fullpath));
			//mutex_unlock(&p_proc_drv->mutex);

			pre_parent = parent;
		}

        strcat(fullpath, "/");
		cur = strsep(&next, "/");
	}

	_new_procfs_entry(&proc_entry, procfs_name, prot, p_proc_dev);
	pr_debug("%s %d %s %s\n", __func__, __LINE__, cur, procfs_name);
	dentry = proc_create_data(cur, prot, parent, &mpp_procfs_fops, proc_entry);
	BUG_ON(!dentry);

	pr_debug("procfs_name:%s dentry:%p parent:%p\n", procfs_name, dentry, parent);

	proc_entry->dentry = dentry;
	proc_entry->parent = parent;
	_insert_dir_entry_to_proc_dev(p_proc_dev, proc_entry);

	//mutex_lock(&p_proc_drv->mutex);
	hash_add(p_proc_drv->proc_hashtable, &proc_entry->hnode, str_hash(procfs_name));
	//mutex_unlock(&p_proc_drv->mutex);

	kfree(next_start);

	pr_debug("create entry: %p \n", proc_entry);
	mutex_unlock(&p_proc_drv->mutex);

	return proc_entry;
}

static bool _is_valid_proc_entry(struct proc_dev *p_proc_dev, struct mpp_procfs* proc_entry, bool* exist_in_ht)
{
	struct mpp_procfs *proc_entry_tmp = NULL;

	if(!proc_entry)
	{
		pr_debug("invalid proc handle %p \n", proc_entry);
		return false;
	}

	if(proc_entry->bitmap_idx < 0 || proc_entry->bitmap_idx >= MAX_PROC_HANDLE_NUM)
	{
		pr_debug("invalid proc handle idx %d \n", proc_entry->bitmap_idx);
		return false;
	}

	if(proc_entry->p_proc_dev != p_proc_dev)
	{
		pr_debug("mismatch dev %p != %p \n", p_proc_dev, proc_entry->p_proc_dev);
		return false;
	}

	//mutex_lock(&p_proc_dev->p_proc_drv->mutex);
	proc_entry_tmp = _find_entry_in_proc_drv(p_proc_dev->p_proc_drv, proc_entry->procfs_name);
	//mutex_unlock(&p_proc_dev->p_proc_drv->mutex);
	if(proc_entry_tmp != proc_entry) {
		if(!proc_entry_tmp)
			*exist_in_ht = false;
		else
			*exist_in_ht = true;
		pr_debug("found proc entry %p not match %p \n", proc_entry_tmp, proc_entry);
		return false;
	}

	return true;
}

static int _remove_procfs_file(struct proc_dev *p_proc_dev, struct mpp_procfs* proc_entry)
{
	pr_debug("%s %d proc_entry:%p procfs_name:%s\n", __func__, __LINE__, proc_entry, proc_entry->procfs_name);

	mutex_lock(&p_proc_dev->mutex);	//need add mutex, race condition: _register_create_procfs_file
	hash_del(&proc_entry->hnode);				//delete from p_proc_drv

	if (proc_entry->pdata_write_from_procdev) {
		kfree(proc_entry->pdata_write_from_procdev);
		proc_entry->pdata_write_from_procdev = NULL;
	}

	if (proc_entry->pdata_write_from_procfs) {
		kfree(proc_entry->pdata_write_from_procfs);
		proc_entry->pdata_write_from_procfs = NULL;
	}

	proc_remove(proc_entry->dentry);
	proc_entry->procfs_name[0] = 0;
	proc_entry->p_proc_dev = NULL;
	proc_entry->dentry = NULL;
	proc_entry->parent = NULL;

	kfree(proc_entry);
	mutex_unlock(&p_proc_dev->mutex);

	return 0;
}

static int _insert_entry_to_proc_dev(struct proc_dev *p_proc_dev, struct mpp_procfs *entry)
{
	s32 idx = 0;
	int rc = 0;

	pr_debug("%s ==> : p_proc_dev=%p entry=%p %s\n", __func__, p_proc_dev, entry, entry->procfs_name);
	mutex_lock(&p_proc_dev->mutex);

	if(entry->bitmap_idx >= 0 && entry->bitmap_idx < MAX_PROC_HANDLE_NUM) {
		if(p_proc_dev->created_dentry_procfs[entry->bitmap_idx] == entry) {
			pr_debug("%p %s already created\n", entry, entry->procfs_name);
		} else {
			pr_debug("%p %s already created, but idx wrong with %p %s\n",
					entry,
					entry->procfs_name,
					p_proc_dev->created_dentry_procfs[entry->bitmap_idx],
					p_proc_dev->created_dentry_procfs[entry->bitmap_idx]->procfs_name);
		}
	} else {
		for (idx = 0; idx < MAX_PROC_HANDLE_NUM; idx ++) {
			if (p_proc_dev->created_dentry_procfs[idx] == NULL) {
				p_proc_dev->created_dentry_procfs[idx] = entry;
				p_proc_dev->created_procfs_cnt ++;

				entry->bitmap_idx = idx;
				break;
			}
		}

		if (idx >= MAX_PROC_HANDLE_NUM) {
			rc = -EINVAL;
		}
	}

	mutex_unlock(&p_proc_dev->mutex);
	pr_debug("%s <== : p_proc_dev=%p entry=%p idx=%d rc=%d\n", __func__, p_proc_dev, entry, idx, rc);

	return rc;
}


static int _insert_dir_entry_to_proc_dev(struct proc_dev *p_proc_dev, struct mpp_procfs *entry)
{
	s32 idx;
	int rc = 0;

	pr_debug("%s ==> : p_proc_dev=%p entry=%p %s\n", __func__, p_proc_dev, entry, entry->procfs_name);
	//mutex_lock(&p_proc_dev->mutex);

	for (idx = 0; idx < MAX_PROC_HANDLE_NUM; idx ++) {
		if (p_proc_dev->created_dir_dentry_procfs[idx] == NULL) {
			p_proc_dev->created_dir_dentry_procfs[idx] = entry;
			break;
		}
	}

	if (idx >= MAX_PROC_HANDLE_NUM) {
		rc = -EINVAL;
	}

	//mutex_unlock(&p_proc_dev->mutex);
	pr_debug("%s <== : p_proc_dev=%p entry=%p idx=%d rc=%d\n", __func__, p_proc_dev, entry, idx, rc);

	return rc;
}

static int _remove_entry_from_proc_dev(struct proc_dev *p_proc_dev, struct mpp_procfs *entry)
{
	s32 idx = 0;
	int rc = 0;

	pr_debug("%s ==> : p_proc_dev=%p entry=%p idx=%d\n", __func__, p_proc_dev, entry, entry->bitmap_idx);
	mutex_lock(&p_proc_dev->mutex);

	idx = entry->bitmap_idx;

	if (idx >= MAX_PROC_HANDLE_NUM) {
		rc = -EINVAL;
	} else if (p_proc_dev->created_dentry_procfs[idx] != entry) {
		rc = -EINVAL;
	} else {
		if (p_proc_dev->created_dentry_procfs[idx] == entry) {
			p_proc_dev->created_dentry_procfs[idx] = NULL;
			p_proc_dev->created_procfs_cnt --;
			entry->bitmap_idx = MAX_PROC_HANDLE_NUM;
		}

		// also cleanup dir table. no care parent
		for (idx = 0; idx < MAX_PROC_HANDLE_NUM; idx ++) {
			if (p_proc_dev->created_dir_dentry_procfs[idx] == entry) {
				p_proc_dev->created_dir_dentry_procfs[idx] = NULL;
				break;
			}
		}
	}

	mutex_unlock(&p_proc_dev->mutex);
	pr_debug("%s <== : p_proc_dev=%p entry=%p idx=%d rc=%d\n", __func__, p_proc_dev, entry, idx, rc);

	return rc;
}

static int _remove_dir_entry_from_proc_dev(struct proc_dev *p_proc_dev, struct mpp_procfs *entry)
{
	s32 idx = 0;
	int rc = 0;

	pr_debug("%s ==> : p_proc_dev=%p entry=%p idx=%d\n", __func__, p_proc_dev, entry, entry->bitmap_idx);
	mutex_lock(&p_proc_dev->mutex);

	for (idx = 0; idx < MAX_PROC_HANDLE_NUM; idx ++) {
		if (p_proc_dev->created_dir_dentry_procfs[idx] == entry) {
			p_proc_dev->created_dir_dentry_procfs[idx] = NULL;
			break;
		}
	}

	if (idx >= MAX_PROC_HANDLE_NUM) {
		rc = -EINVAL;
	}

	mutex_unlock(&p_proc_dev->mutex);
	pr_debug("%s <== : p_proc_dev=%p entry=%p idx=%d rc=%d\n", __func__, p_proc_dev, entry, idx, rc);

	return rc;
}

static int _chain_remove_procfs_file(struct proc_dev *p_proc_dev, struct mpp_procfs* proc_entry)
{
	int rc = 0;
	u32 count = 0;
	struct mpp_procfs* entry = proc_entry;
	struct mpp_procfs* parent = NULL;
	struct mpp_proc_drv *p_proc_drv = p_proc_dev->p_proc_drv;

	char *next = kstrdup(entry->procfs_name, GFP_KERNEL);
	char *last = NULL;

	do {
		if(entry->bitmap_idx < MAX_PROC_HANDLE_NUM) {
			rc = _remove_entry_from_proc_dev(p_proc_dev, entry);
			if(rc) {
				pr_debug("%s _remove_entry_from_proc_dev failed %d\n", __func__, rc);
				break;
			}
		} else {
			rc = _remove_dir_entry_from_proc_dev(p_proc_dev, entry);
			if(rc) {
				pr_debug("%s _remove_entry_from_proc_dev failed %d\n", __func__, rc);
				break;
			}
		}

		_remove_procfs_file(p_proc_dev, entry);

		last = strrchr(next, '/');
		pr_debug("%s %d %s %s\n", __func__, __LINE__, next, last);
		if(!last)
			break;
		*last = 0;
		parent = _find_descendants_in_proc_drv(p_proc_drv, next, &count);
		if(count > 1)
			break;
		entry = parent;
	} while(entry);

	if(next)
		kfree(next);

	return rc;
}

static int ar_mpp_procdev_open(struct inode *inode, struct file *filp)
{
	int i;
	struct mpp_proc_drv *p_proc_drv = container_of(inode->i_cdev, struct mpp_proc_drv, cdev);

	pr_debug("==> %s: p_proc_drv = %p\n", __func__, p_proc_drv);

	mutex_lock(&p_proc_drv->mutex);

	for (i = 0; i < MAX_PROC_DEV_NUM; i++) {
		if (p_proc_drv->p_proc_dev[i] == NULL) {
			p_proc_drv->p_proc_dev[i] = kzalloc(sizeof(*p_proc_drv->p_proc_dev[i]), GFP_KERNEL);
			if (IS_ERR(p_proc_drv->p_proc_dev[i])) {
				pr_debug("<== %s open fail: kzalloc \n", __func__);
				mutex_unlock(&p_proc_drv->mutex);
				return -ENOMEM;
			}
			mutex_init(&p_proc_drv->p_proc_dev[i]->mutex);

			p_proc_drv->p_proc_dev[i]->idx = i;
			init_waitqueue_head(&p_proc_drv->p_proc_dev[i]->wait_q);
			p_proc_drv->p_proc_dev[i]->p_proc_drv = p_proc_drv;
			p_proc_drv->proc_dev_cnt ++;

			break;
		}
	}

	if (i == MAX_PROC_DEV_NUM) {
		pr_debug("<== %s open fail: overflow \n", __func__);
		mutex_unlock(&p_proc_drv->mutex);
		return (-EMFILE);
	}

	filp->private_data = p_proc_drv->p_proc_dev[i];
	mutex_unlock(&p_proc_drv->mutex);

	pr_debug("<== %s idx=%d p_proc_dev=%p\n", __func__, i, p_proc_drv->p_proc_dev[i]);
	return 0;
}

static int ar_mpp_procdev_flush(struct file* filp, fl_owner_t id)
{
	int i, idx;
	u32 procfs_cnt = 0;

	struct proc_dev *p_proc_dev = filp->private_data;
	struct mpp_proc_drv *p_proc_drv = p_proc_dev->p_proc_drv;
	struct mpp_procfs *entry = NULL;

	pr_debug("==> %s: %d p_proc_drv = %p %p\n", __func__, __LINE__, p_proc_dev, id);

	idx = p_proc_dev->idx;

	mutex_lock(&p_proc_dev->mutex);
	for (i = 0; i < MAX_PROC_HANDLE_NUM; i ++) {
		entry = p_proc_dev->created_dentry_procfs[i];
		if(entry) {
			procfs_cnt++;
		}
	}

	if(procfs_cnt != p_proc_dev->created_procfs_cnt) {
		pr_err("%s %d: procfs_cnt=%d created_procfs_cnt=%d\n", __func__, __LINE__, procfs_cnt, p_proc_dev->created_procfs_cnt);
	}
	mutex_unlock(&p_proc_dev->mutex);

	mutex_lock(&p_proc_drv->mutex);
	if(p_proc_dev->created_procfs_cnt == 0) {
		wake_up(&p_proc_dev->wait_q);
		if(p_proc_drv->p_proc_dev[idx]) {
			p_proc_drv->proc_dev_cnt--;
			p_proc_drv->p_proc_dev[idx] = NULL;
		}
	}
	mutex_unlock(&p_proc_drv->mutex);

	pr_debug("<== %s: %d idx=%d p_proc_dev = %p \n", __func__, __LINE__, idx, p_proc_dev);

	return 0;
}

static int ar_mpp_procdev_close(struct inode *inode, struct file *filp)
{
	int i, idx, ret;

	struct proc_dev *p_proc_dev = filp->private_data;
	struct mpp_proc_drv *p_proc_drv = p_proc_dev->p_proc_drv;
	struct mpp_procfs *entry = NULL;

	pr_debug("==> %s %d:  p_proc_drv = %p\n", __func__, __LINE__, p_proc_drv);

	mutex_lock(&p_proc_drv->mutex);
	idx = p_proc_dev->idx;

	for (i = 0; i < MAX_PROC_HANDLE_NUM; i ++) {
		entry = p_proc_dev->created_dentry_procfs[i];
		if(entry) {
			pr_warn("%s %d: %s not closed by user\n", __func__, __LINE__, entry->procfs_name);
			ret = _chain_remove_procfs_file(p_proc_dev, entry);
		}
	}

//	mutex_lock(&p_proc_dev->mutex);
	p_proc_dev->created_procfs_cnt = 0;
	wake_up(&p_proc_dev->wait_q);
//	mutex_unlock(&p_proc_dev->mutex);

	if(p_proc_drv->p_proc_dev[idx]) {
		p_proc_drv->proc_dev_cnt--;
		p_proc_drv->p_proc_dev[idx] = NULL;
	}

	kfree(p_proc_dev);
	mutex_unlock(&p_proc_drv->mutex);

	pr_debug("<== %s: %d idx=%d p_proc_dev = %p \n", __func__, __LINE__, idx, p_proc_dev);

	return 0;
}

static long ar_mpp_procdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rc = 0;
	struct proc_dev *p_proc_dev = filp->private_data;

	pr_debug("%s %x\n", __func__, cmd);

	switch (cmd) {
		case AR_PROC_REQUEST_CREATE: {
			AR_PROC_REQUEST_CREATE_T create_req;

			pr_debug("==> AR_PROC_REQUEST_CREATE \n");

			rc = copy_from_user(&create_req, (const void __user *)arg, sizeof(AR_PROC_REQUEST_CREATE_T));
			if (rc == 0) {
				struct mpp_procfs * proc_entry = _register_create_procfs_file(p_proc_dev, create_req.name,
												create_req.prot, filp->private_data);
				if(!proc_entry)
				{
					rc = -ENFILE;
					goto __out;
				}
				//insert last entry to proc_dev created_dentry_procfs entry list
				if (create_req.flag_subscribe)
					rc = _insert_entry_to_proc_dev(p_proc_dev, proc_entry);
				//duplciated create from user, return existed entry
				put_user((AR_PROC_HANDLE)proc_entry, &((AR_PROC_REQUEST_CREATE_T *)arg)->hdl);

				pr_debug("<== AR_PROC_REQUEST_CREATE line=%d entry=%p rc=%d\n", __LINE__, proc_entry, rc);
				break;
			} else {
				pr_debug("<== AR_PROC_REQUEST_CREATE line=%d rc=%d\n", __LINE__, rc);
				break; //AR_PROC_REQUEST_CREATE
			}
		}
		break; //AR_PROC_REQUEST_CREATE

		case AR_PROC_REQUEST_CLOSE: {
			AR_PROC_REQUEST_CLOSE_T close_req;

			pr_debug("==> AR_PROC_REQUEST_CLOSE \n");

			rc = copy_from_user(&close_req, (const void __user *)arg, sizeof(AR_PROC_REQUEST_CLOSE_T));
			if (rc == 0) {
				struct mpp_procfs* proc_entry = (struct mpp_procfs*)(close_req.hdl);
				bool exist = false;

				if(!_is_valid_proc_entry(p_proc_dev, proc_entry, &exist))
				{
					if(exist)
						rc = -EINVAL;
					goto __out;
				}

				rc = _chain_remove_procfs_file(p_proc_dev, proc_entry);
			}
			pr_debug("<== AR_PROC_REQUEST_CLOSE line=%d rc=%d\n", __LINE__, rc);
		}
		break; //AR_PROC_REQUEST_CLOSE

		case AR_PROC_REQUEST_MSG_FROM_PROC_DEV: {
			AR_PROC_QUERY_EVENT_T event;
			AR_PROC_QUERY_EVENT_T * __user_event = (AR_PROC_QUERY_EVENT_T *)arg;

			pr_debug("==> AR_PROC_REQUEST_MSG_FROM_PROC_DEV \n");
			rc = copy_from_user(&event, __user_event, sizeof(event));
			if (rc) {
				pr_err("%d: __user_event=%p rc=%d\n", __LINE__, __user_event, rc);
				goto __out;
			}

			pr_debug("%p bitmap wr-0: 0x%lx 1:0x%lx, rd-0: 0x%lx 1:0x%lx\n", p_proc_dev, p_proc_dev->avail_write_msg_bitmap[0],
												  p_proc_dev->avail_write_msg_bitmap[1],
												  p_proc_dev->avail_read_msg_bitmap[0],
												  p_proc_dev->avail_read_msg_bitmap[1]);
			rc = wait_event_freezable(p_proc_dev->wait_q,
							 (!bitmap_empty(p_proc_dev->avail_write_msg_bitmap, MAX_PROC_HANDLE_NUM)) ||
							 (!bitmap_empty(p_proc_dev->avail_read_msg_bitmap, MAX_PROC_HANDLE_NUM)) ||
							 (!p_proc_dev->created_procfs_cnt));
			if (rc != 0) {
				pr_info("%s: wait_event_freezable p_proc_dev->wait_q = %p rc = %d \n", __func__, (void *)&p_proc_dev->wait_q, rc);
				rc = -ERESTARTSYS;
			} else {
				unsigned long bit_n;

				if(!p_proc_dev->created_procfs_cnt) {
					pr_debug("%s: created_procfs_cnt is 0\n", __func__);
					goto __out;
				}

				mutex_lock(&p_proc_dev->mutex);

				bit_n = find_first_bit(p_proc_dev->avail_write_msg_bitmap, MAX_PROC_HANDLE_NUM);
				if (bit_n < MAX_PROC_HANDLE_NUM) {
					int copy_size = min(p_proc_dev->created_dentry_procfs[bit_n]->data_len_from_procfs, event.msg_size);

					rc |= copy_to_user(__user_event->msg, p_proc_dev->created_dentry_procfs[bit_n]->pdata_write_from_procfs, copy_size);
					rc |= put_user(copy_size, &__user_event->msg_size);
					rc |= put_user((AR_PROC_HANDLE)p_proc_dev->created_dentry_procfs[bit_n], &__user_event->hdl);
					rc |= put_user(1, &__user_event->flag_write);
					if (rc != 0) {
						pr_err("%s: copy_to_user rc = %d \n", __func__, rc);
					}

					bitmap_clear(p_proc_dev->avail_write_msg_bitmap, bit_n, 1);
					pr_debug("%p bitmap wr-0:0x%lx 1:0x%lx bit_n=%ld\n", p_proc_dev, p_proc_dev->avail_write_msg_bitmap[0],
											p_proc_dev->avail_write_msg_bitmap[1], bit_n);

				} else {
					bit_n = find_first_bit(p_proc_dev->avail_read_msg_bitmap, MAX_PROC_HANDLE_NUM);
					if (bit_n < MAX_PROC_HANDLE_NUM) {
						rc |= put_user((AR_PROC_HANDLE)p_proc_dev->created_dentry_procfs[bit_n], &__user_event->hdl);
						rc |= put_user(1, &__user_event->flag_read);
						if (rc != 0) {
							pr_err("%s: copy_to_user rc = %d \n", __func__, rc);
						}
						bitmap_clear(p_proc_dev->avail_read_msg_bitmap, bit_n, 1);
						pr_debug("%p bitmap rd-0:0x%lx 1:0x%lx bit_n=%ld\n", p_proc_dev, p_proc_dev->avail_read_msg_bitmap[0],
												p_proc_dev->avail_read_msg_bitmap[1], bit_n);
					} else {
						BUG();
					}
				}

				mutex_unlock(&p_proc_dev->mutex);
			}
			pr_debug("<== AR_PROC_REQUEST_MSG_FROM_PROC_DEV \n");
		}
		break; //AR_PROC_REQUEST_MSG_FROM_PROC_DEV

		case AR_PROC_WRITE_MSG_TO_PROC_FS: {
			AR_PROC_REQUEST_WRITE_T write_request;
			AR_PROC_REQUEST_WRITE_T * __user_write_msg = (AR_PROC_REQUEST_WRITE_T * )arg;
			struct mpp_procfs *proc_entry;
			bool exist = false;

			pr_debug("==> AR_PROC_WRITE_MSG_TO_PROC_FS \n");
			rc |= get_user(write_request.msg_size, &__user_write_msg->msg_size);
			rc |= get_user(write_request.hdl, &__user_write_msg->hdl);
			if (rc) {
				pr_err("get_user from %p: rc = 0x%x \n", __user_write_msg, rc);
				goto __out;
			}

			proc_entry = (struct mpp_procfs *)write_request.hdl;

			if(!_is_valid_proc_entry(p_proc_dev, proc_entry, &exist))
			{
				rc = -EINVAL;
				goto __out;
			}

			if(p_proc_dev->created_dentry_procfs[proc_entry->bitmap_idx] != proc_entry)
			{
			   pr_err("invalid proc handle %p \n", proc_entry);
			   rc = -EINVAL;
			   goto __out;
			}

			mutex_lock(&proc_entry->mutex);

			if (proc_entry->pdata_write_from_procdev == NULL) {
				proc_entry->pdata_write_from_procdev = kmalloc(write_request.msg_size * 2, GFP_KERNEL | __GFP_ZERO);
				proc_entry->buf_len_write_from_procdev = write_request.msg_size * 2;
			} else if (proc_entry->buf_len_write_from_procdev <= write_request.msg_size) {
				proc_entry->pdata_write_from_procdev = krealloc(proc_entry->pdata_write_from_procdev,  write_request.msg_size * 2, GFP_KERNEL | __GFP_ZERO);
				proc_entry->buf_len_write_from_procdev = write_request.msg_size * 2;
			}

			if (proc_entry->pdata_write_from_procdev) {
				if ((rc = copy_from_user(proc_entry->pdata_write_from_procdev, __user_write_msg->msg, write_request.msg_size)) != 0) {

					mutex_unlock(&proc_entry->mutex);
					goto __out;
				} else {
					*(proc_entry->pdata_write_from_procdev + write_request.msg_size) = 0;
					pr_debug(":::: get msg proc_entry = %p pdata_write_from_procdev = %s \n", proc_entry, proc_entry->pdata_write_from_procdev);
				}
			} else {
				pr_err("%s:%d -ENOMEM \n", __func__, __LINE__);
				rc = -ENOMEM;
				mutex_unlock(&proc_entry->mutex);
				goto __out;
			}

			mutex_unlock(&proc_entry->mutex);
			pr_debug("<== AR_PROC_WRITE_MSG_TO_PROC_FS \n");
		}
		break; //AR_PROC_WRITE_MSG_TO_PROC_FS

		default:
			rc = -EOPNOTSUPP;
			break;
	}

__out:
	return rc;
}


static struct file_operations ar_mpp_proc_fops = {
	.owner					= THIS_MODULE,
	.open					= ar_mpp_procdev_open,
	.flush					= ar_mpp_procdev_flush,
	.release				= ar_mpp_procdev_close,
	.unlocked_ioctl			= ar_mpp_procdev_ioctl,
#if defined(CONFIG_COMPAT)
	.compat_ioctl			= ar_mpp_procdev_ioctl,
#endif
};


int __init ar_mpp_proc_init(void)
{
	int rc = 0;
	struct mpp_proc_drv *p_proc_drv = &g_ar_proc_drv;

	pr_debug("%s: %p\n", __func__, p_proc_drv);

	memset(p_proc_drv, 0, sizeof(struct mpp_proc_drv));

	mutex_init(&p_proc_drv->mutex);
	hash_init(p_proc_drv->proc_hashtable);

	/*
	 * drvier init
	*/
	if (alloc_chrdev_region(&p_proc_drv->dev_id, 0, 64, AR_MPP_PROC_DEVICE_NAME)) {
		pr_err("unable to allocate chrdev region\n");
		return -EFAULT;
	}

	p_proc_drv->drv_class = class_create(THIS_MODULE, AR_MPP_PROC_DEVICE_NAME);
	if (IS_ERR(p_proc_drv->drv_class)) {
		pr_err("class_create failed for adf_ctl\n");
		rc = PTR_ERR(p_proc_drv->drv_class);
		goto err_chrdev_unreg;
	}

	p_proc_drv->major = MAJOR(p_proc_drv->dev_id);

	cdev_init(&p_proc_drv->cdev, &ar_mpp_proc_fops);
	p_proc_drv->cdev.owner = THIS_MODULE;
	if ((rc = cdev_add(&p_proc_drv->cdev, p_proc_drv->dev_id, MAX_PROC_DEV_NUM)) != 0) {
		pr_err("cdev add failed\n");
		goto err_class_destr;
	}

	p_proc_drv->drv_device = device_create(p_proc_drv->drv_class, NULL,
						MKDEV(p_proc_drv->major, 0),
						NULL, AR_MPP_PROC_DEVICE_NAME);
	if (IS_ERR(p_proc_drv->drv_device)) {
		pr_err("failed to create device\n");
		rc = PTR_ERR(p_proc_drv->drv_device);
		goto err_cdev_del;
	}

	pr_debug("ar_mpp_procdev_init done\n");
	return 0;

err_cdev_del:
	cdev_del(&p_proc_drv->cdev);

err_class_destr:
	class_destroy(p_proc_drv->drv_class);

err_chrdev_unreg:
	unregister_chrdev_region(p_proc_drv->dev_id, MAX_PROC_DEV_NUM);

	return rc;
}

void __exit ar_mpp_proc_exit(void)
{
	struct mpp_proc_drv *p_proc_drv = &g_ar_proc_drv;

	pr_debug("%s >>\n", __func__);

	cdev_del(&p_proc_drv->cdev);

	device_destroy(p_proc_drv->drv_class, MKDEV(p_proc_drv->major, 0));

	class_destroy(p_proc_drv->drv_class);

	unregister_chrdev_region(MKDEV(p_proc_drv->major, 0), MAX_PROC_DEV_NUM);

	pr_debug("%s <<\n", __func__);
}

module_init(ar_mpp_proc_init);
module_exit(ar_mpp_proc_exit);

MODULE_AUTHOR("Artosyn");
MODULE_DESCRIPTION("ar mpp proc driver");
MODULE_LICENSE("GPL v2");


