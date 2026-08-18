#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "hal_sys.h"
#include <signal.h>

#define PAGE_SIZE 4096

#define GET_PAGE_COUNT(size) (size/4096+2)

#define SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))
#define GET_REG_BITS(reg,start_bit,end_bit) (((reg)&((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))>>(start_bit))
#define __REG32__(addr) (*(volatile unsigned int *)(ar_dev_pa2va(addr)))
unsigned int g_run = 1;
static void sighandler(int signo)
{
    g_run = 0;
}

static int find_pra_index_by_name(char *name,int argc, char *argv[])
{
    int i=0;
	for(i=0;i<argc;i++)
	{
         if(!strcmp(name,argv[i]))
         {
             return i;
         }
	}
	return 0;
}

static int check_data(char *number)
{
   if(number[0]=='0'&&(number[1]=='x'||number[1]=='X'))
   {
        return 16;
   }else
   {
       return 10;
   }
}

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define	REG_ADDR_START			(0x00000000)
#define	REG_ADDR_END			(0x10000000)

#define AR_DEV_MEM          ("/dev/mem")
#define AR_MMAP_MAGIC       (0x53535353U)

#define round_boundary(value, boundary) \
    ((__typeof__(value))((boundary) - 1))

#define round_up(value, boundary)       \
    ((((value) - 1) | round_boundary(value, boundary)) + 1)

#define round_down(value, boundary)     \
    ((value) & ~round_boundary(value, boundary))

typedef unsigned long AR_PHYS_ADDR;

enum ar_mmp_cache_attr
{
    CACHEABLE       = 0,
    NON_CACHEABLE   = 1,
};

struct ar_mmp_attr
{
    int		prot;
    int		flags;
    enum	ar_mmp_cache_attr cache;
};

#define AR_MMAP_ATTR_CACHEABLE(name)                \
    struct ar_mmp_attr name = {                     \
    .prot   =   PROT_READ | PROT_WRITE,             \
    .flags  =   MAP_SHARED,                         \
    .cache = CACHEABLE,                             \
};

#define AR_MMAP_ATTR_NONCACHEABLE(name)             \
    struct ar_mmp_attr name = {                     \
    .prot   =   PROT_READ | PROT_WRITE,             \
    .flags  =   MAP_SHARED,                         \
    .cache  =   NON_CACHEABLE,                      \
};

typedef struct
{
	unsigned long start;
	unsigned long end;
	void		*va;
	void		*map_hdl;
} va_map;


static va_map ar_dev_addr_map[] = {
	{REG_ADDR_START,  REG_ADDR_END, NULL, NULL},
};


struct _ar_mmap_module_info
{
    int cache_fd;
    int noncache_fd;
};

struct _ar_mmaped_info
{
    void 			*real_start_va;
    size_t			real_sz;
    unsigned int	magic;
};

static struct _ar_mmap_module_info _mmap_inf = {
    .cache_fd		= -1,
    .noncache_fd	= -1
};

static int ar_mmap_clear(void)
{
    int ret = -1;
    if (_mmap_inf.noncache_fd > 0)
        ret = close(_mmap_inf.noncache_fd);

    if (_mmap_inf.cache_fd > 0)
        ret = close(_mmap_inf.cache_fd);

    return ret;
}

static int ar_mmap_init(void)
{
    if ((_mmap_inf.noncache_fd = open("/dev/mem", O_RDWR | O_SYNC)) <= 0) {
        printf("open noncache_fd = %d\n", _mmap_inf.noncache_fd);
        goto err_open;
    }

    if ((_mmap_inf.cache_fd = open("/dev/mem", O_RDWR)) <= 0) {
        printf("open cache_fd = %d\n", _mmap_inf.cache_fd);
        goto err_open;
    }

    return AR_SUCCESS;

err_open:

    ar_mmap_clear();
    return AR_FAILURE;
}

static void *ar_mmap(AR_PHYS_ADDR pa, size_t length, struct ar_mmp_attr attr, void **hdl)
{
    assert(_mmap_inf.cache_fd > 0 && _mmap_inf.noncache_fd > 0);

    unsigned int page_size = sysconf(_SC_PAGE_SIZE);

    off_t offset  =  round_down(pa, page_size);

    length += (pa & (page_size - 1));
    void *real_start_va = mmap(NULL, length, attr.prot, attr.flags,
                        ((attr.cache == NON_CACHEABLE) ? _mmap_inf.noncache_fd :
                                                          _mmap_inf.cache_fd),
                        offset);
    if (real_start_va != MAP_FAILED) {
        struct _ar_mmaped_info *mapped = malloc(sizeof(*mapped));
        if (mapped) {
            mapped->real_start_va	= real_start_va;
            mapped->real_sz			= length;
            mapped->magic			= AR_MMAP_MAGIC;

            *hdl = mapped;
            return (real_start_va + (pa & (page_size - 1)));

        } else {
            printf("ar_malloc fail\n");
            munmap(real_start_va, length);
        }
    }

    return NULL;
}

static int ar_munmap(void *hdl)
{
    int ret;
    struct _ar_mmaped_info *info = (struct _ar_mmaped_info *)hdl;

    if (info->magic != AR_MMAP_MAGIC) {
        printf("wrong magic: 0x%x\n", info->magic);
        return AR_FAILURE;
    }

    ret = munmap(info->real_start_va, info->real_sz);
    if (ret)
        printf("munmap fail ret=%d errono:%d\n", ret, errno);

    free(hdl);

    return ret;
}


int ar_dev_map_deinit(void);

void ar_dev_map_print(void)
{
	int i;
	for (i = 0; i< ARRAY_SIZE(ar_dev_addr_map); i++){
		printf("seciotn%d: 0x%08lx => 0x%08lx\r\n",
				i, ar_dev_addr_map[i].start, ar_dev_addr_map[i].end);
	}
}

int ar_dev_map_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar_dev_addr_map); i++) {

		AR_MMAP_ATTR_NONCACHEABLE(attr);

		ar_dev_addr_map[i].va = ar_mmap(ar_dev_addr_map[i].start,
									(ar_dev_addr_map[i].end - ar_dev_addr_map[i].start + 1),
									attr, &ar_dev_addr_map[i].map_hdl);

		if (ar_dev_addr_map[i].va == NULL)
			goto err_mmap;
	}

	return AR_SUCCESS;

err_mmap:
	ar_dev_map_deinit();

	return AR_FAILURE;
}

int ar_dev_map_deinit(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar_dev_addr_map); i++) {
		if (!ar_dev_addr_map[i].va)
			break;

		ar_munmap(ar_dev_addr_map[i].map_hdl);
		ar_dev_addr_map[i].va = NULL;
	}

	return 0;
}


void * ar_dev_pa2va(AR_PHYS_ADDR pa)
{
	for (int i = 0; i<ARRAY_SIZE(ar_dev_addr_map); i++) {
		if ((pa >= ar_dev_addr_map[i].start) && (pa <= ar_dev_addr_map[i].end))
			return ar_dev_addr_map[i].va + (pa-ar_dev_addr_map[i].start);
	}

	return NULL;
}

static pthread_t *worker_threads = NULL;
static int worker_number = 0;
static volatile char worker_watcher = 0;
static AR_U32 mmz_datasize = 512*1024;
static AR_U32 mmz_duration = 10000;

static void stop_fullspeed_dmacpy() {
    if(!worker_threads || !worker_number){
        return ;
    }
    worker_watcher = ~0;
    for(int i = 0; i < worker_number; i++){
        if(worker_threads[i]){
            pthread_join(worker_threads[i], 0);
        }
    }
    worker_number = 0;
    free(worker_threads);
    worker_threads = NULL;
}

static void* fullspeed_dmacpy_routine(void* args) {
    #define BATCH_SIZE 4
    int mmz_count = 0;
    AR_VOID *virtaddr[BATCH_SIZE] = {NULL};
    AR_U64 phyaddr[BATCH_SIZE] = {0};
    STRU_SYS_DMA_TRAN_INFO traninfo;

    for(int i = 0; i < BATCH_SIZE; i++){
        if(ar_hal_sys_mmz_alloc(&phyaddr[i],&virtaddr[i],NULL,NULL, mmz_datasize) != 0){
            printf("alloc mmz failed\n");
            break;
        }
        mmz_count++;
    }

    while(worker_watcher == 0 && mmz_count > 0){
        memset(&traninfo, 0, sizeof(traninfo));
        traninfo.s32_tran_cnt_sum = mmz_count/2;
        traninfo.e_mod = _CHAN_SHARE;
        traninfo.e_channel = _DMAC_CHANNEL_0;
        traninfo.s32_disable_cci = 1;
        for(int i = 0; i < mmz_count/2; i++){
            traninfo.st_addr_size[i].pv_src_pa = phyaddr[2*i+0];
            traninfo.st_addr_size[i].pv_dst_pa = phyaddr[2*i+1];
            traninfo.st_addr_size[i].u32_size = mmz_datasize;
        }
        ar_hal_sys_mem_transfer_pa(&traninfo);
    }
    for(int i = 0; i < mmz_count; i++){
        ar_hal_sys_mmz_free(phyaddr[i], virtaddr[i]);
    }
    return NULL;
}

static int start_fullspeed_dmacpy(int threads) {
    worker_watcher = 0;
    worker_number = threads;
    if(worker_number > 0){
        worker_threads = (pthread_t*)malloc(sizeof(pthread_t)*threads);
        memset(worker_threads, 0, sizeof(pthread_t)* threads);
        for(int i = 0; i < worker_number; i++){
            if(pthread_create(&worker_threads[i], NULL, &fullspeed_dmacpy_routine, NULL) != 0){
                printf("create thread %d failed with %d\n", i, errno);
                goto exit;
            }
        }
        sleep(1);
    }
    return 0;
exit:
    stop_fullspeed_dmacpy();
    return -1;
}



static uint32_t ddr_wr_counter = 0;
static uint32_t ddr_rd_counter = 0;

static void reset_record_monitor(int ddr_freq, int noc_freq,int duration) {
    //reset the monitor
    SET_REG_BITS(__REG32__(0x06010058),0,2,5);
    SET_REG_BITS(__REG32__(0x06010058),0,11,14);
    // ar_delay(1);
    SET_REG_BITS(__REG32__(0x06010058),0xf,2,5);
    SET_REG_BITS(__REG32__(0x06010058),0xf,11,14);

    uint32_t reg_ddr_freq = ddr_freq/4*duration;
    SET_REG_BITS(__REG32__(0x06020004),reg_ddr_freq,0,31);
    // ar_always("ddr freq reg=%u %u",reg, GET_REG_BITS(__REG32__(0x06020004),0,31));
    uint32_t reg_noc_freq = noc_freq*duration;
    SET_REG_BITS(__REG32__(0x06030004),reg_noc_freq,0,31);
    SET_REG_BITS(__REG32__(0x06040004),reg_noc_freq,0,31);
    SET_REG_BITS(__REG32__(0x06050004),reg_noc_freq,0,31);
    // ar_always("noc freq reg=%u %u",reg, GET_REG_BITS(__REG32__(0x06030004),0,31));
}

static void start_record_monitor() {
        // trigger on

		// SET_REG_BITS(__REG32__(0x06020000),0x10020,0,31);
		SET_REG_BITS(__REG32__(0x06020000),0,0,31);
		SET_REG_BITS(__REG32__(0x06030000),0,0,31);
		SET_REG_BITS(__REG32__(0x06040000),0,0,31);
		SET_REG_BITS(__REG32__(0x06050000),0,0,31);

		// SET_REG_BITS(__REG32__(0x06020000),0x10021,0,31);
		SET_REG_BITS(__REG32__(0x06020000),1,0,31);
		SET_REG_BITS(__REG32__(0x06030000),1,0,31);
		SET_REG_BITS(__REG32__(0x06040000),1,0,31);
		SET_REG_BITS(__REG32__(0x06050000),1,0,31);
}

static void stop_record_monitor() {
        SET_REG_BITS(__REG32__(0x06020000),0,0,31);
        SET_REG_BITS(__REG32__(0x06030000),0,0,31);
        SET_REG_BITS(__REG32__(0x06040000),0,0,31);
        SET_REG_BITS(__REG32__(0x06050000),0,0,31);

        uint32_t reg;
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);
	    reg=GET_REG_BITS(__REG32__(0x06020200),0,31);

		ddr_wr_counter = GET_REG_BITS(__REG32__(0x06020200),0,31);
	    //reg=GET_REG_BITS(__REG32__(0x0602011c),0,31);
        // ar_always("ddr_wr_bw=%u",reg);
		// uint32_t ddr_wr_bw_MB=reg*16/times_ms;
		ddr_rd_counter = GET_REG_BITS(__REG32__(0x06020200),0,31);
	    //reg=GET_REG_BITS(__REG32__(0x06020120),0,31);
        // ar_always("ddr_rd_bw=%u",reg);
		// uint32_t ddr_rd_bw_MB=reg*16/times_ms;


}

static void output_record_monitor(int duration, int index) {
    uint32_t ddr_wr_bw_MB=ddr_wr_counter*16/duration;
    uint32_t ddr_rd_bw_MB=ddr_rd_counter*16/duration;
    if (index != 0) {
        printf("%d: ddr_wr_bw_MB=%u ddr_rd_bw_MB=%u\n",index, ddr_wr_bw_MB,ddr_rd_bw_MB);
    }
    else {
        printf("---ddr_wr_bw_MB=%u ddr_rd_bw_MB=%u\n", ddr_wr_bw_MB,ddr_rd_bw_MB);
    }
}


int main(int argc, char **argv)
{
	int ret=0;
	int index=0;
	int val=0;
    int duration = 5000;
    int datasize = 1000*1024;
    int threads = 4;
    int ddr_freq = 3200;
    int noc_freq = 600;
    int i = 0;
    int mode = 0;
    struct sigaction        action;

	index=find_pra_index_by_name("--help",argc,argv);
	if(index>0)
    {
       printf("--help:show this help\n");
	   printf("--mode: 0(monitor), 1(test), default monitor\n");
	   printf("--duration: ddr test time, default 5000ms, eg: --duration 5000\n");
	   printf("--datasize: block size byte, default 16KB, eg: --datasize 0x8000\n");
	   printf("--threads: thread number, default 4, eg: --threads 4\n");
       printf("--ddrfreq: ddr frequency, default 3200MB, eg: --ddrfreq 3200\n");
       printf("--nocfreq: noc frequency, default 600MB, eg: --nocfreq 6000\n");
	   return -1;
    }

    action.sa_flags                 = 0;
    action.sa_handler               = sighandler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGKILL, &action, NULL);
    sigaction(SIGTERM, &action, NULL);

    index=find_pra_index_by_name("--mode",argc,argv);
	if(index>0)
	{
        mode = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
	}
    index=find_pra_index_by_name("--duration",argc,argv);
	if(index>0)
	{
        duration = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
	}
	index=find_pra_index_by_name("--datasize",argc,argv);
	if(index>0)
	{
        datasize = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
	}
    index=find_pra_index_by_name("--threads",argc,argv);
	if(index>0)
	{
        threads = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
	}
    index=find_pra_index_by_name("--ddrfreq", argc, argv);
    if(index > 0){
        ddr_freq = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
    }
    index=find_pra_index_by_name("--nocfreq", argc, argv);
    if(index > 0){
        noc_freq = strtoul(argv[index+1], NULL, check_data(argv[index+1]));
    }
    mmz_datasize = datasize;
    mmz_duration = duration;
    ar_mmap_init();
    ar_dev_map_init();
    printf("---begin ddr loading testing\n");
    uint32_t bitWidth  = GET_REG_BITS(__REG32__(0x6000000),12,12) ;
    printf("---ddr bitwidth : %d\n",bitWidth > 0 ? 16:32);
    printf("---arguments: ddrfreq %dMB, nocfreq %dMB, duration %dms, datesize %dKB, threads %d\n",
        ddr_freq, noc_freq, duration, datasize/1024, threads);
    start_fullspeed_dmacpy(threads);
    reset_record_monitor(ddr_freq, noc_freq, duration);
    start_record_monitor();
    //usleep(duration*1000);
    while(g_run) {
        usleep(duration*1000);
        i++;
        if (mode == 1) {
            break;
        }
        stop_record_monitor();
        output_record_monitor(duration,i);
        reset_record_monitor(ddr_freq, noc_freq, duration);
        start_record_monitor();
    }
    stop_record_monitor();
    stop_fullspeed_dmacpy();
    if (g_run == 1) {
        output_record_monitor(duration, 0);
    }
    printf("---end ddr loading testing\n");
    ar_dev_map_deinit();
    ar_mmap_clear();
	return 0;
}
