#include <cpu_func.h>
#include "vo/util.h"
#include "vo/display_core/display_type.h"
#include "vo/display_hal/display_rpc.h"

int ar_vo_dev_enable(uint32_t dev_id,
                     uint32_t intf_type,
                     uint32_t sub_intf_type,
                     uint32_t sync_type)
{
    STRU_AR_HAL_VO_DEV_ATTR dev_attr;
    int ret;

    dev_attr.bg_color = 0x808080;
    dev_attr.e_interface = intf_type;
    if (dev_attr.e_interface == AR_HAL_VO_DEV_INTF_MIPI)
    {
        dev_attr.u_sub_intance.e_mipi_pannel = sub_intf_type;
        if (sync_type == 2)
        {
            dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_400x960_60;
        }
        else if (sync_type == 3)
        {
            dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_720x1440_60;
        }
    }
    else if (dev_attr.e_interface == AR_HAL_VO_DEV_INTF_DVP)
    {
        dev_attr.u_sub_intance.e_sub_intf = sub_intf_type;
    }
    // dev_attr.e_timing_template = AR_HAL_VO_OUTPUT_1024x600_60;
    dev_attr.bit_count_per_channel = 8;
    ret = ar_vo_rpc_dev_set_attr(dev_id, &dev_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_vo_rpc_dev_enable(dev_id);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

End:
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x\r\n", ret);

    return ret;
}

int ar_vo_dev_disable(uint32_t dev_id)
{
    return ar_vo_rpc_dev_disable(dev_id);
}

int ar_vo_layer_enable(uint32_t layer_id, uint32_t addr, uint32_t stride,
                       uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t csc)
{
    STRU_AR_HAL_VO_LAYER_ATTR layer_attr;
    int ret;
    display_buffer_t buffer = {0};
    STRU_AR_HAL_VO_POS pos = {0};
    STRU_AR_HAL_VO_LAYER_CSC layer_csc = {0};

    ar_debug("ar_vo_layer_enable: layer_id=%d addr=0x%x stride=%d x=%d y=%d w=%d h=%d csc=%d\n",
             layer_id, addr, stride, x, y, w, h, csc);

    /* init vo layer */
    layer_attr.width = w;
    layer_attr.height = h;
    layer_attr.luma_stride = stride;
    layer_attr.chroma_stride = stride / 2;
    layer_attr.format = AR_HAL_VO_FMT_YV12;
    layer_attr.fps = 30;
    ret = ar_vo_rpc_layer_set_attr(layer_id, &layer_attr);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    layer_csc.input = (ENUM_AR_HAL_VO_CSC)csc;
    layer_csc.output = (ENUM_AR_HAL_VO_CSC)csc;
    ret = ar_vo_rpc_layer_set_csc(layer_id, &layer_csc);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    ret = ar_vo_rpc_layer_enable(layer_id);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

    // video layer(layer 0) doesn't support set position
    pos.x = x;
    pos.y = y;
    ret = ar_vo_rpc_layer_set_position(layer_id, &pos);
    if ((ret != AR_HAL_VO_SUCCESS) && (ret != HAL_ERR_VO_LAYER_ID))
        goto End;

    flush_dcache_all();

    buffer.frame_id = 0;
    buffer.panel_count = 3;
    buffer.pannel[0].buffer_pa = (void *)addr;
    buffer.pannel[1].buffer_pa = (void *)addr + stride * h;
    buffer.pannel[2].buffer_pa = (void *)addr + stride * h * 5 / 4;
    ret = ar_vo_rpc_layer_enqueue(layer_id, &buffer);
    if (ret != AR_HAL_VO_SUCCESS)
        goto End;

End:
    if (ret != AR_HAL_VO_SUCCESS)
        ar_err("ret = 0x%x\r\n", ret);

    return ret;
}

int ar_vo_layer_disable(uint32_t layer_id)
{
    return ar_vo_rpc_layer_disable(layer_id);
}

int ar_vo_init(void)
{
    ar_display_init();
    return 0;
}
