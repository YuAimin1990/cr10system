#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
//#include <arm_neon.h>
#include <vec-c.h>
#include "Uart.h"
#include "ArCnnOperatorCommon.h"

#if 0 //The following neon code runs a bit slower than C code, so we use C code.
void dcnn_exec_scale(char* input_data, float* output_data, int data_size, float scale_factor, int zero_point, int precision)
{
    int i = 0;
    int loop = data_size / 4;
    int left = data_size % 4;

    if(precision == 16)
    {
        int16x4_t vzeropoint = vdup_n_s16((int16_t)zero_point);
        //float32x4_t vscale = vdupq_n_f32((float32_t)scale_factor);

        signed short * input = (signed short *)input_data;
        for (i = 0; i < loop; ++i)
        {
            int16x4_t vdata = vld1_s16(input);
            int32x4_t vtmp = vsubl_s16(vdata, vzeropoint);
            float32x4_t vtmp1 = vcvtq_f32_s32(vtmp);
            float32x4_t vres = vmulq_n_f32(vtmp1, scale_factor);
            vst1q_f32(output_data, vres);
            input += 4;
            output_data += 4;
            //output_data[ind] = (input[ind] - zero_point) * scale_factor;
        }

        for(i = 0; i < left; i++)
        {
            *output_data++ = (*input++ - zero_point) * scale_factor;
        }
    }
    else
    {
        //signed char * input = (signed char *)input_data;
        int loop = data_size / 8;
        int left = data_size % 8;

        int8x8_t vzeropoint = vdup_n_s8((int8_t)zero_point);

        for (i = 0; i < loop; ++i)
        {
            int8x8_t vdata = vld1_s8((int8_t *)input_data);
            int16x8_t vtmp = vsubl_s8(vdata, vzeropoint);
            int16x4_t vlow = vget_low_s16(vtmp);
            int16x4_t vhigh = vget_high_s16(vtmp);
            int32x4_t vlow32 = vmovl_s16(vlow);
            int32x4_t vhigh32 = vmovl_s16(vhigh);

            float32x4_t vflow = vcvtq_f32_s32(vlow32);
            float32x4_t vfhigh = vcvtq_f32_s32(vhigh32);
            float32x4_t vreslow = vmulq_n_f32(vflow, scale_factor);
            float32x4_t vreshigh = vmulq_n_f32(vfhigh, scale_factor);

            vst1q_f32(output_data, vreslow);
            output_data += 4;
            vst1q_f32(output_data, vreshigh);
            output_data += 4;

            input_data += 8;
        }

        for(i = 0; i < left; i++)
        {
            *output_data++ = ((signed char)(*input_data++) - zero_point) * scale_factor;
        }
    }
}

void dcnn_exec_quantization(float* float_data, char* quant_data, int data_size, float scale_factor, int zero_point, int precision)
{
    int i = 0;
	int positive_limit = (int)pow((float)2, (float)(precision - 1)) - 1;
	int negative_limit = (int)-pow((float)2, (float)(precision - 1));

    if(precision == 16)
    {
        int loop = data_size / 4;
        int left = data_size % 4;

        short * quant = (short *)quant_data;
        float32x4_t vscale = vdupq_n_f32(scale_factor);
        int16x4_t vzero = vdup_n_s16((int16_t)zero_point);
        int16x4_t vpos = vdup_n_s16((int16_t)positive_limit);
        int16x4_t vneg = vdup_n_s16((int16_t)negative_limit);
        for (i = 0; i < loop; ++i)
        {
            float32x4_t vfin = vld1q_f32(float_data);
            float32x4_t vftmp = vdivq_f32(vfin, vscale);
            int16x4_t vs16tmp = vqmovn_s32( vcvtq_s32_f32(vftmp));
            int16x4_t vrestmp = vadd_s16(vs16tmp, vzero);
            int16x4_t vres = vmax_s16(vmin_s16(vrestmp, vpos), vneg);
            vst1_s16(quant, vres);
            float_data += 4;
            quant += 4;
        }

        for(i = 0; i < left; i++)
        {
            quant[i] = (short)round(*float_data++ / scale_factor) + zero_point;
            quant[i] = MIN(quant[i], positive_limit);
            quant[i] = MAX(quant[i], negative_limit);
        }
    }
    else
    {
        int loop = data_size / 8;
        int left = data_size % 8;
        float32x4_t vscale = vdupq_n_f32(scale_factor);
        //float32x4_t vrecip = vrecpeq_f32(vscale);
        int8x8_t vzero = vdup_n_s8((int8_t)zero_point);
        int8x8_t vpos = vdup_n_s8((int8_t)positive_limit);
        int8x8_t vneg = vdup_n_s8((int8_t)negative_limit);

        for (i = 0; i < loop; ++i)
        {
            float32x4_t vfin0 = vld1q_f32(float_data);
            float_data += 4;
            float32x4_t vfin1 = vld1q_f32(float_data);
            float32x4_t vftmp0 = vdivq_f32(vfin0, vscale);
            float32x4_t vftmp1 = vdivq_f32(vfin1, vscale);
            int16x4_t vs16tmp0 = vqmovn_s32(vcvtq_s32_f32(vftmp0));
            int16x4_t vs16tmp1 = vqmovn_s32(vcvtq_s32_f32(vftmp1));
            int8x8_t vs8tmp = vqmovn_s16(vcombine_s16(vs16tmp0, vs16tmp1));
            int8x8_t vrestmp = vadd_s8(vs8tmp, vzero);
            int8x8_t vres = vmax_s8(vmin_s8(vrestmp, vpos), vneg);
            vst1_s8((int8_t *)quant_data, vres);
            float_data += 4;
            quant_data += 8;
        }

        for(i = 0; i < left; i++)
        {
            quant_data[i] = (char)round(float_data[i] / scale_factor) + zero_point;
            quant_data[i] = MIN(quant_data[i], positive_limit);
            quant_data[i] = MAX(quant_data[i], negative_limit);
        }
    }
}

#endif

void dcnn_exec_scale(char* input_data, double* output_data, int data_size, double scale_factor, int zero_point, int precision)
{
	for (int ind = 0; ind < data_size; ++ind)
	{
		if (precision == 16)
		{
		    signed short * input = (signed short *)input_data;
            output_data[ind] = (input[ind] - zero_point) * scale_factor;
		}
		else
		{
    		signed char * input = (signed char *)input_data;
		    output_data[ind] = (input[ind] - zero_point) * scale_factor;
		}
	}
}
void dcnn_exec_quantization(double* float_data, char* quant_data, int data_size, double scale_factor, int zero_point, int precision)
{
	int positive_limit = (int)pow((double)2, (double)(precision - 1)) - 1;
	int negative_limit = (int)-pow((double)2, (double)(precision - 1));
    int tmp = 0;

    if(precision == 16)
    {
        signed short * quant = (signed short *)quant_data;

        for (int i = 0; i < data_size; ++i)
        {
            tmp = round(float_data[i] / scale_factor) + zero_point;
            quant[i] = MIN(tmp, positive_limit);
            quant[i] = MAX(quant[i], negative_limit);
        }
    }
    else
    {
	    signed char * quant = (signed char *)quant_data;
		
        for (int i = 0; i < data_size; ++i)
        {
            tmp = round(float_data[i] / scale_factor) + zero_point;
            quant[i] = MIN(tmp, positive_limit);
            quant[i] = MAX(quant[i], negative_limit);
        }
    }
}


void dcnn_hwc_to_chw(char * hwc, char * chw, AR_NPU_TENSOR_S * pTensor)
{
	int c = 0, w = 0, h = 0, k = 0;
	unsigned int width = pTensor->u32Width;
	unsigned int height = pTensor->u32Height;
	unsigned int k_size_norm = pTensor->u32KSizeNorm;
	unsigned int k_norm_num = pTensor->u32KNormNum;
	unsigned int k_step = pTensor->u32KStep;
	unsigned int row_ddr_step = pTensor->u32RowStep;
	unsigned int precision = pTensor->u32Precision;
	//unsigned int pixel_byte = precision / 8;
	unsigned int k_size_last = pTensor->u32KSizeLast;
	unsigned int batch = pTensor->u32Num;
	//int channels_norm = k_size_norm * k_norm_num;
	//int channels_total = channels_norm + k_size_last;
	//unsigned int hwc_tensor_step = pTensor->u32TensorStep;
	//unsigned int chw_tensor_step = width * height * channels_total * pixel_byte;

	//the norm block

	if(precision == 16)
	{
	    AR_S16 * dst = (AR_S16 *)chw;
        AR_S16 * src = (AR_S16 *)hwc;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    src = (AR_S16 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *dst++ = *(src + w * k_size_norm + c);
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    src = (AR_S16 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*dst++ = *(src + w * k_size_last + c);
				}
			}
		}
	}
    else
    {
        AR_S8 * dst = (AR_S8 *)chw;
        AR_S8 * src = (AR_S8 *)hwc;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    src = (AR_S8 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *dst++ = *(src + w * k_size_norm + c);
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    src = (AR_S8 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*dst++ = *(src + w * k_size_last + c);
				}
			}
		}
    }

}

void dcnn_chw_to_hwc(char * chw, char * hwc, AR_NPU_TENSOR_S * pTensor)
{
	int c = 0, w = 0, h = 0, k = 0;
	unsigned int width = pTensor->u32Width;
	unsigned int height = pTensor->u32Height;
	//unsigned int output_off = pCBParams->astOutputTensor[0].u32Offset;
	unsigned int k_size_norm = pTensor->u32KSizeNorm;
	unsigned int k_step = pTensor->u32KStep;
	unsigned int row_ddr_step = pTensor->u32RowStep;
	unsigned int precision = pTensor->u32Precision;
	//unsigned int pixel_byte = precision / 8;
	unsigned int k_size_last = pTensor->u32KSizeLast;
	unsigned int k_norm_num = pTensor->u32KNormNum;
	unsigned int batch = pTensor->u32Num;
	//int channels_norm = k_size_norm * k_norm_num;
	//int channels_total = channels_norm + k_size_last;
	//unsigned int hwc_tensor_step = pTensor->u32TensorStep;
	//unsigned int chw_tensor_step = width * height * channels_total * pixel_byte;

    if(precision == 16)
	{
	    AR_S16 * dst = (AR_S16 *)hwc;
        AR_S16 * src = (AR_S16 *)chw;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    dst = (AR_S16 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *(dst + w * k_size_norm + c) = *src++;
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    dst = (AR_S16 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*(dst + w * k_size_last + c) = *src++;
				}
			}
		}
	}
    else
    {
        AR_S8 * dst = (AR_S8 *)hwc;
        AR_S8 * src = (AR_S8 *)chw;
		for (k = 0; k < k_norm_num; k++)
		{
			for (c = 0; c < k_size_norm; c++)
			{
				for(h = 0; h < height * batch; h++)
				{
				    dst = (AR_S8 *)(hwc + k * k_step + h * row_ddr_step);
				    for(w = 0; w < width; w++)
                    {
                        *(dst + w * k_size_norm + c) = *src++;
                    }
				}
			}
		}

		//the last block
		for (c = 0; c < k_size_last; c++)
		{
			for (h = 0; h < height * batch; h++)
			{
			    dst = (AR_S8 *)(hwc + k_norm_num * k_step + h * row_ddr_step);
				for (w = 0; w < width; w++)
				{
					*(dst + w * k_size_last + c) = *src++;
				}
			}
		}
    }

}


void NearestCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, AR_U32  u32CoordTransMode, AR_U32 u32NearestMode)
{
	double scale = (u32CoordTransMode == E_DCNN_UPSAMPLE_ALIGN_CORNERS) ? (double)(u32InW - 1) / (u32OutW - 1) : (double)u32InW / u32OutW;

	for (int dx = 0; dx < u32OutW; dx++)
	{
		double fx = 0.;
		switch (u32CoordTransMode)
		{
		case E_DCNN_UPSAMPLE_HALF_PIXEL:
			fx = (double)((dx + 0.5) * scale - 0.5);
			break;
		case E_DCNN_UPSAMPLE_PYTORCH_HALF_PIXEL:
			fx = u32InW > 1 ? (double)((dx + 0.5) * scale - 0.5) : 0;
			break;
		case E_DCNN_UPSAMPLE_ALIGN_CORNERS: // for deeplab/deeplabv3+ resizebilinear align_corner=True
			//fx = (float)dx * scale;
			//break;
		case E_DCNN_UPSAMPLE_ASYMMETRIC:
			fx = (double)dx * scale;
			break;
		case E_DCNN_UPSAMPLE_TF_HALF_PIEXL_FOR_NN:
			fx = (double)(dx + 0.5) * scale;
			break;
		case E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE:
			printf(" E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE is not supported!\n");
			break;
		default:
			printf("coordinate_transformation_mode %d is not supported!\n", u32CoordTransMode);
			break;
		}

		int sx = 0;
		switch (u32NearestMode)
		{
		case E_DCNN_UPSAMPLE_ROUND_PREFER_FLOOR:
			if (fx == (int)(fx)+0.5)
				sx = (int)(floor(fx));
			else
				sx = (int)(round(fx));
			break;
		case E_DCNN_UPSAMPLE_ROUND_PREFER_CEIL:
			sx = (int)(round(fx));
			break;
		case E_DCNN_UPSAMPLE_FLOOR:
			sx = (int)(floor(fx));
			break;
		case E_DCNN_UPSAMPLE_CEIL:
			sx = (int)(ceil(fx));
			break;
		default:
			printf("nearest_mode %d is not supported!", u32NearestMode);
			break;
		}

		if (sx < 0)
		{
			sx = 0;
		}
		if (sx > u32InW - 1)
		{
			sx = u32InW - 1;
		}

		ps32Ofs[dx] = sx;
	}
}

void LinearCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, double* alpha, AR_U32 u32CoordTransMode)
{
	double scale = (u32CoordTransMode == E_DCNN_UPSAMPLE_ALIGN_CORNERS) ? (double)(u32InW - 1) / (u32OutW - 1) : (double)u32InW / u32OutW; // for deeplab/deeplabv3+ resizebilinear align_corner=True

	for (int dx = 0; dx < u32OutW; dx++)
	{
		double fx = 0.;
		switch (u32CoordTransMode)
		{
		case E_DCNN_UPSAMPLE_HALF_PIXEL:
			fx = (double)((dx + 0.5) * scale - 0.5);
			break;
		case E_DCNN_UPSAMPLE_PYTORCH_HALF_PIXEL:
			fx = u32InW > 1 ? (double)((dx + 0.5) * scale - 0.5) : 0;
			break;
		case E_DCNN_UPSAMPLE_ALIGN_CORNERS: // for deeplab/deeplabv3+ resizebilinear align_corner=True
			//fx = (float)dx * scale;
			//break;
		case E_DCNN_UPSAMPLE_ASYMMETRIC:
			fx = (double)dx * scale;
			break;
		case E_DCNN_UPSAMPLE_TF_HALF_PIEXL_FOR_NN:
			fx = (double)(dx + 0.5) * scale;
			break;
		case E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE:
			printf(" E_DCNN_UPSAMPLE_TF_CROP_AND_RESIZE is not supported!\n");
			break;
		default:
			printf("coordinate_transformation_mode %d is not supported!", u32CoordTransMode);
			break;
		}

		int sx = (int)(floor(fx));
		fx -= sx;

		if (sx < 0)
		{
			sx = 0;
			fx = 0.f;
		}
		if (sx >= u32InW - 1)
		{
			sx = u32InW - 2;
			fx = 1.f;
		}

		ps32Ofs[dx] = sx;

		alpha[dx * 2] = 1.f - fx;
		alpha[dx * 2 + 1] = fx;
	}
}

void interpolate_cubic(double fx, double* coeffs)
{
	const double A = -0.75f;

	double fx0 = fx + 1;
	double fx1 = fx;
	double fx2 = 1 - fx;
	// float fx3 = 2 - fx;

	coeffs[0] = A * fx0*fx0*fx0 - 5 * A * fx0*fx0 + 8 * A * fx0 - 4 * A;
	coeffs[1] = (A + 2) * fx1*fx1*fx1 - (A + 3) * fx1*fx1 + 1;
	coeffs[2] = (A + 2) * fx2*fx2*fx2 - (A + 3) * fx2*fx2 + 1;
	coeffs[3] = 1.f - coeffs[0] - coeffs[1] - coeffs[2];
}

void CubicCoeffs(AR_U32 u32InW, AR_U32 u32OutW, AR_S32 * ps32Ofs, double* alpha)
{
	double scale = (double)u32InW / u32OutW;

	for (int dx = 0; dx < u32OutW; dx++)
	{
		double fx = (double)((dx + 0.5) * scale - 0.5);
		int sx = (int)(floor(fx));
		fx -= sx;

		interpolate_cubic(fx, alpha + dx * 4);

		if (sx <= -1)
		{
			sx = 1;
			alpha[dx * 4 + 0] = 1.f - alpha[dx * 4 + 3];
			alpha[dx * 4 + 1] = alpha[dx * 4 + 3];
			alpha[dx * 4 + 2] = 0.f;
			alpha[dx * 4 + 3] = 0.f;
		}
		if (sx == 0)
		{
			sx = 1;
			alpha[dx * 4 + 0] = alpha[dx * 4 + 0] + alpha[dx * 4 + 1];
			alpha[dx * 4 + 1] = alpha[dx * 4 + 2];
			alpha[dx * 4 + 2] = alpha[dx * 4 + 3];
			alpha[dx * 4 + 3] = 0.f;
		}
		if (sx == u32InW - 2)
		{
			sx = u32InW - 3;
			alpha[dx * 4 + 3] = alpha[dx * 4 + 2] + alpha[dx * 4 + 3];
			alpha[dx * 4 + 2] = alpha[dx * 4 + 1];
			alpha[dx * 4 + 1] = alpha[dx * 4 + 0];
			alpha[dx * 4 + 0] = 0.f;
		}
		if (sx >= u32InW - 1)
		{
			sx = u32InW - 3;
			alpha[dx * 4 + 3] = 1.f - alpha[dx * 4 + 0];
			alpha[dx * 4 + 2] = alpha[dx * 4 + 0];
			alpha[dx * 4 + 1] = 0.f;
			alpha[dx * 4 + 0] = 0.f;
		}

		ps32Ofs[dx] = sx;
	}
}

int execNearestUpsample(const double * input_float_data, double * output_float_data, int* xofs, int* yofs,
	const int batch_size, const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width)
{
	const int input_map_size = input_height * input_width;
	const int output_map_size = output_height * output_width;
	const int input_single_size = input_height * input_width * batch_size;
	const int output_single_size = output_height * output_width * batch_size;

	//const float hs = output_height ? input_height / (float)output_height : 1.f / scale_h;
	//const float ws = output_width ? input_width / (float)output_width : 1.f / scale_w;
	for (int q = 0; q < input_channel; q++)
	{
		for (int n = 0; n < batch_size; n++)
		{
			const double * input_data_ptr = input_float_data + n * input_map_size + q * input_single_size;
			double * output_data_ptr = output_float_data + n * output_map_size + q * output_single_size;

			for (int y = 0; y < output_height; y++)
			{
				//int in_y = min((int)(y * hs), (input_height - 1));
				int in_y = yofs[y];
				for (int x = 0; x < output_width; x++)
				{
					//int in_x = min((int)(x * ws), (input_width - 1));
					int in_x = xofs[x];
					*output_data_ptr++ = input_data_ptr[in_y * input_width + in_x];
				}
			}
		}
	}
	return 0;
}

void resize_bilinear_image(const double * src, double * dst, double * alpha, int* xofs, double * beta, int* yofs,
	const int ih, const int iw, const int oh, const int ow)
{
	double * rows0 = (double *)malloc(ow * sizeof(double));
	double * rows1 = (double *)malloc(ow * sizeof(double));

	int prev_sy1 = -2;

	for (int dy = 0; dy < oh; dy++)
	{
		int sy = yofs[dy];

		if (sy == prev_sy1)
		{
			// reuse all rows
		}
		else if (sy == prev_sy1 + 1)
		{
			// hresize one row
			double * rows0_old = rows0;
			rows0 = rows1;
			rows1 = rows0_old;
			const double * S1 = src + (sy + 1) * iw;

			const double * alphap = alpha;
			double * rows1p = rows1;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double * S1p = S1 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				rows1p[dx] = S1p[0] * a0 + S1p[1] * a1;

				alphap += 2;
			}
		}
		else
		{
			// hresize two rows
			const double* S0 = src + sy * iw;
			const double* S1 = src + (sy + 1) * iw;

			const double* alphap = alpha;
			double* rows0p = rows0;
			double* rows1p = rows1;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double* S0p = S0 + sx;
				const double* S1p = S1 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				rows0p[dx] = S0p[0] * a0 + S0p[1] * a1;
				rows1p[dx] = S1p[0] * a0 + S1p[1] * a1;

				alphap += 2;
			}
		}

		prev_sy1 = sy;

		// vresize
		double b0 = beta[0];
		double b1 = beta[1];

		double* rows0p = rows0;
		double* rows1p = rows1;
		double* Dp = dst + dy * ow;
		for (int dx = 0; dx < ow; dx++)
		{
			//             D[x] = rows0[x]*b0 + rows1[x]*b1;
			*Dp++ = *rows0p++ * b0 + *rows1p++ * b1;
		}

		beta += 2;
	}

	free(rows0);
	free(rows1);
}

void resize_bicubic_image(const double* src, double* dst, double* alpha, int* xofs, double* beta, int* yofs,
	const int ih, const int iw, const int oh, const int ow)
{
	double* rows0 = (double *)malloc(ow * sizeof(double));
	double* rows1 = (double *)malloc(ow * sizeof(double));
	double* rows2 = (double *)malloc(ow * sizeof(double));
	double* rows3 = (double *)malloc(ow * sizeof(double));

	int prev_sy1 = -3;

	for (int dy = 0; dy < oh; dy++)
	{
		int sy = yofs[dy];

		if (sy == prev_sy1)
		{
			// reuse all rows
		}
		else if (sy == prev_sy1 + 1)
		{
			// hresize one row
			double* rows0_old = rows0;
			rows0 = rows1;
			rows1 = rows2;
			rows2 = rows3;
			rows3 = rows0_old;
			const double* S3 = src + (sy + 2)*iw;

			const double* alphap = alpha;
			double* rows3p = rows3;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double* S3p = S3 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				double a2 = alphap[2];
				double a3 = alphap[3];
				rows3p[dx] = S3p[-1] * a0 + S3p[0] * a1 + S3p[1] * a2 + S3p[2] * a3;

				alphap += 4;
			}
		}
		else if (sy == prev_sy1 + 2)
		{
			// hresize two rows
			double* rows0_old = rows0;
			double* rows1_old = rows1;
			rows0 = rows2;
			rows1 = rows3;
			rows2 = rows0_old;
			rows3 = rows1_old;
			const double* S2 = src + (sy + 1) * iw;
			const double* S3 = src + (sy + 2) * iw;

			const double* alphap = alpha;
			double* rows2p = rows2;
			double* rows3p = rows3;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double* S2p = S2 + sx;
				const double* S3p = S3 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				double a2 = alphap[2];
				double a3 = alphap[3];
				rows2p[dx] = S2p[-1] * a0 + S2p[0] * a1 + S2p[1] * a2 + S2p[2] * a3;
				rows3p[dx] = S3p[-1] * a0 + S3p[0] * a1 + S3p[1] * a2 + S3p[2] * a3;

				alphap += 4;
			}
		}
		else if (sy == prev_sy1 + 3)
		{
			// hresize three rows
			double* rows0_old = rows0;
			double* rows1_old = rows1;
			double* rows2_old = rows2;
			rows0 = rows3;
			rows1 = rows0_old;
			rows2 = rows1_old;
			rows3 = rows2_old;
			const double* S1 = src + (sy)* iw;
			const double* S2 = src + (sy + 1) * iw;
			const double* S3 = src + (sy + 2) * iw;

			const double* alphap = alpha;
			double* rows1p = rows1;
			double* rows2p = rows2;
			double* rows3p = rows3;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double* S1p = S1 + sx;
				const double* S2p = S2 + sx;
				const double* S3p = S3 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				double a2 = alphap[2];
				double a3 = alphap[3];
				rows1p[dx] = S1p[-1] * a0 + S1p[0] * a1 + S1p[1] * a2 + S1p[2] * a3;
				rows2p[dx] = S2p[-1] * a0 + S2p[0] * a1 + S2p[1] * a2 + S2p[2] * a3;
				rows3p[dx] = S3p[-1] * a0 + S3p[0] * a1 + S3p[1] * a2 + S3p[2] * a3;

				alphap += 4;
			}
		}
		else
		{
			// hresize four rows
			const double* S0 = src + (sy - 1) * iw;
			const double* S1 = src + (sy)* iw;
			const double* S2 = src + (sy + 1) * iw;
			const double* S3 = src + (sy + 2) * iw;

			const double* alphap = alpha;
			double* rows0p = rows0;
			double* rows1p = rows1;
			double* rows2p = rows2;
			double* rows3p = rows3;
			for (int dx = 0; dx < ow; dx++)
			{
				int sx = xofs[dx];
				const double* S0p = S0 + sx;
				const double* S1p = S1 + sx;
				const double* S2p = S2 + sx;
				const double* S3p = S3 + sx;

				double a0 = alphap[0];
				double a1 = alphap[1];
				double a2 = alphap[2];
				double a3 = alphap[3];
				rows0p[dx] = S0p[-1] * a0 + S0p[0] * a1 + S0p[1] * a2 + S0p[2] * a3;
				rows1p[dx] = S1p[-1] * a0 + S1p[0] * a1 + S1p[1] * a2 + S1p[2] * a3;
				rows2p[dx] = S2p[-1] * a0 + S2p[0] * a1 + S2p[1] * a2 + S2p[2] * a3;
				rows3p[dx] = S3p[-1] * a0 + S3p[0] * a1 + S3p[1] * a2 + S3p[2] * a3;

				alphap += 4;
			}
		}

		prev_sy1 = sy;

		// vresize
		double b0 = beta[0];
		double b1 = beta[1];
		double b2 = beta[2];
		double b3 = beta[3];

		double* rows0p = rows0;
		double* rows1p = rows1;
		double* rows2p = rows2;
		double* rows3p = rows3;
		double* Dp = dst + dy * ow;
		for (int dx = 0; dx < ow; dx++)
		{
			//             D[x] = rows0[x]*b0 + rows1[x]*b1 + rows2[x]*b2 + rows3[x]*b3;
			*Dp++ = *rows0p++ * b0 + *rows1p++ * b1 + *rows2p++ * b2 + *rows3p++ * b3;
		}

		beta += 4;
	}

	free(rows0);
	free(rows1);
	free(rows2);
	free(rows3);
}

int execBilinearUpsample(const double* input_float_data, double* output_float_data, double* alpha, int* xofs, double* beta, int* yofs, const int batch_size,
	const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width)
{
	const int input_map_size = input_height * input_width;
	const int output_map_size = output_height * output_width;
	const int input_single_size = input_height * input_width * batch_size;
	const int output_single_size = output_height * output_width * batch_size;

#if 0
	int* xofs = new int[output_width];
	int* yofs = new int[output_height];

	float* alpha = new float[output_width * 2];
	float* beta = new float[output_height * 2];

	linear_coeffs(input_width, output_width, xofs, alpha, align_corners);
	linear_coeffs(input_height, output_height, yofs, beta, align_corners);
#endif

	for (int q = 0; q < input_channel; q++)
	{
		for (int n = 0; n < batch_size; n++)
		{
			const double* input_data_ptr = input_float_data + n * input_map_size + q * input_single_size;
			double* output_data_ptr = output_float_data + n * output_map_size + q * output_single_size;
			resize_bilinear_image(input_data_ptr, output_data_ptr, alpha, xofs, beta, yofs, input_height, input_width, output_height, output_width);
		}
	}

	return 0;
}

int execBicubicUpsample(const double* input_float_data, double* output_float_data, double* alpha, int* xofs, double* beta, int* yofs, const int batch_size,
	const int input_channel, const int input_height, const int input_width, const int output_height, const int output_width)
{
	const int input_map_size = input_height * input_width;
	const int output_map_size = output_height * output_width;
	const int input_single_size = batch_size * input_height * input_width;
	const int output_single_size = batch_size * output_height * output_width;

#if 0
	int* xofs = new int[output_width];
	int* yofs = new int[output_height];

	float* alpha = new float[output_width * 4];
	float* beta = new float[output_height * 4];

	cubic_coeffs(input_width, output_width, xofs, alpha);
	cubic_coeffs(input_height, output_height, yofs, beta);
#endif

	for (int n = 0; n < batch_size; n++)
	{
		for (int q = 0; q < input_channel; q++)
		{
			const double* input_data_ptr = input_float_data + n * input_map_size + q * input_single_size;
			double* output_data_ptr = output_float_data + n * output_map_size + q * output_single_size;
			resize_bicubic_image(input_data_ptr, output_data_ptr, alpha, xofs, beta, yofs, input_height, input_width, output_height, output_width);
		}
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
