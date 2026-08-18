/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"


AR_S32 ARCnnParseLrn(const void* JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseLrn\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_LRN_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_LRN_PARAM_S);
	*pOpParams = (AR_NPU_LRN_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_LRN_PARAM_S *pLrnParam = (AR_NPU_LRN_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		xDebugPrint("pOperatorParamNode is NULL.\n");
		return -1;
	}

    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dAlphaDivSize, pOperatorParamNode, "alpha_div_size");
    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dBeta, pOperatorParamNode, "beta");
    JSON_PARSE_DOUBLE_VALUE(pLrnParam, dK, pOperatorParamNode, "k");
    JSON_PARSE_INT_VALUE(pLrnParam, u32LocalSize, pOperatorParamNode, "local_size");
    JSON_PARSE_STR_VALUE(pLrnParam, achNormRegion, pOperatorParamNode, "norm_region");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



AR_CNN_PARSE_DECLARE(ARCnnParseLrn) = {
    .pName = "lrn",
    .pCnnParseHandler = ARCnnParseLrn,
};

