#ifndef CRYP_SYMC_H_
#define CRYP_SYMC_H_

#include "drv_osal_lib.h"
#include "drv_symc.h"

/******************************* API Declaration *****************************/
/** \addtogroup      aes */
/** @{ */  /** <!--[aes]*/


/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      cipher drivers*/
/** @{*/  /** <!-- [cipher]*/


/**
* \brief          Initialize crypto of symc *
*/
AR_S32 cryp_symc_init(void);

/**
* \brief          Deinitialize crypto of symc *
*/
void cryp_symc_deinit(void);

/**
 * \brief          Create symc handle
 *
 * \param handle   symc handle to be initialized
 * \param chn      symc channel
 */
typedef void *(*func_symc_create)( AR_U32 hard_chn );

/**
 * \brief          Clear symc context
 *
 * \param handle      symc handle to be destory
 */
typedef AR_S32 (*func_symc_destroy)( void *ctx);

/**
 * \brief          symc key schedule
 *
 * \param handle   SYMC handle
 * \param[in]  fkey first  key buffer, defualt
 * \param[in]  skey second key buffer, expand
 * \param hisi_klen input key type, output key length in bytes
 *
 * \return         0 if successful, or HI_SYMC_ERR_KEY_LEN_INVALID
 */
typedef AR_S32 (*func_symc_setkey)( void *ctx, const AR_U8 *fkey, const AR_U8 *skey, AR_U32 *hisi_klen);

/**
 * \brief          Symc iv schedule
 *
 * \param handle   symc handle
 * \param IV       Symc IV
 * \param ivlen    length of iv
 *
 * \return         0 if successful.
 */
typedef AR_S32 (*func_symc_setiv)( void *ctx, const AR_U8 *iv, AR_U32 ivlen, AR_U32 usage);

/**
 * \brief          Symc iv schedule
 *
 * \param handle   symc handle
 * \param IV       Symc IV
 * \param ivlen    must be 128, 192 or 256
 *
 * \return         0 if successful.
 */
typedef void (*func_symc_getiv)( void *ctx, AR_U8 *iv, AR_U32 *ivlen);

/**
 * \brief          SYMC alg and mode
 *
 * \param handle   SYMC handle
 * \param alg      Symmetric cipher alg
 * \param mode     Symmetric cipher mode
 * \param keybits  must be 128, 192 or 256
 *
 * \return         0 if successful.
 */
typedef void (*func_symc_setmode)( void *ctx, symc_alg alg, symc_mode mode, symc_width width);

/**
 * \brief          SYMC wait done
 *
 * \param ctx      SYMC handle
 * \return         0 if successful.
 */
typedef AR_S32 (*func_symc_wait_done)(void *ctx, AR_U32 timeout);

/**
 * \brief          SYMC alg and mode
 *
 * \param handle   SYMC handle
 * \param round    SM1 round number
 *
 * \return         0 if successful.
 */
typedef AR_S32 (*func_symc_sm1_setround)( void *ctx, AR_U32 round);

/**
 * \brief          symc  buffer encryption/decryption.
 *
 * Note: Due to the nature of aes you should use the same key schedule for
 * both encryption and decryption.
 *
 * \param handle   symc handle
 * \param operation  decrypt or encrypt
 * \param mode     mode
 * \param length   length of the input data
 * \param input    buffer holding the input data
 * \param output   buffer holding the output data
 * \param usage_list usage of buffer
 * \param pkg_num  numbers of buffer
 * \param last     last or not
 *
 * \return         0 if successful
 */
typedef AR_S32 (*func_symc_crypto)( void *ctx, AR_U32 operation,
                    compat_addr input[],
                    compat_addr output[],
                    AR_U32 length[],
                    symc_node_usage usage_list[],
                    AR_U32 pkg_num,
                    AR_U32 wait);

/**
 * \brief          CCM/GCM set Associated Data
 *
 * \param ctx      SYMC handle
 * \param aad      Associated Data
 * \param alen     Associated Data Length
 * \param tlen     Tag length
 *
 * \return         0 if successful.
 */
typedef AR_S32 (*func_aead_set_aad)( void *ctx, compat_addr aad, AR_U32 alen, AR_U32 tlen);

/**
 * \brief          SYMC multiple buffer encryption/decryption.
 * \param[in]  id The channel number.
 * \param[in]  tag tag data of CCM/GCM
 * \param uuid uuid The user identification.
 *
 * \return         0 if successful
 */
typedef AR_S32 (*func_aead_get_tag)(void *ctx, AR_U32 tag[AEAD_TAG_SIZE_IN_WORD], AR_U32 *taglen);

/*! \struct of Symmetric cipher function template */
typedef struct
{
    AR_U32 valid;                  /*!<  vliad or not */
    symc_alg alg;               /*!<  Alg of Symmetric cipher */
    symc_mode mode;             /*!<  Mode of Symmetric cipher */
    func_symc_setmode setmode;  /*!<  Set mode function */
    func_symc_sm1_setround setround; /*!<  SM1 set round function */
    func_symc_create  create;   /*!<  Create function */
    func_symc_destroy destroy;  /*!<  destroy function */
    func_symc_setkey  setkey;   /*!<  setkey function */
    func_symc_setiv   setiv;    /*!<  setiv function */
    func_symc_getiv   getiv;    /*!<  getiv function */
    func_aead_set_aad setadd;   /*!<  setadd function */
    func_aead_get_tag gettag;   /*!<  get tag function */
    func_symc_crypto  crypto;   /*!<  crypto function */
    func_symc_wait_done waitdone; /*!<  wait done */
}symc_func;

/**
\brief  symc alloc channel.
\param[out]  hard_chn symc channel.
\retval     On success, func is returned.  On error, HI_NULL is returned.
*/
AR_S32 cryp_symc_alloc_chn(AR_U32 *hard_chn);

/**
\brief  symc free channel.
\param[in]  hard_chn symc channel.
\retval     On success, func is returned.  On error, HI_NULL is returned.
*/
void cryp_symc_free_chn(AR_U32 hard_chn);

/**
\brief  Clone the function from template of aes engine.
\param[in]  alg The alg of Symmetric cipher.
\param[in]  mode The work mode.
\retval     On success, func is returned.  On error, HI_NULL is returned.
*/
symc_func * cryp_get_symc_op(AR_UNF_CIPHER_ALG_E alg, AR_UNF_CIPHER_WORK_MODE_E mode);

/** @} */  /** <!-- ==== API declaration end ==== */

#endif
