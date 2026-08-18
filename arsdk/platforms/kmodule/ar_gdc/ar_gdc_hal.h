#ifndef __AR_GDC_HAL_H__
#define __AR_GDC_HAL_H__
#include <linux/miscdevice.h>

#define AR_GLOBAL_CONTROL_RANGE 0x8000
#define AR_GDC_REG_SPACE_RANGE 0x1000
#define GDC_DEFAULT_TIMEOUT   1000 //1s

#ifdef AR9341
#define GDC_CLK_CTRL_REG       0x003C
#define AR_GLOBAL_CONTROL_BASE  0x01070000
#define GDC_GLB_CTRL_REG    0x4010
#else
#define GDC_CLK_CTRL_REG       0x005C
#define AR_GLOBAL_CONTROL_BASE  0x0A100000
#define GDC_GLB_CTRL_REG    0x6010

#define GDC_PMU_CTRL_ACCESS 0x6200
#define GDC_PMU_CTRL_REG    0x6204
#endif

#define GDC_ALIGNE_TO(size,num) ( (num)*( ( (size)+(num)-1)/(num) ) )
#define AR_SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))

#define GDC_PPL_MODE (0)
#define GDC_PPL_BYPASS (1)
#define GDC_PPL_CLK_GATE (1)
#define GDC_RST_REQ (0)


typedef struct
{
   unsigned int ISP_GDC_REG_00;
   unsigned int ISP_GDC_REG_01;
   unsigned int ISP_GDC_REG_02;
   unsigned int ISP_GDC_REG_03;
   unsigned int ISP_GDC_REG_04;
   unsigned int ISP_GDC_REG_05;
   unsigned int ISP_GDC_REG_06;
   unsigned int ISP_GDC_REG_07;
   unsigned int ISP_GDC_REG_08;
   unsigned int ISP_GDC_REG_09;
   unsigned int ISP_GDC_REG_10;
   unsigned int ISP_GDC_REG_11;
   unsigned int ISP_GDC_REG_12;
   unsigned int ISP_GDC_REG_13;
   unsigned int ISP_GDC_REG_14;
   unsigned int ISP_GDC_REG_15;
   unsigned int ISP_GDC_REG_16;
   unsigned int ISP_GDC_REG_17;
   unsigned int ISP_GDC_REG_18;
   unsigned int ISP_GDC_REG_19;
   unsigned int ISP_GDC_REG_20;
   unsigned int ISP_GDC_REG_21;
   unsigned int ISP_GDC_REG_22;
   unsigned int ISP_GDC_REG_23;
   unsigned int ISP_GDC_REG_24;
   unsigned int ISP_GDC_REG_25;
   unsigned int ISP_GDC_REG_26;
   unsigned int ISP_GDC_REG_27;
   unsigned int ISP_GDC_REG_28;
   unsigned int ISP_GDC_REG_29;
   unsigned int ISP_GDC_REG_30;
   unsigned int ISP_GDC_REG_31;
   unsigned int ISP_GDC_REG_32;
   unsigned int ISP_GDC_REG_33;
   unsigned int ISP_GDC_REG_34;
   unsigned int ISP_GDC_REG_35;
}ar_gdc_reg_st;

struct AR_GDC_DEV
{
    void * lutbase;
    unsigned int lutlen;
    void __iomem * base;
    void __iomem * global_ctrl;
    struct miscdevice miscdev;
    unsigned int irq;
    struct completion done;
    struct mutex lock;
    struct proc_dir_entry * proc_dir;
	unsigned int frequency;
	unsigned int suspended;
	unsigned long long tprocess;
};

int ar_gdc_isp_clk_check(void);
int ar_gdc_set_frequency(unsigned int frequency);
int ar_gdc_hw_init(struct AR_GDC_DEV * pdev);
irqreturn_t ar_gdc_irq_handler(int irq, void * dev);
int ar_gdc_start_process(struct AR_GDC_DEV * pdev, AR_GDC_PARAMS_S* params);
int ar_gdc_poweroff(struct AR_GDC_DEV * pdev);
#endif
