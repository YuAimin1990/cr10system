#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define GPIO_DATA_HIGH 1
#define GPIO_DATA_LOW 0
//#define ar_delay_us(arg) udelay(arg)

#define IPC_MAX_VO_WINDOW_NUM  9
#define COLOR_RGB_BLUE     0x0000FF
typedef enum{
    LCD_ID_ST7703_LCD,
    LCD_ID_7INCH_LCD,
    LCD_ID_8INCH_LCD,
    LCD_ID_DVP_OLED ,
    LCD_ID_5INCH_LCD,
    LCD_ID_MAX,
}LCD_INDEX_E;
#if 0
enum
{
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_0_PARA     = 0x03,
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_1_PARA     = 0x13,
    MIPI_DATA_TYPE_GENERIC_SHORT_WRITE_2_PARA     = 0x23,

    MIPI_DATA_TYPE_GENERIC_SHORT_READ_0_PARA      = 0x04,
    MIPI_DATA_TYPE_GENERIC_SHORT_READ_1_PARA      = 0x14,
    MIPI_DATA_TYPE_GENERIC_SHORT_READ_2_PARA      = 0x24,

    MIPI_DATA_TYPE_DCS_SHORT_WRITE_0_PARA         = 0x05,
    MIPI_DATA_TYPE_DCS_SHORT_WRITE_1_PARA         = 0x15,

    MIPI_DATA_TYPE_DCS_SHORT_READ_0_PARA          = 0x06,

    MIPI_DATA_TYPE_SET_MAX_READ_SIZE              = 0x37,

    MIPI_DATA_TYPE_NULL_PACKET                    = 0x09,
    MIPI_DATA_TYPE_BLANK_PACKET                   = 0x19,
    MIPI_DATA_TYPE_GENERIC_LONG_WRITE             = 0x29,
    MIPI_DATA_TYPE_DCS_LONG_WRITE                 = 0x39,

    MIPI_DATA_TYPE_PACKED_PIXEL_RGB565            = 0x0E,
    MIPI_DATA_TYPE_PACKED_PIXEL_RGB666            = 0x1E,
    MIPI_DATA_TYPE_LOOSELY_PACKED_PIXEL_RGB666    = 0x2E,
    MIPI_DATA_TYPE_PACKED_PIXEL_RGB888            = 0x3E
};
#endif
int dsi_short_cmd_2pra(VO_DEV voDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms);
int IPC_VO_dsi_long_cmd(VO_DEV voDev, uint8_t data_type, uint8_t *data, uint16_t size, int delayms);
int dsi_short_cmd(VO_DEV voDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms);

int IPC_VO_read_panel_id();
int read_reg32(unsigned long addr);
int write_reg32(unsigned long addr, unsigned int data);
int IPC_VO_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr, LCD_INDEX_E eLcdId);

int IPC_VO_Lcd_Init(VO_DEV VoDev, LCD_INDEX_E eLcdId);

#ifdef __cplusplus
}
#endif
#endif

