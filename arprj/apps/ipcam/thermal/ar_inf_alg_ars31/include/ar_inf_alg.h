#ifndef __AR_INF_ALGO__
#define __AR_INF_ALGO__

#include "ar_comm_video.h"
#define INF_ALG_VER 200

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct arINF_ALGO_COLOR_PALETTE_PARAM_S
    {
        char ColorPaletteFilePath[128];         //色板文件路径
        int palette_index;                      //色板序号
    }INF_ALGO_COLOR_PALETTE_PARAM_S;

    typedef enum {
        FUSION_MODE_0,      //普通模式
        FUSION_MODE_1,      //细节模式1
        FUSION_MODE_2,      //细节模式2
        FUSION_MODE_3,      //todo
    }ENUM_FUSION_MODE;

    typedef struct arINF_ALGO_FUSION_PARAM_S
    {
        int luma_alpha;                 //亮度透明度
        int chroma_alpha;               //色度透明度
        int edge_weight;                //边缘权重
        int palette_index;              //融合色板序号
        int calib_enable;               //标定使能(0 disable, 1 enable),使能后利用标定点进行变换对齐
        ENUM_FUSION_MODE fusion_mode;   //融合模式
    }INF_ALGO_FUSION_PARAM_S;

    typedef struct arINF_ALGO_ALL_PARAM_S
    {
        INF_ALGO_COLOR_PALETTE_PARAM_S stColorPaletteParam;     //伪彩色参数
        INF_ALGO_FUSION_PARAM_S stFusionParam;                  //融合参数
    }INF_ALGO_ALL_PARAM_S;

    typedef struct arINF_ALGO_OBJECT_S INF_ALGO_OBJECT_S;

    struct arINF_ALGO_OBJECT_S
    {
        //config
        /**
        * @brief  获取所有参数
        */
        int (* get_config_params)(INF_ALGO_OBJECT_S *pObj, INF_ALGO_ALL_PARAM_S *pParams);
        /**
        * @brief  设置所有参数
        */
        int (* set_config_params)(INF_ALGO_OBJECT_S *pObj, INF_ALGO_ALL_PARAM_S *pParams);

        /**
        * @brief  融合初始化
        * @param  pObj 创建成功后的object
        * @param  y y数据地址
        * @param  u u数据地址
        * @param  v v数据地址
        * @param  w 有效宽度
        * @param  h 有效高度
        * @param  y_stride y对齐后的宽度
        * @param  u_stride u对齐后的宽度
        * @param  v_stride v对齐后的宽度
        * @retval 0 成功 , 其它 失败.
        */
        int (* mapping_color_process)(INF_ALGO_OBJECT_S *pObj, void *y, void *u, void *v, int w, int h, int y_stride, int u_stride, int v_stride);

        /**
        * @brief  融合初始化(参数应为可见光帧)
        * @param  pObj 创建成功后的object
        * @param  width 可见光帧的宽
        * @param  height 可见光帧的高
        * @param  y_stride 可见光帧的亮度stride
        * @param  uv_stride 可见光帧的色度stride
        * @retval 0 成功 , 其它 失败.
        */
        int (* fusion_init)(INF_ALGO_OBJECT_S* pObj, int width, int height, int y_stride, int uv_stride);

        /**
        * @brief  融合处理
        * @param  pObj 创建成功后的object
        * @param  pstFrameVis 可见光帧，同时也是融合后的输出帧
        * @param  pstFrameIr 红外帧数据
        * @retval 0 成功 , 其它 失败.
        */
        int (* fusion_process)(INF_ALGO_OBJECT_S* pObj, VIDEO_FRAME_INFO_S *pstFrameVis, VIDEO_FRAME_INFO_S *pstFrameIr);

        /**
        * @brief  融合释放
        * @param  pObj 创建成功后的object
        * @retval 0 成功 , 其它 失败.
        */
        int (* fusion_release)(INF_ALGO_OBJECT_S* pObj);

        /**
        * @brief  设置标定的对应点
        * @param  pObj 创建成功后的object
        * @param  width 图像宽度
        * @param  height 图像高度
        * @param  src_pt 源图像的四个点，顺时针顺序
        * @param  dst_pt  目标图像的四个点，顺时针顺序
        * @retval 0 成功 , 其它 失败.
        * @note   源图像与目标图像的坐标应该为相同宽高下的坐标
        * @note   src_pt 与 dst_pt 的四个点中，任意一个边对应的Δx，Δy不能大于1.5倍
        */
        int (* set_calib_point)(INF_ALGO_OBJECT_S* pObj, int width, int height, POINT_S src_pt[], POINT_S dst_pt[]);

        /**
        * @brief  获取标定点对应的转换结果
        * @param  pObj 创建成功后的object
        * @param  ppLutData 返回lut数据
        * @param  pLutLen  返回lut长度
        * @retval 0 成功 , 其它 失败.
        * @note   set_calib_point成功后，并希望外部调用gdc api时，利用该接口获取lut
        */
        int (* get_calib_result)(INF_ALGO_OBJECT_S* pObj, void** ppLutData, int* pLutLen);
        //int (* calib_process)(INF_ALGO_OBJECT_S* pObj, VIDEO_FRAME_INFO_S *pstFrameIn, VIDEO_FRAME_INFO_S *pstFrameOut);
        
        //other
        void *pri; //内部使用请勿修改
        void *usr;
    };

    /**
    * @brief  创建红外算法库对象
    * @param  ppObj object地址的指针
    * @param  sConfigFilePath 配置文件路径，文件不存在则自动创建
    * @param  pstDefaultParams 默认参数，配置文件不存在时的默认参数
    * @retval 0 成功 , 其它 失败.
    */
    int inf_algo_create(INF_ALGO_OBJECT_S **ppObj, const char *sConfigFilePath, INF_ALGO_ALL_PARAM_S *pstDefaultParams, void* usrdata);
    /**
    * @brief  释放红外算法库对象
    */
    int inf_algo_release(INF_ALGO_OBJECT_S **ppObj);

#ifdef __cplusplus
};
#endif

#endif