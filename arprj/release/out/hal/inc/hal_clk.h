#ifndef __HAL_CLK_RPC_H__
#define __HAL_CLK_RPC_H__

#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define AR_CLK_DEV "/dev/ar_clk"

/* 9311 CLOCK ID */
#define CGU_AXI_CLK			0x00000000
#define CGU_AXI_CLK_PRE_A		0x01000000
#define CGU_AXI_CLK_PRE_B		0x02000000
#define CGU_SYS_CLK			0x03000000
#define CGU_SYS_CLK_PRE_A		0x04000000
#define CGU_SYS_CLK_PRE_B		0x05000000
#define CGU_ISP_CLK			0x06000000
#define CGU_ISP_HDR_CLK			0x07000000
#define CGU_VIF_AXI_CLK			0x08000000
#define CGU_DLA_CLK			0x09000000
#define CGU_RSZ_CLK			0x0a000000
#define CGU_DE_CLK			0x0b000000
#define CGU_VENC_CLK			0x0c000000
#define CGU_JPEG_CLK			0x0d000000
#define CGU_MIPI_CSI_0_CLK		0x0e000000
#define CGU_MIPI_CSI_1_CLK		0x0f000000
#define CGU_MIPI_PCS_CLK		0x10000000
#define CGU_SD0_FIX_CLK			0x11000000
#define CGU_SD0_SAMPLE_CLK		0x12000000
#define CGU_SD0_DRV_CLK			0x13000000
#define CGU_SD1_FIX_CLK			0x14000000
#define CGU_SD1_SAMPLE_CLK		0x15000000
#define CGU_SD1_DRV_CLK			0x16000000
#define CGU_EMMC_FIX_CLK		0x17000000
#define CGU_EMMC_SAMPLE_CLK		0x18000000
#define CGU_EMMC_DRV_CLK		0x19000000
#define CGU_QSPI_CLK			0x1a000000
#define CGU_I2S_MCLK			0x1b000000
#define CGU_I2S_MST0_SCLK		0x1c000000
#define CGU_I2S_MST1_SCLK		0x1d000000
#define CGU_USB_PHY0_CLK		0x1e000000
#define CGU_AUDIO_300M			0x1f000000
#define CGU_AUDIO_ADC_CLK		0x20000000
#define CGU_GMAC_CORE_CLK		0x21000000
#define CGU_GMAC_PHY_CLK		0x22000000
#define CGU_HDECOMP_CLK			0x23000000
#define CGU_EFUSE_CLK			0x24000000
#define CGU_SENSOR_MCLK0		0x25000000
#define CGU_SENSOR_MCLK1		0x26000000
#define CGU_SENSOR_MCLK2		0x27000000
#define CGU_DVP_PATTERN_CLK		0x28000000
#define CGU_DVP_SUB_1_2X_PIX_CLK	0x29000000
#define CGU_NUC_CLK			0x2a000000
#define CGU_SCGMAC_PTP_CLK		0x2b000000
#define CGU_SCGMAC_RGMIITX_CLK		0x2c000000
#define CGU_SCGMAC_MDC_CLK		0x2d000000
#define CGU_NPU_CLK			0x2e000000
#define CGU_NPU_CLK_PRE_A		0x2f000000
#define CGU_NPU_CLK_PRE_B		0x30000000
#define CGU_NPU_ACLK			0x31000000
#define CGU_IFC_CLK			0x32000000
#define CGU_EIS_CLK			0x33000000
#define CGU_SCALER_CLK			0x34000000
#define CGU_GEN_CLK			0x35000000
#define CGU_SCAN_CLK_60M		0x36000000
#define CGU_SCAN_CLK_100M		0x37000000
#define CGU_SCAN_CLK_150M		0x38000000
#define CGU_SCAN_CLK_300M		0x39000000
#define CGU_SCAN_CLK_480M		0x3a000000
#define CGU_SCAN_CLK_2400M		0x3b000000
#define CGU_SCAN_CLK_I2S		0x3c000000
#define MIPI_TX_PLL_ATE_OUT_CLK		0x3d000000
#define AUDIO_PLL_ATE_OUT_CLK		0x3e000000
#define PIXEL_PLL_ATE_OUT_CLK		0x3f000000
#define FIX_PLL_ATE_OUT_CLK		0x40000000
#define ADC_PLL_ATE_OUT_CLK		0x41000000
#define CGU_CPU_CLK			0x42000000
#define CGU_CPU_CLK_PRE			0x43000000
#define CGU_CPU_CLK_PRE_A		0x44000000
#define CGU_CPU_CLK_PRE_B		0x45000000
#define CGU_CS_DBG_CLK			0x46000000

enum ar9311_clk_src {
	CGU_OSCIN_CLK,
	FIX_PLL_CLK100,
	FIX_PLL_CLK125,
	FIX_PLL_CLK250,
	FIX_PLL_CLK333,
	FIX_PLL_CLK400,
	FIX_PLL_CLK500,
	FIX_PLL_CLK600,
	FIX_PLL_CLK666,
	FIX_PLL_CLK800,
	FIX_PLL_CLK1000,
	FIX_PLL_CLK2000,
	ADC_PLL_CLK20,
	ADC_PLL_CLK25,
	ADC_PLL_CLK50,
	ADC_PLL_CLK60,
	ADC_PLL_CLK100,
	ADC_PLL_CLK150,
	ADC_PLL_CLK300,
	ADC_PLL_CLK400,
	ADC_PLL_CLK600,
	SD0_FIX_CLK,
	SD0_SAMPLE_CLK,
	SD0_DRV_CLK,
	SD1_FIX_CLK,
	SD1_SAMPLE_CLK,
	SD1_DRV_CLK,
	EMMC_FIX_CLK,
	EMMC_SAMPLE_CLK,
	EMMC_DRV_CLK,
//	CGU_I2S_MCLK,
	CLK_AUDIO_DIG,
	PIXEL_PLL_CLK,
	PIXEL_PLL_CLK1,
	PIXEL_PLL_CLK2,
	PIXEL_PLL_CLK3,
	PIXEL_PLL_CLK4,
	AUDIO_PLL_CLK,
	MIPI_PLL,
	MIPI_TX_PLL_DIV2,	// = MIPI_PLL*POST_DIV/2
	TST_PLL_DDR,
	CLK_SRC_INVAILD
};

enum ar9311_power_gate {
	PWR_CORE1,
	PWR_VISION,
	PWR_NPU_CORE,
	PWR_NPU_TOP,
	PWR_DLA,
	PWR_VENC
};

/* 9301 CLOCK ID */
// coresight
#define CLK_CFG_CS		0
#define CLK_CORE_CS		1
// hdecomp
#define CLK_CFG_HDECOMP		2
#define CLK_CORE_HDECOMP	3
// topdmac
#define CLK_CFG_TOPDMAC		4
#define CLK_AXI_TOPDMAC		5
#define CLK_CORE_TOPDMAC	6
// emmc
#define CLK_CFG_EMMC		7
#define CLK_EMMC_CARDCLK	8
// security
#define CLK_CFG_SEC		9
#define CLK_CFG_EFUSE		10
// rom
#define CLK_CFG_ROM		11
// spi debug
#define CLK_CFG_SPIDBG		12
// unknown
#define CLK_150_NOC_G0		13
// isp rsz ifc eis
#define CLK_CFG_ISP		14
#define CLK_ISP			15
#define CLK_AXI_VIF		16
#define CLK_HDR			17
#define CLK_CFG_SCALER		18
#define CLK_SCALER		19
// jpeg
#define CLK_CFG_JPEG		20
#define CLK_CORE_JPEG		21
// disp
#define CLK_CFG_DISP		22
#define CLK_CORE_DISP		23
// Vision Network
#define CLK_150_NOC_G1		24
// ceva top
#define CLK_CFG_CEVA		25
#define CLK_150_NOC_G2		26
// dla_top
#define CLK_CFG_DLA		27
// venc
#define CLK_CFG_VENC		28
#define CLK_AXI_VENC		29
#define CLK_BPU_VENC		30
// unknown
#define CLK_150_NOC_G3		31
// unknown
#define CLK_150_NOC_G4		32
// disp
#define CLK_PIXEL_DISP		33
// dvpctl
#define CLK_PATTERN_GEN		34
// dvp_pin_topology
#define CLK_SUB_1_1X_PIX	35
// disp vif
#define CLK_DVP_PIX_HLY		36
// vif dvp_pin_topology
#define CLK_SUB_1_2X_PIX	37
// vif
#define CLK_DVP_PIX		38
// mipi_dsi  disp
#define CLK_DSI_PIX		39
// ddr
#define CLK_CFG_DDR		40
#define CLK_DDR			41
// mipi_csi
#define CLK_CFG_MIPI		42
#define CLK_VIDEO_MIPI0		43
#define CLK_VIDEO_MIPI1		44
#define CLK_VIDEO_MIPI2		45
#define CLK_VIDEO_MIPI3		46
#define CLK_PCS_MIPI		47
// mipi_dsi
#define CLK_CFG_DSI		48
// vif
#define CLK_CFG_VIF		49
// dvpctl
#define CLK_CFG_DVPCTL		50
// usb20_0
#define CLK_CFG_USB2OTG0	51
#define CLK_PHY_USB2PHY0	52
// usb20_1
#define CLK_CFG_USB2OTG1	53
#define CLK_PHY_USB2PHY1	54
// USB20_2
#define CLK_CFG_USB2OTG2	55
#define CLK_PHY_USB2PHY2	56
// unknown
#define CLK_150_NOC_G5		57
// gmac
#define CLK_CFG_GMAC		58
#define CLK_CORE_GMAC		59
#define CLK_PHY_GMAC		60
// cci
#define CLK_CCI			61
// sram
#define CLK_SRAM		62
// unknow
#define CLK_600_NOC		63
// clk sensor
#define CLK_SENSOR0		64
#define CLK_SENSOR1		65
#define CLK_SENSOR2		66
#define CLK_SENSOR3		67
// peripheral
#define CLK_CORE_PERIP		68
// audio_codec
#define CLK_AUDIO_300M		69
#define CLK_AUDIO_ADC		70
// i2s_mst
#define CLK_PERIP_I2SM		71
// sdcard
#define CLK_SDC_FIXED		72
#define CLK_SDC_SAMPLE		73
#define CLK_SDC_DRV		74
// qspi
#define CLK_CORE_QSPI		75
#define CLK_MAX_GATING		76
// pll0
#define CLK_PLL_0		77
// pll1
#define CLK_PLL_1		78
// pll_adda
#define CLK_PLL_ADDA		79
// pll_back
#define CLK_PLL_BACK		80
// pll_pixel
#define CLK_PLL_PIXEL		81

// pll_audio
#define CLK_PLL_AUDIO		82

/* Selector value */
#define	CLK_SELECT_0		0
#define	CLK_SELECT_1		1
#define	CLK_SELECT_2		2
#define	CLK_SELECT_3		3
#define	CLK_SELECT_4		4
#define	CLK_SELECT_5		5
#define	CLK_SELECT_6		6
#define	CLK_SELECT_7		7
#define	CLK_SELECT_8		8
#define	CLK_SELECT_9		9
#define	CLK_SELECT_10		10
#define	CLK_SELECT_11		11
#define	CLK_SELECT_12		12
#define	CLK_SELECT_13		13
#define	CLK_SELECT_14		14
#define	CLK_SELECT_15		15
#define CLK_SELECT_SHIFT	16

typedef enum
{
    CLK_RPC_ID_SET_RATE           = 1,
    CLK_RPC_ID_ENABLE_BYID,
    CLK_RPC_ID_DISABLE_BYID,
    CLK_RPC_ID_SET_PIX_CLK,
    CLK_RPC_ID_GET_RATE,
    CLK_RPC_ID_SET_RATE_SAFE,

    CLK_RPC_ID_SOURCE_CLAIM,
    CLK_RPC_ID_SOURCE_RELEASE,
    CLK_RPC_ID_SOURCE_BIND,
    CLK_RPC_ID_SET_FREQ,
    CLK_RPC_ID_GET_FREQ,
    PWR_RPC_ID_POWER_ON,
    PWR_RPC_ID_POWER_OFF,
} ENUM_CLK_RPC_ID;

typedef enum
{
    CLK_AUDIO_RATE_8000_64000, //8000, 16000, 32000, 64000
    CLK_AUDIO_RATE_11025_44100,//11025, 22050, 44100
    CLK_AUDIO_RATE_12000_96000,//12000, 24000, 48000, 96000
    CLK_AUDIO_RATE_BUTT,
} ENUM_CLK_AUDIO;

struct clk_data_t
{
    unsigned long id;
    unsigned long rate_khz;
    unsigned int src_id;
    unsigned int freq;
};

int ar_hal_clk_set_rate(unsigned int clk_id, unsigned long rate_khz);
int ar_hal_clk_set_rate_safe_hz(unsigned int clk_id, unsigned long rate_hz);
unsigned long ar_hal_clk_get_rate_hz(unsigned int clk_id);
int ar_hal_clk_enable_byid(unsigned int clk_id);
int ar_hal_clk_disable_byid(unsigned int clk_id);
int ar_hal_set_pix_clk(float rate_khz);
int ar_hal_audio_clk_select(ENUM_CLK_AUDIO rate);

int ar_hal_clk_src_claim(unsigned int src_id, unsigned int freq);
int ar_hal_clk_src_release(unsigned int src_id);
int ar_hal_clk_src_bind(unsigned int clk_id, unsigned int src_id);
int ar_hal_clk_set_freq(unsigned int clk_id, unsigned int freq);
int ar_hal_clk_get_freq(unsigned int clk_id, unsigned int *freq);
int ar_hal_pwr_ctrl_on(unsigned int pwr_id);
int ar_hal_pwr_ctrl_off(unsigned int pwr_id);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif //__HAL_CLK_RPC_H__

