#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <arm_neon.h>

#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"

//#define NCHW_NHWC_NEON

#ifdef NCHW_NHWC_NEON   
AR_S32 GetConfidenceOperator(char *pcIn, char *pcOut, AR_NPU_CB_PARAM_S * pCBParams)
{
    AR_U32 h, c, b;
    AR_U32 u32B = 0, u32H = 0, u32W = 0, u32C = 0;
    u32B = pCBParams->astInputTensor[0].u32Num;
    u32H = pCBParams->astInputTensor[0].u32Height;
    u32W = pCBParams->astInputTensor[0].u32Width;
    u32C = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
    
    AR_S32 s32InZero = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 u32InKSizeNorm = pCBParams->astInputTensor[0].u32KSizeNorm;	
    AR_CHAR * pInput = pcIn;
    AR_U32 u32InPrecision = pCBParams->astInputTensor[0].u32Precision;	
    
    AR_S32 s32OutZero = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_U32 u32OutKSizeNorm = pCBParams->astOutputTensor[0].u32KSizeNorm;
    AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_CHAR * pOutput = pcOut;
    AR_U32 u32OutPrecision = pCBParams->astOutputTensor[0].u32Precision;	
    
    AR_U32 i = 0;
    AR_U32 u32BatchInOffset = u32B * u32W * u32H * u32InKSizeNorm;
    AR_U32 u32BatchOutOffset = u32B * u32W * u32H * u32OutKSizeNorm;
    AR_U32 u32HeightInOffset = u32W;
    AR_U32 u32HeightOutOffset = u32W;
	
    if(u32InPrecision == 16 && u32OutPrecision == 16)
    {
        AR_S16 *s16In = (AR_S16 *)pInput;            
        AR_S16 *s16Out = (AR_S16 *)pOutput;
        AR_S32 s32WLoop = u32W/8;
        AR_S32 s32WLeft = u32W%8;
        
        int32x4_t s32InZeroPoint = vdupq_n_s32(s32InZero);
        int32x4_t s32OutZeroPoint = vdupq_n_s32(s32OutZero);
        float32x4_t f32Scale = vdupq_n_f32(dOutScale);
        
        float32x4_t f32SumRece_High;
        float32x4_t f32SumRece_Low;
        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(i = 0; i < s32WLoop; i++)
                {
                    AR_S16* s16Load = (AR_S16 *)(s16In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * i);
                    AR_S16* s16Store = (AR_S16 *)(s16Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * i);
                    int32x4_t s32Sum_High = vdupq_n_s32(0);
                    int32x4_t s32Sum_Low = vdupq_n_s32(0);
                    for(c = 0; c < u32C; c++)
                    {
                        int16x8_t s16InVec = vld1q_s16((int16_t *)(s16Load + c * u32H * u32W));
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        int32x4_t s32Sub_High = vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint);
                        int32x4_t s32Sub_Low = vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint);
                        s32Sum_High = vaddq_s32(s32Sum_High, s32Sub_High);
                        s32Sum_Low = vaddq_s32(s32Sum_Low, s32Sub_Low);
                    }

                    float32x4_t f32Sum_High = vmulq_f32(vcvtq_f32_s32(s32Sum_High), f32Scale);
                    float32_t f32Sum_High0 = vgetq_lane_f32(f32Sum_High, 0);
                    float32_t f32Sum_High1 = vgetq_lane_f32(f32Sum_High, 1);
                    float32_t f32Sum_High2 = vgetq_lane_f32(f32Sum_High, 2);
                    float32_t f32Sum_High3 = vgetq_lane_f32(f32Sum_High, 3);
                    
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High0, f32SumRece_High, 0);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High1, f32SumRece_High, 1);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High2, f32SumRece_High, 2);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High3, f32SumRece_High, 3);  
                    
                    float32x4_t f32Sum_Low = vmulq_f32(vcvtq_f32_s32(s32Sum_Low), f32Scale);
                    float32_t f32Sum_Low0 = vgetq_lane_f32(f32Sum_Low, 0);
                    float32_t f32Sum_Low1 = vgetq_lane_f32(f32Sum_Low, 1);
                    float32_t f32Sum_Low2 = vgetq_lane_f32(f32Sum_Low, 2);
                    float32_t f32Sum_Low3 = vgetq_lane_f32(f32Sum_Low, 3);
                    
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low0, f32SumRece_Low, 0);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low1, f32SumRece_Low, 1);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low2, f32SumRece_Low, 2);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low3, f32SumRece_Low, 3);

                    for(c = 0; c < u32C; c++)
                    { 
                        float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
                        int16x8_t s16InVec = vld1q_s16((int16_t *)(s16Load + c * u32H * u32W));
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        float32x4_t f32Sub_High = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint));
                        float32x4_t f32Sub_Low = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint));
                        
                        //high
                        float32x4_t f32ScaleOut_High = vmulq_f32(f32Sub_High, f32SumRece_High);
                        int32x4_t s32Reinterpre_High = vreinterpretq_s32_f32(f32ScaleOut_High);
                        int32x4_t s32SignSignal_High = vandq_s32(s32Reinterpre_High, vdupq_n_s32(2147483648));
                        
                        
                        float32x4_t f32PlusValue_High = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_High));
                        float32x4_t f32SumValue_High = vaddq_f32(f32ScaleOut_High, f32PlusValue_High);
                                             
                        int32x4_t s32Cast_High = vcvtq_s32_f32(f32SumValue_High);
                        int32x4_t s32CastAdd_High = vqaddq_s32(s32Cast_High, s32OutZeroPoint);
                        int16x4_t s16out_High = vqmovn_s32(s32CastAdd_High);

                        //low
                        float32x4_t f32ScaleOut_Low = vmulq_f32(f32Sub_Low, f32SumRece_Low);
                        int32x4_t s32Reinterpre_Low = vreinterpretq_s32_f32(f32ScaleOut_Low);
                        int32x4_t s32SignSignal_Low = vandq_s32(s32Reinterpre_Low, vdupq_n_s32(2147483648));
                        
                        float32x4_t f32PlusValue_Low = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_Low));
                        float32x4_t f32SumValue_Low = vaddq_f32(f32ScaleOut_Low, f32PlusValue_Low);
                                             
                        int32x4_t s32Cast_Low = vcvtq_s32_f32(f32SumValue_Low);
                        int32x4_t s32CastAdd_Low = vqaddq_s32(s32Cast_Low, s32OutZeroPoint);
                        int16x4_t s16out_Low = vqmovn_s32(s32CastAdd_Low);
                        
                        int16x8_t s16out = vcombine_s16(s16out_Low, s16out_High);
                        
                        vst1q_s16((int16_t *)(s16Store + c * u32H * u32W), s16out);
                    } 	
                }
                
                for(i = 0; i < s32WLeft; i++)
                {
                    AR_DOUBLE dLeftSum = 0.0;
                    AR_S16* s16LeftLoad = (AR_S16 *)(s16In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * s32WLoop + i);
                    AR_S16* s16LeftStore = (AR_S16 *)(s16Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * s32WLoop + i);
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_S16 * s16Load1 = (AR_S16 *)(s16LeftLoad +  c * u32H * u32W);
                        dLeftSum += *s16Load1 - s32InZero;
                    }
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_S16 * s16Load1 = (AR_S16 *)(s16LeftLoad +  c * u32H * u32W);
                        AR_S16 * s16Store1 = (AR_S16 *)(s16LeftStore +  c * u32H * u32W);
                        *s16Store1 = (int)round(((*s16Load1 - s32InZero)/dLeftSum)/dOutScale) + s32OutZero;
                    }
                }
            }
        }
    }
    else if(u32InPrecision == 16 && u32OutPrecision == 8)
    {
        AR_S16 *s16In = (AR_S16 *)pInput;            
        AR_CHAR *s8Out = (AR_CHAR *)pOutput;
        AR_U32 u32Loop = u32W/8;
        AR_U32 u32Left = u32W%8;
        
        int32x4_t s32InZeroPoint = vdupq_n_s32(s32InZero);
        int32x4_t s32OutZeroPoint = vdupq_n_s32(s32OutZero);
        float32x4_t f32Scale = vdupq_n_f32(dOutScale);
        
        float32x4_t f32SumRece_High, f32SumRece_Low;
        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(i = 0; i < u32Loop; i++)
                {
                    AR_S16* s16Load = (AR_S16 *)(s16In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * i);
                    AR_CHAR* s8Store = (AR_CHAR *)(s8Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * i);
                    int32x4_t s32Sum_High = vdupq_n_s32(0);
                    int32x4_t s32Sum_Low = vdupq_n_s32(0);
                    
                    for(c = 0; c < u32C; c++)
                    { 
                        int16x8_t s16InVec = vld1q_s16((int16_t *)(s16Load + c * u32H * u32W));
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        int32x4_t s32Sub_High = vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint);
                        int32x4_t s32Sub_Low = vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint);
                        s32Sum_High = vaddq_s32(s32Sum_High, s32Sub_High);
                        s32Sum_Low = vaddq_s32(s32Sum_Low, s32Sub_Low);
                    }

                    float32x4_t f32Sum_High = vmulq_f32(vcvtq_f32_s32(s32Sum_High), f32Scale);
                    float32_t f32Sum_High0 = vgetq_lane_f32(f32Sum_High, 0);
                    float32_t f32Sum_High1 = vgetq_lane_f32(f32Sum_High, 1);
                    float32_t f32Sum_High2 = vgetq_lane_f32(f32Sum_High, 2);
                    float32_t f32Sum_High3 = vgetq_lane_f32(f32Sum_High, 3);
                    
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High0, f32SumRece_High, 0);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High1, f32SumRece_High, 1);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High2, f32SumRece_High, 2);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High3, f32SumRece_High, 3);  
                    
                    float32x4_t f32Sum_Low = vmulq_f32(vcvtq_f32_s32(s32Sum_Low), f32Scale);
                    float32_t f32Sum_Low0 = vgetq_lane_f32(f32Sum_Low, 0);
                    float32_t f32Sum_Low1 = vgetq_lane_f32(f32Sum_Low, 1);
                    float32_t f32Sum_Low2 = vgetq_lane_f32(f32Sum_Low, 2);
                    float32_t f32Sum_Low3 = vgetq_lane_f32(f32Sum_Low, 3);
                    
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low0, f32SumRece_Low, 0);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low1, f32SumRece_Low, 1);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low2, f32SumRece_Low, 2);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low3, f32SumRece_Low, 3);

                    for(c = 0; c < u32C; c++)
                    { 
                        float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
                        int16x8_t s16InVec = vld1q_s16((int16_t *)(s16Load + c * u32H * u32W));
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        float32x4_t f32Sub_High = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint));
                        float32x4_t f32Sub_Low = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint));
                        
                        //high
                        float32x4_t f32ScaleOut_High = vmulq_f32(f32Sub_High, f32SumRece_High);
                        int32x4_t s32Reinterpre_High = vreinterpretq_s32_f32(f32ScaleOut_High);
                        int32x4_t s32SignSignal_High = vandq_s32(s32Reinterpre_High, vdupq_n_s32(2147483648));
                        
                        float32x4_t f32PlusValue_High = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_High));
                        float32x4_t f32SumValue_High = vaddq_f32(f32ScaleOut_High, f32PlusValue_High);
                                                 
                        int32x4_t s32Cast_High = vcvtq_s32_f32(f32SumValue_High);
                        int32x4_t s32CastAdd_High = vqaddq_s32(s32Cast_High, s32OutZeroPoint);
                        int16x4_t s16out_High = vqmovn_s32(s32CastAdd_High);

                        //low
                        float32x4_t f32ScaleOut_Low = vmulq_f32(f32Sub_Low, f32SumRece_Low);
                        int32x4_t s32Reinterpre_Low = vreinterpretq_s32_f32(f32ScaleOut_Low);
                        int32x4_t s32SignSignal_Low = vandq_s32(s32Reinterpre_Low, vdupq_n_s32(2147483648));
                        
                        float32x4_t f32PlusValue_Low = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_Low));
                        float32x4_t f32SumValue_Low = vaddq_f32(f32ScaleOut_Low, f32PlusValue_Low);
                                             
                        int32x4_t s32Cast_Low = vcvtq_s32_f32(f32SumValue_Low);
                        int32x4_t s32CastAdd_Low = vqaddq_s32(s32Cast_Low, s32OutZeroPoint);
                        int16x4_t s16out_Low = vqmovn_s32(s32CastAdd_Low);
                        
                        int16x8_t s16out = vcombine_s16(s16out_Low, s16out_High);
                        int8x8_t s8out = vqmovn_s16(s16out);
                        
                        vst1_s8((int8_t *)(s8Store + c * u32H * u32W), s8out);
                    }        
                }

                for(i = 0; i < u32Left; i++)
                {
                    AR_DOUBLE dLeftSum = 0.0;
                    AR_S16* s16LeftLoad = (AR_S16 *)(s16In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * u32Loop + i);
                    AR_CHAR* s8LeftStore = (AR_CHAR *)(s8Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * u32Loop + i);
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_S16 * s16Load1 = (AR_S16 *)(s16LeftLoad +  c * u32H * u32W);
                        dLeftSum += *s16Load1 - s32InZero;
                    }
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_S16 * s16Load1 = (AR_S16 *)(s16LeftLoad +  c * u32H * u32W);
                        AR_CHAR * s8Store1 = (AR_CHAR *)(s8LeftStore +  c * u32H * u32W);
                        *s8Store1 = (int)round(((*s16Load1 - s32InZero)/dLeftSum)/dOutScale) + s32OutZero;
                    }
                }
            }
        }
    }
    else if(u32InPrecision == 8  && u32OutPrecision == 8)
    {
        AR_CHAR *s8In = (AR_CHAR *)pInput;            
        AR_CHAR *s8Out = (AR_CHAR *)pOutput;
        AR_U32 u32Loop = u32W/8;
        AR_U32 u32Left = u32W%8;
        
        int32x4_t s32InZeroPoint = vdupq_n_s32(s32InZero);
        int32x4_t s32OutZeroPoint = vdupq_n_s32(s32OutZero);
        float32x4_t f32Scale = vdupq_n_f32(dOutScale);
        
        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(i = 0; i < u32Loop; i++)
                {
                    AR_CHAR* s8Load = (AR_CHAR *)(s8In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * i);
                    AR_CHAR* s8Store = (AR_CHAR *)(s8Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * i);
                    int32x4_t s32Sum_High = vdupq_n_s32(0);
                    int32x4_t s32Sum_Low = vdupq_n_s32(0);
                    
                    for(c = 0; c < u32C; c++)
                    { 
                        int8x8_t s8InVec = vld1_s8((int8_t *)(s8Load + c * u32W * u32H));
                        
                        int16x8_t s16InVec = vmovl_s8(s8InVec);
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        int32x4_t s32Sub_High = vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint);
                        int32x4_t s32Sub_Low = vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint);
                        s32Sum_High = vaddq_s32(s32Sum_High, s32Sub_High);
                        s32Sum_Low = vaddq_s32(s32Sum_Low, s32Sub_Low);
                    }

                    float32x4_t f32Sum_High = vmulq_f32(vcvtq_f32_s32(s32Sum_High), f32Scale);
                    float32_t f32Sum_High0 = vgetq_lane_f32(f32Sum_High, 0);
                    float32_t f32Sum_High1 = vgetq_lane_f32(f32Sum_High, 1);
                    float32_t f32Sum_High2 = vgetq_lane_f32(f32Sum_High, 2);
                    float32_t f32Sum_High3 = vgetq_lane_f32(f32Sum_High, 3);
                    
                    float32x4_t f32Sum_Low = vmulq_f32(vcvtq_f32_s32(s32Sum_Low), f32Scale);
                    float32_t f32Sum_Low0 = vgetq_lane_f32(f32Sum_Low, 0);
                    float32_t f32Sum_Low1 = vgetq_lane_f32(f32Sum_Low, 1);
                    float32_t f32Sum_Low2 = vgetq_lane_f32(f32Sum_Low, 2);
                    float32_t f32Sum_Low3 = vgetq_lane_f32(f32Sum_Low, 3);
                    
                    float32x4_t f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High0, f32SumRece_High, 0);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High1, f32SumRece_High, 1);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High2, f32SumRece_High, 2);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High3, f32SumRece_High, 3);  
                    
                    float32x4_t f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low0, f32SumRece_Low, 0);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low1, f32SumRece_Low, 1);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low2, f32SumRece_Low, 2);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low3, f32SumRece_Low, 3);
                    
                    for(c = 0; c < u32C; c++)
                    { 
                        float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
                        
                        int8x8_t s8InVec = vld1_s8((int8_t *)(s8Load + c * u32W * u32H));
                        int16x8_t s16InVec = vmovl_s8(s8InVec);
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        float32x4_t f32Sub_High = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_High), s32OutZeroPoint));
                        float32x4_t f32Sub_Low = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_Low), s32OutZeroPoint));
                        
                        float32x4_t f32ScaleOut_High = vmulq_f32(f32Sub_High, f32SumRece_High);
                        int32x4_t s32Reinterpre_High = vreinterpretq_s32_f32(f32ScaleOut_High);
                        int32x4_t s32SignSignal_High = vandq_s32(s32Reinterpre_High, vdupq_n_s32(2147483648));
                        float32x4_t f32PlusValue_High = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_High));
                        float32x4_t f32SumValue_High = vaddq_f32(f32ScaleOut_High, f32PlusValue_High);                               
                        int32x4_t s32Cast_High = vcvtq_s32_f32(f32SumValue_High);
                        int32x4_t s32CastAdd_High = vqaddq_s32(s32Cast_High, s32OutZeroPoint);
                        int16x4_t s16Out_High = vqmovn_s32(s32CastAdd_High);

                        float32x4_t f32ScaleOut_Low= vmulq_f32(f32Sub_Low, f32SumRece_Low);
                        int32x4_t s32Reinterpre_Low = vreinterpretq_s32_f32(f32ScaleOut_Low);
                        int32x4_t s32SignSignal_Low = vandq_s32(s32Reinterpre_Low, vdupq_n_s32(2147483648));
                        float32x4_t f32PlusValue_Low = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_Low));
                        float32x4_t f32SumValue_Low = vaddq_f32(f32ScaleOut_Low, f32PlusValue_Low);                               
                        int32x4_t s32Cast_Low = vcvtq_s32_f32(f32SumValue_Low);
                        int32x4_t s32CastAdd_Low = vqaddq_s32(s32Cast_Low, s32OutZeroPoint);
                        int16x4_t s16Out_Low = vqmovn_s32(s32CastAdd_Low);
                        
                        int16x8_t s16Out = vcombine_s16(s16Out_Low, s16Out_High);
                        int8x8_t s8Out = vqmovn_s16(s16Out);
                        vst1_s8((int8_t *)(s8Store + c * u32W * u32H), s8Out);                     
                    }                          
                }

                for(i = 0; i < u32Left; i++)
                {
                    AR_DOUBLE dLeftSum = 0.0;
                    AR_CHAR* s8LeftLoad = (AR_CHAR *)(s8In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * u32Loop + i);
                    AR_CHAR* s8LeftStore = (AR_CHAR *)(s8Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * u32Loop + i);
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_CHAR * s16Load1 = (AR_CHAR *)(s8LeftLoad +  c * u32H * u32W);
                        dLeftSum += *s16Load1 - s32InZero;
                    }
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_CHAR * s8Load1 = (AR_CHAR *)(s8LeftLoad +  c * u32H * u32W);
                        AR_CHAR * s8Store1 = (AR_CHAR *)(s8LeftStore +  c * u32H * u32W);
                        *s8Store1 = (int)round(((*s8Load1 - s32InZero)/dLeftSum)/dOutScale) + s32OutZero;
                    }
                }
            }
        }
    }   
    if(u32InPrecision == 8  && u32OutPrecision == 16)
    {
        AR_CHAR *s8In = (AR_CHAR *)pInput;            
        AR_S16 *s16Out = (AR_S16 *)pOutput;
        AR_U32 u32Loop = u32W/8;
        AR_U32 u32Left = u32W%8;
        
        int32x4_t s32InZeroPoint = vdupq_n_s32(s32InZero);
        int32x4_t s32OutZeroPoint = vdupq_n_s32(s32OutZero);
        float32x4_t f32Scale = vdupq_n_f32(dOutScale);

        for(b = 0; b < u32B; b++)
        {
            for(h = 0; h < u32H; h++)
            {
                for(i = 0; i < u32Loop; i++)
                {
                    AR_CHAR* s8Load = (AR_CHAR *)(s8In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * i);
                    AR_S16* s16Store = (AR_S16 *)(s16Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * i);
                    int32x4_t s32Sum_High = vdupq_n_s32(0);
                    int32x4_t s32Sum_Low = vdupq_n_s32(0);
                    
                    for(c = 0; c < u32C; c++)
                    { 
                        int8x8_t s8InVec = vld1_s8((int8_t *)(s8Load + c * u32W * u32H));
                        
                        int16x8_t s16InVec = vmovl_s8(s8InVec);
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        int32x4_t s32Sub_High = vsubq_s32(vmovl_s16(s16InVec_High), s32InZeroPoint);
                        int32x4_t s32Sub_Low = vsubq_s32(vmovl_s16(s16InVec_Low), s32InZeroPoint);
                        s32Sum_High = vaddq_s32(s32Sum_High, s32Sub_High);
                        s32Sum_Low = vaddq_s32(s32Sum_Low, s32Sub_Low);
                    }

                    float32x4_t f32Sum_High = vmulq_f32(vcvtq_f32_s32(s32Sum_High), f32Scale);
                    float32_t f32Sum_High0 = vgetq_lane_f32(f32Sum_High, 0);
                    float32_t f32Sum_High1 = vgetq_lane_f32(f32Sum_High, 1);
                    float32_t f32Sum_High2 = vgetq_lane_f32(f32Sum_High, 2);
                    float32_t f32Sum_High3 = vgetq_lane_f32(f32Sum_High, 3);
                    
                    float32x4_t f32Sum_Low = vmulq_f32(vcvtq_f32_s32(s32Sum_Low), f32Scale);
                    float32_t f32Sum_Low0 = vgetq_lane_f32(f32Sum_Low, 0);
                    float32_t f32Sum_Low1 = vgetq_lane_f32(f32Sum_Low, 1);
                    float32_t f32Sum_Low2 = vgetq_lane_f32(f32Sum_Low, 2);
                    float32_t f32Sum_Low3 = vgetq_lane_f32(f32Sum_Low, 3);
                    
                    float32x4_t f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High0, f32SumRece_High, 0);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High1, f32SumRece_High, 1);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High2, f32SumRece_High, 2);
                    f32SumRece_High = vsetq_lane_f32(1.0 /f32Sum_High3, f32SumRece_High, 3);  
                    
                    float32x4_t f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low0, f32SumRece_Low, 0);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low1, f32SumRece_Low, 1);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low2, f32SumRece_Low, 2);
                    f32SumRece_Low = vsetq_lane_f32(1.0 /f32Sum_Low3, f32SumRece_Low, 3);
                    
                    for(c = 0; c < u32C; c++)
                    { 
                        float32x4_t f32RoundVal = vdupq_n_f32(0.5f);
                        
                        int8x8_t s8InVec = vld1_s8((int8_t *)(s8Load + c * u32W * u32H));
                        int16x8_t s16InVec = vmovl_s8(s8InVec);
                        int16x4_t s16InVec_High = vget_high_s16(s16InVec);
                        int16x4_t s16InVec_Low = vget_low_s16(s16InVec);
                        float32x4_t f32Sub_High = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_High), s32OutZeroPoint));
                        float32x4_t f32Sub_Low = vcvtq_f32_s32(vsubq_s32(vmovl_s16(s16InVec_Low), s32OutZeroPoint));
                        
                        float32x4_t f32ScaleOut_High = vmulq_f32(f32Sub_High, f32SumRece_High);
                        int32x4_t s32Reinterpre_High = vreinterpretq_s32_f32(f32ScaleOut_High);
                        int32x4_t s32SignSignal_High = vandq_s32(s32Reinterpre_High, vdupq_n_s32(2147483648));
                        float32x4_t f32PlusValue_High = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_High));
                        float32x4_t f32SumValue_High = vaddq_f32(f32ScaleOut_High, f32PlusValue_High);                               
                        int32x4_t s32Cast_High = vcvtq_s32_f32(f32SumValue_High);
                        int32x4_t s32CastAdd_High = vqaddq_s32(s32Cast_High, s32OutZeroPoint);
                        int16x4_t s16Out_High = vqmovn_s32(s32CastAdd_High);

                        float32x4_t f32ScaleOut_Low= vmulq_f32(f32Sub_Low, f32SumRece_Low);
                        int32x4_t s32Reinterpre_Low = vreinterpretq_s32_f32(f32ScaleOut_Low);
                        int32x4_t s32SignSignal_Low = vandq_s32(s32Reinterpre_Low, vdupq_n_s32(2147483648));
                        float32x4_t f32PlusValue_Low = vreinterpretq_f32_s32(vorrq_s32(vreinterpretq_s32_f32(f32RoundVal), s32SignSignal_Low));
                        float32x4_t f32SumValue_Low = vaddq_f32(f32ScaleOut_Low, f32PlusValue_Low);                               
                        int32x4_t s32Cast_Low = vcvtq_s32_f32(f32SumValue_Low);
                        int32x4_t s32CastAdd_Low = vqaddq_s32(s32Cast_Low, s32OutZeroPoint);
                        int16x4_t s16Out_Low = vqmovn_s32(s32CastAdd_Low);
                        
                        int16x8_t s16Out = vcombine_s16(s16Out_Low, s16Out_High);
                        //int8x8_t s8Out = vqmovn_s16(s16Out);
                        vst1q_s16((int16_t *)(s16Store + c * u32W * u32H), s16Out);                     
                    }                          
                        
                }        

                for(i = 0; i < u32Left; i++)
                {
                    AR_DOUBLE dLeftSum = 0.0;
                    AR_CHAR* s8LeftLoad = (AR_CHAR *)(s8In + b * u32BatchInOffset + h * u32HeightInOffset + 8 * u32Loop + i);
                    AR_S16* s16LeftStore = (AR_S16 *)(s16Out + b * u32BatchOutOffset + h * u32HeightOutOffset + 8 * u32Loop + i);
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_CHAR * s16Load1 = (AR_CHAR *)(s8LeftLoad +  c * u32H * u32W);
                        dLeftSum += *s16Load1 - s32InZero;
                    }
                    
                    for(c = 0; c < u32C; c++)
                    {
                        AR_CHAR * s8Load1 = (AR_CHAR *)(s8LeftLoad +  c * u32H * u32W);
                        AR_S16 * s16Store1 = (AR_S16 *)(s16LeftStore +  c * u32H * u32W);
                        *s16Store1 = (int)round(((*s8Load1 - s32InZero)/dLeftSum)/dOutScale) + s32OutZero;
                    }
                }
            }
        }
    } 
    
    return 0;
}

    AR_S32 AR_CNN_OperatorGetConf(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
    {
    
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Offset;   
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;
    
    AR_U32 u32InputMemSize = pCBParams->astInputTensor[0].u32Size * pCBParams->astInputTensor[0].u32Precision/8;
    AR_U32 u32OutMemSize = pCBParams->astOutputTensor[0].u32Size * pCBParams->astOutputTensor[0].u32Precision/8;
    
    AR_CHAR * pSoftmaxInput = NULL;
    AR_CHAR * pSoftmaxOutput = NULL;
    pSoftmaxInput = (AR_CHAR *)malloc(u32InputMemSize);
    if (!pSoftmaxInput)
    {
        printf("Malloc softmax input error.\n");
        return -1;
    }

    pSoftmaxOutput = (AR_CHAR *)malloc(u32OutMemSize);
    if (!pSoftmaxOutput)
    {
        free(pSoftmaxInput);
        printf("Malloc softmax output error.\n");
        return -1;
    }
    
    memset(pSoftmaxInput, 0, u32InputMemSize);
    memset(pSoftmaxOutput, 0, u32OutMemSize);
    
    //nhwc->nchw
    dcnn_hwc_to_chw((AR_CHAR *)pInput, pSoftmaxInput, &pCBParams->astInputTensor[0]);
    
    //getconfidenc
    GetConfidenceOperator(pSoftmaxInput, pSoftmaxOutput, pCBParams);
    
    //nchw->nhwc
    dcnn_chw_to_hwc(pSoftmaxOutput, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
    
    free(pSoftmaxInput);
    free(pSoftmaxOutput);
    
    return 0;
}

#else
AR_S32 AR_CNN_OperatorGetConf(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
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
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Offset;
	AR_U32 u32InByte = pCBParams->astInputTensor[0].u32Precision/8;

	AR_U32 u32OutKStep = pCBParams->astOutputTensor[0].u32KStep;
	AR_U32 u32OutRowStep = pCBParams->astOutputTensor[0].u32RowStep;
    AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
    AR_S32 s32OutZero = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;
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
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeLast * u32InByte);
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
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeLast * u32OutByte));
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
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeLast * u32InByte);
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
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeLast * u32OutByte));
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
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeLast * u32InByte);
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
	                    pOutC = (AR_S16 *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeLast * u32OutByte));
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
	                    AR_U32 offset =(b * u32InTensorStep + u32InKNormNum * u32InKStep + h * u32InRowStep + w * u32InKSizeLast * u32InByte);
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
	                    pOutC = (signed char *)(pOutput + (b * u32OutTensorStep + u32OutKNormNum * u32OutKStep + h * u32OutRowStep + w * u32OutKSizeLast * u32OutByte));
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

#endif

#if 0
AR_S32 AR_CNN_OperatorGetConf(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
    AR_U32 h, w, c, b;
    AR_U32 u32B = 0, u32H = 0, u32W = 0, u32C = 0;
    AR_U32 u32RowStep = pCBParams->astInputTensor[0].u32RowStep;
    AR_S32 s32InZero = pCBParams->astInputTensor[0].s32ZeroPoint;
    AR_U32 u32InTensorStep = pCBParams->astInputTensor[0].u32TensorStep;
    AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Offset;

    AR_S32 s32OutZero = pCBParams->astOutputTensor[0].s32ZeroPoint;
    AR_DOUBLE dOutScale = pCBParams->astOutputTensor[0].dScaleFactor;
    AR_U32 u32OutTensorStep = pCBParams->astOutputTensor[0].u32TensorStep;
    AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Offset;

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

