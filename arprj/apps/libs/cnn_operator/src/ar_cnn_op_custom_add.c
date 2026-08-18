#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

AR_S32 AR_CNN_CustomAddForward(AR_DOUBLE *pInput, AR_DOUBLE *pOutput, AR_NPU_CB_PARAM_S *pCBParams, AR_U32 tensor_id)
{
	AR_NPU_CUSTOMADD_PARAM_S *pCustomAddParam =(AR_NPU_CUSTOMADD_PARAM_S*)(pCBParams->pOpParams);
	
	AR_U32 batch = pCBParams->astInputTensor[tensor_id].u32Num;
	AR_U32 channel = pCBParams->astInputTensor[tensor_id].u32OriChannels;
	AR_U32 height = pCBParams->astInputTensor[tensor_id].u32Height;
	AR_U32 width = pCBParams->astInputTensor[tensor_id].u32Width;
	AR_U32 u32DataSize = batch * channel * height * width;

	if(tensor_id == 0){
		memcpy(pOutput, pInput, u32DataSize * sizeof(AR_DOUBLE));
	}
	else{
		for(AR_S32 s = 0; s < u32DataSize; s++){
			pOutput[s] = pOutput[s] + pInput[s] + pCustomAddParam->bias; 
		}
	}

	return 0;
}

AR_S32 AR_CNN_OperatorCustomAdd(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pCBParamsExt)
{
	AR_NPU_CB_PARAM_EXT_S *pstCBParaExt = (AR_NPU_CB_PARAM_EXT_S *)pCBParamsExt;
	AR_U32 input_tensor_num = pCBParams->u32InputTensorNum;
	AR_UINTPTR pInput;
	AR_UINTPTR pOutput;

	AR_S32 output_batch_size = pCBParams->astOutputTensor[0].u32Num;
	AR_S32 output_channel = pCBParams->astOutputTensor[0].u32OriChannels;
	if(!output_channel)
		output_channel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_S32 output_height = pCBParams->astOutputTensor[0].u32Height;
	AR_S32 output_width = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
	if(0 == strcmp(pstCBParaExt->astOutputTensorExt[0].achMemoryType, "output")) 
		pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;
	else
		pOutput = pIn + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;
	AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_U32 u32OutPixelByte = u32OutPrecision / 8;
	AR_U32 u32OutChSize = output_width * output_height;
	AR_U32 u32OutDataSize = output_channel * u32OutChSize * output_batch_size;

	AR_CHAR *pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32OutPixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		return -1;
	}
	AR_DOUBLE *pdOutput = (AR_DOUBLE *)malloc(u32OutDataSize * sizeof(AR_DOUBLE));
	if (!pdOutput)
	{
		printf("Malloc output buffer failed.\n");
		free(pOutNchw);
		return -1;
	}
	
	// static int idx=0;
	for(AR_U32 i = 0; i < input_tensor_num; i++){
		AR_S32 input_batch_size = pCBParams->astInputTensor[i].u32Num;
		AR_S32 input_channel = pCBParams->astInputTensor[i].u32OriChannels;
		if(!input_channel)
			input_channel = pCBParams->astInputTensor[i].u32KNormNum * pCBParams->astInputTensor[i].u32KSizeNorm + pCBParams->astInputTensor[i].u32KSizeLast;
		AR_S32 input_height = pCBParams->astInputTensor[i].u32Height;
		AR_S32 input_width = pCBParams->astInputTensor[i].u32Width;
		AR_U32 u32InPrecision = pCBParams->astInputTensor[i].u32Precision;
		if(0 == strcmp(pstCBParaExt->astInputTensorExt[0].achMemoryType, "output")) 
			pInput = pOut + pCBParams->astInputTensor[i].u32Bank * 0x2000000 + pCBParams->astInputTensor[i].u32Offset;
		else
			pInput = pIn + pCBParams->astInputTensor[i].u32Bank * 0x2000000 +  pCBParams->astInputTensor[i].u32Offset;
		AR_DOUBLE dInScale = pCBParams->astInputTensor[i].dScaleFactor;
		AR_S32 s32InZeroPoint = pCBParams->astInputTensor[i].s32ZeroPoint;
		
		AR_U32 u32InPixelByte = u32InPrecision / 8;
		AR_U32 u32InChSize =  input_width * input_height;		
		AR_U32 u32InDataSize = input_channel * u32InChSize * input_batch_size;

		//malloc input fix NCHW buf
		AR_CHAR *pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32InPixelByte);
		if (!pInNchw)
		{
			printf("Malloc input nchw failed.\n");
			free(pOutNchw);
			free(pdOutput);
			return -1;
		}
		//malloc input double NCHW data buf
		AR_DOUBLE * pdInput = (AR_DOUBLE *)malloc(u32InDataSize * sizeof(AR_DOUBLE));
		if (!pdInput)
		{
			printf("Malloc input buffer failed.\n");
			free(pOutNchw);
			free(pdOutput);
			free(pInNchw);
			return -1;
		}
		memset(pdInput, 0, u32InDataSize * sizeof(AR_DOUBLE));
		//input fix nhwc -> nchw
		dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[i]);
		//input fix to double
		dcnn_exec_scale(pInNchw, pdInput, u32InDataSize, dInScale, s32InZeroPoint, u32InPrecision);

#if 0

		char save_pathin[256];
		sprintf(save_pathin, "/mnt/add/add_input_%d_%d.txt", idx, i);
		FILE *fpin;
		fpin = fopen(save_pathin, "w");
		for(AR_U32 x = 0; x < u32InDataSize; x++){
			fprintf(fpin, "%lf\n", pdInput[x]);
		}
		fclose(fpin);
#endif

		AR_CNN_CustomAddForward(pdInput, pdOutput, pCBParams, i);

		free(pInNchw);
		free(pdInput);
	}

#if 0
	char save_path[256];
	sprintf(save_path, "/mnt/add/add_output_%d.txt", idx);
	FILE *fp;
	fp = fopen(save_path, "w");
	for(AR_U32 k = 0; k < u32OutDataSize; k++){
		fprintf(fp, "%lf\n", pdOutput[k]);
	}
	fclose(fp);
	idx++;
#endif
	
	//output double to fix
	dcnn_exec_quantization(pdOutput, pOutNchw, u32OutDataSize, dOutScale, s32OutZeroPoint, u32OutPrecision);
	//output fix nchw to nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
	
	free(pOutNchw);
	free(pdOutput);
	
	return 0;
}