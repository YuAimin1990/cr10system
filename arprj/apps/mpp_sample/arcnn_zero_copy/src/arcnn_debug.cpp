#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include "mpi_npu_api.h"

#ifdef __cplusplus
extern "C"{
#endif

AR_S32 DumpLayerData(void * handle, AR_U32 u32CurrLayer)
{
    AR_U32 u32RuntimeSize = 0;
    AR_UINTPTR pchRuntimeAddrAligned = 0;
    AR_U32 pchRuntimeAddrAlignedPhy = 0;
    FILE * fp;
    AR_CHAR name[128] = {0};

    //dump runtime
    AR_MPI_NPU_GetRuntimeBuffer(handle, &pchRuntimeAddrAligned, &pchRuntimeAddrAlignedPhy, &u32RuntimeSize);
    sprintf(name, "%s/%d.runtime", AR_MPI_NPU_GetDumpPath(handle), u32CurrLayer);
    fp = fopen(name, "w+");
    if(!fp)
    {
        printf("Open runtime file error.\n");
        return -1;
    }
    fwrite((char *)pchRuntimeAddrAligned, 1, u32RuntimeSize, fp);
    fflush(fp);
    fclose(fp);
    printf("Dump runtime data to %s finish.\n", name);



	//dump cbuff
    sprintf(name, "%s/%d.cbuff",  AR_MPI_NPU_GetDumpPath(handle), u32CurrLayer);

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
    AR_U32 u32Size = 10240; //tmp compare 10K
    FILE * fp;
    AR_CHAR name[128] = {0};
    static AR_U32 u32Id = 0;

    //dump input bank to file
    sprintf(name, "%s/ifc_output_bank%d.bin", AR_MPI_NPU_GetDumpPath(handle), u32Id++);
    fp = fopen(name, "w+");
    if(!fp)
    {
        printf("Open runtime file error.\n");
        return -1;
    }
    fwrite((char *)pIfcOutput, 1, u32Size, fp);
    fflush(fp);
    fclose(fp);
    printf("Dump IFC output data from 0x%x to %s finish.\n", pIfcOutput, name);

    return 0;
}

#ifdef __cplusplus
}
#endif