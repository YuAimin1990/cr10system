#include "ar_cnn_parser.h"
#include "ar_cnn_operator.h"
#include "ar_cnn_op_common.h"


AR_S32 _AR_CNN_PermuteForward(AR_CHAR * pInF, AR_CHAR * pOutF, AR_NPU_CB_PARAM_S * pCBParams, AR_NPU_PERMUTE_PARAM_S *pPermuteParam)
{
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32OriChannels;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;

	AR_U32 u32OutHeight = pCBParams->astOutputTensor[0].u32Height;
	AR_U32 u32OutWidth = pCBParams->astOutputTensor[0].u32Width;
	AR_U32 u32OutChannel = pCBParams->astOutputTensor[0].u32OriChannels;
	AR_U32 u32OutBatch = pCBParams->astOutputTensor[0].u32Num;
	
	AR_U32 u32Precision = pCBParams->astInputTensor[0].u32Precision;
	AR_U32 u32PixelByte = u32Precision / 8;

	AR_U32 u32InChSize = u32InHeight * u32InWidth;
	AR_U32 u32OutChSize = u32OutHeight * u32OutWidth;

	AR_U32 u32InSingleSize = u32InChannel * u32InChSize;
	AR_U32 u32OutSingleSize = u32OutChannel * u32OutChSize;

	AR_U32 u32InDataSize = u32InSingleSize * u32Batch;
	AR_U32 u32OutDataSize = u32OutSingleSize * u32OutBatch;
   
	// --> 0 2 3 1
	if (strcmp(pPermuteParam->sOrder_type, "nhwc") == 0)
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
	                    for (int w =0; w<u32InWidth; w++)
	                    {  
	                        ps16Out[b * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	      				}
	      			}
	      		}
	      	}
	  	}     
		else  //8bit
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
		                	ps8Out[b * u32OutSingleSize + h * u32OutChSize + w * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
						}
					}
				}
			}
		}
	} 
	// --> 0 1 3 2
	else if (strcmp(pPermuteParam->sOrder_type, "ncwh") == 0)
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
		            	for (int w =0; w<u32InWidth; w++)
		              	{  
		                	ps16Out[b * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
					 	}
				  	}
			  	}
		  	}
	  	} 
	  	else  //8bit
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
	                      	ps8Out[b * u32OutSingleSize + c * u32OutChSize + w * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	  				  	}
	  			  	}
	  		  	}
	  	  	}
	  	} 
	}
	// --> 0 2 1 3 
	else if (strcmp(pPermuteParam->sOrder_type, "nhcw") == 0) 
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
	                	for (int w =0; w<u32InWidth; w++)
	                  	{  
	                      	ps16Out[b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                    	ps8Out[b * u32OutSingleSize + h * u32OutChSize + c * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	  				  	}
	  			  	}
	  		  	}
	  	  	}
	   	}  
	} 
	// --> 0 3 1 2
	else if (strcmp(pPermuteParam->sOrder_type, "nwch") == 0) 
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
	                  	for (int w =0; w<u32InWidth; w++)
	                  	{  
	                      	ps16Out[b * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + h] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                      	ps8Out[b * u32OutSingleSize + w * u32OutChSize + c * u32OutWidth + h] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	  				  	}
	  			  	}
	  		  	}
	      	}
		}    
	}
	// --> 0 3 2 1
	else if(strcmp(pPermuteParam->sOrder_type, "nwhc") == 0)
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
	                	for (int w =0; w<u32InWidth; w++)
	                    {  
	                    	ps16Out[b * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + c] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                    	ps8Out[b * u32OutSingleSize + w * u32OutChSize + h * u32OutWidth + c] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	      				}
	      			}
	      		}
	      	}
	  	}   
	}
	
	//---------
	else if(strcmp(pPermuteParam->sOrder_type, "chwn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "chwn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "cwnh") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "cwhn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "cnwh") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "cnhw") == 0)
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
	                	for (int w =0; w<u32InWidth; w++)
	                    {  
	                    	ps16Out[c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                    	ps8Out[c * u32OutSingleSize + b * u32OutChSize + h * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	      				}
	      			}
	      		}
	      	}
	  	}   
	}
	else if(strcmp(pPermuteParam->sOrder_type, "hcwn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "hcnw") == 0)
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
	                	for (int w =0; w<u32InWidth; w++)
	                    {  
	                    	ps16Out[h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                    	ps8Out[h * u32OutSingleSize + c * u32OutChSize + b * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	      				}
	      			}
	      		}
	      	}
	  	}   
	}
	else if(strcmp(pPermuteParam->sOrder_type, "hnwc") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "hncw") == 0)
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
	                	for (int w =0; w<u32InWidth; w++)
	                    {  
	                    	ps16Out[h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + w] = ps16In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
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
	                    	ps8Out[h * u32OutSingleSize + b * u32OutChSize + c * u32OutWidth + w] = ps8In[b * u32InSingleSize + c * u32InChSize + h * u32InWidth + w];
	      				}
	      			}
	      		}
	      	}
	  	}   
	}
	else if(strcmp(pPermuteParam->sOrder_type, "hwnc") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "hwcn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "wnch") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "wnhc") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "wcnh") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "wchn") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "whnc") == 0)
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
	else if(strcmp(pPermuteParam->sOrder_type, "whcn") == 0)
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


AR_S32 AR_CNN_OperatorPermute(AR_UINTPTR pIn, AR_UINTPTR pOut, AR_NPU_CB_PARAM_S * pCBParams, void * pOpParams)
{
  	AR_NPU_PERMUTE_PARAM_S *pPermuteParam = (AR_NPU_PERMUTE_PARAM_S*)(pOpParams);
	printf('permute');
  
	AR_U32 u32InHeight = pCBParams->astInputTensor[0].u32Height;
	AR_U32 u32InWidth = pCBParams->astInputTensor[0].u32Width;
 	AR_U32 u32InChannel = pCBParams->astInputTensor[0].u32KNormNum * pCBParams->astInputTensor[0].u32KSizeNorm + pCBParams->astInputTensor[0].u32KSizeLast;
	AR_U32 u32Batch = pCBParams->astInputTensor[0].u32Num;
	AR_UINTPTR pInput = pIn + pCBParams->astInputTensor[0].u32Bank * 0x2000000 + pCBParams->astInputTensor[0].u32Offset;

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
		printf("Malloc pInNchw(AR_CNN_OperatorPermute) failed.\n");
		free(pInNchw);
		return -1;
	}

	AR_CHAR * pOutNchw = (AR_CHAR *)malloc(u32OutDataSize * u32PixelByte);
	if (!pOutNchw)
	{
		printf("Malloc pOutNchw(AR_CNN_OperatorPermute) failed.\n");
		free(pOutNchw);
		return -1;
	}
   	memset(pInNchw, 0, u32InDataSize * u32PixelByte);
	memset(pOutNchw, 0, u32OutDataSize * u32PixelByte);

 	dcnn_hwc_to_chw((AR_CHAR *)pInput, pInNchw, &pCBParams->astInputTensor[0]);
  
  	AR_S32 ret = _AR_CNN_PermuteForward(pInNchw, pOutNchw, pCBParams, pPermuteParam);
  	if(!ret){
		printf("Permute callback only supports conversion between chw in 4D input data!!\n");
		return -1;
	}
  
  	dcnn_chw_to_hwc(pOutNchw, (AR_CHAR *)pOutput, &pCBParams->astOutputTensor[0]);
  
  	free(pInNchw);
  	free(pOutNchw);
  	printf("permute End! \n");
  	return 0;
}
 
