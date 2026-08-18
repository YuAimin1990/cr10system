/*
 * ArCevaSimulator.c
 *
 *  Created on: 2021Äê3ÔÂ19ÈÕ
 *      Author: SW
 */

#include <stdlib.h>
#include <string.h>
#include "Uart.h"
#include "ArNpuPlatform.h"
#include "ArCnnOperatorDeclare.h"
#include <ArCnnParseDeclare.h>
#include "ArDspArtSimulator.h"

 void vInferenceResultClean(volatile unsigned int *pMem, volatile unsigned int *pMemLen)
 {
	 memset(pMem,0,*(unsigned int *)pMemLen);
	 *(volatile unsigned int *)pMemLen = 0;
 }

 void vInferenceCallBackAck(unsigned int* pResultAddr,int status)
 {
	 pResultAddr[0]=CmdOpFInferAck;
	 pResultAddr[1]=status;
 }

 void vInferenceTaskAndFlagClean( volatile unsigned int *pFlag, volatile unsigned int *pMem, volatile unsigned int *pMemLen)
 {
	 memset(pMem,0,*(unsigned int*)pMemLen);
	 *(volatile unsigned int*)pMemLen = 0;
	 *(volatile unsigned int*)pFlag =0;
 }


 static int xParserCallbackJson(void *pCbJsonAddr, AR_NPU_CB_PARAM_S *CallBackParam, int CallBackId)
 {
    int ret = -1;

	AR_NPU_LIST_ITEMS_st *NpuListItem = (AR_NPU_LIST_ITEMS_st *)malloc(sizeof(AR_NPU_LIST_ITEMS_st));
	if(NpuListItem == NULL)
	{
		xDebugPrint("NpuListItem malloc failed\r\n");
		return ret;
	}

	AR_U32 u32CBNum = 0;
	AR_NPU_CB_PARAM_S * pstCB = NpuListItem->astCBParam;
	ret = xArNpuParseCBFile((AR_CHAR*)pCbJsonAddr, pstCB, &u32CBNum);
	if(ret < 0)
	{
		xDebugPrint("Parse CB failed\r\n");
		free(NpuListItem);
		return ret;
	}

	memcpy((void*)CallBackParam,(void*)&NpuListItem->astCBParam[CallBackId],sizeof(AR_NPU_CB_PARAM_S));
	xDebugPrint("NPU opName %s opType %s\r\n",CallBackParam->achOperatorName,CallBackParam->achOperatorType);
	free(NpuListItem);

	return ret;
 }


static int32_t xParserCustomerJson(void *pUsrJsonAddr, void **CustomerParam, int NetworkId)
{
 	int ret = -1;
 	ArCnnParseDeclare_t* entry=NULL;

 	ArCnnParseDeclare_t *start = xArCnnParseEntryStart(ArCnnParseDeclare_t);
	const int32_t n_entry = xArCnnParseEntryStart(ArCnnParseDeclare_t);
	for (entry = start; entry != start + n_entry; entry++)
	{
		//if (NetworkId == entry->xNetID)
			return entry;
	}
	xDebugPrint("NetworkID %d not register!\r\n",NetworkId);

    entry=xArCnnParseLookUpName(NetworkId);
    if(entry)
    {
	    ret=entry->pCnnParseHandler(pUsrJsonAddr, 0, CustomerParam, NULL);
    }
    xDebugPrint("UsrParamAddr Has Parse Done\r\n");
    return ret;

}


static int32_t xInferenceCallBack( AR_CHAR *pIn, AR_CHAR *pOut, AR_NPU_CB_PARAM_S NpuCallBackParam, void *CustomerParam)
 {
	 xDebugPrint("Starting xInferenceCallBack\r\n");

	 int ret = 0;
	 ArCnnOperatorDeclare_t * entry=NULL;
	 entry=xArNpuOperatorLookUpName(&NpuCallBackParam);
	 if(entry)
	 {
		 ret=entry->pCnnCbHandler((AR_U32)pIn,(AR_U32)pOut,&NpuCallBackParam,CustomerParam);
	 }
	 xDebugPrint("NPU CB Process Done RET=%d\n",ret);
	 return ret;
 }


 void xArBareMetalArtSimulator()
 {

     int ret= -1;
 	 volatile unsigned int *pServerTargetFlag = (unsigned int*)SERVERTARGETFLAG;
	 volatile unsigned int *pServerTargetTaskMemLen  = (unsigned int*)SERVERTARGETTASKMEMLEN;
	 volatile unsigned int *pServerTargetTaskMem  = (unsigned int*)SERVERTARGETTASKMEM;
	 volatile unsigned int *pServerTargetResultMemLen  = (unsigned int*)SERVERTARGETRSLTMEMLEN;
	 volatile unsigned int *pServerTargetResultMem  = (unsigned int*)SERVERTARGETRSLTMEM;
	 *(unsigned int *)pServerTargetResultMemLen = 0;

	 while(1)
	 {
			 if(*(unsigned int *)pServerTargetFlag == 0)
			 {
				   vBlockInstructuon(100000);
				   continue;
			 }

			 vInferenceResultClean(pServerTargetResultMem, pServerTargetResultMemLen);

			 unsigned int* pCmdId 		= (unsigned int*)pServerTargetTaskMem + 0;
			 unsigned int* pNetId 		= (unsigned int*)pCmdId + 1;
			 unsigned int* pCbJsonLen   = (unsigned int*)pNetId + 1;
			 unsigned char*pCbJson 		= (unsigned char*)pCbJsonLen + 4;
			 unsigned int* pUsrJsonLen  = (unsigned char*)pCbJson + *(unsigned int*)pCbJsonLen;
			 unsigned char*pUsrJson 	= (unsigned char*)pUsrJsonLen + 4;
			 unsigned int* pNetworkId   = (unsigned char*)pUsrJson + *(unsigned int*)pUsrJsonLen;
			 unsigned int* pCallBackId  = (unsigned int*)pNetworkId + 1;
			 unsigned int* pInLen 		= (unsigned int*)pCallBackId + 1;
			 unsigned char*pIn 			= (unsigned char*)pInLen + 4;

			 xDebugPrint("TargetFlag  = %d\r\n",*(unsigned int *)pServerTargetFlag);
			 xDebugPrint("TaskMemLen  = %d\r\n",*(unsigned int *)pServerTargetTaskMemLen);
			 xDebugPrint("pCmdId 	  = %d\r\n",*(unsigned int *)pCmdId);
			 xDebugPrint("pNetId      = %d\r\n",*(unsigned int *)pNetId);
			 xDebugPrint("pCbJsonLen  = %d\r\n",*(unsigned int *)pCbJsonLen);
			 xDebugPrint("pUsrJsonLen = %d\r\n",*(unsigned int *)pUsrJsonLen);
			 xDebugPrint("pNetworkId  = %d\r\n",*(unsigned int *)pNetworkId);
			 xDebugPrint("pCallBackId = %d\r\n",*(unsigned int *)pCallBackId);
			 xDebugPrint("pInLen 	  = %d\r\n",*(unsigned int *)pInLen);

             if((*(unsigned int*)pCmdId != CmdOpFInfer)||(*(unsigned int*)pNetId != *(unsigned int*)pNetworkId))
             {
            	 *(unsigned int*)pServerTargetResultMemLen =2;
				 vInferenceCallBackAck(pServerTargetResultMem, ret);
				 vInferenceTaskAndFlagClean(pServerTargetFlag,pServerTargetTaskMem,pServerTargetTaskMemLen);
				 xDebugPrint("Task: CmdId || NetId error\r\n");
				 continue;
			 }

			 AR_NPU_CB_PARAM_S CallBackParam = {0};
			 ret = xParserCallbackJson((void *)pCbJson, &CallBackParam, *(unsigned int*)pCallBackId);
             if(ret<0)
             {
            	 *(unsigned int*)pServerTargetResultMemLen =2;
				 vInferenceCallBackAck(pServerTargetResultMem, ret);
				 vInferenceTaskAndFlagClean(pServerTargetFlag,pServerTargetTaskMem,pServerTargetTaskMemLen);
				 xDebugPrint("Task: xParserCallbackJson Fail\r\n");
				 continue;
			 }


			 void *CustomerParam = NULL;
			 ret = xParserCustomerJson((void *)pUsrJson, (void **)&CustomerParam, *(unsigned int*)pNetworkId);
			 if(ret<0)
			 {
				 *(unsigned int*)pServerTargetResultMemLen =2;
				 vInferenceCallBackAck(pServerTargetResultMem, ret);
				 vInferenceTaskAndFlagClean(pServerTargetFlag,pServerTargetTaskMem,pServerTargetTaskMemLen);
				 xDebugPrint("Task: xParserCustomerJson Fail\r\n");
				 continue;
			 }


			AR_U32 u32OutHeight 	= CallBackParam.astOutputTensor[0].u32Height;
			AR_U32 u32OutWidth 		= CallBackParam.astOutputTensor[0].u32Width;
			AR_U32 u32OutRowStep 	= CallBackParam.astOutputTensor[0].u32RowStep;
			AR_U32 u32OutKSizeNorm  = CallBackParam.astOutputTensor[0].u32KSizeNorm;
			AR_U32 u32OutKSizeLast  = CallBackParam.astOutputTensor[0].u32KSizeLast;
			AR_U32 u32OutPrecision  = CallBackParam.astOutputTensor[0].u32Precision;
			AR_U32 u32OutKStep 	    = CallBackParam.astOutputTensor[0].u32KStep;
			AR_U32 u32OutKNormNum   = CallBackParam.astOutputTensor[0].u32KNormNum;
			AR_U32 u32OutOffset       = CallBackParam.astOutputTensor[0].u32Offset;

			unsigned int outLen = u32OutOffset + u32OutKNormNum * u32OutKStep + u32OutHeight *u32OutRowStep + u32OutWidth*u32OutKSizeLast *(u32OutPrecision/8);
			xDebugPrint("outLen %x\r\n",outLen);

			*(unsigned int*)pServerTargetResultMemLen=outLen;//4505600;
			unsigned char* pOut = (unsigned char *)pServerTargetResultMem + 8;

			ret=xInferenceCallBack((char*)pIn,(char*)pOut,CallBackParam,CustomerParam);

			vInferenceCallBackAck(pServerTargetResultMem, ret);
			vInferenceTaskAndFlagClean(pServerTargetFlag,pServerTargetTaskMem,pServerTargetTaskMemLen);

			xDebugPrint("Task: CmdOpFInfer Done\r\n");

			break;

	 }
 }

