/*
 * PKA - DesignWare PKA driver support for Public Key Cryptography
 *
 * Copyright 2022 Artosyn Microelectronics
 *
 */

#include <crypto/akcipher.h>
#include <crypto/internal/akcipher.h>
#include <crypto/internal/rsa.h>
#include <crypto/scatterwalk.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/rtnetlink.h>
#include <linux/scatterlist.h>
#include <linux/sched.h>
#include <linux/sizes.h>
#include <linux/slab.h>

//Register address
#define PKA_CTRL_OFFSET                                 0x00
#define PKA_ENTRY_PNT_OFFSET                            0x04
#define PKA_RTN_CODE_OFFSET                             0x08
#define PKA_BUILD_CONFIG_OFFSET                         0x0c
#define PKA_STACK_PNTR_OFFSET                           0x10
#define PKA_INSTR_SINCE_GO_OFFSET                       0x14
#define PKA_CONFIG_OFFSET                               0x1c
#define PKA_STAT_OFFSET                                 0x20
#define PKA_FLAGS_OFFSET                                0x24
#define PKA_WATCHDOG_OFFSET                             0x28
#define PKA_CYCLES_SINCE_GO_OFFSET                      0x2c
#define PKA_INDEX_I_OFFSET                              0x30
#define PKA_INDEX_J_OFFSET                              0x34
#define PKA_INDEX_K_OFFSET                              0x38
#define PKA_INDEX_L_OFFSET                              0x3c
#define PKA_IRQ_EN_OFFSET                               0x40
#define PKA_JMP_PROB_OFFSET                             0x44
#define PKA_JMP_PROB_LFSR_OFFSET                        0x48
#define PKA_BANK_SW_A_OFFSET                            0x50
#define PKA_BANK_SW_B_OFFSET                            0x54
#define PKA_BANK_SW_C_OFFSET                            0x58
#define PKA_BANK_SW_D_OFFSET                            0x5c

#define PKA_REGION_A_OFFSET                             0x400
#define PKA_REGION_B_OFFSET                             0x800
#define PKA_REGION_C_OFFSET                             0xc00
#define PKA_REGION_D_OFFSET                             0x1000

////////////////////////////////////////////////////////////////////////////////////

//Register bit define
//CTRL
#define CTRL_PARTIAL_RADIX_LEN                8
#define CTRL_PARTIAL_RADIX_SHIFT              0
#define CTRL_BASE_RADIX_LEN                   3
#define CTRL_BASE_RADIX_SHIFT                 8
#define CTRL_BASE_512_MODE_LEN                5
#define CTRL_BASE_512_MODE_SHIFT              16
#define CTRL_STOP_RQST_BIT                    27
#define CTRL_GO_BIT                           31

//RTN_CODE
#define RTN_CODE_STOP_REASON_LEN              8
#define RTN_CODE_STOP_REASON_SHIFT            16
#define RTN_CODE_DPA_EN_BIT                   27
#define RTN_CODE_ZERO_BIT                     28
#define RTN_CODE_IRO_BIT                      30
#define RTN_CODE_BUSY_BIT                     31

//BUILD_CONFIG
#define BUILD_CONFIG_BANK_SW_A_LEN            2
#define BUILD_CONFIG_BANK_SW_A_SHIFT          0
#define BUILD_CONFIG_BANK_SW_B_LEN            2
#define BUILD_CONFIG_BANK_SW_B_SHIFT          2
#define BUILD_CONFIG_BANK_SW_C_LEN            2
#define BUILD_CONFIG_BANK_SW_C_SHIFT          4
#define BUILD_CONFIG_BANK_SW_D_LEN            2
#define BUILD_CONFIG_BANK_SW_D_SHIFT          6
#define BUILD_CONFIG_FW_RAM_SIZE_LEN          3
#define BUILD_CONFIG_FW_RAM_SIZE_SHIFT        8
#define BUILD_CONFIG_FW_ROM_SIZE_LEN          3
#define BUILD_CONFIG_FW_ROM_SIZE_SHIFT        11
#define BUILD_CONFIG_ECC_MAX_LEN              2
#define BUILD_CONFIG_ECC_MAX_SHIFT            14
#define BUILD_CONFIG_RSA_MAX_LEN              3
#define BUILD_CONFIG_RSA_MAX_SHIFT            16
#define BUILD_CONFIG_ALU_WIDTH_LEN            2
#define BUILD_CONFIG_ALU_WIDTH_SHIFT          19
#define BUILD_CONFIG_TA_DPA_SUPPORT_LEN       2
#define BUILD_CONFIG_TA_DPA_SUPPORT_SHIFT     21
#define BUILD_CONFIG_FORMAT_TYPE_LEN          2
#define BUILD_CONFIG_FORMAT_TYPE_SHIFT        30

//CONFIG
#define CONFIG_ALT_ACCESS_BIT                 0
#define CONFIG_ENDIAN_SWAP_BIT                26

//STAT
#define STAT_DONE_BIT                         30

//FlAGS
#define FLAGS_Z_BIT                           0
#define FLAGS_M_BIT                           1
#define FLAGS_B_BIT                           2
#define FLAGS_C_BIT                           3
#define FLAGS_F0_BIT                          4
#define FLAGS_F1_BIT                          5
#define FLAGS_F2_BIT                          6
#define FLAGS_F3_BIT                          7

//IRQ EN
#define IRQ_EN_IE_BIT                         30

/////////////////////////////////////////////////////////////////////
//RSA RADIX
enum {
    RSA_256 = 2,
    RSA_512 = 3,
    RSA_1024 = 4,
    RSA_2048 = 5,
    RSA_4096 = 6,
};

//STOP_REASON
enum {
	STOP_NORMAL                 = 0,
	STOP_INV_OPCODE             = 1,
	STOP_STACK_UNDER_FLOW       = 2,
	STOP_STACK_OVER_FLOW        = 3,
	STOP_WATCH_DOG              = 4,
	STOP_MEMORTY_PORT_COLLISION = 8,
};

//CTRL bit settings
#define CTRL_BASE_RADIX_256                   2 // 256-bit
#define CTRL_BASE_RADIX_512                   3 // 512-bit
#define CTRL_BASE_RADIX_1024                  4 // 1024-bit
#define CTRL_BASE_RADIX_2048                  5 // 2048-bit
#define CTRL_BASE_RADIX_4096                  6 // 4096-bit

//Entry define
#define ENTRY_BIT_SERIAL_MOD                  0x18
#define ENTRY_BIT_SERIAL_MOD_DP               0x17
#define ENTRY_CALC_MP                         0x10
#define ENTRY_CALC_R_INV                      0x11
#define ENTRY_CALC_R_SQR                      0x12
#define ENTRY_CRT                             0x16
#define ENTRY_CRT_KEY_SETUP                   0x15
#define ENTRY_IS_A_M3                         0x22
#define ENTRY_IS_P_EQUAL_Q                    0x20
#define ENTRY_IS_P_REFLECT_Q                  0x21
#define ENTRY_MODADD                          0xb
#define ENTRY_MODDIV                          0xd
#define ENTRY_MODEXP                          0x14
#define ENTRY_MODINV                          0xe
#define ENTRY_MODMULT                         0xa
#define ENTRY_MODSUB                          0xc
#define ENTRY_MULT                            0x13
#define ENTRY_PADD                            0x1c
#define ENTRY_PADD_STD_PRJ                    0x1d
#define ENTRY_PDBL                            0x1a
#define ENTRY_PDBL_STD_PRJ                    0x1b
#define ENTRY_PMULT                           0x19
#define ENTRY_PVER                            0x1e
#define ENTRY_REDUCE                          0xf
#define ENTRY_SHAMIR                          0x23
#define ENTRY_STD_PRJ_TO_AFFINE               0x1f

//BUILDC_CONFIG bit settings
#define BANK_SW_1                             0 // 1 bank
#define BANK_SW_2                             1 // 2 banks
#define BANK_SW_4                             2 // 4 banks

#define FW_SIZE_NO                            0 // No F/W Rom/Ram
#define FW_SIZE_256                           1 // 256 words
#define FW_SIZE_512                           2 // 512 words
#define FW_SIZE_1024                          3 // 1024 words
#define FW_SIZE_2048                          4 // 2048 words
#define FW_SIZE_4096                          5 // 4096 words

#define ECC_MAX_NOT_SUPPORT                   0 // ECC ops not supported
#define ECC_MAX_256                           1 // 256bits
#define ECC_MAX_512                           2 // 512bits
#define ECC_MAX_1024                          3 // 1024bits

#define RSA_MAX_NOT_SUPPORT                   0 // RSA ops not supported
#define RSA_MAX_512                           1 // 512bits
#define RSA_MAX_1024                          2 // 1024 bits
#define RSA_MAX_2048                          3 // 2048 bits
#define RSA_MAX_4986                          4 // 4096 bits

#define RSA_ALU_WIDTH_32                      0 // 32bits
#define RSA_ALU_WIDTH_64                      1 // 64 bits
#define RSA_ALU_WIDTH_128                     2 // 128bits
#define RSA_ALU_WIDTH_REERVEDS                 3 // reserved

#define RSA_TA_DPA_SUPPORT_NO_SUPPORT         0 // No Support
#define RSA_TA_DPA_SUPPORT_TA                 1 // TA support
#define RSA_TA_DPA_SUPPORT_DPATA              2 // DPA/TA support
#define RSA_TA_DPA_SUPPORT_RESERVED           3 //Reserved

#define FORMAT_TYPE_DEFAULT                   0x2

//////////////////////////////////////////////////////////////////////////
#define PKA_GET_BIT(value, bit)             ((value >> bit) & 0x1)
#define PKA_SET_BIT(value, bit)             (value | (1 << bit))
#define PKA_CLEAR_BIT(value, bit)           (value & (~(1 << bit)))
#define PKA_GET_BITS(value, shift, len) \
		((value >> shift) & ((1 << len) - 1))
#define PKA_SET_BITS(value, shift, value2) \
		(value | (value2 << shift))

/////////////////////////////////////////////////////////////////////////

struct dw_pka_engine {
	void __iomem		*regs;
	spinlock_t			hw_lock;
	const char			*name;
	struct device		*dev;
    u32                 rsa_max_size;
};

struct pka_rsa_key {
    u8                  *n;
    u8                  *e; 
    u8                  *d;
    u32                 n_len;
    u32                 e_len;
    u32                 d_len;
};

struct dw_pka {
	struct dw_pka_engine    *engine;
    struct akcipher_alg     alg;
};

struct pka_rsa_ctx {
	struct dw_pka_engine    *engine;
    struct pka_rsa_key      key;
};

struct pka_job
{
	u8 *n, *e;
	u8 *src, *dst;
	u32 n_len;
	u32 e_len;
	u32 src_len;
	u32 dst_len;
	u32 radix;
	u32 key_size;
};

static void swap_bytes(void *pv, size_t n)
{
    char *p = pv;
    size_t lo, hi;

    for (lo=0, hi=n-1; hi>lo; lo++, hi--) {
        char tmp=p[lo];
        p[lo] = p[hi];
        p[hi] = tmp;
    }
}

static u32 pka_radix(u32 n_len)
{
    if (n_len > 2048/8)
        return RSA_4096;
    else if (n_len > 1024/8)
        return RSA_2048;
    else if (n_len > 512/8)
        return RSA_1024;
    else
        return RSA_512;
}
#if 0
static int pka_hardware_init(struct dw_pka_engine *engine)
{
	unsigned value = 0;

	//Enable interrupt
	value = PKA_SET_BIT(value, IRQ_EN_IE_BIT);
	writel(value, engine->regs + PKA_IRQ_EN_OFFSET);

	//Set build config
	//value = 0x80049801;//Don't modify it
	//writel(value, engine->regs + PKA_BUILD_CONFIG);
    return 0;
}
#endif
static int pka_polling_for_finish(struct dw_pka_engine *engine)
{
	unsigned int value;

	value = readl(engine->regs + PKA_RTN_CODE_OFFSET);

	while((PKA_GET_BIT(value, RTN_CODE_IRO_BIT)) == 0) {
	    value = readl(engine->regs + PKA_RTN_CODE_OFFSET);
        //printk("PKA_RTN_CODE = %x, PKA_INSTR_SINCE_GO = %x\n", value, readl(engine->regs + PKA_INSTR_SINCE_GO_OFFSET));
	}
	
	value = PKA_GET_BITS(value, RTN_CODE_STOP_REASON_SHIFT, RTN_CODE_STOP_REASON_LEN);
	writel(1 << STAT_DONE_BIT, engine->regs + PKA_STAT_OFFSET);

	return value;
}

static void pka_write_data(struct dw_pka_engine *engine, unsigned long dst, u32 *src, unsigned long len)
{
	int i;
    u32 residue = 0;
    int residue_len = len % 4;
    
	for (i = 0; i < len / 4; i++) {
		writel(*src, engine->regs + dst);
        dst += 4;
        src++;
    }
    if (residue_len == 1) {
        residue = *(u8 *)src;
    }
    else if (residue_len == 2) {
        residue = *(u8 *)src;
        residue |= *((u8 *)src + 1);
    }
    else if (residue_len == 3) {
        residue = *(u8 *)src;
        residue |= *((u8 *)src + 1) << 8;
        residue |= *((u8 *)src + 2) << 16;
    }
    if (residue != 0) {
        writel(residue, engine->regs + dst);
    }
}

static void pka_read_data(struct dw_pka_engine *engine, u32 *dst, unsigned long src, unsigned long len)
{
	int i;
	for(i = 0; i < (len+3) / 4; i++) {
		*dst = readl(engine->regs + src);
        src += 4;
        dst++;
    }
}

//Calc r^-1, and result is store in register C0
static int pka_calc_r_inv(struct dw_pka_engine *engine, struct pka_job *job)
{
	unsigned int value = 0;

	//Clear D0
	memset_io(engine->regs + PKA_REGION_D_OFFSET, 0, job->key_size);
	//Move modulus to region D
	pka_write_data(engine, PKA_REGION_D_OFFSET, (u32 *)job->n, job->n_len);

	//printk("pka caculate r_inv:\n");

	//Write stack pointer register
	writel(0, engine->regs + PKA_STACK_PNTR_OFFSET);
	//Write flag register
	writel(0, engine->regs + PKA_FLAGS_OFFSET);
	//Write entry register, enter calc_r_inv
	writel(ENTRY_CALC_R_INV, engine->regs + PKA_ENTRY_PNT_OFFSET);

	//Set RSA radix bits, and start operation
	value = PKA_SET_BIT(value, CTRL_GO_BIT);
	value = PKA_SET_BITS(value, CTRL_BASE_RADIX_SHIFT, job->radix);
	writel(value, engine->regs + PKA_CTRL_OFFSET);

	// wait for finish
	return pka_polling_for_finish(engine);
}

//Calc m', and result is store in register D1
static int pka_calc_mp(struct dw_pka_engine *engine, struct pka_job *job)
{
	unsigned int value = 0;

	//printf("pka caculate mp:\n");

	writel(0, engine->regs + PKA_FLAGS_OFFSET);
	writel(ENTRY_CALC_MP, engine->regs + PKA_ENTRY_PNT_OFFSET);  // entry of calc_mp

	//Set RSA radix bits, and start operation
	value = PKA_SET_BIT(value, CTRL_GO_BIT);
	value = PKA_SET_BITS(value, CTRL_BASE_RADIX_SHIFT, job->radix);
	writel(value, engine->regs + PKA_CTRL_OFFSET);

	// wait for finish
	return pka_polling_for_finish(engine);
}

//Calc R^2, and result is store in register D3
static int pka_calc_r_square(struct dw_pka_engine *engine, struct pka_job *job)
{
	unsigned int value = 0;

	//printf("pka caculate r_squar:\n");

	writel(0x00000000, engine->regs + PKA_FLAGS_OFFSET);
	writel(ENTRY_CALC_R_SQR, engine->regs + PKA_ENTRY_PNT_OFFSET);  // entry of calc_r_sqr
	writel(0x40000000, engine->regs + PKA_IRQ_EN_OFFSET);

	//Set RSA radix bits, and start operation
	value = PKA_SET_BIT(value, CTRL_GO_BIT);
	value = PKA_SET_BITS(value, CTRL_BASE_RADIX_SHIFT, job->radix);
	writel(value, engine->regs + PKA_CTRL_OFFSET);

	// wait for finish
	return pka_polling_for_finish(engine);
}

/*
 *n: modulus
 *e: exponent
 *radix: rsa radix, for example, radix=2048, RSA2048 operation
 */
static int pka_rsa_expmod(struct dw_pka_engine *engine,
            u8 *src, u32 src_len,
			u8 *dst, u32 dst_len,
			u8 *n, u32 n_len,
			u8 *e, u32 e_len,
			unsigned long radix)
{
	struct pka_job job;
	u32 value = 0;
	int ret;
	int keysize;// key size in byte

	switch(radix) {
	case RSA_256:
		keysize = 256/8;
		break;
	case RSA_512:
		keysize = 512/8;
		break;
	case RSA_1024:
		keysize = 1024/8;
		break;
	case RSA_2048:
		keysize = 2048/8;
		break;
	case RSA_4096:
		keysize = 4096/8;
		break;
	default:
        return -1;
	}; 
    
	memset(&job, 0, sizeof(struct pka_job));
	job.n = n;
	job.e = e;
	job.src = src;
	job.dst = dst;
	job.n_len = n_len;
	job.e_len = e_len;
	job.src_len = src_len;
	job.dst_len = dst_len;
	job.radix = radix;
    job.key_size = keysize;

	writel(0x40000, engine->regs + PKA_WATCHDOG_OFFSET);

	ret = pka_calc_r_inv(engine, &job);
	if(ret)
		return ret;

	ret = pka_calc_mp(engine, &job);
	if(ret)
		return ret;

	ret = pka_calc_r_square(engine, &job);
	if(ret)
		return ret;

	//Clear D2
	memset_io(engine->regs + (PKA_REGION_D_OFFSET + 2 * keysize), 0, keysize);
	//Move public exponent to D2
	pka_write_data(engine, PKA_REGION_D_OFFSET + 2 * keysize, (u32 *)job.e, job.e_len);

    //Clear A0
	memset_io(engine->regs + PKA_REGION_A_OFFSET, 0, keysize);

    swap_bytes(src, src_len);

	//Move data before decrypt to A0
	pka_write_data(engine, PKA_REGION_A_OFFSET, (u32 *)job.src, job.src_len);

	//do RSA encryption
	writel(0, engine->regs + PKA_FLAGS_OFFSET);
	writel(ENTRY_MODEXP, engine->regs + PKA_ENTRY_PNT_OFFSET);
	writel(0, engine->regs + PKA_JMP_PROB_OFFSET);

	//Set RSA radix bits, and start operation
	value = 0;
	value = PKA_SET_BIT(value, CTRL_GO_BIT);
	value = PKA_SET_BITS(value, CTRL_BASE_RADIX_SHIFT, job.radix);
	writel(value, engine->regs + PKA_CTRL_OFFSET);

	// wait for finish
	ret = pka_polling_for_finish(engine);
	if(ret)
		return ret;

	//Move data after encrypt from A0 to dst buffer
	pka_read_data(engine, (u32 *)dst, PKA_REGION_A_OFFSET, dst_len);

    swap_bytes(dst, dst_len);

	return 0;
}

static inline struct dw_pka *to_dw_pka(struct akcipher_alg *alg)
{
	return container_of(alg, struct dw_pka, alg);
}

static void pka_rsa_free_key(struct pka_rsa_key *job)
{
	kzfree(job->d);
	kfree(job->e);
	kfree(job->n);
	job->d = NULL;
	job->e = NULL;
	job->n = NULL;
	job->d_len = 0;
	job->e_len = 0;
	job->n_len = 0;
}

/**
 * pka_read_raw_data - Read a raw byte stream as a positive integer.
 * The function skips buffer's leading zeros, copies the remained data
 * to a buffer allocated in the GFP_DMA | GFP_KERNEL zone and returns
 * the address of the new buffer.
 *
 * @buf   : The data to read
 * @nbytes: The amount of data to read
 */
static inline u8 *pka_read_raw_data(const u8 *buf, size_t *nbytes)
{
	u8 *val;

	while (!*buf && *nbytes) {
		buf++;
		(*nbytes)--;
	}

	val = kzalloc(*nbytes, GFP_KERNEL);
	if (!val)
		return NULL;

	memcpy(val, buf, *nbytes);

	return val;
}

static int pka_rsa_check_key_length(struct dw_pka_engine *engine, unsigned int len)
{
	if (len > engine->rsa_max_size) {
		return -EINVAL;
    }
	return 0;
}

static int pka_rsa_enc(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *key = &ctx->key;
	struct dw_pka_engine *engine = ctx->engine;
	int ret;
    u8 *src = NULL;
    u8 *dst = NULL;

	if (unlikely(!key->n || !key->e))
		return -EINVAL;

	if (req->dst_len < key->n_len) {
		req->dst_len = key->n_len;
		dev_err(engine->dev, "Output buffer length less than parameter n\n");
		return -EOVERFLOW;
	}

	src = kzalloc(req->src_len, GFP_KERNEL);
    if (!src) {
        return -ENOMEM;
    }
	dst = kzalloc(req->dst_len+3, GFP_KERNEL);
    if (!dst) {
        goto err;
    }

    scatterwalk_map_and_copy(src, req->src, 0, req->src_len, 0);

    ret = pka_rsa_expmod(engine, src, req->src_len, dst, req->dst_len,
            key->n, key->n_len, key->e, key->e_len, pka_radix(key->n_len));
    if (ret != 0) {
		dev_err(engine->dev, "PKA stop with error %d\n", ret);
		return -EINVAL;
    }

    scatterwalk_map_and_copy(dst, req->dst, 0, req->dst_len, 1);

    kfree(src);
    kfree(dst);

	return ret;
err:
    if (src)
        kfree(src);
    if (dst)
        kfree(dst);
    return -ENOMEM;
}

static int pka_rsa_dec(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *key = &ctx->key;
	struct dw_pka_engine *engine = ctx->engine;
	int ret;
    u8 *src = NULL;
    u8 *dst = NULL;

	if (unlikely(!key->n || !key->d))
		return -EINVAL;

	if (req->dst_len < key->n_len) {
		req->dst_len = key->n_len;
		dev_err(engine->dev, "Output buffer length less than parameter n\n");
		return -EOVERFLOW;
	}

	src = kzalloc(req->src_len, GFP_KERNEL);
    if (!src) {
        return -ENOMEM;
    }
	dst = kzalloc(req->dst_len+3, GFP_KERNEL);
    if (!dst) {
        goto err;
    }

    scatterwalk_map_and_copy(src, req->src, 0, req->src_len, 0);

    ret = pka_rsa_expmod(engine, src, req->src_len, dst, req->dst_len,
            key->n, key->n_len, key->d, key->d_len, pka_radix(key->n_len));
    if (ret != 0) {
		dev_err(engine->dev, "PKA stop with error %d\n", ret);
		return -EINVAL;
    }

    scatterwalk_map_and_copy(dst, req->dst, 0, req->dst_len, 1);

    kfree(src);
    kfree(dst);

	return ret;
err:
    if (src)
        kfree(src);
    if (dst)
        kfree(dst);
    return -ENOMEM;
}

static int pka_rsa_set_pub_key(struct crypto_akcipher *tfm, const void *key,
				unsigned int keylen)
{
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *rsa_key = &ctx->key;
	struct rsa_key raw_key;
	int ret;

	/* Free the old RSA key if any */
	pka_rsa_free_key(rsa_key);

    memset(&raw_key, 0, sizeof(struct rsa_key));
	ret = rsa_parse_pub_key(&raw_key, key, keylen);
	if (ret)
		return ret;

	/* Copy key in DMA zone */
	rsa_key->e = kzalloc(raw_key.e_sz, GFP_KERNEL);
	if (!rsa_key->e)
		goto err;

	/*
	 * Skip leading zeros and copy the positive integer to a buffer
	 * allocated in the GFP_DMA | GFP_KERNEL zone. The decryption descriptor
	 * expects a positive integer for the RSA modulus and uses its length as
	 * decryption output length.
	 */
	rsa_key->n = pka_read_raw_data(raw_key.n, &raw_key.n_sz);
	if (!rsa_key->n)
		goto err;

	if (pka_rsa_check_key_length(ctx->engine, raw_key.n_sz << 3)) {
		pka_rsa_free_key(rsa_key);
		return -EINVAL;
	}

	rsa_key->e_len = raw_key.e_sz;
	rsa_key->n_len = raw_key.n_sz;

	memcpy(rsa_key->e, raw_key.e, raw_key.e_sz);

    swap_bytes(rsa_key->n, rsa_key->n_len);
    swap_bytes(rsa_key->e, rsa_key->e_len);

	return 0;
err:
	pka_rsa_free_key(rsa_key);
	return -ENOMEM;
}

static int pka_rsa_set_priv_key(struct crypto_akcipher *tfm, const void *key,
				 unsigned int keylen)
{
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *rsa_key = &ctx->key;
	struct rsa_key raw_key;
	int ret;

	/* Free the old RSA key if any */
	pka_rsa_free_key(rsa_key);

    memset(&raw_key, 0, sizeof(struct rsa_key));
	ret = rsa_parse_priv_key(&raw_key, key, keylen);
	if (ret)
		return ret;

	/* Copy key in DMA zone */
	rsa_key->d = kzalloc(raw_key.d_sz, GFP_KERNEL);
	if (!rsa_key->d)
		goto err;

	rsa_key->e = kzalloc(raw_key.e_sz, GFP_KERNEL);
	if (!rsa_key->e)
		goto err;

	/*
	 * Skip leading zeros and copy the positive integer to a buffer
	 * allocated in the GFP_DMA | GFP_KERNEL zone. The decryption descriptor
	 * expects a positive integer for the RSA modulus and uses its length as
	 * decryption output length.
	 */
	rsa_key->n = pka_read_raw_data(raw_key.n, &raw_key.n_sz);
	if (!rsa_key->n)
		goto err;

	if (pka_rsa_check_key_length(ctx->engine, raw_key.n_sz << 3)) {
		pka_rsa_free_key(rsa_key);
		return -EINVAL;
	}

	rsa_key->d_len = raw_key.d_sz;
	rsa_key->e_len = raw_key.e_sz;
	rsa_key->n_len = raw_key.n_sz;

	memcpy(rsa_key->d, raw_key.d, raw_key.d_sz);
	memcpy(rsa_key->e, raw_key.e, raw_key.e_sz);
    
    /*
     * PKA hardware use little endian data
     */
    swap_bytes(rsa_key->n, rsa_key->n_len);
    swap_bytes(rsa_key->e, rsa_key->e_len);
    swap_bytes(rsa_key->d, rsa_key->d_len);

	return 0;

err:
	pka_rsa_free_key(rsa_key);
	return -ENOMEM;
}

static int pka_rsa_max_size(struct crypto_akcipher *tfm)
{
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *key = &ctx->key;

	return (key->n) ? key->n_len : -EINVAL;
}

static int pka_rsa_init_tfm(struct crypto_akcipher *tfm)
{
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);
    struct dw_pka *pka = to_dw_pka(alg);
	struct dw_pka_engine *engine = pka->engine;

	ctx->engine = engine;

	return 0;
}

static void pka_rsa_exit_tfm(struct crypto_akcipher *tfm)
{
	struct pka_rsa_ctx *ctx = akcipher_tfm_ctx(tfm);
	struct pka_rsa_key *key = &ctx->key;

	pka_rsa_free_key(key);
}

static struct dw_pka dw_pka_rsa = {
    .alg = {
        .encrypt = pka_rsa_enc,
        .decrypt = pka_rsa_dec,
        .sign = pka_rsa_dec,
        .verify = pka_rsa_enc,
        .set_pub_key = pka_rsa_set_pub_key,
        .set_priv_key = pka_rsa_set_priv_key,
        .max_size = pka_rsa_max_size,
        .init = pka_rsa_init_tfm,
        .exit = pka_rsa_exit_tfm,
        .base = {
            .cra_name = "rsa",
            .cra_driver_name = "rsa-dw",
            .cra_priority = 3000,
            .cra_module = THIS_MODULE,
            .cra_ctxsize = sizeof(struct pka_rsa_ctx),
        },
    },
};

#ifdef CONFIG_OF
static const struct of_device_id dw_pka_of_id_table[] = {
	{ .compatible = "snps,dw-pka" },
	{}
};
MODULE_DEVICE_TABLE(of, dw_pka_of_id_table);
#endif /* CONFIG_OF */

static bool dw_pka_is_compatible(struct platform_device *pdev,
				const char *dw_pka_type)
{
	const struct platform_device_id *platid = platform_get_device_id(pdev);

	if (platid && !strcmp(platid->name, dw_pka_type))
		return true;

#ifdef CONFIG_OF
	if (of_device_is_compatible(pdev->dev.of_node, dw_pka_type))
		return true;
#endif /* CONFIG_OF */

	return false;
}

static int dw_pka_probe(struct platform_device *pdev)
{
	int err;
    u32 build_config;
    u32 rsa_max_size = 0;
	struct resource *mem;
	struct dw_pka_engine *engine = devm_kzalloc(&pdev->dev, sizeof(*engine),
						   GFP_KERNEL);
	if (!engine)
		return -ENOMEM;

	if (!dw_pka_is_compatible(pdev, "snps,dw-pka"))
		return -EINVAL;

	engine->name = dev_name(&pdev->dev);

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	engine->regs = devm_ioremap_resource(&pdev->dev, mem);
	if (IS_ERR(engine->regs))
		return PTR_ERR(engine->regs);

	build_config = readl(engine->regs + PKA_BUILD_CONFIG_OFFSET);
    rsa_max_size = 512 * (1 << (PKA_GET_BITS(build_config, 
            BUILD_CONFIG_RSA_MAX_SHIFT, BUILD_CONFIG_RSA_MAX_LEN) - 1));
    if (rsa_max_size == 0) {
        return -ENODEV;
    }

    engine->rsa_max_size = rsa_max_size;
	engine->dev	= &pdev->dev;
	spin_lock_init(&engine->hw_lock);
	platform_set_drvdata(pdev, engine);
    dw_pka_rsa.engine = engine;
	err = crypto_register_akcipher(&dw_pka_rsa.alg);
	if (err) {
		dev_warn(engine->dev, "%s alg registration failed\n", 
                dw_pka_rsa.alg.base.cra_driver_name);
        return err;
    }

    //pka_hardware_init(engine);

	return 0;
}

static int dw_pka_remove(struct platform_device *pdev)
{
	crypto_unregister_akcipher(&dw_pka_rsa.alg);
	return 0;
}

static const struct platform_device_id dw_pka_id_table[] = {
	{ "snps,dw-pka", },
	{ }
};

#ifdef CONFIG_PM
static int dw_pka_suspend(struct device *dev)
{
#if 0
	struct platform_device *pdev = to_platform_device(dev);
	struct spacc_engine *engine = platform_get_drvdata(pdev);

	/*
	 * We only support standby mode. All we have to do is gate the clock to
	 * the spacc. The hardware will preserve state until we turn it back
	 * on again.
	 */
	clk_disable(engine->clk);
#endif
	return 0;
}

static int dw_pka_resume(struct device *dev)
{
#if 0
	struct platform_device *pdev = to_platform_device(dev);
	struct spacc_engine *engine = platform_get_drvdata(pdev);

	return clk_enable(engine->clk);
#endif
    return 0;
}

static const struct dev_pm_ops dw_pka_pm_ops = {
	.suspend	= dw_pka_suspend,
	.resume		= dw_pka_resume,
};
#endif /* CONFIG_PM */

static struct platform_driver dw_pka_driver = {
	.probe		= dw_pka_probe,
	.remove		= dw_pka_remove,
	.driver		= {
		.name	= "snps,dw-pka",
#ifdef CONFIG_PM
		.pm	= &dw_pka_pm_ops,
#endif /* CONFIG_PM */
		.of_match_table	= of_match_ptr(dw_pka_of_id_table),
	},
	.id_table	= dw_pka_id_table,
};

module_platform_driver(dw_pka_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DesignWare PKA driver for Asymmetric functions of crypto API");
MODULE_AUTHOR("xin.chen@artosyn.cn");
