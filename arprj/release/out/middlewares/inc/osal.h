/*****************************************************************************
Copyright: 2021-2025, Artosyn. Co., Ltd.
File name: osal.h
Description: The osal APIs.
Author: Artosyn Software Team
Version: 0.0.1
Date: 2021/04/21
History:
        0.0.1    2021/04/21    The initial version of osal.h
*****************************************************************************/

#ifndef __OSAL_H__
#define __OSAL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include "osal_def.h"

void my_bt(void);
void* ar_malloc(size_t size);
void  ar_free(void *addr);
//void* ar_dma_malloc(size_t size);
//void  ar_dma_free(void * addr);
//void* ar_dma_malloc_cacheable(size_t size);
//void* ar_dma_va_to_pa(void * va);

//lock api
ar_lock_t ar_creat_lock();
void ar_delete_lock(ar_lock_t lock);//notice:must be called when exit, other wise will lead to memory leak;
int ar_lock(ar_lock_t lock);
int ar_lock_timeout(ar_lock_t lock, uint32_t timeout_ms);
int ar_unlock(ar_lock_t lock);

//semaphore api
// need confirm, defination is same as signal
ar_os_semaphore_id_t ar_creat_lock_semaphore(uint32_t max_count, uint32_t initial_count, void *attr);
ar_os_semaphore_id_t ar_create_binary_semaphore();

int ar_delete_lock_semaphore(ar_os_semaphore_id_t lock_id); //notice:must be called when kmod exit, other wise will lead to memory leak;
int ar_lock_semaphore(ar_os_semaphore_id_t lock_id, uint32_t timeout_ms);
int ar_lock_semaphore_forever(ar_os_semaphore_id_t lock_id);
int ar_unlock_semaphore(ar_os_semaphore_id_t lock_id);

//signal semaphore api
ar_signal_t ar_create_signal();
int ar_delete_signal(ar_signal_t signal);

int ar_signal_wait(ar_signal_t signal);
int ar_signal_wait_timeout(ar_signal_t signal, uint32_t timeout_ms);

int ar_signal(ar_signal_t signal);
int ar_signal_irq(ar_signal_t signal, long *woken);
int ar_irq_trig_schelue(long woken);

//signal binary api
ar_signal_t ar_create_signal_binary();
int ar_delete_signal_binary(ar_signal_t signal);

int ar_signal_wait_binary(ar_signal_t signal);
int ar_signal_wait_timeout_binary(ar_signal_t signal, uint32_t timeout_ms);

int ar_signal_binary(ar_signal_t signal);
int ar_signal_irq_binary(ar_signal_t signal, long *woken);
int ar_irq_trig_schelue_binary(long woken);

//timer api
void ar_delay(uint32_t ms);
uint64_t ar_get_timestamp();
uint64_t ar_get_timestamp_us();
void ar_delay_us(uint64_t us);

ar_os_timer_id_t ar_os_timer_create(ar_os_time_func_t func, void *ptr, void *attr);
int ar_os_timer_start(ar_os_timer_id_t id, uint32_t interval, uint32_t timeout);
int ar_os_timer_stop(ar_os_timer_id_t id);
int ar_os_timer_delete(ar_os_timer_id_t id);

//message queue api
ar_os_msg_queue_id_t ar_osal_msg_queue_new(uint32_t msg_count, uint32_t msg_size, ar_os_msg_queue_attr_t* attr);
const char * ar_osal_msg_queue_get_name(ar_os_msg_queue_id_t mq_id);
ar_os_status_t ar_osal_msg_queue_put(ar_os_msg_queue_id_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
ar_os_status_t ar_osal_msg_queue_get(ar_os_msg_queue_id_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
ar_os_status_t ar_osal_msg_queue_peek(ar_os_msg_queue_id_t mq_id, void *msg_ptr);
uint32_t ar_osal_msg_queue_get_capacity(ar_os_msg_queue_id_t mq_id);
uint32_t ar_osal_msg_queue_get_msg_size(ar_os_msg_queue_id_t mq_id);
uint32_t ar_osal_msg_queue_get_count(ar_os_msg_queue_id_t mq_id);
uint32_t ar_osal_msg_queue_get_space(ar_os_msg_queue_id_t mq_id);
ar_os_status_t ar_osal_msg_queue_reset(ar_os_msg_queue_id_t mq_id);
void ar_osal_msg_queue_wakeup_sender(ar_os_msg_queue_id_t mq_id);
void ar_osal_msg_queue_wakeup_receiver(ar_os_msg_queue_id_t mq_id);
ar_os_status_t ar_osal_msg_queue_delete(ar_os_msg_queue_id_t mq_id);

// thread api
ar_os_thread_id_t ar_osal_thread_new(ar_os_thread_func_t func, void *argument, ar_os_thread_attr_t *attr);
const char * ar_osal_thread_get_name(ar_os_thread_id_t thread_id);
ar_os_thread_id_t ar_osal_thread_get_id(void);
ar_os_thread_state_t ar_osal_thread_get_state(ar_os_thread_id_t thread_id);
uint32_t ar_osal_thread_get_stack_size(ar_os_thread_id_t thread_id);
uint32_t ar_osal_thread_get_stack_space(ar_os_thread_id_t thread_id);
ar_os_status_t ar_osal_thread_set_priority(ar_os_thread_id_t thread_id, ar_os_priority_t priority);
ar_os_priority_t ar_osal_thread_get_priority(ar_os_thread_id_t thread_id);
ar_os_status_t ar_osal_thread_yield();
ar_os_status_t ar_osal_thread_suspend(ar_os_thread_id_t thread_id);
ar_os_status_t ar_osal_thread_resume(ar_os_thread_id_t thread_id);
ar_os_status_t ar_osal_thread_detach(ar_os_thread_id_t thread_id);
ar_os_status_t ar_osal_thread_join(ar_os_thread_id_t thread_id);
void ar_osal_thread_exit();
ar_os_status_t ar_osal_thread_terminate(ar_os_thread_id_t thread_id);
uint32_t ar_osal_thread_get_count();
uint32_t ar_osal_thread_enumerate(ar_os_thread_id_t *thread_array, uint32_t array_items);
uint32_t ar_osal_thread_flags_set(ar_os_thread_id_t thread_id, uint32_t flags);
uint32_t ar_osal_thread_clear(uint32_t flags);
uint32_t ar_osal_thread_flags_get();
uint32_t ar_osal_thread_flags_wait(uint32_t flags, uint32_t options, uint32_t timeout);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // __OSAL_H__
