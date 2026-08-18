/**
 * @file hal_vb.h
 * @brief  hal 视频流缓冲区API和数据结构
 * @author Artosyn Software Team
 * @version 0.0.1
 * @date 2021/04/28
 * @license 2021-2025, Artosyn. Co., Ltd.
**/
#ifndef __HAL_SYS_H__
#define __HAL_SYS_H__

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#include "hal_type.h"
#include "hal_errno.h"
#include "hal_comm.h"
#include "osal_def.h"
#include "binder_ipc.h"

#define	DW_DMAC_MAX_BLOCK_NUM			(128)
#define AR_SYS_BIND_DEST_MAXNUM        (64)

#define AR_ALIGN_NUM                     8
#define AR_ATTRIBUTE                     __attribute__((aligned(AR_ALIGN_NUM)))


#define HAL_ERR_SYS_INVALID_CHNID        AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_CHNID)
#define HAL_ERR_SYS_INVALID_DEVID        AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_INVALID_DEVID)
#define HAL_ERR_SYS_TIMEOUT              AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_TIMEOUT)
#define HAL_ERR_SYS_NULL_PTR             AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NULL_PTR)
#define HAL_ERR_SYS_NOMEM                AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOMEM)
#define HAL_ERR_SYS_NOBUF                AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOBUF)
#define HAL_ERR_SYS_UNEXIST              AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_UNEXIST)
#define HAL_ERR_SYS_ILLEGAL_PARAM        AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_ILLEGAL_PARAM)
#define HAL_ERR_SYS_NOT_READY            AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SYS_NOTREADY)
#define HAL_ERR_SYS_BUSY                 AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BUSY)
#define HAL_ERR_SYS_NOT_PERM             AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_NOT_PERM)
#define HAL_ERR_SYS_SIZE_NOT_ENOUGH      AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_SIZE_NOT_ENOUGH)
#define HAL_ERR_SYS_BADADDR              AR_HAL_DEF_ERR(AR_SYS_ID_SYS, HAL_ERR_LEVEL_ERROR, HAL_ERR_BADADDR)

typedef enum dw_dmac_chan {
	_DMAC_CHANNEL_0		= 0,
	_DMAC_CHANNEL_1		= 1,
	_DMAC_CHANNEL_2		= 2,
	_DMAC_CHANNEL_3		= 3,
	_DMAC_NUM_CHANNELS	= 4,
}ENMU_SYS_DMA_CHN;

typedef enum dmac_chan_mod {
	_CHAN_SHARE		= 0,
	_CHAN_EXCLUSIVE	= 1,
}ENMU_SYS_DMA_CHN_MOD;

/*
 * dst_pa: dma copy destination pa address
 * src_pa: dma copy source pa address
 * size:  copy size
*/
typedef struct {
	void	*pv_dst_pa;
	void	*pv_src_pa;
	AR_U32	u32_size;
} STRU_SYS_DMA_TRAN_ADDR_SIZE;

/*
 * tran_cnt_sum : number of dma_tran_addr_size valid for copy
 * mod: _CHAN_SHARE:       dma copy channel allocate automatically, and share with other request.
 *      _CHAN_EXCLUSIVE:   use specify channel (specify by channel) for transfer. Only BB Use this mode.
 * channel: the channel for transfer if using _CHAN_EXCLUSIVE
 *
 */
typedef struct {
	AR_S32 				    s32_tran_cnt_sum;
	ENMU_SYS_DMA_CHN_MOD	e_mod;
	ENMU_SYS_DMA_CHN	    e_channel;
	STRU_SYS_DMA_TRAN_ADDR_SIZE	st_addr_size[DW_DMAC_MAX_BLOCK_NUM];
	AR_S32                  s32_disable_cci;
	unsigned long long      u64_flush_status;
} STRU_SYS_DMA_TRAN_INFO;

typedef struct{
    ENMU_SYS_MOD_ID    e_mod_id;
    AR_S32      s32_dev_id;
    AR_S32      s32_chn_id;
} STRU_SYS_CHN_S;

typedef struct {
    AR_U64 u64PhyAddr;
    AR_BOOL bCached;
} STRU_SYS_VIRMEM_INFO;

typedef struct {
    AR_S32 num;
    STRU_SYS_CHN_S mpp_chn[AR_SYS_BIND_DEST_MAXNUM];
} STRU_SYS_BIND_DEST;

typedef enum {
    AR_SYS_VIDEO_FIELD_TOP         = 0x1,    /* even field */
    AR_SYS_VIDEO_FIELD_BOTTOM      = 0x2,    /* odd field */
    AR_SYS_VIDEO_FIELD_INTERLACED  = 0x3,    /* two interlaced fields */
    AR_SYS_VIDEO_FIELD_FRAME       = 0x4,    /* frame */

    AR_SYS_VIDEO_FIELD_BUTT
} ENMU_SYS_VIDEO_FIELD;

typedef enum {
    AR_SYS_PIXEL_FORMAT_RGB_444 = 0,
    AR_SYS_PIXEL_FORMAT_RGB_555,
    AR_SYS_PIXEL_FORMAT_RGB_565,
    AR_SYS_PIXEL_FORMAT_RGB_888,

    AR_SYS_PIXEL_FORMAT_BGR_444,
    AR_SYS_PIXEL_FORMAT_BGR_555,
    AR_SYS_PIXEL_FORMAT_BGR_565,
    AR_SYS_PIXEL_FORMAT_BGR_888,

    AR_SYS_PIXEL_FORMAT_ARGB_1555,
    AR_SYS_PIXEL_FORMAT_ARGB_4444,
    AR_SYS_PIXEL_FORMAT_ARGB_8565,
    AR_SYS_PIXEL_FORMAT_ARGB_8888,
    AR_SYS_PIXEL_FORMAT_ARGB_2BPP,

    AR_SYS_PIXEL_FORMAT_ABGR_1555,
    AR_SYS_PIXEL_FORMAT_ABGR_4444,
    AR_SYS_PIXEL_FORMAT_ABGR_8565,
    AR_SYS_PIXEL_FORMAT_ABGR_8888,

    AR_SYS_PIXEL_FORMAT_RGB_BAYER_8BPP,
    AR_SYS_PIXEL_FORMAT_RGB_BAYER_10BPP,
    AR_SYS_PIXEL_FORMAT_RGB_BAYER_12BPP,
    AR_SYS_PIXEL_FORMAT_RGB_BAYER_14BPP,
    AR_SYS_PIXEL_FORMAT_RGB_BAYER_16BPP,


    AR_SYS_PIXEL_FORMAT_YVU_PLANAR_422,
    AR_SYS_PIXEL_FORMAT_YVU_PLANAR_420,
    AR_SYS_PIXEL_FORMAT_YVU_PLANAR_444,

    AR_SYS_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
    AR_SYS_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    AR_SYS_PIXEL_FORMAT_YVU_SEMIPLANAR_444,

    AR_SYS_PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    AR_SYS_PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    AR_SYS_PIXEL_FORMAT_YUV_SEMIPLANAR_444,

    AR_SYS_PIXEL_FORMAT_YUYV_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_YVYU_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_UYVY_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_VYUY_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_YYUV_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_YYVU_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_UVYY_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_VUYY_PACKAGE_422,
    AR_SYS_PIXEL_FORMAT_VY1UY0_PACKAGE_422,

    AR_SYS_PIXEL_FORMAT_YUV_400,
    AR_SYS_PIXEL_FORMAT_UV_420,

    /* SVP data format */
    AR_SYS_PIXEL_FORMAT_BGR_888_PLANAR,
    AR_SYS_PIXEL_FORMAT_HSV_888_PACKAGE,
    AR_SYS_PIXEL_FORMAT_HSV_888_PLANAR,
    AR_SYS_PIXEL_FORMAT_LAB_888_PACKAGE,
    AR_SYS_PIXEL_FORMAT_LAB_888_PLANAR,
    AR_SYS_PIXEL_FORMAT_S8C1,
    AR_SYS_PIXEL_FORMAT_S8C2_PACKAGE,
    AR_SYS_PIXEL_FORMAT_S8C2_PLANAR,
    AR_SYS_PIXEL_FORMAT_S8C3_PLANAR,
    AR_SYS_PIXEL_FORMAT_S16C1,
    AR_SYS_PIXEL_FORMAT_U8C1,
    AR_SYS_PIXEL_FORMAT_U16C1,
    AR_SYS_PIXEL_FORMAT_S32C1,
    AR_SYS_PIXEL_FORMAT_U32C1,
    AR_SYS_PIXEL_FORMAT_U64C1,
    AR_SYS_PIXEL_FORMAT_S64C1,

    AR_SYS_PIXEL_FORMAT_BUTT
} ENMU_SYS_PIXEL_FORMAT;

typedef enum {
    AR_SYS_VIDEO_FORMAT_LINEAR = 0,       /* nature video line */
    AR_SYS_VIDEO_FORMAT_TILE_64x16,       /* tile cell: 64pixel x 16line */
    AR_SYS_VIDEO_FORMAT_TILE_16x8,        /* tile cell: 16pixel x 8line */
    AR_SYS_VIDEO_FORMAT_LINEAR_DISCRETE,  /* The data bits are aligned in bytes */
    AR_SYS_VIDEO_FORMAT_BUTT
} ENMU_SYS_VIDEO_FORMAT;

typedef enum {
    AR_SYS_COMPRESS_MODE_NONE = 0,   /* no compress */
    AR_SYS_COMPRESS_MODE_SEG,        /* compress unit is 256x1 bytes as a segment.*/
    AR_SYS_COMPRESS_MODE_TILE,       /* compress unit is a tile.*/
    AR_SYS_COMPRESS_MODE_LINE,       /* compress unit is the whole line.  raw for VI  */
    AR_SYS_COMPRESS_MODE_FRAME,      /* compress unit is the whole frame. YUV for VI(3DNR), RGB for TDE(write)/VO(read) */

    AR_SYS_COMPRESS_MODE_BUTT
} ENMU_SYS_COMPRESS_MODE;

typedef enum {
    AR_SYS_DYNAMIC_RANGE_SDR8 = 0,
    AR_SYS_DYNAMIC_RANGE_SDR10,
    AR_SYS_DYNAMIC_RANGE_HDR10,
    AR_SYS_DYNAMIC_RANGE_HLG,
    AR_SYS_DYNAMIC_RANGE_SLF,
    AR_SYS_DYNAMIC_RANGE_XDR,
    AR_SYS_DYNAMIC_RANGE_BUTT
} ENMU_SYS_DYNAMIC_RANGE;

typedef enum {
    AR_SYS_COLOR_GAMUT_BT601 = 0,
    AR_SYS_COLOR_GAMUT_BT709,
    AR_SYS_COLOR_GAMUT_BT2020,
    AR_SYS_COLOR_GAMUT_USER,
    AR_SYS_COLOR_GAMUT_BUTT
} ENMU_SYS_COLOR_GAMUT;

typedef enum {
    AR_SYS_DATA_VI_FRAME,
    AR_SYS_DATA_VOU_FRAME,
    AR_SYS_DATA_VDEC_FRAME,
    AR_SYS_DATA_VIDEO_FRAME,
    AR_SYS_DATA_VOU_WBC_FRAME,
    AR_SYS_DATA_AUDIO_FRAME,
    AR_SYS_DATA_VPSS_FRAME,
    AR_SYS_DATA_DPU_RECT_FRAME,
    AR_SYS_DATA_AVS_FRAME,
    AR_SYS_DATA_BUTT
} ENMU_SYS_DATA_TYPE;

typedef enum {
    AR_SOC_UNKNOWN = 0,
    AR_9311     = 0x9311,
    AR_9321     = 0x9321,
    AR_9331     = 0x9331,
    AR_9341     = 0x9341,
    ARS_31      = 0x5331,
    AR_SOC_BUTT
} ENMU_SYS_AR_SOC_TYPE;

/**
* @note  定义视频图像帧补充信息。
*/
typedef struct
{
    AR_U64                  u64_jpeg_DCF_phy_addr;      /* Jpeg DCF信息的物理地址. */
    AR_U64                  u64_isp_info_phy_addr;      /* ISP 辅助信息的物理地址. */
    AR_U64                  u64_low_delay_phy_addr;     /* 低延时信息物理地址. */
    AR_U64                  u64_motion_data_phy_addr;   /* VPSS 运动信息物理地址. */
    AR_U64                  u64_frame_DNG_phy_addr;     /* DNG 信息的物理地址. */

    AR_VOID* AR_ATTRIBUTE   p_jpeg_DCF_vir_addr;        /* Jpeg DCF信息的虚拟地址，指向JPEG_DCF_S 结构体,内核态虚拟地址. */
    AR_VOID* AR_ATTRIBUTE   p_isp_info_vir_addr;        /* ISP 辅助信息的内核态虚拟地址，指向ISP_FRAME_INFO_S 结构体,内核态虚拟地址. */
    AR_VOID* AR_ATTRIBUTE   p_low_delay_vir_addr;       /* 低延时信息虚拟地址,内核态虚拟地址. */
    AR_VOID* AR_ATTRIBUTE   p_motion_data_vir_addr;     /* VPSS 运动信息虚拟地址,内核态虚拟地址. */
    AR_VOID* AR_ATTRIBUTE   p_frame_DNG_vir_addr;       /* DNG 格式信息的虚拟地址，内核态虚拟地址. */
} STRU_VIDEO_SUPPLEMENT_S;

typedef struct
{
    AR_S32 s32NalType;                  /**< This is nal Type of decoded picture */
    AR_S32 s32PicType;                  /**< This is the picture type of decoded picture. It reports the picture type of bottom field for interlaced stream */
    AR_S32 s32picTypeFirst;             /**< This is only valid in interlaced mode and indicates the picture type of the top field */
    AR_S32 s32NumOfErrMBsInDisplay;     /**< This is the total number of error coded unit(MB/CTU) in the display picture of indexFrameDisplay */
    AR_S32 s32NumOfTotMBsInDisplay;     /**< This is the total number of coded unit(MB/CTU) in the display picture of indexFrameDisplay */
    AR_S32 s32TopFieldFirst;            /**< For decoded picture consisting of two fields, 0 : VPU decodes the bottom field and then top field;
                                                                                           1 : VPU decodes the top field and then bottom field */
    AR_S32 s32RateNumerator;            /**< The numerator part of frame rate fraction */
    AR_S32 s32RateDenominator;          /**< The denominator part of frame rate fraction */
} STRU_VIDEO_EXT_S;

typedef struct{
	
    AR_U32                      u32FrameId;
    AR_U32                      u32_width;
    AR_U32                      u32_height;
    ENMU_SYS_VIDEO_FIELD        e_field;
    ENMU_SYS_PIXEL_FORMAT       e_pixel_format;
    ENMU_SYS_VIDEO_FORMAT       e_video_format;
    ENMU_SYS_COMPRESS_MODE      e_compress_mode;
    ENMU_SYS_DYNAMIC_RANGE      e_dynamic_range;
    ENMU_SYS_COLOR_GAMUT        e_color_gamut;
    AR_U32                      u32_header_stride[3];
    AR_U32                      u32_stride[3];
    AR_U32                      u32_ext_stride[3];

    AR_U64                      u64_header_phy_addr[3];
    AR_U64                      u64_header_vir_addr[3];
    AR_U64                      u64_phy_addr[3];
    AR_U64                      u64_vir_addr[3];
    AR_U64                      u64_ext_phy_addr[3];
    AR_U64                      u64_ext_vir_addr[3];

    AR_S16                      s16_offset_top;        /* top offset of show area */
    AR_S16                      s16_offset_bottom;    /* bottom offset of show area */
    AR_S16                      s16_offset_left;        /* left offset of show area */
    AR_S16                      s16_offset_right;        /* right offset of show area */

    AR_U32                      u32_max_luminance;
    AR_U32                      u32_min_Luminance;

    AR_U32                      u32_time_ref;
    AR_U64                      u64_PTS;

    AR_U64                      u64_private_data;
    AR_U32                      u32_frame_flag;     /* FRAME_FLAG_E, can be OR operation. */
    STRU_VIDEO_SUPPLEMENT_S     stSupplement;
	
    AR_U32              u32HeaderLen[3];  /**<note 新加参数，cf50 ，头的使用的buffer的大小；*/
    AR_U32              u32Len[3]; /**<note 新加参数；buffer 使用的大小*/
	AR_FLOAT            fLineTime; /**<note 新加参数；sensor 采集一行的时间，单位s*/

    // attention, new added for video decoder used
    AR_S32              s32IndexFrameDisplay; /* This is a frame buffer index for the picture to be displayed at the moment */

    // attention, new added for video decoder used
    STRU_VIDEO_EXT_S         stExt;
}  STRU_SYS_VIDEO_FRAME;

typedef struct{
    STRU_SYS_VIDEO_FRAME        stVFrame;
    AR_U32                      u32PoolId;
    ENMU_SYS_MOD_ID             enModId;
} STRU_SYS_VIDEO_FRAME_INFO;

#if 0

typedef struct {
    char            ch_gps_latitude_ref;        /* GPS LatitudeRef Indicates whether the latitude is north or south latitude,
                                                 * 'N'/'S', default 'N' */
    unsigned int    au32_gps_latitude[3][2];    /* GPS Latitude is expressed as degrees, minutes and seconds, a typical format
                                                 * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    char            ch_gps_longitude_ref;       /* GPS LongitudeRef Indicates whether the longitude is east or west longitude,
                                                 * 'E'/'W', default 'E' */
    unsigned int    au32_gps_longitude[3][2];   /* GPS Longitude is expressed as degrees, minutes and seconds, a typical format
                                                 * like "dd/1, mm/1, ss/1", default 0/0, 0/0, 0/0 */
    unsigned char   u8_gps_altitude_ref;        /* GPS AltitudeRef Indicates the reference altitude used, 0 - above sea level,
                                                 * 1 - below sea level default 0 */
    unsigned int    au32_gps_altitude[2];       /* GPS AltitudeRef Indicates the altitude based on the reference u8GPSAltitudeRef,
                                                 * the reference unit is meters, default 0/0 */
} STRU_SYS_GPS_INFO;

typedef struct {
    ar_u32 num;
    ar_void *mmz_name[SYS_DDR_MAXNUM];
} mpp_sys_ddr_name;

typedef struct {
    ar_pixel_format pixel_format;
    ar_u32 width;
    ar_u32 height;
    ar_u32 buf_line;
    ar_u32 align;  /* 0:auto */
    ar_dynamic_range dynamic_range;
    ar_compress_mode compress_mode;
    ar_video_format video_format;
    ar_bool is_3dnr_buffer;
} vb_base_info;
#endif

typedef struct {
    ENMU_SYS_MOD_ID e_mod_id;
    AR_U32 u32_max_dev_cnt;
    AR_U32 u32_max_chn_cnt;
    AR_S32 s32_flag;            //receiver callback block or not :0 no block, 1 block
    ENMU_SYS_DATA_TYPE e_data_type;
    AR_VOID *pv_priv_data;
    AR_S32 (*give_bind_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, STRU_SYS_BIND_DEST *pst_bind_send);
    AR_S32 (*get_frame_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id,  AR_VOID *pv_data, AR_VOID *pv_priv_data);
    AR_S32 (*release_frame_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, const AR_VOID *pv_data, AR_VOID *pv_priv_data);
} STRU_SYS_BIND_SENDER;

typedef struct {
    ENMU_SYS_MOD_ID e_mod_id;
    AR_U32 u32_max_dev_cnt;
    AR_U32 u32_max_chn_cnt;
    AR_VOID *pv_priv_data;
    /*接受数据的回掉函数*/
    AR_S32 (*call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_BOOL b_block,
    ENMU_SYS_DATA_TYPE e_data_type, AR_VOID *pv_data, AR_VOID *pv_priv_data);
    /*reset数据的回掉函数*/
    AR_S32 (*reset_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_VOID *pv_data, AR_VOID *pv_priv_data);
    AR_S32 (*give_bind_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_VOID *pv_priv_data);
    AR_S32 (*give_unbind_call_back)(AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_VOID *pv_priv_data);
    AR_BOOL support_delay_data;
} STRU_SYS_BIND_RECEIVER;

typedef struct ar_soc_info{
    AR_U8   revision_id;
    AR_S8   soc_suffix;
    AR_U16  soc_product_id;
    AR_U8   pd_year;
    AR_U8   pd_week;
    AR_U8   feb_id:4;
    AR_U8   jedec_id_bank:4;
    AR_U8   jedec_id_code;
    AR_S8   id_info[8];
    AR_U32  sn;
}STRU_SYS_AR_SOC_INFO;


/**
* @brief  memory存储映射接口.
* @param  u64_phy_addr 需映射的内存单元起始地址.
* @param  u32_size 映射的字节数。取值范围为（0, 0xFFFFF000）.
* @retval 0 无效地址 , 其它 有效地址.
* @note   open的节点是dev/mmz_userdev，在MMZ中实现映射非device类型（不可cache、可reorder）的DDR 地址
* @note   输入的地址需为合法的物理地址
* @note   对应的解映射接口为ar_hal_sys_munmap
*/
AR_VOID * ar_hal_sys_mmap(AR_U64 u64_phy_addr, AR_U32 u32_size);

/**
* @brief  存储映射接口，映射成cache属性.
* @param  u64_phy_addr 需映射的内存单元起始地址.
* @param  u32_size 映射的字节数。取值范围为（0, 0xFFFFF000）.
* @retval 0 无效地址 , 其它 有效地址.
* @note   输入的地址需为合法的物理地址
*/
AR_VOID * ar_hal_sys_mmap_cache(AR_U64 u64_phy_addr, AR_U32 u32_size);

/**
* @brief  存储反映射接口.
* @param  u64_phy_addr 需映射的内存单元起始地址.
* @param  u32_size 映射的字节数。取值范围为（0, 0xFFFFF000）.
* @retval 0 成功 , 其它 失败.
* @note   等同于系统munmap 函数
*/
AR_S32 ar_hal_sys_munmap(AR_VOID * p_vir_addr, AR_U32 u32_size);

/**
* @brief  刷新 cache里的内容到内存并且使cache里的内容无效.
* @param  u64_phy_addr 待操作数据的起始物理地址，需要 4字节对齐.
* @param  p_vir_addr 待操作数据的起始虚拟地址指针，必须为用户态地址，
          且不能为空，虚拟地址必须为物理地址映射的地址.
* @param  u32_size 待操作数据的大小，不能为0.
* @retval 0 成功 , 其它 失败.
* @note   此接口应与ar_hal_sys_mmap_cache接口配套使用
* @note   用户需要确保传入参数的合法性
*/
AR_S32 ar_hal_sys_mflush_cache(AR_U64 u64_phy_addr, AR_VOID *p_vir_addr, AR_U32 u32_size);

/**
* @brief  在用户态分配MMZ内存.
* @param  u64_phy_addr 分配的物理地址指针.
* @param  p_vir_addr 指向分配的虚拟地址指针的指针.
* @param  pstr_mmb Mmb 名称的字符串指针.
* @param  pstr_zone MMZ zone 名称的字符串指针.
* @param  u32_len 内存块大小.
* @retval 0 成功 , 其它 失败.
* @note   MMZ分为许多区域（Zone），每个区域下有多个Mmb，调用此接口在MMZ的名为
          *pstr_zone 的区域中分配一个名为*pstr_mmb 的内存块，大小为u32_len，并返回物理地
          址和用户态虚拟地址指针。如果MMZ中有名为anonymous的区域，*pstr_zone可设为
          NULL。如果*pstr_mmb 设为NULL，创建的Mmb 分块名为“<null>”
*/
AR_S32 ar_hal_sys_mmz_alloc(AR_U64* pu64_phy_addr, AR_VOID** p_vir_addr,
            const AR_CHAR* pstr_mmb, const AR_CHAR* pstr_zone, AR_U32 u32_len);

/**
* @brief  在用户态分配MMZ内存.
* @param  u64_phy_addr 分配的物理地址指针.
* @param  p_vir_addr 指向分配的虚拟地址指针的指针.
* @param  pstr_mmb Mmb 名称的字符串指针.
* @param  pstr_zone MMZ zone 名称的字符串指针.
* @param  u32_len 内存块大小.
* @param  u32_align 内存块起始地址对齐单位.
* @retval 0 成功 , 其它 失败.
* @note   MMZ分为许多区域（Zone），每个区域下有多个Mmb，调用此接口在MMZ的名为
          *pstr_zone 的区域中分配一个名为*pstr_mmb 的内存块，大小为u32_len，并返回物理地
          址和用户态虚拟地址指针。如果MMZ中有名为anonymous的区域，*pstr_zone可设为
          NULL。如果*pstr_mmb 设为NULL，创建的Mmb 分块名为“<null>”
*/
AR_S32 ar_hal_sys_mmz_alloc_align(AR_U64* pu64_phy_addr, AR_VOID** p_vir_addr,
            const AR_CHAR* pstr_mmb, const AR_CHAR* pstr_zone, AR_U32 u32_len, AR_U32 u32_align);

/**
* @brief  在用户态分配MMZ内存，该内存支持cache缓存.
* @param  u64_phy_addr 分配的物理地址指针.
* @param  p_vir_addr 指向分配的虚拟地址指针的指针.
* @param  pstr_mmb Mmb 名称的字符串指针.
* @param  pstr_zone MMZ zone 名称的字符串指针.
* @param  u32_len 内存块大小.
* @retval 0 成功 , 其它 失败.
* @note   本接口与ar_hal_sys_mmz_alloc接口的区别：通过本接口分配的内存支持cache
          缓存，对于频繁使用的内存，最好使用本接口分配内存，这样可以提高cpu读写
          的效率，提升系统性能，如用户在使用ive算子时，就存在大量数据频繁读写，此
          时使用此接口来分配内存，就能很好的提高cpu 的效率
* @note   当 cpu访问此接口分配的内存时，会将内存中的数据放在cache 中，而硬件设备
          只能访问物理内存，不能访问cache 的内容，对于这种cpu和硬件会共同
          操作的内存，需调用ar_hal_sys_mmz_flush_cache做好数据同步
*/
AR_S32 ar_hal_sys_mmz_alloc_cached(AR_U64* pu64_phy_addr, AR_VOID** p_vir_addr,
            const AR_CHAR* pstr_mmb, const AR_CHAR* pstr_zone, AR_U32 u32_len);

/**
* @brief  在用户态分配MMZ内存，该内存支持cache缓存.
* @param  u64_phy_addr 分配的物理地址指针.
* @param  p_vir_addr 指向分配的虚拟地址指针的指针.
* @param  pstr_mmb Mmb 名称的字符串指针.
* @param  pstr_zone MMZ zone 名称的字符串指针.
* @param  u32_len 内存块大小.
* @param  u32_align 内存块起始地址对齐单位.
* @retval 0 成功 , 其它 失败.
* @note   本接口与ar_hal_sys_mmz_alloc接口的区别：通过本接口分配的内存支持cache
          缓存，对于频繁使用的内存，最好使用本接口分配内存，这样可以提高cpu读写
          的效率，提升系统性能，如用户在使用ive算子时，就存在大量数据频繁读写，此
          时使用此接口来分配内存，就能很好的提高cpu 的效率
* @note   当 cpu访问此接口分配的内存时，会将内存中的数据放在cache 中，而硬件设备
          只能访问物理内存，不能访问cache 的内容，对于这种cpu和硬件会共同
          操作的内存，需调用ar_hal_sys_mmz_flush_cache做好数据同步
*/			
AR_S32 ar_hal_sys_mmz_alloc_cached_align(AR_U64* pu64_phy_addr, AR_VOID** p_vir_addr,
            const AR_CHAR* pstr_mmb, const AR_CHAR* pstr_zone, AR_U32 u32_len, AR_U32 u32_align);


/**
* @brief  刷新 cache里的内容到内存并且使cache里的内容无效.
* @param  u64_phy_addr 待操作数据的起始物理地址.
* @param  p_vir_addr 待操作数据的起始虚拟地址指针，不能传NULL.
* @param  u32_size 待操作数据的大小.
* @retval 0 成功 , 其它 失败.
* @note   当 cache里的数据为最新数据时，为了保证不能直接访问cache 的硬件在访
          问内存时能够得到正确的数据，此时需要先调用此接口将cache里的内容更新到
          内存，这样，当硬件访问内存时，保证了数据的一致性和正确性
* @note   此接口应与ar_hal_sys_mmz_alloc_cached接口配套使用
* @note   若将u32PhyAddr 设为0，则表示操作整个cache 区域
* @note   用户需要确保传入参数的合法性
* @note   用户必须保证进行flush操作时不能调用ar_hal_sys_mmz_free接口释放当前
          flush 的内存，否则会引发不可预知的异常
*/
AR_S32 ar_hal_sys_mmz_flush_cache(AR_U64 u64_phy_addr, AR_VOID *p_vir_addr, AR_U32 u32_size);

/**
* @brief  使 cache里的内容无效.
* @param  u64_phy_addr 待操作数据的起始物理地址.
* @param  p_vir_addr 待操作数据的起始虚拟地址指针，不能传NULL.
* @param  u32_size 待操作数据的大小.
* @retval 0 成功 , 其它 失败.
* @note   当 cache里的数据为老时，为了保证不能直接访
          问内存时能够得到正确的数据，此时需要先调用此接口将cache里的内容无效
          ，这样，当CPU访问内存时，保证了数据的一致性和正确性
* @note   此接口应与ar_hal_sys_mmz_alloc_cached接口配套使用
* @note   用户需要确保传入参数的合法性
* @note   用户必须保证进行invalid操作时不能调用ar_hal_sys_mmz_free接口释放当前
          invalid 的内存，否则会引发不可预知的异常
*/
AR_S32 ar_hal_sys_mmz_invalid_cache(AR_U64 u64_phy_addr, AR_VOID *p_vir_addr, AR_U32 u32_size);

/**
* @brief  刷新 cache里的内容到内存并且使cache里的内容无效.
* @param  u64_phy_addr 待操作数据的起始物理地址.
* @param  u32_size 待操作数据的大小.
* @retval 0 成功 , 其它 失败.
* @note   当 cache里的数据为最新数据时，为了保证不能直接访问cache 的硬件在访
          问内存时能够得到正确的数据，此时需要先调用此接口将cache里的内容更新到
          内存，这样，当硬件访问内存时，保证了数据的一致性和正确性
* @note   此接口应与ar_hal_sys_mmz_alloc_cached接口配套使用
* @note   用户需要确保传入参数的合法性
* @note   用户必须保证进行flush操作时不能调用ar_hal_sys_mmz_free接口释放当前
          flush 的内存，否则会引发不可预知的异常
*/
AR_S32 ar_hal_sys_mmz_flush_cache_pa(AR_U64 u64_phy_addr, AR_U32 u32_size);

/**
* @brief  使 cache里的内容无效.
* @param  u64_phy_addr 待操作数据的起始物理地址.
* @param  u32_size 待操作数据的大小.
* @retval 0 成功 , 其它 失败.
* @note   当 cache里的数据为老时，为了保证不能直接访
          问内存时能够得到正确的数据，此时需要先调用此接口将cache里的内容无效
          ，这样，当CPU访问内存时，保证了数据的一致性和正确性
* @note   此接口应与ar_hal_sys_mmz_alloc_cached接口配套使用
* @note   用户需要确保传入参数的合法性
* @note   用户必须保证进行invalid操作时不能调用ar_hal_sys_mmz_free接口释放当前
          invalid 的内存，否则会引发不可预知的异常
*/
AR_S32 ar_hal_sys_mmz_invalid_cache_pa(AR_U64 u64_phy_addr, AR_U32 u32_size);

/**
* @brief  在用户态释放MMZ内存.
* @param  u64_phy_addr 物理地址.
* @param  p_vir_addr 虚拟地址指针.
* @retval 0 成功 , 其它 失败.
* @note   输入的地址必须为有效的物理地址，虚拟地址指针可以置为NULL
* @note   不能释放正在进行flush操作的内存，否则会引发不可预知的异常
*/
AR_S32 ar_hal_sys_mmz_free(AR_U64 u64_phy_addr, AR_VOID* p_vir_addr);

/**
* @brief  根据虚拟地址获取对应的内存信息，包括物理地址及cached 属性.
* @param  p_vir_addr 虚拟地址指针.
* @retval 0 成功 , 其它 失败.
* @note   输入必须为用户态虚拟地址。
* @note   不支持跨进程使用。
*/
AR_S32 ar_hal_sys_get_virmeminfo(AR_VOID* p_vir_addr, STRU_SYS_VIRMEM_INFO *pst_meminfo);

/**
* @brief  在用户态使用dma搬运物理内存.
* @param  u64_dest_phy_addr 目标物理地址.
* @param  u64_src_phy_addr 目标虚拟地址.
* @param  u32_len 拷贝长度.
* @retval 0 成功 , 其它 失败.
* @note   输入的地址必须为有效的物理地址
* @note   传输效率与目标地址，源地址和长度的最小对齐数有关，目标/源地址16字节对齐可以达到最大效率
*/
AR_S32 ar_hal_sys_memcpy_pa(AR_U64 u64_dest_phy_addr, AR_U64 u64_src_phy_addr, AR_U32 u32_len);

/**
* @brief  在用户态使用link list dma搬运物理内存.
* @param  tran_info 需要搬运的多个目标.
* @retval 0 成功 , 其它 失败.
* @note   输入的地址必须为有效的物理地址
* @note   传输效率与目标地址，源地址和长度的最小对齐数有关，目标/源地址16字节对齐可以达到最大效率
*/
AR_S32 ar_hal_sys_mem_transfer_pa(STRU_SYS_DMA_TRAN_INFO * pst_tran_info);
#if 0
/**
* @brief  在用户态使用dma按字节长度使用0填充物理地址（noncacheable内存）.
* @param  u64_dest_phy_addr 目标物理地址.
* @param  u32_len 填充长度（以字节为单位）.
* @retval 0 成功 , 其它 失败.
* @note   输入的地址必须为有效的物理地址
* @note   输入的物理地址必须为noncacheable的内存，不支持cacheable内存
*/
AR_S32 ar_hal_sys_zeromem_noncache_pa(AR_U64 u64_phy_addr, AR_U32 u32_len);
#endif
/**
* @brief  通过设备号和通道号获取索引.
* @param  u32_dir 表示方向 0 sender 1 receiver.
* @param  pst_chn 设备描述.
* @retval 大于等于0 成功 返回值为索引 , 其它 失败.
* @note   输入的pst_chn必须为有效的设备描述
*/
AR_S32 ar_hal_sys_get_index_by_dev_chn(AR_S32 u32_dir, const STRU_SYS_CHN_S *pst_chn);

/**
* @brief  检查设备号和通道号是否合法.
* @param  u32_dir 表示方向 0 sender 1 receiver.
* @param  pst_chn 设备描述.
* @retval 大于等于0 成功 返回值为索引 , 其它 失败.
* @note   输入的pst_chn必须为有效的设备描述
*/
AR_S32 ar_hal_sys_check_bind_chn(AR_S32 u32_dir, const STRU_SYS_CHN_S *pst_chn);

/**
* @brief  初始化系统绑定.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_bind_init();

/**
* @brief  去初始化系统绑定.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_bind_exit();
//AR_S32 ar_hal_sys_bind_show();

/**
* @brief  hal 层注册发送者.
* @param  pst_info 发送者的发送信息.
* @retval 0 成功 , 其它 失败.
* @note   必须给予合法的发送者信息，其中除了give_bind_call_back外，均需要给予初始化
*/
AR_S32 ar_hal_sys_bind_register_sender(STRU_SYS_BIND_SENDER *pst_info);

/**
* @brief  hal 层反注册发送者.
* @param  e_mod_id 发送者的module id .
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_bind_unregister_sender(ENMU_SYS_MOD_ID e_mod_id);

/**
* @brief  hal 层注册接收者.
* @param  pst_info 接收者的发送信息.
* @retval 0 成功 , 其它 失败.
* @note   必须给予合法的发送者信息，其中除了support_delay_data和reset_call_back外，均需要给予初始化
*/
AR_S32 ar_hal_sys_bind_register_receiver(STRU_SYS_BIND_RECEIVER *pst_info);

/**
* @brief  hal 层反注册接收者.
* @param  e_mod_id 接收者的module id .
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_bind_unregister_receiver(ENMU_SYS_MOD_ID e_mod_id);

/**
* @brief  数据源到数据接收者绑定接口.
* @param  pst_src_chn 源通道指针.
* @param  pst_dest_chn 目的通道指针.
* @retval 0 成功 , 其它 失败.
* @note   同一个数据接收者只能绑定一个数据源
* @note   绑定是指数据源和数据接收者建立关联关系。绑定后，数据源生成的数据将自动发送给接收者
* @note   VI和VDEC作为数据源，是以通道为发送者，向其他模块发送数据，用户将设备号置为0，SDK不检查输入的设备号
* @note   VPSS 作为数据接收者时，是以设备（GROUP）为接收者，接收其他模块发来的数据，用户将通道号置为0
* @note   VENC 作为数据接收者时，是以通道号为接收者，接收其他模块发过来的数据，用 户将设备号置为0，SDK不检查输入的设备号
* @note   AVS作为数据接收者时，是以设备（GROUP）、通道（PIPE）为接收者
*/
AR_S32 ar_hal_sys_bind(const STRU_SYS_CHN_S *pst_src_chn, const STRU_SYS_CHN_S *pst_dest_chn);

/**
* @brief  数据源到数据接收者解绑定接口.
* @param  pst_src_chn 源通道指针.
* @param  pst_dest_chn 目的通道指针.
* @retval 0 成功 , 其它 失败.
* @note   pstDestChn 如果找不到绑定的源通道，则直接返回成功，如果找到了绑定的源通道，
          但是绑定的源通道和pstSrcChn不匹配，则返回失败
*/
AR_S32 ar_hal_sys_unbind(const STRU_SYS_CHN_S *pst_src_chn, const STRU_SYS_CHN_S *pst_dest_chn);

//AR_S32 ar_hal_sys_send_data(ENMU_SYS_MOD_ID e_mod_id, AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_S32 s32_flag, ENMU_SYS_DATA_TYPE e_data_type, AR_VOID *pv_data);
//AR_S32 ar_hal_sys_reset_data(ENMU_SYS_MOD_ID e_mod_id, AR_S32 s32_dev_id, AR_S32 s32_chn_id, AR_VOID *pv_private);

/**
* @brief  获取此通道上绑定的源通道的信息.
* @param  pst_src_chn 源通道指针.
* @param  pst_bind_src 绑定的目的指针.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_bind_by_src(STRU_SYS_CHN_S *pst_src_chn, STRU_SYS_BIND_DEST *pst_bind_src);

/**
* @brief  获取此通道上绑定的源通道的信息.
* @param  pst_src_chn 源通道指针.
* @param  u32_bind_num 绑定的数量.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_bind_num_by_src(STRU_SYS_CHN_S *pst_src_chn, AR_U32 *u32_bind_num);

/**
* @brief  获取此通道上绑定的源通道的信息.
* @param  pst_dest_chn 目的通道指针.
* @param  pst_src_chn 源通道指针.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_bind_by_dest(STRU_SYS_CHN_S *pst_dest_chn, STRU_SYS_CHN_S *pst_src_chn);

/**
* @brief  获取当前的时间戳.
* @param  pst_cur_pts 当前时间戳指针.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_cur_pts(AR_U64* pst_cur_pts);

/**
* @brief  初始化 MPP 的时间戳基准.
* @param  u64_pts_base 时间戳基准。单位：微秒.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_init_pts_base(AR_U64 u64_pts_base);

/**
* @brief  同步 MPP 的时间戳.
* @param  u64_pts_base 时间戳基准。单位：微秒.
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_sync_pts(AR_U64 u64_pts_base);

/**
* @brief  获取时间戳offset.
* @retval 时间戳offset
*/
AR_S64 ar_hal_sys_get_pts_offset(AR_S64* ps64_pts_offset);

/**
* @brief  设置时区信息.（时区设置对JPEG DCF信息有效，设置后拍摄时间（UTC 时间）将转换为本地时间）
* @param  s32_time_zone 时区，相对于UTC时间的偏移，单位：秒，取值范围：[-86400, 86400]，对应为[-24, 24]小时。
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_set_time_zone(AR_S32 s32_time_zone);

/**
* @brief  获取时区信息.
* @param  s32_time_zone 时区，相对于UTC时间的偏移，单位：秒。
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_time_zone(AR_S32 *ps32_time_zone);

/**
* @brief  设置GPS 信息.（GPS 信息设置对JPEG DCF信息有效。）
* @param  pst_gps_info GPS 信息，输入。
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_set_gps_info(const STRU_SYS_GPS_INFO *pst_gps_info);

/**
* @brief  获取GPS 信息.（GPS 信息设置对JPEG DCF信息有效。）
* @param  pst_gps_info GPS 信息，输出。
* @retval 0 成功 , 其它 失败.
*/
AR_S32 ar_hal_sys_get_gps_info(STRU_SYS_GPS_INFO *pst_gps_info);

/**
* @brief  获取SOC 信息.
* @retval NULL 失败 , 其它 成功.
*/
STRU_SYS_AR_SOC_INFO * ar_hal_sys_get_soc_info();

/**
* @brief  获取SOC Revision 信息.
* @retval AR_FAILURE 失败 , 其它 成功.
*/
AR_S32 ar_hal_sys_get_soc_revision();

/**
* @brief  获取SOC Product ID信息.
* @retval AR_FAILURE 失败 , 其它 成功, 相关定义参看ENMU_SYS_AR_SOC_TYPE.
*/
AR_S32 ar_hal_sys_get_soc_id();

AR_S32 ar_hal_sys_check_open();


AR_S32 ar_hal_sys_exit(void);

///////////////////  dump file	///////////////////////
typedef  struct __STRU_AR_FIFO_T STRU_AR_FIFO_T;
typedef struct __STRU_DUMP_OBJ_T STRU_DUMP_OBJ_T;
/**
* @brief  dump 服务对象定义
*/
struct	__STRU_DUMP_OBJ_T
{
  AR_S32 current_index;
  AR_S32 req_count;
  ar_lock_t dump_lock;
  ar_lock_t req_lock;
  STRU_AR_FIFO_T *dump_queue;
  AR_S32  remote_fd;
  AR_S32 dma_fd;
  ar_os_semaphore_id_t dump_queue_signal;
  ar_os_thread_id_t dump_task;
  AR_VOID (*dump)(STRU_DUMP_OBJ_T *server_l,AR_VOID *y,AR_VOID *u,AR_VOID *v,AR_VOID *meta,AR_S32 y_len,AR_S32 u_len,AR_S32 v_len,AR_S32 meta_len);
  AR_VOID (*dump_segment)(STRU_DUMP_OBJ_T *server_l,AR_VOID *y,AR_VOID *u,AR_VOID *v,AR_VOID *meta,AR_S32 y_len,AR_S32 u_len,AR_S32 v_len,AR_S32 meta_len);
  AR_VOID (*dump_req)(STRU_DUMP_OBJ_T *server_l,AR_CHAR *name,AR_S32 num);
};
/**
* @brief  创建一个dump 对象
* @retval NULL 失败 , 其它 成功, 
*/


STRU_DUMP_OBJ_T *ar_hal_sys_creat_dump_server();
/**
* @brief  删除一个dump 对象
* @retval 无
*/

AR_VOID ar_hal_sys_delete_dump_server(STRU_DUMP_OBJ_T *server_l);


//ar sys queue


STRU_AR_FIFO_T *ar_hal_sys_creat_queue(AR_S32 queue_size,char *name);
AR_S32 ar_hal_sys_delete_queue(STRU_AR_FIFO_T **queue);

struct __STRU_AR_FIFO_T
{
 AR_CHAR * name;
 AR_S32 queue_size;
 AR_S32 valid_data_num;
 //queue data
 AR_VOID **data;
 AR_S32 header;
 AR_S32 tail;
 //lock
 ar_lock_t lock;
 AR_S32 (*queue_pop)(STRU_AR_FIFO_T * queue, AR_VOID **item);
 AR_S32 (*queue_insert)(STRU_AR_FIFO_T * queue,AR_VOID *item);
 AR_S32 (*get_queue_size)(STRU_AR_FIFO_T * queue);
 AR_S32 (*look_up_head)(STRU_AR_FIFO_T * queue, AR_VOID **item);
};


typedef struct
{
 AR_S32 args_num;
 AR_CHAR args[64][128];
}STRU_CMD_ARG_T;
STRU_CMD_ARG_T *ar_hal_sys_get_args(const AR_CHAR *pcCommandString);
AR_S32 ar_hal_sys_find_pra_index_by_name(STRU_CMD_ARG_T * args,AR_CHAR *name);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif /* __HAL_SYS_H__ */


