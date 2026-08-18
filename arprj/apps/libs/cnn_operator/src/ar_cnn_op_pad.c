#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

AR_VOID copy_make_border_constant_image16to16(AR_S16 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left, AR_S16 v)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = v;
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = v;
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	for (; y < oh; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
}
AR_VOID copy_make_border_reflect_image16to16(AR_S16 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	ptr += top * iw;
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= 2 * iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12)
		{
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else
		{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
}
AR_VOID copy_make_border_edge_image16to16(AR_S16 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S16));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		outptr += ow;
	}
}
AR_VOID copy_make_border_constant_image16to8(AR_S16 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left, AR_S8 v)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = v;
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = v;
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	for (; y < oh; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
}
AR_VOID copy_make_border_reflect_image16to8(AR_S16 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	ptr += top * iw;
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - (x - left - iw) - 2];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= 2 * iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
}
AR_VOID copy_make_border_edge_image16to8(AR_S16 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - 1];
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - 1];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S8)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S8)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S8)ptr[iw - 1];
		outptr += ow;
	}
}
AR_VOID copy_make_border_constant_image8to8(AR_S8 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left, AR_S8 v)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = v;
		if (iw < 12){
			for(; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = v;
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	for (; y < oh; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
}
AR_VOID copy_make_border_reflect_image8to8(AR_S8 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	ptr += top * iw;
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12)
		{
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else
		{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= 2 * iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[left - x];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
}
AR_VOID copy_make_border_edge_image8to8(AR_S8 *ptr, AR_S8 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = ptr[0];
		if (iw < 12){
			for (; x < (left + iw); x++)
				outptr[x] = ptr[x - left];
		}
		else{
			memcpy(outptr + left, ptr, iw * sizeof(AR_S8));
			x += iw;
		}
		for (; x < ow; x++)
			outptr[x] = ptr[iw - 1];
		outptr += ow;
	}
}
AR_VOID copy_make_border_constant_image8to16(AR_S8 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left, AR_S16 v)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++)
	{
		x = 0;
		for (; x < left; x++)
			outptr[x] = v;
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = v;
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	for (; y < oh; y++)
	{
		x = 0;
		for (; x < ow; x++)
			outptr[x] = v;
		outptr += ow;
	}
}

AR_VOID copy_make_border_reflect_image8to16(AR_S8 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	ptr += top * iw;
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - (x - left - iw) - 2];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= 2 * iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[left - x];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - (x - left - iw) - 2];
		outptr += ow;
		ptr -= iw;
	}
}

AR_VOID copy_make_border_edge_image8to16(AR_S8 *ptr, AR_S16 *outptr, AR_S32 ih, AR_S32 iw, AR_S32 oh, AR_S32 ow, AR_S32 top, AR_S32 left)
{
	int y = 0;
	int x = 0;
	// fill top
	for (; y < top; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - 1];
		outptr += ow;
	}
	// fill center
	for (; y < (top + ih); y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - 1];
		ptr += iw;
		outptr += ow;
	}
	// fill bottom
	ptr -= iw;
	for (; y < oh; y++){
		x = 0;
		for (; x < left; x++)
			outptr[x] = (AR_S16)ptr[0];
		for (; x < (left + iw); x++)
			outptr[x] = (AR_S16)ptr[x - left];
		for (; x < ow; x++)
			outptr[x] = (AR_S16)ptr[iw - 1];
		outptr += ow;
	}
}

AR_S32 constant_value_float2fix(AR_NPU_CB_PARAM_S *pCBParams, AR_NPU_PAD_PARAM_S *pPadParam)
{
	AR_S32 fix_val = 0;
	AR_U32 u32Precision = pCBParams->astOutputTensor[0].u32Precision;
	AR_DOUBLE scale_factor = pCBParams->astOutputTensor[0].dScaleFactor;
	AR_S32 zero_point = pCBParams->astOutputTensor[0].s32ZeroPoint;
	int positive_limit = 0;
	int negative_limit = 0;
	if(pPadParam->u32PadOpVer){
		positive_limit = (int)pow((float)2, (float)(u32Precision - 1)) - 1;
		negative_limit = (int)-pow((float)2, (float)(u32Precision - 1));
	}
	else{
		positive_limit = (int)pow((double)2, (double)(u32Precision - 1)) - 1;
		negative_limit = (int)-pow((double)2, (double)(u32Precision - 1));
	}
	if(pPadParam->u32PadOpVer)
    	fix_val = round((float)pPadParam->val / (float)scale_factor) + zero_point;
	else
		fix_val = round((double)pPadParam->val / (double)scale_factor) + zero_point;
    fix_val = MIN(fix_val, positive_limit);
    fix_val = MAX(fix_val, negative_limit);

	return fix_val;
}

void _AR_CNN_PadForward(AR_CHAR *pInput, AR_CHAR *pOutput, AR_NPU_CB_PARAM_S *pCBParams, AR_NPU_PAD_PARAM_S *pPadParam)
{
	AR_S32 input_batch_size = pCBParams->astInputTensor[0].u32Num;
	AR_S32 input_channel = pCBParams->astInputTensor[0].u32OriChannels;
	AR_S32 input_height = pCBParams->astInputTensor[0].u32Height;
	AR_S32 input_width = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;

	AR_S32 output_batch_size = input_batch_size;
	AR_S32 output_channel = input_channel;
	AR_S32 output_height = pCBParams->astOutputTensor[0].u32Height;
	AR_S32 output_width = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;

	AR_S32 input_map_size = input_height * input_width;
	AR_S32 input_single_size = input_channel * input_height * input_width;

	AR_S32 output_map_size = output_height * output_width;
	AR_S32 output_single_size = output_channel * output_height * output_width;

	AR_CHAR *optype = pPadParam->achPadMode;

	AR_S32 pad_t = pPadParam->pad_t;
	AR_S32 pad_l = pPadParam->pad_l;

	AR_S32 fix_val = 0;

	/************constant double value convert to fix value************/
	if(strcmp(optype, "constant") == 0)
		fix_val = constant_value_float2fix(pCBParams, pPadParam);
	/********************************************************************/
	AR_S32 n = 0;
	if(u32InPrecision == 16){
		AR_S16 *pIn = (AR_S16 *)pInput;
		if(u32OutPrecision == 16){
			AR_S16 *pOut = (AR_S16 *)pOutput;
			for (n = 0; n < output_batch_size; n++)
			{
				for (int q = 0; q < output_channel; q++)
				{
					AR_S16 *inptr = pIn + n * input_single_size + q * input_map_size;
					AR_S16 *outptr = pOut + n * output_single_size + q * output_map_size;
					if(strcmp(optype, "constant") == 0)
						copy_make_border_constant_image16to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l, (AR_S16)fix_val);
					else if(strcmp(optype, "reflect") == 0)
						copy_make_border_reflect_image16to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
					else
						copy_make_border_edge_image16to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
				}
			}
		}
		else{
			AR_S8 *pOut = (AR_S8 *)pOutput;
			for (n = 0; n < output_batch_size; n++)
			{
				for (int q = 0; q < output_channel; q++)
				{
					AR_S16 *inptr = pIn + n * input_single_size + q * input_map_size;
					AR_S8 *outptr = pOut + n * output_single_size + q * output_map_size;
					if(strcmp(optype, "constant") == 0)
						copy_make_border_constant_image16to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l, (AR_S8)fix_val);
					else if(strcmp(optype, "reflect") == 0)
						copy_make_border_reflect_image16to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
					else
						copy_make_border_edge_image16to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
				}
			}
		}
	}
	else{
		AR_S8 *pIn = (AR_S8 *)pInput;
		if(u32OutPrecision == 16){
			AR_S16 *pOut = (AR_S16 *)pOutput;
			for (n = 0; n < output_batch_size; n++)
			{
				for (int q = 0; q < output_channel; q++)
				{
					AR_S8 *inptr = pIn + n * input_single_size + q * input_map_size;
					AR_S16 *outptr = pOut + n * output_single_size + q * output_map_size;
					if(strcmp(optype, "constant") == 0)
						copy_make_border_constant_image8to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l, (AR_S16)fix_val);
					else if(strcmp(optype, "reflect") == 0)
						copy_make_border_reflect_image8to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
					else
						copy_make_border_edge_image8to16(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
				}
			}
		}
		else{
			AR_S8 *pOut = (AR_S8 *)pOutput;
			for (n = 0; n < output_batch_size; n++)
			{
				for (int q = 0; q < output_channel; q++)
				{
					AR_S8 *inptr = pIn + n * input_single_size + q * input_map_size;
					AR_S8 *outptr = pOut + n * output_single_size + q * output_map_size;
					if(strcmp(optype, "constant") == 0)
						copy_make_border_constant_image8to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l, (AR_S8)fix_val);
					else if(strcmp(optype, "reflect") == 0)
						copy_make_border_reflect_image8to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
					else
						copy_make_border_edge_image8to8(inptr, outptr, input_height, input_width, output_height, output_width, pad_t, pad_l);
				}
			}
		}
	}
}

AR_S32 AR_CNN_OperatorPad(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
	AR_NPU_PAD_PARAM_S *pPadParam =(AR_NPU_PAD_PARAM_S*)(pOpParams);

	AR_S32 input_batch_size = pCBParams->astInputTensor[0].u32Num;
	AR_S32 input_channel = pCBParams->astInputTensor[0].u32OriChannels;
	if(!input_channel)
		input_channel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
	AR_S32 input_height = pCBParams->astInputTensor[0].u32Height;
	AR_S32 input_width = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;
	AR_UINTPTR* pInput = (AR_UINTPTR *)(pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset);
	
	AR_S32 output_batch_size = pCBParams->astOutputTensor[0].u32Num;
	AR_S32 output_channel = pCBParams->astOutputTensor[0].u32OriChannels;
	if(!output_channel)
		output_channel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_S32 output_height = pCBParams->astOutputTensor[0].u32Height;
	AR_S32 output_width = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;
	AR_UINTPTR* pOutput = (AR_UINTPTR *)(pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset);
	if(input_batch_size != output_batch_size){
		printf("Parameter Error: the output batch size of pad layer must be equal to the input batch size!!\r\n");
		return -1;
	}

	AR_CHAR *optype = pPadParam->achPadMode;
	if((strcmp(optype, "constant") != 0) && (strcmp(optype, "reflect") != 0) && (strcmp(optype, "edge") != 0)){
		printf("Error: The callback pad operator only supports constant, reflect!!\r\n");
		return -1;
	}

	AR_U32 u32InPixelByte = u32InPrecision / 8;
	AR_U32 u32OutPixelByte = u32OutPrecision / 8;

	AR_U32 u32InChSize =  input_width * input_height;
	AR_U32 u32OutChSize = output_width * output_height;
	
	AR_U32 u32InDataSize = input_channel * u32InChSize * input_batch_size;
	AR_U32 u32OutDataSize = output_channel * u32OutChSize * output_batch_size;
	//malloc fix NCHW buf
	AR_CHAR *pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32InPixelByte);
	if (!pInNchw)
	{
		printf("Malloc input nchw failed.\n");
		return -1;
	}
	AR_CHAR * pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32OutPixelByte);
	if (!pOutNchw)
	{
		printf("Malloc output nchw failed.\n");
		free(pInNchw);
		return -1;
	}
	//nhwc -> nchw
	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);

	_AR_CNN_PadForward(pInNchw, pOutNchw, pCBParams, pPadParam);
	//nchw to nhwc
	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);

	free(pInNchw);
	free(pOutNchw);
	
	return 0;
}