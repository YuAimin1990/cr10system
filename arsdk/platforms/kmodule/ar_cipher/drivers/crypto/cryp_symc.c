#include "drv_osal_lib.h"
#include "drv_symc.h"
#include "cryp_symc.h"
//#include "ext_alg.h"

/*************************** Internal Structure Definition *******************/
/** \addtogroup      aes */
/** @{*/  /** <!-- [aes]*/

/* AES set IV for first package */
#define SYMC_IV_USAGE_ONE_PKG        (1)

/* AES set IV for first package */
#define SYMC_IV_USAGE_ALL_PKG        (2)

/* SYMC interrupt level */
#define SYMC_INT_LEVEL              (SYMC_MAX_LIST_NUM - 15) //(1UL)

/* Length of SYMC ccm q  */
#define SYMC_CCM_Q_LEN_2B        (2)
#define SYMC_CCM_Q_LEN_3B        (3)
#define SYMC_CCM_Q_LEN_4B        (4)

/* Length of SYMC ccm P  */
#define SYMC_CCM_P_LEN_2B        (0xFFFF)
#define SYMC_CCM_P_LEN_3B        (0xFFFFFF)

/* length range of aead */
#define AES_CCM_MIN_IV_LEN      (7)
#define AES_CCM_MAX_IV_LEN      (13)
#define AES_CCM_NQ_LEN          (14)
#define AES_CCM_MIN_TAG_LEN     (4)
#define AES_CCM_MAX_TAG_LEN     (16)
#define AES_GCM_MIN_IV_LEN      (1)
#define AES_GCM_MAX_IV_LEN      (16)
#define AES_GCM_MIN_TAG_LEN     (1)
#define AES_GCM_MAX_TAG_LEN     (16)

/* Multi nodes added status, finished or finished */
#define SYMC_NODES_ADD_FINISHED    (0x0a0a0a0a)
#define SYMC_NODES_ADD_NOTFINISHED (0X05050505)

/**
 * \brief          symc context structure
 *
 * \note           if the aes key derived from klad, the context msut
 *                 attached with a independent hard key channel,
 *                 otherwise the context can attached with a fixed common channel.
 */
typedef struct
{
    AR_U32 even_key[SYMC_KEY_SIZE/4]; /*!<  SYMC even round keys, default */
    AR_U32 odd_key[SYMC_KEY_SIZE/4];  /*!<  SYMC odd round keys, default */
    AR_U32 sk[SYMC_SM1_SK_SIZE/4];    /*!<  sm1 sk */
    AR_U32 iv[AES_IV_SIZE/4];   /*!<  symc IV */
    AR_U32 tag[AEAD_TAG_SIZE/4];   /*!<  aead tag */
    AR_U32 ivlen;               /*!<  symc IV length */
    AR_U32 iv_usage;            /*!<  symc IV usage */
    AR_U32 oddkey_usage;            /*!<  symc IV usage */

    AR_U32 hard_chn;            /*!<  hard channel number */
    AR_U32 hard_key;            /*!<  Key derived from klad or CPU */

    symc_alg alg;            /*!<  Symmetric cipher algorithm */
    symc_width width;        /*!<  Symmetric cipher width */
    AR_U32 klen;                /*!<  Symmetric cipher key length */

    compat_addr aad;         /*!<  Associated Data */
    AR_U32 alen;                /*!<  Associated Data length */
    AR_U32 tlen;                /*!<  Tag length */

    symc_mode mode;          /*!<  Symmetric cipher mode */

    AR_U32 sm1_round;           /*!<  SM1 round number */
    AR_U32 enclen;              /*!<  encrypt length */

    AR_U32 block_size;          /*!<  Block size */

    AR_U32 cur_nodes;           /*!<  current nodes id  */
    AR_U32 total_nodes;         /*!<  total number of nodes */

    compat_addr *input_list; /*!<  input node list */
    compat_addr *output_list;/*!<  output node list */
    AR_U32 *length_list;        /*!<  length of node list */
    symc_node_usage *usage_list; /*!< usage of node list */
    AR_BOOL tdes2dma;           /*!<  3des with invalid key turns to dma */
}
cryp_symc_context;

/* The max tab size of symc function */
#define SYMC_FUNC_TAB_SIZE          (SYMC_ALG_COUNT * SYMC_MODE_COUNT)

/* symc function list */
static symc_func symc_descriptor[SYMC_FUNC_TAB_SIZE];

/* symc context */
static cryp_symc_context symc_context[CRYPTO_HARD_CHANNEL_MAX];

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers*/
/** @{*/  /** <!-- [cipher]*/

/* symc function register */
static void cryp_register_all_symc(void);

#ifdef CHIP_AES_CCM_GCM_SUPPORT
static AR_U32 cyp_aead_gcm_clen(AR_U8 *buf, AR_U32 alen, AR_U32 enclen);
#endif

AR_S32 cryp_symc_init(void)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    crypto_memset(symc_descriptor, sizeof(symc_descriptor), 0, sizeof(symc_descriptor));

    ret = drv_symc_init();
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_init, ret);
        return ret;
    }

    cryp_register_all_symc();

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

void cryp_symc_deinit(void)
{
    AR_LOG_FuncEnter();

    drv_symc_deinit();

    AR_LOG_FuncExit();
    return;
}

AR_S32 cryp_symc_alloc_chn(AR_U32 *hard_chn)
{
    AR_S32 ret = AR_FAILURE;
    //AR_U32 key[SYMC_KEY_MAX_SIZE_IN_WORD] = {0, 1, 2, 3, 4, 5, 6, 7};
    //AR_U32 sm1_key[SYMC_SM1_SK_SIZE_IN_WORD] = {0, 1, 2, 3};

    AR_LOG_FuncEnter();

    /* allocate a aes hard key channel */
    ret = drv_symc_alloc_chn(hard_chn);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_alloc_chn, ret);
        return ret;
    }

    /* Set a fake key to clear the true key. */
    //drv_symc_set_key(*hard_chn, key, AR_FALSE);
    //drv_symc_set_sm1_sk(*hard_chn, sm1_key);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

void cryp_symc_free_chn(AR_U32 hard_chn)
{
    AR_LOG_FuncEnter();

    drv_symc_free_chn(hard_chn);

    AR_LOG_FuncExit();
}

static void * cryp_symc_create(AR_U32 hard_chn)
{
    cryp_symc_context *hisi_ctx = AR_NULL;

    AR_LOG_FuncEnter();

    hisi_ctx = &symc_context[hard_chn];
    crypto_memset(hisi_ctx, sizeof(cryp_symc_context), 0, sizeof(cryp_symc_context));
    hisi_ctx->hard_key = AR_FALSE;
    hisi_ctx->hard_chn = hard_chn;

    AR_LOG_FuncExit();
    return hisi_ctx;
}

static void cryp_symc_setkey(void *ctx, const AR_U8 *key, AR_U32 keylen, AR_U32 odd)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if (AR_NULL == hisi_ctx)
    {
        AR_LOG_ERROR("Invalid point.\n");
        return;
    }

    if (odd)
    {
        crypto_memcpy(hisi_ctx->odd_key, SYMC_KEY_SIZE, key, keylen);
		hisi_ctx->oddkey_usage = AR_TRUE;
    }
    else
    {
        crypto_memcpy(hisi_ctx->even_key, SYMC_KEY_SIZE, key, keylen);
    }
    hisi_ctx->klen = keylen;

    AR_LOG_FuncExit();
    return;
}

static void cryp_symc_setiv(void *ctx, const AR_U8 *iv, AR_U32 ivlen, AR_U32 usage)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if (AR_NULL == hisi_ctx)
    {
        AR_LOG_ERROR("Invalid point.\n");
        return;
    }
    if ((AR_NULL == iv) && (0 != ivlen))
    {
        AR_LOG_ERROR("Invalid iv.\n");
        return;
    }
    crypto_memcpy(hisi_ctx->iv, AES_IV_SIZE, iv, ivlen);
    hisi_ctx->iv_usage = usage;
    hisi_ctx->ivlen = ivlen;

    AR_LOG_FuncExit();
    return;
}

static void cryp_symc_getiv(void *ctx, AR_U8 *iv, AR_U32 *ivlen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if (AR_NULL == hisi_ctx)
    {
        AR_LOG_ERROR("Invalid point.\n");
        return;
    }
    crypto_memcpy(iv, AES_IV_SIZE, hisi_ctx->iv, hisi_ctx->ivlen);
    *ivlen = hisi_ctx->ivlen;

    AR_LOG_FuncExit();
    return;
}

static void cryp_symc_setmode(void *ctx, symc_alg alg, symc_mode mode, symc_width width)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if (AR_NULL == hisi_ctx)
    {
        AR_LOG_ERROR("Invalid point.\n");
        return;
    }
    hisi_ctx->mode = mode;
    hisi_ctx->alg = alg;
    hisi_ctx->width = width;

    AR_LOG_FuncExit();
    return;
}

static void cryp_3des2dma_setmode(void *ctx, symc_alg alg, symc_mode mode, symc_width width)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if (AR_NULL == hisi_ctx)
    {
        AR_LOG_ERROR("Invalid point.\n");
        return;
    }

    CRYPTO_UNUSED(alg);
    CRYPTO_UNUSED(mode);
    CRYPTO_UNUSED(width);

    hisi_ctx->mode = SYMC_MODE_ECB;
    hisi_ctx->alg = SYMC_ALG_TDES;
    hisi_ctx->width = SYMC_DAT_WIDTH_64;

    AR_LOG_FuncExit();
    return;
}

static AR_S32 cryp_symc_sm1_setsk(void *ctx, const AR_U8 *key)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    crypto_memcpy(hisi_ctx->sk, SYMC_SM1_SK_SIZE, key, SYMC_SM1_SK_SIZE);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_sm1_setround(void *ctx, AR_U32 round)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    hisi_ctx->sm1_round = round;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 symc_add_buf(cryp_symc_context *ctx, symc_node_usage out_uasge)
{
    AR_S32 ret = AR_FAILURE;
    AR_U32 cur = ctx->cur_nodes;

    AR_LOG_FuncEnter();

    /*Add P in*/
    ret = drv_symc_add_inbuf(ctx->hard_chn,
                             ctx->input_list[cur],
                             ctx->length_list[cur],
                             ctx->usage_list[cur]);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_add_inbuf, ret);
        return ret;
    }

    /*Add P out, only need the last flag*/
    ret = drv_symc_add_outbuf(ctx->hard_chn,
                              ctx->output_list[cur],
                              ctx->length_list[cur],
                              out_uasge);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_add_outbuf, ret);
        return ret;
    }

    AR_LOG_FuncExit();

    return ret;
}

static symc_node_usage symc_get_out_usage(symc_mode mode, AR_U32 cur_node, AR_U32 total_node)
{
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    if (SYMC_MODE_GCM != mode && (cur_node + 1) == total_node)
    {
        usage = SYMC_NODE_USAGE_LAST;
    }

    return usage;
}

static AR_S32 symc_add_buf_list(void *ctx)
{
    AR_S32 ret = AR_FAILURE;
    AR_U32 i = 0;
    AR_U32 nodes = 0;
    AR_U32 cur = 0;
    AR_U32 total_len = 0;
    cryp_symc_context *hisi_ctx = ctx;
    symc_node_usage usage = SYMC_NODE_USAGE_NORMAL;

    AR_LOG_FuncEnter();

    /* compute finished*/
    if(hisi_ctx->cur_nodes == hisi_ctx->total_nodes)
    {
        AR_LOG_FuncExit();
        return SYMC_NODES_ADD_FINISHED;
    }

    /* compute not finished*/
    /* select the minimum numbers of nodes to calculate*/
    nodes = MIN(SYMC_INT_LEVEL, hisi_ctx->total_nodes - hisi_ctx->cur_nodes);
    total_len = 0;

    for (i = 0; i < nodes; i++)
    {
        cur = hisi_ctx->cur_nodes;
        usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);

        /*Add one node*/
        ret = symc_add_buf(hisi_ctx,usage);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(symc_add_buf, ret);
            return ret;
        }

        total_len += hisi_ctx->length_list[cur];
        hisi_ctx->cur_nodes++;
    }

   /* For each compute, the total length of valid nodes list
    * must aligned with block size, otherwise can't recv interrupt,
    * which limit to hardware devising.
    */

    /* Compute the tail length*/
    total_len %= hisi_ctx->block_size;
    if (total_len > 0)
    {
        total_len = hisi_ctx->block_size - total_len;
    }

    /*if the total length don't aligned with block size, split joint the follow nodes*/
    while((total_len > 0) && (hisi_ctx->cur_nodes < hisi_ctx->total_nodes))
    {
        cur = hisi_ctx->cur_nodes;

        /*The next node large than tail size, just split it to 2 nodes */
        if (hisi_ctx->length_list[cur] > total_len)
        {
            /*Add P in*/
            ret = drv_symc_add_inbuf(hisi_ctx->hard_chn, hisi_ctx->input_list[cur],
                total_len, hisi_ctx->usage_list[cur]);
            if(AR_SUCCESS != ret)
            {
                AR_LOG_PrintFuncErr(drv_symc_add_inbuf, ret);
                return ret;
            }

            /*Add P out*/
            usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);
            ret = drv_symc_add_outbuf(hisi_ctx->hard_chn, hisi_ctx->output_list[cur],
                total_len, usage);
            if(AR_SUCCESS != ret)
            {
                AR_LOG_PrintFuncErr(drv_symc_add_outbuf, ret);
                return ret;
            }

            /*Let next node skip the tail size*/
            ADDR_U64(hisi_ctx->input_list[cur]) += total_len;
            ADDR_U64(hisi_ctx->output_list[cur]) += total_len;
            hisi_ctx->length_list[cur] -= total_len;
            total_len = 0;
        }
        else
        {
            /*The next node less than tail size, add it to nodes list */

            /*Add one node*/
            usage = symc_get_out_usage(hisi_ctx->mode, cur, hisi_ctx->total_nodes);
            ret = symc_add_buf(hisi_ctx, usage);
            if(AR_SUCCESS != ret)
            {
                AR_LOG_PrintFuncErr(symc_add_buf, ret);
                return ret;
            }

            /*re-compute the tail size*/
            total_len -= hisi_ctx->length_list[cur];

            /*Process next node*/
            hisi_ctx->cur_nodes++;
        }
    }
#ifdef CHIP_AES_CCM_GCM_SUPPORT
     /* gcm add nodes finished ? */
    if ((SYMC_MODE_GCM == hisi_ctx->mode)
        && (hisi_ctx->cur_nodes == hisi_ctx->total_nodes))
    {
         u8 clen[AES_BLOCK_SIZE];

         /* At the and of GCM, must add a empty node to nodes list,
         * limit to hardware devising
         */
        drv_symc_add_outbuf(hisi_ctx->hard_chn, ADDR_NULL, 0x00, SYMC_NODE_USAGE_LAST);
        /*Format the length fields of C and add to nodes list*/
        cyp_aead_gcm_clen(clen, hisi_ctx->alen, hisi_ctx->enclen);
        drv_aead_gcm_add_clen(hisi_ctx->hard_chn, clen);
    }
#endif

    AR_LOG_FuncExit();
    return SYMC_NODES_ADD_NOTFINISHED;

}

static symc_klen cryp_symc_key_type(symc_alg alg, AR_U32 klen)
{
    symc_klen type;

    if ((SYMC_ALG_AES == alg)
        && (AES_KEY_192BIT == klen))
    {
        type = SYMC_KEY_AES_192BIT;
    }
    else if (SYMC_ALG_AES == alg
        && klen == AES_KEY_256BIT)
    {
        type = SYMC_KEY_AES_256BIT;
    }
    else if (SYMC_ALG_AES == alg
        && klen == TDES_KEY_128BIT)
    {
        type = SYMC_KEY_TDES_2KEY;
    }
    else if (SYMC_ALG_TDES == alg
        && klen == TDES_KEY_128BIT)
    {
        type = SYMC_KEY_TDES_2KEY;
    }
    else
    {
        type = SYMC_KEY_DEFAULT;
    }

    return type;
}

static AR_S32 cryp_symc_config(void *ctx, AR_U32 decrypt)
{
    cryp_symc_context *hisi_ctx = ctx;
    symc_klen type;
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    AR_LOG_INFO("symc configure, chn %d, alg %d, mode %d, dec %d, klen %d, hard %d, iv len %d, iv usage %d\n",
        hisi_ctx->hard_chn, hisi_ctx->alg, hisi_ctx->mode,
        decrypt, hisi_ctx->klen, hisi_ctx->hard_key,
        hisi_ctx->ivlen, hisi_ctx->iv_usage);

    type = cryp_symc_key_type(hisi_ctx->alg, hisi_ctx->klen);

    /* configure */
    ret = drv_symc_config(hisi_ctx->hard_chn, hisi_ctx->alg, hisi_ctx->mode, hisi_ctx->width,
        decrypt, hisi_ctx->sm1_round, type, hisi_ctx->hard_key);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_config, ret);
        return ret;
    }

    if(AR_TRUE != hisi_ctx->hard_key)
    {
        /* set odd key */
        if(hisi_ctx->oddkey_usage)
        {
            drv_symc_set_key(hisi_ctx->hard_chn, hisi_ctx->odd_key, AR_TRUE);
        }

        /* set even key */
        drv_symc_set_key(hisi_ctx->hard_chn, hisi_ctx->even_key, AR_FALSE);
    }
    else
    {
        /* set odd key */
        if(hisi_ctx->oddkey_usage)
        {
            drv_symc_set_key(hisi_ctx->hard_chn, NULL, AR_TRUE);
        }

        /* set even key */
        drv_symc_set_key(hisi_ctx->hard_chn, NULL, AR_FALSE);
    }

    if (SYMC_ALG_SM1 == hisi_ctx->alg)
    {
        drv_symc_set_sm1_sk(hisi_ctx->hard_chn, hisi_ctx->sk);
    }

    /* set iv */
    ret = drv_symc_set_iv(hisi_ctx->hard_chn, hisi_ctx->iv, hisi_ctx->ivlen, hisi_ctx->iv_usage);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_set_iv, ret);
        return ret;
    }

    /*first node must set iv except ecb mode*/
    if (AR_CIPHER_IV_CHANGE_ONE_PKG == hisi_ctx->iv_usage)
    {
        /* don't set iv any more*/
        hisi_ctx->iv_usage = 0;
    }

    if (SYMC_ALG_DES == hisi_ctx->alg)
    {
        hisi_ctx->block_size = DES_BLOCK_SIZE;
    }
    else if (SYMC_ALG_TDES == hisi_ctx->alg)
    {
        hisi_ctx->block_size = DES_BLOCK_SIZE;
    }
    else
    {
        hisi_ctx->block_size = AES_BLOCK_SIZE;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_wait_done(void *ctx, AR_U32 timeout)
{
    AR_S32 ret = AR_FAILURE;
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    /* wait done */
    ret = drv_symc_wait_done(hisi_ctx->hard_chn, timeout);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_wait_done, ret);
        return ret;
    }

    drv_symc_get_iv(hisi_ctx->hard_chn, hisi_ctx->iv);

    if (SYMC_MODE_CCM == hisi_ctx->mode
        || SYMC_MODE_GCM == hisi_ctx->mode)
    {
        ret = drv_aead_get_tag(hisi_ctx->hard_chn, hisi_ctx->tag);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_aead_get_tag, ret);
            return ret;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_crypto_init(cryp_symc_context *hisi_ctx,
                    AR_U32 operation,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num, symc_node_usage usage)
{
    AR_U32 i = 0;
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    /* length of pkage can't be zero */
    hisi_ctx->enclen = 0;
    if (0x01 == pkg_num)
    {
        hisi_ctx->enclen += length[i];
        usage_list[i] |= usage;
    }
    else
    {
        for (i=0; i<pkg_num; i++)
        {
            if (0x00 == length[i])
            {
                AR_LOG_PrintErrCode(AR_ERR_CIPHER_OVERFLOW);
                return AR_ERR_CIPHER_INVALID_LENGTH;
            }

            AR_LOG_CHECK_PARAM(hisi_ctx->enclen + length[i] < hisi_ctx->enclen);

            hisi_ctx->enclen += length[i];
            usage_list[i] |= usage;
        }
    }

    /* configuration parameter */
    ret = cryp_symc_config(hisi_ctx, operation);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_config, ret);
        return ret;
    }

    hisi_ctx->input_list = input;
    hisi_ctx->output_list = output;
    hisi_ctx->length_list = length;
    hisi_ctx->usage_list = usage_list;
    hisi_ctx->total_nodes = pkg_num;
    hisi_ctx->cur_nodes = 0;

    /* set isr callback function */
    ret = drv_symc_set_isr_callback(hisi_ctx->hard_chn, AR_NULL, AR_NULL);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_set_isr_callback, ret);
        return ret;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_crypto_process(cryp_symc_context *hisi_ctx, AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    if (AR_TRUE == wait)
    {
        while (SYMC_NODES_ADD_NOTFINISHED == symc_add_buf_list(hisi_ctx))
        {
            /* start running */
            drv_symc_start(hisi_ctx->hard_chn);

            /* wait done */
            ret = drv_symc_wait_done(hisi_ctx->hard_chn, CRYPTO_TIME_OUT);
            if(AR_SUCCESS != ret)
            {
                AR_LOG_PrintFuncErr(drv_symc_wait_done, ret);
                return ret;
            }
        }
    }
    else
    {
        /* add buf list once */
        ret = symc_add_buf_list(hisi_ctx);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(symc_add_buf_list, ret);
            return ret;
        }

        /* set isr callback function */
        ret = drv_symc_set_isr_callback(hisi_ctx->hard_chn, symc_add_buf_list, hisi_ctx);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_set_isr_callback, ret);
            return ret;
        }

        /* start running */
        ret = drv_symc_start(hisi_ctx->hard_chn);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_start, ret);
            return ret;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_crypto(void *ctx,
                    AR_U32 operation,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num,
                    AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    if (SYMC_ALG_NULL_CIPHER == hisi_ctx->alg)
    {
        /* set last flag for each node when DMA copy */
        hisi_ctx->iv_usage = AR_CIPHER_IV_CHANGE_ALL_PKG;
    }

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
        usage_list, pkg_num, SYMC_NODE_USAGE_NORMAL);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_crypto_init, ret);
        return ret;
    }
    usage_list[pkg_num - 1] |= SYMC_NODE_USAGE_LAST;

    /* tdes used as dma */
    if (AR_TRUE == hisi_ctx->tdes2dma)
    {
        if ((0x01 != pkg_num) && (DES_BLOCK_SIZE > length[0]))
        {
            AR_LOG_ERROR("Invalid 3des dma for pkg num (0x%x) or data lenth (0x%x).\n", pkg_num, length[0]);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }
    else
    {
        /* check the length of nodes list */
        ret = drv_symc_node_check(hisi_ctx->alg,
                                  hisi_ctx->mode,
                                  hisi_ctx->klen,
                                  hisi_ctx->block_size,
                                  input,
                                  output,
                                  length,
                                  usage_list,
                                  pkg_num);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_node_check, ret);
            return ret;
        }
    }

    ret = cryp_symc_crypto_process(hisi_ctx, wait);
    if (AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_crypto_process, ret);
        return ret;
    }

    drv_symc_get_iv(hisi_ctx->hard_chn, hisi_ctx->iv);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

#ifdef CHIP_AES_CCM_GCM_SUPPORT
static AR_S32 cryp_aead_ccm_setiv(void *ctx, const AR_U8 *iv, AR_U32 ivlen, AR_U32 usage)
{
    AR_U8 ccm_iv[AES_IV_SIZE] = {0};

    /* The octet lengths of N are denoted  n,
     * The octet length of the binary represen tation of the
     * octet length of the payload denoted q,
     * n is an element of {7, 8, 9, 10, 11, 12, 13}
     * n + q = 15
     * here the string of N  is pConfig->AR_U32IV, and n is pConfig->AR_U32IVLen.
     */
    if ((AES_CCM_MIN_IV_LEN > ivlen)
        || (AES_CCM_MAX_IV_LEN < ivlen))
    {
        AR_LOG_ERROR("Invalid ccm iv len, ivlen = 0x%x.\n", ivlen);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    /* Formatting of the Counter Blocks(IV for CTR)
     *
     * According to the CCM spec, the counter is equivalent to
     * a formatting of the counter index i into a complete data block.
     * The counter blocks Ctri are formatted as shown below:
     * | Octet number:  0    1 ... 15-q    16-q ... 15
     * | Contents:     Flags     N             [i]
     * Within each block Ctri, the N is get from pConfig->AR_U32IV, n + q = 15,
     * so the q equal to 15 - pConfig->AR_U32IVLen.
     * the [i] is the block conut start with 0,
     * In the Flags field, Bits 0, 1, and 2 contain the encoding of q - 1,
     * others bits shall be set to 0.
     * so the first byte of IV shall be q -1, that is 15 - pConfig->AR_U32IVLen - 1
     */
    crypto_memset(ccm_iv, sizeof(ccm_iv), 0, AES_IV_SIZE);
    ccm_iv[0] = AES_CCM_NQ_LEN - ivlen; /*IV[0] = q - 1 = 15 - n -1*/
    crypto_memcpy(&ccm_iv[1], sizeof(ccm_iv)-1, iv, ivlen);
    ivlen += 1;

    cryp_symc_setiv(ctx, ccm_iv, ivlen, usage);

    return AR_SUCCESS;
}

static AR_S32 cryp_aead_gcm_setiv(void *ctx, const AR_U8 *iv, AR_U32 ivlen, AR_U32 usage)
{
    if ((AES_GCM_MIN_IV_LEN > ivlen)
        || (AES_GCM_MAX_IV_LEN < ivlen))
    {
        AR_LOG_ERROR("Invalid gcm iv len, ivlen = 0x%x.\n", ivlen);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    cryp_symc_setiv(ctx, iv, ivlen, usage);

    return AR_SUCCESS;
}

static AR_S32 cyp_aead_ccm_n(AR_U8 *buf, AR_U8 *iv, AR_U32 ivlen,
                          AR_U32 alen, AR_U32 enclen, AR_U32 tlen)
{
    AR_U32 idx = 0;
    AR_U32 q = 0;

    AR_LOG_FuncEnter();

    /* Format B0
     * The leading octet of the first block of the formatting, B0,
     * contains four flags for control information: two single bits,
     * called  Reserved  and  Adata, and two strings of three bits,
     * to encode the values t and q.  The encoding of t is [(t -2)/2],
     * and the encoding of q is [ q-1].
     * The ordering of the flags with in the octet is given:
     *  _____________________________________________________
     * |Bit number  7     |   6   | 5  4  3     |  2  1  0   |
     * |Contents  Reserved  Adata   [(t -2)/2] |  [q-1]     |
     *  -----------------------------------------------------
     * The remaining 15 octets of the first block of  the formatting are
     * devoted to the nonce and the binary representation of
     * the message length in  q octets, as given:
     *  _____________________________________________
     * |Octet number  0   | 1 ... 15-q | 16-q ... 15 |
     * |Contents    Flags |      N     |      Q      |
     *  ---------------------------------------------
    */
    crypto_memset(buf, AES_BLOCK_SIZE, 0, AES_BLOCK_SIZE);
    buf[idx]  = (alen > 0 ? 1 : 0) << 6;//Adata
    buf[idx] |= ((tlen- 2)/2) << 3;// (t -2)/2
    buf[idx] |= (15 - ivlen);// q-1, n+q=15
    idx++;

    /* copy N, skip Flags in byte0*/
    crypto_memcpy(&buf[idx], AES_BLOCK_SIZE - idx, &iv[1], ivlen - 1);
    idx += ivlen - 1;

    q = AES_BLOCK_SIZE - idx;

    if(SYMC_CCM_Q_LEN_4B <= q)
    {
        /* max payload len of 2^32, jump to the location of last word */
        idx = AES_BLOCK_SIZE - SYMC_CCM_Q_LEN_4B;

        buf[idx++] = (AR_U8)(enclen >> 24);
        buf[idx++] = (AR_U8)(enclen >> 16);
        buf[idx++] = (AR_U8)(enclen >> 8);
        buf[idx++] = (AR_U8)(enclen);
    }
    else if ((SYMC_CCM_Q_LEN_3B == q) && (enclen <= SYMC_CCM_P_LEN_3B))
    {
        /* max payload len of 2^24*/
        buf[idx++] = (AR_U8)(enclen >> 16);
        buf[idx++] = (AR_U8)(enclen >> 8);
        buf[idx++] = (AR_U8)(enclen);
    }
    else if ((SYMC_CCM_Q_LEN_2B == q) && (enclen <= SYMC_CCM_P_LEN_2B))
    {
        /* max payload len of 2^16*/
        buf[idx++] = (AR_U8)(enclen >> 8);
        buf[idx++] = (AR_U8)(enclen);
    }
    else
    {
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_U32 cyp_aead_gcm_clen(u8 *buf, AR_U32 alen, AR_U32 enclen)
{
    AR_U32 idx = 0;

    AR_LOG_FuncEnter();

    /* Format len(C), 16 byets, coding in bits.
     * Byet0~7:  bits number of Add
     * Byet8~15: bits number of P
     */

    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = (AR_U8)((alen >> 29)&0x07);
    buf[idx++] = (AR_U8)((alen >> 21)&0xff);
    buf[idx++] = (AR_U8)((alen >> 13)&0xff);
    buf[idx++] = (AR_U8)((alen >> 5)&0xff);
    buf[idx++] = (AR_U8)((alen << 3)&0xff);

    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = 0x00;
    buf[idx++] = (AR_U8)((enclen >> 29)&0x07);
    buf[idx++] = (AR_U8)((enclen >> 21)&0xff);
    buf[idx++] = (AR_U8)((enclen >> 13)&0xff);
    buf[idx++] = (AR_U8)((enclen >> 5)&0xff);
    buf[idx++] = (AR_U8)((enclen << 3)&0xff);

    return idx;
}

static AR_S32 cryp_aead_ccm_set_aad(void *ctx, compat_addr aad, AR_U32 alen, AR_U32 tlen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    /* the parameter t denotes the octet length of T(tag)
     * t is an element of  { 4, 6, 8, 10, 12, 14, 16}
     * here t is pConfig->AR_U32TagLen
     */
    if ((tlen & 0x01)
        || (AES_CCM_MIN_TAG_LEN > tlen)
        || (AES_CCM_MAX_TAG_LEN < tlen))
    {
        AR_LOG_ERROR("Invalid tag len, tlen = 0x%x.\n", tlen);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    hisi_ctx->aad = aad;
    hisi_ctx->alen = alen;
    hisi_ctx->tlen = tlen;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_aead_gcm_set_aad(void *ctx, compat_addr aad, AR_U32 alen, AR_U32 tlen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    if ((AES_GCM_MIN_TAG_LEN > tlen)
        || (AES_GCM_MAX_TAG_LEN < tlen))
    {
        AR_LOG_ERROR("Invalid tag len, tlen = 0x%x.\n", tlen);
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    hisi_ctx->aad = aad;
    hisi_ctx->alen = alen;
    hisi_ctx->tlen = tlen;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_aead_crypto_zero(cryp_symc_context *hisi_ctx, AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;

    AR_LOG_FuncEnter();

    /* start running */
    ret = drv_symc_start(hisi_ctx->hard_chn);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_symc_start, ret);
        return ret;
    }

    /* wait done */
    if (AR_TRUE == wait)
    {
        ret = drv_symc_wait_done(hisi_ctx->hard_chn, CRYPTO_TIME_OUT);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_wait_done, ret);
            return ret;
        }

        ret = drv_aead_get_tag(hisi_ctx->hard_chn, hisi_ctx->tag);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_aead_get_tag, ret);
            return ret;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_aead_ccm_crypto(void *ctx,
                    AR_U32 operation,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num, AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;
    u8 n[AES_BLOCK_SIZE] = {0};
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
        usage_list, pkg_num, SYMC_NODE_USAGE_IN_CCM_P);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_crypto_init, ret);
        return ret;
    }

    /***
     * NIST Special Publication 800-38C
     * The data that CCM protects consists of a message, i.e., a bit string,
     * called the payload, denoted P, of bit length denoted Plen,
     * and a bit string, called the associated data, denoted A.
     * The associated data is optional, i.e., A may be the empty string.
     * CCM provides assurance of the confidentiality of P and assurance of
     * the authenticity of the origin of both A and P;
     * confidentiality is not provided for A.
     ***/

    /* Compute N */
    ret  = cyp_aead_ccm_n(n,
                          (u8*)hisi_ctx->iv,
                          hisi_ctx->ivlen,
                          hisi_ctx->alen,
                          hisi_ctx->enclen,
                          hisi_ctx->tlen);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cyp_aead_ccm_n, ret);
        return ret;
    }

    ret = drv_aead_ccm_add_n(hisi_ctx->hard_chn, n);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_aead_ccm_add_n, ret);
        return ret;
    }

    /* Compute A */
    ret = drv_aead_ccm_add_a(hisi_ctx->hard_chn, hisi_ctx->aad, hisi_ctx->alen);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_aead_ccm_add_a, ret);
        return ret;
    }

    if (0 == hisi_ctx->enclen)
    {
        /*Set CCM last flag*/
        drv_symc_add_buf_usage(hisi_ctx->hard_chn,
                               AR_TRUE,
                               SYMC_NODE_USAGE_CCM_LAST);

        /* If P is AR_NULL, must add a empty node into node list, limit to hardware devising*/
        ret = drv_symc_add_outbuf(hisi_ctx->hard_chn,
                                       ADDR_NULL,
                                       0x00,
                                       SYMC_NODE_USAGE_LAST);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_add_outbuf, ret);
            return ret;
        }

        ret = cryp_aead_crypto_zero(hisi_ctx, wait);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_aead_crypto_zero, ret);
            return ret;
        }
    }
    else
    {
        /* for CCM, must set P last and ccm last flag */
        usage_list[pkg_num - 1] |= SYMC_NODE_USAGE_CCM_LAST | SYMC_NODE_USAGE_LAST;
        ret = cryp_symc_crypto_process(hisi_ctx, wait);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_symc_crypto_len, ret);
            return ret;
        }

        ret = drv_aead_get_tag(hisi_ctx->hard_chn, hisi_ctx->tag);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_aead_get_tag, ret);
            return ret;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_aead_gcm_crypto(void *ctx,
                    AR_U32 operation,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num, AR_U32 wait)
{
    AR_S32 ret = AR_FAILURE;
    cryp_symc_context *hisi_ctx = ctx;
    AR_U8 clen[AES_BLOCK_SIZE] = {0};

    AR_LOG_FuncEnter();

    ret = cryp_symc_crypto_init(hisi_ctx, operation, input, output, length,
        usage_list, pkg_num, SYMC_NODE_USAGE_IN_GCM_P);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_crypto_init, ret);
        return ret;
    }

    /***
     * NIST Special Publication 800-38D
     * A || P || Clen.
     ***/

    /* Compute A */
    ret = drv_aead_gcm_add_a(hisi_ctx->hard_chn, hisi_ctx->aad, hisi_ctx->alen);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(drv_aead_gcm_add_a, ret);
        return ret;
    }

    if (0 == hisi_ctx->enclen)
    {
        /* At the and of GCM, must add a empty node to nodes list,
         * limit to hardware devising
         */
        ret = drv_symc_add_outbuf(hisi_ctx->hard_chn, ADDR_NULL, 0x00, SYMC_NODE_USAGE_LAST);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_symc_add_outbuf, ret);
            return ret;
        }

        /*Format the length fields of C and add to nodes list*/
        cyp_aead_gcm_clen(clen, hisi_ctx->alen, 0x00);
        ret = drv_aead_gcm_add_clen(hisi_ctx->hard_chn, clen);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_aead_gcm_add_clen, ret);
            return ret;
        }

        ret = cryp_aead_crypto_zero(hisi_ctx, wait);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_aead_crypto_zero, ret);
            return ret;
        }
    }
    else
    {
        /* for GCM, must set P last and gcm last flag */
        usage_list[pkg_num - 1] |= SYMC_NODE_USAGE_LAST;
        ret = cryp_symc_crypto_process(hisi_ctx, wait);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_symc_crypto_len, ret);
            return ret;
        }

        ret = drv_aead_get_tag(hisi_ctx->hard_chn, hisi_ctx->tag);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(drv_aead_get_tag, ret);
            return ret;
        }
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_aead_get_tag(void *ctx, AR_U32 tag[AEAD_TAG_SIZE_IN_WORD], AR_U32 *taglen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(*taglen < hisi_ctx->tlen);

    AR_LOG_DEBUG("tag buffer len %d, tag len %d\n", *taglen,  hisi_ctx->tlen);

    *taglen = hisi_ctx->tlen;

    crypto_memcpy(tag, AEAD_TAG_SIZE, hisi_ctx->tag, hisi_ctx->tlen);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}
#endif

static AR_S32 cryp_register_symc(symc_func *func)
{
    AR_U32 i = 0;

    AR_LOG_FuncEnter();

    /* check availability */
    if ((AR_NULL == func->create)
        || (AR_NULL == func->crypto))
    {
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
        return AR_ERR_CIPHER_INVALID_PARA;
    }

    /* is it already registered? */
    for(i=0; i<SYMC_FUNC_TAB_SIZE; i++)
    {
        if (symc_descriptor[i].valid
            && symc_descriptor[i].alg == func->alg
            && symc_descriptor[i].mode == func->mode)
        {
            AR_LOG_FuncExit();
            return AR_SUCCESS;
        }
    }

    /* find a blank spot */
    for(i=0; i<SYMC_FUNC_TAB_SIZE; i++)
    {
        if (!symc_descriptor[i].valid)
        {
            crypto_memcpy(&symc_descriptor[i], sizeof(symc_func), func, sizeof(symc_func));
            symc_descriptor[i].valid = AR_TRUE;
            AR_LOG_DEBUG("symc_descriptor[%d], alg %d, mode %d\n", i,
                symc_descriptor[i].alg, symc_descriptor[i].mode);

            AR_LOG_FuncExit();
            return AR_SUCCESS;
        }
    }

    /* Can't find a blank spot */
    AR_LOG_PrintErrCode(AR_ERR_CIPHER_OVERFLOW);
    return AR_ERR_CIPHER_OVERFLOW;
}

static symc_func * cryp_get_symc(AR_U32 alg, AR_U32 mode)
{
    AR_U32 i = 0;
    symc_func *template = AR_NULL;

    AR_LOG_FuncEnter();

    /* find the valid function */
    for(i=0; i<SYMC_FUNC_TAB_SIZE; i++)
    {
        AR_LOG_DEBUG("symc_descriptor[%d] valid %d, alg %d, mode %d \n",
            i, symc_descriptor[i].valid, symc_descriptor[i].alg, symc_descriptor[i].mode);

        if (symc_descriptor[i].valid)
        {
            if(symc_descriptor[i].alg == alg
              && symc_descriptor[i].mode == mode)
            {
                template = &symc_descriptor[i];
                break;
            }
        }
    }

    AR_LOG_FuncExit();
    return template;
}

/***
 * Defined the default template of Symmetric cipher function,
 * the function can be replaced by other engine
 */
static AR_S32 cryp_aes_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    AR_U32 klen = 0;
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

   /* set the key length depend on alg
    * des/3des support 2key and 3key
    * aes support 128, 192, and 256
    * sm1 support ak/ek/sk
    * sm4 support 128
    */
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    hisi_ctx->tdes2dma = AR_FALSE;

    switch(*hisi_klen)
    {
        case AR_UNF_CIPHER_KEY_AES_128BIT:
        {
            klen = AES_KEY_128BIT;
            break;
        }
        case AR_UNF_CIPHER_KEY_AES_192BIT:
        {
            klen = AES_KEY_192BIT;
            break;
        }
        case AR_UNF_CIPHER_KEY_AES_256BIT:
        {
            klen = AES_KEY_256BIT;
            break;
        }
        default:
        {
            AR_LOG_ERROR("aes with invalid keylen.\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }
    AR_LOG_INFO("key len %d, type %d\n", klen, hisi_klen);

    *hisi_klen = klen;

    if (AR_NULL == fkey)
    {
        hisi_ctx->hard_key = AR_TRUE;
        hisi_ctx->klen = klen;
        AR_LOG_FuncExit();
        return AR_SUCCESS;
    }

    cryp_symc_setkey(ctx, fkey, klen, AR_FALSE);

	AR_LOG_INFO("111cryp_aes_setkey skey %p\n", skey);

    if (skey)
    {
        cryp_symc_setkey(ctx, skey, klen, AR_TRUE);
    }

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_tdes_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    AR_U32 klen = 0;
    cryp_symc_context *hisi_ctx = ctx;
    symc_capacity capacity;
    AR_U32 invalid = AR_FALSE;

    AR_LOG_FuncEnter();

   /* set the key length depend on alg
    * des/3des support 2key and 3key
    * aes support 128, 192, and 256
    * sm1 support ak/ek/sk
    * sm4 support 128
    */
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    CRYPTO_UNUSED(skey);

    hisi_ctx->tdes2dma = AR_FALSE;

    if (AR_NULL == fkey)
    {
        if(AR_UNF_CIPHER_KEY_DES_2KEY != *hisi_klen)
        {
            AR_LOG_ERROR("error, tdes hard key must be 2key.\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_ILLEGAL_KEY);
            return AR_ERR_CIPHER_ILLEGAL_KEY;
        }

        hisi_ctx->hard_key = AR_TRUE;
        hisi_ctx->klen = TDES_KEY_128BIT;
        *hisi_klen = TDES_KEY_128BIT;

        AR_LOG_FuncExit();
        return AR_SUCCESS;
    }

    /* get symc capacity */
    drv_symc_get_capacity(&capacity);

    /*check k1 != k2*/
    if (0 == memcmp(&fkey[0], &fkey[8], DES_BLOCK_SIZE))
    {
        invalid = AR_TRUE;
    }

    switch(*hisi_klen)
    {
        case AR_UNF_CIPHER_KEY_DES_2KEY:
        {
            klen = TDES_KEY_128BIT;
            break;
        }
        case AR_UNF_CIPHER_KEY_DES_3KEY:
        {
            klen = TDES_KEY_192BIT;

            /*check k2 != k3*/
            if (0 == memcmp(&fkey[8], &fkey[16], DES_BLOCK_SIZE))
            {
                invalid = AR_TRUE;
            }
            break;
        }
        default:
        {
            AR_LOG_ERROR("3des with invalid keylen, keylen = 0x%x.\n", *hisi_klen);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_INVALID_PARA;
        }
    }

    if (AR_TRUE == invalid)
    {
        if (CRYPTO_CAPACITY_SUPPORT == capacity.dma)
        {
            AR_LOG_ERROR("3des with invalid key.\n");
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_ERR_CIPHER_ILLEGAL_KEY;
        }

        /* if don't support dma, the tdes with invalid key can be used as dma*/
        hisi_ctx->tdes2dma = AR_TRUE;
    }

    cryp_symc_setkey(ctx, fkey, klen, AR_FALSE);

    *hisi_klen = klen;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_des_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();

    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    CRYPTO_UNUSED(skey);

    hisi_ctx->tdes2dma = AR_FALSE;

    if (AR_NULL == fkey)
    {
        AR_LOG_ERROR("error, des nonsupport hard key.\n");
        AR_LOG_PrintErrCode(AR_ERR_CIPHER_ILLEGAL_KEY);
        return AR_ERR_CIPHER_ILLEGAL_KEY;
    }

    cryp_symc_setkey(ctx, fkey, DES_KEY_SIZE, AR_FALSE);

    *hisi_klen = DES_KEY_SIZE;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_3des2dma_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    u8 key[TDES_KEY_128BIT] = {0};
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);

    CRYPTO_UNUSED(fkey);
    CRYPTO_UNUSED(skey);

    cryp_symc_setkey(ctx, key, TDES_KEY_128BIT, AR_FALSE);

    *hisi_klen = TDES_KEY_128BIT;
    hisi_ctx->tdes2dma = AR_TRUE;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_sm1_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    AR_S32 ret = AR_FAILURE;
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_UNF_CIPHER_KEY_DEFAULT != *hisi_klen);
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);
    CRYPTO_UNUSED(skey);

    hisi_ctx->tdes2dma = AR_FALSE;

    if (AR_NULL == fkey)
    {
        hisi_ctx->hard_key = AR_TRUE;

        AR_LOG_FuncExit();
        return AR_SUCCESS;
    }

    cryp_symc_setkey(ctx, fkey, SM1_AK_EK_SIZE, AR_FALSE);

    /* sm1 support ak/ek/sk */
    ret = cryp_symc_sm1_setsk(ctx, skey);
    if(AR_SUCCESS != ret)
    {
        AR_LOG_PrintFuncErr(cryp_symc_sm1_setsk, ret);
        return ret;
    }

    *hisi_klen = SM1_AK_EK_SIZE;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_sm4_setkey(void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen)
{
    cryp_symc_context *hisi_ctx = ctx;

    AR_LOG_FuncEnter();
    AR_LOG_CHECK_PARAM(AR_UNF_CIPHER_KEY_DEFAULT != *hisi_klen);
    AR_LOG_CHECK_PARAM(AR_NULL == hisi_ctx);
    CRYPTO_UNUSED(skey);

    hisi_ctx->tdes2dma = AR_FALSE;

    if (AR_NULL == fkey)
    {
        hisi_ctx->hard_key = AR_TRUE;

        AR_LOG_FuncExit();
        return AR_SUCCESS;
    }

    /*  sm4 support 128 */
    cryp_symc_setkey(ctx, fkey, SM4_KEY_SIZE, AR_FALSE);

    *hisi_klen = SM4_KEY_SIZE;

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

static AR_S32 cryp_symc_setiv_default(void *ctx, const AR_U8 *iv, AR_U32 ivlen, AR_U32 usage)
{
    AR_LOG_FuncEnter();

    if (AR_NULL == iv)
    {
        return AR_SUCCESS;
    }

    if (AES_IV_SIZE < ivlen)
    {
        return AR_FAILURE;
    }

    cryp_symc_setiv(ctx, iv, ivlen, usage);

    AR_LOG_FuncExit();
    return AR_SUCCESS;
}

/* Default As AES */
static void cryp_register_symc_default(symc_func *func, symc_alg alg, symc_mode mode)
{
    crypto_memset(func, sizeof(symc_func), 0, sizeof(symc_func));

    func->mode = mode;
    func->alg = alg;
    func->create = cryp_symc_create;
    func->setiv = cryp_symc_setiv_default;
    func->getiv = cryp_symc_getiv;
    func->crypto = cryp_symc_crypto;
    func->setmode = cryp_symc_setmode;
    func->setkey = cryp_aes_setkey;
    func->waitdone = cryp_symc_wait_done;
    return;
}

static void cryp_register_symc_aes(AR_U32 capacity, symc_mode mode)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        ret = cryp_register_symc(&func);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#ifdef SOFT_AES_SUPPORT
    else
    {
        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.create = ext_mbedtls_symc_create;
        func.destroy= ext_mbedtls_symc_destory;
        func.setiv = ext_mbedtls_symc_setiv;
        func.getiv = ext_mbedtls_symc_getiv;
        func.setkey = ext_mbedtls_symc_setkey;
        func.setmode = ext_mbedtls_symc_setmode;
        func.crypto = ext_mbedtls_symc_crypto;
        func.waitdone = AR_NULL;
        ret = cryp_register_symc(&func);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#endif

    return;
}

static void cryp_register_symc_dma(AR_U32 dma_capacity, AR_U32 tdes_capacity)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(func), 0, sizeof(func));

    if (CRYPTO_CAPACITY_SUPPORT == dma_capacity)
    {
        func.mode = SYMC_MODE_ECB;
        func.alg = SYMC_ALG_NULL_CIPHER;
        func.create = cryp_symc_create;
        func.setmode = cryp_symc_setmode;
        func.crypto = cryp_symc_crypto;
        ret = cryp_register_symc(&func);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
    else if (CRYPTO_CAPACITY_SUPPORT == tdes_capacity)
    {
        func.mode = SYMC_MODE_ECB;
        func.alg = SYMC_ALG_NULL_CIPHER;
        func.create = cryp_symc_create;
        func.setmode = cryp_3des2dma_setmode;
        func.setkey = cryp_3des2dma_setkey;
        func.crypto = cryp_symc_crypto;
        ret = cryp_register_symc(&func);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
    return;
}

static void cryp_register_symc_aes_cts(AR_U32 capacity, symc_mode mode)
{
#ifdef SOFT_AES_CTS_SUPPORT
    AR_S32 ret = AR_FAILURE;

    AR_LOG_DEBUG("CTS crypto capacity %d, mode %d\n", capacity, mode);

    if (CRYPTO_CAPACITY_SUPPORT != capacity)
    {
        symc_func func;

        crypto_memset(&func, sizeof(func), 0, sizeof(func));

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.crypto = cryp_aes_cbc_cts_crypto;
        func.waitdone = AR_NULL;
        AR_LOG_DEBUG("CTS crypto 0x%p, mode %d\n", func.crypto, mode);
        ret = cryp_register_symc(&func);
        if(AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#endif
    return;
}

static void cryp_register_aead_ccm(AR_U32 capacity, symc_mode mode)
{
    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
#ifdef CHIP_AES_CCM_GCM_SUPPORT
        symc_func func;
        AR_S32 ret = AR_FAILURE;

        crypto_memset(&func, sizeof(func), 0, sizeof(func));

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.setadd = cryp_aead_ccm_set_aad;
        func.gettag = cryp_aead_get_tag;
        func.crypto = cryp_aead_ccm_crypto;
        func.setiv = cryp_aead_ccm_setiv;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
#endif
    }
    else
    {
#ifdef SOFT_AES_CCM_GCM_SUPPORT
        symc_func func;
        AR_S32 ret = AR_FAILURE;

        crypto_memset(&func, sizeof(func), 0, sizeof(func));

        func.mode = mode;
        func.alg = SYMC_ALG_AES;
        func.create = ext_mbedtls_aead_create;
        func.destroy = ext_mbedtls_aead_destory;
        func.setiv = ext_mbedtls_aead_setiv;
        func.getiv = AR_NULL;
        func.crypto = ext_mbedtls_aead_ccm_crypto;
        func.setmode = AR_NULL;
        func.setkey = ext_mbedtls_aead_setkey;
        func.setadd = ext_mbedtls_aead_set_aad;
        func.gettag = ext_mbedtls_aead_get_tag;
        func.waitdone = AR_NULL;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
#endif
    }

    return;
}

static void cryp_register_aead_gcm(AR_U32 capacity, symc_mode mode)
{
    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
#ifdef  CHIP_AES_CCM_GCM_SUPPORT
        symc_func func;
        AR_S32 ret = AR_FAILURE;

        crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

        cryp_register_symc_default(&func, SYMC_ALG_AES, mode);
        func.setadd = cryp_aead_gcm_set_aad;
        func.gettag = cryp_aead_get_tag;
        func.crypto = cryp_aead_gcm_crypto;
        func.setiv = cryp_aead_gcm_setiv;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
#endif
    }
    else
    {
#ifdef SOFT_AES_CCM_GCM_SUPPORT
        symc_func func;
        AR_S32 ret = AR_FAILURE;

        crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

        func.mode = mode;
        func.alg = SYMC_ALG_AES;
        func.create = ext_mbedtls_aead_create;
        func.destroy = ext_mbedtls_aead_destory;
        func.setiv = ext_mbedtls_aead_setiv;
        func.getiv = AR_NULL;
        func.crypto = ext_mbedtls_aead_gcm_crypto;
        func.setmode = AR_NULL;
        func.setkey = ext_mbedtls_aead_setkey;
        func.setadd = ext_mbedtls_aead_set_aad;
        func.gettag = ext_mbedtls_aead_get_tag;
        func.waitdone = AR_NULL;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
#endif
    }

    return;
}

static void cryp_register_symc_tdes(AR_U32 capacity, symc_mode mode)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
        cryp_register_symc_default(&func, SYMC_ALG_TDES, mode);
        func.setkey = cryp_tdes_setkey;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#ifdef SOFT_TDES_SUPPORT
    else
    {
        cryp_register_symc_default(&func, SYMC_ALG_TDES, mode);
        func.create = ext_mbedtls_symc_create;
        func.destroy= ext_mbedtls_symc_destory;
        func.setiv = ext_mbedtls_symc_setiv;
        func.getiv = ext_mbedtls_symc_getiv;
        func.setkey = ext_mbedtls_symc_setkey;
        func.setmode = ext_mbedtls_symc_setmode;
        func.crypto = ext_mbedtls_symc_crypto;
        func.waitdone = AR_NULL;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#endif
    return;
}

static void cryp_register_symc_des(AR_U32 capacity, symc_mode mode)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
        cryp_register_symc_default(&func, SYMC_ALG_DES, mode);
        func.setkey = cryp_des_setkey;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#ifdef SOFT_TDES_SUPPORT
    else
    {
        cryp_register_symc_default(&func, SYMC_ALG_DES, mode);
        func.create = ext_mbedtls_symc_create;
        func.destroy= ext_mbedtls_symc_destory;
        func.setiv = ext_mbedtls_symc_setiv;
        func.getiv = ext_mbedtls_symc_getiv;
        func.setkey = ext_mbedtls_symc_setkey;
        func.setmode = ext_mbedtls_symc_setmode;
        func.crypto = ext_mbedtls_symc_crypto;
        func.waitdone = AR_NULL;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#endif
    return;
}

static void cryp_register_symc_sm1(AR_U32 capacity, symc_mode mode)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
        cryp_register_symc_default(&func, SYMC_ALG_SM1, mode);
        func.setround = cryp_symc_sm1_setround;
        func.setkey = cryp_sm1_setkey;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
    return;
}

static void cryp_register_symc_sm4(AR_U32 capacity, symc_mode mode)
{
    symc_func func;
    AR_S32 ret = AR_FAILURE;

    crypto_memset(&func, sizeof(symc_func), 0, sizeof(symc_func));

    if (CRYPTO_CAPACITY_SUPPORT == capacity)
    {
        cryp_register_symc_default(&func, SYMC_ALG_SM4, mode);
        func.setkey = cryp_sm4_setkey;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#ifdef SOFT_SM4_SUPPORT
    else
    {
        cryp_register_symc_default(&func, SYMC_ALG_SM4, mode);
        func.create = ext_sm4_create;
        func.destroy= ext_sm4_destory;
        func.setiv = ext_sm4_setiv;
        func.getiv = ext_sm4_getiv;
        func.setkey = ext_sm4_setkey;
        func.setmode = ext_sm4_setmode;
        func.crypto = ext_sm4_crypto;
        ret = cryp_register_symc(&func);
        if (AR_SUCCESS != ret)
        {
            AR_LOG_PrintFuncErr(cryp_register_symc, ret);
            return;
        }
    }
#endif
    return;
}

/* symc function register */
static void cryp_register_all_symc(void)
{
    symc_capacity capacity;

    AR_LOG_FuncEnter();

    crypto_memset(&capacity, sizeof(capacity), 0, sizeof(capacity));

    /* get symc capacity */
    drv_symc_get_capacity(&capacity);

    /* register the symc function if supported */

    /* AES */
    cryp_register_symc_aes(capacity.aes_ecb, SYMC_MODE_ECB);
    cryp_register_symc_aes(capacity.aes_cbc, SYMC_MODE_CBC);
    cryp_register_symc_aes(capacity.aes_cfb, SYMC_MODE_CFB);
    cryp_register_symc_aes(capacity.aes_ofb, SYMC_MODE_OFB);
    cryp_register_symc_aes(capacity.aes_ctr, SYMC_MODE_CTR);
    cryp_register_symc_dma(capacity.dma, capacity.tdes_ecb);
    cryp_register_symc_aes_cts(capacity.aes_cts, SYMC_MODE_CTS);

    /* AEAD */
    cryp_register_aead_ccm(capacity.aes_ccm, SYMC_MODE_CCM);
    cryp_register_aead_gcm(capacity.aes_gcm, SYMC_MODE_GCM);

    /* TDES */
    cryp_register_symc_tdes(capacity.tdes_ecb, SYMC_MODE_ECB);
    cryp_register_symc_tdes(capacity.tdes_cbc, SYMC_MODE_CBC);
    cryp_register_symc_tdes(capacity.tdes_cfb, SYMC_MODE_CFB);
    cryp_register_symc_tdes(capacity.tdes_ofb, SYMC_MODE_OFB);
    cryp_register_symc_tdes(capacity.tdes_ctr, SYMC_MODE_CTR);

    /* DES */
    cryp_register_symc_des(capacity.des_ecb, SYMC_MODE_ECB);
    cryp_register_symc_des(capacity.des_cbc, SYMC_MODE_CBC);
    cryp_register_symc_des(capacity.des_cfb, SYMC_MODE_CFB);
    cryp_register_symc_des(capacity.des_ofb, SYMC_MODE_OFB);
    cryp_register_symc_des(capacity.des_ctr, SYMC_MODE_CTR);

    /* SM1 */
    cryp_register_symc_sm1(capacity.sm1_ecb, SYMC_MODE_ECB);
    cryp_register_symc_sm1(capacity.sm1_cbc, SYMC_MODE_CBC);
    cryp_register_symc_sm1(capacity.sm1_cfb, SYMC_MODE_CFB);
    cryp_register_symc_sm1(capacity.sm1_ofb, SYMC_MODE_OFB);
    cryp_register_symc_sm1(capacity.sm1_ctr, SYMC_MODE_CTR);

    /* SM4 */
    cryp_register_symc_sm4(capacity.sm4_ecb, SYMC_MODE_ECB);
    cryp_register_symc_sm4(capacity.sm4_cbc, SYMC_MODE_CBC);
    cryp_register_symc_sm4(capacity.sm4_cfb, SYMC_MODE_CFB);
    cryp_register_symc_sm4(capacity.sm4_ofb, SYMC_MODE_OFB);
    cryp_register_symc_sm4(capacity.sm4_ctr, SYMC_MODE_CTR);

    AR_LOG_FuncExit();

    return;
}

symc_func * cryp_get_symc_op(AR_UNF_CIPHER_ALG_E alg, AR_UNF_CIPHER_WORK_MODE_E mode)
{
    AR_U32 cryp_mode = 0;
    symc_func *func = AR_NULL;
    symc_alg cryp_alg;

    AR_LOG_FuncEnter();

    switch(alg)
    {
        case AR_UNF_CIPHER_ALG_DES:
            cryp_alg = SYMC_ALG_DES;
            break;
        case AR_UNF_CIPHER_ALG_3DES:
            cryp_alg = SYMC_ALG_TDES;
            break;
        case AR_UNF_CIPHER_ALG_AES:
            cryp_alg = SYMC_ALG_AES;
            break;
        case AR_UNF_CIPHER_ALG_SM1:
            cryp_alg = SYMC_ALG_SM1;
            break;
        case AR_UNF_CIPHER_ALG_SM4:
            cryp_alg = SYMC_ALG_SM4;
            break;
        case AR_UNF_CIPHER_ALG_DMA:
            cryp_alg = SYMC_ALG_NULL_CIPHER;
            mode = AR_UNF_CIPHER_WORK_MODE_ECB;
            break;
        default:
            AR_LOG_ERROR("Invalid alg, alg = 0x%x.\n", alg);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_NULL;
    }

    switch(mode)
    {
        case AR_UNF_CIPHER_WORK_MODE_ECB:
            cryp_mode = SYMC_MODE_ECB;
            break;
        case AR_UNF_CIPHER_WORK_MODE_CBC:
            cryp_mode = SYMC_MODE_CBC;
            break;
        case AR_UNF_CIPHER_WORK_MODE_CFB:
            cryp_mode = SYMC_MODE_CFB;
            break;
        case AR_UNF_CIPHER_WORK_MODE_OFB:
            cryp_mode = SYMC_MODE_OFB;
            break;
        case AR_UNF_CIPHER_WORK_MODE_CTR:
            cryp_mode = SYMC_MODE_CTR;
            break;
        case AR_UNF_CIPHER_WORK_MODE_CCM:
            cryp_mode = SYMC_MODE_CCM;
            break;
        case AR_UNF_CIPHER_WORK_MODE_GCM:
            cryp_mode = SYMC_MODE_GCM;
            break;
        default:
            AR_LOG_ERROR("Invalid mode, mode = 0x%x.\n", mode);
            AR_LOG_PrintErrCode(AR_ERR_CIPHER_INVALID_PARA);
            return AR_NULL;
    }

    func = cryp_get_symc(cryp_alg, cryp_mode);

    AR_LOG_FuncExit();

    return func;
}

/** @}*/  /** <!-- ==== API Code end ====*/
