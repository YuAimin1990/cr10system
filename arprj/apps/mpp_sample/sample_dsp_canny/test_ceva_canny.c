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
//#include "ar_ion.h"
#include "mpi_dsp_api.h"
#include "hal_dsp_api.h"
#include <assert.h>

#define CANNY_PADDING     (4)

AR_S32 AR_DSP_Malloc(AR_U64* u64PhyAddr, void ** pvVirtAddr, AR_CHAR* pstr_mmb, AR_U32 u32Size)
{
	AR_S32 s32Ret;
	s32Ret = ar_hal_sys_mmz_alloc(u64PhyAddr, pvVirtAddr, pstr_mmb, NULL, u32Size);

    	return s32Ret;
}

void AR_DSP_Free(AR_U64 u64_phy_addr, void* p_vir_addr)
{
    	ar_hal_sys_mmz_free(u64_phy_addr, p_vir_addr);
	u64_phy_addr = 0;
	p_vir_addr = NULL;
}

static AR_S32 SAMPLE_Find_Pra_Index_By_Name(AR_CHAR *ps8Name,AR_S32 s32Argc, AR_CHAR *ps8Argv[])
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

AR_S32 main(AR_S32 argc, AR_CHAR const *argv[])
{
	AR_S32 s32Ret = 0;
	AR_S32 s32Index = 0;

    s32Index=SAMPLE_Find_Pra_Index_By_Name("--help",argc,argv);
    if(s32Index>0 || argc==1)
    {
        printf("-input:  src file name\n");
        printf("-output: dst file name\n");
		printf("-bitdp:  bit_depth\n");
		printf("-w: src_width\n");
		printf("-h: src_height\n");
		printf("-s: src_stride\n");
		printf("-hth: high_threshold\n");
		printf("-lth: low_threshold\n");
        return 0;
    }

    AR_CHAR s8PathIn[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-input",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8PathIn,argv[s32Index+1]);
		printf("input=%s \r\n",s8PathIn);
	}
	else{
		printf(" NO input path defined\r\n");
        return -1;
	}


  	AR_CHAR s8PathOut[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-output",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8PathOut,argv[s32Index+1]);
		printf("output0=%s \r\n",s8PathOut);
	}
	else{
		printf(" NO output0 path defined\r\n");
        return -1;
	}

	AR_S32 bit_depth = 8;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-bitdp",argc,argv);
	if(s32Index>0)
	{
		bit_depth=atoi(argv[s32Index+1]);
		printf("bit_depth=%d \r\n",bit_depth);
	}

	AR_S32 src_width=1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-w",argc,argv);
	if(s32Index>0)
	{
		src_width=atoi(argv[s32Index+1]);
		printf("src_width=%d \r\n",src_width);
	}

	AR_S32 src_height=1080;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-h",argc,argv);
	if(s32Index>0)
	{
		src_height=atoi(argv[s32Index+1]);
		printf("src_height=%d \r\n",src_height);
	}

	AR_S32 src_stride=1920;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-s",argc,argv);
	if(s32Index>0)
	{
		src_stride=atoi(argv[s32Index+1]);
		printf("src_stride=%d \r\n",src_stride);
	}

	AR_S32 high_threshold=210;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-hth",argc,argv);
	if(s32Index>0)
	{
		high_threshold=atoi(argv[s32Index+1]);
		printf("high_threshold=%d \r\n",high_threshold);
	}

	AR_S32 low_threshold=180;
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-lth",argc,argv);
	if(s32Index>0)
	{
		low_threshold=atoi(argv[s32Index+1]);
		printf("low_threshold=%d \r\n",low_threshold);
	}	
  
/*******************Input*****************/
	FILE *pFile = fopen(s8PathIn, "rb");
	if(!pFile)
	{
		printf("fopen %s file failed\n", s8PathIn); 
		return -1;
	}
	fseek(pFile, 0 ,SEEK_END);
	AR_U32 u32ReadSize = ftell(pFile);
	printf("file %s read size = %d\n",s8PathIn, u32ReadSize);
	rewind(pFile);
	AR_CHAR *pu8FileBuf = (AR_CHAR *)malloc(u32ReadSize);
	
	s32Ret = fread((AR_CHAR *)pu8FileBuf, 1, u32ReadSize, pFile);
	if(s32Ret != u32ReadSize)
	{
		printf("read file %s failed \n", s8PathIn);
		return -1;
	}
	fclose(pFile);

	
    AR_U32 u32PadReadSize = (src_stride + CANNY_PADDING*2)*(src_height + CANNY_PADDING*2);
	AR_U64 u64PhyAddrIn = 0;
	void * pvVirtAddrIn = NULL; 
	AR_DSP_Malloc(&u64PhyAddrIn, &pvVirtAddrIn, "ArArmCvIn", u32PadReadSize);
	if((!pvVirtAddrIn) || (u64PhyAddrIn == 0))
	{
		printf("Malloc WarpIn failed.\n");
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrIn, 0 , u32PadReadSize);

	AR_CHAR * pcSrcVa = (AR_CHAR *)pvVirtAddrIn;
	for (AR_S32 y=0;y<src_height;y++) {
		for(AR_S32 x=0;x<src_width; x++) {
			pcSrcVa[x+CANNY_PADDING+(src_width+2*CANNY_PADDING)*(y+CANNY_PADDING)]= pu8FileBuf[x+src_width*y];
		}
	}
    free(pu8FileBuf);

    
/*********************Output **********************/
	AR_U64 u64PhyAddrOut = 0;
	void * pvVirtAddrOut = NULL;  
	AR_U32 u32WriteSize =  u32ReadSize;  
	AR_DSP_Malloc(&u64PhyAddrOut, &pvVirtAddrOut, "ArCvout", u32WriteSize);//
	if((!pvVirtAddrOut) || (u64PhyAddrOut == 0))
	{
		printf("Malloc ArCvout failed.\n");
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
		return -1;
	}
	memset((AR_CHAR *)pvVirtAddrOut, 0 , u32WriteSize);

	AR_U32 au32Msg[8] = {0}; 
	au32Msg[0] = (AR_U32)(u64PhyAddrIn & 0x0FFFFFFFF); 
 	au32Msg[1] = (AR_U32)(u64PhyAddrOut & 0x0FFFFFFFF);
	au32Msg[2] = src_width;
	au32Msg[3] = src_height;
	au32Msg[4] = src_stride;
	au32Msg[5] = bit_depth;
	au32Msg[6] = high_threshold;	
	au32Msg[7] = low_threshold;

	printf("au32Msg[0] = %x\r\n",au32Msg[0]);
	printf("au32Msg[1] = %x\r\n",au32Msg[1]);
 	printf("au32Msg[2] = %d\r\n",au32Msg[2]);
 	printf("au32Msg[3] = %d\r\n",au32Msg[3]);
 	printf("au32Msg[4] = %d\r\n",au32Msg[4]);
	printf("au32Msg[5] = %d\r\n",au32Msg[5]);
 	printf("au32Msg[6] = %d\r\n",au32Msg[6]);
 	printf("au32Msg[7] = %d\r\n",au32Msg[7]);


    AR_DSP_TASK_S stTask = {0};
	strncpy((char *)stTask.s8Name, "ArArmCvCanny", 31);
	stTask.u32Priority = 0;
	stTask.u64MsgBodyAddr = (AR_U64)au32Msg;
	stTask.u32MsgLen = sizeof(au32Msg);
  
	printf("stTask.MsgBodyAddr = %x\r\n",stTask.u64MsgBodyAddr);

	s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 1);
 	if(s32Ret < 0)
	 	printf("AR_MPI_DSP_RemoteCall return error %d\n",s32Ret); 

 /***********************Store output****************************/
 	FILE * fp = NULL;
	fp = fopen(s8PathOut, "wb+");
	if(!fp)
	{
		printf("Open file %s failed.\n", s8PathOut); 
		return -1;
	}

	s32Ret = fwrite((unsigned char*)pvVirtAddrOut, 1, u32WriteSize, fp);
	if(s32Ret != u32WriteSize)
	{
	 	 printf("write file %s failed \n", s8PathOut);
	}
	fflush(fp);
	fclose(fp); 

 /*******************************************************************/

	if(pvVirtAddrIn)
		AR_DSP_Free(u64PhyAddrIn, pvVirtAddrIn);
	if(pvVirtAddrOut)
		AR_DSP_Free(u64PhyAddrOut, pvVirtAddrOut); 

  	printf("ar arm cv finished\n");
  
	return s32Ret;
}

