#include "ar_cnn_parser.h"

AR_S32 AR_CNN_ParseCustomLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pParamLen)
{
    printf("AR_CNN_ParseCustomLrn\r\n");
	if ((!pJsonNode) || (u32JsonLen==0))
	{
		printf("pJsonNode or u32JsonLen err \r\n");
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

	cJSON *pOperatorParamNode =(cJSON *)pJsonNode;
    AR_CHAR *pJsonString = pOperatorParamNode->valuestring;
	cJSON *pJson = cJSON_Parse(pJsonString);
	if(!pJson)
	{
		printf("Parse json file failed.\n");
		return -1;
	}

	JSON_PARSE_INT_VALUE(pCustomLrnParams, u32Size, pJson, "size");
    JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dAlpha, pJson, "alpha");
	JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dBeta, pJson, "beta");
    JSON_PARSE_DOUBLE_VALUE(pCustomLrnParams, dBias, pJson, "bias");

	cJSON_Delete(pJson);
	return 0;
}



