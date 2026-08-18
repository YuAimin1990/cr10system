#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "hal_type.h"
#include "mpi_unf_cipher.h"
#include "mpi_vb.h"
//#include "hi_common.h"
//#include "hi_mmz_api.h"

#define AR_ERR_CIPHER(format, arg...)     printf( "\033[0;1;31m" format "\033[0m", ## arg)
#define AR_INFO_CIPHER(format, arg...)    printf( "\033[0;1;32m" format "\033[0m", ## arg)
#define TEST_END_PASS()                   AR_INFO_CIPHER("****************** %s app test PASS !!! ******************\n", __FUNCTION__)
#define TEST_END_FAIL()                   AR_ERR_CIPHER("****************** %s app test FAIL !!! ******************\n", __FUNCTION__)
#define TEST_RESULT_PRINT()               { if (ret) TEST_END_FAIL(); else TEST_END_PASS();}

typedef AR_S32 (*list_func)(AR_VOID);

/*
static AR_U8 aes_128_cbc_IV[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,};
static AR_U8 aes_128_cbc_key[16]= {0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
static AR_U8 aes_128_src_buf[16] = {0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static AR_U8 aes_128_dst_buf[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
*/

static AR_S32 printBuffer(AR_CHAR *string, AR_U8 *pu8Input, AR_U32 u32Length)
{
    AR_U32 i = 0;

    if ( NULL != string )
    {
        printf("%s\n", string);
    }

    for ( i = 0 ; i < u32Length; i++ )
    {
        if( (i % 16 == 0) && (i != 0)) printf("\n");
        printf("0x%02x ", pu8Input[i]);
    }
    printf("\n");

    return AR_SUCCESS;
}

VB_POOL sPoolId = VB_INVALID_POOLID;
static AR_S32 TestVbInit()
{
	AR_MPI_VB_Init();

	VB_POOL_CONFIG_S stPoolConfig;
	memset(&stPoolConfig, 0, sizeof(VB_POOL_CONFIG_S));
	stPoolConfig.u64BlkSize = 256;
	stPoolConfig.u32BlkCnt  = 10;

	sPoolId = AR_MPI_VB_CreatePool(&stPoolConfig);
	if(VB_INVALID_POOLID != sPoolId){
		if(AR_MPI_VB_MmapPool(sPoolId)){
			AR_MPI_VB_DestroyPool(sPoolId);
		}
		else{
			printf("TestVbInit success!\n");
			return 0;
		}
	}

	return -1;
}

int GetBuf(AR_U64 *u64_physaddr, AR_VOID ** pp_vir_addr)
{
	VB_BLK block_id = AR_MPI_VB_GetBlock(sPoolId, 32, NULL);
	if(VB_INVALID_HANDLE == block_id){
		return -1;
	}

	AR_U64 physaddr = 0;
	physaddr = AR_MPI_VB_Handle2PhysAddr(block_id);
	if(!physaddr){
		return -1;
	}
	*u64_physaddr = physaddr;

	AR_VOID *vir_addr = NULL;
	if(AR_MPI_VB_GetBlockVirAddr(sPoolId, physaddr, &vir_addr)){
		return -1;
	}
	*pp_vir_addr = vir_addr;

	return 0;
}
static AR_S32 Setconfiginfo(AR_HANDLE chnHandle, AR_BOOL bKeyByCA, UNF_CIPHER_ALG_E alg, UNF_CIPHER_WORK_MODE_E mode, UNF_CIPHER_KEY_LENGTH_E keyLen,
                                                const AR_U8 u8KeyBuf[32], const AR_U8 u8IVBuf[16])
{
    AR_S32 s32Ret = AR_SUCCESS;
    UNF_CIPHER_CTRL_S CipherCtrl;

    memset(&CipherCtrl, 0, sizeof(UNF_CIPHER_CTRL_S));
    CipherCtrl.enAlg = alg;
    CipherCtrl.enWorkMode = mode;
    CipherCtrl.enBitWidth = UNF_CIPHER_BIT_WIDTH_128BIT;
    CipherCtrl.enKeyLen = keyLen;
    CipherCtrl.bKeyByCA = bKeyByCA;
    if(CipherCtrl.enWorkMode != UNF_CIPHER_WORK_MODE_ECB)
    {
        CipherCtrl.stChangeFlags.bit1IV = 1;  //must set for CBC , CFB mode
        memcpy(CipherCtrl.u32IV, u8IVBuf, 16);
    }

    if(keyLen == UNF_CIPHER_KEY_AES_128BIT)
    {
        memcpy(CipherCtrl.u32Key, u8KeyBuf, 16);
    }
    else
    {
        memcpy(CipherCtrl.u32Key, u8KeyBuf, 32);
    }

    s32Ret = AR_MPI_UNF_CIPHER_ConfigHandle(chnHandle, &CipherCtrl);
    if(AR_SUCCESS != s32Ret)
    {
    	AR_ERR_CIPHER("Setconfiginfo failed!, handle:%d\n", chnHandle);
        return AR_FAILURE;
    }

	printf("Setconfiginfo success, handle:%d!\n", chnHandle);

    return AR_SUCCESS;
}


/* encrypt data using special chn*/
static AR_S32 ECB_AES256(AR_VOID)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 u32TestDataLen = 32;
    AR_UINTPTR szInputAddrPhy = 0;
    AR_UINTPTR szOutPutAddrPhy = 0;
    AR_U8 *pInputAddrVir = AR_NULL;
    AR_U8 *pOutputAddrVir = AR_NULL;
    AR_HANDLE hTestchnid = 0;
    UNF_CIPHER_ATTS_S stCipherAttr;

    AR_U8 aes_key[32] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                         0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
    AR_U8 aes_src[32] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                         0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
    AR_U8 aes_dst[32] = {0x86,0x3a,0xe9,0x1f,0xd4,0xcf,0x51,0x13,0x2d,0xc3,0xe4,0xed,0xf0,0x9d,0xbc,0x6e,
                         0x86,0x3a,0xe9,0x1f,0xd4,0xcf,0x51,0x13,0x2d,0xc3,0xe4,0xed,0xf0,0x9d,0xbc,0x6e};
    AR_U8 aes_IV[16]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};

    s32Ret = AR_MPI_UNF_CIPHER_Init();
    if(AR_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stCipherAttr.enCipherType = UNF_CIPHER_TYPE_NORMAL;
    s32Ret = AR_MPI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(AR_SUCCESS != s32Ret)
    {
        AR_MPI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    printf("hTestchnid 0x%d\n", hTestchnid);

	if(GetBuf((AR_U64*)&szInputAddrPhy, (AR_VOID**)&pInputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

	if(GetBuf((AR_U64*)&szOutPutAddrPhy, (AR_VOID**)&pOutputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            AR_FALSE,
                            UNF_CIPHER_ALG_AES,
                            UNF_CIPHER_WORK_MODE_ECB,
                            UNF_CIPHER_KEY_AES_256BIT,
                            aes_key,
                            aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = AR_MPI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-ENC:", pOutputAddrVir, sizeof(aes_dst));

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    AR_FALSE,
                                    UNF_CIPHER_ALG_AES,
                                    UNF_CIPHER_WORK_MODE_ECB,
                                    UNF_CIPHER_KEY_AES_256BIT,
                                    aes_key,
                                    aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = AR_MPI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-DEC:", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    AR_MPI_UNF_CIPHER_DestroyHandle(hTestchnid);
    AR_MPI_UNF_CIPHER_DeInit();

    return s32Ret;
}


/* encrypt data using special chn*/
static AR_S32 CBC_AES256(AR_VOID)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 u32TestDataLen = 16;
    AR_UINTPTR szInputAddrPhy = 0;
    AR_UINTPTR szOutPutAddrPhy = 0;
    //AR_U32 u32Testcached = 0;
    AR_U8 *pInputAddrVir = AR_NULL;
    AR_U8 *pOutputAddrVir = AR_NULL;
    AR_HANDLE hTestchnid = 0;
    UNF_CIPHER_ATTS_S stCipherAttr;

    AR_U8 aes_key[32] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                         0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
    AR_U8 aes_IV[16]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    AR_U8 aes_src[16] = {0x6B,0xC1,0xBE,0xE2,0x2E,0x40,0x9F,0x96,0xE9,0x3D,0x7E,0x11,0x73,0x93,0x17,0x2A};
    AR_U8 aes_dst[16] = {0xe0,0xfa,0xab,0x42,0xd5,0xa5,0x5c,0x18,0xb0,0x13,0x1f,0x47,0x6c,0xa5,0xd0,0x0a};

    s32Ret = AR_MPI_UNF_CIPHER_Init();
    if(AR_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stCipherAttr.enCipherType = UNF_CIPHER_TYPE_NORMAL;
    s32Ret = AR_MPI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(AR_SUCCESS != s32Ret)
    {
        AR_MPI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    printf("hTestchnid 0x%d\n", hTestchnid);

	if(GetBuf((AR_U64*)&szInputAddrPhy, (AR_VOID**)&pInputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

	if(GetBuf((AR_U64*)&szOutPutAddrPhy, (AR_VOID**)&pOutputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            AR_FALSE,
                            UNF_CIPHER_ALG_AES,
                            UNF_CIPHER_WORK_MODE_CBC,
                            UNF_CIPHER_KEY_AES_256BIT,
                            aes_key,
                            aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

    memset(pOutputAddrVir, 0x0, u32TestDataLen);
	printf("szInputAddrPhy:%02lx pInputAddrVir:%p\n", szInputAddrPhy, pInputAddrVir);
	printf("szOutPutAddrPhy:%02lx pOutputAddrVir:%p\n", szOutPutAddrPhy, pOutputAddrVir);

    s32Ret = AR_MPI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-ENC:", pOutputAddrVir, sizeof(aes_dst));

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, pOutputAddrVir, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    AR_FALSE,
                                    UNF_CIPHER_ALG_AES,
                                    UNF_CIPHER_WORK_MODE_CBC,
                                    UNF_CIPHER_KEY_AES_256BIT,
                                    aes_key,
                                    aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = AR_MPI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-DEC:", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    AR_MPI_UNF_CIPHER_DestroyHandle(hTestchnid);
    AR_MPI_UNF_CIPHER_DeInit();

    return s32Ret;
}

static AR_S32 CBC_AES192(AR_VOID)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 u32TestDataLen = 16;
    AR_UINTPTR szInputAddrPhy = 0;
    AR_UINTPTR szOutPutAddrPhy = 0;
    AR_U8 *pInputAddrVir = AR_NULL;
    AR_U8 *pOutputAddrVir = AR_NULL;
    AR_HANDLE hTestchnid = 0;
    UNF_CIPHER_ATTS_S stCipherAttr;

/*
    AR_U8 aes_src[16] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00};
    AR_U8 aes_dst[16]  = {0xc1,0x1b,0x54,0x4a,0x12,0x9c,0x08,0xa5,0xcc,0xd3,0xeb,0xec,0x7a,0x3b,0x00,0x2b};
    AR_U8 aes_key[16] = {0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04};
    AR_U8 aes_IV[16]  = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
*/

    AR_U8 aes_key[32] = {0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,
                         0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11};
    AR_U8 aes_IV[16]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    AR_U8 aes_src[16] = {0x6B,0xC1,0xBE,0xE2,0x2E,0x40,0x9F,0x96,0xE9,0x3D,0x7E,0x11,0x73,0x93,0x17,0x2A};
    AR_U8 aes_dst[16] = {0x69,0x25,0x8f,0xc2,0x8d,0x58,0x8b,0x84,0x67,0x87,0x76,0x02,0x2f,0x9c,0x1c,0x91};

    s32Ret = AR_MPI_UNF_CIPHER_Init();
    if(AR_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stCipherAttr.enCipherType = UNF_CIPHER_TYPE_NORMAL;
    s32Ret = AR_MPI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(AR_SUCCESS != s32Ret)
    {
        AR_MPI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    printf("hTestchnid 0x%d\n", hTestchnid);

	if(GetBuf((AR_U64*)&szInputAddrPhy, (AR_VOID**)&pInputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

	if(GetBuf((AR_U64*)&szOutPutAddrPhy, (AR_VOID**)&pOutputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            AR_FALSE,
                            UNF_CIPHER_ALG_AES,
                            UNF_CIPHER_WORK_MODE_CBC,
                            UNF_CIPHER_KEY_AES_192BIT,
                            aes_key,
                            aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = AR_MPI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-ENC:", pOutputAddrVir, sizeof(aes_dst));

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    AR_FALSE,
                                    UNF_CIPHER_ALG_AES,
                                    UNF_CIPHER_WORK_MODE_CBC,
                                    UNF_CIPHER_KEY_AES_192BIT,
                                    aes_key,
                                    aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = AR_MPI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-DEC:", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:

    AR_MPI_UNF_CIPHER_DestroyHandle(hTestchnid);
    AR_MPI_UNF_CIPHER_DeInit();

    return s32Ret;
}

/* encrypt data using special chn*/
static AR_S32 CBC_AES128(AR_VOID)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_U32 u32TestDataLen = 16;
    AR_UINTPTR szInputAddrPhy = 0;
    AR_UINTPTR szOutPutAddrPhy = 0;
    AR_U8 *pInputAddrVir = AR_NULL;
    AR_U8 *pOutputAddrVir = AR_NULL;
    AR_HANDLE hTestchnid = 0;
    UNF_CIPHER_ATTS_S stCipherAttr;

/*
    AR_U8 aes_src[16] = {0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00};
    AR_U8 aes_dst[16]  = {0xc1,0x1b,0x54,0x4a,0x12,0x9c,0x08,0xa5,0xcc,0xd3,0xeb,0xec,0x7a,0x3b,0x00,0x2b};
    AR_U8 aes_key[16] = {0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04};
    AR_U8 aes_IV[16]  = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
*/

    AR_U8 aes_key[16] = {0x2B,0x7E,0x15,0x16,0x28,0xAE,0xD2,0xA6,0xAB,0xF7,0x15,0x88,0x09,0xCF,0x4F,0x3C};
    AR_U8 aes_IV[16]  = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
    AR_U8 aes_src[16] = {0x6B,0xC1,0xBE,0xE2,0x2E,0x40,0x9F,0x96,0xE9,0x3D,0x7E,0x11,0x73,0x93,0x17,0x2A};
    AR_U8 aes_dst[16] = {0x76,0x49,0xAB,0xAC,0x81,0x19,0xB2,0x46,0xCE,0xE9,0x8E,0x9B,0x12,0xE9,0x19,0x7D};

    s32Ret = AR_MPI_UNF_CIPHER_Init();
    if(AR_SUCCESS != s32Ret)
    {
        return s32Ret;
    }

    stCipherAttr.enCipherType = UNF_CIPHER_TYPE_NORMAL;
    s32Ret = AR_MPI_UNF_CIPHER_CreateHandle(&hTestchnid, &stCipherAttr);
    if(AR_SUCCESS != s32Ret)
    {
        AR_MPI_UNF_CIPHER_DeInit();
        return s32Ret;
    }

    printf("hTestchnid 0x%d\n", hTestchnid);

	if(GetBuf((AR_U64*)&szInputAddrPhy, (AR_VOID**)&pInputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

	if(GetBuf((AR_U64*)&szOutPutAddrPhy, (AR_VOID**)&pOutputAddrVir))
	{
		goto __CIPHER_EXIT__;
	}

    /* For encrypt */
    s32Ret = Setconfiginfo(hTestchnid,
                            AR_FALSE,
                            UNF_CIPHER_ALG_AES,
                            UNF_CIPHER_WORK_MODE_CBC,
                            UNF_CIPHER_KEY_AES_128BIT,
                            aes_key,
                            aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    memset(pInputAddrVir, 0x0, u32TestDataLen);
    memcpy(pInputAddrVir, aes_src, u32TestDataLen);
    printBuffer("CBC-AES-128-ORI:", aes_src, sizeof(aes_src));

    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = AR_MPI_UNF_CIPHER_Encrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher encrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-ENC:", pOutputAddrVir, sizeof(aes_dst));

    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_dst, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher encrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

   /* For decrypt */
    memcpy(pInputAddrVir, aes_dst, u32TestDataLen);
    memset(pOutputAddrVir, 0x0, u32TestDataLen);

    s32Ret = Setconfiginfo(hTestchnid,
                                    AR_FALSE,
                                    UNF_CIPHER_ALG_AES,
                                    UNF_CIPHER_WORK_MODE_CBC,
                                    UNF_CIPHER_KEY_AES_128BIT,
                                    aes_key,
                                    aes_IV);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Set config info failed.\n");
        goto __CIPHER_EXIT__;
    }

    s32Ret = AR_MPI_UNF_CIPHER_Decrypt(hTestchnid, szInputAddrPhy, szOutPutAddrPhy, u32TestDataLen);
    if(AR_SUCCESS != s32Ret)
    {
        AR_ERR_CIPHER("Cipher decrypt failed.\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    printBuffer("CBC-AES-128-DEC:", pOutputAddrVir, u32TestDataLen);
    /* compare */
    if ( 0 != memcmp(pOutputAddrVir, aes_src, u32TestDataLen) )
    {
        AR_ERR_CIPHER("cipher decrypt, memcmp failed!\n");
        s32Ret = AR_FAILURE;
        goto __CIPHER_EXIT__;
    }

    TEST_END_PASS();

__CIPHER_EXIT__:
    AR_MPI_UNF_CIPHER_DestroyHandle(hTestchnid);
    AR_MPI_UNF_CIPHER_DeInit();

    return s32Ret;
}

int main(int argc,char* argv[])
{
	AR_S32 s32Ret = AR_SUCCESS;

	if(TestVbInit())
	{
		AR_MPI_VB_Exit();
		return AR_FAILURE;
	}

	s32Ret = ECB_AES256();
	if (s32Ret != AR_SUCCESS)
	{
		return s32Ret;
	}

	s32Ret = CBC_AES256();
	if (s32Ret != AR_SUCCESS)
	{
		return s32Ret;
	}

	s32Ret = CBC_AES192();
	if (s32Ret != AR_SUCCESS)
	{
		return s32Ret;
	}

	s32Ret = CBC_AES128();
	if (s32Ret != AR_SUCCESS)
	{
		return s32Ret;
	}

	AR_MPI_VB_Exit();

	return AR_SUCCESS;
}

