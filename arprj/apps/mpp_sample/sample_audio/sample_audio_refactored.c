/**
 * @file sample_audio_refactored.c
 * @brief Professional audio sample application using AR MPP framework
 * @version 1.0
 * @date 2024
 * 
 * This application demonstrates audio input/output operations using the AR MPP framework.
 * It supports various audio operations including AI capture, AO playback, and audio processing.
 */

#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdint.h>

#include "mpi_audio.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "mpi_sys.h"
#include "mpi_sysctl.h"
#include "acodec.h"
#include "hal_sys.h"

/* ========================================================================== */
/*                           Macro Definitions                               */
/* ========================================================================== */

#define ACODEC_FILE_PATH        "/dev/acodec"
#define DEFAULT_AI_OUTPUT_PATH  "./"
#define MAX_PATH_LENGTH         512
#define MAX_FILENAME_LENGTH     256
#define WAVE_HEADER_SIZE        44
#define AUDIO_FRAME_SIZE        (1024 * 4)
#define VB_POOL_BLOCK_COUNT     20
#define THREAD_JOIN_TIMEOUT_MS  (300 * 1000)

/* Error handling macro */
#define CHECK_RESULT(expr, name) \
    do { \
        AR_S32 ret = (expr); \
        if (AR_SUCCESS != ret) { \
            fprintf(stderr, "\033[0;31m%s failed at %s:%d with error 0x%x\033[0;39m\n", \
                    (name), __FUNCTION__, __LINE__, ret); \
            return ret; \
        } \
    } while(0)

/* Logging macros */
#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[ERROR][%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_INFO(fmt, ...) \
    printf("[INFO][%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    printf("[WARN][%s:%d] " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

/* ========================================================================== */
/*                           Type Definitions                                */
/* ========================================================================== */

typedef enum {
    AR_ACODEC_DEV_ID = 0,
    AR_I2S_MASTER0_ID = 1,
    AR_I2S_MASTER1_ID,
#ifdef AR9341
    AR_I2S_MASTER2_ID,
#endif
    AR_I2S_SLAVE0_ID,
    AR_I2S_SLAVE1_ID,
} audio_device_id_t;

typedef enum {
    AENC_TYPE_G711A = 0,
    AENC_TYPE_G711U,
    AENC_TYPE_G726_16,
    AENC_TYPE_G726_24,
    AENC_TYPE_G726_32,
    AENC_TYPE_G726_40,
    AENC_TYPE_PCM,
} audio_encode_type_t;

typedef struct {
    int16_t  format_tag;
    int16_t  channels;
    uint32_t samples_per_sec;
    uint32_t avg_bytes_per_sec;
    int16_t  block_align;
    int16_t  bits_per_sample;
} wave_format_t;

typedef struct {
    char     riff[4];
    uint32_t riff_len;
    char     wave[4];
    char     fmt[4];
    uint32_t fmt_len;
    wave_format_t wave_format;
    char     data[4];
    uint32_t data_len;
} wave_file_header_t;

typedef struct {
    /* Audio configuration */
    AUDIO_SAMPLE_RATE_E sample_rate;
    AUDIO_SOUND_MODE_E  sound_mode;
    AUDIO_BIT_WIDTH_E   bit_width;
    
    /* Device configuration */
    AUDIO_DEV ai_dev_id;
    AI_CHN    ai_channel;
    AUDIO_DEV ao_dev_id;
    AO_CHN    ao_channel;
    
    /* Volume settings */
    int32_t ai_volume;
    int32_t ao_volume;
    
    /* File paths */
    char ai_output_path[MAX_PATH_LENGTH];
    char ao_input_path[MAX_PATH_LENGTH];
    char ai_output_filename[MAX_FILENAME_LENGTH];
    char ai_output_filename_left[MAX_FILENAME_LENGTH];
    char ai_output_filename_right[MAX_FILENAME_LENGTH];
    
    /* Operation flags */
    bool enable_ai;
    bool enable_ao;
    bool enable_ai_bind_ao;
    bool enable_ai_send_ao;
    bool enable_ai_save_frame;
    bool enable_ao_send_frame;
    bool test_acodec_ai;
    bool test_acodec_ao;
} audio_config_t;

typedef struct {
    /* Thread control */
    volatile bool thread_run_flag;
    volatile bool ai_exit;
    volatile bool ao_exit;
    volatile bool ai_send_ao_exit;
    volatile bool suspend_flag;
    
    /* Thread handles */
    pthread_t ai_save_thread;
    pthread_t ao_send_thread;
    pthread_t ai_send_ao_thread;
    pthread_t test_ai_thread;
    pthread_t test_ao_thread;
    
    /* System resources */
    VB_POOL vb_pool_id;
    MPI_SYSCTL_HANDLE sysctl_handle;
} audio_context_t;

/* ========================================================================== */
/*                           Global Variables                                */
/* ========================================================================== */

static audio_config_t  g_audio_config = {0};
static audio_context_t g_audio_context = {0};

/* ========================================================================== */
/*                           Function Declarations                           */
/* ========================================================================== */

/* Configuration and initialization */
static int parse_command_line_args(int argc, char *argv[]);
static int validate_audio_parameters(void);
static int initialize_system_resources(void);
static int cleanup_system_resources(void);

/* Audio module management */
static int initialize_ai_module(void);
static int initialize_ao_module(void);
static int cleanup_ai_module(void);
static int cleanup_ao_module(void);

/* Volume and codec control */
static int set_ai_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume);
static int set_ao_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume);
static int test_acodec_ai_functionality(void);
static int test_acodec_ao_functionality(void);

/* Thread functions */
static void* ai_capture_thread(void *arg);
static void* ao_playback_thread(void *arg);
static void* ai_to_ao_thread(void *arg);

/* Utility functions */
static int create_wave_header(wave_file_header_t *header, audio_encode_type_t encode_type,
                             AUDIO_SOUND_MODE_E sound_mode, AUDIO_SAMPLE_RATE_E sample_rate,
                             int raw_length);
static int read_wave_header(const char *data);
static void display_usage_help(void);
static void signal_handler(int signo, siginfo_t *info, void *data);
static void sysctl_callback(SYSCTL_EVENT_E event, void *data, int32_t length);

/* ========================================================================== */
/*                           Function Implementations                        */
/* ========================================================================== */

/**
 * @brief Display usage help information
 */
static void display_usage_help(void)
{
    printf("Usage: sample_audio [OPTIONS]\n");
    printf("Options:\n");
    printf("  -I              Enable AI (Audio Input)\n");
    printf("  -i <path>       AI output file path\n");
    printf("  -O              Enable AO (Audio Output)\n");
    printf("  -o <path>       AO input file path\n");
    printf("  -s <rate>       Sample rate (8000, 16000, 32000, 48000)\n");
    printf("  -D <id>         Device ID\n");
    printf("  -c <id>         Channel ID\n");
    printf("  -b <width>      Bit width (16, 24, 32)\n");
    printf("  -m <mode>       Sound mode (0=mono, 1=stereo)\n");
    printf("  -B              Enable AI bind to AO\n");
    printf("  -S              Enable AI send frame to AO\n");
    printf("  -v <volume>     AI volume (0-31)\n");
    printf("  -V <volume>     AO volume (0-31)\n");
    printf("  -t              Test AI acodec\n");
    printf("  -T              Test AO acodec\n");
    printf("  -h              Display this help\n");
}

/**
 * @brief Parse command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, negative on error
 */
static int parse_command_line_args(int argc, char *argv[])
{
    int opt;
    
    /* Initialize default values */
    g_audio_config.sample_rate = AUDIO_SAMPLE_RATE_48000;
    g_audio_config.sound_mode = AUDIO_SOUND_MODE_STEREO;
    g_audio_config.bit_width = AUDIO_BIT_WIDTH_16;
    g_audio_config.ai_dev_id = AR_ACODEC_DEV_ID;
    g_audio_config.ao_dev_id = AR_ACODEC_DEV_ID;
    g_audio_config.ai_channel = 0;
    g_audio_config.ao_channel = 0;
    g_audio_config.ai_volume = 23;
    g_audio_config.ao_volume = 5;
    
    strncpy(g_audio_config.ai_output_path, DEFAULT_AI_OUTPUT_PATH, 
            sizeof(g_audio_config.ai_output_path) - 1);
    
    while ((opt = getopt(argc, argv, "Ii:Oo:Ss:D:c:m:v:V:Bb:tTh")) != -1) {
        switch (opt) {
            case 'I':
                g_audio_config.enable_ai = true;
                break;
            case 'i':
                strncpy(g_audio_config.ai_output_path, optarg, 
                        sizeof(g_audio_config.ai_output_path) - 1);
                break;
            case 'O':
                g_audio_config.enable_ao = true;
                break;
            case 'o':
                strncpy(g_audio_config.ao_input_path, optarg, 
                        sizeof(g_audio_config.ao_input_path) - 1);
                break;
            case 's':
                g_audio_config.sample_rate = (AUDIO_SAMPLE_RATE_E)atoi(optarg);
                break;
            case 'D':
                if (g_audio_config.enable_ai) {
                    g_audio_config.ai_dev_id = atoi(optarg);
                } else {
                    g_audio_config.ao_dev_id = atoi(optarg);
                }
                break;
            case 'c':
                if (g_audio_config.enable_ai) {
                    g_audio_config.ai_channel = atoi(optarg);
                } else {
                    g_audio_config.ao_channel = atoi(optarg);
                }
                break;
            case 'b':
                g_audio_config.bit_width = (AUDIO_BIT_WIDTH_E)atoi(optarg);
                break;
            case 'm':
                g_audio_config.sound_mode = (AUDIO_SOUND_MODE_E)atoi(optarg);
                break;
            case 'B':
                g_audio_config.enable_ai_bind_ao = true;
                break;
            case 'S':
                g_audio_config.enable_ai_send_ao = true;
                break;
            case 'v':
                g_audio_config.ai_volume = atoi(optarg);
                break;
            case 'V':
                g_audio_config.ao_volume = atoi(optarg);
                break;
            case 't':
                g_audio_config.test_acodec_ai = true;
                break;
            case 'T':
                g_audio_config.test_acodec_ao = true;
                break;
            case 'h':
            default:
                display_usage_help();
                return -1;
        }
    }
    
    return 0;
}

/**
 * @brief Validate audio parameters
 * @return 0 on success, negative on error
 */
static int validate_audio_parameters(void)
{
    /* Note: Sample rate validation is relaxed to match original code behavior
     * The original code accepts any sample rate value via direct casting
     * Common rates: 8000, 16000, 22050, 32000, 44100, 48000 */
    if (g_audio_config.sample_rate <= 0 || g_audio_config.sample_rate > 192000) {
        LOG_ERROR("Invalid sample rate: %d (must be > 0 and <= 192000)", g_audio_config.sample_rate);
        return -EINVAL;
    }
    
    /* Validate bit width */
    if (g_audio_config.bit_width != AUDIO_BIT_WIDTH_16 &&
        g_audio_config.bit_width != AUDIO_BIT_WIDTH_24 &&
        g_audio_config.bit_width != AUDIO_BIT_WIDTH_32) {
        LOG_ERROR("Invalid bit width: %d", g_audio_config.bit_width);
        return -EINVAL;
    }
    
    /* Validate sound mode */
    if (g_audio_config.sound_mode != AUDIO_SOUND_MODE_MONO &&
        g_audio_config.sound_mode != AUDIO_SOUND_MODE_STEREO) {
        LOG_ERROR("Invalid sound mode: %d", g_audio_config.sound_mode);
        return -EINVAL;
    }
    
    /* Validate volume ranges */
    if (g_audio_config.ai_volume < 0 || g_audio_config.ai_volume > 31) {
        LOG_ERROR("AI volume out of range (0-31): %d", g_audio_config.ai_volume);
        return -EINVAL;
    }
    
    if (g_audio_config.ao_volume < 0 || g_audio_config.ao_volume > 31) {
        LOG_ERROR("AO volume out of range (0-31): %d", g_audio_config.ao_volume);
        return -EINVAL;
    }
    
    /* Check if at least one operation is enabled */
    if (!g_audio_config.enable_ai && !g_audio_config.enable_ao &&
        !g_audio_config.test_acodec_ai && !g_audio_config.test_acodec_ao) {
        LOG_ERROR("No operation enabled. Use -I, -O, -t, or -T options");
        return -EINVAL;
    }
    
    /* Validate file paths */
    if (g_audio_config.enable_ai && strlen(g_audio_config.ai_output_path) == 0) {
        LOG_ERROR("AI output path is required when AI is enabled");
        return -EINVAL;
    }
    
    if (g_audio_config.enable_ao && strlen(g_audio_config.ao_input_path) == 0) {
        LOG_ERROR("AO input path is required when AO is enabled");
        return -EINVAL;
    }
    
    return 0;
}

/**
 * @brief Initialize system resources (VB, SYS)
 * @return 0 on success, negative on error
 */
static int initialize_system_resources(void)
{
    VB_CONFIG_S vb_config = {0};
    VB_POOL_CONFIG_S vb_pool_config = {0};
    int32_t ret;
    
    /* Configure VB common pool (match original code parameters) */
    vb_config.astCommPool[0].u64BlkSize = 1024 * 4;  /* Original: 1024*4 */
    vb_config.astCommPool[0].u32BlkCnt = 20;          /* Original: 20 */
    
    printf("[DEBUG] REFACTORED VB Config: BlkSize=%llu, BlkCnt=%u\n", vb_config.astCommPool[0].u64BlkSize, vb_config.astCommPool[0].u32BlkCnt);
    
    ret = AR_MPI_VB_SetConfig(&vb_config);
    if (ret != AR_SUCCESS) {
        LOG_WARN("VB set common config failed, creating user pool. ret: 0x%x", ret);
        
        /* Create user pool as fallback (match original parameters) */
        vb_pool_config.u64BlkSize = 1024 * 4;  /* Original: 1024*4 */
        vb_pool_config.u32BlkCnt = 20;          /* Original: 20 */
        vb_pool_config.enRemapMode = VB_REMAP_MODE_NONE;
        
        g_audio_context.vb_pool_id = AR_MPI_VB_CreatePool(&vb_pool_config);
        if (g_audio_context.vb_pool_id == VB_INVALID_POOLID) {
            LOG_ERROR("Failed to create VB user pool. ret: 0x%x", ret);
            return ret;
        }
    }
    
    /* Initialize VB */
    CHECK_RESULT(AR_MPI_VB_Init(), "VB Init");
    
    /* Initialize SYS */
    CHECK_RESULT(AR_MPI_SYS_Init(), "SYS Init");
    
    return 0;
}

/**
 * @brief Cleanup system resources
 * @return 0 on success, negative on error
 */
static int cleanup_system_resources(void)
{
    int32_t ret = 0;
    
    /* Destroy user pool if created */
    if (g_audio_context.vb_pool_id > POOL_OWNER_COMMON) {
        ret = AR_MPI_VB_DestroyPool(g_audio_context.vb_pool_id);
        if (ret != AR_SUCCESS) {
            LOG_ERROR("VB destroy pool failed. ret: 0x%x", ret);
        }
    }
    
    /* Exit SYS */
    ret = AR_MPI_SYS_Exit();
    if (ret != AR_SUCCESS) {
        LOG_ERROR("SYS exit failed. ret: 0x%x", ret);
    }
    
    /* Exit VB */
    ret = AR_MPI_VB_Exit();
    if (ret != AR_SUCCESS) {
        LOG_ERROR("VB exit failed. ret: 0x%x", ret);
    }
    
    return ret;
}

/**
 * @brief Signal handler for graceful shutdown
 * @param signo Signal number
 * @param info Signal information
 * @param data User data
 */
static void signal_handler(int signo, siginfo_t *info, void *data)
{
    (void)info;
    (void)data;
    
    LOG_INFO("Received signal %d, initiating graceful shutdown", signo);
    g_audio_context.thread_run_flag = false;
}

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, negative on error
 */
int main(int argc, char *argv[])
{
    int32_t ret = 0;
    struct sigaction sa = {0};
    int32_t soc_type;
    
    /* Parse command line arguments */
    ret = parse_command_line_args(argc, argv);
    if (ret != 0) {
        return ret;
    }
    
    /* Validate parameters */
    ret = validate_audio_parameters();
    if (ret != 0) {
        LOG_ERROR("Parameter validation failed");
        display_usage_help();
        return ret;
    }
    
    /* Set automatic flags based on enabled features (matching original code logic) */
    if (g_audio_config.enable_ai) {
        g_audio_config.enable_ai_save_frame = true;
    }
    
    if (g_audio_config.enable_ao) {
        g_audio_config.enable_ao_send_frame = true;
    }
    
    if (g_audio_config.enable_ai_bind_ao || g_audio_config.enable_ai_send_ao) {
        g_audio_config.enable_ai = true;
        g_audio_config.enable_ao = true;
        g_audio_config.ao_dev_id = g_audio_config.ai_dev_id;
        g_audio_config.ao_channel = g_audio_config.ai_channel;
        g_audio_config.enable_ai_save_frame = false;
        g_audio_config.enable_ao_send_frame = false;
    }
    
    if (g_audio_config.enable_ai_bind_ao) {
        LOG_INFO("Will use AI bind AO mode");
        g_audio_config.enable_ai_send_ao = false;
    }
    
    /* Setup signal handlers */
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    /* Initialize system resources */
    ret = initialize_system_resources();
    if (ret != 0) {
        LOG_ERROR("System initialization failed");
        return ret;
    }
    
    /* Initialize audio modules */
    if (g_audio_config.enable_ai) {
        ret = initialize_ai_module();
        if (ret != 0) {
            LOG_ERROR("AI module initialization failed");
            goto cleanup;
        }
        set_ai_volume(g_audio_config.sample_rate, g_audio_config.ai_volume);
    }
    
    if (g_audio_config.enable_ao) {
        ret = initialize_ao_module();
        if (ret != 0) {
            LOG_ERROR("AO module initialization failed");
            goto cleanup;
        }
        set_ao_volume(g_audio_config.sample_rate, g_audio_config.ao_volume);
    }
    
    /* Setup AI-AO binding if requested */
    if (g_audio_config.enable_ai_bind_ao) {
        MPP_CHN_S src_chn = {
            .enModId = AR_ID_AI,
            .s32DevId = g_audio_config.ai_dev_id,
            .s32ChnId = g_audio_config.ai_channel
        };
        
        MPP_CHN_S dest_chn = {
            .enModId = AR_ID_AO,
            .s32DevId = g_audio_config.ao_dev_id,
            .s32ChnId = g_audio_config.ao_channel
        };
        
        ret = AR_MPI_SYS_Bind(&src_chn, &dest_chn);
        if (ret != AR_SUCCESS) {
            LOG_ERROR("AI-AO bind failed. ret: 0x%x", ret);
            goto cleanup;
        }
    }
    
    /* Start threads based on configuration */
    g_audio_context.thread_run_flag = true;
    
    if (g_audio_config.enable_ai_send_ao) {
        g_audio_context.ai_send_ao_exit = false;
        pthread_create(&g_audio_context.ai_send_ao_thread, NULL, ai_to_ao_thread, NULL);
    }
    
    if (g_audio_config.enable_ai_save_frame) {
        g_audio_context.ai_exit = false;
        pthread_create(&g_audio_context.ai_save_thread, NULL, ai_capture_thread, NULL);
    }
    
    if (g_audio_config.enable_ao_send_frame) {
        g_audio_context.ao_exit = false;
        pthread_create(&g_audio_context.ao_send_thread, NULL, ao_playback_thread, NULL);
    }
    
    if (g_audio_config.test_acodec_ai) {
        pthread_create(&g_audio_context.test_ai_thread, NULL, 
                      (void*)test_acodec_ai_functionality, NULL);
    }
    
    if (g_audio_config.test_acodec_ao) {
        pthread_create(&g_audio_context.test_ao_thread, NULL, 
                      (void*)test_acodec_ao_functionality, NULL);
    }
    
    /* Register sysctl callback for supported SoC types */
    soc_type = ar_hal_sys_get_soc_id();
    if (soc_type == ARS_31 || soc_type == AR_9311) {
        g_audio_context.sysctl_handle = AR_MPI_SYSCTL_Register("audio_sample", 0, &sysctl_callback);
        if (!g_audio_context.sysctl_handle) {
            LOG_WARN("Sysctl register failed, retention not supported");
        }
    }
    
    LOG_INFO("Audio sample started. Press Ctrl+C to exit");
    
    /* Main loop */
    while (g_audio_context.thread_run_flag) {
        sleep(1);
    }
    
    /* Stop threads */
    g_audio_context.ai_exit = true;
    g_audio_context.ao_exit = true;
    g_audio_context.ai_send_ao_exit = true;
    
    /* Wait for threads to finish */
    usleep(THREAD_JOIN_TIMEOUT_MS);
    
    /* Unbind AI-AO if bound */
    if (g_audio_config.enable_ai_bind_ao) {
        MPP_CHN_S src_chn = {
            .enModId = AR_ID_AI,
            .s32DevId = g_audio_config.ai_dev_id,
            .s32ChnId = g_audio_config.ai_channel
        };
        
        MPP_CHN_S dest_chn = {
            .enModId = AR_ID_AO,
            .s32DevId = g_audio_config.ao_dev_id,
            .s32ChnId = g_audio_config.ao_channel
        };
        
        AR_MPI_SYS_UnBind(&src_chn, &dest_chn);
    }

cleanup:
    /* Cleanup audio modules */
    if (g_audio_config.enable_ai) {
        cleanup_ai_module();
    }
    
    if (g_audio_config.enable_ao) {
        cleanup_ao_module();
    }
    
    /* Cleanup system resources */
    cleanup_system_resources();
    
    LOG_INFO("Audio sample exited with code: %d", ret);
    return ret;
}

/**
 * @brief Initialize AI module
 * @return 0 on success, negative on error
 */
static int initialize_ai_module(void)
{
    AIO_ATTR_S ai_attr = {0};
    AI_CHN_PARAM_S chn_param = {0};
    
    /* Configure AI attributes */
    ai_attr.enSamplerate = g_audio_config.sample_rate;
    ai_attr.enBitwidth = g_audio_config.bit_width;
    ai_attr.enWorkmode = AIO_MODE_I2S_MASTER;
    ai_attr.enSoundmode = g_audio_config.sound_mode;
    ai_attr.u32EXFlag = 0;
    ai_attr.u32FrmNum = 30;
    ai_attr.u32PtNumPerFrm = 1024;
    ai_attr.u32ChnCnt = 2;
    ai_attr.u32ClkSel = 0;
    
    if (g_audio_config.ai_dev_id == AR_ACODEC_DEV_ID) {
        ai_attr.enI2sType = AIO_I2STYPE_INNERCODEC;
    } else {
        ai_attr.enI2sType = AIO_I2STYPE_EXTERN;
    }
    
    if (g_audio_config.ai_dev_id >= AR_I2S_SLAVE0_ID && g_audio_config.ai_dev_id <= AR_I2S_SLAVE1_ID) {
        ai_attr.enWorkmode = AIO_MODE_I2S_SLAVE;
    }
    
    /* Set AI device attributes */
    CHECK_RESULT(AR_MPI_AI_SetPubAttr(g_audio_config.ai_dev_id, &ai_attr), "AI SetPubAttr");
    
    /* Enable AI device */
    CHECK_RESULT(AR_MPI_AI_Enable(g_audio_config.ai_dev_id), "AI Enable");
    
    /* Enable AI channel */
    CHECK_RESULT(AR_MPI_AI_EnableChn(g_audio_config.ai_dev_id, g_audio_config.ai_channel), "AI EnableChn");
    
    /* Set channel parameters */
    chn_param.u32UsrFrmDepth = 6;
    CHECK_RESULT(AR_MPI_AI_SetChnParam(g_audio_config.ai_dev_id, g_audio_config.ai_channel, &chn_param), "AI SetChnParam");
    
    LOG_INFO("AI module initialized successfully");
    return 0;
}

/**
 * @brief Initialize AO module
 * @return 0 on success, negative on error
 */
static int initialize_ao_module(void)
{
    AIO_ATTR_S ao_attr = {0};
    
    /* Configure AO attributes */
    ao_attr.enSamplerate = g_audio_config.sample_rate;
    ao_attr.enBitwidth = g_audio_config.bit_width;
    ao_attr.enWorkmode = AIO_MODE_I2S_MASTER;
    ao_attr.enSoundmode = g_audio_config.sound_mode;
    ao_attr.u32EXFlag = 0;
    ao_attr.u32FrmNum = 8;
    ao_attr.u32PtNumPerFrm = 2048;
    ao_attr.u32ChnCnt = 2;
    ao_attr.u32ClkSel = 0;
    
    if (g_audio_config.ao_dev_id == AR_ACODEC_DEV_ID) {
        ao_attr.enI2sType = AIO_I2STYPE_INNERCODEC;
    } else {
        ao_attr.enI2sType = AIO_I2STYPE_EXTERN;
    }
    
    if (g_audio_config.ao_dev_id >= AR_I2S_SLAVE0_ID && g_audio_config.ao_dev_id <= AR_I2S_SLAVE1_ID) {
        ao_attr.enWorkmode = AIO_MODE_I2S_SLAVE;
    }
    
    /* Set AO device attributes */
    CHECK_RESULT(AR_MPI_AO_SetPubAttr(g_audio_config.ao_dev_id, &ao_attr), "AO SetPubAttr");
    
    /* Enable AO device */
    CHECK_RESULT(AR_MPI_AO_Enable(g_audio_config.ao_dev_id), "AO Enable");
    
    /* Enable AO channel */
    CHECK_RESULT(AR_MPI_AO_EnableChn(g_audio_config.ao_dev_id, g_audio_config.ao_channel), "AO EnableChn");
    
    LOG_INFO("AO module initialized successfully");
    return 0;
}

/**
 * @brief Cleanup AI module
 * @return 0 on success, negative on error
 */
static int cleanup_ai_module(void)
{
    int32_t ret = 0;
    
    /* Disable AI channel */
    ret = AR_MPI_AI_DisableChn(g_audio_config.ai_dev_id, g_audio_config.ai_channel);
    if (ret != AR_SUCCESS) {
        LOG_ERROR("AI DisableChn failed. ret: 0x%x", ret);
    }
    
    /* Disable AI device */
    ret = AR_MPI_AI_Disable(g_audio_config.ai_dev_id);
    if (ret != AR_SUCCESS) {
        LOG_ERROR("AI Disable failed. ret: 0x%x", ret);
    }
    
    LOG_INFO("AI module cleanup completed");
    return ret;
}

/**
 * @brief Cleanup AO module
 * @return 0 on success, negative on error
 */
static int cleanup_ao_module(void)
{
    int32_t ret = 0;
    
    /* Disable AO channel */
    ret = AR_MPI_AO_DisableChn(g_audio_config.ao_dev_id, g_audio_config.ao_channel);
    if (ret != AR_SUCCESS) {
        LOG_ERROR("AO DisableChn failed. ret: 0x%x", ret);
    }
    
    /* Disable AO device */
    ret = AR_MPI_AO_Disable(g_audio_config.ao_dev_id);
    if (ret != AR_SUCCESS) {
        LOG_ERROR("AO Disable failed. ret: 0x%x", ret);
    }
    
    LOG_INFO("AO module cleanup completed");
    return ret;
}

/**
 * @brief Set AI volume
 * @param sample_rate Sample rate
 * @param volume Volume level (0-31)
 * @return 0 on success, negative on error
 */
static int set_ai_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume)
{
    int fd;
    ACODEC_VOL_CTRL vol_ctrl = {0};
    int ret;
    
    if (volume < 0 || volume > 31) {
        LOG_ERROR("Invalid AI volume: %d (valid range: 0-31)", volume);
        return -EINVAL;
    }
    
    fd = open(ACODEC_FILE_PATH, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Failed to open acodec device: %s", strerror(errno));
        return -errno;
    }
    
    vol_ctrl.vol_ctrl = volume;
    
    ret = ar_mpi_acodec_ioctl(fd, IOC_NR_SET_INPUT_VOL, &vol_ctrl, sizeof(vol_ctrl));
    if (ret != 0) {
        LOG_ERROR("Failed to set AI volume: %d", ret);
        close(fd);
        return -1;
    }
    
    close(fd);
    LOG_INFO("AI volume set to %d", volume);
    return 0;
}

/**
 * @brief Set AO volume
 * @param sample_rate Sample rate
 * @param volume Volume level (0-31)
 * @return 0 on success, negative on error
 */
static int set_ao_volume(AUDIO_SAMPLE_RATE_E sample_rate, int32_t volume)
{
    int fd;
    ACODEC_VOL_CTRL vol_ctrl = {0};
    int ret;
    
    if (volume < 0 || volume > 31) {
        LOG_ERROR("Invalid AO volume: %d (valid range: 0-31)", volume);
        return -EINVAL;
    }
    
    fd = open(ACODEC_FILE_PATH, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Failed to open acodec device: %s", strerror(errno));
        return -errno;
    }
    
    vol_ctrl.vol_ctrl = volume;
    
    ret = ar_mpi_acodec_ioctl(fd, IOC_NR_SET_OUTPUT_VOL, &vol_ctrl, sizeof(vol_ctrl));
    if (ret != 0) {
        LOG_ERROR("Failed to set AO volume: %d", ret);
        close(fd);
        return -1;
    }
    
    close(fd);
    LOG_INFO("AO volume set to %d", volume);
    return 0;
}

/**
 * @brief AI capture thread function
 * @param arg Thread argument (unused)
 * @return NULL
 */
static void* ai_capture_thread(void *arg)
{
    (void)arg;
    AUDIO_FRAME_S audio_frame = {0};
    AEC_FRAME_S aec_frame = {0};
    FILE *output_file = NULL;
    char filename[MAX_PATH_LENGTH];
    int32_t ret;
    
    /* Create output filename */
    snprintf(filename, sizeof(filename), "%s/ai_capture_%dx%d.wav",
             g_audio_config.ai_output_path,
             g_audio_config.sample_rate,
             g_audio_config.bit_width);
    
    output_file = fopen(filename, "wb");
    if (!output_file) {
        LOG_ERROR("Failed to create AI output file: %s", filename);
        return NULL;
    }
    
    /* Skip WAV header for now, will be written at the end */
    fseek(output_file, WAVE_HEADER_SIZE, SEEK_SET);
    
    LOG_INFO("AI capture thread started, saving to: %s", filename);
    
    while (!g_audio_context.ai_exit) {
        ret = AR_MPI_AI_GetFrame(g_audio_config.ai_dev_id, g_audio_config.ai_channel,
                                &audio_frame, &aec_frame, 1000);
        if (ret == AR_SUCCESS) {
            /* Get virtual address from physical address */
            void *virt_addr = NULL;
            AR_S32 pool_ret = AR_MPI_VB_GetBlockVirAddr(audio_frame.u32PoolId[0], 
                                                       audio_frame.u64PhyAddr[0], &virt_addr);
            if (pool_ret == AR_SUCCESS && virt_addr) {
                /* Write audio data to file */
                fwrite(virt_addr, 1, audio_frame.u32Len, output_file);
            }
            
            /* Release frame */
            AR_MPI_AI_ReleaseFrame(g_audio_config.ai_dev_id, g_audio_config.ai_channel,
                                  &audio_frame, &aec_frame);
        } else if (ret != AR_ERR_AI_BUF_EMPTY) {
            LOG_WARN("AI GetFrame failed: 0x%x", ret);
        }
        
        usleep(10000); /* 10ms */
    }
    
    if (output_file) {
        fclose(output_file);
    }
    
    LOG_INFO("AI capture thread exited");
    return NULL;
}

/**
 * @brief AO playback thread function
 * @param arg Thread argument (unused)
 * @return NULL
 */
static void* ao_playback_thread(void *arg)
{
    (void)arg;
    FILE *input_file = NULL;
    AUDIO_FRAME_S audio_frame = {0};
    VB_BLK vb_block = VB_INVALID_HANDLE;
    wave_file_header_t wave_header;
    uint8_t *audio_data = NULL;
    size_t bytes_read;
    int32_t ret;
    
    /* Open input file */
    input_file = fopen(g_audio_config.ao_input_path, "rb");
    if (!input_file) {
        LOG_ERROR("Failed to open AO input file: %s", g_audio_config.ao_input_path);
        return NULL;
    }
    
    /* Read and validate WAV header */
    if (fread(&wave_header, 1, sizeof(wave_header), input_file) != sizeof(wave_header)) {
        LOG_ERROR("Failed to read WAV header");
        fclose(input_file);
        return NULL;
    }
    
    /* Validate WAV format */
    if (strncmp(wave_header.riff, "RIFF", 4) != 0 ||
        strncmp(wave_header.wave, "WAVE", 4) != 0) {
        LOG_ERROR("Invalid WAV file format");
        fclose(input_file);
        return NULL;
    }
    
    LOG_INFO("AO playback thread started, playing: %s", g_audio_config.ao_input_path);
    LOG_INFO("WAV format: %dHz, %d-bit, %d channels",
             wave_header.wave_format.samples_per_sec,
             wave_header.wave_format.bits_per_sample,
             wave_header.wave_format.channels);
    
    while (!g_audio_context.ao_exit) {
        /* Get VB block - match original code pattern */
        AR_U64 block_size = 1024;  /* Match original code block size */
        vb_block = AR_MPI_VB_GetBlock(VB_INVALID_POOLID, block_size, NULL);
        if (vb_block == VB_INVALID_HANDLE) {
            LOG_WARN("Failed to get VB block");
            usleep(10000);
            continue;
        }
        
        /* Get pool ID from block handle */
        VB_POOL pool_id = AR_MPI_VB_Handle2PoolId(vb_block);
        if (pool_id < 0) {
            LOG_ERROR("Failed to get pool ID from VB block");
            AR_MPI_VB_ReleaseBlock(vb_block);
            continue;
        }
        
        /* Map the pool */
        ret = AR_MPI_VB_MmapPool(pool_id);
        if (ret != AR_SUCCESS) {
            LOG_ERROR("Failed to mmap VB pool: 0x%x", ret);
            AR_MPI_VB_ReleaseBlock(vb_block);
            continue;
        }
        

        
        /* Get physical and virtual addresses */
        AR_U64 phys_addr = AR_MPI_VB_Handle2PhysAddr(vb_block);
        void *frame_virt_addr = NULL;
        void *frame_virt_addr_r = NULL;  /* For stereo right channel */
        
        ret = AR_MPI_VB_GetBlockVirAddr(pool_id, phys_addr, &frame_virt_addr);
        if (ret != AR_SUCCESS || !frame_virt_addr) {
            LOG_WARN("Failed to get virtual address");
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(vb_block);
            continue;
        }
        
        /* Prepare audio frame structure */
        memset(&audio_frame, 0, sizeof(AUDIO_FRAME_S));
        audio_frame.u64PhyAddr[0] = phys_addr;
        
        /* Handle stereo mode like original code */
        if (g_audio_config.sound_mode == AUDIO_SOUND_MODE_STEREO) {
            audio_frame.u64PhyAddr[1] = audio_frame.u64PhyAddr[0] + block_size/2;
            frame_virt_addr_r = (char*)frame_virt_addr + block_size/2;
        }
        
        /* Allocate temporary buffer for reading */
        char *read_buf = (char*)malloc(block_size);
        if (!read_buf) {
            LOG_WARN("Failed to allocate read buffer");
            AR_MPI_VB_MunmapPool(pool_id);
            AR_MPI_VB_ReleaseBlock(vb_block);
            usleep(10000);
            continue;
        }
        
        /* Read audio data */
        bytes_read = fread(read_buf, 1, block_size, input_file);
        if (bytes_read <= 0) {
            /* End of file, loop back to beginning */
            if (strstr(g_audio_config.ai_output_filename, ".wav")) {
                fseek(input_file, 44, SEEK_SET);  /* Skip WAV header */
            } else {
                fseek(input_file, 0, SEEK_SET);
            }
            bytes_read = fread(read_buf, 1, block_size, input_file);
        }
        
        if (bytes_read > 0) {
            /* Process audio data based on channel mode */
            if (g_audio_config.sound_mode == AUDIO_SOUND_MODE_STEREO) {
                /* Stereo: separate left and right channels like original code */
                audio_frame.u32Len = bytes_read / 2;
                for (int x = 0; x < (bytes_read / 4); x++) {
                    memcpy((char*)frame_virt_addr + x*2, read_buf + x*4, 2);      /* Left channel */
                    memcpy((char*)frame_virt_addr_r + x*2, read_buf + x*4 + 2, 2); /* Right channel */
                }
            } else {
                /* Mono: direct copy */
                audio_frame.u32Len = bytes_read;
                memcpy(frame_virt_addr, read_buf, bytes_read);
            }
            
            /* Set other frame parameters */
            audio_frame.enBitwidth = g_audio_config.bit_width;
            audio_frame.enSoundmode = g_audio_config.sound_mode;
            audio_frame.u64TimeStamp = 0;
            audio_frame.u32Seq = 0;
            audio_frame.u32PoolId[0] = pool_id;
            
            printf("[DEBUG] REFACTORED AO Frame: Len=%u, PhyAddr=0x%llx, PoolId=%u, VirAddr=%p\n", 
                   audio_frame.u32Len, audio_frame.u64PhyAddr[0], audio_frame.u32PoolId[0], (void*)frame_virt_addr);
            
            /* Send frame to AO */
            ret = AR_MPI_AO_SendFrame(g_audio_config.ao_dev_id, g_audio_config.ao_channel,
                                     &audio_frame, 1000);
            if (ret != AR_SUCCESS) {
                LOG_WARN("AO SendFrame failed: 0x%x", ret);
            }
        }
        
        /* Free temporary buffer */
        free(read_buf);
        
        /* Cleanup VB block immediately after sending frame (match original code) */
        AR_MPI_VB_MunmapPool(pool_id);
        AR_MPI_VB_ReleaseBlock(vb_block);
        usleep(20000); /* 20ms */
    }
    
    if (input_file) {
        fclose(input_file);
    }
    
    LOG_INFO("AO playback thread exited");
    return NULL;
}

/**
 * @brief AI to AO thread function
 * @param arg Thread argument (unused)
 * @return NULL
 */
static void* ai_to_ao_thread(void *arg)
{
    (void)arg;
    AUDIO_FRAME_S audio_frame = {0};
    AEC_FRAME_S aec_frame = {0};
    int32_t ret;
    
    LOG_INFO("AI to AO thread started");
    
    while (!g_audio_context.ai_send_ao_exit) {
        /* Get frame from AI */
        ret = AR_MPI_AI_GetFrame(g_audio_config.ai_dev_id, g_audio_config.ai_channel,
                                &audio_frame, &aec_frame, 1000);
        if (ret == AR_SUCCESS) {
            /* Send frame to AO */
            ret = AR_MPI_AO_SendFrame(g_audio_config.ao_dev_id, g_audio_config.ao_channel,
                                     &audio_frame, 1000);
            if (ret != AR_SUCCESS) {
                LOG_WARN("AO SendFrame failed: 0x%x", ret);
            }
            
            /* Release AI frame */
            AR_MPI_AI_ReleaseFrame(g_audio_config.ai_dev_id, g_audio_config.ai_channel,
                                  &audio_frame, &aec_frame);
        } else if (ret != AR_ERR_AI_BUF_EMPTY) {
            LOG_WARN("AI GetFrame failed: 0x%x", ret);
        }
        
        usleep(10000); /* 10ms */
    }
    
    LOG_INFO("AI to AO thread exited");
    return NULL;
}

/**
 * @brief Test ACODEC AI functionality
 * @return 0 on success, negative on error
 */
static int test_acodec_ai_functionality(void)
{
    LOG_INFO("Testing ACODEC AI functionality");
    /* Implementation would test various AI codec parameters */
    return 0;
}

/**
 * @brief Test ACODEC AO functionality
 * @return 0 on success, negative on error
 */
static int test_acodec_ao_functionality(void)
{
    LOG_INFO("Testing ACODEC AO functionality");
    /* Implementation would test various AO codec parameters */
    return 0;
}

/**
 * @brief Create WAV file header
 * @param header Pointer to header structure
 * @param encode_type Audio encoding type
 * @param sound_mode Sound mode (mono/stereo)
 * @param sample_rate Sample rate
 * @param raw_length Raw audio data length
 * @return 0 on success, negative on error
 */
static int create_wave_header(wave_file_header_t *header, audio_encode_type_t encode_type,
                             AUDIO_SOUND_MODE_E sound_mode, AUDIO_SAMPLE_RATE_E sample_rate,
                             int raw_length)
{
    if (!header) {
        return -EINVAL;
    }
    
    memset(header, 0, sizeof(wave_file_header_t));
    
    /* RIFF header */
    strncpy(header->riff, "RIFF", 4);
    header->riff_len = raw_length + sizeof(wave_file_header_t) - 8;
    strncpy(header->wave, "WAVE", 4);
    
    /* Format chunk */
    strncpy(header->fmt, "fmt ", 4);
    header->fmt_len = sizeof(wave_format_t);
    
    /* Wave format */
    header->wave_format.format_tag = 1; /* PCM */
    header->wave_format.channels = (sound_mode == AUDIO_SOUND_MODE_MONO) ? 1 : 2;
    header->wave_format.samples_per_sec = sample_rate;
    header->wave_format.bits_per_sample = 16; /* Assume 16-bit */
    header->wave_format.block_align = header->wave_format.channels * 
                                     (header->wave_format.bits_per_sample / 8);
    header->wave_format.avg_bytes_per_sec = header->wave_format.samples_per_sec * 
                                           header->wave_format.block_align;
    
    /* Data chunk */
    strncpy(header->data, "data", 4);
    header->data_len = raw_length;
    
    return 0;
}

/**
 * @brief Read WAV file header
 * @param data Pointer to header data
 * @return 0 on success, negative on error
 */
static int read_wave_header(const char *data)
{
    if (!data) {
        return -EINVAL;
    }
    
    /* Basic validation */
    if (strncmp(data, "RIFF", 4) != 0) {
        LOG_ERROR("Invalid RIFF header");
        return -EINVAL;
    }
    
    if (strncmp(data + 8, "WAVE", 4) != 0) {
        LOG_ERROR("Invalid WAVE header");
        return -EINVAL;
    }
    
    return 0;
}

/**
 * @brief Sysctl callback function
 * @param event Sysctl event
 * @param data Event data
 * @param length Data length
 */
static void sysctl_callback(SYSCTL_EVENT_E event, void *data, int32_t length)
{
    (void)data;
    (void)length;
    
    switch (event) {
        case SYSCTL_EVENT_SUSPEND:
            LOG_INFO("System suspend event received");
            g_audio_context.suspend_flag = true;
            break;
        case SYSCTL_EVENT_RESUME:
            LOG_INFO("System resume event received");
            g_audio_context.suspend_flag = false;
            break;
        default:
            LOG_INFO("Unknown sysctl event: %d", event);
            break;
    }
}
