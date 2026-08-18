#include "csl.h"
#include "dma_driver.h"
#include "dma_int.h"
#include "dma_sync_fifo.h"
#ifndef CEVAXM
#include <Simulator.h>
#else
#include <vec-c.h>
#endif

dma_status_e dma_init_manager()
{
	dma_status_e status = DMA_STATUS_OK;
	status = dma_sync_message_fifo_init();
	if ( DMA_STATUS_OK == status)
	{
	#ifndef CEVAXM
        init_manager();
		return DMA_STATUS_OK;
	#else
        mss_data_memory_region_t mem_region;
        // Set max burst for a region, use the global outstanding burst limits
	    mss_set_ddma_global_max_outstanding(MSS_DDMA_GLOBAL_MAX_OUTSTANDING_READ_16, MSS_DDMA_GLOBAL_MAX_OUTSTANDING_WRITE_32);
	    memset (&mem_region, 0, sizeof(mss_data_memory_region_t));
	    mem_region.master_id = MSS_DATA_MEM_REGION_MASTER_EDP;
	    mem_region.ddma_max_burst_size  = MSS_DDMA_MAX_BURST_SIZE_16_INCR;
	    mss_set_data_memory_region(&mem_region);
		return DMA_STATUS_OK;
	#endif
	}
	return status;
}

dma_status_e dma_deinit_manager()
{
#ifndef CEVAXM
	return deinit_manager()? DMA_STATUS_OK : DMA_STATUS_MANAGER_FAILURE;
#else
	return DMA_STATUS_OK;
#endif
}

dma_status_e dma_enable_queue(dma_queue_base_t* queue)
{
#ifdef _DMA_API_CHECK_
	if (queue == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

	return DMA_STATUS_NOT_IMPLEMENTED;
}

dma_status_e dma_disable_queue(dma_queue_base_t* queue)
{
#ifdef _DMA_API_CHECK_
	if (queue == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

	return DMA_STATUS_NOT_IMPLEMENTED;
}


dma_status_e dma_allocate_queue(dma_queue_base_t* out_queue, uint32_t* start_address, uint32_t priority, uint32_t depth)
{
	uint32_t i = 0;
    dma_queue_depth_t depth_reg;
#ifdef CEVAXM
	uint32_t rst;
	uint32_t id;
	uint32_t mask_id;
	uint32_t queue_base_addr;
#endif

	if (out_queue == NULL || depth <= 1 || depth > QUEUE_MAX_DEPTH)
    {
		return DMA_STATUS_ERROR_PARAMS;
    }

    // round down the depth to the power of 2:
	while ((depth >>= 1) != 1)
    {
		i++;
    }
	depth = 1 << (i + 1);
    depth_reg.overlay = 0;
	depth_reg.fields.depth = depth - 1;
	depth_reg.fields.pri_abs_sel = 1;
	depth_reg.fields.pri_abs = priority;
	depth_reg.fields.enable = 1;

	out_queue->read_ptr = 0;
	out_queue->write_ptr = 0;
	out_queue->last_core_id = 0xFFFFFFFF;
	out_queue->start_address = (uint32_t)start_address;
    out_queue->queue_depth_register = depth_reg.overlay;
	out_queue->sync_counter = 0;
	out_queue->current_sync_value = 0;
	dma_create_internal_message_desc(&out_queue->sync_message_desc);

#ifndef CEVAXM
    i = allocate_queue(priority, depth);
    if (i == -1)
        return DMA_STATUS_ALLOC_FAILED;
    out_queue->id = i;
	return DMA_STATUS_OK;
#else
	rst = cpm_in(QMAN_RST_ADDR);
	id = ffb(set, lsb, rst);
	mask_id = ~(1 << id);
	queue_base_addr = id << QMAN_X_OFFSET_SHIFT;

	if (id > 128)
		return DMA_STATUS_ALLOC_FAILED;

	rst &= mask_id;
	out_queue->id = id;

	cpm_out(QX_BASE_PTR    + queue_base_addr, (uint32_t)out_queue);
	cpm_out(QX_FIRST_ADDR  + queue_base_addr, (uint32_t)out_queue->start_address);
	cpm_out(QX_CHNK_SIZE   + queue_base_addr, QMAN_DESC_CHK_SIZE);
	cpm_out(QX_BASE_PTR    + queue_base_addr, (uint32_t)out_queue);
	cpm_out(QX_DSC_EN_INC0 + queue_base_addr, QMAN_DESC_INIT_INC_VALUE);
	cpm_out(QX_DSC_EN_INC1 + queue_base_addr, QMAN_DESC_INIT_INC_VALUE);
	cpm_out(QMAN_RST_ADDR, rst);
	cpm_out(QX_EN_DEPTH    + queue_base_addr, depth_reg.overlay);
	return DMA_STATUS_OK;
#endif
}


dma_status_e dma_delete_queue(dma_queue_base_t* queue)
{
#ifdef _DMA_API_CHECK_
	if (queue == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

#ifndef CEVAXM
    return delete_queue(queue->id) ? DMA_STATUS_OK : DMA_STATUS_ERROR_PARAMS;
#else
    {
    	uint32_t mask_id;
	    uint32_t rst;
	    rst = cpm_in(QMAN_RST_ADDR);
	    mask_id = (1 << queue->id);
	    cpm_out(QMAN_RST_ADDR, mask_id | rst);
	    return DMA_STATUS_OK;
    }
#endif
}

#if defined XM6 || (defined XM4 && !defined CEVA_DMA_EXTENDED_DESC)
	// For these cases we have the assembly routine, but may need a wrapper for API check
	#if defined _DMA_API_CHECK_
	dma_status_e dma_enqueue_sync_point_asm(dma_queue_base_t* queue, uint32_t* value);

	dma_status_e dma_enqueue_sync_point(dma_queue_base_t* queue, uint32_t* p_value)
	{
		if (queue == NULL)
			return DMA_STATUS_ERROR_PARAMS;
		return dma_enqueue_sync_point_asm(queue, p_value);
	}
	#endif
#else
	// C implementation, based on dma_enqueue_desc, with optional API check
	dma_status_e dma_enqueue_sync_point(dma_queue_base_t* queue, uint32_t* p_value)
	{
		#ifdef _DMA_API_CHECK_
		if (queue == NULL)
			return DMA_STATUS_ERROR_PARAMS;
		#endif //_DMA_API_CHECK_

		{
			uint32_t value = ++queue->sync_counter;
			*p_value = value;
			#ifndef CEVAXM
				DEBUG_LOG(LOG_PREFIX "Set sync point %d\0", value);
			#endif
			return dma_enqueue_desc(queue, &queue->sync_message_desc, (void*)value, (void*)&queue->current_sync_value);
		}
	}
#endif


dma_status_e dma_wait_sync_point(dma_queue_base_t* queue, uint32_t value)
{

#ifdef _DMA_API_CHECK_
	if (queue == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

	// Wait until the sync point is reached
	#ifndef CEVAXM
		DEBUG_LOG(LOG_PREFIX "Wait on sync point %d", value);
	#endif

	while(value > queue->current_sync_value);

	return DMA_STATUS_OK;
}


dma_status_e dma_reset_sync_point(dma_queue_base_t* queue)
{
#ifdef _DMA_API_CHECK_
	if (queue == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

	queue->sync_counter = 0;
	queue->current_sync_value = 0;
	return DMA_STATUS_OK;
}
