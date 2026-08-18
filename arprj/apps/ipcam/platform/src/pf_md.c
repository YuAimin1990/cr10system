#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "ar_ivs.h"
#include "pf_type.h"
#include "cfg_md.h"
#include "pf_md.h"

static AR_S32 g_md_init = 0;

AR_S32 IPC_PF_MD_SetMd(AR_BOOL bMdEn)
{
	AR_S32 s32Ret = 0;
	IPC_CFG_MD_S * pstMdCfg = IPC_CFG_MD_GetParam();

	pstMdCfg->bEnable= bMdEn;

	s32Ret = IPC_CFG_MD_Save();

	return s32Ret;
}


void IPC_PF_MD_UnInit(void)
{
	if (!g_md_init) {
		return;
	}

	printf("IPC_PF_MD_UnInit()<--\r\n");

	AR_IVS_MD_Exit();

	g_md_init = 0;

	printf("IPC_PF_MD_UnInit()-->\r\n");
    return;
}


AR_S32 IPC_PF_MD_Init()
{
	if (g_md_init)
    {
		return -1;
	}

	printf("IPC_PF_MD_Init()<--\r\n");

	AR_IVS_MD_Init();

	g_md_init = 1;

	printf("IPC_PF_MD_Init()-->\r\n");

    return 0;
}

void IPC_PF_MD_Suspend()
{
	return;
}

void IPC_PF_MD_Resume()
{
	return;
}
