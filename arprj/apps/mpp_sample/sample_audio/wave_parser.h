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