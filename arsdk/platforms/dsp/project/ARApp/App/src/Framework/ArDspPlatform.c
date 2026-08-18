/*
*********************************************************************************************
** File:         ArDspPlatform.c
**
** Version:      1.0.0.0
**
** Brief:
**
** Copyright(c) 2021 by artosyn. All rights reserved.
** Created on: 2021-1-7
**
*********************************************************************************************
*/

#include "Uart.h"
#include "mss.h"
#include "MemoryAttribution.h"
#include "ArDspPlatform.h"


#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------
//=================================================
// General Functions for SoC Design
//=================================================
void vWriteReg128(unsigned int addr, long long  data)
{
	long long  *reg_addr = (long long *)addr;
	*reg_addr = data;
}


void vWriteReg64(long int addr, long int data){
    long int *reg_addr = (long int *)addr;
	*reg_addr = data;
}

long int vReadReg64(long int addr){
    long int *reg_addr = (long int *)addr;
	return (*reg_addr);
}


void vWriteReg32(unsigned int addr, unsigned int data)
{
	unsigned int *reg_addr = (unsigned int *)addr;
	*reg_addr = data;
}

uint32_t xReadReg32(unsigned int addr)
{
	unsigned int *reg_addr = (unsigned int *)addr;
	return (*reg_addr);
}

char xReadReg8(unsigned int addr)
{
	char *reg_addr = (char *)addr;
	return (*reg_addr);
}

void xWriteReg8(unsigned int addr,char data)
{
	char *reg_addr = (char *)addr;
	*reg_addr = data;
}

/////////////////////////////////////////////////////////
/*system apb timer used for dsp, index:[0 - 7], enable_mode: [0x1 free_running_mode; 0x3 user defined counter mode] */
void vDspApbTimerLoad(int index, unsigned int value, unsigned int enable_mode)
{
	HWMEM_WORD(CEVAXM6_APB_TIMER1_LOAD + index * CEVAXM6_APB_TIMER_OFFSET) = value;
	HWMEM_WORD(CEVAXM6_APB_TIMER1_CONTR + index * CEVAXM6_APB_TIMER_OFFSET) = enable_mode;
}

void vDspGetApbTimerValue(int index, unsigned int *value)
{
	*value = HWMEM_WORD(CEVAXM6_APB_TIMER1_VALUE + index * CEVAXM6_APB_TIMER_OFFSET);
}

void vDspClearApbTimerInt(int index)
{
	int value = 0;
	value = HWMEM_WORD(CEVAXM6_APB_TIMER1_CLEAR + index * CEVAXM6_APB_TIMER_OFFSET);
}

void vDspInterruptInit()
{
#if USE_CEVA_TIMER
	//internal timer is disabled by default
#else
	//disable external interrupts in case they are active already
	for(int i = 0; i < CEVAXM6_APB_TIMER_NUM; i++)
	{
		HWMEM_WORD(CEVAXM6_APB_TIMER1_CONTR + i * CEVAXM6_APB_TIMER_OFFSET) = 0;
	}

	int val = HWMEM_WORD(CEVAXM6_APB_INTERRUPT_CLEAR);
#endif
	HWMEM_WORD(CORE0_INT0_MASK_ADDR) = TIMER0_OUT_EN;
	HWMEM_WORD(CORE0_INT1_MASK_ADDR) = (NPU_INTERRUPT_EN | TIMER1_INTERRUPT_EN);
	HWMEM_WORD(CORE0_INT2_MASK_ADDR) = 0xFFFFFFFF;

	vMaskDbgGen(0x10000048);
	vInternalCpmWrite(0xD14, 0);
	//vMaskDbgGen2();

	//enable interrupts to CPU when violation happens
	HWMEM_WORD(CORE0_TO_ARM_UOP_MASK_ADDR) = 0x1FF;

	__asm__("setp {imask} #0x3f");
	//enable soft interrupt 0
	__asm__("setp {imaskt} #0x1");
	__asm__("eint");
}


void vCevaCacheableInit()
{
	/* Mem cachable attribute table
	 * Non-Cachable: 0 - 83850000 (icc 83840000)
	 * Cachable    : 83850000 - 8c000000
	 * Non-Cachable: 8c000000 - a0000000
	 * Cachable    : a0000000 - ffffffff
	 */
    #if 0
	//config non cachable starting from 0
	vConfigAddxAttribute();
	//config cachable starting from DDR 80000000 so that CCI can be used
	xConfigRegionCacheable(0x22200000, 1);
	xConfigRegionNonCacheable(0x22400000, 2);
	//config_region_cacheable(0xA0000000, 3);
	//xConfigRegionCacheable(0x8E800000, 3);
	#endif
}

//#define MSS_ACCESS_PROCTECTION
#define DDR_ADDRESS_START 					0x20000000
#define ICC_RESERVED_ADDRESS_START 			0x22200000
#define ICC_RESERVED_ADDRESS_END 			0x22400000
#define CEVA_RESERVED_ADDRESS_START 		0x32400000
#define CEVA_RESERVED_ADDRESS_END 			0x34400000
#define MMZ_RESERVED_ADDRESS_START 			0x40000000  //modify with system mmz address/size
#define MMZ_RESERVED_ADDRESS_END 			0x60000000  //modify with system mmz address/size

void vCevaIacuAccessProtectionInit()
{
	xConfigRegionProgramAccessProtection(DDR_ADDRESS_START, 0, 7, 1, 1);
    xConfigRegionProgramAccessProtection(CEVA_RESERVED_ADDRESS_START, 0, 0, 2, 1);
    xConfigRegionProgramAccessProtection(CEVA_RESERVED_ADDRESS_END, 0, 7, 3, 1);
}

void vCevaDacuAccessProtectionInit()
{
	xConfigRegionDataAccessProtection(DDR_ADDRESS_START, 0, 7, 1, 0);
	xConfigRegionDataAccessProtection(ICC_RESERVED_ADDRESS_START, 0, 0, 2, 0);
	xConfigRegionDataAccessProtection(ICC_RESERVED_ADDRESS_END, 0, 7, 3, 0);
	xConfigRegionDataAccessProtection(CEVA_RESERVED_ADDRESS_START, 0, 0, 4, 0);
	xConfigRegionDataAccessProtection(CEVA_RESERVED_ADDRESS_END, 0, 7, 5, 0);
	xConfigRegionDataAccessProtection(MMZ_RESERVED_ADDRESS_START, 0, 0, 6, 0);
	xConfigRegionDataAccessProtection(MMZ_RESERVED_ADDRESS_END, 0, 7, 7, 0);
}

void vCevaStackAccessViolationInit()
{
	extern char __stack_start;
	extern char __stack_end;
	unsigned int STACK_END_ADDR = (unsigned int)&__stack_start;
	unsigned int STACK_START_ADDR = (unsigned int)&__stack_end;

	xConfigStackAccessVioltationRegion(STACK_START_ADDR, STACK_END_ADDR);
}

void vDspSystemInit(void)
{
	extern char __shm_print_start;
	char * DSPMSG_BUF_ADDR = (char *)&__shm_print_start;

	xDebugInit(DSPMSG_BUF_ADDR);
	vDspInterruptInit();

#ifdef MSS_ACCESS_PROCTECTION
	vCevaIacuAccessProtectionInit();
	vCevaDacuAccessProtectionInit();
	vMssEnableGlobalProgramCache();
	vCevaCacheableInit();
	xConfigMssAccessProtectionEnable(1, 1);
#else
	vMssEnableGlobalProgramCache();
	vCevaCacheableInit();
#endif
}


#ifdef __cplusplus
}
#endif

