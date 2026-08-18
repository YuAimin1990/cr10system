#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

//#include "ar_ion.h"
#include "hal_sys.h"
#include "mpi_scaler_api.h"
#include "mpi_sysctl.h"

MPI_SYSCTL_HANDLE g_phandle = NULL;


#define SCALER_STRIDE_ALIGNE(size, num) ( (num)*( ( (size)+(num)-(1))/(num) ) )
//#define AR_SCALER_SINGLE_MODE

typedef struct 
{
    AR_S32 s32SrcWidth;
    AR_S32 s32SrcHeight;
    AR_S32 s32SrcStride;
    AR_S32 s32DstWidth;
    AR_S32 s32DstHeight;
    AR_S32 s32DstStride;
    AR_IMG_FORMAT_E eImgFormat;
}AR_MPI_IMG_INFO_S;

#define SAMPLE_SCALER_MMZ_BUFFER_MALLOC_SIZE_MAX (4096*2160)
#define INVALID_IMG_FILE -1
#define RGB_RAW_DATA_FILE 0
#define YUV_RAW_DATA_FILE 1

AR_S32 GetImageFileType(AR_CHAR * pchFileName)
{
    AR_CHAR * pchExt = NULL;
    AR_CHAR * pchTmp = NULL;
    AR_S32 s32Ret = 0;

    if(!pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //ignore "." ".." and any non-image files
    if(strcmp(pchFileName, ".") == 0 || strcmp(pchFileName, "..") == 0)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    pchExt = strrchr(pchFileName, '.');
    if(!pchExt || pchExt == pchFileName)
    {
        s32Ret = INVALID_IMG_FILE;
        return s32Ret;
    }

    //convert to lowercase
    pchTmp = pchExt;
    for(; *pchTmp; ++pchTmp) *pchTmp = tolower(*pchTmp);

    if((strcmp(pchExt, ".rgb") == 0) || (strcmp(pchExt, ".bgr") == 0))
    {
        s32Ret = RGB_RAW_DATA_FILE;
    }
    else if(strcmp(pchExt, ".yuv") == 0)
    {
        s32Ret = YUV_RAW_DATA_FILE;
    }
    else
    {
        s32Ret = INVALID_IMG_FILE;
    }

    //Need to add RGBD or RGB interleave format later..

    return s32Ret;
}


static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 i=0;
	for(i=0;i<s32Argc;i++)
	{
         if(!strcmp(ps8Name,ps8Argv[i]))
         {
            printf("Find Pra %s @ %d \r\n",ps8Name,i);
            return i;
         }
	}
	return 0;
}

AR_S32 s32Loop = 0;
//YUV422 to RGB
static AR_S32 SAMPLE_SCALER_MMZ_Buffer_Request(AR_IMG_S *pstImgIn, AR_IMG_S *pstImgOut)
{
    AR_S32 s32Ret = -1;
    AR_CHAR s8Name[64] = {0};
    AR_U32 u32Ch0SrcSize = 0, u32Ch1SrcSize = 0, u32Ch2SrcSize = 0, u32Ch3SrcSize = 0;
    AR_U32 u32Ch0DstSize = 0, u32Ch1DstSize = 0, u32Ch2DstSize = 0, u32Ch3DstSize = 0;
    
    if((!pstImgIn) || (!pstImgOut)){
        printf("pstImg is NULL\r\n");
        return -1;
    }

    switch(pstImgIn->enFormat){
        case AR_IMG_YUV444P:
        case AR_IMG_RGB:
        case AR_IMG_BGR:
        case AR_IMG_YUV444SP:
        case AR_IMG_YUV422P:
        case AR_IMG_GRAY:
        case AR_IMG_RGB_INTLV:
        case AR_IMG_BGR_INTLV:
        case AR_IMG_RGBD_INTLV:
            u32Ch0SrcSize = pstImgIn->astChannels[0].u32Stride * pstImgIn->u32Height;
            u32Ch1SrcSize = pstImgIn->astChannels[1].u32Stride * pstImgIn->u32Height;
            u32Ch2SrcSize = pstImgIn->astChannels[2].u32Stride * pstImgIn->u32Height;
            
            u32Ch0DstSize = pstImgOut->astChannels[0].u32Stride * pstImgOut->u32Height;
            u32Ch1DstSize = pstImgOut->astChannels[1].u32Stride * pstImgOut->u32Height;
            u32Ch2DstSize = pstImgOut->astChannels[2].u32Stride * pstImgOut->u32Height;
            break;
        case AR_IMG_YUV420P:
        case AR_IMG_YUVI420:
        case AR_IMG_YV12:
        case AR_IMG_YUV420SP:
        case AR_IMG_NV12:
        case AR_IMG_NV21:
            u32Ch0SrcSize = pstImgIn->astChannels[0].u32Stride * pstImgIn->u32Height;
            u32Ch1SrcSize = pstImgIn->astChannels[1].u32Stride * pstImgIn->u32Height/2;
            u32Ch2SrcSize = pstImgIn->astChannels[2].u32Stride * pstImgIn->u32Height/2;
            
            u32Ch0DstSize = pstImgOut->astChannels[0].u32Stride * pstImgOut->u32Height;
            u32Ch1DstSize = pstImgOut->astChannels[1].u32Stride * pstImgOut->u32Height/2;
            u32Ch2DstSize = pstImgOut->astChannels[2].u32Stride * pstImgOut->u32Height/2;
            break; 
        case AR_IMG_RGBD:
            u32Ch0SrcSize = pstImgIn->astChannels[0].u32Stride * pstImgIn->u32Height;
            u32Ch1SrcSize = pstImgIn->astChannels[1].u32Stride * pstImgIn->u32Height;
            u32Ch2SrcSize = pstImgIn->astChannels[2].u32Stride * pstImgIn->u32Height;
            u32Ch3SrcSize = pstImgIn->astChannels[3].u32Stride * pstImgIn->u32Height;
            
            u32Ch0DstSize = pstImgOut->astChannels[0].u32Stride * pstImgOut->u32Height;
            u32Ch1DstSize = pstImgOut->astChannels[1].u32Stride * pstImgOut->u32Height;
            u32Ch2DstSize = pstImgOut->astChannels[2].u32Stride * pstImgOut->u32Height;
            u32Ch3SrcSize = pstImgOut->astChannels[3].u32Stride * pstImgOut->u32Height;
            break;
        default:
           printf("input format is error %d\r\n", pstImgIn->enFormat);
           return -1;
    }

	/*******************SRC CH0******************************/
    AR_U64 u64YSrcPa = 0;
    void * pvYSrcVa = NULL;
    sprintf(s8Name, "Ch0Src%d", s32Loop);
    if(u32Ch0SrcSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, s8Name, NULL, u32Ch0SrcSize);
        if(s32Ret){
            printf(" get src CH0 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvYSrcVa, 0, u32Ch0SrcSize);
        
        pstImgIn->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pvYSrcVa;
        pstImgIn->astChannels[0].u32AddrPhy = (AR_UINTPTR)u64YSrcPa;
    }
	/*******************SRC CH1******************************/
    AR_U64 u64USrcPa = 0;
    void * pvUSrcVa = NULL;
    sprintf(s8Name, "Ch1Src%d", s32Loop);
    if(u32Ch1SrcSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, s8Name, NULL, u32Ch1SrcSize);
        if(s32Ret){
            printf(" get src CH1 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvUSrcVa, 0, u32Ch1SrcSize);
        pstImgIn->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pvUSrcVa;
        pstImgIn->astChannels[1].u32AddrPhy = (AR_UINTPTR)u64USrcPa;
    }
	/*******************SRC CH2******************************/
    AR_U64 u64VSrcPa = 0;
    void * pvVSrcVa = NULL;
    sprintf(s8Name, "Ch2Src%d", s32Loop);
    if(u32Ch2SrcSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, s8Name, NULL, u32Ch2SrcSize);
        if(s32Ret){
            printf(" get src CH2 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvVSrcVa, 0, u32Ch2SrcSize);
        pstImgIn->astChannels[2].uptrAddrVirt = (AR_UINTPTR)pvVSrcVa;
        pstImgIn->astChannels[2].u32AddrPhy = (AR_UINTPTR)u64VSrcPa;
    }
	/*******************SRC CH3******************************/
    AR_U64 u64DSrcPa = 0;
    void * pvDSrcVa = NULL;
    sprintf(s8Name, "Ch3Src%d", s32Loop);
    if(u32Ch3SrcSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64DSrcPa, &pvDSrcVa, s8Name, NULL, u32Ch3SrcSize);
        if(s32Ret){
            printf(" get src CH3 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvDSrcVa, 0, u32Ch3SrcSize);
        pstImgIn->astChannels[3].uptrAddrVirt = (AR_UINTPTR)pvDSrcVa;
        pstImgIn->astChannels[3].u32AddrPhy = (AR_UINTPTR)u64DSrcPa;
    }
	/*******************DST CH0******************************/
    AR_U64 u64RDstPa = 0;
    void * pvRDstVa = NULL;
    sprintf(s8Name, "Ch0Dst%d", s32Loop);
    if(u32Ch0DstSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64RDstPa, &pvRDstVa, s8Name, NULL, u32Ch0DstSize);
        if(s32Ret){
            printf(" get dst CH0 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvRDstVa,0,u32Ch0DstSize);
        pstImgOut->astChannels[0].uptrAddrVirt = (AR_UINTPTR)pvRDstVa;
        pstImgOut->astChannels[0].u32AddrPhy = (AR_UINTPTR)u64RDstPa;
    }
	/*******************DST G******************************/
    AR_U64 u64GDstPa = 0;
    void * pvGDstVa = NULL;
    sprintf(s8Name, "GDst%d", s32Loop);
    if(u32Ch1DstSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64GDstPa, &pvGDstVa, s8Name, NULL, u32Ch1DstSize);
        if(s32Ret){
            printf(" get dst Ch1 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvGDstVa, 0, u32Ch1DstSize);
        pstImgOut->astChannels[1].uptrAddrVirt = (AR_UINTPTR)pvGDstVa;
        pstImgOut->astChannels[1].u32AddrPhy = (AR_UINTPTR)u64GDstPa;
    }
	/*******************DST B******************************/
    AR_U64 u64BDstPa = 0;
    void * pvBDstVa = NULL;
    sprintf(s8Name, "BDst%d", s32Loop);
    if(u32Ch2DstSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64BDstPa, &pvBDstVa, s8Name, NULL, u32Ch2DstSize);
        if(s32Ret){
            printf(" get dst CH2 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvBDstVa,0,u32Ch2DstSize);
        pstImgOut->astChannels[2].uptrAddrVirt = (AR_UINTPTR)pvBDstVa;
        pstImgOut->astChannels[2].u32AddrPhy = (AR_UINTPTR)u64BDstPa;
    }

	/*******************DST D******************************/
    AR_U64 u64DDstPa = 0;
    void * pvDDstVa = NULL;
    sprintf(s8Name, "DDst%d", s32Loop);
    if(u32Ch3DstSize){
        s32Ret = ar_hal_sys_mmz_alloc(&u64DDstPa, &pvDDstVa, s8Name, NULL, u32Ch3DstSize);
        if(s32Ret){
            printf(" get dst CH3 addr error!\r\n");
            return -1;
        }
        memset((AR_CHAR *)pvDDstVa,0,u32Ch3DstSize);
        pstImgOut->astChannels[3].uptrAddrVirt = (AR_UINTPTR)pvDDstVa;
        pstImgOut->astChannels[3].u32AddrPhy = (AR_UINTPTR)u64DDstPa;
    }
    
    s32Loop++;
    return 0;

}

static AR_S32 SAMPLE_SCALER_MMZ_Buffer_Release(AR_IMG_S * pstScalerIN, AR_IMG_S * pstScalerOUT)
{

    if(pstScalerIN->astChannels[0].u32AddrPhy) {
        printf("release src ch0\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerIN->astChannels[0].u32AddrPhy, (AR_VOID *)pstScalerIN->astChannels[0].uptrAddrVirt);
    }
	
    if(pstScalerIN->astChannels[1].u32AddrPhy){
        printf("release src ch1\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerIN->astChannels[1].u32AddrPhy, (AR_VOID *)pstScalerIN->astChannels[1].uptrAddrVirt);
    }
	
    if(pstScalerIN->astChannels[2].u32AddrPhy){
        printf("release src ch2\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerIN->astChannels[2].u32AddrPhy, (AR_VOID *)pstScalerIN->astChannels[2].uptrAddrVirt);
    }
	
    if(pstScalerIN->astChannels[3].u32AddrPhy){
        printf("release src ch3\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerIN->astChannels[3].u32AddrPhy, (AR_VOID *)pstScalerIN->astChannels[3].uptrAddrVirt);
    }
	
    if(pstScalerOUT->astChannels[0].u32AddrPhy){
        printf("release dst ch0\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerOUT->astChannels[0].u32AddrPhy, (AR_VOID *)pstScalerOUT->astChannels[0].uptrAddrVirt);
    }
	
    if(pstScalerOUT->astChannels[1].u32AddrPhy){
        printf("release dst ch1\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerOUT->astChannels[1].u32AddrPhy, (AR_VOID *)pstScalerOUT->astChannels[1].uptrAddrVirt);
    }
	
    if(pstScalerOUT->astChannels[2].u32AddrPhy){
        printf("release dst ch2\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerOUT->astChannels[2].u32AddrPhy, (AR_VOID *)pstScalerOUT->astChannels[2].uptrAddrVirt);
    }
	
    if(pstScalerOUT->astChannels[3].u32AddrPhy){
        printf("release dst ch3\n");
        ar_hal_sys_mmz_free((AR_U64)pstScalerOUT->astChannels[3].u32AddrPhy, (AR_VOID *)pstScalerOUT->astChannels[3].uptrAddrVirt);
    }
	
    return 0;
}

static AR_S32 SAMPLE_SCALER_Load_Src_Img(AR_IMG_S * stImgIn, AR_CHAR*s8SrcFileName, AR_MPI_IMG_INFO_S stImgFormat)
{
    //AR_S32 s32Ret =-1;  
    if((!stImgIn) || (!s8SrcFileName)){
        printf("stImgIn || src_file_name is NULL\r\n");
        return -1;
    }
    
    printf("SAMPLE_SCALER_Load_Src_Img : s8SrcFileName = %s\n", s8SrcFileName);
    FILE* pSrcFileFp =0;
    pSrcFileFp = fopen(s8SrcFileName, "rb+");
    if(!pSrcFileFp){
        printf("open file fail!!\n");
        return -1;
    } 
    
    AR_S32 s32SrcHeight = stImgIn->u32Height;
    AR_S32 s32SrcWeight = stImgIn->u32Width;
    
    AR_UCHAR *pu8SrcVaCh0 = (AR_UCHAR *)stImgIn->astChannels[0].uptrAddrVirt;
    AR_UCHAR *pu8SrcVaCh1 = (AR_UCHAR *)stImgIn->astChannels[1].uptrAddrVirt;
    AR_UCHAR *pu8SrcVaCh2 = (AR_UCHAR *)stImgIn->astChannels[2].uptrAddrVirt;
    AR_UCHAR *pu8SrcVaCh3 = (AR_UCHAR *)stImgIn->astChannels[3].uptrAddrVirt;
    
    AR_S32 s32WidthCh0=0;
    AR_S32 s32HeightCh0=0;
    AR_S32 s32WidthCh1=0;
    AR_S32 s32HeightCh1=0;
    AR_S32 s32WidthCh2=0;
    AR_S32 s32HeightCh2=0;
    AR_S32 s32WidthCh3=0;
    AR_S32 s32HeightCh3=0;
    AR_S32 i = 0;

    if(stImgIn->enFormat == AR_IMG_RGB_INTLV){
        if(0 == stImgFormat.s32SrcStride){
            s32WidthCh0 = s32SrcWeight * 3;
            s32HeightCh0 = s32SrcHeight;
        }else{
            s32WidthCh0 = stImgIn->astChannels[0].u32Stride;
            s32HeightCh0 = s32SrcHeight;
        }
        
        for( i = 0; i < s32HeightCh0; i++){
            fread(pu8SrcVaCh0, 1, s32WidthCh0, pSrcFileFp);
            pu8SrcVaCh0 += stImgIn->astChannels[0].u32Stride;
        }
        
        fclose(pSrcFileFp);
        pSrcFileFp=0;
        return 0;
    }
	
    if(stImgIn->enFormat == AR_IMG_RGBD_INTLV){
        if(0 == stImgFormat.s32SrcStride){
            s32WidthCh0 = s32SrcWeight * 4;
            s32HeightCh0 = s32SrcHeight;
        }else{
            s32WidthCh0 = stImgIn->astChannels[0].u32Stride;
            s32HeightCh0 = s32SrcHeight;
        }
        
        for( i = 0; i < s32HeightCh0; i++){
            fread(pu8SrcVaCh0, 1, s32WidthCh0, pSrcFileFp);
            pu8SrcVaCh0 += stImgIn->astChannels[0].u32Stride;
        }
        
        fclose(pSrcFileFp);
        pSrcFileFp=0;
        return 0;
    }

    //CH0 
    if(0 == stImgFormat.s32SrcStride){
        s32WidthCh0 = s32SrcWeight;
        s32HeightCh0 = s32SrcHeight;
    }else{
        s32WidthCh0 = stImgIn->astChannels[0].u32Stride;
        s32HeightCh0 = s32SrcHeight;
    }
    
    for(i = 0; i < s32HeightCh0; i++){
        fread(pu8SrcVaCh0, 1, s32WidthCh0, pSrcFileFp);
        pu8SrcVaCh0 += stImgIn->astChannels[0].u32Stride;
    }

    switch(stImgIn->enFormat){
        case AR_IMG_YUV444P:
        case AR_IMG_RGB:
        case AR_IMG_BGR:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = s32SrcWeight;
                s32HeightCh2 = s32SrcHeight;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight;
            }
            break;
        case AR_IMG_YUV444SP:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight * 2;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = 0;
                s32HeightCh2 = 0;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight;
            }
            break;
        case AR_IMG_YUV422P:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight/2;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = s32SrcWeight/2;
                s32HeightCh2 = s32SrcHeight;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight;
            }
            break;
        case AR_IMG_YUV422SP:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = 0;
                s32HeightCh2 = 0;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight;
            }
            break;
        case AR_IMG_YUV420P:
        case AR_IMG_YUVI420:
        case AR_IMG_YV12:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight/2;
                s32HeightCh1 = s32SrcHeight/2;
                s32WidthCh2 = s32SrcWeight/2;
                s32HeightCh2 = s32SrcHeight/2;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight/2;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight/2;
            }
            break;
        case AR_IMG_YUV420SP:
        case AR_IMG_NV12:
        case AR_IMG_NV21:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight;
                s32HeightCh1 = s32SrcHeight/2;
                s32WidthCh2 = 0;
                s32HeightCh2 = 0;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight/2;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight/2;
            }
            break;
        case AR_IMG_GRAY:
            s32WidthCh1 = 0;
            s32HeightCh1 = 0;
            s32WidthCh2 = 0;
            s32HeightCh2 = 0;
            break;
        case AR_IMG_RGBD:
            if(0 == stImgFormat.s32SrcStride){
                s32WidthCh1 = s32SrcWeight;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = s32SrcWeight;
                s32HeightCh2 = s32SrcHeight;
                s32WidthCh3 = s32SrcWeight;
                s32HeightCh3 = s32SrcHeight;
            }
            else{
                s32WidthCh1 = stImgIn->astChannels[1].u32Stride;
                s32HeightCh1 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[2].u32Stride;
                s32HeightCh2 = s32SrcHeight;
                s32WidthCh2 = stImgIn->astChannels[3].u32Stride;
                s32HeightCh2 = s32SrcHeight;
            }
            break;
        default:
            printf("input format is error %d\r\n", stImgIn->enFormat);
            return -1;
    }
	
    printf("SAMPLE_SCALER_Load_Src_Img enSrcType = %d\r\n", stImgIn->enFormat);
    
    //CH1
    if(s32WidthCh1){   
        for(i = 0; i < s32HeightCh1; i++){
            fread(pu8SrcVaCh1, 1, s32WidthCh1, pSrcFileFp);
            pu8SrcVaCh1 += stImgIn->astChannels[1].u32Stride;
        }
    }

	//CH2
    if(s32WidthCh2){
        for(i = 0; i < s32HeightCh2; i++){
            fread(pu8SrcVaCh2, 1, s32WidthCh2, pSrcFileFp);
            pu8SrcVaCh2 += stImgIn->astChannels[2].u32Stride;
        }
    }

    //CH3
    if(s32WidthCh3){
        for(i = 0; i < s32HeightCh3; i++){
            fread(pu8SrcVaCh3, 1, s32WidthCh3, pSrcFileFp);
            pu8SrcVaCh3 += stImgIn->astChannels[3].u32Stride;
        }
    }
  
    fclose(pSrcFileFp);
    pSrcFileFp=0;
    return 0;
}

static AR_S32 SAMPLE_SCALER_Store_Dst_Img(AR_IMG_S * stImgOut, AR_CHAR*s8DstFileName)
{
	if((!stImgOut) || (!s8DstFileName))
	{
		printf("pstParam || s8DstFileName\r\n");
		return -1;
	}
    
	FILE* pDstFileFp =0;
	pDstFileFp = fopen(s8DstFileName, "wb+");
	if(!pDstFileFp)
	{
		printf("open file fail!!\n");
		return -1;
	}

	AR_UCHAR *pu8RDstVa = (AR_UCHAR *)stImgOut->astChannels[0].uptrAddrVirt;
	AR_UCHAR *pu8GDstVa = (AR_UCHAR *)stImgOut->astChannels[1].uptrAddrVirt;
	AR_UCHAR *pu8BDstVa = (AR_UCHAR *)stImgOut->astChannels[2].uptrAddrVirt;
	AR_UCHAR *pu8DDstVa = (AR_UCHAR *)stImgOut->astChannels[3].uptrAddrVirt;

	AR_S32 s32Ret =-1;
	AR_S32 s32DataLenDst=0;
	AR_S32 s32PlanarDst=0;
	
	if(stImgOut->enFormat == AR_IMG_RGB  || stImgOut->enFormat == AR_IMG_YUV444P || stImgOut->enFormat == AR_IMG_BGR)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8BDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(stImgOut->enFormat == AR_IMG_RGBD)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8BDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8DDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(stImgOut->enFormat == AR_IMG_RGB_INTLV || stImgOut->enFormat == AR_IMG_RGBD_INTLV || stImgOut->enFormat == AR_IMG_BGR_INTLV)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(stImgOut->enFormat == AR_IMG_YUV422P)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
        s32DataLenDst = stImgOut->astChannels[1].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8BDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if((stImgOut->enFormat == AR_IMG_YUV420P) || (stImgOut->enFormat == AR_IMG_YUVI420) || (stImgOut->enFormat == AR_IMG_YV12))
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		printf("s32DataLenDst = %d\n", s32DataLenDst);
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
        
		s32DataLenDst = stImgOut->astChannels[1].u32Stride * stImgOut->u32Height/2;
		printf("s32DataLenDst1 = %d\n", s32DataLenDst);
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8BDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
    else if(stImgOut->enFormat == AR_IMG_YUV444SP)
    {
        s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
        s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
        s32DataLenDst = stImgOut->astChannels[1].u32Stride * stImgOut->u32Height;
        s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst*2,pDstFileFp);
        if(s32Ret != s32DataLenDst*2)
        {
        	printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
        	return -1;
        }
    }
    else if(stImgOut->enFormat == AR_IMG_YUV422SP)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		s32DataLenDst = stImgOut->astChannels[1].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
    else if((stImgOut->enFormat == AR_IMG_YUV420SP) || (stImgOut->enFormat == AR_IMG_NV12) || (stImgOut->enFormat == AR_IMG_NV21))
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		s32DataLenDst = stImgOut->astChannels[1].u32Stride * stImgOut->u32Height/2;
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(stImgOut->enFormat == AR_IMG_GRAY)
	{
		s32DataLenDst = stImgOut->astChannels[0].u32Stride * stImgOut->u32Height;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}	
    else
    {
        printf("the output format is not support\n");
        return -1;
    }

	fflush(pDstFileFp);
	fclose(pDstFileFp);
	pDstFileFp=0;
	printf("SAMPLE_SCALER_Store_Dst_Img : s8DstFileName =%s \n", s8DstFileName);
	printf("dst file store finished\r\n");

	return 0;
}


static AR_U32 gu32QuitFlag = 0;

void SignalHandler(int sig)
{
    printf("Get signal %d\r\n", sig);
    
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    
    if (SIGINT == sig || SIGTERM == sig) {
        printf("termination abnormally, signo:%d!\n", sig);
        gu32QuitFlag = 1;
    }  
}

AR_S32 ImgLoadStride(AR_IMG_S * stImgIn, AR_IMG_S * stImgOut, AR_MPI_IMG_INFO_S stImgFormat)
{
    /*********************Load Src/Dst Stride***********************/    
    stImgIn->u32FrameId = 1;
    stImgOut->u32FrameId = 1;
    stImgIn->enFormat = stImgFormat.eImgFormat;
    stImgIn->u32Width= stImgFormat.s32SrcWidth;
    stImgIn->u32Height= stImgFormat.s32SrcHeight;
    stImgOut->enFormat = stImgFormat.eImgFormat;
    stImgOut->u32Height = stImgFormat.s32DstHeight;
    stImgOut->u32Width = stImgFormat.s32DstWidth;
    
    switch(stImgFormat.eImgFormat){
        case AR_IMG_YUV444P:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_YUV444P;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_YUV444P;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }
            break;
        case AR_IMG_YUV422P:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_YUV422P;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 32);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_YUV422P;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 32);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }
            break;
        case AR_IMG_YUV420P:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_YUV420P;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 32);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_YUV420P;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 32);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }
            break;
        case AR_IMG_RGB:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_RGB;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_RGB;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }
            break;
		case AR_IMG_BGR:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_BGR;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_BGR;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
            }
            break;
        case AR_IMG_RGBD:
            stImgIn->u32ChannelNum = 4;
            stImgIn->enFormat = AR_IMG_RGBD;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[3].u32Stride = stImgIn->astChannels[0].u32Stride;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[3].u32Stride = stImgIn->astChannels[0].u32Stride;
            }
            
            stImgOut->u32ChannelNum = 4;
            stImgOut->enFormat = AR_IMG_RGBD;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[3].u32Stride = stImgOut->astChannels[0].u32Stride;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[3].u32Stride = stImgOut->astChannels[0].u32Stride;
            }
            break;
        case AR_IMG_RGB_INTLV: 
            stImgIn->u32ChannelNum = 1;
            stImgIn->enFormat = AR_IMG_RGB_INTLV;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = 3 * SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 1;
            stImgOut->enFormat = AR_IMG_RGB_INTLV;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = 3 * SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
		case AR_IMG_BGR_INTLV: 
            stImgIn->u32ChannelNum = 1;
            stImgIn->enFormat = AR_IMG_BGR_INTLV;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = 3 * SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 1;
            stImgOut->enFormat = AR_IMG_BGR_INTLV;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = 3 * SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_RGBD_INTLV:
            stImgIn->u32ChannelNum = 1;
            stImgIn->enFormat = AR_IMG_RGBD_INTLV;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
                stImgIn->astChannels[3].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = 4 * SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
                stImgIn->astChannels[3].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 1;
            stImgOut->enFormat = AR_IMG_RGBD_INTLV;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
                stImgOut->astChannels[3].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = 4 * SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
                stImgOut->astChannels[3].u32Stride = 0;
            }
            break;
        case AR_IMG_YUV444SP:
            stImgIn->u32ChannelNum = 2;
            stImgIn->enFormat = AR_IMG_YUV444SP;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = 2 * stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = 2 * stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 2;
            stImgOut->enFormat = AR_IMG_YUV444SP;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = 2 * stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = 2 * stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_YUV422SP:
            stImgIn->u32ChannelNum = 2;
            stImgIn->enFormat = AR_IMG_YUV422SP;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 2;
            stImgOut->enFormat = AR_IMG_YUV422SP;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_YUV420SP:
            stImgIn->u32ChannelNum = 2;
            stImgIn->enFormat = AR_IMG_YUV420SP;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 2;
            stImgOut->enFormat = AR_IMG_YUV420SP;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_NV12:
            stImgIn->u32ChannelNum = 2;
            stImgIn->enFormat = AR_IMG_NV12;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 2;
            stImgOut->enFormat = AR_IMG_NV12;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_NV21:
            stImgIn->u32ChannelNum = 2;
            stImgIn->enFormat = AR_IMG_NV21;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 2;
            stImgOut->enFormat = AR_IMG_NV21;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        case AR_IMG_YUVI420:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_YUVI420;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 32);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_YUVI420;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 32);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }
            break;
        case AR_IMG_YV12:
            stImgIn->u32ChannelNum = 3;
            stImgIn->enFormat = AR_IMG_YV12;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 32);
                stImgIn->astChannels[1].u32Stride = stImgIn->astChannels[0].u32Stride/2;
                stImgIn->astChannels[2].u32Stride = stImgIn->astChannels[0].u32Stride/2;
            }
            
            stImgOut->u32ChannelNum = 3;
            stImgOut->enFormat = AR_IMG_YV12;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 32);
                stImgOut->astChannels[1].u32Stride = stImgOut->astChannels[0].u32Stride/2;
                stImgOut->astChannels[2].u32Stride = stImgOut->astChannels[0].u32Stride/2;
            }
            break;
        case AR_IMG_GRAY:
            stImgIn->u32ChannelNum = 1;
            stImgIn->enFormat = AR_IMG_GRAY;
            if(stImgFormat.s32SrcStride){
                stImgIn->astChannels[0].u32Stride = stImgFormat.s32SrcStride;
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }else{
                stImgIn->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgIn->u32Width, 16);
                stImgIn->astChannels[1].u32Stride = 0;
                stImgIn->astChannels[2].u32Stride = 0;
            }
            
            stImgOut->u32ChannelNum = 1;
            stImgOut->enFormat = AR_IMG_GRAY;
            if(stImgFormat.s32DstStride){
                stImgOut->astChannels[0].u32Stride = stImgFormat.s32DstStride;
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }else{
                stImgOut->astChannels[0].u32Stride = SCALER_STRIDE_ALIGNE(stImgOut->u32Width, 16);
                stImgOut->astChannels[1].u32Stride = 0;
                stImgOut->astChannels[2].u32Stride = 0;
            }
            break;
        default:
            printf("input format not support %d\r\n", stImgFormat.eImgFormat);
            return -1;
    }

    printf("stImgIn.astChannels[0].u32Stride = %d\r\n", stImgIn->astChannels[0].u32Stride);
    printf("stImgIn.astChannels[1].u32Stride = %d\r\n", stImgIn->astChannels[1].u32Stride);
    printf("stImgIn.astChannels[2].u32Stride = %d\r\n", stImgIn->astChannels[2].u32Stride); 
    printf("stImgIn.astChannels[3].u32Stride = %d\r\n", stImgIn->astChannels[3].u32Stride);
    printf("stImgOut.astChannels[0].u32Stride = %d\r\n", stImgOut->astChannels[0].u32Stride);
    printf("stImgOut.astChannels[1].u32Stride = %d\r\n", stImgOut->astChannels[1].u32Stride);
    printf("stImgOut.astChannels[2].u32Stride = %d\r\n", stImgOut->astChannels[2].u32Stride);
    printf("stImgOut.astChannels[3].u32Stride = %d\r\n", stImgOut->astChannels[3].u32Stride);
    
    return 0;
}

AR_S32 s32ScalerDone = 0;

void AR_SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    int flag = -1;
    AR_U64 u64Tic = 0;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
        SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        //while(!s32ScalerDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_SCALER_Suspend();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("scaler suspend failed, ret:%d!\n", ret);
        else
            printf("scaler suspend success!\n");
        
        flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        //while(!s32ScalerDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_SCALER_Resume();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("scaler resume failed, ret:%d!\n", ret);
        else
            printf("scaler resume success!\n");
        
        flag = 0;
    }
    else
    {
        printf("fault sysctl event:%d\n", eSysctlEvent);
        return;
    }

    ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);
    if(ret)
        printf("event SYSCTL_Event_done fail!\n");
    else
        printf("event SYSCTL_Event_done!\n");
    
    if(-1 != flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}

AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
    AR_S32 s32Ret = 0;
    /*********************Args Parser***********************/
    AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-f   : test format(0:scale 1:ratio_scale)\n");
        printf("-SrcW: Src Img Width\n");
        printf("-SrcH: Src Img Height\n");
        printf("-SrcStride: Src Img Stride, Y/U/V should align with 16, if no equal 0\n");
        printf("-DstW: Dst Img Width\n");
        printf("-DstH: Dst Img Height\n");
        printf("-DstStride: Dst Img Stride, Y/U/V should align with 16, if no equal 0\n");
        printf("-Xc  : x coordinate\n");
        printf("-Yc  : y coordinate\n");
        printf("-Wc  : scaler width\n");
        printf("-Hc  : scaler height\n");
        printf("-Path : file path, /mnt/yuv_420p.yuv");
        printf("-Scalemode : 0:BICU, 1:BILINEAR \n");
        printf("-AlignMode : output IMG AlignMode(for ratio mode)\n");
        printf("-Num : Img Number\n");
        printf("-ImgF : IMG Format: (0:YUV444P, 1:YUV444SP, 2:YUV422P ...)\n");
        printf("FastBoot : if enable fastboot, default no need\n");
        printf("default: test_img_scaler -f 0 -SrcW 1024 -SrcH 768 -SrcStride 0 -DstW 1024 -DstH 1024 -DstStride 0 -Xc 0 -Yc 0 -Wc 1024 -Hc 768 -Path /mnt/yuv420p.yuv -Scalemode 0 -AlignMode 0 -Num 1 -ImgF 4\n");
        printf("note:\n");
        return 0;
    }

    AR_S32 s32Format = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-f", argc, argv);
    if(s32Index>0){
        s32Format=atoi(argv[s32Index+1]);
    }
    
    AR_S32 s32SrcWidth = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-SrcW", argc, argv);
    if(s32Index>0){
        s32SrcWidth=atoi(argv[s32Index+1]);
        if(s32SrcWidth >4096 || s32SrcWidth <0){
            printf("s32SrcWidth error \r\n");
            return -1;
        }
    }else{
        printf("no src width defined\r\n");
        return -1;
    }

    AR_S32 s32SrcHeight = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-SrcH",argc,argv);
    if(s32Index>0){
        s32SrcHeight=atoi(argv[s32Index+1]);
        if(s32SrcHeight >2160 || s32SrcHeight <0){
            printf("s32Height error \r\n");
            return -1;
        }
    }else{
            printf("no src height defined\r\n");
            return -1;
    }
	
    AR_S32 s32SrcStride = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-SrcStride",argc,argv);
    if(s32Index>0){
        s32SrcStride=atoi(argv[s32Index+1]);
    }else{
        printf("no src stride defined\r\n");
        return -1;
    }

    AR_S32 s32DstWidth = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-DstW",argc,argv);
    if(s32Index>0){
        s32DstWidth=atoi(argv[s32Index+1]);
        if(s32DstWidth >4096 || s32DstWidth <0){
            printf("s32DstWidth error \r\n");
            return -1;
        }
    }else{
        printf("no dst width defined\r\n");
        return -1;
    }
	
    AR_S32 s32DstHeight = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-DstH",argc,argv);
    if(s32Index>0){
        s32DstHeight=atoi(argv[s32Index+1]);
        if(s32DstHeight >2160 || s32DstHeight <0){
            printf("s32DstHeight error \r\n");
            return -1;
        }
    }else{
        printf("no dst height defined\r\n");
        return -1;
    }

    AR_S32 s32DstStride = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-DstStride",argc,argv);
    if(s32Index>0){
        s32DstStride=atoi(argv[s32Index+1]);
    }else{
        printf("no dst stride defined\r\n");
        return -1;
    }
    
    AR_S32 s32XCoordinate = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Xc",argc,argv);
    if(s32Index>0){
        s32XCoordinate=atoi(argv[s32Index+1]);
        if(s32XCoordinate >4096 || s32XCoordinate <0){
            printf("s32XCoordinate error, should not > 4096 && <0 \r\n");
            return -1;
        }
    }

    AR_S32 s32YCoordinate = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Yc",argc,argv);
    if(s32Index>0){
        s32YCoordinate=atoi(argv[s32Index+1]);
        if(s32YCoordinate >2160 || s32YCoordinate <0){
            printf("s32YCoordinate error, should not > 2160 && <0 \r\n");
            return -1;
        }
    }

    AR_S32 s32WCoordinate = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Wc",argc,argv);
    if(s32Index>0){
        s32WCoordinate=atoi(argv[s32Index+1]);
        if(s32WCoordinate >4096 || s32WCoordinate <0){
            printf("s32WCoordinate error, should not > 4096 && <0 \r\n");
            return -1;
        }
    }

    AR_S32 s32HCoordinate = 0;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Hc",argc,argv);
    if(s32Index>0){
        s32HCoordinate=atoi(argv[s32Index+1]);
        if(s32HCoordinate >2160 || s32HCoordinate <0){
            printf("s32HCoordinate error, should not > 2160 && <0 \r\n");
            return -1;
        }
    }

    AR_CHAR s8Path[64] = {};
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Path",argc,argv);
    if(s32Index>0){
        strcpy(s8Path,argv[s32Index+1]);
        printf("s8Path=%s \r\n",s8Path);
    }else{
        printf(" no Path defined\r\n");
        return -1;
    }
	
    AR_S32 s32Mode = 0;;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Scalemode",argc,argv);
    if(s32Index>0){
        s32Mode = atoi(argv[s32Index+1]);
    }else{
        printf(" no Scalemode defined\r\n");
        return -1;
    }

    AR_IMG_FORMAT_E enInputFormat;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-ImgF",argc,argv);
    if(s32Index>0){
        enInputFormat = atoi(argv[s32Index+1]);
    }else{
        printf(" no ImgF defined\r\n");
        return -1;
    }


    AR_U32 u32AlignMode = AR_IMG_ALIGN_TO_UP;
    if(s32Format){
        s32Index=SAMPLE_Find_Pra_Index_By_Name("-AlignMode",argc,argv);
        if(s32Index>0){
            u32AlignMode = strtoul(argv[s32Index+1], 0, 0);
        }else{
            printf(" NO AlignMode defined\r\n");
            return -1;
        }
    }

    AR_S32 s32ImgNum = 1;
    s32Index=SAMPLE_Find_Pra_Index_By_Name("-Num",argc,argv);
    if(s32Index>0){
        s32ImgNum = atoi(argv[s32Index+1]);
    }

    AR_S32 s32FastBoot = 0;

    s32Index=SAMPLE_Find_Pra_Index_By_Name("FastBoot",argc,argv);
    if(s32Index>0){
        s32FastBoot = 1;
    }
    /*** register in app to sysctl ***/
    if(s32FastBoot){
        g_phandle = AR_MPI_SYSCTL_Register(argv[0], 0, &AR_SAMPLE_fast_boot_func);
        if(!g_phandle){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("register sysctl %s success!\n", argv[0]);
    }

    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
	
    printf("******************Param********************\n");
    printf("test format : %d (1: for ratio scale, 0: for normal scale)\n", s32Format);
    printf("Src Width : %d\n", s32SrcWidth);
    printf("Src Height : %d\n", s32SrcHeight);
    printf("Src Stride : %d\n", s32SrcStride);
    printf("Dst Width : %d\n", s32DstWidth);
    printf("Dst Height : %d\n", s32DstHeight);
    printf("Dst Stride : %d\n", s32DstStride);
    printf("Xc : %d\n", s32XCoordinate);
    printf("Yc : %d\n", s32YCoordinate);
    printf("Wc : %d\n", s32WCoordinate);
    printf("Hc : %d\n", s32HCoordinate);
    printf("Path : %s\n", s8Path);
    printf("AlignMode : 0x%x\n", u32AlignMode);
    printf("ScaleMode (0:BICU, 1:BILINEAR): %d\n", s32Mode);
    printf("Num : %d\n", s32ImgNum);
    printf("Img format : %d\n", enInputFormat);
    printf("******************Param********************\n");

    AR_MPI_IMG_INFO_S stImgInfo = {0};
    stImgInfo.s32SrcWidth = s32SrcWidth;
    stImgInfo.s32SrcHeight = s32SrcHeight;
    stImgInfo.s32SrcStride = s32SrcStride;
    stImgInfo.s32DstWidth = s32DstWidth;
    stImgInfo.s32DstHeight = s32DstHeight;
    stImgInfo.s32DstStride = s32DstStride;
    stImgInfo.eImgFormat = enInputFormat;
    
    AR_CHAR s8SrcFileName[64];
    AR_CHAR s8DstFileName[64];
    AR_S32 i = 0;
    
    strcpy(s8SrcFileName, s8Path);	
    /*********************Caculate Stride/	Fill Params***********************/
    AR_HAL_SCALER_CROP_S *stScalerCrop = NULL;

    stScalerCrop = (AR_HAL_SCALER_CROP_S * )malloc(s32ImgNum * sizeof(AR_HAL_SCALER_CROP_S));
    for(i = 0; i < s32ImgNum; i++){
    stScalerCrop[i].u32H = s32HCoordinate;
    stScalerCrop[i].u32W = s32WCoordinate;
    stScalerCrop[i].u32X = s32XCoordinate;
    stScalerCrop[i].u32Y = s32YCoordinate;
    }

    AR_IMG_S *stImgIn = NULL;
    AR_IMG_S *stImgOut = NULL;
    stImgIn = (AR_IMG_S * )malloc(s32ImgNum * sizeof(AR_IMG_S));
    stImgOut = (AR_IMG_S * )malloc(s32ImgNum * sizeof(AR_IMG_S));
    
    for(i = 0; i < s32ImgNum; i++){
        ImgLoadStride(stImgIn+i, stImgOut+i, stImgInfo);
        s32Ret = SAMPLE_SCALER_MMZ_Buffer_Request(stImgIn+i, stImgOut+i);
        if(s32Ret < 0){
            printf("scaler mmz buffer failed\n");
            free(stScalerCrop);
            free(stImgIn);
            free(stImgOut);
            return -1;
        }
        s32Ret = SAMPLE_SCALER_Load_Src_Img(stImgIn+i, s8SrcFileName, stImgInfo);
        if(s32Ret < 0){
            printf("scaler load img failed\n");
            goto exit_handler;
        }
    }
	
	/*********************AR_MPI_SCALER***********************/

    printf("s32ImgNum = %d\n", s32ImgNum);
    AR_U64 u32UsecTime = 0;
    struct timeval stTime1, stTime2;
    if(s32FastBoot){
		AR_S32 s32Cnt = 0;
        while(!gu32QuitFlag){
            //usleep(1000*1000);
            //s32ScalerDone = 0;
            s32Ret= AR_MPI_SCALER_CropResize(stImgIn, stScalerCrop, stImgOut, s32ImgNum,s32Mode);
            //s32ScalerDone = 1;
            if(s32Ret < 0){
                if(s32Cnt++ > 10000)
                {
                    s32Cnt = 0;
                    printf("scaler reszie maybe suspend!\n");
                }
            }
        }
    }
    else{	
        gettimeofday(&stTime1, NULL); 
        if(s32Format == 0){
            s32Ret= AR_MPI_SCALER_CropResize(stImgIn, stScalerCrop, stImgOut, s32ImgNum,s32Mode);
        }else{
            s32Ret= AR_MPI_SCALER_CropResizeRatio(stImgIn, stScalerCrop, stImgOut, s32ImgNum, s32Mode, u32AlignMode);
        }
        gettimeofday(&stTime2, NULL);
        u32UsecTime = (stTime2.tv_sec * 1000 + stTime2.tv_usec/1000) - (stTime1.tv_sec * 1000 + stTime1.tv_usec/1000);
    }
	if(s32Ret < 0){
        printf("AR_MPI_SCALER_CropResize fail : %d\r\n", s32Ret);
        goto exit_handler;
    }
       
    printf("begin SAMPLE_SCALER_Store_Dst_Img\r\n");
    /*********************Store DST Img***********************/
    for(i = 0; i < s32ImgNum; i++){      
        sprintf(s8DstFileName, "Resize_to_%d_%d_from_%d_%d_with_stride_%d_%d_%d.yuv", 
        s32DstWidth, s32DstHeight, s32SrcWidth, s32SrcHeight, stImgOut->astChannels[0].u32Stride, i, u32AlignMode);
        s32Ret = SAMPLE_SCALER_Store_Dst_Img(stImgOut + i, s8DstFileName);
        if(s32Ret < 0){
           printf("SAMPLE_SCALER_Store_Dst_Img fail\r\n");
        }
        printf("after SAMPLE_SCALER_Store_Dst_Img : %s\r\n",s8DstFileName);
    }
    printf("SCALER RESIZE time is %lld ms\n",u32UsecTime);
	/*********************Release MMZ Buffer***********************/
exit_handler:

    for(i = 0; i < s32ImgNum; i++){
        s32Ret = SAMPLE_SCALER_MMZ_Buffer_Release(stImgIn + i, stImgOut + i);
        if(s32Ret < 0){
            printf("SAMPLE_SCALER_MMZ_Buffer_Release fail\r\n");
        }
    }

    /*** unregister in app to sysctl ***/
    if(s32FastBoot){
        s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
        if(s32Ret < 0){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("unregister sysctl %s success!\n", argv[0]);
    }

    if(stScalerCrop){
        free(stScalerCrop);
    }
    if(stImgIn){
        free(stImgIn);
    }
    if(stImgOut){
       free(stImgOut);
    }
     
    printf("test SCALER finished\r\n");
    return 0;
}
