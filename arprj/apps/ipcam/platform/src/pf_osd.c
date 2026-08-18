#include "ar_common.h"
#include "ar_buffer.h"
#include "ar_comm_sys.h"
#include "ar_comm_vb.h"
#include "ar_comm_isp.h"
#include "ar_comm_vi.h"
#include "ar_comm_vo.h"
#include "ar_comm_venc.h"
#include "ar_comm_vdec.h"
#include "ar_comm_vpss.h"
#include "ar_comm_region.h"
#include "ar_comm_adec.h"
#include "ar_comm_aenc.h"
#include "ar_comm_ai.h"
#include "ar_comm_ao.h"
#include "ar_comm_aio.h"
#include "ar_defines.h"
#include "ar_comm_hdmi.h"
#include "ar_combo_dev.h"
#include "ar_mipi_tx.h"
#include "util_profile.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_vpss.h"
#include "mpi_region.h"
#include "mpi_audio.h"
#include "mpi_isp.h"
#include "mpi_ae.h"
#include "mpi_awb.h"
#include "ar_math.h"
#include "ar_sns_ctrl.h"
#include "mpi_hdmi.h"

#include "hal_sys.h"
#include "pf_video.h"
#include "pf_osd.h"
#include "util_profile.h"


#define WITH_FORMAT


/* Region id define, totally 576:
*       OSD        ALG_OSD      ALG_RECT
*Grp 0: 0 - 7       64 - 95      320 - 351(all chn share 32 rect)
*Grp 1: 8 - 15      96 - 127
* ......
*Grp 7: 56 - 63     288 - 319
*/
#define IPC_PF_GET_ALG_OSD_HANDLE(Pipe) ((Pipe) * IPC_ALG_OSD_MAX_NUM + IPC_MAX_PIPELINE_NUM * IPC_MAX_STREAM_PER_PIPE)
#define IPC_PF_GET_ALG_RECT_HANDLE(Pipe) (8 * (IPC_ALG_OSD_MAX_NUM) + IPC_MAX_PIPELINE_NUM * IPC_MAX_STREAM_PER_PIPE)

#define IPC_OSD_FONT_SIZE_DEFAULT   (FONT_16X26_ID)
//#define IPC_OSD_FONT_SIZE_DEFAULT FONT_12X16_ID

typedef struct
{
    unsigned int m_width;
    unsigned int m_height;

    unsigned int m_start;
    unsigned int m_end;

    char* p_font_file; //dotmatrix file name
    char* p_content; //the content of dotmatrix
} IPC_FONT_DESC_S;

//the font bit width is aligned to 8 bits.
static IPC_FONT_DESC_S gFontList[] =
{
    /* shoule match with context */
    {8, 16, ' ', 'z', NULL, NULL}, // default
    {8, 6, 0x20, 0x7E, "/usrdata/local/factory/font/font_4_6.bin", NULL},
    {8, 8, 0x20, 0x7E, "/usrdata/local/factory/font/font_5_8.bin", NULL},
    {8, 12, 0x20, 0x7E, "/usrdata/local/factory/font/font_5_12.bin", NULL},
    {8, 8, 0x20, 0x7E, "/usrdata/local/factory/font/font_6_8.bin", NULL},
    {8, 10, 0x20, 0x7E, "/usrdata/local/factory/font/font_6_10.bin", NULL},
    {8, 12, 0x20, 0x7E, "/usrdata/local/factory/font/font_7_12.bin", NULL},
    {8, 8, 0x20, 0x7E, "/usrdata/local/factory/font/font_8_8.bin", NULL},
    {8, 12, 0x20, 0x7E, "/usrdata/local/factory/font/font_8_12.bin", NULL},
    {8, 12, 0x20, 0x7E, "/usrdata/local/factory/font/font_8_12_cyrillic.bin", NULL},
    {8, 14, 0x20, 0x7E, "/usrdata/local/factory/font/font_8_14.bin", NULL},
    {16, 16, 0x20, 0x7E, "/usrdata/local/factory/font/font_10_16.bin", NULL},
    {16, 16, 0x20, 0x7E, "/usrdata/local/factory/font/font_12_16.bin", NULL},
    {16, 20, 0x20, 0x7E, "/usrdata/local/factory/font/font_12_20.bin", NULL},
    {16, 26, 0x20, 0x7E, "/usrdata/local/factory/font/font_16_26.bin", NULL},
    {24, 36, 0x20, 0x7E, "/usrdata/local/factory/font/font_22_36.bin", NULL},
    {24, 40, 0x20, 0x7E, "/usrdata/local/factory/font/font_24_40.bin", NULL},
    {32, 53, 0x20, 0x7E, "/usrdata/local/factory/font/font_32_53.bin", NULL},
};

static const char* weekday_map[1][7] = {
    {
        "Mon.",
        "Tues.",
        "Wed.",
        "Thurs.",
        "Fri.",
        "Sat.",
        "Sun."
    }
};

static IPC_FONT_DESC_S * s_pAsciiDotMatrix = NULL; // bytes 128 x 16 x 1 size 8x16
static IPC_PF_OSD_PARAM_S s_astOsd[IPC_MAX_PIPELINE_NUM][IPC_MAX_STREAM_PER_PIPE] = {0};
//for ALG to draw string on the screen
static IPC_PF_OSD_PARAM_S s_astAlgOsd[IPC_MAX_PIPELINE_NUM] = {0};

static AR_U16 RGB888_TO_RGB555(AR_U32 u32RGB)
{
    AR_U16 RGB555 = 0;

    RGB555 = ((u32RGB >> 3) & 0x01F) /*R*/
            | ( ((u32RGB >> 11) & 0x01F) << 5) /*G*/
            | ( ((u32RGB >> 19) & 0x01F) << 10);

    return RGB555;
}

static AR_U32 IPC_OSD_GetVpssGrp(AR_U32 u32Chn, AR_U32 u32Stream)
{
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    if(strcmp(pChnCfg->stChannel[u32Chn].chType, "low_bw") == 0)
    {
        if(u32Stream == 0)
            return u32Chn * 2;
        else
            return u32Chn * 2 + 1;
    }
    //for "basic" pipeline
    return u32Chn;
}

static AR_U32 IPC_OSD_GetVpssChn(AR_U32 u32Chn, AR_U32 u32Stream)
{
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();

    if(strcmp(pChnCfg->stChannel[u32Chn].chType, "low_bw") == 0)
    {
        if(u32Stream == 0)
            return 0;
        else
            return u32Stream - 1;
    }
    //for "basic" pipeline
    return u32Stream;
}

static AR_U32 IPC_OSD_GetChnCopyAttr(AR_U32 u32Chn, AR_U32 u32Stream, IPC_MOD_ID_E enMode)
{
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();
    AR_U32 u32VpssGrp = 0, u32VpssChn = 0, u32VencChn = 0;

    //for "low_bw" pipeline, stream 0 is from vi main channel, stream 1-N from vi sub channel,
    //so we copy the buffer on stream 1 because the src of stream 2-N is from stream 1.
    if(strcmp(pChnCfg->stChannel[u32Chn].chType, "low_bw") == 0)
    {
        if(u32Stream == 1 || u32Stream == 2)
        {
            if(enMode == MOD_ID_VPSS)
            {
                u32VpssGrp = IPC_OSD_GetVpssGrp(u32Chn, u32Stream);
                u32VpssChn = IPC_OSD_GetVpssChn(u32Chn, u32Stream);

                if(pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn].bEnable
                    && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn].u32Width == pVioCfg->vi[u32Chn].channel_1_width
                    && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn].u32Height == pVioCfg->vi[u32Chn].channel_1_height)
                {
                    if(pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 1].bEnable
                        && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 1].u32Width == pVioCfg->vi[u32Chn].channel_1_width
                        && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 1].u32Height == pVioCfg->vi[u32Chn].channel_1_height)
                    {
                        return 1;
                    }
                    #if 0
                    else if(pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 2].bEnable
                        && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 2].u32Width == pVioCfg->vi[u32Chn].channel_1_width
                        && pVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn + 2].u32Height == pVioCfg->vi[u32Chn].channel_1_height)
                    {
                        return 1;
                    }
                    #endif
                }

                return 0;
            }
            else
            {
                u32VencChn = IPC_GET_VENC_CHN_ID(u32Chn, u32Stream);
                if(pVencCfg->vencStream[u32Chn][u32VencChn].enable
                    && pVencCfg->vencStream[u32Chn][u32VencChn].cfg.width == pVioCfg->vi[u32Chn].channel_1_width
                    && pVencCfg->vencStream[u32Chn][u32VencChn].cfg.height == pVioCfg->vi[u32Chn].channel_1_height)
                {
                    if(pVencCfg->vencStream[u32Chn][u32VencChn + 1].enable
                        && pVencCfg->vencStream[u32Chn][u32VencChn + 1].cfg.width == pVioCfg->vi[u32Chn].channel_1_width
                        && pVencCfg->vencStream[u32Chn][u32VencChn + 1].cfg.height == pVioCfg->vi[u32Chn].channel_1_height)
                    {
                        return 1;
                    }
                    #if 0
                    else if(pVencCfg->vencStream[u32Chn][u32VencChn + 2].enable
                       && pVencCfg->vencStream[u32Chn][u32VencChn + 2].cfg.width == pVioCfg->vi[u32Chn].channel_1_width
                       && pVencCfg->vencStream[u32Chn][u32VencChn + 2].cfg.height == pVioCfg->vi[u32Chn].channel_1_height)
                    {
                        return 1;
                    }
                    #endif
                }

                return 0;
            }
        }
    }
    //for "basic" pipeline, all stream comes from one src, so we copy it on the first stream
    else
    {
        if(u32Stream == 0)
        {
            return 1;
        }
    }

    return 0;
}

//By default, we use 16x26 size.
static AR_S32 IPC_PF_OSD_InitFont(OSD_FONT_IDX_E enFontId)
{
    s_pAsciiDotMatrix = &gFontList[enFontId];
    s_pAsciiDotMatrix->p_content = IPC_MID_UTIL_OpenFile(s_pAsciiDotMatrix->p_font_file, NULL);

    return 0;
}

static void IPC_PF_OSD_GetFontSize(OSD_FONT_IDX_E enFontId, AR_U32 * pW, AR_U32 * pH)
{
    *pW = gFontList[enFontId].m_width;
    *pH = gFontList[enFontId].m_height;
}

static void IPC_PF_OSD_UnInitFont()
{
    IPC_MID_UTIL_FreeMem((void **)&s_pAsciiDotMatrix->p_content);
}

#define WEEK(y,m,d) ((d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7)
static void IPC_PF_OSD_TimeToText(AR_S32 ch, AR_S32 stream, AR_CHAR *text)
{
    AR_S32 year = 0, month = 0, mday = 0, hour = 0, min = 0, sec = 0, week = 0;
    AR_U8 dateFormat = 0, timeFormat = 0, displayWeek;

    struct tm newtime;
    time_t ltime;
    ltime = time(NULL);
    localtime_r(&ltime, &newtime);

    year = newtime.tm_year+1900;
    month = newtime.tm_mon+1;
    mday = newtime.tm_mday;
    hour = newtime.tm_hour;
    min = newtime.tm_min;
    sec = newtime.tm_sec;
    week = WEEK(year, month, mday);

    AR_CHAR date_buf[128] = {0}, time_buf[128] = {0}, weekday[32] = {0};
    const AR_CHAR *meridiem = (hour < 12)? "AM":"PM";
    IPC_CFG_OSD_S * pstOsdCfg = IPC_CFG_OSD_GetParam();
    //IPC_CFG_SYSTEM_S * pstSysCfg = IPC_CFG_SYSTEM_GetParam();


    if(pstOsdCfg->osdInfo[ch][stream].osdDatetime.enable)
    {
        dateFormat = pstOsdCfg->osdInfo[ch][stream].osdDatetime.dateFormat;
        timeFormat = pstOsdCfg->osdInfo[ch][stream].osdDatetime.timeFmt;
        displayWeek = pstOsdCfg->osdInfo[ch][stream].osdDatetime.displayWeek;

        switch (dateFormat)
        {
            default:
            case 0:
                snprintf(date_buf, sizeof(date_buf), "%04d-%02d-%02d", year, month, mday);
                break;
            case 1:
                snprintf(date_buf, sizeof(date_buf), "%02d-%02d-%04d", month, mday, year);
                break;
            case 2:
                snprintf(date_buf, sizeof(date_buf), "%04d/%02d/%02d", year, month, mday);
                break;
            case 3:
                snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d", month, mday, year);
                break;
            case 4:
                snprintf(date_buf, sizeof(date_buf), "%02d-%02d-%04d", mday, month, year);
                break;
            case 5:
                snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d", mday, month, year);
                break;
        }

        switch (timeFormat)
        {
            default:
            case 0:
                snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", hour, min, sec);
                break;
            case 1:
                snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d %s", hour < 12 ? hour : hour - 12, min, sec, meridiem);
                break;
        }

        if (displayWeek)
        {
            snprintf(weekday, sizeof(weekday), "%s", weekday_map[0][week]);
        }
    }

    if (pstOsdCfg->osdInfo[ch][stream].osdChannelName.enable)
    {
        sprintf(text, "%s %s %s %s", pstOsdCfg->osdInfo[ch][stream].osdChannelName.text, date_buf, time_buf, weekday);
    }
    else
    {
        sprintf(text, "%s %s %s", date_buf, time_buf, weekday);
    }
}

static void osd_calc_xy(AR_S32 ch, AR_S32 stream, AR_S32 *x, AR_S32 *y)
{
    IPC_CFG_OSD_S * pstOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_VPSS_S * pstVpssCfg = IPC_CFG_VPSS_GetParam();

    AR_U32 u32VpssGrp = IPC_OSD_GetVpssGrp(ch, stream);//i;//vpss grp
    AR_U32 u32VpssChn = IPC_OSD_GetVpssChn(ch, stream);//j;//vpss chn

    float x_ratio = pstOsdCfg->osdInfo[ch][stream].x_ratio;
    float y_ratio = pstOsdCfg->osdInfo[ch][stream].y_ratio;
    AR_S32 stream_width = pstVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn].u32Width;
    AR_S32 stream_height = pstVpssCfg->stGrpCfg[u32VpssGrp].stChnCfg[u32VpssChn].u32Height;

    AR_S32 canvasX = (AR_S32)(x_ratio * (float)stream_width);
    AR_S32 canvasY = (AR_S32)(y_ratio * (float)stream_height);
    *x = canvasX;
    *y = canvasY;
}

static void IPC_PF_OSD_Show(IPC_PF_OSD_PARAM_S *pOsd, RGN_HANDLE handle, MPP_CHN_S * pstChn, AR_BOOL bShow)
{
    AR_S32 s32Ret = 0;
    RGN_CHN_ATTR_S stChnAttr = {0};
    //IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();

    s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, pstChn, &stChnAttr);
    if(s32Ret)
    {
        PRINT_ERR("Get chn %d display attr failed.\n", pstChn->s32ChnId);
        return;
    }

    stChnAttr.bShow = bShow;

    s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, pstChn, &stChnAttr);
    if(s32Ret)
    {
        PRINT_ERR("Set chn %d display attr failed.\n", pstChn->s32ChnId);
        return;
    }

    //pOsd->valid = 0;
}

static AR_S32 IPC_PF_OSD_TextToDotMatrix(IPC_PF_OSD_PARAM_S *pOsd)
{
    AR_S32 s32FontW = 0, s32FontH = 0;
    AR_S32 i = 0, idx = 0;
    AR_CHAR * pSrc = NULL, * pDst = NULL;
    AR_S32 s32StrLen = strlen(pOsd->text);

    if(!pOsd->pDotMatrix)
    {
        PRINT_ERR("Dotmatrix has not been malloced.\n");
        return -1;
    }

    s32FontW = s_pAsciiDotMatrix->m_width;
    s32FontH = s_pAsciiDotMatrix->m_height;

    memset(pOsd->pDotMatrix, 0, sizeof(pOsd->text) * s_pAsciiDotMatrix->m_width * s_pAsciiDotMatrix->m_height / 8);

    for(i = 0; i < s32StrLen; i++)
    {
        if(pOsd->text[i] < s_pAsciiDotMatrix->m_start || pOsd->text[i] > s_pAsciiDotMatrix->m_end)
        {
            PRINT_ERR("Ascii not supported.\n");
            continue;
        }

        idx = pOsd->text[i] - s_pAsciiDotMatrix->m_start;
        pDst = (AR_CHAR *)(pOsd->pDotMatrix + i * s32FontW * s32FontH / 8);
        pSrc = s_pAsciiDotMatrix->p_content + idx * s32FontW * s32FontH / 8;
        memcpy(pDst, pSrc, s32FontW * s32FontH / 8);
    }

    //pOsd->stBmp.enPixelFormat = bitmap;
    pOsd->stBmp.u32Width = s32StrLen * s32FontW;
    pOsd->stBmp.u32Height = s32FontH;

	return 0;
}

static AR_S32 IPC_PF_OSD_DotMatrixToCanvas(IPC_PF_OSD_PARAM_S *pOsd, RGN_CANVAS_INFO_S * pCanvas)
{
    AR_U32 i = 0, j = 0;
    AR_U32 u32FontWBits = 0; //each ascii width, bit
    AR_U32 u32FontHBits = 0; //each ascii height, bit
    AR_U32 u32FontSize = 0; //byte
    AR_U32 u32PixelW = 0;
    AR_U32 u32PixelH = 0;
    AR_U32 u32CanvasW = 0;
    AR_U32 u32CanvasH = 0;
    AR_U32 u32CanvasS = 0;
    AR_U32 u32StrLen = 0;
    AR_CHAR * pDst = 0;
    AR_CHAR * pSrc = 0;

    IPC_PF_OSD_GetFontSize(pOsd->enFontSizeId, &u32FontWBits, &u32FontHBits);
    u32FontSize = u32FontWBits * u32FontHBits / 8;

    u32PixelW = pOsd->stBmp.u32Width;
    u32PixelH = pOsd->stBmp.u32Height;
    u32CanvasW = pCanvas->stSize.u32Width;
    u32CanvasH = pCanvas->stSize.u32Height;
    u32CanvasS = pCanvas->u32Stride;
    u32StrLen = strlen(pOsd->text);

    if(u32PixelW > u32CanvasW || u32PixelH > u32CanvasH)
    {
        PRINT_ERR("The canvas[%d x %d] is not enough for this OSD[%d x %d].\n",
            u32CanvasW, u32CanvasH, u32PixelW, u32PixelH);
        return -1;
    }

    //we need to reshape to 2D dotmatrix
    memset((AR_CHAR *)pCanvas->u64VirtAddr, 0, u32CanvasS * u32CanvasH);
    for(i = 0; i < u32PixelH; i++)
    {
        pDst = (AR_CHAR *)(pCanvas->u64VirtAddr + i * u32CanvasS);

        for(j = 0; j < u32StrLen; j++)
        {
            pSrc = (AR_CHAR *)(pOsd->pDotMatrix + j * u32FontSize + i * u32FontWBits / 8);
            char * pCanvasAddr = pDst + j * u32FontWBits / 8;
            memcpy(pCanvasAddr, pSrc, u32FontWBits / 8);
        }
    }

    return 0;
}

static AR_S32 IPC_PF_OSD_DotMatrixToBMP(IPC_PF_OSD_PARAM_S *pOsd, RGN_CANVAS_INFO_S * pCanvas, AR_U32 u32RGB)
{
    AR_U32 i = 0, j = 0, n = 0;
    AR_U32 u32PixelW = 0;
    AR_U32 u32PixelH = 0;
    AR_U16 * pu16Tmp = 0;
    AR_UCHAR bit = 0;
    AR_U8 u8ChLine = 0, u8Flag = 0;
    AR_U32 u32FontSizeByte = s_pAsciiDotMatrix->m_width * s_pAsciiDotMatrix->m_height / 8;
    AR_U32 u32Noffset = 0, u32Hoffset = 0, u32NdotOffset = 0;

    pOsd->stBmp.u32Width = pCanvas->stSize.u32Width;
    pOsd->stBmp.u32Height = pCanvas->stSize.u32Height;
    pOsd->stBmp.pData = (void *)pCanvas->u64VirtAddr;
    pOsd->stBmp.enPixelFormat = pCanvas->enPixelFmt;

    if(pOsd->stBmp.enPixelFormat != PIXEL_FORMAT_ARGB_1555)
    {
        PRINT_ERR("Only implemented ARGB1555, other format will come soon.\n");
        return -1;
    }

    u32PixelW = strlen(pOsd->text) * s_pAsciiDotMatrix->m_width; //how many pixels per line totally
    u32PixelH = s_pAsciiDotMatrix->m_height;

    if(u32PixelW > pOsd->stBmp.u32Width || u32PixelH > pOsd->stBmp.u32Height)
    {
        PRINT_ERR("The canvas[%d x %d] is not enough for this OSD[%d x %d].\n",
            pOsd->stBmp.u32Width, pOsd->stBmp.u32Height,
            u32PixelW, u32PixelH);
        return -1;
    }

    memset(pOsd->stBmp.pData, 0, pCanvas->u32Stride * pCanvas->stSize.u32Height);

    for(n = 0; n < strlen(pOsd->text); n++)
    {
        u32Noffset = n * s_pAsciiDotMatrix->m_width * 2;//argb1555 = 2 byte
        u32NdotOffset = n * u32FontSizeByte;

        for(i = 0; i < s_pAsciiDotMatrix->m_height; i++)
        {
            u32Hoffset = i * pCanvas->u32Stride;
            for(j = 0; j < s_pAsciiDotMatrix->m_width; j++) //each pixel
            {
                pu16Tmp = (AR_U16 *)(pOsd->stBmp.pData + u32Hoffset + u32Noffset + j * 2 );
                u8ChLine = *(AR_U8 *)(pOsd->pDotMatrix + u32NdotOffset + (j / 8) + i * s_pAsciiDotMatrix->m_width / 8);
                bit = j % 8;
                u8Flag = u8ChLine & (0x080 >> bit);

                if(u8Flag)
                {
                    *pu16Tmp = 0x8000 | RGB888_TO_RGB555(u32RGB);
                }
            }
        }
    }

    return 0;
}

//update OSD string, called by a timer thread.
void IPC_PF_OSD_Update(void)
{
    AR_U32 i, j;
    AR_S32 s32Ret = 0;
    RGN_HANDLE handle;
    RGN_CANVAS_INFO_S stCanvasInfo = {0};
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
	IPC_MOD_ID_E enMode = MOD_ID_VPSS;
#ifndef WITH_FORMAT
	AR_CHAR strDate[16] = {0};
	AR_CHAR strTime[16] = {0};
#endif

    //0. get date and time str

#ifndef WITH_FORMAT
    IPC_MID_UTIL_GetDateAndTimeStr(strDate, strTime, 0);
#endif

    //1. text to dot matrix
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }
        //we don't draw osd on ir cameras, do we need to do this ?
        if(strcmp(pChnCfg->stChannel[i].chType, "infrared") == 0)
        {
            continue;
        }

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            if(!pOsdCfg->osdInfo[i][j].osdChannelName.enable && !pOsdCfg->osdInfo[i][j].osdDatetime.enable)
            {
                if(s_astOsd[i][j].valid)
                {
                    if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
                    {
                        stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                        stChn.s32DevId = 0;
                        stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
                    }
                    else
                    {
                        stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                        stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;
                        stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;
                    }

                    //clear OSD.
                    handle = IPC_GET_ID_FROM_CHN_STREAM(i, j);
                    IPC_PF_OSD_Show(&s_astOsd[i][j], handle, &stChn, AR_FALSE);
                }

                continue;
            }

            memset(s_astOsd[i][j].text, 0, sizeof(s_astOsd[i][j].text));
#ifndef WITH_FORMAT
            if(pOsdCfg->osdInfo[i][j].osdChannelName.enable)
            {
                strcpy(s_astOsd[i][j].text, (AR_CHAR *)(pOsdCfg->osdInfo[i][j].osdChannelName.text));
                strcat(s_astOsd[i][j].text, " ");
            }

            if(pOsdCfg->osdInfo[i][j].osdDatetime.enable)
            {
                strcat(s_astOsd[i][j].text, strDate);
                strcat(s_astOsd[i][j].text, " ");
                strcat(s_astOsd[i][j].text, strTime);
            }
#else
            IPC_PF_OSD_TimeToText(i, j, s_astOsd[i][j].text);

#endif
            IPC_PF_OSD_TextToDotMatrix(&s_astOsd[i][j]);

            handle = IPC_GET_ID_FROM_CHN_STREAM(i, j);

            s32Ret = AR_MPI_RGN_GetCanvasInfo(handle, &stCanvasInfo);
            if(s32Ret)
            {
                PRINT_ERR("Get canvas info failed.\n");
                continue;
            }

            if((s_astOsd[i][j].stBmp.u32Width > stCanvasInfo.stSize.u32Width) || (s_astOsd[i][j].stBmp.u32Height > stCanvasInfo.stSize.u32Height))
            {
                RGN_ATTR_S stRgnAttr = {0};
                RGN_CHN_ATTR_S stChnAttr = {0};

                if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
                {
                	enMode = MOD_ID_VENC;
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                    stChn.s32DevId = 0;
                    stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
                }
                else
                {
                	enMode = MOD_ID_VPSS;
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                    stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//vpss grp
                    stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//vpss chn
                }

                AR_MPI_RGN_DetachFromChn(handle, &stChn);

                s32Ret = AR_MPI_RGN_Destroy(handle);
                if(s32Ret)
                {
                    PRINT_ERR("Destroy Region failed, err: %d.\n", s32Ret);
                    return;
                }

                stRgnAttr.enType = OVERLAY_RGN;
                stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_GRAY1;
                stRgnAttr.unAttr.stOverlay.stSize.u32Width = s_astOsd[i][j].stBmp.u32Width;
                stRgnAttr.unAttr.stOverlay.stSize.u32Height = s_astOsd[i][j].stBmp.u32Height;
                stRgnAttr.unAttr.stOverlay.u32CanvasNum = 2;
                stRgnAttr.unAttr.stOverlay.u32BgColor = 0x7FFFFFFF;

                stChnAttr.bShow = AR_TRUE;
                stChnAttr.enType = OVERLAY_RGN;
                stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0x20;
                stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0xff;
                stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
                stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = AR_FALSE;
                stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = AR_FALSE;
                stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp = 0;
                stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = 0;
                stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x0000; //ARGB1555
                stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0xffff;
                stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = s_astOsd[i][j].x;
                stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = s_astOsd[i][j].y;

                AR_MPI_RGN_Create(handle, &stRgnAttr);

                //if the resource of stream2 and stream3 is the same with vi, cp a new buffer
                if(j == 1)
                {
                    RGN_CHN_PARA_S stChnPara;
                    stChnPara.bCopyBufEnable = IPC_OSD_GetChnCopyAttr(i, j, enMode);

                    AR_MPI_RGN_Set_Chn_Para(&stChn, &stChnPara);
                }

                AR_MPI_RGN_AttachToChn(handle, &stChn, &stChnAttr);

                s32Ret = AR_MPI_RGN_GetCanvasInfo(handle, &stCanvasInfo);
                if(s32Ret)
                {
                    PRINT_ERR("Get canvas info failed.\n");
                    continue;
                }
            }


            IPC_PF_OSD_DotMatrixToCanvas(&s_astOsd[i][j], &stCanvasInfo);
            //4. update canvas
            s32Ret = AR_MPI_RGN_UpdateCanvas(handle);
            if(s32Ret)
            {
                PRINT_ERR("Update canvas failed.\n");
                continue;
            }
            if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//0;
                stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//IPC_GET_ID_FROM_CHN_STREAM(i, j);
            }
						static AR_S32 s32SetVi = 0;
            if(0 ==i && 0 == j && !s32SetVi)
            {
                VI_CH_CF50_CMP_ATTR_T pstChnCmpAttr = {0};
                pstChnCmpAttr.stChCmpAttr.s32OsdCount = 1;
                pstChnCmpAttr.stChCmpAttr.stOsd[0].s32X = s_astOsd[i][j].x;
                pstChnCmpAttr.stChCmpAttr.stOsd[0].s32Y = s_astOsd[i][j].x;
                pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Width = 512;
                pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Height = 32;
								pstChnCmpAttr.stChCmpAttr.enMode = CF50_CMP_MODE_LOSS;
								pstChnCmpAttr.stChCmpAttr.enCmpRate = CF50_CMP_RATE_LOW;
                AR_MPI_VI_SetChnCmpAttr(0, 0, &pstChnCmpAttr);
                s32SetVi = 1;
                PRINT_INFO("VI_SetChnCmpAttr X:%d Y:%d W:%d H:%d.\n",
                    pstChnCmpAttr.stChCmpAttr.stOsd[0].s32X,
                    pstChnCmpAttr.stChCmpAttr.stOsd[0].s32Y,
                    pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Width,
                    pstChnCmpAttr.stChCmpAttr.stOsd[0].u32Height);
            }

            //clear OSD: detach rgn from this channel.
            IPC_PF_OSD_Show(&s_astOsd[i][j], handle, &stChn, AR_TRUE);

        }
    }

}

//For another algrithom app to draw osd on the screen.
AR_S32 IPC_PF_OSD_InitEx()
{
    int i = 0, j = 0, n = 0;
    AR_S32 s32Ret = 0;
    //AR_U32 u32X = 0, u32Y = 0;
    RGN_HANDLE strHandle = 0;
    RGN_HANDLE rectHandle = 0;
    RGN_ATTR_S stStrRgnAttr = {0};
    RGN_ATTR_S stRectRgnAttr = {0};
    RGN_CHN_ATTR_S stStrChnAttr = {0};
    RGN_CHN_ATTR_S stRectChnAttr = {0};
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();

    stStrRgnAttr.enType = OVERLAY_RGN;
    stStrRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_ARGB_1555;//PIXEL_FORMAT_GRAY1;
    stStrRgnAttr.unAttr.stOverlay.stSize.u32Width = 320; //320 pixel
    stStrRgnAttr.unAttr.stOverlay.stSize.u32Height = 32; //tmp
    stStrRgnAttr.unAttr.stOverlay.u32CanvasNum = 2; //ping pong
    stStrRgnAttr.unAttr.stOverlay.u32BgColor = 0x3FFFFFFF; //Alpha = 0x3F,RGB = white

    stRectRgnAttr.enType = COVER_RGN;

    stStrChnAttr.bShow = AR_TRUE;
    stStrChnAttr.enType = OVERLAY_RGN;
    stStrChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 10;
    stStrChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 128;
    stStrChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
    stStrChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = AR_FALSE;
    stStrChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = AR_FALSE;
    stStrChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp = 0;
    stStrChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = 0;

    stRectChnAttr.bShow = AR_TRUE;
    stRectChnAttr.enType = COVER_RGN;
    stRectChnAttr.unChnAttr.stCoverChn.enCoverType = AREA_QUAD_RANGLE;
    stRectChnAttr.unChnAttr.stCoverChn.stQuadRangle.bSolid = AR_FALSE;
    stRectChnAttr.unChnAttr.stCoverChn.stQuadRangle.u32Thick = 2;
    stRectChnAttr.unChnAttr.stCoverChn.u32Color = 0xFF0000FF; //solid blue
    stRectChnAttr.unChnAttr.stCoverChn.enCoordinate = RGN_ABS_COOR;

    //Create 32 rect regions for all grp/chn to share.
    for(n = 0; n < IPC_ALG_OSD_MAX_NUM; n++)
    {
        rectHandle = IPC_PF_GET_ALG_RECT_HANDLE(i) + n;
        s32Ret = AR_MPI_RGN_Create(rectHandle, &stRectRgnAttr);
        if(s32Ret)
        {
            PRINT_ERR("Create Region failed, err: %d.\n", s32Ret);
            return -1;
        }
    }

    //Create 32 str regions for each grp, they will display on all channel if "alg_osd_enable" is on.
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        s_astAlgOsd[i].channel = i;
        s_astAlgOsd[i].enFontSizeId = IPC_OSD_FONT_SIZE_DEFAULT;
        //malloc max memory for dotmatrix
        s_astAlgOsd[i].pDotMatrix = malloc(sizeof(s_astAlgOsd[i].text)
                                            * s_pAsciiDotMatrix->m_width
                                            * s_pAsciiDotMatrix->m_height / 8); //16bit x 26 per ascii.
        s_astAlgOsd[i].stBmp.enPixelFormat = PIXEL_FORMAT_ARGB_1555;//PIXEL_FORMAT_GRAY1;
        s_astAlgOsd[i].valid = 1;

        for(n = 0; n < IPC_ALG_OSD_MAX_NUM; n++)
        {
            strHandle = IPC_PF_GET_ALG_OSD_HANDLE(i) + n;
            s32Ret = AR_MPI_RGN_Create(strHandle, &stStrRgnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Create Region failed, err: %d.\n", s32Ret);
                return -1;
            }

            rectHandle = IPC_PF_GET_ALG_RECT_HANDLE(i) + n;

            for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
            {
                if(!pOsdCfg->osdInfo[i][j].u32AlgOsdEnable)
                {
                    continue;
                }

                //venc only supports max 4 stream per channel, so for vpss ch > 4, osd can only attach to vpss.
                if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
                {
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                    stChn.s32DevId = 0;
                    stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
                }
                else
                {
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                    stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//vpss grp
                    stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//vpss chn
                }

                s32Ret = AR_MPI_RGN_AttachToChn(strHandle, &stChn, &stStrChnAttr);
                if(s32Ret)
                {
                    PRINT_ERR("Attach region to chn failed.\n");
                    return -1;
                }

                s32Ret = AR_MPI_RGN_AttachToChn(rectHandle, &stChn, &stRectChnAttr);
                if(s32Ret)
                {
                    PRINT_ERR("Attach region to chn failed.\n");
                    return -1;
                }
            }
        }
    }

    return 0;
}

///////////////////////////////////////
//This OSD apis need to be called AFTER video channels are created.
//Handle is the pipeline id (0-7)
AR_S32 IPC_PF_OSD_Init()
{
    ProfileStart(__FUNCTION__);

    int i = 0, j = 0;
    AR_S32 s32Ret = 0;
    AR_S32 s32X = 0, s32Y = 0;
    RGN_HANDLE handle = 0;
    RGN_ATTR_S stRgnAttr = {0};
    RGN_CHN_ATTR_S stChnAttr = {0};
    MPP_CHN_S stChn = {0};
    IPC_MOD_ID_E enMode = MOD_ID_VPSS;
    //pthread_t pid;
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    //IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    //IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    //IPC_CFG_VENC_S * pVencCfg = IPC_CFG_VENC_GetParam();

    //pthread_rwlock_init(&overlay_lock, NULL);

    IPC_PF_OSD_InitFont(IPC_OSD_FONT_SIZE_DEFAULT);

    stRgnAttr.enType = OVERLAY_RGN;
    stRgnAttr.unAttr.stOverlay.enPixelFmt = PIXEL_FORMAT_GRAY1;
    stRgnAttr.unAttr.stOverlay.stSize.u32Width = 320; //tmp
    stRgnAttr.unAttr.stOverlay.stSize.u32Height = 30; //tmp
    stRgnAttr.unAttr.stOverlay.u32CanvasNum = 2; //ping pong
    stRgnAttr.unAttr.stOverlay.u32BgColor = 0x7FFFFFFF; //Alpha = 0x7F,RGB = white

    stChnAttr.bShow = AR_TRUE;
    stChnAttr.enType = OVERLAY_RGN;
    stChnAttr.unChnAttr.stOverlayChn.u32BgAlpha = 0x20;
    stChnAttr.unChnAttr.stOverlayChn.u32FgAlpha = 0xff;
    stChnAttr.unChnAttr.stOverlayChn.u32Layer = 0;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bQpDisable = AR_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.bAbsQp = AR_FALSE;
    stChnAttr.unChnAttr.stOverlayChn.stQpInfo.s32Qp = 0;
    stChnAttr.unChnAttr.stOverlayChn.stInvertColor.bInvColEn = 0;
    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[0] = 0x0000; //ARGB1555
    stChnAttr.unChnAttr.stOverlayChn.u16ColorLUT[1] = 0xffff;

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }

        //we don't draw osd on ir cameras, do we need to do this ?
        if(strcmp(pChnCfg->stChannel[i].chType, "infrared") == 0)
        {
            continue;
        }

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            s_astOsd[i][j].channel = i;
            s_astOsd[i][j].stream = j;
            s_astOsd[i][j].enFontSizeId = IPC_OSD_FONT_SIZE_DEFAULT;
            //malloc max memory for dotmatrix
            s_astOsd[i][j].pDotMatrix = malloc(sizeof(s_astOsd[i][j].text)
                                                * s_pAsciiDotMatrix->m_width
                                                * s_pAsciiDotMatrix->m_height / 8); //16bit x 26 per ascii.
            s_astOsd[i][j].stBmp.enPixelFormat = PIXEL_FORMAT_GRAY1;
            s_astOsd[i][j].valid = 1;

            handle = IPC_GET_ID_FROM_CHN_STREAM(i, j);
            s32Ret = AR_MPI_RGN_Create(handle, &stRgnAttr);

            if(s32Ret)
            {
                PRINT_ERR("Create Region failed, err: %d.\n", s32Ret);
                goto EXIT;
            }

            osd_calc_xy(i, j, &s32X, &s32Y);

            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = s32X;
            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = s32Y;

            if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
            {
                enMode = MOD_ID_VENC;
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
            }
            else
            {
            	enMode = MOD_ID_VPSS;
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//vpss grp
                stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//vpss chn
            }

            //if the resource of stream2 and stream3 is the same with vi, cp a new buffer
            if(j == 1)
            {
                RGN_CHN_PARA_S stChnPara;
                stChnPara.bCopyBufEnable = IPC_OSD_GetChnCopyAttr(i, j, enMode);

                AR_MPI_RGN_Set_Chn_Para(&stChn, &stChnPara);
            }

            AR_MPI_RGN_AttachToChn(handle, &stChn, &stChnAttr);

            s_astOsd[i][j].x = s32X;
            s_astOsd[i][j].y = s32Y;
        }
    }

    IPC_MID_UTIL_TIMER_AddTask(IPC_PF_OSD_Update, 2, TIMER_TRUE, TIMER_FALSE);  // 2*250ms = 500ms

    s32Ret = IPC_PF_OSD_InitEx(); // about 10M for mem reduce
    ProfileEnd(__FUNCTION__);

    return s32Ret;

EXIT:
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }
        //we don't draw osd on ir cameras, do we need to do this ?
        if(strcmp(pChnCfg->stChannel[i].chType, "infrared") == 0)
        {
            continue;
        }

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            handle = IPC_GET_ID_FROM_CHN_STREAM(i, j);

            s32Ret = AR_MPI_RGN_Destroy(handle);
            if(s32Ret)
            {
                //do nothing, maybe this region has not been created yet.
            }

            if(s_astOsd[i][j].pDotMatrix)
            {
                free(s_astOsd[i][j].pDotMatrix);
            }
        }
    }

    IPC_PF_OSD_UnInitFont();
    ProfileEnd(__FUNCTION__);

    return s32Ret;
}

AR_S32 IPC_PF_OSD_UnInitEx()
{
    int i = 0, j = 0, n = 0;
    AR_S32 s32Ret = 0;
    RGN_HANDLE strHandle = 0;
    RGN_HANDLE rectHandle = 0;

    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();

    //Create 32 str regions for each grp, they will display on all channel if "alg_osd_enable" is on.
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        for(n = 0; n < IPC_ALG_OSD_MAX_NUM; n++)
        {
            strHandle = IPC_PF_GET_ALG_OSD_HANDLE(i) + n;
            rectHandle = IPC_PF_GET_ALG_RECT_HANDLE(i) + n;

            for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
            {
                if(!pOsdCfg->osdInfo[i][j].u32AlgOsdEnable)
                {
                    continue;
                }

                //venc only supports max 4 stream per channel, so for vpss ch > 4, osd can only attach to vpss.
                if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
                {
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                    stChn.s32DevId = 0;
                    stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
                }
                else
                {
                    stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                    stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//vpss grp
                    stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//vpss chn
                }

                s32Ret = AR_MPI_RGN_DetachFromChn(strHandle, &stChn);
                if(s32Ret)
                {
                    PRINT_ERR("Detach region to chn failed.\n");
                    return -1;
                }

                s32Ret = AR_MPI_RGN_DetachFromChn(rectHandle, &stChn);
                if(s32Ret)
                {
                    PRINT_ERR("Detach region to chn failed.\n");
                    return -1;
                }
            }

            s32Ret = AR_MPI_RGN_Destroy(strHandle);
            if(s32Ret)
            {
                PRINT_ERR("Destroy Region failed, err: %d.\n", s32Ret);
                return -1;
            }
        }

        if(s_astAlgOsd[i].pDotMatrix)
        {
            free(s_astAlgOsd[i].pDotMatrix);
        }
        s_astAlgOsd[i].valid = 0;
    }

    //Create 32 rect regions for all grp/chn to share.
    for(n = 0; n < IPC_ALG_OSD_MAX_NUM; n++)
    {
        rectHandle = IPC_PF_GET_ALG_RECT_HANDLE(i) + n;
        s32Ret = AR_MPI_RGN_Destroy(rectHandle);
        if(s32Ret)
        {
            PRINT_ERR("Destroy Region failed, err: %d.\n", s32Ret);
            return -1;
        }
    }

    return 0;
}

void IPC_PF_OSD_UnInit()
{
    RGN_HANDLE handle = 0;
    AR_S32 i, j;
    AR_S32 s32Ret;
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_CHANNEL_S * pChnCfg = IPC_CFG_CHANNEL_GetParam();
    IPC_PF_OSD_UnInitEx();

    IPC_MID_UTIL_TIMER_DelTask(IPC_PF_OSD_Update);

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        if(!pChnCfg->stChannel[i].u32Enable)
        {
            continue;
        }
        //we don't draw osd on ir cameras, do we need to do this ?
        if(strcmp(pChnCfg->stChannel[i].chType, "infrared") == 0)
        {
            continue;
        }

        for(j = 0; j < IPC_MAX_STREAM_PER_PIPE; j++)
        {
            handle = IPC_GET_ID_FROM_CHN_STREAM(i, j);
            if(j < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[i][j].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(i, j);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(i, j);//i;//vpss grp
                stChn.s32ChnId = IPC_OSD_GetVpssChn(i, j);//j;//vpss chn
            }

            AR_MPI_RGN_DetachFromChn(handle, &stChn);

            s32Ret = AR_MPI_RGN_Destroy(handle);
            if(s32Ret)
            {
                PRINT_ERR("Destroy Region failed, err: %d.\n", s32Ret);
                return;
            }

            if(s_astOsd[i][j].pDotMatrix)
            {
                free(s_astOsd[i][j].pDotMatrix);
            }
        }
    }

    IPC_PF_OSD_UnInitFont();

    memset(s_astOsd, 0, sizeof(s_astOsd));

}

AR_S32 IPC_PF_OSD_Suspend()
{
    AR_S32 s32Ret = 0;
    
    s32Ret = AR_MPI_RGN_Suspend();
    if(s32Ret < 0)
    {
        printf("[%s %d] rgn suspend failed while suspend osd\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    return 0;
}

AR_S32 IPC_PF_OSD_Resume()
{
    AR_S32 s32Ret = 0;
    
    s32Ret = AR_MPI_RGN_Resume();
    if(s32Ret < 0)
    {
        printf("[%s %d] rgn resume failed while resume osd\n", __FUNCTION__, __LINE__);
        return -1;
    }
    
    return 0;
}


//Only support single app call
//if u32strnum == 0, means clear all osd.
AR_S32 IPC_PF_OSD_DrawString(VPSS_GRP VpssGrp, AR_U32 u32StrNum, IPC_OSD_STRING_S * pastStr, AR_U32 u32ImgW, AR_U32 u32ImgH)
{
    AR_S32 s32Ret;
    RGN_HANDLE handle;
    VPSS_CHN VpssChn = 0;
    RGN_CANVAS_INFO_S stCanvasInfo = {0};
    RGN_CHN_ATTR_S stChnAttr = {0};
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    AR_FLOAT fWRatio = 0.0, fHRatio = 0.0;
    AR_U32 u32NewX, u32NewY;
    AR_U32 i = 0;
    AR_S32 s32Pipe = 0, s32Stream = 0;

    s32Pipe = VpssGrp/2;
    for(i = 0; i < u32StrNum; i++)
    {
        //update string
        memset(s_astAlgOsd[s32Pipe].text, 0, 128);
        strcpy(s_astAlgOsd[s32Pipe].text, pastStr[i].achStr);

        IPC_PF_OSD_TextToDotMatrix(&s_astAlgOsd[s32Pipe]);
        handle = IPC_PF_GET_ALG_OSD_HANDLE(s32Pipe) + i; //eash osd is a region.

        //draw the osd on each pipe channel.
        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
                continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);
                stChn.s32ChnId = VpssChn;
            }

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }

            fWRatio = (AR_FLOAT)u32ImgW / (AR_FLOAT)pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width;
            fHRatio = (AR_FLOAT)u32ImgH / (AR_FLOAT)pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height;

            u32NewX = (AR_FLOAT)pastStr[i].u32X / fWRatio;
            u32NewY = (AR_FLOAT)pastStr[i].u32Y / fHRatio;
            stChnAttr.bShow = AR_TRUE;
            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32X = u32NewX;
            stChnAttr.unChnAttr.stOverlayChn.stPoint.s32Y = u32NewY;

            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }

            s32Ret = AR_MPI_RGN_GetCanvasInfo(handle, &stCanvasInfo);
            if(s32Ret)
            {
                PRINT_ERR("Get canvas info failed.\n");
                return -1;
            }

            //IPC_PF_OSD_DotMatrixToCanvas(&s_astAlgOsd[VpssGrp], &stCanvasInfo);
            IPC_PF_OSD_DotMatrixToBMP(&s_astAlgOsd[s32Pipe], &stCanvasInfo, pastStr[i].u32RGB);
            //4. update canvas
            s32Ret = AR_MPI_RGN_UpdateCanvas(handle);
            if(s32Ret)
            {
                PRINT_ERR("Update canvas failed.\n");
                return -1;
            }
        }
    }

    //clear other region
    for(i = u32StrNum; i < IPC_ALG_OSD_MAX_NUM; i++)
    {
        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
                continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);;
                stChn.s32ChnId = VpssChn;
            }

            handle = IPC_PF_GET_ALG_OSD_HANDLE(s32Pipe) + i; //eash osd is a region.

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }

            stChnAttr.bShow = AR_FALSE;
            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }
        }
    }

    return 0;
}

AR_S32 IPC_PF_OSD_DrawRect(VPSS_GRP VpssGrp, AR_U32 u32RectNum, IPC_OSD_RECT_S * pastRect, AR_U32 u32ImgW, AR_U32 u32ImgH)
{
    AR_S32 s32Ret;
    VPSS_CHN VpssChn = 0;
    RGN_HANDLE handle;
    RGN_CHN_ATTR_S stChnAttr = {0};
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    AR_FLOAT fWRatio = 0.0, fHRatio = 0.0;
    AR_U32 u32NewX, u32NewY, u32NewW, u32NewH;
    AR_U32 i = 0;
    AR_S32 s32Pipe = 0, s32Stream = 0;

    //draw rects
    s32Pipe = VpssGrp/2;
    for(i = 0; i < u32RectNum; i++)
    {
        handle = IPC_PF_GET_ALG_RECT_HANDLE(s32Pipe) + i;

        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
                continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);
                stChn.s32ChnId = VpssChn;
            }

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }

            fWRatio = (AR_FLOAT)u32ImgW / (AR_FLOAT)pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width;
            fHRatio = (AR_FLOAT)u32ImgH / (AR_FLOAT)pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height;
            u32NewX = (AR_FLOAT)pastRect[i].u32X / fWRatio;
            u32NewY = (AR_FLOAT)pastRect[i].u32Y / fHRatio;
            u32NewW = (AR_FLOAT)pastRect[i].u32W / fWRatio;
            u32NewH = (AR_FLOAT)pastRect[i].u32H / fHRatio;

            stChnAttr.bShow = AR_TRUE;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[0].s32X = u32NewX;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[0].s32Y = u32NewY;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[1].s32X = u32NewX + u32NewW;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[1].s32Y = u32NewY;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[2].s32X = u32NewX + u32NewW;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[2].s32Y = u32NewY + u32NewH;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[3].s32X = u32NewX;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[3].s32Y = u32NewY + u32NewH;
            stChnAttr.unChnAttr.stCoverChn.u32Color = 0xFF000000 | pastRect[i].u32RGB;

            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }
        }

    }

    //clear other rect
    for(i = u32RectNum; i < IPC_ALG_OSD_MAX_NUM; i++)
    {
        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
                continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);
                stChn.s32ChnId = VpssChn;
            }

            handle = IPC_PF_GET_ALG_RECT_HANDLE(s32Pipe) + i;

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }
            stChnAttr.bShow = AR_FALSE;

            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed: %x.\n", stChn.s32ChnId, s32Ret);
                return -1;
            }
        }
    }

    return 0;
}


AR_S32 IPC_PF_OSD_DrawGeomRect(VPSS_GRP VpssGrp, AR_U32 u32GeomNum, IPC_OSD_GEOM_S * pastGeom, AR_U32 u32ImgW, AR_U32 u32ImgH)
{
    AR_S32 s32Ret;
    VPSS_CHN VpssChn = 0;
    RGN_HANDLE handle;
    RGN_CHN_ATTR_S stChnAttr = {0};
    MPP_CHN_S stChn = {0};
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();
    IPC_CFG_VPSS_S * pVpssCfg = IPC_CFG_VPSS_GetParam();
    AR_FLOAT fWRatio = 0.0, fHRatio = 0.0;
    AR_U32 u32NewX0, u32NewY0, u32NewX1, u32NewY1;
    AR_U32 i = 0;
    AR_S32 s32Width = 0, s32Height = 0;
    AR_S32 s32Pipe = 0, s32Stream = 0;

    s32Pipe = VpssGrp/2;
    //draw rects
    for(i = 0; i < u32GeomNum; i++)
    {
        handle = IPC_PF_GET_ALG_RECT_HANDLE(s32Pipe) + i;

        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
              continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);//VpssGrp;
                stChn.s32ChnId = VpssChn;//VpssChn;
            }

            s32Width = pVpssCfg->stGrpCfg[stChn.s32DevId].stChnCfg[stChn.s32ChnId].u32Width;
            s32Height = pVpssCfg->stGrpCfg[stChn.s32DevId].stChnCfg[stChn.s32ChnId].u32Height;
            if(!(s32Width > 0 && s32Height > 0)){
                continue;
            }

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }

            #define ROUND(v,m) ((v)>(m)?(m-2):((v)<(0)?(1):(v)))

            fWRatio = (AR_FLOAT)u32ImgW / s32Width;
            fHRatio = (AR_FLOAT)u32ImgH / s32Height;
            u32NewX0 = (AR_FLOAT)pastGeom[i].u32X0 / fWRatio;
            u32NewX0 = ROUND(u32NewX0, s32Width);
            u32NewY0 = (AR_FLOAT)pastGeom[i].u32Y0 / fHRatio;
            u32NewY0 = ROUND(u32NewY0, s32Height);
            u32NewX1 = (AR_FLOAT)pastGeom[i].u32X1 / fWRatio;
            u32NewX1 = ROUND(u32NewX1, s32Width);
            u32NewY1 = (AR_FLOAT)pastGeom[i].u32Y1 / fHRatio;
            u32NewY1 = ROUND(u32NewY1, s32Height);

            // printf("Geom Orign [%d,%d] [%d,%d] Display [%d,%d] [%d,%d]\n",
            //     pastGeom[i].u32X0,pastGeom[i].u32Y0,pastGeom[i].u32X1,pastGeom[i].u32Y1,u32NewX0,u32NewY0,u32NewX1,u32NewY1);
            // printf("Group %d, Chan %d, Display width %d, height %d, Ration %f,%f\n",VpssGrp,VpssChn,
            //     pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Width,pVpssCfg->stGrpCfg[VpssGrp].stChnCfg[VpssChn].u32Height,fWRatio,fHRatio);

            stChnAttr.bShow = AR_TRUE;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[0].s32X = u32NewX0;//u32NewX0-1;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[0].s32Y = u32NewY0;//u32NewY0-1;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[1].s32X = u32NewX0+1;//u32NewX0+2;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[1].s32Y = u32NewY0+1;//u32NewY0-1;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[2].s32X = u32NewX1+1;//u32NewX1+2;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[2].s32Y = u32NewY1+1;//u32NewY1+1;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[3].s32X = u32NewX1;//u32NewX1-1;
            stChnAttr.unChnAttr.stCoverChn.stQuadRangle.stPoint[3].s32Y = u32NewY1;//u32NewY1+1;
            stChnAttr.unChnAttr.stCoverChn.u32Color = 0xFF000000 | pastGeom[i].u32RGB;

            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }
        }

    }

    //clear other rect
    for(i = u32GeomNum; i < IPC_ALG_OSD_MAX_NUM; i++)
    {
        for(s32Stream = 0; s32Stream < IPC_MAX_STREAM_PER_PIPE; s32Stream++)
        {
            if(!pOsdCfg->osdInfo[s32Pipe][s32Stream].u32AlgOsdEnable)
            {
                continue;
            }

            if((VpssGrp%2 && !s32Stream) || (!(VpssGrp%2) && s32Stream))
            {
              continue;
            }

            VpssChn = IPC_OSD_GetVpssChn(s32Pipe, s32Stream);

            //set x,y
            if(s32Stream < IPC_MAX_VENC_STREAM_PER_CHN && strcmp(pOsdCfg->osdInfo[s32Pipe][s32Stream].achAttachedTo, "venc") == 0)
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VENC);
                stChn.s32DevId = 0;
                stChn.s32ChnId = IPC_GET_VENC_CHN_ID(s32Pipe, s32Stream);
            }
            else
            {
                stChn.enModId = IPC_PF_MOD_TRANS(MOD_ID_VPSS);
                stChn.s32DevId = IPC_OSD_GetVpssGrp(s32Pipe, s32Stream);//VpssGrp;
                stChn.s32ChnId = VpssChn;
            }

            handle = IPC_PF_GET_ALG_RECT_HANDLE(s32Pipe) + i;

            s32Ret = AR_MPI_RGN_GetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Get chn %d display attr failed.\n", stChn.s32ChnId);
                return -1;
            }
            stChnAttr.bShow = AR_FALSE;

            s32Ret = AR_MPI_RGN_SetDisplayAttr(handle, &stChn, &stChnAttr);
            if(s32Ret)
            {
                PRINT_ERR("Set chn %d display attr failed: %x.\n", stChn.s32ChnId, s32Ret);
                return -1;
            }
        }
    }

    return 0;
}


AR_S32 IPC_PF_SetStreamOsd(AR_U32 u32ChannelId,
		AR_U32 u32StreamId,
		AR_U32 u32ChnNameEnable,
        AR_CHAR * strChnName,
		AR_U32 u32DateTimeEnable,
		AR_U32 u32DateFormat,
		AR_U32 u32TimeFormat,
		AR_U32 u32DisplayWeek,
		AR_U32 u32RGB,
		AR_U32 u32FontSize)
{
    IPC_CFG_OSD_S * pOsdCfg = IPC_CFG_OSD_GetParam();

    if(u32ChannelId > IPC_MAX_PIPELINE_NUM || u32StreamId > IPC_MAX_VENC_STREAM_PER_CHN)
    {
        PRINT_ERR("Invalid ch/stream: %d/%d\n", u32ChannelId, u32StreamId);
        return -1;
    }

    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdChannelName.enable = u32ChnNameEnable;
    if(strlen(strChnName))
    {
        strcpy((AR_CHAR *)(pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdChannelName.text), strChnName);
    }

    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.enable = u32DateTimeEnable;
    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.dateFormat = u32DateFormat;
    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.timeFmt =  u32TimeFormat;
    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.displayWeek =  u32DisplayWeek;
    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].u32RGB = u32RGB;
    pOsdCfg->osdInfo[u32ChannelId][u32StreamId].enFontSize = u32FontSize;

    printf("SetStreamOsd param(%d,%s,%d,%d,%d,%d)\n",
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdChannelName.enable,
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdChannelName.text,
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.enable,
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.dateFormat,
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.timeFmt,
        pOsdCfg->osdInfo[u32ChannelId][u32StreamId].osdDatetime.displayWeek
        );


    IPC_CFG_OSD_Save();
    return 0;
}

