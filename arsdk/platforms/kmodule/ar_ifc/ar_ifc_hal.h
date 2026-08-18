#ifndef __ARIFC_HAL_H__
#define __ARIFC_HAL_H__
#include <linux/miscdevice.h>
#include "ar_ifc.h"

#ifdef AR9341
#define AR_IFC_BASE_ADDR   0x01094000
#define AR_IFC_REG_SPACE_RANGE   0x1000
#define AR_IFC_GLB_CTRL_ADDR  0x01070000
#define AR_IFC_GLB_SPACE_RANGE   0x8000

//Global control regs
#define IFC_GLB_CTRL_REG       0x4010
#define IFC_CLK_CTRL_REG       0x003C
#else
#define AR_IFC_BASE_ADDR   0x08844000
#define AR_IFC_REG_SPACE_RANGE   0x1000
#define AR_IFC_GLB_CTRL_ADDR  0x0A100000
#define AR_IFC_GLB_SPACE_RANGE   0x8000

//Global control regs
#define IFC_GLB_CTRL_REG       0x6010
#define IFC_CLK_CTRL_REG       0x4058

#define IFC_PMU_CTRL_ACCESS    0x6200
#define IFC_PMU_CTRL_REG       0x6204
#endif

#define IFC_YUV_WIDTH_REG     (0x0)
#define IFC_YUV_HEIGHT_REG    (0x4)
#define IFC_YUV_Y_STRIDE_REG  (0x8)
#define IFC_YUV_U_STRIDE_REG  (0xC)
#define IFC_YUV_V_STRIDE_REG  (0x10)
#define IFC_YUV_Y_ADDR_REG    (0x14)
#define IFC_YUV_U_ADDR_REG    (0x18)
#define IFC_YUV_V_ADDR_REG    (0x1C)
#define IFC_YUV_FORMAT_REG    (0x20)
#define IFC_YUV_PIXEL_BIT_REG (0x24)
#define IFC_YUV_PIXEL_BYTE_REG (0x28)

#define IFC_RGB_R_STRIDE_REG  (0x2C)
#define IFC_RGB_G_STRIDE_REG  (0x30)
#define IFC_RGB_B_STRIDE_REG  (0x34)
#define IFC_RGB_R_ADDR_REG    (0x38)
#define IFC_RGB_G_ADDR_REG    (0x3C)
#define IFC_RGB_B_ADDR_REG    (0x40)
#define IFC_RGB_R_C0_REG      (0x44)
#define IFC_RGB_R_C1_REG      (0x48)
#define IFC_RGB_R_C2_REG      (0x4C)
#define IFC_RGB_R_C3_REG      (0x50)
#define IFC_RGB_G_C0_REG      (0x54)
#define IFC_RGB_G_C1_REG      (0x58)
#define IFC_RGB_G_C2_REG      (0x5C)
#define IFC_RGB_G_C3_REG      (0x60)
#define IFC_RGB_B_C0_REG      (0x64)
#define IFC_RGB_B_C1_REG      (0x68)
#define IFC_RGB_B_C2_REG      (0x6C)
#define IFC_RGB_B_C3_REG      (0x70)
#define IFC_RGB_BITS_SHIFT_REG (0x74)
#define IFC_RGB_MIN_REG       (0x78)
#define IFC_RGB_MAX_REG       (0x7C)
#define IFC_RGB_AVG_R_REG     (0x80)
#define IFC_RGB_AVG_G_REG     (0x84)
#define IFC_RGB_AVG_B_REG     (0x88)
#define IFC_RGB_FORMAT_REG    (0x8C)

#define IFC_CTRL_AUTO_FLAG_REG      (0x90)
#define IFC_CTRL_AUTO_ADDR_REG      (0x94)
#define IFC_CTRL_AUTO_FRAME_NUM_REG (0x98)
#define IFC_CTRL_AUTO_RESULT_ADDR_REG (0x9C)
#define IFC_CTRL_IRQ_MODE_REG       (0xA0)
#define IFC_CTRL_AXI_CFG_REG        (0xA4)
#define IFC_CTRL_AXI_OVFL_CNT_REG   (0xA8)
#define IFC_CTRL_START_REG          (0xAC)

#define IFC_RESULT_SUM_RH_REG         (0xB0)
#define IFC_RESULT_SUM_RL_REG         (0xB4)
#define IFC_RESULT_SUM_GH_REG         (0xB8)
#define IFC_RESULT_SUM_GL_REG         (0xBC)
#define IFC_RESULT_SUM_BH_REG         (0xC0)
#define IFC_RESULT_SUM_BL_REG         (0xC4)
#define IFC_RESULT_SSUM_RH_REG        (0xC8)
#define IFC_RESULT_SSUM_RL_REG        (0xCC)
#define IFC_RESULT_SSUM_GH_REG        (0xD0)
#define IFC_RESULT_SSUM_GL_REG        (0xD4)
#define IFC_RESULT_SSUM_BH_REG        (0xD8)
#define IFC_RESULT_SSUM_BL_REG        (0xDC)
#define IFC_RESULT_FRAME_INDEX_REG    (0xE0)
#define IFC_RESULT_STATE_REG          (0xE4)

//Values
#define IFC_YUV_FORMAT_MASK        0x0000000F
#define IFC_YUV_FORMAT_420         0
#define IFC_YUV_FORMAT_422         1
#define IFC_YUV_FORMAT_444         2

#define IFC_YUV_PLANAR_MASK        0x000000F0
#define IFC_YUV_PLANAR             0
#define IFC_YUV_SEMI_PLANAR        1

#define IFC_YUV_PIXEL_BYTE_MASK    0x0000000F
#define IFC_YUV_PIXEL_BYTE_1P1B    0
#define IFC_YUV_PIXEL_BYTE_1P2B    1
#define IFC_YUV_PIXEL_BYTE_3P4B    2
#define IFC_YUV_PIXEL_BYTE_21P16B  3

#define IFC_RGB_FORMAT_MASK       0x0000000F
#define IFC_RGB_FORMAT_PLANAR     0
#define IFC_RGB_FORMAT_INTLVED    1
#define IFC_RGB_FORMAT_RGB0       3

#define IFC_IRQ_ENABLE_MASK       0x1
#define IFC_IRQ_FRAME_DONE_MASK   0x2

#define IFC_STATE_STOP             0
#define IFC_STATE_WORKING          1
#define IFC_STATE_AXI_ERROR       (1<<2)
#define IFC_STATE_AXI_WR_WORKING  (1<<18)
#define IFC_STATE_AXI_RD_WORKING  (1<<19)

#define IFC_STATE_FINISH_OK     0x1234
#define IFC_STATE_FINISH_FAIL   (-1)

#define IFC_DEFAULT_TIMEOUT   1000 //1s

struct ar_ifc_dev
{
    void __iomem * base;
    void __iomem * global_ctrl;
    unsigned int frequency;
    struct miscdevice miscdev;
    unsigned int irq;
    struct proc_dir_entry * proc_dir;
    struct completion done;
    struct mutex lock;
    unsigned int suspended;
};

void _ar_ifc_poweron(struct ar_ifc_dev * pdev);
void _ar_ifc_poweroff(struct ar_ifc_dev * pdev);
irqreturn_t _ar_ifc_irq_handler(int irq, void * dev);
int _ar_ifc_start_single(struct ar_ifc_dev * pdev, ar_ifc_params_st * ioctl_params);
int _ar_ifc_start_auto(struct ar_ifc_dev * pdev, ar_ifc_ioctl_auto_st * ioctl_params);
void _ar_ifc_calculate_result(ar_ifc_result_s * ifc_result);
int _ar_ifc_set_cache_coherency(unsigned int cache_enable);
int _ar_ifc_set_security(unsigned int security);
int _ar_ifc_set_frequency(unsigned int frequency);
int ar_ifc_proc_state_show(struct seq_file *s, void *p);
#endif