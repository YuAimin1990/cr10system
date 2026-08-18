#ifndef __UAC_SERVICE_H__
#define __UAC_SERVICE_H__

#include "mpi_vb.h"

#define ACODEC_FILE             "/dev/acodec"
#define UAC_DEVICE_PATH         "plughw:0,0"

typedef struct
{
    int                         run;
    int                         is_usrpool;
    VB_POOL                     pool_id;

    snd_pcm_t                   *in_handle;
    snd_pcm_t                   *out_handle;
}ar_uac_info;
typedef enum {
    AR_ACODEC_DEV_ID = 0,
    AR_I2S_MASTER0_ID = 1,
    AR_I2S_MASTER1_ID,
#ifdef AR9341
    AR_I2S_MASTER2_ID,
#endif
    AR_I2S_SLAVE0_ID,
    AR_I2S_SLAVE1_ID,
} AR_AUDIO_DEV_ID;
#endif