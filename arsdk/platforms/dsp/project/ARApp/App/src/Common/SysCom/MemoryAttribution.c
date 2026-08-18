/******************************************************************************
* Copyright (C), artosyn
* File name: memory_attribution.c
* Author : jianliu    Version:Initial Draft     Create: 2017骞�6鏈�15鏃�
* Description : cofigure address region attribution
* Others :
* History :
* 1.Date : 2017骞�6鏈�15鏃�   Author : jianliu   Modify content : Create File
*
******************************************************************************/

/*----------------------------------------------*
 * header file                                   *
 *----------------------------------------------*/

#include <vec-c.h>
#include <stdio.h>
#include "cevaxm.h"
#include "cpm_io.h"
#include "mss.h"

#include "MemoryAttribution.h"

int xConfigRegionCacheable(unsigned int addr, unsigned int region_index)
{
	addx_start_t region1_start = { 0 };
	addx_att1_t region1_att1 = { 0 };
	unsigned int addr_offset = ADD_ADDR_OFFSET * region_index;
	/* addr = start_addr * 4k  */
	unsigned int start_addr = addr >> 12;
	
	region1_start.field.region_start = start_addr;
	region1_start.field.region_mid=1;
	region1_start.field.inactive = 0;
	cpm_out((ADD0_START + addr_offset), region1_start.overlay);
	/* write_back and no-allocate */
	region1_att1.field.lxa_wr = 0x7;
	region1_att1.field.lxa_rd = 0xb;
	cpm_out((ADD0_ATT1 + addr_offset), region1_att1.overlay);

	return 0;
}

unsigned int xGetRegionStartAddr(const unsigned int region_index)
{
	unsigned int addr_offset = ADD_ADDR_OFFSET * region_index;
	addx_start_t region1_start;

	region1_start.overlay = cpm_in((ADD0_START + addr_offset));

	return region1_start.field.region_start;
}

int xConfigRegionNonCacheable(unsigned int addr, unsigned int region_index)
{
	addx_start_t region1_start = { 0 };
	addx_att1_t region1_att1 = { 0 };
	unsigned int addr_offset = ADD_ADDR_OFFSET * region_index;
	/* addr = start_addr * 4k  */
	unsigned int start_addr = addr >> 12;

	region1_start.field.region_start = start_addr;
	region1_start.field.region_mid=1;
	region1_start.field.inactive = 0;
	cpm_out((ADD0_START + addr_offset), region1_start.overlay);
	/* write_back and no-allocate */
	region1_att1.field.lxa_wr = 0x2;
	region1_att1.field.lxa_rd = 0x2;
	cpm_out((ADD0_ATT1 + addr_offset), region1_att1.overlay);

	return 0;
}

int xConfigRegionProgramCacheable(unsigned int addr, unsigned int region_index)
{
	p_addx_start_t region1_start = { 0 };
	p_addx_attr0_t region1_att0 = { 0 };
	unsigned int addr_offset = P_ADD_ADDR_OFFSET * region_index;
	/* 	addr = start_addr * 4k  */
	unsigned int start_addr = addr >> 12;

	region1_start.field.region_start = start_addr;
	region1_start.field.inactive = 0;
	//debug_print("before P_ADD0_START\n");
	cpm_out((P_ADD0_START + addr_offset), region1_start.overlay);
	/* cacheable and no lock after cache-line fill */
	region1_att0.field.cacheable = 1;
	region1_att0.field.lock = 0;
	//debug_print("before P_ADD0_ATT0\n");
	cpm_out((P_ADD0_ATT0 + addr_offset), region1_att0.overlay);

	return 0;
}

int config_region_blank(unsigned int addr, unsigned int region_index)
{
	addx_start_t region1_start = { 0 };
	addx_att1_t region1_att1 = { 0 };
	addx_attr0_t region1_att0 = {0};
	unsigned int addr_offset = ADD_ADDR_OFFSET * region_index;
	/* 	addr = start_addr * 4k  */
	unsigned int start_addr = addr >> 12;

	region1_start.field.region_start = start_addr;
	region1_start.field.inactive = 0;
	region1_start.field.region_mid = 0x80;
	vInternalCpmWrite((ADD0_START + addr_offset), region1_start.overlay);
	region1_att0.field.ap = 0x111;
	vInternalCpmWrite((ADD0_ATT0 + addr_offset), region1_att0.overlay);

	/* write_back and no-allocate */
	//region1_att1.field.lxa_wr = 0x7;
	//region1_att1.field.lxa_rd = 0xb;
	//internal_cpm_write((ADD0_ATT1 + addr_offset), region1_att1.overlay);

	return 0;
}

void xConfigMssAccessProtectionEnable(unsigned int program_access_protection, unsigned int data_access_protection)
{
	mss_set_access_protection(program_access_protection, data_access_protection);
}

/*    bit (4-6)
 *     AP/MODE:     Supervisor       User0         User1
 *     000          r/w				r/w				r/w
 *     001			r/w				r/w				r
 *     010			r/w				r				r
 *     011			r/w				r/w				no access
 *     100			r/w				no access		no access
 *     101			r				r				r
 *     110			r				no access		no access
 *     111			no access		no access		no access
*/
/* @param addr : access protection start phy address
 * @param region index : program access protection region index [0 - 15], index 0 form addr 0
 * @param active : region is active, 0:active, 1:inactive
 * @param ap_value : access protection operation mode 000 - 111
*/
void xConfigRegionProgramAccessProtection(unsigned int addr, int active, unsigned int ap_value, unsigned int region_index, unsigned int is_global_pcache)
{
	//{ 0x0, MSS_MEMORY_REGION_0, MSS_MEMORY_REGION_ACTIVE, MSS_PROGRAM_MEM_REGION_CACHE_DISABLE, MSS_PROGRAM_MEM_REGION_LOCK_AFTER_FILL_DISABLE, 0, 0, 0 };
	mss_program_memory_region_t region_settings = {0};

	region_settings.memory_region = P_ADD_ADDR_OFFSET * region_index;
	mss_get_program_memory_region(&region_settings);

	if(is_global_pcache)
	{
		region_settings.cache_enable = MSS_PROGRAM_MEM_REGION_CACHE_ENABLE;
		region_settings.cache_lock = MSS_PROGRAM_MEM_REGION_LOCK_AFTER_FILL_DISABLE;
	}

	region_settings.start_address = addr;
	region_settings.access_protection = ap_value;
	region_settings.active = active;
	mss_set_program_memory_region(&region_settings);
}

/* @param addr : access protection start phy address
 * @param region index : data access protection region index [0 - 31], index 0 form addr 0
 * @param active : region is active, 0:active, 1:inactive
 * @param ap_value : access protection operation mode
 * @param is_global_dcache : is enable global data cache
*/
void xConfigRegionDataAccessProtection(unsigned int addr, int active, unsigned int ap_value, unsigned int region_index, unsigned int is_global_dcache)
{
	mss_data_memory_region_t region_settings = {0};

	region_settings.memory_region = ADD_ADDR_OFFSET * region_index;
	mss_get_program_memory_region(&region_settings);

	if(is_global_dcache)
	{
		region_settings.l2_cache_write_policy = 0x7;
		region_settings.l2_cache_read_policy = 0xb;
	}

	region_settings.start_address = addr;
	region_settings.master_id = MSS_DATA_MEM_REGION_MASTER_EDP;
	region_settings.active = active;
	region_settings.access_protection = ap_value;
	mss_set_data_memory_region(&region_settings);
}

/* @param stack_start_addr : defined the stack violation start address
 * @param stack_end_addr : defined the stack violation end address
 * @note : stack_end_addr<=stack access<=stack_start_addr
*/
void xConfigStackAccessVioltationRegion(unsigned int stack_start_addr, unsigned int stack_end_addr)
{
	cpm_out(DBG_STACK_START, stack_start_addr);
	cpm_out(DBG_STACK_END, stack_end_addr);
}
