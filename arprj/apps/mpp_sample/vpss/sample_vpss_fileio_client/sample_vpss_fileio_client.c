#include "sample_vpss_fileio.h"

AR_BOOL bDebug = AR_FALSE;

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
            mapped->real_start_va	= real_start_va;
            mapped->real_sz			= length;
            mapped->magic			= AR_MMAP_MAGIC;

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


AR_VOID vpss_log_func(const char *level, const char *file, int line, const char *func, const char *format, ...){
    va_list args;
    va_start(args, format);
    char fmt[2048];
    vsnprintf(fmt, 2048, format, args);
    va_end(args);
    if(strcmp(level, "ERROR") == 0){
        printf("%s[%s]%s %s:%d %s(): ", RED, level, NONE, file, line, func);
        printf("%s", fmt);
    } else if(strcmp(level, "DEBUG") == 0 && bDebug) {
        printf("%s[%s]%s %s:%d %s(): ", YELLOW, level, NONE, file, line, func);
        printf("%s", fmt);
    } else if(strcmp(level, "INFO") == 0){
        printf("%s[%s ]%s %s:%d %s(): ", CYAN, level, NONE, file, line, func);
        printf("%s", fmt);
    }
}

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

AR_CHAR* pixel_fmt_to_name(PIXEL_FORMAT_E p_pixel_fmt)
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

AR_S32 AR_SAMPLE_VPSS_ClientSaveChnFrame(VIDEO_FRAME_INFO_S* frame, AR_S32 fp)
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
            void *y_virt_addr = p;
            for(int i = 0; i < height; i++){
                s32Ret = write(fp, y_virt_addr, width);
                if(s32Ret != width) LOG_ERROR("STORE y component failed!\n");
                y_virt_addr += stride0;
            }
            p+=y_size;

            void *u_virt_addr = p;
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, u_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE u component failed!\n");
                u_virt_addr += stride1;
            }
            p+=u_size;

            void *v_virt_addr = p;
            for(int i = 0; i < height/2; i++){
                s32Ret = write(fp, v_virt_addr, width/2);
                if(s32Ret != width/2) LOG_ERROR("STORE v component failed!\n");
                v_virt_addr += stride2;
            }
        } else {
            LOG_ERROR("mapped virtual address is NULL!\n");
			free(pData);
            return -1;
        }
        ar_munmap(hdl);
        free(pData);
    }

    return AR_SUCCESS;
}

AR_BOOL g_bClientStop = AR_FALSE;

static AR_VOID signal_handler(int signum)
{
    LOG_INFO("recive signal SIGINT\n");
    g_bClientStop = AR_TRUE;

    return;
}

void  usage(char *argv0)
{
    printf("Usage : %s <group index> <channel index> [save path] [debug]\n", argv0);

    return;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        usage(argv[0]);
        return AR_FAILURE;
    }
    AR_CHAR save_path[MAX_NAME_LEN] = "/tmp";
    VPSS_GRP VpssGrp = atoi(argv[1]);
    VPSS_CHN VpssChn = atoi(argv[2]);

    printf("argc = %d, %s %s %s %s %s\n", argc, argv[0], argv[1], argv[2], argv[3], argv[4]);

    if (argc > 3)
    {
    	strncpy(save_path, argv[3], MAX_NAME_LEN-1);
    	save_path[MAX_NAME_LEN-1] = 0;
    }
    if (argc == 5 && strcmp(argv[4], "debug") == 0 ) bDebug = AR_TRUE;

    struct stat s_buf;
    stat(save_path, &s_buf);
    if(!S_ISDIR(s_buf.st_mode)){
        LOG_ERROR("out dir %s does not exist.\n", save_path);
        usage(argv[0]);
        return -1;
    }

    signal(SIGINT, signal_handler);

    AR_S32 s32Ret = AR_FAILURE;
    AR_S32 fp = -1;
    char saveFileName[MAX_NAME_LEN] = {0};
    AR_S32 widthTmp = -1;
    AR_S32 heightTmp = -1;

    ar_mmap_init();

    /*** test_vpss should be run first, then you can dump vpss chanel frame.****/
    int count = 0;
    while (!g_bClientStop)
    {
        VIDEO_FRAME_INFO_S stFrame;
        memset(&stFrame, 0, sizeof(VIDEO_FRAME_INFO_S));
        s32Ret = AR_MPI_VPSS_GetChnFrame(VpssGrp, VpssChn, &stFrame, -1);
        if(AR_SUCCESS == s32Ret){
            AR_S32 frameWidth  = stFrame.stVFrame.u32Width;
            AR_S32 frameHeight = stFrame.stVFrame.u32Height;
            AR_S32 frameFormat = stFrame.stVFrame.enPixelFormat;

            char *formatName = pixel_fmt_to_name(frameFormat);
            AR_S32 stride0 = stFrame.stVFrame.u32Stride[0];
            AR_S32 stride1 = stFrame.stVFrame.u32Stride[1];
            AR_S32 stride2 = stFrame.stVFrame.u32Stride[2];

            count++;
            LOG_DEBUG("AR_MPI_VPSS_GetChnFrame from group %d channel %d success! total frames %d, width %d height %d, format %s, stride %d %d %d\n", \
                    VpssGrp, VpssChn, count, frameWidth, frameHeight, formatName, stride0, stride1, stride2);

            if(widthTmp != frameWidth || heightTmp != frameHeight){
                if(fp != -1){
                    close(fp);
                }
                sprintf(saveFileName, "%s/test_vpss_client_grp%d_chn%d_%dx%d_Y-stride_%d_%s.yuv", \
                        save_path, VpssGrp, VpssChn, frameWidth, frameHeight, stride0, formatName);
                fp = open(saveFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
                if (fp < 0) {
                    LOG_ERROR("create output file %s failed! %s", saveFileName, strerror(errno));
                }
                widthTmp = frameWidth;
                heightTmp = frameHeight;
            }

            if(fp != -1){
                s32Ret = AR_SAMPLE_VPSS_ClientSaveChnFrame(&stFrame, fp);
                if(AR_SUCCESS == s32Ret){
                    LOG_DEBUG("Write frame to file %s Success!\n", saveFileName);
                } else {
                    LOG_ERROR("Write frame to file %s failed! s32Ret=%x\n", saveFileName, s32Ret);
                }
            }

            s32Ret = AR_MPI_VPSS_ReleaseChnFrame(VpssGrp, VpssChn, &stFrame);
            if(AR_SUCCESS != s32Ret){
                LOG_ERROR("AR_MPI_VPSS_ReleaseChnFrame group %d channel %d failed! ret=%x\n", VpssGrp, VpssChn, s32Ret);
            } else {
                LOG_DEBUG("AR_MPI_VPSS_ReleaseChnFrame group %d channel %d Success!\n", VpssGrp, VpssChn);
            }
        } else {
            LOG_ERROR("AR_MPI_VPSS_GetChnFrame from group %d channel %d failed! ret=%x\n", VpssGrp, VpssChn, s32Ret);
        }
    }

	if(fp != -1){
		close(fp);
		fp = -1;
	}
    ar_mmap_clear();

    return 0;

}
