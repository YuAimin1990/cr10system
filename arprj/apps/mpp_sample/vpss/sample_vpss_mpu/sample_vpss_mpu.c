#include "sample_vpss_fileio.h"
#include "inv_mpu_ioctl.h"
#include "hal_eis_algo.h"

#define DEFAULT_TESTFILE  "test.yuv"
#define DEFAULT_WIDTH     1920
#define DEFAULT_HEIGHT    1080
#define DEFAULT_FRAME_CNT 12
#define DEFAULT_ODR 8

static char testfile[128] = {0};
static AR_U32 width = 0;
static AR_U32 height = 0;
static AR_U32 frame_cnt = 0;
static AR_U32 odr = 0;
static AR_U32 blk_size = 0;
static int quit = 0;

static void usage(char* argv0)
{
    printf("Usage:\n"
			"%s -f <yuv file> width height -n frame_cnt -r odr\n", argv0);

    printf("Options:\n"
		    "    -f  <string> <int> <int>\n"
		    "            Input yuv file parameters: filename width height\n"
		    "    -n  <int>\n"
		    "            frame count to read from yuv file\n"
		    "    -r  <string>\n"
		    "			 output data rate, in Hz\n"
			"            can only be [12.5, 25, 50, 100, 200, 500, 1k, 2k, 4k, 8k, 16k, 32k]\n"
		    "    -h  <No arg>\n"
		    "            Show help message\n");
}

static const char* odr_tab[16] =
{
	"resv0", "32k", "16k", "8k", "4k", "2k", "1k", "200", "100", "50", "25", "12.5", "resv12", "resv13", "resv14", "500"
};

static const unsigned int wm_tab[16] =
{
	0, 16, 16, 16, 16, 16, 16, 1, 1, 1, 1, 1, 1, 1, 1, 8
};

static AR_U32 conv_odr(char* odrstr)
{
	int i=0;
	for(i=0; i<16; i++)
	{
		if(!strcmp(odrstr, odr_tab[i]))
		{
			return i;
		}
	}
	return DEFAULT_ODR;
}

static void parse_param(int argc, char** argv)
{
	int opt;

	strcpy(testfile, DEFAULT_TESTFILE);
	width = DEFAULT_WIDTH;
	height = DEFAULT_HEIGHT;
	frame_cnt = DEFAULT_FRAME_CNT;
	odr = DEFAULT_ODR;

	if(argc < 2)
	{
		printf("no input param, use default\n");
		printf("%s -f %s %d %d -n %d -r %d\n", argv[0], DEFAULT_TESTFILE, DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_FRAME_CNT, DEFAULT_ODR);
		return;
	}

	while ((opt=getopt(argc, argv, "f:n:r:h")) != -1)
	{
		switch (opt)
		{
			case 'f':
				strcpy(testfile, optarg);
                width = atoi(argv[optind]);
                height = atoi(argv[optind+1]);
				break;
			case 'n':
				frame_cnt = atoi(optarg);
				break;
			case 'r':
				odr = conv_odr(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
				exit(0);
		}
	}

	printf("%s done\n", __func__);
}

// only support yuv420
static int read_one_frame(FILE* fp, VIDEO_FRAME_S* frame)
{
	unsigned int row;
	unsigned char* dst;

	dst = (unsigned char* ) frame->u64VirAddr[0];
	for ( row = 0; row < frame->u32Height; row++ ) {
        if (fread( dst, 1, frame->u32Width, fp ) != frame->u32Width) {
            AR_LOG_RAW("Read frame Y failed\n");
            return -1;
        }
        dst += frame->u32Stride[0];
    }

	dst = (unsigned char* ) frame->u64VirAddr[1];
	for ( row = 0; row < frame->u32Height / 2; row++ ) {
        if (fread( dst, 1, frame->u32Width, fp ) != frame->u32Width) {
            AR_LOG_RAW("Read frame U failed\n");
            return -2;
        }
        dst += frame->u32Stride[1];
    }

	dst = (unsigned char* ) frame->u64VirAddr[2];
	for ( row = 0; row < frame->u32Height / 2; row++ ) {
        if (fread( dst, 1, frame->u32Width, fp ) != frame->u32Width) {
            AR_LOG_RAW("Read frame V failed\n");
            return -3;
        }
        dst += frame->u32Stride[1];
    }

    return 0;
}

//#define __VPSS_EIS_TEST_SIMU__

#ifdef __VPSS_EIS_TEST_SIMU__

#define VPSS_EIS_DEV_NAME			__CONCAT("/dev/", INV_MPU_DEV_NAME)
#define VPSS_EIS_DEV_FLAG			(O_CREAT | O_WRONLY)
#define VPSS_EIS_READ_INTERVAL_MS	50

static int mpu_dev_fd = -1;
static pthread_t mpu_data_producer = -1;

void* produce_mpu_data(void* arg)
{
	STRU_EIS_IMMU_DATA_T data;
	AR_S32 count = sizeof(data);

	data.x = 1.0F;
	data.y = 2.0F;
	data.z = 3.0F;
	data.x_acce = 4.0F;
	data.y_acce = 5.0F;
	data.z_acce = 6.0F;
	data.time_stamps = 0x1122334455667788ULL;

	while(!quit)
	{
		write(mpu_dev_fd, &data, count);
		usleep(VPSS_EIS_READ_INTERVAL_MS*1000);
	}

	return NULL;
}

static int init_mpu_data(void)
{
	int ret = 0;

	mpu_dev_fd = open(VPSS_EIS_DEV_NAME, VPSS_EIS_DEV_FLAG);
	if(mpu_dev_fd < 0)
	{
		AR_LOG_RAW("open %s failed %d", VPSS_EIS_DEV_NAME, mpu_dev_fd);
		return mpu_dev_fd;
	}
	ret = pthread_create(&mpu_data_producer, NULL, produce_mpu_data, NULL);
	return ret;
}
#endif

static int init_sys(void)
{
	VB_CONFIG_S stVbConf = {0};
	
	stVbConf.u32MaxPoolCnt = 1;
	
	blk_size = AR_SAMPLE_VPSS_GetPicBufferSize(width, height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
	stVbConf.astCommPool[0].u64BlkSize	= blk_size;
	stVbConf.astCommPool[0].u32BlkCnt	= 30;

   	int ret = AR_MPI_VB_SetConfig(&stVbConf);

    if (ret != 0)
    {
        AR_LOG_RAW("AR_MPI_VB_SetConf failed 0x%x\n", ret);
        return ret;
    }

	ret = ar_log_init();
    if (ret != 0)
    {
        AR_LOG_RAW("ar_log_init failed 0x%x\n", ret);
        return ret;
    }

    ret = AR_MPI_VB_Init();
    if (ret != 0)
    {
        AR_LOG_RAW("AR_MPI_VB_Init failed 0x%x\n", ret);
        return ret;
    }

#ifdef __VPSS_EIS_TEST_SIMU__
	ret = init_mpu_data();
    if (ret != 0)
    {
        AR_LOG_RAW("init_mpu_data failed 0x%x\n", ret);
        return ret;
    }
#endif

	AR_LOG_RAW("%s done\n", __func__);
	return ret;
}

static int exit_sys(void)
{
#ifdef __VPSS_EIS_TEST_SIMU__
	pthread_join(mpu_data_producer, NULL);

	if(mpu_dev_fd >= 0)
	{
		close(mpu_dev_fd);
	}
#endif

	return AR_MPI_VB_Exit();
}

static void init_video_frame(VIDEO_FRAME_INFO_S *frame)
{
	frame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	frame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
	frame->stVFrame.u32Width = width;
	frame->stVFrame.u32Height = height;
	frame->stVFrame.u32Stride[0] = ALIGN_UP(width, 64);
	frame->stVFrame.u32Stride[1] = ALIGN_UP(width, 64) >> 1;
	frame->stVFrame.u32Stride[2] = ALIGN_UP(width, 64) >> 1;
	frame->stVFrame.u32TimeRef = 0;
	frame->stVFrame.u64PTS = 0;
	frame->enModId = 10;   //AR_VB_UID_VPSS;

	AR_U64 size = blk_size;
	VB_BLK block_hdl = ar_hal_vb_get_block_ex(VB_INVALID_POOLID, size, NULL, frame->enModId);
	AR_U32 u32LumaSize = frame->stVFrame.u32Stride[0] * height;
	AR_U32 u32ChromaSize = frame->stVFrame.u32Stride[0] * height / 4;

	frame->u32PoolId = AR_MPI_VB_Handle2PoolId(block_hdl);
	frame->stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(block_hdl);
	frame->stVFrame.u64PhyAddr[1] = frame->stVFrame.u64PhyAddr[0] + u32LumaSize;
	frame->stVFrame.u64PhyAddr[2] = frame->stVFrame.u64PhyAddr[1] + u32ChromaSize;
	
	frame->stVFrame.u64VirAddr[0] = (AR_U64)AR_MPI_SYS_Mmap(frame->stVFrame.u64PhyAddr[0], blk_size);
	frame->stVFrame.u64VirAddr[1] = (AR_U64)(frame->stVFrame.u64VirAddr[0]) + u32LumaSize;
	frame->stVFrame.u64VirAddr[2] = (AR_U64)(frame->stVFrame.u64VirAddr[1]) + u32ChromaSize;

	vpss_frame_ref(frame);

	AR_LOG_RAW("%s done\n", __func__);
}

static int deinit_video_frame(VIDEO_FRAME_INFO_S *frame)
{
	int ret;

	vpss_frame_unref(frame);

	ret = AR_MPI_SYS_Munmap((AR_VOID*)(AR_U64)frame->stVFrame.u64VirAddr[0], blk_size);
	if(ret < 0)
	{
		AR_LOG_RAW("AR_MPI_SYS_Munmap failed 0x%x\n", ret);
	}

	AR_LOG_RAW("%s done\n", __func__);
	return ret;
}

static void signal_handler(int signum)
{
    AR_LOG_RAW("recive signal SIGINT\n");
    quit = 1;
}

int main(int argc, char** argv)
{
	int ret = 0;
	AR_BOOL need_deinit = AR_FALSE;

    signal(SIGINT, signal_handler);

	parse_param(argc, argv);

	ret = init_sys();
	if(ret < 0)
	{
		goto VPSS_EXIT;
	}

	STRU_EIS_TUNING_T eis_algo_tuning =
	{
		.enable = 1,
		.eis_enable = 1,
	};
	ret = vpss_eis_init(&eis_algo_tuning);
	AR_LOG_RAW("vpss_eis_init ret=%x\n", ret);
	if(ret < 0)
	{
		goto VPSS_EXIT;
	}

	ret = vpss_eis_start();
	AR_LOG_RAW("vpss_eis_start ret=%x\n", ret);
	if(ret < 0)
	{
		goto VPSS_STOP;
	}

	MPU_CONFIG_T config;
	vpss_eis_get_config(&config);

	config.odr = odr;
	config.fifo_wm = wm_tab[odr];
	ret = vpss_eis_set_config(&config);
	AR_LOG_RAW("vpss_eis_set_config ret=%x\n", ret);
	if(ret < 0)
	{
		goto VPSS_STOP;
	}

	FILE *fp = fopen(testfile, "rb");
	if(!fp)
	{
		AR_LOG_RAW("open %s failed\n", testfile);
		goto VPSS_STOP;
	}

	VIDEO_FRAME_INFO_S frame;
	// backup for unref at stop
	VIDEO_FRAME_INFO_S srcFrame;

	init_video_frame(&srcFrame);
	frame = srcFrame;

	int cnt = 0;
	while(!feof(fp) && !quit && cnt < frame_cnt)
	{
		ret = read_one_frame(fp, &frame.stVFrame);
		AR_LOG_RAW("read_one_frame ret=%x\n", ret);
		if(ret < 0)
		{
			quit = 1;
		}
		else
		{
			STRU_EIS_PROCESS_PARA para =
			{
				.s32_dev_id = 0,
				.s32_chn_id = 0,
				.u32_walign = 64,
				.u32_halign = 64
			};
			ret = vpss_eis_process_frame(&frame, &para);
			AR_LOG_RAW("vpss_eis_process_frame ret=%x\n", ret);
			if(ret < 0)
			{
				quit = 1;
			}
		}
		cnt++;
	}
	need_deinit = AR_TRUE;

	fclose(fp);
VPSS_STOP:
	ret = vpss_eis_reset();
	AR_LOG_RAW("vpss_eis_reset ret=%x\n", ret);

	ret = vpss_eis_stop();
	AR_LOG_RAW("vpss_eis_stop ret=%x\n", ret);

	ret = vpss_eis_exit();
	AR_LOG_RAW("vpss_eis_exit ret=%x\n", ret);

	if(need_deinit)
	{
		vpss_frame_unref(&srcFrame);
		deinit_video_frame(&frame);
	}

VPSS_EXIT:
	ret = exit_sys();
	AR_LOG_RAW("exit_sys ret=%x\n", ret);

	return ret;
}
