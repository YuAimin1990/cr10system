#include "vo/util.h"
#include "vo/display_core/display_type.h"
#include "vo/ar_display.h"

int ar_vo_rpc_dev_enable(ENUM_AR_HAL_VO_DEV_ID        e_dev_id)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    if (server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_HAS_ENABLED;
        goto End;
    }

    /*vo dev is attached to video layer, make sure video layer is initialized */
    disp_handler = ar_get_display_layer(AR_HAL_VO_LAYER_ID_VIDEO_0);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    ret = ar_init_display(disp_handler, AR_HAL_VO_LAYER_ID_VIDEO_0);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_display_dev_enable();
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    server->hal.vo_dev.enable = AR_TRUE;
    ar_always("vo dev init success.");

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

int ar_vo_rpc_dev_disable(ENUM_AR_HAL_VO_DEV_ID        e_dev_id)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    for (int i = 0; i < AR_HAL_VO_LAYER_ID_MAX; i++)
    {
        if (server->hal.vo_layer[i].enable)
        {
            ret = HAL_ERR_VO_LAYER_HAS_ENABLED;
            goto End;
        }
    }

    ret = ar_display_dev_disable();
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    server->hal.vo_dev.enable = AR_FALSE;
    ar_always("vo dev deinit success.");

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

int ar_vo_rpc_dev_set_attr(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, STRU_AR_HAL_VO_DEV_ATTR *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    if (server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_HAS_ENABLED;
        goto End;
    }

    server->hal.vo_dev.attr = *para;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

int ar_vo_rpc_dev_get_attr(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, STRU_AR_HAL_VO_DEV_ATTR *attr)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    *attr = server->hal.vo_dev.attr;

    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_dev_set_csc(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, ENUM_AR_HAL_VO_CSC *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    if (server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_HAS_ENABLED;
        goto End;
    }

    server->hal.vo_dev.e_csc = *para;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_dev_get_csc(ENUM_AR_HAL_VO_DEV_ID         e_dev_id, ENUM_AR_HAL_VO_CSC *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_dev_id >= AR_HAL_VO_DEV_ID_MAX)
        return HAL_ERR_VO_DEV_ID;

    ar_lock(server->hal.lock);
    *para = server->hal.vo_dev.e_csc;

    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_enable(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;
    disp_obj_t *obj = NULL;
    int luma_stride = 0;
    int chroma_stride = 0;
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    if (server->hal.vo_layer[e_layer_id].enable)
    {
        ret = HAL_ERR_VO_LAYER_HAS_ENABLED;
        goto End;
    }

    disp_handler = ar_get_display_layer(e_layer_id);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    obj = (disp_obj_t *)ar_get_display_layer(AR_HAL_VO_LAYER_ID_VIDEO_0);

    if (server->hal.vo_layer[e_layer_id].attr.luma_stride < 0)
        luma_stride = ar_disp_get_buffer_width(server->hal.vo_layer[e_layer_id].attr.width);
    else
        luma_stride = server->hal.vo_layer[e_layer_id].attr.luma_stride;

    if (server->hal.vo_layer[e_layer_id].attr.chroma_stride < 0)
        chroma_stride = ar_disp_get_buffer_width_div2(server->hal.vo_layer[e_layer_id].attr.width/2);
    else
        chroma_stride = server->hal.vo_layer[e_layer_id].attr.chroma_stride;

    /* start layer */
    ret = ar_display_layer_enable(disp_handler,
                            server->hal.vo_layer[e_layer_id].attr.width,
                            server->hal.vo_layer[e_layer_id].attr.height,
                            ar_display_hal_format_trans(server->hal.vo_layer[e_layer_id].attr.format),
                            obj->res_timing.fps,   // use vo dev fps, layer fps is used to specify surface's fps
                            luma_stride,
                            chroma_stride
                            );
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_display_set_layer_csc(disp_handler, &server->hal.vo_layer[e_layer_id].csc);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    server->hal.vo_layer[e_layer_id].enable = AR_TRUE;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_disable(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    if (!server->hal.vo_layer[e_layer_id].enable)
    {
        ret = HAL_ERR_VO_LAYER_NOT_ENABLE;
        goto End;
    }

    disp_handler = ar_get_display_layer(e_layer_id);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    for (int i=0; i<MAX_VO_CHN_NUM; i++)
    {
        if (server->hal.vo_layer[e_layer_id].chn[i].e_state != AR_HAL_VO_CHN_STATE_DISABLE)
        {
            ret = HAL_ERR_VO_CHN_EXSIT;
            goto End;
        }
    }

    ret = ar_display_layer_disable(disp_handler);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    server->hal.vo_layer[e_layer_id].enable = AR_FALSE;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_set_attr(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_LAYER_ATTR *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    if (server->hal.vo_layer[e_layer_id].enable)
    {
        ret = HAL_ERR_VO_LAYER_HAS_ENABLED;
        goto End;
    }

    server->hal.vo_layer[e_layer_id].attr = *para;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_get_attr(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_LAYER_ATTR *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    *para = server->hal.vo_layer[e_layer_id].attr;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_set_csc(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                        STRU_AR_HAL_VO_LAYER_CSC   *csc)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    disp_handler = ar_get_display_layer(e_layer_id);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    if (server->hal.vo_layer[e_layer_id].enable)
    {
        /* layer has enable and just set the parameter */
        ret = ar_display_set_layer_csc(disp_handler, csc);
        if (ret != AR_HAL_VO_SUCCESS)
            goto End;
    }

    server->hal.vo_layer[e_layer_id].csc = *csc;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_get_csc(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                        STRU_AR_HAL_VO_LAYER_CSC  *csc)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    if (!server->hal.vo_layer[e_layer_id].enable)
    {
        ret = HAL_ERR_VO_LAYER_NOT_ENABLE;
        goto End;
    }

    *csc = server->hal.vo_layer[e_layer_id].csc;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_capture_buf(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_DISP_BUF  *para)
{
    /* TODO: just for debug*/
    return 0;
}

 int ar_vo_rpc_layer_release(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_DISP_BUF  *para)
{
    /* TODO: just for debug*/
    return 0;
}

 int ar_vo_rpc_layer_set_position(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                            STRU_AR_HAL_VO_POS *para)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;
    int ret = AR_HAL_VO_SUCCESS;

    if ((e_layer_id >= AR_HAL_VO_LAYER_ID_MAX) ||
         (e_layer_id == AR_HAL_VO_LAYER_ID_VIDEO_0))
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    disp_handler = ar_get_display_layer(e_layer_id);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    if (!server->hal.vo_layer[e_layer_id].enable)
    {
        ret = HAL_ERR_VO_LAYER_NOT_ENABLE;
        goto End;
    }

    ar_display_overlay_set(disp_handler, para->x, para->y);
    server->hal.vo_layer[e_layer_id].position = *para;

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}

 int ar_vo_rpc_layer_enqueue(ENUM_AR_HAL_VO_LAYER_ID  e_layer_id,
                                                        display_buffer_t *buffer)
{
    display_server_t *server = (display_server_t *)get_display_dev();
    display_handle_t *disp_handler = NULL;

    int ret = AR_HAL_VO_SUCCESS;

    if (e_layer_id >= AR_HAL_VO_LAYER_ID_MAX)
        return HAL_ERR_VO_LAYER_ID;

    ar_lock(server->hal.lock);
    if (!server->hal.vo_dev.enable)
    {
        ret = HAL_ERR_VO_DEV_NOT_ENABLE;
        goto End;
    }

    disp_handler = ar_get_display_layer(e_layer_id);
    if (disp_handler == NULL)
    {
        ret = HAL_ERR_VO_LAYER_ID;
        goto End;
    }

    ret = ar_display_enqueue(disp_handler, buffer);

End:
    ar_unlock(server->hal.lock);
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x", ret);
    return ret;
}


