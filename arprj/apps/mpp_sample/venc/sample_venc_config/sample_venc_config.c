#include <stdio.h>
#include <sys/types.h>
#include <getopt.h>
#include "ar_comm_venc.h"

AR_BOOL bDebug = AR_FALSE;

#define MAX_CHANNEL_NUM  8
#define MAX_VENC_STREAM_PER_CHN    3
#define GET_VENC_CHN_ID(ch, stream) ((ch) * MAX_VENC_STREAM_PER_CHN + (stream))


#define CBR 0
#define VBR 1

#define H264 0
#define H265 1

#define RED     "\033[31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define NONE    "\033[0m"

 AR_VOID venc_log_func(const char *level, const char *file, int line, const char *func, const char *format, ...){
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


#define LOG_PRINT(level, ...) venc_log_func(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LOG_DEBUG(...) LOG_PRINT("DEBUG", __VA_ARGS__);
#define LOG_ERROR(...) LOG_PRINT("ERROR", __VA_ARGS__);
#define LOG_INFO(...)  LOG_PRINT("INFO", __VA_ARGS__);


static AR_VOID usage(const char* argv0)
{
    printf("Usage:\n");
    printf("Options:\n");
    printf("    -c  <int>\n");
    printf("            Channel index, range: [0, %d]\n", MAX_CHANNEL_NUM - 1);
    printf("    -s  <int>\n");
    printf("            Stream index, range: [0, %d]\n", MAX_VENC_STREAM_PER_CHN - 1);
    printf("    -t  <int>\n");
    printf("        Codec type, 0:H264,1:H265, default 0\n");
    printf("    --brc  <int>\n");
    printf("         Bitrate type, 0:cbr, 1:vbr, default 0\n");
    printf("    --avg  <int>\n");
    printf("       average bitrate for CBR\n");
    printf("    --brc  <int>\n");
    printf("         Bitrate type, 0:cbr, 1:vbr, default 0\n");
    printf("    --avg  <int>\n");
    printf("       average bitrate for CBR, no default\n");
    printf("    --min  <int>\n");
    printf("       min bitrate for CBR, no default\n");
    printf("    --max  <int>\n");
    printf("       max bitrate for CBR, no default\n");
    printf("    --fps  <int>\n");
    printf("       dst fps, default 30\n");
    return;
}

static const char const *short_options="c:s:m:b:a:n:m";
static struct option long_options[] = {

    {"brc", required_argument, 0, 'b' },
    {"avg", required_argument, 0, 'a'},
    {"min", required_argument, 0, 'n' },
    {"max", required_argument, 0, 'm' },
    {"fps", required_argument, 0, 'f' },

    {0, 0, 0, 0 }

};


static AR_U32 SAMPLE_GET_RcMode(AR_U32 u32VencType, AR_U32 u32BrcMode)
{
    if(u32VencType == PT_H264)
    {
        if(u32BrcMode == CBR)
        {
            return VENC_RC_MODE_H264CBR;
        }
        if(u32BrcMode == VBR)
        {
            return VENC_RC_MODE_H264VBR;
        }
    }
    else if(u32VencType == PT_H265)
    {
        if(u32BrcMode == CBR)
        {
            return VENC_RC_MODE_H265CBR;
        }
        if(u32BrcMode == VBR)
        {
            return VENC_RC_MODE_H265VBR;
        }
    }
    else
    {
        if(u32BrcMode == CBR)
        {
            return VENC_RC_MODE_MJPEGCBR;
        }
        if(u32BrcMode == VBR)
        {
            return VENC_RC_MODE_MJPEGVBR;
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        usage(argv[0]);
        return -1;
    }

    AR_S32 s32Ret = AR_FAILURE;
    int chnIdx = 0;
    int streamIdx = 0;
    int codecType = 1;
    int brc = 0;
    int avg = -1;
    int min = -1;
    int max = -1;
    int fps = -1;

    int opt;
    while ((opt=getopt_long(argc, argv, short_options, long_options, NULL)) != EOF)
    {
        switch (opt) {
            case 'c':
                chnIdx = atoi(optarg);
                break;
            case 's':
                streamIdx = atoi(optarg);
                break;
            case 't':
                codecType = atoi(optarg);
                break;
            case 'b':
                brc = atoi(optarg);
                break;
            case 'a':
                avg = atoi(optarg);
                break;
            case 'n':
                min = atoi(optarg);
                break;
            case 'm':
                max = atoi(optarg);
                break;
            case 'f':
                fps = atoi(optarg);
                break;
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    if (chnIdx >= MAX_CHANNEL_NUM ) {
        LOG_ERROR("Maximum channel exceeded!!\n");
        exit(1);
    }
    if (streamIdx >= MAX_VENC_STREAM_PER_CHN ) {
        LOG_ERROR("Maximum stream exceeded!!\n");
        exit(1);
    }

    if (min == -1 && max == -1 && avg == -1) {
        LOG_ERROR("more argument need!!\n");
        exit(1);
    }

    VENC_CHN VeChn = GET_VENC_CHN_ID(chnIdx, streamIdx);
    VENC_CHN_ATTR_S stChnAttr = {0};

#if 1
    s32Ret = AR_MPI_VENC_GetFd(VeChn);
    if(s32Ret < 0)
    {
        LOG_ERROR("Get Venc fd failed.\n");
        exit(1);
    }
#endif

    s32Ret = AR_MPI_VENC_GetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        LOG_ERROR("Get Venc chn attr failed.\n");
        exit(1);
    }

    if (codecType != -1) {
        stChnAttr.stVencAttr.enType = (codecType == H265) ? PT_H265:PT_H264;
    }


    if (brc != -1) {
        stChnAttr.stRcAttr.enRcMode = SAMPLE_GET_RcMode(stChnAttr.stVencAttr.enType, brc);
    }

    int current_fps = 0;

    if (avg > 0) {
        if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264CBR) {
            stChnAttr.stRcAttr.stH264Cbr.u32BitRate = avg;
            current_fps = stChnAttr.stRcAttr.stH264Cbr.fr32DstFrameRate;
        }
        else if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265CBR)
        {
            stChnAttr.stRcAttr.stH265Cbr.u32BitRate = avg;
            current_fps = stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate;
        }
    }

    if ((min != -1) && (max != -1)) {
        if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H264VBR) {
            stChnAttr.stRcAttr.stH264Vbr.u32MaxBitRate = max; //Unit Kbps;
            stChnAttr.stRcAttr.stH264Vbr.u32MinBitRate = min;
            current_fps = stChnAttr.stRcAttr.stH264Vbr.fr32DstFrameRate;
        }
        else if (stChnAttr.stRcAttr.enRcMode == VENC_RC_MODE_H265VBR)
        {
            stChnAttr.stRcAttr.stH265Vbr.u32MaxBitRate = max; //Unit Kbps;
            stChnAttr.stRcAttr.stH265Vbr.u32MinBitRate = min;
            current_fps = stChnAttr.stRcAttr.stH265Vbr.fr32DstFrameRate;
        }
    }

    if (fps <= 0 && current_fps <= 0 ) {
        LOG_ERROR("fps required!\n");
        exit(1);
    }

    if (fps > 0) {
        stChnAttr.stRcAttr.stH265Cbr.fr32DstFrameRate = fps;
    }

    s32Ret = AR_MPI_VENC_SetChnAttr(VeChn, &stChnAttr);
    if(s32Ret < 0)
    {
        LOG_ERROR("Set Venc chn attr failed.\n");
        exit(1);
    }
    LOG_INFO("VENC parameter set success! \n");

#if 1
    AR_MPI_VENC_CloseFd(VeChn);
#endif
    exit(0);
}
