/*
 *	AR mpp driver defines
 */

#ifndef _AR_MPP_PROC_H
#define _AR_MPP_PROC_H

#include <linux/ioctl.h>
#include <linux/types.h>
/*
 *	AR mpp proc defines
 */
#define AR_MPP_IOCTL_MAGIC		'M'

#define MAX_PROC_HANDLE_NUM		(192)

#define MAX_PROC_NAME_LEN		(128)
#define MAX_PROC_MSG_SIZE		(256)

#ifdef CONFIG_BIT_32BITS
typedef uint64_t AR_PROC_HANDLE;
#else
typedef void * AR_PROC_HANDLE;
#endif

#define AR_MPP_PROC_DEVICE_NAME		"ar_mpp_proc_ctl"

typedef struct
{
	char					name[MAX_PROC_NAME_LEN];
	unsigned int			prot;
	unsigned int			flag_subscribe;
	AR_PROC_HANDLE			hdl;							//output: get kernel proc handler
} AR_PROC_REQUEST_CREATE_T;


typedef struct
{
	char					*msg;
	int						msg_size;
	AR_PROC_HANDLE			hdl;
} AR_PROC_REQUEST_WRITE_T;


typedef struct
{
	char					msg[MAX_PROC_MSG_SIZE];
	int						msg_size;
	int						flag_write;		//userspace write message to proc fs
	int						flag_read;		//userspace read proc fs
	AR_PROC_HANDLE			hdl;
} AR_PROC_QUERY_EVENT_T;

typedef struct
{
	AR_PROC_HANDLE			hdl;
} AR_PROC_REQUEST_CLOSE_T;

#define AR_PROC_REQUEST_CREATE				_IOWR(AR_MPP_IOCTL_MAGIC, 6, AR_PROC_REQUEST_CREATE_T)
#define AR_PROC_REQUEST_MSG_FROM_PROC_DEV	_IOR(AR_MPP_IOCTL_MAGIC, 7, AR_PROC_QUERY_EVENT_T)
#define AR_PROC_WRITE_MSG_TO_PROC_FS		_IOR(AR_MPP_IOCTL_MAGIC, 8, AR_PROC_REQUEST_WRITE_T)
#define AR_PROC_REQUEST_CLOSE				_IOW(AR_MPP_IOCTL_MAGIC, 9, AR_PROC_REQUEST_CLOSE_T)

#define AR_PROC_REQUEST_MSG_FROM_PROC_FS   AR_PROC_REQUEST_MSG_FROM_PROC_DEV

#endif /* _AR_MPP_PROC_H */

