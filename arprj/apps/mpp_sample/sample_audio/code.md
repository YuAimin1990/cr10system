好的，我们来对这个程序进行一次全面的重构。

原始代码是一个功能性的单文件程序，它成功地实现了一个通过Unix Socket接收命令来播放音频的服务。然而，正如您所指出的，它可以从更专业、更规范的软件工程实践中受益。

重构的目标是：
1.  **模块化 (Modularity)**：将代码分解为逻辑单元（模块），每个单元都有清晰的职责。
2.  **封装 (Encapsulation)**：隐藏实现细节，通过定义良好的API（头文件）进行交互，并使用不透明指针（Opaque Pointers）来管理状态。
3.  **状态管理 (State Management)**：消除混乱的全局变量，将它们组织到各个模块的状态结构体中。
4.  **健壮性 (Robustness)**：增强错误处理和并发控制，确保资源管理的正确性。
5.  **可读性与可维护性 (Readability & Maintainability)**：使用清晰的命名、一致的风格和适当的注释。

---

### 重构后的结构

我们将项目分解为以下几个模块：

*   **`main.c`**: 程序入口。职责非常简单：创建、运行和销毁主音频服务。
*   **`audio_service.h/.c`**: 核心服务模块。负责IPC（进程间通信）服务器的生命周期、接收和解析命令。它是“控制器”。
*   **`audio_player.h/.c`**: 音频播放器模块。负责管理播放线程、处理音频文件、与硬件接口交互。它是“执行者”。
*   **`wave_parser.h/.c`**: WAV文件解析器。一个专门的工具模块，用于从.wav文件中提取元数据。
*   **`audio_hw_if.h/.c`**: 硬件接口层。将所有对 `AR_MPI_*` 和 `acodec` 的调用包装起来，使上层代码与具体的SDK解耦。
*   **`utils.h`**: 存放通用的宏、定义和日志功能。
*   **`ipc_protocol.h`**: 定义IPC协议相关的常量，如socket路径和命令格式。

---

### `Makefile` (用于编译)

首先，这是一个用于编译所有这些文件的`Makefile`，它体现了项目的模块化结构。

```makefile
# Makefile for Audio Service

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I. -I/path/to/artosyn/sdk/include  # <- 修改为你的SDK头文件路径
LDFLAGS = -lpthread
LDLIBS = -L/path/to/artosyn/sdk/lib -lmpi_audio -lmpi_vb -lmpi_vo -lmpi_sys # <- 修改为你的SDK库文件路径

# Source files
SRCS = main.c \
       audio_service.c \
       audio_player.c \
       wave_parser.c \
       audio_hw_if.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = audio_service

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
```
**注意**: 你需要将 `-I/path/to/...` 和 `-L/path/to/...` 修改为你环境中实际的SDK路径。

---

### 重构后的代码

#### 1. `utils.h` (通用工具)

```c
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// A simple logging macro
#define LOG_INFO(fmt, ...)  fprintf(stdout, "[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) fprintf(stderr, "\033[0;31m[ERROR] %s:%d: " fmt "\033[0;39m\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  fprintf(stdout, "\033[0;33m[WARN] " fmt "\033[0;39m\n", ##__VA_ARGS__)


// A macro to check return values, similar to the original
#define CHECK_RET(expression, name) \
    do { \
        int ret_val = (expression); \
        if (0 != ret_val) { \
            LOG_ERROR("%s failed with code %#x!", name, ret_val); \
            return ret_val; \
        } \
    } while(0)

#endif // UTILS_H
```

#### 2. `ipc_protocol.h` (IPC协议定义)

```c
#ifndef IPC_PROTOCOL_H
#define IPC_PROTOCOL_H

#define SOCKET_PATH "/tmp/audio_service.sock"
#define MAX_COMMAND_LEN 1024
#define MAX_FILENAME_LEN 512

typedef enum {
    CMD_PLAY,
    CMD_STOP,
    CMD_SET_VOLUME,
    CMD_UNKNOWN
} command_type_t;

typedef struct {
    command_type_t type;
    char filename[MAX_FILENAME_LEN];
    int volume;
} command_t;

#endif // IPC_PROTOCOL_H
```

#### 3. `wave_parser.h`

```c
#ifndef WAVE_PARSER_H
#define WAVE_PARSER_H

#include <stdio.h>
#include <stdint.h>
#include "acodec.h" // For AUDIO_* enums

typedef struct {
    uint16_t format_tag;
    uint16_t channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint32_t data_len;
} wave_info_t;

/**
 * @brief Parses the header of a WAV file.
 * @param fp File pointer to the open WAV file.
 * @param info Pointer to a wave_info_t struct to be filled.
 * @return 0 on success, -1 on failure.
 */
int wave_parse_header(FILE *fp, wave_info_t *info);

#endif // WAVE_PARSER_H
```

#### 4. `wave_parser.c`

```c
#include "wave_parser.h"
#include <string.h>
#include "utils.h"

// Internal WAV header structures
#pragma pack(push, 1)
typedef struct {
    uint16_t wFormatTag;
    uint16_t wChannels;
    uint32_t dwSamplesPerSec;
    uint32_t dwAvgBytesPerSec;
    uint16_t wBlockAlign;
    uint16_t wBitsPerSample;
} WaveFormat;

typedef struct {
    char chRIFF[4];
    uint32_t dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    uint32_t dwFMTLen;
    WaveFormat wave;
} WaveFileHeader;
#pragma pack(pop)

int wave_parse_header(FILE *fp, wave_info_t *info) {
    if (!fp || !info) {
        return -1;
    }

    WaveFileHeader header;
    rewind(fp);

    if (fread(&header, 1, sizeof(header), fp) != sizeof(header)) {
        LOG_ERROR("Failed to read WAV header.");
        return -1;
    }

    if (strncmp(header.chRIFF, "RIFF", 4) != 0 || strncmp(header.chWAVE, "WAVE", 4) != 0) {
        LOG_ERROR("Not a valid WAV file.");
        return -1;
    }
    
    // Find the 'data' chunk
    char chunk_id[4];
    uint32_t chunk_size;
    while (fread(chunk_id, 1, 4, fp) == 4) {
        if (fread(&chunk_size, 1, 4, fp) != 4) {
             LOG_ERROR("Failed to read chunk size.");
             return -1;
        }
        if (strncmp(chunk_id, "data", 4) == 0) {
             info->data_len = chunk_size;
             break;
        }
        // Skip this chunk's data
        fseek(fp, chunk_size, SEEK_CUR);
    }
    
    if (info->data_len == 0) {
        LOG_ERROR("Could not find 'data' chunk in WAV file.");
        return -1;
    }

    info->format_tag = header.wave.wFormatTag;
    info->channels = header.wave.wChannels;
    info->samples_per_sec = header.wave.dwSamplesPerSec;
    info->avg_bytes_per_sec = header.wave.dwAvgBytesPerSec;
    info->block_align = header.wave.wBlockAlign;
    info->bits_per_sample = header.wave.wBitsPerSample;

    LOG_INFO("WAV Info: %u Hz, %d-bit, %d-ch", info->samples_per_sec, info->bits_per_sample, info->channels);
    
    // The file pointer is now at the start of the audio data.
    return 0;
}
```

#### 5. `audio_hw_if.h` (Hardware Interface)

```c
#ifndef AUDIO_HW_IF_H
#define AUDIO_HW_IF_H

#include "acodec.h" // For AUDIO_* enums

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
```

#### 6. `audio_hw_if.c`

```c
#include "audio_hw_if.h"
#include "utils.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "mpi_audio.h"
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

    CHECK_RET(AR_MPI_VB_SetConfig(&vb_config), "AR_MPI_VB_SetConfig");
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
```

#### 7. `audio_player.h`

```c
#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

// Opaque pointer to the internal state
typedef struct audio_player_s* audio_player_handle;

/**
 * @brief Creates an audio player instance.
 * @return Handle to the player, or NULL on failure.
 */
audio_player_handle audio_player_create(void);

/**
 * @brief Destroys the audio player instance and cleans up resources.
 * @param handle Handle to the player.
 */
void audio_player_destroy(audio_player_handle handle);

/**
 * @brief Starts playing an audio file. If a file is already playing, it will be stopped first.
 * @param handle Handle to the player.
 * @param filename Path to the audio file.
 * @param volume Initial volume (device-specific range).
 * @return 0 on success, -1 on failure.
 */
int audio_player_play(audio_player_handle handle, const char *filename, int volume);

/**
 * @brief Stops the current playback.
 * @param handle Handle to the player.
 * @return 0 on success.
 */
int audio_player_stop(audio_player_handle handle);

/**
 * @brief Checks if the player is currently playing.
 * @param handle Handle to the player.
 * @return 1 if playing, 0 otherwise.
 */
int audio_player_is_playing(audio_player_handle handle);

/**
 * @brief Sets the playback volume.
 * @param handle Handle to the player.
 * @param volume The new volume.
 * @return 0 on success, -1 on failure.
 */
int audio_player_set_volume(audio_player_handle handle, int volume);

#endif // AUDIO_PLAYER_H
```

#### 8. `audio_player.c`

```c
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
    audio_player_stop(handle);
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
                fseek(fp, sizeof(wave_info_t), SEEK_SET); // Seek past header
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
        
        void *virt_addr = AR_MPI_VB_Mmap(vb_handle, bytes_read);
        if (!virt_addr) {
             LOG_WARN("Failed to mmap VB block.");
             AR_MPI_VB_ReleaseBlock(vb_handle);
             continue;
        }

        memcpy(virt_addr, buffer, bytes_read);
        
        AUDIO_FRAME_S frame = {0};
        frame.u32Len = bytes_read;
        frame.u64PhyAddr[0] = AR_MPI_VB_Handle2PhysAddr(vb_handle);
        frame.enBitwidth = handle->hw_params.bit_width;
        frame.enSoundmode = handle->hw_params.sound_mode;
        
        if (hw_if_ao_send_frame(&frame) != 0) {
            LOG_WARN("Failed to send audio frame.");
        }
        
        AR_MPI_VB_Munmap(virt_addr, bytes_read);
        AR_MPI_VB_ReleaseBlock(vb_handle);
    }
    
    free(buffer);
    fclose(fp);
    LOG_INFO("Player thread finished for %s.", handle->current_file);
    return NULL;
}
```

#### 9. `audio_service.h`

```c
#ifndef AUDIO_SERVICE_H
#define AUDIO_SERVICE_H

// Opaque pointer
typedef struct audio_service_s* audio_service_handle;

audio_service_handle audio_service_create(void);
void audio_service_destroy(audio_service_handle handle);

/**
 * @brief Runs the audio service. This is a blocking call.
 *        It starts listening for IPC commands and will run until
 *        audio_service_stop() is called from another thread (e.g., signal handler).
 * @param handle Handle to the service.
 * @return 0 on normal exit, -1 on failure.
 */
int audio_service_run(audio_service_handle handle);

/**
 * @brief Requests the audio service to stop.
 * @param handle Handle to the service.
 */
void audio_service_stop(audio_service_handle handle);

#endif // AUDIO_SERVICE_H
```

#### 10. `audio_service.c`

```c
#include "audio_service.h"
#include "audio_player.h"
#include "ipc_protocol.h"
#include "utils.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

struct audio_service_s {
    audio_player_handle player;
    volatile bool stop_requested;
    int server_fd;
};

static int parse_ipc_command(const char *buffer, command_t *cmd) {
    // A more robust parser than sscanf
    char cmd_str[32];
    char arg1[MAX_FILENAME_LEN];
    int arg2;

    if (sscanf(buffer, "%31s %511s %d", cmd_str, arg1, &arg2) == 3 && strcmp(cmd_str, "play") == 0) {
        cmd->type = CMD_PLAY;
        strncpy(cmd->filename, arg1, MAX_FILENAME_LEN - 1);
        cmd->volume = arg2;
    } else if (sscanf(buffer, "%31s %d", cmd_str, &arg2) == 2 && strcmp(cmd_str, "volume") == 0) {
        cmd->type = CMD_SET_VOLUME;
        cmd->volume = arg2;
    } else if (sscanf(buffer, "%31s", cmd_str) == 1 && strcmp(cmd_str, "stop") == 0) {
        cmd->type = CMD_STOP;
    } else {
        cmd->type = CMD_UNKNOWN;
        return -1;
    }
    return 0;
}

static void handle_client_connection(int client_fd, audio_player_handle player) {
    char buffer[MAX_COMMAND_LEN];
    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    
    command_t cmd;
    parse_ipc_command(buffer, &cmd);
    
    const char *response = "OK\n";
    switch (cmd.type) {
        case CMD_PLAY:
            LOG_INFO("IPC: Received PLAY command for '%s' at volume %d", cmd.filename, cmd.volume);
            if (audio_player_play(player, cmd.filename, cmd.volume) != 0) {
                response = "ERROR: Failed to start playback\n";
            }
            break;
        case CMD_STOP:
            LOG_INFO("IPC: Received STOP command");
            audio_player_stop(player);
            break;
        case CMD_SET_VOLUME:
            LOG_INFO("IPC: Received SET_VOLUME command to %d", cmd.volume);
            if (audio_player_set_volume(player, cmd.volume) != 0) {
                 response = "ERROR: Failed to set volume\n";
            }
            break;
        default:
            LOG_WARN("IPC: Received unknown command");
            response = "ERROR: Unknown command\n";
            break;
    }
    write(client_fd, response, strlen(response));
    close(client_fd);
}

audio_service_handle audio_service_create(void) {
    audio_service_handle handle = calloc(1, sizeof(struct audio_service_s));
    if (!handle) {
        LOG_ERROR("Failed to allocate service handle.");
        return NULL;
    }
    handle->player = audio_player_create();
    if (!handle->player) {
        free(handle);
        return NULL;
    }
    handle->server_fd = -1;
    return handle;
}

void audio_service_destroy(audio_service_handle handle) {
    if (!handle) return;
    if (handle->server_fd != -1) {
        close(handle->server_fd);
        unlink(SOCKET_PATH);
    }
    audio_player_destroy(handle->player);
    free(handle);
}

void audio_service_stop(audio_service_handle handle) {
    if (handle) {
        handle->stop_requested = true;
    }
}

int audio_service_run(audio_service_handle handle) {
    struct sockaddr_un addr;
    
    unlink(SOCKET_PATH);
    handle->server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (handle->server_fd < 0) {
        LOG_ERROR("socket() failed: %s", strerror(errno));
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    if (bind(handle->server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR("bind() failed: %s", strerror(errno));
        return -1;
    }
    
    chmod(SOCKET_PATH, 0666);
    
    if (listen(handle->server_fd, 5) < 0) {
        LOG_ERROR("listen() failed: %s", strerror(errno));
        return -1;
    }

    LOG_INFO("Audio service listening on %s", SOCKET_PATH);

    while (!handle->stop_requested) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(handle->server_fd, &read_fds);
        
        struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
        
        int ret = select(handle->server_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ret < 0) {
            if (errno == EINTR) continue; // Interrupted by signal
            LOG_ERROR("select() failed: %s", strerror(errno));
            break;
        }
        
        if (ret > 0 && FD_ISSET(handle->server_fd, &read_fds)) {
            int client_fd = accept(handle->server_fd, NULL, NULL);
            if (client_fd >= 0) {
                handle_client_connection(client_fd, handle->player);
            }
        }
    }
    
    LOG_INFO("Audio service shutting down.");
    return 0;
}
```

#### 11. `main.c`

```c
#include "audio_service.h"
#include "audio_hw_if.h"
#include "utils.h"

#include <signal.h>
#include <stdlib.h>

// Global handle for the signal handler to access
static audio_service_handle g_service_handle = NULL;

static void signal_handler(int signum) {
    LOG_INFO("Caught signal %d, requesting service shutdown.", signum);
    if (g_service_handle) {
        audio_service_stop(g_service_handle);
    }
    // To handle forceful exit if shutdown takes too long
    static int quit_count = 0;
    quit_count++;
    if (quit_count > 2) {
        LOG_WARN("Forcing exit now!");
        exit(1);
    }
}

static int setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Don't restart syscalls
    
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        LOG_ERROR("Failed to set up signal handlers.");
        return -1;
    }
    return 0;
}

int main(void) {
    LOG_INFO("Audio Service starting...");

    if (setup_signal_handlers() != 0) {
        return EXIT_FAILURE;
    }

    if (hw_if_system_init() != 0) {
        LOG_ERROR("Failed to initialize hardware system.");
        return EXIT_FAILURE;
    }
    
    g_service_handle = audio_service_create();
    if (!g_service_handle) {
        LOG_ERROR("Failed to create the audio service.");
        hw_if_system_deinit();
        return EXIT_FAILURE;
    }
    
    // This is a blocking call
    audio_service_run(g_service_handle);
    
    LOG_INFO("Cleaning up resources...");
    audio_service_destroy(g_service_handle);
    hw_if_system_deinit();
    
    LOG_INFO("Audio Service stopped cleanly.");
    return EXIT_SUCCESS;
}
```