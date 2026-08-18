/******************************************************************************

  Copyright (C), 2011-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     :hi_drv_cipher.h
  Version       : Initial Draft
  Author        : Hisilicon hisecurity team
  Created       :
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/

#ifndef __TEE_DRV_CIPHER_KAPI_H__
#define __TEE_DRV_CIPHER_KAPI_H__

#include "hal_unf_cipher.h"
/*
#ifdef HI_PLATFORM_TYPE_TEE
#include "hi_tee_cipher.h"
#include "tee_drv_cipher_compat.h"
#else
#include "hi_unf_cipher.h"
#include "hi_common_cipher.h"
#include "drv_cipher_compat.h"
#endif
*/

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus */

/******************************* API Declaration *****************************/
/** \addtogroup      crypto */
/** @{ */  /** <!--[link]*/

/* base data type */

/*
typedef unsigned char           u8;

#ifdef __HuaweiLite__
typedef char                    s8;
#endif

typedef unsigned short u16;

typedef unsigned int            u32;
typedef int                     s32;

typedef unsigned long long      u64;
typedef long long               s64;

typedef unsigned long           HI_SIZE_T;
*/

/*! \success */
//#undef  HI_SUCCESS
//#define HI_SUCCESS                      (0)

/*! \failure */
//#undef  HI_FAILURE
//#define HI_FAILURE                      (-1)

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/*width of word*/
#define WORD_WIDTH                      (4)
#define WORD_BIT_WIDTH                  (32)
#define U32_MAX_SIZE                    (0xFFFFFFFF)

/*width of double word*/
#define DOUBLE_WORD_WIDTH               (8)

#ifndef SM2_LEN_IN_WROD
#define SM2_LEN_IN_WROD                 (8)
#define SM2_LEN_IN_BYTE                 (32)
#endif

/*! \big coding transform to litte coding*/
#define CPU_TO_BE16(v) (((v)<< 8) | ((v)>>8))
#define CPU_TO_BE32(v) ((((AR_U32)(v))>>24) | ((((AR_U32)(v))>>8)&0xff00) | ((((AR_U32)(v))<<8)&0xff0000) | (((AR_U32)(v))<<24))
#define CPU_TO_BE64(x) ((AR_U64)(                         \
        (((AR_U64)(x) & (AR_U64)0x00000000000000ffULL) << 56) |   \
        (((AR_U64)(x) & (AR_U64)0x000000000000ff00ULL) << 40) |   \
        (((AR_U64)(x) & (AR_U64)0x0000000000ff0000ULL) << 24) |   \
        (((AR_U64)(x) & (AR_U64)0x00000000ff000000ULL) <<  8) |   \
        (((AR_U64)(x) & (AR_U64)0x000000ff00000000ULL) >>  8) |   \
        (((AR_U64)(x) & (AR_U64)0x0000ff0000000000ULL) >> 24) |   \
        (((AR_U64)(x) & (AR_U64)0x00ff000000000000ULL) >> 40) |   \
        (((AR_U64)(x) & (AR_U64)0xff00000000000000ULL) >> 56)))

/*! \defined the base error code */
#define AR_BASE_ERR_BASE                (0x400)
#define AR_BASE_ERR_BASE_SYMC           (AR_BASE_ERR_BASE + 0x100)
#define AR_BASE_ERR_BASE_HASH           (AR_BASE_ERR_BASE + 0x200)
#define AR_BASE_ERR_BASE_RSA            (AR_BASE_ERR_BASE + 0x300)
#define AR_BASE_ERR_BASE_TRNG           (AR_BASE_ERR_BASE + 0x400)
#define AR_BASE_ERR_BASE_SM2            (AR_BASE_ERR_BASE + 0x500)

#define CRYPTO_MAGIC_NUM          (0xc0704d19)

/*! \enumeration module_id*/
typedef enum
{
    CRYPTO_MODULE_ID_SYMC,        /*!<  Symmetric Cipher */
    CRYPTO_MODULE_ID_SYMC_KEY,    /*!<  Symmetric Cipher key */
    CRYPTO_MODULE_ID_HASH,        /*!<  Message Digest */
    CRYPTO_MODULE_ID_IFEP_RSA,    /*!<  Asymmetric developed by IFEP */
    CRYPTO_MODULE_ID_SIC_RSA,     /*!<  Asymmetric developed by SIC */
    CRYPTO_MODULE_ID_TRNG,        /*!<  Random Data Generation */
    CRYPTO_MODULE_ID_SM2,         /*!<  Public Key Cryptographic Algorithm Based on Elliptic Curves */
    CRYPTO_MODULE_ID_SM4,         /*!<  SM4 */
    CRYPTO_MODULE_ID_SMMU,        /*!<  SMMU */
    CRYPTO_MODULE_ID_CNT,         /*!<  Count of module id */
}module_id;

#define CRYPTO_UNUSED(x)    ((x)=(x))

#define HASH_BLOCK_SIZE            (64)

#define CRYPTO_IOC_NA           0U
#define CRYPTO_IOC_W            1U
#define CRYPTO_IOC_R            2U
#define CRYPTO_IOC_RW           3U

#define AR_ID_CIPHER            0x4D

#define CIPHER_NAME              "AR_CIPHER"
#define UMAP_DEVNAME_CIPHER      "ar_cipher"
#define UMAP_MIN_MINOR_CIPHER    50

#define CRYPTO_IOC(dir,type,nr,size) (((dir) << 30)|((size) << 16)|((type) << 8)|((nr) << 0))
#define CRYPTO_IOR(nr,size)    CRYPTO_IOC(CRYPTO_IOC_R, AR_ID_CIPHER,(nr), size)
#define CRYPTO_IOW(nr,size)    CRYPTO_IOC(CRYPTO_IOC_W, AR_ID_CIPHER,(nr), size)
#define CRYPTO_IOWR(nr,size)   CRYPTO_IOC(CRYPTO_IOC_RW, AR_ID_CIPHER,(nr),size)

#define CRYPTO_IOC_DIR(cmd)       (((cmd) >> 30) & 0x03)
#define CRYPTO_IOC_TYPE(cmd)      (((cmd) >> 8) & 0xFF)
#define CRYPTO_IOC_NR(cmd)        (((cmd) >> 0) & 0xFF)
#define CRYPTO_IOC_SIZE(cmd)      (((cmd) >> 16) & 0x3FFF)

#define CRYPTO_CMD_SYMC_CREATEHANDLE      CRYPTO_IOR (0x00, sizeof(symc_create_t))
#define CRYPTO_CMD_SYMC_DESTROYHANDLE     CRYPTO_IOW (0x01, sizeof(symc_destroy_t))
#define CRYPTO_CMD_SYMC_CONFIGHANDLE      CRYPTO_IOW (0x02, sizeof(symc_config_t))
#define CRYPTO_CMD_SYMC_ENCRYPT           CRYPTO_IOW (0x03, sizeof(symc_encrypt_t))
#define CRYPTO_CMD_SYMC_ENCRYPTMULTI      CRYPTO_IOW (0x04, sizeof(symc_encrypt_multi_t))
#define CRYPTO_CMD_SYMC_GETTAG            CRYPTO_IOWR(0x05, sizeof(aead_tag_t))
#define CRYPTO_CMD_HASH_START             CRYPTO_IOWR(0x06, sizeof(hash_start_t))
#define CRYPTO_CMD_HASH_UPDATE            CRYPTO_IOW (0x07, sizeof(hash_update_t))
#define CRYPTO_CMD_HASH_FINISH            CRYPTO_IOWR(0x08, sizeof(hash_finish_t))
#define CRYPTO_CMD_RSA_ENC                CRYPTO_IOWR(0x09, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_DEC                CRYPTO_IOWR(0x0a, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_SIGN               CRYPTO_IOWR(0x0b, sizeof(rsa_info_t))
#define CRYPTO_CMD_RSA_VERIFY             CRYPTO_IOWR(0x0c, sizeof(rsa_info_t))
#define CRYPTO_CMD_TRNG                   CRYPTO_IOWR(0x0d, sizeof(trng_t))
#define CRYPTO_CMD_SYMC_GET_CONFIG        CRYPTO_IOWR(0x0e, sizeof(symc_get_config_t))
#define CRYPTO_CMD_KLAD_KEY               CRYPTO_IOWR(0x0f, sizeof(klad_key_t))
#define CRYPTO_CMD_COUNT                  0x10

AR_S32 TEE_CIPHER_Ioctl(AR_U32 cmd, AR_VOID *argp);

/*! \assert */

#define CRYPTO_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            AR_LOG_ERROR("assertion '%s' failed\n", #expr); \
            AR_LOG_ERROR("at %s:%d (func '%s')\n", __FILE__, __LINE__, __func__); \
            return AR_ERR_CIPHER_INVALID_PARA; \
        } \
    } while (0)

#define CHECK_EXIT(expr) \
    do { \
        if (AR_SUCCESS != (ret = expr)) { \
            AR_LOG_PrintFuncErr(expr, ret); \
            goto exit__; \
        } \
    } while (0)

#if 0
#define AR_PRINT_HEX(name, str, len) \
{\
    AR_U32 _i = 0;\
    AR_U8 *_str; \
    _str = (AR_U8*)str; \
    AR_PRINT("[%s]:\n", name);\
    for ( _i = 0 ; _i < (len); _i++ )\
    {\
        if( (_i % 16 == 0) && (_i != 0)) AR_PRINT("\n");\
        AR_PRINT("\\x%02x", *((_str)+_i));\
    }\
    AR_PRINT("\n");\
}
#else
//#define AR_PRINT_HEX print_string
#endif

/*! \AES KEY size */
#define SYMC_KEY_SIZE       (32)

/*! \SM1 SK size */
#define SYMC_SM1_SK_SIZE    (16)

/*! \AES IV size */
#define AES_IV_SIZE         (16)

/*! \AES BLOCK size */
#define AES_BLOCK_SIZE      (16)

/*! \DES IV size */
#define DES_IV_SIZE         (8)

/*! \aead tag length */
#define AEAD_TAG_SIZE                  (16)
#define AEAD_TAG_SIZE_IN_WORD          (4)

/*! \bits in a byte */
#define BITS_IN_BYTE                   (8)

/*! \hash result max size */
#define HASH_RESULT_MAX_SIZE           (64)

#ifndef CHIP_TYPE_hi3516ev200
/*! \hash result max size in word */
#define HASH_RESULT_MAX_SIZE_IN_WORD   (16)
#else
/*! \hash result max size in word */
#define HASH_RESULT_MAX_SIZE_IN_WORD   (8)
#endif

/*! capacity upport */
#define CRYPTO_CAPACITY_SUPPORT        (1)
#define CRYPTO_CAPACITY_NONSUPPORT     (0)

/* max length of SM2 ID */
#define SM2_ID_MAX_LEN          0x1FFF

/*! Define the time out */
#define CRYPTO_TIME_OUT         6000
#define MS_TO_US                1000

/* result size */
#define SHA1_RESULT_SIZE           (20)  /* SHA1 */
#define SHA224_RESULT_SIZE         (28)  /* SHA224 */
#define SHA256_RESULT_SIZE         (32)  /* SHA256 */
#define SHA384_RESULT_SIZE         (48)  /* SHA384 */
#define SHA512_RESULT_SIZE         (64)  /* SHA512 */
#define SM3_RESULT_SIZE            (32)  /* SM3 */

/* rsa key length */
#define RSA_MIN_KEY_LEN     (128)
#define RSA_MAX_KEY_LEN     (512)
#define RSA_KEY_BITWIDTH_1024     (128)
#define RSA_KEY_BITWIDTH_2048     (256)
#define RSA_KEY_BITWIDTH_3072     (384)
#define RSA_KEY_BITWIDTH_4096     (512)
#define RSA_KEY_EXPONENT_VALUE1   (0X3)
#define RSA_KEY_EXPONENT_VALUE2   (0X10001)

/*! \the source of hash message */
typedef enum
{
    HASH_CHUNCK_SRC_LOCAL, /*!<  Local buffer, e.g. Kernel  */
    HASH_CHUNCK_SRC_USER,  /*!<  User buffer, use copy_from_user to read data */
}hash_chunk_src;

/*! \union of compat addr*/
typedef union
{
    void *p;                /*!<  virtual address */
    const void *cp;         /*!<  const virtual address */
    unsigned long long phy; /*!<  physical address */
    unsigned int word[2];   /*!<  double word of address */
}compat_addr;

extern compat_addr compat_addr_zero;
#define ADDR_H32(addr)          addr.word[1]  /*!<  High 32 bit of u64 */
#define ADDR_L32(addr)          addr.word[0]  /*!<  Low 32 bit of u64 */
#define ADDR_U64(addr)          addr.phy      /*!<  64 bit of u64 */
#define ADDR_VIA(addr)          addr.p        /*!<  buffer point */
#define ADDR_VIA_CONST(addr)    addr.cp       /*!<  const buffer point */
#define ADDR_NULL        compat_addr_zero  /*!<  buffer point */

#define ADDR_P_H32(addr) addr->word[1]  /*!<  High 32 bit of u64 */
#define ADDR_P_L32(addr) addr->word[0]  /*!<  Low 32 bit of u64 */
#define ADDR_P_U64(addr) addr->phy      /*!<  64 bit of u64 */
#define ADDR_P_VIA(addr) addr->p        /*!<  buffer point */

/*! \struct of Symmetric cipher create */
typedef struct
{
    AR_U32 id;              /*!< to store the id of soft channel */
    AR_U32 reserve;         /*!<  reserve to make align at 64bit */
}symc_create_t;

/*! \struct of Symmetric cipher destroy */
typedef struct
{
    AR_U32 id;             /*!< id of soft channel */
    AR_U32 reserve;        /*!<  reserve to make align at 64bit */
}symc_destroy_t;

/*! \struct of Symmetric cipher configure infomation */
typedef struct
{
    AR_U32 id;                               /*!<  Id of soft channel */
    AR_U32 hard_key;                         /*!<  Use hard key or not */
    AR_UNF_CIPHER_ALG_E alg;         /*!<  Symmetric cipher algorithm */
    AR_UNF_CIPHER_WORK_MODE_E mode;  /*!<  Symmetric cipher algorithm */
    AR_UNF_CIPHER_BIT_WIDTH_E width; /*!<  Symmetric cipher bit width */
    AR_UNF_CIPHER_KEY_LENGTH_E klen; /*!<  Symmetric cipher key length */
    AR_UNF_CIPHER_SM1_ROUND_E sm1_round_num; /*!<  The round number of sm1 */
    AR_U8 fkey[SYMC_KEY_SIZE];               /*!< first  key buffer, defualt */
    AR_U8 skey[SYMC_KEY_SIZE];               /*!< second key buffer */
    AR_U8 iv[AES_IV_SIZE];                   /*!<  IV buffer */
    AR_U32 ivlen;                            /*!<  IV length */
    AR_U32 iv_usage;                         /*!<  Usage of IV */
    AR_U32 reserve;        /*!<  reserve to make align at 64bit */
    compat_addr aad;    /*!<  Associated Data */
    AR_U32  alen;          /*!<  Associated Data Length */
    AR_U32  tlen;          /*!<  Tag length */
}symc_config_t;

typedef enum
{
    SYMC_OPERATION_ENCRYPT = 0,
    SYMC_OPERATION_DECRYPT = 1,
    SYMC_OPERATION_ENCRYPT_VIA = 0x10,
    SYMC_OPERATION_DECRYPT_VIA = 0x11,
}symc_operation_type;

/*! \struct of Symmetric cipher encrypt/decrypt */
typedef struct
{
    AR_U32 id;              /*!<  Id of soft channel */
    AR_U32 length;          /*!<  Length of the encrypted data */
    AR_U32 operation;       /*!<  operation type*/
    AR_U32 last;            /*!<  last or not */
    compat_addr input;   /*!<  Physical address of the input data */
    compat_addr output;  /*!<  Physical address of the output data */
}symc_encrypt_t;

/*! \struct of Symmetric cipher multiple encrypt/decrypt */
typedef struct
{
    AR_U32 id;             /*!<  Id of soft channel */
    compat_addr pkg;    /*!<  Buffer of package infomation */
    AR_U32 pkg_num;        /*!<  Number of package infomation */
    AR_U32 operation;      /*!<  Decrypt or encrypt */
}symc_encrypt_multi_t;

/*! \struct of Symmetric cipher get tag */
typedef struct
{
    AR_U32 id;                            /*!<  Id of soft channel */
    AR_U32 tag[AEAD_TAG_SIZE_IN_WORD];    /*!<  Buffer of tag */
    AR_U32 taglen ;                       /*!<  Length of tag */
}aead_tag_t;

/*! \struct of Symmetric cipher get ctrl */
typedef struct
{
    AR_U32 id;                            /*!<  Id of soft channel */
    AR_UNF_CIPHER_CTRL_S  ctrl;          /*!<  control infomation */
}symc_get_config_t;

/*! \struct of Hash start */
typedef struct
{
    AR_U32 id;                               /*!<  Id of soft channel */
    AR_UNF_CIPHER_HASH_TYPE_E  type;        /*!<  HASH type */
    compat_addr key;                      /*!<  HMAC key */
    AR_U32 keylen;                           /*!<  HMAC key */
    AR_U32 reserve;                          /*!<  reserve for align at 64bit */
}hash_start_t;

/*! \struct of Hash update */
typedef struct
{
    AR_U32 id;             /*!<  Id of soft channel */
    AR_U32 length;         /*!<  Length of the message */
    compat_addr input;  /*!<  Message data buffer */
    hash_chunk_src src; /*!<  source of hash message */
    AR_U32 reserve;        /*!<  reserve for align at 64bit */
}hash_update_t;

/*! \struct of Hash update */
typedef struct
{
    AR_U32 id;             /*!<  Id of soft channel */
    AR_U32 hash[HASH_RESULT_MAX_SIZE_IN_WORD]; /*!<  buffer holding the hash data */
    AR_U32 hashlen;        /*!<  length of the hash data */
    AR_U32 reserve;        /*!<  reserve for align at 64bit */
}hash_finish_t;

/*! \struct of rsa encrypt/decrypt */
typedef struct
{
    AR_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme; /*!<  RSA encryption scheme */
    AR_U16 public;             /** Type of key, true-public or false-private */
    AR_U16 catype;             /** ca Type of key */
    AR_U32 klen;               /*!<  length of rsa key */
    AR_U32 e;                  /*!<  The public exponent */
    compat_addr d;          /*!<  The private exponent */
    compat_addr N;          /*!<  The modulus */
    compat_addr p;          /*!<  The p factor of N */
    compat_addr q;          /*!<  The q factor of N */
    compat_addr qP;         /*!<  The 1/q mod p CRT param */
    compat_addr dP;         /*!<  The d mod (p - 1) CRT param */
    compat_addr dQ;         /*!<  The d mod (q - 1) CRT param */
    compat_addr in;         /*!<  input data to be encryption */
    compat_addr out;        /*!<  output data of encryption */
    AR_U32 inlen;              /*!<  length of input data to be encryption */
    AR_U32 outlen;             /*!<  length of output data */
}rsa_info_t;

/** RSA PKCS style key */
typedef struct
{
    /** Type of key, true-public or false-private */
    AR_U8 public;
    /** The key source */
    AR_U8 catype;
    /** The key length */
    AR_U16 klen;
    /** The public exponent */
    AR_U32 e;
    /** The private exponent */
    AR_U8 *d;
    /** The modulus */
    AR_U8 *N;
    /** The p factor of N */
    AR_U8 *p;
    /** The q factor of N */
    AR_U8 *q;
    /** The 1/q mod p CRT param */
    AR_U8 *qP;
    /** The d mod (p - 1) CRT param */
    AR_U8 *dP;
    /** The d mod (q - 1) CRT param */
    AR_U8 *dQ;
} cryp_rsa_key;

/*! \struct of klad key */
typedef struct
{
    AR_U32 keysel;
    AR_U32 target;
    AR_U32 clear[AES_BLOCK_SIZE / WORD_WIDTH];
    AR_U32 encrypt[AES_BLOCK_SIZE / WORD_WIDTH];
}klad_key_t;

/*! \struct of trng */
typedef struct
{
    AR_U32 randnum;     /*!<  randnum rand number  */
    AR_U32 timeout;     /*!<  time out  */
}trng_t;

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      crypto*/
/** @{*/  /** <!-- [link]*/

/**
\brief   Kapi Init.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_init(void);

/**
\brief   Kapi Deinit.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_deinit(void);

/**
\brief   Kapi release.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_release(void);

/**
\brief   Create symc handle.
\param[in]  id The channel number.
\param[in]  uuid The user identification.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_create(AR_U32 *id);

/**
\brief   Destroy symc handle.
\param[in]  id The channel number.
\param[in]  uuid The user identification.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_destroy(AR_U32 id);

/**
\brief  set work params.
* \param[in]  id The channel number.
* \param[in]  hard_key whether use the hard key or not.
* \param[in]  alg The symmetric cipher algorithm.
* \param[in]  mode The symmetric cipher mode.
* \param[in]  sm1_round_num The round number of sm1.
* \param[in]  fkey first  key buffer, defualt
* \param[in]  skey second key buffer, expand
* \param[in]  klen The key length.
* \param[in]  aad      Associated Data
* \param[in]  alen     Associated Data Length
* \param[in]  tlen     Tag length
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_config(AR_U32 id,
                    AR_U32 hard_key,
                    AR_UNF_CIPHER_ALG_E enAlg,
                    AR_UNF_CIPHER_WORK_MODE_E enWorkMode,
                    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth,
                    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen,
                    AR_UNF_CIPHER_SM1_ROUND_E sm1_round_num,
                    AR_U8 *fkey, AR_U8 *skey,
                    AR_U8 *iv, AR_U32 ivlen, AR_U32 iv_usage,
                    compat_addr aad, AR_U32 alen, AR_U32 tlen);

/**
\brief  get work params.
* \param[in]  id The channel number.
* \param[out] ctrl infomation.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_symc_get_config(AR_U32 id, AR_UNF_CIPHER_CTRL_S *ctrl);

/**
 * \brief          SYMC  buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param length   length of the input data
 * \param operation  decrypt or encrypt
 *
 * \return         0 if successful
 */
AR_S32 kapi_symc_crypto(AR_U32 id, compat_addr input,
                      compat_addr output, AR_U32 length,
                      AR_U32 operation, AR_U32 last);

/**
 * \brief          SYMC  via buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param length   length of the input data
 * \param operation  decrypt or encrypt
 *
 * \return         0 if successful
 */
AR_S32 kapi_symc_crypto_via(AR_U32 id, compat_addr input,
                     compat_addr output, AR_U32 length,
                     AR_U32 operation, AR_U32 last, AR_U32 is_from_user);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param[in]  id The channel number.
 * \param pkg       Buffer of package infomation
 * \param pkg_num   Number of package infomation
 * \param operation  decrypt or encrypt
 * \param last       last or not
 *
 * \return         0 if successful
 */
AR_S32 kapi_symc_crypto_multi(AR_U32 id, void *pkg,
                            AR_U32 pkg_num, AR_U32 operation, AR_U32 last);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 * \param[in]  id The channel number.
 * \param[in]  tag tag data of CCM/GCM
 *
 * \return         0 if successful
 */
AR_S32 kapi_aead_get_tag(AR_U32 id, AR_U32 tag[AEAD_TAG_SIZE_IN_WORD], AR_U32 *taglen);

/**
\brief   Kapi Init.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_hash_init(void);

/**
\brief   Kapi Deinit.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_hash_deinit(void);

/**
 * \brief          HASH context setup.
 *
 *
 * \param[out] id The channel number.
 * \param[in] type    Hash type
 * \param[in] key     hmac key
 * \param[in] keylen  hmac key length
 *
 * \return         0 if successful
 */
AR_S32 kapi_hash_start(AR_U32 *id, AR_UNF_CIPHER_HASH_TYPE_E type,
                    AR_U8 *key, AR_U32 keylen);

/**
 * \brief          HASH process buffer.
 *
 * \param[in]  id The channel number.
 * \param[in] input    buffer holding the input data
 * \param[in] length   length of the input data
 * \param[in] src      source of hash message
 *
 * \return         0 if successful
 */
AR_S32 kapi_hash_update(AR_U32 id, AR_U8 *input, AR_U32 length,
                     hash_chunk_src src);

/**
 * \brief          HASH final digest.
 *
 * \param[in]  id The channel number.
 * \param[out] hash    buffer holding the hash data
 * \param[out] hashlen length of the hash data
 *
 * \return         0 if successful
 */
AR_S32 kapi_hash_finish(AR_U32 id, AR_U8 *hash, AR_U32 *hashlen);

/**
\brief   hash release.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_hash_release(void);

/**
\brief   Kapi Init.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_rsa_init(void);

/**
\brief   Kapi Deinitialize.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_rsa_deinit(void);

/**
* \brief RSA encryption a plaintext with a RSA private key.
*
* \param[in] key:       rsa key struct.
* \param[in] enScheme:  rsa encrypt/decrypt scheme.
* \param[in] in:      input data to be encryption
* \param[out] inlen:  length of input data to be encryption
* \param[out] out:    output data of encryption
* \param[out] outlen: length of output data to be encryption
* \retval ::AR_SUCCESS  Call this API successful
* \retval ::AR_FAILURE  Call this API fails.
*/
AR_S32 kapi_rsa_encrypt(cryp_rsa_key *key,
                     AR_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme,
                     AR_U8 *in, AR_U32 inlen,
                     AR_U8 *out, AR_U32 *outlen);

/**
* \brief RSA decryption a ciphertext with a RSA public key.
*
* \param[in] key:       rsa key struct.
* \param[in] enScheme:  rsa encrypt/decrypt scheme.
* \param[in] in:     input data to be encryption
* \param[in] inlen:  length of input data to be encryption
* \param[out] out:   output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \retval ::AR_SUCCESS  Call this API successful
* \retval ::AR_FAILURE  Call this API fails.
*/
AR_S32 kapi_rsa_decrypt(cryp_rsa_key *key,
                    AR_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme,
                    AR_U8 *in, AR_U32 inlen,
                    AR_U8 *out, AR_U32 *outlen);

/**
* \brief RSA signature a context with appendix, where a signer's RSA private key is used.
*
* \param[in] key:       rsa key struct.
* \param[in] enScheme:  rsa signature/verify scheme.
* \param[in] in:     input data to be encryption
* \param[in] inlen:  length of input data to be encryption
* \param[in] hash:   hash value of context,if NULL, let hash = Hash(context) automatically
* \param[out] out:   output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \param[in]  src      source of hash message
* \param[in]  uuid uuid The user identification.
* \retval ::AR_SUCCESS  Call this API successful
* \retval ::AR_FAILURE  Call this API fails.
*/
AR_S32 kapi_rsa_sign_hash(cryp_rsa_key *key,
                       AR_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme,
                       AR_U8* hash, AR_U32 hlen,
                       AR_U8 *sign, AR_U32 *signlen);

/**
* \brief RSA verify a ciphertext with a RSA public key.
*
* \param[in]  key_info:   encryption struct.
* \param[in]  in:    input data to be encryption
* \param[out] inlen:  length of input data to be encryption
* \param[in]  hash:   hash value of context,if NULL, let hash = Hash(context) automatically
* \param[out] out:    output data to be encryption
* \param[out] outlen: length of output data to be encryption
* \param[in]  src      source of hash message
* \retval ::AR_SUCCESS  Call this API successful
* \retval ::AR_FAILURE  Call this API fails.
*/
AR_S32 kapi_rsa_verify_hash(cryp_rsa_key *key,
                         AR_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme,
                        AR_U8 *hash, AR_U32 hlen,
                        AR_U8 *sign, AR_U32 signlen);

/**
\brief get rand number.
\param[out]  randnum rand number.
\param[in]   timeout time out.
\retval     On success, AR_SUCCESS is returned.  On error, AR_FAILURE is returned.
*/
AR_S32 kapi_trng_get_random( AR_U32 *randnum, AR_U32 timeout);

/* cipher kapi_test */
void kapi_test(void);

/** @}*/  /** <!-- ==== API Code end ====*/

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif    /* End of #ifndef __AR_DRV_CIPHER_H__*/
