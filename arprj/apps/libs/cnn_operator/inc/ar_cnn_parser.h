#ifndef __AR_CNN_PARSER_H__
#define __AR_CNN_PARSER_H__

#include "cJSON.h"
#include "hal_npu_types.h"

#ifdef __cplusplus
extern "C"{
#endif


#define JSON_PARSE_INT_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        structptr->member = pNode->valueint;\
    }\
}while(0)

#define JSON_PARSE_DOUBLE_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        structptr->member = pNode->valuedouble;\
    }\
}while(0)

#define JSON_PARSE_STR_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        strcpy(structptr->member, pNode->valuestring);\
    }\
}while(0)

#define JSON_PARSE_STR_ARRAY_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        AR_U32 u32ArraySize = cJSON_GetArraySize(pNode);\
        for(int idx = 0; idx < u32ArraySize; idx++)\
        {\
            cJSON * pSub = cJSON_GetArrayItem(pNode, idx);\
            if (!pSub) continue;\
            strcpy(structptr->member[idx], pSub->valuestring); \
        }\
    }\
}while(0)

#define JSON_PARSE_INT_ARRAY_VALUE(structptr, member, node, name) \
do{\
    cJSON * pNode = cJSON_GetObjectItemCaseSensitive(node, name);\
    if(pNode)\
    {\
        AR_U32 u32ArraySize = cJSON_GetArraySize(pNode);\
        for(int idx = 0; idx < u32ArraySize; idx++)\
        {\
            cJSON * pSub = cJSON_GetArrayItem(pNode, idx);\
            if (!pSub) continue;\
            structptr->member[idx]= pSub->valueint; \
        }\
    }\
}while(0)


// std-callback operator params
typedef struct
{
	AR_U32 u32MaxUnPoolKernelC;
	AR_U32 u32MaxUnPoolKernelH;
	AR_U32 u32MaxUnPoolKernelW;
	AR_U32 u32MaxUnPoolPadB;
	AR_U32 u32MaxUnPoolPadL;
	AR_U32 u32MaxUnPoolPadO;
	AR_BOOL bMaxUnPoolPadOutH;
	AR_BOOL bMaxUnPoolPadOutW;
	AR_U32 u32MaxUnPoolPadR;
	AR_U32 u32MaxUnPoolPadT;
	AR_U32 u32MaxUnPoolPadU;
	AR_U32 u32MaxUnPoolStrideC;
	AR_U32 u32MaxUnPoolStrideH;
	AR_U32 u32MaxUnPoolStrideW;

	AR_U32 u32PoolKernelC;
	AR_U32 u32PoolKernelH;
	AR_U32 u32PoolKernelW;
	AR_U32 u32PoolPadB;
	AR_U32 u32PoolPadL;
	AR_U32 u32PoolPadO;
	AR_U32 u32PoolPadR;
	AR_U32 u32PoolPadT;
	AR_U32 u32PoolPadU;
	AR_U32 u32PoolStrideC;
	AR_U32 u32PoolStrideH;
	AR_U32 u32PoolStrideW;
} AR_NPU_MAXUNPOOL_PARAM_S;

typedef struct
{
	AR_DOUBLE dAlphaDivSize;
	AR_DOUBLE dBeta;
	AR_DOUBLE dK;
	AR_U32 u32LocalSize;
	AR_CHAR achNormRegion[MAX_NAME_LEN];
} AR_NPU_LRN_PARAM_S;

typedef struct
{
	AR_DOUBLE dAlpha;
	AR_DOUBLE dBeta;
	AR_DOUBLE dBias;
	AR_U32 u32LocalSize;
} AR_NPU_ONNX_LRN_PARAM_S;


typedef struct
{
	AR_U32 u32CoordTransMode;
	AR_U32 u32NearestMode;
	AR_CHAR achMode[MAX_NAME_LEN];
} AR_NPU_UPSAMPLE_PARAM_S;

typedef struct
{
	AR_CHAR achCoordTransMode[MAX_NAME_LEN];
	AR_CHAR achNearestMode[MAX_NAME_LEN];
	AR_CHAR achMode[MAX_NAME_LEN];
} AR_NPU_RESIZE_PARAM_S;


typedef struct
{
	AR_CHAR achConvMode[MAX_NAME_LEN];
	AR_U32 u32DeformableGroup;
	AR_U32 u32DilateH;
	AR_U32 u32DilateW;
	AR_U32 u32Group;
	AR_U32 u32KernelH;
	AR_U32 u32KernelW;
	AR_U32 u32PadB;
	AR_U32 u32PadL;
	AR_U32 u32PadR;
	AR_U32 u32PadT;
	AR_U32 u32PadX;
	AR_U32 u32PadY;
	AR_DOUBLE dRescaleFactor;
	AR_U32 u32StrideH;
	AR_U32 u32StrideW;
} AR_NPU_DEFORMABLE_PARAM_S;

typedef struct
{
	AR_CHAR achPadMode[MAX_NAME_LEN];
	AR_S32 pad_t;
	AR_S32 pad_l;
	AR_S32 pad_b;
	AR_S32 pad_r;
	AR_U32 u32PadTensor[8]; 
	AR_DOUBLE val;
	AR_U32 u32PadOpVer;
} AR_NPU_PAD_PARAM_S;

//------------------------
//Permute operator params
typedef struct
{
	AR_CHAR sOrder_type[MAX_NAME_LEN];
} AR_NPU_PERMUTE_PARAM_S;

//------------------------
//Transpose operator params
typedef struct
{
  	AR_U32 u32Perm[MAX_NAME_LEN];
	AR_U32 u32Perm_size;	
} AR_NPU_TRANSPOSE_PARAM_S;


typedef struct
{
	AR_CHAR achLayerType[2][MAX_NAME_LEN];
	AR_U32 u32CoordTransMode;
	AR_U32 u32NearestMode;
	AR_CHAR achMode[MAX_NAME_LEN];
	AR_U32 u32UpsampleInC;
	AR_U32 u32UpsampleInH;
	AR_U32 u32UpsampleInW;
	AR_U32 u32UpsampleOutC;
	AR_U32 u32UpsampleOutH;
	AR_U32 u32UpsampleOutW;

	AR_U32 u32PoolCrossH;
	AR_U32 u32PoolCrossW;
	AR_U32 u32PoolInC;
	AR_U32 u32PoolInH;
	AR_U32 u32PoolInW;
	AR_U32 u32PoolKernelH;
	AR_U32 u32PoolKernelW;
	AR_CHAR achPoolMethod[MAX_NAME_LEN];
	AR_U32 u32PoolOutC;
	AR_U32 u32PoolOutH;
	AR_U32 u32PoolOutW;
	AR_U32 u32PoolPadB;
	AR_U32 u32PoolPadB1;
	AR_U32 u32PoolPadL;
	AR_U32 u32PoolPadL1;
	AR_U32 u32PoolPadR;
	AR_U32 u32PoolPadR1;
	AR_U32 u32PoolPadT;
	AR_U32 u32PoolPadT1;
	AR_U32 u32PoolStrideH;
	AR_U32 u32PoolStrideW;
	AR_CHAR achPoolAvePadMethod[MAX_NAME_LEN];
} AR_NPU_SEVCALLBACK_PARAM_S;

typedef struct
{
	//AR_U64 u64CdmaKernelBytesToal;
	AR_U32 u32CdmaKernelBytesToal;
	AR_U32 u32CdmaKernelKBytesNorm;
	AR_U32 *u32CdmaKernelKBytesVec;
	AR_U32 *u32CdmaKernelKSizeVec;
	AR_U32 u32CdmaKernelKNormNum;
	AR_U32 u32CdmaKernelKSizeLast;
	AR_U32 u32CdmaKerneLKSizeNorm;

	AR_U32 u32Precision;
	AR_U32 u32ConvCmacKernelKNorm;
	AR_U32 u32ConvFoldingBit;
	AR_U32 u32ConvFtFoldingNum;
	AR_U32 u32ConvGroupInputChannel;
	AR_U32 u32ConvGroupNum;
	AR_U32 u32ConvGroupOuutputChannel;
	AR_U32 u32ConvKernelH;
	AR_U32 u32ConvKernelW;
	AR_U32 u32ConvKernelSplitNum;

	AR_BOOL bConvGroupPartitionEn;
	AR_BOOL bConvWtPartitionEn;
	AR_BOOL bConvDeconvEn;
	AR_BOOL bConvDeconvOptimizeEn;

	AR_CHAR achFtwMethod[MAX_NAME_LEN];
	AR_CHAR achConvMode[MAX_NAME_LEN];
	//AR_CHAR achOperatorChannelMode[MAX_NAME_LEN];
	//AR_CHAR achOperatorType[MAX_NAME_LEN];

} AR_NPU_FEATURE_TO_WEIGHT_PARAM_S;

//usr-defined callback operator params
//custom-LRN
typedef struct
{
	AR_U32 u32Size;
	AR_DOUBLE dAlpha;
	AR_DOUBLE dBeta;
	AR_DOUBLE dBias;
} AR_NPU_CUSTOM_LRN_PARAM_S;

typedef struct
{
	AR_S32 bias;
}AR_NPU_CUSTOMADD_PARAM_S;

typedef struct
{
	AR_S32 add;
	AR_S32 pads[8];
	AR_DOUBLE value;
	AR_CHAR mode[MAX_NAME_LEN];
}AR_NPU_CUSTOMPAD_PARAM_S;


//Concat operator params
typedef struct
{
	AR_U32 u32Axis;
	
} AR_NPU_CONCAT_PARAM_S;
AR_S32 AR_CNN_ParseLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseOnnxLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseSevCallback(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseMaxUnpool(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseDeformable(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseUpsample(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseResize(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseFeatureToWeight(const void* pJsonNode, AR_U32 u32JsonLen, void** pOpParams, AR_U32* pOpParamLen);
AR_S32 AR_CNN_ParsePad(const void *pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseCustomLrn(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);

AR_S32 AR_CNN_ParsePermute(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseTranspose(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseCustomAdd(const void *pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);
AR_S32 AR_CNN_ParseCustomPad(const void * pJsonNode, AR_U32 u32JsonLen, void * * pOpParams, AR_U32 * pOpParamLen);

//Concat
AR_S32 AR_CNN_ParseConcat(const void* pJsonNode, AR_U32 u32JsonLen, void **pOpParams, AR_U32 *pOpParamLen);

#ifdef __cplusplus
}
#endif

#endif
