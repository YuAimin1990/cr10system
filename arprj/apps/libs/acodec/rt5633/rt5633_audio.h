#ifndef _RT5633_AUDIO_H_
#define _RT5633_AUDIO_H_
#define ALC5633_RESET						       0x00
#define ALC5633_SPK_OUT_CTRL				   0x02
#define ALC5633_SPK_HP_OUT_MIX			   0x03
#define ALC5633_HP_OUT_VOL					   0x04
#define ALC5633_AUX_OUT_CTRL				   0x06
#define ALC5633_LINE1_IN_VOL				   0x08
#define ALC5633_LINE2_IN_VOL				   0x0A
#define ALC5633_DAC_CTRL					     0x0C
#define ALC5633_DAC_VOL						     0x0E
#define ALC5633_MIC_IN_CTRL					   0x10
#define ALC5633_ADC_CTRL					     0x12
#define ALC5633_REC_MIX_CTRL				   0x14
#define ALC5633_ADC_VOL						     0x16
#define ALC5633_HP_MIX_CTRL					   0x18
#define ALC5633_AUX_MIX_CTRL				   0x1A
#define ALC5633_SPK_MIX_CTRL				   0x1C
#define ALC5633_SPK_AMP_CTRL				   0x1E
#define ALC5633_MIC_CTRL					     0x22
#define ALC5633_I2S_AUDIO_PORT_CTRL	   0x34
#define ALC5633_STEREO_CLK_CTRL			   0x38
#define ALC5633_PWR_MANAG_ADD1			   0x3A
#define ALC5633_PWR_MANAG_ADD2			   0x3B
#define ALC5633_PWR_MANAG_ADD3			   0x3C
#define ALC5633_PWR_MANAG_ADD4			   0x3E
#define ALC5633_GENERAL1_CTRL				   0x40
#define ALC5633_INTER_CLK_CTRL			   0x42
#define ALC5633_PLL_CTRL					     0x44
#define ALC5633_DIGIT_BEEP_IRQ_CTRL	   0x48
#define ALC5633_INTER_STAT_STICKY_CTRL 0x4A
#define ALC5633_GPIO1_CTRL					   0x4C
#define ALC5633_GPIO2_CTRL					   0x4D
#define ALC5633_GENERAL2_CTRL				   0x52
#define ALC5633_DEPOP1_CTRL					   0x54
#define ALC5633_DEPOP2_CTRL					   0x56
#define ALC5633_JACK_DETECT_CTRL			 0x5A
#define ALC5633_SOFT_VOL1_CTRL				 0x5C
#define ALC5633_SOFT_VOL2_CTRL				 0x5E
#define ALC5633_FUNC1_CTRL					   0x64
#define ALC5633_FUNC2_CTRL					   0x65
#define ALC5633_FUNC3_CTRL					   0x66
#define ALC5633_PRESUDO_STEREO_SPATIAL_CTRL	0x68
#define ALC5633_INDEX_ADDR					   0x6A
#define ALC5633_INDEX_DATA					   0x6C
#define ALC5633_EQ1							       0x6E
#define ALC5633_EQ2							       0x70

#define ALC5633_PRI_CLS_AB_AMP_RATIO	 0x45

//SPK out Control (0x2)
#define MU_SPK_P							    (1<<15)
#define SPK_SOURCE_SHFT						(13)
#define SPK_SOURCE_MASK						(0x3<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_D_RN						(0x0<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_D_RP						(0x1<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_D_LN						(0x2<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_D_VM						(0x3<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_AB_VM					(0x0<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_AB_RP					(0x1<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_AB_LN					(0x2<<SPK_SOURCE_SHFT)
#define SPK_SOURCE_AB_RN					(0x3<<SPK_SOURCE_SHFT)
#define SPK_VOL_L_SHFT						(5)
#define SPK_VOL_L_MASK						(0x1f<<SPK_VOL_L_SHFT)
#define SPK_VOL_R_SHFT						(0)
#define SPK_VOL_R_MASK						(0x1f<<SPK_VOL_R_SHFT)

//SPK HP Mixer (0x3)
#define MU_DAC_L_SPK_L						(1<<8)
#define MU_DAC_R_SPK_R						(1<<6)
#define MU_HP_HP							    (1<<5)
#define MU_DAC_HP							    (1<<4)

//HP Volume (0x4)
#define MU_HP_L								    (1<<15)
#define SEL_HPVOL_L_HP						(1<<14)
#define MU_HP_VOL_L_SHFT					(8)
#define MU_HP_VOL_L_MASK					(0x1f<<MU_HP_VOL_L_SHFT)
#define MU_HP_R								    (1<<7)
#define SEL_HPVOL_R_HP						(1<<6)
#define MU_HP_VOL_R_SHFT					(0)
#define MU_HP_VOL_R_MASK					(0x1f<<MU_HP_VOL_R_SHFT)

//AUX ctrl (0x6)
#define MU_AXO_L							    (1<<15)
#define MU_AXO_MODE_STEREO				(1<<14)
#define MU_AXO_L_VOL_SHFT					(8)
#define MU_AXO_L_VOL_MASK					(0x1f<<MU_AXO_L_VOL_SHFT)
#define MU_AXO_R							    (1<<7)
#define MU_AXO_R_VOL_SHFT					(0)
#define MU_AXO_R_VOL_MASK					(0x1f<<MU_AXO_R_VOL_SHFT)

//DAC control (0xc)
#define MU_DAC_L							    (1<<15)
#define MU_DA_TO_MIX						  (1<<14)
#define EN_A2A_PATH							  (1<<13)
#define MU_DAC_R							    (1<<7)
#define DAC_PRE_BOOST_SHFT				(0)
#define DAC_PRE_BOOST_MASK				(0x7f<<DAC_PRE_BOOST_SHFT)


//DAC volume (0xe)
#define DAC_VOL_L_SHFT						(8)
#define DAC_VOL_L_MASK						(0xff<<DAC_VOL_L_SHFT)
#define DAC_VOL_R_SHFT						(0)
#define DAC_VOL_R_MASK						(0xff<<DAC_VOL_R_SHFT)

//Mic input control (0x10)
#define MIC1_DF_INPUT						  (0x1<<15)
#define MIC1_VOL_SHFT             (8)
#define MIC1_VOL_MSK						  (0x1f<<MIC1_VOL_SHFT)
#define MIC2_DF_INPUT						  (0x1<<7)
#define MIC2_VOL_SHFT						  (0)
#define MIC2_VOL_MSK						  (0x1f<<MIC2_VOL_SHFT)

//REC Mixer Control (0x14)
#define MU_HPMIXl_TO_RECMIXL      (0x1 << 14)
#define MU_AUXMIXl_TO_RECMIXL     (0x1 << 13)
#define MU_SPK_TO_RECMIXL         (0x1 << 12)
#define MU_LIN1_L_TO_RECMIXL      (0x1 << 11)
#define MU_LIN2_L_TO_RECMIXL      (0x1 << 10)
#define MU_BST1_TO_RECMIXL        (0x1 << 9)
#define MU_BST2_TO_RECMIXL        (0x1 << 8)
#define MU_HPMIXR_TO_RECMIXR      (0x1 << 6)
#define MU_AUXMIXR_TO_RECMIXR     (0x1 << 5)
#define MU_SPK_TO_RECMIXR         (0x1 << 4)
#define MU_LIN1_R_TO_RECMIXR      (0x1 << 3)
#define MU_LIN2_R_TO_RECMIXR      (0x1 << 2)
#define MU_BST1_TO_RECMIXR        (0x1 << 1)
#define MU_BST2_TO_RECMIXR        (0x1 << 0)

//ADC vol (0x16)
#define ADC_VOL_L_SHFT						(8)
#define ADC_VOL_L_MASK						(0xff<<ADC_VOL_L_SHFT)
#define ADC_VOL_R_SHFT						(0)
#define ADC_VOL_R_MASK						(0xff<<ADC_VOL_R_SHFT)

//HP Mixer control (0x18)
#define MU_RECL_HPMIXL						(1<<13)
#define MU_MIC1_HPMIXL						(1<<12)
#define MU_MIC2_HPMIXL						(1<<11)
#define MU_LIN1_HPMIXL						(1<<10)
#define MU_LIN2_HPMIXL						(1<<9)
#define MU_DACL_HPMIXL						(1<<8)
#define MU_RECR_HPMIXR						(1<<5)
#define MU_MIC1_HPMIXR						(1<<4)
#define MU_MIC2_HPMIXR						(1<<3)
#define MU_LIN1_HPMIXR						(1<<2)
#define MU_LIN2_HPMIXR						(1<<1)
#define MU_DACR_HPMIXR						(1<<0)

//AUX Mixer control (0x1a)
#define MU_RECL_AUXMIXL						(1<<13)
#define MU_MIC1_AUXMIXL						(1<<12)
#define MU_MIC2_AUXMIXL						(1<<11)
#define MU_LIN1_AUXMIXL						(1<<10)
#define MU_LIN2_AUXMIXL						(1<<9)
#define MU_DACL_AUXMIXL						(1<<8)
#define MU_RECR_AUXMIXR						(1<<5)
#define MU_MIC1_AUXMIXR						(1<<4)
#define MU_MIC2_AUXMIXR						(1<<3)
#define MU_LIN1_AUXMIXR						(1<<2)
#define MU_LIN2_AUXMIXR						(1<<1)
#define MU_DACR_AUXMIXR						(1<<0)

//SPK MIXER control (0x1c)
#define SPK_VOL_SEL_SHFT					10
#define SPK_VOL_SEL_MASK					(0x3<<SPK_VOL_SEL_SHFT)
#define SPK_VOL_SEL_VMID					(0 << SPK_VOL_SEL_SHFT)
#define SPK_VOL_SEL_HP						(1 << SPK_VOL_SEL_SHFT)
#define SPK_VOL_SEL_SPK						(2 << SPK_VOL_SEL_SHFT)
#define SPK_VOL_SEL_AUX						(3 << SPK_VOL_SEL_SHFT)
#define MU_MIC1_TO_SPKMIX					(1<<7)
#define MU_MIC2_TO_SPKMIX					(1<<6)
#define MU_LIN1_L_TO_SPKMIX				(1<<5)
#define MU_LIN1_R_TO_SPKMIX				(1<<4)
#define MU_LIN2_L_TO_SPKMIX				(1<<3)
#define MU_LIN2_R_TO_SPKMIX				(1<<2)
#define MU_DACL_TO_SPKMIX					(1<<1)
#define MU_DACR_TO_SPKMIX					(1<<0)

//SPK AMP control
#define ABD_MOD_D							    (1<<15)

//MIC CTRL (0x22)
#define SEL_BST1_SHFT						  (12)
#define SEL_BST2_SHFT						  (8)
#define SEL_BST1_MASK						  (0xf<<SEL_BST1_SHFT)
#define SEL_BST2_MASK						  (0xf<<SEL_BST2_SHFT)
#define SEL_BST_0DB							  (0x0)
#define SEL_BST_20DB						  (0x1)
#define SEL_BST_24DB						  (0x2)
#define SEL_BST_30DB						  (0x3)
#define SEL_BST_35DB						  (0x4)
#define SEL_BST_40DB						  (0x5)
#define SEL_BST_44DB						  (0x6)
#define SEL_BST_50DB						  (0x7)
#define SEL_BST_52DB						  (0x8)

//I2S Ctrl (0x34)
#define I2S_SLV								    (0x1<<15)
#define I2S_WIDTH_SHFT						(2)
#define I2S_WIDTH_MASK						(0x3<<I2S_WIDTH_SHFT)
#define I2S_WIDHT_16BIT						(0x0<<I2S_WIDTH_SHFT)
#define I2S_WIDHT_20BIT						(0x1<<I2S_WIDTH_SHFT)
#define I2S_WIDHT_24BIT						(0x2<<I2S_WIDTH_SHFT)
#define I2S_WIDHT_8BIT						(0x3<<I2S_WIDTH_SHFT)

//Internal clk select (0x42)
#define INTER_SYS_CLK_SHFT				(14)
#define INTER_SYS_CLK_MASK				(0x3<<INTER_SYS_CLK_SHFT)
#define INTER_SYS_CLK_MCLK				(0x0<<INTER_SYS_CLK_SHFT)
#define INTER_SYS_CLK_PLL					(0x1<<INTER_SYS_CLK_SHFT)
#define INTER_CLK_SRC_SHFT				(13)
#define INTER_CLK_SRC_MASK				(0x1<<INTER_CLK_SRC_SHFT)
#define INTER_CLK_SRC_MCLK				(0x0<<INTER_CLK_SRC_SHFT)
#define INTER_CLK_SRC_BCLK				(0x1<<INTER_CLK_SRC_SHFT)
#define INTER_CLK_PRE_DEVIDER			(0x1<<11)

//PLL func ctrl (0x44)
#define PLL1_FUNC_N_SHFT					(8)
#define PLL1_FUNC_N_MASK					(0xff<<PLL1_FUNC_N_SHFT)
#define PLL1_FUNC_MBYPASS_SHFT		(7)
#define PLL1_FUNC_MBYPASS_MASK		(0x1<<PLL1_FUNC_MBYPASS_SHFT)
#define PLL1_FUNC_MBYPASS					(0x1<<PLL1_FUNC_MBYPASS_SHFT)
#define PLL1_FUNC_K_SHFT					(4)
#define PLL1_FUNC_K_MASK					(0x7<<PLL1_FUNC_K_SHFT)
#define PLL1_FUNC_M_SHFT					(0)
#define PLL1_FUNC_M_MASK					(0xf<<PLL1_FUNC_M_SHFT)

//Power managment addition 1 (0x3A),0:Disable,1:Enable
#define PWR_MAIN_I2S_EN						(0x1<<15)
#define PWR_CLSD_EN							  (0x1<<13)
#define PWR_ADC_L_EN						  (0x1<<12)
#define PWR_ADC_R_EN						  (0x1<<11)
#define PWR_DAC_L_EN						  (0x1<<10)
#define PWR_DAC_R_EN						  (0x1<<9)
#define PWR_DAC_REF_EN						(0x1<<8)
#define PWR_DAC_DF2SE_L_EN				(0x1<<7)
#define PWR_DAC_DF2SE_R_EN				(0x1<<6)
#define PWR_CLSAB_EN						  (0x1<<5)

//Power managment addition 2 (0x3B),0:Disable,1:Enable
#define PWR_HPMIX_L_EN						(0x1<<15)
#define PWR_HPMIX_R_EN						(0x1<<14)
#define PWR_AUXMIX_L_EN						(0x1<<13)
#define PWR_AUXMIX_R_EN						(0x1<<12)
#define PWR_RECMIX_L_EN						(0x1<<11)
#define PWR_RECMIX_R_EN						(0x1<<10)
#define PWR_LINE1_L_EN						(0x1<<9)
#define PWR_LINE1_R_EN						(0x1<<8)
#define PWR_LINE2_L_EN						(0x1<<7)
#define PWR_LINE2_R_EN						(0x1<<6)
#define PWR_MIC1_EN							  (0x1<<5)
#define PWR_MIC2_EN							  (0x1<<4)
#define PWR_MIC_BIAS_EN						(0x1<<3)
#define PWR_PLL1_EN							  (0x1<<1)
#define PWR_SPKMIX_EN						  (0x1<<0)

//Power managment addition 3(0x3C),0:Disable,1:Enable
#define PWR_VREF_EN							  (0x1<<15)
#define PWR_FASTB_EN						  (0x1<<14)
#define PWR_MAIN_BIAS_EN					(0x1<<13)
#define PWR_HP_EN							    (0x1<<3)
#define PWR_HP_OUT							  (0x1<<1)
#define PWR_HP_AMP							  (0x1<<0)

//Power managment addition 4(0x3E),0:Disable,1:Enable
#define PWR_SPK_L_VOL						  (0x1<<15)
#define PWR_SPK_R_VOL						  (0x1<<14)
#define PWR_HP_L_VOL						  (0x1<<11)
#define PWR_HP_R_VOL						  (0x1<<10)
#define PWR_AUX_L_VOL						  (0x1<<9)
#define PWR_AUX_R_VOL						  (0x1<<8)

//Class AB amplify ratio gain (private 0x45)
#define FBGAIN_CLSAB_SHFT					(13)
#define FBGAIN_CLSAB_MASK					(0x7<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_1P00x				(0x0<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_1P25x				(0x1<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_1P50x				(0x2<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_1P75x				(0x3<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_2P00x				(0x4<<FBGAIN_CLSAB_SHFT)
#define FBGAIN_CLSAB_2P25x				(0x5<<FBGAIN_CLSAB_SHFT)

//Depop hpout(0x54)
#define SOFT_GENERATER	          (0x1 << 15)
#define ENABLE_SOFT_TRIGGER	      (0x1 << 14)
#define DEPOP_MODE2_HPOUT	        (0x1 << 7)
#define ENABLE_HPL_UNMUTE_DEPOP	  (0x1 << 1)
#define ENABLE_HPR_UNMUTE_DEPOP	  (0x1 << 0)

int ar_rt5633_play(int resolution, int clk, int volume);
int ar_rt5633_record(    int resolution, int clk, int mic_bitmap, int mic_vol);
#endif//_RT5633_AUDIO_H_
