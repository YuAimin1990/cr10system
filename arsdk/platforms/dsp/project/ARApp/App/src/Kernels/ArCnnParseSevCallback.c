/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"


AR_S32 ARCnnParseSevCallback(const void *JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseSevCallback\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_SEVCALLBACK_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_SEVCALLBACK_PARAM_S);
	*pOpParams = (AR_NPU_SEVCALLBACK_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_SEVCALLBACK_PARAM_S *pSevParam = (AR_NPU_SEVCALLBACK_PARAM_S*) *pOpParams;

	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		xDebugPrint("pOperatorParamNode is NULL.\n");
		return -1;
	}

	JSON_PARSE_STR_ARRAY_VALUE(pSevParam, achLayerType, pOperatorParamNode, "layer_type");
	JSON_PARSE_INT_VALUE(pSevParam, u32CoordTransMode, pOperatorParamNode, "coordinate_transformation_mode");
	JSON_PARSE_STR_VALUE(pSevParam, achMode, pOperatorParamNode, "mode");
	JSON_PARSE_INT_VALUE(pSevParam, u32NearestMode, pOperatorParamNode, "nearest_mode");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleInC, pOperatorParamNode, "upsample_in_c");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleInH, pOperatorParamNode, "upsample_in_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleInW, pOperatorParamNode, "upsample_in_w");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleOutC, pOperatorParamNode, "upsample_out_c");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleOutH, pOperatorParamNode, "upsample_out_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32UpsampleOutW, pOperatorParamNode, "upsample_out_w");

	JSON_PARSE_INT_VALUE(pSevParam, u32PoolCrossH, pOperatorParamNode, "pool_cross_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolCrossW, pOperatorParamNode, "pool_cross_w");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolInC, pOperatorParamNode, "pool_in_c");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolInH, pOperatorParamNode, "pool_in_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolInW, pOperatorParamNode, "pool_in_w");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolKernelH, pOperatorParamNode, "pool_kernel_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolKernelW, pOperatorParamNode, "pool_kernel_w");
	JSON_PARSE_STR_VALUE(pSevParam, achPoolMethod, pOperatorParamNode, "pool_method");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolOutC, pOperatorParamNode, "pool_out_c");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolOutH, pOperatorParamNode, "pool_out_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolOutW, pOperatorParamNode, "pool_out_w");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadB, pOperatorParamNode, "pool_pad_b");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadB1, pOperatorParamNode, "pool_pad_b_1");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadL, pOperatorParamNode, "pool_pad_l");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadL1, pOperatorParamNode, "pool_pad_l_1");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadR, pOperatorParamNode, "pool_pad_r");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadR1, pOperatorParamNode, "pool_pad_r_1");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadT, pOperatorParamNode, "pool_pad_t");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolPadT1, pOperatorParamNode, "pool_pad_t_1");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolStrideH, pOperatorParamNode, "pool_stride_h");
	JSON_PARSE_INT_VALUE(pSevParam, u32PoolStrideW, pOperatorParamNode, "pool_stride_w");
	JSON_PARSE_STR_VALUE(pSevParam, achPoolAvePadMethod, pOperatorParamNode, "pooling_ave_padding_cal_method");


	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


AR_CNN_PARSE_DECLARE(ARCnnParseSevCallback) = {
    .pName = "sevcallback",
    .pCnnParseHandler = ARCnnParseSevCallback,
};

