#ifndef __AR_CNN_OPERATOR_H__
#define __AR_CNN_OPERATOR_H__


#include "hal_npu_types.h"

#ifdef __cplusplus
extern "C"{
#endif

AR_S32 AR_CNN_OperatorLrn(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorOnnxLrn(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorGetConf(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorMaxUnPool(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorReciprocal(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorOnnxReshape(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
//Concat
AR_S32 AR_CNN_OperatorConcat(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pCBParamsExt);
AR_S32 AR_CNN_OperatorUpsample(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorResize(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorDeformable(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorSevCallback(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorFeatureToWeight(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S* pCBParams, void* pOpParams);
AR_S32 AR_CNN_OperatorPad(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorPermute(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorTranspose(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);

AR_S32 AR_CNN_OperatorCustomLrn(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams);
AR_S32 AR_CNN_OperatorCustomAdd(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pCBParamsExt);
AR_S32 AR_CNN_OperatorCustomPad(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pCBParamsExt);


#ifdef __cplusplus
}
#endif

#endif
