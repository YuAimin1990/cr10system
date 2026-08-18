/**
* \file
* \brief Describes the API about the cipher.
*/

#ifndef __AR_HAL_UNF_CIPHER_H__
#define __AR_HAL_UNF_CIPHER_H__

#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*************************** Structure Definition ****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */

/** max length of SM2, unit: word */
#define AR_SM2_LEN_IN_WROD                   (8)

/** max length of SM2, unit: byte */
#define AR_SM2_LEN_IN_BYTE                   (SM2_LEN_IN_WROD * 4)

/** CIPHER set IV for first package */
#define AR_CIPHER_IV_CHANGE_ONE_PKG        (1)

/** CIPHER set IV for first package */
#define AR_CIPHER_IV_CHANGE_ALL_PKG        (2)

//typedef unsigned long           AR_SIZE_T;
/** Cipher work mode */
typedef enum arAR_UNF_CIPHER_WORK_MODE_E
{
    AR_UNF_CIPHER_WORK_MODE_ECB,        /**<Electronic codebook (ECB) mode*/
    AR_UNF_CIPHER_WORK_MODE_CBC,        /**<Cipher block chaining (CBC) mode*/
    AR_UNF_CIPHER_WORK_MODE_CFB,        /**<Cipher feedback (CFB) mode*/
    AR_UNF_CIPHER_WORK_MODE_OFB,        /**<Output feedback (OFB) mode*/
    AR_UNF_CIPHER_WORK_MODE_CTR,        /**<Counter (CTR) mode*/
    AR_UNF_CIPHER_WORK_MODE_CCM,        /**<Counter (CCM) mode*/
    AR_UNF_CIPHER_WORK_MODE_GCM,        /**<Counter (GCM) mode*/
    AR_UNF_CIPHER_WORK_MODE_CBC_CTS,    /**<Cipher block chaining CipherStealing mode*/
    AR_UNF_CIPHER_WORK_MODE_BUTT,
    AR_UNF_CIPHER_WORK_MODE_INVALID  = 0xffffffff,
}AR_UNF_CIPHER_WORK_MODE_E;

/** Cipher algorithm */
typedef enum arAR_UNF_CIPHER_ALG_E
{
    AR_UNF_CIPHER_ALG_DES           = 0x0,  /**< Data encryption standard (DES) algorithm */
    AR_UNF_CIPHER_ALG_3DES          = 0x1,  /**< 3DES algorithm */
    AR_UNF_CIPHER_ALG_AES           = 0x2,  /**< Advanced encryption standard (AES) algorithm */
    AR_UNF_CIPHER_ALG_SM1           = 0x3,  /**<SM1 algorithm*/
    AR_UNF_CIPHER_ALG_SM4           = 0x4,  /**<SM4 algorithm*/
    AR_UNF_CIPHER_ALG_DMA           = 0x5,  /**<DMA copy*/
    AR_UNF_CIPHER_ALG_BUTT          = 0x6,
    AR_UNF_CIPHER_ALG_INVALID       = 0xffffffff,
}AR_UNF_CIPHER_ALG_E;

/** Key length */
typedef enum arAR_UNF_CIPHER_KEY_LENGTH_E
{
    AR_UNF_CIPHER_KEY_AES_128BIT    = 0x0,  /**< 128-bit key for the AES algorithm */
    AR_UNF_CIPHER_KEY_AES_192BIT    = 0x1,  /**< 192-bit key for the AES algorithm */
    AR_UNF_CIPHER_KEY_AES_256BIT    = 0x2,  /**< 256-bit key for the AES algorithm */
    AR_UNF_CIPHER_KEY_DES_3KEY      = 0x2,  /**< Three keys for the DES algorithm */
    AR_UNF_CIPHER_KEY_DES_2KEY      = 0x3,  /**< Two keys for the DES algorithm */
    AR_UNF_CIPHER_KEY_DEFAULT       = 0x0,  /**< default key length, DES-8, SM1-48, SM4-16 */
    AR_UNF_CIPHER_KEY_INVALID       = 0xffffffff,
}AR_UNF_CIPHER_KEY_LENGTH_E;

/** Cipher bit width */
typedef enum arAR_UNF_CIPHER_BIT_WIDTH_E
{
    AR_UNF_CIPHER_BIT_WIDTH_64BIT   = 0x0,  /**< 64-bit width */
    AR_UNF_CIPHER_BIT_WIDTH_8BIT    = 0x1,  /**< 8-bit width */
    AR_UNF_CIPHER_BIT_WIDTH_1BIT    = 0x2,  /**< 1-bit width */
    AR_UNF_CIPHER_BIT_WIDTH_128BIT  = 0x3,  /**< 128-bit width */
    AR_UNF_CIPHER_BIT_WIDTH_INVALID = 0xffffffff,
}AR_UNF_CIPHER_BIT_WIDTH_E;

/** Cipher control parameters */
typedef struct TEE_CIPHER_CTRL_CHANGE_FLAG_S
{
    AR_U32   bit1IV:2;              /**< Initial Vector change flag, 0-don't set, 1-set IV for first package, 2-set IV for each package  */
    AR_U32   bitsResv:30;           /**< Reserved */
}AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S;

/** Key ladder selecting parameters */
typedef enum TEE_CIPHER_CA_TYPE_E
{
    AR_UNF_CIPHER_KEY_SRC_USER       = 0x0,  /**< User Key*/
    AR_UNF_CIPHER_KEY_SRC_KLAD_1,           /**< KLAD Key 1*/
    AR_UNF_CIPHER_KEY_SRC_KLAD_2,           /**< KLAD Key 2*/
    AR_UNF_CIPHER_KEY_SRC_KLAD_3,           /**< KLAD Key 3*/
    AR_UNF_CIPHER_KEY_SRC_BUTT,
    AR_UNF_CIPHER_KEY_SRC_INVALID = 0xffffffff,
}AR_UNF_CIPHER_CA_TYPE_E;

/** Encryption/Decryption type selecting */
typedef enum
{
    AR_UNF_CIPHER_KLAD_TARGET_AES       = 0x0,  /**< Klad for AES*/
    AR_UNF_CIPHER_KLAD_TARGET_RSA,              /**< Klad for RSA*/
    AR_UNF_CIPHER_KLAD_TARGET_BUTT,
}AR_UNF_CIPHER_KLAD_TARGET_E;

/** Encryption/Decryption type selecting */
typedef enum
{
    AR_UNF_CIPHER_TYPE_NORMAL  = 0x0,
    AR_UNF_CIPHER_TYPE_COPY_AVOID,
    AR_UNF_CIPHER_TYPE_BUTT,
    AR_UNF_CIPHER_TYPE_INVALID = 0xffffffff,
}AR_UNF_CIPHER_TYPE_E;

/** Structure of the cipher type */
typedef struct
{
    AR_UNF_CIPHER_TYPE_E enCipherType;
}AR_UNF_CIPHER_ATTS_S;

/** sm1 round config */
typedef enum arAR_UNF_CIPHER_SM1_ROUND_E
{
    AR_UNF_CIPHER_SM1_ROUND_08 = 0x00,            /**< sm1 round 08 */
    AR_UNF_CIPHER_SM1_ROUND_10 = 0x01,            /**< sm1 round 10 */
    AR_UNF_CIPHER_SM1_ROUND_12 = 0x02,            /**< sm1 round 12 */
    AR_UNF_CIPHER_SM1_ROUND_14 = 0x03,            /**< sm1 round 14 */
    AR_UNF_CIPHER_SM1_ROUND_BUTT,
    AR_UNF_CIPHER_SM1_ROUND_INVALID  = 0xffffffff,
}AR_UNF_CIPHER_SM1_ROUND_E;

/** Structure of the cipher control information */
typedef struct arAR_UNF_CIPHER_CTRL_S
{
    AR_U32 u32Key[8];                               /**< Key input */
    AR_U32 u32IV[4];                                /**< Initialization vector (IV) */
    AR_BOOL bKeyByCA;                               /**< Encryption using advanced conditional access (CA) or decryption using keys */
    AR_UNF_CIPHER_CA_TYPE_E enCaType;               /**< Select keyladder type when using advanced CA */
    AR_UNF_CIPHER_ALG_E enAlg;                      /**< Cipher algorithm */
    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    AR_UNF_CIPHER_WORK_MODE_E enWorkMode;           /**< Operating mode */
    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all would be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_S;

/** Structure of the cipher AES control information */
typedef struct arAR_UNF_CIPHER_CTRL_AES_S
{
    AR_U32 u32EvenKey[8];                           /**< Key input, default use this key*/
    AR_U32 u32OddKey[8];                            /**< Key input, only valid for Multi encrypt/decrypt*/
    AR_U32 u32IV[4];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_AES_S;

/** Structure of the cipher AES CCM/GCM control information */
typedef struct arAR_UNF_CIPHER_CTRL_AES_CCM_GCM_S
{
    AR_U32 u32Key[8];                               /**< Key input */
    AR_U32 u32IV[4];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */
    AR_U32 u32IVLen;                                /**< IV lenght for CCM/GCM, which is an element of {4,6,8,10,12,14,16} for CCM, and is an element of [1-16] for GCM*/
    AR_U32 u32TagLen;                               /**< Tag lenght for CCM which is an element of {4,6,8,10,12,14,16}*/
    AR_U32 u32ALen;                                 /**< Associated data for CCM and GCM*/
    AR_UINTPTR szAPhyAddr;                             /**< Physical address of Associated data for CCM and GCM*/
} AR_UNF_CIPHER_CTRL_AES_CCM_GCM_S;

/** Structure of the cipher DES control information */
typedef struct arAR_UNF_CIPHER_CTRL_DES_S
{
    AR_U32 u32Key[2];                               /**< Key input */
    AR_U32 u32IV[2];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_DES_S;

/** Structure of the cipher 3DES control information */
typedef struct arAR_UNF_CIPHER_CTRL_3DES_S
{
    AR_U32 u32Key[6];                               /**< Key input */
    AR_U32 u32IV[2];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen;            /**< Key length */
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_3DES_S;

/** Structure of the cipher SM1 control information */
typedef struct arAR_UNF_CIPHER_CTRL_SM1_S
{
    AR_U32 u32EK[4];                               /**< Key of EK input */
    AR_U32 u32AK[4];                               /**< Key of AK input */
    AR_U32 u32SK[4];                               /**< Key of SK input */
    AR_U32 u32IV[4];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth;           /**< Bit width for encryption or decryption */
    AR_UNF_CIPHER_SM1_ROUND_E enSm1Round;           /**< SM1 round number, should be 8, 10, 12 or 14*/
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_SM1_S;

/** Structure of the cipher SM4 control information */
typedef struct arAR_UNF_CIPHER_CTRL_SM4_S
{
    AR_U32 u32Key[4];                               /**< Key  input */
    AR_U32 u32IV[4];                                /**< Initialization vector (IV) */
    AR_UNF_CIPHER_CTRL_CHANGE_FLAG_S stChangeFlags; /**< control information exchange choices, we default all woulde be change except they have been in the choices */
} AR_UNF_CIPHER_CTRL_SM4_S;

/** Expand Structure of the cipher control information */
typedef struct arAR_UNF_CIPHER_CTRL_EX_S
{
    AR_UNF_CIPHER_ALG_E enAlg;                      /**< Cipher algorithm */
    AR_UNF_CIPHER_WORK_MODE_E enWorkMode;           /**< Operating mode */
    AR_BOOL bKeyByCA;                               /**< Encryption using advanced conditional access (CA) or decryption using keys */
    /**< Parameter for special algorithm
        for AES, the pointer should point to AR_UNF_CIPHER_CTRL_AES_S;
        for AES_CCM or AES_GCM, the pointer should point to AR_UNF_CIPHER_CTRL_AES_CCM_GCM_S;
        for DES, the pointer should point to AR_UNF_CIPHER_CTRL_DES_S;
        for 3DES, the pointer should point to AR_UNF_CIPHER_CTRL_3DES_S;
        for SM1, the pointer should point to AR_UNF_CIPHER_CTRL_SM1_S;
        for SM4, the pointer should point to AR_UNF_CIPHER_CTRL_SM4_S;
    */
    AR_VOID *pParam;
} AR_UNF_CIPHER_CTRL_EX_S;

/** Cipher data */
typedef struct arAR_UNF_CIPHER_DATA_S
{
    AR_UINTPTR szSrcPhyAddr;     /**< phy address of the original data */
    AR_UINTPTR szDestPhyAddr;    /**< phy address of the purpose data */
    AR_U32 u32ByteLength;     /**< cigher data length*/
    AR_BOOL bOddKey;          /**< Use odd key or even key*/
} AR_UNF_CIPHER_DATA_S;

/** Hash algrithm type */
typedef enum arAR_UNF_CIPHER_HASH_TYPE_E
{
    AR_UNF_CIPHER_HASH_TYPE_SHA1,
    AR_UNF_CIPHER_HASH_TYPE_SHA224,
    AR_UNF_CIPHER_HASH_TYPE_SHA256,
    AR_UNF_CIPHER_HASH_TYPE_SHA384,
    AR_UNF_CIPHER_HASH_TYPE_SHA512,
    AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA1,
    AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA224,
    AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA256,
    AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA384,
    AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA512,
    AR_UNF_CIPHER_HASH_TYPE_SM3,
    AR_UNF_CIPHER_HASH_TYPE_BUTT,
    AR_UNF_CIPHER_HASH_TYPE_INVALID  = 0xffffffff,
}AR_UNF_CIPHER_HASH_TYPE_E;

/** Hash init struct input */
typedef struct
{
    AR_U8 *pu8HMACKey;
    AR_U32 u32HMACKeyLen;
    AR_UNF_CIPHER_HASH_TYPE_E eShaType;
}AR_UNF_CIPHER_HASH_ATTS_S;

typedef enum arAR_UNF_CIPHER_RSA_ENC_SCHEME_E
{
    AR_UNF_CIPHER_RSA_ENC_SCHEME_NO_PADDING,            /**< without padding */
    AR_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_0,          /**< PKCS#1 block type 0 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_1,          /**< PKCS#1 block type 1 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_BLOCK_TYPE_2,          /**< PKCS#1 block type 2 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA1,       /**< PKCS#1 RSAES-OAEP-SHA1 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA224,     /**< PKCS#1 RSAES-OAEP-SHA224 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA256,     /**< PKCS#1 RSAES-OAEP-SHA256 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA384,     /**< PKCS#1 RSAES-OAEP-SHA384 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_OAEP_SHA512,     /**< PKCS#1 RSAES-OAEP-SHA512 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_RSAES_PKCS1_V1_5,      /**< PKCS#1 RSAES-PKCS1_V1_5 padding*/
    AR_UNF_CIPHER_RSA_ENC_SCHEME_BUTT,
    AR_UNF_CIPHER_RSA_ENC_SCHEME_INVALID  = 0xffffffff,
}AR_UNF_CIPHER_RSA_ENC_SCHEME_E;

typedef enum arAR_UNF_CIPHER_RSA_SIGN_SCHEME_E
{
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA1 = 0x100, /**< PKCS#1 RSASSA_PKCS1_V15_SHA1 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA224,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA224 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA256,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA256 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA384,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA384 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_V15_SHA512,       /**< PKCS#1 RSASSA_PKCS1_V15_SHA512 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA1,         /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA224,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA224 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA1 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512,       /**< PKCS#1 RSASSA_PKCS1_PSS_SHA256 signature*/
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_BUTT,
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_INVALID  = 0xffffffff,
}AR_UNF_CIPHER_RSA_SIGN_SCHEME_E;

typedef struct
{
    AR_U8  *pu8N;              /**< point to public modulus  */
    AR_U8  *pu8E;              /**< point to public exponent */
    AR_U16 u16NLen;            /**< length of public modulus, max value is 512Byte*/
    AR_U16 u16ELen;            /**< length of public exponent, max value is 512Byte*/
}AR_UNF_CIPHER_RSA_PUB_KEY_S;

/** RSA private key struct */
typedef struct
{
    AR_U8 *pu8N;                      /*!<  public modulus    */
    AR_U8 *pu8E;                      /*!<  public exponent   */
    AR_U8 *pu8D;                      /*!<  private exponent  */
    AR_U8 *pu8P;                      /*!<  1st prime factor  */
    AR_U8 *pu8Q;                      /*!<  2nd prime factor  */
    AR_U8 *pu8DP;                     /*!<  D % (P - 1)       */
    AR_U8 *pu8DQ;                     /*!<  D % (Q - 1)       */
    AR_U8 *pu8QP;                     /*!<  1 / (Q % P)       */
    AR_U16 u16NLen;                   /**< length of public modulus */
    AR_U16 u16ELen;                   /**< length of public exponent */
    AR_U16 u16DLen;                   /**< length of private exponent */
    AR_U16 u16PLen;                   /**< length of 1st prime factor */
    AR_U16 u16QLen;                   /**< length of 2nd prime factor */
    AR_U16 u16DPLen;                  /**< length of D % (P - 1) */
    AR_U16 u16DQLen;                  /**< length of D % (Q - 1) */
    AR_U16 u16QPLen;                  /**< length of 1 / (Q % P) */
}AR_UNF_CIPHER_RSA_PRI_KEY_S;

/** RSA public key encryption struct input */
typedef struct
{
    AR_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme;   /** RSA encryption scheme*/
    AR_UNF_CIPHER_RSA_PUB_KEY_S stPubKey;      /** RSA private key struct */
    AR_UNF_CIPHER_CA_TYPE_E enCaType;
}AR_UNF_CIPHER_RSA_PUB_ENC_S;

/** RSA private key decryption struct input */
typedef struct
{
    AR_UNF_CIPHER_RSA_ENC_SCHEME_E enScheme; /** RSA encryption scheme */
    AR_UNF_CIPHER_RSA_PRI_KEY_S stPriKey;    /** RSA public key struct */
    AR_UNF_CIPHER_CA_TYPE_E enCaType;
}AR_UNF_CIPHER_RSA_PRI_ENC_S;

/** RSA signature struct input */
typedef struct
{
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme;  /** RSA signature scheme*/
    AR_UNF_CIPHER_RSA_PRI_KEY_S stPriKey;      /** RSA private key struct */
    AR_UNF_CIPHER_CA_TYPE_E enCaType;
 }AR_UNF_CIPHER_RSA_SIGN_S;

/** RSA signature verify struct input */
typedef struct
{
    AR_UNF_CIPHER_RSA_SIGN_SCHEME_E enScheme; /** RSA signature scheme*/
    AR_UNF_CIPHER_RSA_PUB_KEY_S stPubKey;     /** RSA public key struct */
 }AR_UNF_CIPHER_RSA_VERIFY_S;

/** @} */  /** <!-- ==== Structure Definition End ==== */


#define AR_UNF_CIPHER_Open(AR_VOID) AR_UNF_CIPHER_Init(AR_VOID);
#define AR_UNF_CIPHER_Close(AR_VOID) AR_UNF_CIPHER_DeInit(AR_VOID);

/******************************* API Declaration *****************************/
/** \addtogroup      CIPHER */
/** @{ */  /** <!-- [CIPHER] */
/* ---CIPHER---*/
/**
\attention \n
This API is used to start the cipher device.
\param N/A
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_FAILED_INIT  The cipher device fails to be initialized.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_Init(AR_VOID);

/**
\brief  Deinit the cipher device.
\attention \n
This API is used to stop the cipher device. If this API is called repeatedly, AR_SUCCESS is returned, but only the first operation takes effect.

\param N/A
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_DeInit(AR_VOID);

/**
\brief Obtain a cipher handle for encryption and decryption.

\param[out] phCipher Cipher handle
\param[in] cipher attributes
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_POINT  The pointer is null.
\retval ::AR_ERR_CIPHER_FAILED_GETHANDLE  The cipher handle fails to be obtained, because there are no available cipher handles.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_CreateHandle(AR_HANDLE* phCipher, const AR_UNF_CIPHER_ATTS_S *pstCipherAttr);

/**
\attention \n
This API is used to destroy existing cipher handles.

\param[in] hCipher Cipher handle
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_DestroyHandle(AR_HANDLE hCipher);

/**
\brief Configures the cipher control information.
\attention \n
Before encryption or decryption, you must call this API to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.

\param[in] hCipher Cipher handle.
\param[in] pstCtrl Cipher control information.
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_POINT  The pointer is null.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_ConfigHandle(AR_HANDLE hCipher, const AR_UNF_CIPHER_CTRL_S* pstCtrl);

/**
\brief Configures the cipher control information.
\attention \n
Before encryption or decryption, you must call this API to configure the cipher control information.
The first 64-bit data and the last 64-bit data should not be the same when using TDES algorithm.

\param[in] hCipher Cipher handle.
\param[in] pstExCtrl Cipher control information.
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_POINT  The pointer is null.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_ConfigHandleEx(AR_HANDLE hCipher, const AR_UNF_CIPHER_CTRL_EX_S* pstExCtrl);

/**
\brief Performs encryption.

\attention \n
This API is used to perform encryption by using the cipher module.
The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering AR_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle
\param[in] u32SrcPhyAddr Physical address of the source data
\param[in] u32DestPhyAddr Physical address of the target data
\param[in] u32ByteLength   Length of the encrypted data
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_Encrypt(AR_HANDLE hCipher, AR_UINTPTR szSrcPhyAddr, AR_UINTPTR szDestPhyAddr, AR_U32 u32ByteLength);

/**
\brief Performs decryption.

\attention \n
This API is used to perform decryption by using the cipher module.
The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering AR_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle.
\param[in] u32SrcPhyAddr Physical address of the source data.
\param[in] u32DestPhyAddr Physical address of the target data.
\param[in] u32ByteLength Length of the decrypted data
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_Decrypt(AR_HANDLE hCipher, AR_UINTPTR szSrcPhyAddr, AR_UINTPTR szDestPhyAddr, AR_U32 u32ByteLength);

/**
\brief Performs encryption.

\attention \n
This API is used to perform encryption by using the cipher module.
The length of the encrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering AR_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle
\param[in] pu8SrcData: buffer of the source data.
\param[out] pu8DestData: buffer of the target data
\param[in] u32ByteLength   Length of the encrypted data
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_EncryptVir(AR_HANDLE hCipher, const AR_U8 *pu8SrcData, AR_U8 *pu8DestData, AR_U32 u32ByteLength);

/**
\brief Performs decryption.

\attention \n
This API is used to perform decryption by using the cipher module.
The length of the decrypted data should be a multiple of 8 in TDES mode and 16 in AES mode. Besides, the length can not be bigger than 0xFFFFF.After this operation, the result will affect next operation.If you want to remove vector, you need to config IV(config pstCtrl->stChangeFlags.bit1IV with 1) by transfering AR_UNF_CIPHER_ConfigHandle.
\param[in] hCipher Cipher handle.
\param[in] pu8SrcData: buffer of the source data.
\param[out] pu8DestData: buffer of the target data
\param[in] u32ByteLength Length of the decrypted data
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_DecryptVir(AR_HANDLE hCipher, const AR_U8 *pu8SrcData, AR_U8 *pu8DestData, AR_U32 u32ByteLength);

/**
\brief Encrypt multiple packaged data.
\attention \n
You can not encrypt more than 128 data package one time. When AR_ERR_CIPHER_BUSY return, the data package you send will not be deal, the customer should decrease the number of data package or run cipher again.Note:When encrypting more than one packaged data, every one package will be calculated using initial vector configured by AR_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle
\param[in] pstDataPkg data package ready for cipher
\param[in] u32DataPkgNum  number of package ready for cipher
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  cipher device have not been initialized
\retval ::AR_ERR_CIPHER_INVALID_PARA  parameter error
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  handle invalid
\retval ::AR_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_EncryptMulti(AR_HANDLE hCipher, const AR_UNF_CIPHER_DATA_S *pstDataPkg, AR_U32 u32DataPkgNum);

/**
\brief Get the cipher control information.

\param[in] hCipher Cipher handle.
\param[in] pstCtrl Cipher control information.
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_POINT  The pointer is null.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_GetHandleConfig(AR_HANDLE hCipher, AR_UNF_CIPHER_CTRL_S* pstCtrl);

/**
\brief Decrypt multiple packaged data.
\attention \n
You can not decrypt more than 128 data package one time.When AR_ERR_CIPHER_BUSY return, the data package you send will not be deal, the custmer should decrease the number of data package or run cipher again.Note:When decrypting more than one packaged data, every one package will be calculated using initial vector configured by AR_UNF_CIPHER_ConfigHandle.Previous result will not affect the later result.
\param[in] hCipher cipher handle
\param[in] pstDataPkg data package ready for cipher
\param[in] u32DataPkgNum  number of package ready for cipher
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  cipher device have not been initialized
\retval ::AR_ERR_CIPHER_INVALID_PARA  parameter error
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  handle invalid
\retval ::AR_ERR_CIPHER_BUSY  hardware is busy, it can not deal with all data package once time
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_DecryptMulti(AR_HANDLE hCipher, const AR_UNF_CIPHER_DATA_S *pstDataPkg, AR_U32 u32DataPkgNum);

/**
\brief Get the tag data of CCM/GCM.

\attention \n
This API is used to get the tag data of CCM/GCM.
\param[in] hCipher cipher handle
\param[out] pu8Tag tag data of CCM/GCM
\param[in/out] pu32TagLen tag data length of CCM/GCM, the input should be 16 now.
\retval ::AR_SUCCESS  Call this API succussful.
\retval ::AR_FAILURE  Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_GetTag(AR_HANDLE hCipher, AR_U8 *pu8Tag, AR_U32 *pu32TagLen);

/**
\brief Encrypt the clean key data by KLAD.
\attention \n
N/A
\param[in] enRootKey klad root key.
\param[in] pu8CleanKey clean key.
\param[in] enTarget the module who to use this key.
\param[out] pu8EcnryptKey encrypt key.
\param[in] u32KeyLen clean key.
\retval ::AR_SUCCESS Call this API successful.
\retval ::AR_FAILURE Call this API fails.
\retval ::AR_ERR_CIPHER_NOT_INIT  The cipher device is not initialized.
\retval ::AR_ERR_CIPHER_INVALID_POINT  The pointer is null.
\retval ::AR_ERR_CIPHER_INVALID_PARA  The parameter is invalid.
\retval ::AR_ERR_CIPHER_INVALID_HANDLE  The handle is invalid.
\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_KladEncryptKey(AR_UNF_CIPHER_CA_TYPE_E enRootKey,
                                    AR_UNF_CIPHER_KLAD_TARGET_E enTarget,
                                    const AR_U8 *pu8CleanKey, AR_U8* pu8EcnryptKey, AR_U32 u32KeyLen);

/**
\brief Get the random number.

\attention \n
This API is used to obtain the random number from the hardware.

\param[out] pu32RandomNumber Point to the random number.
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_GetRandomNumber(AR_U32 *pu32RandomNumber);

/**
\brief Init the hash module, if other program is using the hash module, the API will return failure.

\attention \n
N/A

\param[in] pstHashAttr: The hash calculating structure input.
\param[out] pHashHandle: The output hash handle.
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_HashInit(const AR_UNF_CIPHER_HASH_ATTS_S *pstHashAttr, AR_HANDLE *pHashHandle);

/**
\brief Calculate the hash, if the size of the data to be calculated is very big and the DDR ram is not enough, this API can calculate the data one block by one block. Attention: The input block length must be 64bytes aligned except for the last block.

\attention \n
N/A

\param[in] hHashHandl:  Hash handle.
\param[in] pu8InputData:  The input data buffer.
\param[in] u32InputDataLen:  The input data length, attention: the block length input must be 64bytes aligned except the last block!
\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_HashUpdate(AR_HANDLE hHashHandle, const AR_U8 *pu8InputData, AR_U32 u32InputDataLen);



/**
\brief Get the final hash value, after calculate all of the data, call this API to get the final hash value and close the handle.If there is some reason need to interrupt the calculation, this API should also be call to close the handle.

\attention \n
N/A

\param[in] hHashHandle:  Hash handle.
\param[out] pu8OutputHash:  The final output hash value.

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_HashFinal(AR_HANDLE hHashHandle, AR_U8 *pu8OutputHash);

/**
\brief RSA encryption a plaintext with a RSA public key.

\attention \n
N/A

\param[in] pstRsaEnc:   encryption struct.
\param[in] pu8Input:    input data to be encryption
\param[out] u32InLen:   length of input data to be encryption
\param[out] pu8Output: output data to be encryption
\param[out] pu32OutLen: length of output data to be encryption

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaPublicEncrypt(const AR_UNF_CIPHER_RSA_PUB_ENC_S *pstRsaEnc,
                                  const AR_U8 *pu8Input, AR_U32 u32InLen,
                                  AR_U8 *pu8Output, AR_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA private key.

\attention \n
N/A

\param[in] pstRsaDec:   decryption struct.
\param[in] pu8Input:    input data to be decryption
\param[out] u32InLen:   length of input data to be decryption
\param[out] pu8Output:  output data to be decryption
\param[out] pu32OutLen: length of output data to be decryption

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaPrivateDecrypt(const AR_UNF_CIPHER_RSA_PRI_ENC_S *pstRsaDec,
                                    const AR_U8 *pu8Input, AR_U32 u32InLen,
                                    AR_U8 *pu8Output, AR_U32 *pu32OutLen);

/**
\brief RSA encryption a plaintext with a RSA private key.

\attention \n
N/A

\param[in] pstRsaSign:   encryption struct.
\param[in] pu8Input:     input data to be encryption
\param[out] u32InLen:   length of input data to be encryption
\param[out] pu8Output:  output data to be encryption
\param[out] pu32OutLen: length of output data to be encryption

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaPrivateEncrypt(const AR_UNF_CIPHER_RSA_PRI_ENC_S *pstRsaEnc,
                                    const AR_U8 *pu8Input, AR_U32 u32InLen,
                                    AR_U8 *pu8Output, AR_U32 *pu32OutLen);

/**
\brief RSA decryption a ciphertext with a RSA public key.

\attention \n
N/A

\param[in] pstRsaVerify:   decryption struct.
\param[in] pu8Input:   input data to be decryption
\param[out] u32InLen:   length of input data to be decryption
\param[out] pu8Output: output data to be decryption
\param[out] pu32OutLen: length of output data to be decryption

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaPublicDecrypt(const AR_UNF_CIPHER_RSA_PUB_ENC_S *pstRsaDec,
                               const AR_U8 *pu8Input, AR_U32 u32InLen,
                               AR_U8 *pu8Output, AR_U32 *pu32OutLen);

/**
\brief RSA signature a context with appendix, where a signer's RSA private key is used.

\attention \n
N/A

\param[in] pstRsaSign:      signature struct.
\param[in] pu8Input:        input context to be signature.
\param[in] u32InLen:        length of input context to be signature
\param[in] pu8HashData:    hash value of context,if NULL, let pu8HashData = Hash(context) automatically
\param[out] pu8OutSign:    output message of signature
\param[out] pu32OutSignLen: length of message of signature

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaSign(const AR_UNF_CIPHER_RSA_SIGN_S *pstRsaSign,
                             const AR_U8 *pu8InData, AR_U32 u32InDataLen,
                             const AR_U8 *pu8HashData,
                             AR_U8 *pu8OutSign, AR_U32 *pu32OutSignLen);

/**
\brief RSA signature verification a context with appendix, where a signer's RSA public key is used.

\attention \n
N/A

\param[in] pstRsaVerify:    signature verification struct.
\param[in] pu8Input:       input context to be signature verification, maybe null
\param[in] u32InLen:        length of input context to be signature
\param[in] pu8HashData:    hash value of context,if NULL, let pu8HashData = Hash(context) automatically
\param[in] pu8InSign:      message of signature
\param[in] pu32InSignLen:   length of message of signature

\retval ::AR_SUCCESS  Call this API successful.
\retval ::AR_FAILURE  Call this API fails.

\see \n
N/A
*/
AR_S32 AR_HAL_UNF_CIPHER_RsaVerify(const AR_UNF_CIPHER_RSA_VERIFY_S *pstRsaVerify,
                               const AR_U8 *pu8InData, AR_U32 u32InDataLen,
                               const AR_U8 *pu8HashData,
                               const AR_U8 *pu8InSign, AR_U32 u32InSignLen);

/** @} */  /** <!-- ==== API declaration end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AR_HAL_UNF_CIPHER_H__ */

