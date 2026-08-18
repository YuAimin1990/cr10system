//This is the HAL for IFC
#include <asm/barrier.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>

#include "ar_ifc.h"
#include "ar_ifc_hal.h"

static unsigned int g_ifc_status = 0;

extern struct ar_ifc_dev ifc_dev;

void _ar_ifc_poweron(struct ar_ifc_dev * pdev)
{
    unsigned int val = 0;

    //Freq, tmp set to 500M
    //val = readl(pdev->global_ctrl + IFC_CLK_CTRL_REG);
    //val &= (~(0xF<<16)); //bit 19:16
    //val |= (0x3 << 16);
    //writel(val, pdev->global_ctrl + IFC_CLK_CTRL_REG);
    _ar_ifc_set_frequency(ifc_dev.frequency);
#ifdef AR9341  
    //Assert reset
    val = readl(pdev->global_ctrl + IFC_GLB_CTRL_REG);
    val &= (~(1<<20));
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);
    udelay(1000);

    val |= (1 << 20) | (1<<13); //bit14 clk en
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);
    udelay(1000);

    //enable internal sram
    writel(0, pdev->base + IFC_CTRL_AXI_CFG_REG);
#else  
    //Assert reset  
    val = readl(pdev->global_ctrl + IFC_GLB_CTRL_REG);
    val &= (~(1<<9));
    val &= (~(1<<11));
    val &= (~(1<<13));
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);
    udelay(100);

    //power on vision
    val = readl(ifc_dev.global_ctrl + IFC_PMU_CTRL_REG);
    val &= ~(1<<19); //iso off
    val |= (1<<3); //power on
    val |= (1<<11);
    writel(0xACCE55, ifc_dev.global_ctrl + IFC_PMU_CTRL_ACCESS);
    writel(val, ifc_dev.global_ctrl + IFC_PMU_CTRL_REG);
    msleep(5);
    
    //release reset 
    val = readl(pdev->global_ctrl + IFC_GLB_CTRL_REG);
    val |= (1 << 9);
    val |= (1 << 11);
    val |= (1 << 13);
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);
    udelay(100);

    //enable internal sram
    val = 0x0;
    val |= (1<<21);
    val |= (1<<20);
    writel(val, pdev->base + IFC_CTRL_AXI_CFG_REG);
#endif

    //enable irq
    val = readl(pdev->base + IFC_CTRL_IRQ_MODE_REG);
    val &= ~IFC_IRQ_ENABLE_MASK; //clear bit 0
    writel(val, pdev->base + IFC_CTRL_IRQ_MODE_REG);
}

void _ar_ifc_poweroff(struct ar_ifc_dev * pdev)
{
    unsigned int val = 0;

    printk(KERN_INFO"Power off IFC\n");

    //clk off and reset
#ifdef AR9341  
    //disable sram
    val = 0x0;
    val |= (1<<19);
    writel(val, pdev->base + IFC_CTRL_AXI_CFG_REG);

    //clock off
    val = readl(pdev->global_ctrl + IFC_GLB_CTRL_REG);
    val &= (~(1<<20));
    val &= (~(1<<13));
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);    
#else  
    //disable internal sram
    val = 0x0;
    val |= (1<<19);
    writel(val, pdev->base + IFC_CTRL_AXI_CFG_REG);

    //clock off  
    val = readl(pdev->global_ctrl + IFC_GLB_CTRL_REG);
    val &= (~(1<<9));
    val &= (~(1<<11));
    val &= (~(1<<13));
    writel(val, pdev->global_ctrl + IFC_GLB_CTRL_REG);
#endif
    
}

irqreturn_t _ar_ifc_irq_handler(int irq, void * dev)
{
    unsigned int val = 0;

    struct ar_ifc_dev * pdev = (struct ar_ifc_dev * )dev;
    //Reading this irq status clears irq automatically
    val = readl(pdev->base + IFC_RESULT_STATE_REG);
    if(val & IFC_STATE_AXI_ERROR)
    {
        printk("IFC err: 0x%x, AXI error!\n", val);
        g_ifc_status = IFC_STATE_FINISH_FAIL;
    }
    else
    {
        g_ifc_status = IFC_STATE_FINISH_OK;
    }

    mb();//Add barrier to make sure status is updated.

    complete(&pdev->done);

    return IRQ_HANDLED;
}

int _ar_ifc_start_single(struct ar_ifc_dev * pdev, ar_ifc_params_st * params)
{
    int ret = 0;

#if 0    
    if(params->y_addr < 0x20000000)
    {
        printk("ERR: IFC Y addr is invalid: [0x%x]!\n", params->y_addr);
        return -EFAULT;
    }
    if(params->u_addr < 0x20000000)
    {
        printk("ERR: IFC U addr is invalid: [0x%x]!\n", params->u_addr);
        return -EFAULT;
    }
    if((params->yuv_format & 0xF0) == 0)
    {
        if(params->v_addr < 0x20000000)
        {
            printk("ERR: IFC V addr is invalid: [0x%x]!\n", params->v_addr);
            return -EFAULT;
        }
    }
    if(params->r_addr < 0x20000000)
    {
        printk("ERR: IFC R addr is invalid: [0x%x]!\n", params->r_addr);
        return -EFAULT;
    }
    if((params->rgb_format & 0x0F) == 0)
    {
        if(params->g_addr < 0x20000000)
        {
            printk("ERR: IFC G addr is invalid: [0x%x], with format 0x%x!\n", params->g_addr, params->rgb_format);
            return -EFAULT;
        }
        if(params->b_addr < 0x20000000)
        {
            printk("ERR: IFC B addr is invalid: [0x%x], with format 0x%x!\n", params->b_addr, params->rgb_format);
            return -EFAULT;
        }
    }

#endif
#ifdef AR9341
	 if(params->rgb_format == 0)
     {
        if(params->yuv_format == 2) // yuv444
        {
	        if(params->y_stride % 256 != 0 || params->u_stride % 256 != 0 || params->v_stride % 256 != 0
	        || params->r_stride % 256 != 0 || params->g_stride % 256 != 0 || params->b_stride % 256 != 0)
	        {
	            printk("stride should 256-aligned with this R/G/B planar format.\n");
	            return -EFAULT;
	        }
        }
		else  // yuv420,yuv422
		{
			if(params->y_stride % 256 != 0 || params->u_stride % 128 != 0 || params->v_stride % 128 != 0
	        || params->r_stride % 256 != 0 || params->g_stride % 256 != 0 || params->b_stride % 256 != 0)
	        {
	            printk("stride should 128/256-aligned with this R/G/B planar format.\n");
	            return -EFAULT;
			}

		}
     }
#endif

    ret = mutex_lock_interruptible(&pdev->lock);
    if(ret)
    {
        printk(KERN_ERR"_ar_ifc_start_single acquire lock error\n!");
        return -ENOTBLK;
    }

    if(pdev->suspended)
    {
        mutex_unlock(&pdev->lock);
        return -EAGAIN;
    }
    
    writel(params->yuv_width, pdev->base + IFC_YUV_WIDTH_REG);
    writel(params->yuv_height, pdev->base + IFC_YUV_HEIGHT_REG);
    writel(params->y_stride, pdev->base + IFC_YUV_Y_STRIDE_REG);
    writel(params->u_stride, pdev->base + IFC_YUV_U_STRIDE_REG);
    writel(params->v_stride, pdev->base + IFC_YUV_V_STRIDE_REG);
    writel(params->y_addr, pdev->base + IFC_YUV_Y_ADDR_REG);
    writel(params->u_addr, pdev->base + IFC_YUV_U_ADDR_REG);
    writel(params->v_addr, pdev->base + IFC_YUV_V_ADDR_REG);
    writel(params->yuv_format, pdev->base + IFC_YUV_FORMAT_REG);
    writel(params->yuv_pixel_bit, pdev->base + IFC_YUV_PIXEL_BIT_REG);
    writel(params->yuv_pixel_byte, pdev->base + IFC_YUV_PIXEL_BYTE_REG);

    writel(params->r_stride, pdev->base + IFC_RGB_R_STRIDE_REG);
    writel(params->g_stride, pdev->base + IFC_RGB_G_STRIDE_REG);
    writel(params->b_stride, pdev->base + IFC_RGB_B_STRIDE_REG);
    writel(params->r_addr, pdev->base + IFC_RGB_R_ADDR_REG);
    writel(params->g_addr, pdev->base + IFC_RGB_G_ADDR_REG);
    writel(params->b_addr, pdev->base + IFC_RGB_B_ADDR_REG);
    writel(params->r_c0, pdev->base + IFC_RGB_R_C0_REG);
    writel(params->r_c1, pdev->base + IFC_RGB_R_C1_REG);
    writel(params->r_c2, pdev->base + IFC_RGB_R_C2_REG);
    writel(params->r_c3, pdev->base + IFC_RGB_R_C3_REG);
    writel(params->g_c0, pdev->base + IFC_RGB_G_C0_REG);
    writel(params->g_c1, pdev->base + IFC_RGB_G_C1_REG);
    writel(params->g_c2, pdev->base + IFC_RGB_G_C2_REG);
    writel(params->g_c3, pdev->base + IFC_RGB_G_C3_REG);
    writel(params->b_c0, pdev->base + IFC_RGB_B_C0_REG);
    writel(params->b_c1, pdev->base + IFC_RGB_B_C1_REG);
    writel(params->b_c2, pdev->base + IFC_RGB_B_C2_REG);
    writel(params->b_c3, pdev->base + IFC_RGB_B_C3_REG);
    writel(params->rgb_bits_shift, pdev->base + IFC_RGB_BITS_SHIFT_REG);
    writel(params->rgb_min, pdev->base + IFC_RGB_MIN_REG);
    writel(params->rgb_max, pdev->base + IFC_RGB_MAX_REG);
    writel(params->r_avg, pdev->base + IFC_RGB_AVG_R_REG);
    writel(params->g_avg, pdev->base + IFC_RGB_AVG_G_REG);
    writel(params->b_avg, pdev->base + IFC_RGB_AVG_B_REG);
    writel(params->rgb_format, pdev->base + IFC_RGB_FORMAT_REG);

    g_ifc_status = 0;

    writel(1, pdev->base + IFC_CTRL_START_REG);

    //wait for completion
    ret = wait_for_completion_interruptible_timeout(&pdev->done, msecs_to_jiffies(IFC_DEFAULT_TIMEOUT));
    mutex_unlock(&pdev->lock);

    if(ret == 0)
    {
        printk("IFC err: timeout in single mode!\n");
        return -ETIME;
    }

    if(ret < 0)
    {
        printk("IFC err: wait-done failed in single mode.\n");
        return -EINTR;
    }

    return g_ifc_status;
}

//by default, irq is raised when the last frame is done.
int _ar_ifc_start_auto(struct ar_ifc_dev * pdev, ar_ifc_ioctl_auto_st * params)
{
    int ret = 0;

    if(params->params_addr_phy < 0x20000000)
    {
        printk("ERR: IFC auto start addr is invalid: [0x%x]!\n", params->params_addr_phy);
        return -EFAULT;
    }
    if(params->result_addr_phy < 0x20000000)
    {
        printk("ERR: IFC auto result addr is invalid: [0x%x]!\n", params->result_addr_phy);
        return -EFAULT;
    }

    ret = mutex_lock_interruptible(&pdev->lock);
    if(ret)
    {
        printk(KERN_ERR"_ar_ifc_start_auto acquire lock error\n");
        return -ENOTBLK;
    }

    if(pdev->suspended)
    {
        mutex_unlock(&pdev->lock);
        return -EAGAIN;
    }
    
    writel(params->params_addr_phy, ifc_dev.base + IFC_CTRL_AUTO_ADDR_REG);
    writel(params->frame_num, ifc_dev.base + IFC_CTRL_AUTO_FRAME_NUM_REG);
    writel(params->result_addr_phy, ifc_dev.base + IFC_CTRL_AUTO_RESULT_ADDR_REG);
    writel(3, ifc_dev.base + IFC_CTRL_AUTO_FLAG_REG);

    g_ifc_status = 0;

    writel(1, ifc_dev.base + IFC_CTRL_START_REG);

    //wait for completion
    ret = wait_for_completion_interruptible_timeout(&pdev->done, msecs_to_jiffies(IFC_DEFAULT_TIMEOUT));
    mutex_unlock(&pdev->lock);

    if(ret == 0)
    {
        printk("IFC err: timeout in auto mode!\n");
        return -ETIME;
    }

    if(ret < 0)
    {
        printk("IFC err: wait-done failed in auto mode.\n");
        return -EINTR;
    }

    return g_ifc_status;
}

void _ar_ifc_calculate_result(ar_ifc_result_s * ifc_result)
{
    void __iomem * base = ifc_dev.base;

    mutex_lock(&ifc_dev.lock);
    if(ifc_dev.suspended)
    {
        mutex_unlock(&ifc_dev.lock);
        return -EAGAIN;
    }
    
    ifc_result->sum_rh = readl(base + IFC_RESULT_SUM_RH_REG);
    ifc_result->sum_rl = readl(base + IFC_RESULT_SUM_RL_REG);
    ifc_result->sum_gh = readl(base + IFC_RESULT_SUM_GH_REG);
    ifc_result->sum_gl = readl(base + IFC_RESULT_SUM_GL_REG);
    ifc_result->sum_bh = readl(base + IFC_RESULT_SUM_BH_REG);
    ifc_result->sum_bl = readl(base + IFC_RESULT_SUM_BL_REG);
    ifc_result->ssum_rh = readl(base + IFC_RESULT_SSUM_RH_REG);
    ifc_result->ssum_rl = readl(base + IFC_RESULT_SSUM_RL_REG);
    ifc_result->ssum_gh = readl(base + IFC_RESULT_SSUM_GH_REG);
    ifc_result->ssum_gl = readl(base + IFC_RESULT_SSUM_GL_REG);
    ifc_result->ssum_bh = readl(base + IFC_RESULT_SSUM_BH_REG);
    ifc_result->ssum_bl = readl(base + IFC_RESULT_SSUM_BL_REG);
    ifc_result->frame_index = readl(base +IFC_RESULT_FRAME_INDEX_REG);
    mutex_unlock(&ifc_dev.lock);
}


int _ar_ifc_set_cache_coherency(unsigned int cache_enable)
{
    void __iomem * base = ifc_dev.base;
    unsigned int val = 0;

    mutex_lock(&ifc_dev.lock);
    if(ifc_dev.suspended)
    {
        mutex_unlock(&ifc_dev.lock);
        return -EAGAIN;
    }
    
    if(cache_enable)
    {
        val = readl(base + 0xa4);
        val |= 0xF;
        writel(val, base + 0xa4);
    }
    else
    {
        val = readl(base + 0xa4);
        val &= 0xFFFFFFF0;
        writel(val, base + 0xa4);
    }

    mutex_unlock(&ifc_dev.lock);
    return IFC_STATE_FINISH_OK;

}


int _ar_ifc_set_security(unsigned int security)
{
    void __iomem * base = ifc_dev.base;
    unsigned int val = 0;

    mutex_lock(&ifc_dev.lock);
    if(ifc_dev.suspended)
    {
        mutex_unlock(&ifc_dev.lock);
        return -EAGAIN;
    }

    if(security)
    {
        val = readl(base + 0xa4);
        val &= 0xFFFFFF8F;
        writel(val, base + 0xa4);
    }
    else
    {
        val = readl(base + 0xa4);
        val |= 0x70;
        writel(val, base + 0xa4);
    }

    mutex_unlock(&ifc_dev.lock);
    return IFC_STATE_FINISH_OK;
}

int _ar_ifc_set_frequency(unsigned int frequency)
{
    unsigned int frequency_bit = 0x0;
    unsigned int val = 0x0;
	
    if(frequency == 0)
    {
       return 0;
	}

    //frequency = frequency / 1000000;

    frequency = frequency > 1000 ? 1000 : frequency;
    frequency = frequency < 40 ? 40 : frequency;
#ifdef AR9341
	if(frequency>=666)
		frequency_bit = 1;
	else if(frequency>=600)
		frequency_bit = 2;
	else if(frequency>=500)
		frequency_bit = 3;
	else if(frequency>=450)
		frequency_bit = 4;
	else if(frequency>=400)
		frequency_bit = 5;
	else if(frequency>=360)
		frequency_bit = 6;
	else if(frequency>=300)
		frequency_bit = 7;
	else if(frequency>=250)
		frequency_bit = 8;
	else if(frequency>=200)
		frequency_bit = 9;
	else if(frequency>=150)
		frequency_bit = 10;
	else if(frequency>=75)
		frequency_bit = 11;
	else
		frequency_bit = 12;

    val = readl(ifc_dev.global_ctrl + IFC_CLK_CTRL_REG);
	val &= (~(0xF<<16)); //bit 19:16
    val |= (frequency_bit << 16);
#else
    if(frequency>=600)
    	frequency_bit = 0x1400;
    else if(frequency>=500)
    	frequency_bit = 0x1100;
    else if(frequency>=400)
    	frequency_bit = 0x1200;
    else if(frequency>=333)
    	frequency_bit = 0x1300;
    else if(frequency>=300)
    	frequency_bit = 0x1401;
    else if(frequency>=200)
    	frequency_bit = 0x1402;
    else if(frequency>=150)
    	frequency_bit = 0x1403;
    else if(frequency>=100)
    	frequency_bit = 0x1405;
    else //50MHz
    	frequency_bit = 0x1207;

    val = readl(ifc_dev.global_ctrl + IFC_CLK_CTRL_REG);
    val &= (~(0xFFFF<<16)); //bit 31:16
    val |= (frequency_bit << 16);
#endif
    writel(val, ifc_dev.global_ctrl + IFC_CLK_CTRL_REG);

    return 0;
}

int ar_ifc_proc_state_show(struct seq_file *s, void *p)
{
    unsigned int val = 0;
    unsigned char str[16] = {0};

    mutex_lock(&ifc_dev.lock);
    if(ifc_dev.suspended)
    {
        seq_printf(s,"IFC has been suspended!\n");
        mutex_unlock(&ifc_dev.lock);
        return 0;
    }
    
    seq_printf(s,"--------------------YUV--------------------------\n");
    seq_printf(s, "  YUV Width    :  %d\n", readl(ifc_dev.base + IFC_YUV_WIDTH_REG));
    seq_printf(s, "  YUV Height   :  %d\n", readl(ifc_dev.base + IFC_YUV_HEIGHT_REG));
    seq_printf(s, "  YUV YStride  :  %d\n", readl(ifc_dev.base + IFC_YUV_Y_STRIDE_REG));
    seq_printf(s, "  YUV UStride  :  %d\n", readl(ifc_dev.base + IFC_YUV_U_STRIDE_REG));
    seq_printf(s, "  YUV VStride  :  %d\n", readl(ifc_dev.base + IFC_YUV_V_STRIDE_REG));
    seq_printf(s, "  YUV YAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_YUV_Y_ADDR_REG));
    seq_printf(s, "  YUV UAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_YUV_U_ADDR_REG));
    seq_printf(s, "  YUV VAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_YUV_V_ADDR_REG));
    val = readl(ifc_dev.base + IFC_YUV_FORMAT_REG);
    if((val & 0xf) == 0)
    {
        strcpy(str, "YUV420");
    }
    if((val & 0xf) == 1)
    {
        strcpy(str, "YUV422");
    }
    if((val & 0xf) == 2)
    {
        strcpy(str, "YUV444");
    }
    if(((val >> 4) & 0xf) == 0)
    {
        strcat(str, "P");
    }
    else
    {
        strcat(str, "SP");
    }
    seq_printf(s, "  YUV Format   :  0x%08x(%s)\n", val, str);
    seq_printf(s, "  YUV PixBit   :  0x%08x\n", readl(ifc_dev.base + IFC_YUV_PIXEL_BIT_REG));
    memset(str, 0, 16);
    val = readl(ifc_dev.base + IFC_YUV_PIXEL_BYTE_REG);
    if((val & 0xf) == 0)
    {
        strcpy(str, "1P1B");
    }
    if((val & 0xf) == 1)
    {
        strcpy(str, "1P2B");
    }
    if((val & 0xf) == 2)
    {
        strcpy(str, "3P4B");
    }
    if((val & 0xf) == 3)
    {
        strcpy(str, "21P16B");
    }
    seq_printf(s, "  YUV PixByte  :  0x%08x(%s)\n", val, str);
    seq_printf(s,"--------------------RGB--------------------------\n");
    seq_printf(s, "  RGB RStride  :  %d\n", readl(ifc_dev.base + IFC_RGB_R_STRIDE_REG));
    seq_printf(s, "  RGB GStride  :  %d\n", readl(ifc_dev.base + IFC_RGB_G_STRIDE_REG));
    seq_printf(s, "  RGB BStride  :  %d\n", readl(ifc_dev.base + IFC_RGB_B_STRIDE_REG));
    seq_printf(s, "  RGB RAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_RGB_R_ADDR_REG));
    seq_printf(s, "  RGB GAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_RGB_G_ADDR_REG));
    seq_printf(s, "  RGB BAddr    :  0x%08x\n", readl(ifc_dev.base + IFC_RGB_B_ADDR_REG));
    seq_printf(s, "  RGB R_C0     :  %d\n", readl(ifc_dev.base + IFC_RGB_R_C0_REG));
    seq_printf(s, "  RGB R_C1     :  %d\n", readl(ifc_dev.base + IFC_RGB_R_C1_REG));
    seq_printf(s, "  RGB R_C2     :  %d\n", readl(ifc_dev.base + IFC_RGB_R_C2_REG));
    seq_printf(s, "  RGB R_C3     :  %d\n", readl(ifc_dev.base + IFC_RGB_R_C3_REG));
    seq_printf(s, "  RGB G_C0     :  %d\n", readl(ifc_dev.base + IFC_RGB_G_C0_REG));
    seq_printf(s, "  RGB G_C1     :  %d\n", readl(ifc_dev.base + IFC_RGB_G_C1_REG));
    seq_printf(s, "  RGB G_C2     :  %d\n", readl(ifc_dev.base + IFC_RGB_G_C2_REG));
    seq_printf(s, "  RGB G_C3     :  %d\n", readl(ifc_dev.base + IFC_RGB_G_C3_REG));
    seq_printf(s, "  RGB B_C0     :  %d\n", readl(ifc_dev.base + IFC_RGB_B_C0_REG));
    seq_printf(s, "  RGB B_C1     :  %d\n", readl(ifc_dev.base + IFC_RGB_B_C1_REG));
    seq_printf(s, "  RGB B_C2     :  %d\n", readl(ifc_dev.base + IFC_RGB_B_C2_REG));
    seq_printf(s, "  RGB B_C3     :  %d\n", readl(ifc_dev.base + IFC_RGB_B_C3_REG));
    seq_printf(s, "  RGB Shift    :  %d\n", readl(ifc_dev.base + IFC_RGB_BITS_SHIFT_REG));
    seq_printf(s, "  RGB Min      :  %d\n", readl(ifc_dev.base + IFC_RGB_MIN_REG));
    seq_printf(s, "  RGB Max      :  %d\n", readl(ifc_dev.base + IFC_RGB_MAX_REG));
    seq_printf(s, "  RGB RAvg     :  %d\n", readl(ifc_dev.base + IFC_RGB_AVG_R_REG));
    seq_printf(s, "  RGB GAvg     :  %d\n", readl(ifc_dev.base + IFC_RGB_AVG_G_REG));
    seq_printf(s, "  RGB BAvg     :  %d\n", readl(ifc_dev.base + IFC_RGB_AVG_B_REG));
    
    memset(str, 0, 16);
    val = readl(ifc_dev.base + IFC_RGB_FORMAT_REG);
    if((val & 0xf) == 0)
    {
        strcpy(str, "RGB Planar");
    }
    if((val & 0xf) == 1)
    {
        strcpy(str, "RGB Packed");
    }
    if((val & 0xf) == 3)
    {
        strcpy(str, "RGB0 Packed");
    }    
    seq_printf(s, "  RGB Format   :  0x%08x(%s)\n", val, str);
    seq_printf(s,"--------------------CTRL-------------------------\n");
    seq_printf(s, "  Auto Flag    :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AUTO_FLAG_REG));
    seq_printf(s, "  Auto Addr    :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AUTO_ADDR_REG));
    seq_printf(s, "  Auto FrameN  :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AUTO_FRAME_NUM_REG));
    seq_printf(s, "  Auto Result  :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AUTO_RESULT_ADDR_REG));
    memset(str, 0, 16);
    val = readl(ifc_dev.base + IFC_CTRL_IRQ_MODE_REG);
    if((val & 0x1) == 0)
    {
        strcpy(str, "IRQ Enable");
    }
    else
    {
        strcpy(str, "IRQ Disable");
    }
    
    seq_printf(s, "  Irq Mode     :  0x%08x(%s)\n", val, str);
    seq_printf(s, "  AXI Config   :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AXI_CFG_REG));
    seq_printf(s, "  AXI Overflow :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_AXI_OVFL_CNT_REG));
    seq_printf(s, "  Start        :  0x%08x\n", readl(ifc_dev.base + IFC_CTRL_START_REG));

    mutex_unlock(&ifc_dev.lock);

    return 0;
}

