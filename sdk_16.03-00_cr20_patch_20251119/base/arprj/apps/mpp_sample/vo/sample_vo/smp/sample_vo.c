#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/select.h>
#include <sys/time.h>

#include "sample_comm.h"
#include "ar_mipi_tx.h"
#include "../sample_vo.h"
#include "sample_vo_config.h"
#include "hal_gpio.h"
#include "hal_vo.h"
#include "osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

#define RES_PATH "/usrdata/"

#define YUV_320_240 RES_PATH "320_240_420.yuv"
// #define YUV_1920_1080 RES_PATH"1080p.yuv"
#define YUV_1920_1080 RES_PATH "test_rc.yuv"
#define YUV_720_1440 RES_PATH "test_vo.yuv"
#define SAMPLE_VO_LAYER_FPS 30

    AR_BOOL is_running = AR_TRUE;

    VB_POOL Pool = -1;

    typedef struct stSAMPLE_VOU_ThreadCtrl_Info
    {
        AR_CHAR filename[1024];
        AR_U32 u32Width;
        AR_U32 u32Height;
        PIXEL_FORMAT_E enPixelFmt;
        VIDEO_FORMAT_E enVideoFmt;
        AR_BOOL bQuit;
        AR_BOOL bDestroy;
        AR_S32 s32ToDev;
        DYNAMIC_RANGE_E enSrcDynamicRange;
        AR_BOOL abToChn[VO_MAX_CHN_NUM];
        COLOR_GAMUT_E enColrGamut;
        AR_U32 u32ChnNum;
        float u32Fps;
        AR_BOOL bVoEnable;

        pthread_t tid;
    } SAMPLE_VOU_ThreadCtrl_Info;

    typedef struct stSAMPLE_USER_VO_CONFIG_S
    {
        VO_SYNC_INFO_S stSyncInfo;
        VO_USER_INTFSYNC_ATTR_S stUserIntfSyncAttr;
        AR_U32 u32PreDiv;
        AR_U32 u32DevDiv;
        AR_U32 u32Framerate;
        combo_dev_cfg_t stcombo_dev_cfgl;
    } SAMPLE_USER_VO_CONFIG_S;

#define ALIGN_BACK(x, a) ((a) * (((x + a - 1) / (a))))
#define ALIGN_UP(x, a) ((((x) + ((a) - 1)) / a) * a)

#define SAMPLE_CHECK_RET(express, name)                                                          \
    do                                                                                           \
    {                                                                                            \
        AR_S32 Ret;                                                                              \
        Ret = express;                                                                           \
        if (Ret != AR_SUCCESS)                                                                   \
        {                                                                                        \
            printf("%s failed at %s : LINE: %d with %#x!\n", name, __FUNCTION__, __LINE__, Ret); \
            SAMPLE_VOU_SYS_Exit();                                                               \
            return Ret;                                                                          \
        }                                                                                        \
    } while (0)

#define VO_VB_PIC_BLK_SIZE(Width, Height, Type, size)                         \
    do                                                                        \
    {                                                                         \
        unsigned int u32AlignWidth;                                           \
        unsigned int u32AlignHeight;                                          \
        unsigned int u32HeadSize;                                             \
        u32AlignWidth = ALIGN_UP(Width, 16);                                  \
        u32AlignHeight = ALIGN_UP(Height, 2);                                 \
        u32HeadSize = 16 * u32AlignHeight; /* compress header stride 16 */    \
        if (Type == PIXEL_FORMAT_YVU_SEMIPLANAR_422)                          \
        {                                                                     \
            size = (u32AlignWidth * u32AlignHeight + u32HeadSize) * 2;        \
        }                                                                     \
        \ else if (Type == PIXEL_FORMAT_YUV_400)                              \
        {                                                                     \
            size = (u32AlignWidth * u32AlignHeight + u32HeadSize);            \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            size = ((u32AlignWidth * u32AlignHeight + u32HeadSize) * 3) >> 1; \
        }                                                                     \
    } while (0)

    AR_S32 SAMPLE_VOU_SYS_Init(void)
    {
        VB_CONFIG_S stVbConf = {0};
        AR_U32 u32BlkSize;

        AR_MPI_SYS_Exit();

        AR_MPI_VB_Exit();

        stVbConf.u32MaxPoolCnt = 64;

        u32BlkSize = COMMON_GetPicBufferSize(1920, 1080, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
        stVbConf.astCommPool[0].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[0].u32BlkCnt = 20;

        u32BlkSize = COMMON_GetPicBufferSize(720, 576, SAMPLE_PIXEL_FORMAT, DATA_BITWIDTH_8, COMPRESS_MODE_NONE, DEFAULT_ALIGN);
        stVbConf.astCommPool[1].u64BlkSize = u32BlkSize;
        stVbConf.astCommPool[1].u32BlkCnt = 20;

        SAMPLE_CHECK_RET(SAMPLE_COMM_SYS_Init(&stVbConf), "SAMPLE_COMM_SYS_Init");

        return AR_SUCCESS;
    }

    AR_VOID SAMPLE_VOU_SYS_Exit(void)
    {
        AR_MPI_SYS_Exit();
        AR_MPI_VB_Exit();
    }

    static AR_BOOL gVoEnable = true;
    AR_S32 SAMPLE_VO_Handel_Control_Cmd(SAMPLE_VO_CONFIG_CMD_E cmd, SAMPLE_VO_CONTROL_S *params)
    {
        printf("handle cmd: %d\n", cmd);
        switch (cmd)
        {
        case CMD_VO_CONFIG_SWITCH:
            gVoEnable = params->bVoEnable;
            printf("cmd value : %d\n", gVoEnable);
            break;
        default:
            break;
        }

        return AR_SUCCESS;
    }

    void SAMPLE_VO_HandleSig(AR_S32 signo)
    {
        signal(SIGINT, SIG_IGN);
        signal(SIGTERM, SIG_IGN);

        if (SIGINT == signo || SIGTERM == signo)
        {
            is_running = AR_FALSE;
            SAMPLE_PRT("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        }
    }

    AR_S32 SAMPLE_VOU_ReadOneFrame(FILE *fp, AR_U8 *pY, AR_U8 *pU, AR_U8 *pV,
                                   AR_U32 width, AR_U32 height, AR_U32 stride, AR_U32 stride2,
                                   PIXEL_FORMAT_E enPixFrm)
    {
        AR_U8 *pDst;
        AR_U32 u32UVHeight;
        AR_U32 u32Row;
        AR_U32 total = 0;
        AR_U32 readlen = 0;

        if (enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
        {
            u32UVHeight = height;
        }
        else
        {
            u32UVHeight = height / 2;
        }

        pDst = pY;
        for (u32Row = 0; u32Row < height; u32Row++)
        {
            if ((readlen = fread(pDst, 1, width, fp)) != width)
            {
                //            printf("ret = -1, readlen = %d\n");
                return -1;
            }
            pDst += stride;
            total += width;
        }

        if (pU)
        {
            pDst = pU;
            for (u32Row = 0; u32Row < u32UVHeight; u32Row++)
            {
                if ((readlen = fread(pDst, 1, width / 2, fp)) != width / 2)
                {
                    //            printf("ret = -2, readlen = %d\n");
                    return -2;
                }
                pDst += stride2;
                total += (width / 2);
            }
        }

        if (pV)
        {
            pDst = pV;
            for (u32Row = 0; u32Row < u32UVHeight; u32Row++)
            {
                if ((readlen = fread(pDst, 1, width / 2, fp)) != width / 2)
                {
                    //            printf("ret = -3, readlen = %d\n");
                    return -3;
                }
                pDst += stride2;
                total += (width / 2);
            }
        }
        //   printf("*******************u32UVHeight: %d, width: %d, total: %u\n", u32UVHeight, width, total);
        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_PlanToSemi(AR_U8 *pY, AR_S32 yStride,
                                AR_U8 *pU, AR_S32 uStride,
                                AR_U8 *pV, AR_S32 vStride,
                                AR_S32 picWidth, AR_S32 picHeight, PIXEL_FORMAT_E enPixFrm)
    {
        AR_S32 i;
        AR_U8 *pTmpU, *ptu;
        AR_U8 *pTmpV, *ptv;

        AR_S32 s32HafW = uStride >> 1;
        AR_S32 s32HafH;

        if (enPixFrm == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
        {
            s32HafH = picHeight;
        }
        else
        {
            s32HafH = picHeight >> 1;
        }

        AR_S32 s32Size = s32HafW * s32HafH;

        pTmpU = malloc(s32Size);
        ptu = pTmpU;
        pTmpV = malloc(s32Size);
        ptv = pTmpV;

        memcpy(pTmpU, pU, s32Size);
        memcpy(pTmpV, pV, s32Size);

        for (i = 0; i < s32Size >> 1; i++)
        {
            *pU++ = *pTmpV++;
            *pU++ = *pTmpU++;
        }
        for (i = 0; i < s32Size >> 1; i++)
        {
            *pV++ = *pTmpV++;
            *pV++ = *pTmpU++;
        }

        free(ptu);
        free(ptv);

        return AR_SUCCESS;
    }

    AR_VOID *SAMPLE_VO_FileVO(AR_VOID *pData)
    {
        AR_S32 i;
        AR_S32 s32Ret;
        FILE *pfd;
        VB_BLK hBlkHdl;
        AR_U32 u32Size;
        AR_U32 u32SrcWidth;
        AR_U32 u32SrcHeight;
        VIDEO_FRAME_INFO_S stUserFrame = {{0}};
        VB_POOL_CONFIG_S stVbPoolCfg;
        AR_U32 u32LumaSize = 0;
        AR_U32 u32ChromaSize = 0;
        AR_BOOL bVoEnablePre = AR_TRUE;
        uint64_t tm = 0;
        uint64_t delay_ms = 0;
        uint64_t frame_rate_ms;

        SAMPLE_VOU_ThreadCtrl_Info *pInfo = (SAMPLE_VOU_ThreadCtrl_Info *)pData;
        VO_LAYER VoLayer = pInfo->s32ToDev;
        memset(&stUserFrame, 0x0, sizeof(VIDEO_FRAME_INFO_S));

        frame_rate_ms = 1000 / pInfo->u32Fps;

        u32SrcWidth = pInfo->u32Width;
        u32SrcHeight = pInfo->u32Height;

        pfd = fopen(pInfo->filename, "rb");
        if (pfd == AR_NULL)
        {
            printf("open file %s fail \n", pInfo->filename);
            return AR_NULL;
        }
        else
        {
            printf("open file %s success!\n", pInfo->filename);
        }

        fflush(stdout);

        u32Size = u32SrcWidth * u32SrcHeight * 3;
        memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));
        stVbPoolCfg.u64BlkSize = u32Size;
        printf("u32Size %d, %d, %d\n", u32Size, u32SrcWidth, u32SrcHeight);
        stVbPoolCfg.u32BlkCnt = 10;
        stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;
        Pool = AR_MPI_VB_CreatePool(&stVbPoolCfg);
        if (Pool == VB_INVALID_POOLID)
        {
            printf("Maybe you not call sys init\n");
            return AR_NULL;
        }

        stUserFrame.stVFrame.enField = VIDEO_FIELD_INTERLACED;
        stUserFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
        stUserFrame.stVFrame.enPixelFormat = pInfo->enPixelFmt;
        stUserFrame.stVFrame.enVideoFormat = pInfo->enVideoFmt;
        stUserFrame.stVFrame.enColorGamut = COLOR_GAMUT_BT709;
        stUserFrame.stVFrame.u32Width = u32SrcWidth;
        stUserFrame.stVFrame.u32Height = u32SrcHeight;
        stUserFrame.stVFrame.u32Stride[0] = ALIGN_BACK(u32SrcWidth, 32);
        if ((pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_420) || (pInfo->enPixelFmt == PIXEL_FORMAT_YUV_400))
        {
            stUserFrame.stVFrame.u32Stride[1] = ALIGN_BACK(u32SrcWidth, 32) / 2;
            stUserFrame.stVFrame.u32Stride[2] = ALIGN_BACK(u32SrcWidth, 32) / 2;
        }
        printf("u32Stride %d %d %d\r\n", stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1], stUserFrame.stVFrame.u32Stride[2]);
        stUserFrame.stVFrame.u32TimeRef = 0;
        stUserFrame.stVFrame.u64PTS = 0;
        stUserFrame.stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;

        u32LumaSize = stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight;
        if (pInfo->enPixelFmt == PIXEL_FORMAT_YVU_SEMIPLANAR_422)
        {
            u32ChromaSize = stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight / 2;
        }
        else if ((pInfo->enPixelFmt == PIXEL_FORMAT_YVU_SEMIPLANAR_420))
        {
            u32ChromaSize = stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight / 4;
        }
        else if ((pInfo->enPixelFmt == PIXEL_FORMAT_YUV_400))
        {
            u32ChromaSize = 0;
        }
        else if ((pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_420))
        {
            u32ChromaSize = stUserFrame.stVFrame.u32Stride[0] * u32SrcHeight / 4;
        }
        else if ((pInfo->enPixelFmt == PIXEL_FORMAT_U8C1))
        {
            u32ChromaSize = 0;
        }

        do
        {
            tm = ar_get_linux_time();

            if (bVoEnablePre != gVoEnable)
            {
                if (gVoEnable == AR_FALSE)
                {
                    for (i = 0; i < pInfo->u32ChnNum; i++)
                    {
                        SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
                    }
                    SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");
                    SAMPLE_CHECK_RET(AR_MPI_VO_Disable(0), "AR_MPI_VO_Disable");
                }
                else
                {
                    SAMPLE_CHECK_RET(AR_MPI_VO_Enable(0), "AR_MPI_VO_Enable");
                    SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");
                    for (i = 0; i < pInfo->u32ChnNum; i++)
                    {
                        SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
                    }
                }
                bVoEnablePre = gVoEnable;
            }

            // if display is closed
            if (gVoEnable == AR_FALSE)
            {
                ar_delay(2000 * 200);
                continue;
            }

            if (feof(pfd) != 0)
            {
                fseek(pfd, 0, SEEK_SET);
            }

            hBlkHdl = AR_MPI_VB_GetBlock(Pool, u32Size, NULL);
            if (hBlkHdl == VB_INVALID_HANDLE)
            {
                printf("[VOU_MST_File2VO] get vb fail!!!\n");
                sleep(1);
                continue;
            }

            stUserFrame.u32PoolId = AR_MPI_VB_Handle2PoolId(hBlkHdl);
            stUserFrame.stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(hBlkHdl);
            if (u32ChromaSize != 0)
            {
                stUserFrame.stVFrame.u64PhyAddr[1] = stUserFrame.stVFrame.u64PhyAddr[0] + u32LumaSize;
                stUserFrame.stVFrame.u64PhyAddr[2] = stUserFrame.stVFrame.u64PhyAddr[1] + u32ChromaSize;
            }

            stUserFrame.stVFrame.u64VirAddr[0] = (AR_U64)AR_MPI_SYS_Mmap(stUserFrame.stVFrame.u64PhyAddr[0], u32Size);
            {
                stUserFrame.stVFrame.u64VirAddr[1] = (AR_U64)(stUserFrame.stVFrame.u64VirAddr[0]) + u32LumaSize;
                stUserFrame.stVFrame.u64VirAddr[2] = (AR_U64)(stUserFrame.stVFrame.u64VirAddr[1]) + u32ChromaSize;
            }

            if ((pInfo->enPixelFmt == PIXEL_FORMAT_YVU_PLANAR_420) ||
                (pInfo->enPixelFmt == PIXEL_FORMAT_YUV_400) ||
                (pInfo->enPixelFmt == PIXEL_FORMAT_U8C1))
            {
                s32Ret = SAMPLE_VOU_ReadOneFrame(pfd, (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                                 (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                                 stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                                 stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                                 stUserFrame.stVFrame.enPixelFormat);
                if (s32Ret != AR_SUCCESS)
                {
                    goto OUT;
                }
            }
            else
            {
                s32Ret = SAMPLE_VOU_ReadOneFrame(pfd, (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0],
                                                 (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2],
                                                 stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                                 stUserFrame.stVFrame.u32Stride[0], stUserFrame.stVFrame.u32Stride[1],
                                                 stUserFrame.stVFrame.enPixelFormat);
                if (s32Ret == AR_SUCCESS)
                {
                    SAMPLE_VO_PlanToSemi((AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0], stUserFrame.stVFrame.u32Stride[0],
                                         (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[1], stUserFrame.stVFrame.u32Stride[1],
                                         (AR_U8 *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[2], stUserFrame.stVFrame.u32Stride[1],
                                         stUserFrame.stVFrame.u32Width, stUserFrame.stVFrame.u32Height,
                                         stUserFrame.stVFrame.enPixelFormat);
                }
                else
                {
                    goto OUT;
                }
            }

            stUserFrame.stVFrame.u64PTS += 40000;
            stUserFrame.stVFrame.u32TimeRef += 40000;
            stUserFrame.stVFrame.u32FrameId++;

            for (i = 0; i < pInfo->u32ChnNum; i++)
            {
                s32Ret = AR_MPI_VO_SendFrame(VoLayer, i, &stUserFrame, 0);
                if (s32Ret != AR_SUCCESS)
                {
                    goto OUT;
                }
            }

        OUT:
            AR_MPI_VB_ReleaseBlock(hBlkHdl);
            AR_MPI_SYS_Munmap((AR_VOID *)(AR_U64)stUserFrame.stVFrame.u64VirAddr[0], u32Size);
            delay_ms = ar_get_linux_time() - tm; /* impossible overflow*/
                                                 //      printf("spend %lu\n", delay_ms);
            delay_ms = (delay_ms < frame_rate_ms) ? (frame_rate_ms - delay_ms) : 1;
            //      printf("delay %lu\n", delay_ms);
            ar_delay(delay_ms * 1000);
            //          sleep(3);
        } while (pInfo->bQuit == AR_FALSE);

        while (pInfo->bDestroy == AR_FALSE)
        {
            ;
        }

        fclose(pfd);
        printf("SAMPLE_VO_FileVO end \n");
        return NULL;
    }

    AR_VOID SAMPLE_VO_Get_USER_CONFIG(SAMPLE_USER_VO_CONFIG_S *pstUserVoConfig, combo_dev_cfg_t *pstComboDevCfg)
    {
        /* USER SET SYNCINFO CONFIG */
        pstUserVoConfig->stSyncInfo.u16Vact = 1080;
        pstUserVoConfig->stSyncInfo.u16Vbb = 28;
        pstUserVoConfig->stSyncInfo.u16Vfb = 130;
        pstUserVoConfig->stSyncInfo.u16Vpw = 10;
        pstUserVoConfig->stSyncInfo.u16Hact = 1920;
        pstUserVoConfig->stSyncInfo.u16Hbb = 36;
        pstUserVoConfig->stSyncInfo.u16Hfb = 16;
        pstUserVoConfig->stSyncInfo.u16Hpw = 8;

        /* USER SET FRAME TARE */
        pstUserVoConfig->u32Framerate = 60;

        /* USER SET Div INFOMATION */
        pstUserVoConfig->u32PreDiv = 1;
        pstUserVoConfig->u32DevDiv = 1;

        /* USER SET INTFSYNC ATTR */
        pstUserVoConfig->stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
        pstUserVoConfig->stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
        pstUserVoConfig->stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x7aebc4;
        pstUserVoConfig->stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
        pstUserVoConfig->stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
        pstUserVoConfig->stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

        /* USER SET MIPI ATTR */
        pstComboDevCfg->devno = 0;
        pstComboDevCfg->lane_id[0] = 0;
        pstComboDevCfg->lane_id[1] = 1;
        pstComboDevCfg->lane_id[2] = 2;
        pstComboDevCfg->lane_id[3] = 3;
        pstComboDevCfg->output_format = OUTPUT_MODE_DSI_VIDEO;
        pstComboDevCfg->output_mode = OUT_FORMAT_RGB_24_BIT;
        //---xy
        // pstComboDevCfg->video_mode = BURST_MODE;
        pstComboDevCfg->video_mode = BRUST_MODE;
        //---xy
        pstComboDevCfg->phy_data_rate = 879;
        pstComboDevCfg->pixel_clk = 146481;
        pstComboDevCfg->sync_info.vid_pkt_size = 1920;
        pstComboDevCfg->sync_info.vid_hbp_pixels = 28;
        pstComboDevCfg->sync_info.vid_hsa_pixels = 8;
        pstComboDevCfg->sync_info.vid_hline_pixels = 1972;
        pstComboDevCfg->sync_info.vid_active_lines = 1080;
        pstComboDevCfg->sync_info.vid_vbp_lines = 18;
        pstComboDevCfg->sync_info.vid_vfp_lines = 130;
        pstComboDevCfg->sync_info.vid_vsa_lines = 10;

        return;
    }

    AR_VOID SAMPLE_VO_GetUserPubBaseAttr(VO_PUB_ATTR_S *pstPubAttr)
    {
        pstPubAttr->u32BgColor = COLOR_RGB_BLUE;
        pstPubAttr->enIntfSync = VO_OUTPUT_USER;
        pstPubAttr->stSyncInfo.bSynm = 0;
        pstPubAttr->stSyncInfo.u8Intfb = 0;
        pstPubAttr->stSyncInfo.bIop = 1;

        pstPubAttr->stSyncInfo.u16Hmid = 1;
        pstPubAttr->stSyncInfo.u16Bvact = 1;
        pstPubAttr->stSyncInfo.u16Bvbb = 1;
        pstPubAttr->stSyncInfo.u16Bvfb = 1;

        pstPubAttr->stSyncInfo.bIdv = 0;
        pstPubAttr->stSyncInfo.bIhs = 0;
        pstPubAttr->stSyncInfo.bIvs = 0;

        return;
    }

    AR_VOID SAMPLE_VO_GetUserLayerAttr(VO_VIDEO_LAYER_ATTR_S *pstLayerAttr, SIZE_S *pstDevSize)
    {
        pstLayerAttr->bClusterMode = AR_FALSE;
        pstLayerAttr->bDoubleFrame = AR_FALSE;
        pstLayerAttr->enDstDynamicRange = DYNAMIC_RANGE_SDR8;
        pstLayerAttr->enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;

        pstLayerAttr->stDispRect.s32X = 0;
        pstLayerAttr->stDispRect.s32Y = 0;
        pstLayerAttr->stDispRect.u32Height = pstDevSize->u32Height;
        pstLayerAttr->stDispRect.u32Width = pstDevSize->u32Width;

        pstLayerAttr->stImageSize.u32Height = pstDevSize->u32Height;
        pstLayerAttr->stImageSize.u32Width = pstDevSize->u32Width;
        printf("x%d y%d h%d w%d imgH%d imgW%d \n", pstLayerAttr->stDispRect.s32X, pstLayerAttr->stDispRect.s32Y,
               pstLayerAttr->stDispRect.u32Height, pstLayerAttr->stDispRect.u32Width,
               pstLayerAttr->stImageSize.u32Height, pstLayerAttr->stImageSize.u32Width);
        return;
    }

    AR_VOID SAMPLE_VO_GetUserPipChnAttr(VO_CHN_ATTR_S *pstChnAttr, SIZE_S *pstDevSize)
    {
        AR_U32 u32Height = pstDevSize->u32Height;
        AR_U32 u32Width = pstDevSize->u32Width;

        pstChnAttr[0].bDeflicker = AR_FALSE;
        pstChnAttr[0].u32Priority = 0;
        pstChnAttr[0].stRect.s32X = 0;
        pstChnAttr[0].stRect.s32Y = 0;
        pstChnAttr[0].stRect.u32Height = u32Height;
        pstChnAttr[0].stRect.u32Width = u32Width;

        pstChnAttr[1].bDeflicker = AR_FALSE;
        pstChnAttr[1].u32Priority = 1;
        pstChnAttr[1].stRect.s32X = 600;
        pstChnAttr[1].stRect.s32Y = 300;
        pstChnAttr[1].stRect.u32Height = 480;
        pstChnAttr[1].stRect.u32Width = 640;
    }

    AR_VOID SAMPLE_VO_GetUserChnAttr(VO_CHN_ATTR_S *pstChnAttr, SIZE_S *pstDevSize, AR_S32 VoChnNum)
    {
        AR_S32 i;
        AR_U32 u32Height = pstDevSize->u32Height / VoChnNum;
        AR_U32 u32Width = pstDevSize->u32Width / VoChnNum;
        AR_U32 u32Square;

        if (VoChnNum == 1)
            u32Square = 1;
        else if (VoChnNum <= 4)
            u32Square = 2;
        else if (VoChnNum <= 9)
            u32Square = 3;
        else
            u32Square = 1;

        u32Height = pstDevSize->u32Height / u32Square;
        u32Width = pstDevSize->u32Width / u32Square;

        for (i = 0; i < VoChnNum; i++)
        {
            pstChnAttr[i].bDeflicker = AR_FALSE;
            pstChnAttr[i].u32Priority = 0;
            pstChnAttr[i].stRect.s32X = (i % u32Square) * u32Width;
            pstChnAttr[i].stRect.s32Y = (i / u32Square) * u32Height;
            pstChnAttr[i].stRect.u32Height = u32Height;
            pstChnAttr[i].stRect.u32Width = u32Width;
        }

        return;
    }

    AR_VOID SAMPLE_VO_GetBaseThreadInfo(SAMPLE_VOU_ThreadCtrl_Info *pstThreadInfo, SIZE_S *pstFrmSize)
    {
        pstThreadInfo->bDestroy = AR_FALSE;
        pstThreadInfo->bVoEnable = AR_TRUE;
        pstThreadInfo->bQuit = AR_FALSE;
        pstThreadInfo->enColrGamut = COLOR_GAMUT_BT709;
        pstThreadInfo->enPixelFmt = PIXEL_FORMAT_YVU_PLANAR_420;
        pstThreadInfo->enVideoFmt = VIDEO_FORMAT_LINEAR;
        pstThreadInfo->u32Width = pstFrmSize->u32Width;
        pstThreadInfo->u32Height = pstFrmSize->u32Height;

        return;
    }

    AR_VOID SAMPLE_VO_StartUserCtrl(SAMPLE_VOU_ThreadCtrl_Info *pInfo, AR_S32 VoLayer, AR_S32 VoChnNum)
    {
        fd_set read_fds;
        AR_S32 s32Ret;
        struct timeval TimeoutVal;
        const AR_S32 stdinFd = 0;
        char c = 0;

        while (pInfo->bQuit == AR_FALSE)
        {

            FD_ZERO(&read_fds);
            FD_SET(stdinFd, &read_fds);

            TimeoutVal.tv_sec = 2;
            TimeoutVal.tv_usec = 0; // 500 * 1000;
            s32Ret = select(stdinFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
            if (s32Ret < 0)
            {
                printf("select stdin failed\n");
                break;
            }
            else if (s32Ret == 0)
            {
                continue;
            }
            read(0, (void *)&c, 1);
            if (c == 'o')
            {
                printf("open vo \n");
                pInfo->bVoEnable = AR_TRUE;
            }
            else if (c == 'c')
            {
                printf("close vo \n");
                pInfo->bVoEnable = AR_FALSE;
            }
            // printf("read return %d, c=%c", s32Ret, c);
        }
    }

    AR_VOID SAMPLE_VO_StartUserThd(SAMPLE_VOU_ThreadCtrl_Info *pstThreadInfo, AR_S32 VoLayer, AR_S32 VoChnNum,
                                   AR_CHAR filename[256], SIZE_S *pstFrmSize, AR_U32 u32Fps, PIXEL_FORMAT_E eFmt)
    {
        /* CREATE USER THREAD */
        SAMPLE_VO_GetBaseThreadInfo(pstThreadInfo, pstFrmSize);

        pstThreadInfo->s32ToDev = VoLayer;
        pstThreadInfo->u32ChnNum = VoChnNum;
        pstThreadInfo->u32Fps = u32Fps;
        pstThreadInfo->enPixelFmt = eFmt;

        strncpy(pstThreadInfo->filename, filename, sizeof(pstThreadInfo->filename) - 1);
        pthread_create(&pstThreadInfo->tid, NULL, SAMPLE_VO_FileVO, (AR_VOID *)pstThreadInfo);

        printf("SAMPLE_VO_StartUserThd end \n");
        return;
    }

    AR_VOID SAMPLE_VO_StopUserThd(SAMPLE_VOU_ThreadCtrl_Info *pstThdInfo)
    {
        pstThdInfo->bQuit = AR_TRUE;
        pstThdInfo->bDestroy = AR_TRUE;
        pthread_join(pstThdInfo->tid, AR_NULL);

        return;
    }

    static VO_DSI_ATTR_S pstDsiCfg =
        {
            .u32Lane = {1, 1, 1, 1},
            .u32BitsPerPixel = 24,
            .stSyncInfo = {
                .u32Hsa = 72,
                .u32Hbp = 162,
                .u32Hact = 1024,
                .u32Hfp = 162,

                .u32Vsa = 10,
                .u32Vbp = 23,
                .u32Vact = 600,
                .u32Vfp = 12},
            .fDphyClkMhz = 360.3744,
#if defined(AR9341)
            .u32PllFreqReg2c0 = 0x2c65f509,
            .u32PllFreqReg38c = 0x2800
#endif
    };

    int SAMPLE_VO_SysCall(const char *cmd, char *buff, int bufsize)
    {
        FILE *fp = NULL;
        // printf("==> %s\n", cmd);

        fp = popen(cmd, "r");
        if (fp == NULL)
        {
            printf("popen error\n");
            return -1;
        }

        int ret = 0;
        if ((buff != NULL) && (bufsize > 0))
        {
            ret = fread(buff, 1, bufsize, fp);
        }

        if (ret >= 0)
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }

        pclose(fp);

        return ret;
    }

    int SAMPLE_VO_GPIO_SetValue(int pin, int value)
    {
        char dir_tmp[128] = {0};
        char cmd[128] = {0};
        int ret = 0;

        sprintf(dir_tmp, "/sys/class/gpio/gpio%d", pin);
        if (access(dir_tmp, F_OK) != 0)
        {
            snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export", pin);
            SAMPLE_VO_SysCall(cmd, NULL, 0);
        }

        snprintf(cmd, sizeof(cmd), "cat /sys/class/gpio/gpio%d/direction", pin);
        char buff[30] = {0};
        ret = SAMPLE_VO_SysCall(cmd, buff, sizeof(buff));
        if ((ret != 0) || (strlen(buff) == 0))
        {
            printf("%s failed\n", cmd);
            return -1;
        }
        if (strncmp(buff, "in", 2) == 0)
        {
            snprintf(cmd, sizeof(cmd), "echo out > /sys/class/gpio/gpio%d/direction", pin);
            // printf("1 cmd:%s, buff:%s", cmd, buff);
            SAMPLE_VO_SysCall(cmd, NULL, 0);
        }
        else
        {
            // printf("1 buff:%s", buff);
        }

        snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value", value, pin);
        // printf("1 cmd:%s\n", cmd);
        SAMPLE_VO_SysCall(cmd, NULL, 0);

        return 0;
    }

    void SAMPLE_VO_7INCH_LCD_Init(void)
    {
#if defined(AR9341)
        int backlight = 40;
        int power_gpio = 42;
        int standby_gpio = 38;
        int reset_gpio = 41;
        int updown_gpio = 63;
#endif

#if defined(PROXIMA)
        int backlight = 58;
        int power_gpio = 57;
        int standby_gpio = 53;
        int reset_gpio = 56;
        int updown_gpio = 54;
#endif
        printf("7inch lcd init\n");

        SAMPLE_VO_GPIO_SetValue(power_gpio, 1);
        SAMPLE_VO_GPIO_SetValue(backlight, 1);

        // standby the lcd
        SAMPLE_VO_GPIO_SetValue(updown_gpio, 0);
        SAMPLE_VO_GPIO_SetValue(standby_gpio, 1);

        ar_delay(10000);

        SAMPLE_VO_GPIO_SetValue(reset_gpio, 1);
        ar_delay(1000);
        SAMPLE_VO_GPIO_SetValue(reset_gpio, 0);
        ar_delay(10000);
        SAMPLE_VO_GPIO_SetValue(reset_gpio, 1);

        // final we backlight the lcd
        SAMPLE_VO_GPIO_SetValue(backlight, 1);
        return;
    }

    AR_S32 SAMPLE_VO_REGION_Init(RGN_TYPE_E enRgnType, AR_U32 u32Num)
    {
        MPP_CHN_S stChn = {AR_ID_VO, 0, 0};

        switch (enRgnType)
        {
        case COVER_RGN:
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_Create(u32Num, COVER_RGN), "SAMPLE_COMM_REGION_Create");
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_AttachToChn(u32Num, COVER_RGN, &stChn), "SAMPLE_REGION_AttachToChn");
            break;

        case MOSAIC_RGN:
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_Create(u32Num, MOSAIC_RGN), "SAMPLE_COMM_REGION_Create");
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_AttachToChn(u32Num, MOSAIC_RGN, &stChn), "SAMPLE_REGION_AttachToChn");
            break;

        case OVERLAY_RGN:
            break;

        default:
            break;
        }

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_REGION_DeInit(RGN_TYPE_E enRgnType, AR_U32 u32Num)
    {
        MPP_CHN_S stChn = {AR_ID_VO, 0, 0};

        switch (enRgnType)
        {
        case COVER_RGN:
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_DetachFrmChn(u32Num, COVER_RGN, &stChn), "SAMPLE_COMM_REGION_DetachFrmChn");
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_Destroy(u32Num, COVER_RGN), "SAMPLE_COMM_REGION_Destroy");
            break;

        case MOSAIC_RGN:
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_DetachFrmChn(u32Num, MOSAIC_RGN, &stChn), "SAMPLE_COMM_REGION_DetachFrmChn");
            SAMPLE_CHECK_RET(SAMPLE_COMM_REGION_Destroy(u32Num, MOSAIC_RGN), "SAMPLE_COMM_REGION_Destroy");
            break;

        case OVERLAY_RGN:
            break;

        default:
            break;
        }

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_MIPILCD_1024_600(AR_BOOL MutiDev)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
        VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
        AR_U32 u32Framerate;
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo = {0};

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        /* USER SET VO FRAME RATE */
        u32Framerate = 60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

        /* USER SET VO DEV SYNC INFO */
        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        stPubAttr.stSyncInfo.bSynm = AR_TRUE;
        stPubAttr.stSyncInfo.bIop = AR_TRUE;
        stPubAttr.stSyncInfo.u8Intfb = 0;
        stPubAttr.stSyncInfo.u16Hpw = 96;
        stPubAttr.stSyncInfo.u16Hbb = 216;
        stPubAttr.stSyncInfo.u16Hact = 1024;
        stPubAttr.stSyncInfo.u16Hfb = 216;
        stPubAttr.stSyncInfo.u16Vpw = 10;
        stPubAttr.stSyncInfo.u16Vbb = 23;
        stPubAttr.stSyncInfo.u16Vact = 600;
        stPubAttr.stSyncInfo.u16Vfb = 12;
        stPubAttr.stSyncInfo.u16Hmid = 1;
        stPubAttr.stSyncInfo.u16Bvact = 1;
        stPubAttr.stSyncInfo.u16Bvbb = 1;
        stPubAttr.stSyncInfo.u16Bvfb = 1;
        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        stPubAttr.enIntfType = VO_INTF_MIPI;

        if (MutiDev)
            stPubAttr.enIntfType |= VO_INTF_LCD_24BIT;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* USER SET VO SYNC INFO OF USER INTF */
        stUserInfo.bClkReverse = AR_TRUE;
        stUserInfo.u32DevDiv = 1;
        stUserInfo.u32PreDiv = 1;
        stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /* SET LCD */
        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");
        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
        SAMPLE_VO_7INCH_LCD_Init();

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
        stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();

        return AR_SUCCESS;
    }

#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW 0

// LCD_VCC_EN GPIO_D1_5
#define lcd_power_port PORT_D
#define lcd_power_group GROUP_1
#define lcd_power_number 5

// LCD_RST GPIO_C1_4
#define lcd_rest_port PORT_C
#define lcd_rest_group GROUP_1
#define lcd_rest_number 4

// LCD_PWM GPIO_D3_4
#define lcd_backlight_port PORT_D
#define lcd_backlight_group GROUP_3
#define lcd_backlight_number 4

    static int backlight_lcd(int on)
    {
#ifdef BACKLIGHT_PWM
        int num = 0;
        int duty = 0;

        if (on)
        {
            duty = 100;
        }

        if (pwm_export(num) < 0)
        {
            printf("PWM export failed!");
            return -1;
        }

        if (pwm_enable_set(num, 0) < 0)
        {
            printf("mode_1 disable pwm failed!\n");
            return -1;
        }

        if (pwm_period_set(num, 1000) < 0) // 1M
        {
            printf("mode_1 set period failed!\n");
            return -1;
        }

        if (pwm_duty_cycle_percent_set(num, duty) < 0)
        {
            printf("mode_1 set duty cycle percent failed!\n");
            return -1;
        }
        if (pwm_enable_set(num, 1) < 0)
        {
            printf("mode_1 enable pwm failed!\n");
            return -1;
        }
#else
    int gpio_num = ar_hal_gpio_name_to_num(lcd_backlight_group, lcd_backlight_port, lcd_backlight_number);
    ar_hal_gpio_export(gpio_num);
    ar_hal_gpio_set_dir(gpio_num, OUTPUT);
    if (on)
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
    }
    else
    {
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
    }
#endif
        return 0;
    }

    static int power_lcd(int on)
    {
        int gpio_num = ar_hal_gpio_name_to_num(lcd_power_group, lcd_power_port, lcd_power_number);
        ar_hal_gpio_export(gpio_num);
        ar_hal_gpio_set_dir(gpio_num, OUTPUT);
        if (on)
        {
            ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
        }
        else
        {
            ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
        }

        return 0;
    }

    static int reset_lcd(int delay1, int delay2, int delay3)
    {
        int gpio_num = ar_hal_gpio_name_to_num(lcd_rest_group, lcd_rest_port, lcd_rest_number);
        ar_hal_gpio_export(gpio_num);
        ar_hal_gpio_set_dir(gpio_num, OUTPUT);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
        ar_delay(delay1);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
        ar_delay(delay2);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
        ar_delay(delay3);
        return 0;
    }

    static AR_S32 power_on_st7703(void)
    {
        power_lcd(0);
        ar_delay(20 * 1000); // sleep 20
        power_lcd(1);
        backlight_lcd(0);

        // reset_lcd(15,3,130);

        return 0;
    }

    static int SAMPLE_VO_dsi_short_cmd(VO_DEV voDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms)
    {
        VO_DSI_CMD_S pstCmd;

        pstCmd.u16DataType = data_type;
        pstCmd.u16CmdSize = param2 << 8 | param1;
        pstCmd.pu8Cmd = NULL;
        AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
        ar_delay(delayms);
        return 0;
    }

    static int SAMPLE_VO_dsi_long_cmd(VO_DEV voDev, uint8_t data_type, uint8_t *data, uint16_t size, int delayms)
    {
        VO_DSI_CMD_S pstCmd;

        pstCmd.u16DataType = data_type;
        pstCmd.u16CmdSize = size;
        pstCmd.pu8Cmd = data;
        AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
        ar_delay(delayms);
        return 0;
    }

    static AR_S32 init_mipi_port_st7703(VO_DEV voDev)
    {
        int delayms = 10;

        printf("%s: line:%d init mipi port:  LCD_ST7703\n", __FILE__, __LINE__);

        uint8_t para0[] = {0xB9, 0xF1, 0x12, 0x83};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para0, sizeof(para0), delayms);

        uint8_t para1[] = {0xBA, 0x33, 0x81, 0x05, 0xF9, 0x0e, 0x0e,
                           0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x44, 0x25, 0x00, 0x91, 0x0a, 0x00, 0x00, 0x02,
                           0x4F, 0x11, 0x00, 0x00, 0x37};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para1, sizeof(para1), delayms);

        uint8_t para2[] = {0xB8, 0x25, 0x22, 0x20, 0x03};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para2, sizeof(para2), delayms);

        uint8_t para3[] = {0xB3, 0x10, 0x10, 0x05, 0x05, 0x03, 0xFF,
                           0x00, 0x00, 0x00, 0x00};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para3, sizeof(para3), delayms);

        uint8_t para4[] = {0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00,
                           0x08, 0x70, 0x00};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para4, sizeof(para4), delayms);

        SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xBC, 0x4E, delayms);
        SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xCC, 0x0B, delayms);
        SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA, 0xB4, 0x80, delayms);

        uint8_t para8[] = {0xB2, 0xF0, 0x12, 0x30};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para8, sizeof(para8), delayms);

        uint8_t para9[] = {0xE3, 0x07, 0x07, 0x0B, 0x0B, 0x03, 0x0B,
                           0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xC0, 0x10};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para9, sizeof(para9), delayms);

        uint8_t para10[] = {0xC1, 0x54, 0x00, 0x1E, 0x1E, 0x77, 0xF1,
                            0xFF, 0xFF, 0xCC, 0xCC, 0x77, 0x77};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para10, sizeof(para10), delayms);

        uint8_t para11[] = {0xB5, 0x0D, 0x0D};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para11, sizeof(para11), delayms);

        uint8_t para12[] = {0xB6, 0x43, 0x43};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para12, sizeof(para12), delayms);

        uint8_t para13[] = {0xBF, 0x02, 0x11, 0x00};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para13, sizeof(para13), delayms);

        uint8_t para14[] = {0xE9, 0x82, 0x10, 0x06, 0x05, 0x9E, 0x0A,
                            0xA5, 0x12, 0x31, 0x23, 0x37, 0x83, 0x04, 0xBC,
                            0x27, 0x38, 0x0C, 0x00, 0x03, 0x00, 0x00, 0x00,
                            0x0C, 0x00, 0x03, 0x00, 0x00, 0x00, 0x75, 0x75,
                            0x31, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x13,
                            0x88, 0x64, 0x64, 0x20, 0x88, 0x88, 0x88, 0x88,
                            0x88, 0x88, 0x02, 0x88, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para14, sizeof(para14), delayms);

        uint8_t para15[] = {0xEA, 0x02, 0x21, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x46,
                            0x02, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x64,
                            0x88, 0x13, 0x57, 0x13, 0x88, 0x88, 0x88, 0x88,
                            0x88, 0x88, 0x75, 0x88, 0x23, 0x04, 0x00, 0x00,
                            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x30, 0x0A, 0xA5, 0x00, 0x00, 0x00, 0x00};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para15, sizeof(para15), delayms);

        uint8_t para16[] = {0xE0, 0x00, 0x0F, 0x1A, 0x2D, 0x32, 0x3C,
                            0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13, 0x11,
                            0x13, 0x12, 0x19, 0x00, 0x0F, 0x1A, 0x2D, 0x32,
                            0x3C, 0x51, 0x40, 0x08, 0x0E, 0x0E, 0x11, 0x13,
                            0x11, 0x13, 0x12, 0x19};
        SAMPLE_VO_dsi_long_cmd(voDev, MIPI_DATA_TYPE_GENERIC_LONG_WRITE, para16, sizeof(para16), delayms);

        delayms = 250;
        SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x11, 0x00, delayms); // Sleep Out
        delayms = 50;
        SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x29, 0x00, delayms); // Display On
        // delayms = 10;
        // SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x13, 0x00, delayms); //Display normal
        // delayms = 10;
        // SAMPLE_VO_dsi_short_cmd(voDev, MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA, 0x23, 0x00, delayms); //all pix

        printf("%s: line:%d init mipi port: LCD_ST7703 end!\n", __FILE__, __LINE__);
        return;
    }

    AR_S32 init_display_st7703(VO_DEV VoDev)
    {
        // read display id
        // uint8_t id[3]={0};

        // VO_DSI_READ_S pstRead;
        // pstRead.u16DataType = MIPI_DATA_TYPE_GENERIC_SHORT_READ_1_PARA;
        // pstRead.u16DataSize = 3;
        // pstRead.u32Para = 0x04;
        // pstRead.pu8Data = id;
        // AR_MPI_VO_Dsi_Read(VoDev, &pstRead);
        // printf("read display id %x %x %x\n", id[0], id[1], id[2]);

#ifdef SHOW_PATTERN
        VO_DSI_CMD_S pstCmd;
        uint8_t para0[] = {0xFF, 0x98, 0x81, 0x04};
        pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_LONG_WRITE;
        pstCmd.u16CmdSize = sizeof(para0);
        pstCmd.pu8Cmd = para0;
        AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);

        pstCmd.u16DataType = MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA;
        pstCmd.u16CmdSize = 0x012F;
        pstCmd.pu8Cmd = NULL;
        AR_MPI_VO_Dsi_Cmd(VoDev, &pstCmd);
#else
    init_mipi_port_st7703(VoDev);
#endif

        backlight_lcd(1);
        return 0;
    }

    static VO_DSI_ATTR_S pstDsiCfg_st7703 =
        {
            .u32Lane = {1, 1, 1, 1},
            .u32BitsPerPixel = 24,
            .stSyncInfo = {
                .u32Hsa = 6,
                .u32Hbp = 39,
                .u32Hact = 720,
                .u32Hfp = 39,

                .u32Vsa = 4,
                .u32Vbp = 21,
                .u32Vact = 1440,
                .u32Vfp = 17},
            .fDphyClkMhz = 443.88864,
            .u32PllFreqReg2c0 = 0x240B8A22,
            .u32PllFreqReg38c = 0x2800};

// touch
//  TOUCH_INT
#define touch_power_port PORT_A
#define touch_power_group GROUP_3
#define touch_power_number 5
// TOUCH_RST
#define touch_rest_port PORT_D
#define touch_rest_group GROUP_1
#define touch_rest_number 6
// I2C_SDA3
#define touch_sda_port PORT_C
#define touch_sda_group GROUP_1
#define ltouch_sda_number 3
// I2C_SCLK3
#define touch_sclk_port PORT_C
#define touch_sclk_group GROUP_1
#define ltouch_sclk_number 4

    void touch_power_on(void)
    {
        int gpio_num = ar_hal_gpio_name_to_num(touch_power_port, touch_power_group, touch_power_number);
        ar_hal_gpio_export(gpio_num);
        ar_hal_gpio_set_dir(gpio_num, OUTPUT);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
        ar_delay(10);
    }

    void touch_power_off(void)
    {
        int gpio_num = ar_hal_gpio_name_to_num(touch_power_port, touch_power_group, touch_power_number);
        ar_hal_gpio_export(gpio_num);
        ar_hal_gpio_set_dir(gpio_num, OUTPUT);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
        ar_delay(10);
    }

    void touch_reset(void)
    {
        int gpio_num = ar_hal_gpio_name_to_num(touch_rest_port, touch_rest_group, touch_rest_number);
        ar_hal_gpio_export(gpio_num);
        ar_hal_gpio_set_dir(gpio_num, OUTPUT);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_LOW);
        ar_delay(10);
        ar_hal_gpio_set_value(gpio_num, GPIO_DATA_HIGH);
        ar_delay(10);
    }

    AR_S32 SAMPLE_VO_MIPILCD_720_1440(AR_BOOL MutiDev)
    {
        printf("SAMPLE_VO_MIPILCD_720_1440 start \r\n");
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_CSC_S stLayerCsc = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {0};
        VO_USER_INTFSYNC_INFO_S stUserInfo = {0};
        AR_U32 u32Framerate;
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo = {0};

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_720_1440;
        SIZE_S stFrameSize = {720, 1440};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        /* USER SET VO FRAME RATE */
        u32Framerate = 60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        stPubAttr.stSyncInfo.bSynm = AR_TRUE;
        stPubAttr.stSyncInfo.bIop = AR_TRUE;
        stPubAttr.stSyncInfo.u8Intfb = 0;
        stPubAttr.stSyncInfo.u16Hpw = 8;
        stPubAttr.stSyncInfo.u16Hbb = 52;
        stPubAttr.stSyncInfo.u16Hact = 720;
        stPubAttr.stSyncInfo.u16Hfb = 52;
        stPubAttr.stSyncInfo.u16Vpw = 4;
        stPubAttr.stSyncInfo.u16Vbb = 21;
        stPubAttr.stSyncInfo.u16Vact = 1440;
        stPubAttr.stSyncInfo.u16Vfb = 17;
        stPubAttr.stSyncInfo.u16Hmid = 0;
        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb = 0;
        stPubAttr.stSyncInfo.u16Bvfb = 0;
        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        stPubAttr.enIntfType = VO_INTF_MIPI;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* USER SET VO SYNC INFO OF USER INTF */
        stUserInfo.bClkReverse = AR_TRUE;
        stUserInfo.u32DevDiv = 1;
        stUserInfo.u32PreDiv = 1;
        stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        // power on display
        power_on_st7703();
        printf("power_on_st7703 \r\n");

        /* SET LCD */
        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg_st7703), "AR_MPI_VO_Dsi_SetAttr");

        reset_lcd(15, 3, 130);

        /* display st7703 init */
        init_display_st7703(VoDev);
        printf("init_display_st7703 \r\n");

        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
        // stLayerCsc.enCscMatrix = VO_CSC_MATRIX_BT601_TO_RGB_PC;
        // SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerCSC(VoLayer, &stLayerCsc), AR_MPI_VO_SetVideoLayerCSC);

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
        stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = 60; // SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        // i2c_init();
        // uint8_t data_w = 0x00;
        // i2c_write_data(i2c_addr, &data_w, 1);
        // ar_delay(1000);
        // unsigned char data_r[4];
        // int ret = -1;
        // ret = i2c_read_data(i2c_addr, 0, 4, data_r);
        // if (ret > 0)
        // {
        //     printf("data %x %x %x %x\n", data_r[0], data_r[1], data_r[2], data_r[3]);
        // }
        // else
        // {
        //     printf("no data \n");
        // }
        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);
        while (is_running)
        {
            // sleep(1);
            ar_delay(2000);
        }
        // i2c_uninit();

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();
        printf("SAMPLE_VO_MIPILCD_720_1440 stop \r\n");
        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_MIPILCD_1024_600_RESUME()
    {
        AR_S32 VoDev = 0;

        /* SET LCD */
        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_SetAttr(VoDev, &pstDsiCfg), "AR_MPI_VO_Dsi_SetAttr");
        SAMPLE_CHECK_RET(AR_MPI_VO_Dsi_Enable(VoDev), "AR_MPI_VO_Dsi_Enable");
        SAMPLE_VO_7INCH_LCD_Init();

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_HDMI_1080P60(RGN_TYPE_E enRgnType, AR_BOOL IsMono)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        if (IsMono)
            SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                                   PIXEL_FORMAT_U8C1);
        else
            SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                                   PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);
#if 0
    if (enRgnType < RGN_BUTT )
    {
        SAMPLE_VO_REGION_Init(COVER_RGN, 3);
        SAMPLE_VO_REGION_Init(MOSAIC_RGN, 3);
    }
#else
        SAMPLE_VO_REGION_Init(enRgnType, 3);
#endif

        while (is_running)
            sleep(1);

#if 0
    if (enRgnType < RGN_BUTT )
    {
        SAMPLE_VO_REGION_DeInit(MOSAIC_RGN, 3);
        SAMPLE_VO_REGION_DeInit(COVER_RGN, 3);
    }
#else
        SAMPLE_VO_REGION_DeInit(enRgnType, 3);
#endif

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();
        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_HDMI_PIP(AR_VOID)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 2;

        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080I60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserPipChnAttr(astChnAttr, &stDevSize);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_HDMI_1080I60(AR_VOID)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080I60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");
        AR_MPI_VO_SetVideoLayerPriority(VoLayer, 1);

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_HDMI_1080P60_MUTI_CHAN(AR_VOID)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 9;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_TEST(void)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr;
        VO_USER_INTFSYNC_INFO_S stUserInfo;
        AR_U32 u32Framerate;
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo = {0};

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        /* USER SET VO FRAME RATE */
        u32Framerate = 60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetDevFrameRate");

        /* USER SET VO DEV SYNC INFO */
        stPubAttr.enIntfSync = VO_OUTPUT_USER;
        stPubAttr.stSyncInfo.bSynm = AR_FALSE;
        stPubAttr.stSyncInfo.bIop = AR_FALSE;
        stPubAttr.stSyncInfo.u8Intfb = 0;
        stPubAttr.stSyncInfo.u16Hpw = 96;
        stPubAttr.stSyncInfo.u16Hbb = 216;
        stPubAttr.stSyncInfo.u16Hact = 1024;
        stPubAttr.stSyncInfo.u16Hfb = 216;
        stPubAttr.stSyncInfo.u16Vpw = 10;
        stPubAttr.stSyncInfo.u16Vbb = 23;
        stPubAttr.stSyncInfo.u16Vact = 600;
        stPubAttr.stSyncInfo.u16Vfb = 12;
        stPubAttr.stSyncInfo.u16Hmid = 0;
        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb = 0;
        stPubAttr.stSyncInfo.u16Bvfb = 0;
        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        stPubAttr.enIntfType = VO_INTF_BT656;
        stPubAttr.enPinMode.bt656 = VO_PIN_MODE_BT656_DEFAULT;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* USER SET VO SYNC INFO OF USER INTF */
        stUserInfo.bClkReverse = AR_FALSE;
        stUserInfo.u32DevDiv = 1;
        stUserInfo.u32PreDiv = 1;
        stUserInfo.stUserIntfSyncAttr.enClkSource = VO_CLK_SOURCE_PLL;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Fbdiv = 73;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Frac = 0x3D75E2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Refdiv = 2;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv1 = 3;
        stUserInfo.stUserIntfSyncAttr.stUserSyncPll.u32Postdiv2 = 2;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetUserIntfSyncInfo(VoDev, &stUserInfo), "AR_MPI_VO_SetUserIntfSyncInfo");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = stPubAttr.stSyncInfo.u16Hact;
        stDevSize.u32Height = stPubAttr.stSyncInfo.u16Vact;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_TEST_DVP_1080P_TP2803(AR_VOID)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_BT1120;
        stPubAttr.enIntfSync = VO_OUTPUT_USER; // VO_OUTPUT_1080P60;
        stPubAttr.stSyncInfo.bSynm = AR_FALSE; // sync internal
        stPubAttr.stSyncInfo.bIop = AR_TRUE;   // P
        stPubAttr.stSyncInfo.u8Intfb = 0;

        stPubAttr.stSyncInfo.u16Hmid = 0;
        stPubAttr.stSyncInfo.u16Bvact = 0;
        stPubAttr.stSyncInfo.u16Bvbb = 0;
        stPubAttr.stSyncInfo.u16Bvfb = 0;

        stPubAttr.stSyncInfo.bIdv = AR_FALSE;
        stPubAttr.stSyncInfo.bIhs = AR_FALSE;
        stPubAttr.stSyncInfo.bIvs = AR_FALSE;
        /* SET VO DEV SYNC INFO */
        stPubAttr.stSyncInfo.u16Hpw = 280;
        stPubAttr.stSyncInfo.u16Hbb = 0;
        stPubAttr.stSyncInfo.u16Hact = 1920;
        stPubAttr.stSyncInfo.u16Hfb = 0;
        stPubAttr.stSyncInfo.u16Vpw = 45;
        stPubAttr.stSyncInfo.u16Vbb = 0;
        stPubAttr.stSyncInfo.u16Vact = 1080;
        stPubAttr.stSyncInfo.u16Vfb = 0;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        AR_U32 u32Framerate = 30;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetDevFrameRate(VoDev, u32Framerate), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        tp28xx_dvp_init();

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerPartitionMode(VoLayer, VO_PART_MODE_SINGLE), "AR_MPI_VO_SetVideoLayerPartitionMode");

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;
        stLayerAttr.bClusterMode = AR_FALSE;
        stLayerAttr.bDoubleFrame = AR_FALSE;
        stLayerAttr.enDstDynamicRange = DYNAMIC_RANGE_SDR8;
        stLayerAttr.enPixFormat = PIXEL_FORMAT_YVU_PLANAR_420;
        stLayerAttr.stDispRect.s32X = 0;
        stLayerAttr.stDispRect.s32Y = 0;
        stLayerAttr.stDispRect.u32Height = stDevSize.u32Height;
        stLayerAttr.stDispRect.u32Width = stDevSize.u32Width;
        stLayerAttr.stImageSize.u32Height = stDevSize.u32Height;
        stLayerAttr.stImageSize.u32Width = stDevSize.u32Width;
        stLayerAttr.u32DispFrmRt = 30;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt, PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);

        while (is_running)
            sleep(1);

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();
        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_TEST_DVP_1080P_TP2803_RESUME(AR_VOID)
    {
        tp28xx_dvp_init();
    }

    static AR_S32 S_VoDev = 0;
    static AR_S32 S_VoLayer = 0;
    static AR_S32 S_VoChnNum = 1;
    static SAMPLE_VOU_ThreadCtrl_Info S_ThreadInfo = {0};

    AR_S32 SAMPLE_VO_HDMI_1080P60_DDR_RETENTION()
    {
        int ret = 0;
        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        ret = SAMPLE_VO_HDMI_1080P60_CREATE();
        if (ret)
            goto End;

        while (is_running)
            sleep(1);

        ret = SAMPLE_VO_HDMI_1080P60_DESTORY();

    End:
        SAMPLE_VOU_SYS_Exit();

        return ret;
    }

    AR_S32 SAMPLE_VO_HDMI_1080P60_CREATE()
    {
        AR_U32 i = 0;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        memset(&S_ThreadInfo, 0, sizeof(SAMPLE_VOU_ThreadCtrl_Info));

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(S_VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(S_VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(S_VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(S_VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, S_VoChnNum);

        for (i = 0; i < S_VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(S_VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(S_VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        SAMPLE_VO_StartUserThd(&S_ThreadInfo, S_VoLayer, S_VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                               PIXEL_FORMAT_YVU_PLANAR_420);

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_VO_HDMI_1080P60_DESTORY()
    {
        int i = 0;
        int ret = 0;
        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&S_ThreadInfo);

        /*DISABLE VO CHN*/
        for (i = 0; i < S_VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(S_VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(S_VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(S_VoDev), "AR_MPI_VO_Disable");

        ret = AR_MPI_VB_DestroyPool(Pool);
        if (ret)
            printf("destory pool failed, ret:%d\n", ret);

        return AR_SUCCESS;
    }

    AR_S32 SAMPLE_SUBSCRIBE_CALL_BACK(VO_DEV dev_id, VO_SUBSCRIBE_INFO_S *sub_info)
    {
#if 1
        uint64_t t = ar_get_timestamp_us();
        if (sub_info->u32IrqType & IRQ_TYPE_FRAME_DONE)
        {
            AR_LOG_RAW("%02x %lu %d %lu %lu %lu\n", sub_info->u32IrqType,
                       sub_info->u64IrqTimeNs,
                       sub_info->stExtpara.stFrameDonePara.u32SrcFrameId,
                       sub_info->stExtpara.stFrameDonePara.u32SrcPTS,
                       t,
                       t - sub_info->u64IrqTimeNs / 1000);
        }
        else
        {
            AR_LOG_RAW("%02x %lu %lu %lu\n", sub_info->u32IrqType,
                       sub_info->u64IrqTimeNs,
                       t,
                       t - sub_info->u64IrqTimeNs / 1000);
        }
#endif
    }

    AR_S32 SAMPLE_VO_HDMI_1080P60_SUBSCRIBE(RGN_TYPE_E enRgnType, AR_BOOL IsMono)
    {
        AR_U32 i = 0;
        AR_S32 VoDev = 0;
        AR_S32 VoLayer = 0;
        AR_S32 VoChnNum = 1;
        VO_PUB_ATTR_S stPubAttr = {0};
        VO_VIDEO_LAYER_ATTR_S stLayerAttr = {{0}};
        SIZE_S stDevSize;

        SAMPLE_VOU_ThreadCtrl_Info stThreadInfo;

        VO_CHN_ATTR_S astChnAttr[VO_MAX_CHN_NUM];

        AR_CHAR filename[256] = YUV_1920_1080;
        SIZE_S stFrameSize = {1920, 1080};

        SAMPLE_CHECK_RET(SAMPLE_VOU_SYS_Init(), "SAMPLE_VOU_SYS_Init");

        /* SET SUBSCRIBE ATTR */
        VO_SUBSCRIBE_ATTR_S stSubAttr = {0};
        stSubAttr.u32SubscribeType = IRQ_TYPE_FRAME_DONE;
        stSubAttr.subscribe_call_back = SAMPLE_SUBSCRIBE_CALL_BACK;
        SAMPLE_CHECK_RET(AR_MPI_VO_SubscribeEnable(VoDev, &stSubAttr), "AR_MPI_VO_SubscribeEnable");

        /* SET VO PUB ATTR OF USER TYPE */
        SAMPLE_VO_GetUserPubBaseAttr(&stPubAttr);

        stPubAttr.enIntfType = VO_INTF_HDMI;
        stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
        SAMPLE_CHECK_RET(AR_MPI_VO_SetPubAttr(VoDev, &stPubAttr), "AR_MPI_VO_SetPubAttr");

        /* ENABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Enable(VoDev), "AR_MPI_VO_Enable");

        /*SET VO LAYER ATTR*/
        stDevSize.u32Width = 1920;
        stDevSize.u32Height = 1080;

        SAMPLE_VO_GetUserLayerAttr(&stLayerAttr, &stDevSize);
        stLayerAttr.u32DispFrmRt = SAMPLE_VO_LAYER_FPS;

        SAMPLE_CHECK_RET(AR_MPI_VO_SetVideoLayerAttr(VoLayer, &stLayerAttr), "AR_MPI_VO_SetVideoLayerAttr");

        /* ENABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_EnableVideoLayer(VoLayer), "AR_MPI_VO_EnableVideoLayer");

        /* SET AND ENABLE VO CHN */
        SAMPLE_VO_GetUserChnAttr(astChnAttr, &stDevSize, VoChnNum);

        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_SetChnAttr(VoLayer, i, &astChnAttr[i]), "AR_MPI_VO_SetChnAttr");

            SAMPLE_CHECK_RET(AR_MPI_VO_EnableChn(VoLayer, i), "AR_MPI_VO_EnableChn");
        }

        /* START USER THREAD */
        if (IsMono)
            SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                                   PIXEL_FORMAT_U8C1);
        else
            SAMPLE_VO_StartUserThd(&stThreadInfo, VoLayer, VoChnNum, filename, &stFrameSize, stLayerAttr.u32DispFrmRt,
                                   PIXEL_FORMAT_YVU_PLANAR_420);

        // SAMPLE_VO_StartUserCtrl(&stThreadInfo, VoLayer, VoChnNum);
#if 0
    if (enRgnType < RGN_BUTT )
    {
        SAMPLE_VO_REGION_Init(COVER_RGN, 3);
        SAMPLE_VO_REGION_Init(MOSAIC_RGN, 3);
    }
#else
        SAMPLE_VO_REGION_Init(enRgnType, 3);
#endif

        while (is_running)
            sleep(1);

#if 0
    if (enRgnType < RGN_BUTT )
    {
        SAMPLE_VO_REGION_DeInit(MOSAIC_RGN, 3);
        SAMPLE_VO_REGION_DeInit(COVER_RGN, 3);
    }
#else
        SAMPLE_VO_REGION_DeInit(enRgnType, 3);
#endif

        /* STOP USER THREAD */
        SAMPLE_VO_StopUserThd(&stThreadInfo);

        SAMPLE_CHECK_RET(AR_MPI_VO_SubscribeDisable(VoDev), "AR_MPI_VO_SubscribeDisable");

        /*DISABLE VO CHN*/
        for (i = 0; i < VoChnNum; i++)
        {
            SAMPLE_CHECK_RET(AR_MPI_VO_DisableChn(VoLayer, i), "AR_MPI_VO_DisableChn");
        }

        /* DISABLE VO LAYER */
        SAMPLE_CHECK_RET(AR_MPI_VO_DisableVideoLayer(VoLayer), "AR_MPI_VO_DisableVideoLayer");

        /* DISABLE VO DEV */
        SAMPLE_CHECK_RET(AR_MPI_VO_Disable(VoDev), "AR_MPI_VO_Disable");

        SAMPLE_VOU_SYS_Exit();
        return AR_SUCCESS;
    }

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
