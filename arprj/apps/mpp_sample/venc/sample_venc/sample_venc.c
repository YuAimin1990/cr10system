#include "stdio.h"
#include "stdlib.h"
#include <getopt.h>
#include <fcntl.h>
#include "mpi_venc.h"
#include "ar_math.h"
#include "mpi_vb.h"
#include "ar_comm_venc.h"
#include "hal_sys.h"
#include "mpi_sys.h"
#include  "signal.h"
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include "mpi_sysctl.h"

#include "hal_vcodec_type_comm.h"
#define MP4_PACKING
#ifdef MP4_PACKING
#include "mp4packing.h"
#endif

enum TEST_OPTION {
    TEST_VBR = 1,
    TEST_FIXQP = 2,
};

struct VinPara {
  int fd;
  AR_S32 width;
  AR_S32 height;
  AR_S32 frameRate;
};

struct ThreadPara {
  int picNum;
  int id;
};

#define SUPPORT_NFS_WRITE 1

#define YUV_FRAME_CACHED_NUM    10
static AR_U8 gloablRun = 0;
static AR_U8 exit_enc = 0;
static AR_U8 gSuspend = 0;
/*
static AR_U32 send_count = 0;
*/
static int sendcount = 0;
static AR_U32 recv_count = 0;

static AR_U32 statTime = 30;
static AR_U32 maxBitRate = 5000;
static AR_U32 minBitRate = 3000;
static AR_U32 maxIpRatio = 100;
static AR_U32 minIpRatio = 50;
static AR_U32 maxIQp = 51;
static AR_U32 minIQp = 0;
static AR_U32 maxPQp = 45;
static AR_U32 minPQp = 0;
static AR_U32 maxBQp = 45;
static AR_U32 minBQp = 0;
static AR_U32 inst_parallelism = 0;
static VENC_RC_MODE_E rcMode = VENC_RC_MODE_H264CBR;
static AR_U32 u32Qfactor = 80;//0~99
static AR_U32 bufScale = 10;
static AR_S32 insertSEI = 0;

static AR_S32 testFlags = 0;

static AR_S32 id;
static AR_S32 picNum = -1;
static PAYLOAD_TYPE_E type = PT_H265;
static AR_S32 width = 1920;
static AR_S32 height = 1080;
static AR_S32 bitrate = 4096;
static AR_S32 frameRate = 25;
static AR_S32 gopsize = 100;
static AR_U32 profile = 0;
static AR_S32 cf50Ratio = 0;//0: disable 64: 50% loss 96: 25% loss 128: lossless
static AR_S32 attach_vb_pool = 0;
static VENC_CHN_POOL_S venc_fb_ex_pool = {VB_INVALID_POOLID, VB_INVALID_POOLID};

static int s32FrameCount = 0;
static AR_S32 s32SkipNum = 0;
VB_POOL dedicated_vbpool = VB_INVALID_POOLID;

MPI_SYSCTL_HANDLE g_phandle = NULL;

static AR_S32 hevcAlignment = 16;
static AR_S32 otherAlignment = 16;
static AR_S32 rotationAngle = 0;
static AR_S32 mirrorDirection= 0;

AR_S32 VencInit(AR_S32 id, AR_S32 num, PAYLOAD_TYPE_E type, AR_S32 width, AR_S32 height, AR_S32 bitrate, AR_S32 gopsize, AR_S32 frameRate, AR_U32 u32Profile);
#define logline(fmt, ...) printf("test_mpp_venc [%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
void handle_sigsegv(int signo, siginfo_t *info, void *data)
{
    printf("recv SIGINT signo=%d\n", signo);
    gloablRun = 0;
    exit_enc = 1;
}

void sample_venc_callback(SYSCTL_EVENT_E eSysctlEvent, void *pData, AR_S32 s32Len)
{
    logline("eSysctlEvent=%d", eSysctlEvent);

    if(SYSCTL_EVENT_FAST_RESUME == eSysctlEvent || SYSCTL_EVENT_RESUME == eSysctlEvent){
        logline("resume start");
        //usleep(1*1000);
        uint64_t t1, t2;
        AR_MPI_SYS_GetCurPTS(&t1);

        int ret = VencInit(id, picNum, type, width, height, bitrate, gopsize, frameRate, profile);
        logline("VencInit ret %d", ret);
        if (ret) {
            logline("init venc failed");
            return;
        }
        AR_MPI_VENC_Resume(id);

        AR_MPI_SYS_GetCurPTS(&t2);
        logline("resume done, overhead %3.1f ms", (t2-t1)/1000.0);
        gSuspend = 0;
    } else if(SYSCTL_EVENT_FAST_SUSPEND == eSysctlEvent || SYSCTL_EVENT_SUSPEND == eSysctlEvent) {
        logline("suspend start");
        //usleep(1*1000);
        gSuspend = 1;
        uint64_t t1, t2;
        AR_MPI_SYS_GetCurPTS(&t1);
        int ret = AR_MPI_VENC_StopRecvFrame(id);
        logline("AR_MPI_VENC_StopRecvFrame ret %#x", ret);
        ret = AR_MPI_VENC_DestroyChn(id);
        logline("AR_MPI_VENC_DestroyChn ret %#x", ret);
        AR_MPI_VENC_Suspend(id);
        AR_MPI_SYS_GetCurPTS(&t2);

        logline("suspend done, overhead %3.1f ms", (t2-t1)/1000.0);
    } else {
        logline("unknown event %d for suspend/resume", eSysctlEvent);
    }

    int ret = AR_MPI_SYSCTL_Event_done(g_phandle, eSysctlEvent);

    if(ret)
        logline("event %d SYSCTL_Event_done fail", eSysctlEvent);
    else
        logline("event %d SYSCTL_Event_done succes", eSysctlEvent);
}

static AR_S32 inst_created = 0;
static AR_S32 dev_opened = 0;
static AR_S32 dev_started = 0;
static AR_S32 dev_id = 0;
static AR_S32 useBind = 0;
static AR_S32 mp4Packing = 0;
static struct VinPara vinData = {0};
static char yuvFile[128] = "/tmp/test_rc.yuv";
static char streamFile[128] = "/media/debug.hevc";

static struct option long_options[] = {
    {"verbose", no_argument, NULL, 'v'},
    {"bind", no_argument, NULL, 'B'},
    {"channel", required_argument, NULL, 'c'},
    {"frameRate", required_argument, NULL, 'f'},
    {"picNum", required_argument, NULL, 'n'},
    {"type", required_argument, NULL, 't'},
    {"height", required_argument, NULL, 'h'},
    {"width", required_argument, NULL, 'w'},
    {"bitrate", required_argument, NULL, 'b'},
    {"gopsize", required_argument, NULL, 'g'},
    {"input", required_argument, NULL, 'i'},
    {"output", required_argument, NULL, 'o'},
    {"profile", required_argument, NULL, 'P'},
    {"mp4packing", no_argument, NULL, 'O'},
    {"help", no_argument, NULL, 'H'},

    {"statTime",   required_argument, NULL, '0'},
    {"maxBitRate", required_argument, NULL, '1'},
    {"minBitRate", required_argument, NULL, '2'},
    {"maxIpRatio", required_argument, NULL, '3'},
    {"minIpRatio", required_argument, NULL, '4'},

    {"maxIQp", required_argument, NULL, '5'},
    {"minIQp", required_argument, NULL, '6'},
    {"maxPQp", required_argument, NULL, '7'},
    {"minPQp", required_argument, NULL, '8'},
    {"maxBQp", required_argument, NULL, '9'},
    {"minBQp", required_argument, NULL, 'a'},
    {"rcMode", required_argument, NULL, 'd'},
    {"inst_parallelism", required_argument, NULL, 'e'},
    {"bufScale", required_argument, NULL, 'j'},
    {"insertSEI", required_argument, NULL, 'k'},
    {"testFlags", required_argument, NULL, 'q'},
    {"attachVbPool", required_argument, NULL, 'l'},
    {"Qfactor", required_argument, NULL, 'Q'},
    {"rotate", required_argument, NULL, 'R'},
    {"mirror", required_argument, NULL, 'M'},
    {"Cf50Ratio", required_argument, NULL, 'C'},
    {"s32SkipNum", required_argument, NULL, 'S'},
    {NULL, 0, NULL, 0}
};

unsigned int timer_get_ms()
{
    struct timeval tv;
    unsigned int time;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec*1000  + tv.tv_usec / 1000;
    return time;
}

unsigned long timer_get_us()
{
    struct timeval tv;
    unsigned long time;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec*1000*1000  + tv.tv_usec;
    return time;
}

static AR_S32 unbindVindAndVenc(AR_S32 id);

static AR_S32 waitOnFd(AR_S32 devFd, AR_U32 timeout/*in seconds*/) {
    if (devFd < 0) {
        printf("invalid dev fd: %d\n", devFd);
        return -1;
    }

    fd_set rfds;
    struct timeval tv;
    AR_S32 retval;

    FD_ZERO(&rfds);
    FD_SET(devFd, &rfds);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    retval = select(devFd + 1, &rfds, NULL, NULL, &tv);

    /*
    printf("select return %d\n", retval);
    */

    if (retval == -1) {
        printf("select return error: %s\n", strerror(errno));
        return -1;
    } else if (retval) {
        /*
        printf("selet notifying %d fd ready for reading\n", retval);
        */
        /* FD_ISSET(0, &rfds) will be true. */
        return retval;
    } else {
        /*
        if(exit_enc) {
            printf("select timeout after %u second, exit\n", selectTimeout);
            break;
        }
        */

        printf("select timeout after %u second\n", timeout);
        return 0;
    }
}

static void usage(void)
{
    fprintf(stderr,
        "Usage: test_mpp_venc [OPTION]\n"
        "\n"
        " -v, --verbose             display more detail information, deprecated\n"
        " -B, --bind                use bind, deprecated\n"
        " -c, --channel             venc channel: 0 - 40, default 0\n"
        " -f, --frameRate           default 25\n"
        " -n, --picNum              picture numbers, default -1\n"
        " -t, --type                video type: h265, h264, jpeg, mjpeg, default h265\n"
        " -h, --height              default 1080\n"
        " -w, --width               default 1920\n"
        " -b, --bitrate             default 4096 kbps\n"
        " -g, --gopsize             default 50\n"
        " -i, --input               yuv file, default /tmp/test_rc.yuv\n"
        " -o, --output              output stream file, default /mnt/debug.hevc\n"
        " -P, --profile             H.264: [0,4]; 0: auto; 1: baseline; 2: MP; 3: extend; 4: HP;\n"
        "                           H.265: [0,3]; 0: auto; 1: MP; 2: Main 10 Profile; 3: Main still pic;\n"
        "                           Jpege/MJpege: 0; 0: Baseline;\n"
        " -d, --rcMode              rate control mode. \"vbr\": vbr mode; \"cbr\": vbr mode, \"fixqp\": fixqp mode \n"
        " -e, --inst_parallelism    enable parllel coding:[0,1], h26x is valid, jpeg not support\n"
        " -j, --bufScale            adjust the buffersize according to the bitrate. default: 10s\n"
        " -O, --mp4packing          enable packaging in mp4\n"
        " -0, --statTime,           bitrate statistics time. Range:[1,60], kbps\n"
        " -1, --maxBitRate,         the max bitrate. Range:H265[128, 160000],H264[128, 240000]kbps\n"
        " -2, --minBitRate,         the min bitrate. Range:H265[128, 160000],H264[128, 240000]kbps\n"
        " -3, --maxIpRatio,         the min ratio of i frame and p frame. Range: [minIpRatio, 100]\n"
        " -4, --minIpRatio,         the min ratio of i frame and p frame. Range: [1, 100]\n"
        " -5, --maxIQp,             the max I qp. Range:[minIQp, 51]\n"
        " -6, --minIQp,             the min I qp. Range:[0, 51]\n"
        " -7, --maxPQp,             the max P qp. Range:[minPQp, 51]\n"
        " -8, --minPQp,             the min P qp. Range:[0, 51]\n"
        " -9, --maxBQp,             the max B qp. Range:[minBQp, 51]\n"
        " -a, --minBQp,             the min B qp. Range:[0, 51]\n"
        " -k, --bufScale,           bitstream buffer scale relative to bitrate. You should set this param only when you know what this param means\n"
        " -l, --insertSEI,          wether insert sei data into bitstream, only works for h26x"
        " -q, --testFlags,          test flags, 1: vbr dynamic param setting; 2: fixqp dynamic qp setting\n"
        " -Q, --u32Qfactor          fixqp qfactor setting default 80, [0, 99]\n"
        " -C, --Cf50Ratio           0: disable, 64: 50% loss, 96: 25% loss, 128: lossless\n"
        " -R, --rotate              rotation ange: 90, 180, 270\n"
        " -M, --mirror              mirror direction: 1:vertical, 2:horizontal, 3:horizontal+vertical\n"
        " -S, --s32SkipNum          skip frame before encode\n"
        " -H, --help                print this help\n"
        "\n"
        "example:\n"
        " test_mpp_venc -v            display more detail\n"
        " test_mpp_venc -c 1 -t h264 -g 20  use venc channel 1, video type: h264, gopsize: 20\n"
        "\n");
}

static AR_S32 VbInit(AR_S32 height, AR_S32 width, PAYLOAD_TYPE_E type)
{
    AR_S32 s32Ret = 0;
    AR_S32 lumaStride = 0, chromaStride = 0;
    AR_U32 alignment  = (type == PT_H265 ? hevcAlignment : otherAlignment);


    if (cf50Ratio) {
        alignment = hevcAlignment = otherAlignment = 128;
        lumaStride = ALIGN_UP(width, alignment);
        chromaStride = ALIGN_UP(width / 2, alignment);
        lumaStride = (lumaStride*cf50Ratio)>>7;
        chromaStride = (chromaStride*cf50Ratio)>>7;
    } else {
        lumaStride = ALIGN_UP(width, alignment);
        chromaStride = ALIGN_UP(width / 2, alignment);
    }
    logline("y stride %d uv stride %d.\n", lumaStride, chromaStride);
/*
    s32Ret = AR_MPI_VB_Exit();

    if (s32Ret) {
        printf("vb exit failed\n");
        return s32Ret;
    }

    memset(&VbCfg, 0, sizeof(VbCfg));

    VbCfg.astCommPool[0].u64BlkSize = (lumaStride + chromaStride) * height;
    VbCfg.astCommPool[0].u32BlkCnt = YUV_FRAME_CACHED_NUM;
    s32Ret = AR_MPI_VB_SetConfig(&VbCfg);

    if (s32Ret) {
        printf("vb set config failed\n");
        //return s32Ret;
    }

    s32Ret = AR_MPI_VB_Init();

    if (s32Ret) {
        printf("vb init failed\n");
        return s32Ret;
    }
*/
    VB_POOL_CONFIG_S pstVbPoolCfg = {0};
    pstVbPoolCfg.u64BlkSize = (lumaStride + chromaStride) * height;
    pstVbPoolCfg.u32BlkCnt  = 8;
    dedicated_vbpool = AR_MPI_VB_CreatePool(&pstVbPoolCfg);
    if (dedicated_vbpool == VB_INVALID_POOLID) {
        logline("create dedicated_vbpool failed");
        return -1;
    }

    return 0;
}

/******************************************************************************
* funciton : save stream
******************************************************************************/
#if SUPPORT_NFS_WRITE
AR_S32 SaveStream2(FILE  *fout, VENC_STREAM_S* pstStream)
{
    AR_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++) {
        fwrite(pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               1,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset,
               fout);
        fflush(fout);
    }

    return AR_SUCCESS;
}
#else
AR_S32 SaveStream(int pFd, VENC_STREAM_S* pstStream)
{
    AR_S32 i;

    for (i = 0; i < pstStream->u32PackCount; i++) {
        write(pFd, pstStream->pstPack[i].pu8Addr + pstStream->pstPack[i].u32Offset,
               pstStream->pstPack[i].u32Len - pstStream->pstPack[i].u32Offset);
    }

    return AR_SUCCESS;
}
#endif

static AR_S32 VencSetParam(VENC_CHN_ATTR_S *stChnAttr, AR_S32 id, PAYLOAD_TYPE_E type, AR_S32 width, AR_S32 height, AR_S32 bitrate, AR_S32 gopsize, AR_S32 frameRate)
{
    AR_U32 alignment  = (type == PT_H265 ? hevcAlignment : otherAlignment);
    AR_S32 lumaStride = ALIGN_UP(width, alignment);
    /*
    AR_S32 chromaStride = ALIGN_UP(width / 2, alignment);
    */
    AR_S32 alignedHeight = ALIGN_UP(height, alignment);
    stChnAttr->stVencAttr.enType = type;
    stChnAttr->stVencAttr.u32PicWidth = width;
    stChnAttr->stVencAttr.u32PicHeight = height;
    /*
    stChnAttr->stVencAttr.u32MaxPicWidth = 3840;
    stChnAttr->stVencAttr.u32MaxPicHeight = 2160;
    */
    stChnAttr->stVencAttr.u32BufSize = bitrate * (1024 * 2)/8 + lumaStride * alignedHeight * 3 / 2;

    AR_U32 newU32BufSize = (bitrate * 1024) * bufScale / 8;
    stChnAttr->stVencAttr.bByFrame = AR_TRUE;
    stChnAttr->stVencAttr.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;
    stChnAttr->stVencAttr.stAttrH265e.u32CmdQueueDepth = 3;
    stChnAttr->stVencAttr.stAttrH265e.s32Cframe50Enable = cf50Ratio;
    if (cf50Ratio == 128) {
        stChnAttr->stVencAttr.stAttrH265e.s32Cframe50LosslessEnable = 1;
    } else {
        stChnAttr->stVencAttr.stAttrH265e.s32Cframe50Tx16Y = cf50Ratio;
        stChnAttr->stVencAttr.stAttrH265e.s32Cframe50Tx16C = cf50Ratio;
    }

    if (type == PT_H265) {
        stChnAttr->stRcAttr.enRcMode = rcMode;
        stChnAttr->stVencAttr.stAttrH265e.u8KeyFrameSizeMultiplier = 0;
        stChnAttr->stVencAttr.stAttrH265e.u8NonKeyFrameSizeMultiplier = 0;

        if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265FIXQP) {
            stChnAttr->stRcAttr.stH265FixQp.u32Gop = gopsize;
            stChnAttr->stRcAttr.stH265FixQp.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH265FixQp.u32SrcFrameRate = frameRate;
            stChnAttr->stRcAttr.stH265FixQp.u32IQp = 30;
            stChnAttr->stRcAttr.stH265FixQp.u32PQp = 33;
            stChnAttr->stRcAttr.stH265FixQp.u32BQp = 36;
            stChnAttr->stVencAttr.u32BufSize = lumaStride * alignedHeight * 3;
        } else if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265CBR) {
            stChnAttr->stRcAttr.stH265Cbr.u32Gop = gopsize;
            stChnAttr->stRcAttr.stH265Cbr.u32StatTime = statTime;
            stChnAttr->stRcAttr.stH265Cbr.u32SrcFrameRate = frameRate;
            stChnAttr->stRcAttr.stH265Cbr.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH265Cbr.u32BitRate = bitrate;
        } else if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265VBR) {
            stChnAttr->stRcAttr.stH265Vbr.u32SrcFrameRate  = frameRate;
            stChnAttr->stRcAttr.stH265Vbr.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH265Vbr.u32Gop           = gopsize;
            stChnAttr->stRcAttr.stH265Vbr.u32StatTime      = statTime;
            stChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate    = maxBitRate;
            newU32BufSize = stChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate * 1024 * 5 / 8;
            stChnAttr->stRcAttr.stH265Vbr.u32MinBitRate    = minBitRate;
            newU32BufSize = stChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate * 1024 * 5 / 8;
        }
    } else if (type == PT_H264) {
        stChnAttr->stRcAttr.enRcMode = rcMode;
        stChnAttr->stVencAttr.stAttrH264e.u8KeyFrameSizeMultiplier = 2;
        stChnAttr->stVencAttr.stAttrH264e.u8NonKeyFrameSizeMultiplier = 1;
        stChnAttr->stVencAttr.stAttrH264e.s32Cframe50Enable = cf50Ratio;
        if (cf50Ratio == 128) {
            stChnAttr->stVencAttr.stAttrH264e.s32Cframe50LosslessEnable = 1;
        } else {
            stChnAttr->stVencAttr.stAttrH264e.s32Cframe50Tx16Y = cf50Ratio;
            stChnAttr->stVencAttr.stAttrH264e.s32Cframe50Tx16C = cf50Ratio;
        }
        if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264FIXQP) {
            stChnAttr->stRcAttr.stH264FixQp.u32Gop = gopsize;
            stChnAttr->stRcAttr.stH264FixQp.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH264FixQp.u32SrcFrameRate = frameRate;
            stChnAttr->stRcAttr.stH264FixQp.u32IQp = 30;
            stChnAttr->stRcAttr.stH264FixQp.u32PQp = 33;
            stChnAttr->stRcAttr.stH264FixQp.u32BQp = 36;
            stChnAttr->stVencAttr.u32BufSize = lumaStride * alignedHeight * 3;
        } else if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264CBR) {
            stChnAttr->stRcAttr.stH264Cbr.u32Gop = gopsize;
            stChnAttr->stRcAttr.stH264Cbr.u32StatTime = statTime;
            stChnAttr->stRcAttr.stH264Cbr.u32SrcFrameRate = frameRate;
            stChnAttr->stRcAttr.stH264Cbr.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH264Cbr.u32BitRate = bitrate;
        } else if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264VBR) {
            stChnAttr->stRcAttr.stH264Vbr.u32SrcFrameRate  = frameRate;
            stChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRate = frameRate;
            stChnAttr->stRcAttr.stH264Vbr.u32Gop           = gopsize;
            stChnAttr->stRcAttr.stH264Vbr.u32StatTime      = statTime;
            stChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate    = maxBitRate;
            newU32BufSize = stChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate * 1024 * 5 / 8;
            stChnAttr->stRcAttr.stH264Vbr.u32MinBitRate    = minBitRate;
            newU32BufSize = stChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate * 1024 * 5 / 8;
        }
    } else {
        stChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
        stChnAttr->stRcAttr.stMjpegCbr.u32StatTime = statTime;
        stChnAttr->stRcAttr.stMjpegCbr.u32SrcFrameRate = frameRate;
        stChnAttr->stRcAttr.stMjpegCbr.fr32DstFrameRate = frameRate;
        stChnAttr->stRcAttr.stMjpegCbr.u32BitRate = bitrate;

        stChnAttr->stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
        stChnAttr->stRcAttr.stMjpegFixQp.fr32DstFrameRate = frameRate;
        stChnAttr->stRcAttr.stMjpegFixQp.u32Qfactor = u32Qfactor;
        stChnAttr->stRcAttr.stMjpegFixQp.u32SrcFrameRate = frameRate;
    }
    stChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;

    //stChnAttr->stGopAttr.enGopMode = VENC_GOPMODE_DUALP;
    if (stChnAttr->stGopAttr.enGopMode == VENC_GOPMODE_DUALP) {
        stChnAttr->stGopAttr.stDualP.s32IPQpDelta = -2;
        stChnAttr->stGopAttr.stDualP.u32SPInterval = 5;
    } else {
        stChnAttr->stGopAttr.stNormalP.s32IPQpDelta = -2;
    }

    if (stChnAttr->stVencAttr.u32BufSize < newU32BufSize) {
        logline("u32BufSize: using %u over %u", newU32BufSize, stChnAttr->stVencAttr.u32BufSize);
        stChnAttr->stVencAttr.u32BufSize = newU32BufSize;
    }

  return 0;
}

AR_S32 VencInit(AR_S32 id, AR_S32 num, PAYLOAD_TYPE_E type, AR_S32 width, AR_S32 height, AR_S32 bitrate, AR_S32 gopsize, AR_S32 frameRate, AR_U32 u32Profile)
{
    AR_S32 ret = 0;
    VENC_CHN_ATTR_S stChnAttr = { 0 };
    VENC_PARAM_MOD_S stParamMod = { 0 };
    VENC_H265_DBLK_S stH265Dblk = { 0 };
    VENC_CHN_PARAM_S stChnParam = { 0 };

    uint32_t fbSize = ALIGN_UP(width, 32)*ALIGN_UP(height, 32) + ALIGN_UP(ALIGN_UP(width, 32) / 2, 16)*ALIGN_UP(height, 32);
    logline("fbSize=%d", fbSize);
    uint32_t fbNum = 2;
    memset(&stChnAttr, 0, sizeof(stChnAttr));
    /* module parameters */
    stParamMod.enVencModType = MODTYPE_VENC;
    ret = AR_MPI_VENC_GetModParam(&stParamMod);
    stParamMod.stVencModParam.u32VencBufferCache = 1;
    ret = AR_MPI_VENC_SetModParam(&stParamMod);

    switch (type) {
        case PT_JPEG:
            stParamMod.enVencModType = MODTYPE_JPEGE;
        break;
        case PT_H264:
            stParamMod.enVencModType = MODTYPE_H264E;
            if (inst_parallelism)
                stParamMod.stH264eModParam.u64Flags |= AR_VIDEO_CODEC_PARAM_MPI_EXT_INSTANCE_PARALLESIM;
        case PT_H265:
            stParamMod.enVencModType = MODTYPE_H265E;
            if (inst_parallelism)
                stParamMod.stH265eModParam.u64Flags |= AR_VIDEO_CODEC_PARAM_MPI_EXT_INSTANCE_PARALLESIM;
        break;
        default:
            stParamMod.enVencModType = MODTYPE_BUTT;
        break;
    }
    AR_MPI_VENC_GetModParam(&stParamMod);
#if 1
    // For better performance on multi channel
    stParamMod.stH265eModParam.u32CoreClock = 500;
    stParamMod.stH265eModParam.u32BpuClock = 360;
#else
    stParamMod.stH265eModParam.u32CoreClock = 200;//1080p@30fps
    stParamMod.stH265eModParam.u32BpuClock = 150;
#endif
    stParamMod.stEventModParam.u32VencIrqQueueSize = 256;
    stParamMod.stEventModParam.u32VencTaskQueueSize = 256;
    stParamMod.stEventModParam.u32VencDoneQueueSize = 256;
    stParamMod.stEventModParam.u32VencOutQueueSize = 16;
    stParamMod.stEventModParam.u32VencEventQueueSize = 128;

    logline("inst_parallelism=%d", inst_parallelism);
    AR_MPI_VENC_SetModParam(&stParamMod);
    stParamMod.stH265eModParam.u32CoreClock = 0;
    stParamMod.stH265eModParam.u32BpuClock = 0;

    AR_MPI_VENC_GetModParam(&stParamMod);

    switch (type) {
        case PT_JPEG:
            logline("jpeg stParamMod.flags=%d", stParamMod.stJpegeModParam.u64Flags);
            break;
        case PT_H264:
            logline("h264 stParamMod.flags=%d", stParamMod.stH264eModParam.u64Flags);
            break;
        case PT_H265:
            logline("h265 stParamMod.flags=%d", stParamMod.stH265eModParam.u64Flags);
            break;
        default:
            stParamMod.enVencModType = MODTYPE_BUTT;
            break;
    }

    ret = VencSetParam(&stChnAttr, id, type, width, height, bitrate, gopsize, frameRate);
    if (ret != 0) {
        printf("dev id:%d set param failed! ret=%d\n", id, ret);
        return ret;
    } else {
        printf("+++++dev id%d set param success!++++\n", id);
    }

    stChnAttr.stVencAttr.u32Profile = u32Profile;

    ret = AR_MPI_VENC_CreateChn(id, &stChnAttr);
    if (ret) {
        printf("create channel failed\n");
        return ret;
    } else {
        inst_created = 1;
    }
    logline("AR_MPI_VENC_CreateChn success");
    AR_MPI_VENC_GetChnAttr(id, &stChnAttr);

    if (attach_vb_pool) {
        VB_POOL_CONFIG_S pstVbPoolCfg = {0};
        pstVbPoolCfg.u64BlkSize = fbSize;
        pstVbPoolCfg.u32BlkCnt  = fbNum;
        VB_POOL fb_vbpool = AR_MPI_VB_CreatePool(&pstVbPoolCfg);
        logline("fbSize=%d, fbNum=%d, fb_vbpool=%d", fbSize, fbNum, fb_vbpool);
        venc_fb_ex_pool.hPicVbPool = fb_vbpool;

        AR_MPI_VENC_AttachVbPool(id, &venc_fb_ex_pool);
    }

    logline("GetChnAttr frame rate %d dest frame rate %d, keyframe multiplier:%d, nonkeyframe multiplier:%d, fbSize:%u, fbNum:%u",
        stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate,
        stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate,
        stChnAttr.stVencAttr.stAttrH264e.u8KeyFrameSizeMultiplier,
        stChnAttr.stVencAttr.stAttrH264e.u8NonKeyFrameSizeMultiplier,
        fbSize, fbNum);
    AR_MPI_VENC_GetChnParam(id, &stChnParam);
    stChnParam.bColor2Grey = 0;
    #if 0
    stChnParam.stCropCfg.bEnable = 1;
    stChnParam.stCropCfg.stRect.s32X = 320;
    stChnParam.stCropCfg.stRect.u32Width = 1280;
    stChnParam.stCropCfg.stRect.s32Y = 180;
    stChnParam.stCropCfg.stRect.u32Height = 720;
    printf("set u32PollWakeUpFrmCnt to 1\n");
    stChnParam.u32PollWakeUpFrmCnt = 1;
    #endif
    ret = AR_MPI_VENC_SetChnParam(id, &stChnParam);
    if (ret) {
        fprintf(stderr, "get channel parameters failed 0x%x.", ret);
    }

    printf("codec type %d rcMode %d.\n", type, rcMode);
    if (type == PT_H265 && rcMode == VENC_RC_MODE_H265VBR) {
        logline();
        AR_MPI_VENC_GetH265Dblk(id, &stH265Dblk);
        stH265Dblk.slice_deblocking_filter_disabled_flag = 1;
        stH265Dblk.slice_beta_offset_div2 = 2;
        stH265Dblk.slice_tc_offset_div2 = 3;
        AR_MPI_VENC_SetH265Dblk(id, &stH265Dblk);

        VENC_RC_PARAM_S rc_param = {0};
        AR_MPI_VENC_GetRcParam(id, &rc_param);
        rc_param.stParamH265Vbr.u32MaxIQp   = maxIQp;
        rc_param.stParamH265Vbr.u32MinIQp   = minIQp;
        rc_param.stParamH265Vbr.u32MaxPQp   = maxPQp;
        rc_param.stParamH265Vbr.u32MinPQp   = minPQp;
        rc_param.stParamH265Vbr.u32MaxBQp   = maxBQp;
        rc_param.stParamH265Vbr.u32MinBQp   = minBQp;
        rc_param.stParamH265Vbr.u32MaxIprop = maxIpRatio;
        rc_param.stParamH265Vbr.u32MinIprop = minIpRatio;

        logline("maxIpratio %d, minIPRatio %d", maxIpRatio, minIpRatio);
        AR_MPI_VENC_SetRcParam(id, &rc_param);
    }
    else if (type == PT_H264 && rcMode == VENC_RC_MODE_H264VBR) {
        logline();
        VENC_RC_PARAM_S rc_param = {0};
        AR_MPI_VENC_GetRcParam(id, &rc_param);
        rc_param.stParamH264Vbr.u32MaxIQp   = maxIQp;
        rc_param.stParamH264Vbr.u32MinIQp   = minIQp;
        rc_param.stParamH264Vbr.u32MaxPQp   = maxPQp;
        rc_param.stParamH264Vbr.u32MinPQp   = minPQp;
        rc_param.stParamH264Vbr.u32MaxBQp   = maxBQp;
        rc_param.stParamH264Vbr.u32MinBQp   = minBQp;
        rc_param.stParamH264Vbr.u32MaxIprop = maxIpRatio;
        rc_param.stParamH264Vbr.u32MinIprop = minIpRatio;
        AR_MPI_VENC_SetRcParam(id, &rc_param);
    }
    else if (type == PT_H265 && rcMode == VENC_RC_MODE_H265CBR) {
        logline();
        VENC_RC_PARAM_S rc_param = {0};
        AR_MPI_VENC_GetRcParam(id, &rc_param);
        rc_param.stParamH265Cbr.u32MaxIQp   = maxIQp;
        rc_param.stParamH265Cbr.u32MinIQp   = minIQp;
        rc_param.stParamH265Cbr.u32MaxPQp   = maxPQp;
        rc_param.stParamH265Cbr.u32MinPQp   = minPQp;
        rc_param.stParamH265Cbr.u32MaxBQp   = maxBQp;
        rc_param.stParamH265Cbr.u32MinBQp   = minBQp;
        rc_param.stParamH265Cbr.u32MaxIprop = maxIpRatio;
        rc_param.stParamH265Cbr.u32MinIprop = minIpRatio;
        AR_MPI_VENC_SetRcParam(id, &rc_param);
    }
    else if (type == PT_H264 && rcMode == VENC_RC_MODE_H264CBR) {
        logline();
        VENC_RC_PARAM_S rc_param = {0};
        AR_MPI_VENC_GetRcParam(id, &rc_param);
        rc_param.stParamH264Cbr.u32MaxIQp   = maxIQp;
        rc_param.stParamH264Cbr.u32MinIQp   = minIQp;
        rc_param.stParamH264Cbr.u32MaxPQp   = maxPQp;
        rc_param.stParamH264Cbr.u32MinPQp   = minPQp;
        rc_param.stParamH264Cbr.u32MaxBQp   = maxBQp;
        rc_param.stParamH264Cbr.u32MinBQp   = minBQp;
        rc_param.stParamH264Cbr.u32MaxIprop = maxIpRatio;
        rc_param.stParamH264Cbr.u32MinIprop = minIpRatio;
        AR_MPI_VENC_SetRcParam(id, &rc_param);
    }
    else if (type == PT_JPEG) {
        VENC_JPEG_PARAM_S stJpegParam = { 0 };
        AR_MPI_VENC_GetJpegParam(id, &stJpegParam);
        printf("dev id:%d jpeg param u32Qfactor %u\n", id, stJpegParam.u32Qfactor);
        stJpegParam.u32Qfactor = u32Qfactor;
        AR_MPI_VENC_SetJpegParam(id, &stJpegParam);
    }

    if (rotationAngle != 0) {
        VENC_ROTATION_PARAM_S stRotate = {0};
        stRotate.s32RotationEnable = 1;
        stRotate.s32RotationAngle = rotationAngle;
        ret = AR_MPI_VENC_SetChnRotationParam(id, &stRotate);
        logline("AR_MPI_VENC_SetChnRotationParam ret %d", ret);
    }
    if (mirrorDirection != 0) {
        VENC_MIRROR_PARAM_S stMirror = {0};
        stMirror.s32MirrorEnable = 1;
        stMirror.s32MirrorDirection = mirrorDirection;
        ret = AR_MPI_VENC_SetChnMirroParam(id, &stMirror);
        logline("AR_MPI_VENC_SetChnMirroParam ret %d", ret);
    }
    if (!useBind) {
        VENC_RECV_PIC_PARAM_S param;
        param.s32RecvPicNum = -1;

        ret = AR_MPI_VENC_StartRecvFrame(id, &param);
        if (ret != 0) {
            logline("chn%d start failed! ret=%d", id, ret);
            return ret;
        } else {
            logline("chn%d start success", id);
            dev_started = 1;
        }
    }

    AR_MPI_VENC_GetChnAttr(id, &stChnAttr);
    logline("GetChnAttr frame rate %d dest frame rate %d, keyframe multiplier:%d, nonkeyframe multiplier:%d",
        stChnAttr.stRcAttr.stH265Cbr.u32SrcFrameRate,
        stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate,
        stChnAttr.stVencAttr.stAttrH264e.u8KeyFrameSizeMultiplier,
        stChnAttr.stVencAttr.stAttrH264e.u8NonKeyFrameSizeMultiplier);

    return ret;
}

static AR_S32 VencUnint(AR_S32 id)
{
    AR_S32 ret = 0;

    if (dev_opened) {
        ret = AR_MPI_VENC_CloseFd(dev_id);
        if (ret != 0) {
            printf("dev id:%d destroy inst failed! ret=%d\n", id, ret);
        } else {
            printf("++++++dev id:%d close success!------\n", id);
        }
    }

    if (dev_started) {
        ret = AR_MPI_VENC_StopRecvFrame(id);
        if (ret != 0) {
            printf("dev id:%d stop failed! ret=%d\n", id, ret);
        } else {
            printf("++++++dev id:%d stop success!------\n", id);
        }
    }

    if(useBind) {
        ret = unbindVindAndVenc(id);
        if (ret != 0) {
            printf("dev id:%d unbindVindAndVenc ! ret=%d\n", id, ret);
        } else {
            printf("++++++dev id:%d unbind success!------\n", id);
        }
        ar_hal_sys_bind_unregister_sender(AR_SYS_ID_VI);
    }

    if (inst_created) {
        ret = AR_MPI_VENC_DestroyChn(id);
        if (ret != 0) {
            printf("dev id:%d destroy inst failed! ret=%d\n", id, ret);
        } else {
            printf("++++++dev id:%d destroy inst success!------\n", id);
        }

        if (attach_vb_pool) {
            if (venc_fb_ex_pool.hPicVbPool != VB_INVALID_POOLID) {
                logline("releasing hPicVbPool");
                ret = AR_MPI_VB_DestroyPool(venc_fb_ex_pool.hPicVbPool);
                if (ret!=0)
                    logline("AR_MPI_VB_DestroyPool failed");
                venc_fb_ex_pool.hPicVbPool = VB_INVALID_POOLID;
            }

            if (venc_fb_ex_pool.hPicInfoVbPool != VB_INVALID_POOLID) {
                logline("releasing hPicInfoVbPool");
                ret = AR_MPI_VB_DestroyPool(venc_fb_ex_pool.hPicInfoVbPool);
                if (ret!=0)
                    logline("AR_MPI_VB_DestroyPool failed");
                venc_fb_ex_pool.hPicInfoVbPool = VB_INVALID_POOLID;
            }
        }
    }

    return ret;
}

static AR_S32 frameInit(VIDEO_FRAME_INFO_S *stFrame, VB_BLK blk, AR_S32 width, AR_S32 height, AR_U32 alignment)
{
    AR_S32 lumaStride = ALIGN_UP(width, alignment), chromaStride = ALIGN_UP(width / 2, alignment);

    stFrame->stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(blk);
    stFrame->stVFrame.u64PhyAddr[1] = stFrame->stVFrame.u64PhyAddr[0] + lumaStride * height;
    stFrame->stVFrame.u64PhyAddr[2] = stFrame->stVFrame.u64PhyAddr[1] + chromaStride * height / 2;
    stFrame->stVFrame.u64HeaderPhyAddr[0] = 0;
    stFrame->stVFrame.u64HeaderPhyAddr[1] = 0;
    stFrame->stVFrame.u64HeaderPhyAddr[2] = 0;
    stFrame->stVFrame.u64PTS = -1;
    stFrame->stVFrame.u32Width = width;
    stFrame->stVFrame.u32Height = height;
    stFrame->stVFrame.u32Stride[0] = ALIGN_UP(width, alignment);
    stFrame->stVFrame.u32Stride[1] = ALIGN_UP(width / 2, alignment);
    /*
    printf("%s, stride[0]=%d, stride[1]=%d, alignment=%d\n", __FUNCTION__, stFrame->stVFrame.u32Stride[0], stFrame->stVFrame.u32Stride[1], alignment);
    */
    stFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    return 0;
}

static AR_S32 frameInitCF50(VIDEO_FRAME_INFO_S *stFrame, VB_BLK blk, AR_S32 width, AR_S32 height, AR_S32 lumaStride, AR_S32 chromaStride)
{
    stFrame->stVFrame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(blk);
    stFrame->stVFrame.u64PhyAddr[1] = stFrame->stVFrame.u64PhyAddr[0] + lumaStride * height;
    stFrame->stVFrame.u64PhyAddr[2] = stFrame->stVFrame.u64PhyAddr[1] + chromaStride * height / 2;
    stFrame->stVFrame.u64HeaderPhyAddr[0] = 0;
    stFrame->stVFrame.u64HeaderPhyAddr[1] = 0;
    stFrame->stVFrame.u64HeaderPhyAddr[2] = 0;
    stFrame->stVFrame.u64PTS = -1;
    stFrame->stVFrame.u32Width = width;
    stFrame->stVFrame.u32Height = height;
    stFrame->stVFrame.u32Stride[0] = lumaStride;
    stFrame->stVFrame.u32Stride[1] = chromaStride;
    /*
    printf("%s, stride[0]=%d, stride[1]=%d, alignment=%d\n", __FUNCTION__, stFrame->stVFrame.u32Stride[0], stFrame->stVFrame.u32Stride[1], alignment);
    */
    stFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YVU_PLANAR_420;

    return 0;
}

static AR_S32 readCF50YUVToMemory(int fd, AR_VOID *frameVirtAddr, AR_S32 width, AR_S32 height, AR_S32 lumaStride, AR_S32 chromaStride) {

    AR_VOID *yaddr = frameVirtAddr;
    AR_S32 ret = 0;
    logline("width=%d, height=%d, lumaStride=%d, chromaStride=%d", width, height, lumaStride, chromaStride);
    for (int i = 0; i < height; i++) {
        ret = read(fd, yaddr, lumaStride);
        /*short read is treated as an error for there isn't
         * enough data for a complete frame left in file*/
        if (ret < lumaStride) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += lumaStride;
    }

    for (int i = 0; i < height / 2; i++) {
        ret = read(fd, yaddr, chromaStride);

        if (ret < chromaStride) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += chromaStride;
    }

    for (int i = 0; i < height / 2; i++) {
        ret = read(fd, yaddr, chromaStride);

        if (ret < chromaStride) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += chromaStride;
    }

    return (lumaStride  + chromaStride) * height;
}

static AR_S32 readYUVToMemory(int fd, AR_VOID *frameVirtAddr, AR_S32 width, AR_S32 height, AR_U32 alignment) {
    AR_S32 ret = 0, lumaStride = ALIGN_UP(width, alignment), chromaStride = ALIGN_UP(width / 2, alignment);
    AR_VOID *yaddr = frameVirtAddr;
    for (int i = 0; i < height; i++) {
        ret = read(fd, yaddr, width);
        /*short read is treated as an error for there isn't
         * enough data for a complete frame left in file*/
        if (ret < width) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += lumaStride;
    }

    for (int i = 0; i < height / 2; i++) {
        ret = read(fd, yaddr, width / 2);

        if (ret < width /2) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += chromaStride;
    }

    for (int i = 0; i < height / 2; i++) {
        ret = read(fd, yaddr, width / 2);

        if (ret < width / 2) {
            printf("[%s:%d], short read: %d vs %d\n", __FUNCTION__, __LINE__, width, ret);
            return -1;
        }

        yaddr += chromaStride;
    }

    return (lumaStride + chromaStride) * height;
}

static AR_S32 SendOneFrame(AR_S32 id, int fd, AR_S32 width, AR_S32 height, PAYLOAD_TYPE_E type)
{
    AR_S32 s32Ret;
    VIDEO_FRAME_INFO_S stFrame;
    VB_BLK blk;
    AR_S32 yuvFrameSize, lumaStride, chromaStride;
    AR_VOID *frameVirtAddr;
    VB_POOL poolId;
    AR_U32 alignment = (type == PT_H265 ? hevcAlignment : otherAlignment);

    if (cf50Ratio) {
        alignment = hevcAlignment = otherAlignment = 128;
        lumaStride = ALIGN_UP(width, alignment);
        chromaStride = ALIGN_UP(width / 2, alignment);
        lumaStride = (lumaStride*cf50Ratio)>>7;
        chromaStride = (chromaStride*cf50Ratio)>>7;
    } else {
        lumaStride = ALIGN_UP(width, alignment);
        chromaStride = ALIGN_UP(width / 2, alignment);
    }
#if 0
    yuvFrameSize = width * height * 3;
#else
    yuvFrameSize = (lumaStride + chromaStride) * height;
#endif

    blk = AR_MPI_VB_GetBlock(dedicated_vbpool, yuvFrameSize, NULL);

    if (blk == VB_INVALID_HANDLE) {
        logline("get block failed");
        return blk;
    }

    memset(&stFrame, 0, sizeof(stFrame));

    if (cf50Ratio) {
        s32Ret = frameInitCF50(&stFrame, blk, width, height, lumaStride, chromaStride);
    } else {
        s32Ret = frameInit(&stFrame, blk, width, height, alignment);
    }

    if (s32Ret) {
        printf("Frame init failed\n");
        return s32Ret;
    }
    if (sendcount < s32SkipNum) {
        if (lseek(fd, yuvFrameSize*s32SkipNum, SEEK_SET) == yuvFrameSize*s32SkipNum) {
            sendcount = s32SkipNum;
            logline("skip %d frames before encode start.\n", s32SkipNum);
        } else {
            logline("skip %d frames failed.\n", s32SkipNum);
        }
    }
    stFrame.stVFrame.u32FrameId = sendcount;
    stFrame.stVFrame.u64PTS = timer_get_us();
    poolId = AR_MPI_VB_Handle2PoolId(blk);
    if (poolId < 0) {
        printf("get pool id failed\n");
        goto out;
    }

    s32Ret = AR_MPI_VB_MmapPool(poolId);
    if (s32Ret) {
        printf("mmap pool failed\n");
        goto out;
    }

    s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId, stFrame.stVFrame.u64PhyAddr[0], &frameVirtAddr);
    if (s32Ret) {
        printf("get frame virtual address failed\n");
        goto out;
    }

    //printf("get block viraddr poolId = %d\n", poolId);

#if 0
    s32Ret = read(fd, frameVirtAddr, yuvFrameSize);
#else
    /*
    lseek(fd, 0, SEEK_SET); //rewind
    */
    if (cf50Ratio) {
        s32Ret = readCF50YUVToMemory(fd, frameVirtAddr, width, height, lumaStride, chromaStride);
    } else {
        s32Ret = readYUVToMemory(fd, frameVirtAddr, width, height, alignment);
    }
#endif

    if (s32Ret != yuvFrameSize) {
        printf("read frame failed s32Ret=%d\n",s32Ret);
        lseek(fd, 0, SEEK_SET); //rewind

        if (cf50Ratio) {
            s32Ret = readCF50YUVToMemory(fd, frameVirtAddr, width, height, lumaStride, chromaStride);
        } else {
            s32Ret = readYUVToMemory(fd, frameVirtAddr, width, height, alignment);
        }
        if (s32Ret != yuvFrameSize) {
            printf("short read after rewind, something must be wrong\n");
            goto out;
        }
    }

    if (insertSEI && (type == PT_H265 || type == PT_H264)) {
        uint8_t *buf = (uint8_t *) malloc(1024);
        if (buf) {
            memset(buf, 0, 1024);
            int strsize = sprintf(buf, "%s", "sei-buffer-0-123");
            int ret = AR_MPI_VENC_InsertUserData(id, buf, strsize + 1);
            logline("AR_MPI_VENC_InsertUserData ret %d", ret);
        }
        free(buf);
    }

    s32Ret = AR_MPI_VENC_SendFrame(id, &stFrame, -1);

    if (s32Ret) {
        logline("send frame failed %d", s32Ret);
        goto out;
    }

    sendcount++;
    /*
    logline("sendFrame %d", sendcount);

    */
    s32Ret = AR_MPI_VB_ReleaseBlock(blk);
    if (s32Ret) {
        printf("release yuv frame block failed\n");
        goto out;
    }

    s32Ret = AR_MPI_VB_MunmapPool(poolId);
    if (s32Ret) {
        printf("munmap pool failed\n");
        goto out;
    }

    /*
    printf("SendOneFrame success\n");
    */
    return 0;

out:
    s32Ret = AR_MPI_VB_ReleaseBlock(blk);

    if (s32Ret)
        printf("release block failed\n");

    return s32Ret;
}

static AR_S32 VinGetFrame(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    AR_S32 s32Ret;
    VB_BLK blk;
    AR_S32 yuvFrameSize;
    AR_VOID *frameVirtAddr;
    VB_POOL poolId;
    struct VinPara *data = (struct VinPara *)pv_priv_data;
    VIDEO_FRAME_INFO_S *stFrame = (VIDEO_FRAME_INFO_S *)pv_data;

    yuvFrameSize = data->width * data->height * 3 / 2;
    blk = AR_MPI_VB_GetBlock(POOL_OWNER_COMMON, yuvFrameSize, NULL);
    if (blk == VB_INVALID_HANDLE) {
        printf("get block failed\n");
        return blk;
    }
    memset(stFrame, 0, sizeof(*stFrame));

    if (cf50Ratio) {
        s32Ret = frameInitCF50(stFrame, blk, data->width, data->height, ALIGN_UP(data->width, 32), ALIGN_UP(data->width/2, 32));
    } else {
        s32Ret = frameInit(stFrame, blk, data->width, data->height, 16);
    }
    if (s32Ret) {
        printf("Frame init failed\n");
        return s32Ret;
    }

    poolId = AR_MPI_VB_Handle2PoolId(blk);
    if (poolId < 0) {
        printf("get pool id failed\n");
        goto out;
    }

    s32Ret = AR_MPI_VB_MmapPool(poolId);
    if (s32Ret) {
        printf("mmap pool failed\n");
        goto out;
    }

    s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId, stFrame->stVFrame.u64PhyAddr[0], &frameVirtAddr);
    if (s32Ret) {
        printf("get frame virtual address failed\n");
        goto out;
    }

    s32Ret = read(data->fd, frameVirtAddr, yuvFrameSize);
    if (s32Ret != yuvFrameSize) {
        printf("read frame failed, s32Ret=%d\n",s32Ret);
        lseek(data->fd, 0, SEEK_SET); //rewind
        goto out;
    }

    s32Ret = AR_MPI_VB_MunmapPool(poolId);
    if (s32Ret) {
        printf("munmap pool failed\n");
        goto out;
    }

    usleep(1000 / data->frameRate * 1000);

    return 0;
out:
    s32Ret = AR_MPI_VB_ReleaseBlock(blk);

    if (s32Ret)
        printf("release block failed\n");

    return s32Ret;
}

static AR_S32 VinReleaseFrame(AR_S32 s32_dev_id, AR_S32 s32_chn_id, const AR_VOID *pv_data, AR_VOID *pv_priv_data)
{
    AR_S32 s32Ret;
    VB_BLK blk;
    VIDEO_FRAME_INFO_S *stFrame = (VIDEO_FRAME_INFO_S *)pv_data;

    blk = AR_MPI_VB_PhysAddr2Handle(stFrame->stVFrame.u64PhyAddr[0]);
    if (blk == VB_INVALID_HANDLE) {
        printf("vin get block by phy address failed\n");
        return blk;
    }

    s32Ret = AR_MPI_VB_ReleaseBlock(blk);
    if (s32Ret) {
        printf("vin release frame failed\n");
        return s32Ret;
    }

    return 0;
}

AR_S32 unbindVindAndVenc(AR_S32 id)
{
    AR_S32 s32Ret = 0;

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDstChn;
    stSrcChn.enModId = AR_ID_VI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;

    stDstChn.enModId = AR_ID_VENC;
    stDstChn.s32DevId = 0;
    stDstChn.s32ChnId = id;

    s32Ret = AR_MPI_SYS_UnBind(&stSrcChn, &stDstChn);
    if (s32Ret) {
        printf("AR_MPI_SYS_UnBind failed %d\n",s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_SYS_Exit();
    if (s32Ret) {
        printf("AR_MPI_SYS_Exit failed %d\n",s32Ret);
        return s32Ret;
    }

    return s32Ret;
}

static AR_S32 bindVinAndVenc(int fd, AR_S32 id, AR_S32 width, AR_S32 height, AR_S32 frameRate)
{
    AR_S32 s32Ret;
    STRU_SYS_BIND_SENDER sender;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDstChn;

    s32Ret = AR_MPI_SYS_Init();
    if (s32Ret)
        return s32Ret;

    sender.e_mod_id = AR_SYS_ID_VI;
    sender.u32_max_dev_cnt = 1;
    sender.u32_max_chn_cnt = 4;
    sender.s32_flag = 0;
    sender.e_data_type = AR_SYS_DATA_VI_FRAME;
    sender.pv_priv_data = &vinData;
    sender.give_bind_call_back = NULL;
    sender.get_frame_call_back = VinGetFrame;
    sender.release_frame_call_back = VinReleaseFrame;

    vinData.fd = fd;
    vinData.width = width;
    vinData.height = height;
    vinData.frameRate = frameRate;

    s32Ret = ar_hal_sys_bind_register_sender(&sender);
    if (s32Ret) {
        printf("register sender bind failed, ret: %d\n", s32Ret);
        goto out;
    }

    stSrcChn.enModId = AR_ID_VI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;

    stDstChn.enModId = AR_ID_VENC;
    stDstChn.s32DevId = 0;
    stDstChn.s32ChnId = id;

    s32Ret = AR_MPI_SYS_Bind(&stSrcChn, &stDstChn);
    if (s32Ret) {
        printf("bind vi and venc failed\n");
        goto out;
    }

    return 0;
out:
      AR_MPI_SYS_Exit();
      return s32Ret;
}

static void dumpRcParam(VENC_RC_PARAM_S *pstRcParam) {
    if (rcMode == VENC_RC_MODE_H264VBR) {
        logline("s32CuOrMbLevelRcEnable=%d", pstRcParam->s32CuOrMbLevelRcEnable);
        logline("s32HvsQPEnable=%d",         pstRcParam->s32HvsQPEnable);
        logline("s32HvsQpScale=%d",          pstRcParam->s32HvsQpScale);
        logline("s32HvsMaxDeltaQp=%d",       pstRcParam->s32HvsMaxDeltaQp);
        logline("s32FirstFrameStartQp=%d",   pstRcParam->s32FirstFrameStartQp);
        logline("bQpMapEn=%d",               pstRcParam->stParamH264Vbr.bQpMapEn);
        logline("u32MinIprop=%d",            pstRcParam->stParamH264Vbr.u32MinIprop);
        logline("u32MaxIprop=%d",            pstRcParam->stParamH264Vbr.u32MaxIprop);
        logline("s32MaxReEncodeTimes=%d",    pstRcParam->stParamH264Vbr.s32MaxReEncodeTimes);
        logline("u32MaxBQp=%d",              pstRcParam->stParamH264Vbr.u32MaxBQp);
        logline("u32MinBQp=%d",              pstRcParam->stParamH264Vbr.u32MinBQp);
        logline("u32MaxPQp=%d",              pstRcParam->stParamH264Vbr.u32MaxPQp);
        logline("u32MinPQp=%d",              pstRcParam->stParamH264Vbr.u32MinPQp);
        logline("u32MaxIQp=%d",              pstRcParam->stParamH264Vbr.u32MaxIQp);
        logline("u32MinIQp=%d",              pstRcParam->stParamH264Vbr.u32MinIQp);
    } else if (rcMode == VENC_RC_MODE_H265VBR) {
        logline("s32CuOrMbLevelRcEnable=%d", pstRcParam->s32CuOrMbLevelRcEnable);
        logline("s32HvsQPEnable=%d",         pstRcParam->s32HvsQPEnable);
        logline("s32HvsQpScale=%d",          pstRcParam->s32HvsQpScale);
        logline("s32HvsMaxDeltaQp=%d",       pstRcParam->s32HvsMaxDeltaQp);
        logline("s32FirstFrameStartQp=%d",   pstRcParam->s32FirstFrameStartQp);
        logline("bQpMapEn=%d",               pstRcParam->stParamH265Vbr.bQpMapEn);
        logline("u32MinIprop=%d",            pstRcParam->stParamH265Vbr.u32MinIprop);
        logline("u32MaxIprop=%d",            pstRcParam->stParamH265Vbr.u32MaxIprop);
        logline("s32MaxReEncodeTimes=%d",    pstRcParam->stParamH265Vbr.s32MaxReEncodeTimes);
        logline("u32MaxBQp=%d",              pstRcParam->stParamH265Vbr.u32MaxBQp);
        logline("u32MinBQp=%d",              pstRcParam->stParamH265Vbr.u32MinBQp);
        logline("u32MaxPQp=%d",              pstRcParam->stParamH265Vbr.u32MaxPQp);
        logline("u32MinPQp=%d",              pstRcParam->stParamH265Vbr.u32MinPQp);
        logline("u32MaxIQp=%d",              pstRcParam->stParamH265Vbr.u32MaxIQp);
        logline("u32MinIQp=%d",              pstRcParam->stParamH265Vbr.u32MinIQp);
    }
}

static void dumpVencChnAttr(VENC_CHN_ATTR_S *stChnAttr) {
    logline("enRcMode=%d", stChnAttr->stRcAttr.enRcMode);
    if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H264VBR) {
        logline("stChnAttr->stRcAttr.stH264Vbr.u32Gop=%d",              stChnAttr->stRcAttr.stH264Vbr.u32Gop);
        logline("stChnAttr->stRcAttr.stH264Vbr.u32StatTime=%d",         stChnAttr->stRcAttr.stH264Vbr.u32StatTime);
        logline("stChnAttr->stRcAttr.stH264Vbr.u32SrcFrameRate=%d",     stChnAttr->stRcAttr.stH264Vbr.u32SrcFrameRate);
        logline("stChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRate=%d",    stChnAttr->stRcAttr.stH264Vbr.fr32DstFrameRate);
        logline("stChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate=%d",       stChnAttr->stRcAttr.stH264Vbr.u32MaxBitRate);
        logline("stChnAttr->stRcAttr.stH264Vbr.u32MinBitRate=%d",       stChnAttr->stRcAttr.stH264Vbr.u32MinBitRate);
    }
    if (stChnAttr->stRcAttr.enRcMode == VENC_RC_MODE_H265VBR) {
        logline("stChnAttr->stRcAttr.stH265Vbr.u32Gop=%d",              stChnAttr->stRcAttr.stH265Vbr.u32Gop);
        logline("stChnAttr->stRcAttr.stH265Vbr.u32StatTime=%d",         stChnAttr->stRcAttr.stH265Vbr.u32StatTime);
        logline("stChnAttr->stRcAttr.stH265Vbr.u32SrcFrameRate=%d",     stChnAttr->stRcAttr.stH265Vbr.u32SrcFrameRate);
        logline("stChnAttr->stRcAttr.stH265Vbr.fr32DstFrameRate=%d",    stChnAttr->stRcAttr.stH265Vbr.fr32DstFrameRate);
        logline("stChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate=%d",       stChnAttr->stRcAttr.stH265Vbr.u32MaxBitRate);
        logline("stChnAttr->stRcAttr.stH265Vbr.u32MinBitRate=%d",       stChnAttr->stRcAttr.stH265Vbr.u32MinBitRate);
    }
}

static void *rcvStream(void *arg)
{
    AR_S32 s32Ret = 0;
    int num, id;
    VENC_CHN_STATUS_S stStatus;
    VENC_STREAM_S stStream;
    struct ThreadPara *pArg;
    FILE *fout = NULL;
    /*
    static int s32FrameCount = 0;

    */
    pArg = (struct ThreadPara *)arg;

    num = pArg->picNum;
    id = pArg->id;

    if (mp4Packing){
#ifdef MP4_PACKING
        FfmpegConf ffmpegConfig = {0};
        memset(&ffmpegConfig, 0, sizeof(FfmpegConf));
        AR_PDT_CreateMp4(id, streamFile, type, width, height, frameRate, gopsize);
#endif
    } else {
#if SUPPORT_NFS_WRITE
        fout = fopen(streamFile, "wb");
        if (!fout){
            printf("open bitstream file %s failed: %s\n", streamFile, strerror(errno));
            return NULL;
        }
#else
        int fd = -1;
        fd = open(streamFile, O_CREAT | O_WRONLY);
        if (fd < 0){
            printf("open bitstream file failed!");
            return NULL;
        }
#endif
    }

    AR_S32 devFd = AR_MPI_VENC_GetFd(id);
    while(1) {
        if (gSuspend == 1) {
            logline("suspended, retry");
            usleep(500 * 1000);
            continue;
        }

        devFd = AR_MPI_VENC_GetFd(id);

        if(exit_enc) {
            logline("exit on exit_enc");
            break;
        }
        if(mp4Packing) {
            if (recv_count == 0)
            {
                if (AR_MPI_VENC_RequestIDR(id, AR_TRUE) != AR_SUCCESS)
                {
                    printf("request idr failed !!!\n");
                }
            }
        }
        AR_S32 timeout = 1;

        if (devFd < 0) {
            printf("AR_MPI_VENC_GetFd failed!\n");
            goto out;
        }

      AR_S32 ret = waitOnFd(devFd, timeout);

      if (ret == -1) {
          printf("fatal error\n");

          devFd = AR_MPI_VENC_GetFd(id);
          continue;
      } else if (ret == 0) {
          printf("waitOnFd time out after %d second\n", timeout);

          if(exit_enc)
              break;
          else
              continue;
      } else {
          //printf("encoder output ready\n");
      }

      s32Ret = AR_MPI_VENC_QueryStatus(id, &stStatus);
      if (s32Ret) {
          printf("dev id:%d query status failed! ret=%d\n", id, s32Ret);

          devFd = AR_MPI_VENC_GetFd(id);
          usleep(5 * 1000);
          continue;
      }
        /*
        printf("encoder output ready: status left stream frames:%d\n", stStatus.u32LeftStreamFrames);
        */

        /*
        if (stStatus.u32LeftStreamFrames > 0) {
            //printf("status left stream frames:%d\n", stStatus.u32LeftStreamFrames);
            break;
        }
        */

        //usleep(1 * 1000);

        /*
        if(exit_enc) {
            printf("exit on exit_enc\n");
            break;
        }
        */

        int nFrames = stStatus.u32LeftStreamFrames;
        /*
        printf("loop saving %d packt(s)\n", nFrames);
        */
        for (int i = 0; i < 1; i++) {
            stStream.pstPack = (VENC_PACK_S*)malloc(sizeof(VENC_PACK_S) * stStatus.u32CurPacks);
            if (NULL == stStream.pstPack) {
                printf("malloc memory failed!\n");
                goto out;
            }
            stStream.u32PackCount = stStatus.u32CurPacks;

            //usleep(50*1000);
            s32Ret = AR_MPI_VENC_GetStream(id, &stStream, -1);

            if (AR_SUCCESS != s32Ret) {

                ar_free(stStream.pstPack);
                stStream.pstPack = NULL;

                logline("AR_MPI_VENC_GetStream failed with %#x, continue", s32Ret);
                continue;
            }
            if (mp4Packing){
#ifdef MP4_PACKING
                s32Ret = AR_PDT_WriteVideo(id, &stStream, type);
                if (s32Ret != AR_SUCCESS)
                {
                    printf("AR_PDT_WriteVideo failed %d !!!! \n", s32Ret);
                }
#endif
            }
            else
            {
#if SUPPORT_NFS_WRITE
                s32Ret = SaveStream2(fout, &stStream);
#else
                s32Ret = SaveStream(fd, &stStream);
#endif

                if (AR_SUCCESS != s32Ret){
                    printf("save stream failed\n");
                    goto out;
                }
            }
            printf("saved stream count %d \n", ++s32FrameCount);
            /*******************************************************
             release stream
             *******************************************************/
            s32Ret = AR_MPI_VENC_ReleaseStream(id, &stStream);
            if (AR_SUCCESS != s32Ret) {
                printf("AR_MPI_VENC_ReleaseStream failed!\n");

                continue;
            }

            /*******************************************************
             free pack nodes
            *******************************************************/
            ar_free(stStream.pstPack);
            stStream.pstPack = NULL;

            recv_count++;

            /*
            if (recv_count == 10) {
                if ((testFlags & TEST_VBR) && (rcMode == VENC_RC_MODE_H264VBR || rcMode == VENC_RC_MODE_H265VBR)) {
                    VENC_RC_PARAM_S pstRcParam = {0};
                    AR_S32 ret = AR_MPI_VENC_GetRcParam(id, &pstRcParam);

                    //dumpRcParam(&pstRcParam);
                    if (ret == AR_SUCCESS) {
                        if (rcMode == VENC_RC_MODE_H264VBR) {
                            pstRcParam.stParamH264Vbr.u32MaxIprop = pstRcParam.stParamH264Vbr.u32MaxIprop + 1;
                            pstRcParam.stParamH264Vbr.u32MinIprop = pstRcParam.stParamH264Vbr.u32MinIprop +1;
                            pstRcParam.stParamH264Vbr.u32MaxIQp   = pstRcParam.stParamH264Vbr.u32MaxIQp + 1;
                            pstRcParam.stParamH264Vbr.u32MinIQp   = pstRcParam.stParamH264Vbr.u32MinIQp + 1;
                            pstRcParam.stParamH264Vbr.u32MaxPQp   = pstRcParam.stParamH264Vbr.u32MaxPQp + 1;
                            pstRcParam.stParamH264Vbr.u32MinPQp   = pstRcParam.stParamH264Vbr.u32MinPQp + 1;
                            pstRcParam.stParamH264Vbr.u32MaxBQp   = 1;
                            pstRcParam.stParamH264Vbr.u32MinBQp   = 1;
                        }

                        if (rcMode == VENC_RC_MODE_H265VBR) {
                            pstRcParam.stParamH265Vbr.u32MaxIprop = pstRcParam.stParamH265Vbr.u32MaxIprop + 1;
                            pstRcParam.stParamH265Vbr.u32MinIprop = pstRcParam.stParamH265Vbr.u32MinIprop +1;
                            pstRcParam.stParamH265Vbr.u32MaxIQp   = pstRcParam.stParamH265Vbr.u32MaxIQp + 1;
                            pstRcParam.stParamH265Vbr.u32MinIQp   = pstRcParam.stParamH265Vbr.u32MinIQp + 1;
                            pstRcParam.stParamH265Vbr.u32MaxPQp   = pstRcParam.stParamH265Vbr.u32MaxPQp + 1;
                            pstRcParam.stParamH265Vbr.u32MinPQp   = pstRcParam.stParamH265Vbr.u32MinPQp + 1;
                            pstRcParam.stParamH265Vbr.u32MaxBQp = 1;
                            pstRcParam.stParamH265Vbr.u32MinBQp = 1;
                        }

                        ret = AR_MPI_VENC_SetRcParam(id, &pstRcParam);

                        if (ret != AR_SUCCESS)
                            printf("[%s:%d]AR_MPI_VENC_SetRcParam failed", __FUNCTION__, __LINE__);

                        memset((void *)(&pstRcParam), 0, sizeof(VENC_RC_PARAM_S));

                        ret = AR_MPI_VENC_GetRcParam(id, &pstRcParam);

                        //dumpRcParam(&pstRcParam);

                        VENC_CHN_ATTR_S stChnAttr = { 0 };
                        AR_S32 ret = AR_MPI_VENC_GetChnAttr(id, &stChnAttr);
                        if (ret == AR_SUCCESS) {
                            //dumpVencChnAttr(&stChnAttr);
                            stChnAttr.stRcAttr.stH264Vbr.u32SrcFrameRate = stChnAttr.stRcAttr.stH264Vbr.u32SrcFrameRate + 1;
                            stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate = stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate + 1;
                            stChnAttr.stRcAttr.stH264Vbr.u32Gop = stChnAttr.stRcAttr.stH264Vbr.u32Gop + 1;
                            stChnAttr.stRcAttr.stH264Vbr.u32StatTime = stChnAttr.stRcAttr.stH264Vbr.u32StatTime + 1;
                            stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate = stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate + 1000;
                            stChnAttr.stRcAttr.stH265Vbr.u32MinBitRate = stChnAttr.stRcAttr.stH265Vbr.u32MinBitRate + 1000;

                            ret = AR_MPI_VENC_SetChnAttr(id, &stChnAttr);

                            memset((void *)(&stChnAttr), 0, sizeof(VENC_CHN_ATTR_S));
                            ret = AR_MPI_VENC_GetChnAttr(id, &stChnAttr);
                            //dumpVencChnAttr(&stChnAttr);
                        }

                    } else
                        printf("[%s:%d]AR_MPI_VENC_GetRcParam failed", __FUNCTION__, __LINE__);
                } else if ((testFlags & TEST_FIXQP) && (rcMode == VENC_RC_MODE_H264FIXQP || rcMode == VENC_RC_MODE_H265FIXQP)) {
                    VENC_CHN_ATTR_S stChnAttr = { 0 };
                    AR_S32 ret = AR_MPI_VENC_GetChnAttr(id, &stChnAttr);
                    if (ret == AR_SUCCESS) {
                        if (rcMode == VENC_RC_MODE_H264FIXQP) {
                            stChnAttr.stRcAttr.stH264FixQp.u32IQp = 35;
                            stChnAttr.stRcAttr.stH264FixQp.u32PQp = 38;
                        }
                        if (rcMode == VENC_RC_MODE_H265FIXQP) {
                            stChnAttr.stRcAttr.stH265FixQp.u32IQp = 35;
                            stChnAttr.stRcAttr.stH265FixQp.u32PQp = 38;
                        }
                        ret = AR_MPI_VENC_SetChnAttr(id, &stChnAttr);
                        if (ret != AR_SUCCESS)
                            printf("[%s:%d]AR_MPI_VENC_SetChnAttr failed", __FUNCTION__, __LINE__);
                    }
                }
            }
            */

            if(num>=0 && recv_count >= num) {
                printf("done encoding, recv_count=%d\n", recv_count);
                exit_enc = 1;
                gloablRun = 0;
                break;
            }
        }
    }

out:
if(mp4Packing) {
#ifdef MP4_PACKING
    AR_PDT_CloseMp4();
#endif
} else {
#if SUPPORT_NFS_WRITE
    fflush(fout);
    fclose(fout);
    printf("output: %s\n", streamFile);
#else
    close(fd);
#endif
}
    return NULL;
}

static AR_S32 VencRun(AR_S32 id, AR_S32 num, AR_S32 width, AR_S32 height, AR_S32 frameRate, PAYLOAD_TYPE_E type)
{
    AR_S32 s32Ret;
    int fd;
    pthread_t thread;
    struct ThreadPara stPara;

    stPara.picNum = num;
    stPara.id = id;

    fd = open(yuvFile, O_RDONLY);
    if (fd < 0) {
        printf("open yuv file failed!");
        return AR_FAILURE;
    }

    s32Ret = pthread_create(&thread, NULL, rcvStream, &stPara);
    if (s32Ret) {
        printf("thread create failed\n");
        close(fd);
        return s32Ret;
    }

    if (insertSEI && (type == PT_H265 || type == PT_H264))
    {
        uint8_t *buf = (uint8_t *) malloc(1024);
        if (buf)
            memset(buf, 0, 1024);
        else {
            close(fd);
            return -1;
        }
        int strsize = sprintf(buf, "%s", "sei-buffer-0-123");
        int ret = AR_MPI_VENC_InsertUserData(id, buf, strsize + 1);
        logline("AR_MPI_VENC_InsertUserData ret %d", ret);

        memset(buf, 0, 1024);
        strsize = sprintf(buf, "%s", "sei-buffer-1-abcdef\n");
        ret = AR_MPI_VENC_InsertUserData(id, buf, strsize + 1);
        logline("AR_MPI_VENC_InsertUserData ret %d", ret);

        memset(buf, 0, 1024);
        strsize = sprintf(buf, "%s", "sei-buffer-2-artosyn\n");
        ret = AR_MPI_VENC_InsertUserData(id, buf, strsize + 1);
        logline("AR_MPI_VENC_InsertUserData ret %d", ret);

        memset(buf, 0, 1024);
        strsize = sprintf(buf, "%s", "sei-buffer-3-cnm-wave521c\n");
        ret = AR_MPI_VENC_InsertUserData(id, buf, strsize + 1);
        logline("AR_MPI_VENC_InsertUserData ret %d", ret);

        ar_free(buf);
    }
    VENC_STREAM_BUF_INFO_S buf_info = {0};
    s32Ret = AR_MPI_VENC_GetStreamBufInfo(id, &buf_info);
    logline("AR_MPI_VENC_GetStreamBufInfo, ret=%d, %p, %p, %llu",
            s32Ret, buf_info.u64PhyAddr[0], buf_info.pUserAddr[0], buf_info.u64BufSize[0]);

    gloablRun = 1;
    if (useBind) {
        VENC_RECV_PIC_PARAM_S param;
        param.s32RecvPicNum = -1;

        s32Ret = bindVinAndVenc(fd, id, width, height, frameRate);
        if (s32Ret) {
            printf("bind vin and venc failed\n");
            close(fd);
            return s32Ret;
        }

        printf("bind vin and venc suceess\n");

        s32Ret = AR_MPI_VENC_StartRecvFrame(id, &param);
        if (s32Ret != 0) {
            printf("dev id:%d start failed! ret=%d\n", id, s32Ret);
            return s32Ret;
        } else {
            printf("+++++dev id:%d start success!++++\n", id);
            dev_started = 1;
        }
    } else {
        while(gloablRun) {
            if (gSuspend) {
                logline("suspended");
                sleep(1);
                continue;
            }

            //for (i = 0; i < num; i++) {
            /*
            if (sendcount - recv_count >= 5) {
                logline("sendcount=%d recv_count=%d", sendcount, recv_count);
                usleep(1000 / frameRate * 1000);
                continue;
            }
            */

            s32Ret = SendOneFrame(id, fd, width, height, type);
            if (s32Ret) {
                logline("send one frame failed");
                continue;
            }
            logline("send frame, sendcount=%d", sendcount);
            /*
            send_count++;
            if (num>=0 && send_count >= num)
                break;
            */

            usleep(1000 / frameRate * 1000);
        }
    }

    pthread_join(thread, NULL);

    close(fd);

    return 0;
}

int main(int argc, char *argv[])
{
    AR_S32 ret = 0, c;
    AR_S32 verbose = 0;
    gSuspend = 0;
    while ((c = getopt_long(argc, argv, ":vBc:f:n:t:h:w:b:g:i:o:H:P:0:1:2:3:4:5:6:7:8:9:a:e:j:k:l:q:Q:dO:R:M:C:S:", long_options, NULL)) != EOF) {
        switch (c) {
        case 'v':
            verbose = 1;
            break;
        case 'B':
            useBind = 1;
            break;
        case 'c':
            id = atoi(optarg);
            break;
        case 'n':
            picNum = atoi(optarg);
            break;
        case 'f':
            frameRate = atoi(optarg);
            break;
        case 't':
            if (strcmp(optarg, "h265") == 0)
                type = PT_H265;
            else if (strcmp(optarg, "h264") == 0)
                type = PT_H264;
            else if (strcmp(optarg, "jpeg") == 0)
                type = PT_JPEG;
            else if (strcmp(optarg, "mjpeg") == 0)
                type = PT_MJPEG;
            else
                type = PT_H265;
            break;
        case 'w':
            width = atoi(optarg);
            break;
        case 'h':
            height = atoi(optarg);
            break;
        case 'b':
            bitrate = atoi(optarg);
            break;
        case 'g':
            gopsize = atoi(optarg);
            break;
        case 'i':
            memset(yuvFile, 0, sizeof(yuvFile));
            memcpy(yuvFile, optarg, strlen(optarg));
            break;
        case 'o':
            memset(streamFile, 0, sizeof(streamFile));
            memcpy(streamFile, optarg, strlen(optarg));
            break;
        case '0':
            statTime = atoi(optarg);
            break;
        case '1':
            maxBitRate = atoi(optarg);
            break;
        case '2':
            minBitRate = atoi(optarg);
            break;
        case '3':
            maxIpRatio = atoi(optarg);
            break;
        case '4':
            minIpRatio = atoi(optarg);
            break;
        case '5':
            maxIQp = atoi(optarg);
            break;
        case '6':
            minIQp = atoi(optarg);
            break;
        case '7':
            maxPQp = atoi(optarg);
            break;
        case '8':
            minPQp = atoi(optarg);
            break;
        case '9':
            maxBQp = atoi(optarg);
            break;
        case 'a':
            minBQp = atoi(optarg);
            break;
        case 'd':
            logline("optarg=%s", optarg);
            if (strncmp(optarg, "fixqp", 5) == 0)
                rcMode = VENC_RC_MODE_H264FIXQP;
            else if (strncmp(optarg, "vbr", 3) == 0)
                rcMode = VENC_RC_MODE_H264VBR;
            else
                rcMode = VENC_RC_MODE_H264CBR;
            break;
        case 'e':
            inst_parallelism = atoi(optarg);
            logline("inst_parallelism=%d", inst_parallelism);
        case 'P':
            profile = atoi(optarg);
            break;
        case 'O':
            mp4Packing = 1;
            break;
        case 'j':
            bufScale = atoi(optarg);
            logline("bufScale=%d", bufScale);
            break;
        case 'k':
            insertSEI = atoi(optarg);
            logline("insertSEI=%d", insertSEI);
            break;
        case 'q':
            testFlags = atoi(optarg);
            logline("testFlags=%d", testFlags);
        case 'l':
            attach_vb_pool = atoi(optarg);
            logline("attach_vb_pool=%d", attach_vb_pool);
            break;
        case 'Q':
            u32Qfactor = atoi(optarg);
            logline("u32Qfactor = %u.", u32Qfactor);
            break;
        case 'C':
            cf50Ratio = atoi(optarg);
            logline("Cf50Ration = %d.\n", cf50Ratio);
            break;
        case 'R':
            rotationAngle = atoi(optarg);
            logline("rotationAngle=%d", rotationAngle);
            break;
        case 'M':
            mirrorDirection = atoi(optarg);
            logline("mirrorDirection=%d", mirrorDirection);
            break;
        case 'S':
            s32SkipNum = atoi(optarg);
            logline("s32SkipFrameNum=%d.\n", s32SkipNum);
            break;
        case 'H':
        default:
            /* ignore unknown options */
            usage();
            exit(0);
            break;
        }
    }

    /* non-option has only one, image-file */
    if (argc > optind) {
        usage();
        exit(0);
    }

    if (type == PT_H265) {
        if (rcMode == VENC_RC_MODE_H264FIXQP)
            rcMode = VENC_RC_MODE_H265FIXQP;
        else if (rcMode == VENC_RC_MODE_H264VBR)
            rcMode = VENC_RC_MODE_H265VBR;
        else
            rcMode = VENC_RC_MODE_H265CBR;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handle_sigsegv;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    AR_MPI_SYS_Exit();

    AR_MPI_SYS_Init();

    ret = ar_log_init();

    int soc_type = ar_hal_sys_get_soc_id();
    if (soc_type == ARS_31 || soc_type == AR_9311) {
        g_phandle = AR_MPI_SYSCTL_Register(argv[1], 0, &sample_venc_callback);
       if(!g_phandle){
            logline("register %s failed!", "test_mpp_venc");
            return -1;
        }
        logline("AR_MPI_SYSCTL_Register success");
    }

    //system("echo all=3 > /proc/umap/mpp_log");
    usleep(1000 * 1000);

    if (ret) {
        printf("log init failedwtf\n");
        return ret;
    }
    if (verbose)
        printf("log init ok\n");

    /*AR_S32 yuvFrameSize, lumaStride, chromaStride;
    AR_VOID *frameVirtAddr;
    VB_POOL poolId;*/

    ret = VbInit(height, width, type);
    if (ret) {
        printf("vb init failed\n");
        return ret;
    }

    if (verbose)
        printf("vb init ok\n");

    ret = VencInit(id, picNum, type, width, height, bitrate, gopsize, frameRate, profile);
    if (ret) {
        printf("init venc failed\n");
        return ret;
    }
    if (verbose)
        printf("venc init ok\n");

    ret = VencRun(id, picNum, width, height, frameRate, type);
    if (ret) {
        printf("run venc failed\n");
        goto exit;
    }
    if (verbose)
        printf("venc run ok\n");

    while(gloablRun) {
        if(recv_count >= picNum && picNum >= 0)
            break;

        sleep(1);
    }

    sleep(1);
exit:
    ret = VencUnint(id);
    if (ret) {
        printf("venc uninit failed\n");
        return ret;
    }
    if (verbose)
        printf("venc uninit ok, venc run success ^_^\n");

    /*
    ret = AR_MPI_VB_Exit();
    */

    if (dedicated_vbpool != VB_INVALID_POOLID) {
        ret = AR_MPI_VB_DestroyPool(dedicated_vbpool);
        if (ret)
            logline("AR_MPI_VB_DestroyPool on pool %d failed", dedicated_vbpool);
    }

    if (ret) {
        printf("vb exit failed\n");
        return ret;
    }
    if (verbose)
        printf("vb exit ok\n");
    AR_MPI_SYS_Exit();

    return 0;
}
