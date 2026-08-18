#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

void dcnn_exec_scale_interal(char* input_data, float* output_data, int data_size, double scale_factor, int zero_point, int precision)
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
void dcnn_exec_quantization_interal(float* float_data, char* quant_data, int data_size, double scale_factor, int zero_point, int precision)
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



static AR_S32 CustomLrnForward(AR_UINTPTR input_data, AR_UINTPTR output_data,	AR_NPU_CB_PARAM_S *pCBParams, void *pOpParams)
{
	AR_NPU_CUSTOM_LRN_PARAM_S *pLrnParams = (AR_NPU_CUSTOM_LRN_PARAM_S*) (pOpParams);
    printf("pLrnParams %f %f %f %d \r\n",pLrnParams->dAlpha,pLrnParams->dBeta,pLrnParams->dBias, pLrnParams->u32Size);

	float lrn_bias = pLrnParams->dBias;
	float lrn_beta = pLrnParams->dBeta;
	AR_DOUBLE alpha_div_size = pLrnParams->dAlpha/ pLrnParams->u32Size;
	int lrn_local_size = pLrnParams->u32Size;

	AR_S32 batch_size = pCBParams->astInputTensor[0].u32Num;
	AR_S32 operator_input_width = pCBParams->astInputTensor[0].u32Width;
	AR_S32 operator_input_height = pCBParams->astInputTensor[0].u32Height;
	AR_S32 operator_input_channel = pCBParams->astInputTensor[0].u32OriChannels;

	AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
	AR_DOUBLE dInFactor = pCBParams->astInputTensor[0].dScaleFactor;
    AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;

	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_DOUBLE dOutFactor = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;

	AR_S32 map_size = operator_input_height * operator_input_width;
	AR_U32 single_size = operator_input_channel * map_size;
	size_t data_size = single_size * batch_size;

	AR_U32 u32InputMemSize = pCBParams->astInputTensor[0].u32Size * pCBParams->astInputTensor[0].u32Precision/8;
    AR_U32 u32OutMemSize = pCBParams->astOutputTensor[0].u32Size * pCBParams->astOutputTensor[0].u32Precision/8;

	char * pLrnInput = NULL;
	char * pLrnOutput = NULL;
	pLrnInput = (char *)malloc(u32InputMemSize);
	if (!pLrnInput)
	{
		printf("Malloc lrn input error.\n");
		return -1;
	}

	pLrnOutput = (char *)malloc(u32OutMemSize);
	if (!pLrnOutput)
	{
		free(pLrnInput);
		printf("Malloc lrn input error.\n");
		return -1;
	}

	memset(pLrnInput, 0, u32InputMemSize);
	memset(pLrnOutput, 0, u32OutMemSize);
	dcnn_hwc_to_chw((char *)input_data, pLrnInput, &pCBParams->astInputTensor[0]);


	float *input_data_temp = (float *)malloc(data_size * sizeof(double));
	if (!input_data_temp)
	{
		printf("Malloc input buffer failed.\n");
		return -1;
	}
	float *output_data_temp = (float *)malloc(data_size * sizeof(double));
	if (!output_data_temp)
	{
		printf("Malloc output buffer failed.\n");
		free(input_data_temp);
		return -1;
	}

	memset(input_data_temp, 0, data_size * sizeof(double));
	memset(output_data_temp, 0, data_size * sizeof(double));

	dcnn_exec_scale_interal(pLrnInput, input_data_temp, data_size, dInFactor, s32InZeroPoint, u32InPrecision);


	AR_DOUBLE *square_data = (AR_DOUBLE*) malloc(sizeof(AR_DOUBLE) * batch_size * operator_input_channel
					* operator_input_height * operator_input_width);
	AR_DOUBLE *square_data_sum = (AR_DOUBLE*) malloc(sizeof(AR_DOUBLE) * batch_size * operator_input_channel
					* operator_input_height * operator_input_width);
	memset(square_data_sum, 0,sizeof(AR_DOUBLE) * batch_size * operator_input_channel
					* operator_input_height * operator_input_width);
	memset(square_data, 0,sizeof(AR_DOUBLE) * batch_size * operator_input_channel
					* operator_input_height * operator_input_width);

	for (AR_U32 i = 0; i < data_size; ++i) {
		square_data[i] = input_data_temp[i];
		square_data[i] = square_data[i] * square_data[i];
	}

	for (int m = 0; m < batch_size; m++)
	{
		for (int q = 0; q < operator_input_channel; q++)
		{
			// square sum
			double* ssptr = square_data_sum + m * single_size + q * map_size;
			for (int p = q - lrn_local_size / 2; p <= q + lrn_local_size / 2; p++)
			{
				if (p < 0 || p >= operator_input_channel)
					continue;
				const double* sptr = square_data + m * single_size + p * map_size;
				for (int i = 0; i < map_size; i++)
					ssptr[i] += sptr[i];
			}
		}
	}

	for (AR_U32 i = 0; i < data_size; i++) {
		if (square_data_sum[i] == 0 && lrn_bias == 0)
			output_data_temp[i] = 0;
		else
			output_data_temp[i] = (double)(input_data_temp[i]* pow(lrn_bias + alpha_div_size * square_data_sum[i], -lrn_beta));
	}


	free(square_data);
	free(square_data_sum);

	dcnn_exec_quantization_interal(output_data_temp, pLrnOutput, data_size, dOutFactor, s32OutZeroPoint, u32OutPrecision);

	free(input_data_temp);
	free(output_data_temp);

	dcnn_chw_to_hwc(pLrnOutput, (char *)output_data, &pCBParams->astOutputTensor[0]);

    free(pLrnInput);
    free(pLrnOutput);

	return 0;

}

AR_S32 AR_CNN_OperatorCustomLrn(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;
	AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;

	return CustomLrnForward( pInput, pOutput, pCBParams, pOpParams);
}


