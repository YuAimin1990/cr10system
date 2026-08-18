#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

//#include "ar_ion.h"
#include "hal_sys.h"
#include "mpi_ifc_api.h"
#include "mpi_sysctl.h"

MPI_SYSCTL_HANDLE g_phandle = NULL;

typedef struct
{	
	AR_U32 	u32INWidth;
	AR_U32 	u32INHeight;
	AR_U32 	u32AddrPhyCh0;
	void *    	pvAddrVirtCh0;
	AR_U32 	u32AddrPhyCh1;
	void *    	pvAddrVirtCh1;
	AR_U32 	u32AddrPhyCh2;
	void *    	pvAddrVirtCh2;
}AR_MPI_IFC_IN_S;

typedef struct
{	
	AR_U32 u32OUTWidth;
	AR_U32 u32OUTHeight;
	AR_U32 u32AddrPhyCh0;
	void *    pvAddrVirtCh0;
	AR_U32 u32AddrPhyCh1;
	void *    pvAddrVirtCh1;
	AR_U32 u32AddrPhyCh2;
	void *    pvAddrVirtCh2;
}AR_MPI_IFC_OUT_S;


#define SAMPLE_IFC_MMZ_BUFFER_MALLOC_SIZE_MAX (4096*2160)
#define SAMPLE_IFC_STRIDE_ALIGNE_TO(size,num) ( (num)*( ( (size)+(num)-1)/(num) ) )

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

//YUV422 to RGB
static AR_S32 SAMPLE_IFC_MMZ_Buffer_Request(AR_MPI_IFC_IN_S *pstIfcIN, AR_MPI_IFC_OUT_S * pstIfcOUT)
{
	AR_S32 s32Ret = -1;
	AR_U32 u32YSrcSize, u32USrcSize, u32VSrcSize;
	AR_U32 u32RDstSize, u32GDstSize, u32BDstSize;
	
	if((!pstIfcIN) || (!pstIfcOUT))
	{
		printf("pstIFC is NULL\r\n");
		return -1;
	}

	u32YSrcSize = SAMPLE_IFC_MMZ_BUFFER_MALLOC_SIZE_MAX;
	u32USrcSize = 2 * u32YSrcSize ;
	u32VSrcSize = u32YSrcSize;

	u32RDstSize = 3 * u32YSrcSize;
	u32GDstSize = u32YSrcSize;
	u32BDstSize = u32YSrcSize;

	/*******************SRC Y******************************/
	AR_U64 u64YSrcPa = 0;
	void * pvYSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, "YSrc", NULL, u32YSrcSize);
	if(s32Ret)
	{
		printf(" get src y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYSrcVa, 0, u32YSrcSize);
	
	pstIfcIN->pvAddrVirtCh0 = pvYSrcVa;
	pstIfcIN->u32AddrPhyCh0 = (AR_UINTPTR)u64YSrcPa;

	/*******************SRC U******************************/
	AR_U64 u64USrcPa = 0;
	void * pvUSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, "USrc", NULL, u32USrcSize);
	if(s32Ret)
	{
		printf(" get src u addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvUSrcVa,0,u32YSrcSize);
	pstIfcIN->pvAddrVirtCh1 = pvUSrcVa;
	pstIfcIN->u32AddrPhyCh1 = (AR_UINTPTR)u64USrcPa;

	/*******************SRC V******************************/
	AR_U64 u64VSrcPa = 0;
	void * pvVSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, "VSrc", NULL, u32VSrcSize);
	if(s32Ret)
	{
		printf(" get src v addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvVSrcVa, 0, u32YSrcSize);
	pstIfcIN->pvAddrVirtCh2 = pvVSrcVa;
	pstIfcIN->u32AddrPhyCh2 = (AR_UINTPTR)u64VSrcPa;
	
	/*******************DST R******************************/
	AR_U64 u64RDstPa = 0;
	void * pvRDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64RDstPa, &pvRDstVa, "RDst", NULL, u32RDstSize);
	if(s32Ret)
	{
		printf(" get dst_R addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvRDstVa,0,u32RDstSize);
	pstIfcOUT->pvAddrVirtCh0 = pvRDstVa;
	pstIfcOUT->u32AddrPhyCh0 = (AR_UINTPTR)u64RDstPa;

	/*******************DST G******************************/
	AR_U64 u64GDstPa = 0;
	void * pvGDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64GDstPa, &pvGDstVa, "GDst", NULL, u32GDstSize);
	if(s32Ret)
	{
		printf(" get dst_G addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvGDstVa, 0, u32GDstSize);
	pstIfcOUT->pvAddrVirtCh1 = pvGDstVa;
	pstIfcOUT->u32AddrPhyCh1 = (AR_UINTPTR)u64GDstPa;

	/*******************DST B******************************/
	AR_U64 u64BDstPa = 0;
	void * pvBDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64BDstPa, &pvBDstVa, "BDst", NULL, u32BDstSize);
	if(s32Ret)
	{
		printf(" get dst_B addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvBDstVa,0,u32BDstSize);
	pstIfcOUT->pvAddrVirtCh2 = pvBDstVa;
	pstIfcOUT->u32AddrPhyCh2 = (AR_UINTPTR)u64BDstPa;

	return 0;

}

static AR_S32 SAMPLE_IFC_MMZ_Buffer_Release(AR_MPI_IFC_IN_S pstYUV, AR_MPI_IFC_OUT_S pstRGB)
{

	if(pstYUV.pvAddrVirtCh0)
	   ar_hal_sys_mmz_free((AR_U64)pstYUV.u32AddrPhyCh0, pstYUV.pvAddrVirtCh0);

	if(pstYUV.pvAddrVirtCh1)
	   ar_hal_sys_mmz_free((AR_U64)pstYUV.u32AddrPhyCh1, pstYUV.pvAddrVirtCh1);

	if(pstYUV.pvAddrVirtCh2)
   	   ar_hal_sys_mmz_free((AR_U64)pstYUV.u32AddrPhyCh2, pstYUV.pvAddrVirtCh2);

	if(pstRGB.pvAddrVirtCh0)
	   ar_hal_sys_mmz_free((AR_U64)pstRGB.u32AddrPhyCh0, pstRGB.pvAddrVirtCh0);

	if(pstRGB.pvAddrVirtCh1)
	   ar_hal_sys_mmz_free((AR_U64)pstRGB.u32AddrPhyCh1, pstRGB.pvAddrVirtCh1);

	if(pstRGB.pvAddrVirtCh2)
	   ar_hal_sys_mmz_free((AR_U64)pstRGB.u32AddrPhyCh2, pstRGB.pvAddrVirtCh2);

	return 0;
}

static AR_S32 SAMPLE_IFC_Load_Src_Img(AR_MPI_IFC_IN_S *pstIfcIN, AR_CHAR*s8SrcFileName, AR_IMG_FORMAT_E enSrcType)
{
	if((!pstIfcIN) || (!s8SrcFileName))
	{
		printf("pstParam || src_file_name is NULL\r\n");
		return -1;
	}

	 FILE* pSrcFileFp =0;
	 pSrcFileFp = fopen(s8SrcFileName, "rb+");
	 if(!pSrcFileFp)
	 {
		printf("open file fail!!\n");
		return -1;
	 }

	 AR_S32 s32SrcHeight = pstIfcIN->u32INHeight;
	 AR_S32 s32SrcWeight = pstIfcIN->u32INWidth;

	 AR_UCHAR *pu8SrcVaCh0 = pstIfcIN->pvAddrVirtCh0;
	 AR_UCHAR *pu8SrcVaCh1 = pstIfcIN->pvAddrVirtCh1;
	 AR_UCHAR *pu8SrcVaCh2 = pstIfcIN->pvAddrVirtCh2;


    AR_S32 s32Ret =-1;
	AR_S32 s32DataLenSrcCh0=0;
	AR_S32 s32DataLenSrcCh1=0;
	AR_S32 s32DataLenSrcCh2=0;
	//AR_S32 s32PlanarSrc=0;

	//CH0 
	s32DataLenSrcCh0 = s32SrcWeight*s32SrcHeight;
       printf("s32SrcWeight = %d, s32SrcHeight =%d,s32DataLenSrcCh0 = %d\r\n",s32SrcWeight,  s32SrcHeight,s32DataLenSrcCh0);
	s32Ret = fread(pu8SrcVaCh0, 1, s32DataLenSrcCh0, pSrcFileFp);
	if(s32Ret != s32DataLenSrcCh0)
	{
	   printf("data_len:%d  ret_len:%d line:%d\n", s32DataLenSrcCh0, s32Ret, __LINE__);
          fclose(pSrcFileFp);
	   return -1;
	}

	switch(enSrcType)
	{
			case AR_IMG_YUV444P:
			case AR_IMG_RGB:
            case AR_IMG_BGR:
				s32DataLenSrcCh1 = s32SrcWeight*s32SrcHeight;
				s32DataLenSrcCh2 = s32SrcWeight*s32SrcHeight;
				break;
			case AR_IMG_YUV444SP:
				s32DataLenSrcCh1 = 2*s32SrcWeight*s32SrcHeight;
				s32DataLenSrcCh2 = 0;
				break;
			case AR_IMG_YUV422P:
				s32DataLenSrcCh1 = s32SrcWeight*s32SrcHeight/2;
				s32DataLenSrcCh2 = s32SrcWeight*s32SrcHeight/2;
				break;
			case AR_IMG_YUV422SP:
				s32DataLenSrcCh1 = s32SrcWeight*s32SrcHeight;
				s32DataLenSrcCh2 = 0;
				break;
			case AR_IMG_YUV420P:
			case AR_IMG_YUVI420:
			case AR_IMG_YV12:
				s32DataLenSrcCh1 = s32SrcWeight*s32SrcHeight/4;
				s32DataLenSrcCh2 = s32SrcWeight*s32SrcHeight/4;
				break;
			case AR_IMG_YUV420SP:
			case AR_IMG_NV12:
			case AR_IMG_NV21:
				s32DataLenSrcCh1 = s32SrcWeight*s32SrcHeight/2;
				s32DataLenSrcCh2 = 0;
				break;
			default:
				printf("input format is error %d\r\n", enSrcType);
                            fclose(pSrcFileFp);
				return -1;
	}

       //CH1
       if(s32DataLenSrcCh1)
       {
	    printf("input format = %d, s32DataLenSrcCh1 size = %d\r\n", enSrcType, s32DataLenSrcCh1);
	    s32Ret = fread(pu8SrcVaCh1,1,s32DataLenSrcCh1,pSrcFileFp);
	    if(s32Ret != s32DataLenSrcCh1)
	    {
	        printf("data_len:%d  ret_len:%d line:%d\n", s32DataLenSrcCh1, s32Ret, __LINE__);
               fclose(pSrcFileFp);
	        return -1;
	    }
        }
	//CH2
	if(s32DataLenSrcCh2)
	{
	       printf("input format = %d, s32DataLenSrcCh2 size = %d\r\n", enSrcType, s32DataLenSrcCh2);
		s32Ret = fread(pu8SrcVaCh2,1,s32DataLenSrcCh2,pSrcFileFp);
		if(s32Ret != s32DataLenSrcCh2)
		{
	   		printf("data_len:%d  ret_len:%d line:%d\n", s32DataLenSrcCh2, s32Ret, __LINE__);
                     fclose(pSrcFileFp);
	   		return -1;
		}
	}
	fclose(pSrcFileFp);
    	pSrcFileFp=0;
	return 0;

}

#if 0
static AR_S32 SAMPLE_IFC_Store_Bmp_Img(AR_MPI_IFC_OUT_S *pstIfcOUT, AR_CHAR*s8DstFileName, AR_IMG_FORMAT_E enOutType)
{
        //AR_S32 s32Ret = 0;

        if((!pstIfcOUT) || (!s8DstFileName))
        {
                printf("pstParam || s8DstFileName is NULL\r\n");
                return -1;
        }

        AR_S32 s32DstHeight = pstIfcOUT->u32OUTHeight;
        AR_S32 s32DstWidth = pstIfcOUT->u32OUTWidth;

        AR_UCHAR *pu8RDstVa = pstIfcOUT->pvAddrVirtCh0;
        AR_UCHAR *pu8GDstVa = pstIfcOUT->pvAddrVirtCh1;
        AR_UCHAR *pu8BDstVa = pstIfcOUT->pvAddrVirtCh2;

        FILE *pDstFileFp;
        AR_UCHAR * pucImg = NULL;

        AR_S32 s32FileSize = 54 + 3 * s32DstHeight * s32DstWidth;

        pucImg = (AR_UCHAR *)malloc(3 * s32DstHeight * s32DstWidth);
        memset(pucImg, 0, 3 * s32DstHeight * s32DstWidth);

        AR_S32 s32X, s32Y;
        AR_UCHAR s32R, s32G, s32B;
       
		for(AR_S32 i = 0; i < s32DstWidth; i++)
		{
			for(AR_S32 j=0; j<s32DstHeight; j++)
			{
				s32X = i;
				s32Y = (s32DstHeight - 1) - j;
				if(enOutType == AR_IMG_RGB)
		      	{
					s32R = *(pu8RDstVa + i + s32DstWidth * j);
					s32G = *(pu8GDstVa+ i + s32DstWidth * j);
					s32B = *(pu8BDstVa+ i + s32DstWidth * j);
				}
				else
				{
					s32R = *(pu8RDstVa + i + s32DstWidth * j);
					s32G = *(pu8RDstVa+ i +1 + s32DstWidth * j);
					s32B = *(pu8RDstVa+ i + 2 + s32DstWidth * j);
				}

				pucImg[(s32X+s32Y*s32DstWidth)*3+2] = s32R;
				pucImg[(s32X+s32Y*s32DstWidth)*3+1] = s32G;
				pucImg[(s32X+s32Y*s32DstWidth)*3+0] = s32B;
			}
		}
	  
	   
	   AR_UCHAR bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	   AR_UCHAR bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	   AR_UCHAR bmppad[3] = {0,0,0};

	   bmpfileheader[ 2] = (AR_UCHAR)(s32FileSize	 );
	   bmpfileheader[ 3] = (AR_UCHAR)(s32FileSize>> 8);
	   bmpfileheader[ 4] = (AR_UCHAR)(s32FileSize>>16);
	   bmpfileheader[ 5] = (AR_UCHAR)(s32FileSize>>24);

		bmpinfoheader[ 4] = (AR_UCHAR)(s32DstWidth);
		bmpinfoheader[ 5] = (AR_UCHAR)(s32DstWidth>> 8);
		bmpinfoheader[ 6] = (AR_UCHAR)(s32DstWidth>>16);
		bmpinfoheader[ 7] = (AR_UCHAR)(s32DstWidth>>24);
		bmpinfoheader[ 8] = (AR_UCHAR)(s32DstHeight);
		bmpinfoheader[ 9] = (AR_UCHAR)(s32DstHeight>> 8);
		bmpinfoheader[10] = (AR_UCHAR)(s32DstHeight>>16);
		bmpinfoheader[11] = (AR_UCHAR)(s32DstHeight>>24);
	   pDstFileFp = fopen(s8DstFileName,"wb+");
	   if(!pDstFileFp)
	   {
			   printf("open file %s fail\r\n",s8DstFileName);
			   return -1;
	   }
	   fwrite(bmpfileheader,1,14,pDstFileFp);
	   fwrite(bmpinfoheader,1,40,pDstFileFp);
	   for(AR_S32 i=0; i<s32DstHeight; i++)
	   {
			   fwrite(pucImg+s32DstWidth*i*3, 3, s32DstWidth,pDstFileFp);
			   fwrite(bmppad, 1, (4-(s32DstWidth*3)%4)%4, pDstFileFp);
	   }

	   free(pucImg);
	   fclose(pDstFileFp);

	   return 0;

}
#endif

static AR_S32 SAMPLE_IFC_Store_Dst_Img(AR_MPI_IFC_OUT_S *pstIfcOUT, AR_CHAR*s8DstFileName,  AR_IMG_FORMAT_E enOutType)
{
	if((!pstIfcOUT) || (!s8DstFileName))
	{
		printf("pstParam || s8DstFileName is NULL\r\n");
		return -1;
	}

	FILE* pDstFileFp =0;
	pDstFileFp = fopen(s8DstFileName, "wb+");
	if(!pDstFileFp)
	{
		printf("open file fail!!\n");
		return -1;
	}


	AR_S32 s32DstHeight = pstIfcOUT->u32OUTHeight;
	AR_S32 s32DstWeight = pstIfcOUT->u32OUTWidth;

	AR_UCHAR *pu8RDstVa = pstIfcOUT->pvAddrVirtCh0;
	AR_UCHAR *pu8GDstVa = pstIfcOUT->pvAddrVirtCh1;
	AR_UCHAR *pu8BDstVa = pstIfcOUT->pvAddrVirtCh2;

	AR_S32 s32Ret =-1;
	AR_S32 s32DataLenDst=0;
	AR_S32 s32PlanarDst=0;
	
	if(enOutType == AR_IMG_RGB || enOutType == AR_IMG_YUV444P || enOutType == AR_IMG_BGR)
	{
		s32DataLenDst = s32DstWeight*s32DstHeight;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8GDstVa,1,s32DataLenDst,pDstFileFp);
		s32Ret = fwrite(pu8BDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(enOutType == AR_IMG_RGB_INTLV || enOutType == AR_IMG_BGR_INTLV)
	{
		s32DataLenDst = 3 * s32DstWeight*s32DstHeight;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}
	else if(enOutType == AR_IMG_GRAY)
	{
		s32DataLenDst = s32DstWeight*s32DstHeight;
		s32Ret = fwrite(pu8RDstVa,1,s32DataLenDst,pDstFileFp);
		if(s32Ret != s32DataLenDst)
		{
		   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
		   return -1;
		}
	}	

	fclose(pDstFileFp);
    	pDstFileFp=0;
	//printf("dst file store finished\r\n");

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

AR_S32 s32IfcDone = 0;

void AR_SAMPLE_fast_boot_func(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    int ret;
    AR_U64 u64Tic = 0;
    AR_S32 s32Flag = -1;
    struct timeval tm_start;
    struct timeval tm_end;

    if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent ||
        SYSCTL_EVENT_SUSPEND == eSysctlEvent)
    {
        //while(!s32IfcDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_IFC_Suspend();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("ifc suspend failed, ret:%d!\n", ret);
        else
            printf("ifc suspend success!\n");
        
        s32Flag = 1;
    }
    else if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent ||
        SYSCTL_EVENT_RESUME == eSysctlEvent)
    {
        //while(!s32IfcDone){};
        gettimeofday(&tm_start, NULL);
        ret = AR_MPI_IFC_Resume();
        gettimeofday(&tm_end, NULL);
        if(ret)
            printf("ifc resume failed, ret:%d!\n", ret);
        else
            printf("ifc resume success!\n");
        
        s32Flag = 0;
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
    
    if(-1 != s32Flag)
    {
        u64Tic = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
        printf(" %s takes %f ms\n", s32Flag==1 ? "SUSPEND" : "RESUME", u64Tic*1.0/1000);
    }

}

AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{

/*********************Args Parser***********************/

	AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
	if(s32Index>0 || argc==1)
	{
        printf("-w: src width\n");
        printf("-h: src height\n");
        printf("-srcfile: src file name\n");
        printf("-Srctype : 0: YUV444P, 1: YUV444SP, 2:YUV422P, 3:YUV422SP, 4:YUV420P, 5:YUV420SP ...\n");
        printf("-Dsttype : 10:RGB, 11:BGR, 12: RGBD, 13:RGB_INTLV, 14:BGR_INTLV, 15: GRAY... \n");
        printf("-Stype : 0: BT601 FULL, 1: BT601 VIDEO, 2:BT709 FULL, 3:BT709 VIDEO\n");
        printf("FastBoot : if enable fastboot, default no need\n");
        return 0;
	}
	
	AR_S32 s32SrcWidth = 1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-w",argc,argv);
	if(s32Index>0)
	{
		s32SrcWidth=atoi(argv[s32Index+1]);
		printf("s32SrcWidth=%d \r\n",s32SrcWidth);
		if(s32SrcWidth >4096 || s32SrcWidth <0)
		{
			printf("s32SrcWidth error \r\n");
			return -1;
		}
	}

	AR_S32 s32SrcHeight = 1080;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-h",argc,argv);
	if(s32Index>0)
	{
		s32SrcHeight=atoi(argv[s32Index+1]);
		printf("s32SrcHeight=%d \r\n",s32SrcHeight);
		if(s32SrcHeight >2160 || s32SrcHeight <0)
		{
			printf("s32SrcHeight error \r\n");
			return -1;
		}
	}

	AR_CHAR s8SrcFileName[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-srcfile",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8SrcFileName,argv[s32Index+1]);
		printf("s8SrcFileName=%s \r\n",s8SrcFileName);
	}
	else{
		printf(" NO src file defined\r\n");
		return -1;
	}

	AR_CHAR s8DstFileName[64] = {};
#if 0
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-dstfile",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8DstFileName,argv[s32Index+1]);
		printf("s8DstFileName=%s \r\n",s8DstFileName);
	}
	else{
		printf(" NO dst file defined\r\n");
		return -1;
	}
#endif
	AR_IMG_FORMAT_E enInputFormat;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-Srctype",argc,argv);
	if(s32Index>0)
	{
		enInputFormat = atoi(argv[s32Index+1]);
	}
	else{
		printf(" NO Srctype defined\r\n");
		return -1;
	}

	AR_IMG_FORMAT_E enOutputFormat;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-Dsttype",argc,argv);
	if(s32Index>0)
	{
		enOutputFormat = atoi(argv[s32Index+1]);
	}
	else{
		printf(" NO enIfcType defined\r\n");
		return -1;
	}

	AR_MPI_IFC_STAN_E enIfcStatType;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-Stype",argc,argv);
	if(s32Index>0)
	{
		enIfcStatType=atoi(argv[s32Index+1]);
	}
	else{
		printf(" NO enIfcType defined\r\n");
		return -1;
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

	/*********************Caculate Stride/	Fill Params***********************/

    AR_CHAR s8SrcName[64];
    AR_CHAR s8DstName[64];
    AR_MPI_IFC_IN_S stIfcIN={0};
    AR_MPI_IFC_OUT_S stIfcOUT={0};

	stIfcIN.u32INWidth = s32SrcWidth;
	stIfcIN.u32INHeight = s32SrcHeight;
	
	stIfcOUT.u32OUTWidth = s32SrcWidth;
	stIfcOUT.u32OUTHeight = s32SrcHeight;
	
	/*********************Request MMZ Buffer***********************/

	AR_S32 s32Ret = SAMPLE_IFC_MMZ_Buffer_Request(&stIfcIN, &stIfcOUT);
	if(s32Ret < 0)
	{
		printf("SAMPLE_IFC_MMZ_Buffer_Request fail\r\n");
	}
	
	AR_U32 u32Loop =0;
	signal(SIGTERM, SignalHandler);
	signal(SIGINT, SignalHandler);

	AR_IMG_S stImgIn = {0};
	AR_IMG_S stImgOut = {0};

	stImgIn.u32FrameId = 1;
	stImgOut.u32FrameId = 1;
	stImgIn.u32Height = s32SrcHeight;
	stImgIn.u32Width = s32SrcWidth;
	stImgOut.u32Height = s32SrcHeight;
	stImgOut.u32Width = s32SrcWidth;
	switch(enInputFormat)
	{
			case AR_IMG_YUV444P:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_YUV444P;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width;// SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
                            strcpy(s8SrcName, "yuv444p");
				break;
			case AR_IMG_YUV444SP:
				stImgIn.u32ChannelNum = 2;
				stImgIn.enFormat = AR_IMG_YUV444SP;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = 2 * stImgIn.u32Width;//2 * SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = 0;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = 0;
                            strcpy(s8SrcName, "yuv444sp");
				break;
			case AR_IMG_YUV422P:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_YUV422P;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
                            strcpy(s8SrcName, "yuv422p");
				break;
			case AR_IMG_YUV422SP:
				stImgIn.u32ChannelNum = 2;
				stImgIn.enFormat = AR_IMG_YUV422SP;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = 0;
                            strcpy(s8SrcName, "yuv422sp");
				break;
			case AR_IMG_YUV420P:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_YUV420P;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
                            strcpy(s8SrcName, "yuv420p");
				break;
			case AR_IMG_YUVI420:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_YUVI420;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
                           strcpy(s8SrcName, "yuvI420");
				break;
			case AR_IMG_YV12:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_YV12;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width/2;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
                            strcpy(s8SrcName, "yv12");
				break;
			case AR_IMG_YUV420SP:
				stImgIn.u32ChannelNum = 2;
				stImgIn.enFormat = AR_IMG_YUV420SP;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = 0;
                           strcpy(s8SrcName, "yuv420sp");
				break;
			case AR_IMG_NV12:
				stImgIn.u32ChannelNum = 2;
				stImgIn.enFormat = AR_IMG_NV12;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = 0;
                            strcpy(s8SrcName, "nv12");
				break;
			case AR_IMG_NV21:
				stImgIn.u32ChannelNum = 2;
				stImgIn.enFormat = AR_IMG_NV21;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = 0;
                            strcpy(s8SrcName, "nv21");
				break;
			case AR_IMG_RGB:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_RGB;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
				strcpy(s8SrcName, "RGB");
				break;
                     case AR_IMG_BGR:
				stImgIn.u32ChannelNum = 3;
				stImgIn.enFormat = AR_IMG_BGR;
				stImgIn.astChannels[0].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[1].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
				stImgIn.astChannels[2].u32Stride = stImgIn.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);;
				stImgIn.astChannels[0].u32AddrPhy = stIfcIN.u32AddrPhyCh0;
				stImgIn.astChannels[1].u32AddrPhy = stIfcIN.u32AddrPhyCh1;
				stImgIn.astChannels[2].u32AddrPhy = stIfcIN.u32AddrPhyCh2;
				strcpy(s8SrcName, "BGR");
				break;
			default:
				printf("input format is error %d\r\n", enInputFormat);
				return -1;
	}

	//AR_U32 u32RGBWriteSize = 3 * stImgOut.u32Width * stImgOut.u32Height;
	//AR_U32 u32GRAYWriteSize = stImgOut.u32Width * stImgOut.u32Height;
	switch(enOutputFormat)
	{
		case AR_IMG_RGB:
			stImgOut.u32ChannelNum = 3;
			stImgOut.enFormat = AR_IMG_RGB;
			stImgOut.astChannels[0].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = stIfcOUT.u32AddrPhyCh1;
			stImgOut.astChannels[2].u32AddrPhy = stIfcOUT.u32AddrPhyCh2;
                     strcpy(s8DstName, "RGB");
			break;
             case AR_IMG_BGR:
			stImgOut.u32ChannelNum = 3;
			stImgOut.enFormat = AR_IMG_BGR;
			stImgOut.astChannels[0].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = stIfcOUT.u32AddrPhyCh1;
			stImgOut.astChannels[2].u32AddrPhy = stIfcOUT.u32AddrPhyCh2;
                     strcpy(s8DstName, "BGR");
			break;
		case AR_IMG_RGB_INTLV:
			stImgOut.u32ChannelNum = 1;
			stImgOut.enFormat = AR_IMG_RGB_INTLV;
			stImgOut.astChannels[0].u32Stride =3 * stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = 0;
			stImgOut.astChannels[2].u32AddrPhy = 0;
                    strcpy(s8DstName, "RGB24");
			break;
              case AR_IMG_BGR_INTLV:
			stImgOut.u32ChannelNum = 1;
			stImgOut.enFormat = AR_IMG_BGR_INTLV;
			stImgOut.astChannels[0].u32Stride =3 * stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = 0;
			stImgOut.astChannels[2].u32AddrPhy = 0;
                    strcpy(s8DstName, "BGR24");
			break;
		case AR_IMG_GRAY:
			stImgOut.u32ChannelNum = 1;
			stImgOut.enFormat = AR_IMG_GRAY;
			stImgOut.astChannels[0].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = 0;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = 0;
			stImgOut.astChannels[2].u32AddrPhy = 0;
                     strcpy(s8DstName, "Gray");
			break;
		case AR_IMG_YUV444P:
			stImgOut.u32ChannelNum = 3;
			stImgOut.enFormat = AR_IMG_YUV444P;
			stImgOut.astChannels[0].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[1].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			stImgOut.astChannels[2].u32Stride = stImgOut.u32Width;//SAMPLE_IFC_STRIDE_ALIGNE_TO(s32SrcWidth, 256);
			
			stImgOut.astChannels[0].u32AddrPhy = stIfcOUT.u32AddrPhyCh0;
			stImgOut.astChannels[1].u32AddrPhy = stIfcOUT.u32AddrPhyCh1;
			stImgOut.astChannels[2].u32AddrPhy = stIfcOUT.u32AddrPhyCh2;
                     strcpy(s8DstName, "yuv444p");
			break;
		default:
				printf("output format is error %d\r\n", enOutputFormat);
				return -1;
	}
    
	/*********************Load SRC Img***********************/
	s32Ret = SAMPLE_IFC_Load_Src_Img(&stIfcIN, s8SrcFileName, enInputFormat);
	if(s32Ret < 0)
	{
		printf("SAMPLE_IFC_Load_Src_Img fail\r\n");
		goto exit_handler;
	}
	printf("SAMPLE_IFC_Load_Src_Img finished\r\n");

      
	if(enOutputFormat == AR_IMG_RGB || enOutputFormat == AR_IMG_RGB_INTLV)
	{
		sprintf(s8DstFileName, "IFC_Cvt_from_%s_to_%s_%d_%d.rgb", s8SrcName, s8DstName, s32SrcWidth,s32SrcHeight);
	}
       else if(enOutputFormat == AR_IMG_BGR || enOutputFormat == AR_IMG_BGR_INTLV)
	{
		sprintf(s8DstFileName, "IFC_Cvt_from_%s_to_%s_%d_%d.bgr", s8SrcName, s8DstName, s32SrcWidth,s32SrcHeight);
	}
	else if(enOutputFormat == AR_IMG_YUV444P)
	{
		sprintf(s8DstFileName, "IFC_Cvt_from_%s_to_%s_%d_%d.yuv", s8SrcName, s8DstName, s32SrcWidth,s32SrcHeight);
	}
	else if(enOutputFormat == AR_IMG_GRAY)
	{
		sprintf(s8DstFileName, "IFC_Cvt_from_%s_to_%s_%d_%d.gray", s8SrcName, s8DstName, s32SrcWidth,s32SrcHeight);
	}
    
	/*********************AR_MPI_IFC***********************/
	//AR_MPI_IFC_RESULT_S stResult[2] = {0};
    AR_U64 u64UsecTime = 0;
    struct timeval stTime1, stTime2;
    if(s32FastBoot){
		AR_S32 s32Cnt = 0;
        while(!gu32QuitFlag){
            //usleep(1000*1000);
            //s32IfcDone = 0;
            s32Ret= AR_MPI_IFC_CvtColor(&stImgIn, &stImgOut, enIfcStatType);
            //s32IfcDone = 1;
            if(s32Ret < 0){
		if(s32Cnt++ > 10000){
			s32Cnt = 0;
                    	printf("ifc convert maybe suspend!\n");
		}
            }
        }
    }
    else{
        gettimeofday(&stTime1, NULL);
        s32Ret= AR_MPI_IFC_CvtColor(&stImgIn, &stImgOut, enIfcStatType);
        gettimeofday(&stTime2, NULL);
        u64UsecTime = (stTime2.tv_sec * 1000 + stTime2.tv_usec/1000) - (stTime1.tv_sec * 1000 + stTime1.tv_usec/1000);
    }
	if(s32Ret < 0)
	{
		printf("AR_MPI_IFC_ConvertColor fail, %d\r\n", s32Ret);
		goto exit_handler;
	}
    #if 0
       printf("stResult.s32SumR = %ld\n", stResult[0].s64SumR);
       printf("stResult.s32SumG = %ld\n", stResult[0].s64SumG);
       printf("stResult.s32SumB = %ld\n", stResult[0].s64SumB);
       printf("stResult.f32SSumR = %f\n", stResult[0].dSSumR);
       printf("stResult.f32SSumG = %f\n", stResult[0].dSSumG);
       printf("stResult.f32SSumB = %f\n", stResult[0].dSSumB);
       printf("stResult.f32AvgR = %f\n", stResult[0].dAvgR);
       printf("stResult.f32AvgG = %f\n", stResult[0].dAvgG);
       printf("stResult.f32AvgB = %f\n", stResult[0].dAvgB);
    #endif
	/*********************Store DST Img***********************/
       //sprintf(s8DstFileName, "IFC_Cvt_%d_size_%dx%d_%d.bmp", u32Loop,s32SrcWidth,s32SrcHeight,enIfcStatType);
	//SAMPLE_IFC_Store_Bmp_Img(&stIfcOUT, s8DstFileName, enOutputFormat);
	//printf("%s\r\n",s8DstFileName);
	s32Ret = SAMPLE_IFC_Store_Dst_Img(&stIfcOUT, s8DstFileName, enOutputFormat);
	if(s32Ret < 0)
	{
		printf("SAMPLE_IFC_Store_Dst_Img fail\r\n");
	}
       printf("IFC COLOR CVT time is %lld ms\n",u64UsecTime);
	printf("save file %s\r\n", s8DstFileName);
	u32Loop++;

	//while(1){ usleep(500*1000);}

exit_handler:

    /*** unregister in app to sysctl ***/
    if(s32FastBoot){
        s32Ret = AR_MPI_SYSCTL_Unregister(g_phandle);
        if(s32Ret < 0){
            printf("register sysctl %s failed!\n", argv[0]);
            return -1;
        }
        printf("unregister sysctl %s success!\n", argv[0]);
    }

	/*********************Release MMZ Buffer***********************/
	s32Ret = SAMPLE_IFC_MMZ_Buffer_Release(stIfcIN, stIfcOUT);
	if(s32Ret < 0)
	{
		printf("SAMPLE_IFC_MMZ_Buffer_Release fail\r\n");
	}

	printf("test IFC finished\r\n");

    return 0;
}
