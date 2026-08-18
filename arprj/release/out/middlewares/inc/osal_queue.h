/*****************************************************************************
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: ar_queue.h
Description: define a condition-wait queue to process FIFO msg
Author: Artosyn Software Team
Version: v1.0
Date:2021-01-15
History:2021-01-15 : first release sdk
*****************************************************************************/

#ifndef __OSAL_QUEUE_H__
#define __OSAL_QUEUE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "osal_lock.h"
#include "utils_common.h"


typedef void* ar_data_t;
typedef void* ar_queue_id_t;
typedef const void* ar_const_data_t;

//typedef enum {
//  AR_OK                      =  0,         //
//  AR_ERROR                   = -1,         //
//  AR_ERROR_TIMEOUT           = -2,         //
//  AR_ERROR_QUEUE_FULL        = -3,         //
//  AR_ERROR_QUEUE_EMPTY       = -4,         //
//  AR_ERROR_NO_TIME_FUNCTION  = -5,         //
//  AR_ERROR_MAX               = 0x7FFFFFFF  //
//} ar_status_t;

typedef struct {
    const char* name;
    // TODO, add more attributs.
} ar_queue_attr_t;

typedef struct {
    char    name[MAX_LEN];
    uint32_t head;
    uint32_t tail;
    // element_count:   indicate queue ring buffer size.
    // element_count-1: indicate real element count in this queue.
    uint32_t element_count;
    // indicate each element size.
    uint32_t element_size;
    ar_data_t element;
    pthread_mutex_t mutex;
    pthread_cond_t cond_insert, cond_remove;
} ar_cond_queue_t;

// Create a queue which contains element of ar_data_t type. The element
// number is [in] count and element size is [in] size.
ar_queue_id_t ar_queue_create(uint32_t count, uint32_t size, ar_queue_attr_t* attr);

// return queue name.
const char* ar_queue_name(ar_queue_id_t id);

// push back an element to queue. This queue works like a ring buffer, it will add element to a free space. If queue is full, it will wait untill timeout. element pop action can wake up this condition.
ar_status_t ar_queue_push_timeout(ar_queue_id_t id, ar_const_data_t element, uint8_t priority, uint32_t timeout);

// remove an element frome the front of queue. This queue works like a ring buffer, it will remove a element from tail position. If queue is empty, it will wait untill timeout. element insert action can wake up this condition.
ar_status_t ar_queue_pop_timeout(ar_queue_id_t id, ar_data_t element, void* padding, uint32_t timeout);

// return peek element of queue
ar_status_t ar_queue_peek_element(ar_queue_id_t id, ar_data_t element);

void ar_queue_wakeup_receiver(ar_queue_id_t id);

void ar_queue_wakeup_sender(ar_queue_id_t id);

// push back an element to queue. This queue works like a ring buffer, it will add element to a free space. If queue is full, it will return AR_ERROR_QUEUE_FULL directly.
ar_status_t ar_queue_push(ar_queue_id_t id, ar_const_data_t element);

// push back an element to queue. This queue works like a ring buffer, it will add element to a free space. If queue is full, discard the oldest then push the new
ar_status_t ar_queue_push_force(ar_queue_id_t id, ar_const_data_t element, ar_data_t old_element);

// remove an element frome the front of queue. This queue works like a ring buffer, it will remove a element from tail position. If queue is empty, it will return AR_ERROR_QUEUE_EMPTY directly.
ar_status_t ar_queue_pop(ar_queue_id_t id, ar_data_t element);

// check whether queue is full or not.
bool ar_queue_is_full(ar_queue_id_t id);

// check whether queue is empty or not.
bool ar_queue_is_empty(ar_queue_id_t id);

// reset current offset of a queue producer and consumer to 0
ar_status_t ar_queue_reset(ar_queue_id_t id);

// Delete a queue - freeing all the memory allocated for storing of items placed on the queue.
ar_status_t ar_queue_destroy(ar_queue_id_t id);

// return how much memory this queue occupy.
uint32_t ar_queue_capacity(ar_queue_id_t id);

// return element number in queue.
uint32_t ar_queue_element_size(ar_queue_id_t id);

// return un-handled element count in queue
uint32_t ar_queue_element_count(ar_queue_id_t id);

// return how many elements we can push back latter.
uint32_t ar_queue_free_space(ar_queue_id_t id);

// dump queue debug status
void ar_queue_dump_status(ar_queue_id_t id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _AR_QUEU_H__
