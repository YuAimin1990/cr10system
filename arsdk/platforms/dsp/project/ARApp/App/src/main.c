/*
*********************************************************************************************************************
** File:         main.c
**
** Version:      1.0.0.0
**
** Brief:
**
** Copyright(c) 2021 by artosyn. All rights reserved.
** Created on:  2021-1-7
**
*********************************************************************************************************************
*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Uart.h"
#include "ArMalloc.h"
#include "IccApi.h"
#include "ArDspPlatform.h"
#include "ArTasksSchedule.h"
#include "ArDspArtSimulator.h"
#include "ArDspUnitSimulator.h"

//This addr and len must be set according to icc-reserved node in Linux DTS.
#define SHARE_MEM_ADDR (0x22200000)
#define SHARE_MEM_LEN  (0x200000)

/**********************************************************************************************************************
 **********************************************************************************************************************/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	xDebugPrint("vApplicationStackOverflowHook - out of memory, %s!\n", pcTaskName);
	for(;;);
}

/**********************************************************************************************************************
 **********************************************************************************************************************/
void vApplicationMallocFailedHook( TaskHandle_t pxTask, signed char *pcTaskName )
{
    xDebugPrint("vApplicationMallocFailedHook - out of memory , %s!\n", pcTaskName);
    for(;;);
}

/**********************************************************************************************************************
 **********************************************************************************************************************/
int main(int argc, char *argv[])
{
	extern char __ar_malloc_start;
 	extern size_t __ar_malloc_size;
 	vArMallocInit((size_t)(&__ar_malloc_start), __ar_malloc_size);

#ifdef ArDspArtSimulator

	setbuf(stdout,0);
	xDebugPrint("Artosyn BareMetal ArtSimulator V0.0 \r\n");
	xArBareMetalArtSimulator();

#elif defined(ArDspUnitSimulator)

    setbuf(stdout,0);
	xDebugPrint("Artosyn BareMetal UnitSimulator V0.0 \r\n");
#ifdef ArNpuCallbackSimualtor
	char *NpuOutputFile = "C:\\test\\ud\\output_888.bin";
	char *NpuGoldenFile = "C:\\test\\ud\\input.bin";
	char *NpuCallbackJson = "C:\\test\\ud\\callback.json";
	char *NpuInputFile = "C:\\test\\ud\\out_ylu.bin";

	xArBareMetalUnitSimulatorWithNpu(NpuOutputFile, NpuInputFile, NpuCallbackJson, NpuGoldenFile);
#else
	int ret = 0;
	char *ArmOutputFile = "E:\\dsp_demo\\Output.bin";
	char *ArmInputFile = "E:\\dsp_demo\\input_callback.bin";
	char *ArmGoldenFile = "E:\\dsp_demo\\output_1099.bin";
	char operatorName[32] = "dsp_demo";

	unsigned int  msg[5] = {0};
	Sim_Model_Handle_S SimHandle;
	ret = xArUnitSimulatorLoadModel(&SimHandle, ArmInputFile, ArmGoldenFile);
	if(ret < 0)
	{
		xDebugPrint("Unit Load Model failed! \r\n");
		return -1;
	}
	msg[0] = SimHandle.u32InputAddress;
	msg[1] = SimHandle.u32OutputAddress;
	msg[2] = 128;	//width
	msg[3] = 128;	//height
	msg[4] = 64;	//channel

	//xDebugPrint("SimHandle.u32InputAddress = 0x%x, SimHandle.u32OutputAddress = 0x%x\n", SimHandle.u32InputAddress,SimHandle.u32OutputAddress);
	xArBareMetalUnitSimulatorWithArm((void *)msg, operatorName);
	xArUnitSimulatorUnLoadModel(&SimHandle, ArmOutputFile);
#endif
#else

	vDspSystemInit();
	xDebugPrint("======DspSystemInit Done======\r\n");
	xDmaInit();
	xDebugPrint("======DDMAInit Done======\r\n");
	xArIccInit((unsigned char *)SHARE_MEM_ADDR, SHARE_MEM_LEN);
	xDebugPrint("======ICCInit Done======\r\n");
	xArFsInit();
	xDebugPrint("======FSInit Done======\r\n");

	xDebugPrint("Build Time: %s %s\r\n", __DATE__, __TIME__);

#if _FREERTOS_VERSION_ON_
	xDebugPrint("=== Artosyn FreeRtos V10.7 ===20230312==\r\n");
	xArTasksSchedule();
#else
	xDebugPrint("=== Artosyn BraeMetal V0.1 ===20230312\r\n");
	xArBaremetalLoop();
#endif

#endif
	return 0;
}
