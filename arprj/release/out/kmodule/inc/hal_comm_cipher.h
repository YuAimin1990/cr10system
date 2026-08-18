#ifndef __AR_COMMON_CIPHER__
#define __AR_COMMON_CIPHER__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define  AR_ERR_CIPHER_NOT_INIT                     (AR_S32)(0x804D0001)
#define  AR_ERR_CIPHER_INVALID_HANDLE               (AR_S32)(0x804D0002)
#define  AR_ERR_CIPHER_INVALID_POINT                (AR_S32)(0x804D0003)
#define  AR_ERR_CIPHER_INVALID_PARA                 (AR_S32)(0x804D0004)
#define  AR_ERR_CIPHER_FAILED_INIT                  (AR_S32)(0x804D0005)
#define  AR_ERR_CIPHER_FAILED_GETHANDLE             (AR_S32)(0x804D0006)
#define  AR_ERR_CIPHER_FAILED_RELEASEHANDLE         (AR_S32)(0x804D0007)
#define  AR_ERR_CIPHER_FAILED_CONFIGAES             (AR_S32)(0x804D0008)
#define  AR_ERR_CIPHER_FAILED_CONFIGDES             (AR_S32)(0x804D0009)
#define  AR_ERR_CIPHER_FAILED_ENCRYPT               (AR_S32)(0x804D000A)
#define  AR_ERR_CIPHER_FAILED_DECRYPT               (AR_S32)(0x804D000B)
#define  AR_ERR_CIPHER_BUSY                         (AR_S32)(0x804D000C)
#define  AR_ERR_CIPHER_NO_AVAILABLE_RNG             (AR_S32)(0x804D000D)
#define  AR_ERR_CIPHER_FAILED_MEM                   (AR_S32)(0x804D000E)
#define  AR_ERR_CIPHER_UNAVAILABLE                  (AR_S32)(0x804D000F)
#define  AR_ERR_CIPHER_OVERFLOW                     (AR_S32)(0x804D0010)
#define  AR_ERR_CIPHER_HARD_STATUS                  (AR_S32)(0x804D0011)
#define  AR_ERR_CIPHER_TIMEOUT                      (AR_S32)(0x804D0012)
#define  AR_ERR_CIPHER_UNSUPPORTED                  (AR_S32)(0x804D0013)
#define  AR_ERR_CIPHER_REGISTER_IRQ                 (AR_S32)(0x804D0014)
#define  AR_ERR_CIPHER_ILLEGAL_UUID                 (AR_S32)(0x804D0015)
#define  AR_ERR_CIPHER_ILLEGAL_KEY                  (AR_S32)(0x804D0016)
#define  AR_ERR_CIPHER_INVALID_ADDR                 (AR_S32)(0x804D0017)
#define  AR_ERR_CIPHER_INVALID_LENGTH               (AR_S32)(0x804D0018)
#define  AR_ERR_CIPHER_ILLEGAL_DATA                 (AR_S32)(0x804D0019)
#define  AR_ERR_CIPHER_RSA_SIGN                     (AR_S32)(0x804D001A)
#define  AR_ERR_CIPHER_RSA_VERIFY                   (AR_S32)(0x804D001B)
#define  AR_ERR_CIPHER_MEMSET_S_FAILED              (AR_S32)(0x804D001C)
#define  AR_ERR_CIPHER_MEMCPY_S_FAILED              (AR_S32)(0x804D001D)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CRYP_CIPHER_H__ */

