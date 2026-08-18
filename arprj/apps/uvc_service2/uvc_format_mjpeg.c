#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uvc_service2.h"
#include "pipeline_service.h"
#include "ar_comm_venc.h"
#include "mpi_venc.h"

static int uvc_mjpeg_deinit(uvc_format_data_t *data, void * handle);

static int uvc_mjpeg_init(uvc_format_data_t *data, void * handle, int argc, char *argv[])
{
    uvc_server_t       *server_l        = get_uvc_server();

    if(NULL == data || NULL != data->vs_handle)
    {
        log_err("param check error!\n");
        return -1;
    }

    ar_uvc_init(server_l->handle, argc, argv);

    return 0;
}

static int uvc_mjpeg_deinit(uvc_format_data_t *data, void * handle)
{
    uvc_server_t                    *server_l           = get_uvc_server();

    if(NULL == data)
    {
        log_err("param check error!\n");
        return -1;
    }

    ar_uvc_deinit(server_l->handle);

    return 0;
}
static AR_S32 waitOnFd(AR_S32 devFd)
{
    if (devFd < 0) {
        printf("invalid dev fd: %d\n", devFd);
        return -1;
    }
    AR_S32 retval = 0;

    fd_set rfds;


    FD_ZERO(&rfds);
    FD_SET(devFd, &rfds);

    retval = select(devFd + 1, &rfds, NULL, NULL, NULL);

    if (retval == -1) {
        printf("select return error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static struct uvc_frame_buf_info * uvc_mjpeg_get_frame(uvc_format_data_t *data)
{
    AR_S32                      s32Ret, i = 0;
    VENC_STREAM_S              *pstStream;
    struct uvc_frame_buf_info  *buf_info    = NULL;
    uvc_server_t               *server_l    = get_uvc_server();
    VENC_CHN_STATUS_S stStatus = { 0 };

    if(NULL == data)
    {
        log_err("param check error!\n");
        return NULL;
    }

    buf_info = (struct uvc_frame_buf_info *)malloc(sizeof(struct uvc_frame_buf_info));
    if(NULL == buf_info)
    {
        return NULL;
    }

    pstStream = (VENC_STREAM_S *)malloc(sizeof(VENC_STREAM_S));
    if (!pstStream)
    {
        printf("Malloc failed.\n");
        goto FAILED_0;
    }
    while (1) {
         waitOnFd(AR_MPI_VENC_GetFd(server_l->cam_index));
         s32Ret = AR_MPI_VENC_QueryStatus(server_l->cam_index, &stStatus);
         if (stStatus.u32LeftStreamFrames > 0)
             break;
         else
             usleep(1000);
    }
    pstStream->pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S)*stStatus.u32CurPacks);
    if (NULL == pstStream->pstPack)
    {
        printf("malloc memory failed!\n");
        goto FAILED_1;
    }

  pstStream->u32PackCount = stStatus.u32CurPacks;
  s32Ret = AR_MPI_VENC_GetStream(server_l->cam_index, pstStream, -1);
  if (AR_SUCCESS != s32Ret)
  {
    printf("AR_MPI_VENC_GetStream failed with %d!\n", s32Ret);
    goto FAILED_2;
  }


    buf_info->addr = (unsigned long)pstStream;
    buf_info->len = 0;
    for (i = 0; i < pstStream->u32PackCount; i++)
        buf_info->len += pstStream->pstPack[i].u32Len;
    buf_info->ptr = pstStream;
    buf_info->pts = pstStream->pstPack->u64PTS;

    return buf_info;

FAILED_2:
    free(pstStream->pstPack);
    pstStream->pstPack = NULL;

FAILED_1:
    free(pstStream);

FAILED_0:
    free(buf_info);

    return NULL;
}

static int uvc_mjpeg_release_frame(uvc_format_data_t *data, struct uvc_frame_buf_info * buf_info)
{
    int ret = 0;

    VENC_STREAM_S * pstStream;

    uvc_server_t    *server_l = get_uvc_server();

    if(NULL == data || NULL == buf_info || NULL == buf_info->ptr)
    {
        log_err("param check error!\n");
        return ret;
    }

  pstStream = (VENC_STREAM_S *)buf_info->ptr;

  AR_MPI_VENC_ReleaseStream(server_l->cam_index, pstStream);
  if(pstStream->pstPack)
  {
    free(pstStream->pstPack);
    pstStream->pstPack = NULL;
  }
  if(pstStream)
  {
    free(pstStream);
  }

    free(buf_info);

    return ret;
}

static struct uvc_format_opt uvc_mjpeg_opt =
{
    .format_init = uvc_mjpeg_init,
    .format_deinit = uvc_mjpeg_deinit,
    .get_frame = uvc_mjpeg_get_frame,
    .release_frame = uvc_mjpeg_release_frame,
};

uvc_format_data_t uvc_mjpeg_data =
{
    .vs_handle = NULL,
    .format_opt = &uvc_mjpeg_opt,
};


