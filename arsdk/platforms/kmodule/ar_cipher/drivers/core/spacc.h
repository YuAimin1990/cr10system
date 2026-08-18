#ifndef __SPACC_H__
#define __SPACC_H__

//CBC CS mode setting
#define CBC_CS_SEL_NULL           0
#define CBC_CS_SEL_1              1
#define CBC_CS_SEL_2              2
#define CBC_CS_SEL_3              3


//Cipher algorithm setting
#define CIPH_ALG_NULL             0
#define CIPH_ALG_DES              1
#define CIPH_ALG_AES              2
#define CIPH_ALG_RC4              3
#define CIPH_ALG_MULTI2           4
#define CIPH_ALG_KASUMI           5
#define CIPH_ALG_SNOW             6
#define CIPH_ALG_ZUC              7
#define CIPH_ALG_EE3              128

//Cipher mode setting
#define CIPH_MODE_ECB             0
#define CIPH_MODE_CBC             1
#define CIPH_MODE_CTR             2
#define CIPH_MODE_CCM             3
#define CIPH_MODE_GCM             5
#define CIPH_MODE_OFB             7
#define CIPH_MODE_CFB             8
#define CIPH_MODE_F8              9
#define CIPH_MODE_XTS             10

//Hash algorithm setting
#define HASH_ALG_NULL             0
#define HASH_ALG_MD5              1
#define HASH_ALG_SHA1             2
#define HASH_ALG_SHA224           3
#define HASH_ALG_SHA256           4
#define HASH_ALG_SHA384           5
#define HASH_ALG_SHA512           6
#define HASH_ALG_AES_XCBC_MAC     7
#define HASH_ALG_AES_CMAC         8
#define HASH_ALG_KASUMI_F9        9
#define HASH_ALG_SNOW             10
#define HASH_ALG_CRC32            11
#define HASH_ALG_ZUC              12
#define HASH_ALG_SHA512_224       13
#define HASH_ALG_SHA512_256       14
#define HASH_ALG_MICHAEL          15
#define HASH_ALG_SHA3_224         16
#define HASH_ALG_SHA3_256         17
#define HASH_ALG_SHA3_384         18
#define HASH_ALG_SHA3_512         19

//Hash mode settings
#define HASH_MODE_RAW             0
#define HASH_MODE_SSLMAC          1
#define HASH_MODE_HMAC            2

//Error defination
#define SPACC_ERR_OK              0
#define SPACC_ERR_ICV_FAIL        1
#define SPACC_ERR_MEMORY_ERROR    2
#define SPACC_ERR_BLOCK_ERROR     3
#define SPACC_ERR_SECURITY_ERROR  4

enum padmode {
	AES_PADDING_NONE,
	AES_PADDING_DEFAULT = 0x80,
};

/*
 * spacc module init, the first function to be invocated while use spacc
 */
void spacc_init(void);

void  spacc_set_interrupt(uint8_t isEnable);

/*
 * create spacc chiper instance and init the instance
 *
 * return 0 on success, otherwise failed
 */
void spacc_cipher_create(void);


void spacc_cipher_set_algorithm(uint32_t algo, uint32_t mode);

void spacc_cipher_set_encrypt(uint32_t encrypt);

void spacc_cipher_set_all_input(uint8_t *src, uint32_t length);

void spacc_cipher_set_all_output(uint8_t *dst, uint32_t length);

int spacc_cipher_set_key_ctx(uint32_t ctx);

int spacc_cipher_set_keylen(uint32_t key_sz);

int spacc_cipher_set_key_single(uint8_t *key, uint32_t odd);

int spacc_cipher_set_iv(uint8_t *iv);

int spacc_cipher_get_iv(uint8_t *iv);

int spacc_cipher_set_padmode(enum padmode padding_mode);

/*
 * configure the spacc cipher
 * parameters:
 * 		algo: cipher algorithm
 * 		mode: chiper mode
 * 		encrypt: 1 encrypt, 0 decrypt
 * 		key1: key
 * 		key2: XTS has 2 keys, otherwise is NULL
 * 		key_len: key length
 * 		ctx: key context
 * 		iv: Initalization Vector, CBC mode need it for example
 * 		padding_mode: padding mode
 *
 * return 0 on success, otherwise failed
 */
unsigned int spacc_cipher_config(uint8_t algo, uint8_t mode, uint8_t encrypt,
					uint8_t *key1, uint8_t *key2, uint32_t key_len,
					uint8_t ctx,
					uint8_t *iv,
					enum padmode padding_mode);


unsigned int spacc_cipher_start(int chiper);

/*
 * encrypt or decrypt based on config
 * parameters:
 * 		src: plaintext if encrypt, otherwise ciphertext
 * 		dst: ciphertext buffer to be stored if encrypt, otherwise plaintext buffer to be stored
 * 		len: src length, dst buffer len malloced must >= src + 16 if not AES_PADDING_NONE while encrypt
 *
 * return error code (negative) while error or real data size (positive) while success
 */
unsigned int spacc_cipher_update(uint8_t *src, uint8_t *dst, uint32_t len, int chiper);

/*
 * finish single operation, must pair with spacc_chiper_create()
 *
 * return 0 on success, otherwise failed
 */
unsigned int spacc_cipher_final(void);

#endif
