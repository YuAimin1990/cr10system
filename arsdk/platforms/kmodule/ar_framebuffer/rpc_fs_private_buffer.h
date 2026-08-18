#ifndef RPC_FS_PRIVATE_BUFFER_H
#define RPC_FS_PRIVATE_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif
//following is included in v4ar_buffer_t(v4ar_dev.h) which size limitation is 2M
#define V4AR_PRIVATE_BUFFER_SIZE 40
typedef struct
{
	int type;
	int len;
	char data[V4AR_PRIVATE_BUFFER_SIZE];
}v4ar_private_buffer_info_t;

#ifdef __cplusplus
}
#endif

#endif

