#ifndef __AR_DSP_BOOTUP_H__
#define __AR_DSP_BOOTUP_H__

#define CEVA_BIN_HEADER 0x00000040

typedef int boot_status_t;
typedef int int_32_t;
typedef unsigned int uint_32_t;
typedef unsigned int uint32_t;
typedef char int_8_t;
typedef char uint_8_t;
typedef unsigned char u_int_8_t;
typedef unsigned int addr_t;
typedef unsigned int offset_t;
typedef unsigned int addr_len_t;
typedef void *icc_client_t;

typedef enum
{
    DSP_CORE0 = 0,
    DSP_CORE1,
    DSP_CORE2,
    DSP_CORE3,
    MAX_DSP_CORE
} dsp_core_id_t;

int sirius_verify_ceva(char* filepath);
int sirius_boot_ceva(char* filepath,unsigned int dsp_id);
#endif
