#ifndef __RPC_FS_H__
#define __RPC_FS_H__
#include "rpc_fs_api.h"
#include "osal.h"
#include "utils_list.h"
#include "hal_proc.h"

#define MAX_RPC_FS_FILE_COUNT (256*8)
#define MAX_RPC_FS_DEV_COUNT 256
#define MAX_RPC_NAME_LEN 64
#define BITS_PER_BYTE_TYPE    8
#define BITS_PER_LONG_TYPE    (BITS_PER_BYTE_TYPE * sizeof(long))

typedef struct {
    bool init;
    int open_count;
    int close_count;
    int write_count;
    int read_count;
    int poll_count;
    int notify_count;
    int ioctl_in_count;
    int ioctl_out_count;
} dev_state_info;

typedef struct {
    bool dev_created;
    char name[MAX_RPC_NAME_LEN];
    int dev_minor;
    void *dev_data;
    void *rpc_fs;
    void *context;
    ar_lock_t lock;
    rpc_fs_dev_ops_t ops;
    dev_state_info state;

    struct ar_list_head link;
    struct ar_list_head file_list;
} rpc_fs_dev_t;

typedef struct {
    rpc_file_t *file[MAX_RPC_FS_FILE_COUNT];
    ar_lock_t lock;
    ar_signal_t signal;
    ar_os_thread_id_t register_tid;
    pthread_t cuse_worker_tid;
    STRU_USER_PROC proc;
    AR_PROC_HANDLE handle;
    unsigned long file_id_bitmap[MAX_RPC_FS_FILE_COUNT/BITS_PER_LONG_TYPE];
    unsigned long dev_id_bitmap[MAX_RPC_FS_DEV_COUNT/BITS_PER_LONG_TYPE];
    bool inited;

    struct ar_list_head install_list;
    struct ar_list_head dev_list;
} rpc_fs_t;

#endif
