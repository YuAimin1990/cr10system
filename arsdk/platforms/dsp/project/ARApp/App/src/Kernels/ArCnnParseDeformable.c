/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 ARCnnParseDeformable(const void *JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseDeformable\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_DEFORMABLE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_DEFORMABLE_PARAM_S);
	*pOpParams = (AR_NPU_DEFORMABLE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_DEFORMABLE_PARAM_S *pDeformableParam = (AR_NPU_DEFORMABLE_PARAM_S*) *pOpParams;

	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		printf("pOperatorParamNode is NULL.\n");
		return -1;
	}

	JSON_PARSE_STR_VALUE(pDeformableParam, achConvMode, pOperatorParamNode, "conv_mode");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32DeformableGroup, pOperatorParamNode, "deformable_group");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32DilateH, pOperatorParamNode, "dilation_h");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32DilateW, pOperatorParamNode, "dilation_w");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32Group, pOperatorParamNode, "group");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32KernelH, pOperatorParamNode, "kernel_h");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32KernelW, pOperatorParamNode, "kernel_w");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadB, pOperatorParamNode, "pad_b");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadL, pOperatorParamNode, "pad_l");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadR, pOperatorParamNode, "pad_r");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadT, pOperatorParamNode, "pad_t");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadX, pOperatorParamNode, "pad_x");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32PadY, pOperatorParamNode, "pad_y");
	JSON_PARSE_DOUBLE_VALUE(pDeformableParam, dRescaleFactor, pOperatorParamNode, "rescale_factor");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32StrideH, pOperatorParamNode, "stride_h");
	JSON_PARSE_INT_VALUE(pDeformableParam, u32StrideW, pOperatorParamNode, "stride_w");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}


AR_CNN_PARSE_DECLARE(ARCnnParseDeformable) = {
    .pName = "deformable",
    .pCnnParseHandler = ARCnnParseDeformable,
};

