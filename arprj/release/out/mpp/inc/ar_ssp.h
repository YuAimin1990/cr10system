/*
 * extdrv/include/ar_ssp.h for Linux .
 *
 * History:
 *      2006-4-11 create this file
 */

#ifndef __AR_SSP_H__
#define __AR_SSP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define SSP_READ_ALT    0x1
#define SSP_WRITE_ALT    0X3

typedef struct arSPI_DATA_S
{
    unsigned int     spi_no;
    unsigned char    dev_addr;
    unsigned int     dev_byte_num;
    unsigned int     reg_addr;
    unsigned int     addr_byte_num;
    unsigned int     data;
      unsigned int     data_byte_num;
}SPI_DATA_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

