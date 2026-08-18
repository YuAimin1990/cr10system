/*
 * Artosync spacc driver for optee
 *
 * Support Algorithms:
 * AES CBC/ECB/XTS
 *
 * support hash by xlzhao 20200927, but don't change function name
 *
 */
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/page.h>

#include "spacc.h"
#include "drv_osal_lib_linux.h"

#define SPACC_BASE                (0x01180000)

//------------------------------register definitions------------------------------
#define SPACC_IRQ_EN              (0x0)
#define SPACC_IRQ_STAT            (0x4)
#define SPACC_IRQ_CTRL            (0x8)
#define SPACC_FIFO_STAT           (0xc)
#define SPACC_SDMA_BRST_SZ        (0x10)
#define SPACC_SRC_PTR             (0x20)
#define SPACC_DST_PTR             (0x24)
#define SPACC_AAD_OFFSET          (0x28)
#define SPACC_PRE_AAD_LEN         (0x2c)
#define SPACC_POST_AAD_LEN        (0x30)
#define SPACC_PROC_LEN            (0x34)
#define SPACC_ICV_LEN             (0x38)
#define SPACC_ICV_OFFSET          (0x3c)
#define SPACC_IV_OFFSET           (0x40)
#define SPACC_SW_CTRL             (0x44)
#define SPACC_AUX_INFO            (0x48)
#define SPACC_CTRL                (0x4c)
#define SPACC_STAT_POP            (0x50)
#define SPACC_STATUS              (0x54)
#define SPACC_STAT_WD_CTRL        (0x80)
#define SPACC_KEY_SZ              (0x100)
#define SPACC_VSPACC_RQST         (0x140)
#define SPACC_VSPACC_ALLOC        (0x144)
#define SPACC_VSPACC_PRIORITY     (0x148)
#define SPACC_VSPACC_RC4_KEY_REQ  (0x150)
#define SPACC_VSPACC_RC4_KEY_GNT  (0x154)
#define SPACC_VERSION             (0x180)
#define SPACC_VERSION_EXT         (0x184)
#define SPACC_VERSION_EXT_2       (0x190)
#define SPACC_SECURE_CTRL         (0x1c0)
#define SPACC_SECURE_CTX_RELEASE  (0x1c4)
#define SPACC_CIPH_CTX_0          (0x4000)
#define SPACC_HASH_CTX_0          (0x8000)

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
#define CHIP_ALG_DES 			  1
#define CHIP_ALG_AES 			  2
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
#define SPACC_KEY_CONTEXT_IDX_2   2

//IRQ enable setting
#define IRQ_ENABLE                1

//SPAcc pop value
#define POP_VALUE                 1

#define DES_IV_OFFSET       0
#define DES_IV_LENGTH     	0x08
#define DES_KEY1_OFFSET     0x08
#define DES_KEY2_OFFSET     0x10
#define DES_KEY3_OFFSET     0x18

/* Cipher key context number, changed from 2 to 8 by xlzhao, reference ar9301
 * spacc configure, 9201 is 8 also
 * */
#define SPACC_CIPH_CTX_NM         (0x8)
/* Cipher key context size  */
#define SPACC_CIPH_CTX_SZ         (0x80)

/* Iv offset int key context */
#define AES_IV_LENGTH     0x10
#define AES_IV_OFFSET     0x20

#define SPACC_DEC         0
#define SPACC_ENC         1

/* SPACC HW limitation: data size cannot exceeds 512kB */
#define SPACC_MAX_SIZE (0x80000)
#define SPACC_SIZE		0x40000

/*
 * Buffer data descriptor table, 8 bytes aligned
 * addr: buffer address
 * len:   buffer length
 */
struct spacc_ddt {
	uint32_t addr;
	uint32_t len;
	uint32_t addr1;
	uint32_t len1;
	uint32_t reserve[2];
}__attribute__((aligned(8)));

struct spacc_context
{
	/* ddt for spacc */
	struct spacc_ddt src_ddt;
	struct spacc_ddt dst_ddt;

	/* va used to clean/inv cache*/
	uint32_t va_src;
	uint32_t va_dst;

	/* cipher algorithm and mode */
	uint8_t algo;
	uint8_t mode;

	/* padding mode */
	enum padmode padding_mode;

	/* encrypt or decrypt */
	uint8_t enc;

	/* iv position in key context */
	uint32_t iv_len;
	uint32_t iv_offset;

	/* proc register */
	uint32_t proc_len;

	/* key size */
	uint32_t key_sz;
	uint32_t key1_sz;
	uint32_t key2_sz;

	/* If use root key */
	uint32_t use_root_key;

	/* Key context */
	uint32_t key_ctx;

	/* key/iv buffer */
	uint32_t key1[8];
	uint32_t key2[8];
	uint32_t iv[4];
};

struct spacc_all_ddt {
	uint8_t* src_addr;
	uint32_t src_len;
	uint8_t* dst_addr;
	uint32_t dst_len;
}__attribute__((aligned(8)));

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

#define ADDR_ALIGN(addr, align) 	(((addr) + (typeof(addr))(align) - 1) & ~((typeof(addr))(align) - 1))
/*
 * based on ip spec and synthesize config, dst and src address must not straddle 1M byte
 * memory boundary, data lenght must 16 byte align.
 */
#define CIPHER_ALIGN(addr) ADDR_ALIGN(addr, 0x100000)

static struct spacc_context spacc_ctx;
static void __iomem * SPAcc_BaseAddress;
static struct spacc_ddt* p_src;
static struct spacc_ddt* p_dst;
static struct spacc_all_ddt all_ddt;

/* symc mutex */
static crypto_mutex spacc_mutex;

//static osSemaphoreId_t spacc_semaphore;

static inline void spacc_write32(uint32_t reg, unsigned int value)
{
	//*(volatile unsigned int *)reg32 = value;
	writel(value, SPAcc_BaseAddress + reg);
	mb();
}

static inline unsigned int spacc_read32(uint32_t reg)
{
	unsigned int value;
	value = readl(SPAcc_BaseAddress + reg);
	return value;
}
#if 0
static void spacc_lock()
{
	//osSemaphoreAcquire(spacc_semaphore, osWaitForever);
}

static void spacc_unlock()
{
	//osSemaphoreRelease(spacc_semaphore);
}
#endif
static void spacc_cleaninv_cache(uint32_t start, uint32_t len)
{
#if 0
	void *s, *e;

	s = (void *)start;
	e = s + len - 1;
	arm_cl1_d_cleaninvbyva(s, e);
#endif
	//__asm_flush_dcache_range(start, start + len - 1);
}

static void spacc_inv_cache(uint32_t start, uint32_t len)
{
#if 0
	void *s, *e;

	s = (void *)start;
	e = s + len - 1;
	arm_cl1_d_invbyva(s, e);
#endif
	/* FIXME: invalid by range */
	//__asm_invalidate_dcache_all();
}

void spacc_cipher_set_algorithm(uint32_t algo, uint32_t mode)
{
	spacc_ctx.algo = algo;
	spacc_ctx.mode = mode;
}

void spacc_cipher_set_encrypt(uint32_t encrypt)
{
	spacc_ctx.enc = encrypt;
}

static void spacc_cipher_set_input(uint32_t src, uint32_t length)
{
	uint32_t n;

	memset(&spacc_ctx.src_ddt, 0, sizeof(spacc_ctx.src_ddt));
	n = CIPHER_ALIGN(src);
	/* straddle 1M byte memory boundary */
	if (n > src && n < src + length) {
		spacc_ctx.src_ddt.addr = src;
		spacc_ctx.src_ddt.len = n - src;
		spacc_ctx.src_ddt.addr1 = n;
		spacc_ctx.src_ddt.len1 = src + length - n;
	} else {
		spacc_ctx.src_ddt.addr = src;
		spacc_ctx.src_ddt.len = length;
	}

	spacc_ctx.proc_len = length;

	spacc_ctx.va_src = src;
}

static void spacc_cipher_set_output(uint32_t dst, uint32_t length)
{
	uint32_t n;

	memset(&spacc_ctx.dst_ddt, 0, sizeof(spacc_ctx.dst_ddt));
	n = CIPHER_ALIGN(dst);
	/* straddle 1M byte memory boundary */
	if (n > dst && n < dst + length) {
		spacc_ctx.dst_ddt.addr = dst;
		spacc_ctx.dst_ddt.len = n - dst;
		spacc_ctx.dst_ddt.addr1 = n;
		spacc_ctx.dst_ddt.len1 = dst + length - n;
	} else {
		spacc_ctx.dst_ddt.addr = dst;
		spacc_ctx.dst_ddt.len = length;
	}

	spacc_ctx.va_dst = dst;
}

void spacc_cipher_set_all_input(uint8_t *src, uint32_t length)
{
	all_ddt.src_addr = (uint8_t*)src;
	all_ddt.src_len = length;
}

void spacc_cipher_set_all_output(uint8_t *dst, uint32_t length)
{
	all_ddt.dst_addr = (uint8_t*)dst;
	all_ddt.dst_len = length;
}

int spacc_cipher_set_key_ctx(uint32_t ctx)
{
	if(ctx >= SPACC_CIPH_CTX_NM) {
		return -1;
	}

	spacc_ctx.key_ctx = ctx;
	return 0;
}

/* XTS has 2 keys */
static int spacc_cipher_set_key(uint32_t key_sz, uint8_t *key1, uint8_t *key2)
{
	spacc_ctx.key_sz = key_sz;

	if(!key1) {
		spacc_ctx.use_root_key = 1;
		return 0;
	}

	if(key1) {
		spacc_ctx.key1_sz = key_sz;
		memcpy(spacc_ctx.key1, key1, key_sz);
	}

	if(key2) {
		spacc_ctx.key2_sz = key_sz;
		memcpy(spacc_ctx.key2, key2, key_sz);
	}
	return 0;
}

int spacc_cipher_set_keylen(uint32_t key_sz)
{
	spacc_ctx.key_sz = key_sz;
	return 0;
}

int spacc_cipher_set_key_single(uint8_t *key, uint32_t odd)
{
	if(0 == odd) //key1
	{
		if(!key) {
			spacc_ctx.use_root_key = 1;
			return 0;
		}
		
		spacc_ctx.key1_sz = spacc_ctx.key_sz;
		memcpy(spacc_ctx.key1, key, spacc_ctx.key1_sz);
	}

	if(1 == odd) //key2
	{
		if(key) {
			spacc_ctx.key2_sz = spacc_ctx.key_sz;
			memcpy(spacc_ctx.key2, key, spacc_ctx.key2_sz);
		}
	}

	return 0;
}

int spacc_cipher_set_iv(uint8_t *iv)
{
	if(!iv)
		return 0;

	/* Only support AES */
	if(spacc_ctx.algo != CIPH_ALG_AES && spacc_ctx.algo != CHIP_ALG_DES)
		return -1;

	if(iv) {
		if (spacc_ctx.algo == CHIP_ALG_AES) {
			spacc_ctx.iv_len = AES_IV_LENGTH;
			spacc_ctx.iv_offset = AES_IV_OFFSET;
		} else {
			spacc_ctx.iv_len = DES_IV_LENGTH;
			spacc_ctx.iv_offset = DES_IV_OFFSET;
		}
		memcpy((uint8_t *)&spacc_ctx.iv, iv, spacc_ctx.iv_len);
	}

	return 0;
}

/* spacc store calculated iv in key context */
int spacc_cipher_get_iv(uint8_t *iv)
{
	uint32_t i;

	/* Get iv context */
	for(i = 0; i < spacc_ctx.iv_len / 4; i++) {
		spacc_ctx.iv[i] = spacc_read32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + AES_IV_OFFSET + i * 4);
	}

	if(iv) {
		memcpy(iv, spacc_ctx.iv, spacc_ctx.iv_len);
	}

	return 0;
}

int spacc_cipher_set_padmode(enum padmode padding_mode)
{
	spacc_ctx.padding_mode = padding_mode;

	return 0;
}

static int spacc_cipher_start_one(uint8_t *src, uint8_t *dst, uint32_t len, int chiper)
{
	uint32_t value;
	uint32_t i;

	spacc_cipher_set_input((uint32_t)src, len);
	spacc_cipher_set_output((uint32_t)dst, len);

	/* Set key size register */
	value = spacc_ctx.key_sz;
	if (chiper)
		value = spacc_set_bit(value, KEY_SIZE_CIPHER_BIT);
	else
		value = spacc_clear_bit(value, KEY_SIZE_CIPHER_BIT); 
	value = spacc_set_bits(value, KEY_SIZE_CTX_IDX_SHIFT, spacc_ctx.key_ctx);
	spacc_write32(SPACC_KEY_SZ, value);

	/* Set key context */
	/* |---key1---|---iv(xts tweak)---|---key2---| */
	if(spacc_ctx.key1_sz) {
		if (chiper) {
			if (spacc_ctx.algo == CHIP_ALG_AES) {
				for(i = 0; i < spacc_ctx.key1_sz / 4; i++)
					spacc_write32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + i * 4, spacc_ctx.key1[i]);
			} else {
				for(i = 0; i < spacc_ctx.key1_sz / 4; i++)
					spacc_write32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + DES_KEY1_OFFSET + i * 4, spacc_ctx.key1[i]);
			}
		} else {
			for(i = 0; i < spacc_ctx.key1_sz / 4; i++)
				spacc_write32(SPACC_HASH_CTX_0 + i * 4, spacc_ctx.key1[i]);
		}
	}
	if(spacc_ctx.key2_sz) {
		for(i = 0; i < spacc_ctx.key2_sz / 4; i++)
			spacc_write32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + AES_IV_OFFSET + spacc_ctx.iv_len + i * 4, spacc_ctx.key2[i]);
	}
	/* Set 0 to below registers */
	spacc_write32(SPACC_AAD_OFFSET, 0x0);
	spacc_write32(SPACC_PRE_AAD_LEN, 0x0);
	spacc_write32(SPACC_POST_AAD_LEN, 0x0);

	spacc_write32(SPACC_ICV_LEN, 0x0);
	spacc_write32(SPACC_ICV_OFFSET, 0x0);

	/* Set IV register */
	if (chiper) {
		value = spacc_ctx.iv_offset;
		value = spacc_clear_bit(value, IV_OFFSET_ENABLE_BIT);
		spacc_write32(SPACC_IV_OFFSET, value);
	} else
		spacc_write32(SPACC_IV_OFFSET, 0);

	/* Set SW register */
	if (chiper) {
		if(spacc_ctx.enc)
			spacc_write32(SPACC_SW_CTRL, 0x1);
		else
			spacc_write32(SPACC_SW_CTRL, 0x2);
	} else
		spacc_write32(SPACC_SW_CTRL, 0x5); /* why? */

	spacc_write32(SPACC_AUX_INFO, 0x0);

	memcpy(p_src, &spacc_ctx.src_ddt, sizeof(struct spacc_ddt));
	memcpy(p_dst, &spacc_ctx.dst_ddt, sizeof(struct spacc_ddt));

	__flush_dcache_area(p_src, sizeof(struct spacc_ddt));
	__flush_dcache_area(p_dst, sizeof(struct spacc_ddt));

	/* Set src & dst register */
	//spacc_write32(SPACC_SRC_PTR, virt_to_phys(&spacc_ctx.src_ddt));
	//spacc_write32(SPACC_DST_PTR, virt_to_phys(&spacc_ctx.dst_ddt));
	spacc_write32(SPACC_SRC_PTR, (unsigned int)virt_to_phys(p_src));
	spacc_write32(SPACC_DST_PTR, (unsigned int) virt_to_phys(p_dst));

	/* Set IV */
	if (chiper) {
		if (spacc_ctx.algo == CHIP_ALG_AES) {
			for(i = 0; i < spacc_ctx.iv_len / 4; i++) {
				spacc_write32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + AES_IV_OFFSET + i * 4, spacc_ctx.iv[i]);
			}
		} else {
			for(i = 0; i < spacc_ctx.iv_len / 4; i++) {
				spacc_write32(SPACC_CIPH_CTX_0 + SPACC_CIPH_CTX_SZ * spacc_ctx.key_ctx + DES_IV_OFFSET + i * 4, spacc_ctx.iv[i]);
			}
		}
	}
	/* flush cache of src & dst buffer*/
	spacc_cleaninv_cache(spacc_ctx.va_src, spacc_ctx.proc_len);
	spacc_cleaninv_cache(spacc_ctx.va_dst, spacc_ctx.proc_len);
	/* flush cache of ddt */
	spacc_cleaninv_cache((phys_addr_t)&spacc_ctx, sizeof(spacc_ctx));

	/* Set PROC len register */
	spacc_write32(SPACC_PROC_LEN, spacc_ctx.proc_len);

	/* Set ctrl register */
	value = 0;
	if(spacc_ctx.enc)
		value = spacc_set_bit(value, ENCRYPT_BIT);

	if (chiper) {
		value = spacc_set_bits(value, CIPH_ALG_SHIFT, spacc_ctx.algo);
		value = spacc_set_bits(value, CIPH_MODE_SHIFT, spacc_ctx.mode);
	} else {
		value = spacc_set_bits(value, HASH_ALG_SHIFT, spacc_ctx.algo);
		value = spacc_set_bits(value, HASH_MODE_SHIFT, spacc_ctx.mode);
	}

	if(spacc_ctx.use_root_key) {
		value = spacc_set_bit(value, SEC_KEY_BIT);
	}

	value = spacc_set_bits(value, CTX_IDX_SHIFT, spacc_ctx.key_ctx);
	value = spacc_set_bit(value, MSG_BEGIN_BIT);
	value = spacc_set_bit(value, MSG_END_BIT);

	/* Set key expand bit */
	if (chiper)
		value = spacc_set_bit(value, KEY_EXP_BIT);

	/* start operation */
	spacc_write32(SPACC_CTRL, value);
	return 0;
}

static void spacc_pop(void)
{
	spacc_write32(SPACC_STAT_POP, POP_VALUE);
}

static int spacc_polling_for_finish(void)
{
	uint32_t fifo_stat = 0x0;
	uint32_t value = 0;

	while(fifo_stat == 0x0) {
		fifo_stat = spacc_read32(SPACC_FIFO_STAT);
		fifo_stat = spacc_get_bits(fifo_stat, FIFO_STAT_CNT_SHIFT, FIFO_STAT_CNT_LEN);
	}

	value = spacc_read32(SPACC_STATUS);
	value = spacc_get_bits(value, STATUS_RET_CODE_SHIFT, STATUS_RET_CODE_LEN);

	/* invalid cache of dst buffer */
	spacc_inv_cache(spacc_ctx.va_dst, spacc_ctx.dst_ddt.len);
	return  -value;
}

void spacc_cipher_create(void)
{
	/* Lock until all data done */
	//spacc_lock();

	AR_S32 ret = crypto_mutex_lock(&spacc_mutex);
	if (AR_SUCCESS != ret)
	{
		AR_LOG_ERROR("error, spacc lock failed\n");
		return;
	}

	memset(&spacc_ctx, 0, sizeof(spacc_ctx));
	return;
}

unsigned int spacc_cipher_config(uint8_t algo, uint8_t mode, uint8_t encrypt,
					uint8_t *key1, uint8_t *key2, uint32_t key_len,
					uint8_t ctx,
					uint8_t *iv,
					enum padmode padding_mode)
{
	spacc_cipher_set_algorithm(algo, mode);
	spacc_cipher_set_encrypt(encrypt);
	spacc_cipher_set_key_ctx(ctx);
	spacc_cipher_set_key(key_len, key1, key2);
	spacc_cipher_set_iv(iv);
	spacc_cipher_set_padmode(padding_mode);
	return 0;
}

unsigned int spacc_cipher_start(int chiper)
{
	spacc_cipher_update(all_ddt.src_addr, all_ddt.dst_addr, all_ddt.src_len, chiper);
	return 0;
}

/*
 * encrypt or decrypt based on config
 * src: plaintext if encrypt, otherwise ciphertext
 * dst: ciphertext buffer to be stored if encrypt, otherwise plaintext buffer to be stored
 * len: src length, dst buffer len malloced must >= src + 16 if not AES_PADDING_NONE while encrypt
 * chiper: 1 chiper, 0 hash
 *
 * return error code while error or real data size while success
 */
unsigned int spacc_cipher_update(uint8_t *src, uint8_t *dst, uint32_t len, int chiper)
{
	int ret = 0, length;
	uint32_t size;
	uint8_t *buf = NULL;
	int again = 0, mod = 0;

	length = len;

	if (spacc_ctx.padding_mode == AES_PADDING_NONE && len % 16) {
		printk("data length must 16 bytes align\n");
		return -1;
	} else if (spacc_ctx.padding_mode == AES_PADDING_DEFAULT && spacc_ctx.enc) {
		mod = len % 16;

		buf = (uint8_t *)kmalloc(16, GFP_KERNEL | GFP_DMA);
		if (!buf) {
			printk("out of memory\n");
			return -1;
		}

		if (mod) {
			memcpy(buf, &src[len - mod], mod);
			memset(&buf[mod], 16 - mod, 16 - mod);
		} else
			memset(buf, 16, 16);

		len -= mod;
		again = 1; 	/* cipher the last 16 byte */
	}

do_work:
	while(len) {
		size = len < SPACC_MAX_SIZE ? len : SPACC_MAX_SIZE;

		ret = spacc_cipher_start_one(src, dst, size, chiper);
		if(ret)
			goto out;

		ret = spacc_polling_for_finish();
		if(ret)
			goto out;

		spacc_pop();
		spacc_cipher_get_iv(NULL);

		src += size;
		dst += size;
		len -= size;
	}

	if (again) {
		again = 0;
		len = 16;
		src = buf;
		goto do_work;
	}

	/* decrypt remove padding size */
	if (spacc_ctx.padding_mode == AES_PADDING_DEFAULT && !spacc_ctx.enc) {
		ret = length - dst[-1];
	}

	/* the ciphertext real size */
	if (spacc_ctx.padding_mode == AES_PADDING_DEFAULT && spacc_ctx.enc)
		ret = length + 16 - mod;

	if (spacc_ctx.padding_mode == AES_PADDING_NONE)
		ret = length;
out:
	if(buf){
		kfree(buf);
	}

	return ret;
}

unsigned int spacc_cipher_final(void)
{
	/* Unlock */
	//spacc_unlock();
	crypto_mutex_unlock(&spacc_mutex);
	return 0;
}

static int spacc_initialized = 0;

void spacc_init(void)
{
	if(spacc_initialized)
		return;

	crypto_mutex_init(&spacc_mutex);

	SPAcc_BaseAddress = ioremap_nocache(SPACC_BASE, SPACC_SIZE);

	memset(&spacc_ctx, 0, sizeof(spacc_ctx));

	memset(&all_ddt, 0, sizeof(all_ddt));
	p_src = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);
	p_dst = (struct spacc_ddt *)kmalloc(sizeof(struct spacc_ddt), GFP_KERNEL | GFP_DMA);

	/* Disable interrupt */
	//spacc_write32(SPACC_IRQ_EN, 0x0);
	/* Create semaphore */
	//spacc_semaphore = osSemaphoreNew(1, 1, NULL);
	spacc_initialized = 1;
}

void  spacc_set_interrupt(uint8_t isEnable)
{
	if(isEnable)
	{
		spacc_write32(SPACC_IRQ_EN, 0x01);
	}
	else
	{
		/* Disable interrupt */
		printk("SPAcc_BaseAddress:%02p SPACC_IRQ_EN:%02d\n", SPAcc_BaseAddress, SPACC_IRQ_EN);
		spacc_write32(SPACC_IRQ_EN, 0x00);
	}
}
