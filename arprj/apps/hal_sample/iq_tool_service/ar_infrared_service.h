#ifndef __AR_INFRARED_SERVICE_H__
#define __AR_INFRARED_SERVICE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <pthread.h>

typedef int (*socket_send)(void *client, uint8_t *play_load, uint32_t len, uint8_t msg_id, uint8_t chksum_flag);

typedef struct
{
		//for net communacation used
		void *client;
		unsigned char module_id;
		socket_send send_func;
} infrared_server_t;

/*
typedef enum
{
		VENC_REQ_ERR_SUCCESS = 0,
		VENC_ERR_TIME_OUT,
		VENC_ERR_UNKNOW,
} infrared_code_t;
*/

typedef struct
{
		int cmd_id;
		int len;
} infrared_request_header_t;

typedef struct
{
   int request_id;//cmd id
   int ret_code;//ret_code_t
   int len;//payload - header
} infrared_ack_header_t;

int process_infrared_tuning_func(void *client, unsigned char *msg, uint32_t len);
int set_infrared_tuning_send_func(socket_send func, void *client, uint8_t msg_id);
int clear_infrared_tuning_send_func(void * client);
#endif
