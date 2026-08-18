#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "string.h"
#include "stdlib.h"
#include "mpi_audio.h"
#include "mpi_vb.h"
#include "mpi_vo.h"
#include "mpi_sys.h"
#include "mpi_sysctl.h"
#include "acodec.h"
#include "hal_sys.h"

#define SOCKET_PATH "/tmp/audio_service.sock"
#define MAX_COMMAND_LEN 1024
#define MAX_FILENAME_LEN 512

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

#define CHECK_RET(express,name)\
    do{\
        AR_S32 Ret;\
        Ret = express;\
        if (AR_SUCCESS != Ret)\
        {\
            printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0;39m\n", name, __FUNCTION__, __LINE__, Ret);\
            return Ret;\
        }\
    }while(0)

// Global variables
static AR_BOOL bServiceExit = AR_FALSE;
static AR_BOOL bAoExit = AR_TRUE;
static AR_BOOL bAoInitialized = AR_FALSE;
static AR_BOOL bPlaybackThreadRunning = AR_FALSE;
static pthread_t playback_thread;
static pthread_mutex_t playback_mutex = PTHREAD_MUTEX_INITIALIZER;

static AUDIO_DEV AoDevId = AR_ACODEC_DEV_ID;
static AO_CHN AoChn = 0;
static AUDIO_SAMPLE_RATE_E eSampleRate = AUDIO_SAMPLE_RATE_48000;
static AUDIO_SOUND_MODE_E eSoundMode = AUDIO_SOUND_MODE_STEREO;
static AUDIO_BIT_WIDTH_E eBitWidth = AUDIO_BIT_WIDTH_16;
static AR_S32 s32AoVolume = 5;

static AR_U8 pu8CurrentAudioFile[MAX_FILENAME_LEN] = {0};

VB_POOL vbPoolId = POOL_OWNER_COMMON;

// Function to convert sample rate value to enum
AUDIO_SAMPLE_RATE_E GetSampleRateEnum(AR_U32 sampleRate) {
    switch(sampleRate) {
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
            printf("Unsupported sample rate: %u Hz\n", sampleRate);
            printf("Supported rates: 8000, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 96000 Hz\n");
            return AUDIO_SAMPLE_RATE_48000; // fallback
    }
}

// Function to check if sample rate is supported
AR_BOOL IsSampleRateSupported(AR_U32 sampleRate) {
    switch(sampleRate) {
        case 8000:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
        case 64000:
        case 96000:
            return AR_TRUE;
        default:
            return AR_FALSE;
    }
}

typedef struct WAVE_FORMAT
{
    AR_S16  wFormatTag;
    AR_S16  wChannels;
    AR_U32  dwSamplesPerSec;
    AR_U32  dwAvgBytesPerSec;
    AR_S16  wBlockAlign;
    AR_S16  wBitsPerSample;
} WaveFormat_t;

typedef struct WAVEFILEHEADER
{
    AR_S8       chRIFF[4];
    AR_U32      dwRIFFLen;
    AR_S8       chWAVE[4];
    AR_S8       chFMT[4];
    AR_U32      dwFMTLen;
    WaveFormat_t wave;
    AR_S8       chDATA[4];
    AR_U32      dwDATALen;
} WaveFileHeader_t;

typedef enum
{
    CMD_PLAY = 0,
    CMD_STOP,
    CMD_UNKNOWN
} AudioCommand_e;

typedef struct
{
    AudioCommand_e cmd;
    char filename[MAX_FILENAME_LEN];
    int volume;
} AudioCommandData_t;

// Function prototypes
AR_S32 Module_AO_Init(void);
AR_S32 Module_AO_DeInit(void);
AR_S32 Module_AO_SetVolume(AUDIO_SAMPLE_RATE_E enSample, AR_S32 s32Volume);
void* Task_aoSendFrame(void *argv);
void signal_func(int signo, siginfo_t *info, void *data);
AudioCommand_e parse_command(const char* cmd_str, AudioCommandData_t* cmd_data);
void* ipc_server_thread(void* arg);

AR_S32 Module_AO_SetVolume(AUDIO_SAMPLE_RATE_E enSample, AR_S32 s32Volume)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AR_S32 s32Fd = -1;
    AR_U32 u32Volume = 0;
    
    s32Fd = open("/dev/acodec", O_RDWR);
    if(s32Fd < 0)
    {
        printf("open /dev/acodec failed!\n");
        return AR_FAILURE;
    }
    
    if(s32Volume < 0)
        u32Volume = 0;
    else if(s32Volume > 31)
        u32Volume = 31;
    else
        u32Volume = s32Volume;
        
    s32Ret = ioctl(s32Fd, ACODEC_SET_OUTPUT_VOL, &u32Volume);
    if(s32Ret != AR_SUCCESS)
    {
        printf("set output volume failed!\n");
        close(s32Fd);
        return s32Ret;
    }
    
    close(s32Fd);
    return AR_SUCCESS;
}

AR_S32 Module_AO_Init(void)
{
    AR_S32 s32Ret = AR_SUCCESS;
    AIO_ATTR_S pstAttr;

    memset(&pstAttr, 0x0, sizeof(AIO_ATTR_S));
    pstAttr.enSamplerate = eSampleRate;
    pstAttr.enBitwidth = eBitWidth;
    pstAttr.enWorkmode = AIO_MODE_I2S_MASTER;
    pstAttr.enSoundmode = eSoundMode;
    pstAttr.u32EXFlag = 0;
    pstAttr.u32FrmNum = 8;
    pstAttr.u32PtNumPerFrm = 2048;
    pstAttr.u32ChnCnt = 2;
    pstAttr.u32ClkSel = 0;
    if (AoDevId == AR_ACODEC_DEV_ID)
        pstAttr.enI2sType = AIO_I2STYPE_INNERCODEC;
    else
        pstAttr.enI2sType = AIO_I2STYPE_EXTERN;
    if (AoDevId >= AR_I2S_SLAVE0_ID && AoDevId <= AR_I2S_SLAVE1_ID)
        pstAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
        
    s32Ret = AR_MPI_AO_SetPubAttr(AoDevId, &pstAttr);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Dev setParam failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AO_Enable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Dev Enable failed!ret:%#x\n",__func__,__LINE__,s32Ret);
        return s32Ret;
    }

    s32Ret = AR_MPI_AO_EnableChn(AoDevId, AoChn);
    if(s32Ret != AR_SUCCESS)
    {
        printf("[%s][%d]--->>>AO Chn%d Enable failed!ret:%#x\n",__func__,__LINE__,AoChn,s32Ret);
        return s32Ret;
    }

    Module_AO_SetVolume(eSampleRate, s32AoVolume);

    return s32Ret;
}

AR_S32 Module_AO_DeInit(void)
{
    AR_S32 s32Ret = AR_SUCCESS;
    
    s32Ret = AR_MPI_AO_DisableChn(AoDevId, AoChn);
    if(s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_AO_DisableChn failed with %#x!\n", s32Ret);
        return s32Ret;
    }
    
    s32Ret = AR_MPI_AO_Disable(AoDevId);
    if(s32Ret != AR_SUCCESS)
    {
        printf("AR_MPI_AO_Disable failed with %#x!\n", s32Ret);
        return s32Ret;
    }
    
    return AR_SUCCESS;
}

void *Task_aoSendFrame(void *argv)
{
    pthread_detach(pthread_self());
    AR_S32 s32Ret = AR_SUCCESS, s32raw = 0;
    FILE *s32Fd = NULL;
    AUDIO_FRAME_S pAoSendFrame;
    WaveFileHeader_t pstWaveHeader;
    VB_BLK blk;
    VB_POOL poolId;
    AR_S32 data_len = 0;

    memset(&pstWaveHeader, 0x0, sizeof(WaveFileHeader_t));

    pthread_mutex_lock(&playback_mutex);
    
    if (strlen((char*)pu8CurrentAudioFile) == 0) {
        printf("No audio file specified for playback\n");
        pthread_mutex_unlock(&playback_mutex);
        return NULL;
    }

    s32Fd = fopen((char*)pu8CurrentAudioFile, "r+");
    if (!s32Fd) {
        printf("open %s failed!\n", pu8CurrentAudioFile);
        pthread_mutex_unlock(&playback_mutex);
        return NULL;
    }

    // Parse WAV header if it's a WAV file
    if (strstr((char*)pu8CurrentAudioFile, ".wav")) {
        fread(&pstWaveHeader, 1, sizeof(WaveFileHeader_t), s32Fd);
        printf("**********************\n");
        printf("*** audio wav info:\n");
        printf("*** SoundMode   :%d [0:MONO,1:STEREO]\n", pstWaveHeader.wave.wChannels - 1);
        printf("*** BitWidth    :%d \n", pstWaveHeader.wave.wBitsPerSample);
        printf("*** SampleRate  :%d \n", pstWaveHeader.wave.dwSamplesPerSec);
        printf("*** DataLen     :%d \n", pstWaveHeader.dwDATALen);
        printf("**********************\n");
        data_len = pstWaveHeader.dwDATALen;
        
        // Check if sample rate is supported
        if (!IsSampleRateSupported(pstWaveHeader.wave.dwSamplesPerSec)) {
            printf("Wav file sample rate %u Hz is not supported\n", pstWaveHeader.wave.dwSamplesPerSec);
            printf("Supported rates: 8000, 12000, 16000, 24000, 32000, 44100, 48000, 64000, 96000 Hz\n");
            fclose(s32Fd);
            pthread_mutex_unlock(&playback_mutex);
            return NULL;
        }
        
        // Get the required sample rate enum
        AUDIO_SAMPLE_RATE_E requiredSampleRate = GetSampleRateEnum(pstWaveHeader.wave.dwSamplesPerSec);
        
        // If sample rate changed, reinitialize AO device
        if (eSampleRate != requiredSampleRate) {
            printf("Reconfiguring AO device for sample rate: %u Hz\n", pstWaveHeader.wave.dwSamplesPerSec);
            
            // Deinitialize current AO device
            if (bAoInitialized) {
                Module_AO_DeInit();
                bAoInitialized = AR_FALSE;
            }
            
            // Update sample rate
            eSampleRate = requiredSampleRate;
            
            // Reinitialize with new sample rate
            AR_S32 s32Ret = Module_AO_Init();
            if (s32Ret != AR_SUCCESS) {
                printf("Failed to reinitialize AO device with new sample rate\n");
                fclose(s32Fd);
                pthread_mutex_unlock(&playback_mutex);
                return NULL;
            }
            bAoInitialized = AR_TRUE;
        }
        
        AR_S32 bitwidth = (eBitWidth == AUDIO_BIT_WIDTH_8) ? 8 : 
                         (eBitWidth == AUDIO_BIT_WIDTH_16) ? 16 : 24;
        if (bitwidth != pstWaveHeader.wave.wBitsPerSample) {
            printf("Wav file BitWidth not match AO BitWidth %u\n", eBitWidth);
            fclose(s32Fd);
            pthread_mutex_unlock(&playback_mutex);
            return NULL;
        }
        
        // Get the required sound mode (0=MONO, 1=STEREO)
        AUDIO_SOUND_MODE_E requiredSoundMode = (pstWaveHeader.wave.wChannels == 1) ? AUDIO_SOUND_MODE_MONO : AUDIO_SOUND_MODE_STEREO;
        
        // If sound mode changed, reinitialize AO device
        if (eSoundMode != requiredSoundMode) {
            printf("Reconfiguring AO device for sound mode: %s\n", 
                   (requiredSoundMode == AUDIO_SOUND_MODE_MONO) ? "MONO" : "STEREO");
            
            // Deinitialize current AO device
            if (bAoInitialized) {
                Module_AO_DeInit();
                bAoInitialized = AR_FALSE;
            }
            
            // Update sound mode
            eSoundMode = requiredSoundMode;
            
            // Reinitialize with new sound mode
            AR_S32 s32Ret = Module_AO_Init();
            if (s32Ret != AR_SUCCESS) {
                printf("Failed to reinitialize AO device with new sound mode\n");
                fclose(s32Fd);
                pthread_mutex_unlock(&playback_mutex);
                return NULL;
            }
            bAoInitialized = AR_TRUE;
        }
    } else {
        s32raw = 1;
        fseek(s32Fd, 0, SEEK_END);
        data_len = ftell(s32Fd);
        fseek(s32Fd, 0, SEEK_SET);
    }

    pthread_mutex_unlock(&playback_mutex);

    // Calculate frame size
    AR_S32 channels = (eSoundMode == AUDIO_SOUND_MODE_MONO) ? 1 : 2;
    AR_S32 sample_bytes = (eBitWidth == AUDIO_BIT_WIDTH_8) ? 1 : 
                         (eBitWidth == AUDIO_BIT_WIDTH_16) ? 2 : 3;
    AR_S32 s32FrameSize = 1024 * channels * sample_bytes;

    // Main playback loop
    bAoExit = AR_FALSE;
    while (!bAoExit && !bServiceExit) {
        AR_U64 u64BlkSize = 1024;
        AR_U64 u64ReadSize = 0;
        AR_VOID *frameVirtAddr;
        AR_VOID *frameVirtAddr_r;
        
        blk = AR_MPI_VB_GetBlock(vbPoolId, u64BlkSize, NULL);
        if (blk == VB_INVALID_HANDLE) {
            printf("get block failed\n");
            break;
        }

        poolId = AR_MPI_VB_Handle2PoolId(blk);
        if (poolId < 0) {
            printf("get pool id failed\n");
            bAoExit = AR_TRUE;
            goto RelBlk;
        }
        s32Ret = AR_MPI_VB_MmapPool(poolId);
        if (s32Ret) {
            printf("mmap pool failed\n");
            bAoExit = AR_TRUE;
            goto RelBlk;
        }
        AR_U64 phyAddr = AR_MPI_VB_Handle2PhysAddr(blk);

        memset(&pAoSendFrame, 0x0, sizeof(AUDIO_FRAME_S));

        s32Ret = AR_MPI_VB_GetBlockVirAddr(poolId, phyAddr, &frameVirtAddr);
        if (s32Ret) {
            printf("get frame virtual address failed\n");
            goto Unmap;
        }

        pAoSendFrame.u64PhyAddr[0] = phyAddr;

        if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        {
            pAoSendFrame.u64PhyAddr[1] = pAoSendFrame.u64PhyAddr[0] + u64BlkSize/2;
            frameVirtAddr_r = frameVirtAddr + u64BlkSize/2;
        }

        u64ReadSize = u64BlkSize;
        AR_CHAR *buf = (AR_CHAR *)malloc(u64ReadSize);
        if(!buf)
        {
            printf("malloc buf failed!!!\n");
            usleep(10*1000);
            continue;
        }

        s32Ret = fread(buf, 1, u64ReadSize, s32Fd);
        if(s32Ret <= 0)
        {
            if(strstr((char*)pu8CurrentAudioFile, ".wav"))
            {
                fseek(s32Fd, 44, SEEK_SET);
            }
            else
            {
                fseek(s32Fd, 0, SEEK_SET);
            }
            s32Ret = fread(buf, 1, u64ReadSize, s32Fd);
        }

        if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        {
            pAoSendFrame.u32Len = s32Ret/2;
            for(int x = 0; x < (s32Ret/4); x++)
            {
                memcpy(frameVirtAddr+x*2, buf+x*4, 2);
                memcpy(frameVirtAddr_r+x*2, buf+x*4+2, 2);
            }
        }
        else
        {
            pAoSendFrame.u32Len = s32Ret;
            memcpy(frameVirtAddr, buf, s32Ret);
        }

        free(buf);
        pAoSendFrame.enBitwidth = eBitWidth;
        pAoSendFrame.enSoundmode = eSoundMode;
        pAoSendFrame.u32PoolId[0] = poolId;
        pAoSendFrame.u64VirAddr[0] = frameVirtAddr;
        if(eSoundMode == AUDIO_SOUND_MODE_STEREO)
        {
            pAoSendFrame.u32PoolId[1] = poolId;
            pAoSendFrame.u64VirAddr[1] = frameVirtAddr_r;
        }
        pAoSendFrame.u64TimeStamp = 0;
        pAoSendFrame.u32Seq = 0;

        // Send frame to AO
        s32Ret = AR_MPI_AO_SendFrame(AoDevId, AoChn, &pAoSendFrame, 1000);
        if (s32Ret != AR_SUCCESS) {
            printf("AR_MPI_AO_SendFrame failed with %#x!\n", s32Ret);
        }

Unmap:
        AR_MPI_VB_MunmapPool(poolId);
RelBlk:
        AR_MPI_VB_ReleaseBlock(blk);
        usleep(10000); // Small delay
    }

    if (s32Fd) {
        fclose(s32Fd);
    }

    // Update thread status
    pthread_mutex_lock(&playback_mutex);
    bPlaybackThreadRunning = AR_FALSE;
    bAoExit = AR_TRUE;
    pthread_mutex_unlock(&playback_mutex);

    printf("Audio playback thread exited\n");
    return NULL;
}

// Internal function to stop current playback
// Must be called with playback_mutex already locked
static void stop_current_playback()
{
    if (bPlaybackThreadRunning && !bAoExit) {
        printf("Stopping current playback...\n");
        bAoExit = AR_TRUE;
        pthread_mutex_unlock(&playback_mutex);
        pthread_join(playback_thread, NULL);
        pthread_mutex_lock(&playback_mutex);
        bPlaybackThreadRunning = AR_FALSE;
        printf("Previous playback stopped\n");
    }
}

AudioCommand_e parse_command(const char* cmd_str, AudioCommandData_t* cmd_data)
{
    char cmd[32];
    char filename[MAX_FILENAME_LEN];
    int volume;
    
    memset(cmd_data, 0, sizeof(AudioCommandData_t));
    
    if (sscanf(cmd_str, "%s %s %d", cmd, filename, &volume) == 3) {
        if (strcmp(cmd, "play") == 0) {
            cmd_data->cmd = CMD_PLAY;
            strncpy(cmd_data->filename, filename, MAX_FILENAME_LEN - 1);
            cmd_data->volume = volume;
            return CMD_PLAY;
        }
    } else if (sscanf(cmd_str, "%s", cmd) == 1) {
        if (strcmp(cmd, "stop") == 0) {
            cmd_data->cmd = CMD_STOP;
            return CMD_STOP;
        }
    }
    
    return CMD_UNKNOWN;
}

void* ipc_server_thread(void* arg)
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAX_COMMAND_LEN];
    AudioCommandData_t cmd_data;
    
    // Remove existing socket file
    unlink(SOCKET_PATH);
    
    // Create socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return NULL;
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return NULL;
    }
    
    // Set socket permissions
    chmod(SOCKET_PATH, 0666);
    
    // Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return NULL;
    }
    
    printf("Audio service listening on %s\n", SOCKET_PATH);
    
    while (!bServiceExit) {
        // Use select() with timeout to allow signal interruption
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        
        timeout.tv_sec = 1;  // 1 second timeout
        timeout.tv_usec = 0;
        
        int select_result = select(server_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (select_result == -1) {
            if (errno == EINTR) {
                // Signal received, check if we should exit
                continue;
            }
            perror("select");
            continue;
        }
        
        if (select_result == 0) {
            // Timeout, check if we should exit
            continue;
        }
        
        // Socket is ready for accept
        if (FD_ISSET(server_fd, &read_fds)) {
            client_len = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_fd == -1) {
                if (errno == EINTR) continue;
                perror("accept");
                continue;
            }
        } else {
            continue;
        }
        
        // Read command
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            // Remove trailing newline
            char* newline = strchr(buffer, '\n');
            if (newline) *newline = '\0';
            
            printf("Received command: %s\n", buffer);
            
            AudioCommand_e cmd = parse_command(buffer, &cmd_data);
            
            switch (cmd) {
                case CMD_PLAY:
                    pthread_mutex_lock(&playback_mutex);
                    
                    // Stop current playback if running
                    stop_current_playback();
                    
                    // Set new audio file and volume
                    strncpy((char*)pu8CurrentAudioFile, cmd_data.filename, MAX_FILENAME_LEN - 1);
                    s32AoVolume = cmd_data.volume;
                    
                    // Set volume
                    if (bAoInitialized) {
                        Module_AO_SetVolume(eSampleRate, s32AoVolume);
                    }
                    
                    // Start new playback thread
                    if (pthread_create(&playback_thread, NULL, Task_aoSendFrame, NULL) != 0) {
                        printf("Failed to create playback thread\n");
                    } else {
                        bPlaybackThreadRunning = AR_TRUE;
                        printf("Started playback: %s at volume %d\n", cmd_data.filename, cmd_data.volume);
                    }
                    
                    pthread_mutex_unlock(&playback_mutex);
                    
                    write(client_fd, "OK\n", 3);
                    break;
                    
                case CMD_STOP:
                    pthread_mutex_lock(&playback_mutex);
                    
                    if (bPlaybackThreadRunning && !bAoExit) {
                        stop_current_playback();
                        printf("Stopped audio playback\n");
                    } else {
                        printf("No audio currently playing\n");
                    }
                    
                    pthread_mutex_unlock(&playback_mutex);
                    
                    write(client_fd, "OK\n", 3);
                    break;
                    
                default:
                    write(client_fd, "ERROR: Unknown command\n", 23);
                    break;
            }
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    unlink(SOCKET_PATH);
    return NULL;
}

void signal_func(int signo, siginfo_t *info, void *data)
{
    printf("Received signal %d, shutting down service...\n", signo);
    bServiceExit = AR_TRUE;
    bAoExit = AR_TRUE;
    
    // Force exit if signal received multiple times
    static int signal_count = 0;
    signal_count++;
    if (signal_count > 2) {
        printf("Force exit after multiple signals\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    AR_S32 s32Ret = AR_SUCCESS;
    struct sigaction sa = { 0 };
    VB_CONFIG_S pstVbConfig = { 0 };
    VB_POOL_CONFIG_S pstVbPoolConfig = { 0 };
    pthread_t ipc_thread;
    
    printf("Audio Service Starting...\n");
    
    // Setup signal handlers
    sa.sa_sigaction = signal_func;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    // Initialize VB (Video Buffer) system
    pstVbConfig.astCommPool[0].u64BlkSize = 1024 * 4;
    pstVbConfig.astCommPool[0].u32BlkCnt = 20;
    
    s32Ret = AR_MPI_VB_SetConfig(&pstVbConfig);
    if (s32Ret != AR_SUCCESS) {
        printf("VB set common config failed, creating user pool! ret:%#x\n", s32Ret);
        pstVbPoolConfig.u64BlkSize = 1024 * 4;
        pstVbPoolConfig.u32BlkCnt = 20;
        pstVbPoolConfig.enRemapMode = VB_REMAP_MODE_NONE;
        vbPoolId = AR_MPI_VB_CreatePool(&pstVbPoolConfig);
        if (vbPoolId == VB_INVALID_POOLID) {
            printf("AR_MPI_VB_CreatePool failed!\n");
            return AR_FAILURE;
        }
    }
    
    s32Ret = AR_MPI_VB_Init();
    if (s32Ret != AR_SUCCESS) {
        printf("AR_MPI_VB_Init failed with %#x!\n", s32Ret);
        return s32Ret;
    }
    
    s32Ret = AR_MPI_SYS_Init();
    if (s32Ret != AR_SUCCESS) {
        printf("AR_MPI_SYS_Init failed with %#x!\n", s32Ret);
        AR_MPI_VB_Exit();
        return s32Ret;
    }
    
    // Initialize AO (Audio Output)
    s32Ret = Module_AO_Init();
    if (s32Ret != AR_SUCCESS) {
        printf("Module_AO_Init failed with %#x!\n", s32Ret);
        AR_MPI_SYS_Exit();
        AR_MPI_VB_Exit();
        return s32Ret;
    }
    bAoInitialized = AR_TRUE;
    
    // Start IPC server thread
    if (pthread_create(&ipc_thread, NULL, ipc_server_thread, NULL) != 0) {
        printf("Failed to create IPC server thread\n");
        Module_AO_DeInit();
        AR_MPI_SYS_Exit();
        AR_MPI_VB_Exit();
        return AR_FAILURE;
    }
    
    printf("Audio Service Ready. Listening for commands...\n");
    
    // Wait for service exit signal
    while (!bServiceExit) {
        sleep(1);
    }
    
    // Cleanup
    printf("Shutting down audio service...\n");
    
    // Stop playback if running
    pthread_mutex_lock(&playback_mutex);
    if (!bAoExit) {
        bAoExit = AR_TRUE;
        pthread_join(playback_thread, NULL);
    }
    pthread_mutex_unlock(&playback_mutex);
    
    // Wait for IPC thread to finish
    pthread_join(ipc_thread, NULL);
    
    // Cleanup audio system
    if (bAoInitialized) {
        Module_AO_DeInit();
    }
    
    AR_MPI_SYS_Exit();
    AR_MPI_VB_Exit();
    
    if (vbPoolId != POOL_OWNER_COMMON) {
        AR_MPI_VB_DestroyPool(vbPoolId);
    }
    
    printf("Audio Service Stopped.\n");
    return AR_SUCCESS;
}
