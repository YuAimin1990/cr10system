/*
 * system_sync_register.h
 *
 *  Created on: 2018年8月15日
 *      Author: ylu
 */

#ifndef SYSTEM_SYNC_REGISTER_H_
#define SYSTEM_SYNC_REGISTER_H_
#ifdef __cplusplus
extern "C"{
#endif

enum sys_sync_reg_cmd
{
	AR_MAGIC=0,
	AR_DSP_CEVA0_BOOTUP_ID,
	AR_DSP_CEVA1_BOOTUP_ID,
	AR_DSP_CEVA2_BOOTUP_ID,
	AR_DSP_CEVA3_BOOTUP_ID,
	AR_DSP_BOOTUP_BITMAP_ID,
	AR_DSP_RTOS_BOOTUP_ID,
	AR_DSP_LINUX_ICC_RDY_ID,
	AR_DSP_LINUX_FS_RDY_ID,
	AR_DSP_CEVA0_VERSION_ID,
	AR_DSP_CEVA1_VERSION_ID,
	AR_DSP_CEVA2_VERSION_ID,
	AR_DSP_CEVA3_VERSION_ID,
	AR_DSP_WEIGHT_FILE_RDY_ID,
	AR_DSP_CTRL_ID,
	AR_DSP_CMD_ID_MAX

};

typedef	struct{
	volatile    unsigned int    dsp_magic_reg;   //0x41     ;'A' 'R' 'T' 'O'
	volatile	unsigned int	dsp_ceva0_bootup_reg;
	volatile	unsigned int	dsp_ceva1_bootup_reg;
	volatile	unsigned int	dsp_ceva2_bootup_reg;
	volatile	unsigned int	dsp_ceva3_bootup_reg;
	volatile	unsigned int	dsp_bootup_bitmap_reg;
	volatile	unsigned int	dsp_rtos_bootup_reg;
	volatile	unsigned int	dsp_linux_icc_rdy_reg;
	volatile	unsigned int	dsp_linux_fs_rdy_reg;
	volatile	unsigned int	dsp_ceva0_version_reg;
	volatile	unsigned int	dsp_ceva1_version_reg;
	volatile	unsigned int	dsp_ceva2_version_reg;
	volatile	unsigned int	dsp_ceva3_version_reg;
	volatile	unsigned int	dsp_weight_file_rdy_reg;
	volatile	unsigned int	dsp_ctrl_reg;
	//volatile	unsigned int	DSP_LOG_BUFFER_REG;
	volatile    unsigned int    reserved;  //TO DO: Add your own register here...
}sys_sync_reg_t;

extern volatile sys_sync_reg_t *sys_sync_reg_st; // SRAM_ADDR

extern int ar_ceva_mapset_reg(enum sys_sync_reg_cmd cmd_id,int value);
//extern unsigned int ar_ceva_get_reg(enum dsp_reg_cmd cmd_id);

#ifdef __cplusplus
}
#endif
#endif /* SYSTEM_SYNC_REGISTER_H_ */


