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
#include "hal_sys.h"
#include "mpi_dsp_api.h"

#define SAMPLE_GAUSS_MMZ_BUFFER_MALLOC_SIZE_MAX  4096*3840

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


static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, const AR_CHAR *ps8Argv[])
{
    AR_S32 i=0;
	for(i=0;i<s32Argc;i++)
	{
         if(!strcmp(ps8Name,ps8Argv[i]))
         {
            printf("Find Pra %s @ %d ",ps8Name,i);
            return i;
         }
	}
	return 0;
}

static AR_S32 SAMPLE_DSP_MMZ_Buffer_Request(AR_DSP_IMG_S *pstDspImg)
{
	AR_S32 s32Ret = -1;
	if(!pstDspImg)
	{
		printf("pstDspImg is NULL\r\n");
		return -1;
	}

    /*******************SRC Y******************************/
	int u32YSrcSize = (pstDspImg->astSrcChannels[0].u32Stride+2)*(pstDspImg->astSrcChannels[0].u32Height+2); 
	AR_U64 u64YSrcPa = 0;
	void * pvYSrcVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YSrcPa, &pvYSrcVa, "YSrc", NULL, u32YSrcSize);
	if(s32Ret)
	{
		printf(" get src y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYSrcVa, 128, u32YSrcSize);
	pstDspImg->astSrcChannels[0].u64VirtAddr= (AR_UINTPTR)pvYSrcVa;
	pstDspImg->astSrcChannels[0].u64PhyAddr= (AR_UINTPTR)u64YSrcPa;

    int u32USrcSize=0;
    if(pstDspImg->u32ChannelNum ==3)
    {
        /*******************SRC U******************************/
		int u32USrcSize = (pstDspImg->astSrcChannels[1].u32Stride+2)*(pstDspImg->astSrcChannels[1].u32Height+2);
		AR_U64 u64USrcPa = 0;
		void * pvUSrcVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64USrcPa, &pvUSrcVa, "USrc", NULL, u32USrcSize);
		if(s32Ret)
		{
			printf(" get src u addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvUSrcVa,128,u32USrcSize);
		pstDspImg->astSrcChannels[1].u64VirtAddr= (AR_UINTPTR)pvUSrcVa;
	    pstDspImg->astSrcChannels[1].u64PhyAddr= (AR_UINTPTR)u64USrcPa;

		/*******************SRC V******************************/
		AR_U64 u64VSrcPa = 0;
		void * pvVSrcVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64VSrcPa, &pvVSrcVa, "VSrc", NULL, u32USrcSize);
		if(s32Ret)
		{
			printf(" get src v addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvVSrcVa,128,u32USrcSize);
		pstDspImg->astSrcChannels[2].u64VirtAddr= (AR_UINTPTR)pvVSrcVa;
	    pstDspImg->astSrcChannels[2].u64PhyAddr= (AR_UINTPTR)u64VSrcPa;
    }

	/*******************DST Y******************************/
	int u32YDstSize = pstDspImg->astSrcChannels[0].u32Stride*pstDspImg->astSrcChannels[0].u32Height; 
	AR_U64 u64YDstPa = 0;
	void * pvYDstVa = NULL;
	s32Ret = ar_hal_sys_mmz_alloc(&u64YDstPa, &pvYDstVa, "YDst", NULL, u32YDstSize);
	if(s32Ret)
	{
		printf(" get dst y addr error!\r\n");
		return -1;
	}
	memset((AR_CHAR *)pvYDstVa,128,u32YDstSize);
	pstDspImg->astDstChannels[0].u64VirtAddr= (AR_UINTPTR)pvYDstVa;
	pstDspImg->astDstChannels[0].u64PhyAddr= (AR_UINTPTR)u64YDstPa;

	if(pstDspImg->u32ChannelNum ==3)
    {

		/*******************DST U******************************/
		int u32UDstSize = pstDspImg->astSrcChannels[1].u32Stride*pstDspImg->astSrcChannels[1].u32Height; 
		AR_U64 u64UDstPa = 0;
		void * pvUDstVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64UDstPa, &pvUDstVa, "UDst", NULL, u32UDstSize);
		if(s32Ret)
		{
			printf(" get dst u addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvUDstVa,128,u32USrcSize);
		pstDspImg->astDstChannels[1].u64VirtAddr= (AR_UINTPTR)pvUDstVa;
		pstDspImg->astDstChannels[1].u64PhyAddr= (AR_UINTPTR)u64UDstPa;

		/*******************DST V******************************/
		AR_U64 u64VDstPa = 0;
		void * pvVDstVa = NULL;
		s32Ret = ar_hal_sys_mmz_alloc(&u64VDstPa, &pvVDstVa, "VDst", NULL, u32UDstSize);
		if(s32Ret)
		{
			printf(" get dst_v addr error!\r\n");
			return -1;
		}
		memset((AR_CHAR *)pvVDstVa,128,u32USrcSize);
		pstDspImg->astDstChannels[2].u64VirtAddr= (AR_UINTPTR)pvVDstVa;
		pstDspImg->astDstChannels[2].u64PhyAddr= (AR_UINTPTR)u64VDstPa;
	}
	return 0;

}

static AR_S32 SAMPLE_DSP_MMZ_Buffer_Release(AR_DSP_IMG_S *pstDspImg)
{
    if(!pstDspImg)
    {
    	printf("pstDspImg is NULL\r\n");
		return -1;
    }

	if(pstDspImg->astSrcChannels[0].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[0].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[0].u64VirtAddr);

	if(pstDspImg->astSrcChannels[1].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[1].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[1].u64VirtAddr);

	if(pstDspImg->astSrcChannels[2].u64VirtAddr)
   	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astSrcChannels[2].u64PhyAddr, (AR_VOID *)pstDspImg->astSrcChannels[2].u64VirtAddr);

	if(pstDspImg->astDstChannels[0].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[0].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[0].u64VirtAddr);

	if(pstDspImg->astDstChannels[1].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[1].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[1].u64VirtAddr);

	if(pstDspImg->astDstChannels[2].u64VirtAddr)
	   ar_hal_sys_mmz_free((AR_U64)pstDspImg->astDstChannels[2].u64PhyAddr, (AR_VOID *)pstDspImg->astDstChannels[2].u64VirtAddr);

	return 0;
}

static AR_S32 AR_Load_File(AR_CHAR * path, AR_CHAR * buf)
{
	FILE * pFile = 0;
	AR_S32 file_len;
	AR_S32 ret = 0;

	pFile = fopen(path, "rb+");
	if (!pFile)
	{
		printf("Open file %s error: %s\r\n", path, strerror(errno));
		return -1;
	}


	fseek(pFile, 0, SEEK_END);
	file_len = ftell(pFile);
	rewind(pFile);


	ret = fread(buf, 1, file_len, pFile);
	if (ret < 0)
	{
		printf("Read file %s size %d error: %s\r\n", path, file_len, strerror(errno));
		fclose(pFile);
		return ret;
	}

	fclose(pFile);

	return ret;
}

static AR_S32 SAMPLE_DSP_Load_Src_Img(AR_DSP_IMG_S *pstDspImg, AR_CHAR*s8SrcFileName)
{
    if(!pstDspImg)
    {
    	printf("pstDspImg  is NULL\r\n");
		return -1;
    }

	AR_CHAR *pu8FileBuf = (AR_CHAR *)malloc(SAMPLE_GAUSS_MMZ_BUFFER_MALLOC_SIZE_MAX);
	if(!pu8FileBuf)
	{
		printf("malloc pu8FileBuf fail\r\n");
		return -1;
	}

	AR_S32 s32Ret = AR_Load_File(s8SrcFileName, pu8FileBuf);
	if(s32Ret < 0)
	{
		printf("AR_Load_File fail\r\n");
		return -1;
	}

	AR_CHAR *pOrg = pu8FileBuf;

	AR_CHAR * pcYSrcVa = (AR_CHAR *)pstDspImg->astSrcChannels[0].u64VirtAddr;
	for (AR_S32 y=0;y<pstDspImg->astSrcChannels[0].u32Height;y++) {
		for(AR_S32 x=0;x<pstDspImg->astSrcChannels[0].u32Width; x++) {
			pcYSrcVa[x+1+(pstDspImg->astSrcChannels[0].u32Stride+2)*(y+1)]= pOrg[x+pstDspImg->astSrcChannels[0].u32Stride*y];
		}
	}

    if(pstDspImg->u32ChannelNum ==3)
    {
        pOrg = pOrg +  pstDspImg->astSrcChannels[0].u32Height*pstDspImg->astSrcChannels[0].u32Stride; 
		AR_CHAR * pcUSrcVa = (AR_CHAR *)pstDspImg->astSrcChannels[1].u64VirtAddr;
		for (AR_S32 y=0;y<pstDspImg->astSrcChannels[1].u32Height;y++) {
			for(AR_S32 x=0;x<pstDspImg->astSrcChannels[1].u32Width; x++) {
				pcUSrcVa[x+1+(pstDspImg->astSrcChannels[1].u32Stride+2)*(y+1)]= pOrg[x+pstDspImg->astSrcChannels[1].u32Stride*y];
			}
		}

		pOrg = pOrg +  pstDspImg->astSrcChannels[1].u32Height*pstDspImg->astSrcChannels[1].u32Stride; 
		AR_CHAR * pcVSrcVa = (AR_CHAR *)pstDspImg->astSrcChannels[2].u64VirtAddr;
		for (AR_S32 y=0;y<pstDspImg->astSrcChannels[2].u32Height;y++) {
			for(AR_S32 x=0;x<pstDspImg->astSrcChannels[2].u32Width; x++) {
				pcVSrcVa[x+1+(pstDspImg->astSrcChannels[2].u32Stride+2)*(y+1)]= pOrg[x+pstDspImg->astSrcChannels[2].u32Stride*y];
			}
		}

	}

    free(pu8FileBuf);
	return 0;

}



static AR_S32 SAMPLE_DSP_Store_Dst_Img(AR_DSP_IMG_S *pstDspImg, AR_CHAR*s8DstFileName)
{
    if((!pstDspImg) || (!s8DstFileName))
    {
    	printf("pstDspImg || s8DstFileName is NULL\r\n");
		return -1;
    }

	FILE* pDstFileFp =0;
	pDstFileFp = fopen(s8DstFileName, "w");
	if(!pDstFileFp)
	{
		printf("open file fail!!\n");
		return -1;
	}


	AR_S32 s32DstHeight = pstDspImg->astDstChannels[0].u32Height;
	AR_S32 s32DstLumaStride = pstDspImg->astDstChannels[0].u32Stride;
	AR_S32 s32DstChromaStride = pstDspImg->astDstChannels[1].u32Stride;

	AR_UCHAR *pu8YDstVa = (AR_UCHAR *)pstDspImg->astDstChannels[0].u64VirtAddr;
	AR_UCHAR *pu8UDstVa = (AR_UCHAR *)pstDspImg->astDstChannels[1].u64VirtAddr;
	AR_UCHAR *pu8VDstVa = (AR_UCHAR *)pstDspImg->astDstChannels[2].u64VirtAddr;

	AR_S32 s32Ret =-1;
	AR_S32 s32DataLenDst=0;
	AR_S32 s32PlanarDst=0;
	for(s32PlanarDst = 0; s32PlanarDst < pstDspImg->u32ChannelNum; s32PlanarDst++)
	{
	   if(s32PlanarDst == 0)
	   {
			s32DataLenDst = s32DstLumaStride*s32DstHeight;
			s32Ret = fwrite(pu8YDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
	   }
	   else if(s32PlanarDst == 1)
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8UDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
			   printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			   return -1;
			}
	   }
	   else
	   {
		    s32DataLenDst = s32DstChromaStride*s32DstHeight/2;
			s32Ret = fwrite(pu8VDstVa,1,s32DataLenDst,pDstFileFp);
			printf("STORE planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
			if(s32Ret != s32DataLenDst)
			{
				printf("planar:%d data_len:%d  ret_len:%d line:%d\n", s32PlanarDst, s32DataLenDst, s32Ret, __LINE__);
				return -1;
			}
	   }

    }

	fclose(pDstFileFp);
    pDstFileFp=0;

	return 0;

}


static AR_S32 gu32QuitFlag = 0;
void SignalHandler(AR_S32 sig)
{
    printf("Get signal %d\r\n", sig);
    gu32QuitFlag = 1;
}



AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
    AR_S32 s32Ret = 0;
/*********************Args Parser***********************/

    AR_S32 s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-srcfile: src file name\n");
		printf("-dstfile: dst file name\n");
        printf("-w: src img width\n");
        printf("-h: src img height\n");
		printf("-s: src img stride\n");
		printf("-f: src img format: (0:YUV420P, 1:YUV400P,  other format can be added similarly ...)\n");
        return 0;
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

	AR_S32 s32SrcFormat = 0;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-f",argc,argv);
	if(s32Index>0)
	{
		s32SrcFormat=atoi(argv[s32Index+1]);
		printf("s32SrcFormat=%d \r\n",s32SrcFormat);
		if(s32SrcFormat >1 || s32SrcFormat <0)
		{
			printf("s32SrcFormat error, only sopport yuv400 & yuv420p now \r\n");
			return -1;
		}	
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

	AR_S32 s32SrcStride = 1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-s",argc,argv);
	if(s32Index>0)
	{
		s32SrcStride=atoi(argv[s32Index+1]);
		printf("s32SrcStride=%d \r\n",s32SrcStride);
		if(s32SrcStride >4096 || s32SrcStride <0)
		{
			printf("s32SrcStride error \r\n");
			return -1;
		}
	}

    AR_DSP_IMG_S stDspImg = {};
    if(s32SrcFormat ==0)  //yuv420p
    {
         stDspImg.u32ChannelNum = 3; 
		 stDspImg.astSrcChannels[0].u32Width = s32SrcWidth;
		 stDspImg.astSrcChannels[0].u32Height = s32SrcHeight;
		 stDspImg.astSrcChannels[0].u32Stride = s32SrcStride;
		 stDspImg.astSrcChannels[1].u32Width = s32SrcWidth/2;
		 stDspImg.astSrcChannels[1].u32Height = s32SrcHeight/2;
		 stDspImg.astSrcChannels[1].u32Stride = s32SrcStride/2;
		 stDspImg.astSrcChannels[2].u32Width = s32SrcWidth/2;
		 stDspImg.astSrcChannels[2].u32Height = s32SrcHeight/2;
		 stDspImg.astSrcChannels[2].u32Stride = s32SrcStride/2;
		 memcpy(stDspImg.astDstChannels,stDspImg.astSrcChannels,sizeof(AR_DSP_IMG_CHANNEL_S)*4);
	}
	else   //yuv400p
	{
	     stDspImg.u32ChannelNum = 1;
		 stDspImg.astSrcChannels[0].u32Width = s32SrcWidth;
		 stDspImg.astSrcChannels[0].u32Height = s32SrcHeight;
		 stDspImg.astSrcChannels[0].u32Stride = s32SrcStride;
		 memcpy(stDspImg.astDstChannels,stDspImg.astSrcChannels,sizeof(AR_DSP_IMG_CHANNEL_S)*4);
	}
	
	s32Ret = SAMPLE_DSP_MMZ_Buffer_Request(&stDspImg);
	if(s32Ret < 0)
	{
		goto exit_handler;
		printf("AR_MPI_DSP_RemoteCall fail\r\n");
	}

	s32Ret = SAMPLE_DSP_Load_Src_Img(&stDspImg, s8SrcFileName);
	if(s32Ret < 0)
	{
		goto exit_handler;
		printf("AR_MPI_DSP_RemoteCall fail\r\n");
	}

    AR_S32 u32Loop =0;
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);
	while(1)
	{
		if(gu32QuitFlag)
			break;

		AR_DSP_TASK_S stTask = {0};
    	strncpy((AR_CHAR *)stTask.s8Name, "ArArmCalcGauss3x3", 31);
    	stTask.u32Priority = 0;
    	stTask.u64MsgBodyAddr = (AR_U64)&stDspImg;
    	stTask.u32MsgLen = sizeof(stDspImg);

		struct timeval tm;
        gettimeofday(&tm, NULL);
        AR_U64 u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;

		s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 1);
	    if(s32Ret < 0)
	    {
	    	goto exit_handler;
			printf("AR_MPI_DSP_RemoteCall fail\r\n");
		}
	    gettimeofday(&tm, NULL);
        AR_U64 u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;

		printf("gauss taskes %f ms\r\n",(u64Toc-u64Tic)*1.0/1000);

		SAMPLE_DSP_Store_Dst_Img(&stDspImg,s8DstFileName);

		break;
	}

exit_handler:

    SAMPLE_DSP_MMZ_Buffer_Release(&stDspImg);

	printf("test ceva gauss3x3 finished\r\n");

    return 0;
}
