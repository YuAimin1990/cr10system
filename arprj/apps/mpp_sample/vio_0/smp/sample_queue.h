#pragma once
#ifdef __cplusplus
extern "C"
{
#endif /* End of #ifdef __cplusplus */

#include "hal_sys.h"
#include "mpi_sysctl.h"

typedef  struct __STRU_AR_QUEUE_T STRU_AR_QUEUE_T;

struct __STRU_AR_QUEUE_T
{
	char * name;
	uint32_t queue_size;
	uint32_t valid_data_num;
	//queue data
	void **data;
	uint32_t header;
	uint32_t tail;
	//lock
	ar_lock_t lock;
	int (*queue_pop)(STRU_AR_QUEUE_T * queue, void **item);
	int (*queue_insert)(STRU_AR_QUEUE_T * queue,void *item);
	int (*get_queue_size)(STRU_AR_QUEUE_T * queue);
	int (*look_up_head)(STRU_AR_QUEUE_T * queue, void **item);
};

//define a queue
int queue_pop(STRU_AR_QUEUE_T *queue, void **item);
int get_queue_size(STRU_AR_QUEUE_T *queue);
int look_up_head_item(STRU_AR_QUEUE_T *queue, void **item);
int queue_insert(STRU_AR_QUEUE_T *queue, void *item);
STRU_AR_QUEUE_T *ar_creat_queue(uint32_t queue_size, char *name);
int ar_delete_queue(STRU_AR_QUEUE_T **queue);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */
