#ifndef __SAMPLE_VPSS_VIO_VIFILE_H__
#define __SAMPLE_VPSS_VIO_VIFILE_H__

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
            u32VBSize     = 0;
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

AR_VOID config_vin_data(FILE* fd, AR_S32 width, AR_S32 height, AR_S32 frameRate);
AR_S32 SAMPLE_VPSS_VIFILE_VI_Bind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
AR_S32 SAMPLE_VPSS_VIFILE_VI_Unbind_VPSS(VI_PIPE ViPipe, VI_CHN ViChn, VPSS_GRP VpssGrp, VPSS_CHN VpssChn);
AR_S32 SAMPLE_VPSS_VIFILE_VI_Bind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);
AR_S32 SAMPLE_VPSS_VIFILE_VI_Unbind_VO(VI_PIPE ViPipe, VI_CHN ViChn, VO_LAYER VoLayer, VO_CHN VoChn);

#endif