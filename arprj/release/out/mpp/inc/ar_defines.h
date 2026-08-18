#ifndef __AR_DEFINES_H__
#define __AR_DEFINES_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#ifdef CONFIG_BIT_32BITS
#define ALIGN_NUM 8
#else
#define ALIGN_NUM 4
#endif

#define LUMA_PHY_ALIGN               16

#define DEFAULT_ALIGN                32
#define MAX_ALIGN                    1024
#define SEG_CMP_LENGTH               256

#define MAX_MMZ_NAME_LEN             16

#define VO_MAX_NODE_NUM              16

/* For VB */
#define VB_MAX_POOLS                 768

/* For VENC */
#define VENC_MAX_NAME_LEN            16
#define VENC_MAX_CHN_NUM             40
#define VEDU_IP_NUM                  1
#define H264E_MAX_WIDTH              8192
#define H264E_MAX_HEIGHT             8192
#define H264E_MIN_WIDTH              114
#define H264E_MIN_HEIGHT             114
#define H265E_MAX_WIDTH              8192
#define H265E_MAX_HEIGHT             8192
#define H265E_MIN_WIDTH              114
#define H265E_MIN_HEIGHT             114
#define JPEGE_MAX_WIDTH              32768
#define JPEGE_MAX_HEIGHT             32768
#define JPEGE_MIN_WIDTH              32
#define JPEGE_MIN_HEIGHT             32
#define JPGE_MAX_NUM                 1
#define VENC_MAX_ROI_NUM             8
#define H264E_MIN_HW_INDEX           0
#define H264E_MAX_HW_INDEX           11
#define H264E_MIN_VW_INDEX           0
#define H264E_MAX_VW_INDEX           3
#define VENC_QP_HISGRM_NUM           52
#define MAX_TILE_NUM                 1
#define H265E_ADAPTIVE_FRAME_TYPE    5
#define H265E_ADAPTIVE_QP_TYPE       5

#define VENC_MIN_INPUT_FRAME_RATE    1
#define VENC_MAX_INPUT_FRAME_RATE    240

/* For RC */
#define RC_TEXTURE_THR_SIZE          16
#define MIN_BITRATE         2
#define MAX_BITRATE         200*1024


/* For VDEC */
#define VDEC_MAX_CHN_NUM        40
#define VDH_MAX_CHN_NUM         64
#define VEDU_CHN_START          VDH_MAX_CHN_NUM
#define VEDU_H264D_ERRRATE      10
#define VEDU_H264D_FULLERR      100

#define H264D_ALIGN_W           64
#define H264D_ALIGN_H           64
#define H265D_ALIGN_W           64
#define H265D_ALIGN_H           64
#define JPEGD_ALIGN_W           64
#define JPEGD_ALIGN_H           16
#define JPEGD_RGB_ALIGN         16

#define H264D_MAX_SPS           32
#define H264D_MIN_SPS           1
#define H264D_MAX_PPS           256
#define H264D_MIN_PPS           1
#define H264D_MAX_SLICE         300
#define H264D_MIN_SLICE         1

#define H265D_MAX_VPS           16
#define H265D_MIN_VPS           1
#define H265D_MAX_SPS           16
#define H265D_MIN_SPS           1
#define H265D_MAX_PPS           64
#define H265D_MIN_PPS           1
#define H265D_MAX_SLICE         200
#define H265D_MIN_SLICE         1

#define VDH_H264D_MAX_WIDTH    8192
#define VDH_H264D_MAX_HEIGHT   8192
#define VDH_H264D_MIN_WIDTH    114
#define VDH_H264D_MIN_HEIGHT   114

#define VDH_H265D_MAX_WIDTH    8192
#define VDH_H265D_MAX_HEIGHT   8192
#define VDH_H265D_MIN_WIDTH    114
#define VDH_H265D_MIN_HEIGHT   114
#define VDH_MIN_MSG_NUM         1
#define VDH_MAX_MSG_NUM         8
#define VDH_MIN_BIN_SIZE        72
#define VDH_MAX_BIN_SIZE        2048
#define VDH_MIN_EXT_MEM_LEVEL   1
#define VDH_MAX_EXT_MEM_LEVEL   6


#define VEDU_H264D_MAX_WIDTH    8192
#define VEDU_H264D_MAX_HEIGHT   8192
#define VEDU_H264D_MIN_WIDTH    128
#define VEDU_H264D_MIN_HEIGHT   128

#define VEDU_H265D_MAX_WIDTH    8192
#define VEDU_H265D_MAX_HEIGHT   8192
#define VEDU_H265D_MIN_WIDTH    128
#define VEDU_H265D_MIN_HEIGHT   128

#define JPEGD_IP_NUM            1
#define JPEGD_MAX_WIDTH         8192
#define JPEGD_MAX_HEIGHT        8192
#define JPEGD_MIN_WIDTH         8
#define JPEGD_MIN_HEIGHT        8

/* For Region */
#define RGN_MIN_WIDTH                2
#define RGN_MIN_HEIGHT               2

#define RGN_COVER_MIN_X              -8192
#define RGN_COVER_MIN_Y              -8192
#define RGN_COVER_MAX_X              8190
#define RGN_COVER_MAX_Y              8190
#define RGN_COVER_MAX_WIDTH          8192
#define RGN_COVER_MAX_HEIGHT         8192

#define RGN_COVEREX_MIN_X              -8192
#define RGN_COVEREX_MIN_Y              -8192
#define RGN_COVEREX_MAX_X              8190
#define RGN_COVEREX_MAX_Y              8190
#define RGN_COVEREX_MAX_WIDTH          8192
#define RGN_COVEREX_MAX_HEIGHT         8192

#define RGN_OVERLAY_MIN_X            0
#define RGN_OVERLAY_MIN_Y            0
#define RGN_OVERLAY_MAX_X            8190
#define RGN_OVERLAY_MAX_Y            8190
#define RGN_OVERLAY_MAX_WIDTH        8192
#define RGN_OVERLAY_MAX_HEIGHT       8192

#define RGN_OVERLAYEX_MIN_X            0
#define RGN_OVERLAYEX_MIN_Y            0
#define RGN_OVERLAYEX_MAX_X            8190
#define RGN_OVERLAYEX_MAX_Y            8190
#define RGN_OVERLAYEX_MAX_WIDTH        8192
#define RGN_OVERLAYEX_MAX_HEIGHT       8192

#define RGN_MOSAIC_X_ALIGN           4
#define RGN_MOSAIC_Y_ALIGN           2
#define RGN_MOSAIC_WIDTH_ALIGN       4
#define RGN_MOSAIC_HEIGHT_ALIGN      4

#define RGN_MOSAIC_MIN_X             0
#define RGN_MOSAIC_MIN_Y             0
#define RGN_MOSAIC_MAX_X             8188
#define RGN_MOSAIC_MAX_Y             8190
#define RGN_MOSAIC_MIN_WIDTH         32
#define RGN_MOSAIC_MIN_HEIGHT        32
#define RGN_MOSAIC_MAX_WIDTH         8192
#define RGN_MOSAIC_MAX_HEIGHT        8192

#define RGN_ALIGN                    2

#define RGN_HANDLE_MAX               1024
#define RGN_MAX_BUF_NUM              6

#define COVER_MAX_NUM_VI             0
#define COVEREX_MAX_NUM_VI           16
#define OVERLAY_MAX_NUM_VI           0
#define OVERLAYEX_MAX_NUM_VI         16
#define MOSAIC_MAX_NUM_VI            0

#define OVERLAY_MAX_NUM_VENC         8
#define OVERLAYEX_MAX_NUM_VENC       8

#define COVER_MAX_NUM_VPSS           8
#define COVEREX_MAX_NUM_VPSS         8
#define OVERLAY_MAX_NUM_VPSS         0
#define OVERLAYEX_MAX_NUM_VPSS       8
#define MOSAIC_MAX_NUM_VPSS          4

#define OVERLAYEX_MAX_NUM_AVS        8


#define COVEREX_MAX_NUM_VO           1
#define OVERLAYEX_MAX_NUM_VO         1

#define OVERLAYEX_MAX_NUM_PCIV       1

/*************************************/
#define VENC_MAX_SSE_NUM             8
#define AR_MAX_SENSOR_NUM          2

/********************************Macro Definition********************************/
/** \addtogroup      MPI_VI */
/** @{ */  /** <!-- [MPI_VI] */

/* For VI */
/* number of channle and device on video input unit of chip
 * Note! VI_MAX_CHN_NUM is NOT equal to VI_MAX_DEV_NUM
 * multiplied by VI_MAX_CHN_NUM, because all VI devices
 * can't work at mode of 4 channles at the same time.
 */
#define VI_MAX_DEV_NUM              12               /**<定义VI设备的最大个数*/
#define VI_MAX_PHY_PIPE_NUM         8               /**<定义VI物理PIPE的最大个数*/
#define VI_MAX_VIR_PIPE_NUM         0               /**<定义VI虚拟PIPE的最大个数*/
#define VI_MAX_PIPE_NUM             (VI_MAX_PHY_PIPE_NUM + VI_MAX_VIR_PIPE_NUM)     /**<定义VI PIPE的最大个数*/
#define VI_MAX_PHY_CHN_NUM        5                 /**<定义VI物理通道的最大个数*/
#define VI_MAX_EXT_CHN_NUM        0                 /**<定义VI扩展通道的最大个数*/
#define VI_MAX_CHN_NUM          (VI_MAX_PHY_CHN_NUM + VI_MAX_EXT_CHN_NUM)           /**<定义VI物理通道和扩展通道的总个数*/
#define VI_CMP_PARAM_SIZE              1            /**<VI RAW 数据压缩参数的大小*/

/* For DIS*/
#define DIS_PYRAMID_LAYER_NUM   5                   /**<@note 新添加参数。*/
#define DIS_MAX_CHN_NUM         16                  /**<@note 新添加参数。*/
#define DIS_MAX_IMAGE_WIDTH     (4096)              /**<定义DIS的最大图像宽度*/
#define DIS_MIN_IMAGE_WIDTH     (1280)              /**<定义DIS的最小图像宽度*/
#define DIS_MAX_IMAGE_HEIGHT    (4096)              /**<定义DIS的最大图像高度*/
#define DIS_MIN_IMAGE_HEIGHT    (720)               /**<定义DIS的最小图像高度*/

/** @} */  /** <!-- ==== Macro Definition end ==== */

/* For VO */

#define VO_MAX_VIRT_DEV_NUM     4       /* max virtual dev num*/
#define VO_VIRT_DEV_0           2       /* virtual device 0 */
#define VO_VIRT_DEV_1           3       /* virtual device 1 */
#define VO_VIRT_DEV_2           4       /* virtual device 2 */
#define VO_VIRT_DEV_3           5       /* virtual device 3 */

#define VO_VIRT_LAYER_0         3       /* virtual layer 0 */
#define VO_VIRT_LAYER_1         4       /* virtual layer 1 */
#define VO_VIRT_LAYER_2         5       /* virtual layer 2 */
#define VO_VIRT_LAYER_3         6       /* virtual layer 3 */

#define VO_MIN_CHN_WIDTH        32      /* channel minimal width */
#define VO_MIN_CHN_HEIGHT       32      /* channel minimal height */
#define VO_MAX_ZOOM_RATIO       1000    /* max zoom ratio, 1000 means 100% scale */

#define VO_MAX_PHY_DEV_NUM      2       /* max physical dev num*/

#define VO_MAX_DEV_NUM          (VO_MAX_PHY_DEV_NUM+VO_MAX_VIRT_DEV_NUM)       /* max dev num */
#define VO_MAX_LAYER_NUM        (3+VO_MAX_VIRT_DEV_NUM)       /* max layer num */
#define VO_MAX_PRIORITY         2       /* max layer priority */
#define VO_MAX_CHN_NUM          36      /* max chn num */
#define VO_MAX_LAYER_IN_DEV     2       /* max layer num of each dev */
#define VO_MAX_GRAPHICS_LAYER_NUM   3
#define VO_MAX_WBC_NUM              1
#define VO_MIN_TOLERATE         1       /* min play toleration 1ms */
#define VO_MAX_TOLERATE         100000  /* max play toleration 100s */

/* For AVS */
#define AVS_MAX_GRP_NUM          32      /* max grp num */
#define AVS_PIPE_NUM             4       /* max pipe num */
#define AVS_MAX_CHN_NUM          2       /* max chn num */
#define AVS_SPLIT_NUM            2
#define AVS_SPLIT_PIPE_NUM       6
#define AVS_CUBE_MAP_SURFACE_NUM 6

#define AVS_MAX_IN_WIDTH         8192
#define AVS_MAX_IN_HEIGHT        8192
#define AVS_MIN_IN_WIDTH         1280
#define AVS_MIN_IN_HEIGHT        720

#define AVS_MAX_OUT_WIDTH        8192
#define AVS_MAX_OUT_HEIGHT       8192
#define AVS_MIN_OUT_WIDTH        256
#define AVS_MIN_OUT_HEIGHT       256

/* For AUDIO */
#define AI_DEV_MAX_NUM          2
#define AO_DEV_MIN_NUM          0
#define AO_DEV_MAX_NUM          2
#define AIO_MAX_NUM             2
#define AENC_MAX_CHN_NUM        32
#define ADEC_MAX_CHN_NUM        32

#define AI_MAX_CHN_NUM          16
#define AO_MAX_CHN_NUM          3
#define AO_SYSCHN_CHNID         (AO_MAX_CHN_NUM - 1)

#define AIO_MAX_CHN_NUM         ((AO_MAX_CHN_NUM > AI_MAX_CHN_NUM) ? AO_MAX_CHN_NUM:AI_MAX_CHN_NUM)

/* For VPSS */
#define VPSS_IP_NUM                 1
#define VPSS0                    0
#define VPSS_MAX_GRP_NUM         128
#define VPSS_MAX_GRP_PIPE_NUM    1
#define VPSS_PARALLEL_PIC_NUM    1
#define VPSS_MAX_PHY_CHN_NUM     4
#define VPSS_MAX_EXT_CHN_NUM     8
#define VPSS_MAX_CHN_NUM           (VPSS_MAX_PHY_CHN_NUM + VPSS_MAX_EXT_CHN_NUM)
#define VPSS_MIN_IMAGE_WIDTH_SBS 512
#define VPSS_MIN_IMAGE_WIDTH     64
#define VPSS_MIN_IMAGE_HEIGHT    64
#define VPSS_MAX_IMAGE_WIDTH_SLAVE      8192
#define VPSS_MAX_IMAGE_WIDTH            8192
#define VPSS_MAX_IMAGE_HEIGHT           8192
#define VPSS_EXTCHN_MAX_IMAGE_WIDTH     8192
#define VPSS_EXTCHN_MAX_IMAGE_HEIGHT    8192
#define VPSS_MAX_ZOOMIN                 16
#define VPSS_MAX_ZOOMOUT                15
#define VPSS_EXT_CHN_MAX_ZOOMIN         16
#define VPSS_EXT_CHN_MAX_ZOOMOUT        30

/* For PCIV */
#define PCIV_MAX_CHN_NUM        16       /* max pciv channel number in each pciv device */

/*For IVS_MD*/
#define MD_MAX_CHN                64
#define MD_MAX_WIDTH            1920
#define MD_MAX_HEIGHT            1080
#define MD_MIN_WIDTH            64
#define MD_MIN_HEIGHT            64

/* For RECT */
#define DPU_RECT_MAX_GRP_NUM 8
#define DPU_RECT_MAX_PIPE_NUM 2
#define DPU_RECT_MAX_CHN_NUM 2

#define DPU_RECT_IN_IMAGE_MAX_WIDTH   2048
#define DPU_RECT_IN_IMAGE_MAX_HEIGHT  2048
#define DPU_RECT_IN_IMAGE_MIN_WIDTH   128
#define DPU_RECT_IN_IMAGE_MIN_HEIGHT  64
#define DPU_RECT_OUT_IMAGE_MAX_WIDTH   1920
#define DPU_RECT_OUT_IMAGE_MAX_HEIGHT  1080
#define DPU_RECT_OUT_IMAGE_MIN_WIDTH   128
#define DPU_RECT_OUT_IMAGE_MIN_HEIGHT  64

/* For MATCH */
#define DPU_MATCH_MAX_GRP_NUM 8
#define DPU_MATCH_MAX_PIPE_NUM 2
#define DPU_MATCH_MAX_CHN_NUM 1

#define DPU_MATCH_IN_IMAGE_MAX_WIDTH   1920
#define DPU_MATCH_IN_IMAGE_MAX_HEIGHT  1080
#define DPU_MATCH_IN_IMAGE_MIN_WIDTH   128
#define DPU_MATCH_IN_IMAGE_MIN_HEIGHT  64
#define DPU_MATCH_OUT_IMAGE_MAX_WIDTH   1920
#define DPU_MATCH_OUT_IMAGE_MAX_HEIGHT  1080
#define DPU_MATCH_OUT_IMAGE_MIN_WIDTH   128
#define DPU_MATCH_OUT_IMAGE_MIN_HEIGHT  64


/*For Gdc*/
#define FISHEYE_MIN_IN_IMAGE_WIDTH        1920
#define FISHEYE_MIN_IN_IMAGE_HEIGHT    1080
#define FISHEYE_MIN_OUT_IMAGE_WIDTH    640
#define FISHEYE_MIN_OUT_IMAGE_HEIGHT   360

#define LDC_MIN_IMAGE_WIDTH            640
#define LDC_MIN_IMAGE_HEIGHT           480

#define SPREAD_MIN_IMAGE_WIDTH          640
#define SPREAD_MIN_IMAGE_HEIGHT         480

#define PMF_MIN_IMAGE_WIDTH             480
#define PMF_MIN_IMAGE_HEIGHT            360

#define ROTATION_EX_MIN_IMAGE_WIDTH    480
#define ROTATION_EX_MIN_IMAGE_HEIGHT   360

#define GDC_MAX_IMAGE_WIDTH        8192
#define GDC_MAX_IMAGE_HEIGHT       8192
#define GDC_IP_NUM                 1
#define GDC_FUSION_NUM             9

/*For VGS*/
#define VGS_IP_NUM 1
#define VGS0 0
#define VGS1 1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_DEFINES_H__ */

