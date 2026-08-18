#ifndef _GE2D_UTIL_H_
#define _GE2D_UTIL_H_

#define CGU_REG_BASE  0x01070000

#define AR_SET_REG_BITS(reg,val,start_bit,end_bit) (reg)=((val)<<(start_bit))| ((reg)&~((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))
#define AR_GET_REG_BITS(reg,start_bit,end_bit) (((reg)&((0xffffffff>>(32-((end_bit)-(start_bit)+1)))<<(start_bit)))>>(start_bit))
#define __REG32__(addr) (*(volatile unsigned int *)(ar_dev_pa2va(addr)))
#define __REG64__(addr) (*(volatile uint64_t *)(ar_dev_pa2va(addr)))
#define __REG8__(addr) (*(volatile unsigned char *)(ar_dev_pa2va(addr)))
#define AR_ALIGN4(_x)              (((_x)+0x03)&~0x03)
#define AR_ALIGN8(_x)              (((_x)+0x07)&~0x07)
#define AR_ALIGN16(_x)             (((_x)+0x0f)&~0x0f)
#define AR_ALIGN32(_x)             (((_x)+0x1f)&~0x1f)
#define AR_ALIGN64(_x)             (((_x)+0x3f)&~0x3f)
#define AR_ALIGN128(_x)            (((_x)+0x7f)&~0x7f)
#define AR_ALIGN256(_x)            (((_x)+0xff)&~0xff)
#define AR_ALIGN512(_x)            (((_x)+0x1ff)&~0x1ff)
#define AR_ALIGN16384(_x)          (((_x)+0x3fff)&~0x3fff)

#define IRQ_OFFSET         (32)

#endif // _GE2D_UTIL_H_
