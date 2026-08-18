#ifndef __PF_SENSOR_H__
#define __PF_SENSOR_H__

#include "hal_vin_tuning_def.h"

#include "ar_combo_dev.h"
#include "ar_comm_vi.h"

typedef enum IPC_SNS_TYPE_E
{
    SONY_IMX307_MIPI_2M_30FPS_12BIT,
    SONY_IMX307_MIPI_2M_30FPS_12BIT_WDR2TO1,
    SONY_IMX307_SLAVE_MIPI_2M_60FPS_10BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT,
    SONY_IMX334_MIPI_8M_30FPS_12BIT_WDR2TO1,
    SONY_IMX377_MIPI_8M_30FPS_10BIT,
    NVP6324_MIPI_2M_30FPS_8BIT,
    SONY_IMX415_MIPI_8M_30FPS_12BIT,
    SONY_IMX415_MIPI_8M_30FPS_12BIT_WDR2TO1,
    SONY_IMX415_MIPI_8M_60FPS_12BIT,
    SONY_IMX415_MIPI_8M_60FPS_12BIT_WDR2TO1,
    SONY_IMX464_MIPI_4M_30FPS_12BIT,
    SONY_IMX464_MIPI_4M_30FPS_12BIT_WDR2TO1,
    TP9930_DVP_4VC_1080P_25FPS,//pal
    TP9930_DVP_4VC_1080P_30FPS,//ntsc
	SC_SC910GS_MIPI_9M_20FPS_10BIT,
	SC_SC530AI_MIPI_5M_30FPS_10BIT,
#ifdef THERMAL_ENABLE
    ISP_VIN_COMMON_INF,
    TIMO256_DVP_256_IR,
    ELF1_DVP_256_IR,
    NK1221A_DVP_384_IR,
#endif
    OV_OS04A10_MIPI_4M_25FPS_12BIT,
    OV_OS04A10_MIPI_4M_25FPS_10BIT_WDR2TO1,
    OV_OS04A10_MIPI_BINNING_25FPS_10BIT,
    OV_OS04A10_MIPI_BINNING_25FPS_12BIT,
    SONY_IMX347_MIPI_4M_30FPS_12BIT,
    GST417W_IR_400X308,                     // ir_raw
    ISP_VIN_IR_THERMAL_GST417W,             // ir_thermal
    ISP_VIN_IR_ISP0_GST417W,                    // ir_isp0,1
    ISP_VIN_IR_ISP1_GST417W,
    ISP_VIN_IR_ISP0_WITH_THERMAL_GST417W,   //Thermal+isp0 together with hw gtm
    GST212W4_IR_256X200,                    // gst212w4 ir_raw
    ISP_VIN_IR_THERMAL_GST212W4,            // gst212w4 ir_thermal
    ISP_VIN_IR_ISP0_GST212W4,                   // gst212w4 ir_isp1
    ISP_VIN_IR_ISP0_WITH_THERMAL_GST212W4,      // gst212w4 ir_isp1 and ir_thermal (share one device)
    ISP_VIN_IR_ISP1_GST212W4,                   // gst212w4 ir_isp2
	OV_OS05A20_2688x1944,
	
    H3812C1SG_IR_444X336,                   // h3812c1sg ir_raw
    ISP_VIN_IR_THERMAL_H3812C1SG,           // h3812c1sg ir_thermal
    ISP_VIN_IR_ISP0_H3812C1SG,                  // h3812c1sg ir_isp1
    ISP_VIN_IR_ISP0_WITH_THERMAL_H3812C1SG,     // h3812c1sg ir_isp1 and ir_thermal (share one device)
    ISP_VIN_IR_ISP1_H3812C1SG,                  // h3812c1sg ir_isp2

    SC_SC230AI_MIPI_2M_30FPS_10BIT,

    ISP_RAW_SIMULATION, //isp recv raw data from isp tool
    
    IPC_SNS_TYPE_BUTT,
} IPC_SNS_TYPE_E;

AR_S32 IPC_PF_VI_GetComboAttrBySns(IPC_SNS_TYPE_E enSnsType, combo_dev_t MipiDev, STRU_COMBO_DEV_ATTR_T* pstComboAttr);
AR_S32 IPC_PF_VI_GetDevAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_DEV_ATTR_S* pstViDevAttr);
AR_S32 IPC_PF_VI_GetPipeAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_PIPE_ATTR_S* pstPipeAttr);
AR_S32 IPC_PF_VI_GetChnAttrBySns(IPC_SNS_TYPE_E enSnsType, VI_CHN_ATTR_S* pstChnAttr);


#endif

