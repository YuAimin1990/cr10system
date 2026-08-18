/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _AR9311_CPU_H
#define _AR9311_CPU_H

/*FIXME: These addresses need to be fixed according to Artosyn register map*/

#define BOOT_DEVICE_STATUS_BASE_REG	(0x0A106100)
#define UPGRADE_BOOT_UBOOT_ADDR_REG	(0x0A10613c)
#define UPGRADE_BOOT_UBOOT_MAX_SIZE	(2 * 512 * 1024)

#ifdef CONFIG_SPL_BUILD
#ifdef VELOCE_DDR
#define DDR_PHY_BASE		0x0c000000
#define DDR_CTR_BASE		0x0e000000
#define DDR_REG_BASE		0x0e010000
#define DDR_MON_BASE		0x0e020000
#define AXI_MON0_BASE		0x0e030000
#define AXI_MON1_BASE		0x0e040000
#define AXI_MON2_BASE		0x0e050000
#define CASE_PASS_ADDR		0x5ffffff0
#define CASE_FAIL_ADDR		0x5ffffff4
#endif
#endif

#define ARTOSYN_UART0_BASE  0x01500000

#define ARTOSYN_CCI_BASE    0x00300000
#define CCI_SECURE_ACCESS_REG  (0x8)
#define CCI_PMCR        (0x100)

#define CCI_S0_BASE (0x1000)
#define CCI_S1_BASE (0x2000)
#define CCI_S2_BASE (0x3000)
#define CCI_S3_BASE (0x4000)
#define CCI_S4_BASE (0x5000)

#define CCI_S0_SNOOP_CTRL_REG (CCI_S0_BASE + 0)
#define CCI_S1_SNOOP_CTRL_REG (CCI_S1_BASE + 0)
#define CCI_S2_SNOOP_CTRL_REG (CCI_S2_BASE + 0)
#define CCI_S3_SNOOP_CTRL_REG (CCI_S3_BASE + 0)
#define CCI_S4_SNOOP_CTRL_REG (CCI_S4_BASE + 0)


/**********Tz define****************************/
#define    TZC_4F_BASE      0x08430000  //4-filter

// TZC400 register offset
#define BUILD_CONFIG		0x000
#ifdef __UBOOT__
#define ACTION			0x004
#endif
#define GATE_KEEPER		0x008
#define SPECULATION_CTRL	0x00C
#define INT_STATUS		0x010
#define INT_CLEAR		0x014

//Filter0~3: 0x10*x x=0~3
#define FAIL_ADDR_LOW		0x020 + (0x10 * 0)
#define FAIL_ADDR_HIGH		0x024 + (0x10 * 0)
#define FAIL_CONTROL		0x028 + (0x10 * 0)
#define FAIL_ID			0x02C + (0x10 * 0)

//Region0~8: 0x20*n n=1~8
#define RGN_BASE_LOW		0x100 + (0x20 * 0)
#define RGN_BASE_HIGH		0x104 + (0x20 * 0)
#define RGN_TOP_LOW		0x108 + (0x20 * 0)
#define RGN_TOP_HIGH		0x10C + (0x20 * 0)
#define RGN_ATTRIBUTE		0x110 + (0x20 * 0)
#define RGN_ID_ACCESS		0x114 + (0x20 * 0)

// power-on pull up register
#define RTC_RF_BASE			(0x0a10c000)
#define GLOBAL_TIMER_GATE_CTL		(0x09006000)
#define RTC_CON1			(0x04)
#define SYS_POWER_ON_CTRL		(RTC_RF_BASE + RTC_CON1)

#define RTC_PMC_BASE			(0x0a10c100)
#define  RTC_PMC_PWR_CON		(0x00)
#define  RTC_PMC_SRC_ENA		(0x04)
#define  RTC_PMC_WAKEUP_MODE		(0x08)
#define  RTC_PMC_INT_EN			(0x0c)
#define  RTC_PMC_INT_ST			(0x10)
#define  RTC_PMC_BUTTON_TIME		(0x14)
#define  RTC_PMC_OFF_TIME		(0x18)
#define  RTC_PMC_EN_CTL			(0x1c)
#define  RTC_PMC_EN_ON_TIME		(0x20)
#define  RTC_PMC_EN_OFF_TIME		(0x24)
#define  RTC_PMC_MSC			(0x28)
#define  RTC_PMC_STAT			(0x2c)
#define  RTC_SRAM_CTL			(0x30)
#define  RTC_PMC_RESERVE_0		(0x34)
#define  RTC_PMC_VER			(0x3c)

#define WARMBOOT_FLAG_ADDR		RTC_PMC_BASE + RTC_PMC_RESERVE_0

#define RTC_SRAM_BASE			(0x0a10d000)

#define CPU_CONFIG_BASE			(0x0a100000)
#define CPU_RESET_CTRL			(CPU_CONFIG_BASE + 0xc)
#define CPU_POR_RESET_1			(1 << 1)
#define CPU_CORE_RESET_1		(1 << 5)

#define GIC_BASE			(0x00200000)


#if 0
#define ARTOSYN_PINSHARE_BASE		(0x0a10a000)
#define ARTOSYN_PINSELECT_BASE		(0x01073130)

#define ARTO_USB_BOOT_REG		7
#define ARTO_USB_BOOT_SEL_PAD		2
#define ARTO_USB_BOOT_DEFAULT_SEL_PAD	0
#define ARTO_USB_BOOT_BIT		3

#define ARTO_UART_BOOT_REG		4
#define ARTO_UART_BOOT_SEL_PAD		1
#define ARTO_UART_BOOT_DEFAULT_SEL_PAD	0
#define ARTO_UART_BOOT_BIT		24

#define ARTO_SPIM1_BOOT_REG		19
#define ARTO_SPIM1_BOOT_SEL_PAD		3
#define ARTO_SPIM1_BOOT_BIT		0

#define PIN_FUN(x)			(x & 0x7)

/*SIM 1.8V OD 3.3V IO*/
#define PIN_SIM_3V3_DS0		(1 << 0x3)
#define PIN_SIM_3V3_DS1		(1 << 0x4)
#define PIN_SIM_3V3_PD		(1 << 0x5)
#define PIN_SIM_3V3_PU		(1 << 0x6)
#define PIN_SIM_3V3_ST		(1 << 0x7)
#define PIN_SIM_3V3_SL		(1 << 0x8)

/*SD 1.8V OD 3.3V IO*/
#define PIN_3V3_DS1			(1 << 0x3)
#define PIN_3V3_DS2			(1 << 0x4)
#define PIN_3V3_PD			(1 << 0x5)
#define PIN_3V3_PU			(1 << 0x6)
#define PIN_3V3_ST			(1 << 0x7)
#define PIN_3V3_SL			(1 << 0x8)

/*1.8V IO*/
#define PIN_1V8_DS0			(1 << 0x3)
#define PIN_1V8_DS1			(1 << 0x4)
#define PIN_1V8_PS			(1 << 0x5)
#define PIN_1V8_PE			(1 << 0x6)
#define PIN_1V8_ST1			(1 << 0x7)
#define PIN_1V8_SL			(1 << 0x8)
#define PIN_1V8_HE			(1 << 0x9)
#endif

/**********AHB burst define************************/
#define ARTOSYN_GMAC_BASE		0x08040000
#define ARTOSYN_GMAC_AHB_CFG		(ARTOSYN_GMAC_BASE + 0x1000)
#define ARTOSYN_GMAC_AHB_TX_CFG		(ARTOSYN_GMAC_BASE + 0x100C)
#define ARTOSYN_GMAC_AHB_RX_CFG		(ARTOSYN_GMAC_BASE + 0x1010)

#define ARTOSYN_GMAC_MAC_0   (ARTOSYN_GMAC_BASE + 0x40)
#define ARTOSYN_GMAC_MAC_1   (ARTOSYN_GMAC_BASE + 0x44)

#endif /* _SIRIUS_CPU_H */
