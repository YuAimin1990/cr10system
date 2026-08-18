#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/prctl.h>

#include "ar_mipi_tx.h"

#include "sample_comm.h"
#include "hal_sys.h"

#define round_boundary(value, boundary)		\
		((__typeof__(value))((boundary) - 1))

#define round_up(value, boundary)		\
		((((value) - 1) | round_boundary(value, boundary)) + 1)

#define round_down(value, boundary)		\
	((value) & ~round_boundary(value, boundary))

typedef unsigned long AR_PHYS_ADDR;

enum ar_mmp_cache_attr
{
	CACHEABLE		= 0,
	NON_CACHEABLE	= 1,
};

/*
 * prot:	argument describes the desired memory protection of the mapping:
 *			PROT_EXEC,PROT_READ, PROT_WRITE, PROT_NONE
 * flags:	argument determines whether updates to the mapping are visible to other processes mapping the
 *			same region: MAP_SHARED, MAP_PRIVATE,
 * more of the attr check the mmap help
 */
struct ar_mmp_attr
{
	int 	prot;
	int 	flags;
	enum	ar_mmp_cache_attr cache;
};

#define AR_MMAP_ATTR_NONCACHEABLE(name)             \
    struct ar_mmp_attr name = {                     \
    .prot   =   PROT_READ | PROT_WRITE,             \
    .flags  =   MAP_SHARED,                         \
    .cache  =   NON_CACHEABLE,                      \
};
static int noncache_fd = -1;
void *ar_mmap2(AR_PHYS_ADDR pa, size_t length, struct ar_mmp_attr attr, void **hdl)
{
	noncache_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(noncache_fd < 0)
		return NULL;

	unsigned int page_size = sysconf(_SC_PAGE_SIZE);

	off_t offset  =  round_down(pa, page_size);

	length += (pa & (page_size - 1));
	void *real_start_va = mmap(NULL, length, attr.prot, attr.flags,
						noncache_fd,
						offset);
	if (real_start_va != MAP_FAILED) {
			return (real_start_va + (pa & (page_size - 1)));

	} else {
			printf("ar_malloc fail\n");
	}

	return NULL;
}

void debug_save_vpssframe(VIDEO_FRAME_INFO_S* frame, int save_counts)
{
	char name[64];
	memset(name, 0 ,64);
	sprintf(name, "/tmp/%d", save_counts);

	FILE * fp = fopen(name, "w");
	if (fp)
	{
		AR_U32 s32DataLenDstY = frame->stVFrame.u32Stride[0] * frame->stVFrame.u32Height;
		AR_U32 s32DataLenDstU = frame->stVFrame.u32Stride[1] * frame->stVFrame.u32Height / 2;
		AR_U32 s32DataLenDstV = frame->stVFrame.u32Stride[1] * frame->stVFrame.u32Height / 2;

		AR_U32 size = frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[0] + s32DataLenDstV;
		AR_S32 s32Ret = -1;
		void *hdl= NULL;
		AR_MMAP_ATTR_NONCACHEABLE(attr);

		void *virt_addr = ar_mmap2(frame->stVFrame.u64PhyAddr[0], size, attr, &hdl);

		printf("count:%d Height:%d Width:%d PoolId:%d u64PhyAddr[0]:%llx frame->stVFrame.u32Stride[0]:%d virt_addr:%p size:%d\n", save_counts, frame->stVFrame.u32Height, frame->stVFrame.u32Width, frame->u32PoolId, frame->stVFrame.u64PhyAddr[0], frame->stVFrame.u32Stride[0], virt_addr, size);
		if(virt_addr)
		{
			s32Ret = fwrite(virt_addr, 1, s32DataLenDstY, fp);
			if(s32Ret != s32DataLenDstY)
			{
			   printf("%s: STORE planarY data_len=%d  ret_len=%d\n", name, s32DataLenDstY, s32Ret);
			}
			printf("%s: STORE planarY data_len=%d  ret_len=%d\n", name, s32DataLenDstY, s32Ret);

			s32Ret = fwrite(virt_addr + frame->stVFrame.u64PhyAddr[1] - frame->stVFrame.u64PhyAddr[0], 1, s32DataLenDstU, fp);
			if(s32Ret != s32DataLenDstU)
			{
			   printf("%s: STORE planarU data_len=%d  ret_len=%d\n", name, s32DataLenDstU, s32Ret);
			}
			printf("%s: STORE planarU data_len=%d  ret_len=%d\n", name, s32DataLenDstU, s32Ret);

			s32Ret = fwrite(virt_addr + frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[0], 1, s32DataLenDstV, fp);
			if(s32Ret != s32DataLenDstV)
			{
			   printf("%s: STORE planarV data_len=%d  ret_len=%d\n", name, s32DataLenDstV, s32Ret);
			}
			printf("%s: STORE planarV data_len=%d  ret_len=%d\n", name, s32DataLenDstV, s32Ret);

			munmap(virt_addr - (frame->stVFrame.u64PhyAddr[0] & (sysconf(_SC_PAGE_SIZE) - 1)), size);
		}
		fclose(fp);
		if(noncache_fd >= 0)
			close(noncache_fd);
	}

}

void  SAMPLE_VPSS_CLIENT_Usage()
{
    printf("Usage : <VpssGrp> <VpssChn> <SaveCounts>\n");
}

int main(int argc, char *argv[])
{
	AR_S32			   s32Ret;
    VPSS_GRP           VpssGrp;
    VPSS_CHN           VpssChn;
	AR_S32			   s32SaveCounts;

    if (argc < 4)
    {
        SAMPLE_VPSS_CLIENT_Usage();
        return AR_FAILURE;
    }

	VpssGrp = atoi(argv[1]);
	VpssChn = atoi(argv[2]);
	s32SaveCounts = atoi(argv[3]);

	/*** test_mpp_vpss should be run first, then you can dump vpss chanel frame.****/
	VIDEO_FRAME_INFO_S pstVideoFrame;

	int count = 0;
	while(count < s32SaveCounts){
		memset(&pstVideoFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
		s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &pstVideoFrame, -1);
		if(0 == s32Ret){
			debug_save_vpssframe(&pstVideoFrame, count);
			AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &pstVideoFrame);
			count++;
		}

	}

    return 0;

}
