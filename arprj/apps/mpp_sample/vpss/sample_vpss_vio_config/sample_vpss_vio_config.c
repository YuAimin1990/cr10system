#include <stdio.h>
#include "sample_vpss_vio_config.h"

AR_BOOL bDebug = AR_FALSE;


#define RED     "\033[31m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define NONE    "\033[0m"

 AR_VOID vpss_log_func(const char *level, const char *file, int line, const char *func, const char *format, ...){
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


#define LOG_PRINT(level, ...) vpss_log_func(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#define LOG_DEBUG(...) LOG_PRINT("DEBUG", __VA_ARGS__);
#define LOG_ERROR(...) LOG_PRINT("ERROR", __VA_ARGS__);
#define LOG_INFO(...)  LOG_PRINT("INFO", __VA_ARGS__);

typedef struct {
    ar_ipc_state_t* ipc;
    uint32_t handle;
} ipc_client_context_t;

static int AR_SAMPLE_VPSS_client_ops(ipc_client_context_t* context, SAMPLE_VPSS_CONFIG_CMD_E cmd, void* buffer, int len, void* out)
{
    assert(context);
    assert(cmd > CMD_VPSS_CONFIG_NONE && cmd < CMD_VPSS_CONFIG_BUTT);
    assert(buffer);
    assert(len);

    int ret = ar_ipc_send_buffer(context->ipc, context->handle, cmd, buffer, len, out, 0);

    return ret;
}

static int AR_SAMPLE_VPSS_create_ipc_client(ipc_client_context_t* context)
{
    context->ipc = ar_ipc_create();
    if (!context->ipc) {
        LOG_ERROR("failed to create ipc client");
        return -1;
    }

    context->handle = ar_ipc_get_service(context->ipc, "vpss_control");
    if (!context->handle) {
        LOG_ERROR("failed to get vpss_control service");
        return -1;
    }

    return 0;
}

static AR_VOID usage(const char* argv0)
{
    printf("Usage:\n");
    printf("%s -g 0 -c 0 -S 1280 720 -R 1 -M -F\n", argv0);
    printf("Options:\n");
    printf("    -g  <int>\n");
    printf("            Group index, range: [0, %d]\n", VPSS_MAX_GRP_NUM - 1);
    printf("    -c  <int>\n");
    printf("            Channel index, range: [0, %d]\n", VPSS_MAX_CHN_NUM - 1);
    printf("    -S  <int> <int>\n");
    printf("            Scale paramters: <width> <height>\n");
    printf("    -M  <No arg>\n");
    printf("            Enable mirror\n");
    printf("    -F  <No arg>\n");
    printf("            Enable flip\n");
    printf("    -R  <int>\n");
    printf("            Rotate angle, 1: 90-degrees, 2: 180-degrees, 3: 270-degrees\n");
    printf("    -h  <No arg>\n");
    printf("            help message\n");

    return;
}

int main(int argc, char *argv[])
{
    if(argc < 2){
        usage(argv[0]);
        return -1;
    }

    AR_S32 s32Ret = AR_FAILURE;

    SIZE_S stScaleSize = {0};
    RECT_S stCropRect = {0};
    ROTATION_E eRotateAngle = ROTATION_0;
    AR_BOOL bMirror = AR_FALSE;
    AR_BOOL bFlip = AR_FALSE;
    SAMPLE_VPSS_CONFIG_S stParam = {0};
    stParam.grpId = -1;
    stParam.chnId = -1;

    AR_BOOL bScale = AR_FALSE;
    AR_BOOL bCrop = AR_FALSE;
    VPSS_CROP_COORDINATE_E eCropType = 1;
    AR_BOOL bRotate = AR_FALSE;
    AR_BOOL bFisheye = AR_FALSE;
    AR_BOOL bLdc = AR_FALSE;
    AR_BOOL bSpread = AR_FALSE;
    AR_BOOL bCfgRgnLuma = AR_FALSE;
    SAMPLE_VIDEO_REGION_INFO_S  regionLuma = {0};
    AR_S32  u32BindChn = -1;

    int opt;
    while ((opt=getopt(argc, argv, "g:c:b:S:MFC:R:U:qlfpstdh")) != -1)
    {
        switch (opt) {
            case 'g':
                stParam.grpId = atoi(optarg);
                break;
            case 'c':
                stParam.chnId = atoi(optarg);
                break;
            case 'b':
                u32BindChn = atoi(optarg);
                break;
            case 'S':
                stScaleSize.u32Width = atoi(argv[optind-1]);
                stScaleSize.u32Height = atoi(argv[optind]);
                bScale = AR_TRUE;
                break;
            case 'M':
                bMirror = AR_TRUE;
                break;
            case 'F':
                bFlip = AR_TRUE;
                break;
            case 'C':
                eCropType = atoi(argv[optind-1]);   // 0 ratio, 1 abs
                stCropRect.s32X = atoi(argv[optind]);
                stCropRect.s32Y = atoi(argv[optind+1]);
                stCropRect.u32Width = atoi(argv[optind+2]);
                stCropRect.u32Height = atoi(argv[optind+3]);
                bCrop = AR_TRUE;
                break;
            case 'R':
                eRotateAngle = atoi(optarg);
                bRotate = AR_TRUE;
                break;
			case 'U':
				bCfgRgnLuma = AR_TRUE;
				if(atoi(argv[optind-1]) < 0){
					regionLuma.u32RegionNum = 0;
				}else if(atoi(argv[optind-1]) > SAMPLE_VIDEO_REGION_MAX_CNT){
					regionLuma.u32RegionNum = SAMPLE_VIDEO_REGION_MAX_CNT;
				}else{
					regionLuma.u32RegionNum = atoi(argv[optind-1]);
				}

				for(int i=0; i<regionLuma.u32RegionNum; i++){
					regionLuma.pstRegion[i].s32X = atoi(argv[optind+i*4]);
					regionLuma.pstRegion[i].s32Y = atoi(argv[optind+i*4+1]);
					regionLuma.pstRegion[i].u32Width = atoi(argv[optind+i*4+2]);
					regionLuma.pstRegion[i].u32Height = atoi(argv[optind+i*4+3]);
				}
				break;
            case 'f':
                bFisheye = AR_TRUE;
                break;
            case 'l':
                bLdc = AR_TRUE;
                break;
            case 'p':
                bSpread = AR_TRUE;
                break;
            case 's':
                stParam.startVpss = AR_TRUE;
                break;
            case 't':
                stParam.stopVpss = AR_TRUE;
                break;
            case 'q':
                stParam.quitVpss = AR_TRUE;
                break;
            case 'd':
                bDebug = AR_TRUE;
                break;
            case 'h':
                usage(argv[0]);
                break;
            default:
                usage(argv[0]);
                exit(1);
        }
    }

    ipc_client_context_t context;
    assert(!AR_SAMPLE_VPSS_create_ipc_client(&context));

    // SAMPLE_VPSS_CONFIG_CMD_E cmd = CMD_VPSS_CONFIG_NONE;
    SAMPLE_VPSS_REPLY_S replyRet = {"Unknown", -1};

    AR_BOOL bStartStop = AR_FALSE;
    if(stParam.startVpss || stParam.stopVpss || stParam.quitVpss){
        bStartStop = AR_TRUE;
    }

    if(!bStartStop && (stParam.grpId < 0 || stParam.grpId > VPSS_MAX_GRP_NUM)){
        LOG_ERROR("Group id %d is wrong!\n", stParam.grpId);
        usage(argv[0]);
        return -1;
    }

    if(stParam.chnId >= VPSS_MAX_PHY_CHN_NUM){
        if(u32BindChn < 0 || u32BindChn >= VPSS_MAX_PHY_CHN_NUM){
            LOG_ERROR("The physical channel index (%d) of the extension channel bind is not supported\n", u32BindChn);
            usage(argv[0]);
            return -1;
        }else{
            stParam.extChnAttr.s32BindChn = u32BindChn;
            LOG_INFO("%d bind to %d\n", stParam.chnId, stParam.extChnAttr.s32BindChn);
        }
    }

    if(bScale && stScaleSize.u32Width <= 0 && stScaleSize.u32Height <= 0){
        LOG_ERROR("scaler width and height is wrong!\n")
        usage(argv[0]);
        return -1;
    }

    if(stParam.chnId == -1){
        if(bScale) {
            // config group attr
            stParam.grpAttr.u32Width = stScaleSize.u32Width;
            stParam.grpAttr.u32Height = stScaleSize.u32Height;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_SCALE, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d scale success\n", stParam.grpId);
            } else {
                LOG_ERROR("Set Group %d scale failed!\n", stParam.grpId);
            }
        }

        if (bMirror) {
            stParam.grpAttr.bMirror = AR_TRUE;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_MIRROR, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d mirror success\n", stParam.grpId);
            } else {
                LOG_ERROR("Set Group %d mirror failed!\n", stParam.grpId);
            }
        }

        if (bFlip) {
            stParam.grpAttr.bFlip = AR_TRUE;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_FLIP, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d flip success\n", stParam.grpId);
            } else {
                LOG_ERROR("Set Group %d flip failed!\n", stParam.grpId);
            }
        }
    } else if(stParam.chnId >= 0 && stParam.chnId < VPSS_MAX_CHN_NUM) {
        // config channel attr
        if(bScale) {
            if (stParam.chnId < VPSS_MAX_PHY_CHN_NUM) {
                stParam.chnAttr.u32Width = stScaleSize.u32Width;
                stParam.chnAttr.u32Height = stScaleSize.u32Height;
            } else if(stParam.chnId >= VPSS_MAX_PHY_CHN_NUM){
                stParam.extChnAttr.u32Width = stScaleSize.u32Width;
                stParam.extChnAttr.u32Height = stScaleSize.u32Height;
            }
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_SCALE, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d scale success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d scale failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        if (bMirror) {
            if (stParam.chnId < VPSS_MAX_PHY_CHN_NUM) {
                stParam.chnAttr.bMirror = AR_TRUE;
            } else if(stParam.chnId >= VPSS_MAX_PHY_CHN_NUM){
                stParam.extChnAttr.bMirror = AR_TRUE;
            }
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_MIRROR, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d mirror success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d mirror failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        if (bFlip) {
            if (stParam.chnId < VPSS_MAX_PHY_CHN_NUM) {
                stParam.chnAttr.bFlip = AR_TRUE;
            } else if(stParam.chnId >= VPSS_MAX_PHY_CHN_NUM){
                stParam.extChnAttr.bFlip = AR_TRUE;
            }
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_FLIP, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d flip success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d flip failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        /* enable ldc for physical channel*/
        if (bLdc && (stParam.chnId < VPSS_MAX_PHY_CHN_NUM))
        {
            stParam.ldcAttr.bEnable = AR_TRUE;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_PHY_CHN_LDC, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d ldc success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d ldc failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        /* enable spread for physical channel*/
        if (bSpread && (stParam.chnId < VPSS_MAX_PHY_CHN_NUM))
        {
            stParam.spreadAttr.bEnable = AR_TRUE;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_PHY_CHN_SPREAD, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d spread success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d spread failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        /* enable fisheye for extension channel*/
        if (bFisheye && stParam.chnId >= VPSS_MAX_PHY_CHN_NUM)
        {
            stParam.fisheyeAttr.bEnable = AR_TRUE;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_EXT_CHN_FISHEYE, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d fisheye success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d fisheye failed!\n", stParam.grpId, stParam.chnId);
            }
        }

        /* enable region luma */
        if(bCfgRgnLuma)
        {
            stParam.regionLuma  = regionLuma;
            s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_REGION_LUMA, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
            if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
                LOG_INFO("Set Group %d channel %d region luma success\n", stParam.grpId, stParam.chnId);
            } else {
                LOG_ERROR("Set Group %d channel %d region luma failed!\n", stParam.grpId, stParam.chnId);
            }
        }
    } else {
        LOG_ERROR("channel id %d is wrong!\n", stParam.chnId);
        usage(argv[0]);
        return -1;
    }

    /* config crop*/
    if (bCrop)
    {
        stParam.cropInfo.bEnable = AR_TRUE;
        stParam.cropInfo.enCropCoordinate = eCropType;
        stParam.cropInfo.stCropRect.s32X = stCropRect.s32X;
        stParam.cropInfo.stCropRect.s32Y = stCropRect.s32Y;
        stParam.cropInfo.stCropRect.u32Width = stCropRect.u32Width;
        stParam.cropInfo.stCropRect.u32Height = stCropRect.u32Height;
        s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_CROP, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            LOG_INFO("Set Group %d channel %d crop success\n", stParam.grpId, stParam.chnId);
        } else {
            LOG_ERROR("Set Group %d channel %d crop failed!\n", stParam.grpId, stParam.chnId);
        }
    }

    /* config rotation angle for channel */
    if (bRotate && eRotateAngle > ROTATION_0 && eRotateAngle < ROTATION_BUTT)
    {
        stParam.rotateAngle = eRotateAngle;
        s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_CONFIG_ROTATE, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            LOG_INFO("Set Group %d channel %d rotate %d degrees success\n", stParam.grpId, stParam.chnId, stParam.rotateAngle*90);
        } else {
            LOG_ERROR("Set Group %d channel %d rotate %d degrees failed!\n", stParam.grpId, stParam.chnId, stParam.rotateAngle*90);
        }
    }

    /* send start signal */
    if (stParam.startVpss)
    {
        s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_START, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            LOG_INFO("Send start sigle success\n");
        } else {
            LOG_ERROR("Send start sigle failed!\n");
        }
    }

    /* send stop signal*/
    if (stParam.stopVpss)
    {
        s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_STOP, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            LOG_INFO("Send stop sigle success\n");
        } else {
            LOG_ERROR("Send stop sigle failed!\n");
        }
    }

    /* send quit signal*/
    if (stParam.quitVpss)
    {
        s32Ret = AR_SAMPLE_VPSS_client_ops(&context, CMD_VPSS_QUIT, &stParam, sizeof(SAMPLE_VPSS_CONFIG_S), &replyRet);
        if(s32Ret == AR_SUCCESS && replyRet.s32Ret == AR_SUCCESS){
            LOG_INFO("Send quit sigle success\n");
        } else {
            LOG_ERROR("Send quit sigle failed!\n");
        }
    }
    ar_ipc_close_service(context.ipc, context.handle);
    ar_ipc_close(context.ipc);
    return s32Ret;
}
