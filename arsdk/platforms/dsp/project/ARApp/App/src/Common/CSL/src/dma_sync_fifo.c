/*
 * dma_sync_fifo.c
 *
 *  Created on: 22 ???? 2016
 *      Author: yurys
 */

///////////////////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "dma_driver.h"
#include "dma_int.h"
#include "dma_sync_fifo.h"
#include "cevaxm.h"

#define DMA_SYNC_MESSAGE_FIFO_SIZE 32

typedef struct
{
	dma_sync_message_t fifo[DMA_SYNC_MESSAGE_FIFO_SIZE];
	uint32_t read_idx;  // next index for the SW to read
	uint32_t write_idx; // next index for the HW to write
	uint32_t write_idx_to_enqueue; // next index to include in the sync message

	// private data
	dma_desc_t _msg_desc;
	dma_desc_t _msg_with_int_desc;

#ifdef _DMA_SYNC_FIFO_WITH_MUTEX
	// todo: declare RTOS mutex to synchronize SW access to write_idx_to_enqueue and to the DMA queue
#endif // _DMA_SYNC_FIFO_WITH_MUTEX
} dma_sync_message_fifo_t;


dma_sync_message_fifo_t  ALIGN(g_dma_sync_fifo, 16) PRAGMA_DSECT_NO_LOAD("CSL_INTERNAL_DATA_ALLOCATION");

///////////////////////////////////////////////////////////////////////////////////////////


dma_status_e dma_sync_message_fifo_init( void )
{
	dma_status_e status;
	dma_sync_message_fifo_t* p_fifo = &g_dma_sync_fifo;

	memset( p_fifo, 0, sizeof(dma_sync_message_fifo_t) );

	status = dma_create_internal_message_desc(&p_fifo->_msg_desc);
	if ( DMA_STATUS_OK == status)
	{
		status = dma_create_internal_message_desc(&p_fifo->_msg_with_int_desc);
#ifndef CEVA_DMA_EXTENDED_DESC
		DESC_PTR_CAST(&p_fifo->_msg_with_int_desc)->internal_message.ddie = 1;
#else  //!CEVA_DMA_EXTENDED_DESC
        DESC_PTR_CAST(&p_fifo->_msg_with_int_desc)->ddie = 1;
#endif //!CEVA_DMA_EXTENDED_DESC
    }

	return status;
}


dma_status_e dma_enqueue_desc_with_sync( dma_queue_base_t* queue, dma_desc_t* desc, void* src, void* dst, dma_sync_message_t message )
{
	dma_status_e status;
	uint32_t write_idx;
	dma_sync_message_fifo_t* p_fifo = &g_dma_sync_fifo;

#ifdef _DMA_SYNC_FIFO_WITH_MUTEX
	// todo: acquire mutex
#endif // _DMA_SYNC_FIFO_WITH_MUTEX

	status = dma_enqueue_desc(queue, desc, src, dst);
	if ( DMA_STATUS_OK == status )
	{
		dma_sync_message_t* p_message;
		write_idx = p_fifo->write_idx_to_enqueue;
		p_message = &p_fifo->fifo[write_idx];
		p_fifo->write_idx_to_enqueue = write_idx < (DMA_SYNC_MESSAGE_FIFO_SIZE - 1) ? ++write_idx : 0;

		status = dma_enqueue_desc(queue, &p_fifo->_msg_desc, message, p_message );
		if ( DMA_STATUS_OK == status )
		{
			status = dma_enqueue_desc(queue, &p_fifo->_msg_with_int_desc, (void*)p_fifo->write_idx_to_enqueue, &p_fifo->write_idx);
		}
	}

#ifdef _DMA_SYNC_FIFO_WITH_MUTEX
    // todo: release mutex
#endif // _DMA_SYNC_FIFO_WITH_MUTEX

    return status;
}


uint32_t dma_get_sync_message( dma_sync_message_t* p_message )
{
	dma_sync_message_fifo_t* p_fifo = &g_dma_sync_fifo;
	uint32_t read_idx = p_fifo->read_idx;
	uint32_t write_idx = p_fifo->write_idx;
	uint32_t status = read_idx != write_idx;

	if (status)
	{
		*p_message = p_fifo->fifo[read_idx];
		p_fifo->read_idx = read_idx < (DMA_SYNC_MESSAGE_FIFO_SIZE - 1) ? ++read_idx : 0;
	}
	else
	{
		*p_message = 0;
	}

    return status;
}
