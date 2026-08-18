/**
 * @file audio_service_refactored.h
 * @brief Audio Service Header - Professional Implementation
 */

#ifndef AUDIO_SERVICE_REFACTORED_H
#define AUDIO_SERVICE_REFACTORED_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include "mpi_audio.h"
#include "mpi_vb.h"

/*==============================================================================
 * CONSTANTS
 *============================================================================*/

#define SOCKET_PATH "/tmp/audio_service.sock"
#define MAX_COMMAND_LEN 1024
#define MAX_FILENAME_LEN 512
#define AUDIO_FRAME_SIZE 1024
#define VB_BLOCK_SIZE (1024 * 4)
#define VB_BLOCK_COUNT 20

/*==============================================================================
 * TYPE DEFINITIONS
 *============================================================================*/

typedef enum {
    AUDIO_DEV_ACODEC = 0,
    AUDIO_DEV_I2S_MASTER0 = 1,
    AUDIO_DEV_I2S_MASTER1,
    AUDIO_DEV_I2S_SLAVE0,
    AUDIO_DEV_I2S_SLAVE1,
} audio_dev_id_e;

typedef enum {
    CMD_UNKNOWN = 0,
    CMD_PLAY,
    CMD_STOP,
} audio_command_e;

typedef struct {
    audio_command_e cmd;
    char filename[MAX_FILENAME_LEN];
    int32_t volume;
} audio_command_data_t;

typedef struct {
    int16_t format_tag;
    int16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    int16_t block_align;
    int16_t bits_per_sample;
} wave_format_t;

typedef struct {
    char riff[4];
    uint32_t riff_len;
    char wave[4];
    char fmt[4];
    uint32_t fmt_len;
    wave_format_t format;
    char data[4];
    uint32_t data_len;
} wave_file_header_t;

typedef struct {
    // Threading
    pthread_t ipc_thread;
    pthread_t playback_thread;
    pthread_mutex_t playback_mutex;
    
    // State flags
    volatile bool service_exit;
    volatile bool playback_exit;
    volatile bool playback_thread_running;
    volatile bool ao_initialized;
    
    // Audio configuration
    AUDIO_DEV ao_dev_id;
    AO_CHN ao_channel;
    AUDIO_SAMPLE_RATE_E sample_rate;
    AUDIO_SOUND_MODE_E sound_mode;
    AUDIO_BIT_WIDTH_E bit_width;
    int32_t ao_volume;
    
    // Current playback
    char current_audio_file[MAX_FILENAME_LEN];
    
    // VB pool
    VB_POOL vb_pool_id;
    
    // Signal handling
    volatile int signal_count;
} audio_service_ctx_t;

/*==============================================================================
 * MACROS
 *============================================================================*/

#define CHECK_RESULT(expr, name) \
    do { \
        AR_S32 ret = (expr); \
        if (AR_SUCCESS != ret) { \
            fprintf(stderr, "[ERROR] %s failed at %s:%d (0x%x)\n", \
                   (name), __FUNCTION__, __LINE__, ret); \
            return ret; \
        } \
    } while(0)

#define SAFE_STRNCPY(dest, src, size) \
    do { \
        strncpy((dest), (src), (size) - 1); \
        (dest)[(size) - 1] = '\0'; \
    } while(0)

/*==============================================================================
 * FUNCTION DECLARATIONS
 *============================================================================*/

// Service lifecycle
AR_S32 audio_service_init(void);
void audio_service_cleanup(void);
AR_S32 audio_service_run(void);

// Audio device management
AR_S32 ao_device_init(void);
AR_S32 ao_device_deinit(void);
AR_S32 ao_set_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume);

// Utility functions
AUDIO_SAMPLE_RATE_E get_sample_rate_enum(uint32_t sample_rate);
bool is_sample_rate_supported(uint32_t sample_rate);

// Thread functions
void* playback_thread_func(void* arg);
void* ipc_server_thread_func(void* arg);

// Command processing
audio_command_e parse_command(const char* cmd_str, audio_command_data_t* cmd_data);
void stop_current_playback(void);

// Signal handling
void signal_handler(int signo, siginfo_t *info, void *data);

#endif // AUDIO_SERVICE_REFACTORED_H
