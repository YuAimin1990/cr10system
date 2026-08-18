#ifdef AR_FEAT_EIS
#include "ar_comm_vb.h"
#include "ar_comm_video.h"
#include "ar_math.h"
#include "hal_type.h"
#include "hal_errno.h"
#include "hal_sys.h"
#include "hal_vb.h"
#include "hal_eis_algo.h"
#include "mpi_dbglog.h"
#include "sample_eis.h"
#include "mpi_gdc_api.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>


#define EIS_LOG_TAG						MPP_TAG_ID(AR_ID_SYS)

#define EIS_ALGO_NAME_DEFAULT			"artosyn"

#define EIS_INVALID_DEVICE_HANDLE		(-1)

#define EIS_FRAME_SAME					(0)
#define EIS_FRAME_NEW					(1)

#define EIS_MESH_WIDTH					MAX_BLOCK_W
#define EIS_MESH_HEIGHT					MAX_BLOCK_H

typedef struct
{
	AR_U32 u32VBSize;                           /**< VB size */

	AR_U32 u32HeadStride;                       /**< head stride */
	AR_U32 u32HeadSize;                         /**< head size */
	AR_U32 u32HeadYSize;                        /**< head Y size */

	AR_U32 u32MainStride;                       /**< main stride */
	AR_U32 u32MainUStride;                      /**< main U stride */
	AR_U32 u32MainVStride;                      /**< main V stride */

	AR_U32 u32MainSize;                         /**< main size */
	AR_U32 u32MainYSize;                        /**< main Y size */

	AR_U32 u32MainUSize;                        /**< main U size */
	AR_U32 u32MainVSize;                        /**< main V size */

	AR_U32 u32ExtStride;                        /**< ext stride */
	AR_U32 u32ExtYSize;                         /**< ext Y size */

} STRU_FRAME_CAL_CONFIG;

static AR_VOID AR_MPI_GetPicBufferConfig(AR_U32 u32Width, AR_U32 u32Height,PIXEL_FORMAT_E enPixelFormat,
            DATA_BITWIDTH_E enBitWidth, COMPRESS_MODE_E enCmpMode, AR_U32 u32Align, AR_U32 u32HAlign, STRU_FRAME_CAL_CONFIG* pstCalConfig)
{
    AR_U32 u32BitWidth    = 0;
    AR_U32 u32HeadStride  = 0;
    AR_U32 u32VBSize      = 0;
    AR_U32 u32HeadSize    = 0;
    AR_U32 u32AlignHeight = 0;
    AR_U32 u32MainStride  = 0;
    AR_U32 u32MainUStride  = 0;
    AR_U32 u32MainVStride  = 0;
    AR_U32 u32MainSize    = 0;
    AR_U32 u32ExtStride   = 0;
    AR_U32 u32ExtYSize    = 0;
    AR_U32 u32HeadYSize   = 0;
    AR_U32 u32YSize       = 0;
    AR_U32 u32USize       = 0;
    AR_U32 u32VSize       = 0;

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
        case DATA_BITWIDTH_10:
        {
            u32BitWidth = 10;
            break;
        }
        case DATA_BITWIDTH_12:
        {
            u32BitWidth = 12;
            break;
        }
        case DATA_BITWIDTH_14:
        {
            u32BitWidth = 14;
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

    u32AlignHeight = ALIGN_UP(u32Height, u32HAlign);

    if (COMPRESS_MODE_NONE == enCmpMode)
    {
        u32MainStride = ALIGN_UP((u32Width * u32BitWidth + 7) >> 3, u32Align);
		AR_LOG_DBG(EIS_LOG_TAG, "u32MainStride=%d u32Width =%d %d %d\n",
					u32MainStride, u32Width, u32BitWidth, u32Align);

        u32YSize = u32MainStride * u32AlignHeight;

        if(PIXEL_FORMAT_YVU_SEMIPLANAR_420 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_420 == enPixelFormat ||
	        PIXEL_FORMAT_YVU_PLANAR_420 == enPixelFormat)
        {
            u32MainSize = (u32MainStride * u32AlignHeight) * 3 >> 1;
            u32USize    = (u32MainStride * u32AlignHeight) >> 2;
            u32VSize    = (u32MainStride * u32AlignHeight) >> 2;

            u32MainUStride = (u32MainStride >> 1);
            u32MainVStride = (u32MainStride >> 1);

			AR_LOG_DBG(EIS_LOG_TAG, "enPixelFormat=%d stride =%d %d %d\n",
						enPixelFormat, u32MainStride, u32MainUStride, u32MainVStride);
        }
        else if (PIXEL_FORMAT_YVU_SEMIPLANAR_422 == enPixelFormat || PIXEL_FORMAT_YUV_SEMIPLANAR_422 == enPixelFormat)
        {
            u32MainSize = u32MainStride * u32AlignHeight * 2;
            u32USize    = (u32MainStride * u32AlignHeight) >> 1;
            u32VSize    = (u32MainStride * u32AlignHeight) >> 1;

            u32MainUStride = (u32MainStride >> 1);
            u32MainVStride = (u32MainStride >> 1);

			AR_LOG_DBG(EIS_LOG_TAG, "enPixelFormat=%d stride =%d %d %d\n",
						enPixelFormat, u32MainStride, u32MainUStride, u32MainVStride);
        }
        else if ((PIXEL_FORMAT_YUV_400 == enPixelFormat) || (PIXEL_FORMAT_S16C1 == enPixelFormat))
        {
            u32MainSize = u32MainStride * u32AlignHeight;
            u32USize    = 0;
            u32VSize    = 0;

            u32MainUStride = 0;
            u32MainVStride = 0;

			AR_LOG_DBG(EIS_LOG_TAG, "enPixelFormat=%d stride =%d %d %d\n",
						enPixelFormat, u32MainStride, u32MainUStride, u32MainVStride);
        }
        else
        {
            u32MainSize = u32MainStride * u32AlignHeight * 3;
            u32USize    = u32MainStride * u32AlignHeight;
            u32VSize    = u32MainStride * u32AlignHeight;

            u32MainUStride = u32MainStride;
            u32MainVStride = u32MainStride;

			AR_LOG_WARN(EIS_LOG_TAG, "enPixelFormat=%d stride =%d %d %d\n",
						enPixelFormat, u32MainStride, u32MainUStride, u32MainVStride);
        }

        u32VBSize   = u32MainSize;
    }

    pstCalConfig->u32VBSize     = u32VBSize;
    pstCalConfig->u32HeadYSize  = u32HeadYSize;
    pstCalConfig->u32HeadSize   = u32HeadSize;
    pstCalConfig->u32HeadStride = u32HeadStride;
    pstCalConfig->u32MainStride = u32MainStride;
    pstCalConfig->u32MainUStride = u32MainUStride;
    pstCalConfig->u32MainVStride = u32MainVStride;

    pstCalConfig->u32MainYSize  = u32YSize;
    pstCalConfig->u32MainUSize  = u32USize;
    pstCalConfig->u32MainVSize  = u32VSize;

    pstCalConfig->u32MainSize   = u32MainSize;
    pstCalConfig->u32ExtStride  = u32ExtStride;
    pstCalConfig->u32ExtYSize   = u32ExtYSize;

    return;
}


static void eis_ref_frame(const VIDEO_FRAME_INFO_S *p_frame)
{
	AR_LOG_DBG(EIS_LOG_TAG, "ref frame(++ %p) phy addr=0x%llx\n", p_frame, p_frame->stVFrame.u64PhyAddr[0]);

	AR_S32 ret = ar_hal_vb_user_add_by_frameinfo((STRU_SYS_VIDEO_FRAME_INFO *)p_frame, AR_VB_UID_VPSS);

	if(ret)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "ret=%x modid=%d\n", ret, p_frame->enModId);
		ar_assert(0);
	}
}

static void eis_unref_frame(const VIDEO_FRAME_INFO_S *p_frame)
{
	AR_LOG_DBG(EIS_LOG_TAG, "unref frame(-- %p) phy addr=0x%llx \n", p_frame, p_frame->stVFrame.u64PhyAddr[0]);

	AR_S32 ret = ar_hal_vb_user_sub_by_frameinfo((STRU_SYS_VIDEO_FRAME_INFO *)p_frame, AR_VB_UID_VPSS);

	if(ret)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "ret=%x modid=%d\n", ret, p_frame->enModId);
		ar_assert(0);
	}
}

#ifdef __EIS_SIMU_TABLE__
static STRU_EIS_POINT* prepare_point_data(STRU_EIS_POINT *pPoint, int w, int h)
{
	(void)pPoint;

	STRU_EIS_POINT* point = ar_malloc((EIS_MESH_WIDTH+1) * (EIS_MESH_HEIGHT+1) * sizeof(STRU_EIS_POINT));

	for(int i = 0; i <= h; i++)
	{
		for(int j = 0; j <= w; j++)
		{
		  point[j + i*w].x = (j*EIS_MESH_HEIGHT) << 2;
		  point[j + i*w].y = (i*EIS_MESH_WIDTH) << 2;
		}
	}
}

static AR_VOID gen_lut_done(STRU_EIS_POINT *pPoint)
{
	ar_free(pPoint);
}
#else
static STRU_EIS_POINT* prepare_point_data(STRU_EIS_POINT *pPoint, int w, int h)
{
	(void)w;
	(void)h;

	return pPoint;
}

static AR_VOID gen_lut_done(STRU_EIS_POINT *pPoint)
{
	(void)pPoint;
}
#endif

static AR_S32 eis_gen_lut(VIDEO_FRAME_INFO_S *p_src,AR_GDC_USER_ATTR_S *pstUsrAttr, STRU_EIS_POINT *pPoint, AR_S32 s32PointCnt)
{
	AR_LOG_INFO(EIS_LOG_TAG, "lut (%p %d)", pPoint, s32PointCnt);

	int picw = p_src->stVFrame.u32Width;
	int pich = p_src->stVFrame.u32Height;

	int blk_hnum = ARCEILING(picw, EIS_MESH_WIDTH);
	int blk_vnum = ARCEILING(pich, EIS_MESH_HEIGHT);

	int axis_hnum = blk_hnum + 1;
	int axis_vnum = blk_vnum + 1;

	int axis_stride = ALIGN_UP(axis_hnum, 4);

	STRU_EIS_POINT *pt = prepare_point_data(pPoint, axis_hnum, axis_vnum);

	pstUsrAttr->u32LutLen =axis_stride*axis_vnum*sizeof(AR_U32);
	pstUsrAttr->u64LutVirtAddr = (AR_U64)ar_malloc(pstUsrAttr->u32LutLen);
	if(!pstUsrAttr->u64LutVirtAddr)
	{
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM);
	}

	AR_U32 *pu32Lut = (AR_U32 *)pstUsrAttr->u64LutVirtAddr;

	for (int i = 0; i < axis_vnum; i++)
	{
		for (int j = 0; j < axis_stride; j++)
		{
			int k = i * axis_stride + j;
			if(j < axis_hnum)
			{
				pu32Lut[k] = ((pt[j + i * axis_hnum].x) & 0xFFFF) + (((pt[j + i * axis_hnum].y) & 0xFFFF) << 16);
			}
			else
			{
				pu32Lut[k] = 0;
			}
		}
	}

	gen_lut_done(pt);

	return AR_OK;
}

static AR_S32 eis_alloc_frame_buffer(VIDEO_FRAME_INFO_S *p_frame_info, AR_U32 walign, AR_U32 halign)
{
	AR_U32 buf_size;
	VB_BLK block_handle;
	STRU_FRAME_CAL_CONFIG stCalConfig;
	VIDEO_FRAME_S *p_video_frame = &p_frame_info->stVFrame;

	p_frame_info->u32PoolId = AR_VB_INVALID_POOLID;
	p_frame_info->enModId   = AR_VB_UID_VPSS;

	AR_MPI_GetPicBufferConfig(p_video_frame->u32Width, p_video_frame->u32Height,
										p_video_frame->enPixelFormat, DATA_BITWIDTH_8,
										COMPRESS_MODE_NONE, walign, halign, &stCalConfig);
	buf_size = stCalConfig.u32VBSize;
	ar_assert(stCalConfig.u32VBSize);

	block_handle = ar_hal_vb_get_block_ex(p_frame_info->u32PoolId, buf_size, NULL, p_frame_info->enModId);
	if (block_handle == AR_VB_INVALID_HANDLE)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "alloc frame failed\n");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM);
	}

	p_frame_info->stVFrame.u32Stride[0] = stCalConfig.u32MainStride;
	p_frame_info->stVFrame.u32Stride[1] = stCalConfig.u32MainUStride;
	p_frame_info->stVFrame.u32Stride[2] = stCalConfig.u32MainVStride;

	ar_hal_vb_mmap_pool(ar_hal_vb_handle2poolid(block_handle));

	p_frame_info->stVFrame.u64PhyAddr[0] = ar_hal_vb_handle2physaddr(block_handle);

	ar_hal_vb_get_blockviraddr(ar_hal_vb_handle2poolid(block_handle),
							   ar_hal_vb_handle2physaddr(block_handle),
							   (AR_VOID **)&p_frame_info->stVFrame.u64VirAddr[0]);

	p_frame_info->stVFrame.u64PhyAddr[1] = p_frame_info->stVFrame.u64PhyAddr[0] + stCalConfig.u32MainYSize;
	p_frame_info->stVFrame.u64VirAddr[1] = p_frame_info->stVFrame.u64VirAddr[0] + stCalConfig.u32MainYSize;

	p_frame_info->stVFrame.u64PhyAddr[2] = p_frame_info->stVFrame.u64PhyAddr[1] + stCalConfig.u32MainUSize;
	p_frame_info->stVFrame.u64VirAddr[2] = p_frame_info->stVFrame.u64VirAddr[1] + stCalConfig.u32MainUSize;

	p_frame_info->stVFrame.u32Len[0] = stCalConfig.u32MainYSize;
	p_frame_info->stVFrame.u32Len[1] = stCalConfig.u32MainUSize;
	p_frame_info->stVFrame.u32Len[2] = stCalConfig.u32MainVSize;

	AR_LOG_DBG(EIS_LOG_TAG, "[id:%d]alloc frame %p 0x%lx\n", p_frame_info->enModId, p_frame_info, p_frame_info->stVFrame.u64PhyAddr[0]);

	return AR_OK;
}

static AR_S32 eis_video_frame_to_gdc_buf(VIDEO_FRAME_INFO_S *p_src, void *p_buf)
{
	if (p_src->stVFrame.enPixelFormat != PIXEL_FORMAT_YVU_PLANAR_420
		&& p_src->stVFrame.enPixelFormat != PIXEL_FORMAT_YVU_SEMIPLANAR_420)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "video frame not yuv420 or 420sp\n");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOT_SUPPORT);
	}
	AR_GDC_BUFFER_S *pstBuf = (AR_GDC_BUFFER_S *)p_buf;

	pstBuf->format.height = p_src->stVFrame.u32Height;
	pstBuf->format.width  = p_src->stVFrame.u32Width;

	pstBuf->format.luma_stride   = p_src->stVFrame.u32Stride[0];
	pstBuf->format.chroma_stride = p_src->stVFrame.u32Stride[1];

	ar_assert(pstBuf->format.luma_stride);
	ar_assert(pstBuf->format.chroma_stride);

	pstBuf->pannel[0].addr_phy  = p_src->stVFrame.u64PhyAddr[0];
	pstBuf->pannel[0].addr_virt = p_src->stVFrame.u64VirAddr[0];

	pstBuf->pannel[1].addr_phy  = p_src->stVFrame.u64PhyAddr[1];
	pstBuf->pannel[1].addr_virt = p_src->stVFrame.u64VirAddr[1];

	pstBuf->pannel[2].addr_phy  = p_src->stVFrame.u64PhyAddr[2];
	pstBuf->pannel[2].addr_virt = p_src->stVFrame.u64VirAddr[2];

	return AR_OK;
}

static AR_S32 eis_gdc_transform(VIDEO_FRAME_INFO_S *p_src, VIDEO_FRAME_INFO_S *p_dst, STRU_EIS_ALGO_LIB_OUTPUT_T *p_output, AR_U32 u32WAlign, AR_U32 u32HAlign)
{
	AR_GDC_TRANSFORM_S stGdcTran = {0};

	*p_dst = *p_src;

	AR_S32 ret = eis_alloc_frame_buffer(p_dst, u32WAlign, u32HAlign);
	if(ret != AR_OK)
	{
		return ret;
	}

	eis_video_frame_to_gdc_buf(p_src, &stGdcTran.stInBuffer);
	eis_video_frame_to_gdc_buf(p_dst, &stGdcTran.stOutBuffer);

	AR_GDC_USER_ATTR_S usrAttr = {0};

	ret = eis_gen_lut(p_src, &usrAttr, p_output->eis_point, p_output->eis_point_count);
	if(ret != AR_OK)
	{
		return ret;
	}

	stGdcTran.stParams[0].enType = AR_GDC_USER_DEF_E;
	stGdcTran.stParams[0].pAttr = &usrAttr;

	ret = AR_MPI_GDC_Transform(&stGdcTran);
	if (ret == AR_OK)
	{
		ret = EIS_FRAME_NEW;
	}
	else
	{
		AR_LOG_ERR(EIS_LOG_TAG, "AR_MPI_GDC_Transform failed 0x%x\n", ret);
	}
	ar_free((void*)usrAttr.u64LutVirtAddr);

	return ret;
}

static AR_VOID* eis_algo_feed(AR_VOID* param)
{
	AR_S32 ret = AR_OK;
	STRU_EIS_INFO_S* eis_info = (STRU_EIS_INFO_S*)param;
	STRU_EIS_IMMU_DATA_T data;

	while(!eis_info->stop)
	{
		ret = eis_info->low_intf->read(eis_info->dev_fd, &data);
		if(ret != AR_OK)
		{
			return (void *)(AR_S64)ret;
		}

		hal_eis_process_imu_data(eis_info->lib, &data);
	}

	return (void *)(AR_S64)ret;
}

static AR_S32 sample_eis_init(STRU_EIS_INFO_S* eis_info, AR_CHAR* algo_name, STRU_EIS_TUNING_T* tuning)
{
	eis_info->dev_fd = EIS_INVALID_DEVICE_HANDLE;
	eis_info->is_first = AR_TRUE;

	eis_info->lock = ar_mutex_init();
	if(!eis_info->lock)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "create eis lock failed");
		return AR_ERROR;
	}

	if(!algo_name)
	{
		strcpy(eis_info->algo_name, EIS_ALGO_NAME_DEFAULT);
	}
	else
	{
		strncpy(eis_info->algo_name, algo_name, sizeof(eis_info->algo_name) - 1);
	}

	eis_info->lib = hal_eis_creat_algo_lib(eis_info->algo_name);
	if(!eis_info->lib)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "create eis lib %s failed", eis_info->algo_name);
		ar_mutex_destroy(eis_info->lock);
		eis_info->lock = NULL;
		return AR_ERROR;
	}
	else
	{
		if(tuning)
		{
			eis_info->tuning = *tuning;
		}

		return AR_OK;
	}
}

static AR_S32 sample_eis_exit(STRU_EIS_INFO_S* eis_info)
{
	AR_S32 ret = AR_OK;

	if(eis_info->config.data)
	{
		ar_free(eis_info->config.data);
		eis_info->config.data = NULL;
		eis_info->config.len = 0;
	}

	if(eis_info->lib)
	{
		ret = hal_eis_delete_algo_lib(eis_info->lib, eis_info->algo_name);
		if(ret)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "delete algo %s failed 0x%x", ret, eis_info->algo_name);
		}
		else
		{
			eis_info->lib = NULL;
		}
	}

	if(eis_info->lock)
	{
		ar_mutex_destroy(eis_info->lock);
		eis_info->lock = NULL;
	}

	return ret;
}

static AR_S32 sample_eis_start(STRU_EIS_INFO_S* eis_info)
{
	AR_S32 ret = AR_OK;

	ar_lock(eis_info->lock);

	if(!eis_info->lib)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "algo not init yet");
		ret = AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_SYS_NOTREADY);
		ar_unlock(eis_info->lock);
		return ret;
	}

	ret = hal_eis_algo_start(eis_info->lib);
	if(ret)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "start algo %s failed 0x%x", eis_info->algo_name, ret);
		ar_unlock(eis_info->lock);
		return ret;
	}

	STRU_EIS_CTL_UPDATE_TUNING_T param =
	{
		.is_init = 1,
		.p_algo_tuning = &eis_info->tuning,
	};
	ret = hal_eis_set_tuning_pra(eis_info->lib, &param);
	if(ret)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "set tuning param failed 0x%x", ret);
		ar_unlock(eis_info->lock);
		return ret;
	}

	if(eis_info->low_intf->open)
	{
		eis_info->dev_fd = eis_info->low_intf->open(&eis_info->config);
		if(eis_info->dev_fd < 0)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "open eis device failed ret=%d", eis_info->dev_fd);
			ar_unlock(eis_info->lock);
			return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOT_SUPPORT);
		}
	}

	if(eis_info->low_intf->ctrl_fifo)
	{
		ret = eis_info->low_intf->ctrl_fifo(eis_info->dev_fd, AR_TRUE, AR_TRUE);
		if(ret < 0)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "ctrl_fifo failed errno=%d", ret);
			ar_unlock(eis_info->lock);
			return ret;
		}
	}

	eis_info->thread = ar_thread_new((ar_thread_func_t)eis_algo_feed, eis_info, 0, NULL);
	if(!eis_info->thread)
	{
		ret = AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
		AR_LOG_ERR(EIS_LOG_TAG, "new thread failed 0x%x", ret);
		ar_unlock(eis_info->lock);
		return ret;
	}

	ar_unlock(eis_info->lock);
	return ret;
}

static AR_S32 sample_eis_stop(STRU_EIS_INFO_S* eis_info)
{
	AR_S32 ret = AR_OK;

	ar_lock(eis_info->lock);
	eis_info->stop = AR_TRUE;

	if(eis_info->thread)
	{
		ret = ar_thread_terminate(eis_info->thread);
		eis_info->thread = NULL;
	}

	if(eis_info->low_intf->close && eis_info->dev_fd >= 0)
	{
		ret = eis_info->low_intf->close(eis_info->dev_fd);
		if(ret)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "close %d failed errno=%d, %s",	eis_info->dev_fd, errno, strerror(errno));
		}
		else
		{
			eis_info->dev_fd = -1;
		}
	}

	AR_LOG_DBG(EIS_LOG_TAG, "is_first=%d enModId=%d u32PoolId=%d", eis_info->is_first, eis_info->last_frame.enModId, eis_info->last_frame.u32PoolId);
	if(!eis_info->is_first)
	{
		eis_unref_frame(&eis_info->last_frame);
    }

	if(eis_info->lib)
	{
		ret = hal_eis_algo_stop(eis_info->lib);
		if(ret)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "stop algo failed 0x%x", ret);
		}
	}

	ar_unlock(eis_info->lock);
	return ret;
}

static AR_S32 sample_eis_config(STRU_EIS_INFO_S* eis_info, EIS_CONFIG_T* config)
{
	if(config && config->data && config->len)
	{
		if(eis_info->config.len == config->len && eis_info->config.data)
		{
			memcpy(eis_info->config.data, config->data, config->len);
			return AR_OK;
		}

		if(eis_info->config.data)
		{
			AR_LOG_WARN(EIS_LOG_TAG, "eis config struct might changed");
			ar_free(eis_info->config.data);
		}

		eis_info->config.data = ar_malloc(config->len);
		if(!eis_info->config.data)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "fail to alloc for eis config");
			return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
		}
		memcpy(eis_info->config.data, config->data, config->len);
		eis_info->config.len = config->len;
	}

	return AR_OK;
}

static AR_S32 sample_eis_process_frame(STRU_EIS_INFO_S* eis_info, VIDEO_FRAME_INFO_S *p_src, STRU_EIS_PROCESS_PARA* para)
{
	AR_S32 ret = AR_OK;
	AR_U32 imu_data_num = 0;
	STRU_EIS_IMMU_DATA_T* imu_data = NULL;
	STRU_EIS_ALGO_LIB_INPUT_T input;
	STRU_EIS_ALGO_LIB_OUTPUT_T output;
	VIDEO_FRAME_INFO_S dst;

	if(!eis_info->thread)
	{
		AR_LOG_DBG(EIS_LOG_TAG, "imu might not started, try it");

		ret = sample_eis_mpu_start(eis_info);
		if(ret)
		{
			return ret;
		}
	}

	ar_lock(eis_info->lock);

    AR_U64 imu_phy_addr=0;
	imu_data = hal_eis_get_matched_imu_data_by_frame_pts(eis_info->lib, p_src->stVFrame.u64PTS, &imu_data_num, &imu_phy_addr);
	AR_LOG_DBG(EIS_LOG_TAG, "imu_data_num=%d imu_data=0x%x", imu_data_num, imu_data);
	if(!imu_data_num)
	{
	    eis_info->no_imu_data_match_ref++;
		if(imu_data)
		{
			ret = hal_eis_release_imu_data(eis_info->lib, imu_data, imu_phy_addr);
			if(ret)
			{
				AR_LOG_ERR(EIS_LOG_TAG, "release imu data failed 0x%x", ret);
			}
		}

		ar_unlock(eis_info->lock);
		return EIS_FRAME_SAME;
	}

	input.camera_id = 0;
	input.frame_id = p_src->stVFrame.u32FrameId;
	input.frame_time_sof = p_src->stVFrame.u64PTS/1000.0*32768;
	input.mesh_width = EIS_MESH_WIDTH;
	input.mesh_height = EIS_MESH_HEIGHT;
	input.img_w = p_src->stVFrame.u32Width;
	input.img_h = p_src->stVFrame.u32Height;
	input.line_times_s_q15 = p_src->stVFrame.fLineTime*32768*input.mesh_height*2;
	input.imu_data_num = imu_data_num;
	input.immu_data = imu_data;

	//convert to algo q15
	for(int i=0;i<imu_data_num;i++)
	{
	   input.immu_data[i].time_stamps=(input.immu_data[i].time_stamps)/1000.0*32768;

	   input.immu_data[i].x = input.immu_data[i].x*32768.0;
	   input.immu_data[i].y = input.immu_data[i].y*32768.0;
	   input.immu_data[i].z = input.immu_data[i].z*32768.0;
	}

	ret = hal_eis_process_algo_run(eis_info->lib, &input, &output);
	if(ret == AR_OK)
	{
		if(!output.eis_point || !output.eis_point_count)
		{
			AR_LOG_ERR(EIS_LOG_TAG, "eis algo run success but no point (%p %d)", output.eis_point, output.eis_point_count);
			AR_S32 ret_tmp = hal_eis_release_imu_data(eis_info->lib, imu_data, imu_phy_addr);
        	if(ret_tmp)
        	{
				AR_LOG_ERR(EIS_LOG_TAG, "release imu data failed 0x%x", ret_tmp);
        	}
			ar_unlock(eis_info->lock);
		    return EIS_FRAME_SAME;
		}
		else
		{
			if(eis_info->is_first)
			{
				eis_ref_frame(p_src);
				eis_info->last_frame = *p_src;
				eis_info->is_first = AR_FALSE;
			}
			input.immu_data[0].data[0] = imu_data_num;
			input.immu_data[0].data[1] = output.frame_time;
			input.immu_data[0].data[2] = output.eis_point_count;
			input.immu_data[0].data[3] = output.eis_data_num;
			input.immu_data[0].data[4] = output.line_time_q15;
			input.immu_data[0].data[5] = output.debug_data_count;
			int offset = imu_data_num * sizeof(STRU_EIS_IMMU_DATA_T);
			STRU_EIS_POINT *p_point = (STRU_EIS_POINT *)((uint64_t)imu_data + offset);
			for(int i=0; i < output.eis_point_count; i++)
			{
			     p_point[i]=output.eis_point[i];
			}
			offset = offset + output.eis_point_count * sizeof(STRU_EIS_POINT);
			AR_S32	*p_val_s32 = (AR_S32	*)((uint64_t)imu_data +offset);
			for(int i=0; i < output.eis_data_num; i++)
			{
				p_val_s32[i] = output.gyro_stamp[i];
			}
			offset = offset + output.eis_data_num * sizeof(AR_S32);
			p_val_s32=(AR_S32 *)((uint64_t)imu_data +offset);
			for(int i=0;i<output.eis_data_num;i++)
			{
				p_val_s32[i] = output.anglex[i];
			}
			offset = offset + output.eis_data_num * sizeof(AR_S32);
			p_val_s32=(AR_S32 *)((uint64_t)imu_data +offset);
			for(int i=0; i<output.eis_data_num; i++)
			{
				p_val_s32[i] = output.angley[i];
			}
			offset = offset + output.eis_data_num * sizeof(AR_S32);
			p_val_s32 = (AR_S32 *)((uint64_t)imu_data +offset);
			for(int i=0; i < output.eis_data_num; i++)
			{
				p_val_s32[i] = output.anglez[i];
			}
			offset = offset + output.eis_data_num*sizeof(AR_S32);
			STRU_EIS_DEBUG_T *p_debug = (STRU_EIS_DEBUG_T *)((uint64_t)imu_data + offset);
			for(int i=0; i < output.debug_data_count; i++)
			{
			    p_debug[i] = output.debug_data[i];
			}
			offset = offset + output.debug_data_count*sizeof(STRU_EIS_DEBUG_T);

			if(eis_info->dbg && eis_info->dbg->dump && eis_info->dbg->dump->dump)
			{
				eis_info->dbg->dump->dump(eis_info->dbg->dump,
									(AR_VOID *)eis_info->last_frame.stVFrame.u64PhyAddr[0],
									(AR_VOID *)eis_info->last_frame.stVFrame.u64PhyAddr[1],
									(AR_VOID *)eis_info->last_frame.stVFrame.u64PhyAddr[2],
									(AR_VOID *)imu_phy_addr,
									p_src->stVFrame.u32Len[0],
									p_src->stVFrame.u32Len[1],
									p_src->stVFrame.u32Len[2],
									offset);
			}

			ret = eis_gdc_transform(&eis_info->last_frame, &dst, &output, para->u32_walign, para->u32_halign);

			offset = 0;

			p_point=(STRU_EIS_POINT *)((uint64_t)imu_data + offset);
			for(int i=0; i < output.eis_point_count; i++)
			{
			     p_point[i] = output.eis_point[i];
			}
			offset = offset + output.eis_point_count * sizeof(STRU_EIS_POINT);

			if(eis_info->dbg && eis_info->dbg->dump_after && eis_info->dbg->dump_after->dump)
			{
				eis_info->dbg->dump_after->dump(eis_info->dbg->dump_after,
									(AR_VOID *)dst.stVFrame.u64PhyAddr[0],
									(AR_VOID *)dst.stVFrame.u64PhyAddr[1],
									(AR_VOID *)dst.stVFrame.u64PhyAddr[2],
									(AR_VOID *)imu_phy_addr,
									dst.stVFrame.u32Len[0],
									dst.stVFrame.u32Len[1],
									dst.stVFrame.u32Len[1],
									offset);
			}

			eis_unref_frame(&eis_info->last_frame);
			eis_info->last_frame = *p_src;

			// returns only new_frame or error
			if(ret == EIS_FRAME_NEW)
			{
				AR_LOG_DBG(EIS_LOG_TAG , "NEW_FRAME free %lx: %p\n", p_src->stVFrame.u64PhyAddr[0], p_src);
				*p_src = dst;
			}
			else
			{
				AR_LOG_ERR(EIS_LOG_TAG , "error ret=0x%x\n", ret);
				if (AR_GET_ERRID(ret) != HAL_ERR_NOMEM)
				{
					eis_unref_frame(&dst);
				}
			}
		}
	}
	else
	{
		AR_LOG_ERR(EIS_LOG_TAG, "eis algo run failed 0x%x", ret);
	}

	AR_S32 ret_tmp = hal_eis_release_imu_data(eis_info->lib, imu_data, imu_phy_addr);
	if(ret_tmp)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "release imu data failed 0x%x", ret_tmp);
	}

	ar_unlock(eis_info->lock);

	return ret;
}

static STRU_EIS_HIGH_INTERFACE_S sample_eis_high_intf =
{
	.init = sample_eis_init,
	.exit = sample_eis_exit,
	.start = sample_eis_start,
	.stop = sample_eis_stop,
	.config = sample_eis_config,
	.process = sample_eis_process_frame
};

AR_S32 sample_eis_register(STRU_EIS_INFO_S* eis_info, const STRU_EIS_LOW_INTERFACE_S* low_intf, const STRU_EIS_DEBUG_INTERFACE_S* dbg_intf)
{
	if(eis_info->low_intf)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "low layer interface already exist");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	if(eis_info->dbg)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "debug already exist");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_ILLEGAL_PARAM);
	}

	eis_info->low_intf = ar_malloc(sizeof(STRU_EIS_LOW_INTERFACE_S));
	if(!eis_info->low_intf)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "fail to alloc for eis low interface");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
	}

	eis_info->dbg = ar_malloc(sizeof(STRU_EIS_DEBUG_CNTX_S));
	if(!eis_info->dbg)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "fail to alloc for eis debug");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
	}

	eis_info->dbg->intf = ar_malloc(sizeof(STRU_EIS_DEBUG_INTERFACE_S));
	if(!eis_info->dbg->intf)
	{
		AR_LOG_ERR(EIS_LOG_TAG, "fail to alloc for eis debug interface");
		return AR_MPP_DEF_ERR(AR_ID_SYS, HAL_ERR_LEVEL_CRIT, HAL_ERR_NOMEM);
	}

	eis_info->high_intf = &sample_eis_high_intf;
	*(eis_info->low_intf) = *low_intf;
	*(eis_info->dbg->intf) = *dbg_intf;

	if(dbg_intf->init)
	{
		dbg_intf->init(eis_info);
	}

	return AR_OK;
}

AR_S32 sample_eis_unregister(STRU_EIS_INFO_S* eis_info)
{
	if(eis_info->low_intf)
	{
		ar_free(eis_info->low_intf);
		eis_info->low_intf = NULL;
	}

	if(eis_info->dbg)
	{
		if(eis_info->dbg->intf)
		{
			if(eis_info->dbg->intf->exit)
			{
				eis_info->dbg->intf->exit(eis_info);
			}
			ar_free(eis_info->dbg->intf);
			eis_info->dbg->intf = NULL;
		}
		ar_free(eis_info->dbg);
		eis_info->dbg = NULL;
	}

	return AR_OK;
}
#endif
