#include "sample_vpss_fileio.h"
#include "mpi_sysctl.h"

#define SAVE_PATH "/tmp"

AR_CHAR g_save_path[MAX_NAME_LEN] = {0};

ThreadCtrl_Info_In g_astThreadInfoIn[VPSS_MAX_GRP_NUM] = {0};
ThreadCtrl_Info_Out g_aastThreadInfoOut[VPSS_MAX_GRP_NUM][VPSS_MAX_PHY_CHN_NUM] = {0};

AR_S32 g_groupNum = 0;
AR_S32 g_channelNum = 0;

AR_BOOL g_bCaculateTime = AR_FALSE;
AR_BOOL g_bWriteOutFrame = AR_TRUE;

VPSS_GRP_ATTR_S g_astVpssGrpAttr[VPSS_MAX_GRP_NUM] = {0};
AR_BOOL g_abGrpEnable[VPSS_MAX_GRP_NUM] = {0};

VPSS_CROP_INFO_S g_astVpssGrpCrop[VPSS_MAX_GRP_NUM] = {0};

VPSS_CHN_ATTR_S g_aastVpssChnAttr[VPSS_MAX_GRP_NUM][VPSS_MAX_PHY_CHN_NUM] = {0};
VPSS_EXT_CHN_ATTR_S g_aastVpssExtChnAttr[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};     // only [VPSS_MAX_PHY_CHN_NUM, VPSS_MAX_CHN_NUM)
AR_BOOL g_aabChnEnable[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};

VPSS_CROP_INFO_S g_aastVpssChnCrop[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};

VIDEO_REGION_INFO_S g_aabChnRegionLumaVideoRgnInfo[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};

AR_U32 g_aabChnUpdateNeeded[VPSS_MAX_GRP_NUM][VPSS_MAX_CHN_NUM] = {0};

AR_BOOL g_bVpssQuit = AR_FALSE;
AR_BOOL g_bVpssStart = AR_FALSE;
AR_BOOL g_bVpssStop = AR_FALSE;
AR_BOOL g_bDebug = AR_FALSE;
AR_BOOL g_bSpec = AR_FALSE;
AR_BOOL g_bStopGetFrameThd = AR_FALSE;
AR_BOOL g_bStopSendFrameThd = AR_FALSE;
AR_BOOL g_bSysInited = AR_FALSE;
SIZE_S  g_stSrcSize;
SIZE_S  g_stOutSize;

AR_BOOL g_bVpssSuspend = AR_FALSE;
AR_BOOL g_bVpssResume = AR_FALSE;
/*
#define AR_SAMPLE_PAUSE()  do {\
        printf("---------------press Enter 'q' to exit!---------------\n");\
        fflush(stdin);\
    } while ('q' != getchar());
*/
static STRU_PIXEL_FORMAT_MAP pixel_map[] =
{
    //YUV444
    {PIXEL_FORMAT_YVU_PLANAR_444,       "yuv444p"},
    {PIXEL_FORMAT_YVU_SEMIPLANAR_444,   "nv42"},

    //YUV442
    {PIXEL_FORMAT_YVU_PLANAR_422,       "yuv422p"},
    {PIXEL_FORMAT_YVU_SEMIPLANAR_422,   "nv16"},

    //YUV420
    {PIXEL_FORMAT_YVU_PLANAR_420,       "yuv420p"},
    {PIXEL_FORMAT_YVU_SEMIPLANAR_420,   "nv21"},

};

struct _ar_mmap_module_info
{
    int cache_fd;
    int noncache_fd;
};

struct _ar_mmaped_info
{
    void            *real_start_va;
    size_t          real_sz;
    unsigned int    magic;
};

static struct _ar_mmap_module_info _mmap_inf = {
    .cache_fd       = -1,
    .noncache_fd    = -1
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
        LOG_ERROR("open noncache_fd = %d\n", _mmap_inf.noncache_fd);
        goto err_open;
    }

    if ((_mmap_inf.cache_fd = open("/dev/mem", O_RDWR)) <= 0) {
        LOG_ERROR("open cache_fd = %d\n", _mmap_inf.cache_fd);
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
            mapped->real_start_va   = real_start_va;
            mapped->real_sz         = length;
            mapped->magic           = AR_MMAP_MAGIC;

            *hdl = mapped;
            return (real_start_va + (pa & (page_size - 1)));

        } else {
            LOG_ERROR("ar_malloc fail\n");
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
        LOG_ERROR("wrong magic: 0x%x\n", info->magic);
        return AR_FAILURE;
    }

    ret = munmap(info->real_start_va, info->real_sz);
    if (ret)
        LOG_ERROR("munmap fail ret=%d errono:%d\n", ret, errno);

    free(hdl);

    return ret;
}

static AR_S32 getTimeStemp(AR_S64 *timestemp_ms, AR_S64 *timestemp_us){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *timestemp_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    *timestemp_us = tv.tv_sec * 1000000 + tv.tv_usec;

    return 0;
}

static AR_CHAR* pixel_fmt_to_name(PIXEL_FORMAT_E p_pixel_fmt)
{
    AR_S32 i;
    for (i = 0; i < sizeof(pixel_map) / sizeof(pixel_map[0]); i++)
    {
        if (p_pixel_fmt == pixel_map[i].enPixelFmt)
        {
            return pixel_map[i].formatName;
        }
    }

    return NULL;
}

AR_VOID vpss_log_func(const char *level, const char *file, int line, const char *func, const char *format, ...){
    va_list args;
    va_start(args, format);
    char fmt[2048];
    vsnprintf(fmt, 2048, format, args);
    va_end(args);
    if(strcmp(level, "ERROR") == 0){
        printf("%s[%s]%s %s:%d %s(): ", RED, level, NONE, file, line, func);
        printf("%s", fmt);
    } else if(strcmp(level, "DEBUG") == 0 && g_bDebug) {
        printf("%s[%s]%s %s:%d %s(): ", YELLOW, level, NONE, file, line, func);
        printf("%s", fmt);
    } else if(strcmp(level, "INFO") == 0){
        printf("%s[%s ]%s %s:%d %s(): ", CYAN, level, NONE, file, line, func);
        printf("%s", fmt);
    }
}

MPI_SYSCTL_HANDLE g_pSCHdl = NULL;

void AR_SAMPLE_VPSS_Sysctl_Event_Cb(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
	AR_S32 s32Ret = AR_OK;
    struct timeval tm_start;
    struct timeval tm_end;
    AR_U64 u64TsUs = 0;

	gettimeofday(&tm_start, NULL);

	switch(eSysctlEvent)
	{
	case SYSCTL_EVENT_FAST_SUSPEND:
	case SYSCTL_EVENT_SUSPEND:
		s32Ret = AR_MPI_VPSS_Suspend();
		break;

	case SYSCTL_EVENT_FAST_RESUME:
	case SYSCTL_EVENT_RESUME:
		s32Ret = AR_MPI_VPSS_Resume();
		break;

	default:
		return;
	}

	gettimeofday(&tm_end, NULL);

	if(s32Ret != AR_SUCCESS)
	{
		LOG_ERROR("%s event %d failed! ret:%#x\n", __func__, eSysctlEvent, s32Ret);
	}
	else
	{
        u64TsUs = tm_end.tv_sec * 1000000 + tm_end.tv_usec - tm_start.tv_sec * 1000000 - tm_start.tv_usec;
		LOG_INFO("%s event %d success, cost time %lld us\n", __func__, eSysctlEvent, u64TsUs);
	}

    s32Ret = AR_MPI_SYSCTL_Event_done(g_pSCHdl, eSysctlEvent);
	if(s32Ret != AR_SUCCESS)
	{
		LOG_ERROR("AR_MPI_SYSCTL_Event_done event %d failed! ret:%#x\n", eSysctlEvent, s32Ret);
	}
	else
	{
		LOG_INFO("AR_MPI_SYSCTL_Event_done event %d success\n", eSysctlEvent);
	}
}

static VB_POOL vbPoolId = VB_INVALID_POOLID;

AR_S32 AR_SAMPLE_VPSS_SYS_Init(AR_BOOL reinit, SIZE_S *old_size, SIZE_S *new_size, AR_BOOL is_grp)
{
    AR_U64 u64BlkSize, u64OldBlkSize;
    if(g_bSpec) {
        if(reinit) {
            LOG_ERROR("-S param doesnt support renit vb\n");
            return AR_FAILURE;
        }

        VB_POOL_CONFIG_S stVbPoolCfg;
        memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

        u64BlkSize = AR_SAMPLE_VPSS_GetPicBufferSize(g_stSrcSize.u32Width, g_stSrcSize.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 64);
        stVbPoolCfg.u64BlkSize = u64BlkSize;
        stVbPoolCfg.u32BlkCnt = g_groupNum * g_channelNum + 128;
        vbPoolId = AR_MPI_VB_CreatePool(&stVbPoolCfg);
        if(vbPoolId == VB_INVALID_POOLID) {
            LOG_ERROR("Create VB Pool failed! blksize=%lld\n", u64BlkSize);
            return vbPoolId;
        }
        LOG_INFO("Create VB Pool success, pool id = %d,  blksize=%lld\n", vbPoolId, u64BlkSize);

        return AR_MPI_SYS_Init();
    } else {
        AR_S32 s32Ret = AR_FAILURE;
        VB_CONFIG_S stVbConf = {0};
        SIZE_S max_size = g_stSrcSize;

        if(max_size.u32Width * max_size.u32Height < g_stOutSize.u32Width * g_stOutSize.u32Height) {
            max_size.u32Width = g_stOutSize.u32Width;
            max_size.u32Height = g_stOutSize.u32Height;
        }

        if(reinit) {
            if(max_size.u32Width * max_size.u32Height >= new_size->u32Width * new_size->u32Height) {
                return AR_SUCCESS;
            }

            if(g_bSysInited) {
                s32Ret = AR_MPI_VB_GetConfig(&stVbConf);
                if(s32Ret != AR_SUCCESS) {
                    LOG_ERROR("AR_MPI_VB_GetConfig failed! ret:%#x\n", s32Ret);
                    return s32Ret;
                }
            }

            int delta = 1;
            if(is_grp) {
                delta += g_channelNum;
            }

            u64OldBlkSize = AR_SAMPLE_VPSS_GetPicBufferSize(old_size->u32Width, old_size->u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 64);
            u64BlkSize = AR_SAMPLE_VPSS_GetPicBufferSize(new_size->u32Width, new_size->u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 64);

            int i, old_id = VB_MAX_COMM_POOLS, new_id = VB_MAX_COMM_POOLS, first_hole = VB_MAX_COMM_POOLS;
            for(i=0; i<VB_MAX_COMM_POOLS; i++) {
                LOG_INFO("i %d old_id %d new_id %d first_hole %d VB_MAX_COMM_POOLS %d\n", i, old_id, new_id, first_hole, VB_MAX_COMM_POOLS);
                if(old_id != VB_MAX_COMM_POOLS && new_id != VB_MAX_COMM_POOLS && first_hole != VB_MAX_COMM_POOLS) {
                    break;
                }
                if(stVbConf.astCommPool[i].u64BlkSize == u64OldBlkSize) {
                    if(old_id == VB_MAX_COMM_POOLS) {
                        old_id = i;
                    }
                } else if(stVbConf.astCommPool[i].u64BlkSize == u64BlkSize) {
                    if(new_id == VB_MAX_COMM_POOLS) {
                        new_id = i;
                    }
                } else if(stVbConf.astCommPool[i].u64BlkSize == 0 || stVbConf.astCommPool[i].u32BlkCnt == 0) {
                    if(first_hole == VB_MAX_COMM_POOLS) {
                        first_hole = i;
                    }
                }
            }

            if(old_id == VB_MAX_COMM_POOLS && new_id != VB_MAX_COMM_POOLS) {
                LOG_DEBUG("blk size %lld already configed\n", u64BlkSize);
                // reuse prev cfg
                return AR_SUCCESS;
            }

            if(old_id == VB_MAX_COMM_POOLS) {
                LOG_ERROR("blk size %lld not found\n", u64OldBlkSize);
                return AR_FAILURE;
            }

            if(new_id == VB_MAX_COMM_POOLS && first_hole == VB_MAX_COMM_POOLS) {
                LOG_ERROR("no empty blk\n");
                return AR_FAILURE;
            }

            if(g_bSysInited) {
                s32Ret = AR_MPI_VB_Exit();
                if(s32Ret != AR_SUCCESS) {
                    LOG_ERROR("AR_MPI_VB_Exit failed! ret:%#x\n", s32Ret);
                    return s32Ret;
                }
            }

            if(stVbConf.astCommPool[old_id].u32BlkCnt > delta * 3) {
                stVbConf.astCommPool[old_id].u32BlkCnt -= delta * 3;
                if(new_id < VB_MAX_COMM_POOLS) {
                    stVbConf.astCommPool[new_id].u32BlkCnt += delta * 3;
                    LOG_DEBUG("is_grp=%d delta=%d (old_id, u32BlkCnt)=(%d, %d) (new_id, u32BlkCnt)=(%d, %d) blksize=%lld\n",
                            is_grp, delta, old_id, stVbConf.astCommPool[old_id].u32BlkCnt, new_id, stVbConf.astCommPool[new_id].u32BlkCnt, u64BlkSize);
                } else {
                    stVbConf.astCommPool[first_hole].u64BlkSize = u64BlkSize;
                    stVbConf.astCommPool[first_hole].u32BlkCnt = delta * 3;
                    LOG_DEBUG("is_grp=%d delta=%d (old_id, u32BlkCnt)=(%d, %d) (first_hole, u32BlkCnt)=(%d, %d) blksize=%lld\n",
                            is_grp, delta, old_id, stVbConf.astCommPool[old_id].u32BlkCnt, first_hole, stVbConf.astCommPool[first_hole].u32BlkCnt, u64BlkSize);
                }
            } else {
                if(new_id < VB_MAX_COMM_POOLS) {
                    stVbConf.astCommPool[old_id].u64BlkSize = 0;
                    stVbConf.astCommPool[old_id].u32BlkCnt = 0;
                    stVbConf.astCommPool[new_id].u32BlkCnt += delta * 3;
                    LOG_DEBUG("is_grp=%d delta=%d (old_id, u32BlkCnt)=(%d, %d) (new_id, u32BlkCnt)=(%d, %d) blksize=%lld\n",
                            is_grp, delta, old_id, stVbConf.astCommPool[old_id].u32BlkCnt, new_id, stVbConf.astCommPool[new_id].u32BlkCnt, u64BlkSize);
                } else {
                    stVbConf.astCommPool[old_id].u64BlkSize = u64BlkSize;
                    stVbConf.astCommPool[old_id].u32BlkCnt = delta * 3;
                    LOG_DEBUG("is_grp=%d delta=%d (old_id, u32BlkCnt)=(%d, %d) (new_id, u32BlkCnt)=(-, %d) blksize=%lld\n",
                            is_grp, delta, old_id, stVbConf.astCommPool[old_id].u32BlkCnt, 0, u64BlkSize);
                }
            }

            s32Ret = AR_MPI_VB_SetConfig(&stVbConf);
            if (AR_SUCCESS != s32Ret)
            {
                LOG_ERROR("AR_MPI_VB_SetConf failed %d\n", s32Ret);
                return AR_FAILURE;
            }

            s32Ret = AR_MPI_VB_Init();
            if (AR_SUCCESS != s32Ret)
            {
                LOG_ERROR("AR_MPI_VB_Init failed %d\n", s32Ret);
                return AR_FAILURE;
            }
        } else {
            u64BlkSize = AR_SAMPLE_VPSS_GetPicBufferSize(max_size.u32Width, max_size.u32Height, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 64);
            stVbConf.astCommPool[0].u64BlkSize  = u64BlkSize;
            stVbConf.astCommPool[0].u32BlkCnt   = g_groupNum * (g_channelNum + 1) * 3;
            LOG_INFO("blkcnt = %d,  blksize=%lld\n", g_groupNum * (g_channelNum + 1) * 3, u64BlkSize);

            s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
            if(s32Ret != AR_SUCCESS) {
                LOG_ERROR("SAMPLE_COMM_SYS_Init failed! ret:%#x\n", s32Ret);
            } else {
                LOG_INFO("Create VB Pool success\n");
            }
        }
        return s32Ret;
    }
}

AR_VOID AR_SAMPLE_VPSS_SYS_Exit(void)
{
    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();

    if(g_bSpec) {
        AR_S32 s32Ret = AR_MPI_VB_DestroyPool(vbPoolId);
        if(s32Ret != AR_SUCCESS) {
            LOG_ERROR("VB Destory failed! pool id:%d ret:%#x\n", vbPoolId, s32Ret);
        }
    }
}

static AR_S32 AR_SAMPLE_VPSS_Prepare(const AR_S32 grpNum, const AR_S32 chnNum, const SIZE_S outSize)
{
    /* prepare group */

    if (grpNum > VPSS_MAX_GRP_NUM || chnNum > VPSS_MAX_CHN_NUM)
    {
        LOG_ERROR("group number %d or channel number %d out of range\n", grpNum, chnNum);
        return -1;
    }
    

    DYNAMIC_RANGE_E     enDynamicRange = DYNAMIC_RANGE_SDR8;
    PIXEL_FORMAT_E      enPixFormat    = PIXEL_FORMAT_YVU_PLANAR_420;
    VIDEO_FORMAT_E      enVideoFormat  = VIDEO_FORMAT_LINEAR;
    COMPRESS_MODE_E     enCompressMode = COMPRESS_MODE_NONE;

    for (int i = 0; i < grpNum; i++)
    {
        g_astVpssGrpAttr[i].stFrameRate.s32SrcFrameRate = -1;
        g_astVpssGrpAttr[i].stFrameRate.s32DstFrameRate = -1;
        g_astVpssGrpAttr[i].u32Width = outSize.u32Width;
        g_astVpssGrpAttr[i].u32Height = outSize.u32Height;
        g_astVpssGrpAttr[i].enCompressMode = enCompressMode;
        g_astVpssGrpAttr[i].enDynamicRange = enDynamicRange;
        g_astVpssGrpAttr[i].enVideoFormat = enVideoFormat;
        g_astVpssGrpAttr[i].enPixelFormat = enPixFormat;
        g_astVpssGrpAttr[i].u32ScaleMode = 0;
        g_abGrpEnable[i] = AR_TRUE;
    }

    /* prepare physical channel */
    for (int i = 0; i < grpNum; i++)
    {
        for (int j = 0; j < VPSS_MAX_PHY_CHN_NUM && j < chnNum; j++)
        {
            g_aastVpssChnAttr[i][j].u32Width = outSize.u32Width;
            g_aastVpssChnAttr[i][j].u32Height = outSize.u32Height;
            g_aastVpssChnAttr[i][j].enChnMode = VPSS_CHN_MODE_USER;
            g_aastVpssChnAttr[i][j].enCompressMode = enCompressMode;
            g_aastVpssChnAttr[i][j].enDynamicRange = enDynamicRange;
            g_aastVpssChnAttr[i][j].enVideoFormat = enVideoFormat;
            g_aastVpssChnAttr[i][j].enPixelFormat = enPixFormat;
            g_aastVpssChnAttr[i][j].stFrameRate.s32SrcFrameRate = 30;
            g_aastVpssChnAttr[i][j].stFrameRate.s32DstFrameRate = 30;
            g_aastVpssChnAttr[i][j].u32Depth = 1;
            g_aastVpssChnAttr[i][j].stAspectRatio.enMode = ASPECT_RATIO_NONE;
            g_aastVpssChnAttr[i][j].u32ScaleMode = 0;
            g_aabChnEnable[i][j] = AR_TRUE;

            // Fisheye attribute prepare
            g_aastVpssChnAttr[i][j].stLdcAttr.bEnable = AR_FALSE;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.ldc_k0 = -0.4501;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.ldc_k1 = 0.2584;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.ldc_k2 = -0.0927;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[0]=1367.9;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[1]=0;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[2]=962.29;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[3]=0;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[4]=1367.7;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[5]=513.1752;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[6]=0;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[7]=0;
            g_aastVpssChnAttr[i][j].stLdcAttr.stAttr.k[8]=1;

            // Spread attribute prepare
            g_aastVpssChnAttr[i][j].stSpreadAttr.bEnable = AR_FALSE;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.ldc_k0 = -0.4501;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.ldc_k1 = 0.2584;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.ldc_k2 = -0.0927;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[0]=1367.9;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[1]=0;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[2]=962.29;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[3]=0;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[4]=1367.7;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[5]=513.1752;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[6]=0;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[7]=0;
            g_aastVpssChnAttr[i][j].stSpreadAttr.stAttr.k[8]=1;
        }
    }

    if(chnNum > VPSS_MAX_PHY_CHN_NUM){
        /* prepare extension channel */
        for (int i = 0; i < grpNum; i++)
        {
            for (int j = VPSS_MAX_PHY_CHN_NUM; j < chnNum; j++)
            {
                // two extension channels bind to a physical channel, you can bind it to any physical channel in the same group that you want.
                g_aastVpssExtChnAttr[i][j].s32BindChn = (j - VPSS_MAX_PHY_CHN_NUM) / 2;     // just for test sample.
                g_aastVpssExtChnAttr[i][j].u32Width = outSize.u32Width;
                g_aastVpssExtChnAttr[i][j].u32Height = outSize.u32Height;
                g_aastVpssExtChnAttr[i][j].enChnMode = VPSS_CHN_MODE_USER;
                g_aastVpssExtChnAttr[i][j].enCompressMode = enCompressMode;
                g_aastVpssExtChnAttr[i][j].enDynamicRange = enDynamicRange;
                g_aastVpssExtChnAttr[i][j].enVideoFormat = enVideoFormat;
                g_aastVpssExtChnAttr[i][j].enPixelFormat = enPixFormat;
                g_aastVpssExtChnAttr[i][j].stFrameRate.s32SrcFrameRate = 30;
                g_aastVpssExtChnAttr[i][j].stFrameRate.s32DstFrameRate = 30;
                g_aastVpssExtChnAttr[i][j].u32Depth = 1;
                g_aastVpssExtChnAttr[i][j].stAspectRatio.enMode = ASPECT_RATIO_NONE;
                g_aastVpssExtChnAttr[i][j].u32ScaleMode = 0;
                g_aabChnEnable[i][j] = AR_TRUE;

                // Fisheye attribute prepare
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.bEnable = AR_FALSE;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.ldc_k0 = -0.4501;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.ldc_k1 = 0.2584;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.ldc_k2 = -0.0927;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[0]=1367.9;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[1]=0;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[2]=962.29;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[3]=0;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[4]=1367.7;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[5]=513.1752;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[6]=0;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[7]=0;
                g_aastVpssExtChnAttr[i][j].stFisheyeAttr.stAttr.k[8]=1;
            }
        }
    }

	g_pSCHdl = AR_MPI_SYSCTL_Register("vpss", 0, AR_SAMPLE_VPSS_Sysctl_Event_Cb);
	if(!g_pSCHdl)
	{
		LOG_ERROR("AR_MPI_SYSCTL_Register vpss failed\n");
	}
	else
	{
		LOG_INFO("AR_MPI_SYSCTL_Register vpss success\n");
	}
    return 0;
}

static AR_S32 AR_SAMPLE_VPSS_Start(AR_S32 grpNum, AR_S32 chnNum)
{
    AR_S32 s32Ret = AR_FAILURE;
    for (int i = 0; i < grpNum; i++)
    {
        VPSS_GRP VpssGrp = i;
        if(!g_abGrpEnable[VpssGrp]) continue;

        s32Ret = AR_MPI_VPSS_CreateGrp(VpssGrp, &g_astVpssGrpAttr[VpssGrp]);
        if (AR_SUCCESS != s32Ret)
        {
            LOG_ERROR("AR_MPI_VPSS_CreateGrp (grp:%d) failed with %#x!\n", VpssGrp, s32Ret);
            if(HAL_ERR_BUSY != AR_GET_ERRID(s32Ret))
            {
                return AR_FAILURE;
            }
        } else {
            LOG_INFO("AR_MPI_VPSS_CreateGrp (grp:%d) success. Scale W:%d H:%d, Mirror:%d, Flip:%d\n", VpssGrp, \
                    g_astVpssGrpAttr[VpssGrp].u32Width, g_astVpssGrpAttr[VpssGrp].u32Height, \
                    g_astVpssGrpAttr[VpssGrp].bMirror, g_astVpssGrpAttr[VpssGrp].bFlip);
        }

        if (g_astVpssGrpCrop[VpssGrp].bEnable)
        {
            s32Ret = AR_MPI_VPSS_SetGrpCrop(VpssGrp, &g_astVpssGrpCrop[VpssGrp]);
            if (AR_SUCCESS != s32Ret)
            {
                LOG_ERROR("AR_MPI_VPSS_SetGrpCrop (grp:%d) failed. s32Ret: 0x%x !\n", VpssGrp, s32Ret);
            } else {
                LOG_INFO("AR_MPI_VPSS_SetGrpCrop (grp:%d) success. Crop Type:%s, X:%d Y:%d W:%d H:%d\n", VpssGrp, \
                    g_astVpssGrpCrop[VpssGrp].enCropCoordinate ? "abs" : "ratio", \
                    g_astVpssGrpCrop[VpssGrp].stCropRect.s32X, g_astVpssGrpCrop[VpssGrp].stCropRect.s32Y, \
                    g_astVpssGrpCrop[VpssGrp].stCropRect.u32Width, g_astVpssGrpCrop[VpssGrp].stCropRect.u32Height);
            }
        }

        /* set physical channel */
        for (int j = 0; j < VPSS_MAX_PHY_CHN_NUM; j++)
        {
            if(AR_TRUE == g_aabChnEnable[VpssGrp][j])
            {
                VPSS_CHN VpssChn = j;
                s32Ret = AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &g_aastVpssChnAttr[VpssGrp][VpssChn]);
                if (s32Ret != AR_SUCCESS)
                {
                    LOG_ERROR("AR_MPI_VPSS_SetChnAttr (grp:%d chn:%d) failed with %#x\n", VpssGrp, VpssChn, s32Ret);
                    //if(HAL_ERR_EXIST != AR_GET_ERRID(s32Ret))
                    //{
                    //    return AR_FAILURE;
                    //}
                } else {
                    LOG_INFO("AR_MPI_VPSS_SetChnAttr (grp:%d chn:%d) success. Scale W:%d H:%d, Mirror:%d, Flip:%d\n", VpssGrp, VpssChn, \
                        g_aastVpssChnAttr[VpssGrp][VpssChn].u32Width, g_aastVpssChnAttr[VpssGrp][VpssChn].u32Height, \
                        g_aastVpssChnAttr[VpssGrp][VpssChn].bMirror, g_aastVpssChnAttr[VpssGrp][VpssChn].bFlip);
                }

                AR_U32 u32Align = 64;
                s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssChn, u32Align);
                if (s32Ret != AR_SUCCESS)
                {
                    LOG_ERROR("AR_MPI_VPSS_SetChnAlign (grp:%d phy chn:%d) set align to %d failed with %#x\n", VpssGrp, VpssChn, u32Align, s32Ret);
                    return AR_FAILURE;
                } else {
                    LOG_INFO("AR_MPI_VPSS_SetChnAlign (grp:%d phy chn:%d) set align to %d success\n", VpssGrp, VpssChn, u32Align);
                }

                /* set channel crop */
                if(g_aastVpssChnCrop[VpssGrp][VpssChn].bEnable)
                {
                    s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &g_aastVpssChnCrop[VpssGrp][VpssChn]);
                    if (AR_SUCCESS != s32Ret)
                    {
                        LOG_ERROR("AR_MPI_VPSS_SetChnCrop (grp:%d chn:%d) failed. s32Ret: 0x%x !\n", VpssGrp, VpssChn, s32Ret);
                    } else {
                        LOG_INFO("AR_MPI_VPSS_SetChnCrop (grp:%d chn:%d) success. Crop Type: %s X:%d Y:%d W:%d H:%d\n", VpssGrp, VpssChn, \
                            g_aastVpssChnCrop[VpssGrp][VpssChn].enCropCoordinate ? "abs" : "ratio", \
                            g_aastVpssChnCrop[VpssGrp][VpssChn].stCropRect.s32X, g_aastVpssChnCrop[VpssGrp][VpssChn].stCropRect.s32Y, \
                            g_aastVpssChnCrop[VpssGrp][VpssChn].stCropRect.u32Width, g_aastVpssChnCrop[VpssGrp][VpssChn].stCropRect.u32Height);
                    }
                }

                s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
                if (s32Ret != AR_SUCCESS)
                {
                    LOG_ERROR("AR_MPI_VPSS_EnableChn (grp:%d phy chn:%d) failed with %#x\n", VpssGrp, VpssChn, s32Ret);
                    return AR_FAILURE;
                }
                LOG_INFO("AR_MPI_VPSS_EnableChn (grp:%d phy chn:%d) success.\n", VpssGrp, VpssChn);
            }
        }

        /* set extension channel */
        for(int j = VPSS_MAX_PHY_CHN_NUM; j < chnNum; j++)
        {
            VPSS_CHN VpssExtChn = j;
            if(!g_aabChnEnable[VpssGrp][VpssExtChn]) continue;

            s32Ret = AR_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssExtChn, &g_aastVpssExtChnAttr[VpssGrp][VpssExtChn]);
            if(AR_SUCCESS != s32Ret){
                LOG_ERROR("AR_MPI_VPSS_SetExtChnAttr (grp:%d ext chn:%d binding to physical channel %d) failed with %#x!\n", \
                            VpssGrp, VpssExtChn, g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].s32BindChn, s32Ret);
                return AR_FAILURE;
            } else {
                LOG_INFO("AR_MPI_VPSS_SetExtChnAttr (grp:%d ext chn:%d binding to physical channel %d) success. Scale W:%d H:%d, Mirror:%d, Flip:%d\n", \
                            VpssGrp, VpssExtChn, g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].s32BindChn, \
                            g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].u32Width, g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].u32Height, \
                            g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].bMirror, g_aastVpssExtChnAttr[VpssGrp][VpssExtChn].bFlip);
            }

            AR_U32 u32Align = 64;
            s32Ret = AR_MPI_VPSS_SetChnAlign(VpssGrp, VpssExtChn, u32Align);
            if (s32Ret != AR_SUCCESS)
            {
                LOG_ERROR("AR_MPI_VPSS_SetChnAlign (grp:%d ext chn:%d) set align to %d failed with %#x\n", VpssGrp, VpssExtChn, u32Align, s32Ret);
                return AR_FAILURE;
            } else {
                LOG_INFO("AR_MPI_VPSS_SetChnAlign (grp:%d ext chn:%d) set align to %d success\n", VpssGrp, VpssExtChn, u32Align);
            }

            /* set channel crop */
            if(g_aastVpssChnCrop[VpssGrp][VpssExtChn].bEnable)
            {
                s32Ret = AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssExtChn, &g_aastVpssChnCrop[VpssGrp][VpssExtChn]);
                if (AR_SUCCESS != s32Ret)
                {
                    LOG_ERROR("AR_MPI_VPSS_SetChnCrop (grp:%d chn:%d) failed. s32Ret: 0x%x !\n", VpssGrp, VpssExtChn, s32Ret);
                } else {
                    LOG_INFO("AR_MPI_VPSS_SetChnCrop (grp:%d chn:%d) success. Crop Type: %s X:%d Y:%d W:%d H:%d\n", VpssGrp, VpssExtChn, \
                        g_aastVpssChnCrop[VpssGrp][VpssExtChn].enCropCoordinate ? "abs" : "ratio", \
                        g_aastVpssChnCrop[VpssGrp][VpssExtChn].stCropRect.s32X, g_aastVpssChnCrop[VpssGrp][VpssExtChn].stCropRect.s32Y, \
                        g_aastVpssChnCrop[VpssGrp][VpssExtChn].stCropRect.u32Width, g_aastVpssChnCrop[VpssGrp][VpssExtChn].stCropRect.u32Height);
                }
            }

            s32Ret = AR_MPI_VPSS_EnableChn(VpssGrp, VpssExtChn);
            if (s32Ret != AR_SUCCESS)
            {
                LOG_ERROR("AR_MPI_VPSS_EnableChn (grp:%d ext chn:%d) failed with %#x\n", VpssGrp, VpssExtChn, s32Ret);
                return AR_FAILURE;
            } else {
                LOG_INFO("AR_MPI_VPSS_EnableChn (grp:%d ext chn:%d) success\n", VpssGrp, VpssExtChn);
            }
        }

        s32Ret = AR_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != AR_SUCCESS)
        {
            LOG_INFO("AR_MPI_VPSS_StartGrp (grp %d) failed with %#x\n", VpssGrp, s32Ret);
            return AR_FAILURE;
        } else {
            LOG_INFO("AR_MPI_VPSS_StartGrp (grp %d) success.\n", VpssGrp);
        }
    }

    return s32Ret;
}

static AR_S32 AR_SAMPLE_VPSS_Stop(AR_S32 groupNum, AR_S32 channelNum)
{
    AR_S32 s32Ret = AR_FAILURE;
    for (int i = 0; i < groupNum; i++)
    {
        for(int j = 0; j < channelNum; j++)
        {
            if(AR_TRUE == g_aabChnEnable[i][j])
            {
                AR_MPI_VPSS_DisableChn(i, j);
            }
        }

        s32Ret = AR_MPI_VPSS_StopGrp(i);
    }

    return s32Ret;
}

static AR_S32 AR_SAMPLE_VPSS_Destroy(VPSS_GRP VpssGrp)
{
    return AR_MPI_VPSS_DestroyGrp(VpssGrp);
}

static AR_S32 AR_SAMPLE_VPSS_DestroyAll(AR_S32 groupNum)
{
    AR_S32 s32Ret = AR_FAILURE;
    for (int i = 0; i < groupNum; i++)
    {
        s32Ret = AR_SAMPLE_VPSS_Destroy(i);
    }

	if(g_pSCHdl)
	{
		AR_S32 s32Ret = AR_MPI_SYSCTL_Unregister(g_pSCHdl);
		if(s32Ret != AR_SUCCESS)
		{
			LOG_ERROR("AR_MPI_SYSCTL_Unregister failed! ret:%#x\n", s32Ret);
		}
		else
		{
			LOG_INFO("AR_MPI_SYSCTL_Unregister success\n");
		}
	}
    return s32Ret;
}

static AR_S32 AR_SAMPLE_VPSS_ReadOneFrame(FILE * fp, AR_U8 * pY, AR_U8 * pU, AR_U8 * pV,
                                              AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2,
                                              PIXEL_FORMAT_E enPixFrm)
{
    AR_U8 * pDst;
    AR_U32 u32UVHeight;
    AR_U32 u32Row;

    if(enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_422 || enPixFrm == PIXEL_FORMAT_YVU_PLANAR_422) {
        u32UVHeight = height;
    } else if (enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_420 || enPixFrm == PIXEL_FORMAT_YVU_PLANAR_420){
        u32UVHeight = height / 2;
    } else {
    	return AR_FAILURE;
    }

    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ ) {
        if (fread( pDst, 1, width, fp ) != width) {
            LOG_ERROR("Read frame Y failed!\n");
            return -1;
        }
        pDst += stride;
    }

    pDst = pU;
    for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
        if(fread( pDst, 1, width/2, fp ) != width/2) {
            LOG_ERROR("Read frame U failed!\n");
            return -2;
        }
        pDst += stride2;
    }

    pDst = pV;
    for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
        if(fread( pDst, 1, width/2, fp ) != width/2) {
            LOG_ERROR("Read frame V failed!\n");
            return -3;
        }
        pDst += stride2;
    }

   return AR_SUCCESS;
}

static AR_S32 AR_SAMPLE_VPSS_SaveOneFrame(FILE * fp, AR_U8 * pY, AR_U8 * pU, AR_U8 * pV,
                                        AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2,
                                        PIXEL_FORMAT_E enPixFrm)
{
    AR_U32 u32UVHeight;
    AR_U32 u32Row;

    if(enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_422 || enPixFrm == PIXEL_FORMAT_YVU_PLANAR_422) {
        u32UVHeight = height;
    } else {
        u32UVHeight = height / 2;
    }

    for ( u32Row = 0; u32Row < height; u32Row++ ) {
        if (fwrite(pY, 1, width, fp ) != width) {
            LOG_ERROR("write Y to file filed!\n");
            return -1;
        }
        pY += stride;
    }

    for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
        if(fwrite(pU, 1, width/2, fp ) != width/2) {
            LOG_ERROR("write U to file filed!\n");
            return -2;
        }
        pU += stride2;
    }

    for ( u32Row = 0; u32Row < u32UVHeight; u32Row++ ) {
        if(fwrite(pV, 1, width/2, fp ) != width/2) {
            LOG_ERROR("write V to file filed!\n");
            return -3;
        }
        pV += stride2;
    }
    // fflush(fp);

   return AR_SUCCESS;
}

#if 0
static AR_S32 AR_SAMPLE_VPSS_Planar_YUVToYVU(AR_U8 *pY, AR_S32 yStride,
                       AR_U8 *pU, AR_S32 uStride,
                       AR_U8 *pV, AR_S32 vStride,
                       AR_S32 picWidth, AR_S32 picHeight, PIXEL_FORMAT_E enPixFrm)
{
    LOG_DEBUG("Conver data Plannar to SemiPlanner.\n");
    AR_S32 i;
    AR_U8* pTmpU, *ptu;
    AR_U8* pTmpV, *ptv;

    AR_S32 s32HafW = uStride;
    AR_S32 s32HafH;

    if(enPixFrm == PIXEL_FORMAT_YVU_PLANAR_422) {
        s32HafH = picHeight;
    } else {
        s32HafH = picHeight >>1;
    }

    AR_S32 s32Size = s32HafW*s32HafH;

    pTmpU = malloc( s32Size ); ptu = pTmpU;
    pTmpV = malloc( s32Size ); ptv = pTmpV;

    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);

    for(i = 0; i< s32Size; i++) {
        *pU++ = *pTmpV++;
    }
    for(i = 0; i< s32Size; i++) {
        *pV++ = *pTmpU++;
    }

    free( ptu );
    free( ptv );

    return AR_SUCCESS;
}
#endif

static AR_S32 AR_SAMPLE_VPSS_PlanToSemi(AR_U8 *pY, AR_S32 yStride,
                       AR_U8 *pU, AR_S32 uStride,
                       AR_U8 *pV, AR_S32 vStride,
                       AR_S32 picWidth, AR_S32 picHeight, PIXEL_FORMAT_E enPixFrm)
{
    LOG_DEBUG("Conver data Plannar to SemiPlanner.\n");
    AR_S32 i;
    AR_U8* pTmpU, *ptu;
    AR_U8* pTmpV, *ptv;

    AR_S32 s32HafW = uStride >>1 ;
    AR_S32 s32HafH;

    if(enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_422) {
        s32HafH = picHeight;
    } else {
        s32HafH = picHeight >>1 ;
    }

    AR_S32 s32Size = s32HafW*s32HafH;

    pTmpU = malloc( s32Size ); ptu = pTmpU;
    pTmpV = malloc( s32Size ); ptv = pTmpV;

    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);

    for(i = 0;i<s32Size>>1;i++) {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;
    }
    for(i = 0;i<s32Size>>1;i++) {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free( ptu );
    free( ptv );

    return AR_SUCCESS;
}

static AR_VOID AR_SAMPLE_VPSS_GetBaseThreadInfo(ThreadCtrl_Info_In *pstThreadInfo,SIZE_S *pstFrmSize)
{
    pstThreadInfo->enPixelFmt = PIXEL_FORMAT_YVU_PLANAR_420;
    pstThreadInfo->u32Width = pstFrmSize->u32Width;
    pstThreadInfo->u32Height = pstFrmSize->u32Height;

    return;
}

static AR_VOID *AR_SAMPLE_VPSS_SendFrame2Group(AR_VOID* pData)
{
    ThreadCtrl_Info_In *pInfo = (ThreadCtrl_Info_In *)pData;
    VPSS_GRP vpssGrp = pInfo->s32ToGroup;
    AR_U32 u32BlkSize;

    AR_U32 u32SrcWidth = pInfo->u32Width;
    AR_U32 u32SrcHeight = pInfo->u32Height;

    FILE *pfd = NULL;
    pfd = fopen(pInfo->srcFile, "rb");
    if (pfd == AR_NULL) {
        LOG_ERROR("open file %s fail %s\n", pInfo->srcFile, strerror(errno));
        return AR_NULL;
    }

    u32BlkSize = AR_SAMPLE_VPSS_GetPicBufferSize(u32SrcWidth, u32SrcHeight, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, 64);
    LOG_DEBUG("open file %s success!\n", pInfo->srcFile);
    LOG_DEBUG("START Thread of send frame to group %d\n", vpssGrp);

    FILE* fp1 = NULL;
#if 0
    FILE* fp2 = NULL;
    fp1 = fopen("AR_SAMPLE_VPSS_ReadOneFrame.yuv", "wb");
    fp2 = fopen("AR_SAMPLE_VPSS_Planar_YUVToYVU.yuv", "wb");
#endif

    AR_S64 count = 0;

    while(g_bStopSendFrameThd == AR_FALSE) {
        AR_S64 time_start_ms, time_start_us;
        if(g_bCaculateTime) getTimeStemp(&time_start_ms, &time_start_us);

        AR_S32 s32Ret = 0;
        VB_BLK hBlkHdl;
        VIDEO_FRAME_INFO_S stUserFrame;
        memset(&stUserFrame,0,sizeof(VIDEO_FRAME_INFO_S));
        stUserFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        stUserFrame.stVFrame.enPixelFormat = pInfo->enPixelFmt;
        stUserFrame.stVFrame.u32Width = u32SrcWidth;
        stUserFrame.stVFrame.u32Height = u32SrcHeight;
        stUserFrame.stVFrame.u32Stride[0] = ALIGN_UP(u32SrcWidth, 64);
        stUserFrame.stVFrame.u32Stride[1] = ALIGN_UP(u32SrcWidth, 64) >> 1;
        stUserFrame.stVFrame.u32Stride[2] = ALIGN_UP(u32SrcWidth, 64) >> 1;
        stUserFrame.stVFrame.u32TimeRef = 0;
        stUserFrame.stVFrame.u64PTS = 0;

        AR_U32 u32LumaSize = 0;
        AR_U32 u32ChromaSize = 0;
        u32LumaSize =  stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight;
        if (pInfo->enPixelFmt == PIXEL_FORMAT_YVU_SEMIPLANAR_422 || pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_422) {
            u32ChromaSize =  stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight / 2;
        } else if (pInfo->enPixelFmt == PIXEL_FORMAT_YVU_SEMIPLANAR_420 || pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_420) {
            u32ChromaSize =  stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight / 4;
        } else if ((pInfo->enPixelFmt == PIXEL_FORMAT_YUV_400)) {
            u32ChromaSize =  0;
        }

        if (feof(pfd)) {
            LOG_DEBUG("read end of file, seek to beginning\n");
            fseek(pfd, 0, SEEK_SET);
        }

        hBlkHdl = AR_MPI_VB_GetBlock(vbPoolId, u32BlkSize, NULL);
        if (hBlkHdl == VB_INVALID_HANDLE) {
            LOG_ERROR("get vb fail!!! %d\n", u32BlkSize);
            sleep(1);
            continue;
        }
        // LOG_DEBUG("get vb success!\n");

        stUserFrame.u32PoolId = AR_MPI_VB_Handle2PoolId(hBlkHdl);
        stUserFrame.stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr( hBlkHdl );
        stUserFrame.stVFrame.u64PhyAddr[1] = stUserFrame.stVFrame.u64PhyAddr[0] + u32LumaSize;
        stUserFrame.stVFrame.u64PhyAddr[2] = stUserFrame.stVFrame.u64PhyAddr[1] + u32ChromaSize;

        stUserFrame.stVFrame.u64VirAddr[0] = (AR_U64)AR_MPI_SYS_Mmap(stUserFrame.stVFrame.u64PhyAddr[0], u32BlkSize);
        stUserFrame.stVFrame.u64VirAddr[1] = (AR_U64)(stUserFrame.stVFrame.u64VirAddr[0]) + u32LumaSize;
        stUserFrame.stVFrame.u64VirAddr[2] = (AR_U64)(stUserFrame.stVFrame.u64VirAddr[1]) + u32ChromaSize;

        if(pInfo->enPixelFmt == PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
            s32Ret = AR_SAMPLE_VPSS_ReadOneFrame(pfd, (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                        (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                        stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                        stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                        stUserFrame.stVFrame.enPixelFormat);
            if(s32Ret == AR_SUCCESS) {
               AR_SAMPLE_VPSS_PlanToSemi( (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0], stUserFrame.stVFrame.u32Stride[0],
                                                (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], stUserFrame.stVFrame.u32Stride[1],
                                                (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2], stUserFrame.stVFrame.u32Stride[1],
                                                stUserFrame.stVFrame.u32Width,    stUserFrame.stVFrame.u32Height,
                                                stUserFrame.stVFrame.enPixelFormat);
            }
        } else if(pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_420){
             s32Ret = AR_SAMPLE_VPSS_ReadOneFrame( pfd, (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                       (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                       stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                       stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                       stUserFrame.stVFrame.enPixelFormat);
            if(fp1 != NULL){
                AR_SAMPLE_VPSS_SaveOneFrame(fp1, (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                        (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                        stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                        stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                        stUserFrame.stVFrame.enPixelFormat);
            }
#if 0
            if(s32Ret == AR_SUCCESS){
                AR_SAMPLE_VPSS_Planar_YUVToYVU( (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0], stUserFrame.stVFrame.u32Stride[0],
                                                (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], stUserFrame.stVFrame.u32Stride[1],
                                                (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2], stUserFrame.stVFrame.u32Stride[1],
                                                stUserFrame.stVFrame.u32Width,    stUserFrame.stVFrame.u32Height,
                                                stUserFrame.stVFrame.enPixelFormat);
            }
            if(fp2 != NULL){
                AR_SAMPLE_VPSS_SaveOneFrame(fp2, (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                        (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                        stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                        stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                        stUserFrame.stVFrame.enPixelFormat);
            }
#endif
        }
        if(s32Ret != AR_SUCCESS){
            LOG_ERROR("AR_SAMPLE_VPSS_ReadOneFrame Failed!\n");
            goto OUT;
        }

        stUserFrame.stVFrame.u64PTS += 40000;
        stUserFrame.stVFrame.u32TimeRef += 40000;

        AR_S64 time1_start_ms, time1_start_us;
        if(g_bCaculateTime) getTimeStemp(&time1_start_ms, &time1_start_us);
        s32Ret = AR_MPI_VPSS_SendFrame(vpssGrp, 0 , &stUserFrame, 0);
        if(g_bCaculateTime){
            AR_S64 time1_end_ms, time1_end_us;
            getTimeStemp(&time1_end_ms, &time1_end_us);
            LOG_INFO("===SEND=== Group %d, Call mpi AR_MPI_VPSS_SendFrame cost %lld ms (%lld us)\n", vpssGrp, time1_end_ms-time1_start_ms, time1_end_us-time1_start_us);
        }

        if(s32Ret != AR_SUCCESS){
            LOG_ERROR("AR_MPI_VPSS_SendFrame to group %d Failed! s32Ret=0x%x\n", vpssGrp, s32Ret);
        } else {
            count++;
            LOG_DEBUG("AR_MPI_VPSS_SendFrame to group %d Success! count %lld\n", vpssGrp, count);
        }


OUT:
        AR_MPI_VB_ReleaseBlock(hBlkHdl);
        AR_MPI_SYS_Munmap((AR_VOID*)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0], u32BlkSize);

        if(g_bCaculateTime){
            AR_S64 time_end_ms, time_end_us;
            getTimeStemp(&time_end_ms, &time_end_us);
            LOG_INFO("===SEND=== Group %d, Process send frame cost %lld ms (%lld us)\n", vpssGrp, time_end_ms-time_start_ms, time_end_us-time_start_us);
        }
#ifdef CONFIG_BIT_32BITS
        if(g_bWriteOutFrame)
            usleep(1500*1000);   // sleep 1500ms
        else
#endif
        usleep(500*1000);   // sleep 500ms
    }

    // while (g_astThreadInfoIn[vpssGrp].bDestroy == AR_FALSE) {
    //         ;
    // }

    if(AR_NULL != pfd) fclose(pfd);
    pthread_exit(0);

    LOG_DEBUG("==================== STOP Thread of send frame to group %d\n", vpssGrp);

    return NULL;
}

static AR_VOID AR_SAMPLE_VPSS_SendFrameThd(AR_S32 groupNum, AR_CHAR filename[256],SIZE_S *pstFrmSize)
{
    g_bStopSendFrameThd = AR_FALSE;
    for(int i = 0; i < groupNum; i++){
        VPSS_GRP vpssGrp = i;

        AR_SAMPLE_VPSS_GetBaseThreadInfo(&g_astThreadInfoIn[vpssGrp], pstFrmSize);
        g_astThreadInfoIn[vpssGrp].s32ToGroup = vpssGrp;
        g_astThreadInfoIn[vpssGrp].bGrpEnable = AR_TRUE;

        strncpy(g_astThreadInfoIn[vpssGrp].srcFile, filename, sizeof(g_astThreadInfoIn[vpssGrp].srcFile) - 1);
        pthread_create(&(g_astThreadInfoIn[vpssGrp].tid), NULL, AR_SAMPLE_VPSS_SendFrame2Group, &g_astThreadInfoIn[vpssGrp]);
    }
    LOG_DEBUG("AR_SAMPLE_VPSS_SendFrameThd done\n");

    return;
}

#if 0
static AR_S32 AR_SAMPLE_VPSS_SaveChnFrame(VIDEO_FRAME_INFO_S* frame, AR_S32 fp)
{
    AR_S32 s32Ret = -1;
    if (fp != -1)
    {
        AR_U32 y_size = frame->stVFrame.u64PhyAddr[1] - frame->stVFrame.u64PhyAddr[0];
        AR_U32 u_size = frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[1];
        AR_U32 v_size = u_size;

        AR_U32 width = frame->stVFrame.u32Width;
        AR_U32 height = frame->stVFrame.u32Height;
        AR_U32 stride0 = frame->stVFrame.u32Stride[0];
        AR_U32 stride1 = frame->stVFrame.u32Stride[1];
        AR_U32 stride2 = frame->stVFrame.u32Stride[2];

        AR_MMAP_ATTR_NONCACHEABLE(attr);
        void *y_hdl = NULL;
        void *y_virt_addr = ar_mmap(frame->stVFrame.u64PhyAddr[0], y_size, attr, &y_hdl);
        if(y_virt_addr)
        {
            for(int i = 0; i < height; i++){
                s32Ret = write(fp, y_virt_addr, width);
                if(s32Ret != width) LOG_ERROR("STORE y component failed!\n");
                y_virt_addr += stride0;
            }
            ar_munmap(y_hdl);
        } else {
            LOG_ERROR("mapped y virtual address is NULL!\n")
            return -1;
        }

        void *u_hdl = NULL;
        void *u_virt_addr = ar_mmap(frame->stVFrame.u64PhyAddr[1], u_size, attr, &u_hdl);
        if(u_virt_addr)
        {
            printf("u_virt_addr = %p \n", u_virt_addr);
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, u_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE u component failed!\n");
                u_virt_addr += stride1;
            }
            ar_munmap(u_hdl);
        } else {
            LOG_ERROR("mapped u virtual address is NULL!\n")
            return -1;
        }

        void *v_hdl = NULL;
        void *v_virt_addr = ar_mmap(frame->stVFrame.u64PhyAddr[2], v_size, attr, &v_hdl);
        if(v_virt_addr)
        {
            printf("v_virt_addr = %p \n", v_virt_addr);
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, v_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE v component failed!\n");
                v_virt_addr += stride2;
            }
            ar_munmap(v_hdl);
        } else {
            LOG_ERROR("mapped v virtual address is NULL!\n")
            return -1;
        }
    }

    return AR_SUCCESS;
}
#endif

static AR_S32 AR_SAMPLE_VPSS_SaveChnFrame(VIDEO_FRAME_INFO_S* frame, AR_S32 fp)
{
    AR_S32 s32Ret = -1;
    if (fp != -1)
    {
        AR_U32 width = frame->stVFrame.u32Width;
        AR_U32 height = frame->stVFrame.u32Height;
        AR_U32 stride0 = frame->stVFrame.u32Stride[0];
        AR_U32 stride1 = frame->stVFrame.u32Stride[1];
        AR_U32 stride2 = frame->stVFrame.u32Stride[2];
        AR_U32 y_size = frame->stVFrame.u64PhyAddr[1] - frame->stVFrame.u64PhyAddr[0];
        AR_U32 u_size = frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[1];
        AR_U32 v_size = u_size;
        AR_U32 size = y_size + u_size + v_size;

        char *pData = (char *)malloc(sizeof(char)*size);
        if(pData == NULL){
            LOG_ERROR("malloc %d bytes failed!\n", size);
            return -1;
        }

        void *hdl = NULL;
        AR_MMAP_ATTR_NONCACHEABLE(attr);
        void *virt_addr = ar_mmap(frame->stVFrame.u64PhyAddr[0], size, attr, &hdl);
        if(virt_addr)
        {
            memcpy(pData, virt_addr, size);
            char *p = pData;
            char *y_virt_addr = p;
            for(int i = 0; i < height; i++){
                s32Ret = write(fp, y_virt_addr, width);
                if(s32Ret != width) LOG_ERROR("STORE y component failed!\n");
                y_virt_addr += stride0;
            }
            p+=y_size;

            char *u_virt_addr = p;
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, u_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE u component failed!\n");
                u_virt_addr += stride1;
            }
            p+=u_size;

            char *v_virt_addr = p;
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, v_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE v component failed!\n");
                v_virt_addr += stride2;
            }
        } else {
            LOG_ERROR("mapped virtual address is NULL!\n")
            free(pData);
            return -1;
        }
        ar_munmap(hdl);
        free(pData);
    }

    return AR_SUCCESS;
}

#if 0
static AR_S32 AR_SAMPLE_VPSS_SaveChnFrame(VIDEO_FRAME_INFO_S* frame, AR_S32 fp)
{
    AR_S32 s32Ret = -1;
    if (fp != -1)
    {
        AR_U32 s32DataLenDstY = frame->stVFrame.u32Stride[0] * frame->stVFrame.u32Height;
        AR_U32 s32DataLenDstU = frame->stVFrame.u32Stride[1] * frame->stVFrame.u32Height / 2;
        AR_U32 s32DataLenDstV = frame->stVFrame.u32Stride[1] * frame->stVFrame.u32Height / 2;
        // AR_U32 size = frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[0] + s32DataLenDstV;

        AR_U32 y_size = frame->stVFrame.u64PhyAddr[1] - frame->stVFrame.u64PhyAddr[0];
        AR_U32 u_size = frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[1];
        AR_U32 v_size = u_size;
        AR_U32 size = y_size + u_size + v_size;

        printf("y_size: %d, u_size: %d, v_size: %d, total: %d, size %d\n", y_size, u_size, v_size, y_size+u_size+v_size, size);

        printf("u64PhyAddr[0] = %p, u64PhyAddr[1] = %p, u64PhyAddr[2] = %p\n", &frame->stVFrame.u64PhyAddr[0], &frame->stVFrame.u64PhyAddr[1], &frame->stVFrame.u64PhyAddr[2]);

        void *hdl = NULL;
        AR_MMAP_ATTR_NONCACHEABLE(attr);
        void *virt_addr = ar_mmap(frame->stVFrame.u64PhyAddr[0], size, attr, &hdl);
        if(virt_addr)
        {
            s32Ret = write(fp, virt_addr, s32DataLenDstY);
            if(s32Ret != s32DataLenDstY)
            {
                LOG_ERROR("STORE planarY data_len=%d  ret_len=%d\n", s32DataLenDstY, s32Ret);
                return -1;
            }

            s32Ret = write(fp, virt_addr + frame->stVFrame.u64PhyAddr[1] - frame->stVFrame.u64PhyAddr[0], s32DataLenDstU);
            if(s32Ret != s32DataLenDstU)
            {
                LOG_ERROR("%s: STORE planarU data_len=%d  ret_len=%d\n", s32DataLenDstU, s32Ret);
                return -1;
            }

            s32Ret = write(fp, virt_addr + frame->stVFrame.u64PhyAddr[2] - frame->stVFrame.u64PhyAddr[0], s32DataLenDstV);
            if(s32Ret != s32DataLenDstV)
            {
                LOG_ERROR("STORE planarV data_len=%d  ret_len=%d\n", s32DataLenDstV, s32Ret);
                return -1;
            }
        } else {
            LOG_ERROR("mapped virtual address is NULL!\n")
            return -1;
        }
        ar_munmap(hdl);
    }

    return AR_SUCCESS;
}
#endif

static AR_VOID* AR_SAMPLE_VPSS_GetChnFrame(AR_VOID* args)
{
    ThreadCtrl_Info_Out *pInfo = (ThreadCtrl_Info_Out *)args;
    VPSS_GRP VpssGrp = pInfo->s32Group;
    VPSS_CHN VpssChn = pInfo->s32Channel;

    LOG_DEBUG("START Thread of get frame from group %d channel %d \n", VpssGrp, VpssChn);

    AR_S32 s32Ret = AR_FAILURE;
    AR_S32 fp = -1;
    char saveFileName[MAX_NAME_LEN] = {0};
    AR_S32 widthTmp = -1;
    AR_S32 heightTmp = -1;

    AR_S64 count = 0;
    AR_U64* pu64LumaData = NULL;


    LOG_DEBUG("u32RegionNum=%d\n", g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum);
    for(int i=0; i<g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum; i++){
        LOG_DEBUG("(%d, %d, %d, %d) ", g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion[i].s32X, \
                                        g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion[i].s32Y, \
                                        g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion[i].u32Width, \
                                        g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion[i].u32Height);
    }
    LOG_DEBUG("\n");

    if(g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum){
        pu64LumaData = malloc(g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum * sizeof(AR_U64));
        if(!pu64LumaData){
            LOG_ERROR("malloc failed\n");
        }else{
            memset(pu64LumaData, 0, g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum * sizeof(AR_U64));
        }
    }else if(g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion){
        free(g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion);
        g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].pstRegion = NULL;
    }

    while (g_bStopGetFrameThd == AR_FALSE) {
        VIDEO_FRAME_INFO_S stFrame;
        memset(&stFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

        AR_S64 time_start_ms, time_start_us;
        if(g_bCaculateTime) getTimeStemp(&time_start_ms, &time_start_us);
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, -1);
        // s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, 1000);     // set timeout 1s
        AR_S64 time_end1_ms, time_end1_us;
        if(g_bCaculateTime){
            getTimeStemp(&time_end1_ms, &time_end1_us);
            LOG_INFO("===GET==== Group %d Channel %d, Call mpi AR_MPI_VPSS_GetChnFrame cost %lld ms (%lld us)\n", \
                    VpssGrp, VpssChn, time_end1_ms-time_start_ms, time_end1_us-time_start_us);
        }

        if(AR_SUCCESS == s32Ret){
            AR_S32 frameWidth  = stFrame.stVFrame.u32Width;
            AR_S32 frameHeight = stFrame.stVFrame.u32Height;
            AR_S32 frameFormat = stFrame.stVFrame.enPixelFormat;

            char *formatName = pixel_fmt_to_name(frameFormat);
            AR_S32 stride0 = stFrame.stVFrame.u32Stride[0];
            AR_S32 stride1 = stFrame.stVFrame.u32Stride[1];
            AR_S32 stride2 = stFrame.stVFrame.u32Stride[2];

            count++;
            LOG_DEBUG("AR_MPI_VPSS_GetChnFrame from group %d channel %d success! total frames %lld, width %d height %d, format %s, stride %d %d %d\n", \
                    VpssGrp, VpssChn, count, frameWidth, frameHeight, formatName, stride0, stride1, stride2);

            if(widthTmp != frameWidth || heightTmp != frameHeight){
                if(fp != -1){
                    close(fp);
                }
                sprintf(saveFileName, "%s/grp%d_chn%d_%dx%d_Y-stride_%d_%s.yuv", \
                        g_save_path, VpssGrp, VpssChn, frameWidth, frameHeight, stride0, formatName);
                fp = open(saveFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                if (fp < 0) {
                    LOG_ERROR("create output file %s failed! %s", saveFileName, strerror(errno));
                }
                widthTmp = frameWidth;
                heightTmp = frameHeight;
            }

            if(fp != -1 && g_bWriteOutFrame){
                AR_S64 save_time_start_ms, save_time_start_us;
                if(g_bCaculateTime) getTimeStemp(&save_time_start_ms, &save_time_start_us);
                s32Ret = AR_SAMPLE_VPSS_SaveChnFrame(&stFrame, fp);
                if(g_bCaculateTime){
                    AR_S64 save_time_end_ms, save_time_end_us;
                    getTimeStemp(&save_time_end_ms, &save_time_end_us);
                    LOG_INFO("===SAVE=== Group %d Channel %d, Write frame to file cost %lld ms (%lld us)\n", \
                            VpssGrp, VpssChn, save_time_end_ms-save_time_start_ms, save_time_end_us-save_time_start_us);
                }
                if(AR_SUCCESS == s32Ret){
                    LOG_DEBUG("Write frame to file %s Success!\n", saveFileName);
                } else {
                    LOG_ERROR("Write frame to file %s failed! s32Ret=0x%x\n", saveFileName, s32Ret);
                }
            }

            s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
            if(AR_SUCCESS != s32Ret){
                LOG_ERROR("AR_MPI_VPSS_ReleaseChnFrame group %d channel %d failed! ret=0x%x\n", VpssGrp, VpssChn, s32Ret);
            } else {
                LOG_DEBUG("AR_MPI_VPSS_ReleaseChnFrame group %d channel %d Success!\n", VpssGrp, VpssChn);
            }

            if(g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum){
                s32Ret = AR_MPI_VPSS_GetRegionLuma(VpssGrp, VpssChn, (const VIDEO_REGION_INFO_S*)&g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn], pu64LumaData, 1000);
                if (AR_SUCCESS != s32Ret){
                    LOG_DEBUG("AR_MPI_VPSS_GetRegionLuma failed. s32Ret: 0x%x\n", s32Ret);
                }else{
                    LOG_DEBUG("AR_MPI_VPSS_GetRegionLuma success\n");
                    for(int i=0; i<g_aabChnRegionLumaVideoRgnInfo[VpssGrp][VpssChn].u32RegionNum; i++){
                        LOG_DEBUG("%lld ", *(pu64LumaData+i));
                    }
                    LOG_DEBUG("\n");
                }
            }
        } else {
            LOG_ERROR("AR_MPI_VPSS_GetChnFrame from group %d channel %d failed! ret=0x%x\n", VpssGrp, VpssChn, s32Ret);
        }
        if(g_bCaculateTime){
            AR_S64 time_end_ms, time_end_us;
            getTimeStemp(&time_end_ms, &time_end_us);
            LOG_INFO("===GET==== Group %d Channel %d, Process get frame cost %lld ms (%lld us)\n", \
                    VpssGrp, VpssChn, time_end_ms-time_start_ms, time_end_us-time_start_us);
        }
    }

    // while (g_aastThreadInfoOut[VpssGrp][VpssChn].bDestroy == AR_FALSE) {
    //         ;
    // }

    LOG_DEBUG("==================== STOP Thread of get frame from group %d channel %d \n", VpssGrp, VpssChn);

    if(fp != -1) close(fp);
    pthread_exit(0);

    if(pu64LumaData) free(pu64LumaData);

    return NULL;
}

static AR_VOID AR_SAMPLE_VPSS_GetFrameThd(AR_S32 grpNum, AR_S32 chnNum){
    g_bStopGetFrameThd = AR_FALSE;
    for(int i = 0; i < grpNum; i++){
        for (int j = 0; j < chnNum; j++)
        {
            VPSS_GRP vpssGrp = i;
            VPSS_CHN vpssChn = j;
            g_aastThreadInfoOut[vpssGrp][vpssChn].s32Group = vpssGrp;
            g_aastThreadInfoOut[vpssGrp][vpssChn].s32Channel = vpssChn;
            pthread_create(&(g_aastThreadInfoOut[vpssGrp][vpssChn].tid), NULL, AR_SAMPLE_VPSS_GetChnFrame, (AR_VOID*)&g_aastThreadInfoOut[vpssGrp][vpssChn]);
            usleep(5*1000);
        }
    }

    return;
}

int AR_SAMPLE_VPSS_ipc_callback(uint32_t cmd, ar_ipc_data_t* data, unsigned len, void* reply)
{
    AR_S32 s32Ret = AR_FAILURE;
    AR_BOOL reinit = AR_FALSE;
    SIZE_S oldSize, newSize;
    if(data == AR_NULL){
        LOG_ERROR("AR_SAMPLE_VPSS_ipc_callback data is NULL\n");
        return -1;
    }

    SAMPLE_VPSS_CONFIG_S *pstParam = (SAMPLE_VPSS_CONFIG_S *)data->blob;
    VPSS_GRP grpIndex = pstParam->grpId;
    VPSS_CHN chnIndex = pstParam->chnId;
    if(grpIndex < 0 || grpIndex >= g_groupNum){
        LOG_ERROR("AR_SAMPLE_VPSS_ipc_callback group index %d doesn't exist, range [0, %d]\n", grpIndex, g_groupNum - 1);
        return -1;
    }

    switch (cmd)
    {
        case CMD_VPSS_CONFIG_SCALE:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_SCALE\n");
            s32Ret = AR_SUCCESS;
            if(chnIndex < 0){
                if(pstParam->grpAttr.u32Width * pstParam->grpAttr.u32Height > g_astVpssGrpAttr[grpIndex].u32Width * g_astVpssGrpAttr[grpIndex].u32Height
                    && pstParam->grpAttr.u32Width * pstParam->grpAttr.u32Height > g_stOutSize.u32Width * g_stOutSize.u32Height) {
                    oldSize.u32Width = g_astVpssGrpAttr[grpIndex].u32Width;
                    oldSize.u32Height = g_astVpssGrpAttr[grpIndex].u32Height;
                    newSize.u32Width = pstParam->grpAttr.u32Width;
                    newSize.u32Height = pstParam->grpAttr.u32Height;
                    reinit = AR_TRUE;
                }
                g_astVpssGrpAttr[grpIndex].u32Width = pstParam->grpAttr.u32Width;
                g_astVpssGrpAttr[grpIndex].u32Height = pstParam->grpAttr.u32Height;
            } else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                if(pstParam->chnAttr.u32Width * pstParam->chnAttr.u32Height > g_aastVpssChnAttr[grpIndex][chnIndex].u32Width * g_aastVpssChnAttr[grpIndex][chnIndex].u32Height
                    && pstParam->chnAttr.u32Width * pstParam->chnAttr.u32Height > g_stOutSize.u32Width * g_stOutSize.u32Height) {
                    oldSize.u32Width = g_aastVpssChnAttr[grpIndex][chnIndex].u32Width;
                    oldSize.u32Height = g_aastVpssChnAttr[grpIndex][chnIndex].u32Height;
                    newSize.u32Width = pstParam->chnAttr.u32Width;
                    newSize.u32Height = pstParam->chnAttr.u32Height;
                    reinit = AR_TRUE;
                }
                g_aastVpssChnAttr[grpIndex][chnIndex].u32Width = pstParam->chnAttr.u32Width;
                g_aastVpssChnAttr[grpIndex][chnIndex].u32Height = pstParam->chnAttr.u32Height;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                if(pstParam->extChnAttr.u32Width * pstParam->extChnAttr.u32Height > g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Width * g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Height
                    && pstParam->extChnAttr.u32Width * pstParam->extChnAttr.u32Height > g_stOutSize.u32Width * g_stOutSize.u32Height) {
                    oldSize.u32Width = g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Width;
                    oldSize.u32Height = g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Height;
                    newSize.u32Width = pstParam->extChnAttr.u32Width;
                    newSize.u32Height = pstParam->extChnAttr.u32Height;
                    reinit = AR_TRUE;
                }
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Width = pstParam->extChnAttr.u32Width;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].u32Height = pstParam->extChnAttr.u32Height;
            } else {
                s32Ret = AR_FAILURE;
            }

            if(reinit && !g_bVpssStart)
            {
                if(AR_SAMPLE_VPSS_SYS_Init(AR_TRUE, &oldSize, &newSize, (chnIndex < 0)) != AR_SUCCESS) {
                    LOG_ERROR("AR_SAMPLE_VPSS_SYS_Init failed\n");
                    g_bVpssQuit = AR_TRUE;
                    s32Ret = AR_FAILURE;
                }
            }
            break;
        case CMD_VPSS_CONFIG_MIRROR:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_MIRROR\n");
            s32Ret = AR_SUCCESS;
            if(chnIndex < 0){
                g_astVpssGrpAttr[grpIndex].bMirror = pstParam->grpAttr.bMirror;
            } else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].bMirror = pstParam->chnAttr.bMirror;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].bMirror = pstParam->extChnAttr.bMirror;
            } else {
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_FLIP:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_FLIP\n");
            s32Ret = AR_SUCCESS;
            if(chnIndex < 0){
                g_astVpssGrpAttr[grpIndex].bFlip = pstParam->grpAttr.bFlip;
            } else if(chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].bFlip = pstParam->chnAttr.bFlip;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].bFlip = pstParam->extChnAttr.bFlip;
            } else {
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_CROP:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_CROP\n");
            VPSS_CROP_INFO_S cropInfo = pstParam->cropInfo;
            if (cropInfo.bEnable && cropInfo.enCropCoordinate == 0)
            {
                if (cropInfo.stCropRect.s32X < 0 || cropInfo.stCropRect.s32X > 999 ||
                cropInfo.stCropRect.s32Y < 0 || cropInfo.stCropRect.s32Y > 999 ||
                cropInfo.stCropRect.u32Width < 1 || cropInfo.stCropRect.u32Width > 1000 ||
                cropInfo.stCropRect.u32Height < 1 || cropInfo.stCropRect.u32Height > 1000 )
                {
                    LOG_ERROR("Error crop rect coordinates of type %d!\n", cropInfo.enCropCoordinate);
                    s32Ret = AR_FAILURE;
                    break;
                }
            }

            if(chnIndex < 0){
                memcpy(&g_astVpssGrpCrop[grpIndex], &cropInfo, sizeof(VPSS_CROP_INFO_S));
                s32Ret = AR_SUCCESS;
            } else if(chnIndex >= 0 && chnIndex < g_channelNum){
                if(chnIndex >= VPSS_MAX_PHY_CHN_NUM){
                    g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                }
                memcpy(&g_aastVpssChnCrop[grpIndex][chnIndex], &cropInfo, sizeof(VPSS_CROP_INFO_S));
                s32Ret = AR_SUCCESS;
            } else {
                LOG_ERROR("chnIndex %d doesn't exist\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_ROTATE:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_ROTATE\n");
            s32Ret = AR_SUCCESS;
            if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].enRotation = pstParam->rotateAngle;
            } else if(chnIndex < VPSS_MAX_CHN_NUM){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].enRotation = pstParam->rotateAngle;
            } else {
                LOG_ERROR("chnIndex %d doesn't exist\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_REGION_LUMA:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_REGION_LUMA\n");
            if(chnIndex >= 0 && chnIndex < g_channelNum){
                LOG_DEBUG("u32RegionNum=%d\n", pstParam->regionLuma.u32RegionNum);
                g_aabChnRegionLumaVideoRgnInfo[grpIndex][chnIndex].u32RegionNum = pstParam->regionLuma.u32RegionNum;
                if(pstParam->regionLuma.u32RegionNum) {
	                g_aabChnRegionLumaVideoRgnInfo[grpIndex][chnIndex].pstRegion = malloc(pstParam->regionLuma.u32RegionNum * sizeof(pstParam->regionLuma.pstRegion[0]));
	                if(!g_aabChnRegionLumaVideoRgnInfo[grpIndex][chnIndex].pstRegion) {
	                    g_aabChnRegionLumaVideoRgnInfo[grpIndex][chnIndex].u32RegionNum = 0;
	                    LOG_ERROR("malloc failed\n");
	                }
                }

                for(int i=0; i<pstParam->regionLuma.u32RegionNum; i++){
                    LOG_DEBUG("(%d, %d, %d, %d)\n", pstParam->regionLuma.pstRegion[i].s32X, \
                                                    pstParam->regionLuma.pstRegion[i].s32Y, \
                                                    pstParam->regionLuma.pstRegion[i].u32Width, \
                                                    pstParam->regionLuma.pstRegion[i].u32Height);
                    g_aabChnRegionLumaVideoRgnInfo[grpIndex][chnIndex].pstRegion[i] = pstParam->regionLuma.pstRegion[i];
                }
                s32Ret = AR_SUCCESS;
            } else {
                LOG_ERROR("chnIndex %d doesn't exist\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_PHY_CHN_LDC:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_PHY_CHN_LDC\n");
            if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].stLdcAttr.bEnable = pstParam->ldcAttr.bEnable;
                s32Ret = AR_SUCCESS;
            } else {
                LOG_ERROR("chnIndex %d must be an existing physical channel.\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_PHY_CHN_SPREAD:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_PHY_CHN_SPREAD\n");
            if(chnIndex >= 0 && chnIndex < VPSS_MAX_PHY_CHN_NUM){
                g_aastVpssChnAttr[grpIndex][chnIndex].stSpreadAttr.bEnable = pstParam->spreadAttr.bEnable;
                s32Ret = AR_SUCCESS;
            } else {
                LOG_ERROR("chnIndex %d must be an existing physical channel.\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
        case CMD_VPSS_CONFIG_EXT_CHN_FISHEYE:
            LOG_DEBUG("+++++++++ CMD_VPSS_CONFIG_EXT_CHN_FISHEYE\n");
            if(chnIndex >= VPSS_MAX_PHY_CHN_NUM && chnIndex < g_channelNum){
                g_aastVpssExtChnAttr[grpIndex][chnIndex].s32BindChn = pstParam->extChnAttr.s32BindChn;
                g_aastVpssExtChnAttr[grpIndex][chnIndex].stFisheyeAttr.bEnable = pstParam->fisheyeAttr.bEnable;
                s32Ret = AR_SUCCESS;
            } else {
                LOG_ERROR("chnIndex %d must be an existing extension channel.\n", chnIndex);
                s32Ret = AR_FAILURE;
            }
            break;
		case CMD_VPSS_SUSPEND:
			LOG_DEBUG("+++++++++ CMD_VPSS_SUSPEND\n");
			s32Ret = AR_MPI_SYSCTL_Suspend();
			if(s32Ret == AR_SUCCESS) {
				g_bVpssSuspend = AR_TRUE;
				g_bVpssResume = AR_FALSE;
	            g_bStopSendFrameThd = AR_TRUE;
	            g_bStopGetFrameThd = AR_TRUE;
            }
			break;
		case CMD_VPSS_FAST_SUSPEND:
			LOG_DEBUG("+++++++++ CMD_VPSS_FAST_SUSPEND\n");
			s32Ret = AR_MPI_SYSCTL_Fast_Suspend();
			if(s32Ret == AR_SUCCESS) {
				g_bVpssSuspend = AR_TRUE;
				g_bVpssResume = AR_FALSE;
	            g_bStopSendFrameThd = AR_TRUE;
	            g_bStopGetFrameThd = AR_TRUE;
			}
			break;
		case CMD_VPSS_FAST_RESUME:
			LOG_DEBUG("+++++++++ CMD_VPSS_FAST_RESUME\n");
			s32Ret = AR_MPI_SYSCTL_Fast_Resume();
			if(s32Ret == AR_SUCCESS) {
				g_bVpssSuspend = AR_FALSE;
				g_bVpssResume = AR_TRUE;
			}
			break;
        case CMD_VPSS_START:
            LOG_DEBUG("+++++++++ CMD_VPSS_START\n");
            g_bVpssStart = AR_TRUE;
            g_bVpssStop = AR_FALSE;
            s32Ret = AR_SUCCESS;
            break;
        case CMD_VPSS_STOP:
            LOG_DEBUG("+++++++++ CMD_VPSS_STOP\n");
            g_bVpssStop = AR_TRUE;
            g_bVpssStart = AR_FALSE;
            g_bStopSendFrameThd = AR_TRUE;
            g_bStopGetFrameThd = AR_TRUE;
            s32Ret = AR_SUCCESS;
            break;
        case CMD_VPSS_QUIT:
            LOG_DEBUG("+++++++++ CMD_VPSS_QUIT\n");
            g_bVpssQuit = AR_TRUE;
            g_bStopSendFrameThd = AR_TRUE;
            g_bStopGetFrameThd = AR_TRUE;
            s32Ret = AR_SUCCESS;
            break;
        default:
            LOG_ERROR("Unknown command %d\n", cmd);
            break;
    }

    switch (cmd)
    {
        case CMD_VPSS_CONFIG_SCALE:
        case CMD_VPSS_CONFIG_MIRROR:
        case CMD_VPSS_CONFIG_FLIP:
        case CMD_VPSS_CONFIG_CROP:
        case CMD_VPSS_CONFIG_ROTATE:
        case CMD_VPSS_CONFIG_PHY_CHN_LDC:
        case CMD_VPSS_CONFIG_PHY_CHN_SPREAD:
        case CMD_VPSS_CONFIG_EXT_CHN_FISHEYE:
            if(chnIndex >= 0 && chnIndex < g_channelNum && g_bVpssStart) {
                g_aabChnUpdateNeeded[grpIndex][chnIndex] |= 1UL << cmd;
            }
            break;
    }

    SAMPLE_VPSS_REPLY_S replyRet;
    replyRet.s32Ret = s32Ret;
    if(s32Ret == AR_SUCCESS){
        strcpy(replyRet.result, "SUCCESS");
    } else {
        strcpy(replyRet.result, "FAIL");
    }
    ar_ipc_return_buffer(reply, s32Ret, &replyRet, sizeof(SAMPLE_VPSS_REPLY_S));

    return 0;
}

static ar_ipc_state_t* AR_SAMPLE_VPSS_create_ipc_server()
{
    int ret;
    ar_ipc_state_t* ipc = ar_ipc_create();
    if (!ipc) {
        LOG_ERROR("failed to create server ipc");
        return NULL;
    }

    ret = ar_ipc_add_service(ipc, "vpss_control", AR_SAMPLE_VPSS_ipc_callback);
    if (ret) {
        LOG_ERROR("failed to publish service vpss_control\n");
        return NULL;
    }

    ar_ipc_start_thread_pool(ipc);

    return ipc;
}

static AR_S32 AR_SAMPLE_VPSS_Runtime_Update(AR_S32 grpNum, AR_S32 chnNum)
{
    VPSS_GRP VpssGrp = 0;
    VPSS_CHN VpssChn = 0;

    for(VpssGrp = 0; VpssGrp < grpNum; VpssGrp++)
    {
        for(VpssChn = 0; VpssChn < chnNum; VpssChn++)
        {
            if(g_aabChnUpdateNeeded[VpssGrp][VpssChn] & (1UL << CMD_VPSS_CONFIG_CROP))
            {
                AR_MPI_VPSS_SetChnCrop(VpssGrp, VpssChn, &g_aastVpssChnCrop[VpssGrp][VpssChn]);
                g_aabChnUpdateNeeded[VpssGrp][VpssChn] &= ~ (1UL << CMD_VPSS_CONFIG_CROP);
            }

            if(g_aabChnUpdateNeeded[VpssGrp][VpssChn])
            {
                if(VpssChn < VPSS_MAX_PHY_CHN_NUM)
                {
                    AR_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &g_aastVpssChnAttr[VpssGrp][VpssChn]);
                }
                else
                {
                    AR_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, &g_aastVpssExtChnAttr[VpssGrp][VpssChn]);
                }

                g_aabChnUpdateNeeded[VpssGrp][VpssChn] = 0;
            }
        }
    }
    return 0;
}

static AR_VOID signal_handler(int signum)
{
    LOG_INFO("recive signal SIGINT\n");
    g_bVpssQuit = AR_TRUE;

    return;
}

static AR_VOID usage(char* argv0)
{
    printf("Usage:\n");
    printf("%s -f <yuv file> 1920 1080 -g 1 -c 4 -o 1280 720 -O /mnt\n", argv0);
    printf("Options:\n");
    printf("    -f  <string> <int> <int>\n");
    printf("            Input yuv file parameters: filename width height\n");
    printf("    -g  <int>\n");
    printf("            Number of group, default 1, range [1, %d]\n", VPSS_MAX_GRP_NUM);
    printf("    -c  <int>\n");
    printf("            Number of channel of each group, default 1, range [1, %d]\n", VPSS_MAX_CHN_NUM);
    printf("    -o  <int> <int>\n");
    printf("            Output resolution parameters: width height, default is the same as width and height of the input file\n");
    printf("    -O  <string>\n");
    printf("            Output path, default /tmp\n");
    printf("    -s  <No arg>\n");
    printf("            Start VPSS process\n");
    printf("    -t  <No arg>\n");
    printf("            Caculate time\n");
    printf("    -n  <No arg>\n");
    printf("            Do not write output frame to file\n");
    printf("    -d  <No arg>\n");
    printf("            Show debug message\n");
    printf("    -S  <No arg>\n");
    printf("            Special test for constant\n");
    printf("    -h  <No arg>\n");
    printf("            Show help message\n");

    return;
}

static int get_abs_path_name(char* fileName, char* inname)
{
	if(inname[0] == '/')
	{
		if(strlen(inname) >= MAX_NAME_LEN)
		{
			LOG_ERROR("input file name %s too long\n", inname);
			return -1;
		}
		strcpy(fileName, inname);
		return 0;
	}

    char curpath[MAX_NAME_LEN] = {0};

	if(!getcwd(curpath, MAX_NAME_LEN))
	{
		LOG_ERROR("current path too long\n");
		return -1;
	}

	if(strlen(curpath) + 1 + strlen(inname) >= MAX_NAME_LEN)
	{
		LOG_ERROR("abs path file name too long\n");
		return -1;
	}
	sprintf(fileName, "%s/%s", curpath, inname);
	return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 5){
        usage(argv[0]);
        return -1;
    }

    char fileName[MAX_NAME_LEN] = {0};
    strcpy(g_save_path, SAVE_PATH);
    SIZE_S       stSrcSize = {0};   // Input width and height
    AR_S32       groupNum = 1;      // group total number
    AR_S32       channelNum = 1;    // channel number of each group
    SIZE_S       stOutSize = {0};   // Output width and height


    int opt;
    // while ((opt=getopt(argc, argv, "f:w:h:g:c:W:H:o:stnd")) != -1)
    while ((opt=getopt(argc, argv, "f:g:c:o:O:stnSdh")) != -1)
    {
        switch (opt) {
            case 'f':
                if(get_abs_path_name(fileName, optarg) < 0)
                {
                    LOG_ERROR("file name %s error\n", optarg);
                    exit(1);
                }
                stSrcSize.u32Width = atoi(argv[optind]);
                stSrcSize.u32Height = atoi(argv[optind+1]);
                break;
            case 'g':
                groupNum = atoi(optarg);
                break;
            case 'c':
                channelNum = atoi(optarg);
                break;
            case 'o':
                stOutSize.u32Width = atoi(optarg);
                stOutSize.u32Height = atoi(argv[optind]);
                break;
            case 'O':
                if(get_abs_path_name(g_save_path, optarg) < 0)
                {
                    LOG_ERROR("file name %s error\n", optarg);
                    exit(1);
                }
                break;
            case 's':
                g_bVpssStart = AR_TRUE;
                g_bVpssStop = AR_FALSE;
                break;
            case 't':
                g_bCaculateTime = AR_TRUE;
                break;
            case 'n':
                g_bWriteOutFrame = AR_FALSE;
                break;
            case 'd':
                g_bDebug = AR_TRUE;
                break;
            case 'S':
                g_bSpec = AR_TRUE;
                break;
            case 'h':
                usage(argv[0]);
                exit(1);
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    if(groupNum < 1 || groupNum > VPSS_MAX_GRP_NUM || channelNum < 1 || channelNum > VPSS_MAX_CHN_NUM){
        LOG_ERROR("Group numbers %d or channel numbers %d are out of range!", groupNum, channelNum);
        usage(argv[0]);
        return -1;
    }

    if(access(fileName, R_OK) != 0){
        LOG_ERROR("Cannot access file %s!\n", fileName);
        usage(argv[0]);
        return -1;
    }

    if(stSrcSize.u32Width < 60 || stSrcSize.u32Height < 60){
        LOG_ERROR("Input width %d and height %d is not supported!\n", stSrcSize.u32Width, stSrcSize.u32Height);
        usage(argv[0]);
        return -1;
    }

    if(stOutSize.u32Width > 3840 || stOutSize.u32Height > 3840){
        LOG_ERROR("Output width %d and height %d is not supported!\n", stSrcSize.u32Width, stSrcSize.u32Height);
        usage(argv[0]);
        return -1;
    }

    struct stat s_buf;
    stat(g_save_path, &s_buf);
    if(!S_ISDIR(s_buf.st_mode)){
        LOG_ERROR("%s is not a dir\n", g_save_path);
        usage(argv[0]);
        return -1;
    }

    if(stOutSize.u32Width == 0 || stOutSize.u32Height == 0){
        stOutSize.u32Width = stSrcSize.u32Width;
        stOutSize.u32Height = stSrcSize.u32Height;
    }

    g_stSrcSize = stSrcSize;
    g_stOutSize = stOutSize;

    if(stOutSize.u32Width > stSrcSize.u32Width
        || stOutSize.u32Height > stSrcSize.u32Height)
    {
        if(g_bSpec) {
            LOG_DEBUG("-S doesnt support enlarge src(%d %d) -> out (%d %d)\n", stSrcSize.u32Width, stSrcSize.u32Height, stOutSize.u32Width, stOutSize.u32Height);
            g_bSpec = AR_FALSE;
        }
    }

    g_groupNum = groupNum;
    g_channelNum = channelNum;

    signal(SIGINT, signal_handler);

    /* prepare group and channel attributes */
    if(AR_SAMPLE_VPSS_Prepare(groupNum, channelNum, stOutSize) != AR_SUCCESS){
        usage(argv[0]);
        return -1;
    }

    ar_ipc_state_t* ipc = NULL;
    ipc = AR_SAMPLE_VPSS_create_ipc_server();
    if (ipc) {
        LOG_INFO("AR_SAMPLE_VPSS_create_ipc_server Success.\n");
    } else {
        LOG_ERROR("AR_SAMPLE_VPSS_create_ipc_server Failed!\n");
    }

    /* SYS Init */
    if(!g_bSysInited)
    {
        if(AR_SAMPLE_VPSS_SYS_Init(AR_FALSE, NULL, NULL, AR_FALSE) != AR_SUCCESS){
            LOG_ERROR("AR_SAMPLE_VPSS_SYS_Init failed\n");
            goto end;
        }
        g_bSysInited = AR_TRUE;
    }

    /* initialize mmap */
    ar_mmap_init();

    g_bVpssQuit = AR_FALSE;

    while(!g_bVpssQuit)
    {
        /* wait for vpss start/resume signal */
        if(g_bVpssSuspend)
        {
	        printf("==================== waiting for resume signal...\n");
	        while (!g_bVpssResume)
	        {
	            if(g_bVpssQuit)
	            {
	                goto end1;
	            }
	        }
        }
        else
        {
	        printf("==================== waiting for start signal...\n");

	        while (!g_bVpssStart)
	        {
	            if(g_bVpssQuit)
	            {
	                goto end1;
	            }
	        }

			/* VPSS Start */
			AR_SAMPLE_VPSS_Start(groupNum, channelNum);
        }

        /* Start Send Frame Thread */
        AR_SAMPLE_VPSS_SendFrameThd(groupNum, fileName, &stSrcSize);

        //usleep(50*1000);

        /* Start Get Frame Thread */
        AR_SAMPLE_VPSS_GetFrameThd(groupNum, channelNum);

        printf("==================== waiting for stop/suspend/quit signal...\n");
        while (!g_bVpssStop && !g_bVpssQuit
			&& !g_bVpssSuspend
        )
        {
            AR_SAMPLE_VPSS_Runtime_Update(groupNum, channelNum);
        }

        if(g_bVpssSuspend)
        {
	        printf("====================suspend process...\n");
        }
        else
        {
	        printf("==================== stop process...\n");

	        AR_SAMPLE_VPSS_Stop(groupNum, channelNum);
	        LOG_DEBUG("==================== grop and channel stopped\n");
        }
    }

end1:
    AR_SAMPLE_VPSS_DestroyAll(groupNum);
    ar_ipc_stop_thread_pool(ipc);
    ar_ipc_close(ipc);
    ar_mmap_clear();

end:
    AR_SAMPLE_VPSS_SYS_Exit();
    return 0;
}
