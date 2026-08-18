//This is the HAL for IFC
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/seq_file.h>

#include "ar_scaler.h"
#include "ar_scaler_hal.h"

//static DEFINE_MUTEX(g_scaler_lock);

static uint32_t scaler_lut_table[]={
0x00000100,
0x1FFD0000,
0x00000000,
0x00000000,
0x002FF900,
0x3FF50000,
0x00000000,
0x00000000,
0x004FF100,
0x6FECFF00,
0x00000000,
0x00000000,
0x007FECFE,
0x8FE8FE00,
0x00000000,
0x00000000,
0x00AFE4FD,
0xBFE0FE00,
0x0000FFC0,
0x00000000,
0xC0DFE0FC,
0xFFDCFBFF,
0x0000FFC0,
0x00000000,
0xC10FD8FB,
0x2FD8F9FF,
0x0000FFC1,
0x00000000,
0xC14FD4F8,
0x5FD0F9FF,
0x0000FF81,
0x00000000,
0x817FD0F7,
0x9FCCF6FF,
0x0000FF81,
0x00000000,
0x81BFCCF4,
0xDFC8F3FF,
0x0000FF81,
0x00000000,
0x41FFC8F2,
0x1FC4F1FF,
0x0000FF42,
0x00000000,
0x423FC4EF,
0x5FC4EDFF,
0x0000FF42,
0x00000000,
0x027FC0ED,
0xAFC0EAFF,
0x0000FF02,
0x00000000,
0x02CFBCE9,
0xEFBCE7FF,
0x0000FF02,
0x00000000,
0xC30FBCE6,
0x3FBCE3FE,
0x0000FEC3,
0x00000000,
0xC35FB8E2,
0x8FB8E0FE,
0x0000FE83,
0x00000000,
0x83AFB8DE,
0xCFB8DCFE,
0x0000FE83,
0x00000000,
0x43FFB8DA,
0x1FB8D8FE,
0x0000FE44,
0x00000000,
0x444FB4D6,
0x7FB4D4FE,
0x0000FE04,
0x00000000,
0x049FB4D2,
0xCFB4CFFE,
0x0000FE04,
0x00000000,
0xC4EFB4CE,
0x1FB4CBFD,
0x0000FDC5,
0x00000000,
0xC54FB4C8,
0x6FB4C7FD,
0x0000FD85,
0x00000000,
0x859FB4C4,
0xCFB4C1FD,
0x0000FD85,
0x00000000,
0x45EFB4C0,
0x1FB4BDFD,
0x0000FD46,
0x00000000,
0x464FB4BA,
0x6FB4B9FD,
0x0000FD06,
0x00000000,
0x069FB4B6,
0xCFB8B2FD,
0x0000FD06,
0x00000000,
0xC6FFB8B0,
0x2FB8ADFC,
0x0000FCC7,
0x00000000,
0xC74FB8AB,
0x7FB8A8FC,
0x0000FCC7,
0x00000000,
0x87AFB8A6,
0xDFB8A3FC,
0x0000FC87,
0x00000000,
0x87FFBCA0,
0x2FBC9EFC,
0x0000FC48,
0x00000000,
0x485FBC9B,
0x8FBC98FC,
0x0000FC48,
0x00000000,
0x48AFC095,
0xDFC093FC,
0x0000FC08,
0x00000000,
0x090FC090,
0x3FC08DFC,
0x0000FC09,
0x00000000,
0x095FC48A,
0x8FC488FC,
0x0000FBC9,
0x00000000,
0xC9BFC485,
0xEFC482FB,
0x0000FBC9,
0x00000000,
0xCA0FC87F,
0x3FC87DFB,
0x0000FB8A,
0x00000000,
0x8A6FC87A,
0x8FCC77FB,
0x0000FB8A,
0x00000000,
0x8ABFCC74,
0xEFCC71FB,
0x0000FB8A,
0x00000000,
0x8B0FCC6F,
0x3FD06BFB,
0x0000FB8B,
0x00000000,
0x4B5FD06A,
0x8FD067FB,
0x0000FB4B,
0x00000000,
0x4BAFD464,
0xDFD461FB,
0x0000FB4B,
0x00000000,
0x4BFFD45F,
0x2FD85BFB,
0x0000FB4C,
0x00000000,
0x4C4FD859,
0x6FD857FB,
0x0000FB4C,
0x00000000,
0x4C9FDC53,
0xBFDC51FB,
0x0000FB4C,
0x00000000,
0x4CDFDC4F,
0xFFE04CFB,
0x0000FB4C,
0x00000000,
0x4D2FE049,
0x4FE047FB,
0x0000FB4D,
0x00000000,
0x4D6FE444,
0x8FE441FB,
0x0000FB8D,
0x00000000,
0x8DAFE43F,
0xCFE83CFB,
0x0000FB8D,
0x00000000,
0x8DEFE83A,
0x0FE838FB,
0x0000FB8E,
0x00000000,
0x8E2FEC35,
0x4FEC32FB,
0x0000FBCE,
0x00000000,
0xCE5FEC31,
0x7FF02EFB,
0x0000FBCE,
0x00000000,
0xCE9FF02C,
0xAFF02AFB,
0x0000FC0E,
0x00000000,
0x0ECFF028,
0xEFF424FC,
0x0000FC4E,
0x00000000,
0x4EFFF423,
0x0FF422FC,
0x0000FC4F,
0x00000000,
0x8F2FF41F,
0x3FF81DFC,
0x0000FC8F,
0x00000000,
0xCF4FF81B,
0x6FF819FC,
0x0000FCCF,
0x00000000,
0x0F7FF817,
0x8FF816FD,
0x0000FD0F,
0x00000000,
0x4F9FFC13,
0xAFFC11FD,
0x0000FD8F,
0x00000000,
0x8FBFFC10,
0xCFFC0EFD,
0x0000FDCF,
0x00000000,
0x0FCFFC0D,
0xDFFC0CFE,
0x0000FE0F,
0x00000000,
0x4FE00009,
0xE00008FE,
0x0000FE8F,
0x00000000,
0xCFF00006,
0xF00006FE,
0x0000FECF,
0x00000000,
0x0FF00005,
0x000003FF,
0x0000FF50,
0x00000000,
0x90000002,
0x000001FF,
0x0000FFD0,
0x00000000
};

extern struct ar_scaler_dev scaler_dev;

int ar_scaler_get_lut_size()
{
    return sizeof(scaler_lut_table);
}

int ar_scaler_hw_init(struct ar_scaler_dev * pdev)
{
    volatile scaler_reg_t * p_scaler = (volatile scaler_reg_t *)pdev->base;
	unsigned int val = 0;
	void __iomem * base = scaler_dev.global_ctrl;
    if(!base)
    {
        printk(KERN_ERR"ar_scaler_hw_init aquire base address error!\n");
        return -1;
    }
	ar_scaler_set_frequency(scaler_dev.frequency);
#ifdef PROXIMA
    //Assert reset & enable scaler 
    val = readl(base + SCALER_GLB_CTRL_REG);
    val &= (~(1<<8));
    val &= (~(1<<11));
    val &= (~(1<<12));
    writel(val, base + SCALER_GLB_CTRL_REG);
    udelay(100);

    //power on vision
    val = readl(scaler_dev.global_ctrl + SCALER_PMU_CTRL_REG);
    val &= ~(1<<19); //iso off
    val |= (1<<3); //power on
    val |= (1<<11);
    writel(0xACCE55, scaler_dev.global_ctrl + SCALER_PMU_CTRL_ACCESS);
    writel(val, scaler_dev.global_ctrl + SCALER_PMU_CTRL_REG);
    msleep(5);
    
    //release reset
    val = readl(base + SCALER_GLB_CTRL_REG);
    val |= (1 << 8);
    val |= (1 << 11);
    val |= (1 << 12);
    writel(val, base + SCALER_GLB_CTRL_REG);
    udelay(100);
#else
    val = readl(base + SCALER_GLB_CTRL_REG);
    val |= (1 << 12);
    writel(val, base + SCALER_GLB_CTRL_REG);
    udelay(100);
#endif
    memcpy((char *)pdev->lut_va, scaler_lut_table, sizeof(scaler_lut_table));

    //set to single mode
    AR_SET_REG_BITS(p_scaler->SCALER_REG_00,0,0,0);
    p_scaler->SCALER_REG_10=0x1040d;
    //enable src crop function
    AR_SET_REG_BITS(p_scaler->SCALER_REG_10,1,2,2);
    //enable lut refresh
    AR_SET_REG_BITS(p_scaler->SCALER_REG_10,1,3,3);
    //enable scaler always
    AR_SET_REG_BITS(p_scaler->SCALER_REG_10,0,1,1);
    //set bicubic interp mode
    AR_SET_REG_BITS(p_scaler->SCALER_REG_10,0,5,6);
    //set burst to 4
    AR_SET_REG_BITS(p_scaler->SCALER_REG_16,4,0,7);
    AR_SET_REG_BITS(p_scaler->SCALER_REG_16,0x78,8,14);
    p_scaler->SCALER_REG_28 = pdev->lut_pa;

    //enable the irq for bit31 for single mode
    p_scaler->SCALER_REG_04=0;
    AR_SET_REG_BITS(p_scaler->SCALER_REG_04,1,31,31);
    AR_SET_REG_BITS(p_scaler->SCALER_REG_10,1,3,3);
    return 0;
}

int ar_scaler_poweroff(struct ar_scaler_dev * pdev)
{
    unsigned int val = 0;
    void __iomem * base = pdev->global_ctrl;
#ifdef PROXIMA
    //reset & clk off 
    val = readl(base + SCALER_GLB_CTRL_REG);
    val &= (~(1<<8));
    val &= (~(1<<11));
    val &= (~(1<<12));
    writel(val, base + SCALER_GLB_CTRL_REG);
#else //9341
//reset & clk off 
    val = readl(base + SCALER_GLB_CTRL_REG);
    val &= (~(1<<12));
    writel(val, base + SCALER_GLB_CTRL_REG);
#endif

}

void CoordinateConvetor_axi(int in_size,int out_size,int64_t *res_init,int32_t *div)
{

    int64_t dy_tmp = in_size;
    int64_t dy  = ((dy_tmp<<16)/out_size);
    //X_res = (2.*(0:RangeRes-1)+1).*int_dy - 256 * 256;
    int64_t X_res = dy-(1<<16);
    int64_t X_res_128_ini = X_res;
    if(X_res_128_ini<0){
       X_res_128_ini = ((int64_t)1<<37)+X_res_128_ini;
    }
    *div = (int32_t)dy;
    *res_init=X_res_128_ini;
}

int scaler_set_batch_registers(volatile scaler_cmd_reg_t * batch_reg, ar_scaler_params_st * data, uint32_t lut_table)
{
    volatile scaler_cmd_reg_t * p_scaler = batch_reg;
    uint32_t max_burst = 0;
    uint32_t h_ratio = 0 ;
    int64_t res_init=0;
    int32_t div=0;

    p_scaler->SCALER_NOR_00 = data->src_phy_addr;
#ifdef AR9341    
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01,data->bit_depth,22,23);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01,data->byte_per_pixel,16,17);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01, SCALER_DEFAULT_PIXEL_ENDIAN ,24,24);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01, SCALER_DEFAULT_BUS_ENDIAN,25,25);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01,data->src_stride,0,12);
#else
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01,data->src_stride,0,15);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_01,data->bit_depth,16,31);
#endif
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_02,data->src_w,0,12);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_02,data->src_h,16,28);
    
#ifdef AR9341    
    p_scaler->SCALER_NOR_03=0x1040d;
#else
    p_scaler->SCALER_NOR_03=0x1000c;
#endif

    //enable src crop function
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_03,1,2,2);
    //enable lut refresh
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_03,1,3,3);
    //enable scaler always
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_03,0,1,1);
    //set bicubic interp mode
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_03,data->mode, 5, 6);

    AR_SET_REG_BITS(p_scaler->SCALER_NOR_03,data->channels,16,20);

    AR_SET_REG_BITS(p_scaler->SCALER_NOR_04,data->crop_x,0,12);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_04,data->crop_y,16,28);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_05,data->crop_w,0,12);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_05,data->crop_h,16,28);
    //hw burst
    //AR_SET_REG_BITS(p_scaler->SCALER_REG_16,data->hw_burst,0,7);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_09,0x78,8,14);

    h_ratio = data->crop_h * 100 / data->dst_h;
    if(data->crop_w >= data->dst_w && data->channels * h_ratio < 3200) //k_norm * h_ratio < 32
    {
        //AR_SET_REG_BITS(p_scaler->SCALER_NOR_09,1,16,16);
        max_burst = 16 / data->channels;
        AR_SET_REG_BITS(p_scaler->SCALER_NOR_09, max_burst, 0, 7);
    }
    else
    {
        //AR_SET_REG_BITS(p_scaler->SCALER_NOR_09,0,16,16);
        max_burst = 8 / data->channels;
        AR_SET_REG_BITS(p_scaler->SCALER_NOR_09, max_burst, 0, 7);
    }

    //debug
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_09, 1,16,16);

    AR_SET_REG_BITS(p_scaler->SCALER_NOR_10,data->dst_w,0,12);
    AR_SET_REG_BITS(p_scaler->SCALER_NOR_10,data->dst_h,16,28);
    p_scaler->SCALER_NOR_19=data->dst_phy_addr;
    p_scaler->SCALER_NOR_20=data->dst_stride;

    CoordinateConvetor_axi(data->crop_w, data->dst_w, &res_init, &div);
    p_scaler->SCALER_NOR_11=div;
    p_scaler->SCALER_NOR_12=(res_init&((int64_t)0x1f<<32))>>32;
    p_scaler->SCALER_NOR_13=(res_init&0x00ffffffff);

    res_init=0;
    div=0;
    CoordinateConvetor_axi(data->crop_h, data->dst_h, &res_init, &div);
    p_scaler->SCALER_NOR_15=div;
    p_scaler->SCALER_NOR_16=(res_init&((int64_t)0x1f<<32))>>32;
    p_scaler->SCALER_NOR_17=(res_init&0x00ffffffff);
    p_scaler->SCALER_NOR_21=(uint32_t)lut_table;

#if 0
    printk("====Addr: %x===========\n", p_scaler);
    printk("src_phy_addr     = %x\n", data->src_phy_addr  );
    printk("src_w            = %d\n", data->src_w         );
    printk("src_h            = %d\n", data->src_h         );
    printk("src_stride       = %d\n", data->src_stride    );
    printk("crop_x           = %d\n", data->crop_x        );
    printk("crop_y           = %d\n", data->crop_y        );
    printk("crop_w           = %d\n", data->crop_w        );
    printk("crop_h           = %d\n", data->crop_h        );
    printk("dst_phy_addr     = %x\n", data->dst_phy_addr  );
    printk("dst_w            = %d\n", data->dst_w         );
    printk("dst_h            = %d\n", data->dst_h         );
    printk("dst_stride       = %d\n", data->dst_stride    );
    printk("channels         = %d\n", data->channels      );
    printk("mode             = %d\n", data->mode          );
    printk("byte_per_pixel   = %d\n", data->byte_per_pixel);
    printk("bit_depth        = %d\n", data->bit_depth     );

    printk("SCALER_NOR_00 = %x\n", p_scaler->SCALER_NOR_00);
      printk("SCALER_NOR_01 = %x\n", p_scaler->SCALER_NOR_01);
      printk("SCALER_NOR_02 = %x\n", p_scaler->SCALER_NOR_02);
      printk("SCALER_NOR_03 = %x\n", p_scaler->SCALER_NOR_03);
      printk("SCALER_NOR_04 = %x\n", p_scaler->SCALER_NOR_04);
      printk("SCALER_NOR_05 = %x\n", p_scaler->SCALER_NOR_05);
      printk("SCALER_NOR_06 = %x\n", p_scaler->SCALER_NOR_06);
      printk("SCALER_NOR_07 = %x\n", p_scaler->SCALER_NOR_07);
      printk("SCALER_NOR_08 = %x\n", p_scaler->SCALER_NOR_08);
      printk("SCALER_NOR_09 = %x\n", p_scaler->SCALER_NOR_09);
      printk("SCALER_NOR_10 = %x\n", p_scaler->SCALER_NOR_10);
      printk("SCALER_NOR_11 = %x\n", p_scaler->SCALER_NOR_11);
      printk("SCALER_NOR_12 = %x\n", p_scaler->SCALER_NOR_12);
      printk("SCALER_NOR_13 = %x\n", p_scaler->SCALER_NOR_13);
      printk("SCALER_NOR_14 = %x\n", p_scaler->SCALER_NOR_14);
      printk("SCALER_NOR_15 = %x\n", p_scaler->SCALER_NOR_15);
      printk("SCALER_NOR_16 = %x\n", p_scaler->SCALER_NOR_16);
      printk("SCALER_NOR_17 = %x\n", p_scaler->SCALER_NOR_17);
      printk("SCALER_NOR_18 = %x\n", p_scaler->SCALER_NOR_18);
      printk("SCALER_NOR_19 = %x\n", p_scaler->SCALER_NOR_19);
      printk("SCALER_NOR_20 = %x\n", p_scaler->SCALER_NOR_20);
      printk("SCALER_NOR_21 = %x\n", p_scaler->SCALER_NOR_21);
#endif
    return 0;
}

int scaler_hw_start_batch_frame(void __iomem * base, unsigned int batch_addr, int batch_num)
{
    volatile scaler_reg_t * p_scaler = (volatile scaler_reg_t * )base;

    AR_SET_REG_BITS(p_scaler->SCALER_REG_00, 1, 0, 0);
    p_scaler->SCALER_REG_01 = batch_addr;
    AR_SET_REG_BITS(p_scaler->SCALER_REG_03, 0, 0,15);
    AR_SET_REG_BITS(p_scaler->SCALER_REG_03, batch_num, 16, 31);

    //trigger start scaler
    p_scaler->SCALER_REG_06 = 1;
#if 0
    printk("SCALER_REG_00 = %x\n", p_scaler->SCALER_REG_00);
    printk("SCALER_REG_01 = %x\n", p_scaler->SCALER_REG_01);
    printk("SCALER_REG_02 = %x\n", p_scaler->SCALER_REG_02);
    printk("SCALER_REG_03 = %x\n", p_scaler->SCALER_REG_03);
    printk("SCALER_REG_04 = %x\n", p_scaler->SCALER_REG_04);
    printk("SCALER_REG_05 = %x\n", p_scaler->SCALER_REG_05);
    printk("SCALER_REG_06 = %x\n", p_scaler->SCALER_REG_06);
#endif
    return 0;
}

int scaler_hw_start_single_frame(void __iomem * base)
{
    volatile scaler_reg_t * p_scaler = (volatile scaler_reg_t * )base;

    AR_SET_REG_BITS(p_scaler->SCALER_REG_00, 0, 0, 0);

    //trigger start scaler
    p_scaler->SCALER_REG_06 = 1;
#if 0
    printk("SCALER_REG_00 = %x\n", p_scaler->SCALER_REG_00);
    printk("SCALER_REG_01 = %x\n", p_scaler->SCALER_REG_01);
    printk("SCALER_REG_02 = %x\n", p_scaler->SCALER_REG_02);
    printk("SCALER_REG_03 = %x\n", p_scaler->SCALER_REG_03);
    printk("SCALER_REG_04 = %x\n", p_scaler->SCALER_REG_04);
    printk("SCALER_REG_05 = %x\n", p_scaler->SCALER_REG_05);
    printk("SCALER_REG_06 = %x\n", p_scaler->SCALER_REG_06);
#endif
    return 0;
}


int scaler_hw_process_irq(void __iomem * base)
{
    volatile scaler_reg_t * p_scaler = (volatile scaler_reg_t * )base;
    int status = p_scaler->SCALER_REG_05;
    p_scaler->SCALER_REG_05 = status;
//    printk("scale irq stats=0x%x",status);
    if(status & (1 << 31))
    {
       //printk("=scaler done==\n");
       return 1;
    }

    return -1;
}

irqreturn_t ar_scaler_irq_handler(int irq, void * dev)
{
    struct ar_scaler_dev * pdev = (struct ar_scaler_dev *)dev;

    if(scaler_hw_process_irq(pdev->base) > 0)
    {
        complete(&pdev->done);
    }

    return IRQ_HANDLED;
}
void ar_scaler_dump_reg(struct ar_scaler_dev * pdev, int batch_dump)
{
    int i = 0, j = 0;
    int batch_start = 0, batch_end = 0;
    unsigned int * p_batch = NULL;

    for(i = 0; i < 7; i++)
    {
        printk("TOP %d:   0x%x\n", i, readl(pdev->base + i * 4));
    }

    for(i = 0; i < 30; i++)
    {
        printk("REG %d:   0x%x\n", i, readl(pdev->base + 28 + i * 4));
    }

    printk("Channel: %d\n", readl(pdev->base + 0x11C)); //current work channel

    if(!batch_dump)
    {
        return;
    }

    //print batch registers
    batch_start = readl(pdev->base + 3 * 4) & 0x0ffff;
    batch_end = (readl(pdev->base + 3 * 4) & 0xffff0000) >> 16;

    for(i = batch_start; i < batch_end; i++)
    {
        p_batch = (unsigned int *)(pdev->batch_buffer_va + i * SCALER_CMD_BUFFER_SIZE_PER_CMD * 4);
        printk("Batch %d:\n", i);
        for(j = 0; j < 22; j++)
        {
            printk("BATCH_REG %d:   0x%x\n", j, *(p_batch + j));
        }
    }
}

//by default, irq is raised when the last frame is done.
int ar_scaler_start_batch(struct ar_scaler_dev * pdev, ar_scaler_auto_st * params)
{
    int i = 0;
    int ret = 0;

    //printk("Batch VA %x PA %x\n", pdev->batch_buffer_va, pdev->batch_buffer_pa);
    ret = mutex_lock_interruptible(&pdev->lock);
    if(ret)
    {
        printk(KERN_ERR"ar_scaler_start_batch get lock error\n");
        return -EINVAL;
    }
    if(pdev->suspended)
    {
        mutex_unlock(&pdev->lock);
        return -EAGAIN;
    }
    
    memset((char *)pdev->batch_buffer_va, 0, AR_SCALER_MAX_BATCH * SCALER_CMD_BUFFER_SIZE_PER_CMD * 4);

    for(i = 0; i < params->batch_num; i++)
    {
        //check params
        if(params->batch_params[i].channels == 0
            || params->batch_params[i].dst_h == 0
            || params->batch_params[i].dst_w == 0
            || params->batch_params[i].src_h == 0
            || params->batch_params[i].src_w == 0
            || params->batch_params[i].src_stride % 16 != 0
            || params->batch_params[i].dst_stride % 16 != 0
            || params->batch_params[i].src_phy_addr == 0
            || params->batch_params[i].dst_phy_addr == 0
            || params->batch_params[i].crop_w == 0
            || params->batch_params[i].crop_h == 0)
        {
            printk("%s %d: invalid param!\n", __FUNCTION__, __LINE__);
            printk("channels %d\n", params->batch_params[i].channels);
            printk("dsth %d\n", params->batch_params[i].dst_h);
            printk("dstw %d\n", params->batch_params[i].dst_w);
            printk("srch %d\n", params->batch_params[i].src_h);
            printk("srcw %d\n", params->batch_params[i].src_w);
            printk("srcstride %d\n", params->batch_params[i].src_stride);
            printk("dststride %d\n", params->batch_params[i].dst_stride);
            printk("srcphy %x\n", params->batch_params[i].src_phy_addr);
            printk("dstphy %x\n", params->batch_params[i].dst_phy_addr);
            printk("cropw %d\n", params->batch_params[i].crop_w);
            printk("croph %d\n", params->batch_params[i].crop_h);
            
	    mutex_unlock(&pdev->lock);
	    return -1;
        }
        scaler_set_batch_registers((scaler_cmd_reg_t *)(pdev->batch_buffer_va + i * SCALER_CMD_BUFFER_SIZE_PER_CMD * 4), &params->batch_params[i], pdev->lut_pa);
    }

    scaler_hw_start_batch_frame(pdev->base, pdev->batch_buffer_pa, params->batch_num);

    //wait for complete
    ret = wait_for_completion_timeout(&pdev->done, msecs_to_jiffies(SCALER_DEFAULT_TIMEOUT));
    mutex_unlock(&pdev->lock);

    if(ret == 0)
    {
        printk("wait_for_completion_interruptible_timeout return == 0\n");
        ar_scaler_dump_reg(pdev, 1);
        return -ETIME;
    }

    if(ret < 0)
    {
        printk("wait_for_completion_interruptible_timeout return < 0\n");
        ar_scaler_dump_reg(pdev, 1);
        return -EINTR;
    }

    //printk("Scaler finished.\n");
    return 0;
}

int ar_scaler_start_single(struct ar_scaler_dev * pdev, ar_scaler_params_st * params)
{
    int ret = 0;

    ret = mutex_lock_interruptible(&pdev->lock);
    if(ret)
    {
        printk(KERN_ERR"ar_scaler_start_single get lock error\n");
        return -EINVAL;
    }
    if(pdev->suspended)
    {
        mutex_unlock(&pdev->lock);
        return -EAGAIN;
    }
    
    //check params
    if(params->channels == 0
        || params->dst_h == 0
        || params->dst_w == 0
        || params->src_h == 0
        || params->src_w == 0
        || params->src_stride % 16 != 0
        || params->dst_stride % 16 != 0
        || params->src_phy_addr == 0
        || params->dst_phy_addr == 0
        || params->crop_w == 0
        || params->crop_h == 0)
    {
        printk("%s %d: invalid param!\n", __FUNCTION__, __LINE__);
        printk("channels %d\n", params->channels);
        printk("dsth %d\n", params->dst_h);
        printk("dstw %d\n", params->dst_w);
        printk("srch %d\n", params->src_h);
        printk("srcw %d\n", params->src_w);
        printk("srcstride %d\n", params->src_stride);
        printk("dststride %d\n", params->dst_stride);
        printk("srcphy %x\n", params->src_phy_addr);
        printk("dstphy %x\n", params->dst_phy_addr);
        printk("cropw %d\n", params->crop_w);
        printk("croph %d\n", params->crop_h);
        
	mutex_unlock(&pdev->lock);
        return -1;
    }

    scaler_set_batch_registers((scaler_cmd_reg_t *)(pdev->base + 4 * 7), params, pdev->lut_pa);
    scaler_hw_start_single_frame(pdev->base);

    //wait for complete
    ret = wait_for_completion_timeout(&pdev->done, msecs_to_jiffies(SCALER_DEFAULT_TIMEOUT));
    mutex_unlock(&pdev->lock);

    if(ret == 0)
    {
        printk("wait_for_completion_interruptible_timeout ret == 0\n");
        ar_scaler_dump_reg(pdev, 0);
        return -ETIME;
    }

    if(ret < 0)
    {
        printk("wait_for_completion_interruptible_timeout ret < 0\n");
        ar_scaler_dump_reg(pdev, 0);
        return -EINTR;
    }

    //printk("Scaler finished.\n");
    return 0;
}

int ar_scaler_set_frequency(unsigned int frequency)
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

    val = readl(scaler_dev.global_ctrl + SCALER_CLK_CTRL_REG);
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

    val = readl(scaler_dev.global_ctrl + SCALER_CLK_CTRL_REG);
    val &= ~(0xFFFF); //bit 15:0
    val |= frequency_bit;
#endif
    writel(val, scaler_dev.global_ctrl + SCALER_CLK_CTRL_REG);

    return 0;
}



