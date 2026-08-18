#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include "mpi_npu_api.h"

#ifdef __cplusplus
extern "C"{
#endif


extern AR_U32 u32GSramAddrPhy ;
extern AR_U32 u32GSramSize ;

static AR_S32 SramAddrMmap(AR_U64 *pu64SramAddrVirt, AR_U32 u32SramAddrPhy, AR_U32 u32SramSize)
{
    AR_S32 s32Fd = 0;
    void * pMap = NULL;

    AR_U32 u32SramMapSize = (u32SramSize / 4096 + 1) * 4096;
  
    s32Fd = open("/dev/mem", O_RDWR | O_SYNC);
    if(s32Fd < 0)
    {
        printf("Open dev/mem failed!\r\n");
        return -1;
    }

    pMap = mmap(NULL, u32SramMapSize, PROT_READ | PROT_WRITE, MAP_SHARED, s32Fd, u32SramAddrPhy);
    if(!pMap)
    {
        printf("Mmap failed: %s!\r\n", strerror(errno));
        return -1;
    }
    *pu64SramAddrVirt = (AR_U64)pMap;

    close(s32Fd);
    return 0;
}


static AR_S32 SramAddrunMmap(AR_U64 u64SramAddrVirt,AR_U32 u32SramSize)
{
    AR_S32 s32Fd = 0;
    void * pMap = NULL;

    AR_U32 u32SramMapSize = (u32SramSize / 4096 + 1) * 4096;
  
    munmap((AR_CHAR *)u64SramAddrVirt, u32SramMapSize);

    return 0;
}

AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer)
{
    AR_U32 u32RuntimeSize = 0;
    AR_UINTPTR pchRuntimeAddrAligned = 0;
	AR_UINTPTR pchRuntimeAddrAlignedOffset = 0;
    AR_U32 pchRuntimeAddrAlignedPhy = 0;
    FILE * fp;
    AR_CHAR name[128] = {0};
	AR_U64 u64SramAddrVirt =0;
	AR_S32 ret= 0;

    //dump sram runtime && ddr runtime
	if(u32GSramAddrPhy && u32GSramSize)
	{
		ret= SramAddrMmap(&u64SramAddrVirt, u32GSramAddrPhy, u32GSramSize);
		if(ret < 0)
		{
		    printf("SramAddrMmap error.\n");
			return -1;
		}
	}

    AR_MPI_NPU_GetRuntimeBuffer(handle, &pchRuntimeAddrAligned, &pchRuntimeAddrAlignedPhy, &u32RuntimeSize);
    pchRuntimeAddrAlignedOffset = pchRuntimeAddrAligned + u32GSramSize;


	sprintf(name, "%s/%d.runtime", AR_MPI_NPU_GetDumpPath(handle), u32CurrLayer+1);
    fp = fopen(name, "w+");
    if(!fp)
    {
        printf("Open runtime file error.\n");
        return -1;
    }

	if(u32GSramAddrPhy && u32GSramSize)
	{
		fwrite((char *)u64SramAddrVirt, 1, u32GSramSize, fp);
	}
    fwrite((char *)pchRuntimeAddrAlignedOffset, 1, u32RuntimeSize-u32GSramSize, fp);
    fflush(fp);
    fclose(fp);

	if(u32GSramAddrPhy && u32GSramSize)
	{
		SramAddrunMmap(u64SramAddrVirt, u32GSramSize);
	}	
    printf("Dump runtime data to %s finish.\n", name);



	//dump cbuff
    sprintf(name, "%s/%d.cbuff",  AR_MPI_NPU_GetDumpPath(handle), u32CurrLayer+1);

    //ar_hal_npu_dump_cbuffer(name);
    AR_MPI_NPU_DumpcBuffer(name);
    printf("Dump cbuff data to %s finish.\n", name);

	//for debug convenience
    if(0)
    {
        printf("Stop at %d layer\n", u32CurrLayer);
        while(getchar() != 'g') usleep(1000 * 500);
        printf("Go on...\n");
    }

    return 0;
}

AR_S32 DumpIFCOutput(void * handle, void * pIfcOutput)
{
    AR_U32 u32Size = 0;
    FILE * fp;
    AR_CHAR name[128] = {0};

    u32Size = AR_MPI_NPU_GetInputBuffSize(handle);

    sprintf(name, "%s/ifc_output.bin", AR_MPI_NPU_GetDumpPath(handle));
    fp = fopen(name, "w+");
    if(!fp)
    {
        printf("Open runtime file error.\n");
        return -1;
    }

    fwrite((char *)pIfcOutput, 1, u32Size, fp);
    fflush(fp);
    fclose(fp);
    printf("Dump IFC output data to %s finish.\n", name);

    return 0;
}

#ifdef __cplusplus
}
#endif