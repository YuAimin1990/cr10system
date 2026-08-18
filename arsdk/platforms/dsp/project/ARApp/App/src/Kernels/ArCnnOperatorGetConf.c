
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"
#if 1
AR_S32 ArCnnOperatorGetConf(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{

    AR_U32 h, w, c, b,k;
	AR_U32 u32B = 0, u32H = 0, u32W = 0, u32C = 0;
    u32B = pCBParams->astInputTensor[0].u32Num;
    u32H = pCBParams->astInputTensor[0].u32Height;
    u32W = pCBParams->astInputTensor[0].u32Width;
    u32C = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;

	AR_U32 u32InKStep = pCBParams->astInputTensor[0].u32KStep;
    AR_U32 u32InRowStep = pCBParams->astInputTensor[0].u32RowStep;
    AR_U32 u32InTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
	AR_S32 s32InZero = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;
	AR_U32 u32InByte = pCBParams->astInputTensor[0].u32Precision/8;

	AR_U32 u32OutKStep = pCBParams->astOutputTensor[0].u32KStep;
	AR_U32 u32OutRowStep = pCBParams->astOutputTensor[0].u32RowStep;
    AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
    AR_S32 s32OutZero = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;
	AR_U32 u32OutByte = pCBParams->astOutputTensor[0].u32Precision/8;

    AR_U32 u32InKSizeNorm = pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32InKNormNum =pCBParams->astInputTensor[0].u32KNormNum;
	AR_U32 u32InKSizeLast =pCBParams->astInputTensor[0].u32KSizeLast;

    AR_U32 u32OutKSizeNorm = pCBParams->astOutputTensor[0].u32KSizeNorm;
	AR_U32 u32OutKNormNum =pCBParams->astOutputTensor[0].u32KNormNum;
	AR_U32 u32OutKSizeLast =pCBParams->astOutputTensor[0].u32KSizeLast;


    if((pCBParams->astInputTensor[0].u32Precision == 16) && (pCBParams->astOutputTensor[0].u32Precision == 8))
    {

		AR_S16 * pInC = (AR_S16 *)pInput;
        signed char* pOutC = (signed char *)pOutput;
		AR_S32 * ps32Val = (AR_S32 *)malloc(u32C * sizeof(AR_U32));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(w = 0; w < u32W; w++)
                {
	                AR_S32 * pVal = ps32Val;
					AR_DOUBLE dSum = 0.0;
					AR_DOUBLE dConf0 = 0.0;

	                for(k = 0; k < u32InKNormNum; k++)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + k * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (AR_S16 *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeNorm; c++)
	                    {
	                        *pVal = *pInC++- s32InZero;;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }

	                if(u32InKSizeLast)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (AR_S16 *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeLast; c++)
	                    {
	                        *pVal = *pInC++ - s32InZero;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }


	                pVal = ps32Val;
	                for(k = 0; k < u32OutKNormNum; k++)
	                {
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + k * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
						for(c = 0; c < u32OutKSizeNorm; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++ = (int)round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }

	                if(u32OutKSizeLast)
	                {
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
	                    for(c = 0; c < u32OutKSizeLast; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++= round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }
                }
            }
        }

        free(ps32Val);
    }
	else if((pCBParams->astInputTensor[0].u32Precision == 16) && (pCBParams->astOutputTensor[0].u32Precision == 16))
    {

		AR_S16 * pInC = (AR_S16 *)pInput;
        AR_S16 * pOutC = (AR_S16 *)pOutput;
		AR_S32 * ps32Val = (AR_S32 *)malloc(u32C * sizeof(AR_U32));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(w = 0; w < u32W; w++)
                {
	                AR_S32 * pVal = ps32Val;
					AR_DOUBLE dSum = 0.0;
					AR_DOUBLE dConf0 = 0.0;

	                for(k = 0; k < u32InKNormNum; k++)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + k * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (AR_S16 *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeNorm; c++)
	                    {
	                        *pVal = *pInC++- s32InZero;;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }

	                if(u32InKSizeLast)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (AR_S16 *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeLast; c++)
	                    {
	                        *pVal = *pInC++ - s32InZero;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }


	                pVal = ps32Val;
	                for(k = 0; k < u32OutKNormNum; k++)
	                {
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + k * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
						for(c = 0; c < u32OutKSizeNorm; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++ = (int)round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }

	                if(u32OutKSizeLast)
	                {
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
	                    for(c = 0; c < u32OutKSizeLast; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++= round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }
                }
            }
        }

        free(ps32Val);
    }
	else if((pCBParams->astInputTensor[0].u32Precision == 8) && (pCBParams->astOutputTensor[0].u32Precision == 16))
    {

		signed char * pInC = (signed char *)pInput;
        AR_S16 * pOutC = (AR_S16 *)pOutput;
		AR_S32 * ps32Val = (AR_S32 *)malloc(u32C * sizeof(AR_U32));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(w = 0; w < u32W; w++)
                {
	                AR_S32 * pVal = ps32Val;
					AR_DOUBLE dSum = 0.0;
					AR_DOUBLE dConf0 = 0.0;

	                for(k = 0; k < u32InKNormNum; k++)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + k * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (signed char *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeNorm; c++)
	                    {
	                        *pVal = *pInC++- s32InZero;;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }

	                if(u32InKSizeLast)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (signed char *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeLast; c++)
	                    {
	                        *pVal = *pInC++ - s32InZero;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }


	                pVal = ps32Val;
	                for(k = 0; k < u32OutKNormNum; k++)
	                {
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + k * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
						for(c = 0; c < u32OutKSizeNorm; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++ = (int)round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }

	                if(u32OutKSizeLast)
	                {
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
	                    for(c = 0; c < u32OutKSizeLast; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++= round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }
                }
            }
        }

        free(ps32Val);
    }
    else
	{
	    signed char * pInC = (signed char *)pInput;
        signed char * pOutC = (signed char *)pOutput;
		AR_S32 * ps32Val = (AR_S32 *)malloc(u32C * sizeof(AR_U32));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(w = 0; w < u32W; w++)
                {
	                AR_S32 * pVal = ps32Val;
					AR_DOUBLE dSum = 0.0;
					AR_DOUBLE dConf0 = 0.0;

	                for(k = 0; k < u32InKNormNum; k++)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + k * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (signed char *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeNorm; c++)
	                    {
	                        *pVal = *pInC++- s32InZero;;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }

	                if(u32InKSizeLast)
	                {
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeNorm * u32InByte);
	                    pInC = (signed char *)(pInput + offset);
	                    for(c = 0; c < u32InKSizeLast; c++)
	                    {
	                        *pVal = *pInC++ - s32InZero;
	                        dSum += *pVal;
	                        pVal++;
	                    }
	                }


	                pVal = ps32Val;
	                for(k = 0; k < u32OutKNormNum; k++)
	                {
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + k * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
						for(c = 0; c < u32OutKSizeNorm; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++ = (int)round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }

	                if(u32OutKSizeLast)
	                {
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeNorm * u32OutByte));
	                    for(c = 0; c < u32OutKSizeLast; c++)
	                    {
	                        dConf0 = *pVal++/dSum;
							*pOutC++= round(dConf0 / dOutScale) + s32OutZero;
	                    }
	                }
                }
            }
        }

        free(ps32Val);
    }

    return 0;
}

#else

AR_S32 AR_CNN_OperatorGetConf(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
    AR_U32 h, w, c, b;
    AR_U32 u32B = 0, u32H = 0, u32W = 0, u32C = 0;
    AR_U32 u32RowStep = pCBParams->astInputTensor[0].u32RowStep;
    AR_S32 s32InZero = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 u32InTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
    AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

    AR_S32 s32OutZero = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
    AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

    u32B = pCBParams->astInputTensor[0].u32Num;
    u32H = pCBParams->astInputTensor[0].u32Height;
    u32W = pCBParams->astInputTensor[0].u32Width;
    //u32C = pCBParams->astInputTensor[0].u32OriChannels;
    u32C = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;

    if(pCBParams->astInputTensor[0].u32Precision == 16)
    {
        AR_S32 * ps32Val = (AR_S32 *)malloc(u32W * u32C * sizeof(AR_U32));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                //we pick a row every time then save it to the according to the out.
                AR_S16 * ps16Buff = (AR_S16 *)(pInput + b * u32InTensorStep + h * u32RowStep);
                AR_S16 * ps16Out =  (AR_S16 *)(pOutput + b * u32OutTensorStep + h * u32RowStep);

                //normally they are in the same cacheline
                for(w = 0; w < u32W; w++)
                {
                    AR_DOUBLE dSum = 0.0;
                    AR_DOUBLE dConf0 = 0.0;
                    AR_S32 * pVal = ps32Val;
                    for(c = 0; c < u32C; c++)
                    {
                        *pVal = *ps16Buff++ - s32InZero;
                        dSum += *pVal;
                        pVal++;
                    }
                    pVal = ps32Val;
                    for(c = 0; c < u32C; c++)
                    {
                        dConf0 = *pVal++ / dSum;
                        *ps16Out++ = round(dConf0 / dOutScale) + s32OutZero;
                    }
                }
            }
        }

        free(ps32Val);
    }
    else //8bit
    {
        AR_S16 * ps16Val = (AR_S16 *)malloc(u32W * u32C * sizeof(AR_U16));

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(w = 0; w < u32W; w++)
                {
                    //i = b * u32H * u32W + h * u32W + w;
                    //we pick a row every time then save it to the according to the out.
                    signed char *ps8Buff = (signed char *)(pInput + b * u32InTensorStep + h * u32RowStep);
                    signed char *ps8Out =  (signed char *)(pOutput + b * u32InTensorStep + h * u32RowStep);
                    AR_DOUBLE dSum = 0.0;
                    AR_DOUBLE dConf = 0.0;
                    AR_S16 * pVal = ps16Val;
                    for(c = 0; c < u32C; c++)
                    {
						*pVal = *ps8Buff++ - s32InZero;
                        dSum += *pVal;
                        pVal++;
                    }
                    pVal = ps16Val;
                    for(c = 0; c < u32C; c++)
                    {
						dConf = *pVal++ / dSum;
                        *ps8Out++ = round(dConf / dOutScale) + s32OutZero;
                    }

                }
            }
        }

        free(ps16Val);
    }

    return 0;
}
#endif

AR_CNN_OPERATOR_DECLARE(ArCnnOperatorGetConf) = {
    .pName = "getconfidence",
    .pCnnCbHandler = ArCnnOperatorGetConf,
};
