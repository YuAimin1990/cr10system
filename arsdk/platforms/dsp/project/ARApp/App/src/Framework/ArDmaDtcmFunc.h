/*
 * ArDmaCallFunc.h
 *
 *  Created on: 2021Äê4ÔÂ19ÈÕ
 *      Author: bfchen
 */

#ifndef APP_SRC_ARDMADTCM_FUNC_H_
#define APP_SRC_ARDMADTCM_FUNC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "dma_driver.h"
#include "Uart.h"


#define DTCM_LEN_MAX	(0x80000)	//define the DTCM max length, in 9301 xm6 is 512k


typedef enum
{
	eDmaFuncNPU = 0, //For NPU Callback
	eDmaFuncCV,      //For Ceva CV
	eDmaFuncComm     //For General Func
} eDmaFuncId;


typedef enum
{
	eDmaSuccess = 0,
	eDmaFailure = -1
} eDmaStatus;

typedef enum
{
	eDmaDec1D = 0,
	eDmaDec2D,
	eDmaDec3D
} eDmaDecType;

typedef struct
{
	//dma_desc_t* desc;				//pointer to the descriptor to initialize
	uint32_t length;				//number of bytes to copy
	//dma_transfer_dir_e dir;			//direction of the transfer
	dma_transfer_type_e tr_type;	//internal memory access type
} AR_DMA_1D_PARAM_S;

typedef struct
{
	//dma_desc_t* desc;				//pointer to the descriptor to initialize
	uint32_t width;					//number of bytes in a row to copy
	uint32_t height;				//number of rows to copy
	uint32_t src_stride;			//source stride in bytes
	uint32_t dst_stride;			//destination stride in bytes
	//dma_transfer_dir_e dir;			//direction of the transfer
	dma_transfer_type_e tr_type;	//internal memory access type
} AR_DMA_2D_PARAM_S;

typedef struct
{
	//dma_desc_t* desc;				//pointer to the descriptor to initialize
	uint32_t width;					//number of bytes in a row to copy
	uint32_t height;				//number of rows to copy
	uint32_t num_planes;			//number of planes
	uint32_t src_stride;			//source line stride in bytes
	uint32_t dst_stride;			//destination line stride in bytes
	uint32_t src_plane_stride;		// source plane stride in bytes
	uint32_t dst_plane_stride;		//destination plane stride in bytes
	//dma_transfer_dir_e dir;			//direction of the transfer
	dma_transfer_type_e tr_type;	//internal memory access type
} AR_DMA_3D_PARAM_S;

typedef union
{
     AR_DMA_1D_PARAM_S stDma1dParams;
     AR_DMA_2D_PARAM_S stDma2dParams;
     AR_DMA_3D_PARAM_S stDma3dParams;
} AR_DMA_OPERATOR_PARAM_U;

typedef enum
{
	ASYNC = 0,   //this flag is not wait when dma transfers enabled
	SYNC = 1,    //this flag is wait until dma transfers done
} eDspWaitFlag;

typedef struct
{
	dma_desc_t* desc;				//pointer to the descriptor to initialize
	uint32_t width;					//number of bytes in a row to copy
	uint32_t height;				//number of rows to copy
	uint32_t num_planes;			//number of planes
	uint32_t src_stride;			//source line stride in bytes
	uint32_t dst_stride;			//destination line stride in bytes
	uint32_t src_plane_stride;		// source plane stride in bytes
	uint32_t dst_plane_stride;		//destination plane stride in bytes
	uint32_t *sync_point;
	uchar *src_addr;
	uchar *dest_addr;
} AR_DMA_SIZE_PARAM_S;


#define DDMA_PP_PING 0
#define DDMA_PP_PONG 1

typedef struct
{
	dma_transfer_dir_e dir;			//direction of the transfer
	dma_transfer_type_e tr_type;	//internal memory access type
	eDspWaitFlag syncFlag;
	eDmaDecType dmaDecType;
	eDmaFuncId dmaFuncType;
	AR_DMA_SIZE_PARAM_S dmaPPParam[2];  //0: for Ping;  1: for Pong
} AR_DMA_PARAM_S;


int xDmaInit(void);
void vDmaDeInit(void);
char* ArDTCMGetStartAddr(void);
uint32_t ArDTCMGetAvailableLen(void);
int ArDMALoadE2I(eDmaFuncId DmaFuncID, char *InputAddr,char *InputDtcm, eDmaDecType dmaDecType, AR_DMA_OPERATOR_PARAM_U *OperateParam);
int ArDMAStoreI2E(eDmaFuncId DmaFuncID, char *OutputAddr, char *OutputDtcm, eDmaDecType dmaDecType, AR_DMA_OPERATOR_PARAM_U *OperateParam);
int AR_DMA_Transfer(AR_DMA_PARAM_S *dmaParam, int index);
int AR_DMA_Wait(AR_DMA_PARAM_S *dmaParam, int index);

#ifdef __cplusplus
}
#endif
#endif /* APP_SRC_ARDMACALLFUNC_H_ */
