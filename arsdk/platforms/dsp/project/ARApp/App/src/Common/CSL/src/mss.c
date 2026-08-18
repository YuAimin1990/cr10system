/*****************************************************************************\
* CEVA Confidential property.
* Copyright (C) Ceva Inc. All rights reserved.
*
* This file constitutes proprietary and confidential information of CEVA Inc.
* Any use or copying of this file or any information contained in this file other
* than as expressly approved in writing by Ceva, Inc. is strictly prohibited.
* Any disclosure or distribution of this file or any information contained in
* this file except to the intended recipient is strictly prohibited.
\*****************************************************************************/

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include "cevaxm.h"
#ifdef CEVAXM
#include "csl.h"

// Instruction Cache line size in bytes
#define MSS_ICACHE_LINE_SIZE 64 
unsigned long long mss_get_clock() PRAGMA_CSECT("CSL_INTERNAL_CODE_ALLOCATION");
void mss_load_internal_program(unsigned int int_address, unsigned int ext_address, unsigned int size) PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
void mss_set_program_memory_region(mss_program_memory_region_t *region_settings) PRAGMA_CSECT("CSL_INTERNAL_CODE_ALLOCATION");
void _mss_set_program_memory_region(mss_program_memory_region_t *region_settings, int do_invalidate) PRAGMA_CSECT("CSL_INTERNAL_CODE_ALLOCATION");
void mss_set_software_operation_program_cache_config(
	unsigned int start_address,
	mss_program_cache_operation_type_e operation_type,
	mss_program_cache_operation_size_e operation_size,
	unsigned int cache_lines_num) PRAGMA_CSECT("CSL_INTERNAL_CODE_ALLOCATION");
void mss_enable_global_program_cache() PRAGMA_CSECT("CSL_INTERNAL_CODE_ALLOCATION");
unsigned int mss_get_program_access_violation_address() PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
unsigned int mss_get_data_access_violation_address() PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
unsigned int mss_get_internal_program_memory_size() PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
unsigned int mss_get_program_cache_size() PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
void mss_set_ddma_global_max_outstanding(mss_ddma_global_max_outstanding_read_e outstanding_read, mss_ddma_global_max_outstanding_write_e outstanding_write) PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
void mss_set_power_save_mode(mss_power_save_mode_e mode) PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
void mss_set_data_memory_region(mss_data_memory_region_t *region_settings) PRAGMA_CSECT("CSL_EXTERNAL_CODE_ALLOCATION");
int g_is_iss PRAGMA_DSECT_LOAD("CSL_INTERNAL_DATA_ALLOCATION") = -1;

#ifdef XM6
#define MSS_WAIT() do { \
			__asm__("nop #0x1"); \
			__asm__("nop #0x3"); \
			__asm__("nop #0x3"); \
			__asm__("nop #0x1"); \
			} while(0)
#endif //XM6
#if !defined(CEVAXM)
#error Unsupported HW configuration
#endif 

#define MSS_POLL_CACHE_OP() do {\
    unsigned int val = mss_read_reg(MSS_PMSS_CCOCR_ADDR);\
    if ((val & 2) == 0) break; \
    } while (1)

unsigned long long mss_get_clock()
{
	unsigned int u32_clock_l, u32_clock_h;
	unsigned long long u64_clock;
	u32_clock_l = mss_read_reg(MSS_WD_SYSTIML_ADDR);
	u32_clock_h = mss_read_reg(MSS_WD_SYSTIMH_ADDR);
	u64_clock = u32_clock_h;
	u64_clock<<=32;
	u64_clock = u64_clock + u32_clock_l;
	return u64_clock;
}

void mss_load_internal_program(unsigned int int_address, unsigned int ext_address, unsigned int size)
{
	mss_pdma_transfer_control_reg_t pdtc;
	assert((int_address & 0x001F) == 0);//PDMA blocks are 32 byte aligned
	assert((ext_address & 0x001F) == 0);//PDMA blocks are 32 byte aligned
	assert((size & 0x001F) == 0); //PDMA blocks are 32 byte aligned (the hardware ignores 5 LSBs)
	assert(size <= MSS_MAX_PDMA_TRANSFER_SIZE); 
	do
	{
		pdtc.overlay = mss_read_reg(MSS_PMSS_PDTC_ADDR);
	} while (pdtc.field.pdst);
	pdtc.field.pdtc = size;
	mss_write_reg(MSS_PMSS_PDEA_ADDR, ext_address);
	mss_write_reg(MSS_PMSS_PDIA_ADDR, int_address);
	mss_write_reg(MSS_PMSS_PDTC_ADDR, pdtc.overlay);
	MSS_WAIT();
	do
	{
		pdtc.overlay = mss_read_reg(MSS_PMSS_PDTC_ADDR);
	} while (pdtc.field.pdst);
}

void _mss_set_program_memory_region(mss_program_memory_region_t* region_settings, int do_invalidate)
{
	unsigned int start_address = region_settings->start_address;
	unsigned int end_address;
	unsigned int pcr_val;

	mss_mem_region_e memory_region = region_settings->memory_region;
	mss_mem_region_active_e active = region_settings->active;
	mss_program_mem_region_cacheability_e cache_enable = region_settings->cache_enable;
	mss_program_mem_region_lock_after_line_fill_e cache_lock = region_settings->cache_lock;
	mss_access_protection_t access_protection = region_settings->access_protection;
	mss_mem_region_l2_cache_policy_t l2_cache_policy = region_settings->l2_cache_policy;
	unsigned int qos = region_settings->qos;
	mss_program_mem_region_start_reg_t start_address_reg;
	mss_program_mem_region_attrib_t attribute_reg;
	mss_program_mem_region_start_reg_t next_region;
	start_address_reg.overlay = 0;
	attribute_reg.overlay = 0;

	///// The following sequence is described in the MSS spec 2.10.3 Updating the IACU
    if (do_invalidate)
    {
	    // Invalidate cache entries
	    if ( memory_region < MSS_MEMORY_REGION_15)
	    {
		    next_region.overlay = mss_read_reg(MSS_PMSS_PADD_START_BASE_ADDR + memory_region + 0x10);
		    if (next_region.field.inactive == 0)
		    {
			    end_address = (next_region.field.p_region_start << 12) - 1; //The regions are in blocks of 4KB
		    }
		    else
		    {
			    end_address = 0xFFFFFFFF;
		    }
	    }
	    else
	    {
		    end_address = 0xFFFFFFFF;
	    }
	    mss_set_software_operation_program_cache_config(
			    region_settings->start_address,
			    MSS_PROGRAM_CACHE_OPERATION_INVALIDATE,
			    MSS_PROGRAM_CACHE_OPERATION_SIZE_BY_ADDRESS,
			    (end_address - start_address + 1) / MSS_ICACHE_LINE_SIZE );
    }

	pcr_val = mss_read_reg(MSS_PMSS_PCR_ADDR); // Read PCR register
	mss_write_reg(MSS_PMSS_PCR_ADDR, pcr_val & ~(1<<2)); // Disable HW pre-fetch
	MSS_WAIT();


	assert(memory_region < MSS_MEMORY_REGION_16); //Program has 16 memory regions only
	assert((memory_region != MSS_MEMORY_REGION_0) || (active == 0)); //For ADD0_START the active field is read-only, and is hardwired to the value of 0 (active)
	assert((start_address & 0x0FFF) == 0); //The regions are in blocks of 4KB
	start_address_reg.field.p_region_start = start_address >> 12; //The regions are in blocks of 4KB
	start_address_reg.field.inactive = active;
	mss_write_reg(MSS_PMSS_PADD_START_BASE_ADDR + memory_region, start_address_reg.overlay);
    MSS_WAIT();

	attribute_reg.field.cacheable = cache_enable;
	attribute_reg.field.lock_after_line_fill = cache_lock;
	attribute_reg.field.access_prot = access_protection;
	attribute_reg.field.l2_cache_policy = l2_cache_policy;
	assert(qos<16); //QoS is 4 bits only
	attribute_reg.field.read_qos = qos;
	mss_write_reg(MSS_PMSS_PADD_ATT0_BASE_ADDR + memory_region, attribute_reg.overlay);
	MSS_WAIT();

    mss_write_reg(MSS_PMSS_PCR_ADDR, pcr_val); // restore PCR value
    MSS_WAIT();
}

void mss_set_program_memory_region(mss_program_memory_region_t* region_settings)
{
    _mss_set_program_memory_region(region_settings, TRUE);
}

void mss_get_program_memory_region(mss_program_memory_region_t* region_settings)
{
	mss_mem_region_e memory_region = region_settings->memory_region;
	mss_program_mem_region_start_reg_t start_address_reg;
	mss_program_mem_region_attrib_t attribute_reg;
	start_address_reg.overlay = 0;
	attribute_reg.overlay = 0;

	start_address_reg.overlay = mss_read_reg(MSS_PMSS_PADD_START_BASE_ADDR + memory_region);
	region_settings->start_address = start_address_reg.field.p_region_start;
	region_settings->active = start_address_reg.field.inactive;
	MSS_WAIT();

	attribute_reg.overlay = mss_read_reg(MSS_PMSS_PADD_ATT0_BASE_ADDR + memory_region);
	region_settings->cache_enable = attribute_reg.field.cacheable;
	region_settings->cache_lock = attribute_reg.field.lock_after_line_fill;
	region_settings->access_protection = attribute_reg.field.access_prot;
	region_settings->l2_cache_policy = attribute_reg.field.l2_cache_policy;
	region_settings->qos = attribute_reg.field.read_qos;
	MSS_WAIT();
}

// return non-zero if in ISS simulation, zero otherwise
static int _mss_test_if_iss()
{
	mss_program_cache_software_operation_reg_t ccocr;
    // make sure that no operation is currently performed
    MSS_POLL_CACHE_OP();
    // Start a short invalidate operation (one line), and check that the "operation_status" (P_L1CO) is cleared.
    // If it stays at '1', this indicates ISS mode, which doesn't simulate cache operation.
	mss_write_reg(MSS_PMSS_CCOSAR_ADDR, 0);
	ccocr.overlay = 0;
	ccocr.field.operation_type = MSS_PROGRAM_CACHE_OPERATION_INVALIDATE;
	ccocr.field.operation_size = MSS_PROGRAM_CACHE_OPERATION_SIZE_BY_ADDRESS;
	ccocr.field.num_cache_lines = 1;
	ccocr.field.operation_status = 1;
	mss_write_reg(MSS_PMSS_CCOCR_ADDR, ccocr.overlay);
	MSS_WAIT();
	ccocr.overlay = mss_read_reg(MSS_PMSS_CCOCR_ADDR);
	return (ccocr.field.operation_status == 1);
}

void mss_set_software_operation_program_cache_config(
		unsigned int start_address,
		mss_program_cache_operation_type_e operation_type,
		mss_program_cache_operation_size_e operation_size,
		unsigned int cache_lines_num)
{
    unsigned int cache_size;
    unsigned int max_cache_lines;
    
    mss_program_cache_software_operation_reg_t ccocr;
    if (g_is_iss != 0)
    {
        return;
    }

    cache_size = mss_get_program_cache_size();

    //No operation to perform when there is no cache
    if (cache_size == 0)
        return;

    max_cache_lines = cache_size / MSS_ICACHE_LINE_SIZE;

    if (MSS_PROGRAM_CACHE_OPERATION_SIZE_FULL == operation_size)
        cache_lines_num = max_cache_lines;


    assert((start_address & 0x3F) == 0); //6 LSB are reserved

    ccocr.overlay = 0;
    ccocr.field.operation_type = operation_type;
    ccocr.field.operation_size = operation_size;
    ccocr.field.num_cache_lines = max_cache_lines;

    //loop the entire region by cache size 
    while (cache_lines_num >= max_cache_lines) {
        // make sure that no operation is currently performed
        MSS_POLL_CACHE_OP();
        mss_write_reg(MSS_PMSS_CCOSAR_ADDR, start_address);
        ccocr.field.operation_status = 1;
        mss_write_reg(MSS_PMSS_CCOCR_ADDR, ccocr.overlay);
        MSS_WAIT();
        start_address += cache_size;
        cache_lines_num -= max_cache_lines;
    }

    if (cache_lines_num > 0)
    {
        // make sure that no operation is currently performed
        MSS_POLL_CACHE_OP();
        mss_write_reg(MSS_PMSS_CCOSAR_ADDR, start_address);
        ccocr.field.num_cache_lines = cache_lines_num;
        ccocr.field.operation_status = 1;
        mss_write_reg(MSS_PMSS_CCOCR_ADDR, ccocr.overlay);
        MSS_WAIT();
    }
    // wait until the operation completes
    MSS_POLL_CACHE_OP();
}

void mss_enable_global_program_cache()
{
    unsigned int pcr_val;

    //Set region 0 to start from 0 and be cacheable
    mss_program_memory_region_t region_settings0 = { 0x0, MSS_MEMORY_REGION_0, MSS_MEMORY_REGION_ACTIVE, MSS_PROGRAM_MEM_REGION_CACHE_ENABLE, MSS_PROGRAM_MEM_REGION_LOCK_AFTER_FILL_DISABLE, 0, 0, 0 };

    if (g_is_iss == -1)
	{
		g_is_iss = _mss_test_if_iss();
	}

	mss_set_software_operation_program_cache_config(0x0, MSS_PROGRAM_CACHE_OPERATION_INVALIDATE, MSS_PROGRAM_CACHE_OPERATION_SIZE_FULL, 0);
	_mss_set_program_memory_region(&region_settings0, FALSE); // don't invalidate, as this was already done for the entire cache

    // Enable HW pre-fetch mechanism to improve cache performance
    pcr_val = mss_read_reg(MSS_PMSS_PCR_ADDR);
    mss_write_reg(MSS_PMSS_PCR_ADDR, pcr_val | (1 << 2)); 
    MSS_WAIT();
}

unsigned int mss_get_program_access_violation_address()
{
	unsigned int mapar = 0;
	unsigned int mapsr = mss_read_reg(MSS_PMSS_MAPSR_ADDR);
	if (mapsr & 0x01) //Check if access violation occurred
	{
		mapar = mss_read_reg(MSS_PMSS_MAPAR_ADDR);
		mss_write_reg(MSS_PMSS_MAPSR_ADDR, 0);//Clear access violation bit
		MSS_WAIT();
	}
	return mapar;
}

unsigned int mss_get_data_access_violation_address()
{
	unsigned int mapar = 0;
	unsigned int mapsr = mss_read_reg(MSS_AP_MAPSR_ADDR);
	if (mapsr & 0x01) //Check if access violation occurred
	{
		mapar = mss_read_reg(MSS_AP_MAPAR_ADDR);
		mss_write_reg(MSS_AP_MAPSR_ADDR, 0);//Clear access violation bit
		MSS_WAIT();
	}
	return mapar;
}

unsigned int mss_get_internal_program_memory_size()
{
	unsigned int retVal;
	mss_hardware_memory_config_reg_t hdcfg;
	hdcfg.overlay = mss_read_reg(MSS_DMSS_HDCFG_ADDR);
	retVal = hdcfg.field.prog_tcm_size;
    // Program TCM memory size:
    // 000: 0 KB (No TCM)
    // 001: 32 KB
    // 010: 64 KB
    // 011: 128 KB
    // 100: 256 KB
    // 101-111: Reserved
	if (retVal > 0)
		retVal = 0x8000 << (retVal - 1);
	return retVal;
}

unsigned int mss_get_internal_data_memory_size()
{
	unsigned int retVal;
	mss_hardware_memory_config_reg_t hdcfg;
	hdcfg.overlay = mss_read_reg(MSS_DMSS_HDCFG_ADDR);
	retVal = hdcfg.field.data_tcm_size;
    // IDM size:
    // 000: Reserved
    // 001: 128 KB (XM4 only)
    // 010: Reserved
    // 011: 256 KB
    // 100: 512 KB
    // 101: 1024 KB
    // 110: 2048 KB (XM6 only)
	if (retVal == 1)
        retVal = 0x20000;
    else if (retVal >= 3)
		retVal = 0x40000 << (retVal - 3);
	else
		retVal = 0; // should not happen

    return retVal;
}

unsigned int mss_get_program_cache_size()
{
	unsigned int retVal;
	mss_hardware_memory_config_reg_t hdcfg;
	hdcfg.overlay = mss_read_reg(MSS_DMSS_HDCFG_ADDR);
	retVal = hdcfg.field.prog_cache_size;
    // Instruction cache memory size:
    // 000: 0 KB (no cache)
    // 001: 32 KB cache
    // 010: 64 KB cache
    // 011: 128 KB cache
	if (retVal > 0)
		retVal = 0x8000 << (retVal - 1);
	return retVal;
}

void mss_set_ddma_global_max_outstanding(mss_ddma_global_max_outstanding_read_e outstanding_read, mss_ddma_global_max_outstanding_write_e outstanding_write)
{
	mss_ddma_max_outstanding_config_reg_t dacc;
	dacc.overlay = mss_read_reg(MSS_DMSS_DACC_ADDR);
	dacc.field.ddma_max_outstanding_read = outstanding_read;
	dacc.field.ddma_max_outstanding_write = outstanding_write;
	mss_write_reg(MSS_DMSS_DACC_ADDR, dacc.overlay);
	MSS_WAIT();
}

void mss_set_power_save_mode(mss_power_save_mode_e mode)
{
	mss_power_save_mode_reg_t psvm;
	psvm.overlay = mss_read_reg(MSS_PSU_PSVM_ADDR);
	psvm.field.power_save_mode = mode;
	mss_write_reg(MSS_PSU_PSVM_ADDR, psvm.overlay);
	MSS_WAIT();
    MSS_WAIT();
}

void mss_set_access_protection(mss_program_access_protection_e program_access_protection, mss_data_access_protection_e data_access_protection)
{
	mss_program_mem_config_reg_t pcr;
	mss_memory_access_config_reg_t sdcfg;
	pcr.overlay = mss_read_reg(MSS_PMSS_PCR_ADDR);
	sdcfg.overlay = mss_read_reg(MSS_DMSS_SDCFG_ADDR);
	pcr.field.access_prot_enable = program_access_protection;
	sdcfg.field.data_access_prot = data_access_protection;
	mss_write_reg(MSS_PMSS_PCR_ADDR, pcr.overlay);
	mss_write_reg(MSS_DMSS_SDCFG_ADDR, sdcfg.overlay);
	MSS_WAIT();
}

void mss_get_data_memory_region(mss_data_memory_region_t *region_settings)
{
	mss_mem_region_e memory_region = region_settings->memory_region;
	mss_data_mem_region_reg_t start_address_reg;
	mss_data_mem_region_attrib0_reg_t attribute_reg0;
	mss_data_mem_region_attrib1_reg_t attribute_reg1;

	start_address_reg.overlay = 0;
	attribute_reg0.overlay = 0;
	attribute_reg1.overlay = 0;

	start_address_reg.overlay = mss_read_reg(MSS_IC_ADD_START_BASE_ADDR + memory_region);
	region_settings->start_address = start_address_reg.field.region_start;
	region_settings->master_id = start_address_reg.field.master_id;
	region_settings->active = start_address_reg.field.inactive;
	MSS_WAIT();

	attribute_reg0.overlay = mss_read_reg(MSS_IC_ADD_ATT0_BASE_ADDR + memory_region);
	region_settings->memory_ordering = attribute_reg0.field.memory_ordering;
	region_settings->access_protection = attribute_reg0.field.access_prot;
	region_settings->read_qos = attribute_reg0.field.read_qos;
	region_settings->write_qos = attribute_reg0.field.write_qos;
	MSS_WAIT();

	attribute_reg1.overlay = mss_read_reg(MSS_IC_ADD_ATT1_BASE_ADDR + memory_region);
	region_settings->ddma_read_policy = attribute_reg1.field.ddma_read_policy;
	region_settings->l2_cache_write_policy = attribute_reg1.field.l2_cache_write_policy;
	region_settings->l2_cache_read_policy = attribute_reg1.field.l2_cache_read_policy;
	region_settings->ddma_max_burst_size = attribute_reg1.field.ddma_max_burst_size;
	region_settings->ddma_max_outstanding_read = attribute_reg1.field.ddma_max_outstanding_read;
	region_settings->ddma_max_outstanding_write = attribute_reg1.field.ddma_max_outstanding_write;
	MSS_WAIT();
}

void mss_set_data_memory_region(mss_data_memory_region_t *region_settings)
{
	unsigned int start_address = region_settings->start_address;
	mss_mem_region_e memory_region = region_settings->memory_region;
	mss_data_mem_region_master_id_e master_id = region_settings->master_id;
	mss_mem_region_active_e active = region_settings->active;
	mss_data_memory_ordering_model_e mom = region_settings->memory_ordering;
	mss_access_protection_t access_protection = region_settings->access_protection;
	unsigned int read_qos = region_settings->read_qos;
	unsigned int write_qos = region_settings->write_qos;
	mss_ddma_read_policy_e exact_read = region_settings->ddma_read_policy;
	mss_mem_region_l2_cache_policy_t l2_wr_cache_policy = region_settings->l2_cache_write_policy;
	mss_mem_region_l2_cache_policy_t l2_rd_cache_policy = region_settings->l2_cache_read_policy;
	mss_data_mem_region_max_burst_size_e burst_size = region_settings->ddma_max_burst_size;
	mss_ddma_region_max_outstanding_read_e outstanding_read = region_settings->ddma_max_outstanding_read;
	mss_ddma_region_max_outstanding_write_e outstanding_write = region_settings->ddma_max_outstanding_write;
	mss_data_mem_region_reg_t start_address_reg;
	mss_data_mem_region_attrib0_reg_t attribute_reg0;
	mss_data_mem_region_attrib1_reg_t attribute_reg1;
	start_address_reg.overlay = 0;
#ifdef XM6
	start_address_reg.field.dpraw = 1;
#endif //XM6
	attribute_reg0.overlay = 0;
	attribute_reg1.overlay = 0;

	assert((memory_region != MSS_MEMORY_REGION_0) || (start_address == 0)); //For ADD0_START the start address is read-only, and is hardwired to the value of 0x0_0000
	assert((memory_region != MSS_MEMORY_REGION_0) || (active == 0)); //For ADD0_START the active field is read-only, and is hardwired to the value of 0 (active)
	assert((start_address & 0x0FFF) == 0); //The regions are in blocks of 4KB
	start_address_reg.field.region_start = start_address >> 12; //The regions are in blocks of 4KB
	start_address_reg.field.master_id = master_id;
	start_address_reg.field.inactive = active;
	MSS_WAIT();
	mss_write_reg(MSS_IC_ADD_START_BASE_ADDR + memory_region, start_address_reg.overlay);

	attribute_reg0.field.memory_ordering = mom;
	attribute_reg0.field.access_prot = access_protection;
	assert(read_qos < 16); //QoS is 4 bits only
	assert(write_qos < 16); //QoS is 4 bits only
	attribute_reg0.field.read_qos = read_qos;
	attribute_reg0.field.write_qos = write_qos;
	MSS_WAIT();
	mss_write_reg(MSS_IC_ADD_ATT0_BASE_ADDR + memory_region, attribute_reg0.overlay);

	attribute_reg1.field.ddma_read_policy = exact_read;
	attribute_reg1.field.l2_cache_write_policy = l2_wr_cache_policy;
	attribute_reg1.field.l2_cache_read_policy = l2_rd_cache_policy;
	attribute_reg1.field.ddma_max_burst_size = burst_size;
	attribute_reg1.field.ddma_max_outstanding_read = outstanding_read;
	attribute_reg1.field.ddma_max_outstanding_write = outstanding_write;
	MSS_WAIT();
	mss_write_reg(MSS_IC_ADD_ATT1_BASE_ADDR + memory_region, attribute_reg1.overlay);
	MSS_WAIT();
}
#endif // CEVAXM
