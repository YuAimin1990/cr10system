

#ifndef __ARCNN_OPERATOR_COMMON_H__
#define __ARCNN_OPERATOR_COMMON_H__

#ifdef __cplusplus
extern "C"{
#endif


#include "ArDspPlatform.h"
#include "ArNpuPlatform.h"



#define DBL_MAX 1.79e308

#define SHRT_MIN    (-32768)        /* minimum (signed) short value */
#define SHRT_MAX      32767         /* maximum (signed) short value */


#define SDP_FIXED_SCALE_QUANT_BITS		32
#define SDP_SHIFT_MAX_VALUE				32 //6bit

typedef enum
{
	E_DCNN_LRN_ACROSS_CHANNELS,
	E_DCNN_LRN_WITHIN_CHANNELS
}E_DCNN_LRN_NORM_REGION;
enum E_DCNN_UPSAMPLE_MODE
{
	E_DCNN_UPSAMPLE_NEAREST,
	E_DCNN_UPSAMPLE_BILINEAR,
	E_DCNN_UPSAMPLE_BICUBIC
};

enum E_DCNN_UPSAMPLE_COORDINATE_TRANSFORMATION_MODE
{
	E_DCNN_UPSAMPLE_HALF_PIXEL,
	E_DCNN_UPSAMPLE_PYTORCH_HALF_PIXEL,
	E_DCNN_UPSAMPLE_ALIGN_CORNERS,
	E_DCNN_UPSAMPLE_ASYMMETRIC,
	E_DCNN_UPSAMPLE_TF_HALF_PIEXL_FOR_NN,
	E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE
};

enum E_DCNN_UPSAMPLE_NEAREST_MODE
{
	E_DCNN_UPSAMPLE_ROUND_PREFER_FLOOR,
	E_DCNN_UPSAMPLE_ROUND_PREFER_CEIL,
	E_DCNN_UPSAMPLE_FLOOR,
	E_DCNN_UPSAMPLE_CEIL
};


void dcnn_exec_scale(char* input_data, double* output_data, int data_size, double scale_factor, int zero_point, int precision);
void dcnn_exec_quantization(double * float_data, char* quant_data, int data_size, double scale_factor, int zero_point, int precision);
//void dcnn_exec_limit(char* data, long long data_size, int positive_limit_value, int negative_limit_value, int precision);
void dcnn_hwc_to_chw(char * hwc, char * chw,  AR_NPU_TENSOR_S * pTensor);
void dcnn_chw_to_hwc(char * chw, char * hwc,  AR_NPU_TENSOR_S * pTensor);
void NearestCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, AR_U32  u32CoordTransMode, AR_U32 u32NearestMode);
void LinearCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, double* alpha, AR_U32 u32CoordTransMode);
void CubicCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, double* alpha);
int execNearestUpsample(const double* input_float_data, double* output_float_data, int* xofs, int* yofs,
	const int batch_size, const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);
int execBilinearUpsample(const double* input_float_data, double* output_float_data, double* alpha, int* xofs, double* beta, int* yofs, const int batch_size,
	const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);
int execBicubicUpsample(const double* input_float_data, double* output_float_data, double* alpha, int* xofs, double* beta, int* yofs, const int batch_size,
        const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);


#ifdef __cplusplus
}
#endif

#endif

