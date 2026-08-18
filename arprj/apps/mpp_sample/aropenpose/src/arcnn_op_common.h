#ifndef __ARCNN_OP_COMMON_H__
#define __ARCNN_OP_COMMON_H_

#define DBL_MAX 1.79e308

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) > (b) ? (b) : (a))

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


void dcnn_exec_scale(char* input_data, float* output_data, int data_size, float scale_factor, int zero_point, int precision);
void dcnn_exec_quantization(float * float_data, char* quant_data, int data_size, float scale_factor, int zero_point, int precision);
//void dcnn_exec_limit(char* data, long long data_size, int positive_limit_value, int negative_limit_value, int precision);
void dcnn_hwc_to_chw(char * hwc, char * chw,  AR_NPU_TENSOR_S * pTensor);
void dcnn_chw_to_hwc(char * chw, char * hwc,  AR_NPU_TENSOR_S * pTensor);
void NearestCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, AR_U32  u32CoordTransMode, AR_U32 u32NearestMode);
void LinearCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, float* alpha, AR_U32 u32CoordTransMode);
void CubicCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, float* alpha);
int execNearestUpsample(const float* input_float_data, float* output_float_data, int* xofs, int* yofs,
	const int batch_size, const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);
int execBilinearUpsample(const float* input_float_data, float* output_float_data, float* alpha, int* xofs, float* beta, int* yofs, const int batch_size,
	const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);
int execBicubicUpsample(const float* input_float_data, float* output_float_data, float* alpha, int* xofs, float* beta, int* yofs, const int batch_size,
        const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width);


#endif

