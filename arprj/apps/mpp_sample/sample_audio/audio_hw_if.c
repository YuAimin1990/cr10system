#include "audio_hw_if.h"
#include "utils.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_audio.h"
#include "acodec.h" 
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define ACODEC_DEVICE "/dev/acodec"

static const int AO_DEV_ID = 0;
static const int AO_CHN_ID = 0;

int hw_if_system_init(void) {
    VB_CONFIG_S vb_config = {0};
    vb_config.astCommPool[0].u64BlkSize = 1024 * 4;
    vb_config.astCommPool[0].u32BlkCnt = 20;
    
    // Try to set the VB config. If it fails, it's likely already configured.
    int ret = AR_MPI_VB_SetConfig(&vb_config);
    if (ret != 0) {
        LOG_WARN("AR_MPI_VB_SetConfig failed with code %#x. This is OK if another process already configured the VB system.", ret);
        // We don't return here, we proceed to Init.
    }

    CHECK_RET(AR_MPI_VB_Init(), "AR_MPI_VB_Init");
    CHECK_RET(AR_MPI_SYS_Init(), "AR_MPI_SYS_Init");
    
    LOG_INFO("System and VB initialized.");
    return 0;
}

void hw_if_system_deinit(void) {
    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();
    LOG_INFO("System and VB de-initialized.");
}

int hw_if_ao_init(const hw_audio_params_t *params) {
    AIO_ATTR_S ao_attr = {0};
    ao_attr.enSamplerate = params->sample_rate;
    ao_attr.enBitwidth = params->bit_width;
    ao_attr.enSoundmode = params->sound_mode;
    ao_attr.enWorkmode = AIO_MODE_I2S_MASTER;
    ao_attr.u32PtNumPerFrm = 2048; // Common value
    ao_attr.u32ChnCnt = 2;
    ao_attr.enI2sType = AIO_I2STYPE_INNERCODEC;

    CHECK_RET(AR_MPI_AO_SetPubAttr(AO_DEV_ID, &ao_attr), "AR_MPI_AO_SetPubAttr");
    CHECK_RET(AR_MPI_AO_Enable(AO_DEV_ID), "AR_MPI_AO_Enable");
    CHECK_RET(AR_MPI_AO_EnableChn(AO_DEV_ID, AO_CHN_ID), "AR_MPI_AO_EnableChn");

    hw_if_ao_set_volume(params->volume);
    
    LOG_INFO("Audio Output (AO) initialized.");
    return 0;
}

void hw_if_ao_deinit(void) {
    AR_MPI_AO_DisableChn(AO_DEV_ID, AO_CHN_ID);
    AR_MPI_AO_Disable(AO_DEV_ID);
    LOG_INFO("Audio Output (AO) de-initialized.");
}

int hw_if_ao_set_volume(int volume) {
    int fd = open(ACODEC_DEVICE, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Failed to open %s", ACODEC_DEVICE);
        return -1;
    }
    // Note: The ioctl number and volume range might be specific to your SDK version.
    // This is an example based on common practice.
    if (ioctl(fd, ACODEC_SET_OUTPUT_VOL, &volume) != 0) {
        LOG_ERROR("Failed to set volume via ioctl.");
        close(fd);
        return -1;
    }
    close(fd);
    LOG_INFO("Volume set to %d", volume);
    return 0;
}

int hw_if_ao_send_frame(void *p_frame) {
    // A timeout of 1000ms
    return AR_MPI_AO_SendFrame(AO_DEV_ID, AO_CHN_ID, (AUDIO_FRAME_S *)p_frame, 1000);
}