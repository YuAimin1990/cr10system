/*****************************************************************************
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: osal_memory_pool.h
Description: define a memory pool with several equal size buffe
Author: Artosyn Software Team
Version: v1.0
Date:2021-03-04
History:2021-03-04 : first release sdk
*****************************************************************************/

#ifndef __OSAL_MEMORY_POOL_H__
#define __OSAL_MEMORY_POOL_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef void* ar_memory_pool_id_t;

typedef struct {
    const char* name;
    // TODO, add more attributs.
} ar_memory_pool_attr_t;

typedef struct {
    char name[MAX_LEN];
    // buffer count in memory pool
    uint32_t count;
    // each buffer size
    uint32_t size;
    // index used to indicate buffer used or not
    bool* index;
    // memory pool buffer base address
    void* base;
    pthread_mutex_t mutex;
} ar_memory_pool_t;

// Create a memory pool with buffers of equal size.
// count: buffer count
// size:  buffer size
// attr:  memory pool attr
ar_memory_pool_id_t ar_memory_pool_create(uint32_t count, uint32_t size, ar_memory_pool_attr_t* attr);

// delete memory pool, free all buffers in the pool.
ar_status_t ar_memory_pool_destroy(ar_memory_pool_id_t id);

// get a buffer of pre-defined size from memory pool
void* ar_memory_pool_acquire_buffer(ar_memory_pool_id_t id);

// release buffer to memory pool buffer
ar_status_t ar_memory_pool_release_buffer(ar_memory_pool_id_t id, void* buffer);

// return how much memory in memory pool.
uint32_t ar_memory_pool_capacity(ar_memory_pool_id_t id);

// return total buffer number in memory pool.
uint32_t ar_memory_pool_count(ar_memory_pool_id_t id);

// return how many free buffers in memory pool.
uint32_t ar_memory_pool_free_count(ar_memory_pool_id_t id);

// return how many busy buffers in memory pool.
uint32_t ar_memory_pool_busy_count(ar_memory_pool_id_t id);

// return nth busy buffer in memory pool.
void* ar_memory_pool_nth_busy_buffer(ar_memory_pool_id_t id, int n);

#ifdef __cplusplus
}
#endif
#endif // __OSAL_MEMORY_POOL_H__
