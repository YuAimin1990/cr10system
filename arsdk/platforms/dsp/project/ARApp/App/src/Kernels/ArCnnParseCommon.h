

#ifndef __ARCNN_PARSE_COMMON_H__
#define __ARCNN_PARSE_COMMON_H__

#ifdef __cplusplus
extern "C"{
#endif


#include "ArNpuPlatform.h"

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
	AR_U32 u32CoordTransMode;
	AR_U32 u32NearestMode;
	AR_CHAR achMode[MAX_NAME_LEN];
} AR_NPU_UPSAMPLE_PARAM_S;

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
}AR_NPU_SEVCALLBACK_PARAM_S;


//custom-LRN
typedef struct {
	AR_U32 u32Size;
	AR_DOUBLE dAlpha;
	AR_DOUBLE dBeta;
	AR_DOUBLE dBias;
} AR_NPU_CUSTOM_LRN_PARAM_S;

//Concat operator params
typedef struct
{
	AR_U32 u32Axis;

} AR_NPU_CONCAT_PARAM_S;

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

#ifdef __cplusplus
}
#endif

#endif

