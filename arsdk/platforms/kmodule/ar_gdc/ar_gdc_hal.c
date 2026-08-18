//This is the HAL for EIS
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>

#include "ar_gdc.h"
#include "ar_gdc_hal.h"

extern struct AR_GDC_DEV gdc_dev;

int ar_gdc_isp_clk_check(void)
{
#ifdef AR9341
	volatile unsigned int val_isp_clk = 0x0;
	
	val_isp_clk = readl(gdc_dev.global_ctrl + 0);
	val_isp_clk = val_isp_clk & (0x3<<4); //bit 5:4
    val_isp_clk = val_isp_clk >> 4;
	
	if(!val_isp_clk)
	{
		return 0;
	}
#endif
    return 1;
}

int ar_gdc_set_frequency(unsigned int frequency)
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

    val = readl(gdc_dev.global_ctrl + GDC_CLK_CTRL_REG);
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

    val = readl(gdc_dev.global_ctrl + GDC_CLK_CTRL_REG);
    val &= ~(0xFFFF); //bit 15:0
    val |= frequency_bit;
#endif
    writel(val, gdc_dev.global_ctrl + GDC_CLK_CTRL_REG);

    return 0;
}

int ar_gdc_hw_init(struct AR_GDC_DEV * pdev)
{
	volatile ar_gdc_reg_st * p_gdc_reg = (volatile ar_gdc_reg_st * )pdev->base;
    unsigned int val = 0;
    
#ifdef AR9341    
    //Assert reset  & enable gdc
    val = readl(pdev->global_ctrl + GDC_GLB_CTRL_REG);
    val &= (~(1<<10));
    val &= (~(1<<11));
    val &= (~(1<<14));
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
    udelay(1000);

    val |= (1 << 10);
    val |= (1 << 11);
    val |= (1 << 14);
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
    udelay(1000);
#else
    //Assert reset  & enable gdc
    val = readl(pdev->global_ctrl + GDC_GLB_CTRL_REG);
    val &= (~(1<<10));
    val &= (~(1<<11));
    val &= (~(1<<14));
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
    udelay(1000);

    //power on vision
    val = readl(gdc_dev.global_ctrl + GDC_PMU_CTRL_REG);
    val &= ~(1<<19); //iso off
    val |= (1<<3); //power on
    val |= (1<<11);
    writel(0xACCE55, gdc_dev.global_ctrl + GDC_PMU_CTRL_ACCESS);
    writel(val, gdc_dev.global_ctrl + GDC_PMU_CTRL_REG);
    msleep(5);
    
    //release reset
    val = readl(pdev->global_ctrl + GDC_GLB_CTRL_REG);
    val |= (1 << 10);
    val |= (1 << 11);
    val |= (1 << 14);
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
    udelay(1000);
#endif
	p_gdc_reg->ISP_GDC_REG_32 = 1;
    //int mask 0
	p_gdc_reg->ISP_GDC_REG_02 = 0;//~1 & 0x03;

	//AR_SET_REG_BITS(p_eis_reg->ISP_EIS_LDC_REG_27, 1, EIS_PPL_MODE, EIS_PPL_MODE);
	//AR_SET_REG_BITS(p_eis_reg->ISP_EIS_LDC_REG_27, 0, EIS_PPL_BYPASS, EIS_PPL_BYPASS);

	AR_SET_REG_BITS(p_gdc_reg->ISP_GDC_REG_27, 0, GDC_PPL_MODE, GDC_PPL_MODE);
	AR_SET_REG_BITS(p_gdc_reg->ISP_GDC_REG_27, 0, GDC_PPL_BYPASS, GDC_PPL_BYPASS);

	AR_SET_REG_BITS(p_gdc_reg->ISP_GDC_REG_32, 1, GDC_PPL_CLK_GATE, GDC_PPL_CLK_GATE);

	//AR_SET_REG_BITS(p_eis_ldc->ISP_EIS_LDC_REG_01, 1, EIS_RST_REQ, EIS_RST_REQ);

    return 0;
}

int ar_gdc_poweroff(struct AR_GDC_DEV * pdev)
{
    volatile ar_gdc_reg_st * p_gdc_reg = (volatile ar_gdc_reg_st * )pdev->base;
    unsigned int val = 0;

#ifdef AR9341    
    //Assert reset	& enable gdc
    val = readl(pdev->global_ctrl + GDC_GLB_CTRL_REG);
    val &= (~(1<<10));
    val &= (~(1<<11));
    val &= (~(1<<14));
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
#else
//Assert reset	& enable gdc
    val = readl(pdev->global_ctrl + GDC_GLB_CTRL_REG);
    val &= (~(1<<10));
    val &= (~(1<<11));
    val &= (~(1<<14));
    writel(val, pdev->global_ctrl + GDC_GLB_CTRL_REG);
#endif

}

static int ar_gdc_hw_process_irq(void __iomem * base)
{
    volatile ar_gdc_reg_st * p_gdc_reg = (volatile ar_gdc_reg_st * )base;
    int status = p_gdc_reg->ISP_GDC_REG_03;
    p_gdc_reg->ISP_GDC_REG_04 = status;

    if(status & 0x01)
    {
       return 1;
    }

    return -1;
}

irqreturn_t ar_gdc_irq_handler(int irq, void * dev)
{
    struct AR_GDC_DEV * pdev = (struct AR_GDC_DEV *)dev;

    if(ar_gdc_hw_process_irq(pdev->base) > 0)
    {
        complete(&pdev->done);
    }

    return IRQ_HANDLED;
}


static int ar_gdc_config_in_addr(struct AR_GDC_DEV * pdev,
AR_GDC_PARAMS_S* params)
{

	volatile ar_gdc_reg_st *p_gdc_reg = (volatile ar_gdc_reg_st *)pdev->base;
	AR_GDC_BUFFER_S input_buff = params->in_buffer;

    int y_stride = input_buff.format.luma_stride;//EIS_ALIGNE_TO(input_buff->format.width, 64);
    int u_stride = input_buff.format.chroma_stride;//EIS_ALIGNE_TO(input_buff->format.width / 2, 32);


    p_gdc_reg->ISP_GDC_REG_05 = (input_buff.format.width - 1) | (input_buff.format.height - 1) << 16;
    p_gdc_reg->ISP_GDC_REG_09 = (unsigned int)(input_buff.pannel[0].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_10 = y_stride;
    p_gdc_reg->ISP_GDC_REG_11 = (unsigned int)(input_buff.pannel[1].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_12 = u_stride;
    p_gdc_reg->ISP_GDC_REG_13 = (unsigned int)(input_buff.pannel[2].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_14 = u_stride;

    return 0;
}


static int ar_gdc_config_out_addr(struct AR_GDC_DEV * pdev,
AR_GDC_PARAMS_S * params)
{

	volatile ar_gdc_reg_st *p_gdc_reg = (volatile ar_gdc_reg_st *)pdev->base;
	AR_GDC_BUFFER_S output_buff = params->out_buffer;

    int y_stride = output_buff.format.luma_stride; //EIS_ALIGNE_TO(output_buff->format.width, 64);
    int u_stride = output_buff.format.chroma_stride; //EIS_ALIGNE_TO(output_buff->format.width / 2, 32);


    p_gdc_reg->ISP_GDC_REG_06 = (output_buff.format.width - 1) | (output_buff.format.height - 1) << 16;

    p_gdc_reg->ISP_GDC_REG_15 = (unsigned int)(output_buff.pannel[0].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_16 = y_stride;
    p_gdc_reg->ISP_GDC_REG_17 = (unsigned int)(output_buff.pannel[1].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_18 = u_stride;
    p_gdc_reg->ISP_GDC_REG_19 = (unsigned int)(output_buff.pannel[2].addr_phy & 0xffffffff);
    p_gdc_reg->ISP_GDC_REG_20 = u_stride;

    p_gdc_reg->ISP_GDC_REG_31 = 0x920000;
    p_gdc_reg->ISP_GDC_REG_32 = 0xf0f;
    return 0;
}



static int ar_gdc_config_map_point(struct AR_GDC_DEV * pdev,
AR_GDC_PARAMS_S* params)
{
	volatile ar_gdc_reg_st *p_gdc_reg = (volatile ar_gdc_reg_st *)pdev->base;
	AR_GDC_BUFFER_S output_buff = params->out_buffer;

	int axis_stride = GDC_ALIGNE_TO(output_buff.format.width, 64);

    axis_stride = ((axis_stride / 64) + 1 + 3) / 4 * 16;
	p_gdc_reg->ISP_GDC_REG_07 = params->lut_pa;
    p_gdc_reg->ISP_GDC_REG_08 = axis_stride;
    return 0;
}


static int ar_gdc_config_frame(struct AR_GDC_DEV * pdev,
AR_GDC_PARAMS_S * params)
{
    //config input
    ar_gdc_config_in_addr(pdev, params);
    //config output
    ar_gdc_config_out_addr(pdev, params);
    //cofnig map data
    ar_gdc_config_map_point(pdev, params);

    return 0;
}


static int ar_gdc_config_rotation(struct AR_GDC_DEV * pdev,
AR_GDC_PARAMS_S* params)
{

	volatile ar_gdc_reg_st *p_gdc_reg = (volatile ar_gdc_reg_st *)pdev->base;
	int gdc_rotate_angle = params->rotate_angle;
	if(params->transform_ex)
	{
		gdc_rotate_angle = 0;
	}
    AR_SET_REG_BITS(p_gdc_reg->ISP_GDC_REG_27,(gdc_rotate_angle & 0x3), 2, 3);
    return 0;
}


static int ar_gdc_trigger_start(struct AR_GDC_DEV * pdev)
{
	volatile ar_gdc_reg_st *gdc_reg = (volatile ar_gdc_reg_st *)pdev->base;

    gdc_reg->ISP_GDC_REG_00 = 0x01;

    return 0;

}


int ar_gdc_start_process(struct AR_GDC_DEV * pdev, AR_GDC_PARAMS_S*params)
{
    struct timespec gdctime;
	ktime_get_real_ts64(&gdctime);
    unsigned long long tic = gdctime.tv_sec * 100000 + gdctime.tv_nsec/10000;
   
    int ret = 0;
    int i = 0;
    for(i = 0; i < 3; i++)
    {
        if(params->in_buffer.pannel[i].addr_phy < 0x20000000)
        {
            printk("ERR: gdc input %d addr invalid: [0x%x]1\n", i, params->in_buffer.pannel[i].addr_phy);
            return -EFAULT;
        }
        if(params->out_buffer.pannel[i].addr_phy < 0x20000000)
        {
            printk("ERR: gdc output %d addr invalid: [0x%x]1\n", i, params->out_buffer.pannel[i].addr_phy);
            return -EFAULT;
        }
    }
    
    ret = mutex_lock_interruptible(&pdev->lock);
    if(ret)
    {
        printk(KERN_ERR"ar_gdc_start_process get lock error!\n");
        return -EINVAL;
    }

    if(pdev->suspended)
    {
        mutex_unlock(&pdev->lock);
        return -EAGAIN;
    }

    ar_gdc_config_frame(pdev, params);
    ar_gdc_config_rotation(pdev, params);
    ar_gdc_trigger_start(pdev);

#if 0
	//unsigned int *base = (unsigned int *) ioremap(params->lut_pa, 1024*4);  //mpp
	unsigned int *base = (unsigned int *) phys_to_virt(params->lut_pa);//osal
	if(!base)
	{

		printk(KERN_ERR"phys_to_virt gdc_dev.lutpa failed.\n");
		return -ENOMEM;
	}

	gdc_dev.lutlen = params->lut_len;

	memset(gdc_dev.lutbase, 0, AR_GDC_LUT_SPACE_RANGE);
	int* pmem = (int*)base;
	int* plutmem = (int*)gdc_dev.lutbase;
	for(i=0;i<gdc_dev.lutlen;i++)
	{
	    plutmem[i] = pmem[i];
	}
	iounmap(base);
#endif
    //wait for complete
    ret = wait_for_completion_interruptible_timeout(&pdev->done, msecs_to_jiffies(GDC_DEFAULT_TIMEOUT));
    mutex_unlock(&pdev->lock);

	ktime_get_real_ts64(&gdctime);
	unsigned long long toc = gdctime.tv_sec * 100000 + gdctime.tv_nsec/10000;
	pdev->tprocess = toc-tic;		

    if(ret == 0)
    {
        return -ETIME;
    }

    if(ret < 0)
    {
        return -EINTR;
    }
    return 0;
}


