#include "drv_osal_lib.h"
#include "cryp_symc.h"
//#include "ext_alg.h"
//#include "hi_drv_compat.h"
#include "hal_comm_cipher.h"
#include "hal_type.h"
#include "drv_cipher_kapi.h"


/*************************** Internal Structure Definition *******************/

/* ! \max pakage numher of symc mutli encrypt */
#define SYMC_MULTI_MAX_PKG      (0x1000)

#define RSA_PUBLIC_BUFFER_NUM   (0x03)
#define RSA_PRIVATE_BUFFER_NUM  (0x07)

#define MAX_MALLOC_BUF_SIZE     (0x10000)
#define MAX_CENC_SUB_SAMPLE     (100)

typedef AR_S32 (*ar_drv_func)(void *param);

typedef struct
{
    const char *name;
    ar_drv_func func;
    AR_U32 cmd;
}crypto_dispatch_func;

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      link*/
/** @{*/  /** <!-- [link]*/

static AR_S32 dispatch_symc_create_handle(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    symc_create_t *symc_create = argp;

    AR_LOG_FuncEnter();

    /* allocate a aes channel */
    ret = kapi_symc_create(&symc_create->id);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_create, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_destroy_handle(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    symc_destroy_t *destroy = argp;

    AR_LOG_FuncEnter();

    ret = kapi_symc_destroy(destroy->id);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_destroy, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_config(void *argp)
{
    AR_S32 ret = AR_FAILURE;
	AR_U8* skey = NULL;
    symc_config_t *config = argp;

    AR_LOG_FuncEnter();

	if(config->reserve)
	{
		skey = (AR_U8*)config->skey;
	}

	AR_LOG_DEBUG("config->fkey:%02x.\n", config->fkey);
	AR_LOG_DEBUG("skey:%p\n", skey);
	AR_LOG_DEBUG("!!!reserve:%d\n", config->reserve);

    ret = kapi_symc_config(config->id,
                           config->hard_key,
                           config->alg,
                           config->mode,
                           config->width,
                           config->klen,
                           config->sm1_round_num,
                           (AR_U8*)config->fkey,
                           skey,
                           (AR_U8*)config->iv,
                           config->ivlen,
                           config->iv_usage,
                           config->aad,
                           config->alen,
                           config->tlen);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_config, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_encrypt(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    symc_encrypt_t *encrypt = argp;

    AR_LOG_FuncEnter();

    if((SYMC_OPERATION_ENCRYPT == encrypt->operation)
        || (SYMC_OPERATION_DECRYPT == encrypt->operation))
    {
#if 0 //xfqu
        ret = cipher_check_mmz_phy_addr(ADDR_U64(encrypt->input), encrypt->length);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("Invalid input mmz phy addr for crypt.\n");
            AR_LOG_PrintFuncErr(cipher_check_mmz_phy_addr, ret);
            return ret;
        }

        ret = cipher_check_mmz_phy_addr(ADDR_U64(encrypt->output), encrypt->length);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("Invalid output mmz phy addr for crypt.\n");
            AR_LOG_PrintFuncErr(cipher_check_mmz_phy_addr, ret);
            return ret;
        }
#endif
        ret = kapi_symc_crypto(encrypt->id,
                               encrypt->input,
                               encrypt->output,
                               encrypt->length,
                               encrypt->operation,
                               encrypt->last);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(kapi_symc_crypto, ret);
            return ret;
        }
    }
    else if ((SYMC_OPERATION_ENCRYPT_VIA == encrypt->operation)
         || (SYMC_OPERATION_DECRYPT_VIA == encrypt->operation))
    {
        ret = kapi_symc_crypto_via(encrypt->id,
                           encrypt->input,
                           encrypt->output,
                           encrypt->length,
                           encrypt->operation,
                           encrypt->last,
                           AR_TRUE);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(kapi_symc_crypto_via, ret);
            return ret;
        }
    }
    else
    {
        AR_LOG_ERROR("encrypt operation(0x%x) is unsupported.\n", encrypt->operation);
        return AR_ERR_CIPHER_UNSUPPORTED;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_encrypt_multi(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    symc_encrypt_multi_t *encrypt_mutli = argp;

    AR_LOG_FuncEnter();

    AR_LOG_DEBUG("operation %d\n", encrypt_mutli->operation);
    ret = kapi_symc_crypto_multi(encrypt_mutli->id,
                                 ADDR_VIA(encrypt_mutli->pkg),
                                 encrypt_mutli->pkg_num,
                                 encrypt_mutli->operation,
                                 AR_TRUE);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_crypto_multi, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_get_tag(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    aead_tag_t *aead_tag = argp;

    AR_LOG_FuncEnter();

    ret = kapi_aead_get_tag(aead_tag->id,
                            aead_tag->tag,
                            &aead_tag->taglen);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_aead_get_tag, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_symc_get_config(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    symc_get_config_t *get_config = argp;

    AR_LOG_FuncEnter();

    ret = kapi_symc_get_config(get_config->id, &get_config->ctrl);

    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_get_config, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_klad_key(void *argp)
{
#if 0 //xfqu
    AR_S32 ret = AR_FAILURE;
    klad_key_t *klad = argp;

    AR_LOG_FuncEnter();

    ret = klad_encrypt_key(klad->keysel, klad->target, klad->clear, klad->encrypt);
    if (AR_SUCCESS != ret)
    {
       AR_LOG_PrintFuncErr(klad_encrypt_key, ret);
       return ret;
    }

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static AR_S32 dispatch_hash_start(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    hash_start_t *start = argp;
    AR_U8 *key = AR_NULL;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_UNF_CIPHER_HASH_TYPE_BUTT <= start->type);

    if (AR_UNF_CIPHER_HASH_TYPE_SM3 == start->type)
    {
        AR_LOG_ERROR("Sm3 is unsupported.\n");
        return AR_ERR_CIPHER_UNSUPPORTED;
    }

    if (AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA1 == start->type
        || AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA224 == start->type
        || AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA256 == start->type
        || AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA384 == start->type
        || AR_UNF_CIPHER_HASH_TYPE_HMAC_SHA512 == start->type)
    {

        AR_LOG_CHECK_PARAM(MAX_MALLOC_BUF_SIZE < start->keylen);
        AR_LOG_CHECK_PARAM(AR_NULL == ADDR_VIA(start->key));

        key = (AR_U8*)crypto_calloc(1, start->keylen);
        if (AR_NULL == key)
        {
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_FAILED_MEM);
            AR_LOG_PrintFuncErr(crypto_calloc, ret);
            return AR_ERR_CIPHER_FAILED_MEM;
        }

        CHECK_EXIT(crypto_copy_from_user(key, ADDR_VIA(start->key), start->keylen));
    }
#if 0
    CHECK_EXIT(kapi_hash_start(&start->id, start->type, key, start->keylen));
#endif
    if (AR_NULL != key)
    {
        crypto_free(key);
        key = AR_NULL;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;

exit__:
    if (AR_NULL != key)
    {
        crypto_free(key);
        key = AR_NULL;
    }

    return ret;
}

static AR_S32 dispatch_hash_update(void *argp)
{
#if 0
    AR_S32 ret = AR_FAILURE;
    hash_update_t *update = argp;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == ADDR_VIA(update->input));

    update->src = HASH_CHUNCK_SRC_USER;
    ret = kapi_hash_update(update->id,
                           ADDR_VIA(update->input),
                           update->length,
                           update->src);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_hash_update, ret);
        return ret;
    }

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static AR_S32 dispatch_hash_finish(void *argp)
{
#if 0
    AR_S32 ret = AR_FAILURE;
    hash_finish_t *finish = argp;

    AR_LOG_FuncEnter();

    ret = kapi_hash_finish(finish->id, (AR_U8*)finish->hash, &finish->hashlen);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_hash_finish, ret);
        return ret;
    }

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static AR_S32 rsa_alloc_buffer(cryp_rsa_key *key, rsa_info_t *rsa_info,
                            AR_U8 **in, AR_U8 **out)
{
    AR_U32 size = 0, klen = 0;
    AR_S32 ret = AR_FAILURE;
    AR_U8 *buf = AR_NULL;

    AR_LOG_FuncEnter();

    if (AR_FALSE == rsa_info->public)
    {
        AR_LOG_CHECK_PARAM((AR_NULL == ADDR_VIA(rsa_info->d))
            && ((AR_NULL == ADDR_VIA(rsa_info->p))
            || (AR_NULL == ADDR_VIA(rsa_info->q))
            || (AR_NULL == ADDR_VIA(rsa_info->dP))
            || (AR_NULL == ADDR_VIA(rsa_info->dQ))
            || (AR_NULL == ADDR_VIA(rsa_info->qP))));
    }

    AR_LOG_CHECK_PARAM(rsa_info->inlen > rsa_info->klen);
    AR_LOG_CHECK_PARAM(rsa_info->outlen > rsa_info->klen);
    AR_LOG_CHECK_PARAM(RSA_KEY_BITWIDTH_1024 > rsa_info->klen);
    AR_LOG_CHECK_PARAM(RSA_KEY_BITWIDTH_4096 < rsa_info->klen);

    crypto_memset(key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    key->klen = klen = rsa_info->klen;
    key->public = rsa_info->public;
    key->catype = rsa_info->catype;

    if (rsa_info->public)
    {
        /* buffer size of key, input and output */
        size = rsa_info->klen * RSA_PUBLIC_BUFFER_NUM;

        buf = crypto_calloc(1, size);
        if (AR_NULL == buf)
        {
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_FAILED_MEM);
            AR_LOG_PrintFuncErr(crypto_calloc, ret);
            return AR_ERR_CIPHER_FAILED_MEM;
        }

        key->N = buf; buf += klen;
        *in = buf;    buf += klen;
        *out = buf;   buf += klen;

        CHECK_EXIT(crypto_copy_from_user(key->N, ADDR_VIA(rsa_info->N), klen));
        CHECK_EXIT(crypto_copy_from_user(*in, ADDR_VIA(rsa_info->in), klen));
        key->e = rsa_info->e;
    }
    else
    {
        /* n + d or n + p + q + dP + dQ + qp
         * the length of n/d is klen,
         * the length of p/q/dP/dQ/qp is klen/2,
         * the length of input is klen
         * the length of output is klen
         */
        size = klen * RSA_PRIVATE_BUFFER_NUM;

        buf = crypto_calloc(1, size);
        AR_LOG_CHECK_PARAM(AR_NULL == buf);

        key->N  = buf;       buf += klen;
        key->d  = buf;       buf += klen;
        key->p  = buf;       buf += klen / 2;
        key->q  = buf;       buf += klen / 2;
        key->dP = buf;       buf += klen / 2;
        key->dQ = buf;       buf += klen / 2;
        key->qP = buf;       buf += klen / 2;
        key->e  = rsa_info->e;

        if (AR_NULL != ADDR_VIA(rsa_info->N))
        {
            CHECK_EXIT(crypto_copy_from_user(key->N, ADDR_VIA(rsa_info->N), klen));
        }

        if (AR_NULL != ADDR_VIA(rsa_info->d))
        {
            CHECK_EXIT(crypto_copy_from_user(key->d, ADDR_VIA(rsa_info->d), klen));
        }
        else
        {
            CHECK_EXIT(crypto_copy_from_user(key->p, ADDR_VIA(rsa_info->p), klen/2));
            CHECK_EXIT(crypto_copy_from_user(key->q, ADDR_VIA(rsa_info->q), klen/2));
            CHECK_EXIT(crypto_copy_from_user(key->dP, ADDR_VIA(rsa_info->dP), klen/2));
            CHECK_EXIT(crypto_copy_from_user(key->dQ, ADDR_VIA(rsa_info->dQ), klen/2));
            CHECK_EXIT(crypto_copy_from_user(key->qP, ADDR_VIA(rsa_info->qP), klen/2));
            key->d = AR_NULL;
        }

        *in  = buf;       buf += klen;
        *out = buf;       buf += klen;

        if (AR_NULL != ADDR_VIA(rsa_info->in))
        {
            CHECK_EXIT(crypto_copy_from_user(*in, ADDR_VIA(rsa_info->in), rsa_info->inlen));
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;

exit__:
    if (AR_NULL != key->N)
    {
        crypto_free(key->N);
        key->N = AR_NULL;
    }

    AR_LOG_ERROR("error, copy rsa key from user failed\n");
    AR_LOG_PrintErrCode(AR_ERR_CIPHER_FAILED_MEM);

    return AR_ERR_CIPHER_FAILED_MEM;
}

static void rsa_free_buffer(cryp_rsa_key *key)
{
    AR_LOG_FuncEnter();

    if (AR_NULL != key->N)
    {
        crypto_free(key->N);
        key->N = AR_NULL;
    }

    AR_LOG_FuncExit();
    return;
}

static AR_S32 dispatch_rsa_encrypt(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    rsa_info_t *rsa_info = argp;
    AR_U8 *in = AR_NULL;
    AR_U8 *out = AR_NULL;
    cryp_rsa_key key;

    AR_LOG_FuncEnter();

    crypto_memset(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("error, rsa_alloc_key failed\n");
        AR_LOG_PrintFuncErr(rsa_alloc_buffer, ret);
        return ret;
    }
#if 0
    ret = kapi_rsa_encrypt(&key, rsa_info->enScheme, in, rsa_info->inlen,
            out, &rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(kapi_rsa_encrypt, ret);
        return ret;
    }
#endif
    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), out, rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 dispatch_rsa_decrypt(void *argp)
{
#if 0
    AR_S32 ret = AR_FAILURE;
    rsa_info_t *rsa_info = argp;
    AR_U8 *in = AR_NULL;
    AR_U8 *out = AR_NULL;
    cryp_rsa_key key;

    AR_LOG_FuncEnter();

    crypto_memset(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("error, rsa_alloc_key failed\n");
        AR_LOG_PrintFuncErr(rsa_alloc_buffer, ret);
        return ret;
    }

    ret = kapi_rsa_decrypt(&key, rsa_info->enScheme,
                    in, rsa_info->inlen, out, &rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(kapi_rsa_decrypt, ret);
        return ret;
    }

    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), out, rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static AR_S32 dispatch_rsa_sign_hash(void *argp)
{
#if 0
    AR_S32 ret = AR_FAILURE;
    rsa_info_t *rsa_info = argp;
    AR_U8 *in= AR_NULL;
    AR_U8 *out = AR_NULL;
    cryp_rsa_key key;

    AR_LOG_FuncEnter();

    crypto_memset(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("error, rsa alloc key buffer failed\n");
        AR_LOG_PrintFuncErr(rsa_alloc_buffer, ret);
        return ret;
    }

    ret = kapi_rsa_sign_hash(&key, rsa_info->enScheme, in,
            rsa_info->inlen, out, &rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(kapi_rsa_sign_hash, ret);
        return ret;
    }

    ret = crypto_copy_to_user(ADDR_VIA(rsa_info->out), out, rsa_info->outlen);
    if (AR_SUCCESS != ret)
    {
        rsa_free_buffer(&key);
        AR_LOG_PrintFuncErr(crypto_copy_to_user, ret);
        return ret;
    }

    rsa_free_buffer(&key);

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static AR_S32 dispatch_rsa_verify_hash(void *argp)
{
    AR_S32 ret = AR_FAILURE;
    rsa_info_t *rsa_info = argp;
    AR_U8 *in= AR_NULL;
    AR_U8 *out = AR_NULL;
    cryp_rsa_key key;

    AR_LOG_FuncEnter();

    crypto_memset(&key, sizeof(cryp_rsa_key), 0, sizeof(cryp_rsa_key));

    ret = rsa_alloc_buffer(&key, rsa_info, &in, &out);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("error, rsa_alloc_key failed\n");
        AR_LOG_PrintFuncErr(rsa_alloc_buffer, ret);
        return ret;
    }

    /* copy hash value from user */
    CHECK_EXIT(crypto_copy_from_user(out, ADDR_VIA(rsa_info->out), rsa_info->outlen));
#if 0
    CHECK_EXIT(kapi_rsa_verify_hash(&key,
                                    rsa_info->enScheme,
                                    out,
                                    rsa_info->outlen,
                                    in,
                                    rsa_info->inlen));
#endif
    rsa_free_buffer(&key);
    AR_LOG_FuncExit();
    return AR_SUCCESS;

exit__:
    rsa_free_buffer(&key);

    return ret;
}

static AR_S32 dispatch_trng_get_random(void *argp)
{
#if 0
    trng_t *trng = argp;
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    ret = kapi_trng_get_random(&trng->randnum, trng->timeout);
    if (AR_SUCCESS != ret)
    {
        return ret;
    }

    AR_LOG_FuncExit();
#endif
    return AR_SUCCESS;
}

static crypto_dispatch_func dispatch_func[CRYPTO_CMD_COUNT] =
{
    {"CreateHandle",  dispatch_symc_create_handle,  CRYPTO_CMD_SYMC_CREATEHANDLE},
    {"DestroyHandle", dispatch_symc_destroy_handle, CRYPTO_CMD_SYMC_DESTROYHANDLE},
    {"ConfigChn",     dispatch_symc_config,         CRYPTO_CMD_SYMC_CONFIGHANDLE},
    {"Encrypt",       dispatch_symc_encrypt,        CRYPTO_CMD_SYMC_ENCRYPT},
    {"EncryptMulti",  dispatch_symc_encrypt_multi,  CRYPTO_CMD_SYMC_ENCRYPTMULTI},
    {"GetTag",        dispatch_symc_get_tag,        CRYPTO_CMD_SYMC_GETTAG},
    {"HashStart",     dispatch_hash_start,          CRYPTO_CMD_HASH_START},
    {"HashUpdate",    dispatch_hash_update,         CRYPTO_CMD_HASH_UPDATE},
    {"HashFinish",    dispatch_hash_finish,         CRYPTO_CMD_HASH_FINISH},
    {"RsaEncrypt",    dispatch_rsa_encrypt,         CRYPTO_CMD_RSA_ENC},
    {"RsaDecrypt",    dispatch_rsa_decrypt,         CRYPTO_CMD_RSA_DEC},
    {"RsaSign",       dispatch_rsa_sign_hash,       CRYPTO_CMD_RSA_SIGN},
    {"RsaVerify",     dispatch_rsa_verify_hash,     CRYPTO_CMD_RSA_VERIFY},
    {"TRNG",          dispatch_trng_get_random,     CRYPTO_CMD_TRNG},
    {"GetSymcConfig", dispatch_symc_get_config,     CRYPTO_CMD_SYMC_GET_CONFIG},
    {"KladKey",       dispatch_klad_key,            CRYPTO_CMD_KLAD_KEY},
};

AR_S32 crypto_ioctl(AR_U32 cmd, AR_VOID *argp)
{
    AR_U32 nr = 0;
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    nr = CRYPTO_IOC_NR (cmd);

    AR_LOG_DEBUG("cmd 0x%x, nr %d, size %d, local cmd 0x%x\n",
        cmd, nr, CRYPTO_IOC_SIZE(cmd), dispatch_func[nr].cmd);

    AR_LOG_CHECK_PARAM(AR_NULL == argp);
    AR_LOG_CHECK_PARAM(CRYPTO_CMD_COUNT <= nr);
    AR_LOG_CHECK_PARAM(cmd != dispatch_func[nr].cmd);

    AR_LOG_INFO("Link Func NR %d, Name:  %s\n", nr, dispatch_func[nr].name);
    ret = dispatch_func[nr].func(argp);
    if (AR_SUCCESS != ret)
    {
        /*TRNG may be empty in FIFO, don't report error, try to read it again */
        if (CRYPTO_CMD_TRNG != cmd)
        {
            AR_LOG_ERROR("error, call dispatch_fun fun failed!\n");
            AR_LOG_PrintFuncErr(crypto_dispatch_func, ret);
        }
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 crypto_entry(void)
{
    AR_S32 ret = AR_FAILURE;
#if 0 //xfqu
    AR_LOG_FuncEnter();

    crypto_mem_init();

    ret = module_addr_map();
    if (AR_SUCCESS != ret)
    {
       AR_LOG_ERROR("module addr map failed\n");
       AR_LOG_PrintFuncErr(module_addr_map, ret);
       return ret;
    }
#endif

    ret = kapi_symc_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("kapi symc init failed\n");
        AR_LOG_PrintFuncErr(kapi_symc_init, ret);
        //goto error;
    }
#if 0 //xfqu
    ret = kapi_hash_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("kapi hash init failed\n");
        AR_LOG_PrintFuncErr(kapi_hash_init, ret);
        goto error1;
    }

    ret = kapi_rsa_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("kapi rsa init failed\n");
        AR_LOG_PrintFuncErr(kapi_rsa_init, ret);
        goto error2;
    }

    ret = hi_drv_compat_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(hi_drv_compat_init, ret);
        goto error3;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;

error3:
    kapi_rsa_deinit();
error2:
    kapi_hash_deinit();
error1:
    kapi_symc_deinit();
error:
    module_addr_unmap();
#endif
    return ret;

}

AR_S32 crypto_exit(void)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    ret = kapi_symc_deinit();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_deinit, ret);
        return ret;
    }

#if 0 //xfqu
    ret = kapi_hash_deinit();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_hash_deinit, ret);
        return ret;
    }

    ret = kapi_rsa_deinit();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_rsa_deinit, ret);
        return ret;
    }

    ret = hi_drv_compat_deinit();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(hi_drv_compat_deinit, ret);
        return ret;
    }

    ret = module_addr_unmap();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(module_addr_unmap, ret);
        return ret;
    }
#endif
    AR_LOG_FuncExit();
    return AR_SUCCESS;
}


AR_S32 crypto_release(void)
{
#if 0
    AR_S32 ret = AR_FAILURE;

    ret = kapi_symc_release();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_release, ret);
        return ret;
    }

    ret = kapi_hash_release();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_hash_release, ret);
        return ret;
    }
#endif
    return AR_SUCCESS;
}

/** @}*/  /** <!-- ==== Structure Definition end ====*/

