#ifndef __AR_MIPI_RX_H__
#define __AR_MIPI_RX_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

typedef unsigned int combo_dev_t;


#define MIPI_LANE_NUM           8

#define WDR_VC_NUM              4
#define SYNC_CODE_NUM           4

#define MIPI_RX_MAX_DEV_NUM     5
#define SLVS_MAX_DEV_NUM        1
#define CMOS_MAX_DEV_NUM        2

#define SNS_MAX_CLK_SOURCE_NUM  3
#define SNS_MAX_RST_SOURCE_NUM  3


typedef enum
{
    LANE_DIVIDE_MODE_0    = 0,
    LANE_DIVIDE_MODE_1    = 1,
    LANE_DIVIDE_MODE_2    = 2,
    LANE_DIVIDE_MODE_3    = 3,
    LANE_DIVIDE_MODE_4    = 4,
    LANE_DIVIDE_MODE_5    = 5,
    LANE_DIVIDE_MODE_6    = 6,
    LANE_DIVIDE_MODE_BUTT
} lane_divide_mode_t;

typedef enum
{
    WORK_MODE_LVDS          = 0x0,
    WORK_MODE_MIPI          = 0x1,
    WORK_MODE_CMOS          = 0x2,
    WORK_MODE_BT1120        = 0x3,
    WORK_MODE_SLVS          = 0x4,
    WORK_MODE_BUTT
} work_mode_t;

typedef enum
{
    INPUT_MODE_MIPI         = 0x0,              /* mipi */
    INPUT_MODE_SUBLVDS      = 0x1,              /* SUB_LVDS */
    INPUT_MODE_LVDS         = 0x2,              /* LVDS */
    INPUT_MODE_ARSPI        = 0x3,              /* ARSPI */
    INPUT_MODE_SLVS         = 0x4,              /* SLVS */
    INPUT_MODE_CMOS         = 0x5,              /* CMOS */
    INPUT_MODE_BT601        = 0x6,              /* BT601 */
    INPUT_MODE_BT656        = 0x7,              /* BT656 */
    INPUT_MODE_BT1120       = 0x8,              /* BT1120 */
    INPUT_MODE_BYPASS       = 0x9,              /* MIPI Bypass */
    INPUT_MODE_MEM          = 0Xa,              /* ISP IN ,SEND A RAW MEM TO ISP*/

    INPUT_MODE_BUTT
} input_mode_t;


typedef enum
{
    MIPI_DATA_RATE_X1 = 0,         /* output 1 pixel per clock */
    MIPI_DATA_RATE_X2 = 1,         /* output 2 pixel per clock */
    MIPI_DATA_RATE_BUTT
} mipi_data_rate_t;

typedef struct
{
    int x;
    int y;
    unsigned int width;
    unsigned int height;
} img_rect_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
} img_size_t;

typedef enum
{
    DATA_TYPE_RAW_8BIT = 0,
    DATA_TYPE_RAW_10BIT,
    DATA_TYPE_RAW_12BIT,
    DATA_TYPE_RAW_14BIT,
    DATA_TYPE_RAW_16BIT,
    DATA_TYPE_YUV420_8BIT_NORMAL,
    DATA_TYPE_YUV420_8BIT_LEGACY,
    DATA_TYPE_YUV422_8BIT,
    DATA_TYPE_BUTT
} data_type_t;

/* MIPI D_PHY WDR MODE defines */
typedef enum
{
    AR_MIPI_WDR_MODE_NONE = 0x0,
    AR_MIPI_WDR_MODE_VC   = 0x1,    /* Virtual Channel */
    AR_MIPI_WDR_MODE_DT   = 0x2,    /* Data Type */
    AR_MIPI_WDR_MODE_DOL  = 0x3,    /* DOL Mode */
    AR_MIPI_WDR_MODE_BUTT
} mipi_wdr_mode_t;

typedef struct
{
    unsigned int timing_mode; //0:auto;1 manual
    unsigned int hsa;
    unsigned int hbp;
    unsigned int hsd;
}mipi_ipi_timing_t;

typedef struct
{
    data_type_t           input_data_type;          /**<data type: 8/10/12/14/16 bit */
    mipi_wdr_mode_t       wdr_mode;                 /**<@note 未使用 MIPI WDR mode */
    short                 lane_id[MIPI_LANE_NUM];  /**<lane_id: -1 - disable */
    unsigned int          mipi_pix_clk;           /**<note新加参数，mipi     控制器 像素时钟 */
    unsigned int          settle_count;         /**<note 新加参数，mipi 时序settle count*/
    union
    {
        short data_type[WDR_VC_NUM];                /**<@note 未使用 used by the AR_MIPI_WDR_MODE_DT */
    };
    mipi_ipi_timing_t ipi_timing;   /**<note 新加参数，mipi 控制器时序*/
} mipi_dev_attr_t;

typedef enum
{
    AR_WDR_MODE_NONE    = 0x0,
    AR_WDR_MODE_2F      = 0x1,
    AR_WDR_MODE_3F      = 0x2,
    AR_WDR_MODE_4F      = 0x3,
    AR_WDR_MODE_DOL_2F  = 0x4,
    AR_WDR_MODE_DOL_3F  = 0x5,
    AR_WDR_MODE_DOL_4F  = 0x6,
    AR_WDR_MODE_BUTT
} wdr_mode_t;


typedef struct
{
    int reset_gpio[3];  /**<新加参数 reset_gpio[0]:group  reset_gpio[1]:port  reset_gpio[2]:num*/
    int power_gpio[3];  /**<新加参数 power_gpio[0]:group  power_gpio[1]:port  power_gpio[2]:num*/
    int common_gpio[3]; /**<新加参数 common_gpio[0]:group  common_gpio[1]:port  common_gpio[2]:num*/
    int mclk_src;        /**<新加参数,the src of ckk id*/
	int mclk_k;          /**<新加参数,the fre of mclk_src*/
	int mclk_id;	     /**<新加参数,9311 supported, the mclk id*/
	int mclk_id_k;      /**<新加参数,9311 supported, the clk     khz of mclk_id*/
}ar_dev_power_attr_t;

typedef struct
{
    int timing_mode; //0:auto;1:manual
    int de_delay;
    int htotal;
    int hstart;
}vif_line_buffer_timing_t;

typedef struct
{
    combo_dev_t         devno;              /**<device number */
    input_mode_t        input_mode;         /**<input mode: MIPI/LVDS/SUBLVDS/ARSPI/DC */
    int    data_rate;                       /**<@note 转义参数 sensor mipi out data lane rate M eg:450*/
    img_rect_t          img_rect;           /**<@note 不使用参数 MIPI Rx device crop area (corresponding to the oringnal sensor input image size) */
    union
    {
        mipi_dev_attr_t     mipi_attr;   /**<@note 修改参数 mipi的属性*/
    };
	ar_dev_power_attr_t  dev_power_attr; /**<@note 新加参数        描述sensor 电源*/
	vif_line_buffer_timing_t line_timing; /**<@note 新加参数 mipi数据到vif的 时序信息*/
} STRU_COMBO_DEV_ATTR_T;

/**
\brief 设置mipi vif sensor 等设备的属性。
@attention 新加接口
\param[in] pstDevAttr :   复合设备的属性 
\retval ::0             :   成功。
\retval ::non-zero      :   失败，其值为错误码。
\see \n
N/A
*/

int AR_MPI_VI_SetComboDevAttr(const STRU_COMBO_DEV_ATTR_T *pstDevAttr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* __AR_MIPI_RX_H__ */
