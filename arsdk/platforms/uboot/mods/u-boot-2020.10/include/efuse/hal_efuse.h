#ifndef __HAL_EFUSE_H__
#define __HAL_EFUSE_H__

typedef enum {
    AR_EFUSE_IOC_READ_SECWRITE = 0x00,
    AR_EFUSE_IOC_READ_CNTWRITE,
    AR_EFUSE_IOC_SECREAD_CNTWRITE,
    AR_EFUSE_IOC_CNTREAD_CNTWRITE,
} ENMU_AR_BANK_ACCESS_RIGHT;


int ar_hal_efuse_get_size();
int ar_hal_efuse_read(unsigned char * output, unsigned int read_size);
int ar_hal_efuse_write(unsigned char * input, unsigned int write_size);

/**
* @brief  efuse rootkey读取接口
* @param  key_value：对应存储rootkey值地址；key_bit_len：读取rootkey bit长度；key_id：rootkey id
* @retval 0 成功 , 其它 失败.
* @note   rootkey对应efuse key_id = 0， key_bit_len = 256bits
*/
int ar_hal_efuse_read_rootkey(unsigned char * key_value, int key_bit_len, int key_id);
/**
* @brief  efuse rootkey写入接口
* @param  key_value：对应存储rootkey值地址；key_bit_len：写入rootkey bit长度；key_id：rootkey id
* @retval 0 成功 , 其它 失败.
* @note   rootkey对应efuse key_id = 0， key_bit_len = 256bits
*/
int ar_hal_efuse_write_rootkey(unsigned char * key_value, int key_bit_len, int key_id);
/**
* @brief  efuse pkf读取接口
* @param  key_value：对应存储pkf值地址
* @retval 0 成功 , 其它 失败.
* @note   pkf读取长度128bits
*/
int ar_hal_efuse_read_pkf(unsigned char * key_value);
/**
* @brief  efuse pkf写入接口
* @param  key_value：对应存储pkf值地址
* @retval 0 成功 , 其它 失败.
* @note   pkf读取长度128bits
*/
int ar_hal_efuse_write_pkf(unsigned char * key_value);
/**
* @brief  efuse secure boot状态读取接口
* @param  secureboot：对应存储secureboot值地址
* @retval 0 成功 , 其它 失败.
* @note   key_value = 1 secure boot 使能 
*/
int ar_hal_efuse_read_secureboot(unsigned char * secureboot);
/**
* @brief  efuse secure boot状态使能接口
* @param  none
* @retval 0 成功 , 其它 失败.
* @note   
*/
int ar_hal_efuse_enable_secureboot(void);
/**
* @brief  efuse flexible secure boot状态读取接口
* @param  secureboot：对应灵活存储secureboot值地址
* @retval 0 成功 , 其它 失败.
* @note   key_value = 1 secure boot 使能 
*/
int ar_hal_efuse_read_flexible_secureboot(unsigned char * secureboot);
/**
* @brief  efuse flexible secure boot状态使能接口
* @param  none
* @retval 0 成功 , 其它 失败.
* @note   
*/
int ar_hal_efuse_enable_flexible_secureboot(void);
/**
* @brief  efuse 对应bank_id权限操作接口
* @param  bank_id：对应bank id (1-2)，bank_access_right：对应bank权限
* @retval 0 成功 , 其它 失败.
* @note   关闭bank1 及 bank2的读写权限，两个bank只能被硬件读取，cpu无法读取
*/
int ar_hal_efuse_set_bank_access_right(int bank_id, ENMU_AR_BANK_ACCESS_RIGHT bank_access_right);
/**
* @brief  efuse 获取对应bank_id权限接口
* @param  bank_id：对应bank id (1-2)，bank_access_right：对应bank权限
* @retval 0 成功 , 其它 失败.
* @note   关闭bank1 及 bank2的读写权限，两个bank只能被硬件读取，cpu无法读取
*/
int ar_hal_efuse_get_bank_access_right(int bank_id, ENMU_AR_BANK_ACCESS_RIGHT *bank_access_right);
/**
* @brief  efuse 对应bank reserved bits读取接口
* @param  bank_id：对应bank id (0-3)；bank_offset：对应bank reserved bits 已经写入offset；key_bit_len：对应写入bit长度；key_value：对应写入值地址
* @retval 0 成功 , 其它 失败.
* @note   reserved total length， bank0：192bits；bank1：416bits；bank2：448bits
*/
int ar_hal_efuse_read_reserve(int bank_id, unsigned int bank_offset, int key_bit_len, unsigned char * key_value);
/**
* @brief  efuse 对应bank reserved bits写入接口
* @param  bank_id：对应bank id (0-3)；bank_offset：对应bank reserved bits 已经写入offset；key_bit_len：对应写入bit长度；key_value：对应写入值地址
* @retval 0 成功 , 其它 失败.
* @note   
*/
int ar_hal_efuse_write_reserve(int bank_id, unsigned int bank_offset, int key_bit_len, unsigned char * key_value);
#endif
