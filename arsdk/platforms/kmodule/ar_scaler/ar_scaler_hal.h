#ifndef __AR_SCALER_HAL_H__
#define __AR_SCALER_HAL_H__
#include <linux/miscdevice.h>
#if defined(CONFIG_ION)
#include <linux/artosyn_ion.h>
#else
#include <linux/mman.h>
#include <osal_mmz.h>
#endif


#define ALIGN16KB        (16384)

#ifdef AR9341
#define AR_SCALER_REG_BASE 0x01090000  //need to modify later
#define AR_SCALER_REG_SPACE_RANGE 0x1000
#define AR_GLOBAL_CONTROL_RANGE 0x8000
#define SCALER_CLK_CTRL_REG       0x003C
#define AR_GLOBAL_CONTROL_BASE  0x01070000
#define SCALER_GLB_CTRL_REG    0x4010


#define SCALER_IRQ_SIGNAL 157
#else
#define AR_SCALER_REG_BASE 0x08840000
#define AR_SCALER_REG_SPACE_RANGE 0x1000
#define AR_GLOBAL_CONTROL_RANGE 0x8000
#define SCALER_CLK_CTRL_REG       0x405C
#define AR_GLOBAL_CONTROL_BASE  0x0A100000
#define SCALER_GLB_CTRL_REG    0x6010

#define SCALER_IRQ_SIGNAL 107

#define SCALER_PMU_CTRL_ACCESS 0x6200
#define SCALER_PMU_CTRL_REG    0x6204
#endif

#define SCALER_MAX_PANNELS 4

#define SCALER_DEFAULT_PIXEL_ENDIAN 0
#define SCALER_DEFAULT_BUS_ENDIAN 0
#define SCALER_DEFAULT_HW_BURST   4

#define SCALER_DEFAULT_TIMEOUT   5000 //5s

#define SCALER_INT_END_FRAME_INT (1<<0)
#define SCALER_INT_BUFFER_DONE_INT  (1<<1)
//2-5 REVERTED
#define SCALER_INT_CMD_BUF_WRITE_FAIL_INT  (1<<6)
#define SCALER_INT_CMD_WRITE_OVERFLOW_INT  (1<<7)
#define SCALER_INT_CMD_WRITE_TIME_OUT_INT  (1<<8)
#define SCALER_INT_CANVAS_FMT_UNSUPPORTED_INT  (1<<9)
#define SCALER_INT_BMP_FMT_UNSUPPORTED_INT  (1<<10)
#define SCALER_INT_DRAW_LINE_EXCEPTION_INT  (1<<11)
#define SCALER_INT_ALFA_BLENDING_EXCEPTION_INT  (1<<12)
#define SCALER_CMD_BUFFER_SIZE_PER_CMD  64

#define SCALER_ERR (SCALER_INT_CMD_BUF_WRITE_FAIL_INT|SCALER_INT_CMD_WRITE_OVERFLOW_INT|\
	SCALER_INT_CMD_WRITE_TIME_OUT_INT|SCALER_INT_CANVAS_FMT_UNSUPPORTED_INT|SCALER_INT_BMP_FMT_UNSUPPORTED_INT|\
	SCALER_INT_DRAW_LINE_EXCEPTION_INT|SCALER_INT_ALFA_BLENDING_EXCEPTION_INT)

typedef struct
{
  uint32_t SCALER_REG_00;
  uint32_t SCALER_REG_01;
  uint32_t SCALER_REG_02;
  uint32_t SCALER_REG_03;
  uint32_t SCALER_REG_04;
  uint32_t SCALER_REG_05;
  uint32_t SCALER_REG_06;
  uint32_t SCALER_REG_07;
  uint32_t SCALER_REG_08;
  uint32_t SCALER_REG_09;
  uint32_t SCALER_REG_10;
  uint32_t SCALER_REG_11;
  uint32_t SCALER_REG_12;
  uint32_t SCALER_REG_13;
  uint32_t SCALER_REG_14;
  uint32_t SCALER_REG_15;
  uint32_t SCALER_REG_16;
  uint32_t SCALER_REG_17;
  uint32_t SCALER_REG_18;
  uint32_t SCALER_REG_19;
  uint32_t SCALER_REG_20;
  uint32_t SCALER_REG_21;
  uint32_t SCALER_REG_22;
  uint32_t SCALER_REG_23;
  uint32_t SCALER_REG_24;
  uint32_t SCALER_REG_25;
  uint32_t SCALER_REG_26;
  uint32_t SCALER_REG_27;
  uint32_t SCALER_REG_28;
}scaler_reg_t;

typedef struct
{
    uint32_t SCALER_NOR_00;
    uint32_t SCALER_NOR_01;
    uint32_t SCALER_NOR_02;
    uint32_t SCALER_NOR_03;
    uint32_t SCALER_NOR_04;
    uint32_t SCALER_NOR_05;
    uint32_t SCALER_NOR_06;
    uint32_t SCALER_NOR_07;
    uint32_t SCALER_NOR_08;
    uint32_t SCALER_NOR_09;
    uint32_t SCALER_NOR_10;
    uint32_t SCALER_NOR_11;
    uint32_t SCALER_NOR_12;
    uint32_t SCALER_NOR_13;
    uint32_t SCALER_NOR_14;
    uint32_t SCALER_NOR_15;
    uint32_t SCALER_NOR_16;
    uint32_t SCALER_NOR_17;
    uint32_t SCALER_NOR_18;
    uint32_t SCALER_NOR_19;
    uint32_t SCALER_NOR_20;
    uint32_t SCALER_NOR_21;
}scaler_cmd_reg_t;

typedef struct
{
  void *addr;
  int w;
  int h;
  int stride;
  int x;
  int y;
  int crop_w;
  int crop_h;
}scaler_pannel_t;

typedef struct
{
  int pix_bit_depth;
  int byte_count_per_pix;
  int pix_endian;
  int bus_endians;
  int k_norm;
  int hw_burst;
  scaler_pannel_t src;
  scaler_pannel_t dst;
}scaler_batch_data_t;

typedef struct
{
  //ar_signal_t frame_done;
  int batch_mode;
  int pix_bit_depth;
  int byte_count_per_pix;
  int pix_endian;
  int bus_endians;
  int k_norm;
  int hw_burst;
  int pannels;
  scaler_pannel_t src[SCALER_MAX_PANNELS];
  scaler_pannel_t dst[SCALER_MAX_PANNELS];

  //for scaler batch mode
  int batch_num;
  int current_index;
  int start_index;
  int end_index;
  uint32_t *p_batch_cmd_buffer;
  uint32_t *p_batch_cmd_buffer_orig;
}scaler_cmd_t;

#define AR_SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))

struct ar_scaler_dev
{
    void __iomem * base;
	void __iomem * global_ctrl;
    unsigned int frequency;
    uint64_t lut_va;
    unsigned int lut_pa;
    uint64_t batch_buffer_va;
    unsigned int batch_buffer_pa;
	#if defined(CONFIG_ION)
	int lut_ion_fd;
	int batch_buffer_fd;
	#else
	hil_mmb_t * scaler_lut_mmb;
    hil_mmb_t * batch_buffer_mmb;
	#endif
    struct miscdevice miscdev;
    unsigned int irq;
    struct completion done;
    struct mutex lock;
    struct proc_dir_entry * proc_dir;
    unsigned int suspended;
};

int ar_scaler_get_lut_size(void);
int ar_scaler_hw_init(struct ar_scaler_dev * pdev);
irqreturn_t ar_scaler_irq_handler(int irq, void * dev);
int ar_scaler_start_batch(struct ar_scaler_dev * pdev, ar_scaler_auto_st * params);
int ar_scaler_start_single(struct ar_scaler_dev * pdev, ar_scaler_params_st * params);
int ar_scaler_set_frequency(unsigned int frequency);
int ar_scaler_poweroff(struct ar_scaler_dev * pdev);

#endif
