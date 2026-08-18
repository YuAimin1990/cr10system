#ifndef __AR_INF_ALGO__
#define __AR_INF_ALGO__

#include "ar_comm_video.h"
#define INF_ALG_VER 100

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct arINF_ALGO_GTM_PARAM_S
    {
        float upThrowPointProportion;
        float downThrowPointProportion;
        float target_mean_value;
        float contrastBaseUp;
        float contrastBaseDown;
        float compensate1;
        float compensate2;
    }INF_ALGO_GTM_PARAM_S;

    typedef struct arINF_ALGO_LMS_PARAM_S
    {
        unsigned int enable_horizontal;
        float prob;
        int RaduisVL;
        int RaduisHL;
        float uStepVL;
        float uStepHL;
        int skipFrames;
    }INF_ALGO_LMS_PARAM_S;

    typedef struct arINF_ALGO_COLOR_PALETTE_PARAM_S
    {
        char ColorPaletteFilePath[128];
        int palette_index;
    }INF_ALGO_COLOR_PALETTE_PARAM_S;

    typedef enum {
        FUSION_MODE_0,
        FUSION_MODE_1,
    }ENUM_FUSION_MODE;

    typedef struct arINF_ALGO_FUSION_PARAM_S
    {
        int src_alpha;
        ENUM_FUSION_MODE fusion_mode;
    }INF_ALGO_FUSION_PARAM_S;

    typedef struct arINF_ALGO_ALL_PARAM_S
    {
        INF_ALGO_GTM_PARAM_S stGtmParam;
        INF_ALGO_LMS_PARAM_S stLmsParam;
        INF_ALGO_COLOR_PALETTE_PARAM_S stColorPaletteParam;
        INF_ALGO_FUSION_PARAM_S stFusionParam;
    }INF_ALGO_ALL_PARAM_S;

    typedef enum {
        ST_CONFIG_CHANGED,
        ST_GAIN_UPDATE,
    }ENUM_ALG_STATUS_CODE;
    typedef int (*InfAlgStatusCallbak)(ENUM_ALG_STATUS_CODE enStatusCode,void *data);

    typedef struct arINF_ALGO_OBJECT_S INF_ALGO_OBJECT_S;

    struct arINF_ALGO_OBJECT_S
    {
        int (* get_config_params)(INF_ALGO_OBJECT_S *pObj, INF_ALGO_ALL_PARAM_S *pParams);
        int (* set_config_params)(INF_ALGO_OBJECT_S *pObj, INF_ALGO_ALL_PARAM_S *pParams);
        int (* gtm_process)(INF_ALGO_OBJECT_S* pObj,void *input,void *output);
        int (* lms_process)(INF_ALGO_OBJECT_S* pObj,void *input,void *output);
        int (* mapping_color_process)(INF_ALGO_OBJECT_S *pObj, void *y, void *u, void *v, int w, int h, int y_stride, int u_stride, int v_stride);
        int (* fusion_init)(INF_ALGO_OBJECT_S* pObj, int width, int height, int y_stride, int uv_stride);
        int (* fusion_process)(INF_ALGO_OBJECT_S* pObj, VIDEO_FRAME_INFO_S *pstFrameDst, VIDEO_FRAME_INFO_S *pstFrameSrc, VIDEO_FRAME_INFO_S *pstFrameOut);
        int (* fusion_release)(INF_ALGO_OBJECT_S* pObj);
        int (* set_status_callbak)(INF_ALGO_OBJECT_S* pObj, InfAlgStatusCallbak func);
        void *pri;
        void *usr;
    };

    typedef struct arINF_ALGO_PRI_CONFIG_S
    {
        unsigned int width;                    //width height for gtm, lms is fix size
        unsigned int height;
        unsigned int lms_out_bitwidth;         //lms output valid bit width, 10、12、14 etc
        unsigned int gtm_out_bitwidth;         //gmt output valid bit width, 10、12、14 etc
    }INF_ALGO_PRI_CONFIG_S;

    int inf_algo_create(INF_ALGO_OBJECT_S **ppObj, const char *sConfigFilePath, INF_ALGO_ALL_PARAM_S *pstDefaultParams, INF_ALGO_PRI_CONFIG_S *pstPriConfig, void* usrdata);
    int inf_algo_release(INF_ALGO_OBJECT_S **ppObj);

#ifdef __cplusplus
};
#endif

#endif