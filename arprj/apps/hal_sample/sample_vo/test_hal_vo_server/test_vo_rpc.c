#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "binder_ipc.h"
#include <fcntl.h>
#include <unistd.h>
#include "test_vo_rpc.h"
#include "test_vo.h"
#include "cfg_vo.h"

ar_ipc_state_t *votest_ipc_handle = NULL;

extern STRU_VO_OBJ_T vo_obj;
extern AR_BOOL cmd_is_running[][MAX_VO_CHN_NUM];
extern CFG_VO_S *cfg_vo;

static AR_S32 votest_rpc_chn_pause(void * args, void * reply);
static AR_S32 votest_rpc_chn_resume(void * args, void * reply);
static AR_S32 votest_rpc_chn_show(void * args, void * reply);
static AR_S32 votest_rpc_chn_hide(void * args, void * reply);
static AR_S32 votest_rpc_chn_step(void * args, void * reply);
static AR_S32 votest_rpc_layer_dump_frame(void * args, void * reply);

static AR_S32 votest_rpc_dev_enable(void * args, void * reply);
static AR_S32 votest_rpc_dev_disable(void * args, void * reply);
static AR_S32 votest_rpc_layer_enable(void * args, void * reply);
static AR_S32 votest_rpc_layer_disable(void * args, void * reply);
static AR_S32 votest_rpc_chn_enable(void * args, void * reply);
static AR_S32 votest_rpc_chn_disable(void * args, void * reply);

static AR_S32 votest_rpc_set_dev_attr(void * args, void * reply);
static AR_S32 votest_rpc_set_layer_attr(void * args, void * reply);
static AR_S32 votest_rpc_set_chn_attr(void * args, void * reply);

VOTEST_RPC_SERVICE_T votest_rpc_table[] = {
    {VOTEST_RPC_CHN_PAUSE,         sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_pause},
    {VOTEST_RPC_CHN_RESUME,        sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_resume},
    {VOTEST_RPC_CHN_SHOW,          sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_show},
    {VOTEST_RPC_CHN_HIDE,          sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_hide},
    {VOTEST_RPC_CHN_STEP,          sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_step},
    {VOTEST_RPC_LAYER_DUMP_FRAME,  sizeof(VOTEST_RPC_LAYER_DUMP),   votest_rpc_layer_dump_frame},
    {VOTEST_RPC_DEV_ENABLE,        sizeof(ENUM_AR_HAL_VO_DEV_ID),   votest_rpc_dev_enable},
    {VOTEST_RPC_DEV_DISABLE,       sizeof(ENUM_AR_HAL_VO_DEV_ID),   votest_rpc_dev_disable},
    {VOTEST_RPC_LAYER_ENABLE,      sizeof(VOTEST_RPC_LAYER),        votest_rpc_layer_enable},
    {VOTEST_RPC_LAYER_DISABLE,     sizeof(VOTEST_RPC_LAYER),        votest_rpc_layer_disable},
    {VOTEST_RPC_CHN_ENABLE,        sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_enable},
    {VOTEST_RPC_CHN_DISABLE,       sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_disable},
    {VOTEST_RPC_CHN_STEP,          sizeof(VOTEST_RPC_CHANNEL),      votest_rpc_chn_step},
    {VOTEST_RPC_SET_DEV_ATTR,      sizeof(VOTEST_RPC_DEV_ATTR),     votest_rpc_set_dev_attr},
    {VOTEST_RPC_SET_LAYER_ATTR,    sizeof(VOTEST_RPC_LAYER_ATTR),   votest_rpc_set_layer_attr},
    {VOTEST_RPC_SET_CHN_ATTR,      sizeof(VOTEST_RPC_CHN_ATTR),     votest_rpc_set_chn_attr},
};

static AR_S32 votest_rpc_chn_pause(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_pause(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("pause chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

   End:
    ar_hal_vo_fd_close(fd);
    return ret;
}

static AR_S32 votest_rpc_chn_resume(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_resume(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("resume chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

   End:
    ar_hal_vo_fd_close(fd);
    return ret;
}


static AR_S32 votest_rpc_chn_show(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_show(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("show chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

   End:
    ar_hal_vo_fd_close(fd);
    return ret;
}


static AR_S32 votest_rpc_chn_hide(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_hide(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("resume chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

   End:
    ar_hal_vo_fd_close(fd);
    return ret;
}

static AR_S32 votest_rpc_chn_step(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_step(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("step chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

   End:
    ar_hal_vo_fd_close(fd);
    return ret;
}

static AR_S32 votest_rpc_layer_dump_frame(void * args, void * reply)
{
    VOTEST_RPC_LAYER_DUMP   *pstLayer = (VOTEST_RPC_LAYER_DUMP *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;
    STRU_AR_HAL_VO_DISP_BUF   buf = {0};
    int                       file_fd = -1;
    AR_VOID                  *va;

    printf("%s: layer_%d %ld\r\n", __FUNCTION__, pstLayer->eLayerId, sizeof(VOTEST_RPC_LAYER_DUMP));
    fd = ar_hal_vo_layer_open(pstLayer->eLayerId);
    if (fd < 0)
    {
        printf("open layer_%d failed\n", pstLayer->eLayerId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_layer_get_frame(fd, &buf, 300);
    if (ret != HAL_NO_ERROR)
    {
        printf("get layer_%d frame failed\n", pstLayer->eLayerId);
        ret = -1;
        goto End;
    }

    printf("get frame=%p success, dump to file %s\r\n", buf.pannel[0].buffer_pa, pstLayer->sFileName);
    file_fd = open(pstLayer->sFileName,O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
    if (file_fd < 0)
    {
        printf("open %s failed\n", pstLayer->sFileName);
        ret = -1;
        goto End;
    }

    for(int i=0;i<buf.pannel_num;i++)
    {
        va = ar_hal_sys_mmap((AR_U64)buf.pannel[i].buffer_pa, buf.pannel[i].length);
        printf("%d map pa=%p to va=%p length=%d", i, buf.pannel[i].buffer_pa, va, buf.pannel[i].length);
        write(file_fd,  va, buf.pannel[i].length);
        ar_hal_sys_munmap(va, buf.pannel[i].length);
    }

    ret = ar_hal_vo_layer_release_frame(fd, &buf, 300);
    if (ret != HAL_NO_ERROR)
    {
        printf("release layer_%d frame failed\n", pstLayer->eLayerId);
        ret = -1;
        goto End;
    }

   End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    if (file_fd >= 0)
        close(file_fd);

    return ret;
}

static AR_S32 votest_rpc_dev_enable(void * args, void * reply)
{
    ENUM_AR_HAL_VO_DEV_ID    *DevId = (ENUM_AR_HAL_VO_DEV_ID *)args;
    AR_S32                   ret = HAL_NO_ERROR;
    AR_S32                   fd = -1;

    printf("%s: devId:%d\n", __FUNCTION__, *DevId);

    fd = ar_hal_vo_dev_open(*DevId);
    if (fd < 0)
    {
        printf("open devId_%d failed\n", *DevId);
        return -1;
    }

    ret = ar_hal_vo_dev_enable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("enable dev_%d failed\n", *DevId);
        ret = -1;
    }

    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

static AR_S32 votest_rpc_dev_disable(void * args, void * reply)
{
    ENUM_AR_HAL_VO_DEV_ID    *DevId = (ENUM_AR_HAL_VO_DEV_ID *)args;
    AR_S32                   ret = HAL_NO_ERROR;
    AR_S32                   fd = -1;

    printf("%s: devId:%d\n", __FUNCTION__, *DevId);

    fd = ar_hal_vo_dev_open(*DevId);
    if (fd < 0)
    {
        printf("open devId_%d failed\n", *DevId);
        return -1;
    }

    ret = ar_hal_vo_dev_disable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("enable dev_%d failed\n", *DevId);
        ret = -1;
    }

    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

static AR_S32 votest_rpc_layer_enable(void * args, void * reply)
{
    VOTEST_RPC_LAYER   *pstLayer = (VOTEST_RPC_LAYER *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: dev_%d layer_%d %ld\r\n", __FUNCTION__, pstLayer->eDevId, pstLayer->eLayerId, sizeof(VOTEST_RPC_LAYER_DUMP));
    fd = ar_hal_vo_layer_open(pstLayer->eLayerId);
    if (fd < 0)
    {
        printf("open layer_%d failed\n", pstLayer->eLayerId);
        return -1;
    }

    ret = ar_hal_vo_layer_enable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("disable dev_%d layer_%d frame failed\n", pstLayer->eDevId, pstLayer->eLayerId);
    }

    if (fd >= 0)
        ar_hal_vo_fd_close(fd);

    return ret;
}
static AR_S32 votest_rpc_layer_disable(void * args, void * reply)
{
    VOTEST_RPC_LAYER   *pstLayer = (VOTEST_RPC_LAYER *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    printf("%s: dev_%d layer_%d %ld\r\n", __FUNCTION__, pstLayer->eDevId, pstLayer->eLayerId, sizeof(VOTEST_RPC_LAYER_DUMP));
    fd = ar_hal_vo_layer_open(pstLayer->eLayerId);
    if (fd < 0)
    {
        printf("open layer_%d failed\n", pstLayer->eLayerId);
        return -1;
    }

    ret = ar_hal_vo_layer_disable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("disable dev_%d layer_%d frame failed\n", pstLayer->eDevId, pstLayer->eLayerId);
    }

    if (fd >= 0)
        ar_hal_vo_fd_close(fd);

    return ret;
}
static AR_S32 votest_rpc_chn_enable(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32              ret = HAL_NO_ERROR;
    AR_S32              fd = -1;

    if(cmd_is_running[pstChan->eLayerId][pstChan->u32ChanId])
    {
        printf("%s: chan_%d_%d_%d is enable\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        return -1;
    }

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);

    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_enable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("enable chan_%d_%d_%d failed, ret:%d\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId, ret);
        ret = -1;
        goto End;
    }

    cmd_is_running[pstChan->eLayerId][pstChan->u32ChanId] = AR_TRUE;
    cfg_vo->stVoChn[pstChan->u32ChanId].enable = AR_TRUE;
    printf("enable chan_%d_%d_%d success!\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
End:
    ar_hal_vo_fd_close(fd);
    return ret;
}

static AR_S32 votest_rpc_chn_disable(void * args, void * reply)
{
    VOTEST_RPC_CHANNEL      *pstChan = (VOTEST_RPC_CHANNEL *)args;
    AR_S32                  ret = HAL_NO_ERROR;
    AR_S32                  fd = -1;

    if(!cmd_is_running[pstChan->eLayerId][pstChan->u32ChanId])
    {
        printf("%s: chan_%d_%d_%d is disable\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        return -1;
    }

    printf("%s: chan_%d_%d_%d\n", __FUNCTION__, pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->eLayerId, pstChan->u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);
        ret = -1;
        goto End;
    }

    cmd_is_running[pstChan->eLayerId][pstChan->u32ChanId] = AR_FALSE;
    cfg_vo->stVoChn[pstChan->u32ChanId].enable = AR_FALSE;

    /* pause the channel */
    ret = ar_hal_vo_chn_pause(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("pause chan_%d_%d_%d failed, ret:%d\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId, ret);
        goto End;
    }

    recyle_channel_buffer(pstChan->eLayerId, pstChan->u32ChanId);

    ret = ar_hal_vo_chn_disable(fd);
    if (ret != HAL_NO_ERROR)
    {
        printf("disable chan_%d_%d_%d failed, ret:%d\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId, ret);
        ret = -1;
        goto End;
    }

    printf("disable chan_%d_%d_%d success!\n", pstChan->eDevId, pstChan->eLayerId, pstChan->u32ChanId);

End:
    ar_hal_vo_fd_close(fd);
    return ret;
}

static AR_S32 votest_rpc_set_dev_attr(void * args, void * reply)
{
    VOTEST_RPC_DEV_ATTR      *prpcDevattr = (VOTEST_RPC_DEV_ATTR *)args;
    AR_S32                   ret = HAL_NO_ERROR;
    AR_S32                   fd = -1;
    STRU_AR_HAL_VO_DEV_ATTR  sDevAttr = prpcDevattr->sDevAttr;
    ENUM_AR_HAL_VO_CSC       eDevCsc = prpcDevattr->eDevCsc;

    printf("%s: devId:%d\n", __FUNCTION__, prpcDevattr->eDevId);

    fd = ar_hal_vo_dev_open(prpcDevattr->eDevId);
    if (fd < 0)
    {
        printf("open devId_%d failed\n", prpcDevattr->eDevId);
        return -1;
    }

    ret = ar_hal_vo_dev_set_attr(fd, &sDevAttr);
    if (ret != HAL_NO_ERROR)
    {
        printf("set devId_%d attr failed\n", prpcDevattr->eDevId);
        goto End;
    }

    ret = ar_hal_vo_dev_set_csc(fd, &eDevCsc);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("set devId_%d dev_csc failed\n", prpcDevattr->eDevId);
    }

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

static AR_S32 votest_rpc_set_layer_attr(void * args, void * reply)
{
    VOTEST_RPC_LAYER_ATTR     *pstLayer = (VOTEST_RPC_LAYER_ATTR *)args;
    AR_S32                    ret = HAL_NO_ERROR;
    AR_S32                    fd = -1;
    STRU_AR_HAL_VO_LAYER_ATTR sLayerAttr = pstLayer->sLayerAttr;
    STRU_AR_HAL_VO_LAYER_CSC  sLayerCsc = pstLayer->sLayerCsc;
    STRU_AR_HAL_VO_POS        sLayerPos = pstLayer->sLayerPos;

    printf("%s: layer_%d %ld\r\n", __FUNCTION__, pstLayer->eLayerId, sizeof(VOTEST_RPC_LAYER_ATTR));
    fd = ar_hal_vo_layer_open(pstLayer->eLayerId);
    if (fd < 0)
    {
        printf("open layer_%d failed\n", pstLayer->eLayerId);
        return -1;
    }

    ret = ar_hal_vo_layer_set_attr(fd, &sLayerAttr);
    if (ret != HAL_NO_ERROR)
    {
        printf("set devId_%d layer_%d failed\n", pstLayer->eDevId, pstLayer->eLayerId);
        goto End;
    }

    ret = ar_hal_vo_layer_set_csc(fd, &sLayerCsc);
    if (ret != AR_HAL_VO_SUCCESS)
    {
        printf("set devId_%d layer_%d layer_csc failed\n", pstLayer->eDevId, pstLayer->eLayerId);
        goto End;
    }

    if(pstLayer->eLayerId > 0)
    {
        ret = ar_hal_vo_layer_set_pos(fd, &sLayerPos);
        if (ret != AR_HAL_VO_SUCCESS)
        {
            printf("set devId_%d layer_%d pos failed\n", pstLayer->eDevId, pstLayer->eLayerId);
        }
    }

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

static AR_S32 votest_rpc_set_chn_attr(void * args, void * reply)
{
    VOTEST_RPC_CHN_ATTR     *pstChan = (VOTEST_RPC_CHN_ATTR *)args;
    AR_S32                  ret = HAL_NO_ERROR;
    AR_S32                  fd = -1;
    STRU_AR_VO_CHN_PARAM_S  sChnPara = pstChan->sChnPara;
    STRU_AR_HAL_VO_CHN_ATTR sChnAttr = pstChan->sChnAttr;

    printf("%s: chan_%d_%d_%u\n", __FUNCTION__, pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId);
    fd = ar_hal_vo_chn_open(pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId);
    if (fd < 0)
    {
        printf("open chan_%d_%d_%d failed\n", pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId);
        ret = -1;
        goto End;
    }

    ret = ar_hal_vo_chn_set_attr(fd, &sChnAttr);
    if (ret != HAL_NO_ERROR)
    {
        printf("set chan_%d_%d_%u attr failed, ret:%d\n", pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId, ret);
        goto End;
    }
    printf("set chan_%d_%d_%d attr success!\n", pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId);

    ret = ar_hal_vo_chn_set_para(fd, &sChnPara);
    if (ret != HAL_NO_ERROR)
    {
        printf("set chan_%d_%d_%u para failed, ret:%d\n", pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId, ret);
        goto End;
    }
    printf("set chan_%d_%d_%u para success!\n", pstChan->sChn.eDevId, pstChan->sChn.eLayerId, pstChan->sChn.u32ChanId);

End:
    if (fd >= 0)
       ar_hal_vo_fd_close(fd);

    return ret;
}

static AR_S32 votest_service_handler(uint32_t cmd, ar_ipc_data_t * data, unsigned len, void * reply)
{
    AR_S32 i = 0;

    printf("cmd:%d data:%p len:%d\n", cmd, data, len);
    for(i = 0; i < sizeof(votest_rpc_table)/sizeof(votest_rpc_table[0]); i++)
    {
        if(votest_rpc_table[i].cmd_id == cmd)
        {
            if(votest_rpc_table[i].handler && len == votest_rpc_table[i].param_len)
            {
                return votest_rpc_table[i].handler(data->blob, reply);
            }
        }
    }

    return 0;
}

void votest_rpc_init(void)
{
    AR_S32 ret = 0;

    if(votest_ipc_handle)
        return;

    votest_ipc_handle = ar_ipc_create();
    ret = ar_ipc_add_service(votest_ipc_handle, "votest_ctrl", votest_service_handler);
    if(ret)
    {
        printf("Add service error.\n");
        return;
    }

    ar_ipc_start_thread_pool(votest_ipc_handle);
    return;
}

void votest_rpc_deinit()
{
    ar_ipc_stop_thread_pool(votest_ipc_handle);
    ar_ipc_close(votest_ipc_handle);
}

void votest_rpc_reply(void * reply, void * out, AR_S32 out_len)
{
    uint32_t state = 0;
    ar_ipc_return_buffer(reply, state, out, out_len);
}

