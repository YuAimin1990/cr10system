#ifndef __AR_CNN_CHANNELCVT_H__
#define __AR_CNN_CHANNELCVT_H__

#include "hal_npu_types.h"

#ifdef __cplusplus
extern "C"{
#endif
void AR_NPU_Outbuff_NhwcCvtNchw(AR_CHAR* pcNpuOutput, AR_CHAR* pcNchwOutput, AR_NPU_TENSOR_S* pstTensor);
AR_S32 AR_NPU_Exe_Softmax_With_NhwcCvtNchw_Neon(AR_CHAR* pcNpuOutput, AR_CHAR * pcSoftmaxOutput, AR_NPU_TENSOR_S* pstTensor);
void AR_NPU_Exe_NchwCvtNhwc_F32_Neon(AR_FLOAT * pInput, AR_FLOAT * pOutput, AR_NPU_TENSOR_S * pTensor);
void AR_NPU_Exe_NhwcCvtNchw_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor);
void AR_NPU_Exe_NchwCvtNhwc_Fix_Neon(AR_CHAR * pInput, AR_CHAR * pOutput,AR_NPU_TENSOR_S * pTensor);
void AR_NPU_Exe_NchwCvtNhwc_Fix(AR_CHAR * pInput, AR_CHAR * pOutput, AR_NPU_TENSOR_S * pTensor);
void AR_NPU_Exe_NhwcCvtNchw_Fix(AR_CHAR * pInput, AR_CHAR * pOutput, AR_NPU_TENSOR_S * pTensor);
void AR_NPU_FloatCvtFix(AR_FLOAT* pfInput, AR_CHAR* pcOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor);
void AR_NPU_FixCvtFloat(AR_CHAR* pcInput, AR_FLOAT* pfOutput, AR_U32 u32DataSize, AR_NPU_TENSOR_S * pstTensor);
void AR_NPU_FloatCvtFix_With_Stride_Neon(AR_FLOAT * pfInput, AR_CHAR* pcOutput, AR_NPU_TENSOR_S * pstTensor);
void AR_NPU_FloatCvtFix_With_Stride(AR_FLOAT * pfInput, AR_CHAR* pcOutput, AR_NPU_TENSOR_S * pstTensor);
void AR_ConvertDump_YUV420PImg(AR_IMG_S *SrcImg, AR_CHAR *DstBuf);

#ifdef __cplusplus
}
#endif

#endif


