#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

//copy from hisi sample
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>

#include "mpi_venc.h"
#include "mpi_vpss.h"
#include "vctrl_venc.h"
#include "vctrl_msg.h"
#include "vctrl_common.h"
#include "mpi_sys.h"
#include "vctrl_list.h"
#include "hal_sys.h"
#include "utils_dbglog.h"
#include "ar_comm_vpss.h"

//const AR_U8 g_SOI[2] = {0xFF, 0xD8};
//const AR_U8 g_EOI[2] = {0xFF, 0xD9};
static pthread_t gs_VencPid[VENC_MAX_CHN_NUM];
static pthread_t gs_VencSendPid[VENC_MAX_CHN_NUM];
static pthread_t gs_VencQpmapPid;

static VCHN_INFO gs_VencInfo[VENC_MAX_CHN_NUM];

static VCTRL_VENC_GETSTREAM_PARA_S gs_stPara;
static VCTRL_VENC_SENDSTREAM_PARA_S gs_stSendPara;
static VCTRL_VENC_QPMAP_SENDFRAME_PARA_S stQpMapSendFramePara;
static VIDEO_FRAME_INFO_S gs_stPreLoadPic[VENC_MAX_CHN_NUM][5] = { 0 };
static int gs_picPostion[VENC_MAX_CHN_NUM] = { 0 };

static AR_S32 gs_s32SnapCnt = 0;
static AR_CHAR* DstBuf = NULL;
static ViData gsViData[VENC_MAX_CHN_NUM] = {0};
AR_S64 lastPts[VENC_MAX_CHN_NUM] = { 0 };
AR_S64 diffPts[VENC_MAX_CHN_NUM] = { 0 };


#define TEMP_BUF_LEN 8
#define MAX_THM_SIZE (64*1024)

struct VinPara {
    FILE *pfd;
    AR_S32 width;
    AR_S32 height;
    AR_S32 frameRate;
};

#ifdef __READ_ALL_FILE__
static AR_S32 FileTrans_GetThmFromJpg(AR_CHAR* JPGPath, AR_U32* DstSize)
{
    AR_S32 s32RtnVal = 0;
    FILE* fpJpg = NULL;
    AR_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    AR_S32 bufpos = 0;
    AR_CHAR startflag[2] = {0xff, 0xd8};
    AR_S32 startpos = 0;
    AR_CHAR endflag[2] = {0xff, 0xd9};
    AR_S32 endpos = 0;
    fpJpg = fopen(JPGPath, "rb");
    AR_CHAR* pszFile = NULL;
    AR_S32 fd = 0;
    AR_S32 s32I = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));
    if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return AR_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);
        pszFile = (AR_CHAR*)malloc(stStat.st_size);
        if ((NULL == pszFile) || (stStat.st_size < 6))
        {
            fclose(fpJpg);
            printf("memory malloc fail!\n");
            return AR_FAILURE;
        }

        if (fread(pszFile, stStat.st_size , 1, fpJpg) <= 0)
        {
            fclose(fpJpg);
            free(pszFile);
            printf("fread jpeg src fail!\n");
            return AR_FAILURE;
        }

        fclose(fpJpg);
        AR_U16 u16THMLen = 0;
        u16THMLen = (pszFile[4] << 8) + pszFile[5];
        while (s32I < stStat.st_size)
        {
            tempbuf[bufpos] = pszFile[s32I++];
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = s32I - 2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    if (u16THMLen == s32I)
                    {
                        endpos = s32I;
                        break;
                    }
                    else
                    {
                        endpos = s32I;
                        break;
                    }
                }
            }
            bufpos++;
            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }

            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
                bufpos = 0;
            }

        }

    }
    if (endpos - startpos <= 0)
    {
        free(pszFile);
        printf("get .thm 11 fail!\n");
        return AR_FAILURE;
    }

    if (endpos - startpos >= stStat.st_size)
    {
        free(pszFile);
        printf("NO DCF info, get .thm 22 fail!\n");
        return AR_FAILURE;
    }

    AR_CHAR* temp = pszFile + startpos;
    if(MAX_THM_SIZE < (endpos - startpos))
    {
        printf("Thm is too large than MAX_THM_SIZE, get .thm 33 fail!\n");
        return AR_FAILURE;
    }

    AR_CHAR* cDstBuf = (AR_CHAR*)malloc(endpos - startpos);
    if (NULL == cDstBuf)
    {
        printf("memory malloc fail!\n");
        return AR_FAILURE;
    }

    memcpy(cDstBuf, temp, endpos - startpos);

    DstBuf = cDstBuf;
    *DstSize = endpos - startpos;
    free(pszFile);

    return AR_SUCCESS;
}

#else
static AR_S32 FileTrans_GetThmFromJpg(AR_CHAR* JPGPath, AR_U32* DstSize)
{
    AR_CHAR tempbuf[TEMP_BUF_LEN] = {0};
    AR_S32 bufpos = 0;
    AR_CHAR startflag[2] = {0xff, 0xd8};
    AR_S32 startpos = 0;
    AR_CHAR endflag[2] = {0xff, 0xd9};
    AR_S32 endpos = 0;
    AR_BOOL bStartMatch = AR_FALSE;

    AR_S32 fd = 0;
    struct stat stStat;
    memset(&stStat, 0, sizeof(struct stat));

    FILE* fpJpg = NULL;
    fpJpg = fopen(JPGPath, "rb");
    if (NULL == fpJpg)
    {
        printf("file %s not exist!\n", JPGPath);
        return AR_FAILURE;
    }
    else
    {
        fd = fileno(fpJpg);
        fstat(fd, &stStat);

        while (!feof(fpJpg))
        {
            tempbuf[bufpos]=getc(fpJpg);
            if (bufpos > 0)
            {
                if (0 == memcmp(tempbuf + bufpos - 1, startflag, sizeof(startflag)))
                {
                    startpos = ftell(fpJpg)-2;
                    if (startpos < 0)
                    {
                        startpos = 0;
                    }
                    bStartMatch = AR_TRUE;
                }
                if (0 == memcmp(tempbuf + bufpos - 1, endflag, sizeof(endflag)))
                {
                    endpos = ftell(fpJpg);
                    if(AR_TRUE == bStartMatch)
                    {
                        break;
                    }
                }
            }
            bufpos++;

            if (bufpos == (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos - 1] != 0xFF)
                {
                    bufpos = 0;
                }
            }
            else if (bufpos > (TEMP_BUF_LEN - 1))
            {
                if (tempbuf[bufpos -1] == 0xFF)
                {
                    tempbuf[0] = 0xFF;
                    bufpos = 1;
                }
                else
                {
                    bufpos = 0;
                }
            }
        }
    }

    if (endpos - startpos <= 0)
    {
        printf("get .thm 11 fail!\n");
        fclose(fpJpg);
        return AR_FAILURE;
    }

    if (endpos - startpos > MAX_THM_SIZE)
    {
        printf("Thm is too large than MAX_THM_SIZE, get .thm 22 fail!\n");
        fclose(fpJpg);
        return AR_FAILURE;
    }

    if (endpos - startpos >= stStat.st_size)
    {
        printf("NO DCF info, get .thm 33 fail!\n");
        fclose(fpJpg);
        return AR_FAILURE;
    }

    AR_CHAR* cDstBuf = (AR_CHAR*)malloc(endpos - startpos);
    if (NULL == cDstBuf)
    {
        printf("memory malloc fail!\n");
        fclose(fpJpg);
        return AR_FAILURE;
    }

    fseek(fpJpg, (long)startpos, SEEK_SET);
    *DstSize = fread(cDstBuf,1,endpos-startpos,fpJpg);
    if(*DstSize != (endpos - startpos))
    {
        free(cDstBuf);
        printf("fread fail!\n");
        fclose(fpJpg);
        return AR_FAILURE;
    }

    DstBuf = cDstBuf;
    fclose(fpJpg);

    return AR_SUCCESS;
}
#endif


/******************************************************************************
* function : Set venc memory location
******************************************************************************/
AR_S32 VCTRL_VENC_MemConfig(AR_VOID)
{
    AR_S32 i = 0;
    AR_S32 s32Ret;
    AR_CHAR* pcMmzName;
    MPP_CHN_S stMppChnVENC;
    /* group, venc max chn is 64*/
    for (i = 0; i < 64; i++)
    {
        stMppChnVENC.enModId = AR_ID_VENC;
        stMppChnVENC.s32DevId = 0;
        stMppChnVENC.s32ChnId = i;
        pcMmzName = NULL;

        /*venc*/
        s32Ret = AR_MPI_SYS_SetMemConfig(&stMppChnVENC, pcMmzName);
        if (AR_SUCCESS != s32Ret)
        {
            VCTRL_PRT("AR_MPI_SYS_SetMemConfig with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : get file postfix according palyload_type.
******************************************************************************/
AR_S32 VCTRL_VENC_GetFilePostfix(PAYLOAD_TYPE_E enPayload, char* szFilePostfix)
{
    if (PT_H264 == enPayload)
    {
        strcpy(szFilePostfix, ".h264");
    }
    else if (PT_H265 == enPayload)
    {
        strcpy(szFilePostfix, ".h265");
    }
    else if (PT_JPEG == enPayload)
    {
        strcpy(szFilePostfix, ".jpg");
    }
    else if (PT_MJPEG == enPayload)
    {
        strcpy(szFilePostfix, ".mjp");
    }
    else
    {
        VCTRL_PRT("payload type err!\n");
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}


AR_S32 VCTRL_VENC_GetGopAttr(VENC_GOP_MODE_E enGopMode,VENC_GOP_ATTR_S *pstGopAttr)
{
    switch(enGopMode)
    {
        case VENC_GOPMODE_NORMALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_NORMALP;
            pstGopAttr->stNormalP.s32IPQpDelta = 2;
        break;
        case VENC_GOPMODE_SMARTP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_SMARTP;
            pstGopAttr->stSmartP.s32BgQpDelta  = 4;
            pstGopAttr->stSmartP.s32ViQpDelta  = 2;
            pstGopAttr->stSmartP.u32BgInterval =  90;
        break;

        case VENC_GOPMODE_DUALP:
            pstGopAttr->enGopMode  = VENC_GOPMODE_DUALP;
            pstGopAttr->stDualP.s32IPQpDelta  = 4;
            pstGopAttr->stDualP.s32SPQpDelta  = 2;
            pstGopAttr->stDualP.u32SPInterval = 3;
        break;

        case VENC_GOPMODE_BIPREDB:
            pstGopAttr->enGopMode  = VENC_GOPMODE_BIPREDB;
            pstGopAttr->stBipredB.s32BQpDelta  = -2;
            pstGopAttr->stBipredB.s32IPQpDelta = 3;
            pstGopAttr->stBipredB.u32BFrmNum   = 2;
        break;

        default:
            VCTRL_PRT("not support the gop mode !\n");
            return AR_FAILURE;
        break;
    }
    return AR_SUCCESS;
}


AR_S32 VCTRL_VENC_Getdcfinfo(char* SrcJpgPath, char* DstThmPath)
{
    AR_S32 s32RtnVal = AR_SUCCESS;
    AR_CHAR JPGSrcPath[FILE_NAME_LEN] = {0};
    AR_CHAR JPGDesPath[FILE_NAME_LEN] = {0};
    AR_U32 DstSize = 0;
    snprintf(JPGSrcPath, sizeof(JPGSrcPath), "%s", SrcJpgPath);
    snprintf(JPGDesPath, sizeof(JPGDesPath), "%s", DstThmPath);

    s32RtnVal = FileTrans_GetThmFromJpg(JPGSrcPath, &DstSize);
    if ((AR_SUCCESS != s32RtnVal) || (0 == DstSize))
    {
        printf("fail to get thm\n");
        return AR_FAILURE;
    }

    FILE* fpTHM = fopen(JPGDesPath, "w");
    if (AR_NULL == fpTHM)
    {
        printf("file to create file %s\n", JPGDesPath);
        return AR_FAILURE;
    }

    AR_U32 u32WritenSize = 0;
    while (u32WritenSize < DstSize)
    {
        s32RtnVal = fwrite(DstBuf + u32WritenSize, 1, DstSize, fpTHM);
        if (s32RtnVal <= 0)
        {
            VCTRL_PRT("fail to wirte file, rtn=%d\n", s32RtnVal);
            break;
        }

        u32WritenSize += s32RtnVal;
    }

    if (fpTHM)
    {
        fclose(fpTHM);
        fpTHM = 0;
    }

    if(NULL != DstBuf)
    {
        free(DstBuf);
        DstBuf = NULL;
    }

    return 0;
}



/******************************************************************************
* funciton : save stream
******************************************************************************/
AR_S32 VCTRL_VENC_SaveStream(FILE* pFd, VENC_STREAM_S* pstStream)
{
    AR_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++)
    {
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);

        fflush(pFd);
    }

    return AR_SUCCESS;
}


/******************************************************************************
* funciton : the process of physical address retrace
******************************************************************************/
AR_S32 VCTRL_VENC_SaveStream_PhyAddr(FILE* pFd, VENC_STREAM_BUF_INFO_S *pstStreamBuf, VENC_STREAM_S* pstStream)
{
    AR_U32 i,j;
    AR_U64 u64SrcPhyAddr;
    AR_U32 u32Left;
    AR_S32 s32Ret = 0;

    for(i=0; i<pstStream->u32PackCount; i++)
    {
        for(j=0; j<MAX_TILE_NUM; j++)
        {
            if((pstStream->pstPack[i].u64PhyAddr > pstStreamBuf->u64PhyAddr[j])&&\
                (pstStream->pstPack[i].u64PhyAddr <= pstStreamBuf->u64PhyAddr[j]+pstStreamBuf->u64BufSize[j]))
                break;
        }

        if(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Len >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
        {
            if (pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset >=
                pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j])
            {
                /* physical address retrace in offset segment */
                u64SrcPhyAddr = pstStreamBuf->u64PhyAddr[j] +
                                ((pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset) -
                                (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]));

                s32Ret = fwrite ((void *)(AR_U64)u64SrcPhyAddr, pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    VCTRL_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
            else
            {
                /* physical address retrace in data segment */
                u32Left = (pstStreamBuf->u64PhyAddr[j] + pstStreamBuf->u64BufSize[j]) - pstStream->pstPack[i].u64PhyAddr;

                s32Ret = fwrite((void *)(AR_U64)(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset),
                             u32Left - pstStream->pstPack[i].u32Offset, 1, pFd);
                if(s32Ret<0)
                {
                    VCTRL_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }

                s32Ret = fwrite((void *)(AR_U64)pstStreamBuf->u64PhyAddr[j], pstStream->pstPack[i].u32Len - u32Left, 1, pFd);
                if(s32Ret<0)
                {
                    VCTRL_PRT("fwrite err %d\n", s32Ret);
                    return s32Ret;
                }
            }
        }
        else
        {
            /* physical address retrace does not happen */
            s32Ret = fwrite ((void *)(AR_U64)(pstStream->pstPack[i].u64PhyAddr + pstStream->pstPack[i].u32Offset),
                          pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset, 1, pFd);
            if(s32Ret<0)
            {
                VCTRL_PRT("fwrite err %d\n", s32Ret);
                return s32Ret;
            }
        }
            fflush(pFd);
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_SYS_Init(SIZE_S* pstSnsSize)
{
    AR_S32 s32Ret;
    AR_U64 u64BlkSize;
    VB_CONFIG_S stVbConf;
    SIZE_S     stSnsSize;

    if(NULL == pstSnsSize)
    {
        VCTRL_PRT("pstSnsSize is NULL!\n");
        return AR_FAILURE;
    }

    stSnsSize.u32Width = pstSnsSize->u32Width;
    stSnsSize.u32Height = pstSnsSize->u32Height;

    memset(&stVbConf, 0, sizeof(VB_CONFIG_S));

    u64BlkSize = COMMON_GetPicBufferSize(stSnsSize.u32Width, stSnsSize.u32Height, PIXEL_FORMAT_YVU_PLANAR_420, DATA_BITWIDTH_8, COMPRESS_MODE_SEG,DEFAULT_ALIGN);
    VCTRL_PRT("u64BlkSize =%lld!\n", u64BlkSize);

    stVbConf.astCommPool[0].u64BlkSize   = u64BlkSize;
    stVbConf.astCommPool[0].u32BlkCnt    = YUV_FRAME_CACHED_NUM;
    stVbConf.u32MaxPoolCnt = 1;

    s32Ret = VCTRL_COMM_SYS_Init(&stVbConf);

    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("VCTRL_COMM_SYS_Init failed!\n");
        return s32Ret;
    }

    return AR_SUCCESS;
}

AR_VOID VCTRL_VENC_Preload_Pic(AR_S32 chn_id)
{
    AR_S32 s32Ret;
    ViData * p_data = NULL;
    p_data = (ViData *)&(gsViData[chn_id]);

    VI_USERPIC_ATTR_S * pUsrPic = &p_data->stUsrPic;

    p_data->stViFrameInfo.u32PoolId = p_data->u32PoolId;

    for (int i = 0; i < gs_VencInfo[chn_id].preLoadPicNum ; i++) {
        s32Ret = VCTRL_COMM_VI_Load_UserPic(p_data->pfd, pUsrPic, &p_data->stViFrameInfo);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("VCTRL_VENC_Preload_Pic failed with %#x!\n", s32Ret);
        }

        printf("VCTRL_VENC_Preload_Pic id %d posion %d \n", chn_id, i);
        memcpy(&gs_stPreLoadPic[chn_id][i], &pUsrPic->unUsrPic.stUsrPicFrm, sizeof(VIDEO_FRAME_INFO_S));
        gs_stPreLoadPic[chn_id][i].stVFrame.u32FrameId = i + 1;
    }

}


AR_S32 bind_get_frame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    AR_S32 s32Ret = -1;
    uint64_t u64CurTimeMs = 0, u64CostTimeMs = 0;
    int framerate = 25;
    AR_U32 frameid = 0;
    AR_U64 currentPts = 0, cpyPts = 0, cpy_cost_time = 0;
    /*
    VB_BLK block_handle;
    VB_POOL pool_handle;
    */
    ViData * p_data;
    VIDEO_FRAME_INFO_S * pstFrameInfo = (VIDEO_FRAME_INFO_S *)pv_data;
    if (NULL == pstFrameInfo) {
        VCTRL_PRT("pv_data is NULL!\n");
        return -1;
    }
#if 0
    p_data = (ViData *)pv_priv_data;
#else
    p_data = (ViData *)&(gsViData[s32_chn_id]);
#endif

    if (gs_VencInfo[s32_chn_id].preLoadPicFlag) {
        frameid = gs_stPreLoadPic[s32_chn_id][gs_picPostion[s32_chn_id]].stVFrame.u32FrameId;
        s32Ret = AR_MPI_SYS_GetCurPTS(&currentPts);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_SYS_GetCurPTS failed with %#x!\n", s32Ret);
        }
        gs_stPreLoadPic[s32_chn_id][gs_picPostion[s32_chn_id]].stVFrame.u64PTS = currentPts;
        memcpy(pstFrameInfo, &gs_stPreLoadPic[s32_chn_id][gs_picPostion[s32_chn_id]], sizeof(VIDEO_FRAME_INFO_S));
        gs_stPreLoadPic[s32_chn_id][gs_picPostion[s32_chn_id]].stVFrame.u32FrameId = frameid +
            gs_VencInfo[s32_chn_id].preLoadPicNum;
        gs_picPostion[s32_chn_id] ++;
        if (gs_picPostion[s32_chn_id] == 3) {
            gs_picPostion[s32_chn_id] = 0;
        }
    } else {
        VI_USERPIC_ATTR_S * pUsrPic = &p_data->stUsrPic;

        p_data->stViFrameInfo.u32PoolId = p_data->u32PoolId;

        s32Ret = VCTRL_COMM_VI_Load_UserPic(p_data->pfd, pUsrPic, &p_data->stViFrameInfo);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("VCTRL_COMM_VI_Load_UserPic failed with %#x! chn %d\n", s32Ret, s32_chn_id);
        }
        memcpy(pstFrameInfo, &pUsrPic->unUsrPic.stUsrPicFrm, sizeof(VIDEO_FRAME_INFO_S));
    }

    if (gs_VencInfo[s32_chn_id].u32SrcFrameRate != 0) {
        framerate = gs_VencInfo[s32_chn_id].u32SrcFrameRate;
    }

    if (framerate >= 90 && framerate < 200) {
        framerate = framerate + 1;
    } else if (framerate >= 200 && framerate < 234) {
        framerate = framerate + 2;
    } else if (framerate >= 234) {
        framerate = framerate + 3;
    }

    usleep((1000 * 1000) / framerate);

    return AR_SUCCESS;
}
/*
AR_S32 (*release_frame_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, const AR_VOID *pv_data, AR_VOID *pv_priv_data);
*/
AR_S32 bind_release_frame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  const AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    ViData * p_data;
    VCTRL_VI_FRAME_INFO_S * frame_info = (VCTRL_VI_FRAME_INFO_S * )pv_data;
    if(NULL == frame_info) {
        VCTRL_PRT("pv_data is NULL!\n");
        return -1;
    }
#if 0
    p_data = (ViData *)pv_priv_data;
#else
    p_data = (ViData *)&(gsViData[s32_chn_id]);
#endif

    if (!gs_VencInfo[s32_chn_id].preLoadPicFlag) {
        VCTRL_COMM_VI_Release_UserPic(&p_data->stViFrameInfo);
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_VI_Init( VCTRL_VI_BIND_INFO * pstViInfo)
{
    static int register_sender_called = 0;
    AR_S32 s32Ret;
    STRU_SYS_BIND_SENDER * pViBind = &pstViInfo->stViBind;

    if (register_sender_called) {
        //snder has been called.
        return AR_SUCCESS;
    }

    pViBind->e_mod_id = AR_SYS_ID_VI;
    pViBind->u32_max_dev_cnt = 40;
    pViBind->u32_max_chn_cnt = 40;
    pViBind->s32_flag = 0;
    pViBind->e_data_type = AR_SYS_DATA_VI_FRAME;
    pViBind->give_bind_call_back = NULL;

    pViBind->get_frame_call_back = bind_get_frame;
    pViBind->release_frame_call_back = bind_release_frame;

    VI_USERPIC_ATTR_S stUsrPic;
    stUsrPic.enUsrPicMode = VI_USERPIC_MODE_PIC;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Width = pstViInfo->u32Width;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Height = pstViInfo->u32Height;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    ViData * pViData = pstViInfo->stViData;

    memcpy(&pViData->stUsrPic, &stUsrPic, sizeof(stUsrPic));
    pViData->e_mod_id = AR_SYS_ID_VI;
    pViData->p_sender = pViBind;

/*
    VCTRL_PRT("====to ar_mpi_open YUV file: %s. \n", pstViInfo->inFile);
    pViData->pfd = fopen(pstViInfo->inFile, "rb");
    if (pViData->pfd == NULL)
    {
        VCTRL_PRT("ar_mpi_open file -> %s fail \n", pstViInfo->inFile);
        return AR_FAILURE;
    }
*/
    pViBind->pv_priv_data = pViData;

    //ar_hal_sys_bind_init();

    if ((s32Ret = ar_hal_sys_bind_register_sender(pViBind)) < 0) {
        VCTRL_PRT("bind_register_sender failed (%x) !\n", s32Ret);
        return AR_FAILURE;
    }

    register_sender_called = 1;

    return AR_SUCCESS;
}

AR_VOID VCTRL_VENC_VI_Deinit( VCTRL_VI_BIND_INFO * pstViInfo)
{
    //ar_hal_sys_bind_unregister_sender(AR_SYS_ID_VI);
    ViData *     pViData = pstViInfo->stViData;

    //ar_hal_sys_bind_exit();

    if(pViData->pfd)
       fclose(pViData->pfd);
}

AR_S32 VCTRL_VI_UnBind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VencChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VencChn;

    CHECK_RET(AR_MPI_SYS_UnBind(&stSrcChn, &stDestChn), "AR_MPI_SYS_UnBind(VI-VENC)");

    return AR_SUCCESS;
}


AR_S32 VCTRL_VI_Bind_VENC(VI_PIPE ViPipe, VI_CHN ViChn, VENC_CHN VenChn)
{
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;

    stSrcChn.enModId   = AR_ID_VI;
    stSrcChn.s32DevId  = ViPipe;
    stSrcChn.s32ChnId  = ViChn;

    stDestChn.enModId  = AR_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = VenChn;

    CHECK_RET(AR_MPI_SYS_Bind(&stSrcChn, &stDestChn), "AR_MPI_SYS_Bind(VI-VENC)");

    return AR_SUCCESS;
}

AR_VOID VCTRL_Dump_VencChnAttr(VENC_CHN_ATTR_S* pstVencChnAttr)
{
    printf("stVencAttr.enType=%d\n", pstVencChnAttr->stVencAttr.enType);
    printf("stVencAttr.u32MaxPicWidth=%d\n", pstVencChnAttr->stVencAttr.u32MaxPicWidth);
    printf("stVencAttr.u32MaxPicHeight=%d\n", pstVencChnAttr->stVencAttr.u32MaxPicHeight);
    printf("stVencAttr.u32BufSize=%d\n", pstVencChnAttr->stVencAttr.u32BufSize);
    printf("stVencAttr.u32Profile=%d\n", pstVencChnAttr->stVencAttr.u32Profile);
    printf("stVencAttr.bByFrame=%d\n", pstVencChnAttr->stVencAttr.bByFrame);
    printf("stVencAttr.u32PicWidth=%d\n", pstVencChnAttr->stVencAttr.u32PicWidth);
    printf("stVencAttr.u32PicHeight=%d\n", pstVencChnAttr->stVencAttr.u32PicHeight);

    printf("stRcAttr.enRcMode=%d\n", pstVencChnAttr->stRcAttr.enRcMode);
    printf("stRcAttr.u32SrcFrameRate=%d\n", pstVencChnAttr->stRcAttr.stH265Cbr.u32SrcFrameRate);
    printf("stRcAttr.u32BitRate=%d\n", pstVencChnAttr->stRcAttr.stH265Cbr.u32BitRate);

    printf("stGopAttr.enGopMode=%d\n", pstVencChnAttr->stGopAttr.enGopMode);
    printf("stGopAttr.s32IPQpDelta=%d\n", pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta);

}

/*dynamic change channel attr*/
AR_S32 VCTRL_VENC_SetAttr(VENC_CHN_ATTR_S *pstVencChnAttr, VCHN_INFO* pChnInfo, VENC_GOP_ATTR_S *pstGopAttr)
{
    AR_U32                 u32StatTime;

    AR_U32                 u32Gop = pChnInfo->u32Gop;
    VENC_ATTR_JPEG_S       stJpegAttr;
    AR_U32                 u32BitRate = pChnInfo->u32BitRate;
    AR_U32                 u32FrameRate = pChnInfo->u32FrameRate;
    PAYLOAD_TYPE_E enType = pstVencChnAttr->stVencAttr.enType;
    VCTRL_RC_E enRcMode = pstVencChnAttr->stRcAttr.enRcMode;

    SIZE_S stPicSize;
    stPicSize.u32Width = pChnInfo->u32Width;
    stPicSize.u32Height = pChnInfo->u32Height;

    PIC_SIZE_E enSize = VCTRL_COMM_SYS_GetEnPicSize(&stPicSize);

    if (NULL == pstVencChnAttr) {
        return AR_FAILURE;
    }

    if (VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode) {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval / u32Gop;
    } else {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            pstVencChnAttr->stVencAttr.stAttrH265e.u32CmdQueueDepth = 4;
            if (VENC_RC_MODE_H265CBR == enRcMode) {
                VENC_H265_CBR_S *stH265Cbr = &pstVencChnAttr->stRcAttr.stH265Cbr;
                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stH265Cbr->u32BitRate              = u32BitRate;
                }
                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH265Cbr->u32Gop            = u32Gop;
                }
                stH265Cbr->u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH265Cbr->u32SrcFrameRate   = u32FrameRate; /* input (vi) frame rate */
                    stH265Cbr->fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                }
            } else if (VENC_RC_MODE_H265FIXQP == enRcMode) {
                VENC_H265_FIXQP_S *stH265FixQp = &pstVencChnAttr->stRcAttr.stH265FixQp;

                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH265FixQp->u32Gop              = u32Gop;
                }
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH265FixQp->u32SrcFrameRate     = u32FrameRate;
                    stH265FixQp->fr32DstFrameRate    = u32FrameRate;
                }
                stH265FixQp->u32IQp              = 25;
                stH265FixQp->u32PQp              = 30;
                stH265FixQp->u32BQp              = 32;
            } else if (VENC_RC_MODE_H265VBR == enRcMode) {
                VENC_H265_VBR_S *stH265Vbr = &pstVencChnAttr->stRcAttr.stH265Vbr;


                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stH265Vbr->u32MaxBitRate              = u32BitRate;
                }
                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH265Vbr->u32Gop           = u32Gop;
                }
                stH265Vbr->u32StatTime      = u32StatTime;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH265Vbr->u32SrcFrameRate  = u32FrameRate;
                    stH265Vbr->fr32DstFrameRate = u32FrameRate;
                }
            }
#if 0
            else if(VCTRL_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
#endif
            else if (VENC_RC_MODE_H265QPMAP == enRcMode) {
                VENC_H265_QPMAP_S *stH265QpMap = &pstVencChnAttr->stRcAttr.stH265QpMap;
                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH265QpMap->u32Gop           = u32Gop;
                }
                stH265QpMap->u32StatTime      = u32StatTime;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH265QpMap->u32SrcFrameRate  = u32FrameRate;
                    stH265QpMap->fr32DstFrameRate = u32FrameRate;
                }
                stH265QpMap->enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
            } else {
                VCTRL_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__, enRcMode);
                return AR_FAILURE;
            }
            pstVencChnAttr->stVencAttr.stAttrH265e.bRcnRefShareBuf = AR_FALSE;
        }
        break;
        case PT_H264:
        {
            pstVencChnAttr->stVencAttr.stAttrH264e.u32CmdQueueDepth = 4;
            if (VENC_RC_MODE_H264CBR == enRcMode) {
                VENC_H264_CBR_S *stH264Cbr = &pstVencChnAttr->stRcAttr.stH264Cbr;
                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stH264Cbr->u32BitRate              = u32BitRate;
                }
                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH264Cbr->u32Gop                = u32Gop; /*the interval of IFrame*/
                }
                stH264Cbr->u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH264Cbr->u32SrcFrameRate       = u32FrameRate; /* input (vi) frame rate */
                    stH264Cbr->fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                }
            } else if (VENC_RC_MODE_H264FIXQP == enRcMode) {
                VENC_H264_FIXQP_S   *stH264FixQp = &pstVencChnAttr->stRcAttr.stH264FixQp;

                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH264FixQp->u32Gop           = 30;
                }
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH264FixQp->u32SrcFrameRate  = u32FrameRate;
                    stH264FixQp->fr32DstFrameRate = u32FrameRate;
                }
                stH264FixQp->u32IQp           = 25;
                stH264FixQp->u32PQp           = 30;
                stH264FixQp->u32BQp           = 32;
            } else if (VENC_RC_MODE_H264VBR == enRcMode) {
                VENC_H264_VBR_S   *stH264Vbr = &pstVencChnAttr->stRcAttr.stH264Vbr;

                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stH264Vbr->u32MaxBitRate              = u32BitRate;
                }
                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH264Vbr->u32Gop           = u32Gop;
                }
                stH264Vbr->u32StatTime      = u32StatTime;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH264Vbr->u32SrcFrameRate  = u32FrameRate;
                    stH264Vbr->fr32DstFrameRate = u32FrameRate;
                }
            }
#if 0
            else if (VCTRL_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
#endif
            else if (VENC_RC_MODE_H264QPMAP == enRcMode) {
                VENC_H264_QPMAP_S   *stH264QpMap = &pstVencChnAttr->stRcAttr.stH264QpMap;

                if (pChnInfo->enUpdate & UPDATE_GOPSIZE) {
                    stH264QpMap->u32Gop           = u32Gop;
                }
                stH264QpMap->u32StatTime      = u32StatTime;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stH264QpMap->u32SrcFrameRate  = u32FrameRate;
                    stH264QpMap->fr32DstFrameRate = u32FrameRate;
                }
            } else {
                VCTRL_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return AR_FAILURE;
            }
            pstVencChnAttr->stVencAttr.stAttrH264e.bRcnRefShareBuf = AR_FALSE;
        }
        break;
        case PT_MJPEG:
        {
            if (VENC_RC_MODE_MJPEGFIXQP == enRcMode) {
                VENC_MJPEG_FIXQP_S *stMjpegeFixQp = &pstVencChnAttr->stRcAttr.stMjpegFixQp;
                stMjpegeFixQp->u32Qfactor        = 95;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stMjpegeFixQp->u32SrcFrameRate    = u32FrameRate;
                    stMjpegeFixQp->fr32DstFrameRate   = u32FrameRate;
                }

            } else if (VENC_RC_MODE_MJPEGCBR == enRcMode) {
                VENC_MJPEG_CBR_S *stMjpegeCbr = &pstVencChnAttr->stRcAttr.stMjpegCbr;

                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stMjpegeCbr->u32BitRate              = u32BitRate;
                }
                stMjpegeCbr->u32StatTime         = u32StatTime;
                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stMjpegeCbr->u32SrcFrameRate     = u32FrameRate;
                    stMjpegeCbr->fr32DstFrameRate    = u32FrameRate;
                }
            } else if ((VENC_RC_MODE_MJPEGVBR == enRcMode)) {
                VENC_MJPEG_VBR_S  *stMjpegVbr = &pstVencChnAttr->stRcAttr.stMjpegVbr;

                if (VCTRL_RC_AVBR == enRcMode) {
                    VCTRL_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                if (pChnInfo->enUpdate & UPDATE_BITRATE) {
                    stMjpegVbr->u32MaxBitRate              = u32BitRate;
                }
                stMjpegVbr->u32StatTime      = u32StatTime;

                if (pChnInfo->enUpdate & UPDATE_FPS) {
                    stMjpegVbr->u32SrcFrameRate  = u32FrameRate;
                    stMjpegVbr->fr32DstFrameRate = u32FrameRate;
                }
            } else {
                VCTRL_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return AR_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = AR_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&pstVencChnAttr->stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            VCTRL_PRT("cann't support this enType (%d) in this version!\n",enType);
            return AR_ERR_VENC_NOT_SUPPORT;
    }

    if (PT_MJPEG == enType || PT_JPEG == enType) {
        pstVencChnAttr->stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta = 0;
    } else {
        memcpy(&pstVencChnAttr->stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if ((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode)&&(PT_H264 == enType)) {
            if (0 == pstVencChnAttr->stVencAttr.u32Profile) {
                pstVencChnAttr->stVencAttr.u32Profile = 1;

                VCTRL_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if ((VENC_RC_MODE_H264QPMAP == pstVencChnAttr->stRcAttr.enRcMode)
            || (VENC_RC_MODE_H265QPMAP == pstVencChnAttr->stRcAttr.enRcMode)) {
            if (VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode) {
                pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                VCTRL_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }
    return AR_SUCCESS;
}

/*dynamic change rc attr*/
AR_S32 VCTRL_VENC_SetRcParam(VENC_RC_PARAM_S *pstVencRcParam, VCHN_INFO* pChnInfo, VENC_CHN_ATTR_S *pstVencChnAttr) {

    int minIQp = 0, maxIQp = 0, minPQp = 0, maxPQp = 0, minBQp = 0, maxBQp = 0;
    int minIprop = 0, maxIprop = 0, fixedIQp = 0, fixedPQp = 0, fixedBQp = 0;
    int firstFrameStartQp = 0, minQfactor = 0, maxQfactor = 0;
    PAYLOAD_TYPE_E enType = pstVencChnAttr->stVencAttr.enType;
    VENC_RC_MODE_E enRcMode = pstVencChnAttr->stRcAttr.enRcMode;

    minIQp = pChnInfo->qpChange.enRcUpate & UPDATE_MINIQP;
    maxIQp = pChnInfo->qpChange.enRcUpate & UPDATE_MAXIQP;
    minPQp = pChnInfo->qpChange.enRcUpate & UPDATE_MINPQP;
    maxPQp = pChnInfo->qpChange.enRcUpate & UPDATE_MAXPQP;
    minBQp = pChnInfo->qpChange.enRcUpate & UPDATE_MINBQP;
    maxBQp = pChnInfo->qpChange.enRcUpate & UPDATE_MAXBQP;
    minIprop = pChnInfo->qpChange.enRcUpate & UPDATE_MINIPROP;
    maxIprop = pChnInfo->qpChange.enRcUpate & UPDATE_MAXIPROP;
    fixedIQp = pChnInfo->qpChange.enRcUpate & UPDATE_FIXEDIQP;
    fixedPQp = pChnInfo->qpChange.enRcUpate & UPDATE_FIXEDPQP;
    fixedBQp = pChnInfo->qpChange.enRcUpate & UPDATE_FIXEDBQP;
    firstFrameStartQp = pChnInfo->qpChange.enRcUpate & UPDATE_FIRSTFRAMESTARTQP;
    minQfactor = pChnInfo->qpChange.enRcUpate & UPDATE_MINQFACTOR;
    maxQfactor = pChnInfo->qpChange.enRcUpate & UPDATE_MAXQFACTOR;

    VCTRL_PRT("qpChange rc update 0x%x enType = %d, enRcMode = %d\n", pChnInfo->qpChange.enRcUpate, enType, enRcMode);

    if (firstFrameStartQp) {
        pstVencRcParam->s32FirstFrameStartQp = pChnInfo->qpChange.firstFrameStartQp;
    }

    switch (enType)
    {
        case PT_H265:
        {
            if (VENC_RC_MODE_H265CBR == enRcMode) {
                if (minIQp) {
                    pstVencRcParam->stParamH265Cbr.u32MinIQp = pChnInfo->qpChange.minIQp;
                }
                if (maxIQp) {
                    pstVencRcParam->stParamH265Cbr.u32MaxIQp = pChnInfo->qpChange.maxIQp;
                }
                if (minPQp) {
                    pstVencRcParam->stParamH265Cbr.u32MinPQp = pChnInfo->qpChange.minPQp;
                }
                if (maxPQp) {
                    pstVencRcParam->stParamH265Cbr.u32MaxPQp = pChnInfo->qpChange.maxPQp;
                }
                if (minBQp) {
                    pstVencRcParam->stParamH265Cbr.u32MinBQp = pChnInfo->qpChange.minBQp;
                }
                if (maxBQp) {
                    pstVencRcParam->stParamH265Cbr.u32MaxBQp = pChnInfo->qpChange.maxBQp;
                }
                if (minIprop) {
                    pstVencRcParam->stParamH265Cbr.u32MaxIprop = pChnInfo->qpChange.minIprop;
                }
                if (maxIprop) {
                    pstVencRcParam->stParamH265Cbr.u32MaxIprop = pChnInfo->qpChange.maxIprop;
                }
            } else if (VENC_RC_MODE_H265VBR == enRcMode) {
                if (minIQp) {
                    pstVencRcParam->stParamH265Vbr.u32MinIQp = pChnInfo->qpChange.minIQp;
                }
                if (maxIQp) {
                    pstVencRcParam->stParamH265Vbr.u32MaxIQp = pChnInfo->qpChange.maxIQp;
                }
                if (minPQp) {
                    pstVencRcParam->stParamH265Vbr.u32MinPQp = pChnInfo->qpChange.minPQp;
                }
                if (maxPQp) {
                    pstVencRcParam->stParamH265Vbr.u32MaxPQp = pChnInfo->qpChange.maxPQp;
                }
                if (minBQp) {
                    pstVencRcParam->stParamH265Vbr.u32MinBQp = pChnInfo->qpChange.minBQp;
                }
                if (maxBQp) {
                    pstVencRcParam->stParamH265Vbr.u32MaxBQp = pChnInfo->qpChange.maxBQp;
                }
                if (minIprop) {
                    pstVencRcParam->stParamH265Vbr.u32MaxIprop = pChnInfo->qpChange.minIprop;
                }
                if (maxIprop) {
                    pstVencRcParam->stParamH265Vbr.u32MaxIprop = pChnInfo->qpChange.maxIprop;
                }
            } else if (VENC_RC_MODE_H265FIXQP == enRcMode/* || VENC_RC_MODE_H265QPMAP ==  enRcMode*/) {

                if (fixedIQp) {
                    pstVencRcParam->stParamH265Fixqp.u32IQp = pChnInfo->qpChange.fixedIQp;
                }
                if (fixedPQp) {
                    pstVencRcParam->stParamH265Fixqp.u32PQp = pChnInfo->qpChange.fixedPQp;
                }
                if (fixedPQp) {
                    pstVencRcParam->stParamH265Fixqp.u32BQp = pChnInfo->qpChange.fixedBQp;
                }
            }
        }
        break;
        case PT_H264:
        {
            if (VENC_RC_MODE_H264CBR == enRcMode) {
                if (minIQp) {
                    pstVencRcParam->stParamH264Cbr.u32MinIQp = pChnInfo->qpChange.minIQp;
                }
                if (maxIQp) {
                    pstVencRcParam->stParamH264Cbr.u32MaxIQp = pChnInfo->qpChange.maxIQp;
                }
                if (minPQp) {
                    pstVencRcParam->stParamH264Cbr.u32MinPQp = pChnInfo->qpChange.minPQp;
                }
                if (maxPQp) {
                    pstVencRcParam->stParamH264Cbr.u32MaxPQp = pChnInfo->qpChange.maxPQp;
                }
                if (minBQp) {
                    pstVencRcParam->stParamH264Cbr.u32MinBQp = pChnInfo->qpChange.minBQp;
                }
                if (maxBQp) {
                    pstVencRcParam->stParamH264Cbr.u32MaxBQp = pChnInfo->qpChange.maxBQp;
                }
                if (minIprop) {
                    pstVencRcParam->stParamH264Cbr.u32MaxIprop = pChnInfo->qpChange.minIprop;
                }
                if (maxIprop) {
                    pstVencRcParam->stParamH264Cbr.u32MaxIprop = pChnInfo->qpChange.maxIprop;
                }
            } else if (VENC_RC_MODE_H264VBR == enRcMode) {
                if (minIQp) {
                    pstVencRcParam->stParamH264Vbr.u32MinIQp = pChnInfo->qpChange.minIQp;
                }
                if (maxIQp) {
                    pstVencRcParam->stParamH264Vbr.u32MaxIQp = pChnInfo->qpChange.maxIQp;
                }
                if (minPQp) {
                    pstVencRcParam->stParamH264Vbr.u32MinPQp = pChnInfo->qpChange.minPQp;
                }
                if (maxPQp) {
                    pstVencRcParam->stParamH264Vbr.u32MaxPQp = pChnInfo->qpChange.maxPQp;
                }
                if (minBQp) {
                    pstVencRcParam->stParamH264Vbr.u32MinBQp = pChnInfo->qpChange.minBQp;
                }
                if (maxBQp) {
                    pstVencRcParam->stParamH264Vbr.u32MaxBQp = pChnInfo->qpChange.maxBQp;
                }
                if (minIprop) {
                    pstVencRcParam->stParamH264Vbr.u32MaxIprop = pChnInfo->qpChange.minIprop;
                }
                if (maxIprop) {
                    pstVencRcParam->stParamH264Vbr.u32MaxIprop = pChnInfo->qpChange.maxIprop;
                }
            } else if (VENC_RC_MODE_H264FIXQP == enRcMode/* || VENC_RC_MODE_H264QPMAP ==  enRcMode*/) {

                if (fixedIQp) {
                    pstVencRcParam->stParamH264Fixqp.u32IQp = pChnInfo->qpChange.fixedIQp;
                }
                if (fixedPQp) {
                    pstVencRcParam->stParamH264Fixqp.u32PQp = pChnInfo->qpChange.fixedPQp;
                }
                if (fixedPQp) {
                    pstVencRcParam->stParamH264Fixqp.u32BQp = pChnInfo->qpChange.fixedBQp;
                }
            }
        }
        break;
        case PT_MJPEG:
            if (VENC_RC_MODE_MJPEGCBR == enRcMode) {
                if (minQfactor) {
                    pstVencRcParam->stParamMjpegCbr.u32MinQfactor = pChnInfo->qpChange.minQfactor;
                }
                if (maxQfactor) {
                    pstVencRcParam->stParamMjpegCbr.u32MaxQfactor = pChnInfo->qpChange.maxQfactor;
                }
            } else if (VENC_RC_MODE_MJPEGVBR == enRcMode) {
                if (minQfactor) {
                    pstVencRcParam->stParamMjpegVbr.u32MinQfactor = pChnInfo->qpChange.minQfactor;
                }
                if (maxQfactor) {
                    pstVencRcParam->stParamMjpegVbr.u32MaxQfactor = pChnInfo->qpChange.maxQfactor;
                }

            } else if (VENC_RC_MODE_MJPEGFIXQP == enRcMode) {
            }
            break;
        default:
            break;
    }

    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_InitAttr(VENC_CHN_ATTR_S *pstVencChnAttr, VCHN_INFO* pChnInfo, VENC_GOP_ATTR_S *pstGopAttr)
{
    AR_U32                 u32StatTime;

    AR_U32                 u32Gop = pChnInfo->u32Gop;
    VENC_ATTR_JPEG_S       stJpegAttr;
    AR_U32                 u32FrameRate = pChnInfo->u32FrameRate;
    AR_U32                 u32SrcFrameRate = pChnInfo->u32SrcFrameRate;
    PAYLOAD_TYPE_E enType = PT_BUTT;
    VCTRL_RC_E enRcMode = pChnInfo->enRcMode;

    SIZE_S stPicSize;
    stPicSize.u32Width = pChnInfo->u32Width;
    stPicSize.u32Height = pChnInfo->u32Height;

    PIC_SIZE_E enSize = VCTRL_COMM_SYS_GetEnPicSize(&stPicSize);

    if(NULL == pstVencChnAttr) {
        return AR_FAILURE;
    }

    if (pChnInfo->transcoding) {
        enType = pChnInfo->enTranscodingPayLoad;
        pstVencChnAttr->stVencAttr.enType          = pChnInfo->enTranscodingPayLoad;
    } else {
        enType = pChnInfo->enPayLoad;
        pstVencChnAttr->stVencAttr.enType          = pChnInfo->enPayLoad;
    }
    pstVencChnAttr->stVencAttr.u32MaxPicWidth  = pChnInfo->u32Width;
    pstVencChnAttr->stVencAttr.u32MaxPicHeight = pChnInfo->u32Height;
    pstVencChnAttr->stVencAttr.u32PicWidth     = pChnInfo->u32Width;/*the picture width*/
    pstVencChnAttr->stVencAttr.u32PicHeight    = pChnInfo->u32Height;/*the picture height*/
    pstVencChnAttr->stVencAttr.u32BufSize      = pChnInfo->u32Width * pChnInfo->u32Height * 2;/*stream buffer size*/
    pstVencChnAttr->stVencAttr.u32Profile      = pChnInfo->u32Profile;
    pstVencChnAttr->stVencAttr.bByFrame        = AR_TRUE;/*get stream mode is slice mode or frame mode?*/

    //pstVencChnAttr->stRcAttr.enRcMode = pChnInfo->enRcMode;
    pstVencChnAttr->stRcAttr.stH265Cbr.u32SrcFrameRate = u32SrcFrameRate;
    pstVencChnAttr->stRcAttr.stH265Cbr.u32BitRate = pChnInfo->u32BitRate;
    pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta = -2;

    if(VENC_GOPMODE_SMARTP == pstGopAttr->enGopMode) {
        u32StatTime = pstGopAttr->stSmartP.u32BgInterval/u32Gop;
    } else {
        u32StatTime = 1;
    }

    switch (enType)
    {
        case PT_H265:
        {
            pstVencChnAttr->stVencAttr.stAttrH265e.u32CmdQueueDepth = 4;
            if (VCTRL_RC_CBR == enRcMode) {
                VENC_H265_CBR_S stH265Cbr = { 0 };
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                stH265Cbr.u32Gop            = u32Gop;
                stH265Cbr.u32StatTime       = u32StatTime; /* stream rate statics time(s) */
                stH265Cbr.u32SrcFrameRate   = u32SrcFrameRate; /* input (vi) frame rate */
                stH265Cbr.fr32DstFrameRate  = u32FrameRate; /* target frame rate */
                if (pChnInfo->u32BitRate != 0) {
                    stH265Cbr.u32BitRate = pChnInfo->u32BitRate;
                } else {
                    stH265Cbr.u32BitRate = (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stH265Cbr.u32BitRate = 1024 * 2 + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stH265Cbr.u32BitRate = 1024 * 2 + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stH265Cbr.u32BitRate = 1024 * 3 + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stH265Cbr.u32BitRate = 1024 * 5  + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stH265Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stH265Cbr.u32BitRate = 1024 * 20 + 5120*u32FrameRate/30;
                            break;
                        default :
                            stH265Cbr.u32BitRate = 1024 * 15 + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH265Cbr, &stH265Cbr, sizeof(VENC_H265_CBR_S));
            } else if (VCTRL_RC_FIXQP == enRcMode) {
                VENC_H265_FIXQP_S stH265FixQp = { 0 };

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
                stH265FixQp.u32Gop              = u32Gop;
                stH265FixQp.u32SrcFrameRate     = u32SrcFrameRate;
                stH265FixQp.fr32DstFrameRate    = u32FrameRate;
                stH265FixQp.u32IQp              = 25;
                stH265FixQp.u32PQp              = 30;
                stH265FixQp.u32BQp              = 32;
                memcpy(&pstVencChnAttr->stRcAttr.stH265FixQp, &stH265FixQp, sizeof(VENC_H265_FIXQP_S));
            } else if (VCTRL_RC_VBR == enRcMode) {
                VENC_H265_VBR_S stH265Vbr = { 0 };

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
                stH265Vbr.u32Gop           = u32Gop;
                stH265Vbr.u32StatTime      = u32StatTime;
                stH265Vbr.u32SrcFrameRate  = u32SrcFrameRate;
                stH265Vbr.fr32DstFrameRate = u32FrameRate;
                if (pChnInfo->u32BitRate != 0) {
                    stH265Vbr.u32MaxBitRate = pChnInfo->u32BitRate;
                } else {
                    stH265Vbr.u32MaxBitRate= (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stH265Vbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stH265Vbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stH265Vbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stH265Vbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stH265Vbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stH265Vbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                            break;
                        default :
                            stH265Vbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH265Vbr, &stH265Vbr, sizeof(VENC_H265_VBR_S));
            }
#if 0
            else if(VCTRL_RC_AVBR == enRcMode)
            {
                VENC_H265_AVBR_S    stH265AVbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
                stH265AVbr.u32Gop         = u32Gop;
                stH265AVbr.u32StatTime    = u32StatTime;
                stH265AVbr.u32SrcFrameRate  = u32FrameRate;
                stH265AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH265AVbr.u32MaxBitRate = 1024 * 2 + 2048*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH265AVbr.u32MaxBitRate = 1024 * 3 + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH265AVbr.u32MaxBitRate = 1024 * 5  + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH265AVbr.u32MaxBitRate = 1024 * 10 + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH265AVbr.u32MaxBitRate = 1024 * 20 + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH265AVbr.u32MaxBitRate    = 1024 * 15 + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH265AVbr, &stH265AVbr, sizeof(VENC_H265_AVBR_S));
            }
#endif
            else if(VCTRL_RC_QPMAP == enRcMode) {
                VENC_H265_QPMAP_S  stH265QpMap = { 0 };
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H265QPMAP;
                stH265QpMap.u32Gop           = u32Gop;
                stH265QpMap.u32StatTime      = u32StatTime;
                stH265QpMap.u32SrcFrameRate  = u32SrcFrameRate;
                stH265QpMap.fr32DstFrameRate = u32FrameRate;
                stH265QpMap.enQpMapMode      = VENC_RC_QPMAP_MODE_MEANQP;
                memcpy(&pstVencChnAttr->stRcAttr.stH265QpMap, &stH265QpMap, sizeof(VENC_H265_QPMAP_S));
            } else {
                VCTRL_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return AR_FAILURE;
            }
            pstVencChnAttr->stVencAttr.stAttrH265e.bRcnRefShareBuf = AR_FALSE;
        }
        break;
        case PT_H264:
        {
            pstVencChnAttr->stVencAttr.stAttrH264e.u32CmdQueueDepth = 4;
            if (VCTRL_RC_CBR == enRcMode) {
                VENC_H264_CBR_S  stH264Cbr = { 0 };
                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
                stH264Cbr.u32Gop                = u32Gop; /*the interval of IFrame*/
                stH264Cbr.u32StatTime           = u32StatTime; /* stream rate statics time(s) */
                stH264Cbr.u32SrcFrameRate       = u32SrcFrameRate; /* input (vi) frame rate */
                stH264Cbr.fr32DstFrameRate      = u32FrameRate; /* target frame rate */
                if (pChnInfo->u32BitRate != 0) {
                    stH264Cbr.u32BitRate = pChnInfo->u32BitRate;
                } else {
                    stH264Cbr.u32BitRate = (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stH264Cbr.u32BitRate = 1024 * 2  + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stH264Cbr.u32BitRate = 1024 * 2  + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stH264Cbr.u32BitRate = 1024 * 3  + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stH264Cbr.u32BitRate = 1024 * 5 + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stH264Cbr.u32BitRate = 1024 * 12 + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stH264Cbr.u32BitRate = 1024 * 10 + 5120*u32FrameRate/30;
                            break;
                        default :
                            stH264Cbr.u32BitRate = 1024 * 15 + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }

                memcpy(&pstVencChnAttr->stRcAttr.stH264Cbr, &stH264Cbr, sizeof(VENC_H264_CBR_S));
            } else if (VCTRL_RC_FIXQP == enRcMode) {
                VENC_H264_FIXQP_S    stH264FixQp;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
                stH264FixQp.u32Gop           = u32Gop;
                stH264FixQp.u32SrcFrameRate  = u32SrcFrameRate;
                stH264FixQp.fr32DstFrameRate = u32FrameRate;
                stH264FixQp.u32IQp           = 25;
                stH264FixQp.u32PQp           = 30;
                stH264FixQp.u32BQp           = 32;
                memcpy(&pstVencChnAttr->stRcAttr.stH264FixQp, &stH264FixQp, sizeof(VENC_H264_FIXQP_S));
            } else if (VCTRL_RC_VBR == enRcMode) {
                VENC_H264_VBR_S    stH264Vbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
                stH264Vbr.u32Gop           = u32Gop;
                stH264Vbr.u32StatTime      = u32StatTime;
                stH264Vbr.u32SrcFrameRate  = u32SrcFrameRate;
                stH264Vbr.fr32DstFrameRate = u32FrameRate;
                if (pChnInfo->u32BitRate != 0) {
                    stH264Vbr.u32MaxBitRate = pChnInfo->u32BitRate;
                } else {
                    stH264Vbr.u32MaxBitRate = (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stH264Vbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stH264Vbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stH264Vbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stH264Vbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stH264Vbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stH264Vbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                            break;
                        default :
                            stH264Vbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH264Vbr, &stH264Vbr, sizeof(VENC_H264_VBR_S));
            }
#if 0
            else if (VCTRL_RC_AVBR == enRcMode)
            {
                VENC_H264_VBR_S    stH264AVbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
                stH264AVbr.u32Gop           = u32Gop;
                stH264AVbr.u32StatTime      = u32StatTime;
                stH264AVbr.u32SrcFrameRate  = u32FrameRate;
                stH264AVbr.fr32DstFrameRate = u32FrameRate;
                switch (enSize)
                {
                    case PIC_720P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 1024*u32FrameRate/30;
                        break;
                    case PIC_1080P:
                        stH264AVbr.u32MaxBitRate = 1024 * 2   + 2048*u32FrameRate/30;
                        break;
                    case PIC_2592x1944:
                        stH264AVbr.u32MaxBitRate = 1024 * 3   + 3072*u32FrameRate/30;
                        break;
                    case PIC_3840x2160:
                        stH264AVbr.u32MaxBitRate = 1024 * 5   + 5120*u32FrameRate/30;
                        break;
                    case PIC_4000x3000:
                        stH264AVbr.u32MaxBitRate = 1024 * 10  + 5120*u32FrameRate/30;
                        break;
                    case PIC_7680x4320:
                        stH264AVbr.u32MaxBitRate = 1024 * 20  + 5120*u32FrameRate/30;
                        break;
                    default :
                        stH264AVbr.u32MaxBitRate = 1024 * 15  + 2048*u32FrameRate/30;
                        break;
                }
                memcpy(&pstVencChnAttr->stRcAttr.stH264AVbr, &stH264AVbr, sizeof(VENC_H264_AVBR_S));
            }
#endif
            else if(VCTRL_RC_QPMAP == enRcMode) {
                VENC_H264_QPMAP_S    stH264QpMap;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_H264QPMAP;
                stH264QpMap.u32Gop           = u32Gop;
                stH264QpMap.u32StatTime      = u32StatTime;
                stH264QpMap.u32SrcFrameRate  = u32SrcFrameRate;
                stH264QpMap.fr32DstFrameRate = u32FrameRate;
                memcpy(&pstVencChnAttr->stRcAttr.stH264QpMap, &stH264QpMap, sizeof(VENC_H264_QPMAP_S));
            } else {
                VCTRL_PRT("%s,%d,enRcMode(%d) not support\n",__FUNCTION__,__LINE__,enRcMode);
                return AR_FAILURE;
            }
            pstVencChnAttr->stVencAttr.stAttrH264e.bRcnRefShareBuf = AR_FALSE;
        }
        break;
        case PT_MJPEG:
        {
            if (VCTRL_RC_FIXQP == enRcMode) {
                VENC_MJPEG_FIXQP_S stMjpegeFixQp;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
                stMjpegeFixQp.u32Qfactor        = 20;
                stMjpegeFixQp.u32SrcFrameRate    = u32SrcFrameRate;
                stMjpegeFixQp.fr32DstFrameRate   = u32FrameRate;

                memcpy(&pstVencChnAttr->stRcAttr.stMjpegFixQp, &stMjpegeFixQp,sizeof(VENC_MJPEG_FIXQP_S));
            } else if (VCTRL_RC_CBR == enRcMode) {
                VENC_MJPEG_CBR_S stMjpegeCbr;

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
                stMjpegeCbr.u32StatTime         = u32StatTime;
                stMjpegeCbr.u32SrcFrameRate     = u32SrcFrameRate;
                stMjpegeCbr.fr32DstFrameRate    = u32FrameRate;
                if (pChnInfo->u32BitRate != 0) {
                    stMjpegeCbr.u32BitRate = pChnInfo->u32BitRate;
                } else {
                    stMjpegeCbr.u32BitRate = (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stMjpegeCbr.u32BitRate = 1024 * 5  + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stMjpegeCbr.u32BitRate = 1024 * 8  + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stMjpegeCbr.u32BitRate = 1024 * 20 + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stMjpegeCbr.u32BitRate = 1024 * 25 + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stMjpegeCbr.u32BitRate = 1024 * 30 + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stMjpegeCbr.u32BitRate = 1024 * 40 + 5120*u32FrameRate/30;
                            break;
                        default :
                            stMjpegeCbr.u32BitRate = 1024 * 20 + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }

                memcpy(&pstVencChnAttr->stRcAttr.stMjpegCbr, &stMjpegeCbr,sizeof(VENC_MJPEG_CBR_S));
            } else if ((VCTRL_RC_VBR == enRcMode) ||(VCTRL_RC_AVBR == enRcMode)) {
                VENC_MJPEG_VBR_S   stMjpegVbr;

                if(VCTRL_RC_AVBR == enRcMode) {
                    VCTRL_PRT("Mjpege not support AVBR, so change rcmode to VBR!\n");
                }

                pstVencChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
                stMjpegVbr.u32StatTime      = u32StatTime;
                stMjpegVbr.u32SrcFrameRate  = u32SrcFrameRate;
                stMjpegVbr.fr32DstFrameRate = 5;

                if (pChnInfo->u32BitRate != 0) {
                    stMjpegVbr.u32MaxBitRate = pChnInfo->u32BitRate;
                } else {
                    stMjpegVbr.u32MaxBitRate = (pstVencChnAttr->stVencAttr.u32BufSize * 8 / 10) / 1000;
                    /*
                    switch (enSize)
                    {
                        case PIC_720P:
                            stMjpegVbr.u32MaxBitRate = 1024 * 5 + 1024*u32FrameRate/30;
                            break;
                        case PIC_1080P:
                            stMjpegVbr.u32MaxBitRate = 1024 * 8 + 2048*u32FrameRate/30;
                            break;
                        case PIC_2592x1944:
                            stMjpegVbr.u32MaxBitRate = 1024 * 20 + 3072*u32FrameRate/30;
                            break;
                        case PIC_3840x2160:
                            stMjpegVbr.u32MaxBitRate = 1024 * 25 + 5120*u32FrameRate/30;
                            break;
                        case PIC_4000x3000:
                            stMjpegVbr.u32MaxBitRate    = 1024 * 30 + 5120*u32FrameRate/30;
                            break;
                        case PIC_7680x4320:
                            stMjpegVbr.u32MaxBitRate = 1024 * 40 + 5120*u32FrameRate/30;
                            break;
                        default :
                            stMjpegVbr.u32MaxBitRate = 1024 * 20 + 2048*u32FrameRate/30;
                            break;
                    }
                    */
                }

                memcpy(&pstVencChnAttr->stRcAttr.stMjpegVbr, &stMjpegVbr,sizeof(VENC_MJPEG_VBR_S));
            } else {
                VCTRL_PRT("cann't support other mode(%d) in this version!\n",enRcMode);
                return AR_FAILURE;
            }
        }
        break;

        case PT_JPEG:
            stJpegAttr.bSupportDCF     = AR_FALSE;
            stJpegAttr.stMPFCfg.u8LargeThumbNailNum = 0;
            stJpegAttr.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;
            memcpy(&pstVencChnAttr->stVencAttr.stAttrJpege, &stJpegAttr, sizeof(VENC_ATTR_JPEG_S));
            break;
        default:
            VCTRL_PRT("cann't support this enType (%d) in this version!\n", enType);
            return AR_ERR_VENC_NOT_SUPPORT;
    }

    if (PT_MJPEG == enType || PT_JPEG == enType) {
        pstVencChnAttr->stGopAttr.enGopMode  = VENC_GOPMODE_NORMALP;
        pstVencChnAttr->stGopAttr.stNormalP.s32IPQpDelta = 0;
    } else {
        memcpy(&pstVencChnAttr->stGopAttr,pstGopAttr,sizeof(VENC_GOP_ATTR_S));
        if ((VENC_GOPMODE_BIPREDB == pstGopAttr->enGopMode) && (PT_H264 == enType)) {
            if (0 == pstVencChnAttr->stVencAttr.u32Profile) {
                pstVencChnAttr->stVencAttr.u32Profile = 1;

                VCTRL_PRT("H.264 base profile not support BIPREDB, so change profile to main profile!\n");
            }
        }

        if ((VENC_RC_MODE_H264QPMAP == pstVencChnAttr->stRcAttr.enRcMode) ||
            (VENC_RC_MODE_H265QPMAP == pstVencChnAttr->stRcAttr.enRcMode)) {

            if (VENC_GOPMODE_ADVSMARTP == pstGopAttr->enGopMode) {
                pstVencChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_SMARTP;

                VCTRL_PRT("advsmartp not support QPMAP, so change gopmode to smartp!\n");
            }
        }
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_Creat(VCHN_INFO* pChnInfo, VENC_GOP_ATTR_S *pstGopAttr)
{
    AR_S32 s32Ret;
    VENC_CHN_ATTR_S        stVencChnAttr = {0};
    VENC_CHN VencChn = pChnInfo->chnId;

    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    s32Ret = VCTRL_VENC_InitAttr(&stVencChnAttr, pChnInfo, pstGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("VCTRL_VENC_InitAttr[%d] faild with %#x! ===\n", VencChn, s32Ret);
        return s32Ret;
    }

    VCTRL_Dump_VencChnAttr(&stVencChnAttr);
    s32Ret = AR_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_CreateChn [%d] faild with %#x! ===\n", VencChn, s32Ret);
        return s32Ret;
    }

    return AR_SUCCESS;
}


/******************************************************************************
* funciton : Start venc stream mode
* note      : rate control parameter need adjust, according your case.
******************************************************************************/
AR_S32 VCTRL_VENC_Start(VCHN_INFO* pChnInfo, VENC_GOP_ATTR_S *pstGopAttr)
{
    AR_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam = {0};
    VENC_CHN VencChn = pChnInfo->chnId;

    /******************************************
     step 1:  Creat Encode Chnl
    ******************************************/
    s32Ret = VCTRL_VENC_Creat(pChnInfo, pstGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("VCTRL_VENC_Creat faild with%#x! \n", s32Ret);
        return AR_FAILURE;
    }
    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/

    stRecvParam.s32RecvPicNum = -1;
    s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn,&stRecvParam);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_StartRecvPic faild with%#x! \n", s32Ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : Stop venc ( stream mode -- H264, MJPEG )
******************************************************************************/
AR_S32 VCTRL_VENC_Stop(VENC_CHN VencChn)
{
    AR_S32 s32Ret;
    /******************************************
     step 1:  Stop Recv Pictures
    ******************************************/
    s32Ret = AR_MPI_VENC_StopRecvFrame(VencChn);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return AR_FAILURE;
    }

    /******************************************
     step 2:  Distroy Venc Channel
    ******************************************/
    s32Ret = AR_MPI_VENC_DestroyChn(VencChn);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : Start snap
******************************************************************************/

AR_S32 VCTRL_VENC_SnapStart(VENC_CHN VencChn, SIZE_S* pstSize, AR_BOOL bSupportDCF)
{
    AR_S32 s32Ret;
    VENC_CHN_ATTR_S stVencChnAttr;
    /******************************************
     step 1:  Create Venc Channel
    ******************************************/
    stVencChnAttr.stVencAttr.enType = PT_JPEG;
    stVencChnAttr.stVencAttr.u32MaxPicWidth     = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32MaxPicHeight    = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32PicWidth        = pstSize->u32Width;
    stVencChnAttr.stVencAttr.u32PicHeight       = pstSize->u32Height;
    stVencChnAttr.stVencAttr.u32BufSize         = pstSize->u32Width * pstSize->u32Height * 2;
    stVencChnAttr.stVencAttr.bByFrame           = AR_TRUE;/*get stream mode is field mode  or frame mode*/
    stVencChnAttr.stVencAttr.stAttrJpege.bSupportDCF = bSupportDCF;
    //stVencChnAttr.stVencAttr.stAttrJpege.bSupportXMP = AR_FALSE;
    stVencChnAttr.stVencAttr.stAttrJpege.stMPFCfg.u8LargeThumbNailNum = 0;
    stVencChnAttr.stVencAttr.stAttrJpege.enReceiveMode = VENC_PIC_RECEIVE_SINGLE;

    s32Ret = AR_MPI_VENC_CreateChn(VencChn, &stVencChnAttr);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VENC_CreateChn [%d] faild with %#x!\n", \
                   VencChn, s32Ret);
        return s32Ret;
    }
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : Stop snap
******************************************************************************/
AR_S32 VCTRL_VENC_SnapStop(VENC_CHN VencChn)
{
    AR_S32 s32Ret;
    s32Ret = AR_MPI_VENC_StopRecvFrame(VencChn);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VENC_StopRecvPic vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return AR_FAILURE;
    }
    s32Ret = AR_MPI_VENC_DestroyChn(VencChn);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VENC_DestroyChn vechn[%d] failed with %#x!\n", VencChn, s32Ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

/******************************************************************************
* funciton : snap process
******************************************************************************/

AR_S32 VCTRL_VENC_SnapProcess(VENC_CHN VencChn, AR_U32 SnapCnt, AR_BOOL bSaveJpg, AR_BOOL bSaveThm)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    AR_S32 s32VencFd;
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    AR_S32 s32Ret;
    VENC_RECV_PIC_PARAM_S  stRecvParam;
    AR_U32 i;
    AR_BOOL mpp_service_is_lib = ar_hal_sys_mpp_service_is_lib();


    /******************************************
     step 2:  Start Recv Venc Pictures
    ******************************************/
    stRecvParam.s32RecvPicNum = SnapCnt;
    s32Ret = AR_MPI_VENC_StartRecvFrame(VencChn, &stRecvParam);
    if (AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VENC_StartRecvPic faild with%#x!\n", s32Ret);
        return AR_FAILURE;
    }
    /******************************************
     step 3:  recv picture
    ******************************************/
    s32VencFd = AR_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        VCTRL_PRT("AR_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return AR_FAILURE;
    }

    for(i=0; i<SnapCnt; i++)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32VencFd, &read_fds);
        TimeoutVal.tv_sec  = 10;
        TimeoutVal.tv_usec = 0;

        if(mpp_service_is_lib)
            s32Ret = 1;
        else
            s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if (s32Ret < 0)
        {
            VCTRL_PRT("snap select failed!\n");
            return AR_FAILURE;
        }
        else if (0 == s32Ret)
        {
            VCTRL_PRT("snap time out!\n");
            return AR_FAILURE;
        }
        else
        {
            if (!mpp_service_is_lib || FD_ISSET(s32VencFd, &read_fds))
            {
                while(1) {
                    memset(&stStat, 0, sizeof(VENC_CHN_STATUS_S));
                    s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stStat);
                    if (s32Ret != AR_SUCCESS)
                    {
                        VCTRL_PRT("AR_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }
                    /*******************************************************
                    suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {                VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                        return AR_SUCCESS;
                     }
                     *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                        /*
                        return AR_SUCCESS;
                        */
                        continue;
                    }
                    if (stStat.u32LeftStreamFrames > 0)
                        break;
                }

                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    VCTRL_PRT("malloc memory failed!\n");
                    return AR_FAILURE;
                }
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = AR_MPI_VENC_GetStream(VencChn, &stStream, -1);
                if (AR_SUCCESS != s32Ret)
                {
                    VCTRL_PRT("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    return AR_FAILURE;
                }
                if(bSaveJpg || bSaveThm)
                {
                    char acFile[FILE_NAME_LEN]    = {0};
                    FILE* pFile;

                    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
                    pFile = fopen(acFile, "wb");
                    if (pFile == NULL)
                    {
                        VCTRL_PRT("open file err\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        return AR_FAILURE;
                    }

                    s32Ret = VCTRL_VENC_SaveStream(pFile, &stStream);
                    if (AR_SUCCESS != s32Ret)
                    {
                        VCTRL_PRT("save snap picture failed!\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;

                        fclose(pFile);
                        return AR_FAILURE;
                    }

                    if(bSaveThm)
                    {
                        char acFile_dcf[FILE_NAME_LEN]  = {0};
                        snprintf(acFile_dcf, FILE_NAME_LEN, "snap_thm_%d.jpg", gs_s32SnapCnt);
                        s32Ret = VCTRL_VENC_Getdcfinfo(acFile, acFile_dcf);
                        if (AR_SUCCESS != s32Ret)
                        {
                            VCTRL_PRT("save thm picture failed!\n");

                            free(stStream.pstPack);
                            stStream.pstPack = NULL;

                            fclose(pFile);
                            return AR_FAILURE;
                        }
                    }

                    fclose(pFile);
                    gs_s32SnapCnt++;
                }

                s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (AR_SUCCESS != s32Ret)
                {
                    VCTRL_PRT("AR_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;

                    return AR_FAILURE;
                }

                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }
    /******************************************
     step 4:  stop recv picture
    ******************************************/
    s32Ret = AR_MPI_VENC_StopRecvFrame(VencChn);
    if (s32Ret != AR_SUCCESS)
    {
        VCTRL_PRT("AR_MPI_VENC_StopRecvPic failed with %#x!\n",  s32Ret);
        return AR_FAILURE;
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_SaveJpeg(VENC_CHN VencChn, AR_U32 SnapCnt)
{
    struct timeval TimeoutVal;
    fd_set read_fds;
    AR_S32 s32VencFd;
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    AR_S32 s32Ret;
    AR_U32 i;
    AR_BOOL mpp_service_is_lib = ar_hal_sys_mpp_service_is_lib();

    s32VencFd = AR_MPI_VENC_GetFd(VencChn);
    if (s32VencFd < 0)
    {
        VCTRL_PRT("AR_MPI_VENC_GetFd faild with%#x!\n", s32VencFd);
        return AR_FAILURE;
    }

    for(i=0; i<SnapCnt; i++)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32VencFd, &read_fds);
        TimeoutVal.tv_sec  = 10;
        TimeoutVal.tv_usec = 0;
        if(mpp_service_is_lib)
            s32Ret = 1;
        else
            s32Ret = select(s32VencFd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if (s32Ret < 0)
        {
            VCTRL_PRT("snap select failed!\n");
            return AR_FAILURE;
        }
        else if (0 == s32Ret)
        {
            VCTRL_PRT("snap time out!\n");
            return AR_FAILURE;
        }
        else
        {
            if (!mpp_service_is_lib || FD_ISSET(s32VencFd, &read_fds))
            {
                while (1) {
                    memset(&stStat, 0, sizeof(VENC_CHN_STATUS_S));
                    s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stStat);
                    if (s32Ret != AR_SUCCESS)
                    {
                        VCTRL_PRT("AR_MPI_VENC_QueryStatus failed with %#x!\n", s32Ret);
                        return AR_FAILURE;
                    }
                    /*******************************************************
                    suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {                VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                        return AR_SUCCESS;
                     }
                     *******************************************************/
                    if (0 == stStat.u32CurPacks)
                    {
                        VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                        return AR_SUCCESS;
                    }
                    if (stStat.u32LeftStreamFrames > 0)
                        break;
                }
                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack)
                {
                    VCTRL_PRT("malloc memory failed!\n");
                    return AR_FAILURE;
                }
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = AR_MPI_VENC_GetStream(VencChn, &stStream, -1);
                if (AR_SUCCESS != s32Ret)
                {
                    VCTRL_PRT("AR_MPI_VENC_GetStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    return AR_FAILURE;
                }
                if(1)
                {
                    char acFile[FILE_NAME_LEN]    = {0};
                    FILE* pFile;

                    snprintf(acFile, FILE_NAME_LEN, "snap_%d.jpg", gs_s32SnapCnt);
                    pFile = fopen(acFile, "wb");
                    if (pFile == NULL)
                    {
                        VCTRL_PRT("open file err\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        return AR_FAILURE;
                    }

                    s32Ret = VCTRL_VENC_SaveStream(pFile, &stStream);
                    if (AR_SUCCESS != s32Ret)
                    {
                        VCTRL_PRT("save snap picture failed!\n");

                        free(stStream.pstPack);
                        stStream.pstPack = NULL;

                        fclose(pFile);
                        return AR_FAILURE;
                    }

                    fclose(pFile);
                    gs_s32SnapCnt++;
                }

                s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (AR_SUCCESS != s32Ret)
                {
                    VCTRL_PRT("AR_MPI_VENC_ReleaseStream failed with %#x!\n", s32Ret);

                    free(stStream.pstPack);
                    stStream.pstPack = NULL;

                    return AR_FAILURE;
                }

                free(stStream.pstPack);
                stStream.pstPack = NULL;
            }
        }
    }

    return AR_SUCCESS;
}

#define QpMapBufNum 8
AR_VOID* VCTRL_QpmapSendFrameProc(AR_VOID* p)
{
    AR_U32 i,j,VeChnCnt;
    AR_S32 s32Ret;
    VIDEO_FRAME_INFO_S  *pstVideoFrame;
    USER_FRAME_INFO_S   stFrame[QpMapBufNum];
    VCTRL_VENC_QPMAP_SENDFRAME_PARA_S *pstPara;
    //AR_U32 u32AlignPicHeight ;
    //AR_U32 u32AlignPicWidth  ;

    AR_U32   u32QpMapSize;
    AR_U64   u64QpMapPhyAddr[QpMapBufNum];
    AR_VOID* pQpMapVirAddr[QpMapBufNum];
    AR_U32 u32QpMapSizeHeight ;
    AR_U32 u32QpMapSizeWidth  ;
    AR_U8 *pVirAddr;
    AR_U64 u64PhyAddr;
    AR_U8 *pVirAddrTemp;

    AR_U32   u32SkipWeightHeight_H264 ;
    AR_U32   u32SkipWeightWidth_H264  ;
    AR_U32   u32SkipWeightSize_H264;
    AR_U64   u64SkipWeightPhyAddr_H264[QpMapBufNum];
    AR_VOID* pSkipWeightVirAddr_H264[QpMapBufNum];

    AR_U32   u32SkipWeightHeight_H265 ;
    AR_U32   u32SkipWeightWidth_H265  ;
    AR_U32   u32SkipWeightSize_H265;
    AR_U64   u64SkipWeightPhyAddr_H265[QpMapBufNum];
    AR_VOID* pSkipWeightVirAddr_H265[QpMapBufNum];

    VPSS_CHN_ATTR_S  stChnAttr;

    pstPara = (VCTRL_VENC_QPMAP_SENDFRAME_PARA_S*)p;

    //u32AlignPicWidth  = (pstPara->stSize.u32Height + 15)/16;
    //u32AlignPicHeight = (pstPara->stSize.u32Height + 15)/16;

    /* qpmap */
    u32QpMapSizeWidth  = (pstPara->stSize.u32Width + 511)/512 *32;
    u32QpMapSizeHeight = (pstPara->stSize.u32Height + 15)/16;
    u32QpMapSize      = u32QpMapSizeWidth * u32QpMapSizeHeight;
    s32Ret = AR_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, AR_NULL, u32QpMapSize*QpMapBufNum);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        return NULL;
    }

    for(i=0; i<QpMapBufNum; i++)
    {
        u64QpMapPhyAddr[i] = u64PhyAddr + i*u32QpMapSize;
        pQpMapVirAddr[i]   = pVirAddr + i*u32QpMapSize;
    }

    /* skipweight h.264 */
    u32SkipWeightWidth_H264  = (pstPara->stSize.u32Width + 511)/512 *16;
    u32SkipWeightHeight_H264 = (pstPara->stSize.u32Height + 15)/16;
    u32SkipWeightSize_H264   = u32SkipWeightWidth_H264*u32SkipWeightHeight_H264;
    s32Ret = AR_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, AR_NULL, u32SkipWeightSize_H264*QpMapBufNum);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        AR_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
        return NULL;
    }

    for(i=0; i<QpMapBufNum; i++)
    {
        u64SkipWeightPhyAddr_H264[i] = u64PhyAddr + i*u32SkipWeightSize_H264;
        pSkipWeightVirAddr_H264[i]   = pVirAddr + i*u32SkipWeightSize_H264;
    }

    /* skipweight h.265 */
    u32SkipWeightWidth_H265  = (pstPara->stSize.u32Width + 2047)/2048 *16;
    u32SkipWeightHeight_H265 = (pstPara->stSize.u32Height + 63)/64;
    u32SkipWeightSize_H265   = u32SkipWeightWidth_H265*u32SkipWeightHeight_H265;
    s32Ret = AR_MPI_SYS_MmzAlloc(&u64PhyAddr, (void**)&pVirAddr, NULL, AR_NULL, u32SkipWeightSize_H265*QpMapBufNum);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzAlloc err:0x%x",s32Ret);
        AR_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
        AR_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H264[0],pSkipWeightVirAddr_H264[0]);
        return NULL;
    }
    for(i=0; i<QpMapBufNum; i++)
    {
        u64SkipWeightPhyAddr_H265[i] = u64PhyAddr + i*u32SkipWeightSize_H265;
        pSkipWeightVirAddr_H265[i]   = pVirAddr + i*u32SkipWeightSize_H265;
    }


    s32Ret = AR_MPI_VPSS_GetChnAttr(pstPara->VpssGrp,pstPara->VpssChn,&stChnAttr);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VPSS_GetChnAttr err:0x%x",s32Ret);

        return NULL;
    }

    stChnAttr.u32Depth = 3;
    s32Ret = AR_MPI_VPSS_SetChnAttr(pstPara->VpssGrp,pstPara->VpssChn,&stChnAttr);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_VPSS_SetChnAttr err:0x%x",s32Ret);

        return NULL;
    }

    i=0;
    while(AR_TRUE == pstPara->bThreadStart)
    {
        pstVideoFrame = &stFrame[i].stUserFrame;
        s32Ret = AR_MPI_VPSS_GetChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame,1000);
        if(AR_SUCCESS != s32Ret)
        {
            VCTRL_PRT("AR_MPI_VPSS_GetChnFrame err:0x%x\n",s32Ret);
            continue;
        }

        pVirAddrTemp = (AR_U8 *)pQpMapVirAddr[i];
        for(j=0; j<u32QpMapSize; j++)
        {
            *pVirAddrTemp = 0x5E;
            pVirAddrTemp++;
        }

        pVirAddrTemp = (AR_U8 *)pSkipWeightVirAddr_H264[i];
        for(j=0; j<u32SkipWeightSize_H264; j++)
        {
            *pVirAddrTemp = 0x88;
            pVirAddrTemp++;
        }

        pVirAddrTemp = (AR_U8 *)pSkipWeightVirAddr_H265[i];
        for(j=0; j<u32SkipWeightSize_H265; j++)
        {
            *pVirAddrTemp = 0x88;
            pVirAddrTemp++;
        }

        for(VeChnCnt=0; VeChnCnt<pstPara->s32Cnt; VeChnCnt++)
        {
            VENC_CHN_ATTR_S stChnAttr;
            AR_MPI_VENC_GetChnAttr(pstPara->VeChn[VeChnCnt],&stChnAttr);
            if(PT_H264 ==stChnAttr.stVencAttr.enType)
            {
                //stFrame[i].stUserRcInfo.bSkipWeightValid = 1;
                //stFrame[i].stUserRcInfo.u64SkipWeightPhyAddr = u64SkipWeightPhyAddr_H264[i];
            }
            else if(PT_H265 ==stChnAttr.stVencAttr.enType)
            {
                //stFrame[i].stUserRcInfo.bSkipWeightValid = 1;
                //stFrame[i].stUserRcInfo.u64SkipWeightPhyAddr = u64SkipWeightPhyAddr_H265[i];
            }
            else
            {
                continue;
            }

            stFrame[i].stUserRcInfo.bQpMapValid     = 1;
            stFrame[i].stUserRcInfo.u64QpMapPhyAddr = u64QpMapPhyAddr[i];
            stFrame[i].stUserRcInfo.u32BlkStartQp   = 30;
            stFrame[i].stUserRcInfo.enFrameType = VENC_FRAME_TYPE_NONE;

            s32Ret = AR_MPI_VENC_SendFrameEx(pstPara->VeChn[VeChnCnt], &stFrame[i],-1);
            if(AR_SUCCESS != s32Ret)
            {
                VCTRL_PRT("AR_MPI_VENC_SendFrame err:0x%x\n",s32Ret);
                break;
            }
        }
        if(AR_SUCCESS != s32Ret)
        {
            s32Ret = AR_MPI_VPSS_ReleaseChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame);
            if(AR_SUCCESS != s32Ret)
            {
                VCTRL_PRT("AR_MPI_VPSS_ReleaseChnFrame err:0x%x",s32Ret);
                goto err_out;
            }
            continue;
        }

        s32Ret = AR_MPI_VPSS_ReleaseChnFrame(pstPara->VpssGrp,pstPara->VpssChn,pstVideoFrame);
        if(AR_SUCCESS != s32Ret)
        {
            VCTRL_PRT("AR_MPI_VPSS_ReleaseChnFrame err:0x%x",s32Ret);
            goto err_out;
        }

        i++;
        if(i >= QpMapBufNum)
        {
            i = 0;
        }
    }
err_out:
    s32Ret = AR_MPI_SYS_MmzFree(u64QpMapPhyAddr[0],pQpMapVirAddr[0]);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

    s32Ret =  AR_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H264[0],pSkipWeightVirAddr_H264[0]);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

    s32Ret = AR_MPI_SYS_MmzFree(u64SkipWeightPhyAddr_H265[0],pSkipWeightVirAddr_H265[0]);
    if(AR_SUCCESS != s32Ret)
    {
        VCTRL_PRT("AR_MPI_SYS_MmzFree err:0x%x",s32Ret);
        return NULL;
    }

   return NULL;
}

AR_S32 VCTRL_VENC_QpmapSendFrame(VPSS_GRP VpssGrp,VPSS_CHN VpssChn,VENC_CHN VeChn[],AR_S32 s32Cnt,SIZE_S stSize)
{
    AR_S32 i;

    stQpMapSendFramePara.bThreadStart = AR_TRUE;
    stQpMapSendFramePara.VpssGrp = VpssGrp;
    stQpMapSendFramePara.VpssChn = VpssChn;
    stQpMapSendFramePara.s32Cnt  = s32Cnt;
    stQpMapSendFramePara.stSize  = stSize;
    for(i=0; i<s32Cnt;i++)
    {
        stQpMapSendFramePara.VeChn[i] = VeChn[i];
    }

    return pthread_create(&gs_VencQpmapPid, 0, VCTRL_QpmapSendFrameProc, (AR_VOID*)&stQpMapSendFramePara);
}

AR_VOID* VCTRL_VENC_SendVencStreamProc(AR_VOID* p)
{
    AR_S32 i;
    AR_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    VCTRL_VENC_SENDSTREAM_PARA_S* pstPara;
    AR_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    AR_U32 u32PictureCnt = 0;
    AR_S32 VencFd;
    VENC_CHN VencChn;
    AR_CHAR aszFileName[64];
    FILE* pFile;
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    AR_S32 s32Ret;
    AR_S32 s32SaveFile;
    VCHN_INFO* pChnInfo = NULL;
    PAYLOAD_TYPE_E enPayLoadType;
    VENC_STREAM_BUF_INFO_S stStreamBufInfo;

    int framerate = 25;
    AR_U32 frameid = 0;
    AR_U64 currentPts = 0, cpyPts = 0, cpy_cost_time = 0;
    VIDEO_FRAME_INFO_S stFrame;

    prctl(PR_SET_NAME, "SendVencStream", 0,0,0);

    pChnInfo =  (VCHN_INFO*)p;
    VencChn = pChnInfo->chnId;
    s32SaveFile = pChnInfo->s32SaveFile;
    pstPara = &gs_stSendPara;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (VencChn >= VENC_MAX_CHN_NUM) {
        VCTRL_PRT("input count invaild\n");
        return NULL;
    }
    memset(&stFrame, 0, sizeof(VIDEO_FRAME_INFO_S));

    while (AR_TRUE == pstPara->bThreadStart[VencChn]) {
        if (gs_VencInfo[VencChn].preLoadPicFlag) {
            frameid = gs_stPreLoadPic[VencChn][gs_picPostion[VencChn]].stVFrame.u32FrameId;
            gs_stPreLoadPic[VencChn][gs_picPostion[VencChn]].stVFrame.u64PTS = currentPts;
            memcpy(&stFrame, &gs_stPreLoadPic[VencChn][gs_picPostion[VencChn]] , sizeof(VIDEO_FRAME_INFO_S));
            s32Ret = AR_MPI_VENC_SendFrame(VencChn, &stFrame, -1);
            if (s32Ret) {
                printf("send frame failed %d\n", s32Ret);
            }
            //VCTRL_PRT("cpy_cost_time %lld!\n", cpy_cost_time);
            gs_stPreLoadPic[VencChn][gs_picPostion[VencChn]].stVFrame.u32FrameId = frameid +
            gs_VencInfo[VencChn].preLoadPicNum;
            gs_picPostion[VencChn] ++;
            if (gs_picPostion[VencChn] == 3) {
                gs_picPostion[VencChn] = 0;
            }
        }

        if (gs_VencInfo[VencChn].u32SrcFrameRate != 0) {
            framerate = gs_VencInfo[VencChn].u32SrcFrameRate;
        }

        usleep((1000 * 1000) / (framerate + 4));
    }

    return NULL;
}


/******************************************************************************
* funciton : get stream from each channels and save them
******************************************************************************/
AR_VOID* VCTRL_VENC_GetVencStreamProc(AR_VOID* p)
{
    AR_S32 i;
    AR_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    VCTRL_VENC_GETSTREAM_PARA_S* pstPara;
    AR_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    AR_U32 u32PictureCnt = 0;
    AR_S32 VencFd;
    VENC_CHN VencChn;
    AR_CHAR aszFileName[64];
    FILE* pFile;
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    AR_S32 s32Ret;
    AR_S32 s32SaveFile;
    VCHN_INFO* pChnInfo = NULL;
    PAYLOAD_TYPE_E enPayLoadType;
    VENC_STREAM_BUF_INFO_S stStreamBufInfo;

    prctl(PR_SET_NAME, "GetVencStream", 0,0,0);

    pChnInfo =  (VCHN_INFO*)p;
    VencChn = pChnInfo->chnId;
    s32SaveFile = pChnInfo->s32SaveFile;
    pstPara = &gs_stPara;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (VencChn >= VENC_MAX_CHN_NUM) {
        VCTRL_PRT("input count invaild\n");
        return NULL;
    }

    /* decide the stream file name, and open file to save stream */
    logline("the damn VencChn is %d", VencChn);
    //VencChn = pstPara->VeChn[i];
    s32Ret = AR_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
    if (s32Ret != AR_SUCCESS) {
        VCTRL_PRT("AR_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                   VencChn, s32Ret);
        return NULL;
    }
    enPayLoadType = stVencChnAttr.stVencAttr.enType;

    s32Ret = VCTRL_VENC_GetFilePostfix(enPayLoadType, szFilePostfix);
    if (s32Ret != AR_SUCCESS) {
        VCTRL_PRT("VCTRL_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                   stVencChnAttr.stVencAttr.enType, s32Ret);
        return NULL;
    }

    if (0 != s32SaveFile) {
        if ('\0' == pChnInfo->outputFilePath[0]) {
            snprintf(aszFileName,32, "stream_chn%d%s", VencChn, szFilePostfix);
            pFile = fopen(aszFileName, "wb");
        } else {
            pFile = fopen(pChnInfo->outputFilePath, "wb");
        }

        if (!pFile) {
            VCTRL_PRT("open file[%s] failed!\n", aszFileName);
            return NULL;
        } else
            logline("chn%d dst file %s", VencChn, aszFileName);
    }

    /* Set Venc Fd. */
    VencFd = AR_MPI_VENC_GetFd(VencChn);
    if (VencFd < 0) {
        VCTRL_PRT("AR_MPI_VENC_GetFd failed with %#x!\n", VencFd);
        return NULL;
    }

    logline("VencFd=%d", VencFd);
    if (maxfd <= VencFd) {
        maxfd = VencFd;
    }

    s32Ret = AR_MPI_VENC_GetStreamBufInfo (VencChn, &stStreamBufInfo);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("AR_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
        return (void *)AR_FAILURE;
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (AR_TRUE == pstPara->bThreadStart[VencChn]) {
        FD_ZERO(&read_fds);

        FD_SET(VencFd, &read_fds);

        TimeoutVal.tv_sec  = 3;
        TimeoutVal.tv_usec = 0;

        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if (s32Ret < 0) {
            VCTRL_PRT("select failed: %s, fd=%d\n", strerror(errno), VencFd);
            break;
        } else if (s32Ret == 0) {
            VCTRL_PRT("get chn %d venc stream time out, exit thread\n", VencChn);
            continue;
        } else {
            if (FD_ISSET(VencFd, &read_fds))
            {
                /*******************************************************
                 step 2.1 : query how many packs in one-frame stream.
                *******************************************************/
                while (1) {
                    memset(&stStream, 0, sizeof(stStream));

                    s32Ret = AR_MPI_VENC_QueryStatus(VencChn, &stStat);
                    if (AR_SUCCESS != s32Ret) {
                        VCTRL_PRT("chn%d AR_MPI_VENC_QueryStatus failed with %#x!\n", VencChn, s32Ret);
                        break;
                    }

                    /*******************************************************
                    step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                     if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                     {
                        VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                        continue;
                     }
                    *******************************************************/
                    if(0 == stStat.u32CurPacks) {
                          VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                          continue;
                    }
                    if (stStat.u32LeftStreamFrames > 0)
                        break;
                }

                /*******************************************************
                 step 2.3 : malloc corresponding number of pack nodes.
                *******************************************************/
                stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                if (NULL == stStream.pstPack) {
                    VCTRL_PRT("malloc stream pack failed!\n");
                    break;
                }

                /*******************************************************
                 step 2.4 : call mpi to get one-frame stream
                *******************************************************/
                stStream.u32PackCount = stStat.u32CurPacks;
                s32Ret = AR_MPI_VENC_GetStream(VencChn, &stStream, AR_TRUE);
                if (AR_SUCCESS != s32Ret) {
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    VCTRL_PRT("chn%d AR_MPI_VENC_GetStream failed with %#x!\n", \
                               VencChn, s32Ret);
                    break;
                }
                if (pChnInfo->s32Verbose) {
                    if (lastPts[VencChn] != 0)
                        printf("chn%d AR_MPI_VENC_GetStream ptsdiff %lld!\n", \
                                   VencChn, stStream.pstPack->u64PTS - lastPts[VencChn]);
                    lastPts[VencChn] = stStream.pstPack->u64PTS;
                }

                /*******************************************************
                 step 2.5 : save frame to file
                *******************************************************/
                if (0 != s32SaveFile) {
                    s32Ret = VCTRL_VENC_SaveStream(pFile, &stStream);

                    if (AR_SUCCESS != s32Ret) {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        VCTRL_PRT("save stream failed!\n");
                        break;
                    }
                }
                /*******************************************************
                 step 2.6 : release stream
                 *******************************************************/
                s32Ret = AR_MPI_VENC_ReleaseStream(VencChn, &stStream);
                if (AR_SUCCESS != s32Ret) {
                    VCTRL_PRT("AR_MPI_VENC_ReleaseStream failed!\n");
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                    break;
                }

                /*******************************************************
                 step 2.7 : free pack nodes
                *******************************************************/
                u32PictureCnt++;
                free(stStream.pstPack);
                stStream.pstPack = NULL;

                if(pChnInfo->u32PicNum >=0 && u32PictureCnt >= pChnInfo->u32PicNum) {
                    //printf("done encoding, recv_count=%d\n", recv_count);
                    pstPara->bThreadStart[VencChn] = AR_FALSE;
                    pChnInfo->u32State = STATE_IDLE;
                    printf("change pChnInfo->u32State to STATE_IDLE %d\n", STATE_IDLE);
                    stop_vchnnel_work(VencChn);
                    break;
                }
            } //endof fd set
        }
        //usleep(30 * 1000);
    }

    /*******************************************************
    * step 3 : close save-file
    *******************************************************/
    if(0 != s32SaveFile) {
        fclose(pFile);
    }
    VCTRL_PRT("chn %d stop VCTRL_VENC_GetVencStreamProc!\n", VencChn);
    return NULL;
}

/******************************************************************************
* funciton : get svc_t stream from h264 channels and save them
******************************************************************************/
AR_VOID* VCTRL_VENC_GetVencStreamProc_Svc_t(void* p)
{
    AR_S32 i = 0;
    AR_S32 s32Cnt = 0;
    AR_S32 s32ChnTotal;
    VENC_CHN_ATTR_S stVencChnAttr;
    VCTRL_VENC_GETSTREAM_PARA_S* pstPara;
    AR_S32 maxfd = 0;
    struct timeval TimeoutVal;
    fd_set read_fds;
    AR_S32 VencFd[VENC_MAX_CHN_NUM];
    AR_CHAR aszFileName[VENC_MAX_CHN_NUM][64];
    FILE* pFile[VENC_MAX_CHN_NUM];
    char szFilePostfix[10];
    VENC_CHN_STATUS_S stStat;
    VENC_STREAM_S stStream;
    AR_S32 s32Ret;
    VENC_CHN VencChn;
    PAYLOAD_TYPE_E enPayLoadType[VENC_MAX_CHN_NUM];
    VENC_STREAM_BUF_INFO_S stStreamBufInfo[VENC_MAX_CHN_NUM];
    AR_BOOL mpp_service_is_lib = ar_hal_sys_mpp_service_is_lib();

    pstPara = (VCTRL_VENC_GETSTREAM_PARA_S*)p;
    s32ChnTotal = pstPara->s32Cnt;

    /******************************************
     step 1:  check & prepare save-file & venc-fd
    ******************************************/
    if (s32ChnTotal >= VENC_MAX_CHN_NUM)
    {
        VCTRL_PRT("input count invaild\n");
        return NULL;
    }
    for (i = 0; i < s32ChnTotal; i++)
    {
        /* decide the stream file name, and open file to save stream */
        VencChn = i;
        s32Ret = AR_MPI_VENC_GetChnAttr(VencChn, &stVencChnAttr);
        if (s32Ret != AR_SUCCESS)
        {
            VCTRL_PRT("AR_MPI_VENC_GetChnAttr chn[%d] failed with %#x!\n", \
                       VencChn, s32Ret);
            return NULL;
        }
        enPayLoadType[i] = stVencChnAttr.stVencAttr.enType;

        s32Ret = VCTRL_VENC_GetFilePostfix(enPayLoadType[i], szFilePostfix);
        if (s32Ret != AR_SUCCESS)
        {
            VCTRL_PRT("VCTRL_VENC_GetFilePostfix [%d] failed with %#x!\n", \
                       stVencChnAttr.stVencAttr.enType, s32Ret);
            return NULL;
        }

        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            snprintf(aszFileName[i + s32Cnt],32, "Tid%d%s", i + s32Cnt, szFilePostfix);

            pFile[i + s32Cnt] = fopen(aszFileName[i + s32Cnt], "wb");

            if (!pFile[i + s32Cnt])
            {
                VCTRL_PRT("open file[%s] failed!\n",
                           aszFileName[i + s32Cnt]);
                return NULL;
            }
        }

        /* Set Venc Fd. */
        VencFd[i] = AR_MPI_VENC_GetFd(i);
        if (VencFd[i] < 0)
        {
            VCTRL_PRT("AR_MPI_VENC_GetFd failed with %#x!\n",
                       VencFd[i]);
            return NULL;
        }
        if (maxfd <= VencFd[i])
        {
            maxfd = VencFd[i];
        }
        s32Ret = AR_MPI_VENC_GetStreamBufInfo (i, &stStreamBufInfo[i]);
        if (AR_SUCCESS != s32Ret)
        {
            VCTRL_PRT("AR_MPI_VENC_GetStreamBufInfo failed with %#x!\n", s32Ret);
            return NULL;
        }
    }

    /******************************************
     step 2:  Start to get streams of each channel.
    ******************************************/
    while (AR_TRUE == pstPara->bThreadStart)
    {
        FD_ZERO(&read_fds);
        for (i = 0; i < s32ChnTotal; i++)
        {
            FD_SET(VencFd[i], &read_fds);
        }
        TimeoutVal.tv_sec  = 3;
        TimeoutVal.tv_usec = 0;
		if(mpp_service_is_lib)
	        s32Ret = 1;
		else
	        s32Ret = select(maxfd + 1, &read_fds, NULL, NULL, &TimeoutVal);

        if (s32Ret < 0)
        {
            VCTRL_PRT("select failed!\n");
            break;
        }
        else if (s32Ret == 0)
        {
            VCTRL_PRT("get venc stream time out, exit thread\n");
            continue;
        }
        else
        {
            for (i = 0; i < s32ChnTotal; i++)
            {
                if (!mpp_service_is_lib || FD_ISSET(VencFd[i], &read_fds))
                {

                    /*******************************************************
                    step 2.1 : query how many packs in one-frame stream.
                    *******************************************************/
                    while (1) {
                        memset(&stStream, 0, sizeof(stStream));
                        s32Ret = AR_MPI_VENC_QueryStatus(i, &stStat);
                        if (AR_SUCCESS != s32Ret)
                        {
                            VCTRL_PRT("AR_MPI_VENC_Query chn[%d] failed with %#x!\n", i, s32Ret);
                            break;
                        }
                        /*******************************************************
                        step 2.2 :suggest to check both u32CurPacks and u32LeftStreamFrames at the same time,for example:
                         if(0 == stStat.u32CurPacks || 0 == stStat.u32LeftStreamFrames)
                         {                        VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                            continue;
                         }
                         *******************************************************/
                        if (0 == stStat.u32CurPacks)
                        {
                            VCTRL_PRT("NOTE: Current  frame is NULL!\n");
                            continue;
                        }
                        if (stStat.u32LeftStreamFrames > 0)
                            break;
                    }
                    /*******************************************************
                     step 2.3 : malloc corresponding number of pack nodes.
                    *******************************************************/
                    stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStat.u32CurPacks);
                    if (NULL == stStream.pstPack)
                    {
                        VCTRL_PRT("malloc stream pack failed!\n");
                        break;
                    }
                    /*******************************************************
                     step 2.4 : call mpi to get one-frame stream
                    *******************************************************/
                    stStream.u32PackCount = stStat.u32CurPacks;
                    s32Ret = AR_MPI_VENC_GetStream(i, &stStream, AR_TRUE);
                    if (AR_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        VCTRL_PRT("AR_MPI_VENC_GetStream failed with %#x!\n", \
                                   s32Ret);
                        break;
                    }
                    /*******************************************************
                     step 2.5 : save frame to file
                    *******************************************************/
                    for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
                    {
                        switch (s32Cnt)
                        {
                            case 0:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType)
                                {
                                    s32Ret = VCTRL_VENC_SaveStream(pFile[i+s32Cnt], &stStream);
                                }
                                break;
                            case 1:
                                if (BASE_IDRSLICE == stStream.stH264Info.enRefType      ||
                                    BASE_PSLICE_REFBYBASE == stStream.stH264Info.enRefType ||
                                    BASE_PSLICE_REFBYENHANCE == stStream.stH264Info.enRefType)
                                {
                                    s32Ret = VCTRL_VENC_SaveStream(pFile[i+s32Cnt], &stStream);
                                }
                                break;
                            case 2:
                                s32Ret = VCTRL_VENC_SaveStream(pFile[i+s32Cnt], &stStream);
                                break;
                        }
                        if (AR_SUCCESS != s32Ret)
                        {
                            free(stStream.pstPack);
                            stStream.pstPack = NULL;
                            VCTRL_PRT("save stream failed!\n");
                            break;
                        }
                    }

                    /*******************************************************
                    step 2.6 : release stream
                    *******************************************************/
                    s32Ret = AR_MPI_VENC_ReleaseStream(i, &stStream);
                    if (AR_SUCCESS != s32Ret)
                    {
                        free(stStream.pstPack);
                        stStream.pstPack = NULL;
                        break;
                    }
                    /*******************************************************
                     step 2.7 : free pack nodes
                    *******************************************************/
                    free(stStream.pstPack);
                    stStream.pstPack = NULL;
                }
            }
        }
    }
    /*******************************************************
     step 3 : close save-file
    *******************************************************/
    for (i = 0; i < s32ChnTotal; i++)
    {
        for (s32Cnt = 0; s32Cnt < 3; s32Cnt++)
        {
            if (pFile[i + s32Cnt])
            {
                fclose(pFile[i + s32Cnt]);
            }
        }
    }
    return NULL;
}

AR_S32 VCTRL_VENC_StartSendStream(VCHN_INFO* pChnInfo, AR_S32 s32Cnt)
{
    AR_U32 i;
    VENC_CHN VencChn;

    VencChn = pChnInfo->chnId;
    gs_stSendPara.bThreadStart[VencChn] = AR_TRUE;

    return pthread_create(&gs_VencSendPid[VencChn], 0, VCTRL_VENC_SendVencStreamProc, (AR_VOID*)pChnInfo);
}

/******************************************************************************
* funciton : start get venc stream process thread
******************************************************************************/
AR_S32 VCTRL_VENC_StartGetStream(VCHN_INFO* pChnInfo, AR_S32 s32Cnt)
{
    AR_U32 i;
    VENC_CHN VencChn;

    VencChn = pChnInfo->chnId;
    gs_stPara.bThreadStart[VencChn] = AR_TRUE;

    return pthread_create(&gs_VencPid[VencChn], 0, VCTRL_VENC_GetVencStreamProc, (AR_VOID*)pChnInfo);
}

/******************************************************************************
* funciton : start get venc svc-t stream process thread
******************************************************************************/
AR_S32 VCTRL_VENC_StartGetStream_Svc_t(AR_S32 s32Cnt)
{
    gs_stPara.bThreadStart[0] = AR_TRUE;
    gs_stPara.s32Cnt = s32Cnt;
    return pthread_create(&gs_VencPid, 0, VCTRL_VENC_GetVencStreamProc_Svc_t, (AR_VOID*)&gs_stPara);
}

/******************************************************************************
* funciton : stop get venc stream process.
******************************************************************************/
AR_S32 VCTRL_VENC_StopGetStream(VENC_CHN VeChn)
{
    if (AR_TRUE == gs_stPara.bThreadStart[VeChn]) {
        gs_stPara.bThreadStart[VeChn] = AR_FALSE;
        pthread_join(gs_VencPid[VeChn], 0);
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_StopSendStream(VENC_CHN VeChn)
{
    if (AR_TRUE == gs_stSendPara.bThreadStart[VeChn]) {
        gs_stSendPara.bThreadStart[VeChn] = AR_FALSE;
        pthread_join(gs_VencSendPid[VeChn], 0);
    }
    return AR_SUCCESS;
}

AR_S32 VCTRL_VENC_StopSendQpmapFrame(void)
{
    if (AR_TRUE == stQpMapSendFramePara.bThreadStart)
    {
        stQpMapSendFramePara.bThreadStart = AR_FALSE;
        pthread_join(gs_VencQpmapPid, 0);
    }
    return AR_SUCCESS;
}


AR_VOID VENC_Getdef_Info(VCHN_INFO* pChnInfo)
{
    if(pChnInfo == NULL)
    {
        printf("%s no valid input data\n",__FILE__);
        return;
    }
    pChnInfo->enPayLoad = PT_H265;
    pChnInfo->chnId = 0;
    pChnInfo->enRcMode = VCTRL_RC_CBR;
    pChnInfo->enGopMode = VENC_GOPMODE_NORMALP;
    pChnInfo->u32Profile = 0;
    pChnInfo->u32State = STATE_STOP;

    AR_CHAR EndName[64] = {""};
    memset(EndName, 0, sizeof(EndName));
    strncpy(pChnInfo->outputFilePath, pChnInfo->inputFilePath, (strlen(pChnInfo->inputFilePath) - 4));
    if(pChnInfo->enPayLoad == PT_H265)
        snprintf(EndName, sizeof(EndName), "_chn%d.h265", pChnInfo->chnId);
    else if(pChnInfo->enPayLoad == PT_H264)
        snprintf(EndName, sizeof(EndName), "_chn%d.h264", pChnInfo->chnId);
    else if(pChnInfo->enPayLoad == PT_JPEG)
        snprintf(EndName, sizeof(EndName), "_chn%d", pChnInfo->chnId);
    else if(pChnInfo->enPayLoad == PT_MJPEG)
        snprintf(EndName, sizeof(EndName), "_chn%d.mjpeg", pChnInfo->chnId);
    strncat(pChnInfo->outputFilePath, EndName, strlen(EndName));
}

AR_VOID* VCTRL_VencStartProc(AR_VOID* param)
{
    /*
    AR_S32 i;
    */
    AR_S32 s32Ret;
    SIZE_S          stSize;
    VCHN_INFO* pChnInfo = (VCHN_INFO*)param;
    stSize.u32Width = pChnInfo->u32Width;
    stSize.u32Height = pChnInfo->u32Height;

    /*
    PIC_SIZE_E      enSize = VCTRL_COMM_SYS_GetEnPicSize(&stSize);
    */
    AR_S32          s32ChnNum     = 1;
    VENC_CHN        VencChn    = pChnInfo->chnId;
    /*
    AR_U32          u32Profile = pChnInfo->u32Profile;
    PAYLOAD_TYPE_E  enPayLoad  = pChnInfo->enPayLoad;
    VENC_GOP_MODE_E enGopMode = pChnInfo->enGopMode;
    */
    VENC_GOP_ATTR_S stGopAttr = { 0 };
    /*
    VCTRL_RC_E      enRcMode = VCTRL_RC_CBR;
    AR_BOOL         bRcnRefShareBuf = AR_TRUE;
    AR_U32          u32SupplementConfig = 0;
    VI_DEV          ViDev        = 0;
    */
    VI_PIPE         ViPipe       = 0;
    VI_CHN          ViChn        = pChnInfo->chnId;

    if(pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return (AR_VOID*)AR_FAILURE;
    }

    s32Ret = VCTRL_VENC_SYS_Init(&stSize);
    if(s32Ret != AR_SUCCESS) {
        VCTRL_PRT("Init SYS err for %#x!\n", s32Ret);
        return (AR_VOID*)AR_FAILURE;
    }

    memcpy(&gs_VencInfo[VencChn], pChnInfo, sizeof(VCHN_INFO));

    VCTRL_COMM_CORE_Init(pChnInfo->enPayLoad, pChnInfo->processor, pChnInfo->u32CoreClock,
        pChnInfo->u32BpuClock, pChnInfo->u32JpegCoreClock);

    VCTRL_VI_BIND_INFO * pViInfo = &pChnInfo->stViInfo;

    pViInfo->inFile = pChnInfo->inputFilePath;
    pViInfo->ouFile = pChnInfo->outputFilePath;
    pViInfo->u32Width = pChnInfo->u32Width;
    pViInfo->u32Height= pChnInfo->u32Height;

    pViInfo->stViData = &(gsViData[ViChn]);

    VI_USERPIC_ATTR_S stUsrPic;
    stUsrPic.enUsrPicMode                                 =  VI_USERPIC_MODE_PIC;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Width       =  pChnInfo->u32Width;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.u32Height      =  pChnInfo->u32Height;
    stUsrPic.unUsrPic.stUsrPicFrm.stVFrame.enPixelFormat  =  PIXEL_FORMAT_YVU_PLANAR_420;

    //ViData * pViData = &pstViInfo->stViData;

    memcpy(&pViInfo->stViData->stUsrPic, &stUsrPic, sizeof(stUsrPic));

    VCTRL_PRT("====to ar_mpi_open YUV file: %s. \n", pChnInfo->inputFilePath);
    pViInfo->stViData->pfd = fopen(pChnInfo->inputFilePath, "rb");
    if (pViInfo->stViData->pfd == NULL) {
        VCTRL_PRT("ar_mpi_open file -> %s fail: %s\n", pChnInfo->inputFilePath, strerror(errno));
        return AR_FAILURE;
    }

    AR_U32 blksize = VDEC_GetPicBufferSize(
                            pChnInfo->enPayLoad,
                            pChnInfo->u32Width,
                            pChnInfo->u32Height,
                            PIXEL_FORMAT_YVU_SEMIPLANAR_420,
                            DATA_BITWIDTH_8,
                            64);

    VB_POOL_CONFIG_S stVbPoolCfg;
    memset(&stVbPoolCfg, 0, sizeof(VB_POOL_CONFIG_S));

    pViInfo->stViData->stViFrameInfo.u32Size = blksize;

    stVbPoolCfg.u64BlkSize  = blksize;
    stVbPoolCfg.u32BlkCnt   = 4;
    stVbPoolCfg.enRemapMode = VB_REMAP_MODE_NONE;

    VB_POOL poolid = AR_MPI_VB_CreatePool(&stVbPoolCfg);

    if (VB_INVALID_POOLID == poolid) {
        logline("chn%d AR_MPI_VB_CreatePool failed", ViChn);
        goto EXIT;
    }

    s32Ret = AR_MPI_VB_MmapPool(poolid);
    if (s32Ret) {
        VCTRL_PRT("mmap pool failed\n");
        return AR_FAILURE;
    }

    pViInfo->stViData->u32PoolId = poolid;

    s32Ret = VCTRL_VENC_VI_Init(pViInfo);
    if(s32Ret != AR_SUCCESS) {
        VCTRL_PRT("Init VI err for %#x!\n", s32Ret);
        goto EXIT;
    }

    if (pChnInfo->preLoadPicFlag) {
        VCTRL_VENC_Preload_Pic(VencChn);
    }

   /******************************************
    start stream venc
    ******************************************/

    s32Ret = VCTRL_VENC_GetGopAttr(pChnInfo->enGopMode, &stGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("Venc Get GopAttr for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

    s32Ret = VCTRL_VENC_Start(pChnInfo, &stGopAttr);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("Venc Start failed for %#x!\n", s32Ret);
        goto EXIT_VI_STOP;
    }

#if 1
    s32Ret = VCTRL_VI_Bind_VENC(ViPipe, ViChn, VencChn);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("VCTRL_VI_Bind_VENC failed for %#x!\n", s32Ret);
        goto EXIT_VENC_STOP;
    }
#else
    VCTRL_VENC_StartSendStream(pChnInfo, s32ChnNum);
#endif

    /******************************************
     stream save process
    ******************************************/
    s32Ret = VCTRL_VENC_StartGetStream(pChnInfo, s32ChnNum);
    if (AR_SUCCESS != s32Ret) {
        VCTRL_PRT("Start Venc failed!\n");
        goto EXIT_VENC_UNBind;
    }

    pChnInfo->u32State = STATE_START;

    while(STATE_START == VCTRL_VENC_SERVICE_STATUS(pChnInfo)) {
        sleep(1);
    }

#if 0
    VCTRL_VENC_StopSendStream(VencChn);
#endif

/******************************************
 exit process
******************************************/
VCTRL_PRT("chn %d VCTRL_VENC_StopGetStream.\n", VencChn);
VCTRL_VENC_StopGetStream(VencChn);


EXIT_VENC_UNBind:
    VCTRL_PRT("chn %d VCTRL_VI_UnBind_VENC.\n", VencChn);
    VCTRL_VI_UnBind_VENC(ViPipe, ViChn, VencChn);

EXIT_VENC_STOP:
    VCTRL_PRT("chn %d VCTRL_VENC_Stop.\n", VencChn);
    VCTRL_VENC_Stop(VencChn);


EXIT_VI_STOP:
    VCTRL_PRT("chn %d VCTRL_VENC_VI_Deinit.\n", VencChn);
    VCTRL_VENC_VI_Deinit(pViInfo);

    VCTRL_PRT("AR_MPI_VB_MunmapPool poolid %d\n", poolid);
    s32Ret = AR_MPI_VB_MunmapPool(poolid);
    if (s32Ret) {
        VCTRL_PRT("munmap pool failed\n");
    }

    VCTRL_PRT("AR_MPI_VB_DestroyPool poolid %d.\n", poolid);
    s32Ret = AR_MPI_VB_DestroyPool(poolid);
    if (s32Ret) {
        VCTRL_PRT("Destory VB Pool failed for %#x!\n", s32Ret);
    }

EXIT:
    //VCTRL_COMM_SYS_Exit();
    return (void *)(unsigned long)s32Ret;
}


AR_S32 VCTRL_VENC_SERVICE_START(VCHN_INFO* pChnInfo)
{
    VCTRL_PRT("Start Venc chnId %d, u32State %d, pChnInfo = %p!\n",pChnInfo->chnId, pChnInfo->u32State, pChnInfo);
    return pthread_create(&pChnInfo->ptVencStartTid, 0, VCTRL_VencStartProc, (AR_VOID*)pChnInfo);
}

AR_S32 VCTRL_VENC_SERVICE_STOP(VCHN_INFO* pChnInfo)
{
    VCTRL_PRT("Stop Venc chnId %d, u32State %d! pChnInfo = %p\n",pChnInfo->chnId, pChnInfo->u32State, pChnInfo);
    if(pChnInfo->u32State == STATE_START) {
        pChnInfo->u32State = STATE_STOP;
        pthread_join(pChnInfo->ptVencStartTid, (void **)NULL);
    }
    return AR_SUCCESS;
}


AR_S32 VCTRL_VENC_SERVICE_SET(VCHN_INFO* pChnInfo)
{
    AR_S32 s32Ret;
    if(pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return AR_FAILURE;
    }

    VCTRL_PRT("Set Venc chnId: %d, pChnInfo->enUpdate = %d!\n", pChnInfo->chnId, pChnInfo->enUpdate);

    if((pChnInfo->enUpdate & UPDATE_BITRATE) || (pChnInfo->enUpdate & UPDATE_GOPSIZE) ||
        (pChnInfo->enUpdate & UPDATE_FPS)) {
        VENC_CHN_ATTR_S stChnAttr = { 0 };
        s32Ret = AR_MPI_VENC_GetChnAttr(pChnInfo->chnId, &stChnAttr);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_GetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        VCTRL_VENC_SetAttr(&stChnAttr, pChnInfo, &stChnAttr.stGopAttr);

        s32Ret = AR_MPI_VENC_SetChnAttr(pChnInfo->chnId, &stChnAttr);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_SetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    } else if (pChnInfo->enUpdate & SET_ROIMAP) {
        VENC_ROI_ATTR_S stRoiAttr = { 0 };

        stRoiAttr.bEnable = pChnInfo->roiMap.roiIndexEnable;
        stRoiAttr.u32Index = pChnInfo->roiMap.roiIndex;
        stRoiAttr.s32Qp = pChnInfo->roiMap.roiQp;
        stRoiAttr.stRect.s32X = pChnInfo->roiMap.roiStartX;
        stRoiAttr.stRect.s32Y = pChnInfo->roiMap.roiStartY;
        stRoiAttr.stRect.u32Height = pChnInfo->roiMap.roiHeigth;
        stRoiAttr.stRect.u32Width = pChnInfo->roiMap.roiWidth;

        s32Ret = AR_MPI_VENC_SetRoiAttr(pChnInfo->chnId, &stRoiAttr);
    } else if ((pChnInfo->enUpdate & UPDATE_QP) || (pChnInfo->enUpdate & UPDATE_RCMODE)) {
        VENC_RC_PARAM_S rcParam = {0};
        VENC_CHN_ATTR_S stChnAttr = { 0 };
        s32Ret = AR_MPI_VENC_GetChnAttr(pChnInfo->chnId, &stChnAttr);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_GetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        s32Ret = AR_MPI_VENC_GetRcParam(pChnInfo->chnId, &rcParam);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_GetRcParam with %#x!\n", s32Ret);
            return AR_FAILURE;
        }

        VCTRL_VENC_SetRcParam(&rcParam, pChnInfo, &stChnAttr);
        s32Ret = AR_MPI_VENC_SetRcParam(pChnInfo->chnId, &rcParam);
        if (AR_SUCCESS != s32Ret) {
            VCTRL_PRT("AR_MPI_VENC_SetChnAttr with %#x!\n", s32Ret);
            return AR_FAILURE;
        }
    }

    return AR_SUCCESS;
}


AR_S32 VCTRL_VENC_SERVICE_STATUS(VCHN_INFO* pChnInfo)
{
    if(pChnInfo == NULL) {
        VCTRL_PRT("no valid input data\n");
        return AR_FAILURE;
    }
    return pChnInfo->u32State;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

