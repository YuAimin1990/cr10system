/***************************************************************************** 
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: osal_lock.h
Description: Linux mutex wrapper
Author: Artosyn Software Team
Version: v1.0
Date:2021-01-21
History:2021-01-21 : first release sdk
*****************************************************************************/

#ifndef __OSAL_LOCK_H__
#define __OSAL_LOCK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

typedef void* ar_lock_id_t;
typedef void* ar_signal_id_t;
typedef void* ar_sem_id_t;
typedef void* ar_sem_id_t2;

typedef struct {
    pthread_mutex_t mutex;
} ar_mutex_lock_t;

typedef struct {
    volatile int sem_cnt;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ar_sem_t;

typedef struct {
    pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile int flag;
} ar_sem_binary;

ar_lock_id_t ar_mutex_init();

void ar_mutex_destroy(ar_lock_id_t id);

void ar_mutex_lock(ar_lock_id_t id);

int ar_mutex_trylock(ar_lock_id_t id);

void ar_mutex_timeout(ar_lock_id_t id, uint32_t timeout_ms);

void ar_mutex_unlock(ar_lock_id_t id);

// semaphore
ar_sem_id_t ar_sem_init(int shared, int initial_count, void* attr);

ar_sem_id_t ar_binary_sem_init();

int ar_sem_destroy(ar_sem_id_t id);

int ar_sem_lock(ar_sem_id_t id, uint32_t timeout_ms);

int ar_sem_lock_forever(ar_sem_id_t id);

int ar_sem_unlock(ar_sem_id_t id);

ar_sem_id_t ar_sem_init_binary();

int ar_sem_destroy_binary(ar_sem_id_t2 id);

int ar_sem_lock_binary(ar_sem_id_t2 id, uint32_t timeout_ms);

int ar_sem_lock_forever_binary(ar_sem_id_t2 id);

int ar_sem_unlock_binary(ar_sem_id_t2 id);

int ar_sem_signal_irq_binary(ar_sem_id_t2 id, long* woken);


int ar_sem_signal_irq(ar_sem_id_t id, long* woken);

int ar_sem_irq_trig_schelue(long woken);

int ar_sem_timeout_wait(ar_sem_t* context, const struct timespec *abs_timeout);
#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif //__OSAL_LOCK_H__

