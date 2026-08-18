//This is the HAL for DLA
#include <asm/barrier.h>
#include <asm/io.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#if defined(CONFIG_ION) 
#include <linux/artosyn_ion.h>
#else
#include <linux/mman.h>
#include <osal_mmz.h>
#endif

#include "ar_npu.h"
#include "ar_npu_hal.h"

//static DEFINE_MUTEX(g_npu_lock);
static DEFINE_SPINLOCK(g_npu_status_lock); //only protect write.
static DEFINE_MUTEX(g_fifo_normal_lock);
static DEFINE_MUTEX(g_fifo_high_lock);
static DEFINE_KFIFO(g_npu_normal_fifo, AR_NPU_IOCTL_CFG_S, NPU_MAX_QUEUE_DEPTH);
static DEFINE_KFIFO(g_npu_high_fifo, AR_NPU_IOCTL_CFG_S, NPU_MAX_QUEUE_DEPTH);
static DEFINE_SEMAPHORE(g_normal_sem);
static DEFINE_SEMAPHORE(g_high_sem);

#ifdef AR9341
#define NPU_CHECK_STATE(irq_status, state) ( (state) == ( ( (irq_status) >> 28 ) & 0x0F ) )
#else
#define NPU_CHECK_STATE(irq_status, state) ( (state) == ( ( (irq_status) >> 27 ) & 0x1F ) )
#endif
#define NPU_DEFAULT_TIMEOUT  (1 * HZ)

#define ALIGN16KB        (16384)
#define ROUND_UP_ALIGNED(x, N)  ( ((x)/(N) + 2) * (N) )
#define ADDR_ALIGNED(addr, N)   ( (addr) % (N) == 0 ? (addr) : (((addr) / (N) + 1) * (N)) )
#define AR_ION_SYSTEM_HEAP_ID (27)

#define NPU_SUPPORT_PRIORITY

#define NPU_STATE_TO_STRING(x) \
((x) == 0 ? "IDLE" : (x) == 1 ? "Write SRAM finish" :\
(x) == 2 ? "Write branch and save debug reg" : (x) == 3 ? "Sram single layer write complete" :\
(x) == 4 ? "Wait IRQ and write sram" : (x) == 5 ? "Sram judge" : \
(x) == 6 ? "Layer complete" : (x) == 7 ? "Sending callback" : \
(x) == 8 ? "Waiting callback" : (x) == 9 ? "Hanging" : \
(x) == 0xa ? "After hanging" : (x) == 0xb ? "Pausing" : \
(x) == 0xc ? "Error detect" : "DONE")

unsigned int npu_timeout_factor =10;
extern unsigned char scu_ddr_bin[];
extern unsigned int scu_ddr_bin_len;
extern unsigned char param_bin[];
extern unsigned int param_bin_len;
extern struct ar_npu_dev npu_dev;
extern struct task_struct * g_normal_thread;
extern struct task_struct * g_high_thread;
struct ar_npu_work_status g_npu_status[NPU_MAX_QUEUE_DEPTH] = {0};

struct ar_npu_work_status * _ar_npu_get_status(unsigned short network_id);
#ifdef AR9341
//2022-04-22, we need to check if cpu is in NS state, but SCR can only be accessed from secure state.
//so we check 01074100 sec_boot_en bit instead.
//we suppose that cpu would enter NS state if sec boot is enabled, otherwise cpu enters secure state.
static int _ar_npu_is_cpu_nonsec(void __iomem * global_ctrl)
{
    int val = 0;
    val = readl(global_ctrl + SYS_BOOT_MODE_REG);
    val = (val & 0x40000000) >> 30; //bit 30 = 1 means sec boot mode, kernel will run in NS state.

    return val;
}

static void _ar_npu_hw_irq_mask(void __iomem * base, int cb_to_arm)
{
    unsigned int mask = readl(base + NPU_IRQ_MASK_REG);

    mask |= NPU_IRQ_DONE_CEVA_MASK | NPU_IRQ_HANG_CEVA_MASK | NPU_IRQ_PAUSE_CEVA_MASK;
    writel(mask, base + NPU_IRQ_MASK_REG);

    if(cb_to_arm)
    {
        printk(KERN_INFO"Set CB to ARM.\n");
        //enable all callback to arm
        writel(0, base + NPU_CALLBACK_MASK_ARM_0_REG);
        writel(0, base + NPU_CALLBACK_MASK_ARM_1_REG);
        writel(0, base + NPU_CALLBACK_MASK_ARM_2_REG);
        writel(0, base + NPU_CALLBACK_MASK_ARM_3_REG);

        //disable all callback to ceva
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_CEVA_0_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_CEVA_1_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_CEVA_2_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_CEVA_3_REG);
    }
    else
    {
        //enable all callback to arm
        printk(KERN_INFO"Set CB to CEVA.\n");
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_ARM_0_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_ARM_1_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_ARM_2_REG);
        writel(0xFFFFFFFF, base + NPU_CALLBACK_MASK_ARM_3_REG);

        //disable all callback to ceva
        writel(0, base + NPU_CALLBACK_MASK_CEVA_0_REG);
        writel(0, base + NPU_CALLBACK_MASK_CEVA_1_REG);
        writel(0, base + NPU_CALLBACK_MASK_CEVA_2_REG);
        writel(0, base + NPU_CALLBACK_MASK_CEVA_3_REG);
    }
}

static void _ar_npu_hw_cb_mask(void __iomem * base, unsigned int to_arm,
    unsigned int id0, unsigned int id1, unsigned int id2, unsigned int id3)
{
    unsigned int val = 0;
    //Has configured already in probe, based on the dts setting.
    //later, we may set the mask to ceva/arm based on each callback.
    if(to_arm)
    {
        val = readl(base + NPU_CALLBACK_MASK_ARM_0_REG);
        val &= ~id0;
        writel(val, base + NPU_CALLBACK_MASK_ARM_0_REG);
        val = readl(base + NPU_CALLBACK_MASK_CEVA_0_REG);
        val |= id0;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_0_REG);

        val = readl(base + NPU_CALLBACK_MASK_ARM_1_REG);
        val &= ~id1;
        writel(val, base + NPU_CALLBACK_MASK_ARM_1_REG);
        val = readl(base + NPU_CALLBACK_MASK_CEVA_1_REG);
        val |= id1;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_1_REG);

        val = readl(base + NPU_CALLBACK_MASK_ARM_2_REG);
        val &= ~id2;
        writel(val, base + NPU_CALLBACK_MASK_ARM_2_REG);
        val = readl(base + NPU_CALLBACK_MASK_CEVA_2_REG);
        val |= id2;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_2_REG);

        val = readl(base + NPU_CALLBACK_MASK_ARM_3_REG);
        val &= ~id3;
        writel(val, base + NPU_CALLBACK_MASK_ARM_3_REG);
        val = readl(base + NPU_CALLBACK_MASK_CEVA_3_REG);
        val |= id3;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_3_REG);
    }
    else //to dsp
    {
        val = readl(base + NPU_CALLBACK_MASK_CEVA_0_REG);
        val &= ~id0;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_0_REG);
	    //clear arm mask
	    val = readl(base + NPU_CALLBACK_MASK_ARM_0_REG);
        val |= id0;
        writel(val, base + NPU_CALLBACK_MASK_ARM_0_REG);

        val = readl(base + NPU_CALLBACK_MASK_CEVA_1_REG);
        val &= ~id1;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_1_REG);
        val = readl(base + NPU_CALLBACK_MASK_ARM_1_REG);
        val |= id1;
        writel(val, base + NPU_CALLBACK_MASK_ARM_1_REG);

        val = readl(base + NPU_CALLBACK_MASK_CEVA_2_REG);
        val &= ~id2;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_2_REG);
        val = readl(base + NPU_CALLBACK_MASK_ARM_2_REG);
        val |= id2;
        writel(val, base + NPU_CALLBACK_MASK_ARM_2_REG);

        val = readl(base + NPU_CALLBACK_MASK_CEVA_3_REG);
        val &= ~id3;
        writel(val, base + NPU_CALLBACK_MASK_CEVA_3_REG);
        val = readl(base + NPU_CALLBACK_MASK_ARM_3_REG);
        val |= id3;
        writel(val, base + NPU_CALLBACK_MASK_ARM_3_REG);

    }

}

#else
static void _ar_npu_hw_irq_mask(void __iomem * base, int cb_to_arm)
{
    
	printk(KERN_INFO"Set CB to ARM.\n");
	//enable all callback to arm
	writel(0, base + NPU_CALLBACK_MASK_ARM_0_REG);
	writel(0, base + NPU_CALLBACK_MASK_ARM_1_REG);
	writel(0, base + NPU_CALLBACK_MASK_ARM_2_REG);
	writel(0, base + NPU_CALLBACK_MASK_ARM_3_REG);

}

static void _ar_npu_hw_cb_mask(void __iomem * base, unsigned int to_arm,
    unsigned int id0, unsigned int id1, unsigned int id2, unsigned int id3)
{
    unsigned int val = 0;
    //Has configured already in probe, based on the dts setting.
    //later, we may set the mask to arm based on each callback.

	val = readl(base + NPU_CALLBACK_MASK_ARM_0_REG);
	val &= ~id0;
	writel(val, base + NPU_CALLBACK_MASK_ARM_0_REG);

	val = readl(base + NPU_CALLBACK_MASK_ARM_1_REG);
	val &= ~id1;
	writel(val, base + NPU_CALLBACK_MASK_ARM_1_REG);

	val = readl(base + NPU_CALLBACK_MASK_ARM_2_REG);
	val &= ~id2;
	writel(val, base + NPU_CALLBACK_MASK_ARM_2_REG);

	val = readl(base + NPU_CALLBACK_MASK_ARM_3_REG);
	val &= ~id3;
	writel(val, base + NPU_CALLBACK_MASK_ARM_3_REG);
	
}
#endif

extern int npu_sram_size; 
static void _ar_npu_hw_start(void __iomem * base, AR_NPU_IOCTL_CFG_S * cfg)
{
    int i = 0;
    unsigned int val = 0;
    struct ar_npu_work_status * p_status = NULL;

    if(cfg->u32SCUAddrPhy < 0x20000000)
    {
        printk("ERR: scu addr is invalid: [0x%x]!\n", cfg->u32SCUAddrPhy);
        return;
    }
	// disable addr check for tiny networks without runtime config
	#if 0
    if(cfg->u32RuntimeAddrPhy < 0x20000000)
    {
        printk("ERR: runtime addr is invalid: [0x%x]!\n", cfg->u32RuntimeAddrPhy);
        return;
    }
	#endif
    if(cfg->u32WeightsAddrPhy < 0x20000000)
    {
        printk("ERR: weight addr is invalid: [0x%x]!\n", cfg->u32WeightsAddrPhy);
        return;
    }
    if(cfg->au32InputAddrPhy[0] < 0x20000000)
    {
        printk("ERR: input addr is invalid: [0x%x]!\n", cfg->au32InputAddrPhy[0]);
        return;
    }
    if(cfg->au32OutputAddrPhy[0] < 0x20000000)
    {
        printk("ERR: output addr is invalid: [0x%x]!\n", cfg->au32OutputAddrPhy[0]);
        return;
    }

    writel(cfg->u32SCUAddrPhy, base + NPU_COMMAND_ADDR_REG);
    writel(cfg->u32SCUSize, base + NPU_COMMAND_LEN_REG);
    writel(cfg->u32RuntimeAddrPhy, base + NPU_RUNTIME_ADDR_REG);
    writel(cfg->u32WeightsAddrPhy, base + NPU_PARAMS_ADDR_REG);

    for(i = 0; i < MAX_INPUT_NUM; i++)
    {
        if(cfg->au32InputAddrPhy[i])
        {
            writel(cfg->au32InputAddrPhy[i], base + NPU_INPUT_ADDR_0_REG + i * 4);
        }

        if(cfg->au32OutputAddrPhy[i])
        {
            writel(cfg->au32OutputAddrPhy[i], base + NPU_OUTPUT_ADDR_0_REG + i * 4);
        }
    }

    //writel(cfg->u32SramAddrPhy, base + NPU_SRAM_ADDR_REG);
    //writel(cfg->u32SramSize, base + NPU_SRAM_SIZE_REG);

    writel(NPU_SRAM_BASE_ADDR, base + NPU_SRAM_ADDR_REG);
    writel(npu_sram_size, base + NPU_SRAM_SIZE_REG);

    //set frame id network id etc..
    writel(cfg->u16NetworkId | (cfg->u16FrameId << 16), base + NPU_PROC_NET_FRAME_ID_REG);

    //clear completion in case it's been set DONE last time but nobody waits for it.
    reinit_completion(&npu_dev.idle_wait);
    p_status = _ar_npu_get_status(cfg->u16NetworkId);
    if(p_status)
    {
        reinit_completion(&p_status->done);
        reinit_completion(&p_status->high_done);
    }

    //before start, should clear hang bit, so we just from '0' to start.
    val = 0;
    if(cfg->u32Debug == NPU_LAYER_DEBUG_ON)
    {
        printk(KERN_INFO"==Set debug mode..\n");
#ifdef AR9341
        writel(val | 0x9, base + NPU_START_REG);
#else
        cfg->u32CbufAddrPhy = (cfg->u32SCUAddrPhy + cfg->u32SCUSize + 0xFF) & 0xFFFFFF00;
        val = 0x69 | ((cfg->u32CbufAddrPhy>>8 )<<7);
        writel(val, base + NPU_START_REG);
#endif
    }
    else
    {
        writel(val | 0x1, base + NPU_START_REG);
    }
}

static int _ar_npu_hw_hang(void __iomem * base)
{
    unsigned int reg = 0;
    unsigned short network_id, frame_id;

    reg = readl(base + NPU_START_REG);
    reg |= 0x4; //bit 2 for hang
    writel(reg, base + NPU_START_REG);

    reg = readl(base + NPU_IRQ_NET_FRAME_ID_REG);
    network_id = reg & 0x0000FFFF;
    frame_id = (reg & 0xFFFF0000) >> 16;

    //return the restart addr
    printk(KERN_INFO"NPU hangs at layer: %d of net %d frame %d\n",
        readl(base + NPU_CMPLT_LAYER_NUM_REG), network_id, frame_id);

    return 0;
}

static int _ar_npu_hw_resume(void __iomem * base, struct ar_npu_work_status * p_status)
{
    //recalculate the scu and size, then start dla
    AR_NPU_IOCTL_CFG_S cfg = {0};

    printk(KERN_INFO"==%s %d: net %d restart %x size %x.\n", __FUNCTION__, __LINE__,
        p_status->network_id, p_status->restart_addr, cfg.u32SCUSize);

    cfg.u32Debug = NPU_LAYER_DEBUG_OFF;
    cfg.u32SCUAddrPhy = p_status->restart_addr;
    cfg.u32SCUSize = p_status->scu_size - (p_status->restart_addr - p_status->scu_addr);
    cfg.u32RuntimeAddrPhy = p_status->runtime_addr;
    cfg.u32WeightsAddrPhy = p_status->weight_addr;
    memcpy(cfg.au32InputAddrPhy, p_status->input_addr, sizeof(cfg.au32InputAddrPhy));
    memcpy(cfg.au32OutputAddrPhy, p_status->output_addr, sizeof(cfg.au32OutputAddrPhy));
    cfg.u32SramAddrPhy = p_status->sram_addr;
    cfg.u32SramSize = p_status->sram_size;
    cfg.u16NetworkId = p_status->network_id;
    cfg.u16FrameId = p_status->frame_id;

    _ar_npu_hw_start(base, &cfg);

    return 0;
}

static int _ar_npu_hw_wait_idle(void __iomem * base)
{
    int ret = 0;
    ret = wait_for_completion_timeout(&npu_dev.idle_wait, NPU_DEFAULT_TIMEOUT*npu_timeout_factor);
    if(ret <= 0)
    {
        printk(KERN_ERR"Wait for NPU IDLE timeout.\n");
        return -ETIME;
    }

    return 0;
}

static int _ar_npu_hw_cb_done(void __iomem * base, AR_NPU_IOCTL_CB_DONE_S * cb_done)
{
    unsigned short network_id;

    network_id = readl(base + NPU_IRQ_NET_FRAME_ID_REG) & 0x0000FFFF;

    if(network_id != cb_done->u16NetworkId)
    {
        printk(KERN_ERR"Current network id mismatch: %d/%d.", network_id, cb_done->u16NetworkId);
        return -1;
    }

    if(!cb_done->au32CBAckIDs[0] && !cb_done->au32CBAckIDs[1] && !cb_done->au32CBAckIDs[2] && !cb_done->au32CBAckIDs[3])
    {
        printk(KERN_ERR"Recv invalid cb id in cb_done!\n");
        return -1;
    }
    
    writel(cb_done->au32CBAckIDs[0], base + NPU_CALLBACK_ACK_ID_0_REG);
    writel(cb_done->au32CBAckIDs[1], base + NPU_CALLBACK_ACK_ID_1_REG);
    writel(cb_done->au32CBAckIDs[2], base + NPU_CALLBACK_ACK_ID_2_REG);
    writel(cb_done->au32CBAckIDs[3], base + NPU_CALLBACK_ACK_ID_3_REG);

    return 0;
}

static int _ar_npu_hw_debug_continue(void __iomem * base, AR_NPU_IOCTL_DEBUG_S * debug)
{
    unsigned short network_id;
    unsigned int val;

    network_id = readl(base + NPU_IRQ_NET_FRAME_ID_REG) & 0x0000FFFF;

    if(network_id != debug->u16NetworkId)
    {
        printk(KERN_ERR"Current network id mismatch: %d/%d.", network_id, debug->u16NetworkId);
        return -1;
    }

    val = readl(base + NPU_START_REG);

    if(debug->u32Debug == NPU_LAYER_DEBUG_ON)
    {
    
#ifdef AR9341
        writel(val | 0x9, base + NPU_START_REG);
#else
        if(debug->u32DumpEnable)
        {
            val = (val & 0x7FFFFF80) | 0x29;
        }
        else
        {
            val = (val & 0x7FFFFF80) | 0x9;
        }
        writel(val, base + NPU_START_REG);
        //printk("NPU_LAYER_DEBUG_ON: val = 0x%x\n", val);
#endif
    }
    else
    {
        writel(val | 0x1, base + NPU_START_REG);
    }

    npu_dev.debug_come = 0;

    return 0;
}

static unsigned int _ar_npu_hw_get_layer_num(void __iomem * base)
{
    return readl(base + NPU_CMPLT_LAYER_NUM_REG);
}

#if 0
static unsigned int _ar_npu_hw_get_curr_state(void __iomem * base)
{
    return (readl(base + NPU_IRQ_STATUS_REG) >> 28 ) & 0x0F;
}
#endif
static int _ar_npu_hw_reset(void __iomem * global_ctrl, void __iomem * base)
{
    unsigned int reset_val;
    unsigned int reset_mask;
    //send idle request
    writel(0x2, base + NPU_START_REG);

    printk("==waitidle==\n");

    while((readl(base + NPU_START_REG) & 0x10) == 0)
    {
        msleep(2);
    }

    printk("==reset==\n");

#ifdef AR9341
    //reset
    writel(0xfffffffc, global_ctrl + NPU_RESET_CTRL);
    msleep(5);
    writel(0xffffffff, global_ctrl + NPU_RESET_CTRL);
#else
    reset_mask = readl(global_ctrl + NPU_RESET_MASK_0);
    reset_mask &= 0xf7ffffff;
    writel(reset_mask, global_ctrl + NPU_RESET_MASK_0);
    reset_val = readl(global_ctrl + NPU_RESET_LEVEL_0);
    reset_val &= 0xf7ffffff;
    writel(reset_val, global_ctrl + NPU_RESET_LEVEL_0);

    reset_mask = readl(global_ctrl + NPU_RESET_MASK_1);
    reset_mask &= 0xffffdfff;
    writel(reset_mask, global_ctrl + NPU_RESET_MASK_1);
    reset_val = readl(global_ctrl + NPU_RESET_LEVEL_1);
    reset_val &= 0xffffdfff;
    writel(reset_val, global_ctrl + NPU_RESET_LEVEL_1);
    
    msleep(5);
    
    reset_mask = readl(global_ctrl + NPU_RESET_MASK_0);
    reset_mask |= 0x08000000;
    writel(reset_mask, global_ctrl + NPU_RESET_MASK_0);
    reset_val = readl(global_ctrl + NPU_RESET_LEVEL_0);
    reset_val |= 0x08000000;
    writel(reset_val, global_ctrl + NPU_RESET_LEVEL_0);

    reset_mask = readl(global_ctrl + NPU_RESET_MASK_1);
    reset_mask |= 0x2000;
    writel(reset_mask, global_ctrl + NPU_RESET_MASK_1);
    reset_val = readl(global_ctrl + NPU_RESET_LEVEL_1);
    reset_val |= 0x2000;
    writel(reset_val, global_ctrl + NPU_RESET_LEVEL_1);
#endif
    printk("==resetdone==\n");

    return 0;
}

void _ar_npu_status_init()
{
    int i = 0;
    for(i = 0; i < NPU_MAX_QUEUE_DEPTH; i++)
    {
        init_completion(&g_npu_status[i].done);
        init_completion(&g_npu_status[i].high_done);
    }
}
int _ar_npu_wait_done(unsigned short network_id, unsigned short frame_id);

void _ar_npu_warmup(void)
{
#if defined(CONFIG_ION)
    int ret = 0;
    int in_fd = 0, scu_fd = 0, param_fd = 0, run_fd = 0;
    int aligned_size = 0;
#else
    hil_mmb_t * in_mmb = NULL;
    hil_mmb_t * scu_mmb = NULL;
    hil_mmb_t * param_mmb = NULL;
    hil_mmb_t * run_mmb = NULL;
    struct mmb_info mi;
#endif
    char name[16] = {0}; 
    int in_len = 3072;
    int scu_len = scu_ddr_bin_len;
    int param_len = param_bin_len;
    int runtime_len = 736;

    uint64_t in_va = 0, in_pa = 0;
    //uint64_t out_va, out_pa;
    uint64_t scu_va = 0, scu_pa = 0;
    uint64_t scu_va_al = 0, scu_pa_al = 0;
    uint64_t param_va = 0, param_pa = 0;
    uint64_t param_va_al = 0, param_pa_al = 0;
    uint64_t run_va = 0, run_pa = 0;

    AR_NPU_IOCTL_CFG_S cfg = {0};

    strcpy(name, "warmup");

#if defined(CONFIG_ION) 
    //0. malloc ion mem(input output param scu runtime)
    ret = artosyn_client_create(name);
    if (ret < 0)
    {
    	printk(KERN_ERR"npu creat ion failded");
        return;
    }

    //input
    aligned_size = ROUND_UP_ALIGNED(in_len, ALIGN16KB);
    in_fd = artosyn_alloc_buffer(name, aligned_size, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                (void **)&in_va, (void **)&in_pa);
    if (in_fd < 0 || !in_va || !in_pa)
    {
    	printk(KERN_ERR"alloc npu in ion failded");
        artosyn_destroy_client(name);
    	return;
    }
    in_pa = ADDR_ALIGNED(in_pa, ALIGN16KB);

    //aligned_size = ROUND_UP_ALIGNED(scu_len, ALIGN16KB);
    scu_fd = artosyn_alloc_buffer(name, scu_len, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                     (void **)&scu_va, (void **)&scu_pa);
    if (scu_fd < 0 || !scu_va || !scu_pa)
    {
    	printk(KERN_ERR"alloc npu scu ion failded");
        artosyn_free_buffer(name, in_fd);
        artosyn_destroy_client(name);
        return;
    }
    //scu_pa_al = ADDR_ALIGNED(scu_pa, ALIGN16KB);
    //scu_va_al = scu_va + (scu_pa_al - scu_pa);
    scu_pa_al = scu_pa;
    scu_va_al = scu_va;

    aligned_size = ROUND_UP_ALIGNED(param_len, ALIGN16KB);
    param_fd = artosyn_alloc_buffer(name, aligned_size, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                     (void **)&param_va, (void **)&param_pa);
    if (param_fd < 0 || !param_va || !param_pa)
    {
    	printk(KERN_ERR"alloc npu param ion failded");
        artosyn_free_buffer(name, in_fd);
        artosyn_free_buffer(name, scu_fd);
        artosyn_destroy_client(name);
        return;
    }
    param_pa_al = ADDR_ALIGNED(param_pa, ALIGN16KB);
    param_va_al = param_va + (param_pa_al - param_pa);

    aligned_size = ROUND_UP_ALIGNED(runtime_len, ALIGN16KB);
    run_fd = artosyn_alloc_buffer(name, aligned_size, 0x1000, (1<<AR_ION_SYSTEM_HEAP_ID), 0,
                                     (void **)&run_va, (void **)&run_pa);
    if (run_fd < 0 || !run_va || !run_pa)
    {
    	printk(KERN_ERR"alloc npu run ion failded");
        artosyn_free_buffer(name, in_fd);
        artosyn_free_buffer(name, scu_fd);
        artosyn_free_buffer(name, param_fd);
        artosyn_free_buffer(name, run_fd);
        artosyn_destroy_client(name);
        return;
    }
    run_pa = ADDR_ALIGNED(run_pa, ALIGN16KB);
#else
    // alloc buffer from mmz
    memset(&mi, 0, sizeof(mi));
    mi.flags = MAP_SHARED;
    mi.prot = PROT_READ | PROT_WRITE;
    mi.align = ALIGN16KB;

    //input
    //aligned_size = ROUND_UP_ALIGNED(in_len, ALIGN16KB);
    mi.size = in_len;
    strncpy(mi.mmb_name, "in_mmz", HIL_MMB_NAME_LEN - 1);
    in_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
                                          mi.gfp, mi.mmz_name, mi.order);
    if (!in_mmb)
    {
        printk(KERN_ERR"alloc MMB faild");
        return;
    }
    in_pa = hil_mmb_phys(in_mmb);
    in_va = (uint64_t)hil_mmb_map2kern(in_mmb);
    if (!in_va)
    {
        printk(KERN_ERR"map2kern failed");
        hil_mmb_free(in_mmb);
        return;
    }

    //scu
    mi.size = scu_len;
    strncpy(mi.mmb_name, "scu_mmz", HIL_MMB_NAME_LEN - 1);
    scu_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
                                          mi.gfp, mi.mmz_name, mi.order);
    if (!scu_mmb)
    {
        printk(KERN_ERR"alloc MMB faild");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        return;
    }
    scu_pa = hil_mmb_phys(scu_mmb);
    scu_va = (uint64_t)hil_mmb_map2kern(scu_mmb);
    if (!scu_va)
    {
        printk(KERN_ERR"map2kern failed");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        hil_mmb_free(scu_mmb);
        return;
    }
    scu_pa_al = scu_pa;
    scu_va_al = scu_va;

    //param
    mi.size = param_len;
    strncpy(mi.mmb_name, "param_mmz", HIL_MMB_NAME_LEN - 1);
    param_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
                                          mi.gfp, mi.mmz_name, mi.order);
    if (!param_mmb)
    {
        printk(KERN_ERR"alloc MMB faild");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        hil_mmb_unmap(scu_mmb);
        hil_mmb_free(scu_mmb);
        return;
    }
    param_pa = hil_mmb_phys(param_mmb);
    param_va = (uint64_t)hil_mmb_map2kern(param_mmb);
    if (!param_va)
    {
        printk(KERN_ERR"map2kern failed");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        hil_mmb_unmap(scu_mmb);
        hil_mmb_free(scu_mmb);
        hil_mmb_free(param_mmb);
        return;
    }
    param_pa_al = param_pa;
    param_va_al = param_va;

    //runtime
    mi.size = runtime_len;
    strncpy(mi.mmb_name, "run_mmz", HIL_MMB_NAME_LEN - 1);
    run_mmb = hil_mmb_alloc_v2(mi.mmb_name, mi.size, mi.align,
                                          mi.gfp, mi.mmz_name, mi.order);
    if (!run_mmb)
    {
        printk(KERN_ERR"alloc MMB faild");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        hil_mmb_unmap(scu_mmb);
        hil_mmb_free(scu_mmb);
        hil_mmb_unmap(param_mmb);
        hil_mmb_free(param_mmb);
        return;
    }
    run_pa = hil_mmb_phys(run_mmb);
    run_va = (uint64_t)hil_mmb_map2kern(run_mmb);
    if (!run_va)
    {
        printk(KERN_ERR"map2kern failed");
        hil_mmb_unmap(in_mmb);
        hil_mmb_free(in_mmb);
        hil_mmb_unmap(scu_mmb);
        hil_mmb_free(scu_mmb);
        hil_mmb_unmap(param_mmb);
        hil_mmb_free(param_mmb);
        hil_mmb_free(run_mmb);
        return;
    }
#endif
    //1. cp scu and param
    memcpy((unsigned char *)scu_va_al, scu_ddr_bin, scu_len);
    memcpy((unsigned char *)param_va_al, param_bin, param_len);

    //2. start npu and wait done.
    cfg.u32SCUAddrPhy = scu_pa_al;
    cfg.u32SCUSize = scu_len;
    cfg.u32RuntimeAddrPhy = run_pa;
    cfg.u32WeightsAddrPhy = param_pa_al;
    cfg.au32InputAddrPhy[0] = in_pa;
    //no output, just set a address for api to check
    cfg.au32OutputAddrPhy[0] = 0x20000000;
    cfg.u16FrameId = 0xBEEF;
    cfg.u16NetworkId = 0xFEED;
    g_npu_status[0].network_id = cfg.u16NetworkId;
    _ar_npu_hw_start(npu_dev.base, &cfg);
    _ar_npu_wait_done(cfg.u16NetworkId, cfg.u16FrameId);

    //3. free
#if defined(CONFIG_ION)
    artosyn_free_buffer(name, in_fd);
    artosyn_free_buffer(name, scu_fd);
    artosyn_free_buffer(name, param_fd);
    artosyn_free_buffer(name, run_fd);

    artosyn_destroy_client(name);
#else
    hil_mmb_unmap(in_mmb);
    hil_mmb_free(in_mmb);
    hil_mmb_unmap(scu_mmb);
    hil_mmb_free(scu_mmb);
    hil_mmb_unmap(param_mmb);
    hil_mmb_free(param_mmb);
    hil_mmb_unmap(run_mmb);
    hil_mmb_free(run_mmb);
#endif

    printk("NPU Ready.\n");

}

int _ar_npu_poweron(void)
{
    unsigned int val = 0;
    void __iomem * sec_base = NULL;
    unsigned int reset_val = 0;
    unsigned int reset_mask = 0;
	
    //0. hold reset
#ifdef AR9341    
    writel(0xfffffffc, npu_dev.global_ctrl + NPU_RESET_CTRL);
#else
    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_mask &= 0xf7ffffff;
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_0);
    reset_val &= 0xf7ffffff;
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_0);

    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_mask &= 0xffffdfff;
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_1);
    reset_val &= 0xffffdfff;
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_1);
#endif
    msleep(5);

    //1. set freq
    _ar_npu_set_frequency(npu_dev.frequency);

    //2. Power on and reset DLA
#ifdef AR9341 
    val = readl(npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
    val |= 0x1C0; //bit 8,7,6
    writel(0xACCE55, npu_dev.global_ctrl + NPU_PMU_CTRL_ACCESS);
    writel(val, npu_dev.global_ctrl + NPU_PMU_CTRL_REG);

    //release reset
    msleep(5);
    writel(0xffffffff, npu_dev.global_ctrl + NPU_RESET_CTRL);
#else
    //power on
    val = readl(npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
    val &= ~(1<<21); //iso off
    val |= (1<<5); //power on
    val |= (1<<13);
    writel(0xACCE55, npu_dev.global_ctrl + NPU_PMU_CTRL_ACCESS);
    writel(val, npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
    msleep(5);
    
    //release reset
    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_mask |= 0x08000000;
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_0);
    reset_val |= 0x08000000;
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_0);

    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_mask |= 0x2000;
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_1);
    reset_val |= 0x2000;
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_1);
#endif
    msleep(5);

    //3. set IRQ mask
    _ar_npu_hw_irq_mask(npu_dev.base, npu_dev.cb_to_arm);
	
#if 1
    //set dma channel and base
    writel(DMAC_BASE_ADDR, npu_dev.base + NPU_DMAC_BASE_ADDR_REG);
	writel(NPU_DMAC_SEC_BASE_ADDR, npu_dev.base + NPU_DMAC_SEC_BASE_ADDR_REG);
    writel(npu_dev.dma_ctrl, npu_dev.base + NPU_DMAC_BASE_CTRL_REG);
#endif
    //4. tmp force all sram power on
    writel(0x20000, npu_dev.base + NPU_SRAM_RME_CTRL_REG);

    //only access in secure state
#ifdef AR9341 
    if(!_ar_npu_is_cpu_nonsec(npu_dev.global_ctrl))
    {
        if(npu_dev.npu_sec_ctrl==1)
        {
            sec_base = ioremap_nocache(NPU_SECURITY_BASE, NPU_REG_SPACE_RANGE);
            if(!sec_base)
            {
                return -ENOMEM;
            }
            unsigned int val = readl(sec_base + NPU_SECURITY_MODE_REG)& 0x0003FFFC;
            writel(val, sec_base + NPU_SECURITY_MODE_REG);
        }
    }
#else
	 if(npu_dev.npu_sec_ctrl==1)
     {
          sec_base = ioremap_nocache(NPU_SECURITY_BASE, NPU_REG_SPACE_RANGE);
          if(!sec_base)
          {
              return -ENOMEM;
          }
          unsigned int val = readl(sec_base + NPU_SECURITY_MODE_REG)& 0x0003FFFC;
          writel(val, sec_base + NPU_SECURITY_MODE_REG);
     }
#endif

#ifdef AR9341
   _ar_npu_warmup();
#endif
	return 0;
}

void _ar_npu_poweroff(void)
{
    unsigned int val = 0;
    unsigned int reset_val = 0;
    unsigned int reset_mask = 0;
    
    printk(KERN_INFO"Power off NPU\n");
    //Power off and reset DLA
#ifdef AR9341    
    writel(0xfffffffc, npu_dev.global_ctrl + NPU_RESET_CTRL);
    
    val = readl(npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
    val &= ~0x01C0; //bit 8,7,6
    writel(0xACCE55, npu_dev.global_ctrl + NPU_PMU_CTRL_ACCESS);
    writel(val, npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
#else
    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_mask &= ~(1<<27);
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_0);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_0);
    reset_val &= ~(1<<27);
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_0);
    
    reset_mask = readl(npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_mask &= ~(1<<13);
    writel(reset_mask, npu_dev.global_ctrl + NPU_RESET_MASK_1);
    reset_val = readl(npu_dev.global_ctrl + NPU_RESET_LEVEL_1);
    reset_val &= ~(1<<13);
    writel(reset_val, npu_dev.global_ctrl + NPU_RESET_LEVEL_1);

    //power off
    val = readl(npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
    val &= ~(1<<6); //power off
    val |= (1<<22);
    writel(0xACCE55, npu_dev.global_ctrl + NPU_PMU_CTRL_ACCESS);
    writel(val, npu_dev.global_ctrl + NPU_PMU_CTRL_REG);
#endif
}

//set callbacks to arm
int _ar_npu_register_cb(AR_NPU_IOCTL_CB_S * pstCB)
{
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    _ar_npu_hw_cb_mask(npu_dev.base, pstCB->u32ToArm,
        pstCB->au32CBIDs[0], pstCB->au32CBIDs[1], pstCB->au32CBIDs[2], pstCB->au32CBIDs[3]);

    mutex_unlock(&npu_dev.lock);
    return 0;
}

//set callbacks to dsp
int _ar_npu_register_dsp_cb(AR_NPU_IOCTL_CB_S * pstCB)
{
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    _ar_npu_hw_cb_mask(npu_dev.base, 0,
        pstCB->au32CBIDs[0], pstCB->au32CBIDs[1], pstCB->au32CBIDs[2], pstCB->au32CBIDs[3]);

    mutex_unlock(&npu_dev.lock);
    return 0;
}

unsigned int _ar_npu_is_pausing_or_waitingcb(void)
{
	unsigned int status = readl(npu_dev.base + NPU_IRQ_STATUS_REG);
	return (status & (NPU_IRQ_CB_BIT | NPU_IRQ_PAUSE_BIT));
}

int _ar_npu_can_be_hang(struct ar_npu_work_status * pstatus)
{
    //The cases that we can hang a network:
    //(1) if status is hanging, but the layer num doesn't match, which means
    //dla has been resumed from last hang but not get to next irq point,
    //in this case, we can send a hang-request.
    //(2) if the network is running and not even trigger the first IRQ.

    if(NPU_CHECK_STATE(pstatus->irq_status, NPU_CURRENT_STATE_HANGING)
        && pstatus->layer_num <= _ar_npu_hw_get_layer_num(npu_dev.base))
    {
        return 1;
    }

    return 0;
}

//Since the status will be used by the caller, so lock should be done by callers.
struct ar_npu_work_status * _ar_npu_get_status(unsigned short network_id)
{
    int i = 0;

    mb();//Add barrier to make sure status is updated.

    for(i = 0; i < NPU_MAX_QUEUE_DEPTH; i++)
    {
        if(g_npu_status[i].network_id == network_id)
        {
            return &g_npu_status[i];
        }
    }

    return NULL;
}

int _ar_npu_query(AR_NPU_IOCTL_STATUS_S * status)
{
    int ret = 0;
    unsigned long flags;
    struct ar_npu_work_status * p_status = NULL;

    spin_lock_irqsave(&g_npu_status_lock, flags);
    p_status = _ar_npu_get_status(status->stStatus.u16NetworkId);
    if(p_status)
    {
        status->stStatus.u16FrameId = p_status->frame_id;
        status->stStatus.u32CmpltLayerNum = p_status->layer_num;
        status->stStatus.u32RestartAddrPhy = p_status->restart_addr;
        status->stStatus.u32IRQStatus = p_status->irq_status;
        if(p_status->callback_irq)
        {
            //in case DLA continue running to next state(pause for example), set the cb irq to notify user space.
            status->stStatus.u32IRQStatus |= NPU_IRQ_CB_BIT;
        }

        status->stStatus.au32CBIDs[0] = p_status->callback_id[0];
        status->stStatus.au32CBIDs[1] = p_status->callback_id[1];
        status->stStatus.au32CBIDs[2] = p_status->callback_id[2];
        status->stStatus.au32CBIDs[3] = p_status->callback_id[3];
        if(p_status->callback_irq &&
            (status->stStatus.au32CBIDs[0] + status->stStatus.au32CBIDs[1] + status->stStatus.au32CBIDs[2] + status->stStatus.au32CBIDs[3]) == 0)
        {
            printk("===Query cb but no cb id!\n");
        }

        mb();//Add barrier to make sure status is updated.
        spin_unlock_irqrestore(&g_npu_status_lock, flags);
        return 0;
    }

    //Not found in the array, check DLA ? no.
    //Just set FFFF to network id and frame id to indicates that there's no such network handled,
    //maybe it's still in the queue waiting.
    status->stStatus.u16FrameId = 0xFFFF;
    status->stStatus.u16NetworkId = 0xFFFF;
    spin_unlock_irqrestore(&g_npu_status_lock, flags);
    return 0;
}

//hang and resume could be used for debugging
int _ar_npu_hang(AR_NPU_IOCTL_HANG_S * hang)
{
    return 0;
}
#if 0
int _ar_npu_resume(AR_NPU_IOCTL_RESUME_S * resume)
{
    return 0;
}
#endif

irqreturn_t _ar_npu_irq_handler(int irq, void * dev)
{
    unsigned short network_id = 0;
    unsigned short frame_id = 0;
    unsigned int val = 0;
    int try = 0;
    int j = 0;
    unsigned long flags;
    struct ar_npu_dev * p_npu_dev = (struct ar_npu_dev *)dev;
    struct ar_npu_work_status * p_status = NULL;
    struct timespec npu_time;

    val = readl(p_npu_dev->base + NPU_IRQ_NET_FRAME_ID_REG);
    network_id = val & 0x0000FFFF;
    frame_id = (val & 0xFFFF0000) >> 16;

    spin_lock_irqsave(&g_npu_status_lock, flags);
    //Search for a right position to update the status
    p_status = _ar_npu_get_status(network_id);
    if(!p_status)
    {
        printk("Didn't find a status for net %d\n", network_id);
        spin_unlock_irqrestore(&g_npu_status_lock, flags);
        return IRQ_NONE;
    }

    p_status->network_id = network_id;
    p_status->frame_id = frame_id;
    p_status->layer_num = readl(p_npu_dev->base + NPU_CMPLT_LAYER_NUM_REG);
    p_status->restart_addr = readl(p_npu_dev->base + NPU_RESTART_ADDR_REG);
    p_status->scu_addr = readl(p_npu_dev->base + NPU_COMMAND_ADDR_REG);
    p_status->scu_size = readl(p_npu_dev->base + NPU_COMMAND_LEN_REG);
    p_status->runtime_addr = readl(p_npu_dev->base + NPU_RUNTIME_ADDR_REG);
    p_status->weight_addr = readl(p_npu_dev->base + NPU_PARAMS_ADDR_REG);

    for(j = 0; j < MAX_INPUT_NUM; j++)
    {
        p_status->input_addr[j] = readl(p_npu_dev->base + NPU_INPUT_ADDR_0_REG + j*4);
    }

    for(j = 0; j < MAX_OUTPUT_NUM; j++)
    {
        p_status->output_addr[j] = readl(p_npu_dev->base + NPU_OUTPUT_ADDR_0_REG + j*4);
    }

    p_status->sram_addr = readl(p_npu_dev->base + NPU_SRAM_ADDR_REG);
    p_status->sram_size = readl(p_npu_dev->base + NPU_SRAM_SIZE_REG);

    while(1)
    {
        p_status->irq_status = readl(p_npu_dev->base + NPU_IRQ_STATUS_REG);
        if(!NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_SENDING_CB))
        {
            break;
        }
		
        //cb
        if(p_status->irq_status & NPU_IRQ_CB_BIT)
        {
            p_status->callback_id[0] = readl(p_npu_dev->base + NPU_CALLBACK_ID_0_REG);
            p_status->callback_id[1] = readl(p_npu_dev->base + NPU_CALLBACK_ID_1_REG);
            p_status->callback_id[2] = readl(p_npu_dev->base + NPU_CALLBACK_ID_2_REG);
            p_status->callback_id[3] = readl(p_npu_dev->base + NPU_CALLBACK_ID_3_REG);
            if(p_status->callback_id[0] | p_status->callback_id[1] | p_status->callback_id[2] | p_status->callback_id[3])
            {
                break;
            }
        }

        printk("Sending cb, but no irq or cb id found, check again.\n");
        if(try++ > 10)
        {
            printk("Still not find a cb id when sending_cb comes, we quit.\n");
            spin_unlock_irqrestore(&g_npu_status_lock, flags);
            return IRQ_HANDLED;
        }
    }

    //TBD: if cb or debug, and if set to arm, notify linux, and clear the irq.
    if( (p_status->irq_status & (NPU_IRQ_CB_BIT | NPU_IRQ_PAUSE_BIT)) != 0 )
    {
        if(p_status->irq_status & NPU_IRQ_CB_BIT)
        {
            #if 0
            p_status->callback_id[0] = readl(p_npu_dev->base + NPU_CALLBACK_ID_0_REG);
            p_status->callback_id[1] = readl(p_npu_dev->base + NPU_CALLBACK_ID_1_REG);
            p_status->callback_id[2] = readl(p_npu_dev->base + NPU_CALLBACK_ID_2_REG);
            p_status->callback_id[3] = readl(p_npu_dev->base + NPU_CALLBACK_ID_3_REG);
            if(p_status->callback_id[0] == 0 && p_status->callback_id[1] == 0
                && p_status->callback_id[2] == 0 && p_status->callback_id[3] == 0)
            {
                printk("[Err] NPU got a callback, but no callback id found!\n");
                return IRQ_HANDLED;
            }
            #endif

            p_status->callback_irq = 1;
            p_npu_dev->cb_come = 1;
        }
        else
        {
            p_npu_dev->debug_come = 1;
			//printk("L%d\n",p_status->layer_num);
        }

        mb();//Add barrier to make sure status is updated.
        wake_up_interruptible(&p_npu_dev->wait_queue);
    }

    spin_unlock_irqrestore(&g_npu_status_lock, flags);
    
    if((p_status->irq_status & NPU_IRQ_DONE_BIT) != 0)
    {
        //clear the network id so leave dla a clean context for next network
        writel(0, p_npu_dev->base + NPU_PROC_NET_FRAME_ID_REG);

        //clear npu wait callback regs
        writel(0, p_npu_dev->base + NPU_CALLBACK_ACK_ID_0_REG);
        writel(0, p_npu_dev->base + NPU_CALLBACK_ACK_ID_1_REG);
        writel(0, p_npu_dev->base + NPU_CALLBACK_ACK_ID_2_REG);
        writel(0, p_npu_dev->base + NPU_CALLBACK_ACK_ID_3_REG);

        complete(&p_status->done);

        //get time_toc and time_running	when done
        ktime_get_real_ts64(&npu_time);
        p_npu_dev->time_toc = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
        p_npu_dev->time_running += p_npu_dev->time_toc - p_npu_dev->time_tic;
        //printk("==NPU takes %d x 0.01 ms\n", npu_dev.time_toc - npu_dev.time_tic);
    }

    if((p_status->irq_status & NPU_IRQ_ERROR_BIT) != 0)
    {
        //if any error, must reset the dla
        printk(KERN_ERR"NPU error %x at layer %d, reset...\n", p_status->irq_status, p_status->layer_num);
    #ifdef NPU_HW_RESET_WHEN_ERROR
        complete(&p_status->done);
    #else
        //keep every thing when error for debug.
        return IRQ_HANDLED;
    #endif
        //_ar_npu_hw_reset(p_npu_dev->global_ctrl, p_npu_dev->base);
    }

    if(p_status->irq_status & NPU_IRQ_HANG_BIT)
    {
        printk("[IRQ]: NET %d frame %d status: %x, layer %d\n", network_id, frame_id, p_status->irq_status, p_status->layer_num);
    }

    if(p_status->irq_status & NPU_IRQ_CB_BIT)
    {
        //clear cb irq
        writel(p_status->callback_id[0], p_npu_dev->base + NPU_CALLBACK_ID_0_REG);
        writel(p_status->callback_id[1], p_npu_dev->base + NPU_CALLBACK_ID_1_REG);
        writel(p_status->callback_id[2], p_npu_dev->base + NPU_CALLBACK_ID_2_REG);
        writel(p_status->callback_id[3], p_npu_dev->base + NPU_CALLBACK_ID_3_REG);
    }

    //clear irq
    val = readl(p_npu_dev->base + NPU_IRQ_CLR_REG);
    val |= (p_status->irq_status & 0x00FF); //bit 7:0 is for arm intr
    writel(val, p_npu_dev->base + NPU_IRQ_CLR_REG);

    if((p_status->irq_status & NPU_IRQ_DONE_BIT) != 0)
    {
        //set npu to idle for next case
        complete(&p_npu_dev->idle_wait);
    }

    return IRQ_HANDLED;
}


int _ar_npu_wait_high_priority_done(unsigned short network_id, unsigned short frame_id)
{
    //unsigned int timeout = 100;
    int ret = 0;
    struct ar_npu_work_status * p_status = NULL;

    p_status = _ar_npu_get_status(network_id);
    if(!p_status)
    {
        printk("Looking for net %d failed.\n", network_id);
        return -1;
    }

    //while(1)
    {
        ret = wait_for_completion_timeout(&p_status->high_done, NPU_DEFAULT_TIMEOUT*npu_timeout_factor);
        //p_status = _ar_npu_get_status(network_id);

        if(p_status && p_status->frame_id == frame_id)
        {
            return 0;
        }

        //msleep(1);
        if(ret <= 0)
        {
            printk(KERN_ERR"Wait for NPU DONE timeout.\n");
            return -ETIME;
        }
    }
    //p_status->hprior_done =0;
    return 0;
}

int _ar_npu_wait_done(unsigned short network_id, unsigned short frame_id)
{
    //unsigned int timeout = 100;
    int ret = 0;
    struct ar_npu_work_status * p_status = NULL;

    p_status = _ar_npu_get_status(network_id);
    if(!p_status)
    {
        printk("Looking for net %d failed.\n", network_id);
        return -1;
    }

    //printk("=wait for net %d done...\n", network_id);

    //while(1)
    {
        ret = wait_for_completion_timeout(&p_status->done, NPU_DEFAULT_TIMEOUT*npu_timeout_factor);
    #ifdef NPU_HW_RESET_WHEN_ERROR
        if(p_status->irq_status & NPU_IRQ_ERROR_BIT)
        {
            _ar_npu_hw_reset(npu_dev.global_ctrl, npu_dev.base);
            ret = _ar_npu_poweron();
            if(ret)
            {
                printk(KERN_ERR"_ar_npu_wait_done poweron NPU error.\n");
            }

            return -1;
        }
    #endif
        if(p_status && p_status->frame_id == frame_id
            && (p_status->irq_status & NPU_IRQ_DONE_BIT))
        {
               return 0;
        }
        //msleep(1);
        if(ret <= 0)
        {
            printk(KERN_ERR"[%d]Wait for NPU DONE timeout.\n",network_id);
            return -ETIME;
        }
    }

    //printk(KERN_INFO"DLA[%d] done at layer %d.\n", network_id, p_status->layer_num);

    return 0;
}

int _ar_npu_cb_done(AR_NPU_IOCTL_CB_DONE_S * cb_done)
{
    struct ar_npu_work_status * p_status = NULL;
    unsigned long flags;

    spin_lock_irqsave(&g_npu_status_lock, flags);

    p_status = _ar_npu_get_status(cb_done->u16NetworkId);
    if(p_status)
    {
        p_status->callback_irq = 0;
        p_status->irq_status &= ~NPU_IRQ_CB_BIT;
        p_status->callback_id[0]=0;
        p_status->callback_id[1]=0;
        p_status->callback_id[2]=0;
        p_status->callback_id[3]=0;
        npu_dev.cb_come = 0;
        mb();//Add barrier to make sure status is updated.
    }

    spin_unlock_irqrestore(&g_npu_status_lock, flags);

    //printk(KERN_INFO"==%s %d i %d: net %d cb %x/%x/%x/%x.\n", __FUNCTION__, __LINE__, i,
    //        cb_done->u16NetworkId,
    //        cb_done->au32CBAckIDs[0], cb_done->au32CBAckIDs[1], cb_done->au32CBAckIDs[2], cb_done->au32CBAckIDs[3]);

    return _ar_npu_hw_cb_done(npu_dev.base, cb_done);
}

int _ar_npu_debug_continue(AR_NPU_IOCTL_DEBUG_S * debug)
{
    return _ar_npu_hw_debug_continue(npu_dev.base, debug);
}

//1. put the task into fifo
int _ar_npu_start(AR_NPU_IOCTL_CFG_S * cfg)
{
    unsigned int ret = 0;
    unsigned long flags;
    //unsigned int timeout = NPU_DEFAULT_TIMEOUT;
    //struct timespec npu_time;
    //unsigned long long tic, toc;
    struct ar_npu_work_status * p_status = NULL;
    int i = 0;

    if(cfg->u16NetworkId == 0 || cfg->u16NetworkId == 0xFFFF)
    {
        return -EINVAL;
    }

    if(cfg->u32Debug == NPU_LAYER_DEBUG_ON)
    {
        npu_timeout_factor = 100000;
    }

    //this is the first time for this model, we alloc a id for it.
    spin_lock_irqsave(&g_npu_status_lock, flags);

    p_status = _ar_npu_get_status(cfg->u16NetworkId);
    if(!p_status)
    {
        
        for(i = 0; i < NPU_MAX_QUEUE_DEPTH; i++)
        {
            if(g_npu_status[i].network_id == 0)
            {
                g_npu_status[i].network_id = cfg->u16NetworkId;
                g_npu_status[i].priority = cfg->u32Priority;
                break;
            }
        }
    }
    
    spin_unlock_irqrestore(&g_npu_status_lock, flags);

#ifndef NPU_SUPPORT_PRIORITY
    cfg->u32Priority = NPU_NETWORK_NORMAL;
#endif

    if(cfg->u32Priority == NPU_NETWORK_HIGH)
    {
        ret = mutex_lock_interruptible(&g_fifo_high_lock);
        if(ret)
        {
            printk(KERN_ERR"_ar_npu_start acquire lock failed, priority = %d\n", cfg->u32Priority);
            return -ENOTBLK;
        }
        ret = kfifo_put(&g_npu_high_fifo, *cfg);
        mutex_unlock(&g_fifo_high_lock);
        up(&g_high_sem);
    }
    else
    {
        ret = mutex_lock_interruptible(&g_fifo_normal_lock);
        if(ret)
        {
            printk(KERN_ERR"_ar_npu_start acquire lock failed, priority = %d\n", cfg->u32Priority);
            return -ENOTBLK;
        }
        ret = kfifo_put(&g_npu_normal_fifo, *cfg);
        mutex_unlock(&g_fifo_normal_lock);
        up(&g_normal_sem);
    }

    if(!ret) //0 means the fifo is full
    {
        printk(KERN_ERR"The Fifo is full!\n");
        return -ENOMEM;
    }

    //if blocked call, wait for finish
    if(cfg->u32Block)
    {
        if(cfg->u32Priority == NPU_NETWORK_HIGH)
        {
            ret=_ar_npu_wait_high_priority_done(cfg->u16NetworkId, cfg->u16FrameId);
        }
        else
        {
            ret=_ar_npu_wait_done(cfg->u16NetworkId, cfg->u16FrameId);
        }
    }

    return ret;
}

int _ar_npu_sw_reset()
{
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    _ar_npu_hw_reset(npu_dev.global_ctrl, npu_dev.base);
    mutex_unlock(&npu_dev.lock);

    return 0;
}

int _ar_npu_set_cache_coherency(unsigned int cache_enable)
{
    void __iomem * base = npu_dev.base;
    unsigned int val = 0;
    
#ifdef AR9341
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    if(cache_enable)
    {
        val = readl(base + NPU_AXI_IN_ARCACHE_0_REG);
        val |= 0xF;
        writel(val, base + NPU_AXI_IN_ARCACHE_0_REG);

        val = readl(base + NPU_AXI_OUT_AWCACHE_0_REG);
        val |= 0xF;
        writel(val, base + NPU_AXI_OUT_AWCACHE_0_REG);

        val = readl(base + NPU_AXI_OUT_ARCACHE_0_REG);
        val |= 0xF;
        writel(val, base + NPU_AXI_OUT_ARCACHE_0_REG);
    }
    else
    {
        val = readl(base + NPU_AXI_IN_ARCACHE_0_REG);
        val &= 0xFFFFFFF0;
        writel(val, base + NPU_AXI_IN_ARCACHE_0_REG);

        val = readl(base + NPU_AXI_OUT_AWCACHE_0_REG);
        val &= 0xFFFFFFF0;
        writel(val, base + NPU_AXI_OUT_AWCACHE_0_REG);

        val = readl(base + NPU_AXI_OUT_ARCACHE_0_REG);
        val &= 0xFFFFFFF0;
        writel(val, base + NPU_AXI_OUT_ARCACHE_0_REG);
    }
    mutex_unlock(&npu_dev.lock);
#endif

    return 0;
}

int _ar_npu_set_security(unsigned int security)
{
    void __iomem * base = NULL;
    unsigned int val = 0;

#ifdef AR9341 
    if(_ar_npu_is_cpu_nonsec(npu_dev.global_ctrl))
    {
        printk("NPU can't access sec reg from NS state.\n");
        return -1;
    }
#endif

    base = ioremap_nocache(NPU_SECURITY_BASE, NPU_REG_SPACE_RANGE);
    if(!base)
    {
        return -1;
    }

    if(security)
    {
        val = readl(base + NPU_SECURITY_MODE_REG);
        val &= 0xFFFFFFFC; //bit 0,1
        writel(val, base + NPU_SECURITY_MODE_REG);
#if 0
        writel(0xC, npu_dev.base + NPU_DMAC_BASE_CTRL_REG);

        writel(NPU_DMAC_SEC_BASE_ADDR, npu_dev.base + NPU_DMAC_SEC_BASE_ADDR_REG);
#endif
    }
    else
    {
        val = readl(base + NPU_SECURITY_MODE_REG);
        val |= 3; //bit 0,1
        writel(val, base + NPU_SECURITY_MODE_REG);
#if 0
        writel(0, npu_dev.base + NPU_DMAC_BASE_CTRL_REG);

        writel(0, npu_dev.base + NPU_DMAC_SEC_BASE_ADDR_REG);
#endif
    }

    return 0;
}

int _ar_npu_set_frequency(unsigned int frequency)
{
    unsigned int frequency_bit = 0x0;
    unsigned int val = 0x0;
	
    if(frequency == 0)
    {
       return 0;
	}

    //frequency = frequency / 1000000;

#ifdef AR9341
    frequency = frequency > 1000 ? 1000 : frequency;
    frequency = frequency < 40 ? 40 : frequency;
    //printk("To be implemented: frequency %d MHz.\n", frequency);
    frequency_bit = ((frequency / 10) << 8);

    val = readl(npu_dev.global_ctrl + NPU_PLL_CTRL_0);
    val &= 0xFFFF00FF;
    val |= frequency_bit;
    writel(val, npu_dev.global_ctrl + NPU_PLL_CTRL_0);
#else
    frequency = frequency > 800 ? 800 : frequency;
    frequency = frequency < 50 ? 50 : frequency;
    
    if(frequency >= 800)
        frequency_bit= 0x1000;
    else if(frequency >= 666)
        frequency_bit= 0x1100;
    else if(frequency >= 600)
        frequency_bit= 0x1400;
    else if(frequency >= 500)
        frequency_bit= 0x1200;
    else if(frequency >= 400)
        frequency_bit= 0x1300;
    else if(frequency >= 300)
        frequency_bit= 0x1401;
    else if(frequency >= 200)
        frequency_bit= 0x1402;
    else if(frequency >= 100)
        frequency_bit= 0x1405;
    else //50MHz
        frequency_bit= 0x1307;
    
    val = readl(npu_dev.global_ctrl + NPU_PLL_CTRL_0);
    val |= (frequency_bit <<16);
    writel(val, npu_dev.global_ctrl + NPU_PLL_CTRL_0);
#endif
    return 0;
}


int _ar_npu_set_timeout(unsigned int timeout)
{
    unsigned int val = 0x0;
	
    if(timeout == 0)
    {
       return 0;
	}
    
	npu_timeout_factor = timeout;

    return 0;
}

int _ar_npu_get_io_addr(AR_NPU_IOCTL_IO_ADDR_S * io_addr)
{
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    io_addr->u32InPhyAddr = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG);
    io_addr->u32OutPhyAddr = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG);

    mutex_unlock(&npu_dev.lock);
    return 0;
}

int _ar_npu_get_cbuf_addr(AR_NPU_IOCTL_CBUF_ADDR_S * cbuf_addr)
{
#ifdef AR9341
    cbuf_addr->u32CbufPhyAddr = 0x02000000;
#else
    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        mutex_unlock(&npu_dev.lock);
        return -EAGAIN;
    }
    
    unsigned int val = 0x0;
	val = readl(npu_dev.base + NPU_START_REG);
    cbuf_addr->u32CbufPhyAddr = (val >> 7) << 8;
    mutex_unlock(&npu_dev.lock);
#endif
    return 0;
}

//currently no lock, since only irq write and query read
int _ar_npu_clean_resource(unsigned short network_id)
{
    struct ar_npu_work_status * p_status;
    unsigned long flags;

    //printk("[%d]NPU cleaning...\n",network_id);

    spin_lock_irqsave(&g_npu_status_lock, flags);
    p_status = _ar_npu_get_status(network_id);
    if(p_status)
    {
        p_status->network_id = 0;
        p_status->frame_id = 0;
        p_status->priority = 0;
        p_status->irq_status = 0;
        p_status->layer_num = 0;
        p_status->restart_addr = 0;
        p_status->scu_addr = 0;
        p_status->scu_size = 0;
        p_status->runtime_addr = 0;
        p_status->weight_addr = 0;
        p_status->sram_addr = 0;
        p_status->sram_size = 0;
        p_status->callback_irq = 0;
        memset(p_status->input_addr, 0, sizeof(p_status->input_addr));
        memset(p_status->output_addr, 0, sizeof(p_status->output_addr));
        memset(p_status->callback_id, 0, sizeof(p_status->callback_id));
        
        mb();//Add barrier to make sure status is updated.
    }
    
    spin_unlock_irqrestore(&g_npu_status_lock, flags);

    _ar_npu_clear_net_id(network_id);

    return 0;
}

int _ar_npu_alloc_net_id(unsigned short * p_network_id)
{
    int i = 0;
    int ret = -1;

    ret = mutex_lock_interruptible(&npu_dev.lock);
    if(ret)
    {
        printk(KERN_ERR"_ar_npu_alloc_net_id acquire lock failed!\n");
        return -ENOTBLK;
    }
    for(i = 1; i < 0xffff; i++)
    {
        //this id has been used, go on.
        if(npu_dev.id_bmp[i/32] & (1 << (i%32)))
        {
            continue;
        }
        else
        {
            npu_dev.id_bmp[i/32] |= (1 << (i%32));
            *p_network_id = i;
            ret = 0;

            break;
        }
    }
    mutex_unlock(&npu_dev.lock);

    return ret;
}

int _ar_npu_set_net_id(unsigned short network_id)
{
	int ret = 0;
	
    ret = mutex_lock_interruptible(&npu_dev.lock);
    if(ret)
    {
        printk(KERN_ERR"_ar_npu_set_net_id acquire lock failed!\n");
        return -ENOTBLK;
    }
    if(npu_dev.id_bmp[network_id/32] & (1 << (network_id%32)))
    {
        printk("The net id %d has been used, please set another one or get one from NPU\n", network_id);
        mutex_unlock(&npu_dev.lock);
        return -EINVAL;
    }

    npu_dev.id_bmp[network_id/32] |= (1 << (network_id%32));
    mutex_unlock(&npu_dev.lock);

    return 0;
}

int _ar_npu_clear_net_id(unsigned short network_id)
{
	int ret = 0;
	
    ret = mutex_lock_interruptible(&npu_dev.lock);
    if(ret)
    {
        printk(KERN_ERR"_ar_npu_clear_net_id acquire lock failed!\n");
        return -ENOTBLK;
    }
    npu_dev.id_bmp[network_id/32] &= ~(1 << (network_id%32));
    mutex_unlock(&npu_dev.lock);

    return 0;
}

int _ar_npu_stop_and_reset(void)
{
    int i = 0;
    int ret = 0;
    
    printk(KERN_INFO "_ar_npu_stop_and_reset is called.\n");
    for(i = 0; i < 65535; i++)
    {
	     if(npu_dev.id_bmp[i/32] & (1 << (i%32)))
	     {
	        _ar_npu_clean_resource(i);
	     }
    }

    _ar_npu_hw_reset(npu_dev.global_ctrl, npu_dev.base);

    ret = _ar_npu_poweron();
    if(ret)
    {
        printk(KERN_ERR "_ar_npu_stop_and_reset pweron npu error.\n");
        return ret;
    }

    return 0;
}

int _ar_npu_normal_thread(void * args)
{
    unsigned int ret = 0;
    int timeout = 0;
    AR_NPU_IOCTL_CFG_S params;
    struct timespec npu_time;
    unsigned long long u64toc = 0;
    struct ar_npu_work_status * p_status = NULL;

    set_current_state(TASK_INTERRUPTIBLE);

    while(1)
    {
        ktime_get_real_ts64(&npu_time);
        u64toc = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;

        if(kthread_should_stop())
        {
            do_exit(0);
        }

        //get task from fifo
        ret = mutex_lock_interruptible(&g_fifo_normal_lock);
        if(ret)
        {
        	printk(KERN_ERR"_ar_npu_normal_thread get lock for fifo error\n");
        	continue;
        }
        ret = kfifo_get(&g_npu_normal_fifo, &params);
        mutex_unlock(&g_fifo_normal_lock);
        if(!ret)
        {
            //msleep(1);
            ret = down_timeout(&g_normal_sem, HZ*5);
            continue;
        }
        
        timeout = _ar_npu_hw_wait_idle(npu_dev.base);
        if(timeout < 0)
        {
            printk(KERN_ERR"%d Wait for NPU IDLE timeout.\n",params.u16NetworkId);
            return -1;
        }
        
        //Here, at this moment, maybe the DLA is not idle anymore
        //Because there are only two threads to start dla, so if not idle,
        //that means high priority grab the DLA, so here must wait for the lock.
        //try to get lock and cfg dla
        ret = mutex_lock_interruptible(&npu_dev.lock);
        if(ret)
        {
        	printk(KERN_ERR"_ar_npu_normal_thread get lock for cfg error\n");
        	continue;
        }
        //When npu is suspended, the tasks in fifo will pretend to be done, 
        //so that user app don't have to block here.
        if(npu_dev.suspended)
        {
            p_status = _ar_npu_get_status(params.u16NetworkId);
            if(!p_status)
            {
                printk("NPU suspended, but we didn't find a status for net %d\n", params.u16NetworkId);
                mutex_unlock(&npu_dev.lock);
                continue;
            }
            complete(&p_status->done);
            complete(&npu_dev.idle_wait);
            mutex_unlock(&npu_dev.lock);
            continue;
        }
        
        _ar_npu_hw_start(npu_dev.base, &params);
        mutex_unlock(&npu_dev.lock);
        
        //get the time_tic when normal thread start
        ktime_get_real_ts64(&npu_time);
        npu_dev.time_tic = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
    }

    printk("NPU normal thread exit...\n");

    return 0;
}

int _ar_npu_high_thread(void * args)
{
    int ret = 0;
    int timeout = NPU_DEFAULT_TIMEOUT*npu_timeout_factor;
    AR_NPU_IOCTL_CFG_S params;
    AR_NPU_IOCTL_CFG_S prev_cfg;
    unsigned short network_id, frame_id;
    unsigned int reg;
    struct ar_npu_work_status * p_status;
    struct timespec npu_time;
    unsigned int need_resume = 0;
    int i = 0;

    set_current_state(TASK_INTERRUPTIBLE);

    while(1)
    {
        if(kthread_should_stop())
        {
            do_exit(0);
        }

        //get task from fifo
        ret = mutex_lock_interruptible(&g_fifo_high_lock);
        if(ret)
        {
            printk(KERN_ERR"_ar_npu_high_thread get lock for fifo error\n");
            continue;
        }
		
        ret = kfifo_get(&g_npu_high_fifo, &params);
        mutex_unlock(&g_fifo_high_lock);
        if(!ret)
        {
            ret = down_timeout(&g_high_sem, HZ*1);
            continue;
        }

        //try to cfg dla
        ret = mutex_lock_interruptible(&npu_dev.lock);
        if(ret)
        {
            printk(KERN_ERR"_ar_npu_high_thread get lock for cfg error\n");
            continue;
        }
        //When npu is suspended, the tasks in fifo will pretend to be done, 
        //so that user app don't have to block here.
        if(npu_dev.suspended)
        {
            p_status = _ar_npu_get_status(params.u16NetworkId);
            if(!p_status)
            {
                printk("NPU suspended, but we didn't find a status for net %d\n", params.u16NetworkId);
                mutex_unlock(&npu_dev.lock);
                continue;
            }
            complete(&p_status->high_done);
            complete(&npu_dev.idle_wait);
            mutex_unlock(&npu_dev.lock);
            continue;
        }
        
        //get the network id which will be interrupted.
        reg = readl(npu_dev.base + NPU_IRQ_NET_FRAME_ID_REG);
        network_id = reg & 0x0000FFFF;
        frame_id = (reg & 0xFFFF0000) >> 16;

        //the dla is just ready from reset, start directly
        if(network_id == 0 && frame_id == 0)
        {
            _ar_npu_hw_start(npu_dev.base, &params);
            ret=_ar_npu_wait_done(params.u16NetworkId, params.u16FrameId);
            p_status = _ar_npu_get_status(params.u16NetworkId);
            complete(&p_status->high_done);

            mutex_unlock(&npu_dev.lock);

            //get the time_tic when thread start
            ktime_get_real_ts64(&npu_time);
            npu_dev.time_tic = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
            continue;
        }

        //The cases that we can hang a network:
        //(1) if status is hanging, but the layer num doesn't match, which means
        //dla has been resumed from last hang but not get to next irq point,
        //in this case, we can send a hang-request.
        //(2) if the network is running and not even trigger the first IRQ,
        //therefore its status has not been updated into g_status.

        //p_status = _ar_npu_get_status(network_id);
        //if(!p_status || p_status->frame_id != frame_id || _ar_npu_can_be_hang(p_status))

        {
            _ar_npu_hw_hang(npu_dev.base);

            //Check if the network is done or hanged
            while(1)
            {
                if(kthread_should_stop())
                {
                    mutex_unlock(&npu_dev.lock);
                    do_exit(0);
                }

                p_status = _ar_npu_get_status(network_id);
                //this op has not been updated to the status array.
                if(!p_status || p_status->frame_id != frame_id)
                {
                    msleep(1);
                    if(timeout-- == 0)
                    {
                        mutex_unlock(&npu_dev.lock);
                        return -1;
                    }
                    continue;
                }

                //done or idle,no need to resume it.
                if(NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_DONE)
                 ||NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_IDLE))
                {
                    printk("Other network has done!|| NPU is idle!\n");
                    need_resume = 0;
                }
                else if(NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_HANGING)
                    || NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_AFTER_HANGING))
                {
                    printk("Network %d has been hang\n", network_id);
                    need_resume = 1;
                }
                else if(NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_SENDING_CB)
                    || NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_WAITING_CB)
                    || NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_AFTER_PAUSING)
                    || NPU_CHECK_STATE(p_status->irq_status, NPU_CURRENT_STATE_PAUSING))
                {
                    //keep waiting for cb until the hang point is reached.
                    //keep waiting until the debug finished.
                    //printk("Waiting network %d to finish...\n", network_id);
                    msleep(1);
                    continue;
                }
                else
                {
                    printk(KERN_ERR"Trying to hang net %d frame %d error, will re-run net %d later?.",
                        network_id, frame_id, network_id);

                    need_resume = 2;
                }

                break;
            }
            //get time_toc and time_running when hang
            ktime_get_real_ts64(&npu_time);
            npu_dev.time_toc = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
            npu_dev.time_running += npu_dev.time_toc - npu_dev.time_tic;
        }

        //start this high-priority network
        _ar_npu_hw_start(npu_dev.base, &params);

        //get time_tic when high Thread start
        ktime_get_real_ts64(&npu_time);
        npu_dev.time_tic = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;

        //wait to complete
        _ar_npu_wait_done(params.u16NetworkId, params.u16FrameId);
        for(i = 0; i < NPU_MAX_QUEUE_DEPTH; i++)
        {
            if(g_npu_status[i].network_id == params.u16NetworkId)
            {
                complete(&g_npu_status[i].high_done);
                break;
            }
        }

        //resume the hanged network
        if(need_resume == 1)
        {
            printk("Resume network %d...\n", network_id);
            _ar_npu_hw_resume(npu_dev.base, p_status);//status has all info needed.

            //get time_tic when normal Thread resume
            ktime_get_real_ts64(&npu_time);
            npu_dev.time_tic = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
        }

        //try to re-run the hanged network since it has an error.
        //shall we do this or just drop this ???
        if(need_resume == 2)
        {
            printk("Restart network %d\n", network_id);
            prev_cfg.u32Debug = NPU_LAYER_DEBUG_OFF;
            prev_cfg.u32SCUAddrPhy = p_status->scu_addr;
            prev_cfg.u32SCUSize = p_status->scu_size;
            prev_cfg.u32RuntimeAddrPhy = p_status->runtime_addr;
            prev_cfg.u32WeightsAddrPhy = p_status->weight_addr;
            memcpy(prev_cfg.au32InputAddrPhy, p_status->input_addr, sizeof(prev_cfg.au32InputAddrPhy));
            memcpy(prev_cfg.au32OutputAddrPhy, p_status->output_addr, sizeof(prev_cfg.au32OutputAddrPhy));
            prev_cfg.u32SramAddrPhy = p_status->sram_addr;
            prev_cfg.u32SramSize = p_status->sram_size;
            prev_cfg.u16NetworkId = p_status->network_id;
            prev_cfg.u16FrameId = p_status->frame_id;
            _ar_npu_hw_start(npu_dev.base, &prev_cfg);

         //get time_tic when normal Thread resume
         ktime_get_real_ts64(&npu_time);
         npu_dev.time_tic = npu_time.tv_sec * 100000 + npu_time.tv_nsec/10000;
        }

        mutex_unlock(&npu_dev.lock);
    }

    printk("NPU high thread exit...\n");
    return 0;
}

int ar_npu_proc_apb_state(unsigned int *reg_val, unsigned int sub_modules)
{
    unsigned int reg_ctrl = 0;
    unsigned int i = 0;

    memset(reg_val, 0, 16 * sizeof(unsigned int));

    for(i = 0; i < 16; i++)
    {
        reg_ctrl = (i | sub_modules);
        writel(reg_ctrl, npu_dev.base + NPU_APB_DBG_CTRL_REG);
        reg_val[i] = readl(npu_dev.base + NPU_APB_DBG_VALUE_REG);
    }

    return 0;
}

#ifdef AR9341
int ar_npu_proc_state_show(struct seq_file *s, void *p)
{
    unsigned int val = 0;
    unsigned int start = 0;
    unsigned short network_id, frame_id;
    unsigned int layer, status, input0, input1, input2, input3, output0, output1, output2, output3;
    unsigned int irq_mask, axi_qos, sram_addr, sram_size, runtime, sram_ctrl, params, scu, scu_len;
    unsigned int in_arcache0;
    unsigned int out_awcache0;
    unsigned int out_arcache0;
    unsigned int callbackmask_arm0, callbackmask_arm1, callbackmask_arm2, callbackmask_arm3;
    unsigned int callbackmask_dsp0, callbackmask_dsp1, callbackmask_dsp2, callbackmask_dsp3;
    unsigned int dma_addr, dma_ctrl, dma_sec_base;
    unsigned int apb_ctrl_modules;
    unsigned int abp_debug_value[16];
    int i = 0, j = 0;

    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        seq_printf(s,"NPU has been suspended!\n");
        mutex_unlock(&npu_dev.lock);
        return 0;
    }
    
    val = readl(npu_dev.base + NPU_PROC_NET_FRAME_ID_REG);
    network_id = val & 0x0000FFFF;
    frame_id = val >> 16;
    start = readl(npu_dev.base + NPU_START_REG);
    layer = readl(npu_dev.base + NPU_CMPLT_LAYER_NUM_REG);
    status = readl(npu_dev.base + NPU_IRQ_STATUS_REG);
    input0 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG);
    input1 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 4);
    input2 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 8);
    input3 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 12);
    output0 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG);
    output1 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 4);
    output2 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 8);
    output3 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 12);
    
    irq_mask = readl(npu_dev.base + NPU_IRQ_MASK_REG);
    axi_qos = readl(npu_dev.base + NPU_AXI_QOS_REG);
    sram_addr = readl(npu_dev.base + NPU_SRAM_ADDR_REG);
    sram_size = readl(npu_dev.base + NPU_SRAM_SIZE_REG);
    runtime = readl(npu_dev.base + NPU_RUNTIME_ADDR_REG);
    params = readl(npu_dev.base + NPU_PARAMS_ADDR_REG);
    scu =  readl(npu_dev.base + NPU_COMMAND_ADDR_REG);
    scu_len =  readl(npu_dev.base + NPU_COMMAND_LEN_REG);
    sram_ctrl = readl(npu_dev.base + NPU_SRAM_RME_CTRL_REG);
        
    in_arcache0 = readl(npu_dev.base + NPU_AXI_IN_ARCACHE_0_REG);    
    out_awcache0 = readl(npu_dev.base + NPU_AXI_OUT_AWCACHE_0_REG);    
    out_arcache0 = readl(npu_dev.base + NPU_AXI_OUT_ARCACHE_0_REG);
    
    callbackmask_arm0 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_0_REG);
    callbackmask_arm1 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_1_REG);
    callbackmask_arm2 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_2_REG);
    callbackmask_arm3 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_3_REG);
    callbackmask_dsp0 = readl(npu_dev.base + NPU_CALLBACK_MASK_CEVA_0_REG);
    callbackmask_dsp1 = readl(npu_dev.base + NPU_CALLBACK_MASK_CEVA_1_REG);
    callbackmask_dsp2 = readl(npu_dev.base + NPU_CALLBACK_MASK_CEVA_2_REG);
    callbackmask_dsp3 = readl(npu_dev.base + NPU_CALLBACK_MASK_CEVA_3_REG);
    dma_addr = readl(npu_dev.base + NPU_DMAC_BASE_ADDR_REG);
    dma_ctrl = readl(npu_dev.base + NPU_DMAC_BASE_CTRL_REG);
    dma_sec_base = readl(npu_dev.base + NPU_DMAC_SEC_BASE_ADDR_REG);

    //Make the log more human-readable and user friendly
    seq_printf(s, "-------------------------------- Network --------------------------------\n");
    seq_printf(s, "  ID                Frame               CurrentLayer               \n");
    seq_printf(s, "  %d                %d                    %d                       \n",
               network_id, frame_id, layer);
    seq_printf(s, "-------------------------------- Status ---------------------------------\n");
    seq_printf(s, "  Hang mode   : %s\n", start & (1<<2)?"Y":"N");
    seq_printf(s, "  Debug mode  : %s\n", start & (1<<3)?"Y":"N");
    seq_printf(s, "  IrqReg value: 0x%08x, which means:\n", status);
    seq_printf(s, "  CurrentState: %s\n", NPU_STATE_TO_STRING((status >> 28) & 0xff));
    seq_printf(s, "  IrqStatus   : \n");
    if(status & NPU_IRQ_CBUF_READ_CONFLICT_BIT)
    {
        seq_printf(s, "    Cbuff read conflict!\n");
    }
    if(status & NPU_IRQ_DMAC_ERROR_BIT)
    {
        seq_printf(s, "    DMAC error!\n");
    }
    if(status & NPU_IRQ_AXIM_BRESP_ERROR_BIT)
    {
        seq_printf(s, "    AXIM bresp error!\n");
    }
    if(status & NPU_IRQ_DONE_BIT)
    {
        seq_printf(s, "    NPU DONE!\n");
    }
    if(status & NPU_IRQ_HANG_BIT)
    {
        seq_printf(s, "    NPU hang by another high-priority model!\n");
    }
    if(status & NPU_IRQ_CB_BIT)
    {
        seq_printf(s, "    NPU generates callback IRQ!\n");
    }
    if(status & NPU_IRQ_PAUSE_BIT)
    {
        seq_printf(s, "    NPU paused!\n");
    }
    seq_printf(s, "  Irq mask    : 0x%08x\n", irq_mask);
    seq_printf(s, "  Network ID used: \n");
    for(i = 0; i < 65535; i++)
    {
        if(npu_dev.id_bmp[i/32] & (1 << (i%32)))
        {
            seq_printf(s, "  %d ", i);
            j++;
            if(j%8 == 0)
            {
                seq_printf(s, "\n");
            }
        }
    }
    seq_printf(s, "\n");
    seq_printf(s, "-------------------------------- Memory ---------------------------------\n");
    seq_printf(s, "  Input[0-3] : 0x%08x 0x%08x 0x%08x 0x%08x\n", input0, input1, input2, input3);
    seq_printf(s, "  Output[0-3]: 0x%08x 0x%08x 0x%08x 0x%08x\n", output0, output1, output2, output3);
    seq_printf(s, "  Runtime    : 0x%08x\n", runtime);
    seq_printf(s, "  Params     : 0x%08x\n", params);
    seq_printf(s, "  Scu        : 0x%08x\n", scu);
    seq_printf(s, "  Scu Length : %d\n", scu_len);
    seq_printf(s, "  SRAM       : 0x%08x\n", sram_addr);
    seq_printf(s, "  SRAM size  : %d\n", sram_size);
    seq_printf(s, "------------------------------Configuration------------------------------\n");
    seq_printf(s, "  AXImQos[w/r]        : 0x%04x/0x%04x\n", (axi_qos >> 8 ) & 0xf, (axi_qos >> 12) & 0xf);
    seq_printf(s, "  Input ArCache[0-7]  : 0x%08x\n", in_arcache0);
    seq_printf(s, "  Output ArCache[0-7] : 0x%08x\n", out_arcache0);
    seq_printf(s, "  Output AwCache[0-7] : 0x%08x\n", out_awcache0);
    seq_printf(s, "  CallbackMaskArm[0-3]: 0x%x 0x%x 0x%x 0x%x\n", callbackmask_arm0, callbackmask_arm1, callbackmask_arm2, callbackmask_arm3);
    seq_printf(s, "  CallbackMaskDsp[0-3]: 0x%x 0x%x 0x%x 0x%x\n", callbackmask_dsp0, callbackmask_dsp1, callbackmask_dsp2, callbackmask_dsp3);
    seq_printf(s, "  SRAMCtrl            : 0x%x(%s)\n", sram_ctrl, 
        sram_ctrl & (1<<16) ? "SRAM Off" : (sram_ctrl & (1<<17)) ? "SRAM On" : "Auto");
    seq_printf(s, "  DMA Base            : 0x%x\n", dma_addr);
    seq_printf(s, "  DMA Security Base   : 0x%x\n", dma_sec_base);
    seq_printf(s, "  DMA ch %x security[w/r] %x/%x axcache[w/r] %x/%x\n", dma_ctrl & 0x3, 
        (dma_ctrl >> 2) & 0x01, (dma_ctrl >> 3) & 0x01, (dma_ctrl >> 4) & 0x0f, (dma_ctrl >> 8) & 0x0f);
    
    seq_printf(s, "-------------------------------- Debug ----------------------------------\n");
     //cdma debug value
	 apb_ctrl_modules = 0x0 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  CDMA Module :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

	 //csc debug value
     apb_ctrl_modules = 0x1 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  CSC Module  :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

	 //cmac debug value
	 apb_ctrl_modules = 0x2 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  CMAC Module :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

	 //sdp debug value
	 apb_ctrl_modules = 0x3 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  SDP Module  :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

	 //pdp debug value
	 apb_ctrl_modules = 0x4 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  PDP Module  :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

	 //fwc debug value
	 apb_ctrl_modules = 0x5 << 4;
	 ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
	 seq_printf(s,
	 	"  FWC Module  :\n"
	 	"    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
	 	"    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
	 	abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
	 	abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
	 	abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
	 	abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]
	 	);

    mutex_unlock(&npu_dev.lock);
    return 0;
}
#else
int ar_npu_proc_state_show(struct seq_file *s, void *p)
{
    unsigned int val = 0;
    unsigned int start = 0;
    unsigned short network_id, frame_id;
    unsigned int layer, status, input0, input1, input2, input3, output0, output1, output2, output3;
    unsigned int irq_mask, axi_qos, sram_addr, sram_size, runtime, sram_ctrl, params, scu, scu_len;
    unsigned int process_cycle_real, axi_write_cmd_cnt_real, axi_read_cmd_cnt_real,  axi_write_data_cnt_real, axi_read_data_cnt_real;
    unsigned int process_cycle_int, axi_write_cmd_cnt_int, axi_read_cmd_cnt_int,  axi_write_data_cnt_int, axi_read_data_cnt_int;
    unsigned int callbackmask_arm0, callbackmask_arm1, callbackmask_arm2, callbackmask_arm3;
    unsigned int dma_addr, dma_ctrl, dma_sec_base;
    unsigned int apb_ctrl_modules;
    unsigned int abp_debug_value[16];
    int i = 0, j = 0;

    mutex_lock(&npu_dev.lock);
    if(npu_dev.suspended)
    {
        seq_printf(s,"NPU has been suspended!\n");
        mutex_unlock(&npu_dev.lock);
        return 0;
    }

    val = readl(npu_dev.base + NPU_PROC_NET_FRAME_ID_REG);
    network_id = val & 0x0000FFFF;
    frame_id = val >> 16;
    start = readl(npu_dev.base + NPU_START_REG);
    layer = readl(npu_dev.base + NPU_CMPLT_LAYER_NUM_REG);
    status = readl(npu_dev.base + NPU_IRQ_STATUS_REG);
    input0 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG);
    input1 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 4);
    input2 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 8);
    input3 = readl(npu_dev.base + NPU_INPUT_ADDR_0_REG + 12);
    output0 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG);
    output1 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 4);
    output2 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 8);
    output3 = readl(npu_dev.base + NPU_OUTPUT_ADDR_0_REG + 12);
    
    irq_mask = readl(npu_dev.base + NPU_IRQ_MASK_REG);
    axi_qos = readl(npu_dev.base + NPU_AXI_QOS_REG);
    sram_addr = readl(npu_dev.base + NPU_SRAM_ADDR_REG);
    sram_size = readl(npu_dev.base + NPU_SRAM_SIZE_REG);
    runtime = readl(npu_dev.base + NPU_RUNTIME_ADDR_REG);
    params = readl(npu_dev.base + NPU_PARAMS_ADDR_REG);
    scu =  readl(npu_dev.base + NPU_COMMAND_ADDR_REG);
    scu_len =  readl(npu_dev.base + NPU_COMMAND_LEN_REG);
    sram_ctrl = readl(npu_dev.base + NPU_SRAM_RME_CTRL_REG);

    process_cycle_real = readl(npu_dev.base + NPU_TOTAL_PROCESS_CYCLES_REAL);
    axi_write_cmd_cnt_real = readl(npu_dev.base + NPU_AXI_CMD_WRITE_TOTAL_COUNT_RAEL);
    axi_read_cmd_cnt_real = readl(npu_dev.base + NPU_AXI_CMD_READ_TOTAL_COUNT_REAL);
    axi_write_data_cnt_real = readl(npu_dev.base + NPU_AXI_DATA_WRITE_TOTAL_COUNT_REAL);
    axi_read_data_cnt_real = readl(npu_dev.base + NPU_AXI_DATA_READ_TOTAL_COUNT_REAL);

    process_cycle_int = readl(npu_dev.base + NPU_TOTAL_PROCESS_CYCLES_INT); 
    axi_write_cmd_cnt_int = readl(npu_dev.base + NPU_AXI_CMD_WRITE_TOTAL_COUNT_INT);
    axi_read_cmd_cnt_int = readl(npu_dev.base + NPU_AXI_CMD_READ_TOTAL_COUNT_INT);
    axi_write_data_cnt_int = readl(npu_dev.base + NPU_AXI_DATA_WRITE_TOTAL_COUNT_INT);
    axi_read_data_cnt_int = readl(npu_dev.base + NPU_AXI_DATA_READ_TOTAL_COUNT_INT);
    
    callbackmask_arm0 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_0_REG);
    callbackmask_arm1 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_1_REG);
    callbackmask_arm2 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_2_REG);
    callbackmask_arm3 = readl(npu_dev.base + NPU_CALLBACK_MASK_ARM_3_REG);
   
    dma_addr = readl(npu_dev.base + NPU_DMAC_BASE_ADDR_REG);
    dma_ctrl = readl(npu_dev.base + NPU_DMAC_BASE_CTRL_REG);
    dma_sec_base = readl(npu_dev.base + NPU_DMAC_SEC_BASE_ADDR_REG);

    //Make the log more human-readable and user friendly
    seq_printf(s, "-------------------------------- Network --------------------------------\n");
    seq_printf(s, "  ID                Frame               CurrentLayer               \n");
    seq_printf(s, "  %d                %d                   %d                        \n",
               network_id, frame_id, layer);
    seq_printf(s, "-------------------------------- Status ---------------------------------\n");
    seq_printf(s, "  Hang mode    : %s\n", start & (1<<2)?"Y":"N");
    seq_printf(s, "  Debug mode   : %s\n", start & (1<<3)?"Y":"N");
    seq_printf(s, "  Cbuf dump    : %s\n", start & (1<<5)?"Y":"N");
    seq_printf(s, "  Cbuf clear   : %s\n", start & (1<<6)?"Y":"N");
    seq_printf(s, "  Cbuf dump to : 0x%08x\n", (start << 2) & 0xFFFFFE00);
    seq_printf(s, "  IrqReg value : 0x%08x, which means:\n", status);
    seq_printf(s, "  CurrentState : %s\n", NPU_STATE_TO_STRING((status >> 27) & 0xff));
    seq_printf(s, "  IrqStatus    : \n");
    if(status & NPU_IRQ_CBUF_READ_CONFLICT_BIT)
    {
        seq_printf(s, "    Cbuff read conflict!\n");
    }
    if(status & NPU_IRQ_DMAC_ERROR_BIT)
    {
        seq_printf(s, "    DMAC error!\n");
    }
    if(status & NPU_IRQ_AXIM_BRESP_ERROR_BIT)
    {
        seq_printf(s, "    AXIM bresp error!\n");
    }
    if(status & NPU_IRQ_DONE_BIT)
    {
        seq_printf(s, "    NPU DONE!\n");
    }
    if(status & NPU_IRQ_HANG_BIT)
    {
        seq_printf(s, "    NPU hang by another high-priority model!\n");
    }
    if(status & NPU_IRQ_CB_BIT)
    {
        seq_printf(s, "    NPU generates callback IRQ!\n");
    }
    if(status & NPU_IRQ_PAUSE_BIT)
    {
        seq_printf(s, "    NPU paused!\n");
    }
    seq_printf(s, "  Irq mask     : 0x%08x\n", irq_mask);
    seq_printf(s, "  Network ID used: \n");
    for(i = 0; i < 65535; i++)
    {
        if(npu_dev.id_bmp[i/32] & (1 << (i%32)))
        {
            seq_printf(s, "%d ", i);
            j++;
            if(j%8 == 0)
            {
                seq_printf(s, "\n");
            }
        }
    }
    seq_printf(s, "\n");
    
    seq_printf(s, "-------------------------------- Memory ---------------------------------\n");
    seq_printf(s, "  Input[0-3] : 0x%08x 0x%08x 0x%08x 0x%08x\n", input0, input1, input2, input3);
    seq_printf(s, "  Output[0-3]: 0x%08x 0x%08x 0x%08x 0x%08x\n", output0, output1, output2, output3);
    seq_printf(s, "  Runtime    : 0x%08x\n", runtime);
    seq_printf(s, "  Params     : 0x%08x\n", params);
    seq_printf(s, "  Scu        : 0x%08x\n", scu);
    seq_printf(s, "  Scu Length : %d\n", scu_len);
    seq_printf(s, "  SRAM       : 0x%08x\n", sram_addr);
    seq_printf(s, "  SRAM size  : %d\n", sram_size);
    seq_printf(s, "------------------------------Configuration------------------------------\n");
    seq_printf(s, "  AXImQos[w/r]        : 0x%04x/0x%04x\n", (axi_qos >> 8 ) & 0xf, (axi_qos >> 12) & 0xf);
    seq_printf(s, "  CallbackMaskArm[0-3]: 0x%x 0x%x 0x%x 0x%x\n", callbackmask_arm0, callbackmask_arm1, callbackmask_arm2, callbackmask_arm3);
    seq_printf(s, "  SRAMCtrl            : 0x%x(%s)\n", sram_ctrl, 
    	sram_ctrl & (1<<16) ? "SRAM Off" : (sram_ctrl & (1<<17)) ? "SRAM On" : "Auto");
    seq_printf(s, "  DMA Base            : 0x%x\n", dma_addr);
    seq_printf(s, "  DMA Security Base   : 0x%x\n", dma_sec_base);
    seq_printf(s, "  DMA ch %x security[w/r] %x/%x axcache[w/r] %x/%x\n", dma_ctrl & 0x3, 
    	(dma_ctrl >> 2) & 0x01, (dma_ctrl >> 3) & 0x01, (dma_ctrl >> 4) & 0x0f, (dma_ctrl >> 8) & 0x0f);
    seq_printf(s, "----------------------- Realtime Statistics -----------------------------\n");
    seq_printf(s, "  NPU cycles          : %d\n", process_cycle_real);
    seq_printf(s, "  NPU write cmd count : %d\n", axi_write_cmd_cnt_real);
    seq_printf(s, "  NPU write data count: %d\n", axi_write_data_cnt_real);
    seq_printf(s, "  NPU read cmd count  : %d\n", axi_read_cmd_cnt_real);
    seq_printf(s, "  NPU read data count : %d\n", axi_read_data_cnt_real);
    seq_printf(s, "------------------ Statistics from start to IRQ per network -------------\n");
    seq_printf(s, "  NPU total cycles          : %d\n", process_cycle_int);
    seq_printf(s, "  NPU total write cmd count : %d\n", axi_write_cmd_cnt_int);
    seq_printf(s, "  NPU total write data count: %d\n", axi_write_data_cnt_int);
    seq_printf(s, "  NPU total read cmd count  : %d\n", axi_read_cmd_cnt_int);
    seq_printf(s, "  NPU total read data count : %d\n", axi_read_data_cnt_int);

    seq_printf(s, "-------------------------------- Debug ----------------------------------\n");
    //cdma debug value
    apb_ctrl_modules = 0x0 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  CDMA Module :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

    //csc debug value
    apb_ctrl_modules = 0x1 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  CSC Module  :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

    //cmac debug value
    apb_ctrl_modules = 0x2 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  CMAC Module :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

    //sdp debug value
    apb_ctrl_modules = 0x3 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  SDP Module  :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

    //pdp debug value
    apb_ctrl_modules = 0x4 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  PDP Module  :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

    //fwc debug value
    apb_ctrl_modules = 0x5 << 4;
    ar_npu_proc_apb_state(abp_debug_value, apb_ctrl_modules);
    seq_printf(s,
        "  FWC Module  :\n"
        "    Reg0-Reg7 : 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n"
        "    Reg8-Reg15: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
        abp_debug_value[0], abp_debug_value[1], abp_debug_value[2], abp_debug_value[3],
        abp_debug_value[4], abp_debug_value[5], abp_debug_value[6], abp_debug_value[7],
        abp_debug_value[8], abp_debug_value[9], abp_debug_value[10], abp_debug_value[11],
        abp_debug_value[12], abp_debug_value[13], abp_debug_value[14], abp_debug_value[15]);

     mutex_unlock(&npu_dev.lock);
    return 0;
}

#endif
int ar_npu_proc_fifo_show(struct seq_file *s, void *p)
{
    seq_printf(s,
        "Normal FIFO: size %d used %d\n"
        "High FIFO  : size %d used %d\n",
        kfifo_size(&g_npu_normal_fifo), kfifo_len(&g_npu_normal_fifo),
        kfifo_size(&g_npu_high_fifo), kfifo_len(&g_npu_high_fifo));

    return 0;
}

int ar_npu_proc_result_show(struct seq_file *s, void *p)
{
    int i = 0;

    for(i = 0; i < NPU_MAX_QUEUE_DEPTH; i++)
    {
        if(g_npu_status[i].network_id == 0)
        {
            break;
        }

        seq_printf(s,
            "Network    : %d\n"
            "Frame      : %d\n"
            "Priority   : %d\n"
            "LayerNum   : %d\n"
            "Status     : 0x%08x\n"
            "RestartAddr: 0x%08x\n"
            "CallbackId : 0x%08x 0x%08x 0x%08x 0x%08x\n",
            g_npu_status[i].network_id, g_npu_status[i].frame_id, g_npu_status[i].priority, g_npu_status[i].layer_num, g_npu_status[i].irq_status,
            g_npu_status[i].restart_addr, g_npu_status[i].callback_id[0], g_npu_status[i].callback_id[1], g_npu_status[i].callback_id[2], g_npu_status[i].callback_id[3]);
    }

    return 0;
}

int ar_npu_proc_usage_show(struct seq_file *s, void *p)
{
    struct timespec time_cur;
    static struct ar_npu_usage usage_last;
    struct ar_npu_usage usage_cur;
    unsigned int usage_percentage;

    ktime_get_real_ts64(&time_cur);
    usage_cur.time_total = (time_cur.tv_sec * 100000 + time_cur.tv_nsec / 10000) - npu_dev.time_start;
    usage_cur.time_running = npu_dev.time_running;
    usage_cur.time_idle = usage_cur.time_total - usage_cur.time_running;
    usage_percentage = (usage_cur.time_running - usage_last.time_running) * 100/(usage_cur.time_total - usage_last.time_total);

    usage_last.time_total = usage_cur.time_total;
    usage_last.time_running = usage_cur.time_running;
    usage_last.time_idle = usage_cur.time_idle;

    seq_printf(s,"UsagePercentage     : %d\n"
                 "TotalTime(0.01ms)   : %lld\n"
                 "RunningTime(0.01ms) : %lld\n"
                 "IdleTime(0.01ms)    : %lld\n",
                    usage_percentage, usage_cur.time_total,
                    usage_cur.time_running, usage_cur.time_idle);

    return 0;
}
