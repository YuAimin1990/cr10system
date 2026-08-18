#include "audio_player.h"
#include "audio_hw_if.h"
#include "wave_parser.h"
#include "utils.h"
#include "ipc_protocol.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#include "mpi_vb.h" // For VB block handling

struct audio_player_s {
    pthread_t thread_id;
    pthread_mutex_t mutex;
    bool is_running;
    volatile bool stop_requested;

    char current_file[MAX_FILENAME_LEN];
    hw_audio_params_t hw_params;
    bool hw_initialized;
};

static void* player_thread_func(void *arg);

audio_player_handle audio_player_create(void) {
    audio_player_handle handle = calloc(1, sizeof(struct audio_player_s));
    if (!handle) {
        LOG_ERROR("Failed to allocate player handle.");
        return NULL;
    }
    pthread_mutex_init(&handle->mutex, NULL);
    // Set default params
    handle->hw_params.sample_rate = AUDIO_SAMPLE_RATE_48000;
    handle->hw_params.bit_width = AUDIO_BIT_WIDTH_16;
    handle->hw_params.sound_mode = AUDIO_SOUND_MODE_STEREO;
    handle->hw_params.volume = 15; // A reasonable default
    return handle;
}

void audio_player_destroy(audio_player_handle handle) {
    if (!handle) return;
    audio_player_stop(handle); // 确保停止
    
    // 如果硬件被初始化了，确保它被关闭
    if (handle->hw_initialized) {
        hw_if_ao_deinit();
        handle->hw_initialized = false;
    }

    pthread_mutex_destroy(&handle->mutex);
    free(handle);
}

int audio_player_play(audio_player_handle handle, const char *filename, int volume) {
    if (!handle || !filename) return -1;

    audio_player_stop(handle); // Ensure previous playback is stopped

    pthread_mutex_lock(&handle->mutex);
    strncpy(handle->current_file, filename, sizeof(handle->current_file) - 1);
    handle->hw_params.volume = volume;
    handle->stop_requested = false;

    if (pthread_create(&handle->thread_id, NULL, player_thread_func, handle) != 0) {
        LOG_ERROR("Failed to create player thread.");
        pthread_mutex_unlock(&handle->mutex);
        return -1;
    }
    handle->is_running = true;
    pthread_mutex_unlock(&handle->mutex);

    return 0;
}

int audio_player_stop(audio_player_handle handle) {
    if (!handle) return -1;
    
    pthread_mutex_lock(&handle->mutex);
    if (!handle->is_running) {
        pthread_mutex_unlock(&handle->mutex);
        return 0; // Not running, nothing to do
    }
    
    handle->stop_requested = true;
    pthread_mutex_unlock(&handle->mutex);
    
    pthread_join(handle->thread_id, NULL);
    
    pthread_mutex_lock(&handle->mutex);
    handle->is_running = false;
    pthread_mutex_unlock(&handle->mutex);
    
    LOG_INFO("Playback stopped.");
    return 0;
}

int audio_player_is_playing(audio_player_handle handle) {
    if (!handle) return 0;
    pthread_mutex_lock(&handle->mutex);
    bool playing = handle->is_running;
    pthread_mutex_unlock(&handle->mutex);
    return playing;
}

int audio_player_set_volume(audio_player_handle handle, int volume) {
    if (!handle) return -1;
    pthread_mutex_lock(&handle->mutex);
    handle->hw_params.volume = volume;
    int ret = -1;
    if (handle->hw_initialized) {
        ret = hw_if_ao_set_volume(volume);
    }
    pthread_mutex_unlock(&handle->mutex);
    return ret;
}

// The core playback logic, running in its own thread
static void* player_thread_func(void *arg) {
    audio_player_handle handle = (audio_player_handle)arg;
    
    FILE *fp = fopen(handle->current_file, "rb");
    if (!fp) {
        LOG_ERROR("Failed to open audio file: %s", handle->current_file);
        return NULL;
    }

    wave_info_t winfo = {0};
    if (wave_parse_header(fp, &winfo) != 0) {
        LOG_ERROR("Failed to parse WAV header for %s", handle->current_file);
        fclose(fp);
        return NULL;
    }
    
    // After parsing, the file pointer is at the start of the data.
    // We save this position for looping.
    long data_start_offset = ftell(fp);

    // Configure hardware based on WAV file
    hw_audio_params_t new_params = handle->hw_params; // Copy current settings
    new_params.sample_rate = (AUDIO_SAMPLE_RATE_E)winfo.samples_per_sec;
    new_params.sound_mode = (winfo.channels == 1) ? AUDIO_SOUND_MODE_MONO : AUDIO_SOUND_MODE_STEREO;
    new_params.bit_width = (winfo.bits_per_sample == 16) ? AUDIO_BIT_WIDTH_16 : AUDIO_BIT_WIDTH_24; // Example

    pthread_mutex_lock(&handle->mutex);
    if (memcmp(&handle->hw_params, &new_params, sizeof(hw_audio_params_t)) != 0 || !handle->hw_initialized) {
        LOG_INFO("Audio parameters changed. Re-initializing hardware.");
        if (handle->hw_initialized) {
            hw_if_ao_deinit();
        }
        if (hw_if_ao_init(&new_params) == 0) {
            handle->hw_params = new_params;
            handle->hw_initialized = true;
        } else {
            LOG_ERROR("Failed to re-initialize hardware with new parameters.");
            pthread_mutex_unlock(&handle->mutex);
            fclose(fp);
            return NULL;
        }
    }
    pthread_mutex_unlock(&handle->mutex);

    // Main playback loop
    const size_t buffer_size = 4096;
    char *buffer = malloc(buffer_size);
    if (!buffer) {
        LOG_ERROR("Failed to allocate playback buffer.");
        fclose(fp);
        return NULL;
    }

    while (!handle->stop_requested) {
        size_t bytes_read = fread(buffer, 1, buffer_size, fp);
        if (bytes_read == 0) {
            if (feof(fp)) {
                LOG_INFO("End of file. Rewinding.");
                fseek(fp, data_start_offset, SEEK_SET); // Seek to the start of data
                continue;
            } else {
                LOG_ERROR("File read error.");
                break;
            }
        }
        
        VB_BLK vb_handle = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, bytes_read, NULL);
        if (vb_handle == VB_INVALID_HANDLE) {
            LOG_WARN("Failed to get VB block, skipping frame.");
            usleep(20 * 1000); // Wait a bit
            continue;
        }
        
        VB_POOL pool_id = AR_MPI_VB_Handle2PoolId(vb_handle);
        if (AR_MPI_VB_MmapPool(pool_id) != 0) {
            LOG_WARN("Failed to mmap VB pool.");
            AR_MPI_VB_ReleaseBlock(vb_handle);
            continue;
        }

        AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(vb_handle);
        void *virt_addr = NULL;
        if (AR_MPI_VB_GetBlockVirAddr(pool_id, phy_addr, &virt_addr) != 0) {
            LOG_WARN("Failed to get block virtual address.");
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(vb_handle);
            continue;
        }

        memcpy(virt_addr, buffer, bytes_read);
        
        AUDIO_FRAME_S frame = {0};
        frame.u32Len = bytes_read;
        frame.u64PhyAddr[0] = phy_addr;
        frame.enBitwidth = handle->hw_params.bit_width;
        frame.enSoundmode = handle->hw_params.sound_mode;
        
        if (hw_if_ao_send_frame(&frame) != 0) {
            LOG_WARN("Failed to send audio frame.");
        }
        
        AR_MPI_VB_MunmapPool(pool_id);
        AR_MPI_VB_ReleaseBlock(vb_handle);
    }
    
    free(buffer);
    fclose(fp);
    LOG_INFO("Player thread finished for %s.", handle->current_file);
    return NULL;
}