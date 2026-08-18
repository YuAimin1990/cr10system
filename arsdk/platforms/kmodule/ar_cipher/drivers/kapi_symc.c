#include "drv_osal_lib.h"
#include "cryp_symc.h"
//#include "hi_drv_compat.h"

/* max number of nodes */
#define MAX_PKG_NUMBER              (100000)

/* max length of CCM/GCM AAD */
#define MAX_AEAD_A_LEN              (0x100000)

typedef struct
{
    AR_U32 open   : 1;                  /*!<  open or close */
    AR_U32 config : 1;                  /*!<  aleardy config or not */
    symc_func *func;
    void *cryp_ctx;                  /*!<  Context of cryp instance */
    crypto_owner owner;              /*!<  user ID */
    AR_UNF_CIPHER_CTRL_S  ctrl;        /*!<  control infomation */
}kapi_symc_ctx;

/*! Context of cipher */
static kapi_symc_ctx kapi_ctx[CRYPTO_HARD_CHANNEL_MAX];

/* symc mutex */
static crypto_mutex symc_mutex;

#define KAPI_SYMC_CHECK_HANDLE(handle)   \
do \
{ \
    if((AR_ID_CIPHER != AR_HANDLE_GET_MODID(handle)) \
        || (0 != AR_HANDLE_GET_PriDATA(handle))) \
    { \
        AR_LOG_ERROR("Invalid handle 0x%x!\n", handle); \
        return AR_ERR_CIPHER_INVALID_HANDLE; \
    } \
    if (CRYPTO_HARD_CHANNEL_MAX <= AR_HANDLE_GET_CHNID(handle)) \
    { \
        AR_LOG_ERROR("chan %d is too large, max: %d\n", AR_HANDLE_GET_CHNID(handle), CRYPTO_HARD_CHANNEL_MAX); \
        return AR_ERR_CIPHER_INVALID_HANDLE; \
    } \
    if (AR_FALSE == kapi_ctx[AR_HANDLE_GET_CHNID(handle)].open) \
    { \
        AR_LOG_ERROR("chan %d is not open\n", AR_HANDLE_GET_CHNID(handle)); \
        return AR_ERR_CIPHER_INVALID_HANDLE; \
    } \
} while (0)

#define KAPI_SYMC_LOCK()   \
    ret = crypto_mutex_lock(&symc_mutex);  \
    if (AR_SUCCESS != ret)        \
    {\
        AR_LOG_ERROR("error, symc lock failed\n");\
        AR_LOG_PrintFuncErr(crypto_mutex_lock, ret);\
        return ret;\
    }

#define KAPI_SYMC_UNLOCK()   crypto_mutex_unlock(&symc_mutex)
#define AES_CCM_MIN_TAG_LEN     (4)
#define AES_CCM_MAX_TAG_LEN     (16)
#define AES_GCM_MIN_TAG_LEN     (1)
#define AES_GCM_MAX_TAG_LEN     (16)

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      symc */
/** @{*/  /** <!-- [kapi]*/

AR_S32 kapi_symc_init(void)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_INFO("kapi_symc_init()\n");

    AR_LOG_FuncEnter();

    crypto_mutex_init(&symc_mutex);

    crypto_memset(kapi_ctx, sizeof(kapi_ctx), 0, sizeof(kapi_ctx));

    ret = cryp_symc_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_init, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_symc_deinit(void)
{
    AR_LOG_INFO("kapi_symc_deinit()\n");

    AR_LOG_FuncEnter();

    cryp_symc_deinit();

    crypto_mutex_destroy(&symc_mutex);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_symc_release(void)
{
    AR_U32 i = 0, chn = 0;
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    crypto_owner owner;

    AR_LOG_FuncEnter();

    crypto_get_owner(&owner);

    AR_LOG_INFO("symc release owner 0x%x\n", owner);

    /* destroy the channel which are created by current user */
    for (i=0; i<CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        ctx = &kapi_ctx[i];
        if (AR_TRUE == ctx->open)
        {
            if (0 == memcmp(&owner, &ctx->owner, sizeof(owner)))
            {
                chn = AR_HANDLE_MAKEHANDLE(AR_ID_CIPHER, 0, i);
                AR_LOG_INFO("symc release chn %d\n", chn);
                ret = kapi_symc_destroy(chn);
                if (AR_SUCCESS != ret)
                {
                    AR_LOG_PrintFuncErr(kapi_symc_destroy, ret);
                    return ret;
                }
            }
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}


AR_S32 kapi_symc_create(AR_U32 *id)
{
    AR_S32 ret = AR_FAILURE;
    AR_U32 chn = 0;
    kapi_symc_ctx *ctx = AR_NULL;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == id);

    KAPI_SYMC_LOCK();

    /* allocate a aes soft channel for hard channel allocted */
    ret = cryp_symc_alloc_chn(&chn);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("error, allocate symc channel failed\n");
        AR_LOG_PrintFuncErr(cryp_symc_alloc_chn, ret);
        KAPI_SYMC_UNLOCK();
        return ret;
    }
    ctx = &kapi_ctx[chn];

    crypto_memset(ctx, sizeof(kapi_symc_ctx), 0, sizeof(kapi_symc_ctx));
    crypto_get_owner(&ctx->owner);

    *id = AR_HANDLE_MAKEHANDLE(AR_ID_CIPHER, 0, chn);
    ctx->open = AR_TRUE;
    ctx->config = AR_FALSE;

    AR_LOG_INFO("kapi_symc_create()- chn %d\n", chn);

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_symc_destroy(AR_U32 id)
{
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    AR_U32 softId = 0;

    AR_LOG_FuncEnter();

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);

    KAPI_SYMC_LOCK();

    cryp_symc_free_chn(softId);

    /* Destroy the attached instance of Symmetric cipher engine */
    if ((AR_NULL != ctx->func) && (AR_NULL != ctx->func->destroy))
    {
        ret = ctx->func->destroy(ctx->cryp_ctx);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("error, cryp_symc_destory failed\n");
            AR_LOG_PrintFuncErr(cryp_symc_destory, ret);
            KAPI_SYMC_UNLOCK();
            return ret;
        }
        ctx->cryp_ctx = AR_NULL;
    }

    ctx->open = AR_FALSE;

    AR_LOG_INFO("kapi_symc_destroy()- chn 0x%x\n", id);

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 kapi_symc_width_check(AR_UNF_CIPHER_ALG_E enAlg, AR_UNF_CIPHER_WORK_MODE_E mode, AR_U32 width, AR_U32 round)
{
   /* the bit width depend on alg and mode, which limit to hardware
    * des/3des with cfb/ofb support bit1, bit8, bit 64.
    * aes with cfb/ofb only support bit128.
    * sm1 with ofb only support bit128, cfb support bit1, bit8, bit 64.
    */

    AR_LOG_FuncEnter();

    if ((enAlg == AR_UNF_CIPHER_ALG_3DES) || (enAlg == AR_UNF_CIPHER_ALG_DES))
    {
        if (mode > AR_UNF_CIPHER_WORK_MODE_OFB)
        {
            AR_LOG_ERROR("Invalid enAlg %d and mode %d\n",enAlg, mode);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if((mode == AR_UNF_CIPHER_WORK_MODE_CFB) || (mode == AR_UNF_CIPHER_WORK_MODE_OFB))
        {
            if ((width != SYMC_DAT_WIDTH_64)
                 && (width != SYMC_DAT_WIDTH_8)
                 && (width != SYMC_DAT_WIDTH_1))
            {
                AR_LOG_ERROR("Invalid mode %d and bit width %d\n",mode, width);
                AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
                return AR_ERR_CIPHER_INVALID_PARA;
            }
        }
    }

    if (enAlg == AR_UNF_CIPHER_ALG_AES)
    {
        if (mode > AR_UNF_CIPHER_WORK_MODE_BUTT)
        {
            AR_LOG_ERROR("Invalid enAlg %d and mode %d\n",enAlg, mode);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if ((mode == AR_UNF_CIPHER_WORK_MODE_CFB)
             && (width != SYMC_DAT_WIDTH_1)
             && (width != SYMC_DAT_WIDTH_8)
             && (width != SYMC_DAT_WIDTH_128))
        {
            AR_LOG_ERROR("Invalid enAlg %d mode %d and width %d\n", enAlg, mode, width);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if ((mode == AR_UNF_CIPHER_WORK_MODE_OFB)
            && (width != SYMC_DAT_WIDTH_128))
        {
            AR_LOG_ERROR("Invalid enAlg %d mode %d and width %d\n", enAlg, mode, width);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }

    if (enAlg == AR_UNF_CIPHER_ALG_SM1)
    {
        if (mode > AR_UNF_CIPHER_WORK_MODE_OFB)
        {
            AR_LOG_ERROR("Invalid enAlg %d and mode %d\n",enAlg, mode);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if ((mode == AR_UNF_CIPHER_WORK_MODE_OFB)
            && (width != SYMC_DAT_WIDTH_128))
        {
            AR_LOG_ERROR("Invalid enAlg %d mode %d and width %d\n", enAlg, mode, width);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if ((mode == AR_UNF_CIPHER_WORK_MODE_CFB)
            && (width >= SYMC_DAT_WIDTH_COUNT))
        {
            AR_LOG_ERROR("Invalid enAlg %d mode %d and width %d\n", enAlg, mode, width);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }

    if ((enAlg == AR_UNF_CIPHER_ALG_SM4)
        && (mode != AR_UNF_CIPHER_WORK_MODE_ECB)
        && (mode != AR_UNF_CIPHER_WORK_MODE_CBC)
        && (mode != AR_UNF_CIPHER_WORK_MODE_CTR))
    {
        AR_LOG_ERROR("Invalid enAlg %d and mode %d\n",enAlg, mode);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    if ((enAlg == AR_UNF_CIPHER_ALG_SM1) && (round >= AR_UNF_CIPHER_SM1_ROUND_BUTT))
    {
        AR_LOG_ERROR("Invalid enAlg %d and Sm1Round %d\n",enAlg, round);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    if (enAlg >= AR_UNF_CIPHER_ALG_BUTT)
    {
        AR_LOG_ERROR("Invalid enAlg %d .\n",enAlg);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    if (width >= SYMC_DAT_WIDTH_COUNT)
    {
        AR_LOG_ERROR("Invalid mode %d\n",width);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 kapi_symc_match_width(AR_UNF_CIPHER_WORK_MODE_E enWorkMode,
                                 AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth,
                                 symc_width *width)
{
    AR_LOG_FuncEnter();

    /* set the bit width which depend on alg and mode */
    if ((AR_UNF_CIPHER_WORK_MODE_CFB == enWorkMode)
        || (AR_UNF_CIPHER_WORK_MODE_OFB == enWorkMode))
    {
        switch(enBitWidth)
        {
            case AR_UNF_CIPHER_BIT_WIDTH_64BIT:
            {
                *width = SYMC_DAT_WIDTH_64;
                break;
            }
            case AR_UNF_CIPHER_BIT_WIDTH_8BIT:
            {
                *width = SYMC_DAT_WIDTH_8;
                break;
            }
            case AR_UNF_CIPHER_BIT_WIDTH_1BIT:
            {
                *width = SYMC_DAT_WIDTH_1;
                break;
            }
            case AR_UNF_CIPHER_BIT_WIDTH_128BIT:
            {
                *width = SYMC_DAT_WIDTH_128;
                break;
            }
            default:
            {
                AR_LOG_ERROR("Invalid width: 0x%x, mode 0x%x\n",
                    enBitWidth, enWorkMode);
                AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
                return AR_ERR_CIPHER_INVALID_PARA;
            }
        }
    }
    else
    {
        *width = SYMC_DAT_WIDTH_128;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 kapi_symc_check_param(AR_U32 hard_key, AR_U32 iv_usage,
                                 AR_UNF_CIPHER_ALG_E enAlg,
                                 AR_UNF_CIPHER_WORK_MODE_E enWorkMode,
                                 AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth,
                                 AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen,
                                 AR_UNF_CIPHER_SM1_ROUND_E sm1_round_num,
                                 symc_width *width)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    if (AR_UNF_CIPHER_ALG_DMA == enAlg)
    {
        AR_LOG_INFO("Alg is DMA.\n");
        AR_DBG_PrintU32(enAlg);
        return AR_SUCCESS;
    }

    if ((AR_TRUE != hard_key)  && (AR_FALSE != hard_key))
    {
        AR_LOG_ERROR("Invalid hard_key: 0x%x\n", hard_key);
        return AR_ERR_CIPHER_INVALID_PARA;
    }
    if (AR_UNF_CIPHER_KEY_DES_2KEY < enKeyLen)
    {
        AR_LOG_ERROR("Invalid key len: 0x%x\n", enKeyLen);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    /* set the bit width which depend on alg and mode */
    ret = kapi_symc_match_width(enWorkMode, enBitWidth, width);
    if (AR_SUCCESS != ret)
    {
        AR_ERR_PrintU32(enWorkMode);
        AR_ERR_PrintU32(enBitWidth);
        AR_ERR_PrintU32(*width);
        AR_LOG_PrintFuncErr(kapi_symc_match_width, ret);
        return ret;
    }

    ret = kapi_symc_width_check(enAlg, enWorkMode, *width, sm1_round_num);
    if (AR_SUCCESS != ret)
    {
        AR_ERR_PrintU32(enAlg);
        AR_ERR_PrintU32(enWorkMode);
        AR_ERR_PrintU32(*width);
        AR_ERR_PrintU32(sm1_round_num);
        AR_LOG_PrintFuncErr(kapi_symc_width_check, ret);
        return ret;
    }

    if (AR_CIPHER_IV_CHANGE_ALL_PKG < iv_usage)
    {
        AR_LOG_ERROR("Invalid IV Change Flags: 0x%x\n", iv_usage);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    if ((AR_CIPHER_IV_CHANGE_ALL_PKG == iv_usage)
        && ((AR_UNF_CIPHER_WORK_MODE_CCM == enWorkMode)
            || (AR_UNF_CIPHER_WORK_MODE_GCM == enWorkMode))
            )
    {
        AR_LOG_ERROR("Invalid IV Change Flags: 0x%x\n", iv_usage);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

#if 0
static AR_S32 kapi_symc_check_ccm_gcm_taglen(AR_UNF_CIPHER_ALG_E enAlg, AR_UNF_CIPHER_WORK_MODE_E enWorkMode, AR_U32 tlen)
{
    AR_LOG_CHECK_PARAM(AR_UNF_CIPHER_ALG_AES != enAlg);

    if (AR_UNF_CIPHER_WORK_MODE_CCM == enWorkMode)
    {
        /* the parameter t denotes the octet length of T(tag)
         * t is an element of  { 4, 6, 8, 10, 12, 14, 16}
         * here t is pConfig->AR_U32TagLen
         */
        if ((tlen & 0x01)
            || (AES_CCM_MIN_TAG_LEN > tlen)
            || (AES_CCM_MAX_TAG_LEN < tlen))
        {
            AR_LOG_ERROR("Invalid ccm tag len, tlen = 0x%x.\n", tlen);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }
    else if (AR_UNF_CIPHER_WORK_MODE_GCM == enWorkMode)
    {
        if ((AES_GCM_MIN_TAG_LEN > tlen) || (AES_GCM_MAX_TAG_LEN < tlen))
        {
            AR_LOG_ERROR("Invalid gcm tag len, tlen = 0x%x.\n", tlen);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }
    else
    {
            AR_LOG_ERROR("Aes with invalid work mode 0x%x for check tag lenth.\n", enWorkMode);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
    }

    return AR_SUCCESS;
}
#endif

AR_S32 kapi_symc_config(AR_U32 id,
                    AR_U32 hard_key,
                    AR_UNF_CIPHER_ALG_E enAlg,
                    AR_UNF_CIPHER_WORK_MODE_E enWorkMode,
                    AR_UNF_CIPHER_BIT_WIDTH_E enBitWidth,
                    AR_UNF_CIPHER_KEY_LENGTH_E enKeyLen,
                    AR_UNF_CIPHER_SM1_ROUND_E sm1_round_num,
                    AR_U8 *fkey, AR_U8 *skey,
                    AR_U8 *iv, AR_U32 ivlen, AR_U32 iv_usage,
                    compat_addr aad, AR_U32 alen, AR_U32 tlen)
{
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    symc_width width = SYMC_DAT_WIDTH_COUNT;
    AR_U32 softId = 0;
    AR_U32 byca = AR_FALSE;
    AR_U32 catype = 0;
    AR_U32 klen = enKeyLen;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == fkey);
    AR_LOG_CHECK_PARAM(MAX_AEAD_A_LEN < alen);
    AR_LOG_CHECK_PARAM(ADDR_L32(aad) + alen < alen);

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);

    /***
    hard_key: bit[0~7]  flag of hard key or not
              bit[8~31] ca type
    */
    byca = hard_key & 0xFF;
    catype = hard_key >> BITS_IN_BYTE;

    ret = kapi_symc_check_param(byca, iv_usage, enAlg, enWorkMode,
        enBitWidth, enKeyLen, sm1_round_num, &width);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_ERROR("hard_key 0x%x\n", hard_key);
        AR_LOG_PrintFuncErr(kapi_symc_check_param, ret);
        return ret;
    }

    KAPI_SYMC_LOCK();

    /* Destroy the last attached instance of Symmetric cipher engine */
    if ((AR_NULL != ctx->func) && (AR_NULL != ctx->func->destroy))
    {
        (void)ctx->func->destroy(ctx->cryp_ctx);
    }
    ctx->cryp_ctx = AR_NULL;

    /* Clone the function from template of symc engine*/
    ctx->func = cryp_get_symc_op(enAlg, enWorkMode);

    if (AR_NULL == ctx->func)
    {
        AR_LOG_ERROR("error, get symc function failed, enAlg %d, enWorkMode %d\n",
            enAlg, enWorkMode);
        AR_LOG_PrintFuncErr(cryp_get_symc_op, ret);
        KAPI_SYMC_UNLOCK();
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    /* null means can ignore the function */
    if (ctx->func->create)
    {
        /* Create a instance from template of engine */
        ctx->cryp_ctx = ctx->func->create(softId);
        if (AR_NULL == ctx->cryp_ctx)
        {
            AR_LOG_ERROR("attach contxet buffer to softId %d failed\n", softId);
            AR_LOG_PrintFuncErr(cryp_symc_create, ret);
            goto exit__;
        }
    }

    /* set mode and alg */
    if (ctx->func->setmode)
    {
        ctx->func->setmode(ctx->cryp_ctx, ctx->func->alg, ctx->func->mode, width);
    }

    /* Set even key, may be also need set odd key */
    if (ctx->func->setkey)
    {
        if (AR_TRUE == byca)
        {
            CHECK_EXIT(ctx->func->setkey(ctx->cryp_ctx, AR_NULL, AR_NULL, &klen));

            if(enKeyLen == AR_UNF_CIPHER_KEY_AES_192BIT)
            {
                klen = AES_KEY_256BIT;
            }
#if 0 //xfqu
            CHECK_EXIT(klad_load_hard_key(id, catype, fkey, klen));
#endif
        }
        else
        {
            CHECK_EXIT(ctx->func->setkey(ctx->cryp_ctx, fkey, skey, &klen));
        }
    }

    /* Set IV */
    if (ctx->func->setiv)
    {
        CHECK_EXIT(ctx->func->setiv(ctx->cryp_ctx, iv, ivlen, iv_usage));
    }

    /* set sm1 round num */
    if (ctx->func->setround)
    {
        CHECK_EXIT(ctx->func->setround(ctx->cryp_ctx, sm1_round_num));
    }
#if 0 //xfqu
    /* Set AAD */
    if (ctx->func->setadd)
    {
        ret = cipher_check_mmz_phy_addr(ADDR_U64(aad), alen);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("Invalid aad mmz phy addr.\n");
            AR_LOG_PrintFuncErr(cipher_check_mmz_phy_addr, ret);
            goto exit__;
        }

        AR_LOG_INFO("set add, phy 0x%x, alen %d, tlen %d\n", ADDR_L32(aad), alen, tlen);
        CHECK_EXIT(kapi_symc_check_ccm_gcm_taglen(enAlg, enWorkMode, tlen));
        CHECK_EXIT(ctx->func->setadd(ctx->cryp_ctx, aad, alen, tlen));
    }
#endif
    /* save crtl */
    crypto_memset(&ctx->ctrl, sizeof(AR_UNF_CIPHER_CTRL_S), 0, sizeof(AR_UNF_CIPHER_CTRL_S));
    ctx->ctrl.bKeyByCA = byca;
    ctx->ctrl.enAlg = enAlg;
    ctx->ctrl.enBitWidth = enBitWidth;
    ctx->ctrl.enCaType = catype;
    ctx->ctrl.enKeyLen = enKeyLen;
    ctx->ctrl.enWorkMode = enWorkMode;
    ctx->ctrl.stChangeFlags.bit1IV = iv_usage;

    if (AR_NULL != iv)
    {
        if (AES_IV_SIZE < ivlen)
        {
            AR_LOG_ERROR("Invalid iv len.\n");
            ret = AR_ERR_CIPHER_INVALID_PARA;
            goto exit__;
        }

        crypto_memcpy(ctx->ctrl.u32IV, AES_IV_SIZE, iv, ivlen);
    }
    if (AR_NULL != fkey)
    {
        if (AES_KEY_256BIT < klen)
        {
            AR_LOG_ERROR("Invalid key len.\n");
            ret = AR_ERR_CIPHER_INVALID_PARA;
            goto exit__;
        }

        crypto_memcpy(ctx->ctrl.u32Key, AES_KEY_256BIT, fkey, klen);
    }

    ctx->config = AR_TRUE;

    KAPI_SYMC_UNLOCK();
    AR_LOG_FuncExit();
    return AR_SUCCESS;

exit__:
    KAPI_SYMC_UNLOCK();

    return ret;
}

AR_S32 kapi_symc_get_config(AR_U32 id, AR_UNF_CIPHER_CTRL_S *ctrl)
{
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    AR_U32 softId = 0;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == ctrl);

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);
    AR_LOG_CHECK_PARAM(AR_TRUE != ctx->config);

    KAPI_SYMC_LOCK();

    crypto_memcpy(ctrl, sizeof(AR_UNF_CIPHER_CTRL_S), &ctx->ctrl, sizeof(AR_UNF_CIPHER_CTRL_S));

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();

    return ret;
}

AR_S32 kapi_symc_crypto(AR_U32 id, compat_addr input,
                     compat_addr output, AR_U32 length,
                     AR_U32 operation, AR_U32 last)
{
    AR_S32 ret = AR_FAILURE;
    symc_node_usage usage;
    kapi_symc_ctx *ctx = AR_NULL;
    AR_U32 softId = 0;

    AR_LOG_FuncEnter();

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);
    AR_LOG_CHECK_PARAM(ADDR_U64(input) + length < length);
    AR_LOG_CHECK_PARAM(ADDR_U64(output) + length < length);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func->crypto);
    AR_LOG_CHECK_PARAM(AR_TRUE != ctx->config);
    AR_LOG_CHECK_PARAM((0x00 != operation)&&(0x01 != operation));

    AR_LOG_INFO("src/dest phyaddr information.\n");
    AR_DBG_PrintU32(operation);
    AR_DBG_PrintH32(ADDR_L32(input));
    AR_DBG_PrintH32(ADDR_L32(output));
    AR_DBG_PrintH32(length);

    usage = SYMC_NODE_USAGE_NORMAL;

    KAPI_SYMC_LOCK();

    ret = ctx->func->crypto(ctx->cryp_ctx, operation, &input,
            &output, &length, &usage, 1, AR_TRUE);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(ctx->func->crypto, ret);
        KAPI_SYMC_UNLOCK();
        return ret;
    }

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_symc_crypto_via(AR_U32 id, compat_addr input,
                     compat_addr output, AR_U32 length,
                     AR_U32 operation, AR_U32 last, AR_U32 is_from_user)
{
    AR_S32 ret = AR_FAILURE;
#if 0
    AR_S32 ret_exit = AR_FAILURE;

    crypto_mem mem = {0};

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == ADDR_VIA(input));
    AR_LOG_CHECK_PARAM(AR_NULL == ADDR_VIA(output));
    AR_LOG_CHECK_PARAM(0x00 == length);

    ret = crypto_mem_create(&mem, SEC_MMZ, "AES_IN", length);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(crypto_mem_create, ret);
        return ret;
    }

    if(AR_TRUE == is_from_user)
    {
        ret = crypto_copy_from_user(mem.dma_virt, ADDR_VIA(input), length);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(crypto_copy_from_user, ret);
            goto exit;
        }
    }
    else
    {
        crypto_memcpy(mem.dma_virt, length, ADDR_VIA(input), length);
    }

    ret = kapi_symc_crypto(id, mem.dma_addr, mem.dma_addr, length, operation & 0x01, last);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_crypto, ret);
        goto exit;
    }

    if(AR_TRUE == is_from_user)
    {
        ret = crypto_copy_to_user(ADDR_VIA(output), mem.dma_virt, length);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(crypto_copy_to_user, ret);
            goto exit;
        }
    }
    else
    {
       crypto_memcpy(ADDR_VIA(output), length, mem.dma_virt, length);
    }

exit:
    ret_exit = crypto_mem_destory(&mem);
    if (AR_SUCCESS != ret_exit)
    {
        AR_LOG_PrintFuncErr(crypto_mem_destory, ret_exit);
        AR_ERR_PrintS32(ret);
        return ret_exit;
    }
#endif
    AR_LOG_FuncExit();
    return ret;
}

static AR_S32 kapi_symc_crypto_multi_start(kapi_symc_ctx *ctx, void *pkg, AR_U32 pkg_num, AR_U32 operation, AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;
    void *buf = AR_NULL, *temp = AR_NULL;
    compat_addr *input = AR_NULL;
    compat_addr *output = AR_NULL;
    symc_node_usage *usage = AR_NULL;
    AR_UNF_CIPHER_DATA_S pkg_tmp;
    AR_U32 *length = AR_NULL;
    AR_U32 size = 0;
    AR_U32 i;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func->crypto);
    AR_LOG_CHECK_PARAM(AR_NULL == pkg);
    AR_LOG_CHECK_PARAM(MAX_PKG_NUMBER < pkg_num);
    AR_LOG_CHECK_PARAM(0x00 == pkg_num);

    /* size of input:output:usage:length */
    size = (sizeof(compat_addr) + sizeof(compat_addr) + sizeof(AR_U32) + sizeof(AR_U32)) * pkg_num;

    buf = crypto_malloc(size);
    if (AR_NULL == buf)
    {
        AR_LOG_ERROR("Malloc for pkg failed.\n");
        AR_LOG_PrintFuncErr(crypto_malloc, ret);
        return AR_ERR_CIPHER_FAILED_MEM;
    }

    temp = buf;
    input = (compat_addr *)temp;
    temp = (AR_U8*)temp + sizeof(compat_addr) * pkg_num; /*buf + input*/
    output = (compat_addr *)temp;
    temp = (AR_U8*)temp + sizeof(compat_addr) * pkg_num; /*buf + input + output*/
    usage = temp;
    temp = (AR_U8*)temp + sizeof(AR_U32) * pkg_num; /*buf + input + output + usage*/
    length = temp;

    /*Compute and check the nodes length*/
    for (i=0; i<pkg_num; i++)
    {
        /*copy node list from user space to kernel*/
        ret = crypto_copy_from_user(&pkg_tmp, (AR_U8*)pkg + sizeof(AR_UNF_CIPHER_DATA_S) * i,
                sizeof(AR_UNF_CIPHER_DATA_S));
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("copy data from user fail!\n");
            AR_LOG_PrintFuncErr(crypto_copy_from_user, ret);
            crypto_free(buf);
            buf = AR_NULL;
            return ret;
        }

        if (pkg_tmp.szSrcPhyAddr + pkg_tmp.u32ByteLength < pkg_tmp.u32ByteLength)
        {
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            crypto_free(buf);
            buf = AR_NULL;
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if (pkg_tmp.szDestPhyAddr + pkg_tmp.u32ByteLength < pkg_tmp.u32ByteLength)
        {
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            crypto_free(buf);
            buf = AR_NULL;
            return AR_ERR_CIPHER_INVALID_PARA;
        }

        if ((AR_TRUE != pkg_tmp.bOddKey) && (AR_FALSE != pkg_tmp.bOddKey))
        {
            AR_LOG_ERROR("invalid odd key for multicipher crypt!\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            crypto_free(buf);
            buf = AR_NULL;
            return AR_ERR_CIPHER_INVALID_PARA;
        }
#if 0 //xfqu
        ret = cipher_check_mmz_phy_addr(pkg_tmp.szSrcPhyAddr, pkg_tmp.AR_U32ByteLength);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("Invalid output mmz phy addr for multicipher crypt.\n");
            AR_LOG_PrintFuncErr(cipher_check_mmz_phy_addr, ret);
            crypto_free(buf);
            buf = AR_NULL;
            return ret;
        }

        ret = cipher_check_mmz_phy_addr(pkg_tmp.szDestPhyAddr, pkg_tmp.AR_U32ByteLength);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_ERROR("Invalid output mmz phy addr for multicipher crypt.\n");
            AR_LOG_PrintFuncErr(cipher_check_mmz_phy_addr, ret);
            crypto_free(buf);
            buf = AR_NULL;
            return ret;
        }
#endif
        ADDR_U64(input[i]) = pkg_tmp.szSrcPhyAddr;
        ADDR_U64(output[i]) = pkg_tmp.szDestPhyAddr;
        length[i] = pkg_tmp.u32ByteLength;
        usage[i] = SYMC_NODE_USAGE_EVEN_KEY;

        AR_LOG_DEBUG("pkg %d, in 0x%x, out 0x%x, length 0x%x, usage 0x%x\n", i,
            ADDR_L32(input[i]), ADDR_L32(output[i]), length[i], usage[i]);
    }

    ret = ctx->func->crypto(ctx->cryp_ctx, operation, input,
            output, length, usage, pkg_num, wait);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_crypto, ret);
        crypto_free(buf);
        buf = AR_NULL;
        return ret;
    }

    crypto_free(buf);
    buf = AR_NULL;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_symc_crypto_multi(AR_U32 id, void *pkg, AR_U32 pkg_num, AR_U32 operation, AR_U32 last)
{
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    AR_U32 softId = 0;

    AR_LOG_FuncEnter();

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func);
    AR_LOG_CHECK_PARAM(AR_TRUE != ctx->config);
    AR_LOG_CHECK_PARAM((0x00 != operation) && (0x01 != operation));

    KAPI_SYMC_LOCK();

    ret = kapi_symc_crypto_multi_start(ctx, pkg, pkg_num, operation, AR_TRUE);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(kapi_symc_crypto_multi_start, ret);
        KAPI_SYMC_UNLOCK();
        return ret;
    }

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

AR_S32 kapi_aead_get_tag(AR_U32 id, AR_U32 tag[AEAD_TAG_SIZE_IN_WORD], AR_U32 *taglen)
{
    AR_S32 ret = AR_FAILURE;
    kapi_symc_ctx *ctx = AR_NULL;
    AR_U32 softId = 0;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == tag);
    AR_LOG_CHECK_PARAM(AR_NULL == taglen);
    AR_LOG_CHECK_PARAM(AES_CCM_MAX_TAG_LEN != *taglen);

    KAPI_SYMC_CHECK_HANDLE(id);
    softId = AR_HANDLE_GET_CHNID(id);
    ctx = &kapi_ctx[softId];
    CHECK_OWNER(&ctx->owner);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func);
    AR_LOG_CHECK_PARAM(AR_NULL == ctx->func->gettag);

    KAPI_SYMC_LOCK();

    if (ctx->func->gettag)
    {
        ret = ctx->func->gettag(ctx->cryp_ctx, tag, taglen);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_aead_get_tag, ret);
            KAPI_SYMC_UNLOCK();
            return ret;
        }
    }

    KAPI_SYMC_UNLOCK();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

/** @}*/  /** <!-- ==== Structure Definition end ====*/
