#define ENABALE_ALL_LOG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uvc_service2.h"
#include "pipeline_service.h"
#include "libyuv.h"
#include "mpi_vpss.h"
#include "mpi_vi.h"
#include "mpi_sys.h"
#include "hal_sys.h"

struct yuv_priv_data
{
    unsigned int    index;
    unsigned char   buffer_status[BUFFER_COUNT];
    unsigned char * yuv_buffer[BUFFER_COUNT];
    unsigned char * cache_buf;
    unsigned long long  cache_buf_pa;
};

static int uvc_z16_init(uvc_format_data_t *data, void * handle, int argc, char *argv[])
{
    uvc_server_t       *server_l        = get_uvc_server();

    if(NULL == data || NULL != data->vs_handle)
    {
        log_err("param check error!\n");
        return -1;
    }

    ar_uvc_init(server_l->handle, argc, argv);

    //log_always("pid %d set ar_picture min align %d success %d!\n", getpid(), server_l->g_max_buffer_size, ret);

    return 0;
}

static int uvc_z16_deinit(uvc_format_data_t *data, void * handle)
{
    uvc_server_t       *server_l        = get_uvc_server();

    if(NULL == data)
    {
        log_err("param check error!\n");
        return -1;
    }

    ar_uvc_deinit(server_l->handle);

    return 0;
}

static struct uvc_frame_buf_info * uvc_z16_get_frame(uvc_format_data_t *data)
{
    struct uvc_frame_buf_info      *buf_info = NULL;
    VIDEO_FRAME_INFO_S             *pstFrameInfo;
    uvc_server_t                   *server_l        = get_uvc_server();

    if(NULL == data)
    {
        log_err("uvc_format_data is NULL or error!\n");
        return NULL;
    }

    buf_info = (struct uvc_frame_buf_info *)malloc(sizeof(struct uvc_frame_buf_info));
    if(NULL == buf_info)
    {
        log_err("malloc buf_info failed!\n");
        return NULL;
    }

    pstFrameInfo = (VIDEO_FRAME_INFO_S *)malloc(sizeof(VIDEO_FRAME_INFO_S));
    if(NULL == buf_info)
    {
        log_err("malloc buf_info failed!\n");
        free(buf_info);
        return NULL;
    }

    memset(buf_info, 0, sizeof(struct uvc_frame_buf_info));
    memset(pstFrameInfo, 0, sizeof(VIDEO_FRAME_INFO_S));

    if(AR_MPI_VI_GetChnFrame(server_l->cam_index, 2, pstFrameInfo, -1))
    {
        log_err("AR_MPI_VI_GetChnFrame failed!\n");
        goto FAILED;
    }

    buf_info->addr = (unsigned long)pstFrameInfo->stVFrame.u64VirAddr[0];
    buf_info->len = pstFrameInfo->stVFrame.u32Stride[0] * pstFrameInfo->stVFrame.u32Height;
    buf_info->pts = pstFrameInfo->stVFrame.u64PTS;
    buf_info->ptr = pstFrameInfo;

    printf("0x%lx %u %u %u %u %lu\n", buf_info->addr, pstFrameInfo->stVFrame.u32Stride[0], 
        pstFrameInfo->stVFrame.u32Width, pstFrameInfo->stVFrame.u32Height, 
        buf_info->len, buf_info->pts);

    return buf_info;

FAILED:

    free(pstFrameInfo);
    free(buf_info);

    return NULL;
}

static int uvc_z16_release_frame(uvc_format_data_t *data, struct uvc_frame_buf_info * buf_info)
{
    VIDEO_FRAME_INFO_S             *pstFrameInfo;
    uvc_server_t                   *server_l        = get_uvc_server();
    
    if(NULL == data || NULL == buf_info || NULL == buf_info->ptr)
    {
        log_err("param check failed!\n");
        return -1;
    }

    pstFrameInfo = (VIDEO_FRAME_INFO_S *)buf_info->ptr;

    if(AR_MPI_VI_ReleaseChnFrame(server_l->cam_index, 2, pstFrameInfo))
    {
        log_err("Z16 buffer release error!\n");
    }
    free(pstFrameInfo);

    free(buf_info);

    return 0;
}


static int uvc_yuy2_deinit(uvc_format_data_t *data, void * handle);

static int uvc_yuy2_init(uvc_format_data_t *data, void * handle, int argc, char *argv[])
{
    int                     i;
    uvc_server_t           *server_l        = get_uvc_server();
    struct yuv_priv_data   *priv_data       = NULL;
    

    if(1 == server_l->is_raw &&
        (V4L2_PIX_FMT_YUYV == server_l->dev->fcc || V4L2_PIX_FMT_Z16 == server_l->dev->fcc))
    {
        return uvc_z16_init(data, handle, argc, argv);
    }
    
    if(NULL == data || NULL != data->vs_handle)
    {
        log_err("param check error!\n");
        return -1;
    }

    ar_uvc_init(server_l->handle, argc, argv);

    priv_data = (struct yuv_priv_data *)malloc(sizeof(struct yuv_priv_data));
    if(NULL == priv_data)
    {
        log_err("yuv priv_data malloc failed!\n");
        goto PRIV_DATA_ERROR;
    }

    memset(priv_data, 0, sizeof(struct yuv_priv_data));

    for(i = 0; i < BUFFER_COUNT; ++i)
    {
        priv_data->yuv_buffer[i] = (unsigned char *)malloc(server_l->g_max_buffer_size * sizeof(unsigned char));
        if(NULL == priv_data->yuv_buffer[i])
        {
            log_err("yuv buffer malloc failed!\n");
            goto BUFFER_ERROR;
        }
    }

    if(AR_MPI_SYS_MmzAlloc_Cached(&priv_data->cache_buf_pa, (void **)&priv_data->cache_buf, "uvc cache", NULL, server_l->g_max_buffer_size))
    {
        log_err("AR_MPI_SYS_MmzAlloc_Cached failed!\n");
        goto BUFFER_ERROR;
    }

    priv_data->index = 0;
    data->priv = priv_data;

    return 0;

BUFFER_ERROR:
    for(i = 0; i < BUFFER_COUNT; ++i)
    {
        if(NULL != priv_data->yuv_buffer[i])
        {
            free(priv_data->yuv_buffer[i]);
            priv_data->yuv_buffer[i] = NULL;
        }
    }

    free(priv_data);
    priv_data = NULL;

PRIV_DATA_ERROR:
    uvc_yuy2_deinit(data, handle);

    return -1;
}

static int uvc_yuy2_deinit(uvc_format_data_t *data, void * handle)
{
    uvc_server_t       *server_l        = get_uvc_server();
    struct yuv_priv_data   *priv_data   = NULL;
    int                 i;

    if(1 == server_l->is_raw &&
        (V4L2_PIX_FMT_YUYV == server_l->dev->fcc || V4L2_PIX_FMT_Z16 == server_l->dev->fcc))
    {
        return uvc_z16_deinit(data, handle);
    }

    if(NULL == data || NULL == data->priv)
    {
        log_err("param check error!\n");
        return -1;
    }

    priv_data = (struct yuv_priv_data *)data->priv;

    AR_MPI_SYS_MmzFree(priv_data->cache_buf_pa, priv_data->cache_buf);

    for(i = 0; i < BUFFER_COUNT; ++i)
    {
        if(NULL != priv_data->yuv_buffer[i])
        {
            free(priv_data->yuv_buffer[i]);
            priv_data->yuv_buffer[i] = NULL;
        }
    }

    free(data->priv);
    data->priv = NULL;

    data->vs_handle = NULL;
    ar_uvc_deinit(server_l->handle);

    return 0;
}

int get_free_buffer(struct yuv_priv_data * priv)
{
    int i, ret = -1;

    for(i = 0; i < BUFFER_COUNT; ++i)
    {
        if(0 == priv->buffer_status[i])
        {
            ret = i;
            priv->buffer_status[i] = 1;
            break;
        }
    }

    return ret;
}

static struct uvc_frame_buf_info * uvc_yuy2_get_frame(uvc_format_data_t *data)
{
    struct uvc_frame_buf_info       *buf_info           = NULL;
    VIDEO_FRAME_INFO_S              stVideoFrame;
    STRU_SYS_DMA_TRAN_INFO          dma_info = {0};
    struct yuv_priv_data            *priv_data          = NULL;
    uvc_server_t                    *server_l           = get_uvc_server();

    unsigned char                   *y = NULL;
    unsigned char                   *u = NULL;
    unsigned char                   *v = NULL;

    if(1 == server_l->is_raw &&
        (V4L2_PIX_FMT_YUYV == server_l->dev->fcc || V4L2_PIX_FMT_Z16 == server_l->dev->fcc))
    {
        return uvc_z16_get_frame(data);
    }

    if(NULL == data || NULL == data->priv)
    {
        log_err("uvc_format_data is NULL or error!\n");
        return NULL;
    }

    buf_info = (struct uvc_frame_buf_info *)malloc(sizeof(struct uvc_frame_buf_info));
    if(NULL == buf_info)
    {
        log_err("malloc buf_info failed!\n");
        return NULL;
    }

    memset(buf_info, 0, sizeof(struct uvc_frame_buf_info));
    memset(&stVideoFrame, 0, sizeof(stVideoFrame));

    if(0 != AR_MPI_VPSS_GetChnFrame(server_l->cam_index, 0, &stVideoFrame, -1))
    {
        log_err("AR_MPI_VPSS_GetChnFrame failed!\n");
        goto FAILED;
    }


    priv_data = (struct yuv_priv_data *)data->priv;

    if(-1 == (buf_info->index = get_free_buffer(priv_data)))
    {
        log_err("No free yuv buffer!\n");
        if(0 != AR_MPI_VPSS_ReleaseChnFrame(server_l->cam_index, 0, &stVideoFrame))
        {
            log_err("0 YUV buffer release error!\n");
        }
        goto FAILED;
    }

    y = priv_data->cache_buf;
    u = y + (stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    v = y + (stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    //unsigned long start = ar_get_timestamp();
    
    dma_info.s32_tran_cnt_sum = 3;
    dma_info.st_addr_size[0].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[0];
    dma_info.st_addr_size[1].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[1];
    dma_info.st_addr_size[2].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[2];

    dma_info.st_addr_size[0].pv_dst_pa = (void *)priv_data->cache_buf_pa;
    dma_info.st_addr_size[1].pv_dst_pa = (void *)priv_data->cache_buf_pa + (stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    dma_info.st_addr_size[2].pv_dst_pa = (void *)priv_data->cache_buf_pa + (stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[0]);

    dma_info.st_addr_size[0].u32_size = stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0];
    dma_info.st_addr_size[1].u32_size = stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[1];
    dma_info.st_addr_size[2].u32_size = stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[1];

    ar_hal_sys_mem_transfer_pa(&dma_info);

    //unsigned long middle = ar_get_timestamp();

    I420ToYUY2((const uint8_t *)y, stVideoFrame.stVFrame.u32Stride[0],
               (const uint8_t *)u, stVideoFrame.stVFrame.u32Stride[1],
               (const uint8_t *)v, stVideoFrame.stVFrame.u32Stride[2],
               priv_data->yuv_buffer[buf_info->index], stVideoFrame.stVFrame.u32Width * 2, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);

    /*I420ToYUY2((const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[0]), stVideoFrame.stVFrame.u32Stride[0],
               (const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[1]), stVideoFrame.stVFrame.u32Stride[1],
               (const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[2]), stVideoFrame.stVFrame.u32Stride[2],
               priv_data->yuv_buffer[buf_info->index], stVideoFrame.stVFrame.u32Width * 2, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);*/

    //unsigned long end = ar_get_timestamp();
    //printf("cost time %lu %lu ms\n", middle -start, end - middle);

    buf_info->addr = (unsigned long)priv_data->yuv_buffer[buf_info->index];
    buf_info->len = stVideoFrame.stVFrame.u32Width * 2 * stVideoFrame.stVFrame.u32Height;
    buf_info->pts = stVideoFrame.stVFrame.u64PTS;

    /*printf("YUV format(0x%08x): width %d height %d stride %d %d %d\n",
                stVideoFrame.stVFrame.enPixelFormat,
                stVideoFrame.stVFrame.u32Width,
                stVideoFrame.stVFrame.u32Height,
                stVideoFrame.stVFrame.u32Stride[0],
                stVideoFrame.stVFrame.u32Stride[1],
                stVideoFrame.stVFrame.u32Stride[2]);*/

    if(AR_MPI_VPSS_ReleaseChnFrame(server_l->cam_index, 0, &stVideoFrame))
    {
        log_err("YUV buffer release error!\n");
    }

    return buf_info;

FAILED:
    free(buf_info);

    return NULL;
}

static struct uvc_frame_buf_info * uvc_nv12_get_frame(uvc_format_data_t *data)
{
    struct uvc_frame_buf_info       *buf_info = NULL;
    VIDEO_FRAME_INFO_S              stVideoFrame;
    struct yuv_priv_data            *priv_data = NULL;
    STRU_SYS_DMA_TRAN_INFO          dma_info = {0};
    uvc_server_t                    *server_l           = get_uvc_server();

    unsigned char                   *y = NULL;
    unsigned char                   *u = NULL;
    unsigned char                   *v = NULL;

    if(NULL == data || NULL == data->priv)
    {
        log_err("uvc_format_data is NULL or error!\n");
        return NULL;
    }

    buf_info = (struct uvc_frame_buf_info *)malloc(sizeof(struct uvc_frame_buf_info));
    if(NULL == buf_info)
    {
        log_err("malloc buf_info failed!\n");
        return NULL;
    }

    memset(buf_info, 0, sizeof(struct uvc_frame_buf_info));
    memset(&stVideoFrame, 0, sizeof(stVideoFrame));

    if(0 != AR_MPI_VPSS_GetChnFrame(server_l->cam_index, 0, &stVideoFrame, -1))
    {
        log_err("AR_MPI_VPSS_GetChnFrame failed!\n");
        goto FAILED;
    }

    priv_data = (struct yuv_priv_data *)data->priv;

    if(-1 == (buf_info->index = get_free_buffer(priv_data)))
    {
        log_err("No free yuv buffer!\n");
        if(0 != AR_MPI_VPSS_ReleaseChnFrame(server_l->cam_index, 0, &stVideoFrame))
        {
            log_err("0 YUV buffer release error!\n");
        }
        goto FAILED;
    }

    y = priv_data->cache_buf;
    u = y + (stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    v = y + (stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    //unsigned long start = ar_get_timestamp();
    
    dma_info.s32_tran_cnt_sum = 3;
    dma_info.st_addr_size[0].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[0];
    dma_info.st_addr_size[1].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[1];
    dma_info.st_addr_size[2].pv_src_pa = (void *)stVideoFrame.stVFrame.u64PhyAddr[2];

    dma_info.st_addr_size[0].pv_dst_pa = (void *)priv_data->cache_buf_pa;
    dma_info.st_addr_size[1].pv_dst_pa = (void *)priv_data->cache_buf_pa + (stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0]);
    dma_info.st_addr_size[2].pv_dst_pa = (void *)priv_data->cache_buf_pa + (stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[0]);

    dma_info.st_addr_size[0].u32_size = stVideoFrame.stVFrame.u64PhyAddr[1] - stVideoFrame.stVFrame.u64PhyAddr[0];
    dma_info.st_addr_size[1].u32_size = stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[1];
    dma_info.st_addr_size[2].u32_size = stVideoFrame.stVFrame.u64PhyAddr[2] - stVideoFrame.stVFrame.u64PhyAddr[1];

    ar_hal_sys_mem_transfer_pa(&dma_info);

    //unsigned long middle = ar_get_timestamp();

    I420ToNV12((const uint8_t *)y, stVideoFrame.stVFrame.u32Stride[0],
           (const uint8_t *)u, stVideoFrame.stVFrame.u32Stride[1],
           (const uint8_t *)v, stVideoFrame.stVFrame.u32Stride[2],
           priv_data->yuv_buffer[buf_info->index], stVideoFrame.stVFrame.u32Width,
           priv_data->yuv_buffer[buf_info->index] + stVideoFrame.stVFrame.u32Width * stVideoFrame.stVFrame.u32Height, 
           stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);

    /*I420ToNV12((const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[0]), stVideoFrame.stVFrame.u32Stride[0],
               (const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[1]), stVideoFrame.stVFrame.u32Stride[1],
               (const uint8_t *)(stVideoFrame.stVFrame.u64VirAddr[2]), stVideoFrame.stVFrame.u32Stride[2],
               priv_data->yuv_buffer[buf_info->index], stVideoFrame.stVFrame.u32Width,
               priv_data->yuv_buffer[buf_info->index] + stVideoFrame.stVFrame.u32Width * stVideoFrame.stVFrame.u32Height, 
               stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Width, stVideoFrame.stVFrame.u32Height);*/

    //unsigned long end = ar_get_timestamp();
    //printf("cost time %lu %lu ms\n", middle -start, end - middle);

    buf_info->addr = (unsigned long)priv_data->yuv_buffer[buf_info->index];
    buf_info->len = stVideoFrame.stVFrame.u32Width * 3 / 2 * stVideoFrame.stVFrame.u32Height;
    buf_info->pts = stVideoFrame.stVFrame.u64PTS;

    /*printf("NV12 format(0x%08x): width %d height %d stride %d %d %d\n",
                stVideoFrame.stVFrame.enPixelFormat,
                stVideoFrame.stVFrame.u32Width,
                stVideoFrame.stVFrame.u32Height,
                stVideoFrame.stVFrame.u32Stride[0],
                stVideoFrame.stVFrame.u32Stride[1],
                stVideoFrame.stVFrame.u32Stride[2]);*/

    if(AR_MPI_VPSS_ReleaseChnFrame(server_l->cam_index, 0, &stVideoFrame))
    {
        log_err("YUV buffer release error!\n");
    }

    return buf_info;

FAILED:
    free(buf_info);

    return NULL;
}

static int uvc_yuy2_release_frame(uvc_format_data_t *data, struct uvc_frame_buf_info * buf_info)
{
    struct yuv_priv_data    * priv_data = NULL;
    uvc_server_t            *server_l   = get_uvc_server();

    if(1 == server_l->is_raw &&
        (V4L2_PIX_FMT_YUYV == server_l->dev->fcc || V4L2_PIX_FMT_Z16 == server_l->dev->fcc))
    {
        return uvc_z16_release_frame(data, buf_info);
    }

    if(NULL == data || NULL == data->priv || NULL == buf_info || buf_info->index >= BUFFER_COUNT || buf_info->index < 0)
    {
        log_err("param check failed!\n");
        return -1;
    }

    priv_data = (struct yuv_priv_data *)data->priv;
    priv_data->buffer_status[buf_info->index] = 0;

    free(buf_info);

    return 0;
}

static struct uvc_format_opt uvc_yuy2_opt =
{
    .format_init = uvc_yuy2_init,
    .format_deinit = uvc_yuy2_deinit,
    .get_frame = uvc_yuy2_get_frame,
    .release_frame = uvc_yuy2_release_frame,
};

uvc_format_data_t uvc_yuy2_data =
{
    .vs_handle = NULL,
    .format_opt = &uvc_yuy2_opt,
};

static struct uvc_format_opt uvc_z16_opt =
{
    .format_init = uvc_z16_init,
    .format_deinit = uvc_z16_deinit,
    .get_frame = uvc_z16_get_frame,
    .release_frame = uvc_z16_release_frame,
};

uvc_format_data_t uvc_z16_data =
{
    .vs_handle = NULL,
    .format_opt = &uvc_z16_opt,
};

static struct uvc_format_opt uvc_nv12_opt =
{
    .format_init = uvc_yuy2_init,
    .format_deinit = uvc_yuy2_deinit,
    .get_frame = uvc_nv12_get_frame,
    .release_frame = uvc_yuy2_release_frame,
};

uvc_format_data_t uvc_nv12_data =
{
    .vs_handle = NULL,
    .format_opt = &uvc_nv12_opt,
};
