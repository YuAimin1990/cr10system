/*
 * ar_arm_cb_1.c
 *
 *  Created on: 2021Äê1ÔÂ19ÈÕ
 *      Author: ylu
 */

#include "ArArmRmtCallDeclare.h"

#pragma pack (4)

typedef struct
{
    AR_U64 u64PhyAddr;
	AR_U64 u64VirtAddr;
	AR_U32 u32Width;
    AR_U32 u32Height;
    AR_U32 u32Stride;
} AR_DSP_IMG_CHANNEL_S;

typedef struct
{
    AR_U32 u32ChannelNum;
    AR_DSP_IMG_CHANNEL_S astSrcChannels[4];
	AR_DSP_IMG_CHANNEL_S astDstChannels[4];
} AR_DSP_IMG_S;
#pragma pack ()


//*************************************************************************************************************
// CCV_gaussian3x3_ref - C level reference function of the gaussian3x3
//
// inputs:
// p_u8Src		- pointer to the source chunk
// p_u8Dst		- pointer to the destination chunk
// s32SrcStride	- source stride represent the width + padding if applicable
// s32DstStride - destination stride represent the width + padding if applicable
// u32N			- number of rows in the tile
// u32M         - number of columns in the tile
//*************************************************************************************************************
void CCV_gaussian3x3_ref(uchar *p_u8Src, uchar *p_u8Dst, int s32SrcStride, int s32DstStride, uint u32N, uint u32M)
{
	uint i,j;
	char s8Kernel[9] = {2,4,2,4,8,4,2,4,2};
	uchar  ps = 5;
	ushort res;

	//horizontal
	for (j = 0; j < u32N; j++)
	{
		uchar* p_in_u8 = &p_u8Src[j*s32SrcStride];
		uchar* p_out_u8 = &p_u8Dst[j*s32DstStride];

		for (i = 0; i <u32M; i++)
		{
			uint acc = 0;
			acc += p_in_u8[-1 - s32SrcStride] * s8Kernel[0];
			acc += p_in_u8[0  - s32SrcStride] * s8Kernel[1];
			acc += p_in_u8[1  - s32SrcStride] * s8Kernel[2];
			acc += p_in_u8[-1               ] * s8Kernel[3];
			acc += p_in_u8[ 0               ] * s8Kernel[4];
			acc += p_in_u8[ 1               ] * s8Kernel[5];
			acc += p_in_u8[-1 + s32SrcStride] * s8Kernel[6];
			acc += p_in_u8[0  + s32SrcStride] * s8Kernel[7];
			acc += p_in_u8[1  + s32SrcStride] * s8Kernel[8];

			res = (acc >> ps) &0xFFFF;
			p_out_u8[i] = (uchar)res;
			p_in_u8++;
		}
	}
}

// CCV_gaussian3x3_voperator - vec-c function of gaussian3x3
//
// inputs:
// p_u8Src		- pointer to the source chunk
// p_u8Dst		- pointer to the destination chunk
// s32SrcStride	- source stride represent the width + padding if applicable
// s32DstStride - destination stride represent the width + padding if applicable
// u32N			- number of rows in the tile
// u32M         - number of columns in the tile, best performance if the number is divisible by 32, the code will treat leftovers.
//*************************************************************************************************************
void CCV_gaussian3x3_voperator(uchar * RESTRICT p_u8Src, uchar * RESTRICT p_u8Dst, int s32SrcStride, int s32DstStride, uint u32N, uint u32M)
{
	uchar32 Va, Vb, Vc;
	uchar32 Vd, Ve, Vf;
	uchar32 Vg, Vh, Vi, vout;
	ushort32 vacc;
	ushort j, i;
	uint u32OutLoop = (u32M + 31) >> 5;
	unsigned int vprRightMask, vprMask;
	int s32SrcStrideM2 = s32SrcStride - 2;
	int s32SrcStrideP2 = s32SrcStride + 2;

	vprMask = 0xffffffff;
	vprRightMask = 0xffffffff;
	if (u32OutLoop != u32M >> 5)
		vprRightMask = (1 << (u32M & 31)) - 1;

	p_u8Src = p_u8Src - 1 - s32SrcStride;
	for (i = 0; i <u32OutLoop; i++)
	{
		uchar* p_in_u8  = (uchar *)&p_u8Src[i * 32];
		uchar* p_out_u8 = (uchar *)&p_u8Dst[i * 32];

		if (i == u32OutLoop - 1)
			vprMask = vprRightMask;

		for (j = 0; j < u32N; j++)
		{
			Va = *(uchar32*)p_in_u8;
			p_in_u8++;
			Vb = *(uchar32*)p_in_u8;
			p_in_u8++;
			Vc = *(uchar32*)p_in_u8;
			p_in_u8 += s32SrcStrideM2;
			Vd = *(uchar32*)p_in_u8;
			p_in_u8++;
			Ve = *(uchar32*)p_in_u8;
			p_in_u8++;
			Vf = *(uchar32*)p_in_u8;
			p_in_u8 += s32SrcStrideM2;
			Vg = *(uchar32*)p_in_u8;
			p_in_u8++;
			Vh = *(uchar32*)p_in_u8;
			p_in_u8++;
			Vi = *(uchar32*)p_in_u8;
			p_in_u8 -= s32SrcStrideP2;
			vacc = (ushort32)Va * 2 + (ushort32)Vb * 4;
			vacc += (ushort32)Vc * 2 + (ushort32)Vd * 4;
			vacc += (ushort32)Ve * 8 + (ushort32)Vf * 4;
			vacc += (ushort32)Vg * 2 + (ushort32)Vh * 4;
			vout = (uchar32)((vacc + (ushort32)Vi * 2 + (ushort32)Vi * 0) >> 5);
			vst(vout, (uchar32*)p_out_u8, vprMask);
			p_out_u8 += s32DstStride;

		}
	}
}

//*************************************************************************************************************
// CCV_gaussian3x3_vldov - vec-c function of the gaussian3x3 with optimized load
//
// inputs:
// p_u8Src		- pointer to the source chunk
// p_u8Dst		- pointer to the destination chunk
// s32SrcStride	- source stride represent the width + padding if applicable
// s32DstStride - destination stride represent the width + padding if applicable
// u32N			- number of rows in the tile
// u32M         - number of columns in the tile, best performance if the number is divisible by 32, the code will treat leftovers.
//*************************************************************************************************************
void CCV_gaussian3x3_vldov(uchar * RESTRICT p_u8Src, uchar * RESTRICT p_u8Dst, int s32SrcStep, int s32DstStep, uint u32N, uint u32M)
{
	uchar32 v00, v01, v02, vdummy;
	uchar32 v10, v11, v12;
	uchar32 v20, v21, v22, vout;
	ushort32 vacc;
	ushort j, i;
	uint u32OutLoop = (u32M + 31) >> 5;
	unsigned int vprRightMask, vprMask;
	vprMask = 0xffffffff;
	vprRightMask = 0xffffffff;
	if (u32OutLoop != u32M >> 5)
		vprRightMask = (1 << (u32M & 31)) - 1;
	p_u8Src = p_u8Src - 1 - s32SrcStep;
	for (i = 0; i <u32OutLoop; i++)
	{
		uchar* p_in_u8 = (uchar *)&p_u8Src[i * 32];
		uchar* p_out_u8 = (uchar *)&p_u8Dst[i * 32];

		if (i == u32OutLoop - 1)
			vprMask = vprRightMask;

		for (j = 0; j < u32N; j++)
		{
			vldov((uchar32*)p_in_u8, v00, v01, v02, vdummy);
			p_in_u8 += s32SrcStep;
			vldov((uchar32*)p_in_u8, v10, v11, v12, vdummy);
			p_in_u8 += s32SrcStep;
			vldov((uchar32*)p_in_u8, v20, v21, v22, vdummy);
			p_in_u8 -= s32SrcStep;
			vacc =  (ushort32)v00 * 2 + (ushort32)v01 * 4;
			vacc += (ushort32)v02 * 2 + (ushort32)v10 * 4;
			vacc += (ushort32)v11 * 8 + (ushort32)v12 * 4;
			vacc += (ushort32)v20 * 2 + (ushort32)v21 * 4;
			vout =  (uchar32)((vacc    + (ushort32)v22 * 2 + (ushort32)v22 * 0) >> 5);
			vst(vout, (uchar32*)p_out_u8, vprMask);
			p_out_u8 += s32DstStep;

		}
	}
}

//*************************************************************************************************************
// CCV_gaussian3x3_sliding_window - vec-c function of gaussian3x3 using sliding window
//
// inputs:
// p_u8Src		- pointer to the source chunk
// p_u8Dst		- pointer to the destination chunk
// s32SrcStride	- source stride represent the width + padding if applicable
// s32DstStride - destination stride represent the width + padding if applicable
// u32N			- number of rows in the tile
// u32M         - number of columns in the tile, best performance if the number is divisible by 32, the code will treat leftovers.
//*************************************************************************************************************

void CCV_gaussian3x3_sliding_window(uchar * RESTRICT p_u8Src, uchar * RESTRICT p_u8Dst, int s32SrcStep, int s32DstStep, uint u32N, uint u32M)
{
	//Calculate 32 pixels in 3 cycles (unroll 8).
	uchar32 v00,v10,v20;
	ushort16 vres;
	ushort j,i;
	uint16 vacc0;
	ushort coeff[16] = {2,4,2,0,4,8,4,0,2,4,2,0,0,0,0,0};

	uint u32OutLoop = (u32M + 15) >> 4;

	uint config0_0 = SWGEN_CTRL0(0, 0, 0, SWGEN_BASE_MODE_1D);
	uint config0_1 = SWGEN_CTRL0(0, 0, 4, SWGEN_BASE_MODE_1D);
	uint config0_2 = SWGEN_CTRL0(5, 0, 8, SWGEN_BASE_MODE_1D);

	uint config1 = SWGEN_CTRL1(FILTER_1, 0, SLICE_1, SWGEN_LINE);

	ushort16 v_coeff = *(ushort16*)coeff;
	unsigned short vprRightMask, vprMask;


	vprMask = 0xffff;
	vprRightMask = 0xffff;
	if (u32OutLoop != u32M >> 4)
		vprRightMask = (1 << (u32M & 15)) - 1;

	p_u8Src = p_u8Src - 1 - s32SrcStep;

	for (i = 0; i <u32OutLoop; i++)
	{
		uchar* p_in_u8  = (uchar *)&p_u8Src[i*16];
		uchar* p_out_u8 = (uchar *)&p_u8Dst[i*16];

		if (i == u32OutLoop-1)
			vprMask = vprRightMask;
		v00 = *(uchar32*)p_in_u8;
		p_in_u8+=s32SrcStep;
		v10 = *(uchar32*)p_in_u8;
		p_in_u8+=s32SrcStep;
		v20 = *(uchar32*)p_in_u8;
		p_in_u8+=s32SrcStep;

		for (j = 0; j < u32N; j++)

		{
			vacc0 = vswgenmac(init, v00, v00, v_coeff, v_coeff, config0_0, config1);
			vacc0 = vswgenmac(accumulate, v10, v10, v_coeff, v_coeff, config0_1, config1, vacc0);
			vres = vswgenmac(psl, v20, v20, v_coeff, v_coeff, config0_2, config1, vacc0);

			vst(vres, (uchar16*)p_out_u8, vprMask);
			p_out_u8+=s32DstStep;

			v00 = v10;
			v10 = v20;
			v20 = *(uchar32*)p_in_u8;
			p_in_u8+=s32SrcStep;

		}
	}
}


AR_S32 ArArmCalcGauss3x3(void *pParams)
{
    AR_DSP_IMG_S *pstDspImg =  (AR_DSP_IMG_S*)pParams;
    AR_U32 u32Width         = 0;
    AR_U32 u32Height        = 0;
    AR_U32 u32SrcStride     = 0;
    AR_U32 u32DstStride     = 0;
    AR_U32 au32InputAddr  	= 0;
    AR_U32 au32OutputAddr 	= 0;
    AR_U32 u32C = 0; 


	volatile unsigned long long start, end;
	start = mss_get_clock();

	for(u32C = 0; u32C< pstDspImg->u32ChannelNum; u32C++)
	{
	    u32Width = pstDspImg->astSrcChannels[u32C].u32Width;
		u32Height = pstDspImg->astSrcChannels[u32C].u32Height;
		u32SrcStride = pstDspImg->astSrcChannels[u32C].u32Stride + 2;
		u32DstStride = pstDspImg->astDstChannels[u32C].u32Stride;
		xDebugPrint("u32Width=%d u32Height=%d u32SrcStride=%d u32DstStride=%d\r\n",u32Width,u32Height,u32SrcStride,u32DstStride);
		
		au32InputAddr = pstDspImg->astSrcChannels[u32C].u64PhyAddr  + 1 + u32SrcStride * 1;
		au32OutputAddr = pstDspImg->astDstChannels[u32C].u64PhyAddr; 

		xDebugPrint("au32InputAddr=%x au32OutputAddr=%x \r\n",au32InputAddr,au32OutputAddr);
		
		CCV_gaussian3x3_sliding_window((uchar*)au32InputAddr, (uchar*)au32OutputAddr, u32SrcStride, u32DstStride, u32Height, u32Width);
	}
	end = mss_get_clock();
    xDebugPrint("==CCV_gaussian3x3_sliding_window run cycles : %llu \n",end-start);

   return 0;

}


AR_ARM_REMOTECALL_DECLARE(ArArmCalcGauss3x3) = {
    .pName = "ArArmCalcGauss3x3",
    .pRmtCallHandler = ArArmCalcGauss3x3,
};
