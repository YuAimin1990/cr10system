#include "ipctool_client_handler.h"

ar_ipc_state_t* g_pstIpcClient = NULL;
AR_U32 g_u32ClientHandle = 0;

AR_S32 AR_IPCTOOL_BinderInit(void)
{
	g_pstIpcClient = ar_ipc_create();
	if(!g_pstIpcClient){
		printf("failed to create client ipc\r\n");
		return -1;
	}
	g_u32ClientHandle = ar_ipc_get_service(g_pstIpcClient, BINDER_IPCTOOL_NAME);
	if(!g_u32ClientHandle){
		printf("failed to get service BINDER_IPCTOOL_NAME \r\n");
		return -1;
	}
	return 0;
}

IPCTOOL_CMD_ARGS_S * AR_IPCTOOL_GetArgs(const AR_CHAR *ps8Cmd)
{
	IPCTOOL_CMD_ARGS_S * pstArgs = NULL;
	AR_CHAR *ps8CmdTemp=NULL;
	AR_CHAR *ps8Str = NULL;
	AR_CHAR *ps8SplitChar = " ";

	pstArgs = (IPCTOOL_CMD_ARGS_S *)malloc(sizeof(IPCTOOL_CMD_ARGS_S));
	ps8CmdTemp = (AR_CHAR *)malloc(1024);
	if((!pstArgs) || (!ps8CmdTemp)){
		printf("malloc failed \r\n");
		return NULL;
	}
	memset(pstArgs, 0, sizeof(IPCTOOL_CMD_ARGS_S));
	strcpy(ps8CmdTemp, ps8Cmd);
	ps8Str = strtok(ps8CmdTemp, ps8SplitChar);
	if(ps8Str)
	{
		strcpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0], ps8Str);
		pstArgs->s32ArgsNum++;
	}
	while(ps8Str)
	{
		ps8Str=strtok(NULL, ps8SplitChar);
		if(ps8Str)
		{
			  strcpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0],ps8Str);
			  pstArgs->s32ArgsNum++;
		}
	}
	free(ps8CmdTemp);
	return pstArgs;
}

AR_S32 AR_IPCTOOL_IndexFindByName(AR_CHAR * ps8Name, IPCTOOL_CMD_ARGS_S *pstArgv)
{
    	AR_S32 s32Index=-1;
	for(int i=0; i < pstArgv->s32ArgsNum; i++)
	{
		if(!strcmp(ps8Name, pstArgv->s8Args[i]))
		{
			s32Index = i;
			break;
		}
	}
	return s32Index;
}

AR_S32 AR_IPCTOOL_HandlerViRes(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VI_RES_S stViRes;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-w", pstCmdArgs);
	if(s32Index1 >= 0){
		stViRes.s32W = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-h", pstCmdArgs);
	if(s32Index2 >= 0){
		stViRes.s32H = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	s32Index3 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index3 >= 0){
		stViRes.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_RES, &stViRes, sizeof(stViRes), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerViFps(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VI_FPS_S stViFps;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
	if(s32Index1 >= 0){
		stViFps.s32Fps = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index2 >= 0){
		stViFps.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}

	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_FPS, &stViFps, sizeof(stViFps), &stBinderRet, 0);

	return s32Ret;
}
AR_S32 AR_IPCTOOL_HandlerViAntiFlicker(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_ANTI_FLICKER_S stAnti;
    BINDER_RET_S stBinderRet;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        stAnti.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
    if(s32Index >= 0){
        stAnti.s32Freqency = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_ANTIFLICKER, &stAnti, sizeof(stAnti), &stBinderRet, 0);

    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerSetDenoise3D(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_DENOISE_S stDe;
    BINDER_RET_S stBinderRet;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        stDe.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
    if(s32Index >= 0){
        stDe.s32Strength = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_DENOISE_3D, &stDe, sizeof(stDe), &stBinderRet, 0);

    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerSetDenoise2D(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_DENOISE_S stDe;
    BINDER_RET_S stBinderRet;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        stDe.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
    if(s32Index >= 0){
        stDe.s32Strength = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_DENOISE_2D, &stDe, sizeof(stDe), &stBinderRet, 0);

    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerGetDenoise3D(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_DENOISE_S stArgs;
    IPC_CTRL_VI_DENOISE_S stRet;
    AR_S32 s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0)
    {
        stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_VI_DENOISE_3D, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d denoise 3d strength: %d\n", stRet.s32ChannelId, stRet.s32Strength);

    return stRet.s32Strength;

}

AR_S32 AR_IPCTOOL_HandlerGetDenoise2D(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_DENOISE_S stArgs;
    IPC_CTRL_VI_DENOISE_S stRet;
    AR_S32 s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0)
    {
        stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_VI_DENOISE_2D, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d denoise 2d strength: %d\n", stRet.s32ChannelId, stRet.s32Strength);

    return stRet.s32Strength;

}

AR_S32 AR_IPCTOOL_HandlerGetExpLimit(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_EXPLIMIT_S stArgs;
    IPC_CTRL_VI_EXPLIMIT_S stRet;
    AR_S32 s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0)
    {
        stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_VI_EXPLIMIT, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d explimit: %d us\n", stRet.s32ChannelId, stRet.s32ExpLimitUs);

    return stRet.s32ExpLimitUs;

}

AR_S32 AR_IPCTOOL_HandlerSetExpLimit(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_EXPLIMIT_S stExp;
    BINDER_RET_S stBinderRet;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        stExp.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-e", pstCmdArgs);
    if(s32Index >= 0){
        stExp.s32ExpLimitUs = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_EXPLIMIT, &stExp, sizeof(stExp), &stBinderRet, 0);

    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerViWdr(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_WDR_S stWdr;
    BINDER_RET_S stBinderRet;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        stWdr.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
    if(s32Index >= 0){
        stWdr.s32Enable = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VI_WDR, &stWdr, sizeof(stWdr), &stBinderRet, 0);

    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerVi3aInfo(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VI_3A_S st3aInfoSend;
    IPC_CTRL_VI_3A_S st3aInfoReceive;
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0){
        st3aInfoSend.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    if(s32Index < 0){
        printf("cmd parameters error\r\n");
        return -1;
    }
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_VI_3AINFO, &st3aInfoSend, sizeof(st3aInfoSend), &st3aInfoReceive, 0);

    printf("3A Info Receive Param : channel id = %d\n", st3aInfoReceive.s32ChannelId);
    printf("ACE Param : mode:%d, gain:%f, exp index:%d, lux index:%d, luma_target:%d, exp time us:%f, exp time us short:%f\n",
            st3aInfoReceive.s32AecMode, st3aInfoReceive.f32AecGain,
                    st3aInfoReceive.u32ExpIndex, st3aInfoReceive.u32LuxIndex, st3aInfoReceive.u32AecLumaTarget,
                         st3aInfoReceive.f32AecExpTimeUs, st3aInfoReceive.f32AecExpTimeUsShort);
    printf("AWB Param : awb mode:%d, cct:%d, R gain: %f, G gain: %f, B gain: %f\n",
            st3aInfoReceive.s32AwbMode, st3aInfoReceive.u32AwbCct,
                    st3aInfoReceive.f32AwbRGain, st3aInfoReceive.f32AwbGGain,
                        st3aInfoReceive.f32AwbBGain);
    printf("AF Param :focus value:%d, len position: %d, total step: %d\n",
            st3aInfoReceive.u32AfFocusValue, st3aInfoReceive.u32AfLenPosition, st3aInfoReceive.u32AfTotalSteps);

    return s32Ret;

}


AR_S32 AR_IPCTOOL_HandlerVencBrc(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VENC_BRC_S stVencBrc;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}

	s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32Stream = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-m", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32Mode = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
    s32Index = AR_IPCTOOL_IndexFindByName("-max", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32Max = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
    s32Index = AR_IPCTOOL_IndexFindByName("-min", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32Min = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
    s32Index = AR_IPCTOOL_IndexFindByName("-avg", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32Average = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-stattime", pstCmdArgs);
	if(s32Index >= 0){
		stVencBrc.s32StatTime = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}

    //Need to parse min/max/avg
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VENC_BRC, &stVencBrc, sizeof(stVencBrc), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVencGop(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VENC_GOP_S stVencGop;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index1 >= 0){
		stVencGop.s32Stream = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stVencGop.s32Value= atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	s32Index3 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index3 >= 0){
		stVencGop.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VENC_GOP, &stVencGop, sizeof(stVencGop), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVencType(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VENC_TYPE_S stVencType;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index1 >= 0){
		stVencType.s32Stream = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-t", pstCmdArgs);
	if(s32Index2 >= 0){
		stVencType.s32Type= atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	s32Index3 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index3 >= 0){
		stVencType.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VENC_TYPE, &stVencType, sizeof(stVencType), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVencFps(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VENC_FPS_S stVencFps;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index1 >= 0){
		stVencFps.s32Stream = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
	if(s32Index2 >= 0){
		stVencFps.s32Fps = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	s32Index3 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index3 >= 0){
		stVencFps.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VENC_FPS, &stVencFps, sizeof(stVencFps), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVencQp(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VENC_QP_S stQp = {0};
    BINDER_RET_S stBinderRet;
    int s32Index;
        
    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    
    s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32Stream = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    
    s32Index = AR_IPCTOOL_IndexFindByName("-minQpI", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32MinIQp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    
    s32Index = AR_IPCTOOL_IndexFindByName("-maxQpI", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32MaxIQp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    
    s32Index = AR_IPCTOOL_IndexFindByName("-minQpP", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32MinPQp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    
    s32Index = AR_IPCTOOL_IndexFindByName("-maxQpP", pstCmdArgs);
    if(s32Index >= 0)
    {
        stQp.s32MaxPQp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VENC_QP, &stQp, sizeof(stQp), &stBinderRet, 0);
    
    return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVrecord(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VRECORD_S stVrecord;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3,s32Index4,s32Index5;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index1 >= 0){
		stVrecord.s32Stream = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index2 >= 0){
		stVrecord.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	s32Index3 = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
	if(s32Index3 >= 0){
		stVrecord.s32Format = atoi(pstCmdArgs->s8Args[s32Index3 + 1]);
	}
	s32Index4 = AR_IPCTOOL_IndexFindByName("-m", pstCmdArgs);
	if(s32Index4 >= 0){
		stVrecord.s32Mode = atoi(pstCmdArgs->s8Args[s32Index4 + 1]);
	}
    s32Index5 = AR_IPCTOOL_IndexFindByName("-t", pstCmdArgs);
	if(s32Index5 >= 0){
		stVrecord.s32StartOrStop = atoi(pstCmdArgs->s8Args[s32Index5 + 1]);
	}

	if((s32Index1 < 0) || (s32Index2 < 0) || (s32Index3 < 0) || (s32Index4 < 0) || (s32Index5 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VRECORD, &stVrecord, sizeof(stVrecord), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerArecord(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_ARECORD_S stArecord;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-path", pstCmdArgs);
	if(s32Index1 >= 0){
		strcpy(stArecord.cPathName, pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	
    s32Index2 = AR_IPCTOOL_IndexFindByName("-start",pstCmdArgs);
	if(s32Index2 >= 0){
		stArecord.s32StartOrStop = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}

	s32Index3 = AR_IPCTOOL_IndexFindByName("-stream",pstCmdArgs);
	if(s32Index3 >= 0){
		stArecord.u32StreamId = atoi(pstCmdArgs->s8Args[s32Index3 + 1]);
	}

	if((s32Index1 < 0) || (s32Index2 < 0))
	{
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_ARECORD, &stArecord, sizeof(stArecord), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerPlayback(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_PLAYBACK_S stPb;
	BINDER_RET_S stBinderRet;
	int s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
	if(s32Index >= 0){
		stPb.s32AudioOrVedio = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	
	s32Index = AR_IPCTOOL_IndexFindByName("-path", pstCmdArgs);
	if(s32Index >= 0){
		strcpy(stPb.achFileName, pstCmdArgs->s8Args[s32Index + 1]);
	}
	
	s32Index = AR_IPCTOOL_IndexFindByName("-start", pstCmdArgs);
	if(s32Index >= 0){
		stPb.s32StartOrStop = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_PLAYBACK, &stPb, sizeof(stPb), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerFlip(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_FLIP_S stFlip;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stFlip.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index2 >= 0){
		stFlip.s32Enable = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_FLIP, &stFlip, sizeof(stFlip), &stBinderRet, 0);

	return s32Ret;
}


AR_S32 AR_IPCTOOL_HandlerMd(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_MD_S stMd;
	BINDER_RET_S stBinderRet;
	int s32Index1;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index1 >= 0){
		stMd.s32Enable = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	if(s32Index1 < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_MD, &stMd, sizeof(stMd), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerOd(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_OD_S stOd;
	BINDER_RET_S stBinderRet;
	int s32Index1;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index1 >= 0){
		stOd.s32Enable = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	if(s32Index1 < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_OD, &stOd, sizeof(stOd), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerMirror(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_MIRROR_S stMirror;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stMirror.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index2 >= 0){
		stMirror.s32Enable = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_MIRROR, &stMirror, sizeof(stMirror), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSnapshot(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_SNAPSHOT_S stSnapshot;
	BINDER_RET_S stBinderRet;
	int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-g",pstCmdArgs);
	if(s32Index >= 0){
		stSnapshot.s32VpssGrp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index >= 0){
		stSnapshot.s32VpssChn = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-cnt", pstCmdArgs);
	if(s32Index >= 0){
		stSnapshot.s32Cnt = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
    s32Index = AR_IPCTOOL_IndexFindByName("-path", pstCmdArgs);
	if(s32Index >= 0){
		strcpy(stSnapshot.strPath, pstCmdArgs->s8Args[s32Index + 1]);
	}

	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SNAPSHOT, &stSnapshot, sizeof(stSnapshot), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVoFullScreen(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VOFULLSCREEN_S stVoFs;
	BINDER_RET_S stBinderRet;
	int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index >= 0){
		stVoFs.s32VoChn = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index >= 0){
		stVoFs.s32Enable = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}

	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VOFULLSCREEN, &stVoFs, sizeof(stVoFs), &stBinderRet, 0);

	return s32Ret;
}


AR_S32 AR_IPCTOOL_HandlerImageScene(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_SCENE_S stImageScene;
	BINDER_RET_S stBinderRet;
	int s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index >= 0){
		stImageScene.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-m", pstCmdArgs);
	if(s32Index >= 0){
		stImageScene.s32Mode = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
    s32Index = AR_IPCTOOL_IndexFindByName("-f", pstCmdArgs);
	if(s32Index >= 0){
		strcpy(stImageScene.strTunningFile, pstCmdArgs->s8Args[s32Index + 1]);
	}

	if(s32Index < 0)
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_SCENE, &stImageScene, sizeof(stImageScene), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerImageStyle(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_STYLE_S stImageStyle;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageStyle.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageStyle.s32Style = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_STYLE, &stImageStyle, sizeof(stImageStyle), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetBright(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_BRIGHT_S stImageBright;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageBright.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageBright.s32BrightValue = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_BRIGHT, &stImageBright, sizeof(stImageBright), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetSaturation(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_SATURATION_S stImageSaturation;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageSaturation.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageSaturation.s32SaturationValue = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_SATURATION, &stImageSaturation, sizeof(stImageSaturation), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetContrast(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_CONTRAST_S stImageContrast;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageContrast.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageContrast.s32ContrastValue = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_CONTRAST, &stImageContrast, sizeof(stImageContrast), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetSharpness(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_SHARPNESS_S stImageSharpness;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageSharpness.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageSharpness.s32SharpnessValue = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_SHARPNESS, &stImageSharpness, sizeof(stImageSharpness), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetHue(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_HUE_S stImageHue;
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index1 >= 0){
		stImageHue.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}
	s32Index2 = AR_IPCTOOL_IndexFindByName("-v", pstCmdArgs);
	if(s32Index2 >= 0){
		stImageHue.s32HueValue = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}
	if((s32Index1 < 0) || (s32Index2 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_IMG_HUE, &stImageHue, sizeof(stImageHue), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerGetBright(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_BRIGHT_S stArgs;
	IPC_CTRL_IMG_BRIGHT_S stRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0)
	{
		stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	else
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_IMG_BRIGHT, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d brightness: %d\n", stRet.s32ChannelId, stRet.s32BrightValue);

	return stRet.s32BrightValue;
}

AR_S32 AR_IPCTOOL_HandlerGetSaturation(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_SATURATION_S stArgs;
	IPC_CTRL_IMG_SATURATION_S stRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0)
    {
		stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	else
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_IMG_SATURATION, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d saturation: %d\n", stRet.s32ChannelId, stRet.s32SaturationValue);

	return stRet.s32SaturationValue;
}

AR_S32 AR_IPCTOOL_HandlerGetContrast(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_CONTRAST_S stArgs;
	IPC_CTRL_IMG_CONTRAST_S stRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0)
    {
		stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	else
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_IMG_CONTRAST, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d contrast: %d\n", stRet.s32ChannelId, stRet.s32ContrastValue);

	return stRet.s32ContrastValue;
}

AR_S32 AR_IPCTOOL_HandlerGetSharpness(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_SHARPNESS_S stArgs;
	IPC_CTRL_IMG_SHARPNESS_S stRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0)
    {
		stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	else
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_IMG_SHARPNESS, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d sharpness: %d\n", stRet.s32ChannelId, stRet.s32SharpnessValue);

    return stRet.s32SharpnessValue;
}

AR_S32 AR_IPCTOOL_HandlerGetHue(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
	IPC_CTRL_IMG_HUE_S stArgs;
	IPC_CTRL_IMG_HUE_S stRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0)
    {
		stArgs.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	else
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_IMG_HUE, &stArgs, sizeof(stArgs), &stRet, 0);
    if(s32Ret)
    {
        printf("Send RPC error: %d.\n", s32Ret);
        return s32Ret;
    }

    printf("Channel %d hue: %d\n", stRet.s32ChannelId, stRet.s32HueValue);

	return stRet.s32HueValue;

}

AR_S32 AR_IPCTOOL_HandlerWdt(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_WDT_S stWdt;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-power", pstCmdArgs);
	if(s32Index >= 0){
		stWdt.s32OnOff = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_WDT, &stWdt, sizeof(stWdt), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerGetGpio(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_GPIO_S stGpio = {0};
	IPC_CTRL_GPIO_S stStatus = {0};
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-n", pstCmdArgs);
	if(s32Index >= 0){
		strncpy(stGpio.strName, pstCmdArgs->s8Args[s32Index + 1], 7);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_GPIO, &stGpio, sizeof(stGpio), &stStatus, 0);
    printf("Get GPIO %s status: dir %d, value %d\n", stStatus.strName, stStatus.s32Direction, stStatus.s32Value);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerSetGpio(IPCTOOL_CMD_ARGS_S * pstCmdArgs)

{
	AR_S32 s32Ret = 0;
	IPC_CTRL_GPIO_S stGpio = {0};
    BINDER_RET_S stBinderRet;
	int s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-n", pstCmdArgs);
	if(s32Index >= 0){
		strncpy(stGpio.strName,pstCmdArgs->s8Args[s32Index + 1], 7);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-d", pstCmdArgs);
	if(s32Index >= 0){
		stGpio.s32Direction = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	s32Index = AR_IPCTOOL_IndexFindByName("-v",pstCmdArgs);
	if(s32Index >= 0){
		stGpio.s32Value = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}

	if(s32Index < 0)
    {
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_GPIO, &stGpio, sizeof(stGpio), &stBinderRet, 0);

	return s32Ret;
}


AR_S32 AR_IPCTOOL_HandlerSvp(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_SVP_S stSetSvp;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-power", pstCmdArgs);
	if(s32Index >= 0){
		stSetSvp.s32OnOff = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_SVP, &stSetSvp, sizeof(stSetSvp), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerReboot(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_REBOOT_S stReboot;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-reboot", pstCmdArgs);
	if(s32Index >= 0){
		stReboot.s32RebootFlag = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_REBOOT, &stReboot, sizeof(stReboot), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerPowerOff(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_POWEROFF_S stSysPowerOff;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-power", pstCmdArgs);
	if(s32Index >= 0){
		stSysPowerOff.s32PoweroffFlag = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_POWEROFF, &stSysPowerOff, sizeof(stSysPowerOff), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerWifiMode(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_WIFIMODE_S stWifiMode;
	BINDER_RET_S stBinderRet;
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-m", pstCmdArgs);
	if(s32Index >= 0){
		stWifiMode.s32WifiMode = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_WIFI_MODE, &stWifiMode, sizeof(stWifiMode), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerDumpYuv(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_DUMP_YUV_S stDump = {0};
	BINDER_RET_S stBinderRet = {0};
	AR_S32 s32Index;

	s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
	if(s32Index >= 0){
		stDump.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
	}
	if(s32Index < 0){
		printf("cmd parameters error\r\n");
		return -1;
	}

	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_DUMP_YUV, &stDump, sizeof(stDump), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerGetVencParam(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_VENC_CH_S stCh = {0};
    AR_S32 s32Index = 0;
    AR_CHAR str[8] = {0};

    s32Index = AR_IPCTOOL_IndexFindByName("-c", pstCmdArgs);
    if(s32Index >= 0)
    {
        stCh.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
    if(s32Index >= 0)
    {
        stCh.s32StreamId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
	if(s32Index < 0)
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_GET_VENC_PARAM, &stCh, sizeof(stCh), &stCh, 0);
    if(s32Ret)
    {
        printf("Remote call error: %s\n", strerror(errno));
        return -1;
    }

    if(stCh.stParam.u32CodecType == VENC_ENCODE_TYPE_H264)
    {
        strcpy(str, "H264");
    }
    else if(stCh.stParam.u32CodecType == VENC_ENCODE_TYPE_H265)
    {
        strcpy(str, "H265");
    }
    else if(stCh.stParam.u32CodecType == VENC_ENCODE_TYPE_MJPEG)
    {
        strcpy(str, "MJPEG");
    }
    else
    {
         strcpy(str, "Unkown");
    }

    printf("Codec type: %s\n", str);
    printf("RC mode: %s\n", stCh.stParam.u32RcMode == VENC_BRCMODE_CBR ? "CBR" :
                            stCh.stParam.u32RcMode == VENC_BRCMODE_VBR ? "VBR" : "Unkown");
    printf("Height: %d, width: %d, frame rate: %d\n", 
        stCh.stParam.u32Height, stCh.stParam.u32Width, stCh.stParam.u32FrameRate);
    printf("Gop: %d, Cbr bit rate: %dKbps, min bit rate: %dKbps, max bit rate: %dKbps, stattime: %d\n", 
        stCh.stParam.u32Gop, stCh.stParam.u32CbrKbps, 
        stCh.stParam.u32MinKbps, stCh.stParam.u32MaxKbps, stCh.stParam.u32StatTime);
    printf("MinQpI: %d, maxQpI: %d, minQpP: %d, maxQpP: %d\n",
        stCh.stParam.u32MinIQp, stCh.stParam.u32MaxIQp, stCh.stParam.u32MinPQp, stCh.stParam.u32MaxPQp);
    return s32Ret;

}

AR_S32 AR_IPCTOOL_HandlerDrawOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_OSD_S stOsd = {0};
    BINDER_RET_S stBinderRet = {0};
    int s32Index;
    AR_S32 i = 0;

    s32Index = AR_IPCTOOL_IndexFindByName("-g",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32VpssGrp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-iw",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32ImgWidth = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-ih",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32ImgHeight = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-s", pstCmdArgs);
    if(s32Index >= 0)
    {
        for(i = 0; i < IPC_ALG_OSD_MAX_NUM; i++)
        {
            strncpy(stOsd.stOsdStr[i].achStr, pstCmdArgs->s8Args[s32Index + 1], IPC_ALG_OSD_STR_MAX_LEN);
            stOsd.stOsdStr[i].u32X = 10;
            stOsd.stOsdStr[i].u32Y = 50 + i * 30;
        }
        stOsd.u32StrNum = IPC_ALG_OSD_MAX_NUM;
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-sc", pstCmdArgs);
    if(s32Index >= 0)
    {
        for(i = 0; i < IPC_ALG_OSD_MAX_NUM; i++)
        {
            stOsd.stOsdStr[i].u32RGB = strtoul(pstCmdArgs->s8Args[s32Index + 1], NULL, 0);
        }
    }

    s32Index = AR_IPCTOOL_IndexFindByName("-rect", pstCmdArgs);
    if(s32Index >= 0)
    {
        for(i = 0; i < IPC_ALG_OSD_MAX_NUM; i++)
        {
            stOsd.stOsdRect[i].u32X = 50 + i * 20;
            stOsd.stOsdRect[i].u32Y = 50 + i * 20;
            stOsd.stOsdRect[i].u32W = 100;
            stOsd.stOsdRect[i].u32H = 100;
        }

        stOsd.u32RectNum = IPC_ALG_OSD_MAX_NUM;
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-rc", pstCmdArgs);
    if(s32Index >= 0)
    {
        for(i = 0; i < IPC_ALG_OSD_MAX_NUM; i++)
        {
            stOsd.stOsdRect[i].u32RGB = strtoul(pstCmdArgs->s8Args[s32Index + 1], NULL, 0);
        }
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);

    return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerClearOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_OSD_S stOsd = {0};
    BINDER_RET_S stBinderRet = {0};
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-g",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32VpssGrp = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_DRAW_VPSS_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);

    return s32Ret;
}

//"setstreamosd", "setstreamosd -c [channel] -s [stream] -name [string] -r [RGB] -ce [0/1] -te [0/1] -f [fontid]"
AR_S32 AR_IPCTOOL_HandlerSetStreamOsd(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32 s32Ret = 0;
    IPC_CTRL_STREAM_OSD_S stOsd = {0};
    BINDER_RET_S stBinderRet = {0};
    int s32Index;

    s32Index = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32ChannelId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-s",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32StreamId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-name",pstCmdArgs);
    if(s32Index >= 0)
    {
        strncpy(stOsd.strChnName, pstCmdArgs->s8Args[s32Index + 1], sizeof(stOsd.strChnName) - 1);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-r",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32RGB = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-ce",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32ChnNameEnable = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-te",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32DateTimeEnable = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
	s32Index = AR_IPCTOOL_IndexFindByName("-tf",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32TimeFormat = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
	s32Index = AR_IPCTOOL_IndexFindByName("-df",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32DateFormat = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
	s32Index = AR_IPCTOOL_IndexFindByName("-we",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32DisplayWeek = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }
    s32Index = AR_IPCTOOL_IndexFindByName("-f",pstCmdArgs);
    if(s32Index >= 0)
    {
        stOsd.u32FontSize = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_STREAM_OSD, &stOsd, sizeof(stOsd), &stBinderRet, 0);

    return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVpssFlip(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VPSS_FLIP_S stFlip = {0};
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-g",pstCmdArgs);
	if(s32Index1 >= 0){
		stFlip.s32GroupId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}

	s32Index2 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index2 >= 0){
		stFlip.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}

	s32Index3 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index3 >= 0){
		stFlip.s32Enable = atoi(pstCmdArgs->s8Args[s32Index3 + 1]);
	}
	if((s32Index1 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}
	printf("stFlip.s32GroupId %d, stFlip.s32ChannelId %d, stFlip.s32Enable %d\n", stFlip.s32GroupId, stFlip.s32ChannelId, stFlip.s32Enable);
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VPSS_FLIP, &stFlip, sizeof(stFlip), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerVpssMirror(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	IPC_CTRL_VPSS_MIRROR_S stMirror = {0};
	BINDER_RET_S stBinderRet;
	int s32Index1,s32Index2,s32Index3;

	s32Index1 = AR_IPCTOOL_IndexFindByName("-g",pstCmdArgs);
	if(s32Index1 >= 0){
		stMirror.s32GroupId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
	}

	s32Index2 = AR_IPCTOOL_IndexFindByName("-c",pstCmdArgs);
	if(s32Index2 >= 0){
		stMirror.s32ChannelId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);
	}

	s32Index3 = AR_IPCTOOL_IndexFindByName("-en", pstCmdArgs);
	if(s32Index3 >= 0){
		stMirror.s32Enable = atoi(pstCmdArgs->s8Args[s32Index3 + 1]);
	}
	if((s32Index1 < 0) || (s32Index3 < 0)){
		printf("cmd parameters error\r\n");
		return -1;
	}

	printf("stMirror.s32GroupId %d, stMirror.s32ChannelId %d, stMirror.s32Enable %d\n", stMirror.s32GroupId, stMirror.s32ChannelId, stMirror.s32Enable);
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_SET_VPSS_MIRROR, &stMirror, sizeof(stMirror), &stBinderRet, 0);

	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerStart(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	BINDER_RET_S stBinderRet = {0};
	int t = 1;
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_START, &t, sizeof(int), &stBinderRet, 0);
	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerStop(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	BINDER_RET_S stBinderRet = {0};
	int t = 1;
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_STOP,  &t, sizeof(int), &stBinderRet, 0);
	return s32Ret;
}
AR_S32 AR_IPCTOOL_HandlerCreate(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	BINDER_RET_S stBinderRet = {0};
	int t = 1;
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_CREATE, &t, sizeof(int), &stBinderRet, 0);
	return s32Ret;
}

AR_S32 AR_IPCTOOL_HandlerDestory(IPCTOOL_CMD_ARGS_S * pstCmdArgs)
{
	AR_S32 s32Ret = 0;
	BINDER_RET_S stBinderRet = {0};
	int t = 1;
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_DESTORY,  &t, sizeof(int), &stBinderRet, 0);
	return s32Ret;
}

AR_S32 AR_IPCTOOL_Send_Cmd(char* pstCmdString)
{
	AR_S32 s32Ret = 0;
	IPCTOOL_CMD_STRING_S stCmdString = {0};
	BINDER_RET_S stBinderRet = {0};

	strcpy(stCmdString.s8Cmd_String, pstCmdString);
	s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, IPC_RPC_CMD_RTCMD, &stCmdString, sizeof(IPCTOOL_CMD_STRING_S), &stBinderRet, 0);
	return s32Ret;
}


