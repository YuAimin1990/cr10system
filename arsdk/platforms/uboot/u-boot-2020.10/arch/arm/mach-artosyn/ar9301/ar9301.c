#include <common.h>
#include <fdt_support.h>
#include <init.h>
#include <log.h>
#include <spl.h>
#include <spl_gpio.h>
#include <syscon.h>
#include <image.h>

#include <asm/cache.h>
#include <linux/bitops.h>
#include <asm/arch/boot.h>

#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/spl.h>

#include <asm/arch/boot.h>
#include <asm/arch/cpu.h>
#include <asm/arch/image.h>
#include <configs/artosyn-common.h>

#include <linux/delay.h>
#include <dm/uclass.h>

DECLARE_GLOBAL_DATA_PTR;

static struct mm_region ar9301_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0x10000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		.virt = 0x20000000UL,
		.phys = 0x20000000UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = ar9301_mem_map;

#ifdef CONFIG_DEBUG_UART_BOARD_INIT
void board_debug_uart_init(void)
{
}
#endif

unsigned int read_reg32(unsigned long addr)
{
	volatile unsigned int *reg_addr = (unsigned int *)addr;
	return (*reg_addr);
}
void write_reg32(unsigned long addr, unsigned int data)
{
	*(unsigned int *)addr = data;
}

int secure_boot_enabled(void)
{
	//save the secure mode to register so u-boot/kernel/.. can get it from normal world.
	if(((readl(POLESTAR_EFUSE_SECURE_CONFIG) & BIT_32(SECURE_BOOT_SHIFT)) != 0)) {
		writel(readl(BOOT_DEVICE_STATUS_BASE_REG) | SEC_BOOT_EN, BOOT_DEVICE_STATUS_BASE_REG);
		return 1;
	}

	return 0;
}

void set_slave_cpu_info(unsigned long sprs_el3, unsigned long scr_el3, unsigned long ep)
{
	volatile unsigned long * uboot_cpu_release_addr = (unsigned long)BOOT_INFO_STRICT_BASE_ADDR;
	writel(sprs_el3, BOOT_SPSR_EL3_ADDR);
	writel(scr_el3, BOOT_SCR_EL3_ADDR);
	uboot_cpu_release_addr[0] = ep;//cpu1
	uboot_cpu_release_addr[1] = ep;//cpu2
	uboot_cpu_release_addr[2] = ep;//cpu3
	flush_dcache_all();
}

void release_secondary_cpu(unsigned long cpuid, unsigned long spsr_el3, unsigned long scr_el3, unsigned long entry_point)
{
	volatile unsigned long * uboot_cpu_release_addr = (unsigned long)BOOT_INFO_STRICT_BASE_ADDR;
	writel(spsr_el3, BOOT_SPSR_EL3_ADDR);
	writel(scr_el3, BOOT_SCR_EL3_ADDR);
	uboot_cpu_release_addr[cpuid-1] = entry_point;
	asm volatile("dsb sy" : : : "memory");
	asm volatile("sev" : : : "memory");
}

/* get boot device from boot config pin */
int get_boot_device(void)
{
	int val = readl(POLESTAR_BOOT_MODE_VALUE_REG) & 0x07;

	return val;
}

u32 spl_boot_device(void)
{
	int boot_mode, boot_device;

	boot_mode = get_boot_device();

	switch (boot_mode) {
	case BOOT_VALUE_ROM_QSPI_NOR:
		boot_device = BOOT_DEVICE_SPI;
		break;
	case BOOT_VALUE_ROM_QSPI_NAND:
		boot_device = BOOT_DEVICE_SPINAND;
		break;
	case BOOT_VALUE_ROM_EMMC:
		boot_device = BOOT_DEVICE_MMC1;
		break;
	default:
		printf("Boot mode is not supported now!\n");
		break;
	}

	writel(readl(BOOT_DEVICE_STATUS_BASE_REG) | SPL_RUN_DONE, BOOT_DEVICE_STATUS_BASE_REG);

	return boot_device;
}

struct ar_soc_info{
	unsigned char 	revision_id;
	char			soc_suffix;
	unsigned short	soc_product_id;
	unsigned char	pd_year;
	unsigned char	pd_week;
	unsigned char	feb_id:4;
	unsigned char	jedec_id_bank:4;
	unsigned char	jedec_id_code;
	char			id_info[8];
	unsigned int	sn;
};

#define		AR_SOC_INFO_OFFSET			(0x50 / 0x8)
#define		AR_EFUSE_MAX_SIZE			(512)

int get_soc_info(void)
{
	unsigned char buffer[AR_EFUSE_MAX_SIZE];
	unsigned char new_cmdline[AR_EFUSE_MAX_SIZE];
	struct ar_soc_info * info = NULL;
	int ret = -1;
	char * cmdline = NULL;

	ret = efuse_read((unsigned int *)buffer, AR_EFUSE_MAX_SIZE / 0x4, 0);
	if(ret != 0)
	{
		printf("efuse_read failed!\n");
		return ret;
	}
	info = (struct ar_soc_info *)(buffer + AR_SOC_INFO_OFFSET);
	//printf("Revision_id:        %d\n", info->revision_id);
	//printf("Soc_product_id:     ar%x%c\n", info->soc_product_id, info->soc_suffix ?: ' ');
	//printf("Production_date:    20%x%x\n", info->pd_year, info->pd_week);
	//printf("Feb_id:	            %d\n", info->feb_id);
	//printf("Jedec_id:           %x%x\n", info->jedec_id_bank, info->jedec_id_code);
	//printf("ID_info:            %c%c%c%c%c%c%c%c\n", info->id_info[0], info->id_info[1],
	//												info->id_info[2], info->id_info[3],
	//												info->id_info[4], info->id_info[5],
	//												info->id_info[6], info->id_info[7]);
	//printf("SN:                 %d\n", info->sn);

	cmdline = env_get("bootargs");
	if(NULL == cmdline)
	{
		printf("env_get failed!\n");
		return -1;
	}

	sprintf(new_cmdline,
		"%s revision_id=%d soc_product_id=ar%x%c pd=20%x%x feb_id=%d jedec_id=%x%x id_info=%c%c%c%c%c%c%c%c sn=%d",
		cmdline,
		info->revision_id, info->soc_product_id, info->soc_suffix ?: ' ', info->pd_year, info->pd_week,
		info->feb_id, info->jedec_id_bank, info->jedec_id_code,
		info->id_info[0], info->id_info[1],
		info->id_info[2], info->id_info[3],
		info->id_info[4], info->id_info[5],
		info->id_info[6], info->id_info[7],
		info->sn);
	//printf(new_cmdline);

	ret = env_set("bootargs", new_cmdline);
	if(ret != 0)
	{
		printf("env_set failed!\n");
		return ret;
	}

	return ret;
}

/* 
 * when cpu will goto nonsec world, must set ip to nonsec access
 * when bl31 is available, there is must be set in bl31
 */
void ar_security_setup(void)
{
	/* set ip to nonsec */
	writel(0xff, 0x0105006c);
	writel(0x3e3ff, 0x01050074);
}

#ifdef CONFIG_MISC_INIT_R
__weak int misc_init_r(void)
{
	return get_soc_info();
}
#endif

#ifdef CONFIG_SPL_BUILD

static int ar_cci_init(void)
{
	unsigned int val = 0;

	//read the PMCR, the reset value should be 0x2000
	val = readl(ARTOSYN_CCI_BASE + CCI_PMCR);
	if (val != 0x2000) {
		printf("CCI PMCR ERROR");
		return -1;
	}

	//Enable non-sec access to CCI
	val = readl(ARTOSYN_CCI_BASE + CCI_SECURE_ACCESS_REG);
	val |= 1;
	writel(val, ARTOSYN_CCI_BASE + CCI_SECURE_ACCESS_REG);

	//S0 for NOC main:(ceva, dla,,)
	val = readl(ARTOSYN_CCI_BASE + CCI_S0_SNOOP_CTRL_REG);
	val = 0x01;//val |= 0x1;
	writel(val, ARTOSYN_CCI_BASE + CCI_S0_SNOOP_CTRL_REG);

	//S1 for noc-vision
	val = readl(ARTOSYN_CCI_BASE + CCI_S1_SNOOP_CTRL_REG);
	val = 0x01;//val |= 0x1;
	writel(val, ARTOSYN_CCI_BASE + CCI_S1_SNOOP_CTRL_REG);

#if 0
	//S2 for Video-SMMU-TCU
	val = readl(ARTOSYN_CCI_BASE + CCI_S2_SNOOP_CTRL_REG);
	val |= 0x3;
	writel(val, ARTOSYN_CCI_BASE + CCI_S2_SNOOP_CTRL_REG);
#endif

	//S4 for A53
	val = readl(ARTOSYN_CCI_BASE + CCI_S4_SNOOP_CTRL_REG);
	val = 0x01;//val |= 0x1;
	writel(val, ARTOSYN_CCI_BASE + CCI_S4_SNOOP_CTRL_REG);

	//AR_LOG_INFO(LOG_TAG_ID_SYS, "CCI init OK");
	return 0;
}

static void tzc400_init(unsigned int base_addr,	//Base address
		 unsigned int action,		//Control the interrupt and bus response signal hehavior
		 unsigned int gate_keeper,	//Control gate keeper closed or open
		 unsigned int spec_ctrl		//Spectulation access control
) {
	writel(action, base_addr + ACTION);
	writel(gate_keeper, base_addr + GATE_KEEPER);
	writel(spec_ctrl, base_addr + SPECULATION_CTRL);
}

/*
 * TZC400 Region Configure
 */
static void tzc400_rgn_config(unsigned int base_addr,
		       unsigned int region_num,
		       unsigned int region_base_low,
		       unsigned int region_base_high,
		       unsigned int region_top_low,
		       unsigned int region_top_high,
		       unsigned int region_attribute,
		       unsigned int region_id_access
) {
	writel(region_base_low, base_addr + RGN_BASE_LOW + 0x20 * region_num);
	writel(region_base_high, base_addr + RGN_BASE_HIGH + 0x20 * region_num);
	writel(region_top_low, base_addr + RGN_TOP_LOW + 0x20 * region_num);
	writel(region_top_high, base_addr + RGN_TOP_HIGH + 0x20 * region_num);
	writel(region_attribute, base_addr + RGN_ATTRIBUTE + 0x20 * region_num);
	writel(region_id_access, base_addr + RGN_ID_ACCESS + 0x20 * region_num);
}

static void arch_timer_init(void)
{
	/*0x01: timer enable; int mask: not mask;*/
	unsigned int timer_ctl = readl(GLOBAL_TIMER_GATE_CTL);

	writel(timer_ctl | 0x1, GLOBAL_TIMER_GATE_CTL);
}

/*
 * init ddr for veloce
 */
#ifdef VELOCE_DDR

#define DDR_PHY_BASE			0x04000000
#define DDR_CTR_BASE			0x06000000
#define DDR_REG_BASE			0x06010000
#define DDR_MON_BASE			0x06020000
#define AXI_MON0_BASE			0x06030000
#define AXI_MON1_BASE			0x06040000
#define AXI_MON2_BASE			0x06050000

#define DDR_CON0			0x000
#define DDR_CON1			0x004
#define DDR_CON12			0x030
#define DDR_CON13			0x034
#define DDR_CON14			0x038
#define DDR_CON15			0x03c
#define DDR_CON16			0x040
#define DDR_CON17			0x044
#define DDR_CON22			0x058
#define DDR_CON23			0x05c
#define DDR_CON27			0x06c
#define DDR_CON76			0x130
#define DDR_MRR_VALID			0x134
#define DDR_MRR_DATA0			0x138
#define DDR_MRR_DATA1			0x13c
#define DDR_MRR_DATA2			0x140
#define DDR_MRR_DATA3			0x144
#define DDR_MRR_DATA4			0x148
#define DDR_MRR_DATA5			0x14c
#define DDR_MRR_DATA6			0x150
#define DDR_MRR_DATA7			0x154

#define DDR_MON_CTRL			0x000
#define DDR_MON_PERIOD			0x004
#define DDR_MON_STAT			0x008
#define DDR_MON_FIFO_STAT		0x00c
#define DDR_MON_INT_EN			0x010
#define DDR_MON_INT_STAT		0x014
#define DDR_MON_CREDIT_STAT		0x018
#define DDR_MON_HIF_RW_CNT		0x100
#define DDR_MON_HIF_RD_CNT		0x104
#define DDR_MON_HIF_WR_CNT		0x108
#define DDR_MON_HIF_RMW_CNT		0x10c
#define DDR_MON_HIF_HPR_CNT		0x110
#define DDR_MON_RD_BYP_CNT		0x114
#define DDR_MON_ACT_BYP_CNT		0x118
#define DDR_MON_DFI_WR_DATA_CNT		0x11c
#define DDR_MON_DFI_RD_DATA_CNT		0x120
#define DDR_MON_LPR_XACT_CRIT_CNT	0x124
#define DDR_MON_HPR_XACT_CRIT_CNT	0x128
#define DDR_MON_WR_XACT_CRIT_CNT	0x12c
#define DDR_MON_CAM_RW_CNT		0x130
#define DDR_MON_CAM_RD_CNT		0x134
#define DDR_MON_CAM_WR_CNT		0x138
#define DDR_MON_CAM_ACT_CNT		0x13c
#define DDR_MON_CAM_RD_ACT_CNT		0x140
#define DDR_MON_CAM_PRE_CNT		0x144
#define DDR_MON_PRE_RW_CNT		0x148
#define DDR_MON_PRE_OTHER_CNT		0x14c
#define DDR_MON_RW_TRANSITION_CNT	0x150
#define DDR_MON_WR_COMBINE_CNT		0x154
#define DDR_MON_WAR_HAZARD_CNT		0x158
#define DDR_MON_RAW_HAZARD_CNT		0x15c
#define DDR_MON_SREF_CNT		0x160
#define DDR_MON_PD_CNT			0x164
#define DDR_MON_DPD_CNT			0x168
#define DDR_MON_MPSM_CNT		0x16c
#define DDR_MON_REF_CNT			0x170
#define DDR_MON_CRIT_REF_CNT		0x174
#define DDR_MON_SPEC_REF_CNT		0x178
#define DDR_MON_MR_CNT			0x17c
#define DDR_MON_ZQCL_CNT		0x180
#define DDR_MON_ZQCS_CNT		0x184
#define DDR_MON_LPR_NOCREDIT_CNT	0x188
#define DDR_MON_HPR_NOCREDIT_CNT	0x18c
#define DDR_MON_FIFO_ENTRY		0x200

#define AXI_MON_CTRL			0x00
#define AXI_MON_PERIOD			0x04
#define AXI_MON_STAT			0x08
#define AXI_MON_CUR_CNT			0x0c
#define AXI_MON_INT_EN			0x10
#define AXI_MON_INT_STAT		0x14
#define AXI_MON_WR_CNT			0x40
#define AXI_MON_WRDATA_CNT		0x44
#define AXI_MON_WVALID_BYTE_CNT_L	0x48
#define AXI_MON_WVALID_BYTE_CNT_H	0x4c
#define AXI_MON_RD_CNT			0x50
#define AXI_MON_RDDATA_CNT		0x54

#define CASE_PASS_ADDR			0x5ffffff0
#define CASE_FAIL_ADDR			0x5ffffff4

void ddr4_HBM_phy_init_maji(void) {

	//unsigned int transfer0;
	//===================================================================================
	//PHY INIT
	//===================================================================================
	writel(0x0020403b, DDR_PHY_BASE + 0x090 ); // (         PUB_DSGCR) 'h0020403b PUB  0x024
	writel(0x0f000009, DDR_PHY_BASE + 0x520 ); // (        PUB_IOVCR0) 'h0f000009 PUB  0x148
	writel(0x00000309, DDR_PHY_BASE + 0x524 ); // (        PUB_IOVCR1) 'h00000309 PUB  0x149
	writel(0x40000204, DDR_PHY_BASE + 0xb00 ); // (       PUB_DX4GCR0) 'h40000204 PUB  0x02c0  , disable DX4 ~ 7 for half bus mode
	writel(0x40000204, DDR_PHY_BASE + 0xc00 ); // (       PUB_DX5GCR0) 'h40000204 PUB  0x0300  , disable DX4 ~ 7 for half bus mode
	writel(0x40000204, DDR_PHY_BASE + 0xd00 ); // (       PUB_DX6GCR0) 'h40000204 PUB  0x0340  , disable DX4 ~ 7 for half bus mode
	writel(0x40000204, DDR_PHY_BASE + 0xe00 ); // (       PUB_DX7GCR0) 'h40000204 PUB  0x0380  , disable DX4 ~ 7 for half bus mode
	writel(0x0e00003c, DDR_PHY_BASE + 0x710 ); // (       PUB_DX0GCR4) 'h0e00003c PUB  0x1c4
	writel(0x0e00003c, DDR_PHY_BASE + 0x810 ); // (       PUB_DX1GCR4) 'h0e00003c PUB  0x204
	writel(0x0e00003c, DDR_PHY_BASE + 0x910 ); // (       PUB_DX2GCR4) 'h0e00003c PUB  0x244
	writel(0x0e00003c, DDR_PHY_BASE + 0xa10 ); // (       PUB_DX3GCR4) 'h0e00003c PUB  0x284
	writel(0x0e00003c, DDR_PHY_BASE + 0xb10 ); // (       PUB_DX4GCR4) 'h0e00003c PUB  0x2c4
	writel(0x0e00003c, DDR_PHY_BASE + 0xc10 ); // (       PUB_DX5GCR4) 'h0e00003c PUB  0x304
	writel(0x0e00003c, DDR_PHY_BASE + 0xd10 ); // (       PUB_DX6GCR4) 'h0e00003c PUB  0x344
	writel(0x0e00003c, DDR_PHY_BASE + 0xe10 ); // (       PUB_DX7GCR4) 'h0e00003c PUB  0x384
	writel(0x0fc00172, DDR_PHY_BASE + 0x52c ); // (         PUB_VTCR1) 'h0fc00172 PUB  0x14b
	writel(0x2000b087, DDR_PHY_BASE + 0x0200); // (         PUB_DTCR0) 'h2000b087 PUB  0x0080
	writel(0x0fc00172, DDR_PHY_BASE + 0x052c); // (         PUB_VTCR1) 'h0fc00172 PUB  0x014b
	writel(0x0000040c, DDR_PHY_BASE + 0x100 ); // (           PUB_DCR) 'h0000040c PUB  0x040
	writel(0x20c01884, DDR_PHY_BASE + 0x088 ); // (         PUB_DXCCR) 'h20c01884 PUB  0x022
	writel(0x5e001810, DDR_PHY_BASE + 0x040 ); // (          PUB_PTR0) 'h5e001810 PUB  0x010
	writel(0x008005f0, DDR_PHY_BASE + 0x044 ); // (          PUB_PTR1) 'h008005f0 PUB  0x011
	writel(0x00000a31, DDR_PHY_BASE + 0x180 ); // (           PUB_MR0) 'h00000a31 PUB  0x060
	writel(0x00000001, DDR_PHY_BASE + 0x184 ); // (           PUB_MR1) 'h00000001 PUB  0x061
	writel(0x00000028, DDR_PHY_BASE + 0x188 ); // (           PUB_MR2) 'h00000028 PUB  0x062
	writel(0x00000200, DDR_PHY_BASE + 0x18c ); // (           PUB_MR3) 'h00000200 PUB  0x063
	writel(0x00000000, DDR_PHY_BASE + 0x190 ); // (           PUB_MR4) 'h00000200 PUB  0x064
	writel(0x00000640, DDR_PHY_BASE + 0x194 ); // (           PUB_MR5) 'h00000640 PUB  0x065
	writel(0x00000800, DDR_PHY_BASE + 0x198 ); // (           PUB_MR6) 'h00000800 PUB  0x066
	writel(0x0827100a, DDR_PHY_BASE + 0x110 ); // (         PUB_DTPR0) 'h082f100a PUB  0x044
	writel(0x28250408, DDR_PHY_BASE + 0x114 ); // (         PUB_DTPR1) 'h28250408 PUB  0x045
	writel(0x00070380, DDR_PHY_BASE + 0x118 ); // (         PUB_DTPR2) 'h00070380 PUB  0x046
	writel(0x02000101, DDR_PHY_BASE + 0x11c ); // (         PUB_DTPR3) 'h02000101 PUB  0x047
	writel(0x00c10808, DDR_PHY_BASE + 0x120 ); // (         PUB_DTPR4) 'h00c10808 PUB  0x048
	writel(0x0036100a, DDR_PHY_BASE + 0x124 ); // (         PUB_DTPR5) 'h00111009 PUB  0x049
	writel(0x020046a0, DDR_PHY_BASE + 0x014 ); // (         PUB_PGCR1) 'h020046a0 PUB  0x005
	writel(0x00004797, DDR_PHY_BASE + 0x018 ); // (         PUB_PGCR2) 'h00002493 PUB  0x006
	writel(0xc8aa0060, DDR_PHY_BASE + 0x01c ); // (         PUB_PGCR3) 'hc8aa0060 PUB  0x007
	writel(0x80040000, DDR_PHY_BASE + 0x002c); // (         PUB_PGCR7) 'h80040000 PUB  0x000b
	writel(0x00000073, DDR_PHY_BASE + 0x004 ); // (           PUB_PIR) 'h00000073 PUB  0x001

	writel(0x00040001, DDR_PHY_BASE + 0x0004); // (           PUB_PIR) 'h00040001 PUB  0x0001
	writel(0x04058f00, DDR_PHY_BASE + 0x0680); // (          PUB_ZQCR) 'h04058f00 PUB  0x01a0
	writel(0x00000000, DDR_PHY_BASE + 0x4dc ); // (       PUB_RANKIDR) 'h00000000 PUB  0x137
	writel(0x000000a6, DDR_PHY_BASE + 0x788 ); // (     PUB_DX0LCDLR2) 'h0000016a PUB  0x1e2
	writel(0x000000a6, DDR_PHY_BASE + 0x888 ); // (     PUB_DX1LCDLR2) 'h0000016a PUB  0x222
	writel(0x000000a6, DDR_PHY_BASE + 0x988 ); // (     PUB_DX2LCDLR2) 'h0000016a PUB  0x262
	writel(0x000000a6, DDR_PHY_BASE + 0xa88 ); // (     PUB_DX3LCDLR2) 'h000000a6 PUB  0x2a2
	writel(0x000000a6, DDR_PHY_BASE + 0xb88 ); // (     PUB_DX4LCDLR2) 'h000000a6 PUB  0x2e2
	writel(0x000000a6, DDR_PHY_BASE + 0xc88 ); // (     PUB_DX5LCDLR2) 'h000000a6 PUB  0x322
	writel(0x000000a6, DDR_PHY_BASE + 0xd88 ); // (     PUB_DX6LCDLR2) 'h000000a6 PUB  0x362
	writel(0x000000a6, DDR_PHY_BASE + 0xe88 ); // (     PUB_DX7LCDLR2) 'h000000a6 PUB  0x3a2

	writel(0x00010001, DDR_PHY_BASE + 0x04dc); // (       PUB_RANKIDR) 'h00010001 PUB  0x0137
	writel(0x000000a6, DDR_PHY_BASE + 0x0788); // (     PUB_DX0LCDLR2) 'h000000a6 PUB  0x01e2
	writel(0x000000a6, DDR_PHY_BASE + 0x0888); // (     PUB_DX1LCDLR2) 'h000000a6 PUB  0x0222
	writel(0x000000a6, DDR_PHY_BASE + 0x0988); // (     PUB_DX2LCDLR2) 'h000000a6 PUB  0x0262
	writel(0x000000a6, DDR_PHY_BASE + 0x0a88); // (     PUB_DX3LCDLR2) 'h000000a6 PUB  0x02a2
	writel(0x000000a6, DDR_PHY_BASE + 0x0b88); // (     PUB_DX4LCDLR2) 'h000000a6 PUB  0x02e2
	writel(0x000000a6, DDR_PHY_BASE + 0x0c88); // (     PUB_DX5LCDLR2) 'h000000a6 PUB  0x0322
	writel(0x000000a6, DDR_PHY_BASE + 0x0d88); // (     PUB_DX6LCDLR2) 'h000000a6 PUB  0x0362
	writel(0x000000a6, DDR_PHY_BASE + 0x0e88); // (     PUB_DX7LCDLR2) 'h000000a6 PUB  0x03a2
}

void ddr4_init(void)
{
	unsigned int rdata_t ;

	unsigned int tCK_freq = 533 ;
	unsigned int tCL      = 15  ;
	unsigned int tRCD     = 15  ;
	unsigned int tRP      = 15  ;
	unsigned int tCWL     = 11  ;
	unsigned int tRC      = 48  ;//ns
	unsigned int tPL      = 0   ;
	unsigned int tAL      = 0   ;

	unsigned int dfi_t_ctrl_delay= 3  ;
	unsigned int dfi_t_rddata_en = 10 ;
	unsigned int dfi_tphy_wrdata = 2  ;
	unsigned int dfi_tphy_wrlat  = 6  ;

	unsigned int t_rfc_nom_x1_x32= (78*tCK_freq)/320 ;
	unsigned int t_rfc_min       = (16*tCK_freq)/100+1;

	unsigned int t_wl            = (tAL+tCWL+1)/2 ;
	unsigned int t_rl            = (tAL+tCL+1)/2 ;
	unsigned int t_wr            = (15*tCK_freq)/1000+1;
	unsigned int t_rtp           = t_wr/2;
	unsigned int t_wtr           = t_rtp;
	unsigned int t_wtr_s         = (25*tCK_freq)/10000+1;
	unsigned int t_wr2pre        = t_wl + 2 + t_wr ;
	unsigned int t_faw           = (30*tCK_freq)/1000+1;
	unsigned int t_ras_min       = (39*tCK_freq)/1000+1;
	unsigned int t_ras_max       = ((39*9*tCK_freq)/2560 -1)/2; //9xtREFI
	unsigned int t_xp            = (6*tCK_freq)/1000+1;
	unsigned int t_rd2pre        = t_rtp ;
	unsigned int t_rc            = (tRC*tCK_freq)/1000+1;
	unsigned int t_rd2wr         = t_rl + 3 - t_wl ;
	unsigned int t_wr2rd         = (tCWL+tPL+5)/2 + t_wtr ;
	unsigned int t_wr2rd_s       = (tCWL+tPL+5)/2 + t_wtr_s ;
	unsigned int t_mrw           = 0  ; //for LP2,LP3,LP4
	unsigned int t_mrd           = 4  ;
	unsigned int t_mod           = 12 ;
	unsigned int t_rcd           = (tRCD-tAL+1)/2;
	unsigned int t_ccd_l         = 3  ;
	unsigned int t_ccd_s         = 2  ;
	unsigned int t_rrd_l         = (64*tCK_freq)/10000+1;
	unsigned int t_rrd_s         = (53*tCK_freq)/10000+1;
	unsigned int t_rp            = tRP/2+1 ;
	unsigned int t_cksrx         = tCK_freq/100+1;
	unsigned int t_cksre         = tCK_freq/100+1+(tPL+1)/2;
	unsigned int t_cke           = (5*tCK_freq)/1000+1;
	unsigned int t_ckesr         = t_cke+1+(tPL+1)/2;
	unsigned int t_ckpde         = t_cksre ;
	unsigned int t_ckpdx         = t_cksrx ;
	unsigned int t_xs_fast_x32   = ((12*tCK_freq)/100+1)/32 + 1 ;
	unsigned int t_xs_abort_x32  = t_xs_fast_x32 ;
	unsigned int t_xs_x32        = ((17*tCK_freq)/100+1)/32 + 1 ;
	unsigned int t_xs_dll_x32    = 768/64   ;
	unsigned int t_post_mpsm_x32 = t_xs_x32 + t_xs_dll_x32 ;
	unsigned int t_mpx_lh        = 12 ;
	unsigned int t_mpx_s         = 1  ;
	unsigned int t_ckmpe         = 14 ;
	unsigned int t_cmdcke        = 0  ; //for LP4
	unsigned int t_mrd_pda       = 8  ;


	unsigned int MR0_CL                 ;
	unsigned int MR0_WR_RTP             ; //WR=15ns, RTP=7.5ns
	unsigned int MR0_RBT         = 0    ; //sequence
	unsigned int MR0_DLL_RESET   = 0    ;
	unsigned int MR0_TM          = 0    ;

	unsigned int MR1_DLL_ENABLE = 1     ;
	unsigned int MR1_ODIC       = 0     ;//RZQ/7
	unsigned int MR1_AL         = 0     ;
	unsigned int MR1_WRLEV      = 0     ;
	unsigned int MR1_RTT_NOM    = 0     ;//Disable
	unsigned int MR1_TDQS       = 0     ;
	unsigned int MR1_QOFF       = 0     ;//Output buffer enable

	unsigned int MR2_CWL                ;
	unsigned int MR2_LP_ASR     = 0     ;// Manual(Normal)
	unsigned int MR2_RTT_WR     = 0     ;// Dynamic ODT Off
	unsigned int MR2_WR_CRC     = 0     ;

	unsigned int MR3_RFSH_MODE  = 2     ;//Fix 4X
	unsigned int MR3_WR_CMD_LAT = 1     ;//5nCK

	unsigned int MR4_MPD        = 0     ;

	unsigned int MR5_PL         = 0     ; //Disable, <=533: 3'h1 ; 600: 3'h2
	unsigned int MR5_WR_DBI     = 0     ;
	unsigned int MR5_RD_DBI     = 0     ;
	unsigned int MR5_DM         = 1     ;
	unsigned int MR5_RTT_PARK   = 0     ; //Disable
	unsigned int MR5_PAR_ERR    = 0     ;

	if (MR0_RBT) {
		switch (tCL) {
		case  9 : MR0_CL = 0x2 ; break ;//b00010 ;
		case 10 : MR0_CL = 0x3 ; break ;//b00011 ;
		case 11 : MR0_CL = 0x6 ; break ;//b00110 ;
		case 12 : MR0_CL = 0x7 ; break ;//b00111 ;
		case 13 : MR0_CL = 0xa ; break ;//b01010 ;
		case 14 : MR0_CL = 0xb ; break ;//b01011 ;
		case 15 : MR0_CL = 0xe ; break ;//b01110 ;
		case 16 : MR0_CL = 0xf ; break ;//b01111 ;
		case 18 : MR0_CL = 0x12; break ;//b10010 ;
		case 20 : MR0_CL = 0x13; break ;//b10011 ;
		case 22 : MR0_CL = 0x16; break ;//b10110 ;
		case 24 : MR0_CL = 0x17; break ;//b10111 ;
		default : MR0_CL = 0x1f;//b11111 ;
		}
	} else { // MR0_RBT=0
		switch (tCL) {
		case  9 : MR0_CL = 0x0 ; break ;//b00000 ;
		case 10 : MR0_CL = 0x1 ; break ;//b00001 ;
		case 11 : MR0_CL = 0x4 ; break ;//b00100 ;
		case 12 : MR0_CL = 0x5 ; break ;//b00101 ;
		case 13 : MR0_CL = 0x8 ; break ;//b01000 ;
		case 14 : MR0_CL = 0x9 ; break ;//b01001 ;
		case 15 : MR0_CL = 0xc ; break ;//b01100 ;
		case 16 : MR0_CL = 0xd ; break ;//b01101 ;
		case 18 : MR0_CL = 0x10; break ;//b10000 ;
		case 20 : MR0_CL = 0x11; break ;//b10001 ;
		case 22 : MR0_CL = 0x14; break ;//b10100 ;
		case 24 : MR0_CL = 0x15; break ;//b10101 ;
		default : MR0_CL = 0x1e;//b11101 ;
		}
	}

	switch (tCK_freq) {
		case 333 : MR0_WR_RTP = 0x0 ; break ;//b000 ; //10-5
		case 400 : MR0_WR_RTP = 0x1 ; break ;//b001 ; //12-6
		case 466 : MR0_WR_RTP = 0x2 ; break ;//b010 ; //14-7
		case 533 : MR0_WR_RTP = 0x3 ; break ;//b011 ; //16-8
		//case 600 : MR0_WR_RTP = 0x4 ; break ;//b100 ; //18-9
		case 600 : MR0_WR_RTP = 0x5 ; break ;//b100 ; //18-9
		case 666 : MR0_WR_RTP = 0x5 ; break ;//b101 ; //20-10
		case 800 : MR0_WR_RTP = 0x6 ; break ;//b110 ; //24-12
		default  : MR0_WR_RTP = 0x7 ;//b111 ;
	}

	switch (tCWL) {
		case  9 : MR2_CWL = 0x0 ; break ;//b000 ;
		case 10 : MR2_CWL = 0x1 ; break ;//b001 ;
		case 11 : MR2_CWL = 0x2 ; break ;//b010 ;
		case 12 : MR2_CWL = 0x3 ; break ;//b011 ;
		case 14 : MR2_CWL = 0x4 ; break ;//b100 ;
		case 16 : MR2_CWL = 0x5 ; break ;//b101 ;
		case 18 : MR2_CWL = 0x6 ; break ;//b110 ;
		default : MR2_CWL = 0x7 ;//b111 ;
	}

	unsigned int MR0_REG = MR0_WR_RTP << 9 | MR0_DLL_RESET << 8 |
				MR0_TM<<7 | MR0_CL<<2 | 1;

	unsigned int MR1_REG = MR1_QOFF << 12 | MR1_TDQS << 11 |
				MR1_RTT_NOM << 8 | MR1_WRLEV << 7 |
				MR1_AL << 3 | MR1_ODIC << 1 |
				MR1_DLL_ENABLE;

	unsigned int MR2_REG = MR2_WR_CRC << 12 |  MR2_RTT_WR << 9 |
				MR2_LP_ASR << 6 |  MR2_CWL << 3;

	unsigned int MR3_REG = MR3_WR_CMD_LAT << 9 | MR3_RFSH_MODE << 6;

	unsigned int MR4_REG = MR4_MPD << 1;

	unsigned int MR5_REG = MR5_RD_DBI << 12 | MR5_WR_DBI << 11 |
				MR5_DM << 10 | MR5_PAR_ERR << 9 |
				MR5_RTT_PARK << 6 | MR5_PL;

	unsigned int MR6_REG = 0x0800;

	writel(0x00000001, DDR_CTR_BASE + 0x0304); // (              DBG1) 'h00000001 uMCTL2 0x0304 //disable de-queque trans
	rdata_t = read_reg32(DDR_CTR_BASE + 0x0004);  // (              STAT) 'h00000000 uMCTL2 0x0004 // read stat = 0
	writel(0x81040210, DDR_CTR_BASE + 0x0000); // (              MSTR) 'h83040210 uMCTL2 0x0000 //x16,freq_mode=0,1 rank0, 1:2, burst8, full_DQ, burst_chop, ddr4
	writel(0x00000001, DDR_CTR_BASE + 0x001c); // (           MRCTRL2) 'h401f361f uMCTL2 0x001c //
	writel(0x00000000, DDR_CTR_BASE + 0x0020); // (          DERATEEN) 'h00000042 uMCTL2 0x0020 //derate_en=0
	writel(0x00024d01, DDR_CTR_BASE + 0x0034); // (            PWRTMG) 'h00408804 uMCTL2 0x0034 // selfref=0x40, dpd=0x88, pd=0x04
	writel(0x00000003, DDR_CTR_BASE + 0x0030); // (            PWRCTL) 'h00000003 uMCTL2 0x0030 // selfref_en, powerdown_en
	writel(0x002b0000, DDR_CTR_BASE + 0x0038); // (           HWLPCTL) 'h002b0000 uMCTL2 0x0038 // hw_lp_en=0
	writel(0x00906070, DDR_CTR_BASE + 0x0050); // (          RFSHCTL0) 'h00210000 uMCTL2 0x0050 // refresh_margin=2x32,refresh_to_x1_x32='h10, fefresh_burst=single, per bank refresh
	writel(0x00000020, DDR_CTR_BASE + 0x0060); // (          RFSHCTL3) 'h00000000 uMCTL2 0x0060 // fixed 4x, dis_auto_refresh=0
	writel(t_rfc_nom_x1_x32<<16 | t_rfc_min, DDR_CTR_BASE + 0x0064); // (           RFSHTMG) 'h00928061 uMCTL2 0x0064 // t_rfc_nom_x1_x32=7800/(1.876*32)=129=0x81,t_rfc_min=160/1.876+1=86=0x56

	writel(0x00000000, DDR_CTR_BASE + 0x00c0); // (        CRCPARCTL0) 'h00000000 uMCTL2 0x00c0
	writel(0x00001000, DDR_CTR_BASE + 0x00c4); // (        CRCPARCTL1) 'h00000000 uMCTL2 0x00c4 //dfi_t_phy_rdlat=16,crc_en,parity_en
	writel(0x00010001, DDR_CTR_BASE + 0x00d0); // (             INIT0) 'h00020002 uMCTL2 0x00d0 //post_cke_x1024=1(tXPR), pre_cke_x1024=1(500us)
	writel(0x00010001, DDR_CTR_BASE + 0x00d4); // (             INIT1) 'h00010002 uMCTL2 0x00d4 //dram_rstn_x1024=1(100ns),pre_ocd_x32=1
	writel(0x00000000, DDR_CTR_BASE + 0x00d8); // (             INIT2) 'h00004200 uMCTL2 0x00d8 //for lp2
	writel(MR0_REG<<16 | MR1_REG, DDR_CTR_BASE + 0x00dc);
	// (             INIT3) 'h0a350001 uMCTL2 0x00dc // M0:on-the-fly,CL-16,WR-20,RTP-10, M1:AL disable,ODI 34ohm,Rtt disable
	writel(MR2_REG<<16 | MR3_REG, DDR_CTR_BASE + 0x00e0); // (             INIT4) 'h00280200 uMCTL2 0x00e0 //MR2:CWL-16, Dynamic ODT off; MR3:5nCK
	writel(0x00110000, DDR_CTR_BASE + 0x00e4); // (             INIT5) 'h001103f4 uMCTL2 0x00e4 //dev_zqinit_x32='h10 -1024
	writel(MR4_REG<<16 | MR5_REG, DDR_CTR_BASE + 0x00e8); // (             INIT6) 'h00000640 uMCTL2 0x00e8 // MR4; MR5. read DBI disabled
	writel(MR6_REG, DDR_CTR_BASE + 0x00ec); // (             INIT7) 'h00000800 uMCTL2 0x00ec //tCCD_L min = 6
	writel(0x00000000, DDR_CTR_BASE + 0x00f0); // (           DIMMCTL) 'h00000000 uMCTL2 0x00f0
	writel(0x00000000, DDR_CTR_BASE + 0x00f4); // (           RANKCTL) 'h0000033f uMCTL2 0x00f4 //for multi-rank
	writel(t_wr2pre<<24 | t_faw<<16 | t_ras_max<<8 | t_ras_min, DDR_CTR_BASE + 0x0100); // (          DRAMTMG0) 'h14132813 uMCTL2 0x0100 // wr2pre='h14,t_faw='h13,t_ras_max='h28,t_ras_min='h13
	writel(t_xp<<16 | t_rd2pre<<8 | t_rc, DDR_CTR_BASE + 0x0104); // (          DRAMTMG1) 'h0004051b uMCTL2 0x0104 //t_xp='h4, rd2pre='h5, t_rc='h1b
	writel(t_wl<<24 | t_rl<<16 | t_rd2wr<<8 | t_wr2rd, DDR_CTR_BASE + 0x0108); // (          DRAMTMG2) 'h0808030f uMCTL2 0x0108 //write_latency-8,read_latency-8, rd2wr=3,wr2rd=f
	writel(t_mrw<<20 | t_mrd<<12 | t_mod, DDR_CTR_BASE + 0x010c); // (          DRAMTMG3) 'h0000400c uMCTL2 0x010c //t_mrw=0. t_mrd=4, t_mod=c
	writel(t_rcd<<24 | t_ccd_l<<16 | t_rrd_l<<8 | t_rp, DDR_CTR_BASE + 0x0110); // (          DRAMTMG4) 'h08030409 uMCTL2 0x0110 //t_rcd=8, t_ccd=3, t_rrd=4, t_rp=9
	writel(t_cksrx<<24 | t_cksre<<16 | t_ckesr<<8 | t_cke, DDR_CTR_BASE + 0x0114); // (          DRAMTMG5) 'h07070504 uMCTL2 0x0114 //t_cksrx=7,t_cksre=7, t_ckesr=5, t_cke=4
	writel(0x00000000, DDR_CTR_BASE + 0x0118); // (          DRAMTMG6) 'h00000000 uMCTL2 0x0118 //t_ckdpde=0,t_ckdpdx=0,t_ckcsx=0,
	writel(t_ckpde<<8 | t_ckpdx, DDR_CTR_BASE + 0x011c); // (          DRAMTMG7) 'h00000707 uMCTL2 0x011c //t_ckpde=7,t_ckpdx=7
	writel(t_xs_fast_x32<<24 | t_xs_abort_x32<<16 | t_xs_dll_x32<<8 | t_xs_x32, DDR_CTR_BASE + 0x0120); // (          DRAMTMG8) 'h03030e05 uMCTL2 0x0120 //t_xs_fast_x32=3,t_xs_abort_x32=3,t_xs_dll_x32=e,t_xs_x32=5
	writel(t_ccd_s<<16 | t_rrd_s<<8 | t_wr2rd_s, DDR_CTR_BASE + 0x0124); // (          DRAMTMG9) 'h0002040c uMCTL2 0x0124 //wr_preamble=1tCK, t_ccd_s=2, t_rrd_s=4, wr2rd_s=c
	//writel(t_sync_gear<<16 | t_cmd_gear<<8 | t_gear_setup<<2 | t_gear_hold, DDR_CTR_BASE + 0x0128); // (         DRAMTMG10) 'h00100c0a uMCTL2 0x0128 //t_sync_gear=10, t_cmd_gear=c, t_gear_setup=2, t_gear_hold=2
	writel(t_post_mpsm_x32<<24 | t_mpx_lh<<16 | t_mpx_s<<8 | t_ckmpe, DDR_CTR_BASE + 0x012c); // (         DRAMTMG11) 'h1109010e uMCTL2 0x012c //post_mpsm_gap_x32='h11, t_mpx_lh=9, t_mpx_s=1, t_ckmpe=e
	writel(t_cmdcke<<16 | t_mrd_pda, DDR_CTR_BASE + 0x0130); // (         DRAMTMG12) 'h0000000d uMCTL2 0x0130 //t_cmdcke=0, t_mrd_pda=d
	writel(0x00000000, DDR_CTR_BASE + 0x0138); // (         DRAMTMG14) 'h00000000 uMCTL2 0x0138 //for LP2/3/4
	writel(0x80000000, DDR_CTR_BASE + 0x013c); // (         DRAMTMG15) 'h80000000 uMCTL2 0x013c // en_dfi_lp_t_stab
	writel(0x41000040, DDR_CTR_BASE + 0x0180); // (            ZQCTL0) 'h41000040 uMCTL2 0x0180
	writel(0x0008887d, DDR_CTR_BASE + 0x0184); // (            ZQCTL1) 'h0008887d uMCTL2 0x0184
	writel(0x00000000, DDR_CTR_BASE + 0x0188); // (            ZQCTL2) 'h00000000 uMCTL2 0x0188
	writel(dfi_t_ctrl_delay<<24 | 1<<23 | dfi_t_rddata_en<<16 | 1<<15 | dfi_tphy_wrdata<<8 | dfi_tphy_wrlat, DDR_CTR_BASE + 0x0190);//(0x028b820e); // (           DFITMG0) 'h048b820e uMCTL2 0x0190
	writel(0x01070202, DDR_CTR_BASE + 0x0194); // (           DFITMG1) 'h01070202 uMCTL2 0x0194 //dfi_t_cmd_lat=0,dfi_t_parin_lat=1,dfi_t_wrdata_delay=7, dfi_t_dram_clk_disable=2, dfi_t_dram_clk_enable=2
	writel(0x07e0b011, DDR_CTR_BASE + 0x0198); // (         DFILPCFG0) 'h07e0b011 uMCTL2 0x0198
	writel(0x00000041, DDR_CTR_BASE + 0x019c); // (         DFILPCFG1) 'h00000041 uMCTL2 0x019c
	writel(0x80280005, DDR_CTR_BASE + 0x01a0); // (           DFIUPD0) 'h80400005 uMCTL2 0x01a0
	writel(0x009f007e, DDR_CTR_BASE + 0x01a4); // (           DFIUPD1) 'h009f007e uMCTL2 0x01a4
	writel(0x00000000, DDR_CTR_BASE + 0x01a8); // (           DFIUPD2) 'h00000000 uMCTL2 0x01a8
	writel(0x00000011, DDR_CTR_BASE + 0x01b0); // (           DFIMISC) 'h00000011 uMCTL2 0x01b0
	writel(0x00000202, DDR_CTR_BASE + 0x01b4); // (           DFITMG2) 'h00000000 uMCTL2 0x01b8 //dfi_tphy_rdcslat=0, dfi_tphy_wrcslat=0
	writel(0x00000001, DDR_CTR_BASE + 0x01c0); // (            DBICTL) 'h00000001 uMCTL2 0x01c0  // DM enabled
	writel(0x00000000, DDR_CTR_BASE + 0x01c4); // (        DFIPHYMSTR) 'h00000000 uMCTL2 0x01c4
	//************ No ECC ********************************
	// -RRR RRRR RRRR RRRR RBBC CCCC CCGC CC--  2G
	writel(0x001f1f1f, DDR_CTR_BASE + 0x0200); // (          ADDRMAP0) 'h00000012 uMCTL2 0x0200  // no cs
	writel(0x003f0909, DDR_CTR_BASE + 0x0204); // (          ADDRMAP1) 'h003f0606 uMCTL2 0x0204
	writel(0x01010100, DDR_CTR_BASE + 0x0208); // (          ADDRMAP2) 'h01010100 uMCTL2 0x0208
	writel(0x01010101, DDR_CTR_BASE + 0x020c); // (          ADDRMAP3) 'h12121201 uMCTL2 0x020c
	writel(0x00001f1f, DDR_CTR_BASE + 0x0210); // (          ADDRMAP4) 'h00001f1f uMCTL2 0x0210
	writel(0x07070707, DDR_CTR_BASE + 0x0214); // (          ADDRMAP5) 'h040f0404 uMCTL2 0x0214
	writel(0x07070707, DDR_CTR_BASE + 0x0218); // (          ADDRMAP6) 'h0f0f0404 uMCTL2 0x0218
	writel(0x00000f0f, DDR_CTR_BASE + 0x021c); // (          ADDRMAP7) 'h00000f0f uMCTL2 0x021c
	writel(0x00003f01, DDR_CTR_BASE + 0x0220); // (          ADDRMAP8) 'h00003f01 uMCTL2 0x0220  // bg[0] --> hif[3]
	writel(0x00000000, DDR_CTR_BASE + 0x0224); // (          ADDRMAP9) 'h04040404 uMCTL2 0x0224
	writel(0x00000000, DDR_CTR_BASE + 0x0228); // (         ADDRMAP10) 'h04040404 uMCTL2 0x0228
	writel(0x001f1f00, DDR_CTR_BASE + 0x022c); // (         ADDRMAP11) 'h00000004 uMCTL2 0x022c
	// *******************************************************************
	writel(0x00000001, DDR_CTR_BASE + 0x0f04); // (            SARBASE0) 'h00000002 uMCTL2 0x0f04   // system address region 0 base address 0x8000_0000
	writel(0x0000000f, DDR_CTR_BASE + 0x0f08); // (            SARBASE0) 'h00000001 uMCTL2 0x0f04   // system address region 0 block size 2GB
	writel(0x00000011, DDR_CTR_BASE + 0x0f0c); // (            SARBASE1) 'h00000022 uMCTL2 0x0f0c   // system address region 1 base address 0x1_0000_0000
	writel(0x0000000f, DDR_CTR_BASE + 0x0f10); // (            SARBASE1) 'h0000001d uMCTL2 0x0f10   // system address region 1 block size 4GB

	writel(0x06000500, DDR_CTR_BASE + 0x0240); // (            ODTCFG) 'h06000500 uMCTL2 0x0240
	writel(0x00003030, DDR_CTR_BASE + 0x0244); // (            ODTMAP) 'h00003030 uMCTL2 0x0244
	writel(0x03002f14, DDR_CTR_BASE + 0x0250); // (             SCHED) 'h03230205 uMCTL2 0x0250 // orginal rw policy,
	writel(0x33330008, DDR_CTR_BASE + 0x0254); // (            SCHED1) 'h00000000 uMCTL2 0x0254
	writel(0x0f00000f, DDR_CTR_BASE + 0x025c); // (          PERFHPR1) 'h0f000001 uMCTL2 0x025c
	writel(0x0f00007f, DDR_CTR_BASE + 0x0264); // (          PERFLPR1) 'h0f00007f uMCTL2 0x0264
	writel(0x0f00007f, DDR_CTR_BASE + 0x026c); // (           PERFWR1) 'h0f00007f uMCTL2 0x026c
	writel(0x00000011, DDR_CTR_BASE + 0x0300); // (              DBG0) 'h00000011 uMCTL2 0x0300
	writel(0x00000000, DDR_CTR_BASE + 0x030c); // (            DBGCMD) 'h00000000 uMCTL2 0x030c
	writel(0x00000001, DDR_CTR_BASE + 0x0320); // (             SWCTL) 'h00000001 uMCTL2 0x0320
	writel(0x00110011, DDR_CTR_BASE + 0x036c); // (         POISONCFG) 'h00100000 uMCTL2 0x036c
	// ****************************************************
	writel(0x00000042, DDR_CTR_BASE + 0x2020); // (    FREQ1.DERATEEN) 'h00000042 uMCTL2 0x2020
	writel(0xe4fa1694, DDR_CTR_BASE + 0x2024); // (   FREQ1.DERATEINT) 'he4fa1694 uMCTL2 0x2024
	writel(0x00408804, DDR_CTR_BASE + 0x2034); // (      FREQ1.PWRTMG) 'h00408804 uMCTL2 0x2034
	writel(0x00210000, DDR_CTR_BASE + 0x2050); // (    FREQ1.RFSHCTL0) 'h00210000 uMCTL2 0x2050
	writel(0x00920061, DDR_CTR_BASE + 0x2064); // (     FREQ1.RFSHTMG) 'h00920061 uMCTL2 0x2064
	writel(0x0a310001, DDR_CTR_BASE + 0x20dc); // (       FREQ1.INIT3) 'h0a310001 uMCTL2 0x20dc
	writel(0x00280200, DDR_CTR_BASE + 0x20e0); // (       FREQ1.INIT4) 'h00280200 uMCTL2 0x20e0
	writel(0x00000640, DDR_CTR_BASE + 0x20e8); // (       FREQ1.INIT6) 'h00000640 uMCTL2 0x20e8
	writel(0x00000800, DDR_CTR_BASE + 0x20ec); // (       FREQ1.INIT7) 'h00000800 uMCTL2 0x20ec
	writel(0x0000033f, DDR_CTR_BASE + 0x20f4); // (     FREQ1.RANKCTL) 'h0000033f uMCTL2 0x20f4
	writel(0x14132813, DDR_CTR_BASE + 0x2100); // (    FREQ1.DRAMTMG0) 'h14132813 uMCTL2 0x2100
	writel(0x0004051b, DDR_CTR_BASE + 0x2104); // (    FREQ1.DRAMTMG1) 'h0004051b uMCTL2 0x2104
	writel(0x0808030f, DDR_CTR_BASE + 0x2108); // (    FREQ1.DRAMTMG2) 'h0808030f uMCTL2 0x2108
	writel(0x0000400c, DDR_CTR_BASE + 0x210c); // (    FREQ1.DRAMTMG3) 'h0000400c uMCTL2 0x210c
	writel(0x08030409, DDR_CTR_BASE + 0x2110); // (    FREQ1.DRAMTMG4) 'h08030409 uMCTL2 0x2110
	writel(0x07070504, DDR_CTR_BASE + 0x2114); // (    FREQ1.DRAMTMG5) 'h07070504 uMCTL2 0x2114
	writel(0x00000000, DDR_CTR_BASE + 0x2118); // (    FREQ1.DRAMTMG6) 'h00000000 uMCTL2 0x2118 for LPDDR2/3/4
	writel(0x00000707, DDR_CTR_BASE + 0x211c); // (    FREQ1.DRAMTMG7) 'h00000707 uMCTL2 0x211c
	writel(0x03030e05, DDR_CTR_BASE + 0x2120); // (    FREQ1.DRAMTMG8) 'h03030e05 uMCTL2 0x2120
	writel(0x0002040c, DDR_CTR_BASE + 0x2124); // (    FREQ1.DRAMTMG9) 'h0002040c uMCTL2 0x2124
	writel(0x1109010e, DDR_CTR_BASE + 0x212c); // (   FREQ1.DRAMTMG11) 'h1109010e uMCTL2 0x212c
	writel(0x0000000d, DDR_CTR_BASE + 0x2130); // (   FREQ1.DRAMTMG12) 'h0000000d uMCTL2 0x2130
	writel(0x00000000, DDR_CTR_BASE + 0x2138); // (   FREQ1.DRAMTMG14) 'h00000000 uMCTL2 0x2138
	writel(0x80000000, DDR_CTR_BASE + 0x213c); // (   FREQ1.DRAMTMG15) 'h80000000 uMCTL2 0x213c
	writel(0x41000040, DDR_CTR_BASE + 0x2180); // (      FREQ1.ZQCTL0) 'h41000040 uMCTL2 0x2180
	writel(0x028b820e, DDR_CTR_BASE + 0x2190); // (     FREQ1.DFITMG0) 'h028b820e uMCTL2 0x2190
	writel(0x01070202, DDR_CTR_BASE + 0x2194); // (     FREQ1.DFITMG1) 'h01070202 uMCTL2 0x2194
	writel(0x06000500, DDR_CTR_BASE + 0x2240); // (      FREQ1.ODTCFG) 'h06000500 uMCTL2 0x2240
	writel(0x00000000, DDR_CTR_BASE + 0x0400); // (             PCCFG) 'h00000000 uMCTL2 0x0400
	writel(0x000152fc, DDR_CTR_BASE + 0x0404); // (           PCFGR_0) 'h0000000f uMCTL2 0x0404
	writel(0x000152fc, DDR_CTR_BASE + 0x04b4); // (           PCFGR_1) 'h0000000f uMCTL2 0x04b4
	writel(0x000152fc, DDR_CTR_BASE + 0x0564); // (           PCFGR_2) 'h0000000f uMCTL2 0x0564
	writel(0x000052fc, DDR_CTR_BASE + 0x0408); // (           PCFGW_0) 'h0000100e uMCTL2 0x0408
	writel(0x000052fc, DDR_CTR_BASE + 0x04b8); // (           PCFGW_1) 'h0000100e uMCTL2 0x04b8
	writel(0x000052fc, DDR_CTR_BASE + 0x0568); // (           PCFGW_2) 'h0000100e uMCTL2 0x0568

	//===================================================================================
	writel(0x000007ff, DDR_REG_BASE + DDR_CON23) ; //ddr_phy presetn 1->0
	writel(0x00007fff, DDR_REG_BASE + DDR_CON22); // for aresetn,coreRSTN gen
	//===================================================================================

	writel(0x00000000, DDR_CTR_BASE + 0x0304); // (              DBG1) 'h00000000 uMCTL2 0x0304
	writel(0x00000000, DDR_CTR_BASE + 0x0030); // (            PWRCTL) 'h00000000 uMCTL2 0x0030
	writel(0x00000000, DDR_CTR_BASE + 0x0320); // (             SWCTL) 'h00000000 uMCTL2 0x0320
	writel(0x00000010, DDR_CTR_BASE + 0x01b0); // (           DFIMISC) 'h00000010 uMCTL2 0x01b0

	//===================================================================================
	//PHY INIT
	//===================================================================================
	ddr4_HBM_phy_init_maji();

	writel(0x00000030, DDR_CTR_BASE + 0x1b0); //(           DFIMISC) 'h00000001 uMCTL2

	rdata_t = 0;
	while(rdata_t != 0x00000001) {
		rdata_t = read_reg32(DDR_CTR_BASE + 0x1bc);   //(              DFISTAT) 'h00000000  uMCTL2
	}
	writel(0x00000010, DDR_CTR_BASE + 0x1b0); //(           DFIMISC) 'h00000001 uMCTL2
	writel(0x00000011, DDR_CTR_BASE + 0x1b0); //(           DFIMISC) 'h00000001 uMCTL2

	writel(0x00000001, DDR_CTR_BASE + 0x320); //(             SWCTL) 'h00000001 uMCTL2
	rdata_t = 0;
	while(rdata_t != 0x00000001) {
		rdata_t = read_reg32(DDR_CTR_BASE +0x324);   //(            SWSTAT) 'h00000000  uMCTL2
	}
	rdata_t = 0;
	while(rdata_t != 0x00000001) {
		rdata_t = read_reg32(DDR_CTR_BASE + 0x004);   //(              STAT) 'h00000000  uMCTL2
	}

	// training
	writel(0x00000010, DDR_CTR_BASE + 0x01b0); // (           DFIMISC) 'h00000001 uMCTL2

	writel(0x00000000, DDR_CTR_BASE + 0x0030); //(            PWRCTL) 'h00000003 uMCTL2 0x0030
	writel(0x00000001, DDR_CTR_BASE + 0x0490); //(           PCTRL_0) 'h00000001 uMCTL2 0x0490
	writel(0x00000001, DDR_CTR_BASE + 0x0540); //(           PCTRL_1) 'h00000001 uMCTL2 0x0540
	writel(0x00000001, DDR_CTR_BASE + 0x05f0); //(           PCTRL_2) 'h00000001 uMCTL2 0x05f0

	writel(0x1234abcd, DDR_CTR_BASE + 0x0004); // release timing disable
}

#endif

#if 0

#define ARTOSYN_PINSHARE_BASE		(0x01073000)
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


void config_pinshare(unsigned int pad_num, unsigned int val)
{
	unsigned int reg_val = 0;
	unsigned int reg_num, bit_num;
	unsigned int mask = 0x3ff;

	reg_num = pad_num / 3;
	bit_num = (pad_num % 3) * 10;
	reg_val = readl(ARTOSYN_PINSHARE_BASE + reg_num * 4);
	reg_val &= (~(mask << bit_num));
	reg_val |= ((val & mask) << bit_num);
	writel(reg_val, ARTOSYN_PINSHARE_BASE + reg_num * 4);
}

void config_pinselect(unsigned int reg_offset, unsigned int val, unsigned int bit_offset)
{
	unsigned int reg_val = 0;
	unsigned int mask = 0x7;

	reg_val = readl(ARTOSYN_PINSELECT_BASE + reg_offset * 4);
	reg_val &= (~(mask << bit_offset));
	reg_val |= ((val & mask) << bit_offset);
	writel(reg_val, ARTOSYN_PINSELECT_BASE + reg_offset * 4);
}

void emmc_pinshare_config(void)
{
	config_pinshare(111, PIN_FUN(2));
	config_pinshare(112, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(113, PIN_FUN(2));
	config_pinshare(114, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(115, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(116, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(117, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(118, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(119, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(120, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(121, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(122, PIN_FUN(2));
	udelay(10);
}

void sd_pinshare_config(void)
{
	config_pinshare(28, PIN_FUN(3));
	config_pinselect(17, 0, 9);
	config_pinshare(29, PIN_FUN(3));
	config_pinselect(17, 0, 12);
	config_pinshare(85, PIN_FUN(2));
	config_pinshare(86, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(87, PIN_FUN(2));
	config_pinshare(88, PIN_FUN(2));
	config_pinshare(89, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(90, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(91, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(92, PIN_FUN(2) | PIN_3V3_PU);
	config_pinshare(145, PIN_FUN(1) | PIN_3V3_PU);
	config_pinshare(146, PIN_FUN(1) | PIN_3V3_PU);
	config_pinshare(147, PIN_FUN(1) | PIN_3V3_PU);
	config_pinshare(148, PIN_FUN(1) | PIN_3V3_PU);
	udelay(10);
}
#endif

int arch_cpu_init(void)
{
	int ret;
	struct udevice *dev;
	spl_header *pHeader = (spl_header*)SPL_HEADER_BUF_BASE;
	volatile unsigned int tmp;

	/* Alignment abort disable, otherwise lz4 decompress will failed */
	set_sctlr(get_sctlr() & ~CR_A);

	/* power on system: pull up pwr_en */
	writel(0x10001, SYS_POWER_ON_CTRL);

	//reset normal wdt
	tmp = readl(0x08b00000);
	tmp &= ~(1 << 29);
	writel(tmp, 0x08b00000);

	//reset sec wdt
	tmp = readl(0x0105007c);
	tmp &= ~(1 << 0);
	writel(tmp, 0x0105007c);
	udelay(5);
	tmp = readl(0x0105007c);
	tmp |= (1 << 0);
	writel(tmp, 0x0105007c);

#ifndef FPGA_SIMULATION
	/* set cpu to 900MHz */
	tmp = readl(0x010e0000);
	tmp &= ~0x1;
	writel(tmp, 0x010e0000);
	tmp |= 0x1;
	writel(tmp, 0x010e0000);

	udelay(5);

	tmp &= ~(0x1 << 26);
	writel(tmp, 0x010e0000);
	tmp |= 0x1 << 26;
	writel(tmp, 0x010e0000);
	tmp &= ~(0x1 << 24);
	writel(tmp, 0x010e0000);
	tmp |= 0x1 << 24;
	writel(tmp, 0x010e0000);

	udelay(5);

	tmp &= 0xffff00ff;
	tmp |= ((90) << 8);
	writel(tmp, 0x010e0000);

	tmp &= ~0x1;
	writel(tmp, 0x010e0000);

	udelay(5);

	tmp &= ~(0x1 << 24);
	writel(tmp, 0x010e0000);

	tmp &= ~(0x1 << 26);
	writel(tmp, 0x010e0000);

	udelay(5);

	/* set dsp_pll0 to 2000Mhz(170M to 250M) */
	writel(0xc8, 0x01072048);

	/* set dsp_pll1 to 1800Mhz, noc clock to default value, maybe changed in romcode patch */
	writel(0xb4, 0x01072060);

	/* set wdt enable and select reset cpu mode */
	writel(0x04, 0x01074030);
#endif

	/* set sram nonsecure access */
	writel(0xff, 0x01172448);
	writel(0xff, 0x01172468);

	/* set ip to sec */
	writel(0x0, 0x0105006c);
	writel(0x1c00, 0x01050074);

	arch_timer_init();

	tzc400_init(TZC_4F_BASE, 0x0, 0xf, 0x3); //base addr, action, gate_keeper, spectulation
	tzc400_rgn_config(TZC_4F_BASE, 0, 0x0, 0x0, 0xFFFFFFFF, 0xFFFFFFFF, 0xc000000f, 0xffffffff);

	ar_cci_init();
#ifdef VELOCE_DDR
	ddr4_init();
#endif

#ifdef CONFIG_DEBUG_UART
	/*
	 * Debug UART can be used from here if required:
	 *
	 * debug_uart_init();
	 * printch('a');
	 * printhex8(0x1234);
	 * printascii("string");
	 */
	debug_uart_init();
	debug("\nspl:debug uart enabled in %s\n", __func__);
#endif

	board_early_init_f();

	ret = spl_early_init();
	if (ret) {
		printf("spl_early_init() failed: %d\n", ret);
		hang();
	}

#ifndef FPGA_SIMULATION
	ret = uclass_get_device(UCLASS_CLK, 0, &dev);
	if (ret) {
		debug("%s: Cannot find clock device\n", __func__);
	}
#endif

#if !defined(CONFIG_TPL) || defined(CONFIG_SPL_RAM)
	if(BOOT_DEVICE_USB == ((pHeader->boot_info & 0xffff0000) >> 16) ||
	   BOOT_DEVICE_UART == ((pHeader->boot_info & 0xffff0000) >> 16))
	{
		int ret0, ret1, offset=-1;
		unsigned int params[35];
		unsigned int basic[31];
		void *blob = board_fdt_blob_setup();

		gd->ram_size = 0x40000000;

		offset = fdt_node_offset_by_compatible(blob, 0, "artosyn,ar9301-dmc");
		if (offset >= 0) {
			ret0 = fdtdec_get_int_array(gd->fdt_blob, offset, "params",
					   params, 35);

			ret1 = fdtdec_get_int_array(gd->fdt_blob, offset, "phy-input-basic",
					   basic, 31);
			if(0 == ret0 && 0 == ret1) {
				gd->ram_size = ((phys_size_t)params[3]) * 0x80 * (basic[3] / 2);
				printf("DRAM size is: 0x%08lx, 0x%08x 0x%08x\n", gd->ram_size, params[3], basic[3]);
			}else {
				printf("ret :%d %d\n", ret0, ret1);
			}
		}
		else {
			printf("dmc offset is:%d\n", offset);
		}
	}
	gd->ram_top = gd->ram_base + get_effective_memsize();
//	gd->ram_top = board_get_usable_ram_top(gd->ram_size);
#endif
#if 0
	emmc_pinshare_config();
	sd_pinshare_config();
#endif
	if(BOOT_DEVICE_USB == ((pHeader->boot_info & 0xffff0000) >> 16) ||
	   BOOT_DEVICE_UART == ((pHeader->boot_info & 0xffff0000) >> 16))
	{
		void (*uboot_entry)(void);
		unsigned int * uboot_img_addr = (unsigned int *)UPGRADE_BOOT_UBOOT_ADDR_REG;
		struct sirius_image_header *ar_header = NULL;
		unsigned char * hash_buffer = NULL;
		unsigned char * signature_buffer = NULL;
		unsigned char * uboot = NULL;
		struct image_header *header;
		unsigned int uboot_len = 0;

		if(*uboot_img_addr)
		{
			ar_header = (struct sirius_image_header *)(unsigned long)(*uboot_img_addr);
		}
		else
		{
			ar_header = (struct sirius_image_header *)(unsigned long)(CONFIG_SYS_TEXT_BASE);
		}
		
		hash_buffer = ((unsigned char *)ar_header) + sizeof(*ar_header);
		signature_buffer = hash_buffer + ar_header->hash_size;
		header = signature_buffer + ar_header->sig_len;
		uboot_len = image_get_size(header);		
		uboot_entry = (void (*)())((unsigned long)(image_get_ep(header)));
		uboot = ((unsigned char *)header) + sizeof(*header);
		
		if(0x4152544f != ar_header->magic)
		{
			printf("%s[%d]: magic error!\n", __FUNCTION__, __LINE__);
			hang();
		}

		if(secure_boot_enabled())
		{
			//printf("%p %p %p %p %u %u %u %u\n", ar_header, hash_buffer, signature_buffer, header,
			//	sizeof(*ar_header), ar_header->hash_size, ar_header->sig_len, ar_header->img_len);

			if(0 != spl_verify_sw("uboot", (unsigned char *)hash_buffer, ar_header->hash_size, 
				(unsigned char *)signature_buffer, ar_header->sig_len, 
				(unsigned char *)header, ar_header->img_len))
			{
				printf("%s[%d]: verify %s error!\n", __FUNCTION__, __LINE__, "u-boot");
				hang();
			}
		}

		if(uboot_entry != uboot)
		{
			memmove((void *)uboot_entry, uboot, uboot_len);
		}
		
		printf("jump uboot %p!\n", uboot_entry);
		if(uboot_entry) {
			uboot_entry();
		}
	}

	return 0;
}
#endif
