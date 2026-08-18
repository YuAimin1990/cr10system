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
#include <hal_sys.h>
//#include "ar_ion.h"
#include "mpi_dsp_api.h"


#define SAMPLE_DDMA_MMZ_BUFFER_MALLOC_SIZE_MAX (4096*2160)
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

AR_S32 AR_DSP_BootUp(AR_CHAR * s8DspPath)
{
       AR_S32 s32Ret = 0;
       
       AR_MPI_DSP_PowerOn(0);
	s32Ret = AR_MPI_DSP_LoadBin(0, s8DspPath);
	if(s32Ret < 0)
	{
		printf("dsp load bin fail!\n");
	}
       AR_MPI_DSP_EnableCore(0);
	printf("dsp load bin success!\r\n");

       return s32Ret;
}

AR_S32 AR_DSP_CallCmp(AR_DSP_TASK_S stTask, AR_CHAR * ps8SrcAddr, AR_CHAR * ps8DstAddr, AR_CHAR * ps8DstFile)
{
    AR_S32 s32Ret = 0;
    
    printf("ddma test : name = %s\n", stTask.s8Name);
    s32Ret = AR_MPI_DSP_RemoteCall(0, &stTask, 0);
    if(s32Ret < 0)
    {
        printf("AR_MPI_DSP_RemoteCall fail\r\n");
        return -1;
    }
    sleep(2);
    if(memcmp(ps8DstAddr, ps8SrcAddr, 64*1024)) //only check 64K
    {
        printf("input file cmp with output file is not same\n");
        return -1;
    }
 #if 0
 	FILE * fp = NULL;
    fp = fopen(ps8DstFile, "wb+");
    if(!fp)
    {
        printf("Open file %s failed.\n", ps8DstFile);
        return -1;
    }

    s32Ret = fwrite((char *)ps8DstAddr, 1, 256*1024, fp);
    if(s32Ret != 256*1024)
    {
        printf("write file  failed \n");
        fclose(fp);
        return -1;
    }
    fflush(fp);
    fclose(fp);
 #endif   
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
            printf("-dsp: dsp bin file path\n");
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

       AR_CHAR s8CSVFileName[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-dstfile",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8CSVFileName,argv[s32Index+1]);
		printf("s8DstFileName=%s \r\n",s8CSVFileName);
	}
	else{
		printf(" NO Dst file defined\r\n");
		return -1;
	}

       AR_CHAR s8DspBinPath[64] = {};
	s32Index=SAMPLE_Find_Pra_Index_By_Name("-dsp",argc,argv);
	if(s32Index>0)
	{
		strcpy(s8DspBinPath,argv[s32Index+1]);
		printf("s8DspBinPath=%s \r\n",s8DspBinPath);
	}
	else{
		printf(" NO dsp bin file defined\r\n");
		return -1;
	}

       

        /*********************MMZ Malloc***********************/

	AR_U32 u32SrcSize = SAMPLE_DDMA_MMZ_BUFFER_MALLOC_SIZE_MAX;
	AR_U32 u32DstSize = SAMPLE_DDMA_MMZ_BUFFER_MALLOC_SIZE_MAX;

       AR_U64 u64SrcPa = 0;
       void *pvSrcVa = NULL;
       AR_DSP_Malloc(&u64SrcPa, &pvSrcVa, "input", u32SrcSize);
       memset(pvSrcVa, 0, u32SrcSize);

       AR_U64 u64DstPa = 0;
       void *pvDstVa = NULL;
       AR_DSP_Malloc(&u64DstPa, &pvDstVa, "output", u32DstSize);
       memset(pvDstVa, 0, u32DstSize);

	/*********************Load SRC Img***********************/
	s32Ret = AR_Load_File(s8SrcFileName, pvSrcVa);
	if(s32Ret < 0)
	{
		goto exit_handler;
		printf("SAMPLE_Load_Src_Img fail\r\n");

	}	      

       AR_S32 s32Loop =0;
       signal(SIGTERM, SignalHandler);
       signal(SIGINT, SignalHandler);
       AR_U32 u32Frequency = 100;
       AR_CHAR s8DstFileName[32];

       AR_U32 au32Msg[3] = {0};
       au32Msg[0] = u64SrcPa;
       au32Msg[1] = u64DstPa;

       AR_DSP_TASK_S stTask = {0};
       stTask.u32Priority = 0;
       stTask.u64MsgBodyAddr = (AR_U64)au32Msg;
       stTask.u32MsgLen = sizeof(au32Msg);

       AR_DSP_TASK_ACK_S stAckTask = {0};
       stAckTask.u32Priority = 0;
       stAckTask.u64MsgBodyAddr = (AR_U64)au32Msg;
       stAckTask.u32MsgLen = sizeof(au32Msg);

       FILE *pDstFp = NULL;
       FILE *pDstSpeedFp = NULL;
       pDstFp = fopen(s8CSVFileName, "w+");

       AR_CHAR pcDstName[32] = {0};
       sprintf(pcDstName, "speed_%s", s8CSVFileName);
       pDstSpeedFp = fopen(pcDstName, "w+");
       AR_CHAR * title = "frequency/bank, 1d(DDR->DTCM), 1d_Out(DTCM->DDR), 2d(DDR->DTCM), (DTCM->DDR)\n";
       fwrite(title, strlen(title), 1, pDstFp);
       fwrite(title, strlen(title), 1, pDstSpeedFp);
       
       AR_U32 u32DataFill[5] = {0};
	for(s32Loop = 0; s32Loop < 10; s32Loop++)
	{
		if(gu32QuitFlag)
		    break;

              AR_DSP_BootUp(s8DspBinPath);
              AR_MPI_DSP_SetFrequency(0, u32Frequency);
              printf("u32Frequency = %d\n", u32Frequency);
              u32DataFill[0] = u32Frequency;
		/*********************AR_CEVA_DDMA_TEST***********************/      
              //DDMA 1D load
              strncpy((AR_CHAR *)stTask.s8Name, "ArArmDdma1DLoadTest", 31);           	    	
              sprintf(s8DstFileName, "ArArmDdma1DLoadTest_%d", u32Frequency);
              s32Ret = AR_DSP_CallCmp(stTask, pvSrcVa, pvDstVa, s8DstFileName);
              if(s32Ret < 0)
              {
                    printf("AR_DSP_CallStore ArArmDdma1DLoadTest failed\n");
                    goto exit_handler;
              }

              strncpy((AR_CHAR *)stAckTask.s8Name, "ArArmDdma1DLoadTest", 31); 
              s32Ret = AR_MPI_DSP_Query(0, &stAckTask, 0);
              if(s32Ret < 0)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma1DLoadTest failed\n");
                    goto exit_handler;
              }
              if(stAckTask.s32ErrCode)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma1DLoadTest with error code = %d\n", stAckTask.s32ErrCode);
                    goto exit_handler;
              }

              u32DataFill[1] = stAckTask.u32Time;
              printf("ArArmDdma1DLoadTest with cycle time = %d\n", stAckTask.u32Time);
            
              //DDMA 1D Store
              strncpy((AR_CHAR *)stTask.s8Name, "ArArmDdma1DStoreTest", 31);           
              sprintf(s8DstFileName, "ArArmDdma1DStoreTest_%d", u32Frequency);
              s32Ret = AR_DSP_CallCmp(stTask, pvSrcVa, pvDstVa, s8DstFileName);
              if(s32Ret < 0)
              {
                    printf("AR_DSP_CallStore ArArmDdma1DStoreTest failed\n");
                    goto exit_handler;
              }

              strncpy((AR_CHAR *)stAckTask.s8Name, "ArArmDdma1DStoreTest", 31); 
              s32Ret = AR_MPI_DSP_Query(0, &stAckTask, 0);
              if(s32Ret < 0)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma1DStoreTest failed\n");
                    goto exit_handler;
              }
              if(stAckTask.s32ErrCode)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma1DStoreTest with error code = %d\n", stAckTask.s32ErrCode);
                    goto exit_handler;
              }

               u32DataFill[2] = stAckTask.u32Time;
              printf("ArArmDdma1DStoreTest with cycle time = %d\n", stAckTask.u32Time);

              //DDMA 2D Load
              strncpy((AR_CHAR *)stTask.s8Name, "ArArmDdma2DLoadTest", 31);           
              sprintf(s8DstFileName, "ArArmDdma2DLoadTest_%d", u32Frequency);
              s32Ret = AR_DSP_CallCmp(stTask, pvSrcVa, pvDstVa, s8DstFileName);
              if(s32Ret < 0)
              {
                    printf("AR_DSP_CallStore ArArmDdma2DLoadTest failed\n");
                    goto exit_handler;
              }

              strncpy((AR_CHAR *)stAckTask.s8Name, "ArArmDdma2DLoadTest", 31); 
              s32Ret = AR_MPI_DSP_Query(0, &stAckTask, 0);
              if(s32Ret < 0)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma2DLoadTest failed\n");
                    goto exit_handler;
              }
              if(stAckTask.s32ErrCode)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma2DLoadTest with error code = %d\n", stAckTask.s32ErrCode);
                    goto exit_handler;
              }
              u32DataFill[3] = stAckTask.u32Time;
              printf("ArArmDdma2DLoadTest with cycle time = %d\n", stAckTask.u32Time);
              //DDMA 2D Store
              strncpy((AR_CHAR *)stTask.s8Name, "ArArmDdma2DStoreTest", 31);           
              sprintf(s8DstFileName, "ArArmDdma2DStoreTest_%d", u32Frequency);
              s32Ret = AR_DSP_CallCmp(stTask, pvSrcVa, pvDstVa, s8DstFileName);
              if(s32Ret < 0)
              {
                    printf("AR_DSP_CallStore ArArmDdma2DStoreTest failed\n");
                    goto exit_handler;
              }

              strncpy((AR_CHAR *)stAckTask.s8Name, "ArArmDdma2DStoreTest", 31); 
              s32Ret = AR_MPI_DSP_Query(0, &stAckTask, 0);
              if(s32Ret < 0)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma2DStoreTest failed\n");
                    goto exit_handler;
              }
              if(stAckTask.s32ErrCode)
              {
                    printf("AR_MPI_DSP_Query ArArmDdma2DStoreTest with error code = %d\n", stAckTask.s32ErrCode);
                    goto exit_handler;
              }
              u32DataFill[4] = stAckTask.u32Time;
              printf("ArArmDdma2DStoreTest with cycle time = %d\n", stAckTask.u32Time);

              AR_CHAR data[128] = {0};
              sprintf(data, "%d MHz, %.1f K, %.1f K, %.1f K, %.1f K\n", u32DataFill[0], (AR_FLOAT)u32DataFill[1]/1000, (AR_FLOAT)u32DataFill[2]/1000, (AR_FLOAT)u32DataFill[3]/1000, (AR_FLOAT)u32DataFill[4]/1000);
              fwrite(data, strlen(data), 1, pDstFp);

              sprintf(data, "%d MHz, %.1f GByte/s, %.1f GByte/s, %.1f GByte/s, %.1f GByte/s\n", u32DataFill[0], (AR_FLOAT)256/((AR_FLOAT)u32DataFill[1]/1000) * ((AR_FLOAT)u32Frequency/1000),
                            (AR_FLOAT)256/((AR_FLOAT)u32DataFill[2]/1000) * ((AR_FLOAT)u32Frequency/1000), (AR_FLOAT)256/((AR_FLOAT)u32DataFill[3]/1000) * ((AR_FLOAT)u32Frequency/1000), (AR_FLOAT)256/((AR_FLOAT)u32DataFill[4]/1000) * ((AR_FLOAT)u32Frequency/1000));
              fwrite(data, strlen(data), 1, pDstSpeedFp);
              u32Frequency += 100;

              //for most case, not over 700M
              if(u32Frequency >= 800)
              {
                  break;
              }
	}

exit_handler:

	/*********************Release MMZ Buffer***********************/
       fclose(pDstFp);
       fclose(pDstSpeedFp);
       if(pvSrcVa)
           AR_DSP_Free(u64SrcPa, pvSrcVa);
       if(pvDstVa)
           AR_DSP_Free(u64DstPa, pvDstVa);

	printf("test ceva ddma finished\r\n");
       return 0;
}
