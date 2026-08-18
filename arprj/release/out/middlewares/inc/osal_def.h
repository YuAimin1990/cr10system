/*****************************************************************************
Copyright: 2021-2025, Artosyn. Co., Ltd.
File name: osal_def.h
Description: The osal define.
Author: Artosyn Software Team
Version: 0.0.1
Date: 2021/04/21
History:
        0.0.1    2021/04/21    The initial version of osal_def.h
*****************************************************************************/
#ifndef __OSAL_DEF_H__
#define __OSAL_DEF_H__

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "osal_lock.h"
#include "osal_queue.h"
#include "osal_thread.h"
#include "utils_common.h"
#include "osal_timer.h"
//#include "ar_ion.h"

#define osWaitForever 0xFFFFFFFFU ///< Wait forever timeout value.
#define ar_assert(x) assert(x)
 
//typedef osVersion_t          ar_os_version_t; //TODO, re-write
//typedef osKernelState_t      ar_os_kernel_state_t; // TODO, re-write
typedef ar_priority_t         ar_os_priority_t;
typedef ar_thread_id_t        ar_os_thread_id_t;
typedef ar_timer_id_t         ar_os_timer_id_t;
//typedef osEventFlagsId_t     ar_os_event_flag_id_t;
typedef ar_lock_id_t          ar_os_mutex_id_t;
typedef ar_sem_id_t           ar_os_semaphore_id_t;
typedef ar_sem_id_t2           ar_os_semaphore_id_t2;

//typedef osMemoryPoolId_t     ar_os_memory_pool_id_t; // TODO, re-write
typedef ar_queue_id_t         ar_os_msg_queue_id_t;
typedef ar_thread_attr_t      ar_os_thread_attr_t;
//typedef ar_time_attr_t        ar_os_timer_attr_t; // TODO, re-write
//typedef osEventFlagsAttr_t   ar_os_event_flag_attr_t; // TODO, re-write
////typedef osMutexAttr_t        ar_os_mutex_attr_t; // not needed
////typedef osSemaphoreAttr_t    ar_os_semaphore_attr_t; // not needed
//typedef osMemoryPoolAttr_t   ar_os_memory_pool_attr_t; // TODO, re-write
typedef ar_queue_attr_t       ar_os_msg_queue_attr_t;
typedef ar_status_t           ar_os_status_t;
typedef ar_thread_state_t     ar_os_thread_state_t;
typedef ar_thread_func_t      ar_os_thread_func_t;
typedef ar_lock_id_t          ar_lock_t;
typedef ar_signal_id_t        ar_signal_t;
//typedef xTaskHandle          ar_task_handle; // TODO, re-write
//typedef UBaseType_t          ar_prio_t;      // TODO, re-write
typedef time_func             ar_os_time_func_t;



#endif
