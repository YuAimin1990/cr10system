/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 ARCnnParseUpsample(const void *JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen)
{
	xDebugPrint("ARCnnParseUpsample\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pOpParamLen!=NULL)
	{
		*pOpParamLen = sizeof(AR_NPU_UPSAMPLE_PARAM_S);
	}

    AR_U32 u32Size = sizeof(AR_NPU_UPSAMPLE_PARAM_S);
	*pOpParams = (AR_NPU_UPSAMPLE_PARAM_S*)malloc(u32Size);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32Size);
	AR_NPU_UPSAMPLE_PARAM_S *pUpsampleParam = (AR_NPU_UPSAMPLE_PARAM_S*) *pOpParams;


	cJSON *pOperatorParamNode = (cJSON *)JsonNode;
	if(!pOperatorParamNode)
	{
		xDebugPrint("pOperatorParamNode is NULL.\n");
		return -1;
	}

	JSON_PARSE_INT_VALUE(pUpsampleParam, u32CoordTransMode, pOperatorParamNode, "coordinate_transformation_mode");
	JSON_PARSE_STR_VALUE(pUpsampleParam, achMode, pOperatorParamNode, "mode");
	JSON_PARSE_INT_VALUE(pUpsampleParam, u32NearestMode, pOperatorParamNode, "nearest_mode");

	//cJSON_Delete(pOperatorParamNode);
	return 0;
}



AR_CNN_PARSE_DECLARE(ARCnnParseUpsample) = {
    .pName = "upsample",
    .pCnnParseHandler = ARCnnParseUpsample,
};

