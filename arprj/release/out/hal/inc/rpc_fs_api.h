#ifndef __RPC_FS_API__
#define __RPC_FS_API__
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "osal.h"
#include "util.h"
#include "utils_list.h"
#include "ar_system.h"
#include "v4ar_dev.h"

#define MSG_MASK_COMMON_CTL (1<<0)
#define MSG_MASK_EVENT (1<<1)
#define MSG_MASK_QBUF (1<<2)
#define MSG_MASK_DQBUF (1<<3)

// rpc_fs will not provide protect for each file
// operations, the called should implement self.
typedef struct {
    int fd;
    int is_proxy;
    ar_lock_t lock;
    void *priv;
    void *rpc_fs;
    void *rpc_fs_dev;
    void *pollhandle;
    int dev_is_unregister;
    struct ar_list_head link;
} rpc_file_t;

typedef int(*rpc_fs_open)(rpc_file_t *file);
typedef int(*rpc_fs_close)(rpc_file_t *file);
typedef int(*rpc_fs_read)(rpc_file_t *file, void *data, int len);
typedef int(*rpc_fs_write)(rpc_file_t *file, const void *data, int len);
typedef int(*rpc_fs_ctl)(rpc_file_t *file, int code, void *data, int len);
typedef int(*rpc_fs_poll)(rpc_file_t *file, unsigned *revents);

typedef struct {
    rpc_fs_open  open;
    rpc_fs_close close;
    rpc_fs_read  read;
    rpc_fs_write write;
    rpc_fs_poll  poll;
    rpc_fs_ctl   ctl;
} rpc_fs_dev_ops_t;

void   rpc_fs_init();
void   rpc_fs_deinit();
void   rpc_fs_notify(rpc_file_t *file);
void*  get_dev_data(rpc_file_t *file);
int    set_dev_data(rpc_file_t *file, void *data);
int    clear_dev_data_by_name(char *name);
void*  get_dev_data_by_name(char *name);
void*  get_dev_by_name(char *name);
void*  get_dev_data_by_dev(void *dev);
struct ar_list_head* get_dev_filelist_by_name(char *name);
int    rpc_fs_register_dev(char *name, rpc_fs_dev_ops_t *ops, void *data);
int    rpc_fs_register_dev_sync(char *name, rpc_fs_dev_ops_t *ops, void *data);
int    rpc_fs_register_dev_with_mask(char *name, rpc_fs_dev_ops_t *ops, void *data, int mask);
int    rpc_fs_unregister_dev(char *name);
#endif
