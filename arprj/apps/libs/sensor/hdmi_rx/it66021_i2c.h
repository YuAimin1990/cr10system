#ifndef __IT66021_I2C_H__
#define __IT66021_I2C_H__

#include <stdint.h>
#define I2C_TRUE 1
#define I2C_FALSE -1



typedef enum
{
	AR_I2C_COMPONENT_0 = 0,
	AR_I2C_COMPONENT_1,
	AR_I2C_COMPONENT_2,
	AR_I2C_COMPONENT_3,
	AR_I2C_COMPONENT_4,
	AR_I2C_COMPONENT_5,
	AR_I2C_COMPONENT_MAX,
}ar_i2c_component;

typedef enum
{
	AR_I2C_STANDARD_SPEED = 0,//100K
	AR_I2C_FAST_SPEED,		  //400K
	AR_I2C_HIGH_SPEED,		  //1M
	AR_I2C_UNKNOWN_SPEED,
}ar_i2c_speed;

enum
{
    AR_I2C_SLAV_7BITS = 7
};

struct i2c_msg {
	uint8_t *buf;				/* pointer to msg data		*/
	uint16_t buf_len;		/* read length			*/
	uint8_t *reg_addr;		/* register address			*/
	uint16_t reg_len;		/* register addr length		*/
	uint8_t dev_addr;
};


/*
*en_component          The I2C controller number, the right number should be 0-5 and totally
*addr                  I2C address of the target device.
*speed            	   The I2C speed of the I2C clock of the I2C controller.
*/
struct ar_i2c {
	ar_i2c_component component;
	ar_i2c_speed speed;
	uint16_t flags;
	uint16_t addr;		/* slave address			*/
	int fd;
};
extern struct ar_i2c g_i2c;

int ar_i2c_master_writebyte(struct i2c_msg *wr_msg);
int ar_i2c_master_init(struct ar_i2c *i2c);
int ar_i2c_master_readbyte(struct i2c_msg *rd_msg);
int ar_i2c_master_readdata(struct i2c_msg *wr_msg);
int ar_i2c_master_writedata(struct i2c_msg *wr_msg);

#endif

