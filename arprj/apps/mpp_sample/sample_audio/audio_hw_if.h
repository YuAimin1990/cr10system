#ifndef AUDIO_HW_IF_H
#define AUDIO_HW_IF_H

// ********** 新增的头文件 **********
#include "mpi_audio.h" 
// **********************************

typedef struct {
    AUDIO_SAMPLE_RATE_E sample_rate;
    AUDIO_BIT_WIDTH_E bit_width;
    AUDIO_SOUND_MODE_E sound_mode;
    int volume;
} hw_audio_params_t;

int hw_if_system_init(void);
void hw_if_system_deinit(void);

int hw_if_ao_init(const hw_audio_params_t *params);
void hw_if_ao_deinit(void);

int hw_if_ao_set_volume(int volume);
int hw_if_ao_send_frame(void *p_frame);

#endif // AUDIO_HW_IF_H