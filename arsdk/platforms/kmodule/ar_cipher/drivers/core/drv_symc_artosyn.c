#include <linux/string.h>
#include <asm/cacheflush.h>
#include "drv_symc_artosyn.h"
#include "drv_symc.h"

#if 0
#define SPACC_IRQ_EN              (SPAcc_BaseAddress + 0x0)
#define SPACC_IRQ_STAT            (SPAcc_BaseAddress + 0x4)
#define SPACC_IRQ_CTRL            (SPAcc_BaseAddress + 0x8)
#define SPACC_FIFO_STAT           (SPAcc_BaseAddress + 0xc)
#define SPACC_SDMA_BRST_SZ        (SPAcc_BaseAddress + 0x10)
#define SPACC_SRC_PTR             (SPAcc_BaseAddress + 0x20)
#define SPACC_DST_PTR             (SPAcc_BaseAddress + 0x24)
#define SPACC_AAD_OFFSET          (SPAcc_BaseAddress + 0x28)
#define SPACC_PRE_AAD_LEN         (SPAcc_BaseAddress + 0x2c)
#define SPACC_POST_AAD_LEN        (SPAcc_BaseAddress + 0x30)
#define SPACC_PROC_LEN            (SPAcc_BaseAddress + 0x34)
#define SPACC_ICV_LEN             (SPAcc_BaseAddress + 0x38)
#define SPACC_ICV_OFFSET          (SPAcc_BaseAddress + 0x3c)
#define SPACC_IV_OFFSET           (SPAcc_BaseAddress + 0x40)
#define SPACC_SW_CTRL             (SPAcc_BaseAddress + 0x44)
#define SPACC_AUX_INFO            (SPAcc_BaseAddress + 0x48)
#define SPACC_CTRL                (SPAcc_BaseAddress + 0x4c)
#define SPACC_STAT_POP            (SPAcc_BaseAddress + 0x50)
#define SPACC_STATUS              (SPAcc_BaseAddress + 0x54)
#define SPACC_STAT_WD_CTRL        (SPAcc_BaseAddress + 0x80)
#define SPACC_KEY_SZ              (SPAcc_BaseAddress + 0x100)
#define SPACC_VSPACC_RQST         (SPAcc_BaseAddress + 0x140)
#define SPACC_VSPACC_ALLOC        (SPAcc_BaseAddress + 0x144)
#define SPACC_VSPACC_PRIORITY     (SPAcc_BaseAddress + 0x148)
#define SPACC_VSPACC_RC4_KEY_REQ  (SPAcc_BaseAddress + 0x150)
#define SPACC_VSPACC_RC4_KEY_GNT  (SPAcc_BaseAddress + 0x154)
#define SPACC_VERSION             (SPAcc_BaseAddress + 0x180)
#define SPACC_VERSION_EXT         (SPAcc_BaseAddress + 0x184)
#define SPACC_VERSION_EXT_2       (SPAcc_BaseAddress + 0x190)
#define SPACC_SECURE_CTRL         (SPAcc_BaseAddress + 0x1c0)
#define SPACC_SECURE_CTX_RELEASE  (SPAcc_BaseAddress + 0x1c4)
#define SPACC_CIPH_CTX_0          (SPAcc_BaseAddress + 0x4000)
#define SPACC_HASH_CTX_0          (SPAcc_BaseAddress + 0x8000)

#define  SPACC_CIPH_CTX_SZ        0x80
#define  SPACC_HASH_CTX_SZ        0x80
//------------------------------register field definitions------------------------------
// SPACC_IRQ_EN bit define
#define IRQ_CMD0_EN_BIT           0
#define IRQ_CMD1_EN_BIT           1
#define IRQ_CMD2_EN_BIT           2
#define IRQ_STAT_EN_BIT           4
#define IRQ_RC4_DMA_EN_BIT        8
#define IRQ_STAT_WD_EN_BIT        12
#define IRQ_GLBL_EN_BIT           31

// SPACC_IRQ_STAT bit define
#define IRQ_STAT_CMD0_BIT         0
#define IRQ_STAT_CMD1_BIT         1
#define IRQ_STAT_CMD2_BIT         2
#define IRQ_STAT_STAT_BIT         4
#define IRQ_STAT_RC4_DMA_BIT      8
#define IRQ_STAT_STAT_WD_BIT      12

// SPACC_IRQ_CTRL bit define
#define IRQ_CTRL_CMD0_CNT_LEN     8
#define IRQ_CTRL_CMD0_CNT_SHIFT   0
#define IRQ_CTRL_STAT_CNT_LEN     9
#define IRQ_CTRL_STAT_CNT_SHIFT   16

// SPACC_FIFO_STAT bit define
#define FIFO_STAT_CMD0_CNT_LEN    9
#define FIFO_STAT_CMD0_CNT_SHIFT  0
#define FIFO_STAT_CMD0_FULL_BIT   15
#define FIFO_STAT_CNT_LEN         9
#define FIFO_STAT_CNT_SHIFT       16
#define FIFO_STAT_EMPTY_BIT       31

// SPACC_SDMA_BRST_SZ bit define
#define SDMA_BRST_SZ_LEN          5
#define SDMA_BRST_SZ_SHIFT        0

// SPACC_AAD_OFFSET bit define
#define AAD_SRC_OFFSET_LEN        16
#define AAD_SRC_OFFSET_SHIFT      0
#define AAD_DST_OFFSET_LEN        16
#define AAD_DST_OFFSET_SHIFT      16

// SPACC_IV_OFFSET bit define
#define IV_OFFSET_len             31
#define IV_OFFSET_SHIFT           0
#define IV_OFFSET_ENABLE_BIT      31

// SPACC_SW_CTRL bit define
#define SW_ID_LEN                 8
#define SW_ID_SHIFT               0
#define SW_CTRL_PRIORITY_LEN      2
#define SW_CTRL_PRIORITY_SHIFT    30

// SPACC_AUX_INFO bit define
#define AUX_INFO_DIR_BIT          0
#define AUX_INFO_BIT_ALIGN_LEN    3
#define AUX_INFO_BIT_ALIGN_SHIFT  1
#define CRC_REF_IN_BIT            1
#define CRC_REF_OUT_BIT           2
#define CRC_REF_INV_BIT           3
#define CBC_CS_SEL_LEN            2
#define CBC_CS_SEL_SHIFT          16

// SPACC_CTRL bit define
#define CIPH_ALG_LEN              3
#define CIPH_ALG_SHIFT            0
#define HASH_ALG_LEN              5
#define HASH_ALG_SHIFT            3
#define CIPH_MODE_LEN             4
#define CIPH_MODE_SHIFT           8
#define HASH_MODE_LEN             2
#define HASH_MODE_SHIFT           12
#define MSG_BEGIN_BIT             14
#define MSG_END_BIT               15
#define CTX_IDX_LEN               8
#define CTX_IDX_SHIFT             16
#define ENCRYPT_BIT               24
#define AAD_COPY_BIT              25
#define ICV_PT_BIT                26
#define ICV_ENC_BIT               27
#define ICV_APPEND_BIT            28
#define KEY_EXP_BIT               29
#define SEC_KEY_BIT               31

// SPACC_STATUS bit define
#define STATUS_SW_ID_LEN          8
#define STATUS_SW_ID_SHIFT        0
#define STATUS_RET_CODE_LEN       3
#define STATUS_RET_CODE_SHIFT     24
#define STATUS_SEC_CMD_BIT        31

//SPACC KEY_SZ bit define
#define KEY_SIZE_LEN              8
#define KEY_SIZE_SHIFT            0
#define KEY_SIZE_CTX_IDX_LEN      8
#define KEY_SIZE_CTX_IDX_SHIFT    8
#define KEY_SIZE_CIPHER_BIT       31

//SPACC VSPACC_PRIORITY bit define
#define PRIORITY_MODE_BIT         0
#define PRIORITY_WEIGHT_LEN       4
#define PRIORITY_WEIGHT_SHIFT     8

//SPACC  VERSION bit define
#define VERSION_MINOR_LEN         4
#define VERSION_MINOR_SHIFT       0
#define VERSION_MAJOR_LEN         4
#define VERSION_MAORR_SHIFT       4
#define VERSION_QOS_BIT           8
#define VERSION_TYPE_LEN          2
#define VERSION_TYPE_SHIFT        9
#define VERSION_AUX_BIT           11
#define VERSION_VSPACC_IDX_LEN    3
#define VERSION_VSPACC_IDX_SHIFT  12
#define VERSION_VSPACC_PARTIAL_PKT_BIT 15
#define VERSION_PROJECT_LEN       16
#define VERSION_PROJECT_SHIFT     16

//SPACC VERSION EXT
#define VERSION_EXT_CTX_CNT_LEN        8
#define VERSION_EXT_CTX_CNT_SHIFT      0
#define VERSION_EXT_RC4_CTX_CNT_LEN    8
#define VERSION_EXT_RC4_CTX_CNT_SHIFT  8
#define VERSION_EXT_VSPACC_CNT_LEN     4
#define VERSION_EXT_VSPACC_CNT_SHIFT   16
#define VERSION_EXT_CIPH_CTX_SZ_LEN    3
#define VERSION_EXT_CIPH_CTX_SZ_SHIFT  20
#define VERSION_EXT_HASH_CTX_SZ_LEN    3
#define VERSION_EXT_HASH_CTX_SZ_SHIFT  24
#define VERSION_EXT_DMA_TYPE_LEN       2
#define VERSION_EXT_DMA_TYPE_SHIFT     28


//SPACC  VERSION_EXT_2
#define VERSION_CMD0_DEPTH_LEN         9
#define VERSION_CMD0_DEPTH_SHIFT       0
#define VERSION_CMD0_STAT_DEPTH_LEN    9
#define VERSION_CMD0_STAT_DEPTH_SHIFT  16

//SPACC  SECURE_CTRL
#define MS_SRC_BIT                0
#define MS_DST_BIT                1
#define MS_DDT_BIT                2
#define MS_RC4_LOCK_BIT           30
#define SECURE_CTRL_LOCK_BIT      31

//------------------------------register field values------------------------------
#define SPACC_KEY_CONTEXT_IDX_0   0
#define SPACC_KEY_CONTEXT_IDX_1   1


//IRQ enable setting
#define IRQ_ENABLE                1

//SPAcc pop value
#define POP_VALUE                 1

#define DES_IV_OFFSET       0
#define DES_KEY1_OFFSET     0
#define DES_KEY2_OFFSET     0
#define DES_KEY3_OFFSET     0


//Iv offset int key context
#define AES_IV_OFFSET     0x20

//////////////////////////////////////////////////////////////////////////
#define spacc_get_bit(value, bit) \
		((value >> bit) & 0x1)

#define spacc_set_bit(value, bit) \
		(value | (1 << bit))

#define spacc_clear_bit(value, bit) \
		(value & (~(1 << bit)))

#define spacc_get_bits(value, shift, len) \
		((value >> shift) & ((1 << len) - 1))

#define spacc_set_bits(value, shift, value2) \
		(value | (value2 << shift))

/////////////////////////////////////////////////////////////////////////

static inline void spacc_write_reg32(volatile void __iomem *reg32, unsigned int value)
{
	//*(volatile unsigned int *)reg32 = value;
	writel(value, reg32);
	mb();
}

static inline unsigned int spacc_read_reg32(volatile void __iomem *reg32)
{
	unsigned int value;
	value = readl(reg32);
	return value;
	//return *(volatile unsigned int *)reg32;
}

/* 512k at most for each operation */
#define SPACC_MAX_LENGTH  (512 * 1024)

/*
 * Buffer data descriptor table, 8 bytes aligned
 * addr: buffer address
 * len:   buffer length
 */
struct spacc_ddt {
	unsigned int addr;
	unsigned int len;
} __attribute__((aligned(8)));

struct spacc_cipher_job
{
	struct spacc_ddt* src_ddt;
	struct spacc_ddt* dst_ddt;

	unsigned long algo;
	unsigned long mode;                 // Encription Algorith mode
	unsigned long enc;

	unsigned long iv_len;
	unsigned long iv_offset;

	unsigned long proc_len;

	unsigned long key_sz;

	unsigned int key_ctx;
	unsigned int use_otp;
	unsigned char key[64];
	crypto_queue_head  queue;        /*!<  quene list */
	callback_symc_isr callback;      /*!<  isr callback functon */
	callback_symc_destory destory;   /*!<  destory callback functon */
	void *ctx;                       /*!<  params for isr callback functon */
};

static struct spacc_ddt src_ddt;
static struct spacc_ddt dst_ddt;
static struct spacc_cipher_job job;

static void __iomem * SPAcc_BaseAddress;

#define SPAcc_Base          0x01180000
#define SPACC_SIZE		0x40000

void spacc_driver_init(void)
{
	AR_LOG_FuncEnter();

	SPAcc_BaseAddress = ioremap_nocache(SPAcc_Base, SPACC_SIZE);
	//Disable interrupt
	//spacc_write_reg32(SPACC_IRQ_EN, 0x0);

	job.src_ddt = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);
	printk("==== 0====\n");
	job.dst_ddt = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);
	printk("==== 1====\n");
	memset(job.src_ddt, 0, sizeof(struct spacc_ddt));
	memset(job.dst_ddt, 0, sizeof(struct spacc_ddt));
	// spacc_ddt * p_dst = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);

	//memcpy(p_src, &job.src_ddt, sizeof(struct spacc_ddt));
	//memcpy(p_dst, &job.dst_ddt, sizeof(struct spacc_ddt));

	//memset(&job, 0, sizeof(struct spacc_cipher_job));
	AR_LOG_FuncExit();
}

static void spacc_cipher_set_algorithm(unsigned int algo, unsigned int mode, int encrypt)
{
	job.algo = algo;
	job.mode = mode;
	job.enc = encrypt;
}

static void spacc_cipher_set_input(unsigned char *src, unsigned int length)
{
	//job.src_ddt->addr = (unsigned int)virt_to_phys(src);
	job.src_ddt->addr = (unsigned int)src;
	job.src_ddt->len = length;
	job.proc_len = length;
}

static void spacc_cipher_set_output(unsigned char *dst, unsigned int length)
{
	//job.dst_ddt->addr = (unsigned int)virt_to_phys(dst);
	job.dst_ddt->addr = (unsigned int)dst;
	job.dst_ddt->len = length;
}

spacc_cipher_set_key_size(unsigned int length)
{
	job.key_sz = length;
}

static int spacc_cipher_set_key(unsigned char *key, unsigned int length, unsigned int ctx)
{
	unsigned int value = 0;
	int i =0;

	//Only support AES
	if(job.algo != CIPH_ALG_AES)
		return 1;

	job.key_sz = length;
	job.key_ctx = SPACC_KEY_CONTEXT_IDX_0 + ctx;

	//If use OTP root, set cipher context to zero
	//Or move key to cipher context
	if(!key) {
		job.use_otp = 1;
	}

	//Set key size register
	value = job.key_sz;
	value = spacc_set_bit(value, KEY_SIZE_CIPHER_BIT);
	value = spacc_set_bits(value, KEY_SIZE_CTX_IDX_SHIFT, job.key_ctx);
	spacc_write_reg32(SPACC_KEY_SZ, value);

	if(key) {
		memcpy((unsigned char *)SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * job.key_ctx, key, length);
		for(i = 0; i < length; i++)
			job.key[i] = key[i];
	}
	else {
		memset((unsigned char *)SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * job.key_ctx, 0, length);
	}
}

static int spacc_cipher_set_iv(unsigned char *iv, unsigned long length, unsigned long offs)
{
	unsigned char iv_tmp[16];

    AR_LOG_FuncEnter();

	//Only support AES
	if(job.algo != CIPH_ALG_AES)
		return 1;

	memcpy(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * job.key_ctx + AES_IV_OFFSET, iv, length);

	job.iv_len = length;
	job.iv_offset = offs;

	memcpy(iv_tmp, SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * job.key_ctx + AES_IV_OFFSET, length);
	//printk("get_iv: iv_tmp[0]:%02x\n", iv_tmp[0]);
	//printk("get_iv: iv_tmp[0]:%02x\n", iv_tmp[1]);
	AR_LOG_FuncExit();
	return 0;
}

static void spacc_cipher_get_iv(unsigned char *iv, unsigned long length, unsigned long offs)
{
	memcpy(iv, SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * job.key_ctx + AES_IV_OFFSET, length);
}

static void spacc_start_cipher_operation(void)
{
	unsigned int value;

	//struct spacc_ddt * p_src = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);
	// spacc_ddt * p_dst = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);

	//memcpy(p_src, &job.src_ddt, sizeof(struct spacc_ddt));
	//memcpy(p_dst, &job.dst_ddt, sizeof(struct spacc_ddt));

	//__flush_dcache_area(p_src, 64);
	//__flush_dcache_area(p_dst, 64);

	//Set src & dst register
	//spacc_write_reg32(SPACC_SRC_PTR, (unsigned int)(virt_to_phys(p_src)));
	//spacc_write_reg32(SPACC_DST_PTR, (unsigned int)(virt_to_phys(p_dst)));
	__flush_dcache_area(job.src_ddt, sizeof(struct spacc_ddt));
	__flush_dcache_area(job.dst_ddt, sizeof(struct spacc_ddt));

	//Set src & dst register
	spacc_write_reg32(SPACC_SRC_PTR, (unsigned int)(virt_to_phys(job.src_ddt)));
	spacc_write_reg32(SPACC_DST_PTR, (unsigned int)(virt_to_phys(job.dst_ddt)));

	//Set 0 to below registers
	spacc_write_reg32(SPACC_AAD_OFFSET, 0x0);
	spacc_write_reg32(SPACC_PRE_AAD_LEN, 0x0);
	spacc_write_reg32(SPACC_POST_AAD_LEN, 0x0);

	//Set PROC len register
	spacc_write_reg32(SPACC_PROC_LEN, job.proc_len);

	spacc_write_reg32(SPACC_ICV_LEN, 0x0);
	spacc_write_reg32(SPACC_ICV_OFFSET, 0x0);

	//Set IV register
	value = job.iv_offset;
	value = spacc_clear_bit(value, IV_OFFSET_ENABLE_BIT);
	spacc_write_reg32(SPACC_IV_OFFSET, value);

	//Set SW register
	if(job.enc)
		spacc_write_reg32(SPACC_SW_CTRL, 0x1);
	else
		spacc_write_reg32(SPACC_SW_CTRL, 0x2);

	spacc_write_reg32(SPACC_AUX_INFO, 0x0);

	//Set ctrl register
	value = 0;
	if(job.enc)
		value = spacc_set_bit(value, ENCRYPT_BIT);
	value = spacc_set_bits(value, CIPH_ALG_SHIFT, job.algo);
	value = spacc_set_bits(value, CIPH_MODE_SHIFT, job.mode);
	if(job.use_otp) {
		value = spacc_set_bit(value, SEC_KEY_BIT);
	}
	value = spacc_set_bits(value, CTX_IDX_SHIFT, job.key_ctx);

	value = spacc_set_bit(value, MSG_BEGIN_BIT);
	value = spacc_set_bit(value, MSG_END_BIT);

	//Set key expand bit
	value = spacc_set_bit(value, KEY_EXP_BIT);

	spacc_write_reg32(SPACC_CTRL, value);
}

static void spacc_pop(void)
{
	unsigned int value = 0;

	value = spacc_read_reg32(SPACC_STATUS);
	printk("====spacc_pop SPACC_STATUS value:%02x====\n", value);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);
	printk("====spacc_pop SPACC_STATUS spacc_get_bits value:%02x====\n", value);

	spacc_write_reg32(SPACC_STAT_POP, POP_VALUE);

	value = spacc_read_reg32(SPACC_STATUS);
	printk("====spacc_pop SPACC_STATUS value:%02x====\n", value);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);
	printk("====spacc_pop SPACC_STATUS spacc_get_bits value:%02x====\n", value);
}

static unsigned int spacc_polling_for_finish(void)
{
	unsigned int fifo_stat = 0x0;
	unsigned int value = 0;

	value = spacc_read_reg32(SPACC_STATUS);
	printk("====polling SPACC_STATUS value:%02x====\n", value);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);
	printk("====polling SPACC_STATUS spacc_get_bits value:%02x====\n", value);

	while(fifo_stat == 0x0) {
		fifo_stat = spacc_read_reg32(SPACC_FIFO_STAT);
		printk("====polling SPACC_FIFO_STAT value:%02x====\n", fifo_stat);
		fifo_stat = spacc_get_bits(fifo_stat, FIFO_STAT_CNT_SHIFT, FIFO_STAT_CNT_LEN);
		printk("====polling SPACC_FIFO_STAT spacc_get_bits value:%02x====\n", fifo_stat);
	}

	value = spacc_read_reg32(SPACC_STATUS);
	printk("====polling SPACC_STATUS value:%02x====\n", value);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);
	printk("====polling SPACC_STATUS spacc_get_bits value:%02x====\n", value);

	return value;
}

unsigned int __spacc_cipher(unsigned int algo, unsigned int mode, unsigned int encrypt,
					unsigned char *src, unsigned int src_len,
					unsigned char *dst, unsigned int dst_len,
					unsigned char *key, unsigned int key_len,
					unsigned int ctx,
					unsigned char *iv, unsigned int iv_len)
{
	unsigned int ret;

	//memset(&job, 0, sizeof(struct spacc_cipher_job));

	spacc_cipher_set_algorithm(algo, mode, encrypt);
	spacc_cipher_set_input(src, src_len);
	spacc_cipher_set_output(dst, dst_len);
	spacc_cipher_set_key(key, key_len, ctx);
	spacc_cipher_set_iv(iv, iv_len, 0x20);

	spacc_start_cipher_operation();

	ret = spacc_polling_for_finish();

	spacc_pop();

	spacc_cipher_get_iv(iv, iv_len, 0x20);

	return ret;
}

unsigned int spacc_cipher(unsigned int algo, unsigned int mode, unsigned int encrypt,
					unsigned char *src, unsigned int src_len,
					unsigned char *dst, unsigned int dst_len,
					unsigned char *key, unsigned int key_len,
					unsigned int ctx,
					unsigned char *iv, unsigned int iv_len)
{
	int ret, tmp_len, tmp_len1, tmp_len2;
	uint32_t iv_tmp[8];

	memcpy(iv_tmp, iv, iv_len);

	//while(src_len) {
		//tmp_len1 = SPACC_MAX_LENGTH - ((unsigned long)src &(SPACC_MAX_LENGTH - 1));
		//tmp_len2 = SPACC_MAX_LENGTH - ((unsigned long)dst &(SPACC_MAX_LENGTH - 1));

		//tmp_len = tmp_len1 < tmp_len2 ? tmp_len1 : tmp_len2;
		//if(tmp_len > src_len)
		tmp_len = src_len;

		ret = __spacc_cipher(algo, mode, encrypt,
					src, tmp_len,
					dst, tmp_len,
					key, key_len,
					ctx,
					(unsigned char *)iv_tmp, iv_len);
		if(ret < 0)
			printk(KERN_INFO, "%x\n", ret);
			//break;

		//src += tmp_len;
		//dst += tmp_len;
		//src_len -= tmp_len;
	//}

	return ret;
}
#endif

struct spacc_cipher_job
{
	crypto_queue_head  queue;        /*!<  quene list */
	callback_symc_isr callback;      /*!<  isr callback functon */
	callback_symc_destory destory;   /*!<  destory callback functon */
	void *ctx;                       /*!<  params for isr callback functon */
};

static struct spacc_cipher_job job;

/*! \Length of aes ccm/gcm key */
#define AES_CCM_GCM_KEY_LEN             (16)

/*! symc already initialize or not */
static AR_U32 symc_initialize = AR_FALSE;

#if 0
static AR_U32 drv_symc_done_try(AR_U32 chn_num)
{
//	AR_U32 fifo_stat = 0;
//	uint32_t value;

	CRYPTO_ASSERT(CRYPTO_HARD_CHANNEL_MAX > chn_num);
#if 0
	//fifo_stat = spacc_read_reg32(SPACC_FIFO_STAT);
	//printk("====polling SPACC_FIFO_STAT value:%02x====\n", fifo_stat);
	//fifo_stat = spacc_get_bits(fifo_stat, FIFO_STAT_CNT_SHIFT, FIFO_STAT_CNT_LEN);

	fifo_stat = spacc_read_reg32(SPACC_FIFO_STAT);
	AR_LOG_INFO("fifo_stat = %02x\n", fifo_stat);
	fifo_stat = spacc_get_bits(fifo_stat, FIFO_STAT_CNT_SHIFT, FIFO_STAT_CNT_LEN);
	AR_LOG_INFO("fifo_stat1 = %02x\n", fifo_stat);

	value = spacc_read_reg32(SPACC_STATUS);
	AR_LOG_INFO("SPACC_STATUS = %02x\n", value);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);
	AR_LOG_INFO("value = %02x\n", value);

	return fifo_stat == 0 ? AR_FALSE : AR_TRUE;
#endif
	return AR_TRUE;
}
#endif

#ifdef CRYPTO_OS_INT_SUPPORT

/*! symc interrupt process function */
static irqreturn_t drv_symc_interrupt_isr(AR_S32 irq, void *devId)
{
	irqreturn_t ret = IRQ_HANDLED;
	crypto_queue_wait_up(&job.queue);

#if 0
    AR_U32 mask, i;
    symc_hard_context *ctx = AR_NULL;
    irqreturn_t ret = IRQ_HANDLED;

    /* get channel context*/
    //mask = drv_symc_done_notify();
    AR_LOG_DEBUG("symc irq: %d, mask 0x%x\n", irq, mask);

    //for (i=0; i< CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        //if ((mask >> i) & 0x01)
        {
            ctx = &hard_context[i];
            if ((ctx->callback) && (AR_FALSE == ctx->callback(ctx->ctx)))
            {
                /* contiue to compute */
                AR_LOG_DEBUG("contiue to compute chn %d\n", i);
                drv_symc_start(i);
            }
            else
            {
                /* finish */
                ctx->done = AR_TRUE;
                AR_LOG_DEBUG("chn %d wake up\n", i);
                crypto_queue_wait_up(&ctx->queue);
            }
        }
    }
#endif
    return ret;
}

/*! symc register interrupt process function */
static AR_S32 drv_symc_register_interrupt(void)
{
    AR_S32 ret = AR_FAILURE;
    /* request irq */
    AR_LOG_DEBUG("symc request irq, num %d, name %s\n", SYMC_IRQ_NUM, SYMC_IRQ_NAME);
    ret = crypto_request_irq(SYMC_IRQ_NUM, drv_symc_interrupt_isr, SYMC_IRQ_NAME);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("Irq request failure, irq = %d\n", SYMC_IRQ_NUM);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_REGISTER_IRQ);
        return ret;
    }

    /* initialize queue list*/
    for (i=0; i<CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        crypto_queue_init(&job.queue);
    }

    AR_LOG_FuncExit();

    return AR_SUCCESS;
}

/*! symc unregister interrupt process function */
static void drv_symc_unregister_interrupt(void)
{
    /* free irq */
    AR_LOG_DEBUG("symc free irq, num %d, name %s\n", SYMC_IRQ_NUM, SYMC_IRQ_NAME);
    crypto_free_irq(SYMC_IRQ_NUM, SYMC_IRQ_NAME);

    AR_LOG_FuncExit();
}

#endif

/*! set interrupt */
static void drv_symc_set_interrupt(AR_BOOL isEnable)
{
	AR_LOG_FuncEnter();

	spacc_set_interrupt(isEnable);

	AR_LOG_FuncExit();
}

#ifdef CRYPTO_OS_INT_SUPPORT
static AR_S32 drv_symc_get_err_code(AR_U32 chn_num)
{
	return -1;
}
#endif

static char chn_status[CRYPTO_HARD_CHANNEL_MAX] = {0};

AR_S32 drv_symc_alloc_chn(AR_U32 *chn_num)
{
    /* alloc channel */
    AR_U32 i;

	CRYPTO_ASSERT(AR_TRUE == symc_initialize);

	for(i=0; i<CRYPTO_HARD_CHANNEL_MAX; ++i){
		if(!chn_status[i]){
			chn_status[i] = 1;
			*chn_num = i;	
			AR_LOG_INFO("+++ alloc symc chn %d +++\n", i);
			return AR_SUCCESS;
		}
	}
	AR_LOG_ERROR("drv_symc_alloc_chn %d failed\n", i);

	return AR_FAILURE;
}

void drv_symc_free_chn(AR_U32 chn_num)
{
	AR_LOG_INFO("+++ free symc chn %d +++\n", chn_num);

	if(chn_num < CRYPTO_HARD_CHANNEL_MAX)
	{
		chn_status[chn_num] = 0;
		AR_LOG_INFO("+++ free symc chn  +++\n");
	}else{
		AR_LOG_ERROR("drv_symc_free_chn failed, invalid chn_num:%d\n", chn_num);
	}

	return;
}

AR_S32 drv_symc_init(void)
{
	AR_S32 ret = AR_SUCCESS;

	AR_LOG_FuncEnter();

	spacc_init();

	//clear interrupt
	//drv_symc_set_interrupt(AR_FALSE);

#ifdef CRYPTO_OS_INT_SUPPORT
	AR_LOG_INFO("symc interrupt configure\n");
	drv_symc_set_interrupt(AR_TRUE);

	AR_LOG_INFO("symc register interrupt function\n");
	ret = drv_symc_register_interrupt();
	if (AR_SUCCESS != ret)
	{
		AR_LOG_ERROR("error, register interrupt failed\n");
		AR_LOG_PrintFuncErr(drv_symc_register_interrupt, ret);
	}
#endif
	symc_initialize = AR_TRUE;

    AR_LOG_FuncExit();

	return ret;
}

AR_S32 drv_symc_deinit(void)
{
	AR_S32 i;

    AR_LOG_FuncEnter();

    CRYPTO_ASSERT(AR_TRUE == symc_initialize);

#ifdef CRYPTO_OS_INT_SUPPORT
    drv_symc_unregister_interrupt();
#endif

    /* free all channel */
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++)
    {
//        if (CIPHER_HARD_CHANNEL_MASK & (0x1 << i))
        {
            drv_symc_free_chn(i);
        }
    }

//	spacc_cipher_final();

    symc_initialize = AR_FALSE;

    AR_LOG_FuncExit();

    return AR_SUCCESS;
}

void drv_symc_add_buf_usage(AR_U32 chn_num, AR_U32 in, symc_node_usage usage)
{
    return;
}

AR_S32 drv_symc_set_iv(AR_U32 chn_num, AR_U32 iv[SYMC_IV_MAX_SIZE_IN_WORD], AR_U32 ivlen, AR_U32 flag)
{
	AR_S32 ret = AR_FAILURE;

	AR_LOG_FuncEnter();

	ret = spacc_cipher_set_iv((uint8_t *)iv);

	AR_LOG_FuncExit();

	return ret;
}

void drv_symc_get_iv(AR_U32 chn_num, AR_U32 iv[SYMC_IV_MAX_SIZE_IN_WORD])
{
	spacc_cipher_get_iv((uint8_t*)iv);
	return;
}

void drv_symc_set_key(AR_U32 chn_num, AR_U32 key[SYMC_KEY_MAX_SIZE_IN_WORD], AR_U32 odd)
{
	AR_LOG_FuncEnter();
	if(AR_FALSE == odd)
	{
		spacc_cipher_set_key_single((uint8_t*)key, odd);
	}
	else
	{
		//spacc_cipher_set_key(NULL, (uint8_t*)key);
	}
	AR_LOG_FuncExit();

	return ;
}

void drv_symc_set_sm1_sk(AR_U32 chn_num, AR_U32 key[SYMC_SM1_SK_SIZE_IN_WORD])
{
    return;
}

AR_S32 drv_symc_add_inbuf(AR_U32 chn_num, compat_addr buf_phy, AR_U32 buf_size, symc_node_usage usage)
{
	AR_LOG_FuncEnter();
	spacc_cipher_set_all_input((uint8_t*)ADDR_U64(buf_phy), buf_size);
	AR_LOG_FuncExit();

    return AR_SUCCESS;
}

AR_S32 drv_symc_add_outbuf(AR_U32 chn_num, compat_addr buf_phy, AR_U32 buf_size, symc_node_usage usage)
{
	AR_LOG_FuncEnter();
	spacc_cipher_set_all_output((uint8_t*)ADDR_U64(buf_phy), buf_size);
	AR_LOG_FuncExit();

   return AR_SUCCESS;
}

AR_S32 drv_aead_ccm_add_n(AR_U32 chn_num, AR_U8 *n)
{
	return -1;
}

AR_S32 drv_aead_ccm_add_a(AR_U32 chn_num, compat_addr buf_phy, AR_U32 buf_size)
{
	return -1;
}

AR_S32 drv_aead_gcm_add_a(AR_U32 chn_num, compat_addr buf_phy, AR_U32 buf_size)
{
	return -1;
}

AR_S32 drv_aead_gcm_add_clen(AR_U32 chn_num, AR_U8 *clen)
{
	return -1;
}

AR_S32 drv_aead_get_tag(AR_U32 chn_num, AR_U32 *tag)
{
	return -1;
}

AR_S32 drv_symc_node_check(symc_alg alg, symc_mode mode,
                    AR_U32 klen, AR_U32 block_size,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num)
{
    AR_U32 i = 0;
    AR_U32 total = 0;

    AR_LOG_FuncEnter();

    CRYPTO_ASSERT(AR_TRUE == symc_initialize);
    CRYPTO_ASSERT(0 != block_size);

    CRYPTO_UNUSED(input);
    CRYPTO_UNUSED(output);

    for (i=0; i<pkg_num; i++)
    {
        /* Used the odd key must accord with conditions as follows:*/
        if (usage_list[i] & SYMC_NODE_USAGE_ODD_KEY)
        {
             /* 1. Only support aes ecb/cbc/cfb/ofb/ctr */
            if ((SYMC_ALG_AES != alg)
               || ((SYMC_MODE_ECB != mode)
                  && (SYMC_MODE_CBC != mode)
                  && (SYMC_MODE_CFB != mode)
                  && (SYMC_MODE_OFB != mode)
                  && (SYMC_MODE_CTR != mode)))
            {
                 AR_LOG_ERROR("Odd key only support aes ecb/cbc/cfb/ofb/ctr.");
                 AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
                 return AR_ERR_CIPHER_INVALID_PARA;
            }

            /* 2. Only support aes128 */
            if (AES_CCM_GCM_KEY_LEN != klen)
            {
                 AR_LOG_ERROR("Odd key only support aes128, klen %d\n", klen);
                 AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
                 return AR_ERR_CIPHER_INVALID_PARA;
            }

            /* 3. each node length must be a multiple of 64*/
            if ((length[i] % (AES_BLOCK_SIZE * 4)) != 0)
            {
                 AR_LOG_ERROR("Odd key only supported when each node length is a multiple of 64.");
                 AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_LENGTH);
                 return AR_ERR_CIPHER_INVALID_LENGTH;
            }
        }

        /* each node length can't be zero*/
        if (length[i] == 0)
        {
            AR_LOG_ERROR("PKG len must large than 0.\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_LENGTH);
            return AR_ERR_CIPHER_INVALID_LENGTH;
        }

        /* check overflow */
        if (length[i] > ADDR_L32(input[i]) + length[i])
        {
            AR_LOG_ERROR("PKG len overflow.\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_LENGTH);
            return AR_ERR_CIPHER_INVALID_LENGTH;
        }
        total += length[i];
    }

    if((SYMC_ALG_NULL_CIPHER != alg) &&
        ((SYMC_MODE_ECB == mode)
        || (SYMC_MODE_CBC == mode)
        || (SYMC_MODE_CFB == mode)
        || (SYMC_MODE_OFB == mode)))
    {
       /* The length of data depend on alg and mode, which limit to hardware
        * for ecb/cbc/ofb/cfb, the total data length must aligned with block size.
        * for ctr/ccm/gcm, support any data length.
        */
        if (total % block_size != 0)
        {
            AR_LOG_ERROR("PKG len must align with 0x%x.\n", block_size);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_LENGTH);
            return AR_ERR_CIPHER_INVALID_LENGTH;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 drv_symc_config(AR_U32 chn_num, symc_alg alg, symc_mode mode, symc_width width, AR_U32 decrypt,
                AR_U32 sm1_round_num, symc_klen klen, AR_U32 hard_key)
{
	uint32_t symc_decrypt = 0;
	uint32_t symc_mode = -1;
	uint32_t symc_alg = -1;
	uint32_t symc_klen = -1;

    AR_LOG_FuncEnter();

    CRYPTO_ASSERT(AR_TRUE == symc_initialize);
    CRYPTO_ASSERT(CRYPTO_HARD_CHANNEL_MAX > chn_num);
    AR_LOG_CHECK_PARAM(SYMC_ALG_COUNT <= alg);
    AR_LOG_CHECK_PARAM(SYMC_MODE_GCM < mode);
    AR_LOG_CHECK_PARAM(SYMC_KEY_LEN_COUNT <= klen);

	spacc_cipher_create();

	if(0 == decrypt){
		symc_decrypt = 1;
	}else if(1 == decrypt){
		symc_decrypt = 0;
	}else{
		AR_LOG_ERROR("unsupport decrypt:%d\n", decrypt);
		return AR_FAILURE;
	}

	if(SYMC_MODE_ECB == mode)
	{
		symc_mode = 0;
	}else if(SYMC_MODE_CBC == mode){
		symc_mode = 1;
	}else if(SYMC_MODE_CTR == mode){
		symc_mode = 2;
	}else if(SYMC_MODE_CCM == mode){
		symc_mode = 3;
	}else if(SYMC_MODE_GCM == mode){
		symc_mode = 5;
	}else if(SYMC_MODE_OFB == mode){
		symc_mode = 7;
	}else if(SYMC_MODE_CFB == mode){
		symc_mode = 8;
	}else{
		AR_LOG_ERROR("unsupport mode:%d\n", mode);
		return AR_FAILURE;
	}

	if(SYMC_ALG_DES == alg){
		symc_alg = 1;
	}else if(SYMC_ALG_AES == alg){
		symc_alg = 2;
		AR_LOG_INFO("symc_alg:%d\n", symc_alg);
	}else{
		AR_LOG_ERROR("unsupport alg:%d\n", alg);
		return AR_FAILURE;
	}

	if(SYMC_KEY_AES_192BIT == klen){
		symc_klen = AES_KEY_192BIT;
	}else if(SYMC_KEY_AES_256BIT ==klen ){
		symc_klen = AES_KEY_256BIT;
	}else if(SYMC_KEY_TDES_2KEY ==klen ){
		symc_klen = TDES_KEY_128BIT;
	}else{
		AR_LOG_ERROR("unsupport klen:%d\n", klen);
		return AR_FAILURE;
	}

	spacc_cipher_set_algorithm(symc_alg, symc_mode);
	spacc_cipher_set_encrypt(symc_decrypt);
	spacc_cipher_set_key_ctx(0);
	spacc_cipher_set_keylen(symc_klen);

	//spacc_cipher_set_algorithm(symc_alg, symc_mode, symc_decrypt);
	//spacc_cipher_set_encrypt(symc_decrypt);
	//spacc_cipher_set_key_size(symc_klen);
	//spacc_cipher_set_key_ctx(0);

    AR_LOG_FuncExit();

    return AR_SUCCESS;
}

AR_S32 drv_symc_set_isr_callback(AR_U32 chn_num, callback_symc_isr callback, void *ctx)
{

    AR_LOG_FuncEnter();

    CRYPTO_ASSERT(AR_TRUE == symc_initialize);
    CRYPTO_ASSERT(CRYPTO_HARD_CHANNEL_MAX > chn_num);

    job.callback = callback;
    job.ctx = ctx;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 drv_symc_start(AR_U32 chn_num)
{
	AR_LOG_FuncEnter();

	//spacc_start_cipher_operation();
	spacc_cipher_start(1);
	AR_LOG_FuncExit();

	return AR_SUCCESS;
}

AR_S32 drv_symc_wait_done(AR_U32 chn_num, AR_U32 timeout)
{
	AR_S32 ret = AR_SUCCESS;

#ifdef CRYPTO_OS_INT_SUPPORT
	AR_U32 int_valid = 1;
    /* interrupt support, wait irq*/
    if (int_valid)
    {
		/* wait interrupt */
		ret = crypto_queue_wait_timeout(ctx->queue, &ctx->done, timeout);
		if ((0x00 >= ret) && (-ERESTARTSYS != ret))
		{
			AR_LOG_ERROR("wait done timeout, chn=%d\n", chn_num);
			AR_LOG_PrintFuncErr(crypto_queue_wait_timeout, ret);
			drv_symc_get_err_code(chn_num);
			return AR_ERR_CIPHER_TIMEOUT;
		}
	}
	else /* interrupt unsupport, query the raw interrupt flag*/
#endif
    {
    #if 0
        AR_S32 i;
		AR_LOG_INFO("wait done timeout, timeout=%d\n", timeout);
        for (i=0; i<timeout; i++)
        {
            if (drv_symc_done_try(chn_num))
            {
            	msleep(10);
                break;
            }
			//AR_LOG_INFO("wait done timeout, timeout=%d\n", timeout);
            if (MS_TO_US >= i)
            {
                crypto_udelay(1);  /* short waitting for 1000 us */
            }
            else
            {
                crypto_msleep(1);  /* long waitting for 5000 ms*/
            }
        }

        if (timeout <= i)
        {
            AR_LOG_ERROR("symc wait done timeout, chn=%d\n", chn_num);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_TIMEOUT);
            drv_symc_get_err_code(chn_num);
            return AR_ERR_CIPHER_TIMEOUT;
        }
	#endif
    }

	spacc_cipher_final();

	return ret;
}

AR_S32 drv_symc_set_destory_callbcak(AR_U32 chn_num, callback_symc_destory destory)
{
    AR_LOG_FuncEnter();

    job.destory = destory;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

/******* proc function begin ********/
#if (1 == AR_PROC_SUPPORT)
AR_S32 drv_symc_proc_status(symc_chn_status *status)
{
	return -1;
}
#endif
/******* proc function end ********/

void drv_symc_get_capacity(symc_capacity *capacity)
{
    AR_LOG_FuncEnter();

    // the mode depend on alg, which limit to hardware
    // des/3des support ecb/cbc/cfb/ofb
    // aes support ecb/cbc/cfb/ofb/ctr/ccm/gcm
    // aes support ecb/cbc/ctr/ccm/gcm/ofb/cfb
    // sm1 support ecb/cbc/cfb/ofb
    // sm4 support ecb/cbc/ctr

    crypto_memset(capacity, sizeof(symc_capacity), 0,  sizeof(symc_capacity));

    /* AES */
    capacity->aes_ecb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_cbc = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_ofb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_cfb = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_ctr = CRYPTO_CAPACITY_SUPPORT;
#ifdef CHIP_AES_CCM_GCM_SUPPORT
    capacity->aes_ccm = CRYPTO_CAPACITY_SUPPORT;
    capacity->aes_gcm = CRYPTO_CAPACITY_SUPPORT;
#endif

#if 0
    /* TDES */
    capacity->tdes_ecb = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_cbc = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_ofb = CRYPTO_CAPACITY_SUPPORT;
    capacity->tdes_cfb = CRYPTO_CAPACITY_SUPPORT;
#endif
    /* DES */
    capacity->des_ecb  = CRYPTO_CAPACITY_SUPPORT;
    capacity->des_cbc  = CRYPTO_CAPACITY_SUPPORT;
    capacity->des_ofb  = CRYPTO_CAPACITY_SUPPORT;
    capacity->des_cfb  = CRYPTO_CAPACITY_SUPPORT;

    /* SM1 */
#ifdef CHIP_SYMC_SM1_SUPPORT
    capacity->sm1_ecb  = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm1_cbc  = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm1_ofb  = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm1_cfb  = CRYPTO_CAPACITY_SUPPORT;
#endif

#if 0
    /* SM4 */
    capacity->sm4_ecb  = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm4_cbc  = CRYPTO_CAPACITY_SUPPORT;
    capacity->sm4_ctr  = CRYPTO_CAPACITY_SUPPORT;

    /* DMA */
    capacity->dma = CRYPTO_CAPACITY_SUPPORT;
#endif
    return;
}

