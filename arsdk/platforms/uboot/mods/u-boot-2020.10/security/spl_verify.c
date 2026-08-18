/*
 * Artosyn image verify program
 *
 * 1. verify image header
 * 2. verify hash and signature
 * 3. decrypt encrypted image
 */

#include <asm/byteorder.h>
#include <common.h>
#include "pka/pka.h"
#include "spacc/spacc.h"
#include <u-boot/sha256.h>
#include <asm/arch/image.h>

#include <fs.h>

#define SPL_VENDOR_IMAGE_NAME       "vendor"
#define SPL_BL31_IMAGE_NAME         "bl31"
#define SPL_UBOOT_IMAGE_NAME        "uboot"
#define UBOOT_ENV_IMAGE_NAME        "env"
#define UBOOT_KERNEL_IMAGE_NAME     "kernel"
#define UBOOT_UPGRADE_IMAGE_NAME    "upgrade"

extern int gen_derived_key(unsigned char *str1, unsigned char *str2,
		unsigned char *iv, unsigned char iv_len, 
		unsigned char *dst, unsigned int key_len,
		int key_idx);

static unsigned char calc_hash[32] __attribute__((aligned(4)))   = {0};
static unsigned char calc_sign[256] __attribute__((aligned(4)))  = {0};

static const unsigned char decrypt_iv[17] __attribute__((aligned(4)))  = "ARTOSYNSIGN_IV_0";

static unsigned char derive_key[32] __attribute__((aligned(4)))  = {0};
static const unsigned char derive_iv[17] __attribute__((aligned(4)))   = "ARTOSYNDERIVE_IV";
static const unsigned char derive_str1[33] __attribute__((aligned(4))) = "ARTOSYNDERIVE_STRING1_0123456789";
static const unsigned char derive_str2[33] __attribute__((aligned(4))) = "ARTOSYNDERIVE_STRING2_0123456789";

static int key_derived = 0;
#ifdef CONFIG_SPL_BUILD
extern unsigned char vendor_rsa_n[256];
extern unsigned char vendor_rsa_e[4];
extern unsigned char bl31_rsa_n[256];
extern unsigned char bl31_rsa_e[4];
extern unsigned char uboot_rsa_n[256];
extern unsigned char uboot_rsa_e[4];
#else
extern unsigned char env_rsa_n[256];
extern unsigned char env_rsa_e[4];
extern unsigned char kernel_rsa_n[256];
extern unsigned char kernel_rsa_e[4];
extern unsigned char upgrade_rsa_n[256];
extern unsigned char upgrade_rsa_e[4];
#endif

static void reverse_buffer(unsigned char *buf, int len)
{
	int start = 0, end = len - 1;
	unsigned char tmp;

	while(start < end) {
		tmp = buf[start];
		buf[start] = buf[end];
		buf[end] = tmp;
		start++;
		end--;
	}
}
#if 0
void hex_dump(char *msg, unsigned char *buff, unsigned int len)
{
	unsigned int i = 0;
	if (msg) {
		printf("%s:", msg);
	}
	for (i=0;i<len;i++) {
		if ((i % 16) == 0) {
			printf("\n");
		}
		printf("%02x ", buff[i]);
	}
	printf("\n");
}
#endif
int spl_verify_sw(char *image_name, unsigned char *hash, unsigned long hash_len,
		unsigned char *sig, unsigned long sig_len,
		unsigned char *img, unsigned long img_len)
{
	int i = 0;
	int ret = -1;
	unsigned char *external_rsa_n = NULL;
	unsigned char *external_rsa_e = NULL;
	char sig_buf[256], hash_buf[32];

	if(hash_len != 32 || sig_len != 256 ) {
		printf("%s: Bad size\n", __func__);
		return -1;
	}

#ifdef CONFIG_SPL_BUILD
	if(!strcmp(image_name, SPL_VENDOR_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)vendor_rsa_n;
		external_rsa_e = (unsigned char *)vendor_rsa_e;
	}
	else if(!strcmp(image_name, SPL_BL31_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)bl31_rsa_n;
		external_rsa_e = (unsigned char *)bl31_rsa_e;
	}
	else if(!strcmp(image_name, SPL_UBOOT_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)uboot_rsa_n;
		external_rsa_e = (unsigned char *)uboot_rsa_e;
	}
#else
	if(!strcmp(image_name, UBOOT_ENV_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)env_rsa_n;
		external_rsa_e = (unsigned char *)env_rsa_e;
	}
	else if(!strcmp(image_name, UBOOT_KERNEL_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)kernel_rsa_n;
		external_rsa_e = (unsigned char *)kernel_rsa_e;
	}
	else if(!strcmp(image_name, UBOOT_UPGRADE_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)upgrade_rsa_n;
		external_rsa_e = (unsigned char *)upgrade_rsa_e;
	}
#endif
	else
	{
		printf("%s: unknown image name %s\n", __func__, image_name);
		return -1;
	}

	memcpy(sig_buf, sig, 256);
	memcpy(hash_buf, hash, 32);

	sha256_context ctx;
	sha256_starts(&ctx);
	sha256_update(&ctx, img, img_len);
	sha256_finish(&ctx, calc_hash);

	//Compare hash
	while(i < hash_len) {
		if(calc_hash[i] != hash_buf[i]) {
			printf("%s: hash verify fail, len %d\n", image_name, img_len);
			//hex_dump("calc_hash", calc_hash, 32);
			//hex_dump("hash_buf", hash_buf, 32);
			//hex_dump("img head", img, 1024);
			//break;
			return -1;
		}
		i++;
	}

	reverse_buffer((unsigned char *)sig_buf, sig_len);
	reverse_buffer((unsigned char *)hash_buf, hash_len);

	ret = pka_rsa_verify((unsigned char *)sig_buf, sig_len,
			(unsigned char *)calc_sign, 256,
			(unsigned char *)hash_buf, hash_len,
			(unsigned char *)external_rsa_n, 256,
			(unsigned char *)external_rsa_e, 4,
			RSA_2048);
	if(ret != 0) {
		printf("%s: signature verification fail\n", image_name);
	}
	else {
		printf("%s: signature verification pass\n", image_name);
	}
	return ret;
}

int spl_verify_hash(char *image_name, unsigned char *hash, unsigned long hash_len, unsigned char *sig, unsigned long sig_len)
{
	int ret = -1;
	int i = 0;
	char sig_buf[256], hash_buf[32];
	unsigned char *external_rsa_n = NULL;
	unsigned char *external_rsa_e = NULL;

	if(hash_len != 32 || sig_len != 256) {
		printf("Bad size\n");
		return -1;
	}

#ifdef CONFIG_SPL_BUILD
	if(!strcmp(image_name, SPL_VENDOR_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)vendor_rsa_n;
		external_rsa_e = (unsigned char *)vendor_rsa_e;
	}
	else if(!strcmp(image_name, SPL_BL31_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)bl31_rsa_n;
		external_rsa_e = (unsigned char *)bl31_rsa_e;
	}
	else if(!strcmp(image_name, SPL_UBOOT_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)uboot_rsa_n;
		external_rsa_e = (unsigned char *)uboot_rsa_e;
	}
#else
	if(!strcmp(image_name, UBOOT_ENV_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)env_rsa_n;
		external_rsa_e = (unsigned char *)env_rsa_e;
	}
	else if(!strcmp(image_name, UBOOT_KERNEL_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)kernel_rsa_n;
		external_rsa_e = (unsigned char *)kernel_rsa_e;
	}
	else if(!strcmp(image_name, UBOOT_UPGRADE_IMAGE_NAME)) {
		external_rsa_n = (unsigned char *)upgrade_rsa_n;
		external_rsa_e = (unsigned char *)upgrade_rsa_e;
	}
#endif
	else
	{
		printf("%s: unknown image name %s\n", __func__, image_name);
		return -1;
	}

	memcpy(sig_buf, sig, 256);
	memcpy(hash_buf, hash, 32);
	reverse_buffer(sig_buf, sig_len);
	reverse_buffer(hash_buf, hash_len);
	ret = pka_rsa_verify(sig_buf, sig_len, calc_sign, 256,
			hash_buf, hash_len, external_rsa_n, 256,
			external_rsa_e, 4, RSA_2048);
	if(ret != 0) {
		printf("%s: signature verification fail\n", image_name);
	}
	else {
		printf("%s: signature verification pass\n", image_name);
	}
	return ret;
}

int spl_verify(char *image_name, unsigned char *hash, unsigned long hash_len,
		unsigned char *sig, unsigned long sig_len,
		unsigned char *img, unsigned long img_len)
{
	int ret = -1;
    int i = 0;
    unsigned char *external_rsa_n = NULL;
    unsigned char *external_rsa_e = NULL;
	int out_len = 32;

	if(hash_len != 32 || sig_len != 256) {
		printf("Bad size\n");
		return -1;
	}

#ifdef CONFIG_SPL_BUILD
    if(!strcmp(image_name, SPL_VENDOR_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)vendor_rsa_n;
        external_rsa_e = (unsigned char *)vendor_rsa_e;
    }
    else if(!strcmp(image_name, SPL_BL31_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)bl31_rsa_n;
        external_rsa_e = (unsigned char *)bl31_rsa_e;
    }
    else if(!strcmp(image_name, SPL_UBOOT_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)uboot_rsa_n;
        external_rsa_e = (unsigned char *)uboot_rsa_e;
    }
#else
    if(!strcmp(image_name, UBOOT_ENV_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)env_rsa_n;
        external_rsa_e = (unsigned char *)env_rsa_e;
    }
    else if(!strcmp(image_name, UBOOT_KERNEL_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)kernel_rsa_n;
        external_rsa_e = (unsigned char *)kernel_rsa_e;
    }
    else if(!strcmp(image_name, UBOOT_UPGRADE_IMAGE_NAME)) {
        external_rsa_n = (unsigned char *)upgrade_rsa_n;
        external_rsa_e = (unsigned char *)upgrade_rsa_e;
    }
#endif
    else
    {
		printf("%s: unknown image name %s\n", __func__, image_name);
		return -1;
    }

	//Calculate Hash of the image
	ret = spacc_hash(HASH_ALG_SHA256, HASH_MODE_RAW,
				img, img_len,
				calc_hash, out_len,
				NULL, 0);
	if(ret) {
		printf("Calculate Hash fail 0x%x\n", ret);
		return -1;
	}

	//Compare hash
	while(i < hash_len) {
		if(calc_hash[i] != hash[i]) {
			printf("%s: hash verify fail\n", image_name);
			return 1;
		}
		i++;
	}

	reverse_buffer((unsigned char *)sig, sig_len);
	reverse_buffer((unsigned char *)hash, hash_len);

	ret = pka_rsa_verify((unsigned char *)sig, sig_len,
				(unsigned char *)calc_sign, 256,
				(unsigned char *)hash, hash_len,
				(unsigned char *)external_rsa_n, 256,
				(unsigned char *)external_rsa_e, 4,
				RSA_2048);
    if(ret != 0) {
        printf("%s signature verification fail\n", image_name);
    }
    else {
        printf("%s signature verification pass\n", image_name);
    }
    return ret;
}
/*
 * Decrypt image
 * src: address of encrypted image
 * dst: address of decrypted image
 * len: image size
 */
int spl_decrypt(unsigned char *src, unsigned char *dst, int len)
{
	int ret, outlen = ~0;
	//int i;

	/* generate derived key */
	if(key_derived == 0) {
		ret = gen_derived_key((unsigned char *)derive_str1, (unsigned char *)derive_str2, 
				(unsigned char *)derive_iv, 16, derive_key, 32, 1);
		if(ret) {
			printf("Generate derived key fail\n");
			return ret;
		}
		key_derived = 1;
	}

	return spacc_cipher(CIPH_ALG_AES, CIPH_MODE_CBC, 0,
			(unsigned char *)src, len,
			(unsigned char *)dst, outlen,
			derive_key, 32,
			0,
			(unsigned char *)decrypt_iv, 16);
}

