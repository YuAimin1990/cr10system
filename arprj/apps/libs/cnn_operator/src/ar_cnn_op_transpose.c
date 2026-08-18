#include "stdio.h"
#include "string.h"
#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"
#include <arm_neon.h>
#include "ar_cnn_channelcvt.h"


AR_S32 _AR_CNN_TransposeForward(AR_CHAR * pInF, AR_CHAR * pOutF, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_TRANSPOSE_PARAM_S *pTransposeParam)
{
	AR_U32 u32Reg = 128;

	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32OriChannels;
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	
	AR_U32 u32OutBatch = pCBParams->astOutputTensor[0].u32Num;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32OriChannels;
  	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
  	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;
	
	AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;
	AR_U32 u32InSingleSize = u32InChannel * u32InChSize;
	AR_U32 u32OutSingleSize = u32OutChannel * u32OutChSize;
 	AR_U32 u32InDataSize = u32InSingleSize * u32Batch;
  	AR_U32 u32OutDataSize = u32OutSingleSize * u32OutBatch;
	
	AR_U32 u32Offset = pCBParams->astOutputTensor[0].u32Offset;
	AR_U32 u32NumInReg = u32Reg / u32Precision;
	AR_U32 u32NumInRegM = u32NumInReg / 2;
	
  	// --> 0231
  	if (pTransposeParam->u32Perm[0]==0 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==1)
  	{
		AR_U32 u32CLoop = u32InChannel / u32NumInReg;
		AR_U32 u32CLeft = u32InChannel % u32NumInReg;
		AR_U32 u32CLoopM = u32CLeft / u32NumInRegM;
		AR_U32 u32CLeftM = u32CLeft % u32NumInRegM;
		
		AR_U32 u32WLoop = u32InWidth / u32NumInReg;
		AR_U32 u32WLeft = u32InWidth % u32NumInReg;
		AR_U32 u32WLoopM = u32WLeft / u32NumInRegM;
		AR_U32 u32WLeftM = u32WLeft % u32NumInRegM;

		if(u32Precision == 16)
      	{
      		AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;

			#if 0
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{
                          	ps16Out[b * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for(int b=0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for(int h=0; h<u32InHeight; h++)	
				{
					AR_U32 u32SrcHoffset = h * u32InWidth;
					AR_U32 u32DstHoffset = h * u32InWidth * u32InChannel;
					for(int w=0; w<u32WLoop; w++)
					{
						AR_S16* pInCLoop = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + w*u32NumInReg);
						AR_S16* pOutCLoop = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + w*u32NumInReg*u32InChannel);
						for(int c=0; c<u32CLoop; c++)
						{
							AR_NPU_Matrix8x8_S16_Convert(pInCLoop + c*u32NumInReg*u32InChSize, pOutCLoop + c*u32NumInReg, u32InChSize, u32InChannel);
						}
						
						if(u32CLeft)
						{
							AR_S16* pInCLeft = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + u32CLoop*u32NumInReg*u32InChSize);
							AR_S16* pOutCLeft = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + u32CLoop*u32NumInReg);
							for(int c=0; c<u32CLoopM; c++)
							{
								AR_S16* pInCLoopM = (AR_S16*)(pInCLeft + c*u32NumInRegM*u32InChSize);
								AR_S16* pOutCLoopM = (AR_S16*)(pOutCLeft + c*u32NumInRegM);

								AR_NPU_Matrix4x4_S16_Convert(pInCLoopM + (w*2)*u32NumInRegM, pOutCLoopM + (w*2)*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
								AR_NPU_Matrix4x4_S16_Convert(pInCLoopM + (w*2+1)*u32NumInRegM, pOutCLoopM + (w*2+1)*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
							}
							for(int c=0; c<u32CLeftM; c++)
							{
								AR_S16* pInCLeftM = (AR_S16*)(pInCLeft + (u32CLoopM*u32NumInRegM+c)*u32InChSize);
								AR_S16* pOutCLeftM = (AR_S16*)(pOutCLeft + (u32CLoopM*u32NumInRegM+c));
								for(int wc=0; wc<u32NumInReg; wc++)
								{
									pOutCLeftM[(w*u32NumInReg+wc)*u32InChannel] = pInCLeftM[w*u32NumInReg + wc];
								}
							}
						}
					}
					if(u32WLeft)
					{
						AR_S16* pInWLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + u32WLoop*u32NumInReg);
						AR_S16* pOutWLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + (u32WLoop*u32NumInReg)*u32InChannel);
						for(int w=0; w<u32WLoopM; w++)
						{
							for(int c=0; c<u32CLoop*2+u32CLoopM; c++)
							{
								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + c*u32NumInRegM*u32InChSize + w*u32NumInRegM, pOutWLoopM + c*u32NumInRegM + w*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
							}
						}
						for(int c=0; c<u32CLeftM; c++)
						{
							for(int w=0; w<u32WLoopM*u32NumInRegM; w++)
							{
								pOutWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM+c) + w*u32InChannel] = pInWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM+c)*u32InChSize + w];
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							AR_S16* pInWLeftM = (AR_S16*)(pInWLoopM + u32WLoopM*u32NumInRegM + w);
							AR_S16* pOutWLeftM = (AR_S16*)(pOutWLoopM + (u32WLoopM*u32NumInRegM + w)*u32InChannel);
							for(int c=0; c<u32InChannel; c++)
							{
								pOutWLeftM[c] = pInWLeftM[c*u32InChSize];
							}
						}
					}
				}
			}
			#endif
  		}
      	else
      	{
      		AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;

			#if 0
          	for (int b = 0; b<u32Batch; b++)
         	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[b * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      				  	}
      			  	}
      		  	}
      	  	}
			#else
			for(int b=0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for(int h=0; h<u32InHeight; h++)	
				{
					AR_U32 u32SrcHoffset = h * u32InWidth;
					AR_U32 u32DstHoffset = h * u32InWidth * u32InChannel;
					for(int w=0; w<u32WLoop; w++)
					{
						AR_S8* pInCLoop = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + w*u32NumInReg);
						AR_S8* pOutCLoop = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + w*u32NumInReg*u32InChannel);
						for(int c=0; c<u32CLoop; c++)
						{
							AR_NPU_Matrix16x16_S8_Convert(pInCLoop + c*u32NumInReg*u32InChSize, pOutCLoop + c*u32NumInReg, u32InChSize, u32InChannel);
						}
						
						if(u32CLeft)
						{
							AR_S8* pInCLeft = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + u32CLoop*u32NumInReg*u32InChSize);
							AR_S8* pOutCLeft = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + u32CLoop*u32NumInReg);
							for(int c=0; c<u32CLoopM; c++)
							{
								AR_S8* pInCLoopM = (AR_S8*)(pInCLeft + c*u32NumInRegM*u32InChSize);
								AR_S8* pOutCLoopM = (AR_S8*)(pOutCLeft + c*u32NumInRegM);

								AR_NPU_Matrix8x8_S8_Convert(pInCLoopM + (w*2)*u32NumInRegM, pOutCLoopM + (w*2)*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
								AR_NPU_Matrix8x8_S8_Convert(pInCLoopM + (w*2+1)*u32NumInRegM, pOutCLoopM + (w*2+1)*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
							}
							
							for(int c=0; c<u32CLeftM; c++)
							{
								AR_S8* pInCLeftM = (AR_S8*)(pInCLeft + (u32CLoopM*u32NumInRegM+c)*u32InChSize);
								AR_S8* pOutCLeftM = (AR_S8*)(pOutCLeft + (u32CLoopM*u32NumInRegM+c));
								for(int wc=0; wc<u32NumInReg; wc++)
								{
									pOutCLeftM[(w*u32NumInReg+wc)*u32InChannel] = pInCLeftM[w*u32NumInReg + wc];
								}
							}
						}
					}
					if(u32WLeft)
					{
						AR_S8* pInWLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + u32WLoop*u32NumInReg);
						AR_S8* pOutWLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + (u32WLoop*u32NumInReg)*u32InChannel);
						for(int w=0; w<u32WLoopM; w++)
						{
							for(int c=0; c<u32CLoop*2+u32CLoopM; c++)
							{
								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + c*u32NumInRegM*u32InChSize + w*u32NumInRegM, pOutWLoopM + c*u32NumInRegM + w*u32NumInRegM*u32InChannel, u32InChSize, u32InChannel);
							}
						}
						for(int c=0; c<u32CLeftM; c++)
						{
							for(int w=0; w<u32WLoopM*u32NumInRegM; w++)
							{
								pOutWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM+c) + w*u32InChannel] = pInWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM+c)*u32InChSize + w];
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							AR_S8* pInWLeftM = (AR_S8*)(pInWLoopM + (u32WLoopM*u32NumInRegM + w));
							AR_S8* pOutWLeftM = (AR_S8*)(pOutWLoopM + (u32WLoopM*u32NumInRegM + w)*u32InChannel);
							for(int c=0; c<u32InChannel; c++)
							{
								pOutWLeftM[c] = pInWLeftM[c*u32InChSize];
							}
						}
					}
				}
			}
			#endif
  		}
  	} 
	// --> 0 1 3 2
   	else if (pTransposeParam->u32Perm[0]==0 && pTransposeParam->u32Perm[1]==1 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==2)
   	{
		AR_U32 u32HLoop = u32InHeight / u32NumInReg;
		AR_U32 u32HLeft = u32InHeight % u32NumInReg;
		AR_U32 u32HLoopM = u32HLeft / u32NumInRegM;
		AR_U32 u32HLeftM = u32HLeft % u32NumInRegM;
		
		AR_U32 u32WLoop = u32InWidth / u32NumInReg;
		AR_U32 u32WLeft = u32InWidth % u32NumInReg;
		AR_U32 u32WLoopM = u32WLeft / u32NumInRegM;
		AR_U32 u32WLeftM = u32WLeft % u32NumInRegM;

		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;

			#if 1
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[b * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for (int b = 0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for (int c =0; c<u32InChannel; c++)
				{
					AR_U32 u32SrcCoffset = c * u32InChSize;
					AR_U32 u32DstCoffset = u32SrcCoffset;
					for(int h=0; h<u32HLoop; h++)
					{
						AR_S16* pInWLoop = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + h*u32NumInReg*u32InWidth);
						AR_S16* pOutWLoop = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + h*u32NumInReg);
						for(int w=0; w<u32WLoop; w++)
						{
							AR_NPU_Matrix8x8_S16_Convert(pInWLoop + w*u32NumInReg, pOutWLoop + w*u32NumInReg*u32InHeight, u32InWidth, u32InHeight);
						}
						if(u32WLeft)
						{
							for(int w=0; w<u32WLoopM; w++)
							{
								AR_S16* pInWLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg+w*u32NumInRegM));
								AR_S16* pOutWLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg+w*u32NumInRegM)*u32InHeight);

								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + (h*2)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2)*u32NumInRegM, u32InWidth, u32InHeight);
								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + (h*2+1)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2+1)*u32NumInRegM, u32InWidth, u32InHeight);
							}
							for(int w=0; w<u32WLeftM; w++)
							{
								AR_S16* pInWLeftM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w));
								AR_S16* pOutLeftM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w)*u32InHeight);
								for(int hw=0; hw<u32NumInReg; hw++)
								{
									pOutLeftM[h*u32NumInReg+hw] = pInWLeftM[(h*u32NumInReg+hw)*u32InWidth];
								}
							}
						}
					}
					if(u32HLeft)
					{
						AR_S16* pInHLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + u32HLoop*u32NumInReg*u32InWidth);
						AR_S16* pOutHLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + u32HLoop*u32NumInReg);
						for(int h=0; h<u32HLoopM; h++)
						{
							for(int w=0; w<u32WLoop*2+u32WLoopM; w++)
							{
								AR_NPU_Matrix4x4_S16_Convert(pInHLoopM + h*u32NumInRegM*u32InWidth + w*u32NumInRegM, pOutHLoopM + h*u32NumInRegM + w*u32NumInRegM*u32InHeight, u32InWidth, u32InHeight);
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							for(int h=0; h<u32HLoopM*u32NumInRegM; h++)
							{
								pOutHLoopM[h + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)*u32InHeight] = pInHLoopM[h*u32InWidth + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)];
							}
						}
						for(int h=0; h<u32HLeftM; h++)
						{
							AR_S16* pInHLeftM = (AR_S16*)(pInHLoopM + (u32HLoopM*u32NumInRegM + h)*u32InWidth);
							AR_S16* pOutHLeftM = (AR_S16*)(pOutHLoopM + (u32HLoopM*u32NumInRegM + h));
							for(int w=0; w<u32InWidth; w++)
							{
								pOutHLeftM[w*u32InHeight] = pInHLeftM[w];
							}
						}
					}
				}
			}

			#endif
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;

			#if 1
			for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
					for (int h =0; h<u32InHeight; h++)
                  	{
						for (int w =0; w<u32InWidth; w++)
                      	{  
                        	ps8Out[b * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
              	}
			}
			#else
	        for (int b = 0; b<u32Batch; b++)
          	{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for (int c =0; c<u32InChannel; c++)
              	{
					AR_U32 u32SrcCoffset = c * u32InChSize;
					AR_U32 u32DstCoffset = u32SrcCoffset;
					for(int h=0; h<u32HLoop; h++)
					{
						AR_S8* pInWLoop = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + h*u32NumInReg*u32InWidth);
						AR_S8* pOutWLoop = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + h*u32NumInReg);
						for(int w=0; w<u32WLoop; w++)
						{
							AR_NPU_Matrix16x16_S8_Convert(pInWLoop + w*u32NumInReg, pOutWLoop + w*u32NumInReg*u32InHeight, u32InWidth, u32InHeight);
						}
						if(u32WLeft)
						{
							for(int w=0; w<u32WLoopM; w++)
							{
								AR_S8* pInWLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + w*u32NumInRegM));
								AR_S8* pOutWLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg + w*u32NumInRegM)*u32InHeight);

								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + (h*2)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2)*u32NumInRegM, u32InWidth, u32InHeight);
								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + (h*2+1)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2+1)*u32NumInRegM, u32InWidth, u32InHeight);
							}
							for(int w=0; w<u32WLeftM; w++)
							{
								AR_S8* pInWLeftM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w));
								AR_S8* pOutWLeftM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w)*u32InHeight);
								for(int hw=0; hw<u32NumInReg; hw++)
								{
									pOutWLeftM[h*u32NumInReg + hw] = pInWLeftM[(h*u32NumInReg + hw)*u32InWidth];
								}
							}
						}
					}
					if(u32HLeft)
					{
						AR_S8* pInHLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + u32HLoop*u32NumInReg*u32InWidth);
						AR_S8* pOutHLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + u32HLoop*u32NumInReg);
						for(int h=0; h<u32HLoopM; h++)
						{
							for(int w=0; w<u32WLoop*2+u32WLoopM; w++)
							{
								AR_NPU_Matrix8x8_S8_Convert(pInHLoopM + h*u32NumInRegM*u32InWidth + w*u32NumInRegM, pOutHLoopM + h*u32NumInRegM + w*u32NumInRegM*u32InHeight, u32InWidth, u32InHeight);
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							for(int h=0; h<u32HLoopM*u32NumInRegM; h++)
							{
								pOutHLoopM[h + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)*u32InHeight] = pInHLoopM[h*u32InWidth + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)];
							}
						}
						for(int h=0; h<u32HLeftM; h++)
						{
							AR_S8* pInHLeftM = (AR_S8*)(pInHLoopM + (u32HLoopM*u32NumInRegM+h)*u32InWidth);
							AR_S8* pOutHLeftM = (AR_S8*)(pOutHLoopM + (u32HLoopM*u32NumInRegM+h));
							for(int w=0; w<u32InWidth; w++)
							{
								pOutHLeftM[w*u32InHeight] = pInHLeftM[w];
							}
						}
					}
				}
	        }
			#endif
  		}
	} 
	// --> 0 2 1 3 
 	else if (pTransposeParam->u32Perm[0]==0 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==3)
 	{
      	if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps16Out + b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth), (void*)(ps16In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth*2);
						#endif
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps8Out + b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth), (void*)(ps8In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
  	}
  	// --> 0312
 	else if (pTransposeParam->u32Perm[0]==0 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==2)
 	{
      	AR_U32 u32HLoop = u32InHeight / u32NumInReg;
		AR_U32 u32HLeft = u32InHeight % u32NumInReg;
		AR_U32 u32HLoopM = u32HLeft / u32NumInRegM;
		AR_U32 u32HLeftM = u32HLeft % u32NumInRegM;
		
		AR_U32 u32WLoop = u32InWidth / u32NumInReg;
		AR_U32 u32WLeft = u32InWidth % u32NumInReg;
		AR_U32 u32WLoopM = u32WLeft / u32NumInRegM;
		AR_U32 u32WLeftM = u32WLeft % u32NumInRegM;
		
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;

          	#if 0
			for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[b * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for (int b = 0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for (int c =0; c<u32InChannel; c++)
				{
					AR_U32 u32SrcCoffset = c * u32InChSize;
					AR_U32 u32DstCoffset = c * u32InHeight;
					for(int h=0; h<u32HLoop; h++)
					{
						AR_S16* pInWLoop = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + h*u32NumInReg*u32InWidth);
						AR_S16* pOutWLoop = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + h*u32NumInReg);
						for(int w=0; w<u32WLoop; w++)
						{
							AR_NPU_Matrix8x8_S16_Convert(pInWLoop + w*u32NumInReg, pOutWLoop + w*u32NumInReg*u32InChannel*u32InHeight, u32InWidth, u32InChannel*u32InHeight);
						}
						if(u32WLeft)
						{
							for(int w=0; w<u32WLoopM; w++)
							{
								AR_S16* pInWLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + w*u32NumInRegM));
								AR_S16* pOutWLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg+w*u32NumInRegM)*u32InChannel*u32InHeight);
								
								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + (h*2)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2)*u32NumInRegM, u32InWidth, u32InChannel*u32InHeight);
								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + (h*2+1)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2+1)*u32NumInRegM, u32InWidth, u32InChannel*u32InHeight);
							}
							for(int w=0; w<u32WLeftM; w++)
							{
								AR_S16* pInWLeftM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w));
								AR_S16* pOutWLeftM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM + w)*u32InChannel*u32InHeight);
								for(int hw=0; hw<u32NumInReg; hw++)
								{
									pOutWLeftM[h*u32NumInReg + hw] = pInWLeftM[(h*u32NumInReg + hw)*u32InWidth];
								}
							}
						}
					}
					if(u32HLeft)
					{
						AR_S16* pInHLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcCoffset + u32HLoop*u32NumInReg*u32InWidth);
						AR_S16* pOutHLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstCoffset + u32HLoop*u32NumInReg);
						for(int h=0; h<u32HLoopM; h++)
						{
							for(int w=0; w<u32WLoop*2+u32WLoopM; w++)
							{
								AR_NPU_Matrix4x4_S16_Convert(pInHLoopM + h*u32NumInRegM*u32InWidth + w*u32NumInRegM, pOutHLoopM + h*u32NumInRegM + w*u32NumInRegM*u32InChannel*u32InHeight, u32InWidth, u32InChannel*u32InHeight);
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							for(int h=0; h<u32HLoopM*u32NumInRegM; h++)
							{
								pOutHLoopM[h + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)*u32InChannel*u32InHeight] = pInHLoopM[h*u32InWidth + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)];
							}
						}
						for(int h=0; h<u32HLeftM; h++)
						{
							AR_S16* pInHLeftM = (AR_S16*)(pInHLoopM + (u32HLoopM*u32NumInRegM+h)*u32InWidth);
							AR_S16* pOutHLeftM = (AR_S16*)(pOutHLoopM + (u32HLoopM*u32NumInRegM+h));
							for(int w=0; w<u32InWidth; w++)
							{
								pOutHLeftM[w*u32InChannel*u32InHeight] = pInHLeftM[w];
							}
						}
					}
				}
			}
			#endif
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;

			#if 0
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[b * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for (int b = 0; b<u32Batch; b++)
          	{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for (int c =0; c<u32InChannel; c++)
              	{
					AR_U32 u32SrcCoffset = c * u32InChSize;
					AR_U32 u32DstCoffset = c * u32InHeight;
					for(int h=0; h<u32HLoop; h++)
					{
						AR_S8* pInWLoop = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + h*u32NumInReg*u32InWidth);
						AR_S8* pOutWLoop = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + h*u32NumInReg);
						for(int w=0; w<u32WLoop; w++)
						{
							AR_NPU_Matrix16x16_S8_Convert(pInWLoop + w*u32NumInReg, pOutWLoop + w*u32NumInReg*u32InChannel*u32InHeight, u32InWidth, u32InChannel*u32InHeight);
						}
						if(u32WLeft)
						{
							for(int w=0; w<u32WLoopM; w++)
							{
								AR_S8* pInWLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + w*u32NumInRegM));
								AR_S8* pOutWLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg + w*u32NumInRegM)*u32InHeight*u32InChannel);

								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + (h*2)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2)*u32NumInRegM, u32InWidth, u32InChannel*u32InHeight);
								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + (h*2+1)*u32NumInRegM*u32InWidth, pOutWLoopM + (h*2+1)*u32NumInRegM, u32InWidth, u32InChannel*u32InHeight);
							}
							for(int w=0; w<u32WLeftM; w++)
							{
								AR_S8* pInWLeftM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + (u32WLoop*u32NumInReg + u32WLoopM*u32NumInRegM + w));
								AR_S8* pOutWLeftM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM + w)*u32InChannel*u32InHeight);
								for(int hw=0; hw<u32NumInReg; hw++)
								{
									pOutWLeftM[h*u32NumInReg + hw] = pInWLeftM[(h*u32NumInReg+hw)*u32InWidth];
								}
							}
						}
					}
					if(u32HLeft)
					{
						AR_S8* pInHLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcCoffset + u32HLoop*u32NumInReg*u32InWidth);
						AR_S8* pOutHLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstCoffset + u32HLoop*u32NumInReg);
						for(int h=0; h<u32HLoopM; h++)
						{
							for(int w=0; w<u32WLoop*2+u32WLoopM; w++)
							{
								AR_NPU_Matrix8x8_S8_Convert(pInHLoopM + h*u32NumInRegM*u32InWidth + w*u32NumInRegM, pOutHLoopM + h*u32NumInRegM + w*u32NumInRegM*u32InChannel*u32InHeight, u32InWidth, u32InChannel*u32InHeight);
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							for(int h=0; h<u32HLoopM*u32NumInRegM; h++)
							{
								pOutHLoopM[h + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)*u32InChannel*u32InHeight] = pInHLoopM[h*u32InWidth + (u32WLoop*u32NumInReg+u32WLoopM*u32NumInRegM+w)];
							}
						}
						for(int h=0; h<u32HLeftM; h++)
						{
							AR_S8* pInHLeftM = (AR_S8*)(pInHLoopM + (u32HLoopM*u32NumInRegM+h)*u32InWidth);
							AR_S8* pOutHLeftM = (AR_S8*)(pOutHLoopM + (u32HLoopM*u32NumInRegM + h));
							for(int w=0; w<u32InWidth; w++)
							{
								pOutHLeftM[w*u32InChannel*u32InHeight] = pInHLeftM[w];
							}
						}
					}
				}
	        }

			#endif
  		}
  	}
	// --> 0 3 2 1
  	else if(pTransposeParam->u32Perm[0]==0 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==2 &&pTransposeParam->u32Perm[3]==1)
 	{
		AR_U32 u32CLoop = u32InChannel / u32NumInReg;
		AR_U32 u32CLeft = u32InChannel % u32NumInReg;
		AR_U32 u32CLoopM = u32CLeft / u32NumInRegM;
		AR_U32 u32CLeftM = u32CLeft % u32NumInRegM;
		
		AR_U32 u32WLoop = u32InWidth / u32NumInReg;
		AR_U32 u32WLeft = u32InWidth % u32NumInReg;
		AR_U32 u32WLoopM = u32WLeft / u32NumInRegM;
		AR_U32 u32WLeftM = u32WLeft % u32NumInRegM;
		
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;

			#if 0
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[b * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for(int b=0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for(int h=0; h<u32InHeight; h++)	
				{
					AR_U32 u32SrcHoffset = h * u32InWidth;
					AR_U32 u32DstHoffset = h * u32InChannel;
					for(int w=0; w<u32WLoop; w++)
					{
						AR_S16* pInCLoop = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + w*u32NumInReg);
						AR_S16* pOutCLoop = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + w*u32NumInReg*u32InHeight*u32InChannel);
						for(int c=0; c<u32CLoop; c++)
						{
							AR_NPU_Matrix8x8_S16_Convert(pInCLoop + c*u32NumInReg*u32InChSize, pOutCLoop + c*u32NumInReg, u32InChSize, u32InHeight*u32InChannel);
						}
						if(u32CLeft)
						{
							AR_S16* pInCLeft = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + u32CLoop*u32NumInReg*u32InChSize);
							AR_S16* pOutCLeft = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + u32CLoop*u32NumInReg);
							for(int c=0; c<u32CLoopM; c++)
							{
								AR_S16* pInCLoopM = (AR_S16*)(pInCLeft + c*u32NumInRegM*u32InChSize);
								AR_S16* pOutCLoopM = (AR_S16*)(pOutCLeft + c*u32NumInRegM);

								AR_NPU_Matrix4x4_S16_Convert(pInCLoopM + (w*2)*u32NumInRegM, pOutCLoopM + (w*2)*u32NumInRegM*u32InHeight*u32InChannel, u32InChSize, u32InHeight*u32InChannel);
								AR_NPU_Matrix4x4_S16_Convert(pInCLoopM + (w*2+1)*u32NumInRegM, pOutCLoopM + (w*2+1)*u32NumInRegM*u32InHeight*u32InChannel, u32InChSize, u32InHeight*u32InChannel);
							}
							for(int c=0; c<u32CLeftM; c++)
							{
								AR_S16* pInCLeftM = (AR_S16*)(pInCLeft + (u32CLoopM*u32NumInRegM+c)*u32InChSize);
								AR_S16* pOutCLeftM = (AR_S16*)(pOutCLeft + (u32CLoopM*u32NumInRegM+c));
								for(int wc=0; wc<u32NumInReg; wc++)
								{
									pOutCLeftM[(w*u32NumInReg + wc)*u32InHeight*u32InChannel] = pInCLeftM[w*u32NumInReg + wc];
								}
							}
						}
					}
					if(u32WLeft)
					{
						AR_S16* pInWLoopM = (AR_S16*)(ps16In + u32Batchoffset + u32SrcHoffset + u32WLoop*u32NumInReg);
						AR_S16* pOutWLoopM = (AR_S16*)(ps16Out + u32Batchoffset + u32DstHoffset + (u32WLoop*u32NumInReg)*u32InHeight*u32InChannel);
						for(int w=0; w<u32WLoopM; w++)
						{
							for(int c=0; c<u32CLoop*2+u32CLoopM; c++)
							{
								AR_NPU_Matrix4x4_S16_Convert(pInWLoopM + c*u32NumInRegM*u32InChSize + w*u32NumInRegM*u32InHeight*u32InChannel, pOutWLoopM + c*u32NumInRegM + w*u32NumInRegM, u32InChSize, u32InHeight*u32InChannel);
							}
						}
						for(int c=0; c<u32CLeftM; c++)
						{
							for(int w=0; w<u32WLoopM*u32NumInRegM; w++)
							{
								pOutWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM + c) + w*u32InHeight*u32InChannel] = pInWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM + c)*u32InHeight*u32InWidth + w];
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							AR_S16* pInWLeftM = (AR_S16*)(pInWLoopM + (u32WLoopM*u32NumInRegM + w));
							AR_S16* pOutWLeftM = (AR_S16*)(pOutWLoopM + (u32WLoopM*u32NumInRegM + w)*u32InHeight*u32InChannel);
							for(int c=0; c<u32InChannel; c++)
							{
								pOutWLeftM[c] = pInWLeftM[c*u32InChSize];
							}
						}
					}
				}
			}
			#endif
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;

          	#if 0
			for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[b * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
			#else
			for(int b=0; b<u32Batch; b++)
			{
				AR_U32 u32Batchoffset = b * u32InSingleSize;
				for(int h=0; h<u32InHeight; h++)	
				{
					AR_U32 u32SrcHoffset = h * u32InWidth;
					AR_U32 u32DstHoffset = h * u32InChannel;
					
					for(int w=0; w<u32WLoop; w++)
					{
						AR_S8* pInCLoop = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + w*u32NumInReg);
						AR_S8* pOutCLoop = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + w*u32NumInReg*u32InHeight*u32InChannel);
						for(int c=0; c<u32CLoop; c++)
						{
							AR_NPU_Matrix16x16_S8_Convert(pInCLoop + c*u32NumInReg*u32InChSize, pOutCLoop + c*u32NumInReg, u32InChSize, u32InHeight*u32InChannel);
						}
						if(u32CLeft)
						{
							AR_S8* pInCLeft = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + u32CLoop*u32NumInReg*u32InChSize);
							AR_S8* pOutCLeft = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + u32CLoop*u32NumInReg);
							
							for(int c=0; c<u32CLoopM; c++)
							{
								AR_S8* pInCLoopM = (AR_S8*)(pInCLeft + c*u32NumInRegM*u32InChSize);
								AR_S8* pOutCLoopM = (AR_S8*)(pOutCLeft + c*u32NumInRegM);

								AR_NPU_Matrix8x8_S8_Convert(pInCLoopM + (w*2+0)*u32NumInRegM, pOutCLoopM + (w*2+0)*u32NumInRegM*u32InHeight*u32InChannel, u32InChSize, u32InHeight*u32InChannel);
								AR_NPU_Matrix8x8_S8_Convert(pInCLoopM + (w*2+1)*u32NumInRegM, pOutCLoopM + (w*2+1)*u32NumInRegM*u32InHeight*u32InChannel, u32InChSize, u32InHeight*u32InChannel);
							}
							
							for(int c=0; c<u32CLeftM; c++)
							{
								AR_S8* pInCLeftM = (AR_S8*)(pInCLeft + (u32CLoopM*u32NumInRegM+c)*u32InChSize);
								AR_S8* pOutCLeftM = (AR_S8*)(pOutCLeft + (u32CLoopM*u32NumInRegM+c));
								for(int wc=0; wc<u32NumInReg; wc++)
								{
									pOutCLeftM[(w*u32NumInReg + wc)*u32InHeight*u32InChannel] = pInCLeftM[w*u32NumInReg + wc];
								}
							}
						}
					}
					
					if(u32WLeft)
					{
						AR_S8* pInWLoopM = (AR_S8*)(ps8In + u32Batchoffset + u32SrcHoffset + u32WLoop*u32NumInReg);
						AR_S8* pOutWLoopM = (AR_S8*)(ps8Out + u32Batchoffset + u32DstHoffset + (u32WLoop*u32NumInReg)*u32InHeight*u32InChannel);
						for(int w=0; w<u32WLoopM; w++)
						{
							for(int c=0; c<u32CLoop*2+u32CLoopM; c++)
							{
								AR_NPU_Matrix8x8_S8_Convert(pInWLoopM + c*u32NumInRegM*u32InChSize + w*u32NumInRegM, pOutWLoopM + c*u32NumInRegM + w*u32NumInRegM*u32InHeight*u32InChannel, u32InChSize, u32InHeight*u32InChannel);
							}
						}
						for(int c=0; c<u32CLeftM; c++)
						{
							for(int w=0; w<u32WLoopM*u32NumInRegM; w++)
							{
								pOutWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM + c) + w*u32InHeight*u32InChannel] = pInWLoopM[(u32CLoop*u32NumInReg+u32CLoopM*u32NumInRegM + c)*u32InHeight*u32InWidth + w];
							}
						}
						for(int w=0; w<u32WLeftM; w++)
						{
							AR_S8* pInWLeftM = (AR_S8*)(pInWLoopM + (u32WLoopM*u32NumInRegM + w));
							AR_S8* pOutWLeftM = (AR_S8*)(pOutWLoopM + (u32WLoopM*u32NumInRegM+w)*u32InHeight*u32InChannel);
							for(int c=0; c<u32InChannel; c++)
							{
								pOutWLeftM[c] = pInWLeftM[c*u32InChSize];
							}
						}
					}
				}
			}
			#endif
  		}
  	} 
	// --> 1230
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==0)
 	{
		u32OutChannel = u32InHeight;
		u32OutHeight = u32InWidth;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
			for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 1203
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==3)
 	{
      	u32OutChannel = u32InHeight;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InWidth;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps16Out + c * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth), (void*)(ps16In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
						#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps8Out + c * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth), (void*)(ps8In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
  	} 
	// --> 1302
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==2)
 	{
      	u32OutChannel = u32InWidth;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InHeight;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + w * u32OutChSize + b * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
			for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + w * u32OutChSize + b * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 1320
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==2 &&pTransposeParam->u32Perm[3]==0)
 	{
		u32OutChannel = u32InWidth;
		u32OutHeight = u32InHeight;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 1032
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==2)
 	{
		u32OutChannel = u32Batch;
		u32OutHeight = u32InWidth;
		u32OutWidth = u32InHeight;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + b * u32OutChSize + w * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + b * u32OutChSize + w * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 1023
  	else if(pTransposeParam->u32Perm[0]==1 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==2 &&pTransposeParam->u32Perm[3]==3)
 	{
      	u32OutChannel = u32Batch;
		u32OutHeight = u32InHeight;
		u32OutWidth = u32InWidth;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
						#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps16Out + c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth), (void*)(ps16In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps8Out + c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth), (void*)(ps8In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
  	} 
	// --> 2130
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==1 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==0)
 	{ 
		u32OutChannel = u32InChannel;
		u32OutHeight = u32InWidth;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 2103
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==1 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==3)
 	{
		u32OutChannel = u32InChannel;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InWidth;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChannel * u32OutChSize;
		
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
						#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps16Out + h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth), (void*)(ps16In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
						#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps8Out + h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth), (void*)(ps8In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
  	} 
	// --> 2031
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==3 &&pTransposeParam->u32Perm[3]==1)
 	{
      	u32OutChannel = u32Batch;
		u32OutHeight = u32InWidth;
		u32OutWidth = u32InChannel;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + b * u32OutChSize + w * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + b * u32OutChSize + w * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 2013
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==3)
 	{
      	u32OutChannel = u32Batch;
		u32OutHeight = u32InChannel;
		u32OutWidth = u32InWidth;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps16Out + h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth), (void*)(ps16In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	#if 0
						for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
						#else
						memcpy((void*)(ps8Out + h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth), (void*)(ps8In + b * u32InSingleSize + c * u32InChSize + h * u32InWidth), u32InWidth);
						#endif
      				}
      			}
      		}
  		}
  	} 
	// --> 2301
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==1)
 	{
		u32OutChannel = u32InWidth;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InChannel;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + w * u32OutChSize + b * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + w * u32OutChSize + b * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 2310
  	else if(pTransposeParam->u32Perm[0]==2 && pTransposeParam->u32Perm[1]==3 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==0)
 	{
      	u32OutChannel = u32InWidth;
		u32OutHeight = u32InChannel;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[h * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[h * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3012
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==2)
 	{
		u32OutChannel = u32Batch;
		u32OutHeight = u32InChannel;
		u32OutWidth = u32InHeight;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3021
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==0 && pTransposeParam->u32Perm[2]==2 &&pTransposeParam->u32Perm[3]==1)
 	{
      	u32OutChannel = u32Batch;
		u32OutHeight = u32InHeight;
		u32OutWidth = u32InChannel;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3102
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==1 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==2)
 	{
      	u32OutChannel = u32InChannel;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InHeight;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3120
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==1 && pTransposeParam->u32Perm[2]==2 &&pTransposeParam->u32Perm[3]==0)
 	{
      	u32OutChannel = u32InChannel;
		u32OutHeight = u32InHeight;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + c * u32OutChSize + h * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + c * u32OutChSize + h * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3201
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==0 &&pTransposeParam->u32Perm[3]==1)
 	{
      	u32OutChannel = u32InHeight;
		u32OutHeight = u32Batch;
		u32OutWidth = u32InChannel;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + h * u32OutChSize + b * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	// --> 3210
  	else if(pTransposeParam->u32Perm[0]==3 && pTransposeParam->u32Perm[1]==2 && pTransposeParam->u32Perm[2]==1 &&pTransposeParam->u32Perm[3]==0)
 	{
      	u32OutChannel = u32InHeight;
		u32OutHeight = u32InChannel;
		u32OutWidth = u32Batch;
		u32OutChSize = u32OutHeight * u32OutWidth;
		u32OutSingleSize = u32OutChSize * u32OutChannel;
		if(u32Precision == 16)
      	{
			AR_S16* ps16In = (AR_S16 *)pInF;
      		AR_S16* ps16Out = (AR_S16 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps16Out[w * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + b] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
      	else
      	{
			AR_S8* ps8In = (AR_S8 *)pInF;
      		AR_S8* ps8Out = (AR_S8 *)pOutF;
          	for (int b = 0; b<u32Batch; b++)
          	{
              	for (int c =0; c<u32InChannel; c++)
              	{
                  	for (int h =0; h<u32InHeight; h++)
                  	{
                      	for (int w =0; w<u32InWidth; w++)
                      	{  
                          	ps8Out[w * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + b] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
      					}
      				}
      			}
      		}
  		}
  	} 
	else
	{
		printf("Unsupported operator type!!\n");
		return 0;
	}
	
	return 1;	
}


AR_S32 AR_CNN_OperatorTranspose(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
  	AR_NPU_TRANSPOSE_PARAM_S *pTransposeParam = (AR_NPU_TRANSPOSE_PARAM_S*)(pOpParams);
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
 	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 +  pCBParams->astInputTensor[0].u32Offset;

  	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
  	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32KNormNum * pCBParams->astOutputTensor[0].u32KSizeNorm + pCBParams->astOutputTensor[0].u32KSizeLast;
	AR_U32 u32OutBatch = pCBParams->astOutputTensor[0].u32Num;
	
	AR_UINTPTR pOutput = pOut + pCBParams->astOutputTensor[0].u32Bank * 0x2000000 + pCBParams->astOutputTensor[0].u32Offset;
	
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;
	
	AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;

	AR_U32 u32InDataSize = u32InChannel * u32InChSize * u32Batch;
	AR_U32 u32OutDataSize = u32OutChannel * u32OutChSize * u32OutBatch;
	
	AR_CHAR * pInNchw = (AR_CHAR *)malloc(u32InDataSize * u32PixelByte);
	if (!pInNchw)
	{
		printf("Malloc pInNchw(AR_CNN_OperatorTranspose) failed.\n");
		free(pInNchw);
		return -1;
	}

	AR_CHAR * pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32PixelByte);
	if (!pOutNchw)
	{
		printf("Malloc pOutNchw(AR_CNN_OperatorTranspose) failed.\n");
		free(pInNchw);
		free(pOutNchw);
		return -1;
	}
	
	memset(pInNchw, 0, u32InDataSize * u32PixelByte);
	memset(pOutNchw, 0, u32OutDataSize * u32PixelByte);
	
  	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);

  	AR_S32 ret = _AR_CNN_TransposeForward(pInNchw, pOutNchw, pCBParams, pTransposeParam);
  	if(!ret){
		printf("Transpose callback only supports conversion between chw in 4D input data!!\n");
    free(pInNchw);
  	free(pOutNchw);
		return -1;
  	}
	
  	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);

  	free(pInNchw);
  	free(pOutNchw);
  	return 0;
}
 
