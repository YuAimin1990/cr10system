/*
 * ArDspDdma1DTest.c
 *
 *  Created on: 2021Äê9ÔÂ15ÈÕ
 *      Author: bfchen
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "Uart.h"
#include "ArArmRmtCallDeclare.h"
#include "ArDmaDtcmFunc.h"
#include "mss.h"
#include "ArDspUnitSimulator.h"

extern unsigned long long run_cycle;

AR_S32 ArArmDdma1DLoadTest(void *xParams)
{

	AR_U32 *pu32Msg = xParams;
	AR_U32 u32InputAddr = 0;
	AR_U32 u32OutputAddr = 0;
	AR_CHAR * ps8DtcmAddr = NULL;
	AR_U32 u32DtcmLen = 0;
	unsigned long long load_before = 0;
	unsigned long long load_after = 0;
	unsigned long long store_before = 0;
	unsigned long long store_after = 0;

	u32InputAddr = *pu32Msg;
	u32OutputAddr = *(pu32Msg + 1);

	u32DtcmLen = ArDTCMGetAvailableLen();
	if(u32DtcmLen < 256*1024)
	{
		xDebugPrint("AR_DTCM_GetAvailableLen less than 256k\n");
		//return -1;
	}

	u32DtcmLen &= (~0x03ff);
	ps8DtcmAddr = ArDTCMGetStartAddr();

	AR_DMA_OPERATOR_PARAM_U OperateParam;
	OperateParam.stDma1dParams.length = u32DtcmLen;//256*1024;
	OperateParam.stDma1dParams.tr_type = DMA_TYPE_LINEAR;

	run_cycle = 0;
	//1D up
	load_before = mss_get_clock();
	ArDMALoadE2I(eDmaFuncComm,(char *)u32InputAddr, (char *)ps8DtcmAddr, eDmaDec1D, &OperateParam);
	load_after = mss_get_clock();
	run_cycle = load_after - load_before;

	//1D down
	store_before = mss_get_clock();
	ArDMAStoreI2E(eDmaFuncComm,(char *)u32OutputAddr, (char *)ps8DtcmAddr, eDmaDec1D, &OperateParam);
	store_after = mss_get_clock();

    xDebugPrint("load ddma test 1d load time = %lld\n", run_cycle);

	return 0;
}

AR_S32 ArArmDdma1DStoreTest(void *xParams)
{
	AR_U32 *pu32Msg = xParams;
	AR_U32 u32InputAddr = 0;
	AR_U32 u32OutputAddr = 0;
	AR_CHAR * ps8DtcmAddr = NULL;
	AR_U32 u32DtcmLen = 0;
	unsigned long long load_before = 0;
	unsigned long long load_after = 0;
	unsigned long long store_before = 0;
	unsigned long long store_after = 0;

	u32InputAddr = *pu32Msg;
	u32OutputAddr = *(pu32Msg + 1);

	u32DtcmLen = ArDTCMGetAvailableLen();
	if(u32DtcmLen < 256*1024)
	{
		xDebugPrint("AR_DTCM_GetAvailableLen less than 256k\n");
		//return -1;
	}
	u32DtcmLen &= (~0x03ff);

	ps8DtcmAddr = ArDTCMGetStartAddr();
	
	AR_DMA_OPERATOR_PARAM_U OperateParam;
	OperateParam.stDma1dParams.length = u32DtcmLen;//256*1024;
	OperateParam.stDma1dParams.tr_type = DMA_TYPE_LINEAR;

	run_cycle = 0;
	//1D up
	load_before = mss_get_clock();
	ArDMALoadE2I(eDmaFuncComm, (char *)u32InputAddr, (char *)ps8DtcmAddr, eDmaDec1D, &OperateParam);
	load_after = mss_get_clock();
	run_cycle = load_after - load_before;
	//1D down
	store_before = mss_get_clock();
	ArDMAStoreI2E(eDmaFuncComm, (char *)u32OutputAddr, (char *)ps8DtcmAddr, eDmaDec1D, &OperateParam);
	store_after = mss_get_clock();

	xDebugPrint("store ddma test 1d load time = %lld\n",run_cycle);

	return 0;
}

AR_S32 ArArmDdma2DLoadTest(void *xParams)
{

	AR_U32 *pu32Msg = xParams;
	AR_U32 u32InputAddr = 0;
	AR_U32 u32OutputAddr = 0;
	AR_CHAR * ps8DtcmAddr = NULL;
	AR_U32 u32DtcmLen = 0;
	unsigned long long load_before = 0;
	unsigned long long load_after = 0;
	unsigned long long store_before = 0;
	unsigned long long store_after = 0;

	u32InputAddr = *pu32Msg;
	u32OutputAddr = *(pu32Msg + 1);

	u32DtcmLen = ArDTCMGetAvailableLen();
	if(u32DtcmLen < 256*1024)
	{
		xDebugPrint("AR_DTCM_GetAvailableLen less than 256k\n");
		//return -1;
	}
	u32DtcmLen &= (~0x03ff);

	ps8DtcmAddr = ArDTCMGetStartAddr();

	AR_DMA_OPERATOR_PARAM_U OperateParam;
	OperateParam.stDma2dParams.width = 512;
	OperateParam.stDma2dParams.height = u32DtcmLen/512;
	OperateParam.stDma2dParams.src_stride = 512;
	OperateParam.stDma2dParams.dst_stride = 512;
	OperateParam.stDma2dParams.tr_type = DMA_TYPE_LINEAR;

	run_cycle = 0;
	//2D up
	load_before = mss_get_clock();
	ArDMALoadE2I(eDmaFuncComm, (char *)u32InputAddr, (char *)ps8DtcmAddr, eDmaDec2D, &OperateParam);
	load_after = mss_get_clock();
	//2D down
	store_before = mss_get_clock();
	ArDMAStoreI2E(eDmaFuncComm, (char *)u32OutputAddr, (char *)ps8DtcmAddr, eDmaDec2D, &OperateParam);
	store_after = mss_get_clock();

	run_cycle = load_after - load_before;
	xDebugPrint("ddma test 2d load time = %lld\n", run_cycle);


	return 0;
}

AR_S32 ArArmDdma2DStoreTest(void *xParams)
{

	AR_U32 *pu32Msg = xParams;
	AR_U32 u32InputAddr = 0;
	AR_U32 u32OutputAddr = 0;
	AR_CHAR * ps8DtcmAddr = NULL;
	AR_U32 u32DtcmLen = 0;
	unsigned long long load_before = 0;
	unsigned long long load_after = 0;
	unsigned long long store_before = 0;
	unsigned long long store_after = 0;

	u32InputAddr = *pu32Msg;
	u32OutputAddr = *(pu32Msg + 1);

	u32DtcmLen = ArDTCMGetAvailableLen();
	if(u32DtcmLen < 256*1024)
	{
		xDebugPrint("AR_DTCM_GetAvailableLen less than 256k\n");
		//return -1;
	}
	u32DtcmLen &= (~0x03ff);

	ps8DtcmAddr = ArDTCMGetStartAddr();

	AR_DMA_OPERATOR_PARAM_U OperateParam;
	OperateParam.stDma2dParams.width = 512;
	OperateParam.stDma2dParams.height = u32DtcmLen/512;
	OperateParam.stDma2dParams.src_stride = 512;
	OperateParam.stDma2dParams.dst_stride = 512;
	OperateParam.stDma2dParams.tr_type = DMA_TYPE_LINEAR;

	run_cycle = 0;
	//2D up
	load_before = mss_get_clock();
	ArDMALoadE2I(eDmaFuncComm, (char *)u32InputAddr, (char *)ps8DtcmAddr, eDmaDec2D, &OperateParam);
	load_after = mss_get_clock();
	//2D down
	store_before = mss_get_clock();
	ArDMAStoreI2E(eDmaFuncComm, (char *)u32OutputAddr, (char *)ps8DtcmAddr, eDmaDec2D, &OperateParam);
	store_after = mss_get_clock();

	run_cycle = store_after - store_before;
	xDebugPrint("ddma test 2d store time = %lld\n", run_cycle);


	return 0;
}

AR_ARM_REMOTECALL_DECLARE(ArArmDdma1DLoadTest) = {
    .pName = "ArArmDdma1DLoadTest",
    .pRmtCallHandler = ArArmDdma1DLoadTest,
};

AR_ARM_REMOTECALL_DECLARE(ArArmDdma1DStoreTest) = {
    .pName = "ArArmDdma1DStoreTest",
    .pRmtCallHandler = ArArmDdma1DStoreTest,
};

AR_ARM_REMOTECALL_DECLARE(ArArmDdma2DLoadTest) = {
    .pName = "ArArmDdma2DLoadTest",
    .pRmtCallHandler = ArArmDdma2DLoadTest,
};

AR_ARM_REMOTECALL_DECLARE(ArArmDdma2DStoreTest) = {
    .pName = "ArArmDdma2DStoreTest",
    .pRmtCallHandler = ArArmDdma2DStoreTest,
};
