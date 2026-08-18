#ifndef __AR_NPU_HAL_H__
#define __AR_NPU_HAL_H__

#include <linux/miscdevice.h>
#include <linux/time.h>
#include <linux/timekeeping.h>

//System global registers
#ifdef AR9341
#define AR_SOC_VERSION   1
#define AR_GLOBAL_CONTROL_BASE  0x01070000
#define AR_GLOBAL_CONTROL_RANGE 0x8000
#define NPU_CLK_CTRL_REG  0x4C
#define NPU_RESET_CTRL    0x1054
#define NPU_PLL_CTRL_0    0x4020
#define NPU_PLL_CTRL_1    0x4024
#define NPU_PMU_CTRL_ACCESS 0x5000
#define NPU_PMU_CTRL_REG    0x5004
#define SYS_BOOT_MODE_REG   0x4100
//#define NPU_SECURITY_MODE_REG 0x01050074

#define NPU_SECURITY_BASE 0x01050000
#define NPU_SECURITY_MODE_REG 0x74


#define DMAC_BASE_ADDR    0x01010000
#define DMAC_NPU_CHANNEL  0

#define NPU_REG_SPACE_RANGE   0x1000
#define NPU_CFG_BASE_ADDR 0x01140000

#define NPU_SRAM_BASE_ADDR 0x017C0000
#define NPU_SRAM_DEF_SIZE  0x00040000
#define NPU_SRAM_CEVA_SIZE  0x00080000

#else
#define AR_SOC_VERSION   2
#define AR_GLOBAL_CONTROL_BASE  0x0A100000
#define AR_GLOBAL_CONTROL_RANGE 0x8000 
#define NPU_RESET_CTRL_0    0x2000  //self release
#define NPU_RESET_LEVEL_0   0x2004
#define NPU_RESET_MASK_0    0x2008
#define NPU_RESET_CTRL_1    0x2010  //self release
#define NPU_RESET_LEVEL_1   0x2014
#define NPU_RESET_MASK_1    0x2018
#define NPU_PLL_CTRL_0    0x4010
#define NPU_PMU_CTRL_ACCESS 0x6200
#define NPU_PMU_CTRL_REG    0x6204

//npu_security_mode 0x08420000
#define NPU_SECURITY_BASE 0x08420000
#define NPU_SECURITY_MODE_REG 0x74

#define DMAC_BASE_ADDR    0x08800000  //0x01E10000
#define DMAC_NPU_CHANNEL  0

//npu base config reg
#define NPU_REG_SPACE_RANGE   0x1000
#define NPU_CFG_BASE_ADDR 0x0A000000

#define NPU_SRAM_BASE_ADDR 0x00100000
#define NPU_SRAM_DEF_SIZE  0x00100000
#define NPU_SRAM_CEVA_SIZE  0x00000000

#endif

#define NPU_START_REG             (0x00)
#define NPU_COMMAND_ADDR_REG      (0x04)
#define NPU_CMPLT_LAYER_NUM_REG   (0x08)
#define NPU_COMMAND_LEN_REG       (0x0C)
#define NPU_RESTART_ADDR_REG      (0x10)
#define NPU_IRQ_STATUS_REG        (0x14)
#define NPU_IRQ_NET_FRAME_ID_REG  (0x18)
#define NPU_PROC_NET_FRAME_ID_REG (0x1C)
#define NPU_IRQ_MASK_REG          (0x20)
#define NPU_IRQ_CLR_REG           (0x24)
#define NPU_AXI_QOS_REG           (0x28)
#define NPU_SRAM_ADDR_REG         (0x2C)
#define NPU_SRAM_SIZE_REG         (0x30)
#define NPU_RUNTIME_ADDR_REG      (0x34)
#define NPU_PARAMS_ADDR_REG       (0x38)
#define NPU_INPUT_ADDR_0_REG      (0x3C)
#define NPU_OUTPUT_ADDR_0_REG     (0xBC)
#define NPU_APB_DBG_CTRL_REG      (0x13C)
#define NPU_APB_DBG_VALUE_REG     (0x140)
#define NPU_SRAM_RME_CTRL_REG     (0x144)
#define NPU_CALLBACK_ID_0_REG     (0x148)
#define NPU_CALLBACK_ID_1_REG     (0x14C)
#define NPU_CALLBACK_ID_2_REG     (0x150)
#define NPU_CALLBACK_ID_3_REG     (0x154)
#define NPU_CALLBACK_ACK_ID_0_REG (0x158)
#define NPU_CALLBACK_ACK_ID_1_REG (0x15C)
#define NPU_CALLBACK_ACK_ID_2_REG (0x160)
#define NPU_CALLBACK_ACK_ID_3_REG (0x164)

#ifdef AR9341
#define NPU_AXI_IN_ARCACHE_0_REG     (0x168)
#define NPU_AXI_IN_ARCACHE_1_REG     (0x16C)
#define NPU_AXI_IN_ARCACHE_2_REG     (0x170)
#define NPU_AXI_IN_ARCACHE_3_REG     (0x174)
#define NPU_AXI_OUT_AWCACHE_0_REG     (0x178)
#define NPU_AXI_OUT_AWCACHE_1_REG     (0x17C)
#define NPU_AXI_OUT_AWCACHE_2_REG     (0x180)
#define NPU_AXI_OUT_AWCACHE_3_REG     (0x184)

#define NPU_AXI_OUT_ARCACHE_0_REG    (0x188)
#define NPU_AXI_OUT_ARCACHE_1_REG    (0x18C)
#define NPU_AXI_OUT_ARCACHE_2_REG    (0x190)
#define NPU_AXI_OUT_ARCACHE_3_REG    (0x194)

#define NPU_CALLBACK_MASK_ARM_0_REG    (0x198)
#define NPU_CALLBACK_MASK_ARM_1_REG    (0x19C)
#define NPU_CALLBACK_MASK_ARM_2_REG    (0x1A0)
#define NPU_CALLBACK_MASK_ARM_3_REG    (0x1A4)
#define NPU_CALLBACK_MASK_CEVA_0_REG    (0x1A8)
#define NPU_CALLBACK_MASK_CEVA_1_REG    (0x1AC)
#define NPU_CALLBACK_MASK_CEVA_2_REG    (0x1B0)
#define NPU_CALLBACK_MASK_CEVA_3_REG    (0x1B4)

#define NPU_DMAC_BASE_ADDR_REG    (0x1B8)
#define NPU_DMAC_BASE_CTRL_REG    (0x1BC)
#define NPU_DMAC_SEC_BASE_ADDR_REG    (0x1C0)
#define NPU_DMAC_SEC_BASE_ADDR    0x0105006C


#define NPU_CURRENT_STATE_IDLE           (0x0)
#define NPU_CURRENT_STATE_SENDING_CB     (0x7)
#define NPU_CURRENT_STATE_WAITING_CB     (0x8)
#define NPU_CURRENT_STATE_HANGING        (0x9)
#define NPU_CURRENT_STATE_AFTER_HANGING  (0xA)
#define NPU_CURRENT_STATE_PAUSING        (0xB)
#define NPU_CURRENT_STATE_AFTER_PAUSING  (0xC)
#define NPU_CURRENT_STATE_ERROR          (0xD)
#define NPU_CURRENT_STATE_DONE           (0xE)
#else
#define NPU_TOTAL_PROCESS_CYCLES_REAL    (0x168)
#define NPU_AXI_CMD_WRITE_TOTAL_COUNT_RAEL   (0x16C)
#define NPU_AXI_CMD_READ_TOTAL_COUNT_REAL    (0x170)
#define NPU_AXI_DATA_WRITE_TOTAL_COUNT_REAL   (0x174)
#define NPU_AXI_DATA_READ_TOTAL_COUNT_REAL    (0x178)
#define NPU_TOTAL_PROCESS_CYCLES_INT    (0x17C)
#define NPU_AXI_CMD_WRITE_TOTAL_COUNT_INT   (0x180)
#define NPU_AXI_CMD_READ_TOTAL_COUNT_INT    (0x184)
#define NPU_AXI_DATA_WRITE_TOTAL_COUNT_INT   (0x188)
#define NPU_AXI_DATA_READ_TOTAL_COUNT_INT    (0x18C)

#define NPU_CALLBACK_MASK_ARM_0_REG    (0x198)
#define NPU_CALLBACK_MASK_ARM_1_REG    (0x19C)
#define NPU_CALLBACK_MASK_ARM_2_REG    (0x1A0)
#define NPU_CALLBACK_MASK_ARM_3_REG    (0x1A4)

#define NPU_DMAC_BASE_ADDR_REG    (0x1A8)
#define NPU_DMAC_BASE_CTRL_REG    (0x1AC)
#define NPU_DMAC_SEC_BASE_ADDR_REG    (0x1B0)
#define NPU_DMAC_SEC_BASE_ADDR    0x0842006C              //for 9311?

#define NPU_LOW_POWER_REG   (0x1BC)

#define NPU_CURRENT_STATE_IDLE           (0x0)
#define NPU_CURRENT_STATE_SENDING_CB     (0x8)
#define NPU_CURRENT_STATE_WAITING_CB     (0x9)
#define NPU_CURRENT_STATE_HANGING        (0xA)
#define NPU_CURRENT_STATE_AFTER_HANGING  (0xB)
#define NPU_CURRENT_STATE_WAITINF_CBUF_DUMP  (0xC)
#define NPU_CURRENT_STATE_PAUSING        (0xD)
#define NPU_CURRENT_STATE_AFTER_PAUSING  (0xE)
#define NPU_CURRENT_STATE_ERROR          (0xF)
#define NPU_CURRENT_STATE_DONE           (0x10)
#endif

#define NPU_IRQ_CBUF_READ_CONFLICT_BIT   (0x1 << 0) 
#define NPU_IRQ_DMAC_ERROR_BIT   (0x1 << 1)
#define NPU_IRQ_AXIM_BRESP_ERROR_BIT (0x1 << 2)
#define NPU_IRQ_ERROR_BIT         (NPU_IRQ_CBUF_READ_CONFLICT_BIT | NPU_IRQ_DMAC_ERROR_BIT | NPU_IRQ_AXIM_BRESP_ERROR_BIT) 
#define NPU_IRQ_DONE_BIT          (0x1 << 4)
#define NPU_IRQ_HANG_BIT          (0x1 << 5)
#define NPU_IRQ_CB_BIT            (0x1 << 6)
#define NPU_IRQ_PAUSE_BIT         (0x1 << 7)

#define NPU_IRQ_DONE_ARM_MASK          (0x1 << 4)
#define NPU_IRQ_HANG_ARM_MASK          (0x1 << 5)
#define NPU_IRQ_PAUSE_ARM_MASK         (0x1 << 7)
#ifdef AR9341
#define NPU_IRQ_DONE_CEVA_MASK         (0x1 << 12)
#define NPU_IRQ_HANG_CEVA_MASK         (0x1 << 13)
#define NPU_IRQ_PAUSE_CEVA_MASK        (0x1 << 15)
#endif

#define NPU_IRQ_DONE_CLR          (0x1 << 4)
#define NPU_IRQ_HANG_CLR          (0x1 << 5)
#define NPU_IRQ_PAUSE_CLR         (0x1 << 7)

#define NPU_LAYER_DEBUG_ON   0x1
#define NPU_LAYER_DEBUG_OFF  0x2

#define NPU_MAX_QUEUE_DEPTH  128

#define NPU_NETWORK_NORMAL    0
#define NPU_NETWORK_HIGH      1

//Record enough info for other thread to handle
struct ar_npu_work_status
{
    unsigned int irq_status;
    unsigned short network_id;
    unsigned short frame_id;
    unsigned int priority;
    unsigned int layer_num;
    unsigned int restart_addr;
    unsigned int scu_addr;
    unsigned int scu_size;
    unsigned int runtime_addr;
    unsigned int weight_addr;
    unsigned int input_addr[MAX_INPUT_NUM];
    unsigned int output_addr[MAX_OUTPUT_NUM];
    unsigned int sram_addr;
    unsigned int sram_size;
    //callback irq is a special irq, dla will continue running after send cb irq, it'll probably trigger another irq soon,
    //so save callback irq here
    unsigned int callback_irq;
    unsigned int callback_id[4];
    //each model has a done flag
    struct completion done;
    struct completion high_done;
};

struct ar_npu_dev
{
    void __iomem * base;
    void __iomem * global_ctrl;
    unsigned int frequency;
    unsigned int dma_ctrl;
    unsigned int npu_sec_ctrl;
    unsigned int cb_to_arm;
    struct miscdevice miscdev;
    unsigned int irq;
    struct proc_dir_entry * proc_dir;
    wait_queue_head_t wait_queue;
    unsigned long long time_start; 	//for npu usage, record the start time
    unsigned long long time_running;
    unsigned long long time_tic; //for npu usage, record the time when start and resume
    unsigned long long time_toc; //for npu usage, record the time when done and hang
    unsigned int cb_come;
    unsigned int debug_come;
    unsigned int id_bmp[2048]; //each bit for a net id, so max 2048 x 32bit = 65536.
    struct mutex lock;
    struct completion idle_wait;
    unsigned int suspended;
};

struct ar_npu_usage
{
    unsigned long long time_total;
    unsigned long long time_running;
    unsigned long long time_idle;
};

int _ar_npu_poweron(void);
void _ar_npu_poweroff(void);
int _ar_npu_start(AR_NPU_IOCTL_CFG_S * cfg);
int _ar_npu_hang(AR_NPU_IOCTL_HANG_S * hang);
//int _ar_npu_resume(AR_NPU_IOCTL_RESUME_S * resume);
int _ar_npu_query(AR_NPU_IOCTL_STATUS_S * status);
irqreturn_t _ar_npu_irq_handler(int irq, void * dev);
int _ar_npu_normal_thread(void * args);
int _ar_npu_high_thread(void * args);
int _ar_npu_register_cb(AR_NPU_IOCTL_CB_S * pstCB);
int _ar_npu_register_dsp_cb(AR_NPU_IOCTL_CB_S * pstCB);
int _ar_npu_cb_done(AR_NPU_IOCTL_CB_DONE_S * cb_done);
int _ar_npu_debug_continue(AR_NPU_IOCTL_DEBUG_S * debug);
int _ar_npu_sw_reset(void);
int _ar_npu_set_cache_coherency(unsigned int cache_enable);
unsigned int _ar_npu_is_pausing_or_waitingcb(void);
int _ar_npu_clean_resource(unsigned short network_id);
int _ar_npu_set_frequency(unsigned int frequency);
int _ar_npu_set_timeout(unsigned int timeout);
int _ar_npu_set_security(unsigned int security);
int _ar_npu_get_io_addr(AR_NPU_IOCTL_IO_ADDR_S * io_addr);
int _ar_npu_get_cbuf_addr(AR_NPU_IOCTL_CBUF_ADDR_S * cbuf_addr);
void _ar_npu_status_init(void);
int _ar_npu_stop_and_reset(void);
int _ar_npu_alloc_net_id(unsigned short * p_network_id);
int _ar_npu_set_net_id(unsigned short network_id);
int _ar_npu_clear_net_id(unsigned short network_id);

int ar_npu_proc_state_show(struct seq_file *s, void *p);
int ar_npu_proc_fifo_show(struct seq_file *s, void *p);
int ar_npu_proc_result_show(struct seq_file *s, void *p);
int ar_npu_proc_usage_show(struct seq_file *s, void *p);

#endif
