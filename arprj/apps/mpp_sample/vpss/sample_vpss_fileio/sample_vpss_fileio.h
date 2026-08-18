#ifndef __TEST_VPSS_H__
#define __TEST_VPSS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <getopt.h>
#include <stdarg.h>
#include <signal.h>
#include "sample_comm.h"
#include "binder_ipc.h"

#define RED     "\033[31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define NONE    "\033[0m"

AR_VOID vpss_log_func(const char *level, const char *file, int line, const char *func, const char *format, ...);
#define LOG_PRINT(level, ...) vpss_log_func(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LOG_DEBUG(...) LOG_PRINT("DEBUG", __VA_ARGS__);
#define LOG_ERROR(...) LOG_PRINT("ERROR", __VA_ARGS__);
#define LOG_INFO(...)  LOG_PRINT("INFO", __VA_ARGS__);

typedef enum arSAMPLE_VPSS_TEST_MASK_E
{
    VPSS_TEST_CROP = 1 << 0,
    VPSS_TEST_ROTATION  = 1 << 1,
    VPSS_TEST_MIRROR  = 1 << 2,
    VPSS_TEST_FLIP  = 1 << 3
} SAMPLE_VPSS_TEST_MASK_E;

typedef struct stSAMPLE_VPSS_ThreadCtrl_Info_In {
    AR_CHAR         srcFile[1024];
    AR_U32          u32Width;
    AR_U32          u32Height;
    PIXEL_FORMAT_E  enPixelFmt;
    VPSS_GRP        s32ToGroup;
    AR_BOOL         bGrpEnable;
    AR_U32          u32BlkSize;
    pthread_t       tid;
} ThreadCtrl_Info_In;

typedef struct stSAMPLE_VPSS_ThreadCtrl_Info_Out {
    VPSS_GRP        s32Group;
    VPSS_CHN        s32Channel;
    pthread_t       tid;

} ThreadCtrl_Info_Out;

typedef struct yuv_fmt_map
{
    PIXEL_FORMAT_E      enPixelFmt;
    AR_CHAR             formatName[16];
} STRU_PIXEL_FORMAT_MAP;

#define SAMPLE_VIDEO_REGION_MAX_CNT   16
typedef struct arSAMPLE_VIDEO_REGION_INFO_S
{
    AR_U32           u32RegionNum;       /* W; count of the region */
    RECT_S ATTRIBUTE pstRegion[SAMPLE_VIDEO_REGION_MAX_CNT];         /* W; region attribute */
} SAMPLE_VIDEO_REGION_INFO_S;

typedef struct
{
	AR_S32 s32_dev_id;
	AR_S32 s32_chn_id;
	AR_U32 u32_walign;
	AR_U32 u32_halign;

	AR_VOID* resv;
} STRU_EIS_PROCESS_PARA;


#define AR_DEV_MEM          ("/dev/mem")
#define AR_MMAP_MAGIC       (0x53535353U)

#define round_boundary(value, boundary) \
    ((__typeof__(value))((boundary) - 1))

#define round_up(value, boundary)       \
    ((((value) - 1) | round_boundary(value, boundary)) + 1)

#define round_down(value, boundary)     \
    ((value) & ~round_boundary(value, boundary))


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

#define ALIGN_BACK(x, a)        ((a) * (((x + a -1) / (a))))
#define ALIGN_UP(x, a)           ( ( ((x) + ((a) - 1) ) / a ) * a )

typedef unsigned long AR_PHYS_ADDR;

__inline static AR_VOID AR_SAMPLE_VPSS_GetPicBufferConfig(AR_U32 u32Width, AR_U32 u32Height,PIXEL_FORMAT_E enPixelFormat,
            DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, AR_U32 u32Align, VB_CAL_CONFIG_S* pstCalConfig)
{
    AR_U32 u32BitWidth = 0;
    AR_U32 u32HeadStride  = 0;
    AR_U32 u32VBSize      = 0;
    AR_U32 u32HeadSize    = 0;
    AR_U32 u32AlignHeight = 0;
    AR_U32 u32MainStride  = 0;
    AR_U32 u32MainSize    = 0;
    AR_U32 u32ExtStride   = 0;
    AR_U32 u32ExtSize     = 0;
    AR_U32 u32ExtYSize    = 0;
    AR_U32 u32HeadYSize   = 0;
    AR_U32 u32YSize       = 0;
    AR_U32 u32PaddingSize       = 8192*3;

    /* u32Align: 0 is automatic mode, alignment size following system. Non-0 for specified alignment size */
    if(0 == u32Align)
    {
        u32Align = DEFAULT_ALIGN;
    }
    else if(u32Align > MAX_ALIGN)
    {
        u32Align = MAX_ALIGN;
    }
    else
    {
        u32Align = (ALIGN_UP(u32Align, DEFAULT_ALIGN));
    }

    switch (enBitWidth)
    {
        case DATA_BITWIDTH_8:
        {
            u32BitWidth = 8;
            break;
        }
        case DATA_BITWIDTH_16:
        {
            u32BitWidth = 16;
            break;
        }
        default:
        {
            u32BitWidth = 0;
            break;
        }
    }

    u32AlignHeight = ALIGN_UP(u32Height, 64);

    if (COMPRESS_MODE_NONE == enCmpMode)
    {
        u32MainStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, u32Align);
        u32YSize = u32MainStride * u32AlignHeight;

        if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
        {
            u32MainSize = (u32MainStride * u32AlignHeight)*3 >> 1;
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
        {
            u32MainSize = u32MainStride * u32AlignHeight * 2;
        }
        else if ((PIXEL_FORMAT_YUV_400 == enPixelFormat) || (PIXEL_FORMAT_S16C1 == enPixelFormat))
        {
            u32MainSize = u32MainStride * u32AlignHeight;
        }
        else
        {
            u32MainSize = u32MainStride * u32AlignHeight * 3;
        }

        u32VBSize   = u32MainSize;
    }
    else
    {
        if (u32Width <= 4096)
        {
            u32HeadStride = 16;
        }
        else if (u32Width <= 8192)
        {
            u32HeadStride = 32;
        }
        else
        {
            u32HeadStride = 64;
        }

        if (u32BitWidth == 8)
        {
            u32MainStride  = ALIGN_UP(u32Width, u32Align);
            u32HeadYSize   = u32HeadStride * u32AlignHeight;
            u32YSize       = u32MainStride * u32AlignHeight;

            if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat)
            {
                u32HeadSize = (u32HeadStride * u32AlignHeight * 3) >> 1;
                u32MainSize = (u32MainStride * u32AlignHeight * 3) >> 1;
            }
            else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 2;
                u32MainSize = u32MainStride * u32AlignHeight * 2;
            }
            else if (PIXEL_FORMAT_YUV_400 == enPixelFormat)
            {
                u32HeadSize = u32HeadStride * u32AlignHeight;
                u32MainSize = u32MainStride * u32AlignHeight;
            }
            else
            {
                u32HeadSize = u32HeadStride * u32AlignHeight * 3;
                u32MainSize = u32MainStride * u32AlignHeight * 3;
            }
        }
        else
        {
//            u32VBSize     = 0;
            u32HeadYSize  = 0;
            u32HeadSize   = 0;
            u32HeadStride = 0;
            u32MainStride = 0;
            u32YSize      = 0;
            u32MainSize   = 0;
            u32ExtStride  = 0;
            u32ExtYSize   = 0;
        }

        u32HeadSize = ALIGN_UP(u32HeadSize, u32Align);

        u32VBSize = u32HeadSize + u32MainSize + u32ExtSize;
    }

    pstCalConfig->u32VBSize     = u32VBSize+u32PaddingSize;
    pstCalConfig->u32HeadYSize  = u32HeadYSize;
    pstCalConfig->u32HeadSize   = u32HeadSize;
    pstCalConfig->u32HeadStride = u32HeadStride;
    pstCalConfig->u32MainStride = u32MainStride;
    pstCalConfig->u32MainYSize  = u32YSize;
    pstCalConfig->u32MainSize   = u32MainSize;
    pstCalConfig->u32ExtStride  = u32ExtStride;
    pstCalConfig->u32ExtYSize   = u32ExtYSize;

    return;
}

__inline static AR_U32 AR_SAMPLE_VPSS_GetPicBufferSize(AR_U32 u32Width, AR_U32 u32Height,
        PIXEL_FORMAT_E enPixelFormat, DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, AR_U32 u32Align)
{
    VB_CAL_CONFIG_S stCalConfig;

    AR_SAMPLE_VPSS_GetPicBufferConfig(u32Width, u32Height,enPixelFormat,enBitWidth, enCmpMode, u32Align, &stCalConfig);

    return stCalConfig.u32VBSize;
}

typedef enum arSAMPLE_VPSS_CONFIG_CMD_E{
    CMD_VPSS_CONFIG_NONE = 0,
    CMD_VPSS_CONFIG_SCALE = 1,
    CMD_VPSS_CONFIG_MIRROR,
    CMD_VPSS_CONFIG_FLIP,
    CMD_VPSS_CONFIG_CROP,
    CMD_VPSS_CONFIG_ROTATE,
    CMD_VPSS_CONFIG_REGION_LUMA,
    CMD_VPSS_CONFIG_PHY_CHN_LDC,
    CMD_VPSS_CONFIG_PHY_CHN_SPREAD,
    CMD_VPSS_CONFIG_EXT_CHN_FISHEYE,
    CMD_VPSS_SUSPEND,
    CMD_VPSS_FAST_SUSPEND,
    CMD_VPSS_FAST_RESUME,
    CMD_VPSS_START,
    CMD_VPSS_STOP,
    CMD_VPSS_QUIT,
    CMD_VPSS_CONFIG_BUTT
} SAMPLE_VPSS_CONFIG_CMD_E;

typedef struct arSAMPLE_VPSS_CONFIG_S{
    SAMPLE_VPSS_CONFIG_CMD_E    cmd;
    VPSS_GRP                    grpId;
    VPSS_CHN                    chnId;
    VPSS_GRP_ATTR_S             grpAttr;
    VPSS_CHN_ATTR_S             chnAttr;
    VPSS_EXT_CHN_ATTR_S         extChnAttr;
    VPSS_CROP_INFO_S            cropInfo;
    ROTATION_E                  rotateAngle;
    VPSS_LDC_ATTR_S             ldcAttr;
    VPSS_SPREAD_ATTR_S          spreadAttr;
    VPSS_FISHEYE_ATTR_S         fisheyeAttr;
    AR_BOOL                     suspendVpss;
    AR_BOOL                     fastSuspendVpss;
    AR_BOOL                     fastResumeVpss;
    AR_BOOL                     startVpss;
    AR_BOOL                     stopVpss;
    AR_BOOL                     quitVpss;
    SAMPLE_VIDEO_REGION_INFO_S  regionLuma;
} SAMPLE_VPSS_CONFIG_S;

typedef struct arSAMPLE_VPSS_REPLY_S{
    char result[16];
    AR_S32 s32Ret;
} SAMPLE_VPSS_REPLY_S;

AR_VOID AR_SAMPLE_VPSS_SYS_Exit(void);

#ifdef __cplusplus
}
#endif
#endif /* End of #ifndef __TEST_VPSS_H__*/
