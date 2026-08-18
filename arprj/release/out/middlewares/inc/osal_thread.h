/*****************************************************************************
Copyright: 2016-2021, Artosyn. Co., Ltd.
File name: ar_thread.h
Description: wrap linux pthread
Author: Artosyn Software Team
Version: v1.0
Date:2021-01-19
History:2021-01-19 : first release sdk
*****************************************************************************/

#ifndef __OSAL_THREAD_H__
#define __OSAL_THREAD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include <pthread.h>
#include "utils_common.h"
#include "osal_lock.h"

typedef void*     ar_thread_id_t;
/// Priority values.
typedef enum {
  osPriorityNone          =  0,         ///< No priority (not initialized).
  osPriorityIdle          =  1,         ///< Reserved for Idle thread.
  osPriorityLow           =  8,         ///< Priority: low
  osPriorityLow1          =  8+1,       ///< Priority: low + 1
  osPriorityLow2          =  8+2,       ///< Priority: low + 2
  osPriorityLow3          =  8+3,       ///< Priority: low + 3
  osPriorityLow4          =  8+4,       ///< Priority: low + 4
  osPriorityLow5          =  8+5,       ///< Priority: low + 5
  osPriorityLow6          =  8+6,       ///< Priority: low + 6
  osPriorityLow7          =  8+7,       ///< Priority: low + 7
  osPriorityBelowNormal   = 16,         ///< Priority: below normal
  osPriorityBelowNormal1  = 16+1,       ///< Priority: below normal + 1
  osPriorityBelowNormal2  = 16+2,       ///< Priority: below normal + 2
  osPriorityBelowNormal3  = 16+3,       ///< Priority: below normal + 3
  osPriorityBelowNormal4  = 16+4,       ///< Priority: below normal + 4
  osPriorityBelowNormal5  = 16+5,       ///< Priority: below normal + 5
  osPriorityBelowNormal6  = 16+6,       ///< Priority: below normal + 6
  osPriorityBelowNormal7  = 16+7,       ///< Priority: below normal + 7
  osPriorityNormal        = 24,         ///< Priority: normal
  osPriorityNormal1       = 24+1,       ///< Priority: normal + 1
  osPriorityNormal2       = 24+2,       ///< Priority: normal + 2
  osPriorityNormal3       = 24+3,       ///< Priority: normal + 3
  osPriorityNormal4       = 24+4,       ///< Priority: normal + 4
  osPriorityNormal5       = 24+5,       ///< Priority: normal + 5
  osPriorityNormal6       = 24+6,       ///< Priority: normal + 6
  osPriorityNormal7       = 24+7,       ///< Priority: normal + 7
  osPriorityAboveNormal   = 32,         ///< Priority: above normal
  osPriorityAboveNormal1  = 32+1,       ///< Priority: above normal + 1
  osPriorityAboveNormal2  = 32+2,       ///< Priority: above normal + 2
  osPriorityAboveNormal3  = 32+3,       ///< Priority: above normal + 3
  osPriorityAboveNormal4  = 32+4,       ///< Priority: above normal + 4
  osPriorityAboveNormal5  = 32+5,       ///< Priority: above normal + 5
  osPriorityAboveNormal6  = 32+6,       ///< Priority: above normal + 6
  osPriorityAboveNormal7  = 32+7,       ///< Priority: above normal + 7
  osPriorityHigh          = 40,         ///< Priority: high
  osPriorityHigh1         = 40+1,       ///< Priority: high + 1
  osPriorityHigh2         = 40+2,       ///< Priority: high + 2
  osPriorityHigh3         = 40+3,       ///< Priority: high + 3
  osPriorityHigh4         = 40+4,       ///< Priority: high + 4
  osPriorityHigh5         = 40+5,       ///< Priority: high + 5
  osPriorityHigh6         = 40+6,       ///< Priority: high + 6
  osPriorityHigh7         = 40+7,       ///< Priority: high + 7
  osPriorityRealtime      = 48,         ///< Priority: realtime
  osPriorityRealtime1     = 48+1,       ///< Priority: realtime + 1
  osPriorityRealtime2     = 48+2,       ///< Priority: realtime + 2
  osPriorityRealtime3     = 48+3,       ///< Priority: realtime + 3
  osPriorityRealtime4     = 48+4,       ///< Priority: realtime + 4
  osPriorityRealtime5     = 48+5,       ///< Priority: realtime + 5
  osPriorityRealtime6     = 48+6,       ///< Priority: realtime + 6
  osPriorityRealtime7     = 48+7,       ///< Priority: realtime + 7
  osPriorityISR           = 99,         ///< Reserved for ISR deferred thread.
  osPriorityError         = -1,         ///< System cannot determine priority or illegal priority.
  osPriorityReserved      = 0x7FFFFFFF  ///< Prevents enum down-size compiler optimization.
} ar_priority_t;
typedef void*     (*ar_thread_func_t)(void *);

/// Thread state.
typedef enum {
  AR_THREAD_INACTIVE        =  0,          // Inactive.
  AR_THREAD_RUNNING         =  1,          // Running.
  AR_THREAD_BLOCKED         =  2,          // Blocked.
  AR_THREAD_TERMINATED      =  3,          // Terminated.
  AR_THREAD_MAX             =  0x7FFFFFFF  //
} ar_thread_state_t;

typedef struct {
  // TODO, add more attributes
  const char*              name;       // name of the thread
  int32_t                  timeout;    // timeout timestamp
  uint32_t                 stack_size; // size of stack
  ar_priority_t            priority;   // thread priority
} ar_thread_attr_t;

typedef struct {
    char               name[MAX_LEN];
    bool               running;
    bool               pending;
    bool               is_exit;
    char*              arg;
    void*              argument;
    unsigned           arg_len;
    int32_t            timeout;
    uint32_t           stack_size;

    ar_thread_state_t  state;
    ar_priority_t      priority;
    ar_thread_func_t   thread_fun;

    pthread_t          tid;
    pthread_attr_t     attr;
    pthread_mutex_t    mutex;
    ar_sem_id_t        signal;
} ar_thread_t;

/* thread */
// create thread with callback function(func), argument and attr.
// Thread function func is always run untill ar_thread_terminate.
// ar_thread_suspend/ar_thread_resume can suspend/resume this thread.
ar_thread_id_t ar_thread_new(ar_thread_func_t func, void *argument, unsigned len, ar_thread_attr_t* attr);

// return thread name
const char * ar_thread_name(ar_thread_id_t thread_id);

// return thread id
ar_thread_id_t ar_thread_id(void);

// return thread state
ar_thread_state_t ar_thread_state(ar_thread_id_t thread_id);

// return thread state in string
char* ar_thread_state_2_str(ar_thread_id_t thread_id);

// return thread stack size
uint32_t ar_thread_stack_size(ar_thread_id_t thread_id);

// return thread stack size
uint32_t ar_thread_stack_space(ar_thread_id_t thread_id);

// set thread priority
ar_status_t ar_thread_set_priority(ar_thread_id_t thread_id, ar_priority_t priority);

// get thread priority
ar_priority_t ar_thread_get_priority(ar_thread_id_t thread_id);

// TODO, not used currently
ar_status_t ar_thread_yield(void);

// suspend current thread
ar_status_t ar_thread_suspend(ar_thread_id_t thread_id);

// resume current thread
ar_status_t ar_thread_resume(ar_thread_id_t thread_id);

// detach thread
ar_status_t ar_thread_detach(ar_thread_id_t thread_id);

// join thread to release thread resource
ar_status_t ar_thread_join(ar_thread_id_t thread_id);

// exit thread
void ar_thread_exit(void);

// exit thread
void ar_thread_exit_self_clean(void);

// terminate thread to free thread context resource
ar_status_t ar_thread_terminate(ar_thread_id_t thread_id);

// TODO, not used currently
uint32_t ar_thread_get_count(void);

// TODO, not used currently
uint32_t ar_thread_enumerate(ar_thread_id_t *thread_array, uint32_t array_items);

// TODO, not used currently
uint32_t ar_thread_set_flags(ar_thread_id_t thread_id, uint32_t flags);

// TODO, not used currently
uint32_t ar_thread_clear(uint32_t flags);

// TODO, not used currently
uint32_t ar_thread_get_flags(void);

// TODO, not used currently
uint32_t ar_thread_wait_flags(uint32_t flags, uint32_t options, uint32_t timeout);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // _AR_THREAD_H_
