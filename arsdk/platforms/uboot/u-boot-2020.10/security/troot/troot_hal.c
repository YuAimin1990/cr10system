/*
 * Artosync tRoot verification driver
 */

#include "troot.h"

#define TROOT_GPI  0x64530004
#define TROOT_GPO  0x64530008

#define TROOT_REG1 0x60631024

static inline void troot_write_reg32(unsigned int reg32, unsigned int value)
{
	*(volatile unsigned int *)reg32 = value;
}

static inline unsigned int troot_read_reg32(unsigned int reg32)
{
	return *(volatile unsigned int *)reg32;
}

static void troot_boot(void)
{
	unsigned int data32 = 0x0;
	
	serial_puts(1, "\n1---\n");
	troot_write_reg32(TROOT_REG1, 0xffffffff);  // release troot rst_n

	serial_puts(1, "\n2--\n");
	while(1) {
		data32 = troot_read_reg32(TROOT_GPO);

	   	// wait GPO[0] == 1
		if(data32 & 0x1 == 0x1)
			break;
	}
	serial_puts(1, "\n3---\n");
}

static int troot_verify_image(void)
{
	unsigned int data32 = 0x0;

	serial_puts(1, "\n4---\n");
	// write 0x1 to GPI, start ot verity image
	troot_write_reg32(TROOT_GPI, 0x1);

	serial_puts(1, "\n5---\n");
	while(1) {
		data32 = troot_read_reg32(TROOT_GPO);

	   	// wait GPO[1] or GPO[2] == 1
		if(data32 & 0x6)
			break;
	}

	serial_puts(1, "\n6---\n");
	// write 0x0 to GPI, check verity result
	troot_write_reg32(TROOT_GPI, 0x0);

	serial_puts(1, "\n7---\n");
	// wait GPO[1]/GPO[2] == 1
	data32 = troot_read_reg32(TROOT_GPO);
	data32 &= 0x6;
	if(data32 == 0x2) {
		return 0;//Success
	}
	else {
		return 1;//Fail
	}
	serial_puts(1, "\n8---\n");
}

int troot_verification(void)
{
	troot_boot();

 	return troot_verify_image();
}
