/*
 FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.

 VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

 ***************************************************************************
 *                                                                       *
 *    FreeRTOS provides completely free yet professionally developed,    *
 *    robust, strictly quality controlled, supported, and cross          *
 *    platform software that has become a de facto standard.             *
 *                                                                       *
 *    Help yourself get started quickly and support the FreeRTOS         *
 *    project by purchasing a FreeRTOS tutorial book, reference          *
 *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
 *                                                                       *
 *    Thank you!                                                         *
 *                                                                       *
 ***************************************************************************

 This file is part of the FreeRTOS distribution.

 FreeRTOS is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License (version 2) as published by the
 Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

 >>! NOTE: The modification to the GPL is included to allow you to distribute
 >>! a combined work that includes FreeRTOS without being obliged to provide
 >>! the source code for proprietary components outside of the FreeRTOS
 >>! kernel.

 FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  Full license text is available from the following
 link: http://www.freertos.org/a00114.html

 1 tab == 4 spaces!

 ***************************************************************************
 *                                                                       *
 *    Having a problem?  Start by reading the FAQ "My application does   *
 *    not run, what could be wrong?"                                     *
 *                                                                       *
 *    http://www.FreeRTOS.org/FAQHelp.html                               *
 *                                                                       *
 ***************************************************************************

 http://www.FreeRTOS.org - Documentation, books, training, latest versions,
 license and Real Time Engineers Ltd. contact details.

 http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 including FreeRTOS+Trace - an indispensable productivity tool, a DOS
 compatible FAT file system, and our tiny thread aware UDP/IP stack.

 http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
 Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
 licenses offer ticketed support, indemnification and middleware.

 http://www.SafeRTOS.com - High Integrity Systems also provide a safety
 engineered and independently SIL3 certified version for use in safety and
 mission critical applications that require provable dependability.

 1 tab == 4 spaces!
 */

//**********************************************************
//********************** Includes **************************
//**********************************************************
/* Standard Includes. */
#include <ArDspPlatform.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "Uart.h"

//**********************************************************
//********************* Constants **************************
//**********************************************************
/*
 * Context size in double words
 * This value should be updated according to the save context / restore context mechanism
 */
#define XM_TRAP_MASK_INIT_VALUE 	             0x10

/* Timer */
#define CEVA_TIMER_BASE_ADDR  		             0xC0041000
#define CEVA_TIMER_VALUE_OFS  		             0x8
#define CEVA_TIMER_MASK_OFS   		             0xc
#define CEVA_TIMER_INT_EN_MASK   	             0x1
#define CEVA_TIMER_INT_CLEAR_MASK  	             0x2

/* Idle task */
#define IDLE_TASK_STACK_SIZE_IN_BYTES            (356)
#define IDLE_TASK_PRINT_PERIODICITY_IN_SEC       (2)

//**********************************************************
//********************** Macros ****************************
//**********************************************************
#if (1 == configSUPPORT_STATIC_ALLOCATION)
#define bytes_sizeof(param)	(sizeof(param))
#define portSTACK_SIZE_FROM_BYTES(bytes)  ((bytes) / bytes_sizeof(StackType_t))
#define portTOTAL_STACK_SIZE(stack_size_in_bytes) (configMINIMAL_STACK_SIZE + portSTACK_SIZE_FROM_BYTES(stack_size_in_bytes))
#endif

//**********************************************************
//********************* Globals ****************************
//**********************************************************
#if (1 == configSUPPORT_STATIC_ALLOCATION)
/* Timer Task Memory */
StackType_t  xPortTimerTaskMemory[configMINIMAL_STACK_SIZE * 2];
StaticTask_t xPortTimerTaskTCBBuffer;

StackType_t    IDLE_TASK_taskStack[portTOTAL_STACK_SIZE(IDLE_TASK_STACK_SIZE_IN_BYTES)];
StaticTask_t   IDLE_TASK_taskTcb;
#endif

//**********************************************************
//************** Functions Decelerations *******************
//**********************************************************
__asm__("\t.extern restore_sp_from_pxCurrentTCB");

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt(void);

/*
 * Call back for the alarm function.
 */
void vPortSysTickHandler();

/*
 * Set interrupt mask
 */
void setTimerInterruptMask(void);

/*
 * Initialize stack registers
 */
void initTcbStackRegs(void* pvParameters, StackType_t** sp, void* imask);

//**********************************************************
//********************* Functions **************************
//**********************************************************
/***********************************************************
 * pxPortInitialiseStack
 *
 * Written by: ?? ??/??/????
 * Description: Setup the stack of a new task so it is ready
 *              to be placed under the scheduler control.
 *              The registers have to be placed on the stack
 *              in the order that the port expects to
 *              find them.
 *
 * Input:
 * 		1. Top of the stack pointer.
 *      2. Start of task code.
 *      3. Parameter for the task being created.
 * 	Output:
 * 		New top of the stack pointer.
 ***********************************************************/
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode,
										void *pvParameters )
{
	portLONG *params;
	portLONG *imask;
	StackType_t *pStack  = pxTopOfStack;

    // Set the stack pointer for the new task and pass the pointer to the parameters.
	// Other registers are set to random values.
	initTcbStackRegs(&params, &pStack, &imask);

	//memset(pStack, 0, (pxTopOfStack - pStack) * sizeof(StackType_t));

	*params = (portLONG)pvParameters;
	imask[8] = 0xc000c000;				//set predicates to initial values (pr14 and pr15 are set)
	imask[19] = (portLONG)pxCode;		//set the entry point at retregi in the auxreg10 register set
	*imask++ = XM_INT_MASK_INIT_VALUE;	//set initial value for MODA register (IMASK for INT0-INT4)
	*imask = XM_TRAP_MASK_INIT_VALUE;	//set initial value for MODB register (IMASK for SW interrupts)
	return pStack;
}

/***********************************************************
 * xPortStartScheduler
 *
 * Written by: ?? ??/??/????
 * Description: Setup the hardware ready for the scheduler
 *              to take control.
 *              This generally sets up a tick interrupt
 *              and sets timers for the correct
 *              tick frequency.
 *
 * Input:
 * 		None.
 * 	Output:
 * 		Function finished.
 ***********************************************************/
portBASE_TYPE xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	 here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	__asm__ ("nop");
	__asm__ ( "\tpcu.br {ds1} #restore_sp_from_pxCurrentTCB, #0x0, #0x0, ?pr14.b" );
	__asm__ ("nop");
	/* Should not get here! */
	return 0;
}

/***********************************************************
 * vPortEndScheduler
 *
 * Written by: ?? ??/??/????
 * Description: Undo any hardware/ISR setup that was
 *              performed by xPortStartScheduler() so
 *              the hardware is left in its original
 *              condition after the scheduler stops executing.
 *
 * Input:
 * 		None.
 * 	Output:
 * 		None.
 ***********************************************************/
void vPortEndScheduler( void )
{
}



void prvSetupTimerInterrupt(void)
{
#if USE_CEVA_TIMER
	    xDebugPrint("Run prvSetupTimerInterrupt()... \r\n");

		HWMEM_WORD(CEVA_TIMER0_CFG_ADDR) = ((0xC<<16) | (0x1<<7) | (0x4<<0));//0x000C0084;
		HWMEM_WORD(CEVA_TIMER0_SC_ADDR)  =  ( configCPU_CLOCK_HZ / configTICK_RATE_HZ );//0x10000;
		HWMEM_WORD(CEVA_TIMER0_CFG_ADDR) = ((0x9<<16) | (0x1<<7) | (0x4<<0));//|= ( TIMER_CLOCK_DIVIDED_1 | TIMER_FREE_RUN | TIMER_EN );
		HWMEM_WORD(CEVA_TIMER0_CFG_ADDR) = ((0x8<<16) | (0x1<<7) | (0x4<<0));//0x00080084;

#else //ceva local apb timer
		//APB timer, input clk is dsp clk / 4;
			*(int *)CEVAXM6_APB_TIMER1_LOAD = 20000000 / 1000; //tick 10,000 per sec.
			*(int *)CEVAXM6_APB_TIMER1_CONTR = 0x3; //enable user-defined mode
#endif
}
/*-----------------------------------------------------------*/
/***********************************************************
 * vPortSysTickHandler
 *
 * Written by:  ER 09/07/2020
 * Description: Timer tick interrupt handler
 *              ( To clear the interrupt ).
 *
 * Input:
 * 		None.
 * Output:
 * 		None.
 ***********************************************************/
#if USE_CEVA_TIMER //ceva internal timer
void vPortSysTickHandler( void )
{
    /* Increment the kernel tick. */
	if (xTaskIncrementTick() != pdFALSE)
    {
		vTaskSwitchContext();
	}
}
#else
void vPortSysTickHandler( void )
{
	int val = 0;

    /* Increment the kernel tick. */
	if (xTaskIncrementTick() != pdFALSE)
    {
		vTaskSwitchContext();
	}

	//clear the timer irq
	val = *(volatile int *)CEVAXM6_APB_TIMER1_CLEAR;
}
#endif


static unsigned int cntsys =0;
static unsigned int cntdla =0;

void vPortMuxIrqHandler(void)
{
	int val = 0;

	val = *(volatile int *)CORE0_INT1_STATUS;

	//check if timer1 irq
	if(val & CORE0_INT1_TIMER_BIT)
	{
	    cntsys++;
	    *(unsigned int*)(0x32400440) = cntsys;
		vPortSysTickHandler();
		return;
	}

	//check if npu irq
	if(val & CORE0_INT1_NPU_BIT)
	{
		cntdla++;
	    *(unsigned int*)(0x32400444) = cntdla;
		vPortint1NPUHandler();
		return;
	}
}

void vPortint2Handler()
{

}


#if (1 == configSUPPORT_STATIC_ALLOCATION)
/***********************************************************
 * vApplicationGetTimerTaskMemory
 *
 * Written by:  ER 09/07/2020
 * Description: configSUPPORT_STATIC_ALLOCATION and
 *              configUSE_TIMERS are both set to 1, so the
 *              application must provide an implementation
 *              of vApplicationGetTimerTaskMemory() to
 *              provide the memory that is used by the Timer
 *              service task.
 *
 * Input:
 * 		1. Timer task data structure.
 *      2. Stack resource of the task.
 *      3. Timer task stack size.
 * 	Output:
 * 		None.
 ***********************************************************/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer   = &xPortTimerTaskTCBBuffer;
    *ppxTimerTaskStackBuffer = xPortTimerTaskMemory;
    *pulTimerTaskStackSize   = configMINIMAL_STACK_SIZE * 2;
}

/***********************************************************
 * vApplicationGetIdleTaskMemory
 *
 * Written by:  ER 09/07/2020
 * Description: configSUPPORT_STATIC_ALLOCATION is set to 1,
 *              so the application must provide an
 *              implementation of
 *              vApplicationGetIdleTaskMemory() to provide
 *              the memory that is used by the Idle task.
 *
 * Input:
 * 		1. Timer task data structure.
 *      2. Stack resource of the task.
 *      3. Timer task stack size.
 * 	Output:
 * 		None.
 ***********************************************************/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
   *ppxIdleTaskTCBBuffer   = &IDLE_TASK_taskTcb;
   *ppxIdleTaskStackBuffer = IDLE_TASK_taskStack;
   *pulIdleTaskStackSize   = portTOTAL_STACK_SIZE(IDLE_TASK_STACK_SIZE_IN_BYTES);
}
#endif
//==============================================================================
// follow is for interrupts
//==============================================================================

#if 1
//---------------------------------------
__asm__(".globl vPortTickInterrupt");
__asm__(".globl portYIELD");
__asm__(".globl vPortNPUInterrupt");
__asm__(".globl vPortint2Interrupt");
__asm__(".globl vPortMuxInterrupt");
//---------------------------------------
__asm__(".globl __cxd_int0_handler");
__asm__(".globl __cxd_int1_handler");
__asm__(".globl __cxd_int2_handler");
__asm__(".globl __cxd_trap0_handler");
__asm__(".globl __cxd_nmi_handler");
//---------------------------------------

//**********************************************************
//********************* Functions **************************
//**********************************************************

//Now nmi is used for grabbing PC and SP and save them to sys registers
__asm__("__cxd_nmi_handler:");
__asm__("LS1.push{auxreg0}");
//clear nmi irq
__asm__("PCU.nop #0x4");
__asm__("SC0.mov #0x10c4010, r5.di");
__asm__("PCU.nop #0x4");
__asm__("SC0.mov #0, r6.di || LS1.st r6.ui, (r5.ui).ui");
//__asm__("LS1.st r6.ui, r5.ui");
//__asm__("LS1.st (#8+sp.ui).ui+#0, r5.ui");
//__asm__("SC0.mov #0x10c001c, r6.di");
//__asm__("LS1.st r5.ui, r6.ui");
__asm__("PCU.nop #0x4");
__asm__("PCU.nop #0x4");
__asm__("SC0.mov #0x10c1014, r6.di");
__asm__("PCU.nop #0x4");
__asm__("PCU.nop #0x4");
__asm__("LS1.st retregn.ui, (#0 + r6.ui).ui");
__asm__("PCU.nop #0x4");
__asm__("PCU.nop #0x4");
__asm__("LS0.pop{auxreg0}");
__asm__("PCU.nop #0x4");
__asm__("PCU.nop #0x4");
__asm__("PCU.retn");
__asm__("PCU.nop");


	__asm__("__cxd_int0_handler:");
#ifdef XM6
	#if USE_CEVA_TIMER
	    __asm__("PCU.brr{ds1} #vPortTickInterrupt, #0x0, #0x0");
	    __asm__("PCU.nop");
	#else
	    __asm__("PCU.reti");
	    __asm__("PCU.nop");
	#endif
#else
#ifdef XM4
    __asm__("__cxd_int0_handler:");
    __asm__("PCU.brr{t} #vPortTickInterrupt");
    __asm__("PCU.reti");
#endif
#endif

/***********************************************************
 ***********************************************************/
	__asm__("__cxd_int1_handler:");
#ifdef XM6
	#if USE_CEVA_TIMER
		__asm__("PCU.brr{ds1} #vPortNPUInterrupt, #0x0, #0x0");
		__asm__("PCU.nop");
	#else
	     __asm__("PCU.brr{ds1} #vPortMuxInterrupt, #0x0, #0x0");
	    __asm__("PCU.nop");
	#endif
#else
#ifdef XM4
    __asm__("__cxd_int1_handler:");
    __asm__("PCU.brr{t} #vPortNPUInterrupt");
    __asm__("PCU.reti");
#endif
#endif

/***********************************************************
 ***********************************************************/
	__asm__("__cxd_int2_handler:");
#ifdef XM6
    __asm__("PCU.brr{ds1} #vPortint2Interrupt, #0x0, #0x0");
    __asm__("PCU.nop");
#else
#ifdef XM4
    __asm__("__cxd_int2_handler:");
    __asm__("PCU.brr{t} #vPortint2Interrupt");
    __asm__("PCU.reti");
#endif
#endif

/***********************************************************
 ***********************************************************/
    __asm__("__cxd_trap0_handler:");
#ifdef XM6
    __asm__("PCU.brr{ds1} #portYIELD, #0x0, #0x0");
    __asm__("PCU.nop");
#else
#ifdef XM4
    __asm__("PCU.brr{t} #portYIELD");
    __asm__("PCU.reti");
#endif
#endif

//-------------------------------------------------------------
#endif

/*-----------------------------------------------------------*/

