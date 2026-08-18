/**
 * @file audio_service_refactored.c
 * @brief Professional Audio Service Implementation
 * @details Refactored version with improved error handling, resource management,
 *          and adherence to C best practices.
 */

#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "audio_service_refactored.h"
#include "mpi_audio.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "acodec.h"
#include "hal_sys.h"

/*==============================================================================
 * GLOBAL CONTEXT
 *============================================================================*/

static audio_service_ctx_t g_audio_ctx = {
    .playback_mutex = PTHREAD_MUTEX_INITIALIZER,
    .service_exit = false,
    .playback_exit = true,
    .playback_thread_running = false,
    .ao_initialized = false,
    .ao_dev_id = AUDIO_DEV_ACODEC,
    .ao_channel = 0,
    .sample_rate = AUDIO_SAMPLE_RATE_48000,
    .sound_mode = AUDIO_SOUND_MODE_STEREO,
    .bit_width = AUDIO_BIT_WIDTH_16,
    .ao_volume = 5,
    .current_audio_file = {0},
    .vb_pool_id = POOL_OWNER_COMMON,
    .signal_count = 0
};

/*==============================================================================
 * UTILITY FUNCTIONS
 *============================================================================*/

AUDIO_SAMPLE_RATE_E get_sample_rate_enum(uint32_t sample_rate)
{
    switch(sample_rate) {
        case 8000:  return AUDIO_SAMPLE_RATE_8000;
        case 12000: return AUDIO_SAMPLE_RATE_12000;
        case 16000: return AUDIO_SAMPLE_RATE_16000;
        case 22050: return AUDIO_SAMPLE_RATE_22050;
        case 24000: return AUDIO_SAMPLE_RATE_24000;
        case 32000: return AUDIO_SAMPLE_RATE_32000;
        case 44100: return AUDIO_SAMPLE_RATE_44100;
        case 48000: return AUDIO_SAMPLE_RATE_48000;
        case 64000: return AUDIO_SAMPLE_RATE_64000;
        case 96000: return AUDIO_SAMPLE_RATE_96000;
        default:
            printf("[WARNING] Unsupported sample rate: %u Hz, using 48000 Hz\n", sample_rate);
            return AUDIO_SAMPLE_RATE_48000;
    }
}

bool is_sample_rate_supported(uint32_t sample_rate)
{
    switch(sample_rate) {
        case 8000: case 12000: case 16000: case 22050: case 24000:
        case 32000: case 44100: case 48000: case 64000: case 96000:
            return true;
        default:
            return false;
    }
}

/*==============================================================================
 * AUDIO DEVICE MANAGEMENT
 *============================================================================*/

AR_S32 ao_set_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume)
{
    int fd = open("/dev/acodec", O_RDWR);
    if (fd < 0) {
        printf("[WARNING] Failed to open acodec device for volume control\n");
        return AR_SUCCESS; // Non-critical error
    }
    
    // Volume setting implementation would go here
    // This is hardware-specific and depends on the acodec driver
    
    close(fd);
    return AR_SUCCESS;
}

AR_S32 ao_device_init(void)
{
    AIO_ATTR_S attr = {0};
    
    printf("[INFO] Initializing AO device (rate=%d, mode=%d)\n",
           g_audio_ctx.sample_rate, g_audio_ctx.sound_mode);
    
    // Configure audio attributes
    memset(&attr, 0, sizeof(AIO_ATTR_S));
    attr.enSamplerate = g_audio_ctx.sample_rate;
    attr.enBitwidth = g_audio_ctx.bit_width;
    attr.enWorkmode = AIO_MODE_I2S_MASTER;
    attr.enSoundmode = g_audio_ctx.sound_mode;
    attr.u32EXFlag = 0;
    attr.u32FrmNum = 8;
    attr.u32PtNumPerFrm = 2048;
    attr.u32ChnCnt = 2;
    attr.u32ClkSel = 0;
    
    // Set I2S type based on device ID (matching original logic)
    if (g_audio_ctx.ao_dev_id == AIO_ACODEC_DEV_ID)
        attr.enI2sType = AIO_I2STYPE_INNERCODEC;
    else
        attr.enI2sType = AIO_I2STYPE_EXTERN;
        
    // Set work mode for slave devices
    if (g_audio_ctx.ao_dev_id >= AIO_I2S_SLAVE0_ID && g_audio_ctx.ao_dev_id <= AIO_I2S_SLAVE1_ID)
        attr.enWorkmode = AIO_MODE_I2S_SLAVE;
    
    // Set device attributes
    CHECK_RESULT(AR_MPI_AO_SetPubAttr(g_audio_ctx.ao_dev_id, &attr), 
                 "AR_MPI_AO_SetPubAttr");
    
    // Enable device and channel
    CHECK_RESULT(AR_MPI_AO_Enable(g_audio_ctx.ao_dev_id), "AR_MPI_AO_Enable");
    CHECK_RESULT(AR_MPI_AO_EnableChn(g_audio_ctx.ao_dev_id, g_audio_ctx.ao_channel), 
                 "AR_MPI_AO_EnableChn");
    
    // Set volume
    ao_set_volume(g_audio_ctx.sample_rate, g_audio_ctx.ao_volume);
    
    g_audio_ctx.ao_initialized = true;
    printf("[INFO] AO device initialized successfully\n");
    
    return AR_SUCCESS;
}

AR_S32 ao_device_deinit(void)
{
    if (!g_audio_ctx.ao_initialized) {
        return AR_SUCCESS;
    }
    
    printf("[INFO] Deinitializing AO device\n");
    
    AR_S32 ret1 = AR_MPI_AO_DisableChn(g_audio_ctx.ao_dev_id, g_audio_ctx.ao_channel);
    AR_S32 ret2 = AR_MPI_AO_Disable(g_audio_ctx.ao_dev_id);
    
    if (ret1 != AR_SUCCESS) {
        printf("[ERROR] AR_MPI_AO_DisableChn failed: 0x%x\n", ret1);
    }
    if (ret2 != AR_SUCCESS) {
        printf("[ERROR] AR_MPI_AO_Disable failed: 0x%x\n", ret2);
    }
    
    g_audio_ctx.ao_initialized = false;
    return (ret1 == AR_SUCCESS && ret2 == AR_SUCCESS) ? AR_SUCCESS : AR_FAILURE;
}

/*==============================================================================
 * VB SYSTEM MANAGEMENT
 *============================================================================*/

static AR_S32 vb_system_init(void)
{
    VB_CONFIG_S vb_config = {0};
    VB_POOL_CONFIG_S pool_config = {0};
    AR_S32 ret;
    
    printf("[INFO] Initializing VB system\n");
    
    // Try common pool first
    vb_config.astCommPool[0].u64BlkSize = VB_BLOCK_SIZE;
    vb_config.astCommPool[0].u32BlkCnt = VB_BLOCK_COUNT;
    
    ret = AR_MPI_VB_SetConfig(&vb_config);
    if (ret != AR_SUCCESS) {
        printf("[INFO] Creating user VB pool\n");
        
        pool_config.u64BlkSize = VB_BLOCK_SIZE;
        pool_config.u32BlkCnt = VB_BLOCK_COUNT;
        pool_config.enRemapMode = VB_REMAP_MODE_NONE;
        
        g_audio_ctx.vb_pool_id = AR_MPI_VB_CreatePool(&pool_config);
        if (g_audio_ctx.vb_pool_id == VB_INVALID_POOLID) {
            printf("[ERROR] Failed to create VB pool\n");
            return AR_FAILURE;
        }
    }
    
    CHECK_RESULT(AR_MPI_VB_Init(), "AR_MPI_VB_Init");
    return AR_SUCCESS;
}

static void vb_system_cleanup(void)
{
    if (g_audio_ctx.vb_pool_id != VB_INVALID_POOLID && 
        g_audio_ctx.vb_pool_id != POOL_OWNER_COMMON) {
        AR_MPI_VB_DestroyPool(g_audio_ctx.vb_pool_id);
    }
    AR_MPI_VB_Exit();
}

/*==============================================================================
 * PLAYBACK CONTROL
 *============================================================================*/

void stop_current_playback(void)
{
    if (g_audio_ctx.playback_thread_running && !g_audio_ctx.playback_exit) {
        printf("[INFO] Stopping playback...\n");
        
        g_audio_ctx.playback_exit = true;
        pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
        pthread_join(g_audio_ctx.playback_thread, NULL);
        pthread_mutex_lock(&g_audio_ctx.playback_mutex);
        
        g_audio_ctx.playback_thread_running = false;
        printf("[INFO] Playback stopped\n");
    }
}

void* playback_thread_func(void* arg)
{
    (void)arg;
    pthread_detach(pthread_self());
    
    FILE* file = NULL;
    AUDIO_FRAME_S frame = {0};
    wave_file_header_t header = {0};
    AR_S32 data_len = 0;
    
    pthread_mutex_lock(&g_audio_ctx.playback_mutex);
    
    if (strlen(g_audio_ctx.current_audio_file) == 0) {
        printf("[ERROR] No audio file specified\n");
        goto cleanup;
    }
    
    file = fopen(g_audio_ctx.current_audio_file, "rb");
    if (!file) {
        printf("[ERROR] Cannot open file: %s\n", g_audio_ctx.current_audio_file);
        goto cleanup;
    }
    
    // Handle WAV files
    if (strstr(g_audio_ctx.current_audio_file, ".wav")) {
        if (fread(&header, 1, sizeof(header), file) != sizeof(header)) {
            printf("[ERROR] Failed to read WAV header\n");
            goto cleanup;
        }
        
        printf("[INFO] WAV: %dHz, %d channels, %d bits\n",
               header.format.samples_per_sec, header.format.channels, 
               header.format.bits_per_sample);
        
        data_len = header.data_len;
        
        // Validate and reconfigure if needed
        if (!is_sample_rate_supported(header.format.samples_per_sec)) {
            printf("[ERROR] Unsupported sample rate: %u Hz\n", header.format.samples_per_sec);
            goto cleanup;
        }
        
        AUDIO_SAMPLE_RATE_E req_rate = get_sample_rate_enum(header.format.samples_per_sec);
        AUDIO_SOUND_MODE_E req_mode = (header.format.channels == 1) ? 
                                     AUDIO_SOUND_MODE_MONO : AUDIO_SOUND_MODE_STEREO;
        
        if (g_audio_ctx.sample_rate != req_rate || g_audio_ctx.sound_mode != req_mode) {
            printf("[INFO] Reconfiguring audio device\n");
            
            if (g_audio_ctx.ao_initialized) {
                ao_device_deinit();
            }
            
            g_audio_ctx.sample_rate = req_rate;
            g_audio_ctx.sound_mode = req_mode;
            
            if (ao_device_init() != AR_SUCCESS) {
                printf("[ERROR] Failed to reinitialize AO device\n");
                goto cleanup;
            }
        }
    } else {
        // Raw file
        fseek(file, 0, SEEK_END);
        data_len = ftell(file);
        fseek(file, 0, SEEK_SET);
    }
    
    pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
    
    // Playback loop
    g_audio_ctx.playback_exit = false;
    while (!g_audio_ctx.playback_exit && !g_audio_ctx.service_exit) {
        VB_BLK block = AR_MPI_VB_GetBlock(g_audio_ctx.vb_pool_id, AUDIO_FRAME_SIZE, NULL);
        if (block == VB_INVALID_HANDLE) {
            printf("[ERROR] Failed to get VB block\n");
            break;
        }
        
        VB_POOL pool_id = AR_MPI_VB_Handle2PoolId(block);
        AR_U64 phy_addr = AR_MPI_VB_Handle2PhysAddr(block);
        if (pool_id < 0 || AR_MPI_VB_MmapPool(pool_id) != AR_SUCCESS) {
            AR_MPI_VB_ReleaseBlock(block);
            break;
        }
        
        void* virt_addr;
        AR_S32 ret = AR_MPI_VB_GetBlockVirAddr(pool_id, phy_addr, &virt_addr);
        if (ret != AR_SUCCESS) {
            printf("[ERROR] Failed to get virtual address: 0x%x\n", ret);
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(block);
            break;
        }
        if (!virt_addr) {
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(block);
            break;
        }
        
        size_t read_size = fread(virt_addr, 1, AUDIO_FRAME_SIZE, file);
        if (read_size == 0) {
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(block);
            break;
        }
        
        // Prepare and send frame
        memset(&frame, 0, sizeof(frame));
        frame.enBitwidth = g_audio_ctx.bit_width;
        frame.enSoundmode = g_audio_ctx.sound_mode;
        frame.u32PoolId[0] = pool_id;
        frame.u64VirAddr[0] = (uint64_t)virt_addr;
        
        if (g_audio_ctx.sound_mode == AUDIO_SOUND_MODE_STEREO) {
            frame.u32PoolId[1] = pool_id;
            frame.u64VirAddr[1] = (uint64_t)virt_addr;
        }
        
        AR_S32 send_ret = AR_MPI_AO_SendFrame(g_audio_ctx.ao_dev_id, g_audio_ctx.ao_channel, 
                                        &frame, 1000);
        if (send_ret != AR_SUCCESS) {
            printf("[ERROR] SendFrame failed: 0x%x\n", send_ret);
        }
        
        AR_MPI_VB_MunmapPool(pool_id);
        AR_MPI_VB_ReleaseBlock(block);
        usleep(10000);
    }
    
cleanup:
    if (file) fclose(file);
    
    pthread_mutex_lock(&g_audio_ctx.playback_mutex);
    g_audio_ctx.playback_thread_running = false;
    g_audio_ctx.playback_exit = true;
    pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
    
    printf("[INFO] Playback thread exited\n");
    return NULL;
}

/*==============================================================================
 * COMMAND PROCESSING
 *============================================================================*/

audio_command_e parse_command(const char* cmd_str, audio_command_data_t* cmd_data)
{
    char cmd[64] = {0};
    
    if (!cmd_str || !cmd_data) {
        return CMD_UNKNOWN;
    }
    
    memset(cmd_data, 0, sizeof(audio_command_data_t));
    
    if (sscanf(cmd_str, "%63s %511s %d", cmd, cmd_data->filename, &cmd_data->volume) >= 1) {
        if (strcmp(cmd, "play") == 0) {
            if (strlen(cmd_data->filename) == 0) {
                printf("[ERROR] Play command missing filename\n");
                return CMD_UNKNOWN;
            }
            if (cmd_data->volume <= 0) {
                cmd_data->volume = g_audio_ctx.ao_volume;
            }
            cmd_data->cmd = CMD_PLAY;
            return CMD_PLAY;
        }
        
        if (strcmp(cmd, "stop") == 0) {
            cmd_data->cmd = CMD_STOP;
            return CMD_STOP;
        }
    }
    
    return CMD_UNKNOWN;
}

/*==============================================================================
 * IPC SERVER
 *============================================================================*/

void* ipc_server_thread_func(void* arg)
{
    (void)arg;
    
    int server_fd = -1, client_fd = -1;
    struct sockaddr_un server_addr = {0};
    char buffer[MAX_COMMAND_LEN];
    audio_command_data_t cmd_data;
    
    // Remove existing socket
    unlink(SOCKET_PATH);
    
    // Create and setup socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return NULL;
    }
    
    server_addr.sun_family = AF_UNIX;
    SAFE_STRNCPY(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path));
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return NULL;
    }
    
    chmod(SOCKET_PATH, 0666);
    
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return NULL;
    }
    
    printf("[INFO] Audio service listening on %s\n", SOCKET_PATH);
    
    while (!g_audio_ctx.service_exit) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) continue;
            perror("accept");
            break;
        }
        
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            // Remove trailing newline
            char* newline = strchr(buffer, '\n');
            if (newline) *newline = '\0';
            
            printf("[INFO] Received command: %s\n", buffer);
            
            audio_command_e cmd = parse_command(buffer, &cmd_data);
            
            pthread_mutex_lock(&g_audio_ctx.playback_mutex);
            
            switch (cmd) {
                case CMD_PLAY:
                    // Stop current playback
                    stop_current_playback();
                    
                    // Set new file and volume
                    SAFE_STRNCPY(g_audio_ctx.current_audio_file, cmd_data.filename, 
                               sizeof(g_audio_ctx.current_audio_file));
                    g_audio_ctx.ao_volume = cmd_data.volume;
                    
                    // Initialize AO if needed
                    if (!g_audio_ctx.ao_initialized) {
                        if (ao_device_init() != AR_SUCCESS) {
                            write(client_fd, "ERROR: Failed to initialize audio device\n", 42);
                            pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
                            close(client_fd);
                            continue;
                        }
                    }
                    
                    // Start playback thread
                    if (pthread_create(&g_audio_ctx.playback_thread, NULL, 
                                     playback_thread_func, NULL) == 0) {
                        g_audio_ctx.playback_thread_running = true;
                        printf("[INFO] Started playback: %s (volume %d)\n", 
                               cmd_data.filename, cmd_data.volume);
                        write(client_fd, "OK\n", 3);
                    } else {
                        write(client_fd, "ERROR: Failed to start playback\n", 33);
                    }
                    break;
                    
                case CMD_STOP:
                    if (g_audio_ctx.playback_thread_running) {
                        stop_current_playback();
                        write(client_fd, "OK\n", 3);
                    } else {
                        write(client_fd, "No audio playing\n", 17);
                    }
                    break;
                    
                default:
                    write(client_fd, "ERROR: Unknown command\n", 23);
                    break;
            }
            
            pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    unlink(SOCKET_PATH);
    return NULL;
}

/*==============================================================================
 * SIGNAL HANDLING
 *============================================================================*/

void signal_handler(int signo, siginfo_t *info, void *data)
{
    (void)info;
    (void)data;
    
    printf("\n[INFO] Received signal %d\n", signo);
    g_audio_ctx.service_exit = true;
    g_audio_ctx.signal_count++;
    
    if (g_audio_ctx.signal_count > 2) {
        printf("[WARNING] Force exit after multiple signals\n");
        exit(1);
    }
}

/*==============================================================================
 * SERVICE LIFECYCLE
 *============================================================================*/

AR_S32 audio_service_init(void)
{
    printf("[INFO] Initializing audio service\n");
    
    // Initialize VB system
    if (vb_system_init() != AR_SUCCESS) {
        return AR_FAILURE;
    }
    
    // Initialize AO device
    if (ao_device_init() != AR_SUCCESS) {
        vb_system_cleanup();
        return AR_FAILURE;
    }
    
    printf("[INFO] Audio service initialized successfully\n");
    return AR_SUCCESS;
}

void audio_service_cleanup(void)
{
    printf("[INFO] Cleaning up audio service\n");
    
    // Stop playback
    pthread_mutex_lock(&g_audio_ctx.playback_mutex);
    stop_current_playback();
    pthread_mutex_unlock(&g_audio_ctx.playback_mutex);
    
    // Cleanup audio device
    ao_device_deinit();
    
    // Cleanup VB system
    vb_system_cleanup();
    
    printf("[INFO] Audio service cleanup completed\n");
}

AR_S32 audio_service_run(void)
{
    struct sigaction sa = {0};
    
    // Setup signal handlers
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    // Start IPC server thread
    if (pthread_create(&g_audio_ctx.ipc_thread, NULL, ipc_server_thread_func, NULL) != 0) {
        printf("[ERROR] Failed to create IPC server thread\n");
        return AR_FAILURE;
    }
    
    printf("[INFO] Audio service running (PID: %d)\n", getpid());
    
    // Wait for IPC thread to finish
    pthread_join(g_audio_ctx.ipc_thread, NULL);
    
    return AR_SUCCESS;
}

/*==============================================================================
 * MAIN FUNCTION
 *============================================================================*/

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    printf("=== Professional Audio Service v2.0 ===\n");
    
    // Initialize service
    if (audio_service_init() != AR_SUCCESS) {
        printf("[ERROR] Failed to initialize audio service\n");
        return EXIT_FAILURE;
    }
    
    // Run service
    AR_S32 result = audio_service_run();
    
    // Cleanup
    audio_service_cleanup();
    
    printf("[INFO] Audio service terminated\n");
    return (result == AR_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}
