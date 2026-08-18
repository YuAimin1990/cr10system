
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ArDmaDtcmFunc.h"

/*** NOTICE:
 * *** dtcm allocated for cevacv & npu callback ***
 * 1. for RTOS multi-task switch, CV and NPU tasks must use their reserved DTCM separately.
 * 2. if you only need one task, you can delete the following reserved dTCM and use all the available mem.
 */
//uint8_t	InternalMemCV[64 * 1024]			PRAGMA_DSECT_NO_LOAD("int_sect");//256K
//uint8_t	InternalMemNPU[64 * 1024]			PRAGMA_DSECT_NO_LOAD("int_sect");//64k

// ddma allocate for cevacv & npu callback
dma_queue_base_t ALIGN(DMAQueueBaseNPU,   32) 	 PRAGMA_DSECT_NO_LOAD("DMA_QUEUR_BASE_MEMORY_ALLOCATION");
dma_queue_base_t ALIGN(DMAQueueBaseCV,    32) 	 PRAGMA_DSECT_NO_LOAD("DMA_QUEUR_BASE_MEMORY_ALLOCATION");
dma_queue_base_t ALIGN(DMAQueueBaseComm,  32) 	 PRAGMA_DSECT_NO_LOAD("DMA_QUEUR_BASE_MEMORY_ALLOCATION");

dma_desc_t ALIGN(DMAQueueNPU[64],   32) 		 PRAGMA_DSECT_NO_LOAD("DMA_QUEUE_MEMORY_ALLOCATION");
dma_desc_t ALIGN(DMAQueueCV[256],   32) 		 PRAGMA_DSECT_NO_LOAD("DMA_QUEUE_MEMORY_ALLOCATION");
dma_desc_t ALIGN(DMAQueueComm[64],  32) 		 PRAGMA_DSECT_NO_LOAD("DMA_QUEUE_MEMORY_ALLOCATION");

static char* dtcm_start_addr = NULL;

char* ArDTCMGetStartAddr(void)
{
	extern char __stack_end;
	dtcm_start_addr = (char *)&__stack_end;
	return dtcm_start_addr;
}

uint32_t ArDTCMGetAvailableLen(void)
{
	extern char __stack_end;

	return (uint32_t)(DTCM_LEN_MAX - (uint32_t)&__stack_end);
}

int xDmaInit(void)
{
	int status = DMA_STATUS_OK;
	dma_init_manager();

	if (&DMAQueueBaseNPU == NULL || DMAQueueNPU == NULL)
	{
		xDebugPrint("DMAQueueBaseNPU || DMAQueueNPU is null!\r\n");
		return -1;
	}
	if (&DMAQueueBaseCV == NULL || DMAQueueCV == NULL)
	{
		xDebugPrint("DMAQueueBaseCV || DMAQueueCV is null!\r\n");
		return -1;
	}
	if (&DMAQueueBaseComm == NULL || DMAQueueComm == NULL)
	{
		xDebugPrint("DMAQueueBaseComm || DMAQueueComm is null!\r\n");
		return -1;
	}

	status = dma_allocate_queue((dma_queue_base_t	*)&DMAQueueBaseNPU, (uint32_t *)DMAQueueNPU, 4, sizeof(DMAQueueNPU) / sizeof(dma_desc_t));
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_allocate_queue NPU failed!\r\n");
		return -1;
	}
	status = dma_reset_sync_point((dma_queue_base_t	*)&DMAQueueBaseNPU);
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_reset_sync_point NPU failed!\r\n");
		return -1;
	}

	status = dma_allocate_queue((dma_queue_base_t	*)&DMAQueueBaseCV, (uint32_t *)DMAQueueCV, 4, sizeof(DMAQueueCV) / sizeof(dma_desc_t));
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_allocate_queue CV failed!\r\n");
		return -1;
	}
	status = dma_reset_sync_point((dma_queue_base_t	*)&DMAQueueBaseCV);
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_reset_sync_point CV failed!\r\n");
		return -1;
	}

	status = dma_allocate_queue((dma_queue_base_t	*)&DMAQueueBaseComm, (uint32_t *)DMAQueueComm, 4, sizeof(DMAQueueComm) / sizeof(dma_desc_t));
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_allocate_queue Comm failed!\r\n");
		return -1;
	}
	status = dma_reset_sync_point((dma_queue_base_t	*)&DMAQueueBaseComm);
	if (DMA_STATUS_OK != status)
	{
		xDebugPrint("dma_reset_sync_point Comm failed!\r\n");
		return -1;
	}

	return status;
}

void vDmaDeInit(void)
{
	if (dma_deinit_manager() != DMA_STATUS_OK)
	{
		xDebugPrint("DMA manager de-initialization failed.\n");
	}

	if (dma_delete_queue(&DMAQueueBaseNPU) != DMA_STATUS_OK)
	{
		xDebugPrint("DMA delete queue NPU failed.\n");
	}

	if (dma_delete_queue(&DMAQueueBaseCV) != DMA_STATUS_OK)
	{
		xDebugPrint("DMA delete queue CV failed.\n");
	}

	if (dma_delete_queue(&DMAQueueBaseComm) != DMA_STATUS_OK)
	{
		xDebugPrint("DMA delete queue Comm failed.\n");
	}
		
	return;
}

/*	FuncID    	 : NPU/CV/Common
/*	dmaDecTyp 	 : eDmaDec1D/eDmaDec2D/eDmaDec3D
 *	InputAddr 	 : external address of data input to dtcm
 *	OperateParam : 1D/2D/3D param
 */
int ArDMALoadE2I(eDmaFuncId DmaFuncID, char *InputAddr,char *InputDtcm, eDmaDecType dmaDecType, AR_DMA_OPERATOR_PARAM_U *OperateParam)
{
	dma_desc_t xArDmaLoadDec;
	unsigned int sync_load_dtcm = 0;

	if(dmaDecType == eDmaDec1D)
	{
		AR_DMA_1D_PARAM_S xDma1Dparam = OperateParam->stDma1dParams;

		if (dma_create_1d_desc(&xArDmaLoadDec, xDma1Dparam.length, DMA_DIR_EXTERNAL_INTERNAL, xDma1Dparam.tr_type))
		{
			xDebugPrint("dma_create_1d_desc failed!\r\n");
			return -1;
		}
	}
	else if(dmaDecType == eDmaDec2D)
	{
		AR_DMA_2D_PARAM_S xDma2Dparam = OperateParam->stDma2dParams;

		if (dma_create_2d_desc(&xArDmaLoadDec, xDma2Dparam.width, xDma2Dparam.height, xDma2Dparam.src_stride, xDma2Dparam.dst_stride,
				DMA_DIR_EXTERNAL_INTERNAL, xDma2Dparam.tr_type))
		{
			xDebugPrint("dma_create_2d_desc failed!\r\n");
			return -1;
		}
	}
	else
	{
		AR_DMA_3D_PARAM_S xDma3Dparam = OperateParam->stDma3dParams;
		if(dma_create_3d_desc(&xArDmaLoadDec, xDma3Dparam.width, xDma3Dparam.height, xDma3Dparam.num_planes, xDma3Dparam.src_stride,
				xDma3Dparam.dst_stride, xDma3Dparam.src_plane_stride, xDma3Dparam.dst_plane_stride, DMA_DIR_EXTERNAL_INTERNAL, xDma3Dparam.tr_type))
		{
			xDebugPrint("dma_create_3d_desc failed!\r\n");
			return -1;
		}

	}

	if(DmaFuncID==eDmaFuncNPU)
	{
		if (dma_enqueue_desc(&DMAQueueBaseNPU, &xArDmaLoadDec, InputAddr, InputDtcm))
		{
			xDebugPrint("dma_enqueue_desc NPU failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseNPU, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point NPU failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseNPU, sync_load_dtcm);
	}
	else if(DmaFuncID==eDmaFuncCV)
	{
		if (dma_enqueue_desc(&DMAQueueBaseCV, &xArDmaLoadDec, InputAddr, InputDtcm))
		{
			xDebugPrint("dma_enqueue_desc CV failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseCV, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point CV failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseCV, sync_load_dtcm);
	}
	else
	{
		if (dma_enqueue_desc(&DMAQueueBaseComm, &xArDmaLoadDec, InputAddr, InputDtcm))
		{
			xDebugPrint("dma_enqueue_desc Comm failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseComm, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point Comm failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseComm, sync_load_dtcm);
	}
	
}

int ArDMAStoreI2E(eDmaFuncId DmaFuncID, char *OutputAddr, char *OutputDtcm, eDmaDecType dmaDecType, AR_DMA_OPERATOR_PARAM_U *OperateParam)
{
	dma_desc_t xArDmaLoadDec;
	unsigned int sync_load_dtcm = 0;

	if(dmaDecType == eDmaDec1D)
	{
		AR_DMA_1D_PARAM_S xDma1Dparam = OperateParam->stDma1dParams;

		if (dma_create_1d_desc(&xArDmaLoadDec, xDma1Dparam.length, DMA_DIR_INTERNAL_EXTERNAL, xDma1Dparam.tr_type))
		{
			xDebugPrint("dma_create_1d_desc failed!\r\n");
			return -1;
		}
	}
	else if(dmaDecType == eDmaDec2D)
	{
		AR_DMA_2D_PARAM_S xDma2Dparam = OperateParam->stDma2dParams;

		if (dma_create_2d_desc(&xArDmaLoadDec, xDma2Dparam.width, xDma2Dparam.height, xDma2Dparam.src_stride, xDma2Dparam.dst_stride,
				DMA_DIR_INTERNAL_EXTERNAL, xDma2Dparam.tr_type))
		{
			xDebugPrint("dma_create_2d_desc failed!\r\n");
			return -1;
		}
	}
	else
	{
		AR_DMA_3D_PARAM_S xDma3Dparam = OperateParam->stDma3dParams;
		if(dma_create_3d_desc(&xArDmaLoadDec, xDma3Dparam.width, xDma3Dparam.height, xDma3Dparam.num_planes, xDma3Dparam.src_stride,
				xDma3Dparam.dst_stride, xDma3Dparam.src_plane_stride, xDma3Dparam.dst_plane_stride, DMA_DIR_INTERNAL_EXTERNAL, xDma3Dparam.tr_type))
		{
			xDebugPrint("dma_create_3d_desc failed!\r\n");
			return -1;
		}

	}

	if(DmaFuncID==eDmaFuncNPU)
	{
		if (dma_enqueue_desc(&DMAQueueBaseNPU, &xArDmaLoadDec, OutputDtcm, OutputAddr))
		{
			xDebugPrint("dma_enqueue_desc NPU failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseNPU, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point NPU failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseNPU, sync_load_dtcm);
	}
	else if(DmaFuncID==eDmaFuncCV)
	{
		if (dma_enqueue_desc(&DMAQueueBaseCV, &xArDmaLoadDec, OutputDtcm, OutputAddr))
		{
			xDebugPrint("dma_enqueue_desc CV failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseCV, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point CV failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseCV, sync_load_dtcm);
	}
	else
	{
		if (dma_enqueue_desc(&DMAQueueBaseComm, &xArDmaLoadDec, OutputDtcm, OutputAddr))
		{
			xDebugPrint("dma_enqueue_desc Comm failed!\r\n");
			return -1;
		}
		if(dma_enqueue_sync_point(&DMAQueueBaseComm, &sync_load_dtcm))
		{
			xDebugPrint("dma_enqueue_sync_point Comm failed!\r\n");
			return -1;
		}
		dma_wait_sync_point(&DMAQueueBaseComm, sync_load_dtcm);
	}
}


/*	dmaDecTyp 	 : eDmaDec1D/eDmaDec2D/eDmaDec3D
 *	InputAddr 	 : external address of data input to dtcm
 *	OperateParam : 1D/2D/3D param
 */

int AR_DMA_Transfer(AR_DMA_PARAM_S *dmaParam, int index)
{
    int32_t ret = 0;

    if(dmaParam->dmaDecType == eDmaDec1D)
    {
    	if (dma_create_1d_desc(dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].width, dmaParam->dir, dmaParam->tr_type))
    	{
    		xDebugPrint("dma_create_1d_desc failed!\r\n");
    		ret = DMA_STATUS_ERROR_PARAMS;
    		return ret;
        }
    }
	else if(dmaParam->dmaDecType == eDmaDec2D)
	{
		if (dma_create_2d_desc(dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].width, dmaParam->dmaPPParam[index].height, dmaParam->dmaPPParam[index].src_stride, dmaParam->dmaPPParam[index].dst_stride,
	    	dmaParam->dir, dmaParam->tr_type))
		{
	        xDebugPrint("dma_create_2d_desc failed!\r\n");
	        ret = DMA_STATUS_ERROR_PARAMS;
	        return ret;
		}
	}
	else
	{
		if(dma_create_3d_desc(dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].width, dmaParam->dmaPPParam[index].height, dmaParam->dmaPPParam[index].num_planes, dmaParam->dmaPPParam[index].src_stride,
	    	dmaParam->dmaPPParam[index].dst_stride, dmaParam->dmaPPParam[index].src_plane_stride, dmaParam->dmaPPParam[index].dst_plane_stride, dmaParam->dir, dmaParam->tr_type))
		{
	        xDebugPrint("dma_create_3d_desc failed!\r\n");
	        ret = DMA_STATUS_ERROR_PARAMS;
	        return ret;
		}
	}

    if(dmaParam->dmaFuncType ==eDmaFuncNPU)
	{
    	if (dma_enqueue_desc(&DMAQueueBaseNPU, dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].src_addr , dmaParam->dmaPPParam[index].dest_addr))
		{
			xDebugPrint("dma_enqueue_desc failed!\r\n");
			ret = DMA_STATUS_ENQUEUE_FAILED;
			return ret;
		}

		if(dma_enqueue_sync_point(&DMAQueueBaseNPU, dmaParam->dmaPPParam[index].sync_point))
		{
			xDebugPrint("dma_enqueue_sync_point failed!\r\n");
			ret = DMA_STATUS_MANAGER_FAILURE;
			return ret;
		}

		if (dmaParam->syncFlag)
		{
			ret = dma_wait_sync_point(&DMAQueueBaseNPU, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
		}
	}
	else if(dmaParam->dmaFuncType ==eDmaFuncCV)
	{
		if (dma_enqueue_desc(&DMAQueueBaseCV, dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].src_addr , dmaParam->dmaPPParam[index].dest_addr))
		{
			xDebugPrint("dma_enqueue_desc failed!\r\n");
			ret = DMA_STATUS_ENQUEUE_FAILED;
			return ret;
		}

		if(dma_enqueue_sync_point(&DMAQueueBaseCV, dmaParam->dmaPPParam[index].sync_point))
		{
			xDebugPrint("dma_enqueue_sync_point failed!\r\n");
			ret = DMA_STATUS_MANAGER_FAILURE;
			return ret;
		}

		if (dmaParam->syncFlag)
		{
			ret = dma_wait_sync_point(&DMAQueueBaseCV, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
		}
	}
	else
	{
		if (dma_enqueue_desc(&DMAQueueBaseComm, dmaParam->dmaPPParam[index].desc, dmaParam->dmaPPParam[index].src_addr , dmaParam->dmaPPParam[index].dest_addr))
		{
			xDebugPrint("dma_enqueue_desc failed!\r\n");
			ret = DMA_STATUS_ENQUEUE_FAILED;
			return ret;
		}

		if(dma_enqueue_sync_point(&DMAQueueBaseComm, dmaParam->dmaPPParam[index].sync_point))
		{
			xDebugPrint("dma_enqueue_sync_point failed!\r\n");
			ret = DMA_STATUS_MANAGER_FAILURE;
			return ret;
		}

		if (dmaParam->syncFlag)
		{
			ret = dma_wait_sync_point(&DMAQueueBaseComm, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
		}
	}
	
    return ret;
}

int AR_DMA_Wait(AR_DMA_PARAM_S *dmaParam, int index)
{
	if(dmaParam->dmaFuncType ==eDmaFuncNPU)
	{
		return dma_wait_sync_point(&DMAQueueBaseNPU, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
	}
	else if(dmaParam->dmaFuncType ==eDmaFuncCV)
	{
		return dma_wait_sync_point(&DMAQueueBaseCV, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
	}
	else
	{
		return dma_wait_sync_point(&DMAQueueBaseComm, *(uint32_t*)dmaParam->dmaPPParam[index].sync_point);
	}
}



