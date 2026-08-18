#include "votest_client_handler.h"

ar_ipc_state_t* g_pstIpcClient = NULL;
AR_U32 g_u32ClientHandle = 0;

AR_S32 AR_VOTEST_BinderInit(void)
{
    g_pstIpcClient = ar_ipc_create();
    if(!g_pstIpcClient)
    {
        printf("failed to create client ipc\r\n");
        return -1;
    }

    g_u32ClientHandle = ar_ipc_get_service(g_pstIpcClient, BINDER_VOTEST_NAME);
    if(!g_u32ClientHandle)
    {
        printf("failed to get service %s \r\n", BINDER_VOTEST_NAME);
        return -1;
    }

    return 0;
}

VOTEST_CMD_ARGS_S * AR_VOTEST_GetArgs(const AR_CHAR *ps8Cmd)
{
    VOTEST_CMD_ARGS_S * pstArgs = NULL;
    AR_CHAR *ps8CmdTemp=NULL;
    AR_CHAR *ps8Str = NULL;
    AR_CHAR *ps8SplitChar = " ";

	if(strlen(ps8Cmd) >= 1024)
	{
        printf("cmd %s too long\r\n", ps8Cmd);
        return NULL;
	}

    pstArgs = (VOTEST_CMD_ARGS_S *)malloc(sizeof(VOTEST_CMD_ARGS_S));
    if(!pstArgs)
    {
        printf("malloc failed \r\n");
        return NULL;
    }

    ps8CmdTemp = (AR_CHAR *)malloc(1024);
    if(!ps8CmdTemp)
    {
		free(pstArgs);
        printf("malloc failed \r\n");
        return NULL;
    }

    memset(pstArgs, 0, sizeof(VOTEST_CMD_ARGS_S));
    strcpy(ps8CmdTemp, ps8Cmd);
    ps8Str = strtok(ps8CmdTemp, ps8SplitChar);
    if(ps8Str)
    {
        strncpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0], ps8Str, 127);
        pstArgs->s32ArgsNum++;
    }

    while(ps8Str)
    {
        ps8Str=strtok(NULL, ps8SplitChar);
        if(ps8Str)
        {
            strncpy(&pstArgs->s8Args[pstArgs->s32ArgsNum][0],ps8Str, 127);
            pstArgs->s32ArgsNum++;
        }
    }

    free(ps8CmdTemp);
    return pstArgs;
}

AR_S32 AR_VOTEST_IndexFindByName(AR_CHAR * ps8Name, VOTEST_CMD_ARGS_S *pstArgv)
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

AR_S32 AR_VOTEST_ClientPauseChannel(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index1, s32Index2, s32Index3;

    s32Index1 = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index1 >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);

    s32Index2 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index2 >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);

    s32Index3 = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index3 >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_PAUSE,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientResumeChannel(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index1, s32Index2, s32Index3;

    s32Index1 = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index1 >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);

    s32Index2 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index2 >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);

    s32Index3 = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index3 >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_RESUME,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientShowChannel(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index1, s32Index2, s32Index3;

    s32Index1 = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index1 >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);

    s32Index2 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index2 >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);

    s32Index3 = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index3 >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_SHOW,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientHideChannel(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index1, s32Index2, s32Index3;

    s32Index1 = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index1 >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);

    s32Index2 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index2 >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);

    s32Index3 = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index3 >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_HIDE,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientStepChannel(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index1, s32Index2, s32Index3;

    s32Index1 = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index1 >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);

    s32Index2 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index2 >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index2 + 1]);

    s32Index3 = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index3 >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index3+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_STEP,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientDumpLayerFrame(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32                 s32Ret  = 0;
    BINDER_RET_S           stBinderRet;
    VOTEST_RPC_LAYER_DUMP  stLayer = {0};
    AR_S32                 s32Index1, s32Index2;

    s32Index1 = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index1 >= 0)
        stLayer.eLayerId = atoi(pstCmdArgs->s8Args[s32Index1 + 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Index2 = AR_VOTEST_IndexFindByName("-name", pstCmdArgs);
    if(s32Index2 >= 0)
    {
        if (strlen(pstCmdArgs->s8Args[s32Index2 + 1]) >= MAX_DUMP_FILE_NAME)
        {
            printf("filename too long\r\n");
            return -1;
        }
        strcpy(stLayer.sFileName, pstCmdArgs->s8Args[s32Index2 + 1]);
    }
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    printf("dump layer_%d frame to %s %ld\r\n", stLayer.eLayerId, stLayer.sFileName, sizeof(VOTEST_RPC_LAYER_DUMP));
    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_LAYER_DUMP_FRAME,
                                    &stLayer, sizeof(stLayer), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientEnableDev(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32                s32Ret  = 0;
    ENUM_AR_HAL_VO_DEV_ID eVoDevId = {0};
    BINDER_RET_S          stBinderRet;
    AR_S32                s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        eVoDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_DEV_ENABLE,
                                    &eVoDevId, sizeof(eVoDevId), &stBinderRet, 0);
    return s32Ret;

}
AR_S32 AR_VOTEST_ClientDisableDev(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32                s32Ret  = 0;
    ENUM_AR_HAL_VO_DEV_ID eVoDevId = {0};
    BINDER_RET_S          stBinderRet;
    AR_S32                s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        eVoDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_DEV_DISABLE,
                                    &eVoDevId, sizeof(eVoDevId), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientEnableLayer(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_LAYER    stVoLayer = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        stVoLayer.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index >= 0)
        stVoLayer.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_LAYER_ENABLE,
                                    &stVoLayer, sizeof(stVoLayer), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientDisableLayer(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_LAYER    stVoLayer = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        stVoLayer.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index >= 0)
        stVoLayer.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_LAYER_DISABLE,
                                    &stVoLayer, sizeof(stVoLayer), &stBinderRet, 0);
    return s32Ret;

}

AR_S32 AR_VOTEST_ClientEnableChn(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_ENABLE,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}

AR_S32 AR_VOTEST_ClientDisableChn(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    AR_S32              s32Ret  = 0;
    VOTEST_RPC_CHANNEL  stVoChn = {0};
    BINDER_RET_S        stBinderRet;
    AR_S32              s32Index;

    s32Index = AR_VOTEST_IndexFindByName("-dev", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-chan", pstCmdArgs);
    if(s32Index >= 0)
        stVoChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index+ 1]);
    else
    {
        printf("cmd parameters error\r\n");
        return -1;
    }

    s32Ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_CHN_DISABLE,
                                    &stVoChn, sizeof(stVoChn), &stBinderRet, 0);
    return s32Ret;
}


AR_S32 AR_VOTEST_ClientSetDevAttr(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    VOTEST_RPC_DEV_ATTR      prpcDevAttr = {0};
    BINDER_RET_S             stBinderRet;
    AR_S32                   ret = HAL_NO_ERROR;
    AR_S32                   fd = -1;
    STRU_AR_HAL_VO_DEV_ATTR  dev_attr = {0};
    AR_S32                   s32Index = -1;
    ENUM_AR_HAL_VO_CSC       dev_csc=AR_HAL_VO_CSC_DEFAULT;

    s32Index = AR_VOTEST_IndexFindByName("-dev_dev_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcDevAttr.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no dev_dev_id\r\n");
        return -1;
    }
    printf("%s: devId:%d\n", __FUNCTION__, prpcDevAttr.eDevId);

    fd = ar_hal_vo_dev_open(prpcDevAttr.eDevId);
    if (fd < 0)
    {
        printf("open devId_%d failed\n", prpcDevAttr.eDevId);
        return -1;
    }

    ret = ar_hal_vo_dev_get_attr(fd, &dev_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_hal_vo_dev_get_csc(fd, &dev_csc);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    s32Index = AR_VOTEST_IndexFindByName("-dev_bg_color", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.bg_color = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_interface", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.e_interface = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_sub_interface", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.e_sub_interface = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_timing_template", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.e_timing_template = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_dpi_format", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.dpi_format = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_color_mode", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.color_mode = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_sync_mode", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.sync_mode = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_pin_mode", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.pin_mode.bt656 = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_csc", pstCmdArgs);
    if(s32Index >= 0)
        prpcDevAttr.eDevCsc = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-dev_bit_count_per_channel", pstCmdArgs);
    if(s32Index >= 0)
        dev_attr.bit_count_per_channel = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    if(AR_HAL_VO_OUTPUT_USER == dev_attr.e_timing_template)
    {
        s32Index = AR_VOTEST_IndexFindByName("-syn_interlace_mod", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.interlace_mod = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_hpw", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.hpw = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_hbp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.hbp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_hdp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.hdp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_hfp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.hfp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_vpw", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.vpw = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_vbp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.vbp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_vdp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.vdp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_vfp", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.vfp = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_fps", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.fps = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_de_polarity", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.de_polarity = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_da_polarity", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.da_polarity = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_clock_polarity", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.clock_polarity = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_hsync_polarity", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.hsync_polarity = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-syn_vsync_polarity", pstCmdArgs);
        if(s32Index >= 0)
            dev_attr.timing_customize.vsync_polarity = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    prpcDevAttr.sDevAttr = dev_attr;

    printf("set dev_%d attr %ld\r\n", prpcDevAttr.eDevId, sizeof(VOTEST_RPC_LAYER_DUMP));
    ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_SET_DEV_ATTR,
                                    &prpcDevAttr, sizeof(prpcDevAttr), &stBinderRet, 0);

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

AR_S32 AR_VOTEST_ClientSetLayerAttr(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    VOTEST_RPC_LAYER_ATTR     prpcLayerAttr = {0};
    BINDER_RET_S              stBinderRet;
    AR_S32                    ret = HAL_NO_ERROR;
    AR_S32                    fd = -1;
    STRU_AR_HAL_VO_LAYER_ATTR layer_attr={0};
    AR_S32                    s32Index = -1;
    STRU_AR_HAL_VO_LAYER_CSC   layer_csc={AR_HAL_VO_CSC_DEFAULT, AR_HAL_VO_CSC_DEFAULT};
    STRU_AR_HAL_VO_POS         layer_pos={0};

    s32Index = AR_VOTEST_IndexFindByName("-dev_dev_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcLayerAttr.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no dev_dev_id\r\n");
        return -1;
    }
    printf("%s: devId:%d\n", __FUNCTION__, prpcLayerAttr.eDevId);

    s32Index = AR_VOTEST_IndexFindByName("-layer_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcLayerAttr.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no layer_id\r\n");
        return -1;
    }
    printf("%s: layerId:%d\n", __FUNCTION__, prpcLayerAttr.eLayerId);

    fd = ar_hal_vo_layer_open(prpcLayerAttr.eLayerId);
    if (fd < 0)
    {
        printf("open layer_%d failed\n", prpcLayerAttr.eLayerId);
        return -1;
    }

    ret = ar_hal_vo_layer_get_attr(fd, &layer_attr);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("get layer_%d attr failed, ret:%d\n", prpcLayerAttr.eLayerId, ret);
        goto End;
    }

    ret = ar_hal_vo_layer_get_csc(fd, &layer_csc);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("get layer_%d csc failed, ret:%d\n", prpcLayerAttr.eLayerId, ret);
        goto End;
    }

    ret = ar_hal_vo_layer_get_pos(fd, &layer_pos);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("get layer_%d pos failed, ret:%d\n", prpcLayerAttr.eLayerId, ret);
        goto End;
    }

    s32Index = AR_VOTEST_IndexFindByName("-layer_width", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.width = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_height", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.height = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_luma_stride", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.luma_stride = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_chroma_stride", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.chroma_stride = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_format", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.format = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_fps", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.fps = atof(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_rotation", pstCmdArgs);
    if(s32Index >= 0)
        layer_attr.rotation = atof(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_csc_in", pstCmdArgs);
    if(s32Index >= 0)
        layer_csc.input = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-layer_csc_out", pstCmdArgs);
    if(s32Index >= 0)
        layer_csc.output = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    if(prpcLayerAttr.eLayerId > 0)
    {
        s32Index = AR_VOTEST_IndexFindByName("-overlay_posx", pstCmdArgs);
        if(s32Index >= 0)
            layer_pos.x = atoi(pstCmdArgs->s8Args[s32Index + 1]);

        s32Index = AR_VOTEST_IndexFindByName("-overlay_posy", pstCmdArgs);
        if(s32Index >= 0)
            layer_pos.y = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    }

    prpcLayerAttr.sLayerAttr = layer_attr;
    prpcLayerAttr.sLayerCsc = layer_csc;
    prpcLayerAttr.sLayerPos = layer_pos;

    printf("set dev_%d layer_%d attr %ld\r\n", prpcLayerAttr.eDevId,  prpcLayerAttr.eLayerId, sizeof(prpcLayerAttr));
    ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_SET_LAYER_ATTR,
                                    &prpcLayerAttr, sizeof(prpcLayerAttr), &stBinderRet, 0);

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

AR_S32 AR_VOTEST_ClientSetChnAttr(VOTEST_CMD_ARGS_S * pstCmdArgs)
{
    VOTEST_RPC_CHN_ATTR       prpcChnAttr = {0};
    BINDER_RET_S              stBinderRet;
    AR_S32                    ret = HAL_NO_ERROR;
    AR_S32                    fd = -1;
    AR_S32                    s32Index = -1;
    STRU_AR_HAL_VO_CHN_ATTR   sChnAttr = {0};
    STRU_AR_VO_CHN_PARAM_S    sChnPara = {0};

    s32Index = AR_VOTEST_IndexFindByName("-dev_dev_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcChnAttr.sChn.eDevId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no dev_dev_id\r\n");
        return -1;
    }
    printf("%s: devId:%d\n", __FUNCTION__, prpcChnAttr.sChn.eDevId);

    s32Index = AR_VOTEST_IndexFindByName("-layer_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcChnAttr.sChn.eLayerId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no layer_id\r\n");
        return -1;
    }
    printf("%s: layerId:%d\n", __FUNCTION__, prpcChnAttr.sChn.eLayerId);

    s32Index = AR_VOTEST_IndexFindByName("-ch_id", pstCmdArgs);
    if(s32Index >= 0)
        prpcChnAttr.sChn.u32ChanId = atoi(pstCmdArgs->s8Args[s32Index + 1]);
    else
    {
        printf("cmd parameters error, no ch_id\r\n");
        return -1;
    }
    printf("%s: ch_id:%d\n", __FUNCTION__, prpcChnAttr.sChn.u32ChanId);

    fd = ar_hal_vo_chn_open(prpcChnAttr.sChn.eLayerId, prpcChnAttr.sChn.u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", prpcChnAttr.sChn.eDevId, prpcChnAttr.sChn.eLayerId, prpcChnAttr.sChn.u32ChanId);
        return -1;
    }

    ret = ar_hal_vo_chn_get_attr(fd, &sChnAttr);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("get chan_%d_%d_%d attr failed, ret:%d\n", prpcChnAttr.sChn.eDevId, prpcChnAttr.sChn.eLayerId, prpcChnAttr.sChn.u32ChanId, ret);
        goto End;
    }

    ret = ar_hal_vo_chn_get_para(fd, &sChnPara);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("get chan_%d_%d_%d para failed, ret:%d\n", prpcChnAttr.sChn.eDevId, prpcChnAttr.sChn.eLayerId, prpcChnAttr.sChn.u32ChanId, ret);
        goto End;
    }

    s32Index = AR_VOTEST_IndexFindByName("-ch_priority", pstCmdArgs);
    if(s32Index >= 0)
        sChnAttr.priority = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_width", pstCmdArgs);
    if(s32Index >= 0)
        sChnAttr.rect.w = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_height", pstCmdArgs);
    if(s32Index >= 0)
        sChnAttr.rect.h = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_pos_x", pstCmdArgs);
    if(s32Index >= 0)
        sChnAttr.rect.x = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_pos_y", pstCmdArgs);
    if(s32Index >= 0)
        sChnAttr.rect.y = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_bg_color", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.u32_bgColor = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_aspect_ratio_mode", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.e_mode = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_aspect_ratio_width", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.st_videoRect.w = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_aspect_ratio_height", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.st_videoRect.h = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_aspect_ratio_pos_x", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.st_videoRect.x = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    s32Index = AR_VOTEST_IndexFindByName("-ch_aspect_ratio_pos_y", pstCmdArgs);
    if(s32Index >= 0)
        sChnPara.st_AspectRatio.st_videoRect.y = atoi(pstCmdArgs->s8Args[s32Index + 1]);

    prpcChnAttr.sChnAttr = sChnAttr;
    prpcChnAttr.sChnPara = sChnPara;

    printf("set dev_%d layer_%d attr %ld\r\n", prpcChnAttr.sChn.eDevId, prpcChnAttr.sChn.eLayerId, sizeof(prpcChnAttr));
    ret = ar_ipc_send_buffer(g_pstIpcClient, g_u32ClientHandle, VOTEST_RPC_SET_CHN_ATTR,
                                    &prpcChnAttr, sizeof(prpcChnAttr), &stBinderRet, 0);

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

