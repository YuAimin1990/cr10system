/*
 * dma_driver.c
 *
 *  Created on: Mar 26, 2015
 *      Author: nirg
 */
#include "cevaxm.h"
#include "dma_driver.h"
#include "dma_int.h"

#ifdef CEVA_DMA_EXTENDED_DESC

#ifndef CEVAXM
	#include "Simulator.h"
    #define DMA_ENQUEUE_DESC(q,d,ps,pd) dma_enqueue_desc_c(q,d,ps,pd)
	//uint32_t dma_counter = 0;
#else  //CEVAXM
	#if defined XM6 && defined _DMA_API_CHECK_
		dma_status_e dma_enqueue_desc_asm(dma_queue_base_t* queue, dma_desc_t* desc, void* src, void* dst);
		#define DMA_ENQUEUE_DESC(q,d,ps,pd) dma_enqueue_desc_asm(q,d,ps,pd)
	#else
		dma_status_e dma_enqueue_desc_c(dma_queue_base_t* queue, dma_desc_t* desc, void* src, void* dst);
		#define DMA_ENQUEUE_DESC(q,d,ps,pd) dma_enqueue_desc_c(q,d,ps,pd)
	#endif // _DMA_API_CHECK_
#endif //CEVAXM

#define DMA_TYPE_MESSAGE 3

typedef enum
{
	E_DMA_NO_TDT,
	E_DMA_TDT
} dma_two_dimension_transfer_e;

///////////////////////////////////////////////////////////////////////////////////////////

dma_status_e dma_create_external_message_desc(dma_desc_t* desc)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	memset(desc, 0, sizeof(dma_desc_t));
	DESC_PTR_CAST(desc)->qman_src_type = DMA_TYPE_MESSAGE;
	DESC_PTR_CAST(desc)->qman_dst_type = DMA_TYPE_MESSAGE;
	DESC_PTR_CAST(desc)->floc = 1;
	DESC_PTR_CAST(desc)->dup=DMA_DIR_INTERNAL_EXTERNAL;
	DESC_PTR_CAST(desc)->u0.message.dma_size = 4;
	return DMA_STATUS_OK;
}

dma_status_e dma_create_internal_message_desc(dma_desc_t* desc)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	memset(desc, 0, sizeof(dma_desc_t));
	DESC_PTR_CAST(desc)->qman_src_type = DMA_TYPE_MESSAGE;
	DESC_PTR_CAST(desc)->qman_dst_type = DMA_TYPE_MESSAGE;
	DESC_PTR_CAST(desc)->floc = 1;
	DESC_PTR_CAST(desc)->dup=DMA_DIR_EXTERNAL_INTERNAL;
	DESC_PTR_CAST(desc)->u0.message.dma_size = 4;
	return DMA_STATUS_OK;
}

dma_status_e dma_create_1d_desc(dma_desc_t* desc, uint32_t length, dma_transfer_dir_e dir, dma_transfer_type_e tr_type)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (tr_type == DMA_TYPE_RESERVED || tr_type > DMA_TYPE_TWO_BANK_WRITE)
		return DMA_STATUS_ERROR_PARAMS;

	memset(desc, 0, sizeof(dma_desc_t));

	DESC_PTR_CAST(desc)->dup = dir;
	switch (dir)
	{
		case DMA_DIR_INTERNAL_INTERNAL:
			break;
		case DMA_DIR_INTERNAL_EXTERNAL:
			if (tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE || tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_TWO_BANK_WRITE)
				return DMA_STATUS_ERROR_PARAMS;
			break;
		case DMA_DIR_EXTERNAL_INTERNAL:
			if (tr_type == DMA_TYPE_ONE_BANK_READ || tr_type == DMA_TYPE_TWO_BANK_READ)
				return DMA_STATUS_ERROR_PARAMS;
			break;
		default:
			return DMA_STATUS_ERROR_PARAMS;
	}
	DESC_PTR_CAST(desc)->floc = 1;
	DESC_PTR_CAST(desc)->u0.single_dimension.trtyp = tr_type;
	DESC_PTR_CAST(desc)->u0.single_dimension.dma_size = length;
	return DMA_STATUS_OK;
}

dma_status_e dma_create_2d_desc(dma_desc_t* desc,
									uint32_t width,
									uint32_t height,
									uint32_t src_stride,
									uint32_t dst_stride,
									dma_transfer_dir_e dir,
									dma_transfer_type_e tr_type)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (tr_type == DMA_TYPE_RESERVED || tr_type > DMA_TYPE_TWO_BANK_WRITE)
		return DMA_STATUS_ERROR_PARAMS;

	memset(desc, 0, sizeof(dma_desc_t));
	DESC_PTR_CAST(desc)->dup = dir;
	switch (dir)
	{
	case DMA_DIR_INTERNAL_INTERNAL:
		break;
	case DMA_DIR_INTERNAL_EXTERNAL:
		if (tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE || tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_TWO_BANK_WRITE)
			return DMA_STATUS_ERROR_PARAMS;
		break;
	case DMA_DIR_EXTERNAL_INTERNAL:
		if (tr_type == DMA_TYPE_ONE_BANK_READ || tr_type == DMA_TYPE_TWO_BANK_READ)
			return DMA_STATUS_ERROR_PARAMS;
		break;
	default:
		return DMA_STATUS_ERROR_PARAMS;
	}

	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = src_stride;
	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = dst_stride;
	if (tr_type == DMA_TYPE_ONE_BANK_READ || tr_type == DMA_TYPE_TWO_BANK_READ)
		DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = 64;
	if (tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE || tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_TWO_BANK_WRITE)
		DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = 64;
	DESC_PTR_CAST(desc)->floc = 1;
	DESC_PTR_CAST(desc)->tdt = 1;
	DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp = tr_type;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_height = height;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_width = width;
	DESC_PTR_CAST(desc)->u0.multi_dimension.num_planes = 1;
	return DMA_STATUS_OK;
}

dma_status_e dma_create_3d_desc(dma_desc_t* desc,
									uint32_t width,
									uint32_t height,
                                    uint32_t num_planes,
									uint32_t src_stride,
									uint32_t dst_stride,
									uint32_t src_plane_stride,
									uint32_t dst_plane_stride,
									dma_transfer_dir_e dir,
									dma_transfer_type_e tr_type)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (tr_type == DMA_TYPE_RESERVED || tr_type > DMA_TYPE_TWO_BANK_WRITE)
		return DMA_STATUS_ERROR_PARAMS;

	memset(desc, 0, sizeof(dma_desc_t));
	DESC_PTR_CAST(desc)->dup = dir;
	switch (dir)
	{
	case DMA_DIR_INTERNAL_INTERNAL:
		break;
	case DMA_DIR_INTERNAL_EXTERNAL:
		if (tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE || tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_TWO_BANK_WRITE)
			return DMA_STATUS_ERROR_PARAMS;
		break;
	case DMA_DIR_EXTERNAL_INTERNAL:
		if (tr_type == DMA_TYPE_ONE_BANK_READ || tr_type == DMA_TYPE_TWO_BANK_READ)
			return DMA_STATUS_ERROR_PARAMS;
		break;
	default:
		return DMA_STATUS_ERROR_PARAMS;
	}

    if (tr_type == DMA_TYPE_ONE_BANK_READ)
    {
        if (0 != (src_plane_stride & 3))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_TWO_BANK_READ)
    {
        if (0 != (src_plane_stride & 7))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE)
    {
        if (0 != (dst_plane_stride & 3))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_TWO_BANK_WRITE ||  tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE)
    {
        if (0 != (dst_plane_stride & 7))
            return DMA_STATUS_ERROR_PARAMS;
    }


	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = src_stride;
	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = dst_stride;
	if (tr_type == DMA_TYPE_ONE_BANK_READ || tr_type == DMA_TYPE_TWO_BANK_READ)
		DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = 64;
	if (tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE || tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_TWO_BANK_WRITE)
		DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = 64;
	DESC_PTR_CAST(desc)->floc = 1;
	DESC_PTR_CAST(desc)->tdt = 1;
	DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp = tr_type;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_height = height;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_width = width;
	DESC_PTR_CAST(desc)->u0.multi_dimension.num_planes = num_planes;
    DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_plane_stride = src_plane_stride;
    DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_plane_stride = dst_plane_stride;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_3d_desc_size(dma_desc_t* desc, uint32_t height, uint32_t width, uint32_t num_planes)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (!DESC_PTR_CAST(desc)->tdt)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_height = height;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_width = width;
    DESC_PTR_CAST(desc)->u0.multi_dimension.num_planes = num_planes;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_3d_desc_stride(dma_desc_t* desc, uint32_t src_stride, uint32_t dst_stride, uint32_t src_plane_stride, uint32_t dst_plane_stride)
{
    dma_transfer_type_e tr_type;
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (!DESC_PTR_CAST(desc)->tdt)
		return DMA_STATUS_ERROR_PARAMS;

    tr_type = (dma_transfer_type_e)(DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp);

    if (tr_type == DMA_TYPE_ONE_BANK_READ)
    {
        if (0 != (src_plane_stride & 3))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_TWO_BANK_READ)
    {
        if (0 != (src_plane_stride & 7))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_ONE_BANK_WRITE || tr_type == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE)
    {
        if (0 != (dst_plane_stride & 3))
            return DMA_STATUS_ERROR_PARAMS;
    }
    else if (tr_type == DMA_TYPE_TWO_BANK_WRITE ||  tr_type == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE)
    {
        if (0 != (dst_plane_stride & 7))
            return DMA_STATUS_ERROR_PARAMS;
    }

	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = src_stride;
	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = dst_stride;
    DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_plane_stride = src_plane_stride;
    DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_plane_stride = dst_plane_stride;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_1d_desc_size(dma_desc_t* desc, uint32_t length)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
	if (DESC_PTR_CAST(desc)->qman_dst_type == 3 || DESC_PTR_CAST(desc)->tdt)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_

	DESC_PTR_CAST(desc)->u0.single_dimension.dma_size = length;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_2d_desc_size(dma_desc_t* desc, uint32_t height, uint32_t width)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (!DESC_PTR_CAST(desc)->tdt)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_height = height;
	DESC_PTR_CAST(desc)->u0.multi_dimension.tile_width = width;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_2d_desc_stride(dma_desc_t* desc, uint32_t src_stride, uint32_t dst_stride)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (!DESC_PTR_CAST(desc)->tdt)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.src_line_stride = src_stride;
	DESC_PTR_CAST(desc)->u0.multi_dimension.stride.absolute.dst_line_stride = dst_stride;
	return DMA_STATUS_OK;
}

#if 0
dma_status_e dma_update_external_message_desc_write_barrier(dma_desc_t* desc, unsigned int write_barrier_en)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (DESC_PTR_CAST(desc)->external_message.qman_ext_type == DMA_TYPE_MESSAGE || DESC_PTR_CAST(desc)->external_message.qman_int_type != DMA_TYPE_MESSAGE || DESC_PTR_CAST(desc)->external_message.tdt == E_DMA_TDT)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->external_message.wd_wait = write_barrier_en;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_1d_desc_write_barrier(dma_desc_t* desc, unsigned int write_barrier_en)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (DESC_PTR_CAST(desc)->dma_1d.qman_ext_type == DMA_TYPE_MESSAGE || DESC_PTR_CAST(desc)->dma_1d.qman_int_type  == DMA_TYPE_MESSAGE || DESC_PTR_CAST(desc)->dma_1d.tdt == E_DMA_TDT)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->dma_1d.wd_wait = write_barrier_en;
	return DMA_STATUS_OK;
}

dma_status_e dma_update_2d_desc_write_barrier(dma_desc_t* desc, uint32_t write_barrier_en)
{
#ifdef _DMA_API_CHECK_
	if (desc == NULL)
		return DMA_STATUS_ERROR_PARAMS;
#endif //_DMA_API_CHECK_
	if (DESC_PTR_CAST(desc)->dma_2d.tdt != 1 || DESC_PTR_CAST(desc)->dma_2d.iit == 1)
		return DMA_STATUS_ERROR_PARAMS;

	DESC_PTR_CAST(desc)->dma_2d.wd_wait = write_barrier_en;
	return DMA_STATUS_OK;
}
#endif // disable write-barrier functions - currently not supported


dma_status_e dma_enqueue_desc_c(dma_queue_base_t* queue, dma_desc_t* desc, void* src, void* dst)
{
#ifndef CEVAXM
	dma_desc_int_t temp = *DESC_PTR_CAST(desc);
	temp.u0.single_dimension.src_ptr = (uint32_t)src;
	temp.u0.single_dimension.dst_ptr = (uint32_t)dst;
	if (enqueue(queue->id, (uint32_t*)&temp) == 0)
		return DMA_STATUS_ENQUEUE_FAILED;
	return DMA_STATUS_OK;
#else // CEVAXM
	uint32_t* w_ptr;
	uint32_t queue_base_io = (queue->id << QMAN_X_OFFSET_SHIFT);
	dma_queue_depth_t depth;
	uint32_t q_size;
	depth.overlay = queue->queue_depth_register;
	q_size = (depth.fields.depth * sizeof(dma_desc_t));
	w_ptr = (uint32_t*)(queue->start_address + queue->write_ptr);
	w_ptr[0] = ((uint32_t*)desc)[0];
	w_ptr[1] = (uint32_t)src;
	w_ptr[2] = (uint32_t)dst;
	w_ptr[3] = ((uint32_t*)desc)[3];
	w_ptr[4] = ((uint32_t*)desc)[4];
	w_ptr[5] = ((uint32_t*)desc)[5];
	w_ptr[6] = ((uint32_t*)desc)[6];
	w_ptr[7] = ((uint32_t*)desc)[7];
	w_ptr = (uint32_t*)((queue->write_ptr + sizeof(dma_desc_t)) & q_size);
	if (w_ptr == (uint32_t*)queue->read_ptr)
		return DMA_STATUS_ENQUEUE_FAILED;
	queue->write_ptr += sizeof(dma_desc_t);
	queue->write_ptr &= q_size;

	__asm__("nop");

	// write MSS internal barrier activation
	__asm__("nop");	__asm__("nop");	__asm__("nop");	__asm__("nop");
	cpm_out(MSS_BARRIER_REGISTER_ADDR, MSS_BARRIER_REGISTER_VALUE);
	__asm__("nop");	__asm__("nop");	__asm__("nop");	__asm__("nop");
	
	cpm_out(queue_base_io + QX_DSC_EN_INC0, QMAN_DESC_INC_VALUE);
	cpm_out(queue_base_io + QX_EN_DEPTH, depth.overlay); // re-enable the queue to overcome HW race conditions
	return DMA_STATUS_OK;
#endif // CEVAXM
}

// Note: this wrapper is used for MSVS simulation, on XM4 (because it needs C version) or when API checks are enabled
#if !defined CEVAXM || defined XM4 || defined _DMA_API_CHECK_
dma_status_e dma_enqueue_desc(dma_queue_base_t* queue, dma_desc_t* desc, void* src, void* dst)
{
#ifdef _DMA_API_CHECK_
    if (NULL == queue || NULL == desc)
		return DMA_STATUS_ERROR_PARAMS;

	if (DESC_PTR_CAST(desc)->dup > DMA_DIR_INTERNAL_INTERNAL)
		return DMA_STATUS_ERROR_PARAMS;

	if (DESC_PTR_CAST(desc)->qman_dst_type > 0 && DESC_PTR_CAST(desc)->qman_dst_type < DMA_TYPE_MESSAGE)
		return DMA_STATUS_ERROR_PARAMS;

	if (DESC_PTR_CAST(desc)->qman_src_type > 0 && DESC_PTR_CAST(desc)->qman_src_type < DMA_TYPE_MESSAGE)
		return DMA_STATUS_ERROR_PARAMS;

	if (DESC_PTR_CAST(desc)->qman_src_type != DESC_PTR_CAST(desc)->qman_dst_type)
		return DMA_STATUS_ERROR_PARAMS;

	if (DESC_PTR_CAST(desc)->tdt)
	{
		if (DESC_PTR_CAST(desc)->qman_dst_type == DMA_TYPE_MESSAGE)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.multi_dimension.num_planes == 0)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.multi_dimension.tile_height == 0 || DESC_PTR_CAST(desc)->u0.multi_dimension.tile_width == 0)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.multi_dimension.src_typ &&
				(DESC_PTR_CAST(desc)->u0.multi_dimension.chn_unpack & ~(DESC_PTR_CAST(desc)->u0.multi_dimension.chn_unpack - 1)) != DESC_PTR_CAST(desc)->u0.multi_dimension.chn_unpack)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_RESERVED)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp != DMA_TYPE_LINEAR)
		{
			if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE
					|| DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_ONE_BANK_WRITE) {
				if ((unsigned int) dst & 0x3)
					return DMA_STATUS_ERROR_PARAMS;
			} else {
				if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_TWO_BANK_WRITE
						|| DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp
								== DMA_TYPE_DUPLICATE_TWO_BANK_WRITE) {
					if ((unsigned int) dst & 0x7)
						return DMA_STATUS_ERROR_PARAMS;
				} else {
					if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_ONE_BANK_READ
							&& ((unsigned int) src & 0x3)) {
						return DMA_STATUS_ERROR_PARAMS;
					} else {
						if (DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp
								== DMA_TYPE_TWO_BANK_READ
								&& ((unsigned int) src & 0x7)) {
							return DMA_STATUS_ERROR_PARAMS;
						}
					}
				}
			}
			if (DESC_PTR_CAST(desc)->dup == DMA_DIR_INTERNAL_EXTERNAL &&
				!(DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_ONE_BANK_READ ||
				DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_TWO_BANK_READ))
				return DMA_STATUS_ERROR_PARAMS;

			if (DESC_PTR_CAST(desc)->dup == DMA_DIR_EXTERNAL_INTERNAL &&
				(DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_ONE_BANK_READ ||
				DESC_PTR_CAST(desc)->u0.multi_dimension.trtyp == DMA_TYPE_TWO_BANK_READ))
				return DMA_STATUS_ERROR_PARAMS;
		}
	}
	else
	{
		if (DESC_PTR_CAST(desc)->qman_dst_type == DMA_TYPE_MESSAGE)
		{
			if (DESC_PTR_CAST(desc)->u0.message.dma_size & 0x3)
				return DMA_STATUS_ERROR_PARAMS;
		}
		if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == 1)
			return DMA_STATUS_ERROR_PARAMS;

		if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp != DMA_TYPE_LINEAR)
		{
			if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_DUPLICATE_ONE_BANK_WRITE ||
				 DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_ONE_BANK_WRITE)
			{
				if ((unsigned int)dst & 0x3)
					return DMA_STATUS_ERROR_PARAMS;
			} else
			if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_TWO_BANK_WRITE ||
				 DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_DUPLICATE_TWO_BANK_WRITE)
			{
				if ((unsigned int)dst & 0x7)
					return DMA_STATUS_ERROR_PARAMS;
			} else
			if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_ONE_BANK_READ && ((unsigned int)src & 0x3))
			{
				return DMA_STATUS_ERROR_PARAMS;
			} else
			if (DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_TWO_BANK_READ && ((unsigned int)src & 0x7))
			{
				return DMA_STATUS_ERROR_PARAMS;
			}

			if (DESC_PTR_CAST(desc)->dup == DMA_DIR_INTERNAL_EXTERNAL &&
				!(DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_ONE_BANK_READ ||
				DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_TWO_BANK_READ))
				return DMA_STATUS_ERROR_PARAMS;

			if (DESC_PTR_CAST(desc)->dup == DMA_DIR_EXTERNAL_INTERNAL &&
				(DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_ONE_BANK_READ ||
				DESC_PTR_CAST(desc)->u0.single_dimension.trtyp == DMA_TYPE_TWO_BANK_READ))
				return DMA_STATUS_ERROR_PARAMS;
		}
	}
#endif //_DMA_API_CHECK_

	return DMA_ENQUEUE_DESC(queue, desc, src, dst);
}
#endif //#if !defined CEVAXM || defined XM4 || defined _DMA_API_CHECK_



#endif // CEVA_DMA_EXTENDED_DESC
