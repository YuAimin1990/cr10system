
#include "ArCnnOperatorDeclare.h"
#include "ArCnnParseCommon.h"


AR_S32 ArCnnOperatorrReciprocal(AR_U32 pIn, AR_U32 pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_U32 u32Height = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32Width = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
    AR_U32 u32TensorStep = pCBParams->astInputTensor[0].u32TensorStep;
	AR_U32 u32RowStep = pCBParams->astInputTensor[0].u32RowStep;
	AR_U32 u32KSizeNorm = pCBParams->astInputTensor[0].u32KSizeNorm;
	AR_U32 u32KSizeLast = pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32KNormNum = pCBParams->astInputTensor[0].u32KNormNum;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_S32 s32InZeroPoint = pCBParams->astInputTensor[0].s32ZeroPoint;
	AR_DOUBLE dInFactor = pCBParams->astInputTensor[0].dScaleFactor;
	AR_U32 pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

    AR_S32 s32OutZeroPoint = pCBParams->astOutputTensor[0].s32ZeroPoint;
	AR_DOUBLE dOutFactor = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_U32 pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

	AR_U32 u32Channel = u32KNormNum * u32KSizeNorm + u32KSizeLast;

	AR_S16 s16PositiveLimit = (AR_S16)pow((double)2, (double)(u32Precision - 1)) - 1;
	AR_S16 s16NegativeLimit = (AR_S16)-pow((double)2, (double)(u32Precision - 1));
    AR_S8 s8PositiveLimit = (AR_S8)pow((double)2, (double)(u32Precision - 1)) - 1;
	AR_S8 s8NegativeLimit = (AR_S8)-pow((double)2, (double)(u32Precision - 1));
	AR_U32 u32RowSize = u32Channel * u32Width;
    AR_U32 u32Loop = u32RowSize / 4; //every loop we calculate 4
    AR_U32 u32Left = u32RowSize % 4;
    AR_DOUBLE dVal[4] = {0.};
    AR_DOUBLE dRepri[4] = {0.};
    AR_S32 s32Val[4] = {0};

	if (pCBParams->astInputTensor[0].u32Precision == 16)
	{
        for (int b = 0; b < u32Batch; b++)
        {
    		for (int h = 0; h < u32Height; h++)
    		{
    		    AR_S16 * ps16In = (AR_S16 *)(pInput + b * u32TensorStep + h * u32RowStep);
		        AR_S16 * ps16Out = (AR_S16 *)(pOutput + b * u32TensorStep + h * u32RowStep);
                for(int cnt = 0; cnt < u32Loop; cnt+=4)
                {
                    dVal[0] = (*ps16In++ - s32InZeroPoint) * dInFactor;
                    dVal[1] = (*ps16In++ - s32InZeroPoint) * dInFactor;
                    dVal[2] = (*ps16In++ - s32InZeroPoint) * dInFactor;
                    dVal[3] = (*ps16In++ - s32InZeroPoint) * dInFactor;

                    dRepri[0] = 1./dVal[0];
                    dRepri[1] = 1./dVal[1];
                    dRepri[2] = 1./dVal[2];
                    dRepri[3] = 1./dVal[3];

                    s32Val[0] = (AR_S32)round(dRepri[0] / dOutFactor) + s32OutZeroPoint;
                    s32Val[1] = (AR_S32)round(dRepri[1] / dOutFactor) + s32OutZeroPoint;
                    s32Val[2] = (AR_S32)round(dRepri[2] / dOutFactor) + s32OutZeroPoint;
                    s32Val[3] = (AR_S32)round(dRepri[3] / dOutFactor) + s32OutZeroPoint;

                    s32Val[0] = MIN(s32Val[0], s16PositiveLimit);
                    s32Val[1] = MIN(s32Val[1], s16PositiveLimit);
                    s32Val[2] = MIN(s32Val[2], s16PositiveLimit);
                    s32Val[3] = MIN(s32Val[3], s16PositiveLimit);
                    s32Val[0] = MAX(s32Val[0], s16NegativeLimit);
                    s32Val[1] = MAX(s32Val[1], s16NegativeLimit);
                    s32Val[2] = MAX(s32Val[2], s16NegativeLimit);
                    s32Val[3] = MAX(s32Val[3], s16NegativeLimit);

                    *ps16Out++ = s32Val[0] & 0x0ffff;
                    *ps16Out++ = s32Val[1] & 0x0ffff;
                    *ps16Out++ = s32Val[2] & 0x0ffff;
                    *ps16Out++ = s32Val[3] & 0x0ffff;
                }

                for(int cnt = 0; cnt < u32Left; cnt++)
                {
                    dVal[0] = (*ps16In++ - s32InZeroPoint) * dInFactor;
                    dRepri[0] = 1./dVal[0];
                    s32Val[0] = (AR_S32)round(dRepri[0] / dOutFactor) + s32OutZeroPoint;
                    s32Val[0] = MIN(s32Val[0], s16PositiveLimit);
                    s32Val[0] = MAX(s32Val[0], s16NegativeLimit);
                    *ps16Out++ = s32Val[0] & 0x0ffff;
                }
    		}
        }
	}
	else//8bit
	{
		for (int b = 0; b < u32Batch; b++)
        {
    		for (int h = 0; h < u32Height; h++)
    		{
        		AR_S8 * ps8In = (AR_S8 *)(pInput + b * u32TensorStep + h * u32RowStep);
		        AR_S8 * ps8Out = (AR_S8 *)(pOutput + b * u32TensorStep + h * u32RowStep);
                for(int cnt = 0; cnt < u32Loop; cnt+=4)
                {
                    dVal[0] = (*ps8In++ - s32InZeroPoint) * dInFactor;
                    dVal[1] = (*ps8In++ - s32InZeroPoint) * dInFactor;
                    dVal[2] = (*ps8In++ - s32InZeroPoint) * dInFactor;
                    dVal[3] = (*ps8In++ - s32InZeroPoint) * dInFactor;

                    dRepri[0] = 1./dVal[0];
                    dRepri[1] = 1./dVal[1];
                    dRepri[2] = 1./dVal[2];
                    dRepri[3] = 1./dVal[3];

                    s32Val[0] = (AR_S32)round(dRepri[0] / dOutFactor) + s32OutZeroPoint;
                    s32Val[1] = (AR_S32)round(dRepri[1] / dOutFactor) + s32OutZeroPoint;
                    s32Val[2] = (AR_S32)round(dRepri[2] / dOutFactor) + s32OutZeroPoint;
                    s32Val[3] = (AR_S32)round(dRepri[3] / dOutFactor) + s32OutZeroPoint;

                    s32Val[0] = MIN(s32Val[0], s16PositiveLimit);
                    s32Val[1] = MIN(s32Val[1], s16PositiveLimit);
                    s32Val[2] = MIN(s32Val[2], s16PositiveLimit);
                    s32Val[3] = MIN(s32Val[3], s16PositiveLimit);
                    s32Val[0] = MAX(s32Val[0], s16NegativeLimit);
                    s32Val[1] = MAX(s32Val[1], s16NegativeLimit);
                    s32Val[2] = MAX(s32Val[2], s16NegativeLimit);
                    s32Val[3] = MAX(s32Val[3], s16NegativeLimit);

                    *ps8Out++ = s32Val[0] & 0x0ffff;
                    *ps8Out++ = s32Val[1] & 0x0ffff;
                    *ps8Out++ = s32Val[2] & 0x0ffff;
                    *ps8Out++ = s32Val[3] & 0x0ffff;
                }

                for(int cnt = 0; cnt < u32Left; cnt++)
                {
                    dVal[0] = (*ps8In++ - s32InZeroPoint) * dInFactor;
                    dRepri[0] = 1./dVal[0];
                    s32Val[0] = (AR_S32)round(dRepri[0] / dOutFactor) + s32OutZeroPoint;
                    s32Val[0] = MIN(s32Val[0], s8PositiveLimit);
                    s32Val[0] = MAX(s32Val[0], s8NegativeLimit);
                    *ps8Out++ = s32Val[0] & 0x0ffff;
                }
           }
       }
	}

	return 0;
}




AR_CNN_OPERATOR_DECLARE(ArCnnOperatorrReciprocal) = {
    .pName = "reciprocal",
    .pCnnCbHandler = ArCnnOperatorrReciprocal,
};
