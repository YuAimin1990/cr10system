#ifndef __AR9311_BOOT_H
#define __AR9311_BOOT_H

#define SPL_RUN_DONE 				(1UL << 31)//BIT(31)
#define SEC_BOOT_EN 				(1UL << 30)//BIT(30)


#define ARTOSYN_BOOT_MODE_VALUE_REG 	0x0A10602C

#define BOOT_VALUE_ROM_QSPI_NOR_1V8 	0x0000
#define BOOT_VALUE_ROM_QSPI_NAND_1V8 	0x0001
#define BOOT_VALUE_ROM_QSPI_NOR_3V3 	0x0002
#define BOOT_VALUE_ROM_QSPI_NAND_3V3 	0x0003
#define BOOT_VALUE_ROM_EMMC 			0x0004
#define BOOT_VALUE_QSPI_NOR 			0x0005
#define BOOT_VALUE_ROM_USB 				0x0006
#define BOOT_VALUE_ROM_UART_TIMEOUT 	0x0007

#define AR_BOOT_DEVICE_QSPI_NOR_1V8                0x0001
#define AR_BOOT_DEVICE_QSPI_NAND_1V8               0x0002
#define AR_BOOT_DEVICE_QSPI_NOR_3V3                0x0004
#define AR_BOOT_DEVICE_QSPI_NAND_3V3               0x0008
#define AR_BOOT_DEVICE_EMMC                                0x0010
#define AR_BOOT_DEVICE_QSPI_NOR_XIP                0x0020
#define AR_BOOT_DEVICE_USB                                 0x0040
#define AR_BOOT_DEVICE_UART                                0x0080

#define SHARED_RAM_BASE			0x00100000	//0x00100000
#define SMP_INFO_SIZE			(0x40)
#define SPL_HEADER_BUF_BASE (SHARED_RAM_BASE + SMP_INFO_SIZE)

int get_boot_device(void);
void ar_security_setup(void);

#endif /* __AR9311_BOOT_H */
