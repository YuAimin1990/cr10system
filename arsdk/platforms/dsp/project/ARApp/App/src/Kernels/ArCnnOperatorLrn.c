/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"

static int LrnForward(double *input_data, double *output_data, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_LRN_PARAM_S *pLrnParams)
{

	int                     lrn_local_size = pLrnParams->u32LocalSize;
	double                  lrn_k = pLrnParams->dK;
	double                  lrn_beta = pLrnParams->dBeta;
	double                  alpha_div_size = pLrnParams->dAlphaDivSize;

	int                     batch_size = pCBParams->astInputTensor[0].u32Num;
	int                     operator_input_width = pCBParams->astInputTensor[0].u32Width;
	int                     operator_input_height = pCBParams->astInputTensor[0].u32Height;
	int                     operator_input_channel = pCBParams->astInputTensor[0].u32OriChannels;
	int                     operator_output_width = pCBParams->astOutputTensor[0].u32Width;
	int                     operator_output_height = pCBParams->astOutputTensor[0].u32Height;
	int                     operator_output_channel = pCBParams->astOutputTensor[0].u32OriChannels;

	int map_size = operator_input_height * operator_input_width;
	int single_size = operator_input_channel * map_size;
	size_t data_size = single_size * batch_size;

	double *input_data_tmp = input_data; //hbbai no need to memcpy

	double* square_data = (double*)malloc(sizeof(double)* batch_size*operator_input_channel* operator_input_height* operator_input_width);
	double* square_data_sum = (double*)malloc(sizeof(double)* batch_size*operator_input_channel* operator_input_height* operator_input_width);
	memset(square_data_sum, 0, sizeof(double)* batch_size*operator_input_channel* operator_input_height* operator_input_width);

	for (size_t i = 0; i < data_size; ++i)
		square_data[i] = input_data_tmp[i] * input_data_tmp[i];

	if (strcmp(pLrnParams->achNormRegion, "across_channels") == 0)
	{
		for (int q = 0; q < operator_input_channel; q++)
		{
			//for (int m = 0; m < batch_size; m++)
			{
				// square sum
				double* ssptr = square_data_sum + q * map_size * batch_size;
				for (int p = q - lrn_local_size / 2; p <= q + lrn_local_size / 2; p++)
				{
					if (p < 0 || p >= operator_input_channel)
						continue;
					const double* sptr = square_data + p * map_size * batch_size;
					for (int i = 0; i < map_size * batch_size; i++)
					{
						ssptr[i] += sptr[i];
						//printf("%lf\n", ssptr[i]);
					}

				}
			}
		}
	}
	else // if (lrn_norm_region == E_DCNN_LRN_WITHIN_CHANNELS)
	{
		int pre_padding = (lrn_local_size - 1) / 2;
        double* input_data_tmp = square_data;// + n * operator_output_channel * operator_input_height * operator_input_width;
        double* sum_data_tmp = square_data_sum; // + n * operator_output_channel * operator_output_height * operator_output_width;

		for (int c = 0; c < operator_output_channel; c++)
		{
			for (int h = 0; h < operator_output_height * batch_size; h++)
			{
				for (int w = 0; w < operator_output_width; w++)
				{
					int hstart = h - pre_padding;
					int wstart = w - pre_padding;
					int hend = MIN(hstart + lrn_local_size, operator_input_height * batch_size);
					int wend = MIN(wstart + lrn_local_size, operator_input_width);
					hstart = MAX(hstart, 0);
					wstart = MAX(wstart, 0);

					int out_idx = c * operator_output_height * operator_output_width * batch_size + h * operator_output_width + w;
					for (int lh = hstart; lh < hend; lh++)
					{
						for (int lw = wstart; lw < wend; lw++)
						{
							int current_index = c * operator_input_height * operator_input_width * batch_size + lh * operator_input_width + lw;
							sum_data_tmp[out_idx] += input_data_tmp[current_index];
						}
					}
				}
			}
		}
	}

	for (size_t i = 0; i < data_size; i++)
	{
		if (square_data_sum[i] == 0 && lrn_k == 0)
			output_data[i] = 0;
		else
			output_data[i] = (double)(input_data_tmp[i] * pow(lrn_k + alpha_div_size * square_data_sum[i], -lrn_beta));
	}

	free(square_data);
	free(square_data_sum);
	//free(input_data_tmp);
	return 0;
}

int LrnQInference(char* input_data, char* output_data, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_LRN_PARAM_S *pLrnParams)
{
    AR_U32 u32InDataSize = pCBParams->astInputTensor[0].u32Size;
    AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_DOUBLE dInFactor = pCBParams->astInputTensor[0].dScaleFactor;
    AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;

    AR_U32 u32OutDataSize = pCBParams->astOutputTensor[0].u32Size;
    AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutFactor = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;

    double *input_data_temp = (double *)malloc(u32InDataSize * sizeof(double));
    if (!input_data_temp)
    {
        printf("Malloc input buffer failed.\n");
        return -1;
    }

    double *output_data_temp = (double *)malloc(u32OutDataSize * sizeof(double));
    if (!output_data_temp)
    {
        printf("Malloc output buffer failed.\n");
        free(input_data_temp);
        return -1;
    }

    memset(input_data_temp, 0, u32InDataSize * sizeof(double));
    memset(output_data_temp, 0, u32OutDataSize * sizeof(double));

    dcnn_exec_scale(input_data, input_data_temp, u32InDataSize, dInFactor, s32InZeroPoint, u32InPrecision);

	LrnForward(input_data_temp, output_data_temp, pCBParams, pLrnParams);

    dcnn_exec_quantization(output_data_temp, output_data, u32OutDataSize, dOutFactor, s32OutZeroPoint, u32OutPrecision);

	//dcnn_exec_limit(output_data, data_size, (int)pow((float)2, (float)(u32Precision - 1)) - 1, (int)-pow((float)2, (float)(u32Precision - 1)), u32Precision);



	free(input_data_temp);
	free(output_data_temp);
	return 0;
}


AR_S32 ArCnnOperatorLrn(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void*  pOpParams)
{
    AR_NPU_LRN_PARAM_S *pLrnParams = (AR_NPU_LRN_PARAM_S*) (pOpParams);

    AR_CHAR * pLrnInput = NULL;
    AR_CHAR * pLrnOutput = NULL;
    AR_U32 u32InputMemSize = 0, u32OutMemSize = 0;

    u32InputMemSize = pCBParams->astInputTensor[0].u32Size * pCBParams->astInputTensor[0].u32Precision/8;
    AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;


    u32OutMemSize = pCBParams->astOutputTensor[0].u32Size * pCBParams->astOutputTensor[0].u32Precision/8;
    AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

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

    //first reorg the data
    //printf("HWC input is %x %x %x %x\n", *(int *)pIn, *(int *)(pIn + 4), *(int *)(pIn + 8), *(int *)(pIn + 0xc));
    dcnn_hwc_to_chw((char *)pInput, pLrnInput, &pCBParams->astInputTensor[0]);
    //printf("CHW input is %x %x %x %x\n", *(int *)pLrnInput, *(int *)(pLrnInput + 4), *(int *)(pLrnInput + 8), *(int *)(pLrnInput + 0xc));

    LrnQInference(pLrnInput, pLrnOutput, pCBParams,pLrnParams);
    //printf("AfterLRN: %x %x %x %x\n", *(int *)pLrnOutput, *(int *)(pLrnOutput + 4), *(int *)(pLrnOutput + 8), *(int *)(pLrnOutput + 0xc));
    dcnn_chw_to_hwc(pLrnOutput, (char *)pOutput, &pCBParams->astOutputTensor[0]);

    free(pLrnInput);
    free(pLrnOutput);

    return 0;
}



AR_CNN_OPERATOR_DECLARE(ArCnnOperatorLrn) = {
    .pName = "lrn",
    .pCnnCbHandler = ArCnnOperatorLrn,
};

