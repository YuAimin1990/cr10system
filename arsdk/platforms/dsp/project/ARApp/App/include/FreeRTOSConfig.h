/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 CEVA-DSP.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//**********************************************************
//********************** Includes **************************
//**********************************************************
#include "cevaxm.h"

//**********************************************************
//********************* Constants **************************
//**********************************************************
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION			 1
#define configUSE_TIME_SLICING			 0
// the following def should not be modified to 1
#define portTICK_TYPE_IS_ATOMIC          0 // on these cores the tick read is atomic (32 bit read)
// the following def is used to support functions for sending task notifications
#define configUSE_TASK_NOTIFICATIONS     1
#define configUSE_MALLOC_FAILED_HOOK     1
#define configUSE_IDLE_HOOK				 0
#define configUSE_TICK_HOOK				 1
#define configTICK_RATE_HZ				 ( ( portTickType ) 1000 )

//HBBAI: change to 50M to reduce schedule time, improve icc performance
//HBBAI: Use ext APB timer, which input clk is 1/4 of dsp clk
#define configCPU_CLOCK_HZ				 ( 20000000 )  //10,000 ticks for 800M dsp

#define configMAX_PRIORITIES			 ( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE		 ( 1024 )
#define configAPPLICATION_ALLOCATED_HEAP 1
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION	 1

#define configTOTAL_HEAP_SIZE			( ( size_t ) 0x10000 )
#define configMAX_TASK_NAME_LEN			 ( 32 )
#define configUSE_TRACE_FACILITY		 1
#define configUSE_16_BIT_TICKS			 0
#define configIDLE_SHOULD_YIELD			 1
#define configUSE_MUTEXES				 1
#define configUSE_RECURSIVE_MUTEXES		 0
#define configUSE_COUNTING_SEMAPHORES	 0
#define configCHECK_FOR_STACK_OVERFLOW	 2
#define configQUEUE_REGISTRY_SIZE		 0

extern volatile uint64_t ulCpu_RunTime;
#define configGENERATE_RUN_TIME_STATS	 1
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()  (ulCpu_RunTime = 0UL)
#define portGET_RUN_TIME_COUNTER_VALUE() ulCpu_RunTime

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 			 0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			 1
#define INCLUDE_uxTaskPriorityGet			 1
#define INCLUDE_vTaskDelete				 	 1
#define INCLUDE_vTaskCleanUpResources		 1
#define INCLUDE_vTaskSuspend				 1
#define INCLUDE_vTaskDelayUntil				 1
#define INCLUDE_vTaskDelay					 1
#define INCLUDE_xTaskGetIdleTaskHandle		 1	//Do not touch.
#define configUSE_STATS_FORMATTING_FUNCTIONS 1	//Do not touch.


// for queues support
#define configUSE_QUEUE_SETS		        1

/* The priority at which the tick interrupt runs.  This should probably be
kept at 1. */
#define configKERNEL_INTERRUPT_PRIORITY			0x01

/* The maximum interrupt priority from which FreeRTOS.org API functions can
be called.  Only API functions that end in ...FromISR() can be used within
interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY	0x03

#ifdef XM4
#define XM_INT_MASK_INIT_VALUE					0x1500
#endif //XM4
#ifdef XM6
#define XM_INT_MASK_INIT_VALUE					0x02E0
#endif //XM6
#ifdef SENSPRO
#define XM_INT_MASK_INIT_VALUE					0x02E0
#endif //SENSPRO


#endif /* FREERTOS_CONFIG_H */
