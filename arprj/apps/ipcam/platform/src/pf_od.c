#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "pf_type.h"
#include "cfg_od.h"
#include "pf_od.h"
#include "mpi_vgs.h"
#include "ar_ivs.h"

static AR_S32 g_od_init = 0;

AR_S32 IPC_PF_OD_SetOd(AR_BOOL bOdEn)
{
	AR_S32 s32Ret = 0;
	IPC_CFG_OD_S * pstOdCfg = IPC_CFG_OD_GetParam();

	pstOdCfg->bEnable= bOdEn;

	s32Ret = IPC_CFG_OD_Save();

	return s32Ret;
}


void IPC_PF_OD_UnInit()
{
	if (!g_od_init) {
		return;
	}

	printf("IPC_PF_OD_UnInit()<--\r\n");

	AR_IVE_OD_Exit();

	g_od_init = 0;

	printf("IPC_PF_OD_UnInit()-->\r\n");
    return;
}


AR_S32 IPC_PF_OD_Init()
{
	if (g_od_init)
    {
		return -1;
	}

	printf("IPC_PF_OD_Init()<--\r\n");

	AR_IVE_OD_Init();

	g_od_init = 1;

	printf("IPC_PF_OD_Init()-->\r\n");

    return 0;
}

void IPC_PF_OD_Suspend()
{
	printf("AR_MPI_VGS_Suspend \n");
}

void IPC_PF_OD_Resume()
{
	printf("AR_MPI_VGS_Resume \n");
}

