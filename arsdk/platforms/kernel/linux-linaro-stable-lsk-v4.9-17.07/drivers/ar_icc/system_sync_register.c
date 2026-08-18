/*
 * system_sync_register.c
 *
 *  Created on: 2018年8月15日
 *      Author: ylu
 */



#include "system_sync_register.h"
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/io.h>
//#include "assert.h"
//#include "stdio.h"
//#include "stdlib.h"

volatile sys_sync_reg_t *sys_sync_reg_st = (sys_sync_reg_t *)SYSTEM_SYNC_REG_BASE; // SRAM_ADDR

extern int ar_ceva_mapset_reg(enum sys_sync_reg_cmd cmd_id,int value){
	unsigned int *reserve_address_tmp;
//	reserve_address_tmp=ioremap_nocache((&sys_sync_reg_st->dsp_magic_reg),4096);
//    *(reserve_address_tmp+cmd_id)=value;
//	iounmap(reserve_address_tmp);
	return 0;
}

/*
unsigned int ar_ceva_get_reg(enum dsp_reg_cmd cmd_id){
	//assert(DSP_REG->AR_MAGIC == 0x41424344);
	return *(&DSP_REG->AR_MAGIC_REG+cmd_id);
}
*/


