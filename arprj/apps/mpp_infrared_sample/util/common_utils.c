#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include "common_utils.h"

#include "hal_sys.h"
#include "hal_vin_log.h"
#include "ar_math.h"
#include "mpi_vb.h"

int64_t getTickCount()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return (int64_t)tp.tv_sec*1000000000 + tp.tv_nsec;
}

int64_t getTickInterval(int64_t begin)
{
    int64_t end = getTickCount();
    return end - begin;
}

void time_statistics_begin(STRU_TIME_STATISTICS_T *pTs)
{
    memset(pTs,0,sizeof(STRU_TIME_STATISTICS_T));
    pTs->begin = getTickCount();
}

void time_statistics_end(STRU_TIME_STATISTICS_T *pTs, int print_log, const char *msg)
{
    pTs->interval = getTickInterval(pTs->begin);
    if(print_log) printf("%s time: %.2f ms.\n", msg, pTs->interval/1000000.0f);
}


void dump_to_file(const char* file_path, const void* data, int len)
{
	FILE* fp = fopen(file_path, "w");
	if (fp == NULL)
	{
	    printf("open file %s error\n", file_path);
	    return;
	}
	fwrite(data, len, 1, fp);
	fclose(fp);
	printf("save to file %s success\n", file_path);
}

static int callSystem(const char* command)
{
	int status, ret = 0;
	pid_t pid;
	pid = vfork();
	if (pid < 0) {
		printf("vfork failed\n");
		return -1;
	} else if (pid == 0) {
		ret = execl("/bin/sh", "sh", "-c", command, (char *)0);
		if (ret < 0) {
			printf("execl failed\n");
			return -1;
		}
		return ret;
	} else {
		ret = waitpid(pid, &status, 0);
		if (ret != pid) {
			printf("waitpid failed, ret = %d\n", ret);
			return -1;
		}

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				printf("exit status = %d\n", WEXITSTATUS(status));
				return -1;
			}
		} else {
			printf("abort exceptly\n");
			return -1;
		}
	}
	return 0;
}

int doSystem(const char *fmt, ...)
{
	char cmd_buf[1024];
	int ret;
	va_list args;

	va_start(args, fmt);
	vsprintf(cmd_buf, fmt, args);
	va_end(args);

    //	strcat(cmd_buf, " &2 > 1");
	ret = callSystem(cmd_buf);
    //printf("cmd: %s,ret=%d\n", cmd_buf, ret);
	return ret;
}

//******************************************************************
//buffer utils
//******************************************************************
AR_S32 init_common_vb_buffer(COMMON_VB_BUFFER_T* vb_buff)
{
    if(vb_buff->u32Size<=0){
        return -1;
    }
    vb_buff->vb_blk=AR_MPI_VB_GetBlock(VB_INVALID_POOLID,vb_buff->u32Size,NULL);
    if(vb_buff->vb_blk==VB_INVALID_HANDLE)
    {
        printf("AR_MPI_VB_GetBlock failed\n");
        return -1;
    }
    AR_MPI_VB_MmapPool(AR_MPI_VB_Handle2PoolId(vb_buff->vb_blk));
    vb_buff->u64PhyAddr=AR_MPI_VB_Handle2PhysAddr(vb_buff->vb_blk);
    vb_buff->pViraddr=NULL;
    AR_MPI_VB_GetBlockVirAddr(AR_MPI_VB_Handle2PoolId(vb_buff->vb_blk),vb_buff->u64PhyAddr,&vb_buff->pViraddr);
    //SAMPLE_PRT("vb_blk=%u phy_addr=%p  p_vaddr=%p raw_size=%lu\n",vb_buff->vb_blk,(void *)vb_buff->phy_addr,vb_buff->p_vaddr,vb_buff->size);

    if(!vb_buff->u64PhyAddr || !vb_buff->pViraddr)
    {
       ar_err("vb poll not enough\n");
       return -1;
    }

    vb_buff->pViraddrAlign=(AR_VOID*)ALIGN_UP((AR_U64)vb_buff->pViraddr,256);
    vb_buff->u64PhyAddrAlign=ALIGN_UP(vb_buff->u64PhyAddr,256);

    return 0;
}

AR_S32 init_common_vb_buffer_with_size(COMMON_VB_BUFFER_T* vb_buff, AR_U32 size)
{
    vb_buff->u32Size = size;
    return init_common_vb_buffer(vb_buff);
}

AR_S32 release_common_vb_buffer(COMMON_VB_BUFFER_T* vb_buff)
{
    if(vb_buff->vb_blk!=VB_INVALID_HANDLE) AR_MPI_VB_ReleaseBlock(vb_buff->vb_blk);
    return 0;
}

AR_S32 init_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer)
{
    if(mmz_bufer->u32Size<=0){
        return -1;
    }
    return ar_hal_sys_mmz_alloc_cached(&mmz_bufer->u64PhyAddr,&mmz_bufer->pViraddr,NULL,NULL,mmz_bufer->u32Size);
}

AR_S32 release_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer)
{
    if(mmz_bufer->u32Size<=0){
        return -1;
    }
    return ar_hal_sys_mmz_free(mmz_bufer->u64PhyAddr,mmz_bufer->pViraddr);
}

AR_S32 flush_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer)
{
    if(mmz_bufer->u32Size<=0){
        return -1;
    }
    return ar_hal_sys_mmz_flush_cache(mmz_bufer->u64PhyAddr,mmz_bufer->pViraddr,mmz_bufer->u32Size);
}

AR_S32 invalid_common_mmz_buffer(COMMON_MMZ_BUFFER_T* mmz_bufer)
{
    if(mmz_bufer->u32Size<=0){
        return -1;
    }
    return ar_hal_sys_mmz_invalid_cache(mmz_bufer->u64PhyAddr,mmz_bufer->pViraddr,mmz_bufer->u32Size);
}

