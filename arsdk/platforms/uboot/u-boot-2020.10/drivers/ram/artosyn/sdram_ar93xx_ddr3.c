/*
 * (C) Copyright 2022 Artosyn Electronics Co., Ltd
 *
 * SPDX-License-Identifier: GPL-2.0+
 */
#include "sdram_ar93xx.h"
#include <u-boot/sha256.h>

typedef struct artosyn_user_modify {
	unsigned int type;		// params->type

	unsigned int freq0;		// params->freq0
	unsigned int freq1;		// params->freq1
	unsigned int density;		// params->density
					// 	256MB: 0x200000
					//	512MB: 0x400000
					//	1GB:   0x800000
					//	2GB:   0x1000000
	unsigned int ddr_hbm;		// params->ddr_hbm
					//	16bit: 0x1
					//	32bit: 0x0
	unsigned int mr1_odic;		// params->mr1_odic
					// 	0: 34ohm
					//	1: 48ohm
					//	2: 40ohm(Micron only)
	unsigned int mr1_rtt_nom;	// params->mr1_rtt_nom
					//	0: Disalbe
					//	1: 60ohm
					//	2: 120ohm
					//	3: 40ohm
					//	4: 240ohm
					//	5: 48ohm
					//	6: 80ohm
					//	7: 34ohm
	unsigned int mr4_rdpreamble;	// params->mr4_rdpreamble
	unsigned int mr4_wrpreamble;	// params->mr4_wrpreamble
	unsigned int DramType;		// phy-input-base->DramType
	unsigned int NumDbyte;		// phy-input-base->NumDbyte
					//	16bit: 0x2
					//	32bit: 0x4
	unsigned int NumActiveDbyteDfi0;// phy-input-base->NumActiveDbyteDfi0
					//	16bit: 0x2
					//	32bit: 0x4
	unsigned int NumPStates;	// phy-input-base->NumPStates
	unsigned int Frequency[4];	// phy-input-basic->Frequency[4]
	unsigned int D4RxPreambleLength[4];// phy-input-advanced->D4RxPreambleLength[4]
	unsigned int D4TxPreambleLength[4];// phy-input-advanced->D4TxPreambleLength[4]
	unsigned int ODTImpedance[4];	// phy-input-advanced->ODTImpedance[4]
	unsigned int TxImpedance[4];	// phy-input-advanced->TxImpendance[4]
	unsigned int ATxImpedance;	// phy-input-advanced->ATxImpedance[4]


	unsigned int ssc_en;		// phy-pll-config->ssc_en
	unsigned int ssc_divn;		// phy-pll-config->ssc_divn
	unsigned int ssc_num_step;	// phy-pll-config->ssc_num_step
	unsigned int ssc_kdelta;	// phy-pll-config->ssc_kdelta
	unsigned int ssc_mode;		// phy-pll-config->ssc_mode
	unsigned int skip_train;	// runtimeConfig->skip_train
} __attribute__ ((packed)) artosyn_user_modify_t;

struct artosyn_dram_info {
#ifdef CONFIG_SPL_BUILD
	void __iomem *ddr_phy_base;
	void __iomem *ddr_ctrl_base;
	void __iomem *ddr_reg_base;
	void __iomem *ddr_mon_base;
	void __iomem *axi_mon0_base;
	void __iomem *axi_mon1_base;
	void __iomem *axi_mon2_base;
#endif
	artosyn_vendor_header_t vendor_header;
	unsigned int restore_offset;

	artosyn_dram_params_t	params;
	artosyn_dphy_pll_cfgs_t	pll_default_cfgs;
	artosyn_dphy_misc_t	misc;
	runtime_config_t	runtimeConfig;
	user_input_basic_t	userInputBasic;
	user_input_advanced_t	userInputAdvanced;
	artosyn_user_modify_t	modify;

	struct ram_info info;
	unsigned int warmboot_support;
};

#ifdef CONFIG_SPL_BUILD

uint8_t buff[RET_FLASH_DATA_SIZE];

/* === Global Struct Defines ===
 * instance of runtime objects
 * instance of useInputBasic
 * instance of userInputAdvanced
 * instance of userInputSim
 */
//runtime_config_t	runtimeConfig;
//user_input_basic_t	userInputBasic;
//user_input_advanced_t	userInputAdvanced;
//user_input_sim_t	userInputSim;

/* === Firmware Message Block Structs ===
 * 1D message block instance
 * Shadow of 1D message block. Used by PhyInit to track user changes to the
 * data structure.
 * 2D message block instance
 * Shadow of 2D message block. Used by PhyInit to track user changes to the
 * data structure.
 */
PMU_SMB_DDR3U_1D_t	mb_DDR3U_1D[4] __attribute__((aligned(4)));
PMU_SMB_DDR3U_1D_t	shdw_DDR3U_1D[4] __attribute__((aligned(4)));

PMU_SMB_DIAG_t		mb_diag __attribute__((aligned(4)));

static unsigned int warmboot;

static int *filter_base_addr;

extern int spl_mmc_read_from_hw_partition(int hwpart, lbaint_t offset,
					  lbaint_t blkcnt, void *buffer);
extern int spl_mmc_write_to_hw_partition(int hwpart, lbaint_t offset,
					 lbaint_t blkcnt, void *buffer);

static void ar9301_sdram_write(unsigned int *addr, unsigned int val) {
#ifdef CONFIG_ARTOSYN_AR9311
	size_t offset;

	offset = (size_t)addr - (size_t)filter_base_addr;
	if (offset == 0x0020 || offset == 0x00d8 || offset == 0x00f4 ||
	    offset == 0x0118 || offset == 0x011c || offset == 0x0128 ||
	    offset == 0x0138 || offset == 0x0188 || offset == 0x01b8 ||
	    offset == 0x0564 || offset == 0x0568 || offset == 0x05f4 ||
	    offset == 0x05f8 || offset == 0x05fc || offset == 0x0600 ||
	    offset == 0x05f0 )
		return;
#endif

#ifdef AR9301_SDRAM_DEBUG
	printf("`APB_MST_BFM.apb_write32(32'h%8p, 32'h%08x);\n", addr, val);
	writel(val, addr);
#else
	writel(val, addr);
#endif
}

static unsigned int ar9301_sdram_read(unsigned int *addr) {
	unsigned int val = 0;

#ifdef AR9301_SDRAM_DEBUG
	printf("`APB_MST_BFM.apb_read32(32'h%8p);\n", addr);
	val = readl(addr);
#else
	val = readl(addr);
#endif
	return val;
}

static void ar9301_cfg_params(struct artosyn_dram_info *dram, uint32_t freq,
			uint32_t density, uint32_t refresh_mode,
			artosyn_dram_configs_t *cfgs) {
	switch (freq) {
	case 400: // 800
		cfgs->tck_freq = 200;
		cfgs->tcl = 6;
		cfgs->tcwl = 5;
		break;
	case 533: // 1066
		cfgs->tck_freq = 266;
		cfgs->tcl = 7;
		cfgs->tcwl = 6;
		break;
	case 666: // 1333
		cfgs->tck_freq = 333;
		cfgs->tcl = 9;
		cfgs->tcwl = 7;
		break;
	case 800: // 1600
		cfgs->tck_freq = 400;
		cfgs->tcl = 11;
		cfgs->tcwl = 8;
		break;
	case 933: // 1866
		cfgs->tck_freq = 466;
		cfgs->tcl = 13;
		cfgs->tcwl = 9;
		break;
	case 1066: // 2132
		cfgs->tck_freq = 533;
		cfgs->tcl = 14;
		cfgs->tcwl = 10;
		break;
	default: // 2133
		cfgs->tck_freq = 533;
		cfgs->tcl = 14;
		cfgs->tcwl = 10;
		break;
	}
	cfgs->trcd = cfgs->tcl;
	cfgs->trp = cfgs->tcl;

	if (density == 0x100000) {
		cfgs->trfc = 11;
	} else if (density == 0x200000) {
		cfgs->trfc = 16;
	} else if (density == 0x400000) {
		cfgs->trfc = 26;
	} else if (density == 0x800000) {
		cfgs->trfc = 35;
	} else {
		cfgs->trfc = 35;
	}
	cfgs->trefi = 39;
	cfgs->clk_mode = dram->params.clk_mode;
}

static void ar9301_cal_mode_regs(struct artosyn_dram_info *dram, uint32_t tcl,
				 uint32_t tck_freq, uint32_t tcwl,
				 uint32_t mr_regs[3]) {
	unsigned int mr0_cl		= 0;
	unsigned int mr0_wr		= 0;	//WR=15ns, RTP=7.5ns
	unsigned int mr0_rbt		= 0; 	//sequence
	unsigned int mr0_dll_reset	= 1;
	unsigned int mr0_tm		= 0;

	unsigned int mr1_dll_enable	= 0;	//dll: 0 enable, 1:disable
	unsigned int mr1_odic		= dram->params.mr1_odic;	//drv: 0：34ohm,1:48ohm,2:40ohm (Micron only)
	unsigned int mr1_al		= 0;	//AL: 0:Disable(Normal), 1:CL-1, 2:CL-2
	unsigned int mr1_wrlev		= 0;	//write leveling: 0: off, 1: on
	unsigned int mr1_rtt_nom	= dram->params.mr1_rtt_nom;	//ODT: 0:Disable, 1:60ohm, 2:120ohm, 3:40ohm, 4:240ohm, 5:48ohm, 6:80ohm, 7:34ohm
	unsigned int mr1_tdqs		= 0;	//TDQS: 0:Disable, 1:Enable
	unsigned int mr1_qoff		= 0;	//Output buffer enable: 0:Enable(Normal), 1:Disabled

	unsigned int mr2_cwl		= 0;
	unsigned int mr2_lp_asr		= 0;	// Manual(Normal)
	unsigned int mr2_rtt_wr		= 0;	// Dynamic ODT Off(Normal)

	unsigned int t_wr = 0;

	if (mr0_rbt) {
		switch (tcl) {
		case 5: mr0_cl = 0x06; break;
		case 6: mr0_cl = 0x0a; break;
		case 7: mr0_cl = 0x0e; break;
		case 8: mr0_cl = 0x12; break;
		case 9: mr0_cl = 0x16; break;
		case 10: mr0_cl = 0x1a; break;
		case 11: mr0_cl = 0x1e; break;
		case 12: mr0_cl = 0x03; break;
		case 13: mr0_cl = 0x07; break;
		case 14: mr0_cl = 0x0b; break;
		case 15: mr0_cl = 0x0f; break;
		case 16: mr0_cl = 0x13; break;
		default: mr0_cl = 0x1f; break;
		}
	} else {
		switch (tcl) {
		case 5: mr0_cl = 0x04; break;
		case 6: mr0_cl = 0x08; break;
		case 7: mr0_cl = 0x0c; break;
		case 8: mr0_cl = 0x10; break;
		case 9: mr0_cl = 0x14; break;
		case 10: mr0_cl = 0x18; break;
		case 11: mr0_cl = 0x1c; break;
		case 12: mr0_cl = 0x01; break;
		case 13: mr0_cl = 0x05; break;
		case 14: mr0_cl = 0x09; break;
		case 15: mr0_cl = 0x0d; break;
		case 16: mr0_cl = 0x11; break;
		default: mr0_cl = 0x1d; break;
		}
	}

	t_wr = (15 * tck_freq * 2) / 1000 + 1;
	if (t_wr < 5)
		t_wr = 5;

	switch (t_wr) {
	case 5: mr0_wr = 0x1; break;
	case 6: mr0_wr = 0x2; break;
	case 7: mr0_wr = 0x3; break;
	case 8: mr0_wr = 0x4; break;
	case 10: mr0_wr = 0x5; break;
	case 12: mr0_wr = 0x6; break;
	case 14: mr0_wr = 0x7; break;
	case 16: mr0_wr = 0x0; break;
	default: mr0_wr = 0x7; break;
	}

	switch (tcwl) {
	case 5: mr2_cwl = 0x0; break;
	case 6: mr2_cwl = 0x1; break;
	case 7: mr2_cwl = 0x2; break;
	case 8: mr2_cwl = 0x3; break;
	case 9: mr2_cwl = 0x4; break;
	case 10: mr2_cwl = 0x5; break;
	case 11: mr2_cwl = 0x6; break;
	case 12: mr2_cwl = 0x7; break;
	default: mr2_cwl = 0x5; break;
	}

	mr_regs[0] = 0x1 | mr0_cl << 2 | mr0_tm << 7 | mr0_dll_reset << 8 |
		     mr0_wr << 9 | 0x1 << 12;
	mr_regs[1] = mr1_dll_enable | (mr1_odic & 0x1) << 1 | (mr1_rtt_nom & 0x1) << 2 |
		     mr1_al << 3 | (mr1_odic & 0x2) << 4 | (mr1_rtt_nom & 0x2) << 5 |
		     mr1_wrlev << 7 | (mr1_rtt_nom & 0x4) << 7 | mr1_tdqs << 11 |
		     mr1_qoff << 12;
	mr_regs[2] = mr2_cwl << 3 | mr2_lp_asr << 6 | mr2_rtt_wr << 9;
}

uint16_t ddrphy_io_read16(struct artosyn_dram_info *dram, uint64_t addr);
/*
 * Function to implement a register write to the PHY
 *
 * The default behvior of ddrphy_io_write16(dram, ) is to print
 * the APB write commands calculated by PhyInit. User can edit this function to
 * print differently, or implement a mechanism to trigger a APB write event in
 * simulation.
 *
 * \param adr 32-bit integer indicating address of CSR to be written
 * \param dat 16-bit integer for the value to be written to the CSR
 * \returns \c void
 */
void ddrphy_io_write16(struct artosyn_dram_info *dram, uint64_t addr, uint16_t dat)
{
#ifdef AR9301_SDRAM_DEBUG
	printf("`APB_MST_BFM.apb_write32(`DDR_PHY_BASE + 32'h%08x, 32'h%04x);\n",
	       (uint32_t)(addr * sizeof(uint32_t)), dat);
	writel(dat, dram->ddr_phy_base + addr * sizeof(uint32_t));
#else
	writel(dat, dram->ddr_phy_base + addr * sizeof(uint32_t));
#endif
	// Keep track of register adr/data pairs for retention exit
	// dwc_ddrphy_track_one_reg(addr);
}

/*
 * Function to perform a APB register read on the PHY
 *
 * This is user-editable function.  User can edit this function according to
 * their needs.
 *
 * The user must provide and implementation for reading registers from the PHY
 * in this function.  PhyInit calls this function during PHY initialization to
 * read the register content of the PHY for the purpose of restoring the PHY
 * state during retention exit.

 * Implementation of this function is only necessary if PhyInit retention
 * restore sequence feature is used.
 *
 * For the purpose of generating the output txt file, the below implementation
 * prints a "dwc_ddrphy_apb_rd()" commands to the output for reference. The
 * output text is commented out and 0 is always returned.
 *
 *  \param adr address of register to read
 *  \returns register value
 */
uint16_t ddrphy_io_read16(struct artosyn_dram_info *dram, uint64_t addr)
{
	// user must provide an implementation for returning register value
#ifdef AR9301_SDRAM_DEBUG
	printf("`APB_MST_BFM.apb_read32(`DDR_PHY_BASE + 32'h%08x);\n",
		(uint32_t)(addr * sizeof(uint32_t)));
	return readl(dram->ddr_phy_base + addr * sizeof(uint32_t));
#else
	return readl(dram->ddr_phy_base + addr * sizeof(uint32_t));
#endif
}

/*
 * This is used to initialize the PhyInit structures before user defaults and
 * overrides are applied.
 *
 * Train2D 1=1D & 2D training enabled, 0=only 1D training.
 * @return Void
 */
void ddrphy_init_struct(struct artosyn_dram_info *dram)
{
//	runtime_config_t	*runtime = &dram->runtimeConfig;
//	user_input_basic_t	*basic = &dram->userInputBasic;
//	user_input_advanced_t	*advanced = &dram->userInputAdvanced;

//	pr_info("Initialize structures\n");
	// Zero out struct contents
//	memset((void *)basic, 0, sizeof(userInputBasic));
//	memset((void *)advanced, 0, sizeof(userInputAdvanced));
//	memset((void *)&mb_DDR3U_1D, 0, 4 * sizeof(PMU_SMB_DDR3U_1D_t));
//	memset((void *)&shdw_DDR3U_1D, 0, 4 * sizeof(PMU_SMB_DDR3U_1D_t));
//	if (runtime->Train2D) {
//		memset((void *)&mb_DDR3U_2D, 0, 4 * sizeof(PMU_SMB_DDR3U_2D_t));
//		memset((void *)&shdw_DDR3U_2D, 0,
//			4 * sizeof(PMU_SMB_DDR3U_2D_t));
//	}
	// memset((void *)&userInputSim, 0, sizeof(userInputSim));
}

/*
 * This function provides a default configuration of PHY. To change the default
 * configuration, the user can edit this file directly or use the
 * dwc_ddrphy_phyinit_userCustom_overrideUserInput() function. The default
 * values selected below only represent a general case and may not be correct
 * for every use case.
 *
 * Train2D 1=1D & 2D training enabled, 0=only 1D training.
 * @return Void
 */
void ddrphy_set_default(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	artosyn_dphy_misc_t *misc = &dram->misc;
	// config params base on input params from dts for cal mode regs
	artosyn_dram_configs_t cfgs;
	uint32_t mr_regs[3];

//	printf("Set default\n");

	memset((void *)&cfgs, 0, sizeof(artosyn_dram_configs_t));

	/* ##############################################################
	 * Basic Message Block Variables
	 * ##############################################################
	 */
	uint8_t myps;

	/* ##############################################################
	 * These are typically invariant across Pstate
	 * ##############################################################
	 */
	// For fast simulation
	//uint8_t MsgMisc			= 0x06;

	// Set Reserved00[7] = 1   (If using T28 attenuated receivers)
	// Set Reserved00[6:0] = 0 (Reserved; must be programmed to 0)

	// Byte offset 0x00, CSR Addr 0x54000, Direction=In
	// Reserved00[0:4] RFU, must be zero
	// Reserved00[5] = Train vrefDAC0 During Read Deskew
	//	0x1 = Read Deskew will begin by enabling and roughly training
	//	      the phy's per-lane reference voltages.  Training the
	//	      vrefDACs CSRs will increase the maximum 1D training time
	//	      by around half a millisecond, but will improve 1D training
	//	      accuracy on systems with significant voltage-offsets
	//	      between lane read eyes.
	//	0x0 = Read Deskew will assume the messageblock's phyVref setting
	//	      will work for all lanes.
	//
	// Reserved00[6] = Enable High Effort WrDQ1D
	//	0x1 = WrDQ1D will conditionally retry training at several extra
	//	      RxClkDly Timings. This will increase the maximum 1D
	//	      training time by up to 4 extra iterations of WrDQ1D. This
	//	      is only required in systems that suffer from very large,
	//	      asymmetric eye-collapse when receiving PRBS patterns.
	//	0x0 = WrDQ1D assume rxClkDly values found by SI Friendly RdDqs1D
	//	      will work for receiving PRBS patterns
	//
	// Reserved00[7] = Optimize for the special hard macros in TSMC28.
	//	0x1 = set if the phy being trained was manufactured in any
	//	      TSMC28 process node.
	//	0x0 = otherwise, when not training a TSMC28 phy, leave this
	//	      field as 0.
	uint8_t Reserved00		= 0x20;

	// Indicates presence of DRAM at each chip select for PHY.
	// If the bit is set to 1, the CS is connected to DRAM.
	// If the bit is set to 0, the CS is not connected to DRAM.
	//
	// Set CsPresent[0]   = 1 (if CS0 is populated with DRAM)
	// Set CsPresent[1]   = 1 (if CS1 is populated with DRAM)
	// Set CsPresent[2]   = 1 (if CS2 is populated with DRAM)
	// Set CsPresent[3]   = 1 (if CS3 is populated with DRAM)
	// Set CsPresent[7:4] = 0 (Reserved; must be programmed to 0)
	uint8_t CsPresent		= 0x01;

	// 1 is typically good in DDR3
	uint8_t DFIMRLMargin		= 0x01;

	// Set AddrMirror[pstate] if CS[pstate] is mirrored. (typically odd CS are mirroed in DIMMs)
	uint8_t AddrMirror		= 0x00;

	// When Writing Rank* : Bits[3:0] should be set to the desired setting of ODT[3:0] to the DRAM
	uint8_t WRODTPAT_RANK0		= 0x01;
	uint8_t WRODTPAT_RANK1		= 0x02;
	uint8_t WRODTPAT_RANK2		= 0x04;
	uint8_t WRODTPAT_RANK3		= 0x08;

	// When Reading Rank* : Bits[7:4] should be set to the desired setting of ODT[3:0] to the DRAM
	uint8_t RDODTPAT_RANK0		= 0x20;
	uint8_t RDODTPAT_RANK1		= 0x10;
	uint8_t RDODTPAT_RANK2		= 0x80;
	uint8_t RDODTPAT_RANK3		= 0x40;

	/* ##############################################################
	 * These typically change across Pstate
	 * ##############################################################
	 */
//	uint16_t SequenceCtrl[4] = {0x031f, 0x021f, 0x021f, 0x021f};
#if 0
	uint16_t mr0[4] = {0x0630, 0x0, 0x0, 0x0};
	uint16_t mr1[4] = {0x0201, 0x0, 0x0, 0x0};
	uint16_t mr2[4] = {0x0020, 0x0, 0x0, 0x0};
	uint16_t mr3[4] = {0x0400, 0x0, 0x0, 0x0};
	uint16_t mr4[4] = {0x0000, 0x0, 0x0, 0x0};
	uint16_t mr5[4] = {0x0480, 0x0, 0x0, 0x0};
	// mr6[0] Example Vref : 0x18=907mV = 0.75*VDDQ
	uint16_t mr6[4] = {0x0800 | 0x0018, 0x0, 0x0, 0x0};
#endif

	// 2D Training firmware Variables
	// SequenceCtrl2D[0] 2D Training Sequince. 2DTX, 2DRX, DevInit
//	uint8_t  SequenceCtrl2D[4] = {0x61, 0x0, 0x0, 0x0};

	/* ##############################################################
	 * These are per-pstate Control Words for RCD
	 * Please enter the correct values for your configuration
	 * ##############################################################
	 */

	/* ##############################################################
	 * 95% of users will not need to edit below
	 * ##############################################################
	 */
	// 1D message block defaults
	for (myps = 0; myps < 4; myps++) {
		mb_DDR3U_1D[myps].Pstate		= myps;
		mb_DDR3U_1D[myps].SequenceCtrl		= misc->seq_ctrl_1d[myps];
		mb_DDR3U_1D[myps].PhyConfigOverride	= 0x0;
		mb_DDR3U_1D[myps].HdtCtrl		= misc->hdt_ctrl;
		mb_DDR3U_1D[myps].MsgMisc		= misc->msg_misc;
		mb_DDR3U_1D[myps].Reserved00		= Reserved00;
		mb_DDR3U_1D[myps].DFIMRLMargin		= DFIMRLMargin;
		mb_DDR3U_1D[myps].PhyVref		= misc->phy_vref;

		mb_DDR3U_1D[myps].CsPresent		= CsPresent;
		mb_DDR3U_1D[myps].CsPresentD0		= CsPresent;
		mb_DDR3U_1D[myps].CsPresentD1		= 0x0000; //Unused in UDIMM or DRAM-ON-PCB
		mb_DDR3U_1D[myps].AddrMirror		= AddrMirror;

		mb_DDR3U_1D[myps].AcsmOdtCtrl0	= WRODTPAT_RANK0 | RDODTPAT_RANK0;
		mb_DDR3U_1D[myps].AcsmOdtCtrl1	= WRODTPAT_RANK1 | RDODTPAT_RANK1;
		mb_DDR3U_1D[myps].AcsmOdtCtrl2	= WRODTPAT_RANK2 | RDODTPAT_RANK2;
		mb_DDR3U_1D[myps].AcsmOdtCtrl3	= WRODTPAT_RANK3 | RDODTPAT_RANK3;

		mb_DDR3U_1D[myps].EnabledDQs		= (basic->NumActiveDbyteDfi0 + basic->NumActiveDbyteDfi1) * 8;
		mb_DDR3U_1D[myps].PhyCfg		= advanced->Is2Ttiming[myps];

		// calculate mode register per pstate
		ar9301_cfg_params(dram, basic->Frequency[myps],
				  dram->params.density,
				  dram->params.refresh_mode, &cfgs);
		ar9301_cal_mode_regs(dram, cfgs.tcl, cfgs.tck_freq, cfgs.tcwl, mr_regs);
		mb_DDR3U_1D[myps].MR0			= (uint16_t)mr_regs[0];
		mb_DDR3U_1D[myps].MR1			= (uint16_t)mr_regs[1];
		mb_DDR3U_1D[myps].MR2			= (uint16_t)mr_regs[2];
	} // myps
}

/** @brief Helper function to determine if a given DByte is Disabled given PhyInit inputs.
 * @return 1 if disabled, 0 if enabled.
 */
int ddrphy_is_dbyte_disabled(int DbyteNumber, struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int DisableDbyte;
	DisableDbyte = 0;	// default assume Dbyte is Enabled.

	// Implements Section 1.3 of Pub Databook
	DisableDbyte = (DbyteNumber > basic->NumActiveDbyteDfi0-1 ) ? 1:0;

	// Qualify results against MessageBlock
	if (mb_DDR3U_1D[0].EnabledDQs < 1 ||
	    mb_DDR3U_1D[0].EnabledDQs > 8 * basic->NumActiveDbyteDfi0) {
		pr_err("EnabledDQs(%d). Value must be 0 < EnabledDQs < \
			userInputBasic.NumActiveDbyteDfi0*8.\n",
			mb_DDR3U_1D[0].EnabledDQs);
      }

	if (DbyteNumber < 8 ) {
		DisableDbyte = DisableDbyte | (mb_DDR3U_1D[0].DisabledDbyte &
			       (0x1 << DbyteNumber));
	}

	return DisableDbyte;
}

/*
 * This function sets Message Block parameters based on user_input_basic and
 * user_input_advanced. Parameters are only set if not programed by
 * dwc_ddrphy_phyinit_userCustom_overrideUserInput() or
 * ddrphy_set_default(). user changes in these files takes precedence
 * over this function call.
 *
 * MessageBlock fields set:
 *
 *  - DramType
 *  - Pstate
 *  - DRAMFreq
 *  - PllBypassEn
 *  - DfiFreqRatio
 *  - PhyOdtImpedance
 *  - PhyDrvImpedance
 *  - BPZNResVal
 *  - EnabledDQs
 *  - PhyCfg
 *
 * \return void
 */
void ddrphy_calc_mb(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int nad0, nad1;
	uint8_t myps;

	printf("Calculate MB\n");

	nad0 = basic->NumActiveDbyteDfi0;
	nad1 = 0;
#if 0
	// a few checks to make sure valid programing.
	if (nad0 <= 0 || nad1 < 0 || basic->NumDbyte <= 0) {
		pr_err("NumActiveDbyteDfi* out of range.\n");
	}

	if ((nad0 + nad1) > basic->NumDbyte) {
		pr_err("ActiveDbyteDfi* too large\n");
	}

	if (basic->Dfi1Exists == 0 && nad1 != 0) {
		pr_err("Dfi1Exists==0 but NumDbyteDfi0 != 0\n");
	}
#endif
	// 1D message block defaults
	for (myps = 0; myps < basic->NumPStates; myps++) {
#if 0
		if (basic->DimmType == UDIMM || basic->DimmType == SODIMM || basic->DimmType == NODIMM) {
			mb_DDR3U_1D[myps].DramType = 0x1;		// DDR3 UDIMM
		} else if (basic->DimmType == RDIMM) {
			mb_DDR3U_1D[myps].DramType = 0x3;		// DDR3 RDIMM
		} else {
			pr_err("Unsupported DimmType for DDR3!\n");
		}
#else
		if (basic->DimmType == NODIMM)
			mb_DDR3U_1D[myps].DramType = 0x1;		// DDR4 UDIMM
#endif
		mb_DDR3U_1D[myps].Pstate = myps;
		mb_DDR3U_1D[myps].DRAMFreq = basic->Frequency[myps] * 2;
		mb_DDR3U_1D[myps].PllBypassEn = basic->PllBypass[myps];
		mb_DDR3U_1D[myps].DfiFreqRatio = (0 == basic->DfiFreqRatio[myps]) ? 0x1 : ((1 == basic->DfiFreqRatio[myps]) ? 0x2 : 0x4);
#if 0
		if (basic->HardMacroVer == 4) { //HardMacroE
			mb_DDR3U_1D[myps].PhyOdtImpedance = 0;
			mb_DDR3U_1D[myps].PhyDrvImpedance = 0;
		} else {
			mb_DDR3U_1D[myps].PhyOdtImpedance = 0;
			mb_DDR3U_1D[myps].PhyDrvImpedance = 0;
		}
#else
		mb_DDR3U_1D[myps].PhyOdtImpedance = 0;
		mb_DDR3U_1D[myps].PhyDrvImpedance = 0;
#endif
		mb_DDR3U_1D[myps].BPZNResVal = 0;
		mb_DDR3U_1D[myps].EnabledDQs = nad0 * 8;

		uint16_t DisabledDbyte = 0x0;
		int Dbyte;
		for (Dbyte = 0; Dbyte < basic->NumDbyte && Dbyte < 8; Dbyte++) {
			DisabledDbyte = DisabledDbyte | (ddrphy_is_dbyte_disabled(Dbyte, dram) ? (0x1 << Dbyte) : 0x0);
		}
		mb_DDR3U_1D[myps].DisabledDbyte = DisabledDbyte;
		mb_DDR3U_1D[myps].PhyCfg = advanced->Is2Ttiming[myps];
	}
}

/*
 * This function entry point in the initialization sequence can be used to
 * implement Step (A) of the initialization sequence described in PUB the
 * databook.
 *
 * The User can choose to leave this function as is, or implement a mechanism
 * to trigger power ramp-up event in simulation. In the output text file, this
 * function by default prints a call to a function of the same name.
 *
 * If the user chooses to use this function, the PHY expects all power pins
 * (VDD, VDDQ and VAA) to transition from off to on stage as part of this step.
 *
 * The power supplies can come up and stabilize in any order. While the power
 * supplies are coming up, all outputs will be unknown and the values of the
 * inputs are don't cares.
 *
 * @return Void
 *
 */
void ddrphy_a_bringup_power(void)
{
	//##############################################################
	// Step (A) : Bring up VDD, VDDQ, and VAA \n");
	// See PhyInit App Note for detailed description and function usage
	//##############################################################
	printf("(A) Bring up VDD, VDDQ, and VAA\n");
}

static void ddrphy_pll_config(artosyn_dphy_pll_cfgs_t *pll_cfgs)
{
	uint32_t dig_reg0 = 0;	// ddrpll_synth_dig_top divr
				// FCW = divr/2^24
	uint32_t timeout = 100;

	printf("DDR change freq %d\n", pll_cfgs->ddr_mem_freq * 2);

	switch (pll_cfgs->ddr_mem_freq) {
	case 400: // data 800 pll 200
		dig_reg0 = 0x10000000;
		break;
	case 533: // data 1066 pll 266
		dig_reg0 = 0xC07B301;
		break;
	case 666: // data 1333 pll 333
		dig_reg0 = 0x99C0F60;
		break;
	case 800: // data 1600 pll 400
		dig_reg0 = 0x8000000;
		break;
	case 933: // data 1866 pll 466
		dig_reg0 = 0x6ddf09e;
		break;
	case 1200: // data 2400 pll 600
		dig_reg0 = 0x5555555;
		break;
	case 1333: // data 2666 pll 666
		dig_reg0 = 0x4ce07b0;
		break;
	case 1600: // data 3200 pll 800
		dig_reg0 = 0x4000000;
		break;
	case 1066: // data 2133 pll 533
		dig_reg0 = 0x600f5e9;
		break;
	case 1466: // data 2933 pll 733
		dig_reg0 = 0x45d9952;
		break;
	default: // data 2133 pll 533
		dig_reg0 = 0x600f5e9;
		break;
	}

#ifdef AR9301_SDRAM_DEBUG
	printf("`writel(0x%x, DDRPLL_DIG_REG0_ADDR);\n", dig_reg0);
	writel(dig_reg0, (volatile unsigned int *)DDRPLL_DIG_REG0_ADDR);
#else
	writel(dig_reg0, (volatile unsigned int *)DDRPLL_DIG_REG0_ADDR);
#endif

	while (!(readl(DDRPLL_DIG_REG_ADDR) & DDRPLL_DIG_REG_PLL_LOCK_MASK)) {
		if (timeout == 0) {
			pr_err("%s: DDR PLL never stabled.\n", __func__);
			return;
		}
		timeout--;
		udelay(10);
	}

//	printf("DDR PLL stabled.\n");
}


/*
 * The default behavior of this function is to print comments relating to this
 * process. A function call of the same name will be printed in the output text
 * file. The user can choose to leave this function as is, or implement
 * mechanism within this function to trigger start clock and reset events in
 * simulation.
 *
 * Following is one possible sequence to reset the PHY. Other sequences are also
 * possible. See section "Clocks, Reset, Initialization" of the PUB for other
 * possible reset sequences.
 *
 * -# Drive PwrOkIn to 0. Note: Reset, DfiClk, and APBCLK can be X.
 * -# Start DfiClk and APBCLK
 * -# Drive Reset to 1 and PRESETn_APB to 0.
 *    Note: The combination of PwrOkIn=0 and Reset=1 signals a cold reset to the PHY.
 * -# Wait a minimum of 8 cycles.
 * -# Drive PwrOkIn to 1. Once the PwrOkIn is asserted (and Reset is still asserted),
 *    DfiClk synchronously switches to any legal input frequency.
 * -# Wait a minimum of 64 cycles. Note: This is the reset period for the PHY.
 * -# Drive Reset to 0. Note: All DFI and APB inputs must be driven at valid
 *    reset states before the de-assertion of Reset.
 * -# Wait a minimum of 1 Cycle.
 * -# Drive PRESETn_APB to 1 to de-assert reset on the ABP bus.
 * -# The PHY is now in the reset state and is ready to accept APB transactions.
 *
 * \return Void
 */
void ddrphy_b_start_clk_rst_phy(struct artosyn_dram_info *dram)
{
	//##############################################################
	// Step (B) Start Clocks and Reset the PHY
	// See PhyInit App Note for detailed description and function usage
	//##############################################################
	user_input_basic_t *basic = &dram->userInputBasic;
	runtime_config_t *runtime = &dram->runtimeConfig;
	artosyn_dphy_pll_cfgs_t *pll_cfgs = &dram->pll_default_cfgs;

	printf("(B) Start Clocks and Reset the PHY\n");
	if ((warmboot && dram->warmboot_support) || runtime->skip_train)
		pll_cfgs->ddr_mem_freq = basic->Frequency[0];

	ddrphy_pll_config(pll_cfgs);
}

/**
 *  - Program TxSlewRate:
 *    - TxSlewRate::TxPreDrvMode is dependent on DramType.
 *    - TxSlewRate::TxPreP and TxSlewRate::TxPreN are technology-specific
 *      User should consult the "Output Slew Rate" section of
 *      HSpice Model App Note in specific technology for recommended settings
 */
void ddrphy_prg_txslewrate(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int lane;
	int b_addr;
	int byte;
	int c_addr;
	int pstate;
	int p_addr;
	int TxSlewRate[4];
	int TxPreP[4];		// Default to 0xf (max). Optimal setting is technology specific.
	int TxPreN[4];		// Default to 0xf (max). Optimal setting is technology specific.
	int TxPreDrvMode;

	TxPreDrvMode = 0x3;	// TxPreDrvMode[2] = 0

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		TxPreP[pstate] = advanced->TxSlewRiseDQ[pstate];
		TxPreN[pstate] = advanced->TxSlewFallDQ[pstate];

		TxSlewRate[pstate] = (TxPreDrvMode << csr_TxPreDrvMode_LSB) | (TxPreN[pstate] << csr_TxPreN_LSB) | (TxPreP[pstate] << csr_TxPreP_LSB);

		for (byte = 0; byte < basic->NumDbyte; byte++) {
			c_addr = byte << 12;
			for (lane = 0; lane <= b_max; lane++) {
				b_addr = lane << 8;
				ddrphy_io_write16(dram, (p_addr | tDBYTE | c_addr | b_addr | csr_TxSlewRate_ADDR), TxSlewRate[pstate]);
			}
		}
	}
}

/**
 * - Program ATxSlewRate:
 *  - ATxSlewRate::ATxPreDrvMode is dependent on DramType and whether
 *    the ACX4 instance is used for AC or CK
 *  - ATxSlewRate::ATxPreP and ATxSlewRate::TxPreN are technology-specific
 *    User should consult the "Output Slew Rate" section of
 *    HSpice Model App Note in specific technology for recommended settings
 */
void ddrphy_prg_atxslewrate(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int c_addr;
	int anib;
	int ATxSlewRate;
	int ATxPreP;		// Default to 0xf (max). Optimal setting is technology specific.
	int ATxPreN;		// Default to 0xf (max). Optimal setting is technology specific.
	int ATxPreDrvMode;
	int CK_ANIB_inst[2];
	int ATxDelay;
	int p_addr;

	ATxPreP = advanced->TxSlewRiseAC;
	ATxPreN = advanced->TxSlewFallAC;

	// # of ANIBs      CK ANIB Instance
	// ACX8            ANIB 1
	// ACX10           ANIB 4,5
	// ACX12           ANIB 4,5
	// ACX13           ANIB 4,5
	if (basic->NumAnib == 8) {
		CK_ANIB_inst[0] = 1;
		CK_ANIB_inst[1] = 1;
	} else if (basic->NumAnib == 10 || basic->NumAnib == 12 || basic->NumAnib == 13) {
		CK_ANIB_inst[0] = 4;
		CK_ANIB_inst[1] = 5;
	} else {
		pr_err("Invalid number of ANIBs for protocol.\n");
	}

	for (anib = 0; anib < basic->NumAnib; anib++) {
		c_addr = anib << 12;

		if (anib == CK_ANIB_inst[0] || anib == CK_ANIB_inst[1] ) {// CK ANIB instance
			ATxPreDrvMode = 0x0;
		} else {// non-CK ANIB instance
			ATxPreDrvMode = 0x3;
		}
		ATxSlewRate = (ATxPreDrvMode << csr_ATxPreDrvMode_LSB) | (ATxPreN << csr_ATxPreN_LSB) | (ATxPreP << csr_ATxPreP_LSB);
		ddrphy_io_write16(dram, (tANIB | c_addr | csr_ATxSlewRate_ADDR), ATxSlewRate);
		//Program delay for Atx for LPDDR2 since there is no CA training
		if (anib == CK_ANIB_inst[0] || anib == CK_ANIB_inst[1] ) {// CK ANIB instance	
			ATxDelay = 0x00;
			p_addr = 0x0 << 20;
			ddrphy_io_write16(dram, (p_addr | tANIB | c_addr | csr_ATxDly_ADDR), ATxDelay);
		}
	}
}

/**
 * - Program DfiRdDataCsDestMap and DfiWrDataCsDestMap
 *   - Dependencies:
 *     mb_DDR3U_1D[0].MsgMisc[6] Determine Partial Rank Support
 */
void ddrphy_prg_dfirddatacsdestmap_dfiwrdatacsdestmap(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	uint16_t DfiRdDataCsDestMap;
	uint16_t DfiWrDataCsDestMap;

	if ( (mb_DDR3U_1D[0].MsgMisc & 0x40) != 0) {
		DfiRdDataCsDestMap = 0xa0;
		DfiWrDataCsDestMap = 0xa0;
		for (pstate=0; pstate<basic->NumPStates; pstate++) {
			p_addr = pstate << 20;

			ddrphy_io_write16(dram,  (p_addr | tMASTER | csr_DfiRdDataCsDestMap_ADDR), DfiRdDataCsDestMap );
			ddrphy_io_write16(dram,  (p_addr | tMASTER | csr_DfiWrDataCsDestMap_ADDR), DfiWrDataCsDestMap );
		}
	}
}

/**
 * - Program PllCtrl2:
 *   - Calculate PLL controls per p-state from Frequency
 */
void ddrphy_prg_pllctrl2(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	int PllCtrl2[4];
	int PllCtrl1[4];

	for (pstate=0; pstate<basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		if (basic->Frequency[pstate]/2 < 235) {
			PllCtrl2[pstate] = 0x7;
		} else if(basic->Frequency[pstate]/2 < 313) {
			PllCtrl2[pstate] = 0x6;
		} else if(basic->Frequency[pstate]/2 < 469) {
			PllCtrl2[pstate] = 0xb;
		} else if(basic->Frequency[pstate]/2 < 625) {
			PllCtrl2[pstate] = 0xa;
		} else if(basic->Frequency[pstate]/2 < 938) {
			PllCtrl2[pstate] = 0x19;
		} else if(basic->Frequency[pstate]/2 < 1067) {
			PllCtrl2[pstate] = 0x18;
		} else {
			PllCtrl2[pstate] = 0x19;
		}

		ddrphy_io_write16(dram,  (p_addr | tMASTER | csr_PllCtrl2_ADDR), PllCtrl2[pstate] );

//#ifdef CONFIG_ARTOSYN_AR9311 for temperature issue
		if (basic->Frequency[pstate]/2 < 235) {
			PllCtrl1[pstate] = 0x80;
		} else if(basic->Frequency[pstate]/2 < 313) {
			PllCtrl1[pstate] = 0x60;
		} else if(basic->Frequency[pstate]/2 < 469) {
			PllCtrl1[pstate] = 0x80;
		} else if(basic->Frequency[pstate]/2 < 625) {
			PllCtrl1[pstate] = 0x60;
		} else if(basic->Frequency[pstate]/2 < 938) {
			PllCtrl1[pstate] = 0x80;
		} else if(basic->Frequency[pstate]/2 < 1067) {
			PllCtrl1[pstate] = 0x60;
		} else {
			PllCtrl1[pstate] = 0x80;
		}

		ddrphy_io_write16(dram,  (p_addr | tMASTER | csr_PllCtrl1_ADDR), PllCtrl1[pstate] );
//#endif
	}
}

/**
 * - Program ARdPtrInitVal:
 *   - The values programmed here assume ideal properties of DfiClk and Pclk including:
 *     - DfiClk skew
 *     - DfiClk jitter
 *     - DfiClk PVT variations
 *     - Pclk skew
 *     - Pclk jitter
 *
 *     ARdPtrInitVal Programmed differently based on PLL Bypass mode and Frequency:
 *   - PLL Bypassed mode:
 *     - For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 2-6
 *     - For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
 *   - PLL Enabled mode:
 *     - For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
 *     - For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 0-6
 */
void ddrphy_prg_ardptrinitval(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	int ARdPtrInitVal[4];

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		if (basic->Frequency[pstate] >= 933) {
			ARdPtrInitVal[pstate] = 0x2;
		} else {
			ARdPtrInitVal[pstate] = 0x1;
		}
		// Add one UI for synchronizer on SyncBus when PLL is bypassed
		if (basic->PllBypass[pstate] == 1) {
			ARdPtrInitVal[pstate] ++;
		}

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_ARdPtrInitVal_ADDR), ARdPtrInitVal[pstate]);
	}
}

/**
 * - Program DbyteDllModeCntrl:
 *   - DllRxPreambleMode
 * - Program DqsPreambleControl:
 *   - Fields:
 *     - TwoTckRxDqsPre
 *     - TwoTckTxDqsPre
 *     - PositionDfeInit
 *     - LP4TglTwoTckTxDqsPre
 *     - LP4PostambleExt
 *     - LP4SttcPreBridgeRxEn
 *   - Dependencies:
 *      - user_input_basic.DramType
 */
void ddrphy_prg_dbytedllmodecntrl_dqspreamblecontrol(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	int DqsPreambleControl;
	int TwoTckRxDqsPre[4];
	int TwoTckTxDqsPre=0;
	int PositionDfeInit;
	int LP4TglTwoTckTxDqsPre=0;
	int LP4PostambleExt=0;
	int LP4SttcPreBridgeRxEn=0;
	int WDQSEXTENSION=0;
	int DllRxPreambleMode=0;
	int DbyteDllModeCntrl;
	int DllLockParam;
	int DisDllSeedSel=0;
	int DisDllGainIVSeed=1;
	int LcdlSeed0=0x21;
	int DllGainCtl;
	int DllGainIV=0x1;
	int DllGainTV=0x6;

	for (pstate=0; pstate<basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		// LPDDR3 or DDR3 -- same as default
		TwoTckRxDqsPre[pstate]	= 0x0;
		LP4SttcPreBridgeRxEn	= 0x0;
		DllRxPreambleMode	= 0x0;
		TwoTckTxDqsPre		= 0x0;
		LP4TglTwoTckTxDqsPre	= 0x0;
		PositionDfeInit		= 0x0;
		LP4PostambleExt		= 0x0;

		DqsPreambleControl = (WDQSEXTENSION << csr_WDQSEXTENSION_LSB) | (LP4SttcPreBridgeRxEn << csr_LP4SttcPreBridgeRxEn_LSB) | (LP4PostambleExt << csr_LP4PostambleExt_LSB) | (LP4TglTwoTckTxDqsPre << csr_LP4TglTwoTckTxDqsPre_LSB) | (PositionDfeInit << csr_PositionDfeInit_LSB) | (TwoTckTxDqsPre << csr_TwoTckTxDqsPre_LSB) | (TwoTckRxDqsPre[pstate] << csr_TwoTckRxDqsPre_LSB);

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_DqsPreambleControl_ADDR), DqsPreambleControl);

		DbyteDllModeCntrl = DllRxPreambleMode << csr_DllRxPreambleMode_LSB;
		ddrphy_io_write16(dram, (tMASTER | csr_DbyteDllModeCntrl_ADDR), DbyteDllModeCntrl);

		DllGainCtl = DllGainIV | (DllGainTV<<csr_DllGainTV_LSB);
		DllLockParam =  DisDllSeedSel |  (DisDllGainIVSeed<<csr_DisDllGainIVSeed_LSB)|(LcdlSeed0<<csr_LcdlSeed0_LSB);

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_DllLockParam_ADDR), DllLockParam);
		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_DllGainCtl_ADDR), DllGainCtl);
	}
}

/**
 * - Program ProcOdtTimeCtl:
 *   - Fields:
 *     - POdtStartDelay[3:2]
 *     - POdtTailWidth[1:0]
 *   - Dependencies:
 *     - user_input_basic.Frequency
 *     - user_input_basic.DramType
 */
void ddrphy_prg_procodttimectl(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int pstate;
	int p_addr;
	int ProcOdtTimeCtl[4];

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		if (advanced->WDQSExt) {
			// POdtStartDelay = 0x0
			// POdtTailWidth  = 0x3
			ProcOdtTimeCtl[pstate] = 0x3;
		} else if (basic->Frequency[pstate] <= 933) {
			// Memclk Freq <= 933MHz
			// POdtStartDelay = 0x2
			// POdtTailWidth  = 0x2
			ProcOdtTimeCtl[pstate] = 0xa;
		} else if (basic->Frequency[pstate] <= 1200) {
			// 933MHz < Memclk Freq <= 1200MHz
			if (advanced->D4RxPreambleLength[pstate] == 1) {
				// POdtStartDelay = 0x0
				// POdtTailWidth  = 0x2
				ProcOdtTimeCtl[pstate] = 0x2;
			} else {
				// POdtStartDelay = 0x1
				// POdtTailWidth  = 0x2
				ProcOdtTimeCtl[pstate] = 0x6;
			}
		} else {
			// Memclk Freq > 1200MHz
			if (advanced->D4RxPreambleLength[pstate] == 1) {
				// POdtStartDelay = 0x0
				// POdtTailWidth  = 0x3
				ProcOdtTimeCtl[pstate] = 0x3;
			} else {
				// POdtStartDelay = 0x1
				// POdtTailWidth  = 0x3
				ProcOdtTimeCtl[pstate] = 0x7;
			}
		}
		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_ProcOdtTimeCtl_ADDR), ProcOdtTimeCtl[pstate]);
	}
}

/** \brief maps impedance values to register settings
 *
 *  Reads the pull-up/pull-down driver impedance from DrvStren_ohm input
 *  and encodes that value for the CSR field specified in TargetCSR input,
 *  based on DDR protocol and user_input_basic.HardMacroVer.
 *
 *  @param[in] DrvStren_ohm drive strenght / ODT impedance in Ohms
 *
 *  @param[in] TargetCSR Target CSR for the impedance value. on of following
 *  enum DrvType:
 *    - DrvStrenFSDqP
 *    - DrvStrenFSDqN
 *    - ODTStrenP
 *    - ODTStrenN
 *    - ADrvStren
 *    - ADrvStrenN
 *
 *  @param[in] HardMacroVer Hardmacro version. should be same as
 *  user_input_basic.HardMacroVer
 */
int ddrphy_map_drv_stren(int DrvStren_ohm, enum DrvType TargetCSR, int HardMacroVer)
{
	int Stren_setting;

	// String array which follows the order of the DrvType enum. When debugging this array is used to know which TargetCSR has been selected.
	// char debugDrvType[6][14] = {"DrvStrenFSDqP", "DrvStrenFSDqN", "ODTStrenP", "ODTStrenN", "ADrvStrenP", "ADrvStrenN"};

	//---------------------------------------------------
	//Other HardMacro family
	//---------------------------------------------------
	if ((TargetCSR == DrvStrenFSDqP) ||
	    (TargetCSR == DrvStrenFSDqN)) {
		//...........................................
		// DrvStrenFSDqP or DrvStrenFSDqP
		//...........................................
		if (DrvStren_ohm == 0 )
			Stren_setting = 0x00;	// High-impedance
		else if (DrvStren_ohm < 29)
			Stren_setting = 0x3f;
		else if (DrvStren_ohm < 31)
			Stren_setting = 0x3e;
		else if (DrvStren_ohm < 33)
			Stren_setting = 0x3b;
		else if (DrvStren_ohm < 35)
			Stren_setting = 0x3a;
		else if (DrvStren_ohm < 38)
			Stren_setting = 0x39;
		else if (DrvStren_ohm < 41)
			Stren_setting = 0x38;
		else if (DrvStren_ohm < 45)
			Stren_setting = 0x1b;
		else if (DrvStren_ohm < 50)
			Stren_setting = 0x1a;
		else if (DrvStren_ohm < 56)
			Stren_setting = 0x19;
		else if (DrvStren_ohm < 64)
			Stren_setting = 0x18;
		else if (DrvStren_ohm < 74)
			Stren_setting = 0x0b;
		else if (DrvStren_ohm < 88)
			Stren_setting = 0x0a;
		else if (DrvStren_ohm < 108)
			Stren_setting = 0x09;
		else if (DrvStren_ohm < 140)
			Stren_setting = 0x08;
		else if (DrvStren_ohm < 200)
			Stren_setting = 0x03;
		else if (DrvStren_ohm < 360)
			Stren_setting = 0x02;
		else if (DrvStren_ohm < 481)
			Stren_setting = 0x01;
		else
			Stren_setting = 0x00;	// High-impedance
	} else if (TargetCSR == ODTStrenP) {
		//.........................................
		// ODTStrenP
		//.........................................
		// DDR4 & LPDDR3 - P is non-zero
		if (DrvStren_ohm == 0)
			Stren_setting = 0x00;	// High-impedance
		else if (DrvStren_ohm < 15)
			Stren_setting = 0x3f;
		else if (DrvStren_ohm < 16)
			Stren_setting = 0x3e;
		else if (DrvStren_ohm < 17)
			Stren_setting = 0x3b;
		else if (DrvStren_ohm < 18)
			Stren_setting = 0x3a;
		else if (DrvStren_ohm < 20)
			Stren_setting = 0x39;
		else if (DrvStren_ohm < 21)
			Stren_setting = 0x38;
		else if (DrvStren_ohm < 23)
			Stren_setting = 0x1b;
		else if (DrvStren_ohm < 26)
			Stren_setting = 0x1a;
		else if (DrvStren_ohm < 29)
			Stren_setting = 0x19;
		else if (DrvStren_ohm < 33)
			Stren_setting = 0x18;
		else if (DrvStren_ohm < 38)
			Stren_setting = 0x0b;
		else if (DrvStren_ohm < 45)
			Stren_setting = 0x0a;
		else if (DrvStren_ohm < 55)
			Stren_setting = 0x09;
		else if (DrvStren_ohm < 71)
			Stren_setting = 0x08;
		else if (DrvStren_ohm < 101)
			Stren_setting = 0x03;
		else if (DrvStren_ohm < 181)
			Stren_setting = 0x02;
		else if (DrvStren_ohm < 241)
			Stren_setting = 0x01;
		else
			Stren_setting = 0x00;	// High-impedance
	} else if (TargetCSR == ODTStrenN) {
		//..............................................
		// ODTStrenN
		//..............................................

		// DDR3 - P and N has the same impedance and non-zero
		// Times 2 of user input because of parallel pull-up and pull-down termination

		if (DrvStren_ohm == 0)
			Stren_setting = 0x00; // High-impedance
		else if (DrvStren_ohm < 15)
			Stren_setting = 0x3f;
		else if (DrvStren_ohm < 16)
			Stren_setting = 0x3e;
		else if (DrvStren_ohm < 17)
			Stren_setting = 0x3b;
		else if (DrvStren_ohm < 18)
			Stren_setting = 0x3a;
		else if (DrvStren_ohm < 20)
			Stren_setting = 0x39;
		else if (DrvStren_ohm < 21)
			Stren_setting = 0x38;
		else if (DrvStren_ohm < 23)
			Stren_setting = 0x1b;
		else if (DrvStren_ohm < 26)
			Stren_setting = 0x1a;
		else if (DrvStren_ohm < 29)
			Stren_setting = 0x19;
		else if (DrvStren_ohm < 33)
			Stren_setting = 0x18;
		else if (DrvStren_ohm < 38)
			Stren_setting = 0x0b;
		else if (DrvStren_ohm < 45)
			Stren_setting = 0x0a;
		else if (DrvStren_ohm < 55)
			Stren_setting = 0x09;
		else if (DrvStren_ohm < 71)
			Stren_setting = 0x08;
		else if (DrvStren_ohm < 101)
			Stren_setting = 0x03;
		else if (DrvStren_ohm < 181)
			Stren_setting = 0x02;
		else if (DrvStren_ohm < 241)
			Stren_setting = 0x01;
		else
			Stren_setting = 0x00; // High-impedance
	} else if ((TargetCSR == ADrvStrenP) ||
		   (TargetCSR == ADrvStrenN)) {
		//..............................................
		// ADrvStrenP or ADrvStrenN
		//..............................................
		if (DrvStren_ohm == 120)
			Stren_setting = 0x00;
		else if (DrvStren_ohm == 60)
			Stren_setting = 0x01;
		else if (DrvStren_ohm == 40)
			Stren_setting = 0x03;
		else if (DrvStren_ohm == 30)
			Stren_setting = 0x07;
		else if (DrvStren_ohm == 24)
			Stren_setting = 0x0F;
		else if (DrvStren_ohm == 20)
			Stren_setting = 0x1F;
		else {
			/* userInputAdvanced.ATxImpedance 0x%x \
			 * Ohms value is not valid.\n",
			 * DrvStren_ohm);
			 */
			pr_err("ATxImpedance is not valid\n");
		}
	} else {
		/* pr_err("TargetCSR = %s (%d), in \
		 *	ddrphy_map_drv_stren, is not \
		 *	valid.\n", debugDrvType[TargetCSR], TargetCSR);
		 */
		pr_err("TargetCSR is not valid\n");
	}

	return Stren_setting;
}

/**
 * - Program TxOdtDrvStren:
 *   - Fields:
 *     - ODTStrenP_px[5:0]
 *     - ODTStrenN_px[11:6]
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.HardMacroVer
 *     - user_input_advanced.ODTImpedance
 */
void ddrphy_prg_txodtdrvstren(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int lane;
	int b_addr;
	int byte;
	int c_addr;
	int pstate;
	int p_addr;
	int TxOdtDrvStren[4];
	int ODTStrenP_state[4];
	int ODTStrenN_state[4];

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		ODTStrenP_state[pstate] = ddrphy_map_drv_stren((int)advanced->ODTImpedance[pstate], ODTStrenP, basic->HardMacroVer);
		ODTStrenN_state[pstate] = ddrphy_map_drv_stren((int)advanced->ODTImpedance[pstate], ODTStrenN, basic->HardMacroVer);
		TxOdtDrvStren[pstate] = (ODTStrenN_state[pstate] << csr_ODTStrenN_LSB) | ODTStrenP_state[pstate];

		for (byte=0; byte<basic->NumDbyte; byte++) {
			c_addr = byte << 12;
			for (lane=0; lane <= b_max ; lane++) {
				b_addr = lane << 8;
				ddrphy_io_write16(dram, (p_addr | tDBYTE | c_addr | b_addr | csr_TxOdtDrvStren_ADDR), TxOdtDrvStren[pstate]);
			}
		}
	}
}

/**
 * - Program TxImpedanceCtrl1:
 *   - Fields:
 *     - DrvStrenFSDqP[5:0]
 *     - DrvStrenFSDqN[11:6]
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.HardMacroVer
 *     - user_input_advanced.TxImpedance
 */
void ddrphy_prg_tximpedancectrl1(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int lane;
	int b_addr;
	int byte;
	int c_addr;
	int pstate;
	int p_addr;
	int TxImpedanceCtrl1[4];
	int DrvStrenFSDqP_state[4];
	int DrvStrenFSDqN_state[4];

	for (pstate=0; pstate<basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		DrvStrenFSDqP_state[pstate] = ddrphy_map_drv_stren((int)advanced->TxImpedance[pstate], DrvStrenFSDqP, basic->HardMacroVer);
		DrvStrenFSDqN_state[pstate] = ddrphy_map_drv_stren((int)advanced->TxImpedance[pstate], DrvStrenFSDqN, basic->HardMacroVer);

		TxImpedanceCtrl1[pstate] = (DrvStrenFSDqN_state[pstate] << csr_DrvStrenFSDqN_LSB) | (DrvStrenFSDqP_state[pstate] << csr_DrvStrenFSDqP_LSB);

		for (byte=0; byte<basic->NumDbyte; byte++) {
			c_addr = byte << 12;
			for (lane=0; lane <= b_max ; lane++) {
				b_addr = lane << 8;
				ddrphy_io_write16(dram, (p_addr | tDBYTE | c_addr | b_addr | csr_TxImpedanceCtrl1_ADDR), TxImpedanceCtrl1[pstate]);
			}
		}
	}
}

/**
 * - Program ATxImpedance:
 *   - Fields:
 *     - ADrvStrenP[4:0]
 *     - ADrvStrenN[9:5]
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.HardMacroVer
 *     - user_input_advanced.ATxImpedance
 */
void ddrphy_prg_atximpedance(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int c_addr;
	int anib;
	int ATxImpedance;
	int ADrvStrenP_state;
	int ADrvStrenN_state;
#if 0
	// In hardmacro family E, the default 20 Ohm value is invalid
	// override de default value to be 40ohm
	if (basic->HardMacroVer == 4) {
		if (advanced->ATxImpedance == 20) {
			// In Hardmacro family E, maximum drive strength
			// is 40 Ohms
			pr_err("Overriding ATxImpedance to 40.\n");
			advanced->ATxImpedance = 40;
		}
	}
#endif
	ADrvStrenP_state = ddrphy_map_drv_stren((int)advanced->ATxImpedance, ADrvStrenP, basic->HardMacroVer);
	ADrvStrenN_state = ddrphy_map_drv_stren((int)advanced->ATxImpedance, ADrvStrenN, basic->HardMacroVer);

	ATxImpedance = (ADrvStrenN_state << csr_ADrvStrenN_LSB) | (ADrvStrenP_state << csr_ADrvStrenP_LSB);

	for (anib = 0; anib < basic->NumAnib; anib++) {
		c_addr = anib << 12;
		ddrphy_io_write16(dram, (tANIB | c_addr | csr_ATxImpedance_ADDR), ATxImpedance);
	}
}

/**
 * - Program DfiMode:
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.Dfi1Exists
 *     - user_input_basic.NumActiveDbyteDfi0
 *     - user_input_basic.NumActiveDbyteDfi1
 *     - user_input_basic.NumAnib
 */
void ddrphy_prg_dfimode(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int DfiMode;

	if (basic->Dfi1Exists == 1) {
		DfiMode = 0x5;	// DFI1 exists but disabled; DFI0 controls entire PHY
	} else {
		DfiMode = 0x1;	// DFI1 does not physically exists
	}

	ddrphy_io_write16(dram, (tMASTER | csr_DfiMode_ADDR), DfiMode);
}

/**
 * - Program DfiCAMode:
 *   - Fields:
 *     - DfiLp3CAMode
 *     - DfiD4CAMode
 *     - DfiLp4CAMode
 *     - DfiD4AltCAMode
 *   - Dependencies:
 *     - user_input_basic.DramType
 */
void ddrphy_prg_dficamode(struct artosyn_dram_info *dram)
{
	int DfiCAMode=0;

	// DDR3
	DfiCAMode = 0;

	ddrphy_io_write16(dram, (tMASTER | csr_DfiCAMode_ADDR), DfiCAMode);
}

/**
 * - Program CalDrvStr0:
 *   - Fields:
 *     - CalDrvStrPd50[3:0]
 *     - CalDrvStrPu50[7:4]
 *   - Dependencies:
 *     - user_input_advanced.ExtCalResVal
 */
void ddrphy_prg_caldrvstr0(struct artosyn_dram_info *dram)
{
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	uint16_t CalDrvStr0;
	uint16_t CalDrvStrPd50;
	uint16_t CalDrvStrPu50;

#if 0
	if (basic->HardMacroVer == 4) {
		switch (advanced->ExtCalResVal) {
		case 0x1: // 120
			CalDrvStrPu50 = 0x1;
			break;
		case 0x2: // 40
			CalDrvStrPu50 = 0x2;
			break;
		default:
			CalDrvStrPu50 = 0x0;
			pr_err("ExtCalResVal is not supported in HME.\n");
		}
	} else {
		CalDrvStrPu50 = advanced->ExtCalResVal;
	}
#else
	CalDrvStrPu50 = advanced->ExtCalResVal;
#endif

	CalDrvStrPd50 = CalDrvStrPu50;
	CalDrvStr0 = (CalDrvStrPu50 << csr_CalDrvStrPu50_LSB) | CalDrvStrPd50;

	ddrphy_io_write16(dram, (tMASTER | csr_CalDrvStr0_ADDR), CalDrvStr0);
}

/**
 * - Program CalVRefs:
 *   - Impedance calibration VRef for Hard Macro E.
 *   - Dependencies:
 *     - user_input_basic.HardMacro
 */
void ddrphy_prg_calvrefs(struct artosyn_dram_info *dram)
{
#if 0
	user_input_basic_t *basic = &dram->userInputBasic;
	uint16_t CalVRefs;

	if (basic->HardMacroVer == 4) {
		CalVRefs = 0x2;
		ddrphy_io_write16(dram, (tMASTER | csr_CalVRefs_ADDR), CalVRefs);
	}
#endif
}

#define ceil(x, y)	({ ulong __x = (x), __y = (y); (__x + __y - 1) / __y; })
/**
 * - Program CalUclkInfo:
 *   - Impedance calibration CLK Counter.
 *   - Fields:
 *     - CalUClkTicksPer1uS
 *   - Dependencies:
 *     - user_input_basic.Frequency
 */
void ddrphy_prg_caluclkinfo(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	double CalUClkTicksPer1uS_double[4];
	uint16_t CalUClkTicksPer1uS[4];

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		// Number of DfiClk cycles per 1us
		CalUClkTicksPer1uS_double[pstate] = (double)(1.0 * basic->Frequency[pstate]) / 2.0;
		CalUClkTicksPer1uS[pstate] = (int)ceil(CalUClkTicksPer1uS_double[pstate], 1);

		// Minimum value of CalUClkTicksPer1uS = 24
		if (CalUClkTicksPer1uS[pstate] < 24)
			CalUClkTicksPer1uS[pstate] = 24;

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_CalUclkInfo_ADDR), CalUClkTicksPer1uS[pstate]);
	}
}

//##############################################################
//
// Program Calibration CSRs based on user input
//
// CSRs to program:
//      CalRate:: CalInterval
//             :: CalOnce
//
// User input dependencies::
//      CalInterval
//      CalOnce
//
//##############################################################
void ddrphy_prg_calibration_csrs(struct artosyn_dram_info *dram)
{
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int CalRate;
	int CalInterval;
	int CalOnce;

	CalInterval = advanced->CalInterval;
	CalOnce = advanced->CalOnce;

	CalRate = (CalOnce << csr_CalOnce_LSB) | (CalInterval << csr_CalInterval_LSB);

	ddrphy_io_write16(dram, (tMASTER | csr_CalRate_ADDR), CalRate);
}

/**
 * - Program VrefInGlobal:
 *   - DqDqsRcvCntrl and csrVrefInGlobal to select Global VREF
 *     from Master to be used in each DQ
 *   - Fields:
 *     - GlobalVrefInSel: Select Range of GlobalVref DAC. Default: set to 1.
 *     - GlobalVrefInDAC: Vref level is set based on mb_DDR3U_1D[pstate].PhyVref value.
 *       The following formula is used to convert the PhyVref into the register setting.
 *       \f{eqnarray*}{
 *           PhyVrefPrcnt &=& \frac{mb DDR3U 1D[pstate].PhyVref}{128} \\
 *        if GlobalVrefInSel = 1 :
 *           GlobalVrefInDAC &=& 1+\frac{PhyVrefPrcnt}{0.005} \\
 *        if GlobalVrefInSel = 0 :
 *           GlobalVrefInDAC &=& \frac{(PhyVrefPrcnt-0.345)}{0.005} \\
 *           RxVref &=& (GlobalVrefInDAC == 0) ? Hi-Z : (PhyVrefPrcnt \times VDDQ)
 *        \f}
 *   - Dependencies:
 *     - user_input_basic.DramType
 *
 * - Program DqDqsRcvCntrl:
 *   - DqDqsRcvCntrl and csrVrefInGlobal to select Global VREF
 *     from Master to be used in each DQ
 *   - Fields:
 *    - SelAnalogVref
 *    - MajorModeDbyte
 *    - ExtVrefRange
 *    - DfeCtrl
 *    - GainCurrAdj
 *   - Dependencies:
 *     - user_input_basic.DramType
 */
void ddrphy_prg_vrefinglobal(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int lane;
	int b_addr;
	int byte;
	int c_addr;
	int pstate;
	int p_addr;
	int VrefInGlobal;
	int DqDqsRcvCntrl;
	int SelAnalogVref = 1; // Use Global VREF from Master
	int MajorModeDbyte = 0;
	int ExtVrefRange_defval = 0;
	int DfeCtrl_defval = 0;
	int GainCurrAdj_defval = 0xb;

	// DDR3
	MajorModeDbyte = 0;

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		int32_t vref_percentVddq = (mb_DDR3U_1D[pstate].PhyVref) * 1000 * 100 / 128;
		uint8_t GlobalVrefInDAC = 0x0;
		uint8_t GlobalVrefInSel = 0x4;

		p_addr = pstate << 20;
		// check range1 first. Only use range0 if customer input maxes out range1
		GlobalVrefInDAC = (uint8_t)((vref_percentVddq / 500) + 1);		// Min value is 1
		if (GlobalVrefInDAC > 127) {
			GlobalVrefInDAC = (uint8_t)(max((vref_percentVddq - 34500), 500) / 500); // Min value is 1
			GlobalVrefInSel = 0x0;
		}
		GlobalVrefInDAC = min(GlobalVrefInDAC, (uint8_t)127);

		//GlobalVrefInSel |= 3;

		VrefInGlobal = (GlobalVrefInDAC << csr_GlobalVrefInDAC_LSB) | GlobalVrefInSel;
		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_VrefInGlobal_ADDR), VrefInGlobal);

		DqDqsRcvCntrl = (GainCurrAdj_defval << csr_GainCurrAdj_LSB) | (MajorModeDbyte << csr_MajorModeDbyte_LSB) | (DfeCtrl_defval << csr_DfeCtrl_LSB) | (ExtVrefRange_defval << csr_ExtVrefRange_LSB) | (SelAnalogVref << csr_SelAnalogVref_LSB);

		for (byte=0; byte<basic->NumDbyte; byte++) {
			c_addr = byte << 12;
			for (lane=0; lane <= b_max ; lane++) {
				b_addr = lane << 8;
				ddrphy_io_write16(dram, (p_addr | tDBYTE | c_addr | b_addr | csr_DqDqsRcvCntrl_ADDR), DqDqsRcvCntrl);
			}
		}
	}
}

/**
 * - Program MemAlertControl and MemAlertControl2:
 *   - Fields:
 *     - MALERTVrefLevel
 *     - MALERTPuStren
 *     - MALERTPuEn
 *     - MALERTRxEn
 *     - MALERTSyncBypass
 *     - MALERTDisableVal
 *   - Dependencies:
 *     - user_input_advanced.MemAlertEn
 *     - user_input_advanced.MemAlertPUImp
 *     - user_input_advanced.MemAlertVrefLevel
 */
void ddrphy_prg_memalertcontrol_memalertcontrol2(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int MemAlertControl;
	int MemAlertControl2;
	int MALERTPuEn;
	int MALERTRxEn;
	int MALERTVrefLevel;
	int MALERTPuStren;
	int MALERTSyncBypass;
	int MALERTDisableVal_defval = 1;

	// MemAlert applies to DDR4(all DIMM) or DDR3(RDIMM) only
	if ((basic->DramType == DDR4 || (basic->DramType == DDR3 && basic->DimmType == RDIMM)) && advanced->MemAlertEn == 1) {
		MALERTPuEn = 1;
		MALERTRxEn = 1;
		MALERTPuStren = advanced->MemAlertPUImp;
		MALERTVrefLevel = advanced->MemAlertVrefLevel;
		MALERTSyncBypass = advanced->MemAlertSyncBypass;

		MemAlertControl = (MALERTDisableVal_defval << 14) | (MALERTRxEn << 13) | (MALERTPuEn << 12) | (MALERTPuStren << 8) | MALERTVrefLevel;
		MemAlertControl2 = (MALERTSyncBypass << csr_MALERTSyncBypass_LSB);

		ddrphy_io_write16(dram, (tMASTER | csr_MemAlertControl_ADDR), MemAlertControl);
		ddrphy_io_write16(dram, (tMASTER | csr_MemAlertControl2_ADDR), MemAlertControl2);
	}
}

/**
 * - Program DfiFreqRatio :
 *   - Dependencies:
 *     - user_input_basic.DfiFreqRatio
 */
void ddrphy_prg_dfifreqratio(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	int p_addr;
	int DfiFreqRatio[4];

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		DfiFreqRatio[pstate] = basic->DfiFreqRatio[pstate];

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_DfiFreqRatio_ADDR), DfiFreqRatio[pstate]);
	}
}

/**
 * - Program TristateModeCA based on DramType and 2T Timing
 *   - Fields:
 *     - CkDisVal
 *     - DisDynAdrTri
 *     - DDR2TMode
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_advanced.Is2Ttiming
 *     - user_input_advanced.DisDynAdrTri
 */
void ddrphy_prg_tristatemodeca(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int pstate;
	int p_addr;
	int TristateModeCA[4];
	int DisDynAdrTri[4];
	int DDR2TMode[4];
	int CkDisVal_def;

	// CkDisVal depends on DramType
	// DDR4 or DDR3
	CkDisVal_def = 1; // {CLK_t,CLK_c} = 2'b00;

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;

		DisDynAdrTri[pstate] = advanced->DisDynAdrTri[pstate];

		DDR2TMode[pstate] = advanced->Is2Ttiming[pstate];

		TristateModeCA[pstate] = (CkDisVal_def << csr_CkDisVal_LSB) | (DDR2TMode[pstate] << csr_DDR2TMode_LSB) | (DisDynAdrTri[pstate] << csr_DisDynAdrTri_LSB) ;

		ddrphy_io_write16(dram, (p_addr | tMASTER | csr_TristateModeCA_ADDR), TristateModeCA[pstate]);
	}
}

/**
 * - Program DfiXlat based on Pll Bypass Input
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.PllBypass
 */
void ddrphy_prg_dfixlat(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int pstate;
	uint16_t loopVector;
	uint16_t dfifreqxlat_dat;
	uint16_t pllbypass_dat;
	uint16_t skipddc_dat=0; // set to vector offset based on Pstate frequency to disable dram drift compensation.

	pllbypass_dat = (basic->PllBypass[3] << 12) |
			(basic->PllBypass[2]<< 8) |
			(basic->PllBypass[1]<< 4) |
			(basic->PllBypass[0]);

	for (pstate = 0; pstate <= 3; pstate++) {
		if (basic->Frequency[pstate] < 333)
			skipddc_dat = skipddc_dat | (0x5 << 4 * pstate);
	}

	for(loopVector=0; loopVector<8; loopVector++) {
		if (loopVector == 0 ) { // Relock DfiFreq = 00,01,02,03)  Use StartVec 5 (pll_enabled) or StartVec 6 (pll_bypassed)
			dfifreqxlat_dat = pllbypass_dat + 0x5555;
			//dwc_ddrphy_phyinit_cmnt("%s:DEBUG: D4 : loopVector = %d : dfifreqxlat_dat = %x.\n", printf_header, loopVector, dfifreqxlat_dat);
			ddrphy_io_write16(dram, (c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), dfifreqxlat_dat);
		} else if (loopVector == 7) { // LP3-entry DfiFreq = 1F
			ddrphy_io_write16(dram, (c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0xF000);
		} else { // everything else = skip retrain  (could also map to 0000 since retrain code is excluded, but this is cleaner)
			ddrphy_io_write16(dram, (c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0x5555);
		}
	}
}

/**
 * - Program DqDqsRcvCntrl1 (Receiver Powerdown) and DbyteMiscMode
 *   - see function ddrphy_is_dbyte_disabled() to determine
 *     which DBytes are turned off completely based on PHY configuration.
 *   - Fields:
 *     - DByteDisable
 *     - PowerDownRcvr
 *     - PowerDownRcvrDqs
 *     - RxPadStandbyEn
 *   - Dependencies:
 *     - user_input_basic.DramType
 *     - user_input_basic.Dfi1Exists
 *     - user_input_basic.NumActiveDbyteDfi0
 *     - user_input_basic.NumActiveDbyteDfi1
 *     - user_input_basic.NumDbyte
 *     - user_input_basic.DramDataWidth
 */
void ddrphy_prg_dqdqsrcvcntrl1_dbytemiscmode_a(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int c_addr;
	uint16_t regData;
	regData = 0x1 << csr_DByteDisable_LSB;
	unsigned int regData1;
	regData1 = (0x1ff << csr_PowerDownRcvr_LSB | 0x1 << csr_PowerDownRcvrDqs_LSB | 0x1 << csr_RxPadStandbyEn_LSB) ;
	unsigned int regData2; // turn off Rx of DBI lane
	regData2 = (0x100 << csr_PowerDownRcvr_LSB | csr_RxPadStandbyEn_MASK) ;

	int d;
	// Implements Section 1.3 of Pub Databook
	for (d = 0; d < basic->NumDbyte; d++)  // for each dbyte
	{
		c_addr = d*c1;
		if (ddrphy_is_dbyte_disabled(d, dram)) {
			ddrphy_io_write16(dram, (c_addr | tDBYTE | csr_DbyteMiscMode_ADDR), regData);
			ddrphy_io_write16(dram, (c_addr | tDBYTE | csr_DqDqsRcvCntrl1_ADDR), regData1);
		} else {
			// disable RDBI lane if not used.
			if (basic->DramDataWidth != 4) {
				ddrphy_io_write16(dram, (c_addr | tDBYTE | csr_DqDqsRcvCntrl1_ADDR), regData2);
			} // DBI
		} // DbyteDisable.
	} // for each dbyte
}

/**
 * - Program DqDqsRcvCntrl1 (Receiver Powerdown) and DbyteMiscMode
 *   - Fields:
 *     - X4TG
 *     - MasterX4Config
 *   - Dependencies:
 *     - user_input_basic.DramDataWidth
 *
 * \note PHY does not support mixed dram device data width
 */
void ddrphy_prg_dqdqsrcvcntrl1_dbytemiscmode_b(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	int X4TG;
	int MasterX4Config;

	if (basic->DramDataWidth == 4)
		X4TG = 0xf;
	else
		X4TG = 0x0;

	MasterX4Config = X4TG << csr_X4TG_LSB;

	ddrphy_io_write16(dram, (tMASTER | csr_MasterX4Config_ADDR), MasterX4Config);
}

/**
 * - Program DMIPinPresent based on DramType and Read-DBI enable
 *   - Fields:
 *     - RdDbiEnabled
 *   - Dependencies:
 *     - user_input_basic.DramDataWidth
 *     - mb_DDR3U_1D[0].MR5
 */
void ddrphy_prg_dmipinpresent_readdbienable(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int c_addr;
	int anib;

	uint16_t AForceTriCont=0x0;
	uint16_t Acx4AnibDis= 0x0;
	int NumRank = basic->NumRank_dfi0 + basic->NumRank_dfi1;

	for (anib = 0; (anib < basic->NumAnib) && advanced->DisableUnusedAddrLns; anib++)
	{
		c_addr = anib << 12;
		AForceTriCont=0x0;
		if (basic->NumAnib == 12) {
			if (anib == 0 && NumRank == 1 )
				AForceTriCont = 0xe;
			else if (anib == 0 && NumRank == 2 )
				AForceTriCont = 0xc;
			else if (anib == 0 && NumRank == 3 )
				AForceTriCont = 0x8;
			else if (anib == 0 && NumRank == 4 )
				AForceTriCont = 0x0;
			else if (anib == 4 && NumRank < 3 )
				AForceTriCont = 0xc;
			else if (anib == 5 && NumRank == 1 )
				AForceTriCont = 0xf;
			else if (anib == 5 && NumRank < 4 )
				AForceTriCont = 0xc;
			else if (anib == 8 )
				AForceTriCont = 0x1; // MTEST
			else if (anib == 9 && NumRank < 3)
				AForceTriCont = 0xf;
			else if (anib == 9 && NumRank == 3)
				AForceTriCont = 0xe;
			else if (anib == 9 && NumRank == 4)
				AForceTriCont = 0xc;
			else if (anib == 10 && NumRank == 1 )
				AForceTriCont = 0xe;
			else if (anib == 10 && NumRank == 2 )
				AForceTriCont = 0xc;
			else if (anib == 10 && NumRank == 3 )
				AForceTriCont = 0x8;
			else if (anib == 10 && NumRank == 4 )
				AForceTriCont = 0x0;
			else if (anib == 11 && NumRank == 1 )
				AForceTriCont = 0xe;
			else if (anib == 11 && NumRank == 2 )
				AForceTriCont = 0xc;
			else if (anib == 11 && NumRank == 3 )
				AForceTriCont = 0x8;
		} else if (basic->NumAnib == 10) {
			if (anib == 0 && NumRank == 1 && basic->DimmType == UDIMM)
				AForceTriCont = 0xa;
			else if (anib == 0 && NumRank == 2 && basic->DimmType == UDIMM)
				AForceTriCont = 0x8;
			else if (anib == 0 && NumRank == 1 && basic->DimmType != UDIMM)
				AForceTriCont = 0x2;
			else if (anib == 4 )
				AForceTriCont = 0xc;
			else if (anib == 5 && NumRank == 1 )
				AForceTriCont = 0xf;
			else if (anib == 5 && NumRank == 2 )
				AForceTriCont = 0xc;
			else if (anib == 8 && basic->DimmType == UDIMM )
				AForceTriCont = 0x1;
			else if (anib == 9 && NumRank == 1 && basic->DimmType == UDIMM)
				AForceTriCont = 0xe;
			else if (anib == 9 && NumRank == 2 && basic->DimmType == UDIMM)
				AForceTriCont = 0x8;
			else if (anib == 9 && NumRank == 1 && basic->DimmType != UDIMM)
				AForceTriCont = 0x6;
		} else if (basic->NumAnib == 8) {
			if (anib == 0 && NumRank == 1)
				AForceTriCont = 0x2;
			else if (anib == 1 && NumRank == 1)
				AForceTriCont = 0xc;
			else if (anib == 6)
				AForceTriCont = 0x1;
		}

		// if all the lanes can be disabled, and Anib is not the first or last disable entire chiplet
		if ( AForceTriCont == 0xf && anib !=0 && anib !=basic->NumAnib-1) Acx4AnibDis = Acx4AnibDis | (0x1 << anib);
			ddrphy_io_write16(dram, ( tANIB | c_addr | csr_AForceTriCont_ADDR), AForceTriCont);
	}
	ddrphy_io_write16(dram, ( tMASTER |  csr_Acx4AnibDis_ADDR), Acx4AnibDis);
}

/*
 * This function programs majority of PHY configuration registers based
 * on data input into PhyInit data structures.
 *
 * This function programs PHY configuration registers based on information
 * provided in the PhyInit data structures (userInputBasic, userInputAdvanced).
 * The user can overwrite the programming of this function by modifying
 * dwc_ddrphy_phyinit_userCustom_customPreTrain().  Please see
 * dwc_ddrphy_phyinit_struct.h for PhyInit data structure definition.
 *
 * \return void
 *
 * List of registers programmed by this function:
 */
void ddrphy_c_init_phy_cfg(struct artosyn_dram_info *dram)
{
	//##############################################################
	// Step (C) Initialize PHY Configuration
	// Load the required PHY configuration registers for the appropriate mode and memory configuration\n");
	//##############################################################

	printf("(C) Initialize PHY Configuration\n");

	ddrphy_prg_txslewrate(dram);
	ddrphy_prg_atxslewrate(dram);
	ddrphy_prg_dfirddatacsdestmap_dfiwrdatacsdestmap(dram);
	ddrphy_prg_pllctrl2(dram);
	ddrphy_prg_ardptrinitval(dram);
	ddrphy_prg_dbytedllmodecntrl_dqspreamblecontrol(dram);
	ddrphy_prg_procodttimectl(dram);
	ddrphy_prg_txodtdrvstren(dram);
	ddrphy_prg_tximpedancectrl1(dram);
	ddrphy_prg_atximpedance(dram);
	ddrphy_prg_dfimode(dram);
	ddrphy_prg_dficamode(dram);
	ddrphy_prg_caldrvstr0(dram);
	ddrphy_prg_calvrefs(dram);
	ddrphy_prg_caluclkinfo(dram);
	ddrphy_prg_calibration_csrs(dram);
	ddrphy_prg_vrefinglobal(dram);
	ddrphy_prg_memalertcontrol_memalertcontrol2(dram);
	ddrphy_prg_dfifreqratio(dram);
	ddrphy_prg_tristatemodeca(dram);
	ddrphy_prg_dfixlat(dram);
	ddrphy_prg_dqdqsrcvcntrl1_dbytemiscmode_a(dram);
	ddrphy_prg_dqdqsrcvcntrl1_dbytemiscmode_b(dram);
//	ddrphy_prg_dmipinpresent_readdbienable(dram);
}
// End of ddrphy_c_init_phy_cfg()

/*
 * This function must be used to trigger setting DfiClk to the
 * frequency associated with the input PState.
 *
 * The purpose of this function is to change DfiClk to the desired frequency for
 * the input PState before proceeding to the next step. The default behavior of
 * this function is to print comments relating to this process.  A function call
 * of the same name will be printed in the output text file. The PhyInit
 * ddrphy_init_seq() function calls this function multiple times in
 * order to set DfiClk before triggering training firmware execution for
 * different PStates. The User can edit this function to their needs in order to
 * implement this functionality.
 *
 * the clock should be stable at the new frequency. For more information on
 * clocking requirements, see "Clocks" section in the PUB documentation.
 *
 * \note this routine implies other items such as DfiFreqRatio, DfiCtlClk are
 * also set properly.  Because the clocks are controlled in the SOC, external to
 * the software and PHY, this step is intended to be replaced by the user with
 * the necessary SOC operations to achieve the new input frequency to the PHY.
 *
 * !< Input Pstate indicating associated DfiClk Frequency>
 * \return integer value = Pstate
 */
int ddrphy_e_set_dfi_clk(struct artosyn_dram_info *dram, int pstate)
{
	//##############################################################
	// Step (E) Set the PHY input clocks to the desired frequency for pstate
	// See PhyInit App Note for detailed description and function usage
	//##############################################################\n");

	user_input_basic_t *basic = &dram->userInputBasic;
	artosyn_dphy_pll_cfgs_t pll_cfgs;

	memcpy(&pll_cfgs, &dram->pll_default_cfgs,
	       sizeof(artosyn_dphy_pll_cfgs_t));

	pll_cfgs.ddr_mem_freq = basic->Frequency[pstate];

	ddrphy_pll_config(&pll_cfgs);

	return (pstate);
}

/**
 * @brief writes local memory content into the SRAM via APB interface.
 *
 * This function issued APB writes commands to SRAM address based on values
 * stored in a local PhyInit array that contains consolidated IMEM and DMEM
 * data.
 * @param[in] mem[] Local memory array.
 * @param[in] mem_offset offset index. if provided, skips to the offset index
 * from the local array and issues APB commands from mem_offset to mem_size.
 * @param[in] mem_size size of the memroy (in mem array index)
 * @returns void
 */
void ddrphy_wr_out_mem(struct artosyn_dram_info *dram, uint16_t mem[], int mem_offset, int mem_size) {
	int index;

	// 1. Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.\n");
	//    This allows the memory controller unrestricted access to the configuration CSRs. \n");
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	for (index = 0; index < mem_size; index++) {
		// routine call option
		// dwc_ddrphy_phyinit_print ("WriteOutMem: Attempting Write: Adr:0x%x Dat: 0x%x\n",index + mem_offset,mem[index]);
		// change from real mem_offset to vitual offset increase one per word.
		ddrphy_io_write16(dram, mem_offset / sizeof(uint32_t) + index, mem[index]);
	}
	// 2. Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. \n");
	//    This allows the firmware unrestricted access to the configuration CSRs. \n");
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
}

/** \brief reads firmware image incv file
 *
 * Routine to read an incv file into an internal mem array.
 *
 * \return the address of the first apb write in the incv file to use as the mem
 * offset.
 */
int ddrphy_store_bin_file(uint16_t mem[], unsigned int offset, unsigned int length)
{
	//int ret;

	// emmc
#ifdef CONFIG_MMC
	return spl_mmc_read_from_hw_partition(0, offset / 512, length / 512,
					      mem);
#elif defined(CONFIG_SPI_FLASH)
	return spinor_read(mem, offset, length);
#elif defined(CONFIG_MTD_SPI_NAND)
	return spinand_read(mem, offset, length);
#endif
}

/** \brief store the message block into the bottom of the local DMEM array
 *
 * \return void
 */
void ddrphy_store_msg_blk(void *msgBlkPtr,  int sizeOfMsgBlk, uint16_t mem[])
{
	// Local variables
	int loop;
	uint16_t *dataArray;

	// Recast the structure pointer as a pointer to an array of 16-bit values
	dataArray = (uint16_t *)msgBlkPtr;

	// Loop over the structure 16 bits at a time and load dmem
	for (loop = 0; loop < (sizeOfMsgBlk / sizeof(uint16_t)); loop++) {
		// The data is the data in the structure at the loop offset
		mem[loop] = dataArray[loop];
	}
}

/*
 *  This function reads the incv files form the firmware package to generate a
 *  set of apb writes to load IMEM image into the SRAM. The exact steps in this
 *  function are as follows:
 *
 *  -# Ensure DRAM is in reset.
 *  -# Load the microcontroller memory with the provided training firmware
 *  -# Initialize the firmware mailbox structures to be able to communicate with
 *  the firmware (see "Mailbox facility for firmware" in the "DesignWare Cores
 *  DDR PHY Training Application Note".
 *
 * \return void
 */
void ddrphy_d_load_imem(struct artosyn_dram_info *dram, int Train2D)
{
	//##############################################################
	// (D) Load the %dD IMEM image\n", Train2D+1
	// This function loads the training firmware IMEM image into the SRAM.
	// See PhyInit App Note for detailed description and function usage
	//##############################################################

	/*
	 * Set MemResetL to avoid glitch on BP_MemReset_L during training
	 */
	artosyn_vendor_header_t *header = &dram->vendor_header;
	int MemResetL;
	uint16_t mem[1024];
	int ret;
	int readed, total;
	uint32_t flash_offset;
	//return_offset_lastaddr_t return_type = return_offset;

	if (Train2D == 0) {	// 1-D
		MemResetL = csr_ProtectMemReset_MASK;
		ddrphy_io_write16(dram, (tMASTER | csr_MemResetL_ADDR), MemResetL);
	}

#ifdef AR9301_SDRAM_DIAG
	if (Train2D == 2) {
		flash_offset = header->imem_offset_diag;
		total = header->imem_size_diag;
	} else if (Train2D == 1) {
#else
	if (Train2D == 1) {
#endif
		printf("2D Training not supported for DDR3:UDIMM!\n");
		return;
		flash_offset = header->imem_offset_2d;
		total = header->imem_size_2d;
	} else {
		flash_offset = header->ddr3_imem_offset_1d;
		total = header->ddr3_imem_size_1d;
	}

	for (readed = 0; readed < IMEM_SIZE; readed += sizeof(mem)) {
		memset(mem, 0, sizeof(mem));

		if (readed < total) {
			// Read the IMEM INCV file into the array
			if (total - readed < sizeof(mem)) {
				ret = ddrphy_store_bin_file(mem, flash_offset + readed,
					ALIGN(total - readed, DDRPHY_FLASH_ALIGN));
			} else {
				ret = ddrphy_store_bin_file(mem, flash_offset + readed, sizeof(mem));
			}
		}
		if (ret < 0) {
			pr_err("Store IMEM faild!\n");
			return;
		}

		// Write local imem array
		ddrphy_wr_out_mem(dram, mem, DDRPHY_IMEM_OFFSET + readed / sizeof(uint16_t) * sizeof(uint32_t), sizeof(mem) / sizeof(uint16_t));
	}

	//dwc_ddrphy_phyinit_cmnt("%s WriteImem: COMPLETED\n", printf_header);
}

/*  Message Block parameters for the training firmware into the SRAM.
 *
 *  This function performs the following tasks:
 *
 *  -# Load the firmware DMEM segment to initialize the data structures from the
 *  DMEM incv file provided in the training firmware package.
 *  -# Write the Firmware Message Block with the required contents detailing the training parameters.
 *
 * \return void
 */
void ddrphy_f_load_dmem(struct artosyn_dram_info *dram, int pstate, int Train2D)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	//##############################################################
	// (F) Load the %dD DMEM image and write the %dD Message Block
	// parameters for the training firmware \n", Train2D+1, Train2D+1);
	// See PhyInit App Note for detailed description and function usage
	//##############################################################

	// set a pointer to the message block.
	artosyn_vendor_header_t *header = &dram->vendor_header;
	PMU_SMB_DDR3U_1D_t *msgBlkPtr;
	int sizeOfMsgBlk;
	uint16_t mem[1024];
	int readed, reading;
	uint32_t flash_offset;
	uint32_t dmem_size;
	int ret;

#ifdef AR9301_SDRAM_DIAG
	if (Train2D == 2) {
		flash_offset = header->dmem_offset_diag;
		dmem_size = header->dmem_size_diag;
	} else if (Train2D == 1) {
#else
	if (Train2D == 1) {
#endif
		printf("2D Training not supported for DDR3!\n");
		return;
	} else {
		msgBlkPtr = &mb_DDR3U_1D[pstate];
		flash_offset = header->ddr3_dmem_offset_1d;
		dmem_size = header->ddr3_dmem_size_1d;
	}

	// Some basic checks on MessgeBlock
	if (Train2D == 0 || Train2D == 1) {
		if (msgBlkPtr->EnabledDQs > 8 * (basic->NumActiveDbyteDfi0) ||
		    msgBlkPtr->EnabledDQs <= 0) {
			pr_err("EnabledDQs is invalid\n");
		}
	}

	for (readed = 0; readed < dmem_size; readed += sizeof(mem)) {
		reading = dmem_size < (readed + sizeof(mem)) ?
				(dmem_size - readed) :
				sizeof(mem);

		memset(mem, 0, sizeof(mem));
		ret = ddrphy_store_bin_file(mem, flash_offset + readed, sizeof(mem));
		if (ret < 0) {
			pr_err("Store DMEM faild!\n");
			return;
		}

		if (Train2D == 1 && readed == 0) {
			printf("2D Training not supported for DDR3!\n");
			return;
		} else if (Train2D == 0 && readed == 0) {
			sizeOfMsgBlk = sizeof(mb_DDR3U_1D[pstate]);
			ddrphy_store_msg_blk(&(mb_DDR3U_1D[pstate]), sizeOfMsgBlk, mem);
		} else {
			log_info("DMEM only\n");
		}

		// Write local dmem array
		// Always write an even number of words so no 32bit quantity is uninitialized
		//if (0 == (mem_offset % 1)) {
		//	mem_offset++;
		//}
#ifdef AR9301_SDRAM_DIAG
		if (Train2D == 2)
			ddrphy_wr_out_mem(dram, mem, DDRPHY_DIAG_DMEM_OFFSET + readed / sizeof(uint16_t) * sizeof(uint32_t), reading / sizeof(uint16_t));
		else
#endif
		ddrphy_wr_out_mem(dram, mem, DDRPHY_DMEM_OFFSET + readed / sizeof(uint16_t) * sizeof(uint32_t), reading / sizeof(uint16_t));
	}
}

/*
 * get_mail function to handle protocol and return message
 *
 * mode sets 16bit (major message) or 32bit (streaming message)
 */
uint32_t ddrphy_get_mail(struct artosyn_dram_info *dram, uint32_t mode)
{
	uint32_t mail;
	uint32_t timeout = 1000000;
	uint32_t tmp;

	while (timeout--) {
		if (!timeout) {
			printf("Wait mail timeout!\n");
			return 0xff;
		}
		tmp = ddrphy_io_read16(dram, tAPBONLY | csr_UctShadowRegs_ADDR);
		if ((tmp & csr_UctWriteProtShadow_MASK) == 0)
			break;
		udelay(1);
	}
	mail = ddrphy_io_read16(dram, tAPBONLY | csr_UctWriteOnlyShadow_ADDR);
	if (mode == 32)
		mail |= ddrphy_io_read16(dram, tAPBONLY | csr_UctDatWriteOnlyShadow_ADDR) << 16;

	ddrphy_io_write16(dram, tAPBONLY | csr_DctWriteProt_ADDR, 0);

	timeout = 1000000;
	while (timeout--) {
		if (!timeout) {
			printf("Wait ack receipt timeout!\n");
			return 0xff;
		}
		tmp = ddrphy_io_read16(dram, tAPBONLY | csr_UctShadowRegs_ADDR);
		if ((tmp & csr_UctWriteProtShadow_MASK) == 1)
			break;
		udelay(1);
	}

	ddrphy_io_write16(dram, tAPBONLY | csr_DctWriteProt_ADDR, 1);

	return mail;
}

/*
 * Decode streaming message pseudocode
 */
void ddrphy_get_streaming_msg(struct artosyn_dram_info *dram)
{
	uint32_t string_index;
	uint32_t arg;
	uint32_t i;

	string_index = ddrphy_get_mail(dram, 32);
	printf("message: 0x%x ", string_index);

	if (string_index == 0xff) {
		printf("Get streaming message failed!\n");
		return;
	}

	for (i = 0; i < (string_index & 0xffff); i++) {
		arg = ddrphy_get_mail(dram, 32);
		printf("arg%d: %d ", i, arg);
	}
	printf("\n");
}

/*
 * Protocol initialization
 */
void ddrphy_init_mail(struct artosyn_dram_info *dram)
{
	ddrphy_io_write16(dram, tAPBONLY | csr_DctWriteProt_ADDR, 1);
	ddrphy_io_write16(dram, tAPBONLY | csr_UctWriteProt_ADDR, 1);
}

void ddrphy_polling_mail(struct artosyn_dram_info *dram)
{
	uint32_t mail;
	uint32_t timeout = 1000000;

	while (timeout) {
		if (!timeout) {
			printf("Polling mail timeout!\n");
			break;
		}

		mail = ddrphy_get_mail(dram, 16);
		if (mail == 0x07) {
			printf("Training successfully.\n");
			break;
		} else if (mail == 0xff) {
			printf("Training has failed!\n");
			break;
		} else if (mail != 0x08) {
			printf("mail: 0x%x\n", mail);
		}

		if (mail == 0x08)
			ddrphy_get_streaming_msg(dram);

		udelay(1);
		timeout--;
	}
}

/*
 * Implements the mechanism to wait for completion of training firmware
 * execution.
 *
 * The purpose of user this function is to wait for firmware to finish training.
 * The user can either implement a counter to wait or implement the polling
 * mechanism described in the Training Firmware App Note section "Running the
 * Firmware".  The wait time is highly dependent on the training features
 * enabled via SequenceCtrl input to the message block.  See Training Firmware
 * App note for details.
 *
 * The default behavior of this function is to print comments relating to this
 * process.  A function call of the same name will be printed in the output text
 * file.
 *
 * The user can choose to leave this function as is, or implement mechanism to
 * trigger mailbox poling event in simulation.

 * \return void
 */
void ddrphy_g_wait_fw_done(struct artosyn_dram_info *dram) {

	// Wait for the training firmware to complete.
	// Implement timeout fucntion or follow the procedure in
	// 3.4 Running the firmware of the Training Firmware
	// Application Note to poll the Mailbox message.

	// ddrphy_g_wait_fw_done ();
#ifdef AR9301_SDRAM_DEBUG
	mdelay(500);
	printf("mdelay(500);\n");
#else
	ddrphy_polling_mail(dram);
#endif
}

/*
 * The training firmware is executed with the following procedure:
 *
 * -# Reset the firmware microcontroller by writing the MicroReset register to
 * set the StallToMicro and ResetToMicro fields to 1 (all other fields should be
 * zero). Then rewrite the registers so that only the StallToMicro remains set
 * (all other fields should be zero).
 * -# Begin execution of the training firmware by setting the MicroReset
 * register to 4'b0000.
 * -# Wait for the training firmware to complete by following the procedure in
 * "uCtrl Initialization and Mailbox Messaging" implemented in
 * ddrphy_g_wait_fw_done() function.
 * -# Halt the microcontroller.
 *
 * \return void
 */
void ddrphy_g_exec_fw(struct artosyn_dram_info *dram)
{
	//##############################################################
	// (G) Execute the Training Firmware
	// See PhyInit App Note for detailed description and function usage.
	//##############################################################

	// 1. Reset the firmware microcontroller by writing the MicroReset CSR to set the StallToMicro and
	//    ResetToMicro fields to 1 (all other fields should be zero).
	//    Then rewrite the CSR so that only the StallToMicro remains set (all other fields should be zero).
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroReset_ADDR), csr_ResetToMicro_MASK | csr_StallToMicro_MASK);
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroReset_ADDR), csr_StallToMicro_MASK);

	// 2. Begin execution of the training firmware by setting the MicroReset CSR to 4'b0000.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroReset_ADDR), 0x0);

	// 3. Wait for the training firmware to complete by following the procedure in "uCtrl Initialization and Mailbox Messaging"
	ddrphy_g_wait_fw_done(dram);

	// 4. Halt the microcontroller.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroReset_ADDR), csr_StallToMicro_MASK);
}

/** \brief reads training results
 *
 * Read the Firmware Message Block via APB read commands to the DMEM address to
 * obtain training results.
 *
 * The default behavior of this function is to print comments relating to this
 * process. An example pseudo code for implementing this function is as follows:
 *
 * @code{.c}
 * if (Train2D)
 * {
 *   _read_2d_message_block_outputs_
 * }
 * else
 * {
 *   _read_1d_message_block_outputs_
 * }
 * @endcode
 *
 * A function call of the same name will be printed in the output text file.
 * User can choose to leave this function as is, or implement mechanism to
 * trigger message block read events in simulation.
 *
 * \return void
 */
void ddrphy_h_do_read_msg_blk(struct artosyn_dram_info *dram, int Train2D)
{
	// 2. Read the Firmware Message Block to obtain the results from the training.
	// This can be accomplished by issuing APB read commands to the DMEM addresses.
	// Example:
	// if (Train2D)
	// {
	//   _read_2d_message_block_outputs_
	// }
	// else
	// {
	//   _read_1d_message_block_outputs_
	// }
}

/*
 * The procedure is as follows:
 *
 * -# Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
 * -# Read the Firmware Message Block to obtain the results from the training.
 * The user customizable function ddrphy_h_read_msg_blk()
 * is called for the user to read any specific results from training.
 * -# Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
 * -# If training is required at another frequency, repeat the operations starting at step (E)
 *
 * \return void
 */
void ddrphy_h_read_msg_blk(struct artosyn_dram_info *dram, int Train2D)
{
	//##############################################################
	// (H) Read the Message Block results
	// The procedure is as follows:
	//##############################################################

	// 1. Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	// 2. Read the Firmware Message Block to obtain the results from the training.
	ddrphy_h_do_read_msg_blk(dram, Train2D);

	// 3. Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);

	// 4. If training is required at another frequency, repeat the operations starting at step (E).
}

#ifdef AR9301_SDRAM_DIAG
void ddrphy_diag_test_4(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	uint32_t n = 0, m = 0, i = 0;
	uint16_t data = 0x1234;
	uint16_t data_array[18];
	char data_byte, *p;

	// 1. Enable access to the internal CSRs by setting the MicroContMuxSel
	//    CSR to 0. This allows the memory controller unrestricted access to
	//    the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t));
	data &= 0xff;

	if (data == 0)
		log_info("Global passed\n");
	else if (data == 1)
		log_info("Global failed\n");
	else if (data == 0xff)
		log_info("Lane was not tested\n");
	else
		log_info("Global Unknow result\n");

	for (i = 0; i < basic->NumDbyte * 9 / 2; i++)
		data_array[i] = ddrphy_io_read16(dram,
						 DDRPHY_DIAG_DATA_OFFSET /
						 sizeof(uint32_t) + i + 1);

	p = (char *)data_array;
	for (n = 0; n < basic->NumDbyte; n++) {
		for (m = 0; m < 9; m++) {
			data_byte = p[n * 9 + m];
			if (data_byte == 0)
				log_info("Dbyte %d Lane %d passed\n", n, m);
			else if (data_byte == 1)
				log_info("Dbyte %d Lane %d failed\n", n, m);
			else if (data_byte == 0xff)
				log_info("Dbyte %d Lane %d was not tested\n", n,
					 m);
			else
				log_info("Dbyte %d Lane %d Unknow result\n", n,
					 m);
		}
	}

	// 2. Isolate the APB access from the internal CSRs by setting the
	//    MicroContMuxSel CSR to 1. This allows the firmware unrestricted
	///   access to the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
}

void ddrphy_diag_test_5(struct artosyn_dram_info *dram)
{
	uint32_t ndly, nvref, vref, dly, vrefidx, dlyidx;
	uint32_t i, total = 0;
	uint16_t data = 0x1234;

	// 1. Enable access to the internal CSRs by setting the MicroContMuxSel
	//    CSR to 0. This allows the memory controller unrestricted access to
	//    the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t));
	ndly = data & 0xff;
	nvref = (data & 0xff00) >> 8;

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t) + 1);
	vref = data & 0xff;

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t) + 2);
	dly = data;
	log_info("nDlay %d nVREF %d vref %d TxDqDly %d\n", ndly, nvref, vref,
		 dly);

	total = nvref * ndly;
	for (i = 0; i < total; i += 2) {
		data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
					sizeof(uint32_t) + i / 2 + 3);
		vrefidx = i / ndly;
		dlyidx = i % ndly;

		log_info("%03d ", data & 0xff);
		if (dlyidx == ndly - 1)
			log_info("\n");

		if (total - i == 1)
			break;
		vrefidx = (i + 1) / ndly;
		dlyidx = (i + 1) % ndly;

		log_info("%03d ", (data & 0xff00) >> 8);
		if (dlyidx == ndly - 1)
			log_info("\n");
	}

	// 2. Isolate the APB access from the internal CSRs by setting the
	//    MicroContMuxSel CSR to 1. This allows the firmware unrestricted
	///   access to the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
}

void ddrphy_diag_test_6(struct artosyn_dram_info *dram)
{
	uint32_t ndly, nvref, vrefdac0, vrefdac1, rxclkdly, vrefidx, dlyidx;
	uint32_t i, total = 0;
	uint16_t data = 0x1234;

	// 1. Enable access to the internal CSRs by setting the MicroContMuxSel
	//    CSR to 0. This allows the memory controller unrestricted access to
	//    the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t));
	ndly = data & 0xff;
	nvref = (data & 0xff00) >> 8;

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t) + 1);
	vrefdac0 = data & 0xff;
	vrefdac1 = (data & 0xff00) >> 8;

	data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
				sizeof(uint32_t) + 2);
	rxclkdly = data;
	log_info("nDlay %d nVREF %d VrefDAC0 %d VrefDAC1 %d RxClkDly %d\n",
		 ndly, nvref, vrefdac0, vrefdac1, rxclkdly);

	total = nvref * ndly;
	for (i = 0; i < total; i += 2) {
		data = ddrphy_io_read16(dram, DDRPHY_DIAG_DATA_OFFSET /
					sizeof(uint32_t) + i / 2 + 3);
		vrefidx = i / ndly;
		dlyidx = i % ndly;

		log_info("%03d ", data & 0xff);
		if (dlyidx == ndly - 1)
			log_info("\n");

		if (total - i == 1)
			break;
		vrefidx = (i + 1) / ndly;
		dlyidx = (i + 1) % ndly;

		log_info("%03d ", (data & 0xff00) >> 8);
		if (dlyidx == ndly - 1)
			log_info("\n");
	}

	// 2. Isolate the APB access from the internal CSRs by setting the
	//    MicroContMuxSel CSR to 1. This allows the firmware unrestricted
	///   access to the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
}

void ddrphy_diag_configure_mb(struct artosyn_dram_info *dram, uint32_t test,
			      uint32_t diag_byte, uint32_t diag_lane)
{
	memset((void *)&mb_diag, 0, sizeof(mb_diag));

	switch (test) {
	case 4:
		mb_diag.DiagTestNum = 4;
		mb_diag.DiagPrbs = 1;
		mb_diag.DiagLoopCount = 0;
		mb_diag.DiagRank = 0;
		mb_diag.DiagAddrHigh = 20;
		mb_diag.DiagAddrLow = 10;
		break;
	case 5:
		mb_diag.DiagTestNum = 5;
		mb_diag.DiagByte = diag_byte;
		mb_diag.DiagLane = diag_lane;
		mb_diag.DiagPrbs = 1;
		mb_diag.DiagLoopCount = 0;
		mb_diag.DiagRepeatCount = 127;
		mb_diag.DiagXCount = 1;
		mb_diag.DiagRank = 0;
		mb_diag.DiagVrefInc = 1;
		mb_diag.DiagAddrHigh = 20;
		mb_diag.DiagAddrLow = 10;
		break;
	case 6:
		mb_diag.DiagTestNum = 6;
		mb_diag.DiagByte = diag_byte;
		mb_diag.DiagLane = diag_lane;
		mb_diag.DiagPrbs = 1;
		mb_diag.DiagLoopCount = 0;
		mb_diag.DiagRepeatCount = 127;
		mb_diag.DiagXCount = 1;
		mb_diag.DiagRank = 0;
		mb_diag.DiagVrefInc = 1;
		mb_diag.DiagAddrHigh = 20;
		mb_diag.DiagAddrLow = 10;
		break;
	default:
		log_err("Unknown test cast %d\n", test);
		return;
	}

	ddrphy_wr_out_mem(dram, (uint16_t *)&mb_diag, DDRPHY_DIAG_DMEM_OFFSET,
			  sizeof(mb_diag) / sizeof(uint16_t));
}

void ddrphy_diag_reset_and_exec(struct artosyn_dram_info *dram)
{
#ifndef AR9301_SDRAM_DEBUG
	uint32_t timeout = 1000000;
	uint32_t tmp;
#endif
	// 2. Reset 1st microcontroller message protocol bit
	ddrphy_io_write16(dram, tAPBONLY | csr_UctWriteProt_ADDR, 0x1);
	// 3. Reset 2nd microcontroller message protocol bit
	ddrphy_io_write16(dram, tAPBONLY | csr_DctWriteProt_ADDR, 0x1);
	// 4. Reset microcontroller message
	ddrphy_io_write16(dram, tAPBONLY | csr_UctWriteOnly_ADDR, 0x0);
	// 5. Set control of the internal CSR bus to the PMU
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroContMuxSel_ADDR, 0x1);
	// 6. Assert PMU reset and stall
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroReset_ADDR, 0x9);
	// 7. De-assert PMU reset only
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroReset_ADDR, 0x1);
	// 8. De-assert PMU stall
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroReset_ADDR, 0x0);
	// 9. Wait for the diagnostics test to finish
#ifdef AR9301_SDRAM_DEBUG
	printf("while (UctWriteOnlyShadow != 0x07)\n");
#else
	while (timeout--) {
		if (!timeout) {
			pr_err("Wait diag timeout!\n");
			return;
		}
		tmp = ddrphy_io_read16(dram, tAPBONLY |
			csr_UctWriteOnlyShadow_ADDR);
		if (tmp == 0x07) {
			printf("Diag OK\n");
			break;
		} else if (tmp == 0xff) {
			pr_err("Diag abnormal exit!\n");
			return;
		}
	}
#endif
	mdelay(100);
	// 10. Assert PMU stall
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroReset_ADDR, 0x1);
	// 11. Set control of the internal CSR bus to the APB
	ddrphy_io_write16(dram, tAPBONLY | csr_MicroContMuxSel_ADDR, 0x0);
}

/** Load and execute diagnostics firmware
 * @return void
 */
void ddrphy_diag_load_and_exec(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	uint32_t diag_byte, diag_lane;

	// Halt the microcontroller.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroReset_ADDR),
			  csr_StallToMicro_MASK);

	// Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	ddrphy_io_write16(dram, (tDRTUB | csr_UcclkHclkEnables_ADDR), 0x3);
	ddrphy_d_load_imem(dram, 2);	// 0==1D 1==2D 2==diag
	ddrphy_f_load_dmem(dram, 0, 2);

	// Test 4: Simple Write Read
	log_info("T4: Simple Write Read\n");
	// 1. Configure the diagnostics message block
	ddrphy_diag_configure_mb(dram, 4, 0, 0);
	// 2~11
	ddrphy_diag_reset_and_exec(dram);
	// 12. Read back diagnostics return data
	ddrphy_diag_test_4(dram);

	// Test 5: Tx Eye
	for (diag_byte = 0; diag_byte < basic->NumDbyte; diag_byte++) {
		for (diag_lane = 0; diag_lane < 9; diag_lane++) {
			log_info("T5: Tx Eye DiagByte %d DiagLane %d\n",
				 diag_byte, diag_lane);
			// 1. Configure the diagnostics message block
			ddrphy_diag_configure_mb(dram, 5, diag_byte, diag_lane);
			// 2~11
			ddrphy_diag_reset_and_exec(dram);
			// 12. Read back diagnostics return data
			ddrphy_diag_test_5(dram);
		}
	}

	// Test 6: Rx Eye
	for (diag_byte = 0; diag_byte < basic->NumDbyte; diag_byte++) {
		for (diag_lane = 0; diag_lane < 8; diag_lane++) {
			log_info("T6: Rx Eye DiagByte %d DiagLane %d\n",
				 diag_byte, diag_lane);
			// 1. Configure the diagnostics message block
			ddrphy_diag_configure_mb(dram, 6, diag_byte, diag_lane);
			// 2~11
			ddrphy_diag_reset_and_exec(dram);
			// 12. Read back diagnostics return data
			ddrphy_diag_test_6(dram);
		}
	}
}
#endif

/** @brief Loads PIE instruction sequence PHY registers
 *  @returns void
 */
void ddrphy_load_pie_prod_code(struct artosyn_dram_info *dram)
{
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	uint16_t i;
	uint16_t initeng0_en[] = {
			0x000b,0x0480,0x0109,0x0008,0x0448,0x0139,0x0008,0x0478,
			0x0109,0x0002,0x0010,0x0139,0x000b,0x07c0,0x0139,0x0044,
			0x0633,0x0159,0x014f,0x0630,0x0159,0x0047,0x0633,0x0149,
			0x004f,0x0633,0x0179,0x0008,0x00e0,0x0109,0x0000,0x07c8,
			0x0109,0x0000,0x0001,0x0008,0x0000,0x045a,0x0009,0x0000,
			0x0448,0x0109,0x0040,0x0633,0x0179,0x0001,0x0618,0x0109,
			0x40c0,0x0633,0x0149,0x0008,0x0004,0x0048,0x4040,0x0633,
			0x0149,0x0000,0x0004,0x0048,0x0040,0x0633,0x0149,0x0010,
			0x0004,0x0018,0x0000,0x0004,0x0078,0x0549,0x0633,0x0159,
			0x0d49,0x0633,0x0159,0x094a,0x0633,0x0159,0x0441,0x0633,
			0x0149,0x0042,0x0633,0x0149,0x0001,0x0633,0x0149,0x0000,
			0x00e0,0x0109,0x000a,0x0010,0x0109,0x0009,0x03c0,0x0149,
			0x0009,0x03c0,0x0159,0x0018,0x0010,0x0109,0x0000,0x03c0,
			0x0109,0x0018,0x0004,0x0048,0x0018,0x0004,0x0058,0x000b,
			0x0010,0x0109,0x0001,0x0010,0x0109,0x0005,0x07c0,0x0109,
			0x000e,0x07c0,0x0189,0x0000,0x8140,0x010c,0x0010,0x8138,
			0x010c,0x0008,0x07c8,0x0101,0x0008,0x0448,0x0109,0x000f,
			0x07c0,0x0109,0x0047,0x0630,0x0109,0x0008,0x0618,0x0109,
			0x0008,0x00e0,0x0109,0x0000,0x07c8,0x0109,0x0008,0x8140,
			0x010c,0x0000,0x0478,0x0109,0x0000,0x0001,0x0008,0x0008,
			0x0004,0x0008,0x0008,0x07c8,0x0101
			};
	uint16_t initeng0_dis[] = {
			0x000b,0x0480,0x0109,0x0008,0x0448,0x0139,0x0008,0x0478,
			0x0109,0x0002,0x0010,0x0139,0x000b,0x07c0,0x0139,0x0044,
			0x0633,0x0159,0x014f,0x0630,0x0159,0x0047,0x0633,0x0149,
			0x004f,0x0633,0x0179,0x0008,0x00e0,0x0109,0x0000,0x07c8,
			0x0109,0x0000,0x0001,0x0008,0x0000,0x045a,0x0009,0x0000,
			0x0448,0x0109,0x0040,0x0633,0x0179,0x0001,0x0618,0x0109,
			0x40c0,0x0633,0x0149,0x0008,0x0004,0x0048,0x4040,0x0633,
			0x0149,0x0000,0x0004,0x0048,0x0040,0x0633,0x0149,0x0010,
			0x0004,0x0018,0x0000,0x0004,0x0078,0x0549,0x0633,0x0159,
			0x0d49,0x0633,0x0159,0x094a,0x0633,0x0159,0x0441,0x0633,
			0x0149,0x0042,0x0633,0x0149,0x0001,0x0633,0x0149,0x0000,
			0x00e0,0x0109,0x000a,0x0010,0x0109,0x0009,0x03c0,0x0149,
			0x0009,0x03c0,0x0159,0x0018,0x0010,0x0109,0x0000,0x03c0,
			0x0109,0x0018,0x0004,0x0048,0x0018,0x0004,0x0058,0x000b,
			0x0010,0x0109,0x0001,0x0010,0x0109,0x0005,0x07c0,0x0109,
			0x0000,0x8140,0x010c,0x0010,0x8138,0x010c,0x0008,0x07c8,
			0x0101,0x0008,0x0448,0x0109,0x000f,0x07c0,0x0109,0x0047,
			0x0630,0x0109,0x0008,0x0618,0x0109,0x0008,0x00e0,0x0109,
			0x0000,0x07c8,0x0109,0x0008,0x8140,0x010c,0x0000,0x0478,
			0x0109,0x0000,0x0001,0x0008,0x0008,0x0004,0x0008,0x0008,
			0x07c8,0x0101
			};

	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t), 0x0010);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x01, 0x0400);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x02, 0x010e);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x03, 0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x04, 0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x05, 0x0008);

	if (advanced->EnableHighClkSkewFix) {
		for (i = 0; i <= 164; i++)
			ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET /
				sizeof(uint32_t) + 0x29 + i, initeng0_en[i]);
	} else {
		for (i = 0; i <= 161; i++)
			ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET /
				sizeof(uint32_t) + 0x29 + i, initeng0_dis[i]);
	}
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x06,0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x07,0x0400);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x08,0x010e);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x09,0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x0a,0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x0b,0x0000);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x400e7,0x0400);
	ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x17,0x0000);

	if (advanced->EnableHighClkSkewFix)
		ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x26,0x002c);
	else
		ddrphy_io_write16(dram, DDRPHY_PIE_OFFSET / sizeof(uint32_t) + 0x26,0x002b);
} // dwc_ddrphy_phyinit_LoadPieCode

/*
 * This function programs the PHY Initialization Engine (PIE) instructions and
 * the associated registers.
 *
 * \return void
 *
 * Detailed list of registers programmed by this function:
 */
void ddrphy_i_load_pie_img(struct artosyn_dram_info *dram)
{
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	unsigned int pstate;
	int p_addr;

	printf("(I) Load PIE Image\n");
	//##############################################################
	// (I) Load PHY Init Engine Image
	// Load the PHY Initialization Engine memory with the provided initialization sequence.
	// See PhyInit App Note for detailed description and function usage
	//##############################################################

	// Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	// This allows the memory controller unrestricted access to the configuration CSRs.
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

	ddrphy_load_pie_prod_code(dram);

	/**
	 * - Registers: Seq0BDLY0, Seq0BDLY1, Seq0BDLY2, Seq0BDLY3
	 *   - Program PIE instruction delays
	 *   - Dependencies:
	 *     - user_input_basic.Frequency
	 */
	// Need delays for 0.5us, 1us, 10us, and 25us.
	uint16_t psCount[4][4];
	float delayScale = 1.00;

	// Calculate the counts to obtain the correct delay for each frequency
	// Need to divide by 4 since the delay value are specified in units of
	// 4 clocks.
	double DfiFrq,dllLock;

	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;
		DfiFrq = (0.5 * basic->Frequency[pstate]);
		psCount[pstate][0] = (int)(( 0.5 * 0.25 * DfiFrq * delayScale));

		int LowFreqOpt = 0;
		if (basic->Frequency[pstate] < 400 )
			LowFreqOpt = 3;
		else if (basic->Frequency[pstate] < 533 )
			LowFreqOpt = 11;
		psCount[pstate][1] = (int)(( 1.0 * 0.25 * DfiFrq * delayScale)) - LowFreqOpt;;
		psCount[pstate][2] = (int)((10.0 * 0.25 * DfiFrq * delayScale));

		if (DfiFrq > 266.5) {
			dllLock=176;
		} else if (DfiFrq <= 266.5 && DfiFrq > 200) {
			dllLock=132;
		} else {
			dllLock=64;
		}

		psCount[pstate][3] = (int)(0.25 * dllLock);

		ddrphy_io_write16(dram, (p_addr | c0 | tMASTER | csr_Seq0BDLY0_ADDR), psCount[pstate][0]);
		ddrphy_io_write16(dram, (p_addr | c0 | tMASTER | csr_Seq0BDLY1_ADDR), psCount[pstate][1]);
		ddrphy_io_write16(dram, (p_addr | c0 | tMASTER | csr_Seq0BDLY2_ADDR), psCount[pstate][2]);
		ddrphy_io_write16(dram, (p_addr | c0 | tMASTER | csr_Seq0BDLY3_ADDR), psCount[pstate][3]);
	}

	/**
	 * - Registers: Seq0BDisableFlag0 Seq0BDisableFlag1 Seq0BDisableFlag2
	 *   Seq0BDisableFlag3 Seq0BDisableFlag4 Seq0BDisableFlag5
	 *   - Program PIE Instruction Disable Flags
	 *   - Dependencies:
	 *     - user_input_advanced.DisableRetraining
	 *     - skip_training
	 *     - user_input_basic.Frequency
	 */
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag0_ADDR), 0x0000);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag1_ADDR), 0x0173);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag2_ADDR), 0x0060);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag3_ADDR), 0x6110);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag4_ADDR), 0x2152);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag5_ADDR), 0xDFBD);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag6_ADDR), 0xffff);
	ddrphy_io_write16(dram, (tINITENG | csr_Seq0BDisableFlag7_ADDR), 0x6152);

	// - Register: CalZap
	//   - Prepare the calibration controller for mission mode.
	//     Turn on calibration and hold idle until dfi_init_start is asserted sequence is triggered.
	ddrphy_io_write16(dram, (tMASTER | csr_CalZap_ADDR), 0x1);

	// - Register: CalRate
	//   - Fields:
	//     - CalRun
	//     - CalOnce
	//     - CalInterval
	//   - Dependencies
	//     - user_input_advanced.CalInterval
	//     - user_input_advanced.CalRun
	//     - user_input_advanced.CalOnce

	int CalRate;
	int CalInterval;
	int CalOnce;

	CalInterval = advanced->CalInterval;
	CalOnce = advanced->CalOnce;

	CalRate = (0x1 << csr_CalRun_LSB) | (CalOnce << csr_CalOnce_LSB) | (CalInterval << csr_CalInterval_LSB);

	ddrphy_io_write16(dram, (tMASTER | csr_CalRate_ADDR), CalRate);

	/**
	 * At the end of this function, PHY Clk gating register UcclkHclkEnables is
	 * set for mission mode.  Additionally APB access is Isolated by setting
	 * MicroContMuxSel.
	 */
	ddrphy_io_write16(dram, (tDRTUB | csr_UcclkHclkEnables_ADDR), 0x0);
	ddrphy_io_write16(dram, (tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
}

static void ddrphy_ssc_config(artosyn_dphy_pll_cfgs_t *pll_cfgs) {
	/*
	 * PLL optional SSC opration
	 * Spread spectrum after tunning finished
	 */
	uint32_t dig_reg1 = 0;	// SSC reg
				// ssc_en[10]
				// ssc_reg[25:12]
				// |  ssc_mode[25:24]
				// |  |  00: down spread
				// |  |  01: center spread
				// |  |  10: up spread
				// |  ssc_kdelta[23:18]
				// |  ssc_num_step[17:15]
				// |  ssc_divn[14:12]

	dig_reg1 = readl((volatile unsigned int *)DDRPLL_DIG_REG1_ADDR);
	dig_reg1 &= ~DDRPLL_DIG_REG1_SSC_EN_MASK;
	dig_reg1 &= ~DDRPLL_DIG_REG1_SSC_MODE_MASK;
	dig_reg1 &= ~DDRPLL_DIG_REG1_SSC_KDELTA_MASK;
	dig_reg1 &= ~DDRPLL_DIG_REG1_SSC_NUM_STEP_MASK;
	dig_reg1 &= ~DDRPLL_DIG_REG1_SSC_DIVN_MASK;

	dig_reg1 |= (pll_cfgs->ssc_en << DDRPLL_DIG_REG1_SSC_EN_SHIFT) &
		    DDRPLL_DIG_REG1_SSC_EN_MASK;
	dig_reg1 |= (pll_cfgs->ssc_mode << DDRPLL_DIG_REG1_SSC_MODE_SHIFT) &
		    DDRPLL_DIG_REG1_SSC_MODE_MASK;
	dig_reg1 |= (pll_cfgs->ssc_kdelta << DDRPLL_DIG_REG1_SSC_KDELTA_SHIFT) &
		    DDRPLL_DIG_REG1_SSC_KDELTA_MASK;
	dig_reg1 |= (pll_cfgs->ssc_num_step <<
		     DDRPLL_DIG_REG1_SSC_NUM_STEP_SHIFT) &
		    DDRPLL_DIG_REG1_SSC_NUM_STEP_MASK;
	dig_reg1 |= (pll_cfgs->ssc_divn << DDRPLL_DIG_REG1_SSC_DIVN_SHIFT) &
		    DDRPLL_DIG_REG1_SSC_DIVN_MASK;

	writel(dig_reg1, (volatile unsigned int *)DDRPLL_DIG_REG1_ADDR);
}

/*
 * The default behavior of this function is to print comments relating to this
 * process. User can choose to leave this function as is, or implement mechanism
 * to trigger DFI Initialization in simulation.
 *
 * Initialize the PHY to mission mode as follows:
 * -# Set the PHY input clocks to the desired frequency.
 * -# Initialize the PHY to mission mode by performing DFI Initialization. See
 * PUB Databook section on "DFI Frequency Change" for details on this step.
 *
 * \note to ensure DRAM MR state matches the destination frequency, the first
 * dfi_freq[4:0] must be to a PState matching the last trained PState.  For
 * Example 1) if 3 PStates are used and only 1D training is run, the first
 * dfi_freq[4:0] must be 0x3 on the first dfi_init_start transaction.
 * PState selected via dfi_freq[4:0] must match the
 * Example 2) if 3 PStates are used with 2D training enabled, the first
 * dfi_freq[4:0] must be 0x0 on the first dfi_init_start transaction.
 *
 * \note The PHY training firmware initializes the DRAM state. if skip
 * training is used, the DRAM state is not initialized.
 *
 *
 *
 * \returns void
 */
void ddrphy_j_enter_mission_mode(struct artosyn_dram_info *dram)
{
	//##############################################################
	// (J) Initialize the PHY to Mission Mode through DFI Initialization
	// Initialize the PHY to mission mode as follows:
	// 1. Set the PHY input clocks to the desired frequency.
	// 2. Initialize the PHY to mission mode by performing DFI Initialization.
	//    Please see the DFI specification for more information. See the DFI frequency bus encoding in section <XXX>.
	// Note: The PHY training firmware initializes the DRAM state. if skip
	// training is used, the DRAM state is not initialized.
	//##############################################################
	//artosyn_dphy_pll_cfgs_t pll_cfgs;

	printf("(J) Initialize Mission Mode\n");

	//memcpy(&pll_cfgs, &dram->pll_default_cfgs,
	//       sizeof(artosyn_dphy_pll_cfgs_t));

	//pll_cfgs.ddr_mem_freq = dram->params.freq0;

	//ddrphy_pll_config(&pll_cfgs);

	//ddrphy_ssc_config(&pll_cfgs);
}

/** \brief Tags a register if tracking is enabled in the register
 * interface
 *
 * during PhyInit registers writes, keeps track of address
 * for the purpose of restoring the PHY register state during PHY
 * retention exit process.  Tracking can be turned on/off via the
 * dwc_ddrphy_phyinit_regInterface startTrack, stopTrack instructions. By
 * default tracking is always turned on.
 *
 * \return 0: not tracked 1: tracked
 */
int ddrphy_track_one_reg(uint32_t adr) {
	int regIndx=0;
	int foundReg=0;

	// search register array the address,
	for (regIndx = 0; regIndx < *NumRegSaved; regIndx++) {
		if (RetRegList[regIndx].Address == adr) {
			foundReg=1;
			return 1;
		}
	}

	// register not found, so add it.
	if (!foundReg) {
		if (*NumRegSaved == MAX_NUM_RET_REGS) {
			return 0;
		}
		RetRegList[regIndx].Address = adr;

		(*NumRegSaved)++;
		return 1;
	} else {
		// should never get here
		return 0;
	}
}

void ddrphy_track_all_regs(struct artosyn_dram_info *dram) {
	user_input_basic_t *basic = &dram->userInputBasic;
	runtime_config_t *runtime = &dram->runtimeConfig;

	/// In short the implementation of this function performs tasks:

	// --------------------------------------------------------------------------
	/// 1. Enable tracking of training firmware result registers\n
	///    See firmware training App Note section "IO Retention" for reference
	///    table registers that need to be saved.
	///
	///    \note  The tagged registers in this step are in
	///    addition to what is automatically tagged during Steps C to I.
	///
	// --------------------------------------------------------------------------

	// 95% of users should not require to change the code below.
	int pstate;
	int anib;
	int byte;
	int nibble;
	int lane;
	int p_addr;
	int c_addr;
	int u_addr;
	int b_addr;
	int r_addr;

	ddrphy_track_one_reg(tMASTER | csr_PllCtrl3_ADDR);

	// Non-PState Dbyte Registers
	for (byte = 0; byte < basic->NumDbyte; byte++) {
		c_addr = byte << 12;
		for (lane=0; lane <= r_max ; lane++) {
			r_addr = lane << 8;
			ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg0_ADDR);
			ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg1_ADDR);
			ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg2_ADDR);
			ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg3_ADDR);
		} // r_addr
	} // c_addr

	// PState variable registers
	for (pstate = 0; pstate < basic->NumPStates; pstate++) {
		p_addr = pstate << 20;
		ddrphy_track_one_reg(p_addr | tMASTER | csr_VrefInGlobal_ADDR);
		ddrphy_track_one_reg(p_addr | tMASTER | csr_DqsPreambleControl_ADDR);
		// Anig Registers
		for (anib = 0; anib < basic->NumAnib; anib++) {
			c_addr = anib << 12;
			ddrphy_track_one_reg(p_addr | tANIB | c_addr | csr_ATxDly_ADDR);
		}
		// Dbyte Registers
		for (byte = 0; byte < basic->NumDbyte; byte++) {
			c_addr = byte << 12;
			ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | csr_DFIMRL_ADDR);
			for (nibble=0; nibble <= b_max ; nibble++) {
				b_addr = nibble << 8;
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | b_addr | csr_DqDqsRcvCntrl_ADDR);
			}
			for (nibble=0; nibble < 2; nibble++) {
				u_addr = nibble << 8;
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg0_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg1_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg2_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg3_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg0_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg1_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg2_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg3_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg0_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg1_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg2_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg3_ADDR);
			} // nibble
			for (lane=r_min; lane <= r_max; lane++) {
				r_addr = lane << 8;
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg0_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg1_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg2_ADDR);
				ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg3_ADDR);
			} // r_addr
			for (lane = 0; lane <= r_max ; lane++) {
				r_addr = lane << 8;
				ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_VrefDAC0_ADDR);
				ddrphy_track_one_reg(tDBYTE | c_addr | r_addr | csr_VrefDAC1_ADDR);
			}
		} // c_addr
		// PIE Registers
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR1_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR2_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR3_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR4_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR5_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR6_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR7_ADDR);
		ddrphy_track_one_reg(p_addr | tINITENG | csr_Seq0BGPR8_ADDR);
		// Master Registers
		ddrphy_track_one_reg(p_addr | tMASTER | csr_DllGainCtl_ADDR);
		ddrphy_track_one_reg(p_addr | tMASTER | csr_DllLockParam_ADDR);
	} // p_addr

	// Master Registers
	ddrphy_track_one_reg(tMASTER | csr_HwtCAMode_ADDR);

	if (runtime->Train2D) {
		for (byte = 0; byte < basic->NumDbyte; byte++) {
			c_addr = byte << 12;

			for (pstate = 0; pstate < basic->NumPStates; pstate++) {
				p_addr = pstate << 20;
				for (nibble=0; nibble < 2; nibble++) {
					u_addr = nibble << 8;
					ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg0_ADDR);
					ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg1_ADDR);
					ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg2_ADDR);
					ddrphy_track_one_reg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg3_ADDR);
				} // nibble
			} // p_addr
		} // byte
	}
}

int ddrphy_mem_test(struct artosyn_dram_info *dram) {
	unsigned long long addr;
	unsigned long long size;
	unsigned int i_data, i_addr;
	unsigned long long val, data[5] = {0x1111111111111111, 0xFFFFFFFFFFFFFFFF, 0x5A5A5A5A5A5A5A5A, 0xA5A5A5A5A5A5A5A5, 0x0000000000000000};
	unsigned int val_int, data_int[5] = {0x11111111, 0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5, 0x00000000};
	int ret = 0;

#if defined(CONFIG_ARTOSYN_AR9301)
	size = ((unsigned long long)dram->params.density) * 128 *
			  (2 >> dram->params.ddr_hbm);
#elif defined(CONFIG_ARTOSYN_AR9311)
	size = ((unsigned long long)dram->params.density) * 128 * 1;
#endif

#ifdef CONFIG_SPL_OS_BOOT
	size = 0x8000000;
#endif
#if 0
	addr = 0x20000000;
	for (i_addr = 0; i_addr < 0x10000; i_addr += 4) {
		addr = 0x20000000 + i_addr;
		writel(addr, addr);
		flush_dcache_range((unsigned long)&addr, ((unsigned long)&addr) + 4);
		if (i_addr % 32 == 0)
			printf("\n0x%x: ", addr);
		printf("0x%08x ", addr);
	}
	printf("\n");
	for (i_addr = 0; i_addr < 0x10000; i_addr += 4) {
		addr = 0x20000000 + i_addr;
		if (i_addr % 32 == 0)
			printf("\n0x%x: ", addr);
		printf("0x%08x ", readl(addr));
	}
	printf("\n");
#endif
	addr = 0x20000000;
	for (i_data = 0; i_data < 5; i_data++) {
		writel(data_int[i_data], addr);
		flush_dcache_range((unsigned long)&addr, ((unsigned long)&addr) + 4);
		val_int = readl(addr);
		if (val_int != data_int[i_data]) {
			printf("mem_test failed, addr 0x%08x%08x val 0x%08x(0x%08x)!\n",
				(addr >> 32) & 0xffffffff, addr & 0xffffffff,
				val_int, data_int[i_data]);
			ret = -1;
		}
	}
	addr = 0x20000004;
	for (i_data = 0; i_data < 5; i_data++) {
		writel(data_int[i_data], addr);
		flush_dcache_range((unsigned long)&addr, ((unsigned long)&addr) + 4);
		val_int = readl(addr);
		if (val_int != data_int[i_data]) {
			printf("mem_test failed, addr 0x%08x%08x val 0x%08x(0x%08x)!\n",
				(addr >> 32) & 0xffffffff, addr & 0xffffffff,
				val_int, data_int[i_data]);
			ret = -1;
		}
	}
	for (i_addr = 8; i_addr < size; i_addr = (i_addr << 1)) {
		addr = 0x20000000 + i_addr;
		for (i_data = 0; i_data < 5; i_data++) {
			writeq(data[i_data], addr);
			flush_dcache_range((unsigned long)&addr, ((unsigned long)&addr) + 8);
			val = readq(addr);
			if (val != data[i_data]) {
				printf("mem_test failed, addr 0x%08x%08x val 0x%08x%08x(0x%08x%08x)!\n",
					(addr >> 32) & 0xffffffff, addr & 0xffffffff,
					(val >> 32) & 0xffffffff, val & 0xffffffff,
					(data[i_data] >> 32) & 0xffffffff, data[i_data] & 0xffffffff);
				ret = -1;
			}
		}
	}

	if (ret == 0)
		printf("mem_test PASS!\n");

	return ret;
}

void ddrphy_save_regs(struct artosyn_dram_info *dram, bool flash) {
	int regIndx=0;
	uint16_t data;

	ddrphy_track_all_regs(dram);

	/*
	 * go through all the tracked registers, issue a register
	 * read and place the result in the data structure for future
	 * recovery.
	 */
	debug("(#) Save regs\n");

	writel(0x0, dram->ddr_phy_base + 0x00340000);
	writel(0x3, dram->ddr_phy_base + 0x00300200);

	for (regIndx = 0; regIndx < *NumRegSaved; regIndx++) {
		data = ddrphy_io_read16(dram, RetRegList[regIndx].Address);
		RetRegList[regIndx].Value = data;
	}

	if (!flash)
		return;

	/*
	 * Save RTC SRAM data to the retention field in vendor partition,
	 * Skip top of 0x10 which is the address of bl1 warmboot entry,
	 * RET_REG_NUM_OFFSET = 0x10
	 */
	memset(buff, 0, RET_FLASH_DATA_SIZE);
	memcpy(buff + RET_REG_NUM_OFFSET, (void *)(RET_FLASH_DATA_ADDR + RET_REG_NUM_OFFSET),
		RET_FLASH_DATA_SIZE - RET_REG_NUM_OFFSET);

#ifdef CONFIG_SPL_OS_BOOT
	if (ddrphy_mem_test(dram) == -1)
		*((uint32_t *)(buff + RET_STATE_OFFSET)) = RET_FLASH_STATE_INVALID;
	else
		*((uint32_t *)(buff + RET_STATE_OFFSET)) = RET_FLASH_STATE_VALID;
#else
	*((uint32_t *)(buff + RET_STATE_OFFSET)) = RET_FLASH_STATE_INVALID;
#endif
#ifdef CONFIG_MMC
	spl_mmc_write_to_hw_partition(0, dram->restore_offset / 512, RET_FLASH_DATA_SIZE / 512, buff);
#endif
}

int ddrphy_restore_regs(struct artosyn_dram_info *dram, bool flash) {
	int regIndx=0;

	/*
	 * write PHY registers based on Address, Data value pairs
	 * stores in RetRegList
	 */
	debug("(#) Restore regs\n");

	if (!flash)
		goto rtc_restore;
	/*
	 * Restore RTC SRAM data to the mem from vendor partition,
	 * copy to RTC SARM,
	 * Skip top of 0x10 which is the address of bl1 warmboot entry,
	 * RET_REG_NUM_OFFSET = 0x10
	 */
#ifdef CONFIG_MMC
	spl_mmc_read_from_hw_partition(0, dram->restore_offset / 512, RET_FLASH_DATA_SIZE / 512, buff);
#endif
	memcpy((void *)(RET_FLASH_DATA_ADDR + RET_REG_NUM_OFFSET), buff + RET_REG_NUM_OFFSET,
		RET_FLASH_DATA_SIZE - RET_REG_NUM_OFFSET);
	if (*((uint32_t *)RET_STATE_ADDR) != RET_FLASH_STATE_VALID)
		return -1;

rtc_restore:
	writel(0x0, dram->ddr_phy_base + 0x00340000);
	writel(0x3, dram->ddr_phy_base + 0x00300200);
	writel(0x1, dram->ddr_phy_base + 0x00080224);

	for (regIndx = 0; regIndx < *NumRegSaved; regIndx++) {
		ddrphy_io_write16(dram, RetRegList[regIndx].Address,RetRegList[regIndx].Value);
	}

	return 0;
}

int ddrphy_init_seq(struct artosyn_dram_info *dram)
{
	runtime_config_t *runtime = &dram->runtimeConfig;
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	int pstate;

	// Initialize structures
	ddrphy_init_struct(dram);

	// Enter user input
	ddrphy_set_default(dram);

	// User-editable function to override any user input set in dwc_ddrphy_phyinit_enterUserInput()
	// dwc_ddrphy_phyinit_userCustom_overrideUserInput();

	// Re-calculate Firmware Message Block input based on final user input
	ddrphy_calc_mb(dram);

	// Printing values of user input
	// if (debug == 1)
	// 	dwc_ddrphy_phyinit_print_dat();

	// (A) Bring up VDD, VDDQ, and VAA
	ddrphy_a_bringup_power();

	// (B) Start Clocks and Reset the PHY
	//ddrphy_b_start_clk_rst_phy(dram);

	// mail init
	ddrphy_init_mail(dram);

	// (C) Initialize PHY Configuration
	ddrphy_c_init_phy_cfg(dram);

	// Customize any register write desired; This can include any CSR not covered by PhyInit or user wish
	// to override values calculated in step_C
	// dwc_ddrphy_phyinit_userCustom_customPreTrain ();

	// if (runtime->skip_training == 1) {
		// Skip running training firmware entirely
		// dwc_ddrphy_phyinit_progCsrSkipTrain(runtime->skip_training);
	// } else {
		// if (runtime->skip_training == 2) {
			// Only execute training firmware to initialize the DRAM and
			// skip all training steps.
			// runtime->Train2D = 0;
			// Set sequence Ctrl to 0x1 to only do device initialization.
			// for (pstate = 0; pstate < basic.NumPStates; pstate++) {
				// mb_DDR3U_1D[pstate].SequenceCtrl = 0x1;
			// }

			// dwc_ddrphy_phyinit_progCsrSkipTrain(runtime->skip_training);
		// }

	if (warmboot && dram->warmboot_support) {
		ddrphy_restore_regs(dram, false);
		goto load_pie;
	} else if (runtime->skip_train) {
		if (ddrphy_restore_regs(dram, true) == 0) {
			goto load_pie;
		}
	}

	switch (advanced->PhyInitSequenceNum) {
	case 1: // Run 2D P0  after 1D P0 so 1D P1/P2/P3 can be run at the vrefDAC0 settings trained by 2D. See Share2DVrefResult messageblock field.
                // (D) Load the IMEM Memory for P0 1D training

		printf("(D) Load IMEM 1D\n");
		ddrphy_d_load_imem(dram, 0);	// 0==1D

		for (pstate = 0; pstate < basic->NumPStates; pstate++) {
			// (E) Set the PHY input clocks to the desired frequency
			printf("(E) Set PHY input clocks\n");
			ddrphy_e_set_dfi_clk(dram, pstate);
			// Note: this routine implies other items such as DfiFreqRatio, DfiCtlClk are also set properly.
			// Because the clocks are controlled in the SOC, external to the software and PHY, this step intended to be
			// replaced by the user with the necessary SOC operations to achieve the new input frequency to the PHY.

			// (F) Write the Message Block parameters for the training firmware
			printf("(F) Load DMEM 1D\n");
			ddrphy_f_load_dmem(dram, pstate, 0);  // 2nd argument: 0==1D

			// (G) Execute the Training Firmware
			printf("(G) Execute\n");
			ddrphy_g_exec_fw(dram);

			// (H) Read the Message Block results
			printf("(H) Read msg blk results\n");
			ddrphy_h_read_msg_blk(dram, 0);

			// Now optionally perform P0 2D training for protocols that allow it
			if (pstate == 0 && runtime->Train2D == 1) {
				// Step names here mimic the 1D lettering (E,F,G,H).
				// They can be found in the Training Firmware Application Note

				// if (debug == 1)
				// 	dwc_ddrphy_phyinit_printMb(pstate, 1);  // 2nd argument: 1==2D

				// 2D-E Set the PHY input clocks to the highest frequency
				printf("2D-E Set PHY input clocks highest\n");
				ddrphy_e_set_dfi_clk(dram, 0);  // pstate==0; DfiClk fixed 2:1 ratio with MemClk

				// 2D-F
				printf("2D-F\n");
				ddrphy_d_load_imem(dram, 1); // 2D image

				// 2D-F, cont.  Write the Message Block parameters for the training firmware
				printf("2D-F, Load DMEM 2D\n");
				ddrphy_f_load_dmem(dram, 0, 1);  // 1st argument: pstate==0; 2nd argument: 1==2D

				// 2D-G Execute the Training Firmware
				printf("2D-G Execute\n");
				ddrphy_g_exec_fw(dram);

				// 2D-H Read the Message Block results
				printf("2D-H Read msg blkresults\n");
				ddrphy_h_read_msg_blk(dram, 1);

				if (basic->NumPStates == 1)
					break;

				// (D) Reload the IMEM Memory for P1,P2,and P3 1D
				printf("(D) Reload IMEM 1D\n");
				ddrphy_d_load_imem(dram, 0);  // 0==1D
			}
		}
		break;
	default: // Run all 1D power states, then 2D P0, to reduce total Imem/Dmem loads.
		// (D) Load the IMEM Memory for 1D training
		break;
#if 0
		printf("(D) Load IMEM 1D\n");
		ddrphy_d_load_imem(dram, 0);  // 0==1D

		for (pstate = 0; pstate < basic->NumPStates; pstate++) {
			// Printing values of message block input/inout for current p-state
			//if (runtime->debug == 1)
			//    dwc_ddrphy_phyinit_printMb(pstate, 0);  // 2nd argument: 0==1D

			// (E) Set the PHY input clocks to the desired frequency
			printf("(E) Set PHY input clocks\n");
			ddrphy_e_set_dfi_clk(dram, pstate);
			// Note: this routine implies other items such as DfiFreqRatio, DfiCtlClk are also set properly.
			// Because the clocks are controlled in the SOC, external to the software and PHY, this step intended to be
			// replaced by the user with the necessary SOC operations to achieve the new input frequency to the PHY.

			// (F) Write the Message Block parameters for the training firmware
			printf("(F) Load DMEM 1D\n");
			ddrphy_f_load_dmem(dram, pstate, 0);  // 2nd argument: 0==1D

			// (G) Execute the Training Firmware
			printf("(G) Execute\n");
			ddrphy_g_exec_fw(dram);

			// (H) Read the Message Block results
			printf("(H) Read msg blk results\n");
			ddrphy_h_read_msg_blk(dram, 0);
		}

		// Now optionally perform 2D training for protocols that allow it
		if (runtime->Train2D == 1) {
			// Step names here mimic the 1D lettering (E,F,G,H).
			// They can be found in the Training Firmware Application Note

			//if (debug == 1)
			//    dwc_ddrphy_phyinit_printMb(pstate, 1);  // 2nd argument: 1==2D

			// 2D-E Set the PHY input clocks to the highest frequency
			printf("2D-E Set PHY input clocks highest\n");
			ddrphy_e_set_dfi_clk(dram, 0);  // pstate==0; DfiClk fixed 2:1 ratio with MemClk

			// 2D-F
			printf("2D-F\n");
			ddrphy_d_load_imem(dram, 1); // 2D image

			// 2D-F, cont.  Write the Message Block parameters for the training firmware
			printf("2D-F, Load DMEM 2D\n");
			ddrphy_f_load_dmem(dram, 0, 1);  // 1st argument: pstate==0; 2nd argument: 1==2D

			// 2D-G Execute the Training Firmware
			printf("2D-G Execute\n");
			ddrphy_g_exec_fw(dram);

			// 2D-H Read the Message Block results
			printf("2D-H Read msg blkresults\n");
			ddrphy_h_read_msg_blk(dram, 1);
		}
#endif
	}
	// }

#ifdef AR9301_SDRAM_DIAG
	ddrphy_diag_load_and_exec(dram);

	return 0;
#endif

load_pie:
	// (I) Load PHY Init Engine Image
	ddrphy_i_load_pie_img(dram);

	// Customize any CSR write desired to override values programmed by firmware or ddrphy_i_load_pie_img()
	// dwc_ddrphy_phyinit_userCustom_customPostTrain ();

	// if (runtime->RetEn)
	// {
	// 	// Save value of tracked registers for retention restore sequence.
	// 	dwc_ddrphy_phyinit_userCustom_saveRetRegs();
	// }

	// (J) Initialize the PHY to Mission Mode through DFI Initialization
	ddrphy_j_enter_mission_mode(dram);

	return 0;
}

int ddrphy_init(struct artosyn_dram_info *dram)
{
	runtime_config_t *runtime = &dram->runtimeConfig;

	pr_info("DDRPHY init start\n");

	if (runtime->Train2D != 0 && runtime->Train2D != 1) {
		pr_err("Train2d(%d) invalid!\n", runtime->Train2D);
	}

	// Execute phyinit sequence
	ddrphy_init_seq(dram);

	pr_info("DDRPHY init finish.\n");

	return 0;
}

static void ar9301_caculate_dfi(artosyn_dram_configs_t *cfgs,
				ar9301_dram_ctrl_t *dram_ctrl)
{
	unsigned int ardptrinitval = 2;

	dram_ctrl->dfi_t_ctrl_delay	= ardptrinitval / 2 + 3;	// (ARdPtrinitVal/2+MISC+3)=2/2+0+3=4
	dram_ctrl->dfi_t_rddata_en	= cfgs->tcl - 5;			// 9
	dram_ctrl->dfi_tphy_wrdata	= 2;
	dram_ctrl->dfi_tphy_wrlat	= cfgs->tcwl - 5;			// 5
	dram_ctrl->dfi_tphy_rdcslat	= cfgs->tcl - 5;
	dram_ctrl->dfi_tphy_wrcslat	= dram_ctrl->dfi_tphy_wrlat;		// WL - 5 + 2*(MISC-WR) = 10-5=5
	dram_ctrl->dfi_t_wrdata_delay	= dram_ctrl->dfi_t_ctrl_delay / 2 + 5;	// (dram_ctrl->dfi_t_ctrl_delay + (6+8/2)+Trained_RxDqsDly)/2 =
	dram_ctrl->dfi_t_dram_clk_enable	= dram_ctrl->dfi_t_ctrl_delay - 2;
	dram_ctrl->dfi_t_dram_clk_disable	= dram_ctrl->dfi_t_ctrl_delay - 2;
}

static void ar9301_caculate_time(artosyn_dram_configs_t *cfgs,
				 ar9301_dram_ctrl_t *dram_ctrl)
{
	unsigned int trc = 0;	// ns
	unsigned int tpl = 0;
	unsigned int tal = 0;
	unsigned int t_rtp;
	unsigned int t_wtr;

	dram_ctrl->clk_mode	= cfgs->clk_mode;
#if 0
	dram_ctrl->rd_preamble	= cfgs->rd_preamble;
	dram_ctrl->wr_preamble	= cfgs->wr_preamble;
#endif
	unsigned int tck_10ns = cfgs->tck_freq / 100 + 1;
	unsigned int tck_7_5ns = (15 * cfgs->tck_freq) / 2000 + 1;
	unsigned int tck_6ns = (6 * cfgs->tck_freq) / 1000 + 1;
	unsigned int tck_5ns = cfgs->tck_freq / 200 + 1;
	unsigned int tck_15ns = (15 * cfgs->tck_freq) / 1000 + 1;

	dram_ctrl->t_rfc_nom_x1_x32 = (cfgs->trefi * cfgs->tck_freq) / 320;	// related with Refresh mode, density

	dram_ctrl->t_rfc_min	= (cfgs->trfc * cfgs->tck_freq) / 100 + 1;	// related with Refresh mode, density

	dram_ctrl->t_wl		= (tal + cfgs->tcwl + 1) / 2;			// 8
	dram_ctrl->t_rl		= (tal + cfgs->tcl + 1) / 2;			// 11
	dram_ctrl->t_wr		= (15 * cfgs->tck_freq) / 1000 + 1;		// 13 (twr min value is 5)
	t_rtp			= tck_7_5ns > 2 ? tck_7_5ns : 2;		// 6
	t_wtr			= t_rtp;					// 6
	dram_ctrl->t_wr2pre	= dram_ctrl->t_wl + 2 + dram_ctrl->t_wr ;	// 23 22
	if (cfgs->tck_freq < 333) {
		dram_ctrl->t_ras_min	= (37.5 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		dram_ctrl->t_faw	= (50 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		//dram_ctrl->t_xp	= tck_7_5ns;					// 5 ok
		dram_ctrl->t_xp		= tck_6ns * 4;					// 5 ok
		trc = 53;								// ns
	} else if (cfgs->tck_freq < 400) {
		dram_ctrl->t_ras_min	= (36 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		dram_ctrl->t_faw	= (45 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		//dram_ctrl->t_xp	= tck_6ns;					// 5 ok
		dram_ctrl->t_xp		= tck_6ns * 4;					// 5 ok
		trc = 49;								// ns
	} else if (cfgs->tck_freq < 466) {
		dram_ctrl->t_ras_min	= (35 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		dram_ctrl->t_faw	= (40 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		//dram_ctrl->t_xp	= tck_6ns;					// 5 ok
		dram_ctrl->t_xp		= tck_6ns * 4;					// 5 ok
		trc = 48;								// ns
	} else {
		dram_ctrl->t_ras_min	= (34 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		dram_ctrl->t_faw	= (35 * cfgs->tck_freq) / 1000 + 1;		// 24 ok
		//dram_ctrl->t_xp	= tck_6ns;					// 5 ok
		dram_ctrl->t_xp		= tck_6ns * 4;					// 5 ok
		trc = 47;								// ns
	}

	dram_ctrl->t_ras_max    = ((cfgs->trefi * 9 * cfgs->tck_freq) / 1024 -1) / 2;   // 9xtREFI // 54 ok
	dram_ctrl->t_rd2pre	= t_rtp;					// 6 2
										// DDR4: Max of following two equations:
										// tAL + max (tRTP, 4) or, RL + BL/2 - tRP (*).
	dram_ctrl->t_rc		= (trc * cfgs->tck_freq) / 1000 + 1;		// 39 37
	dram_ctrl->t_rd2wr	= (cfgs->tcl - cfgs->tcwl + 1) / 2 + 3;		// 6 ok
	dram_ctrl->t_wr2rd	= (cfgs->tcwl + tpl + 5) / 2 + t_wtr;		// 16 ok
	dram_ctrl->t_mrw	= 0;						// for LP2,LP3,LP4
	dram_ctrl->t_mrd	= 2;
	dram_ctrl->t_mod	= (tck_15ns < 6) ? 6 : tck_15ns;		// max(12nCK, 15ns)
	dram_ctrl->t_rcd	= (cfgs->trcd - tal + 1) / 2;			// 11 ok
	dram_ctrl->t_ccd_l	= 2;						// 4 ok
	dram_ctrl->t_ccd_s	= 0;						// no use in DDR3
	dram_ctrl->t_rrd_l	= cfgs->tck_freq > 400 ? tck_6ns :
				  cfgs->tck_freq > 266 ? tck_7_5ns :
				  tck_10ns;					// 6 ok
	dram_ctrl->t_rrd_l	= (dram_ctrl->t_rrd_l < 2) ? 2 : dram_ctrl->t_rrd_l;	//max (4nCK, ?ns)
	dram_ctrl->t_rrd_s	= 0;						// no use in DDR3
	dram_ctrl->t_rp		= cfgs->trp / 2 + 1;				// 12 ok
	dram_ctrl->t_cksrx	= tck_10ns > 3 ? tck_10ns : 3;			// max(5nck, 10ns)
	dram_ctrl->t_cksre	= dram_ctrl->t_cksrx;
	dram_ctrl->t_cke	= (tck_7_5ns > 2) ? tck_7_5ns : 2;		// 5 4 7.5ns for ddr3_800
	dram_ctrl->t_ckesr	= dram_ctrl->t_cke + 1;				// 6 5
	dram_ctrl->t_ckpde	= dram_ctrl->t_cksre;				// 9 8
	dram_ctrl->t_ckpdx	= dram_ctrl->t_cksrx;				// 9 8
	dram_ctrl->t_xs_x32	= (dram_ctrl->t_rfc_min + tck_10ns) / 32 + 1;	// 5 tRFC+10ns
	dram_ctrl->t_xs_fast_x32 = dram_ctrl->t_xs_x32;				// 4 tRFC+10ns
	dram_ctrl->t_xs_abort_x32 = dram_ctrl->t_xs_x32;			// 4
	dram_ctrl->t_xs_dll_x32	= 8;						// 12 1024/854/768
	//dram_ctrl->t_zqoper = (cfgs->tck_freq > 400) ? (tck_10ns << 3) : 6*tck_10ns+tck_5ns;
	dram_ctrl->t_zqoper = tck_10ns * 32;
	dram_ctrl->t_zqoper = (dram_ctrl->t_zqoper > 128) ? dram_ctrl->t_zqoper : 128;	//max(256nCK, 320ns)
	dram_ctrl->t_zqcs = dram_ctrl->t_zqoper >> 2; 

#if 0
	dram_ctrl->t_sync_gear	= 14;
	dram_ctrl->t_cmd_gear	= 12;
	dram_ctrl->t_gear_setup	= 1;
	dram_ctrl->t_gear_hold	= 1;
	dram_ctrl->t_post_mpsm_x32 = dram_ctrl->t_xs_x32 +
				     dram_ctrl->t_xs_dll_x32;			// 17
	dram_ctrl->t_mpx_lh	= 12;
	dram_ctrl->t_mpx_s	= 1;
	dram_ctrl->t_ckmpe	= 14;
	dram_ctrl->t_cmdcke	= 0;						//for LP4
	dram_ctrl->t_mrd_pda	= 8;
#endif
	dram_ctrl->wr_odt_hold = 6;						// write preamble = 1ck, 5 + wr_preamble
	dram_ctrl->wr_odt_delay = 0;						// dfi_t_cmd_lat = 0
	dram_ctrl->rd_odt_hold = 6;						// 0x6
	dram_ctrl->rd_odt_delay = (cfgs->tcl - cfgs->tcwl);			// cl - cwl - rd_preamble - wr_preable + dfi_t_cmd_lat
}

static void ar9301_caculate_mode_register(struct artosyn_dram_info *dram,
					  artosyn_dram_configs_t *cfgs,
					  ar9301_dram_ctrl_t *dram_ctrl)
{
	uint32_t mr_regs[3];

	ar9301_cal_mode_regs(dram, cfgs->tcl, cfgs->tck_freq, cfgs->tcwl, mr_regs);

	dram_ctrl->mr0_reg = mr_regs[0];
	dram_ctrl->mr1_reg = mr_regs[1];
	dram_ctrl->mr2_reg = mr_regs[2];
}

static void ar9301_dmc_change_freq(struct artosyn_dram_info *dram)
{
	artosyn_dphy_pll_cfgs_t pll_cfgs;
	user_input_basic_t *basic = &dram->userInputBasic;
	unsigned int i = 0, mr[3];

	// 1
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0490, 0x00000000);	// (           PCTRL_0) 'h00000001 uMCTL2 0x0490
									//			[0] port_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0540, 0x00000000);	// (           PCTRL_1) 'h00000001 uMCTL2 0x0540
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x05f0, 0x00000000);	// (           PCTRL_2) 'h00000001 uMCTL2 0x05f0
	// 2

#ifdef AR9301_SDRAM_DEBUG
	printf("while (PSTAT =! 0)\n");
#else
	while (ar9301_sdram_read(dram->ddr_ctrl_base + 0x03fc) != 0)	// (             PSTAT) PSTAT.rd_port_busy ==0 PSTAT.wr_port_busy_n ==0
		udelay(1);
#endif
	// 3 4 required only if SBR instantiated, so do nothing
	// 5 LPDDR2/3/4 only, so do nothing
	// 6
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0304, 0x00000002);	// (              DBG1) 'h00000000 uMCTL2 0x0304
									//			[0] dis_dq [1] dis_hif
	// 7 8
	ar9301_sdram_read(dram->ddr_ctrl_base + 0x0308);		// (            DBGCAM)
									//			[25] dbg_rd_q_empty
									//			[26] dbg_wr_q_empty
									// 			[28] rd_data_pipeline_empty
									//			[29] wr_datt_pipeline_empty
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((DBGCAM & 0x36000000) == 0);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x0308)
		& 0x36000000) == 0)
		udelay(1);
#endif
	// 9
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0198, 0x07e0b011);	// (         DFILPCFG0) 'h07e0b011 uMCTL2 0x0198
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((DFISTAT & 0x00000002) != 0);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x1bc) &	// (           DFISTAT) 'h00000000  uMCTL2
		0x00000002) != 0)					//			[0] dfi_init_complete
		udelay(1);						//			[1] dfi_lp_ack
#endif
	// 10
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((STAT & 0x7) == 0x3);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x0004) &	// (              STAT) 'h00000000  uMCTL2
		0x7) == 0x3)						//			[x:0] operating_mode: init normal power-down self-refresh
		udelay(1);
#endif
	// 11
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0030, 0x00000020);	// (            PWRCTL) 'h00000003 uMCTL2 0x0030
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((STAT & 0x7) != 0x3);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x0004) &	// (              STAT) 'h00000000  uMCTL2
		0x7) != 0x3)						//			[x:0] operating_mode: init normal power-down self-refresh
		udelay(1);
#endif
	// 12
	ar9301_sdram_read(dram->ddr_ctrl_base + 0x0308);		// (            DBGCAM)
									//			[25] dbg_rd_q_empty
									//			[26] dbg_wr_q_empty
									// 			[28] rd_data_pipeline_empty
									//			[29] wr_datt_pipeline_empty
#ifdef AR9301_SDRAM_DEBUG
	printf("while((DBGCAM & 0x30000000) == 0);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x0308)
		& 0x30000000) == 0)
		udelay(1);
#endif

	// 13 14 15
	memcpy(&pll_cfgs, &dram->pll_default_cfgs,
	       sizeof(artosyn_dphy_pll_cfgs_t));

	for (i = 0; i < 4; i++) {
		if (dram->params.freq0 == basic->Frequency[i]) {
			mr[0] = mb_DDR3U_1D[i].MR0;
			mr[1] = mb_DDR3U_1D[i].MR1;
			mr[2] = mb_DDR3U_1D[i].MR2;
			break;
		}
	}

	pll_cfgs.ddr_mem_freq = basic->Frequency[i];
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b0,
			   0x00000030 | (i << 8));			// (           DFIMISC) 'h00000001 uMCTL2
	// 16
	ddrphy_pll_config(&pll_cfgs);

	// 17 todo
	// 18
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b0,		// (           DFIMISC) 'h00000001 uMCTL2
			   0x00000010 | (i << 8));
	// 19
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((DFI_STAT & 0x00000001) != 1)\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x1bc) &	// (           DFISTAT) 'h00000000  uMCTL2
		0x00000001) != 1)					//			[0] dfi_init_complete
		udelay(1);						//			[1] dfi_lp_ack
#endif

	// 20
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0030, 0x00000000);	// (            PWRCTL) 'h00000003 uMCTL2 0x0030
#ifdef AR9301_SDRAM_DEBUG
	printf("while ((STAT & 0x7) == 0x3);\n");
#else
	while ((ar9301_sdram_read(dram->ddr_ctrl_base + 0x0004) &	// (              STAT) 'h00000000  uMCTL2
		0x7) == 0x3)						//			[x:0] operating_mode: init normal power-down self-refresh
		udelay(1);
#endif
	// 21
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0198, 0x07e0b011);	// (         DFILPCFG0) 'h07e0b011 uMCTL2 0x0198

	// 22
	for (i = 0; i < 3; i++) {
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0010,
				   0x00000010 | (i << 12));		// (           MRCTRL0) 'h0000c010 uMCTL2 0x0010 //  MRi, Rank0
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0014, mr[i]);// (           MRCTRL1) 'h0002c4a9 uMCTL2 0x0014 //
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0010,
				   0x00000010 | (i << 12) | (1 << 31));	// (           MRCTRL0) 'h0000c010 uMCTL2 0x0010 //  MRi, Rank0
	}

	// 23
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0304, 0x00000000);	// (              DBG1) 'h00000000 uMCTL2 0x0304
									//			[0] dis_dq [1] dis_hif
}

static int ar9301_sdram_init(struct artosyn_dram_info *dram)
{
	// config params base on input params from dts
	artosyn_dram_configs_t cfgs;
	ar9301_dram_ctrl_t dram_ctrl0;
	ar9301_dram_ctrl_t dram_ctrl1;
	unsigned int rdata_t;

	printf("SDRAM init start...\n");

	ddrphy_b_start_clk_rst_phy(dram);
#if defined(CONFIG_ARTOSYN_AR9301)
	ar9301_sdram_write((unsigned int *)(PMU_BASE + PMU_CTRL_OFFSET),
			   0xacce55);

	rdata_t = ar9301_sdram_read((unsigned int *)(PMU_BASE + IP_PMU_CTRL_OFFSET));
	ar9301_sdram_write((unsigned int *)(PMU_BASE + IP_PMU_CTRL_OFFSET),//			DDR PwrOkIn = 1
			   rdata_t | (1 << 9));

	rdata_t = ar9301_sdram_read((unsigned int *)(PMU_BASE + IP_PMU_CTRL_OFFSET));
#elif defined(CONFIG_ARTOSYN_AR9311)
	rdata_t = ar9301_sdram_read((unsigned int *)(RTC_PMC_BASE + RTC_PMC_MSC));
	rdata_t |= 0x1;
	ar9301_sdram_write((unsigned int *)(RTC_PMC_BASE + RTC_PMC_MSC), rdata_t);
#endif
	ar9301_cfg_params(dram, dram->params.freq0, dram->params.density,
			  dram->params.refresh_mode, &cfgs);
	ar9301_caculate_dfi(&cfgs, &dram_ctrl0);
	ar9301_caculate_time(&cfgs, &dram_ctrl0);
	ar9301_caculate_mode_register(dram, &cfgs, &dram_ctrl0);

	ar9301_cfg_params(dram, dram->params.freq1, dram->params.density,
			  dram->params.refresh_mode, &cfgs);
	ar9301_caculate_dfi(&cfgs, &dram_ctrl1);
	ar9301_caculate_time(&cfgs, &dram_ctrl1);
	ar9301_caculate_mode_register(dram, &cfgs, &dram_ctrl1);

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0304, 0x00000001);	// (              DBG1) 'h00000001 uMCTL2 0x0304 //disable de-queque trans
	rdata_t = ar9301_sdram_read(dram->ddr_ctrl_base + 0x0004);	// (              STAT) 'h00000000 uMCTL2 0x0004 // read stat = 0

#if defined(CONFIG_ARTOSYN_AR9301)
	if (dram->params.ddr_hbm)
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0000,	// (              MSTR) 'h83040210 uMCTL2 0x0000
				   0x83041201 |
				   dram->params.clk_mode << 10);	// 			x16,freq_mode=0,2 rank, 1:2, burst8, half_DQ, burst_chop, ddr4

	else
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0000,	// (              MSTR) 'h83040210 uMCTL2 0x0000
				   0x83040201 |
				   dram->params.clk_mode << 10);	// 			x16,freq_mode=0,2 rank, 1:2, burst8, full_DQ, burst_chop, ddr4
#elif defined(CONFIG_ARTOSYN_AR9311)
	if (dram->params.ddr_hbm)
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0000,	// (              MSTR) 'h83040210 uMCTL2 0x0000
				   0x81041201 |
				   dram->params.clk_mode << 10);	// 			x16,freq_mode=0,2 rank, 1:2, burst8, half_DQ, burst_chop, ddr4

	else
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0000,	// (              MSTR) 'h83040210 uMCTL2 0x0000
				   0x81040201 |
				   dram->params.clk_mode << 10);	// 			x16,freq_mode=0,2 rank, 1:2, burst8, full_DQ, burst_chop, ddr4
#endif

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0010, 0x0000c010);	// (           MRCTRL0) 'h0000c010 uMCTL2 0x0010 //  MR4, Rank0
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0014, 0x0002a4a9);	// (           MRCTRL1) 'h0002c4a9 uMCTL2 0x0014 //
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x001c, 0x00000001);	// (           MRCTRL2) 'h401f361f uMCTL2 0x001c //
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0020, 0x00000000);	// (          DERATEEN) 'h00000042 uMCTL2 0x0020 //derate_en=0
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0024, 0xe4fa1694);	// (         DERATEINT) 'he4fa1694 uMCTL2 0x0024
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0028, 0x00000000);	// (             MSTR2) 'h00000001 uMCTL2 0x0028 //Don't care
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0034, 0x00024d01);	// (            PWRTMG) 'h00408804 uMCTL2 0x0034 // selfref=0x40, dpd=0x88, pd=0x04
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0030, 0x00000003);	// (            PWRCTL) 'h00000003 uMCTL2 0x0030 // selfref_en, powerdown_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0038, 0x002b0000);	// (           HWLPCTL) 'h002b0000 uMCTL2 0x0038 // hw_lp_en=0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0050, 0x00906070);	// (          RFSHCTL0) 'h00210000 uMCTL2 0x0050
									//			refresh_margin=2x32,refresh_to_x1_x32='h10,
									//			fefresh_burst=single, per bank refresh
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0060, 0x00000000);	// (          RFSHCTL3) 'h00000000 uMCTL2 0x0060
									//			fixed 4x, dis_auto_refresh=0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0064,		// (           RFSHTMG) 'h00928061 uMCTL2 0x0064
			   dram_ctrl0.t_rfc_nom_x1_x32 << 16 |          //			x32 mode, dram_ctrl.t_rfc_nom_x1_x32=7800/(1.876*32)=129=0x81,
			   dram_ctrl0.t_rfc_min);			//			dram_ctrl.t_rfc_min=160/1.876+1=86=0x56

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00c0, 0x00000000);	// (        CRCPARCTL0) 'h00000000 uMCTL2 0x00c0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00c4, 0x00001000);	// (        CRCPARCTL1) 'h00000000 uMCTL2 0x00c4 //dfi_t_phy_rdlat=16,crc_en,parity_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00d0, 0x0002012c);	// (             INIT0) 'h00020002 uMCTL2 0x00d0
									//			post_cke_x1024=1(tXPR), pre_cke_x1024=1(500us)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00d4, 0x00780001);	// (             INIT1) 'h00010002 uMCTL2 0x00d4
									//			dram_rstn_x1024=1(100ns),pre_ocd_x32=1
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00d8, 0x00000000);	// (             INIT2) 'h00004200 uMCTL2 0x00d8 //for lp2

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00dc,		// (             INIT3) 'h0a350001 uMCTL2 0x00dc
			   dram_ctrl0.mr0_reg << 16 |			//			M0:on-the-fly,CL-16,WR-20,RTP-10,
			   dram_ctrl0.mr1_reg);				//			M1:AL disable,ODI 34ohm,Rtt disable
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00e0,		// (             INIT4) 'h00280200 uMCTL2 0x00e0
			   dram_ctrl0.mr2_reg << 16 |			//			MR2:CWL-16, Dynamic ODT off;
			   0);						//			MR3:5nCK

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00e4, 0x00110000);	// (             INIT5) 'h001103f4 uMCTL2 0x00e4 //dev_zqinit_x32='h10 -1024
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00e8, 0x00000000);	// (             INIT6) 'h00000640 uMCTL2 0x00e8
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00ec, 0x00000000);	// (             INIT7) 'h00000800 uMCTL2 0x00ec
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00f0, 0x00000000);	// (           DIMMCTL) 'h00000000 uMCTL2 0x00f0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x00f4, 0x00000000);	// (           RANKCTL) 'h0000033f uMCTL2 0x00f4 //for multi-rank
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0100,		// (          DRAMTMG0) 'h14132813 uMCTL2 0x0100
			   dram_ctrl0.t_wr2pre << 24 |			//			wr2pre='h14,
			   dram_ctrl0.t_faw << 16 |                     //			dram_ctrl.t_faw='h13,
			   dram_ctrl0.t_ras_max << 8 |                  //			dram_ctrl.t_ras_max='h28,
			   dram_ctrl0.t_ras_min);	                //			dram_ctrl.t_ras_min='h13
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0104,		// (          DRAMTMG1) 'h0004051b uMCTL2 0x0104
			   dram_ctrl0.t_xp << 16 |			//			dram_ctrl.t_xp='h4,
			   dram_ctrl0.t_rd2pre << 8 |			//			rd2pre='h5,
			   dram_ctrl0.t_rc);				//			dram_ctrl.t_rc='h1b
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0108,		// (          DRAMTMG2) 'h0808030f uMCTL2 0x0108
			   dram_ctrl0.t_wl << 24 |			//			write_latency-8,
			   dram_ctrl0.t_rl << 16 |                      //			read_latency-8,
			   dram_ctrl0.t_rd2wr << 8 |                    //			rd2wr=3,
			   dram_ctrl0.t_wr2rd);			        //			wr2rd=f

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x010c,		// (          DRAMTMG3) 'h0000400c uMCTL2 0x010c
			   dram_ctrl0.t_mrw << 20 |                     //			dram_ctrl.t_mrw=0.
			   dram_ctrl0.t_mrd << 12 |                     //			dram_ctrl.t_mrd=4,
			   dram_ctrl0.t_mod);				//			dram_ctrl.t_mod=c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0110,		// (          DRAMTMG4) 'h08030409 uMCTL2 0x0110
			   dram_ctrl0.t_rcd << 24 |                     //			dram_ctrl.t_rcd=8,
			   dram_ctrl0.t_ccd_l << 16 |                   //			t_ccd=3,
			   dram_ctrl0.t_rrd_l << 8 |                    //			t_rrd=4,
			   dram_ctrl0.t_rp);				//			dram_ctrl.t_rp=9
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0114,		// (          DRAMTMG5) 'h07070504 uMCTL2 0x0114
			   dram_ctrl0.t_cksrx << 24 |                   //			dram_ctrl.t_cksrx=7,
			   dram_ctrl0.t_cksre << 16 |                   //			dram_ctrl.t_cksre=7,
			   dram_ctrl0.t_ckesr << 8 |                    //			dram_ctrl.t_ckesr=5,
			   dram_ctrl0.t_cke);				//			dram_ctrl.t_cke=4
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0118, 0x00000000);	// (          DRAMTMG6) 'h00000000 uMCTL2 0x0118 //t_ckdpde=0,t_ckdpdx=0,t_ckcsx=0,
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x011c,		// (          DRAMTMG7) 'h00000707 uMCTL2 0x011c
			   dram_ctrl0.t_ckpde << 8 |                    //			dram_ctrl.t_ckpde=7,
			   dram_ctrl0.t_ckpdx);				//			dram_ctrl.t_ckpdx=7
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0120,		// (          DRAMTMG8) 'h03030e05 uMCTL2 0x0120
			   dram_ctrl0.t_xs_fast_x32 << 24 |             //			dram_ctrl.t_xs_fast_x32=3,
			   dram_ctrl0.t_xs_abort_x32 << 16 |            //			dram_ctrl.t_xs_abort_x32=3,
			   dram_ctrl0.t_xs_dll_x32 << 8 |		//			dram_ctrl.t_xs_dll_x32=e,
			   dram_ctrl0.t_xs_x32);			//			dram_ctrl.t_xs_x32=5
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0124, 0x0);		// (          DRAMTMG9) 'h0002040c uMCTL2 0x0124, wr_preamble=1tCK,
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0128, 0x0);		// (         DRAMTMG10) 'h00100c0a uMCTL2 0x0128
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x012c, 0x0);		// (         DRAMTMG11) 'h1109010e uMCTL2 0x012c
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0130, 0x0);		// (         DRAMTMG12) 'h0000000d uMCTL2 0x0130
	//		   dram_ctrl0.t_cmdcke << 16 |                  //			dram_ctrl.t_cmdcke=0,
	//		   dram_ctrl0.t_mrd_pda);			//			dram_ctrl.t_mrd_pda=d
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0138, 0x00000000);	// (         DRAMTMG14) 'h00000000 uMCTL2 0x0138 //for LP2/3/4
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x013c, 0x80000000);	// (         DRAMTMG15) 'h80000000 uMCTL2 0x013c // en_dfi_lp_t_stab
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0180, 0x41000040);	// (            ZQCTL0) 'h41000040 uMCTL2 0x0180
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0180,
				dram_ctrl0.t_zqoper << 16 |
				dram_ctrl0.t_zqcs);	// (            ZQCTL0) 'h41000040 uMCTL2 0x0180
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0184, 0x0008887d);	// (            ZQCTL1) 'h0008887d uMCTL2 0x0184
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0188, 0x00000000);	// (            ZQCTL2) 'h00000000 uMCTL2 0x0188
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0190,		// (           DFITMG0) 'h048b820e uMCTL2 0x0190
			   dram_ctrl0.dfi_t_ctrl_delay << 24 | 1 << 23 |//			(0x028b820e);
			   dram_ctrl0.dfi_t_rddata_en << 16  | 1 << 15 |
			   dram_ctrl0.dfi_tphy_wrdata << 8 |
			   dram_ctrl0.dfi_tphy_wrlat);
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0194,		// (           DFITMG1) 'h01070202 uMCTL2 0x0194
			   1 << 24 |                                    //			dfi_t_cmd_lat=0,dfi_t_parin_lat=1,
			   dram_ctrl0.dfi_t_wrdata_delay << 16 |        //			dram_ctrl.dfi_t_wrdata_delay=7,
			   dram_ctrl0.dfi_t_dram_clk_disable << 8 |     //			dram_ctrl.dfi_t_dram_clk_disable=2,
			   dram_ctrl0.dfi_t_dram_clk_enable);		//			dram_ctrl.dfi_t_dram_clk_enable=2
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0198, 0x07e0b011);	// (         DFILPCFG0) 'h07e0b011 uMCTL2 0x0198
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x019c, 0x00000041);	// (         DFILPCFG1) 'h00000041 uMCTL2 0x019c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01a0, 0x80300018);	// (           DFIUPD0) 'h80400005 uMCTL2 0x01a0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01a4, 0x009f007e);	// (           DFIUPD1) 'h009f007e uMCTL2 0x01a4
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01a8, 0x00000000);	// (           DFIUPD2) 'h00000000 uMCTL2 0x01a8
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b0, 0x00000011);	// (           DFIMISC) 'h00000011 uMCTL2 0x01b0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b4,		// (           DFITMG2) 'h00000000 uMCTL2 0x01b8
			   dram_ctrl0.dfi_tphy_rdcslat << 8 |           //			dram_ctrl.dfi_tphy_rdcslat=0,
			   dram_ctrl0.dfi_tphy_wrcslat);		//			dram_ctrl.dfi_tphy_wrcslat=0
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b8, 0x00000008);	// (           DFITMG3) 'h00000000 uMCTL2 0x01b8 //dfi_t_geardown=0,
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x01c0, 0x00000001);	// (            DBICTL) 'h00000001 uMCTL2 0x01c0  // DM enabled
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01c4, 0x00000000);	// (        DFIPHYMSTR) 'h00000000 uMCTL2 0x01c4

#ifdef CONFIG_ARTOSYN_AR9301
	//************ No ECC ********************************
	// DRRR RRRR RRRR RRRR RBBC CCCC CCGC CC--  4G
	if (dram->params.ddr_hbm) {
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0200, 0x001f1f1f);	// (          ADDRMAP0) 'h00000012 uMCTL2 0x0200  // cs
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0204, 0x00070707);	// (          ADDRMAP1) 'h003f0606 uMCTL2 0x0204
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0208, 0x00000000);	// (          ADDRMAP2) 'h01010100 uMCTL2 0x0208
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x020c, 0x00000000);	// (          ADDRMAP3) 'h12121201 uMCTL2 0x020c
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0210, 0x00001f1f);	// (          ADDRMAP4) 'h00001f1f uMCTL2 0x0210
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0214, 0x06060606);	// (          ADDRMAP5) 'h040f0404 uMCTL2 0x0214
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0218, 0x0f000606);	// (          ADDRMAP6) 'h0f0f0404 uMCTL2 0x0218
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x021c, 0x00000f0f);	// (          ADDRMAP7) 'h00000f0f uMCTL2 0x021c
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0220, 0x00003f3f);	// (          ADDRMAP8) 'h00003f01 uMCTL2 0x0220  // bg[0] --> axi[3]
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0224, 0x00000000);	// (          ADDRMAP9) 'h04040404 uMCTL2 0x0224
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0228, 0x00000000);	// (         ADDRMAP10) 'h04040404 uMCTL2 0x0228
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x022c, 0x001f1f00);	// (         ADDRMAP11) 'h00000004 uMCTL2 0x022c
	} else {
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0200, 0x001f1f1f);	// (          ADDRMAP0) 'h00000012 uMCTL2 0x0200  // cs
#endif
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0204, 0x00080808);	// (          ADDRMAP1) 'h003f0606 uMCTL2 0x0204
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0208, 0x00000000);	// (          ADDRMAP2) 'h01010100 uMCTL2 0x0208
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x020c, 0x00000000);	// (          ADDRMAP3) 'h12121201 uMCTL2 0x020c
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0210, 0x00001f1f);	// (          ADDRMAP4) 'h00001f1f uMCTL2 0x0210
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0214, 0x07070707);	// (          ADDRMAP5) 'h040f0404 uMCTL2 0x0214
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0218, 0x0f070707);	// (          ADDRMAP6) 'h0f0f0404 uMCTL2 0x0218
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x021c, 0x00000f07);	// (          ADDRMAP7) 'h00000f0f uMCTL2 0x021c
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0220, 0x00003f3f);	// (          ADDRMAP8) 'h00003f01 uMCTL2 0x0220  // bg[0] --> axi[3]
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0224, 0x00000000);	// (          ADDRMAP9) 'h04040404 uMCTL2 0x0224
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0228, 0x00000000);	// (         ADDRMAP10) 'h04040404 uMCTL2 0x0228
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x022c, 0x001f1f00);	// (         ADDRMAP11) 'h00000004 uMCTL2 0x022c
#ifdef CONFIG_ARTOSYN_AR9301
	}
#endif

	// *******************************************************************
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0f04, 0x00000002);	// (          SARBASE0) 'h00000002 uMCTL2 0x0f04
									//			system address region 0 base address 0x1000_0000
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0f08, 0x0000000f);	// (          SARBASE0) 'h00000001 uMCTL2 0x0f04
									//			system address region 0 block size 2GB
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0f0c, 0x00000012);	// (          SARBASE1) 'h00000022 uMCTL2 0x0f0c
									//			system address region 1 base address 0x1_1000_0000
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0f10, 0x0000000f);	// (          SARBASE1) 'h0000001d uMCTL2 0x0f10
									//			system address region 1 block size 4GB

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0240, 0x06000604);	// (            ODTCFG) 'h06000500 uMCTL2 0x0240
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0240,                // (      	ODTCFG) 'h06000500 uMCTL2 0x0240
			   dram_ctrl0.wr_odt_hold << 24 |
			   dram_ctrl0.wr_odt_delay << 16 |
			   dram_ctrl0.rd_odt_hold << 8 |
			   dram_ctrl0.rd_odt_delay << 2);
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0244, 0x00000001);	// (            ODTMAP) 'h00003030 uMCTL2 0x0244
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0250, 0x00001f01);	// (             SCHED) 'h03230205 uMCTL2 0x0250
									//			orginal rw policy, 16 LPR
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0254, 0x00000000);	// (            SCHED1) 'h00000000 uMCTL2 0x0254

#if defined(CONFIG_ARTOSYN_AR9301)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x025c, 0x0f00000f);	// (          PERFHPR1) 'h0f000001 uMCTL2 0x025c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0264, 0x0f0001ff);	// (          PERFLPR1) 'h0f00007f uMCTL2 0x0264
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x026c, 0x0f0001ff);	// (           PERFWR1) 'h0f00007f uMCTL2 0x026c
#elif defined(CONFIG_ARTOSYN_AR9311)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x025c, 0x0f00000f);	// (          PERFHPR1) 'h0f000001 uMCTL2 0x025c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0264, 0x0f0001ff);	// (          PERFLPR1) 'h0f00007f uMCTL2 0x0264
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x026c, 0x0f0001ff);	// (           PERFWR1) 'h0f00007f uMCTL2 0x026c
#endif

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0300, 0x00000011);	// (              DBG0) 'h00000011 uMCTL2 0x0300
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0304, 0x00000000);	// (              DBG1) 'h00000000 uMCTL2 0x0304
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x030c, 0x00000000);	// (            DBGCMD) 'h00000000 uMCTL2 0x030c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0320, 0x00000001);	// (             SWCTL) 'h00000001 uMCTL2 0x0320
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x036c, 0x00110011);	// (         POISONCFG) 'h00100000 uMCTL2 0x036c
	// ********************** No ECC **********************
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0374, 0x0000010c);	// (       ADVECCINDEX) 'h0000010c uMCTL2 0x0374
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x037c, 0x00000000);	// (     ECCPOISONPAT0) 'h00000000 uMCTL2 0x037c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0380, 0x00000000);	// (     ECCPOISONPAT1) 'h00000000 uMCTL2 0x0380
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x0384, 0x00000000);	// (     ECCPOISONPAT2) 'h00000000 uMCTL2 0x0384
	// ****************************************************
#if 0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2020, 0x00000042);	// (    FREQ1.DERATEEN) 'h00000042 uMCTL2 0x2020
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2024, 0xe4fa1694);	// (   FREQ1.DERATEINT) 'he4fa1694 uMCTL2 0x2024
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2034, 0x00408804);	// (      FREQ1.PWRTMG) 'h00408804 uMCTL2 0x2034
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2050, 0x00210000);	// (    FREQ1.RFSHCTL0) 'h00210000 uMCTL2 0x2050
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2064, 0x00920061);	// (     FREQ1.RFSHTMG) 'h00920061 uMCTL2 0x2064
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2064,		// (     FREQ1.RFSHTMG) 'h00928061 uMCTL2 0x2064
			   dram_ctrl1.t_rfc_nom_x1_x32 << 16 |          //			x32 mode, dram_ctrl.t_rfc_nom_x1_x32=7800/(1.876*32)=129=0x81,
			   dram_ctrl1.t_rfc_min);			//			dram_ctrl.t_rfc_min=160/1.876+1=86=0x56
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x20dc, 0x0a310001);	// (       FREQ1.INIT3) 'h0a310001 uMCTL2 0x20dc
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x20dc,		// (             INIT3) 'h0a350001 uMCTL2 0x20dc
			   dram_ctrl1.mr0_reg << 16 |			//			M0:on-the-fly,CL-16,WR-20,RTP-10,
			   dram_ctrl1.mr1_reg);				//			M1:AL disable,ODI 34ohm,Rtt disable
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x20e0, 0x00280200);	// (       FREQ1.INIT4) 'h00280200 uMCTL2 0x20e0
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x20e0,		// (             INIT4) 'h00280200 uMCTL2 0x20e0
			   dram_ctrl1.mr2_reg << 16 |			//			MR2:CWL-16, Dynamic ODT off;
			   dram_ctrl1.mr3_reg);				//			MR3:5nCK
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x20e8, 0x00000640);	// (       FREQ1.INIT6) 'h00000640 uMCTL2 0x20e8
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x20e8,		// (             INIT6) 'h00000640 uMCTL2 0x20e8
			   dram_ctrl1.mr4_reg << 16 |			//			MR4;
			   dram_ctrl1.mr5_reg); 			//			MR5. read DBI disabled
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x20ec, 0x00000800);	// (       FREQ1.INIT7) 'h00000800 uMCTL2 0x20ec
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x20ec,		// (             INIT7) 'h00000800 uMCTL2 0x20ec
			   dram_ctrl1.mr6_reg);				//			tCCD_L min = 6

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x20f4, 0x0000033f);	// (     FREQ1.RANKCTL) 'h0000033f uMCTL2 0x20f4

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2100, 0x14132813);	// (    FREQ1.DRAMTMG0) 'h14132813 uMCTL2 0x2100
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2100,		// (          DRAMTMG0) 'h14132813 uMCTL2 0x2100
			   dram_ctrl1.t_wr2pre << 24 |			//			wr2pre='h14,
			   dram_ctrl1.t_faw << 16 |                     //			dram_ctrl.t_faw='h13,
			   dram_ctrl1.t_ras_max << 8 |                  //			dram_ctrl.t_ras_max='h28,
			   dram_ctrl1.t_ras_min);	                //			dram_ctrl.t_ras_min='h13
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2104, 0x0004051b);	// (    FREQ1.DRAMTMG1) 'h0004051b uMCTL2 0x2104
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2104,		// (          DRAMTMG1) 'h0004051b uMCTL2 0x2104
			   dram_ctrl1.t_xp << 16 |			//			dram_ctrl.t_xp='h4,
			   dram_ctrl1.t_rd2pre << 8 |			//			rd2pre='h5,
			   dram_ctrl1.t_rc);				//			dram_ctrl.t_rc='h1b
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2108, 0x0808030f);	// (    FREQ1.DRAMTMG2) 'h0808030f uMCTL2 0x2108
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2108,		// (          DRAMTMG2) 'h0808030f uMCTL2 0x2108
			   dram_ctrl1.t_wl << 24 |			//			write_latency-8,
			   dram_ctrl1.t_rl << 16 |                      //			read_latency-8,
			   dram_ctrl1.t_rd2wr << 8 |                    //			rd2wr=3,
			   dram_ctrl1.t_wr2rd);			        //			wr2rd=f
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x210c, 0x0000400c);	// (    FREQ1.DRAMTMG3) 'h0000400c uMCTL2 0x210c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x210c,		// (          DRAMTMG3) 'h0000400c uMCTL2 0x210c
			   dram_ctrl1.t_mrw << 20 |                     //			dram_ctrl.t_mrw=0.
			   dram_ctrl1.t_mrd << 12 |                     //			dram_ctrl.t_mrd=4,
			   dram_ctrl1.t_mod);				//			dram_ctrl.t_mod=c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2110, 0x08030409);	// (    FREQ1.DRAMTMG4) 'h08030409 uMCTL2 0x2110
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2110,		// (          DRAMTMG4) 'h08030409 uMCTL2 0x2110
			   dram_ctrl1.t_rcd << 24 |                     //			dram_ctrl.t_rcd=8,
			   dram_ctrl1.t_ccd_l << 16 |                   //			t_ccd=3,
			   dram_ctrl1.t_rrd_l << 8 |                    //			t_rrd=4,
			   dram_ctrl1.t_rp);				//			dram_ctrl.t_rp=9
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2114, 0x07070504);	// (    FREQ1.DRAMTMG5) 'h07070504 uMCTL2 0x2114
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2114,		// (          DRAMTMG5) 'h07070504 uMCTL2 0x2114
			   dram_ctrl1.t_cksrx << 24 |                   //			dram_ctrl.t_cksrx=7,
			   dram_ctrl1.t_cksre << 16 |                   //			dram_ctrl.t_cksre=7,
			   dram_ctrl1.t_ckesr << 8 |                    //			dram_ctrl.t_ckesr=5,
			   dram_ctrl1.t_cke);				//			dram_ctrl.t_cke=4

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2118, 0x00000000);	// (    FREQ1.DRAMTMG6) 'h00000000 uMCTL2 0x2118 for LPDDR2/3/4

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x211c, 0x00000707);	// (    FREQ1.DRAMTMG7) 'h00000707 uMCTL2 0x211c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x211c,		// (          DRAMTMG7) 'h00000707 uMCTL2 0x211c
			   dram_ctrl1.t_ckpde << 8 |                    //			dram_ctrl.t_ckpde=7,
			   dram_ctrl1.t_ckpdx);				//			dram_ctrl.t_ckpdx=7
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2120, 0x03030e05);	// (    FREQ1.DRAMTMG8) 'h03030e05 uMCTL2 0x2120
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2120,		// (          DRAMTMG8) 'h03030e05 uMCTL2 0x2120
			   dram_ctrl1.t_xs_fast_x32 << 24 |             //			dram_ctrl.t_xs_fast_x32=3,
			   dram_ctrl1.t_xs_abort_x32 << 16 |            //			dram_ctrl.t_xs_abort_x32=3,
			   dram_ctrl1.t_xs_dll_x32 << 8 |		//			dram_ctrl.t_xs_dll_x32=e,
			   dram_ctrl1.t_xs_x32);			//			dram_ctrl.t_xs_x32=5
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2124, 0x0002040c);	// (    FREQ1.DRAMTMG9) 'h0002040c uMCTL2 0x2124
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2124,		// (          DRAMTMG9) 'h0002040c uMCTL2 0x2124, wr_preamble=1tCK,
			   dram_ctrl1.t_ccd_s << 16 |			//			dram_ctrl.t_ccd_s=2,
			   dram_ctrl1.t_rrd_s << 8 |                    //			dram_ctrl.t_rrd_s=4,
			   dram_ctrl1.t_wr2rd_s);			//			wr2rd_s=c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2128, 0x00100c0a);	// (   FREQ1.DRAMTMG10) 'h00100c0a uMCTL2 0x2128
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2128,		// (         DRAMTMG10) 'h00100c0a uMCTL2 0x2128
	//		     dram_ctrl1.t_sync_gear << 16 |             //			dram_ctrl.t_sync_gear=10,
	//		     dram_ctrl1.t_cmd_gear << 8 |               //			dram_ctrl.t_cmd_gear=c,
	//		     dram_ctrl1.t_gear_setup << 2 |             //			dram_ctrl.t_gear_setup=2,
	//		     dram_ctrl1.t_gear_hold);			//			dram_ctrl.t_gear_hold=2

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x212c, 0x1109010e);	// (   FREQ1.DRAMTMG11) 'h1109010e uMCTL2 0x212c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x212c,		// (         DRAMTMG11) 'h1109010e uMCTL2 0x212c
			   dram_ctrl1.t_post_mpsm_x32 << 24 |           //			post_mpsm_gap_x32='h11,
			   dram_ctrl1.t_mpx_lh << 16 |                  //			dram_ctrl.t_mpx_lh=9,
			   dram_ctrl1.t_mpx_s << 8 |                    //			dram_ctrl.t_mpx_s=1,
			   dram_ctrl1.t_ckmpe);				//			dram_ctrl.t_ckmpe=e
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2130, 0x0000000d);	// (   FREQ1.DRAMTMG12) 'h0000000d uMCTL2 0x2130
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2130,		// (         DRAMTMG12) 'h0000000d uMCTL2 0x2130
			   dram_ctrl1.t_cmdcke << 16 |                  //			dram_ctrl.t_cmdcke=0,
			   dram_ctrl1.t_mrd_pda);			//			dram_ctrl.t_mrd_pda=d
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2138, 0x00000000);	// (   FREQ1.DRAMTMG14) 'h00000000 uMCTL2 0x2138
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x213c, 0x80000000);	// (   FREQ1.DRAMTMG15) 'h80000000 uMCTL2 0x213c
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2180, 0x41000040);	// (      FREQ1.ZQCTL0) 'h41000040 uMCTL2 0x2180
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2190, 0x028b820e);	// (     FREQ1.DFITMG0) 'h028b820e uMCTL2 0x2190
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2190,		// (           DFITMG0) 'h048b820e uMCTL2 0x2190
			   dram_ctrl1.dfi_t_ctrl_delay << 24 | 1 << 23 |//			(0x028b820e);
			   dram_ctrl1.dfi_t_rddata_en << 16  | 1 << 15 |
			   dram_ctrl1.dfi_tphy_wrdata << 8 |
			   dram_ctrl1.dfi_tphy_wrlat);
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x2194, 0x01070202);	// (     FREQ1.DFITMG1) 'h01070202 uMCTL2 0x2194
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2194,		// (           DFITMG1) 'h01070202 uMCTL2 0x2194
			   1 << 24 |                                    //			dfi_t_cmd_lat=0,dfi_t_parin_lat=1,
			   dram_ctrl1.dfi_t_wrdata_delay << 16 |        //			dram_ctrl.dfi_t_wrdata_delay=7,
			   dram_ctrl1.dfi_t_dram_clk_disable << 8 |     //			dram_ctrl.dfi_t_dram_clk_disable=2,
			   dram_ctrl1.dfi_t_dram_clk_enable);		//			dram_ctrl.dfi_t_dram_clk_enable=2
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x21b8, 0x00000000);	// (     FREQ1.DFITMG3) 'h00000000 uMCTL2 0x21b8
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2240, 0x06000500);	// (      FREQ1.ODTCFG) 'h06000500 uMCTL2 0x0240
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2240,		// (      FREQ1.ODTCFG) 'h06000500 uMCTL2 0x0240
			   dram_ctrl1.wr_odt_hold << 24 |
			   dram_ctrl1.wr_odt_delay << 16 |
			   dram_ctrl1.rd_odt_hold << 8 |
			   dram_ctrl1.rd_odt_delay << 2);
//	ar9301_sdram_write(dram->ddr_ctrl_base + 0x2244, 0x00000001);	// (      FREQ1.ODTMAP) 'h00003030 uMCTL2 0x0244

#endif

	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3020, 0x00000042);	// (    FREQ2.DERATEEN) 'h00000042 uMCTL2 0x3020
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3024, 0xe4fa1694);	// (   FREQ2.DERATEINT) 'he4fa1694 uMCTL2 0x3024
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3034, 0x00408804);	// (      FREQ2.PWRTMG) 'h00408804 uMCTL2 0x3034
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3050, 0x00210000);	// (    FREQ2.RFSHCTL0) 'h00210000 uMCTL2 0x3050
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3064, 0x00920061);	// (     FREQ2.RFSHTMG) 'h00920061 uMCTL2 0x3064
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x30dc, 0x0a310001);	// (       FREQ2.INIT3) 'h0a310001 uMCTL2 0x30dc
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x30e0, 0x00280200);	// (       FREQ2.INIT4) 'h00280200 uMCTL2 0x30e0
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x30e8, 0x00000640);	// (       FREQ2.INIT6) 'h00000640 uMCTL2 0x30e8
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x30ec, 0x00000800);	// (       FREQ2.INIT7) 'h00000800 uMCTL2 0x30ec
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x30f4, 0x0000033f);	// (     FREQ2.RANKCTL) 'h0000033f uMCTL2 0x30f4
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3100, 0x14132813);	// (    FREQ2.DRAMTMG0) 'h14132813 uMCTL2 0x3100
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3104, 0x0004051b);	// (    FREQ2.DRAMTMG1) 'h0004051b uMCTL2 0x3104
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3108, 0x0808030f);	// (    FREQ2.DRAMTMG2) 'h0808030f uMCTL2 0x3108
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x310c, 0x0000400c);	// (    FREQ2.DRAMTMG3) 'h0000400c uMCTL2 0x310c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3110, 0x08030409);	// (    FREQ2.DRAMTMG4) 'h08030409 uMCTL2 0x3110
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3114, 0x07070504);	// (    FREQ2.DRAMTMG5) 'h07070504 uMCTL2 0x3114
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3118, 0x00000000);	// (    FREQ2.DRAMTMG6) 'h00000000 uMCTL2 0x3118
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x311c, 0x00000707);	// (    FREQ2.DRAMTMG7) 'h00000707 uMCTL2 0x311c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3120, 0x03030e05);	// (    FREQ2.DRAMTMG8) 'h03030e05 uMCTL2 0x3120
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3124, 0x0002040c);	// (    FREQ2.DRAMTMG9) 'h0002040c uMCTL2 0x3124
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3128, 0x00100c0a);	// (   FREQ2.DRAMTMG10) 'h00100c0a uMCTL2 0x3128
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x312c, 0x1109010e);	// (   FREQ2.DRAMTMG11) 'h1109010e uMCTL2 0x312c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3130, 0x0000000d);	// (   FREQ2.DRAMTMG12) 'h0000000d uMCTL2 0x3130
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3138, 0x00000000);	// (   FREQ2.DRAMTMG14) 'h00000000 uMCTL2 0x3138
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x313c, 0x80000000);	// (   FREQ2.DRAMTMG15) 'h80000000 uMCTL2 0x313c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3180, 0x41000040);	// (      FREQ2.ZQCTL0) 'h41000040 uMCTL2 0x3180
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3190, 0x028b820e);	// (     FREQ2.DFITMG0) 'h028b820e uMCTL2 0x3190
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3194, 0x01070202);	// (     FREQ2.DFITMG1) 'h01070202 uMCTL2 0x3194
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x31b8, 0x00000000);	// (     FREQ2.DFITMG3) 'h00000000 uMCTL2 0x31b8
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x3240, 0x06000500);	// (      FREQ2.ODTCFG) 'h06000500 uMCTL2 0x3240
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4020, 0x00000042);	// (    FREQ3.DERATEEN) 'h00000042 uMCTL2 0x4020
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4024, 0xe4fa1694);	// (   FREQ3.DERATEINT) 'he4fa1694 uMCTL2 0x4024
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4034, 0x00408804);	// (      FREQ3.PWRTMG) 'h00408804 uMCTL2 0x4034
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4050, 0x00210000);	// (    FREQ3.RFSHCTL0) 'h00210000 uMCTL2 0x4050
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4064, 0x00920061);	// (     FREQ3.RFSHTMG) 'h00920061 uMCTL2 0x4064
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x40dc, 0x0a310001);	// (       FREQ3.INIT3) 'h0a310001 uMCTL2 0x40dc
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x40e0, 0x00280200);	// (       FREQ3.INIT4) 'h00280200 uMCTL2 0x40e0
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x40e8, 0x00000640);	// (       FREQ3.INIT6) 'h00000640 uMCTL2 0x40e8
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x40ec, 0x00000800);	// (       FREQ3.INIT7) 'h00000800 uMCTL2 0x40ec
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x40f4, 0x0000033f);	// (     FREQ3.RANKCTL) 'h0000033f uMCTL2 0x40f4
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4100, 0x14132813);	// (    FREQ3.DRAMTMG0) 'h14132813 uMCTL2 0x4100
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4104, 0x0004051b);	// (    FREQ3.DRAMTMG1) 'h0004051b uMCTL2 0x4104
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4108, 0x0808030f);	// (    FREQ3.DRAMTMG2) 'h0808030f uMCTL2 0x4108
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x410c, 0x0000400c);	// (    FREQ3.DRAMTMG3) 'h0000400c uMCTL2 0x410c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4110, 0x08030409);	// (    FREQ3.DRAMTMG4) 'h08030409 uMCTL2 0x4110
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4114, 0x07070504);	// (    FREQ3.DRAMTMG5) 'h07070504 uMCTL2 0x4114
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4118, 0x00000000);	// (    FREQ3.DRAMTMG6) 'h00000000 uMCTL2 0x4118
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x411c, 0x00000707);	// (    FREQ3.DRAMTMG7) 'h00000707 uMCTL2 0x411c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4120, 0x03030e05);	// (    FREQ3.DRAMTMG8) 'h03030e05 uMCTL2 0x4120
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4124, 0x0002040c);	// (    FREQ3.DRAMTMG9) 'h0002040c uMCTL2 0x4124
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4128, 0x00100c0a);	// (   FREQ3.DRAMTMG10) 'h00100c0a uMCTL2 0x4128
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x412c, 0x1109010e);	// (   FREQ3.DRAMTMG11) 'h1109010e uMCTL2 0x412c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4130, 0x0000000d);	// (   FREQ3.DRAMTMG12) 'h0000000d uMCTL2 0x4130
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4138, 0x00000000);	// (   FREQ3.DRAMTMG14) 'h00000000 uMCTL2 0x4138
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x413c, 0x80000000);	// (   FREQ3.DRAMTMG15) 'h80000000 uMCTL2 0x413c
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4180, 0x41000040);	// (      FREQ3.ZQCTL0) 'h41000040 uMCTL2 0x4180
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4190, 0x028b820e);	// (     FREQ3.DFITMG0) 'h028b820e uMCTL2 0x4190
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4194, 0x01070202);	// (     FREQ3.DFITMG1) 'h01070202 uMCTL2 0x4194
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x41b8, 0x00000000);	// (     FREQ3.DFITMG3) 'h00000000 uMCTL2 0x41b8
	//ar9301_sdram_write(dram->ddr_ctrl_base + 0x4240, 0x06000500);	// (      FREQ3.ODTCFG) 'h06000500 uMCTL2 0x4240
	if (dram->params.ddr_hbm)
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0400, 0x00000000);	// (             PCCFG) 'h00000000 uMCTL2 0x0400
	else
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x0400, 0x00000000);	// (             PCCFG) 'h00000000 uMCTL2 0x0400

#if defined(CONFIG_ARTOSYN_AR9301)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0404, 0x000051ff);	// (           PCFGR_0) 'h0000000f uMCTL2 0x0404
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04b4, 0x00005001);	// (           PCFGR_1) 'h0000000f uMCTL2 0x04b4
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0564, 0x000013ff);	// (           PCFGR_2) 'h0000000f uMCTL2 0x0564
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0408, 0x000051ff);	// (           PCFGW_0) 'h0000100e uMCTL2 0x0408
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04b8, 0x00005001);	// (           PCFGW_1) 'h0000100e uMCTL2 0x04b8
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0568, 0x000013ff);	// (           PCFGW_2) 'h0000100e uMCTL2 0x0568

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0494, 0x00200f07);	// (        PCFGQOS0_0)   R0(VPR) : 0-7 ; R1(HPR) : 8-15; R2(HPR): 12-15
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0498, 0x00000000);	// (        PCFGQOS1_0)   100 cycle
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x049c, 0x00100f07);	// (       PCFGWQOS0_0)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04a0, 0x00000200);	// (       PCFGwQOS1_0)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0544, 0x00100f07);	// (        PCFGQOS0_1)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0548, 0x0000000f);	// (        PCFGQOS1_1)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x054c, 0x00100f07);	// (       PCFGWQOS0_1)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0550, 0x000000ff);	// (       PCFGwQOS1_1)  100 cycle
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x05f4, 0x00200f07);	// (        PCFGQOS0_2)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x05f8, 0x00000000);	// (        PCFGQOS1_2)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x05fc, 0x00100f07);	// (       PCFGWQOS0_2)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0600, 0x000002ff);	// (       PCFGwQOS1_2)  100 cycle
#elif defined(CONFIG_ARTOSYN_AR9311)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0404, 0x000050ff);	// (           PCFGR_0) 'h0000000f uMCTL2 0x0404
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04b4, 0x0000500f);	// (           PCFGR_1) 'h0000000f uMCTL2 0x04b4
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0408, 0x000050ff);	// (           PCFGW_0) 'h0000100e uMCTL2 0x0408
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04b8, 0x0000500f);	// (           PCFGW_1) 'h0000100e uMCTL2 0x04b8

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0494, 0x00200f07);	// (        PCFGQOS0_0)   R0(VPR) : 0-7 ; R1(HPR) : 8-15; R2(HPR): 12-15
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0498, 0x00000000);	// (        PCFGQOS1_0)   100 cycle
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x049c, 0x00100f07);	// (       PCFGWQOS0_0)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x04a0, 0x000000ff);	// (       PCFGWQOS1_0)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0544, 0x00200f07);	// (        PCFGQOS0_1) 0915debug
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0548, 0x0000000f);	// (        PCFGQOS1_1)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x054c, 0x00100f07);	// (       PCFGWQOS0_1)
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0550, 0x0000003f);	// (       PCFGwQOS1_1)  100 cycle
#endif

	//===================================================================================
	ar9301_sdram_write(dram->ddr_reg_base + DDR_CON23, 0x000007ff);	//			ddr_phy presetn 1->0
	// ar9301_sdram_write(dram->ddr_reg_base + DDR_CON23, 0x00000001);	// 		power ok 0->1
	ar9301_sdram_write(dram->ddr_reg_base + DDR_CON22, 0x00007fff);	//			for aresetn,coreRSTN gen
	//===================================================================================

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0304, 0x00000000);	// (              DBG1) 'h00000000 uMCTL2 0x0304
									//			[0] dis_dq [1] dis_hif
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0030, 0x00000000);	// (            PWRCTL) 'h00000000 uMCTL2 0x0030
									//			[0] selfref_en [1] powerdown_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0320, 0x00000000);	// (             SWCTL) 'h00000000 uMCTL2 0x0320
									//			[0] sw_done
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b0, 0x00000010);	// (           DFIMISC) 'h00000010 uMCTL2 0x01b0
									//			[0] dfi_init_complete_en
									//			[1] phy_dbi_mode
									//			[2] dfi_data_cs_polarity
									//			[3] share_dfi_dram_clk_disable
									//			[4] ctl_idle_en
									//			[5] dfi_init_start
									//			[6] dis_dyn_adr_tri
									//			[7] lp_optimized_write
									//			[12:8] dfi_frequency
#if 0
	//===================================================================================
	// PHY INIT
	//===================================================================================
	if (dram->params.freq0 == 3200)
		if (dram->params.phy_train_mode == TRAIN1D)
			dwc_ddrphy_phyinit_out_ddr4_800_train1d();
		else
			dwc_ddrphy_phyinit_out_ddr4_800_skiptrain();
	else if (dram->params.freq0 == 2400)
		if (dram->params.phy_train_mode == TRAIN1D)
			dwc_ddrphy_phyinit_out_ddr4_666_train1d();
		else
			dwc_ddrphy_phyinit_out_ddr4_666_skiptrain();
	else
		if (dram->params.phy_train_mode == TRAIN1D)
			dwc_ddrphy_phyinit_out_ddr4_train1d();
		else if (dram->params.phy_train_mode == TRAIN2D)
			dwc_ddrphy_phyinit_out_ddr4_train1d2d();
		else if (dram->params.phy_train_mode == DEVINIT)
			dwc_ddrphy_phyinit_out_ddr4_devinit_skiptrain();
		else
			dwc_ddrphy_phyinit_out_ddr4_skiptrain();
#endif

	ddrphy_init(dram);

	ar9301_sdram_write(dram->ddr_phy_base + 0x340000, 0x0);
	ar9301_sdram_write(dram->ddr_phy_base + 0x08046c, 0x3ff);	//			Interrupt Enable
	ar9301_sdram_write(dram->ddr_phy_base + 0x340000, 0x1);

	artosyn_dphy_pll_cfgs_t pll_cfgs;
	user_input_basic_t *basic = &dram->userInputBasic;
	unsigned int i = 0, mr[3];

	memcpy(&pll_cfgs, &dram->pll_default_cfgs,
	       sizeof(artosyn_dphy_pll_cfgs_t));

	for (i = 0; i < 4; i++) {
		if (dram->params.freq0 == basic->Frequency[i]) {
			mr[0] = mb_DDR3U_1D[i].MR0;
			mr[1] = mb_DDR3U_1D[i].MR1;
			mr[2] = mb_DDR3U_1D[i].MR2;
			break;
		}
	}
	pll_cfgs.ddr_mem_freq = basic->Frequency[i];

	if (basic->NumPStates > 1) {
		ddrphy_pll_config(&pll_cfgs);

		ar9301_sdram_write(dram->ddr_ctrl_base + 0x00dc,	// (             INIT3) 'h0a350001 uMCTL2 0x00dc
				   mr[0] << 16 |			//			M0:on-the-fly,CL-16,WR-20,RTP-10,
				   mr[1]);				//			M1:AL disable,ODI 34ohm,Rtt disable
		ar9301_sdram_write(dram->ddr_ctrl_base + 0x00e0,	// (             INIT4) 'h00280200 uMCTL2 0x00e0
				   mr[2] << 16);			//			MR2:CWL-16, Dynamic ODT off;
	}

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x1b0,
			   0x00000030 | (i << 8));			// (           DFIMISC) 'h00000001 uMCTL2

	rdata_t = 0;
#ifdef AR9301_SDRAM_DEBUG
	printf("while (DFISTAT != 0x00000001);\n");
#else
	while (rdata_t != 0x00000001) {
		rdata_t = ar9301_sdram_read(dram->ddr_ctrl_base + 0x1bc);// (           DFISTAT) 'h00000000  uMCTL2
									//			[0] dfi_init_complete
									//			[1] dfi_lp_ack
   		udelay(1);
	}
#endif
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x1b0, 0x00000010 | (i << 8));	// (           DFIMISC) 'h00000001 uMCTL2
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x1b0, 0x00000011 | (i << 8));	// (           DFIMISC) 'h00000001 uMCTL2
									//			[0] dfi_init_complete_en
									//			[1] phy_dbi_mode
									//			[2] dfi_data_cs_polarity
									//			[3] share_dfi_dram_clk_disable
									//			[4] ctl_idle_en
									//			[5] dfi_init_start
									//			[6] dis_dyn_adr_tri
									//			[7] lp_optimized_write
									//			[12:8] dfi_frequency

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x320, 0x00000001);	// (             SWCTL) 'h00000001 uMCTL2
									//			[0] sw_done
	rdata_t = 0;
#ifdef AR9301_SDRAM_DEBUG
	printf("while (SWSTAT != 0x00000001);\n");
#else
	while (rdata_t != 0x00000001) {
		rdata_t = ar9301_sdram_read(dram->ddr_ctrl_base + 0x324);// (            SWSTAT) 'h00000000  uMCTL2
									//			[0] sw_done_ack
		udelay(1);
	}
#endif
	rdata_t = 0;
#ifdef AR9301_SDRAM_DEBUG
	printf("while (STAT != 0x00000001);\n");
#else
	while (rdata_t != 0x00000001) {
		rdata_t = ar9301_sdram_read(dram->ddr_ctrl_base + 0x004);// (              STAT) 'h00000000  uMCTL2
									//			[x:0] operating_mode: init normal power-down self-refresh
									//			[5:4] selfref_type
									//			[9:8] selfref_state
									//			[12] selfref_cam_not_empty
		udelay(1);
	}
#endif
	// ar9301_dmc_change_freq(dram);
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0030, 0x00000000);	// (           PWRCTL) 'h00000003 uMCTL2 0x0030
									//			[0] selfref_en [1] powerdown_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0490, 0x00000001);	// (           PCTRL_0) 'h00000001 uMCTL2 0x0490
									//			[0] port_en
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0540, 0x00000001);	// (           PCTRL_1) 'h00000001 uMCTL2 0x0540
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x05f0, 0x00000001);	// (           PCTRL_2) 'h00000001 uMCTL2 0x05f0
	// training
	ar9301_sdram_write(dram->ddr_ctrl_base + 0x01b0,
			   0x00000010 | (i << 8));			// (           DFIMISC) 'h00000001 uMCTL2

	ar9301_sdram_write(dram->ddr_ctrl_base + 0x0004, 0x1234abcd);	//			release timing disable

//	ar9301_sdram_write(dram->ddr_mon_base + DDR_MON_PERIOD, 0x0000ffff);
//
//	ar9301_sdram_write(dram->ddr_mon_base + DDR_MON_PERIOD, 0x0000ffff);
//	ar9301_sdram_write(dram->axi_mon0_base + AXI_MON_PERIOD, 0x0000ffff);
//	ar9301_sdram_write(dram->axi_mon1_base + AXI_MON_PERIOD, 0x0000ffff);
//	ar9301_sdram_write(dram->axi_mon2_base + AXI_MON_PERIOD, 0x0000ffff);
//
//	ar9301_sdram_write(dram->ddr_mon_base + DDR_MON_CTRL, 0x00000001);	// DDR Monitor ON
//	ar9301_sdram_write(dram->axi_mon0_base + AXI_MON_CTRL, 0x00000001);	// AXI0 Monitor ON
//	ar9301_sdram_write(dram->axi_mon1_base + AXI_MON_CTRL, 0x00000001);	// AXI1 Monitor ON
//	ar9301_sdram_write(dram->axi_mon2_base + AXI_MON_CTRL, 0x00000001);	// AXI2 Monitor ON

	pr_info("SDRAM init finish\n");

	return 0;
}
#ifdef CONFIG_ARTOSYN_AR9301
void ar9301_ceva_sram_init()
{
	writel(0x4c43544d, CEVA_PWR_CTRL);
	writel(0xf, CEVA_PWR_STAT);
	writel(0x1, CEVA_CORE_0_REG + 0x20);
	writel(0x1, CEVA_CORE_0_RGU + 0x0);
	writel(0x1, CEVA_CORE_0_RGU + 0x4);
	writel(0x1, CEVA_CORE_0_RGU + 0x8);
	writel(0x1, CEVA_CORE_0_RGU + 0xc);
}
#endif

static int ar9301_dmc_init(struct udevice *dev)
{
	struct artosyn_dram_info *priv = dev_get_priv(dev);
	runtime_config_t *runtime = &priv->runtimeConfig;
	void (*warmboot_entry)();
	uint16_t tmp[256];
	uint32_t header_offset = 0;
	int ret;

	if (priv->warmboot_support) {
		RetRegList = (Reg_Addr_Val_t *)RET_REG_LIST_BASE_ADDR;
		NumRegSaved = (unsigned int *)RET_REG_NUM_ADDR;
		warmboot = readl(WARMBOOT_FLAG_ADDR);
	}

#define DMC_BASE_NUM 	14
	uint32_t baddr[DMC_BASE_NUM];
	ret = dev_read_u32_array(dev, "reg", baddr, DMC_BASE_NUM);
	if (ret) {
		pr_err("%s: read dmc base address failed %d\n", __func__, ret);
		return -EINVAL;
	}

	priv->ddr_phy_base = (void *)(size_t)baddr[0];
	priv->ddr_ctrl_base = (void *)(size_t)baddr[2];
	priv->ddr_reg_base = (void *)(size_t)baddr[4];
	priv->ddr_mon_base = (void *)(size_t)baddr[6];
	priv->axi_mon0_base = (void *)(size_t)baddr[8];
	priv->axi_mon1_base = (void *)(size_t)baddr[10];
	priv->axi_mon2_base = (void *)(size_t)baddr[12];

	filter_base_addr = priv->ddr_ctrl_base;

	//printf("%s ddr_ctrl_base %p ddr_phy_base %p ddr_reg_base %p\n",
	//	__func__, priv->ddr_ctrl_base, priv->ddr_phy_base,
	//	priv->ddr_reg_base);

#if CONFIG_MMC
	header_offset = DDRPHY_FLASH_EMMC_HEADER_OFFSET;
#elif defined(CONFIG_SPI_FLASH)
	header_offset = DDRPHY_FLASH_NOR_HEADER_OFFSET;
#elif defined(CONFIG_MTD_SPI_NAND)
	struct mtd_info *mtd;

	mtd = artosyn_qspi_nand_init();
	if (!mtd)
		return -1;

	/* (4 * spl(0x40000 / 0x80000) + 2 * gpt(0x40000 / 0x80000)) */
	if (mtd->writesize == 0x800)
		header_offset = 0x100000 + NAND_PES_PER_GPT * 2 * mtd->erasesize;
	else if (mtd->writesize == 0x1000)
		header_offset = 0x200000 + NAND_PES_PER_GPT * 2 * mtd->erasesize;
#endif
	struct sirius_image_header *ar_header = NULL;
	unsigned char * hash_buffer = NULL;
	unsigned char * signature_buffer = NULL;
	unsigned char * data = NULL;
	unsigned int data_len = 0;

	ddrphy_store_bin_file(tmp, header_offset, sizeof(tmp));

	ar_header = (struct sirius_image_header *)tmp;
	hash_buffer = ((unsigned char *)ar_header) + sizeof(*ar_header);
	signature_buffer = hash_buffer + ar_header->hash_size;
	data_len = ar_header->img_len;

	if(0x4152544f != ar_header->magic)
	{
		printf("%s[%d]: magic error!\n", __FUNCTION__, __LINE__);
		hang();
	}

#ifdef CONFIG_ARTOSYN_AR9301
	if(secure_boot_enabled())
	{
		unsigned int i = 0;
		uint32_t readed = 0;
		unsigned char calc_hash[32] __attribute__((aligned(4)))   = {0};
		uint16_t ceva_tmp[1024];
		unsigned int head_len = sizeof(*ar_header) + ar_header->hash_size + ar_header->sig_len;
		sha256_context ctx;
		uint32_t total_len = data_len + head_len;

		sha256_starts(&ctx);
		for (readed = 0; readed < total_len; readed += sizeof(ceva_tmp)) {
			if (readed < total_len) {
				if (total_len - readed < sizeof(ceva_tmp)) {
					memset(ceva_tmp, 0, sizeof(ceva_tmp));
					ret = ddrphy_store_bin_file(ceva_tmp, header_offset + readed,
						ALIGN(total_len -  readed, DDRPHY_FLASH_ALIGN));
					sha256_update(&ctx, ceva_tmp, total_len - readed);
				}
				else {
					ret = ddrphy_store_bin_file(ceva_tmp, header_offset + readed, sizeof(ceva_tmp));
					if (readed == 0) {
						sha256_update(&ctx, (uint8_t *)ceva_tmp + head_len, sizeof(ceva_tmp)-head_len);
					}
					else {
						sha256_update(&ctx, ceva_tmp, sizeof(ceva_tmp));
					}
				}
			}
			if (ret < 0) {
				pr_err("Store ddr bin faild!\n");
				hang();
			}
		}
		sha256_finish(&ctx, calc_hash);
		//Compare hash
		while(i < ar_header->hash_size) {
			if(calc_hash[i] != hash_buffer[i]) {
				printf("Hash verify Fail %d, %02x-%02x\n", i,   calc_hash[i], hash_buffer[i]);
				return -1;
			}
			i++;
		}
		data = (unsigned char *)ar_header + sizeof(*ar_header) + ar_header->hash_size + ar_header->sig_len;
		if (0 != spl_verify_hash("vendor", (unsigned char *)hash_buffer, ar_header->hash_size, (unsigned char *)signature_buffer, ar_header->sig_len))
		{
			printf("%s[%d]: verify %s error!\n", __FUNCTION__, __LINE__, "vendor");
			hang();
		}
	}
	else
#endif
		data = (unsigned char *)ar_header + sizeof(*ar_header) + ar_header->hash_size + ar_header->sig_len;

	memcpy(&priv->vendor_header, data, sizeof(artosyn_vendor_header_t));

	priv->vendor_header.imem_offset_1d += header_offset;
	priv->vendor_header.dmem_offset_1d += header_offset;
	priv->vendor_header.imem_offset_2d += header_offset;
	priv->vendor_header.dmem_offset_2d += header_offset;
	priv->vendor_header.imem_offset_diag += header_offset;
	priv->vendor_header.dmem_offset_diag += header_offset;
	priv->vendor_header.ddr3_imem_offset_1d += header_offset;
	priv->vendor_header.ddr3_dmem_offset_1d += header_offset;
	priv->restore_offset = header_offset + ALIGN(data_len, DDRPHY_FLASH_ALIGN);

	ar9301_sdram_init(priv);

	if (!warmboot) {
		if (runtime->skip_train) {
			*NumRegSaved = 0;
			ddrphy_save_regs(priv, true);
		} else if (priv->warmboot_support) {
			*NumRegSaved = 0;
			ddrphy_save_regs(priv, false);
		}
	} else if (warmboot && priv->warmboot_support) {
		warmboot_entry = (void (*)())((unsigned long)readl(BL31_WARM_ENTRYPOINT));
		udelay(100000);
		printf("warmboot entry 0x%x \n", (unsigned int)warmboot_entry);
		warmboot_entry();
	}

	return 0;
}

#endif

static void ar9301_dmc_user_modify(struct artosyn_dram_info *dram) {
	artosyn_user_modify_t *modify = &dram->modify;
	artosyn_dram_params_t *params = &dram->params;
	user_input_basic_t *basic = &dram->userInputBasic;
	user_input_advanced_t *advanced = &dram->userInputAdvanced;
	runtime_config_t *runtime = &dram->runtimeConfig;
	artosyn_dphy_pll_cfgs_t *pll_cfgs = &dram->pll_default_cfgs;

	if (!modify)
		return;

	params->type = modify->type;
	params->freq0 = modify->freq0;
	params->freq1 = modify->freq1;
	params->density = modify->density;
	params->ddr_hbm = modify->ddr_hbm;
	params->mr1_odic = modify->mr1_odic;
	params->mr1_rtt_nom = modify->mr1_rtt_nom;
	params->mr4_rdpreamble = modify->mr4_rdpreamble;
	params->mr4_wrpreamble = modify->mr4_wrpreamble;
	basic->DramType = modify->DramType;
	basic->NumDbyte = modify->NumDbyte;
	basic->NumActiveDbyteDfi0 = modify->NumActiveDbyteDfi0;
	basic->NumPStates = modify->NumPStates;
	basic->Frequency[0] = modify->Frequency[0];
	basic->Frequency[1] = modify->Frequency[1];
	basic->Frequency[2] = modify->Frequency[2];
	basic->Frequency[3] = modify->Frequency[3];
	advanced->ODTImpedance[0] = modify->ODTImpedance[0];
	advanced->ODTImpedance[1] = modify->ODTImpedance[1];
	advanced->ODTImpedance[2] = modify->ODTImpedance[2];
	advanced->ODTImpedance[3] = modify->ODTImpedance[3];
	advanced->TxImpedance[0] = modify->TxImpedance[0];
	advanced->TxImpedance[1] = modify->TxImpedance[1];
	advanced->TxImpedance[2] = modify->TxImpedance[2];
	advanced->TxImpedance[3] = modify->TxImpedance[3];
	advanced->ATxImpedance = modify->ATxImpedance;
	pll_cfgs->ssc_en = modify->ssc_en;
	pll_cfgs->ssc_divn = modify->ssc_divn;
	pll_cfgs->ssc_num_step = modify->ssc_num_step;
	pll_cfgs->ssc_kdelta = modify->ssc_kdelta;
	pll_cfgs->ssc_mode = modify->ssc_mode;

	runtime->skip_train = modify->skip_train;
}

static int ar9301_dmc_get_adc(int ch) {
	unsigned int reg_val;
	unsigned int adc_val;
	unsigned int min_val = 0x3ff;
	unsigned int max_val = 0;
	unsigned int sum_val = 0;
	int i;

	reg_val = readl(ABB_CORE_REG6_ADDR);
	reg_val &= ~(0x1);
	writel(reg_val, ABB_CORE_REG6_ADDR);
	//writel((1 << ch), SAR10_CHAN_SEL);
	writel(ch, ABB_CORE_REG9_ADDR);
	writel(0x0, SAR10_WORK_MODE);

	for(i = 0; i < 6; i++) {
		adc_val = readl(SAR10_DATA);
		if(adc_val < min_val)
			min_val = adc_val;
		if(adc_val > max_val)
			max_val = adc_val;
		sum_val += adc_val;
		udelay(100);
	}
	adc_val = (sum_val - min_val - max_val) / 4;

	reg_val = readl(ABB_CORE_REG6_ADDR);
	reg_val |= 0x1;
	writel(reg_val, ABB_CORE_REG6_ADDR);

	return (adc_val * ADC_DEFAULT_A / ADC_DEFAULT_B + ADC_DEFAULT_C);
}

#define ADC_VALUE_0V3    (300)
#define ADC_VALUE_0V6    (600)
#define ADC_VALUE_0V     (0)

#define ADC_VALUE_THRES  (100)

static bool is_in_range(int target_val, int adc_value)
{
	if((target_val < (adc_value + ADC_VALUE_THRES)) &&
	   (target_val > (adc_value - ADC_VALUE_THRES)))
		return true;
	else
		return false;
}

static int ar9301_dmc_get_config_idx(void)
{
	int adc_val;

	adc_val = ar9301_dmc_get_adc(0);
	printf("adc_val0 = %d\n", adc_val);

	if(is_in_range(ADC_VALUE_0V, adc_val))
		return 0; //512M
	else if(is_in_range(ADC_VALUE_0V3, adc_val))
		return 1; //1G
	else if(is_in_range(ADC_VALUE_0V6, adc_val))
		return 2; //2G

	return 0;
}

static int ar9301_dmc_ofdata_to_platdata(struct udevice *dev)
{
#if !CONFIG_IS_ENABLED(OF_PLATDATA)
	struct artosyn_dram_info *priv = dev_get_priv(dev);
	int len, ret;
	int i;
	unsigned int *p;

	ret = dev_read_u32_array(dev, "params",
				 (unsigned int *)&priv->params,
				 sizeof(artosyn_dram_params_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read params failed %d\n", __func__, ret);
		return -EINVAL;
	}

	ret = dev_read_u32_array(dev, "phy-pll-config",
				 (unsigned int *)&priv->pll_default_cfgs,
				 sizeof(artosyn_dphy_pll_cfgs_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read phy-pll-config failed %d\n", __func__, ret);
		return -EINVAL;
	}

	ret = dev_read_u32_array(dev, "phy-runtime-config",
				 (unsigned int *)&priv->runtimeConfig,
				 sizeof(runtime_config_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read phy-runtime-config failed %d\n", __func__, ret);
		return -EINVAL;
	}

	ret = dev_read_u32_array(dev, "phy-input-basic",
				 (unsigned int *)&priv->userInputBasic,
				 sizeof(user_input_basic_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read phy-input-basic failed %d\n", __func__, ret);
		return -EINVAL;
	}
	p =(unsigned int *)&priv->userInputBasic;
	for (i = 0; i < (sizeof(user_input_basic_t) / sizeof(uint32_t)); i++)
		pr_info("%d: 0x%x\n", i, p[i]);

	ret = dev_read_u32_array(dev, "phy-input-advanced",
				 (unsigned int *)&priv->userInputAdvanced,
				 sizeof(user_input_advanced_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read phy-input-advanced failed %d\n", __func__, ret);
		return -EINVAL;
	}

	ret = dev_read_u32_array(dev, "phy-misc",
				 (unsigned int *)&priv->misc,
				 sizeof(artosyn_dphy_misc_t) / sizeof(uint32_t));
	if (ret) {
		pr_err("%s: read phy-misc failed %d\n", __func__, ret);
		return -EINVAL;
	}

	i = ar9301_dmc_get_config_idx();
	ret = dev_read_u32_array(dev, user_modify_name[i],
				 (unsigned int *)&priv->modify,
				 sizeof(artosyn_user_modify_t) / sizeof(uint32_t));
	if (ret && i != 0) {
		pr_err("DDR read %s failed!\n", user_modify_name[i]);
		i = 0;
		ret = dev_read_u32_array(dev, user_modify_name[i],
					 (unsigned int *)&priv->modify,
					 sizeof(artosyn_user_modify_t) / sizeof(uint32_t));
	}
	if (ret) {
		printf("%s: read user-modify failed, use default config\n", __func__);
	} else {
		printf("DDR use %s\n", user_modify_name[i]);
		ar9301_dmc_user_modify(priv);
	}

	if (dev_read_prop(dev, "warmboot", &len)) {
		printf("DDR warmboot support.\n");
		priv->warmboot_support = 1;
	} else {
		printf("DDR warmboot not support\n");
		priv->warmboot_support = 0;
	}

#endif
	return 0;
}

typedef struct rtc_data {
	uint8_t data[2048];
} rtc_data_t;

extern int is_boot_from_outside(void);

int ar9301_dmc_set_flash_state(void)
{
#ifndef CONFIG_SPL_BUILD
#ifdef CONFIG_MMC
	struct mmc *mmc;
#endif
	struct blk_desc *desc;
	struct sirius_image_header *p_img_header = NULL;
	uint32_t img_header_offset = 0;
	uint32_t *p_state;
	rtc_data_t *p_rtc_data;
	unsigned int data_len = 0;
	unsigned int restore_offset = 0;

	ALLOC_CACHE_ALIGN_BUFFER(struct sirius_image_header, tmp_img_header, 1);
	ALLOC_CACHE_ALIGN_BUFFER(rtc_data_t, tmp_rtc, 1);

	if (is_boot_from_outside())
		return 0;

#ifdef CONFIG_MMC
	mmc_initialize(NULL);

	mmc = find_mmc_device(0);
	desc = mmc_get_blk_desc(mmc);
#endif
	img_header_offset = DDRPHY_FLASH_EMMC_HEADER_OFFSET;

	blk_dread(desc, img_header_offset / 512, ALIGN(sizeof(struct sirius_image_header), DDRPHY_FLASH_ALIGN) / 512, tmp_img_header);
	p_img_header = (struct sirius_image_header *)tmp_img_header;
	if (p_img_header->magic != 0x4152544f) {
		debug("%s[%d]: magic error!\n", __FUNCTION__, __LINE__);
		hang();
	}

	restore_offset = img_header_offset + ALIGN(p_img_header->img_len, DDRPHY_FLASH_ALIGN);

	blk_dread(desc, restore_offset / 512, RET_FLASH_DATA_SIZE / 512, tmp_rtc);
	p_rtc_data = (rtc_data_t *)tmp_rtc;
	p_state = (uint32_t *)((uint8_t *)p_rtc_data + RET_STATE_OFFSET);
	*p_state = RET_FLASH_STATE_VALID;
	blk_dwrite(desc, restore_offset / 512, RET_FLASH_DATA_SIZE / 512, tmp_rtc);
#endif
	return 0;
}

static int ar9301_dmc_probe(struct udevice *dev)
{
	struct artosyn_dram_info *priv = dev_get_priv(dev);

#ifdef CONFIG_SPL_BUILD
	if (ar9301_dmc_init(dev))
		return -1;
#endif
	priv->info.base = CONFIG_SYS_SDRAM_BASE;
#if defined(CONFIG_ARTOSYN_AR9301)
	priv->info.size = ((unsigned long long)priv->params.density) * 128 *
			  (2 >> priv->params.ddr_hbm);
#elif defined(CONFIG_ARTOSYN_AR9311)
	priv->info.size = ((unsigned long long)priv->params.density) * 128 * 1;
#endif

	mem_map[1].size = priv->info.size;

	return 0;
}

static int ar9301_dmc_get_info(struct udevice *dev, struct ram_info *info)
{
	struct artosyn_dram_info *priv = dev_get_priv(dev);

	*info = priv->info;

	return 0;
}

static struct ram_ops ar9301_dmc_ops = {
	.get_info = ar9301_dmc_get_info,
};

static const struct udevice_id ar9301_dmc_ids[] = {
	{ .compatible = "artosyn,ar9301-dmc" },
	{ }
};

U_BOOT_DRIVER(artosyn_ar9301_dmc) = {
	.name = "artosyn_ar9301_dmc",
	.id = UCLASS_RAM,
	.of_match = ar9301_dmc_ids,
	.ops = &ar9301_dmc_ops,
	.ofdata_to_platdata = ar9301_dmc_ofdata_to_platdata,
	.probe = ar9301_dmc_probe,
	.priv_auto_alloc_size = sizeof(struct artosyn_dram_info),
	.platdata_auto_alloc_size = sizeof(struct artosyn_dmc_plat),
};
