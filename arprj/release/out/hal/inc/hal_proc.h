#ifndef __AR_PROC_H__
#define __AR_PROC_H__

#ifdef	__cplusplus
extern "C"
{
#endif

#include "hal_type.h"

#ifdef CONFIG_BIT_32BITS
typedef uint64_t AR_PROC_HANDLE;
#else
typedef void* AR_PROC_HANDLE;
#endif

typedef void (*read_callback)(void *priv);

typedef void (*write_callback)(char *pmsg, int size, void *priv);


typedef struct
{
	char		*name;
	read_callback	read_cb;
	write_callback	write_cb;
	unsigned int	prot;
	AR_VOID		*priv;
} STRU_USER_PROC;

AR_S32 ar_proc_init(void);

AR_PROC_HANDLE ar_proc_create(STRU_USER_PROC *p_proc);

/*
 * 写入pbuf信息到proc节点, 写入长度为size
 *
*/
AR_S32 ar_proc_write(AR_PROC_HANDLE handle, const char *pbuf, AR_S32 size);

AR_S32 ar_proc_close(AR_PROC_HANDLE handle);

AR_S32 ar_proc_exit(void);

void *ar_proc_get_private(AR_PROC_HANDLE handle);

#ifdef	__cplusplus
}
#endif

#endif


