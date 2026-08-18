#include "it66021_i2c.h"
#include "hal_vin_log.h"

struct ar_i2c g_i2c;
int ar_i2c_master_init(struct ar_i2c *i2c)
{
    i2c->fd = ar_i2c_open(i2c->component);
    if (i2c->fd < 0)
    {
        ar_printf("open i2c adaptor = %d failed\n", i2c->component);
    }
    else
    {
        ar_printf("open i2c adaptor = %d sucessfully!\n", i2c->component);
    }
}


int ar_i2c_master_writebyte(struct i2c_msg *wr_msg)
{
    uint8_t addr = 0, val = 0;
    int ret = 0;
    if (wr_msg->buf_len == 1)
        val = wr_msg->buf[0];
    else
        ar_printf("buf_len not correct\n");
    if (wr_msg->reg_len == 1)
        addr = wr_msg->reg_addr[0];
    else
        ar_printf("reg_len not correct\n");
    ret = ar_i2c_master_write_byte(g_i2c.fd, wr_msg->dev_addr, g_i2c.flags, addr, val);

    return ret;
}

int ar_i2c_master_readbyte(struct i2c_msg *rd_msg)
{
    uint8_t addr = 0;

    if (rd_msg->reg_len == 1)
        addr = rd_msg->reg_addr[0];
    else
        ar_printf("reg_len not correct\n");
    rd_msg->buf[0] = ar_i2c_master_read_byte(g_i2c.fd, rd_msg->dev_addr, g_i2c.flags, addr);
    return I2C_TRUE;
}

int ar_i2c_master_readdata(struct i2c_msg *rd_msg)
{
    int ret = 0;
    ret = ar_i2c_master_read(g_i2c.fd, rd_msg->dev_addr, g_i2c.flags, \
    rd_msg->reg_addr, rd_msg->reg_len, rd_msg->buf, rd_msg->buf_len);

    return ret;
}

int ar_i2c_master_writedata(struct i2c_msg *wr_msg)
{
    int ret = 0;
    ret = ar_i2c_master_write(g_i2c.fd, wr_msg->dev_addr, g_i2c.flags,  \
    wr_msg->buf, wr_msg->buf_len);

    return ret;
}
