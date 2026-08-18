/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 ARCnnParseMaxUnpool(const void* JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseMaxUnpool\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_MAXUNPOOL_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_MAXUNPOOL_PARAM_S);
	*pOpParams = (AR_NPU_MAXUNPOOL_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_MAXUNPOOL_PARAM_S *pMaxUnPoolParam = (AR_NPU_MAXUNPOOL_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		xDebugPrint("pOperatorParamNode is NULL.\n");
		return -1;
	}

	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolKernelC, pOperatorParamNode, "maxunpool_kernel_c");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolKernelH, pOperatorParamNode, "maxunpool_kernel_h");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolKernelW, pOperatorParamNode, "maxunpool_kernel_w");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadB, pOperatorParamNode, "maxunpool_pad_b");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadL, pOperatorParamNode, "maxunpool_pad_l");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadO, pOperatorParamNode, "maxunpool_pad_o");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, bMaxUnPoolPadOutH, pOperatorParamNode, "maxunpool_pad_out_h");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, bMaxUnPoolPadOutW, pOperatorParamNode, "maxunpool_pad_out_w");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadR, pOperatorParamNode, "maxunpool_pad_r");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadT, pOperatorParamNode, "maxunpool_pad_t");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolPadU, pOperatorParamNode, "maxunpool_pad_u");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolStrideC, pOperatorParamNode, "maxunpool_stride_c");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolStrideH, pOperatorParamNode, "maxunpool_stride_h");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32MaxUnPoolStrideW, pOperatorParamNode, "maxunpool_stride_w");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolKernelC, pOperatorParamNode, "pool_kernel_c");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolKernelH, pOperatorParamNode, "pool_kernel_h");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolKernelW, pOperatorParamNode, "pool_kernel_w");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadB, pOperatorParamNode, "pool_pad_b");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadL, pOperatorParamNode, "pool_pad_l");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadO, pOperatorParamNode, "pool_pad_o");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadR, pOperatorParamNode, "pool_pad_r");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadT, pOperatorParamNode, "pool_pad_t");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolPadU, pOperatorParamNode, "pool_pad_u");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolStrideC, pOperatorParamNode, "pool_stride_c");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolStrideH, pOperatorParamNode, "pool_stride_h");
	JSON_PARSE_INT_VALUE(pMaxUnPoolParam, u32PoolStrideW, pOperatorParamNode, "pool_stride_w");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}

AR_CNN_PARSE_DECLARE(ARCnnParseMaxUnpool) = {
    .pName = "maxunpool",
    .pCnnParseHandler = ARCnnParseMaxUnpool,
};

