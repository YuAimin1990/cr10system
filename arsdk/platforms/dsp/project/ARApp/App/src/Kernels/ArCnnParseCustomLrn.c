/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnParseDeclare.h"
#include "ArCnnParseCommon.h"

AR_S32 ARCnnParseCustomLrn(const void* JsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pParamLen)
{
	xDebugPrint("ARCnnParseCustomLrn\r\n");
	if ((!JsonNode) || (u32JsonLen==0))
	{
		xDebugPrint("JsonNode or u32JsonLen err \r\n");
		return -1;
	}

	if(pParamLen!=NULL)
	{
		*pParamLen = sizeof(AR_NPU_CUSTOM_LRN_PARAM_S);
	}

    AR_U32 u32ParamLen = sizeof(AR_NPU_CUSTOM_LRN_PARAM_S);
	*pOpParams = (AR_NPU_CUSTOM_LRN_PARAM_S*)malloc(u32ParamLen);
	if(*pOpParams ==NULL)
 		return -1;
	memset(*pOpParams,0,u32ParamLen);
	AR_NPU_CUSTOM_LRN_PARAM_S *pCustomLrnParams = (AR_NPU_CUSTOM_LRN_PARAM_S*) *pOpParams;

	cJSON *pOperatorParamNode =(cJSON *)JsonNode;
    AR_CHAR *pJsonString = pOperatorParamNode->valuestring;
	cJSON *pJson = cJSON_Parse(pJsonString);
	if(!pJson)
	{
		xDebugPrint("Parse json file failed.\n");
		return -1;
	}

	JSON_PARSE_INT_VALUE(pCustomLrnParams, u32Size, pJson, "size");
    JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dAlpha, pJson, "alpha");
	JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dBeta, pJson, "beta");
    JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dBias, pJson, "bias");

	cJSON_Delete(pJson);
	return 0;
}


AR_CNN_PARSE_DECLARE(ARCnnParseCustomLrn) = {
    .pName = "LRN",
    .pCnnParseHandler = ARCnnParseCustomLrn,
};

