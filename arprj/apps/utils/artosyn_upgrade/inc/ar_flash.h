/****************************************************************************
 * Copyright (C) 2019 Shanghai Artosyn Microelectronics Limited.            *
 ****************************************************************************/

/** \addtogroup bsp 
 *  @{
 */

/**
 * @file ar_flash.h
 * @author Artosyn
 * @date 9 May 2019
 * @brief artosyn flash program api
 * @example <app/artosyn/rootfs_app/artosyn_upgrade/src/main.c>
 */

#ifndef __AR_FLASH_H__
#define __AR_FLASH_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define ARTOSYN_MAGIC     0x4152544F
typedef struct spl_header
{
	unsigned int magic; 		//'A','R',0x93,0x01
	unsigned short img_type;	//image type.
	unsigned short header_len;  //header length
	unsigned int header_checksum;//header checksum
	unsigned int img_version;	//image version
	unsigned int flag;			//boot flag
	unsigned int boot_info;     //rom code use this field to stroe boot device and partition.
	unsigned int spl_load_addr;
	unsigned int spl_len;
	unsigned int troot_load_addr;
	unsigned int troot_len;
	unsigned int signature_load_addr;
	unsigned int signature_len;
	unsigned int spl_dtb_offset;
	unsigned long checksum;		//payload checksum
	unsigned int reserved; //pad to 64 byte
	unsigned char patch[0];	//reserved for spl use
} __attribute__ ((packed)) spl_header;

/**
* @brief  callback funtion of upgrade
* @param  state upgrade status
*        percent upgrade progress percent
* @return 0	ok.
*         <0 error.
*/
typedef int (*ar_flash_cb)(int state, int percent);

enum {
	ERR_SUCCESS    = 0,
	ERR_VERIFY     = -1,
	ERR_PROGRAM    = -2,
	ERR_ERASE      = -3,
	ERR_FLASH_TYPE = -4,
	ERR_IMG_SIZE   = -5,
	ERR_OTHER      = -6,
	ERR_DDR        = -7,
};

enum {
	STAT_VERIFY_IMAGE = 0,
	STAT_START = 1,
	STAT_IN_PROGRESS,
	STAT_DONE,
	STAT_ENTER_CLEAN_SYSTEM,
	STAT_ENTER_CLEAN_SYSTEM_FAILED,
};

/**
* @brief upgrade prepare, it invocate ota-prepare.sh and check available memory
* 		 size. Attention, for compatibility, ar_flash_program invocate it too,
* 		 it don't matter.
* @return 0	ok.
* 		  <0 error.
*/
int ar_upgrade_prepare(void);

/**
* @brief do not invocate ar_prepare_upgrade, only upgrade
* @param img upgrade image buffer
*        len upgrade image size
*        cb  callback function
* @return 0	ok.
* 		  <0 error.
*/
int ar_upgrade(char *img, int len, ar_flash_cb cb);

/**
* @brief do not invocate ar_prepare_upgrade, only upgrade
* @param filename file name
*        cb  callback function
* @return 0	ok.
* 		  <0 error.
*/
int ar_upgrade_ex(char *filename, ar_flash_cb cb);

/**
* @brief  artosyn upgrade api, this interface is equal to ar_upgrade_prepare + ar_upgrade,
* 		  reserve this interface for backward compatibility.
* 		  you should use new upgrade interfaces: ar_upgrade_prepare + ar_upgrade,
* 		  if ar_upgrade_prepare failed, you can invocate ar_upgrade mandatorily
* 		  in nor or emmc device. but suggest you check your system, to see what
* 		  should be killed or closed.
* @param img upgrade image buffer
*        len upgrade image size
*        cb  callback function
* @return 0	ok.
*         <0 error.
*/
int ar_flash_program(char *img, int len, ar_flash_cb cb);

/**
* @brief  enter clean system, system will be reboot after invocated
* @param  cb  callback function
* @return 0	ok.
* 		  <0 error.
*/
int ar_enter_clean_system(ar_flash_cb cb);

/**
* @brief  enter clean system, system will be reboot after invocated
* @param  cb  callback function
* 		  params params for enter-clean-system.sh,
* 		         format: mode:runshell, eg: web:/usrdata/server-prepare.sh
* @return 0	ok.
* 		  <0 error.
*/
int ar_enter_clean_system_ext(ar_flash_cb cb, char *params);

/**
* @brief  set whether upgrade or not while ddr setting is changed
* @param  true: force upgrade and ignore changes of ddr setting
* 		  false: exit upgrade while ddr setting is changed
*/
void artosyn_set_force_upgrade(bool force);

/**
* @brief  get sdk Version
* @param  img [in] upgrade image buffer
*         len [in] upgrade image size
*         sdkver [out] sdk version to be stored
* @return 0	ok.
* 		  <0 error.
*/
int ar_flash_get_sdkversion(char *img, int len, char *sdkver);

/**
* @brief  set upgrade mode: develop or product, if do not invocate it,
* 		  product phase is default
* @param  true: product phase
* 		  false: develop phase
*/
void ar_upgrade_product(bool product);

/*
* @brief  resize the last partition
*
* @param  0 ok, Otherwise error.
*/
int ar_emmc_last_partition_resize(void);
#ifdef __cplusplus
}
#endif

#endif
/** @}*/
