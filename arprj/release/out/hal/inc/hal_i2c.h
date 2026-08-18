#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
	extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


int32_t ar_hal_i2c_master_read_byte(int file, int slv_addr, uint8_t addr_width,  uint8_t reg_addr);
int32_t ar_hal_i2c_master_write_byte(int file, int slv_addr, uint8_t addr_width, uint8_t reg_addr, uint8_t val);
int32_t ar_hal_i2c_master_read_word(int file, int slv_addr, uint8_t addr_width, uint8_t reg_addr);
int32_t ar_hal_i2c_master_write_word(int file, int slv_addr, uint8_t addr_width, uint8_t reg_addr, uint16_t val);
int ar_hal_i2c_open(uint32_t adapter_num);
void ar_hal_i2c_close(int file);
int32_t ar_hal_i2c_master_set_timeout(int file, uint32_t val);
int32_t ar_hal_i2c_master_set_retry(int file, uint32_t val);
int32_t ar_hal_i2c_master_read(int file, int slv_addr, uint8_t addr_width,
			   uint8_t *reg_addr, uint8_t reg_addr_size, uint8_t* p_data,
			   uint32_t length);
int32_t ar_hal_i2c_master_write(int file, int slv_addr, uint8_t addr_width, uint8_t* p_data, uint32_t length);

#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* End of #ifdef __cplusplus */


#endif
