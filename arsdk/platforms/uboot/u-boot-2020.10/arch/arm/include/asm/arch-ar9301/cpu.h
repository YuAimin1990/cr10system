/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _AR9301_CPU_H
#define _AR9301_CPU_H

// power-on pull up register
#define RTC_RF_BASE			(0x01076040)
#define RTC_CON1			(0x04)
#define SYS_POWER_ON_CTRL		(RTC_RF_BASE + RTC_CON1)
#define GLOBAL_TIMER_GATE_CTL		(0x02106000)

//base addr
#define TZC_4F_BASE		0x01058000	//4-filter
#define TZC_2F_BASE		0x64539000	//2-filter
#define TZC_GLB_REG_BASE	0x6453A000	//tzc400 global register

//register offset
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


/*FIXME: These addresses need to be fixed according to Artosyn register map*/

#define ARTOSYN_CCI_BASE    0x0d090000
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

#define ARTOSYN_QSPI_FLASH_BASE   0x10000000

#define GIC_BASE			(0x01000000)

#define EFUSE_BASE_ADDRESS		(0x01060000)

#define WARMBOOT_FLAG_ADDR		(0)

//#define ARTOSYN_GIC_BASE    0x01000000
//#define ARTOSYN_TIMER_BASE  0x40000000 /*checked*/
//#define ARTOSYN_SPI_BASE    0x40100000
//#define ARTOSYN_I2C_BASE    0x40200000
//#define ARTOSYN_CAN_BASE    0x40300000
//#define ARTOSYN_GPIO_GROUP0_BASE    0x08400000
//#define ARTOSYN_GPIO_GROUP1_BASE    0x08420000
//#define ARTOSYN_GPIO_GROUP2_BASE    0x08440000
//#define ARTOSYN_GPIO_GROUP3_BASE    0x08460000

//#define ARTOSYN_UART0_BASE  0x08500000 /*checked*/
//#define ARTOSYN_QSPI_CONTROLLER_BASE  0x0A000000 /*checked*/

//#define ARTOSYN_MMC_BASE    0x64500000
//#define ARTOSYN_SD_BASE     0x09000000

//#define ARTOSYN_DRAM_CTL_BASE 0x64560000
//#define ARTOSYN_DRAM_PHY_BASE 0x64568000
//#define ARTOSYN_DRAM_REG_BASE 0x6456C000

//#define ARTOSYN_DRAM_BASE     0x80000000

//#define ARTOSYN_PIN_SHARE_CFG 0x60633000

//#define ARTOSYN_CPUCFG_BASE 0x60610000

/*This reg saves the entry point of secondary cores out of WFI*/
//#define ARTOSYN_SECONDARY_CORE_ENTRY   0x6061002C
//#define ARTOSYN_BOOT_MODE_REG          0x6061003C

/**********AHB burst define************************/
#define ARTOSYN_GMAC_BASE		0x01130000
#define ARTOSYN_GMAC_AHB_CFG		(ARTOSYN_GMAC_BASE+0x1000)
#define ARTOSYN_GMAC_AHB_TX_CFG		(ARTOSYN_GMAC_BASE+0x100C)
#define ARTOSYN_GMAC_AHB_RX_CFG		(ARTOSYN_GMAC_BASE+0x1010)

#define ARTOSYN_GMAC_MAC_0   (ARTOSYN_GMAC_BASE + 0x40)
#define ARTOSYN_GMAC_MAC_1   (ARTOSYN_GMAC_BASE + 0x44)

//#define ARTOSYN_MII_BASE    0xffffffff

//#define ARTOSYN_PRCM_BASE   0xffffffff

//#define ARTOSYN_R_PIO_BASE  0xffffffff

//#define ARTOSYN_SS_BOND_ID_SIRIUS 5

#endif /* _SIRIUS_CPU_H */
