/*
 * ArArmCvRotate.c
 *
 *  Created on: 2023Äê3ÔÂ1ÈÕ
 *      Author: syli
 */

#include "Uart.h"
#include "mss.h"
#include "ArArmRmtCallDeclare.h"
#include "ArDmaDtcmFunc.h"
#include "dma_int.h"


//===========================General struct define ==========================//
typedef struct
{
	unsigned int y;
	int y_width;
	int y_height;
	int y_stride;

	unsigned int u;
	int u_width;
	int u_height;
	int u_stride;

	unsigned int v;
	int v_width;
	int v_height;
	int v_stride;
} IMG_MULTI_PLANAR_st;

typedef struct
{
	IMG_MULTI_PLANAR_st src;
	IMG_MULTI_PLANAR_st dst;
	int pingpong_flag;    //0: non-pingpong  1:pingpong
} SPECIFIC_ROTATION_MSG_PARAM_st;


//===========================DDMA define ==========================//

#define TILE_SIZE		128
#define TILE_STRIDE		(TILE_SIZE + 4)

#define MAX_SRC_DIM0 	3840
#define MAX_SRC_DIM1 	2160

#define MAX_NUM_TILE_DIM0	((MAX_SRC_DIM0 + TILE_SIZE-1)/TILE_SIZE)
#define MAX_NUM_TILE_DIM1	((MAX_SRC_DIM1 + TILE_SIZE-1)/TILE_SIZE)

dma_desc_t inp_desc_e2i_ping PRAGMA_DSECT_NO_LOAD("int_sect");
dma_desc_t inp_desc_e2i_pong PRAGMA_DSECT_NO_LOAD("int_sect");
dma_desc_t out_desc_i2e_ping PRAGMA_DSECT_NO_LOAD("int_sect");
dma_desc_t out_desc_i2e_pong PRAGMA_DSECT_NO_LOAD("int_sect");

//===========================rotate func define ==========================//

typedef struct
{
	short x;
	short y;
	short width;
	short height;
}TileDesc;

TileDesc tileQueue[MAX_NUM_TILE_DIM0 * MAX_NUM_TILE_DIM1] PRAGMA_DSECT_NO_LOAD("int_sect");
short idx_seq[16]  PRAGMA_DSECT_NO_LOAD("int_sect") = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

//for phase 0
uchar ALIGN(inp_tile_buf0[TILE_SIZE * TILE_STRIDE], 64)	PRAGMA_DSECT_NO_LOAD("int_sect");
uchar ALIGN(out_tile_buf0[TILE_SIZE * TILE_SIZE],64)  PRAGMA_DSECT_NO_LOAD("int_sect");

//for phase 1
uchar ALIGN(inp_tile_buf1[TILE_SIZE * TILE_STRIDE], 64)	PRAGMA_DSECT_NO_LOAD("int_sect");
uchar ALIGN(out_tile_buf1[TILE_SIZE * TILE_SIZE],64)  PRAGMA_DSECT_NO_LOAD("int_sect");


static void rotate90_kernel(uchar* src_ptr, short tile_width, short tile_height, short src_tile_stride, short dst_tile_stride, uchar* dst_ptr)
{
	int i, j;
	short16 idx = *(short16*)idx_seq;
	idx = (short16)vmpy(idx, (short)src_tile_stride);

	for(j = 0; j < tile_width; j++)
	{
		for(i = 0; i < tile_height; i+=16)
		{
			uchar16 val = vpld(src_ptr + i*src_tile_stride + j, idx);
			vst(val, dst_ptr + (tile_width-1-j)*dst_tile_stride + i, 0xFFFF);
		}
	}
}

int rotate90_anticlockwise(uchar* ext_src, ushort width, ushort height, ushort src_stride, uchar* ext_dst, ushort dst_stride)
{
	short tile_size = TILE_SIZE;
	short tile_stride = TILE_STRIDE;

	short num_tile_x = (width + tile_size-1)/tile_size;
	short num_tile_y = (height + tile_size-1)/tile_size;

	short residual_width  = width%tile_size;
	short residual_height = height%tile_size;

	short tail_width  = (residual_width == 0)?tile_size:residual_width;
	short tail_height = (residual_height == 0)?tile_size:residual_height;

	// pre-calc tile coord for ddma
	int size = 0;
	for (short tile_y = 0; tile_y < num_tile_y; tile_y++)
	{
		short tileHeight = (tile_y == num_tile_y - 1)?tail_height:tile_size;
		for (short tile_x = 0; tile_x < num_tile_x; tile_x++)
		{
			short tileWidth = (tile_x == num_tile_x - 1)?tail_width:tile_size;
			TileDesc tile;
			tile.x = tile_x*tile_size;
			tile.y = tile_y*tile_size;
			tile.width = tileWidth;
			tile.height = tileHeight;	
			tileQueue[size] = tile;
			size++;
		}
	}

	uint32_t  sync_point[2] = {0, 0};
	uint32_t  last_sync_point;

	AR_DMA_PARAM_S stDmaStParam = {0};
	stDmaStParam.dmaFuncType = eDmaFuncCV;
	stDmaStParam.dmaDecType = eDmaDec2D;
	stDmaStParam.syncFlag = 0;
	stDmaStParam.tr_type = DMA_TYPE_LINEAR;
	stDmaStParam.dir = DMA_DIR_INTERNAL_EXTERNAL;
	stDmaStParam.dmaPPParam[DDMA_PP_PING].sync_point = &last_sync_point;
	stDmaStParam.dmaPPParam[DDMA_PP_PONG].sync_point = &last_sync_point;
	
	AR_DMA_PARAM_S stDmaLdParam = {0};
	stDmaLdParam.dmaFuncType = eDmaFuncCV;
	stDmaLdParam.dmaDecType = eDmaDec2D;
	stDmaLdParam.syncFlag = 0;
	stDmaLdParam.tr_type = DMA_TYPE_LINEAR;
	stDmaLdParam.dir = DMA_DIR_EXTERNAL_INTERNAL;

	//Enqeue Ping for current tile
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].desc = &inp_desc_e2i_ping;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_stride = src_stride;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].dst_stride = TILE_STRIDE;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[0].width;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[0].height;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_addr = ext_src;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].dest_addr = inp_tile_buf0;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].sync_point = &sync_point[DDMA_PP_PING];
	AR_DMA_Transfer(&stDmaLdParam, DDMA_PP_PING);

	int i = 0;
	int x_pos;
	int y_pos;
	uchar *ext_src_ptr;
	uchar *ext_dst_ptr;
	int sizeEven = (size & 0x1)?(size - 1):size;

	for(i = 0; i < sizeEven; i+=2)
	{
		//========================phase 0=========================
		//Enqeue Pong for next tile
		x_pos = tileQueue[i+1].x;
		y_pos = tileQueue[i+1].y;
		ext_src_ptr = ext_src + y_pos * src_stride + x_pos;

		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].desc = &inp_desc_e2i_pong;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].src_stride = src_stride;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].dst_stride = TILE_STRIDE;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].width = tileQueue[i+1].width;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].height = tileQueue[i+1].height;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].src_addr = ext_src_ptr;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].dest_addr = inp_tile_buf1;
		stDmaLdParam.dmaPPParam[DDMA_PP_PONG].sync_point = &sync_point[DDMA_PP_PONG];
		AR_DMA_Transfer(&stDmaLdParam, DDMA_PP_PONG);

		//Wait Ping done
		AR_DMA_Wait(&stDmaLdParam, DDMA_PP_PING);
		
		//Work for Ping 
		rotate90_kernel(inp_tile_buf0, tileQueue[i].width, tileQueue[i].height, tile_stride, tile_size, out_tile_buf0);

		//Load Ping to DDR
		y_pos = width - (tileQueue[i].x + tileQueue[i].width);
		x_pos = tileQueue[i].y;
		ext_dst_ptr = ext_dst + y_pos * dst_stride + x_pos;

		stDmaStParam.dmaPPParam[DDMA_PP_PING].desc = &out_desc_i2e_ping;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].src_stride = TILE_SIZE;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].dst_stride = dst_stride;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[i].height;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[i].width;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].src_addr = out_tile_buf0;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].dest_addr = ext_dst_ptr;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].sync_point = &last_sync_point;
		AR_DMA_Transfer(&stDmaStParam, DDMA_PP_PING);

		//========================phase 1=========================
		//Enqeue Ping for next-next tile
		if(i + 1 < size-1)
		{
			x_pos = tileQueue[i+2].x;
			y_pos = tileQueue[i+2].y;
			ext_src_ptr = ext_src + y_pos * src_stride + x_pos;

			stDmaLdParam.dmaPPParam[DDMA_PP_PING].desc = &inp_desc_e2i_ping;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_stride = src_stride;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].dst_stride = TILE_STRIDE;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[i+2].width;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[i+2].height;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_addr = ext_src_ptr;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].dest_addr = inp_tile_buf0;
			stDmaLdParam.dmaPPParam[DDMA_PP_PING].sync_point = &sync_point[DDMA_PP_PING];
			AR_DMA_Transfer(&stDmaLdParam, DDMA_PP_PING);
		}

		//Wait Pong Dma finish
		AR_DMA_Wait(&stDmaLdParam, DDMA_PP_PONG);

		//Work for Pong 
		rotate90_kernel(inp_tile_buf1, tileQueue[i+1].width, tileQueue[i+1].height, tile_stride, tile_size, out_tile_buf1);
	
		//Load Pong to DDR
		y_pos = width - (tileQueue[i+1].x + tileQueue[i+1].width);
		x_pos = tileQueue[i+1].y;
		ext_dst_ptr = ext_dst + y_pos * dst_stride + x_pos;

		stDmaStParam.dmaPPParam[DDMA_PP_PONG].desc = &out_desc_i2e_pong;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].src_stride = TILE_SIZE;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].dst_stride = dst_stride;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].width = tileQueue[i+1].height;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].height = tileQueue[i+1].width;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].src_addr = out_tile_buf1;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].dest_addr = ext_dst_ptr;
		stDmaStParam.dmaPPParam[DDMA_PP_PONG].sync_point = &last_sync_point;
		AR_DMA_Transfer(&stDmaStParam, DDMA_PP_PONG);
	}

	//tail process
	if(size & 0x1)
	{
		//wait dma sync
		AR_DMA_Wait(&stDmaLdParam, DDMA_PP_PING);

		rotate90_kernel(inp_tile_buf0, tileQueue[i].width, tileQueue[i].height, tile_stride, tile_size, out_tile_buf0);

		y_pos = width - (tileQueue[i].x + tileQueue[i].width);
		x_pos = tileQueue[i].y;
		ext_dst_ptr = ext_dst + y_pos * dst_stride + x_pos;

		stDmaStParam.dmaPPParam[DDMA_PP_PING].desc = &out_desc_i2e_ping;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].src_stride = TILE_SIZE;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].dst_stride = dst_stride;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[i].height;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[i].width;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].src_addr = out_tile_buf0;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].dest_addr = ext_dst_ptr;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].sync_point = &last_sync_point;
		AR_DMA_Transfer(&stDmaStParam, DDMA_PP_PING);
	}

	AR_DMA_Wait(&stDmaStParam, DDMA_PP_PONG);
	return 0;
}



int rotate90_anticlockwise_no_pingpong(uchar* ext_src, ushort width, ushort height, ushort src_stride, uchar* ext_dst, ushort dst_stride)
{
	short tile_size = TILE_SIZE;
	short tile_stride = TILE_STRIDE;

	short num_tile_x = (width + tile_size-1)/tile_size;
	short num_tile_y = (height + tile_size-1)/tile_size;

	short residual_width  = width%tile_size;
	short residual_height = height%tile_size;

	short tail_width  = (residual_width == 0)?tile_size:residual_width;
	short tail_height = (residual_height == 0)?tile_size:residual_height;

	int size = 0;
	for (short tile_y = 0; tile_y < num_tile_y; tile_y++)
	{
		short tileHeight = (tile_y == num_tile_y - 1)?tail_height:tile_size;

		for (short tile_x = 0; tile_x < num_tile_x; tile_x++)
		{
			short tileWidth = (tile_x == num_tile_x - 1)?tail_width:tile_size;
			TileDesc tile;
			tile.x = tile_x*tile_size;
			tile.y = tile_y*tile_size;
			tile.width = tileWidth;
			tile.height = tileHeight;
			tileQueue[size] = tile;
			size++;
		}
	}

	AR_DMA_PARAM_S stDmaLdParam = {0};
	stDmaLdParam.dmaFuncType = eDmaFuncCV;
	stDmaLdParam.dmaDecType = eDmaDec2D;
	stDmaLdParam.syncFlag = 1;
	stDmaLdParam.tr_type = DMA_TYPE_LINEAR;
	stDmaLdParam.dir = DMA_DIR_EXTERNAL_INTERNAL;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].desc = &inp_desc_e2i_ping;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_stride = src_stride;
	stDmaLdParam.dmaPPParam[DDMA_PP_PING].dst_stride = TILE_STRIDE;
	
	AR_DMA_PARAM_S stDmaStParam = {0};
	stDmaStParam.dmaFuncType = eDmaFuncCV;
	stDmaStParam.dmaDecType = eDmaDec2D;
	stDmaStParam.syncFlag = 1;
	stDmaStParam.tr_type = DMA_TYPE_LINEAR;
	stDmaStParam.dir = DMA_DIR_INTERNAL_EXTERNAL;
	stDmaStParam.dmaPPParam[DDMA_PP_PING].desc = &out_desc_i2e_ping;
	stDmaStParam.dmaPPParam[DDMA_PP_PING].src_stride = TILE_SIZE;
	stDmaStParam.dmaPPParam[DDMA_PP_PING].dst_stride = dst_stride;

	int i = 0;
	int ret = 0;
	uint32_t  sync_point;
	uchar *ext_src_ptr;
	uchar *ext_dst_ptr;
	int x_pos;
	int y_pos;

	for(i = 0; i < size; i++)
	{
		x_pos = tileQueue[i].x;
		y_pos = tileQueue[i].y;
		ext_src_ptr = ext_src + y_pos * src_stride + x_pos;

		stDmaLdParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[i].width;
		stDmaLdParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[i].height;
		stDmaLdParam.dmaPPParam[DDMA_PP_PING].src_addr = ext_src_ptr;
		stDmaLdParam.dmaPPParam[DDMA_PP_PING].dest_addr = inp_tile_buf0;
		stDmaLdParam.dmaPPParam[DDMA_PP_PING].sync_point = &sync_point;

		ret = AR_DMA_Transfer(&stDmaLdParam, DDMA_PP_PING);
		if(ret < 0)
		{
			return ret;
		}

		rotate90_kernel(inp_tile_buf0, tileQueue[i].width, tileQueue[i].height, tile_stride, tile_size, out_tile_buf0);

		y_pos = width - (tileQueue[i].x + tileQueue[i].width);
		x_pos = tileQueue[i].y;
		ext_dst_ptr = ext_dst + y_pos * dst_stride + x_pos;

		stDmaStParam.dmaPPParam[DDMA_PP_PING].width = tileQueue[i].height;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].height = tileQueue[i].width;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].src_addr = out_tile_buf0;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].dest_addr = ext_dst_ptr;
		stDmaStParam.dmaPPParam[DDMA_PP_PING].sync_point = &sync_point;

		ret = AR_DMA_Transfer(&stDmaStParam, DDMA_PP_PING);
		if(ret < 0)
		{
			return ret;
		}
	}
	
	return 0;
}


int cvYuvRot90(SPECIFIC_ROTATION_MSG_PARAM_st* param)
{
    int ret =0;
	if(param->pingpong_flag ==1)
	{
		ret = rotate90_anticlockwise((uchar*)param->src.y, (ushort)param->src.y_width, (ushort)param->src.y_height, (ushort)param->src.y_stride, (uchar*)param->dst.y, (ushort)param->dst.y_stride);
		if(ret)
		{
			return ret;
		}
		ret = rotate90_anticlockwise((uchar*)param->src.u, (ushort)param->src.u_width, (ushort)param->src.u_height, (ushort)param->src.u_stride, (uchar*)param->dst.u, (ushort)param->dst.u_stride);
		if(ret)
		{
			return ret;
		}
		ret = rotate90_anticlockwise((uchar*)param->src.v, (ushort)param->src.v_width, (ushort)param->src.v_height, (ushort)param->src.v_stride, (uchar*)param->dst.v, (ushort)param->dst.v_stride);
		if(ret)
		{
			return ret;
		}
	}
	else
	{
		ret = rotate90_anticlockwise_no_pingpong((uchar*)param->src.y, (ushort)param->src.y_width, (ushort)param->src.y_height, (ushort)param->src.y_stride, (uchar*)param->dst.y, (ushort)param->dst.y_stride);
		if(ret)
		{
			return ret;
		}
		ret = rotate90_anticlockwise_no_pingpong((uchar*)param->src.u, (ushort)param->src.u_width, (ushort)param->src.u_height, (ushort)param->src.u_stride,(uchar*) param->dst.u, (ushort)param->dst.u_stride);
		if(ret)
		{
			return ret;
		}
		ret = rotate90_anticlockwise_no_pingpong((uchar*)param->src.v, (ushort)param->src.v_width, (ushort)param->src.v_height, (ushort)param->src.v_stride, (uchar*)param->dst.v, (ushort)param->dst.v_stride);
		if(ret)
		{
			return ret;
		}
	}	
	
	return ret;
}


AR_S32 ArArmCvRotate(void* xParams)
{
    
	SPECIFIC_ROTATION_MSG_PARAM_st *ext_specific_rotation_param = (SPECIFIC_ROTATION_MSG_PARAM_st*)xParams;
	SPECIFIC_ROTATION_MSG_PARAM_st param;
	memcpy(&param, ext_specific_rotation_param, sizeof(SPECIFIC_ROTATION_MSG_PARAM_st));
	#if 0
	xDebugPrint("param.src.y = %x \r\n",param.src.y);
	xDebugPrint("param.src.y_width = %d \r\n",param.src.y_width);
	xDebugPrint("param.src.y_height = %d \r\n",param.src.y_height);
	xDebugPrint("param.src.y_stride = %d \r\n",param.src.y_stride);
	xDebugPrint("param.src.u = %x \r\n",param.src.u);
	xDebugPrint("param.src.u_width = %d \r\n",param.src.u_width);
	xDebugPrint("param.src.u_height = %d \r\n",param.src.u_height);
	xDebugPrint("param.src.u_stride = %d \r\n",param.src.u_stride);
	xDebugPrint("param.src.v = %x \r\n",param.src.v);
	xDebugPrint("param.src.v_width = %d \r\n",param.src.v_width);
	xDebugPrint("param.src.v_height = %d \r\n",param.src.v_height);
	xDebugPrint("param.src.v_stride = %d \r\n",param.src.v_stride);
	
	xDebugPrint("param.dst.y = %x \r\n",param.dst.y);
	xDebugPrint("param.dst.y_width = %d \r\n",param.dst.y_width);
	xDebugPrint("param.dst.y_height = %d \r\n",param.dst.y_height);
	xDebugPrint("param.dst.y_stride = %d \r\n",param.dst.y_stride);
	xDebugPrint("param.dst.u = %x \r\n",param.src.u);
	xDebugPrint("param.dst.u_width = %d \r\n",param.dst.u_width);
	xDebugPrint("param.dst.u_height = %d \r\n",param.dst.u_height);
	xDebugPrint("param.dst.u_stride = %d \r\n",param.dst.u_stride);
	xDebugPrint("param.dst.v = %x \r\n",param.src.v);
	xDebugPrint("param.dst.v_width = %d \r\n",param.dst.v_width);
	xDebugPrint("param.dst.v_height = %d \r\n",param.dst.v_height);
	xDebugPrint("param.dst.v_stride = %d \r\n",param.dst.v_stride);
	#endif	
	volatile unsigned long long  start = mss_get_clock();
	
	int ret = cvYuvRot90(&param);
	if(ret < 0) 
	{
		xDebugPrint("call yuv rot 90 failed! \r\n");
	}

	volatile unsigned long long end = mss_get_clock();
	long long elapsed = (end - start);

	xDebugPrint("yuv rot 90 cycles = %d \n", elapsed);

	return 0;
}


AR_ARM_REMOTECALL_DECLARE(ArArmCvRotate) = {
    .pName = "ArArmCvRotate",
    .pRmtCallHandler = ArArmCvRotate,

};

