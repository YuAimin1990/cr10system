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