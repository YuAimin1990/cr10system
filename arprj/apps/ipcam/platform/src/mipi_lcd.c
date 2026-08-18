#include "ar_common.h"
#include "mpi_vo.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "lcd.h"
#include "util_common.h"
#include "osal.h"

extern int IPC_VO_8inch_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr);
extern int IPC_VO_8inch_Lcd_Init(VO_DEV VoDev);
extern int APP_VO_DVP_OLED_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr);
extern int APP_VO_DVP_OLED_Lcd_Init(VO_DEV VoDev);
extern int IPC_PF_VO_7INCH_LCD_Init(VO_DEV VoDev);
extern int IPC_PF_VO_5INCH_LCD_Init(VO_DEV VoDev);
extern int IPC_VO_5inch_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr);

int dsi_short_cmd(VO_DEV voDev, uint8_t data_type, uint8_t param1, uint8_t param2, int delayms){
    VO_DSI_CMD_S pstCmd;

    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = param2 << 8 | param1;
    pstCmd.pu8Cmd = NULL;
    AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

int IPC_VO_dsi_long_cmd(VO_DEV voDev, uint8_t data_type, uint8_t *data, uint16_t size, int delayms){
    VO_DSI_CMD_S pstCmd;

    pstCmd.u16DataType = data_type;
    pstCmd.u16CmdSize = size;
    pstCmd.pu8Cmd = data;
    AR_MPI_VO_Dsi_Cmd(voDev, &pstCmd);
    ar_delay(delayms);
    return 0;
}

#define DSI_HOST_REG_BASE     0x01120000
//This register configures the mode of operation between Video or Command Mode. (Commands can still...
#define DSI_HOST_REG_MODE_CFG               0x34
//his register sets the header for new packets sent using the Generic interface.
#define DSI_HOST_REG_GEN_HDR                0x6c
//This register sets the payload for packets sent using the Generic interface and, when read returns...
#define DSI_HOST_REG_GEN_PLD_DATA           0x70
//This register configures contains information about the status of FIFOs related to DBI and Generic...
#define DSI_HOST_REG_CMD_PKT_STATUS         0x74
//This register controls resets and the PLL of the D-PHY.
#define DSI_HOST_REG_PHY_RSTZ               0xa0

#define AR_ALIGN4(_x)              (((_x)+0x03)&~0x03)


static int reg_ops(unsigned long addr, int wr, int data){
    void *map_base, *virt_addr;
    off_t target = addr;
    unsigned page_size, mapped_size, offset_in_page;
    unsigned width = 8 * sizeof(int);
    uint64_t read_result = 0;

    int fd = open("/dev/mem", wr ? (O_RDWR | O_SYNC) : (O_RDONLY | O_SYNC));
    mapped_size = page_size = getpagesize();
    offset_in_page = (unsigned)target & (page_size - 1);
    if (offset_in_page + width > page_size) {
            /* This access spans pages.
             * Must map two pages to make it possible: */
            mapped_size *= 2;
    }
    map_base = mmap(NULL,
                    mapped_size,
                    wr ? (PROT_READ | PROT_WRITE) : PROT_READ,
                    MAP_SHARED,
                    fd,
                    target & ~(off_t)(page_size - 1));
    if (map_base == MAP_FAILED)
        return -1;

    virt_addr = (char*)map_base + offset_in_page;
    if(wr){
        *(volatile uint32_t*)virt_addr = data;
    }else{
        read_result = *(volatile uint32_t*)virt_addr;
    }
    if (munmap(map_base, mapped_size) == -1)
        printf("unmap reg virt failed! 0x%lx\n", addr);
    close(fd);
    return read_result;

}

int read_reg32(unsigned long addr)
{
    return reg_ops(addr, 0, 0);
}
int write_reg32(unsigned long addr, unsigned int data)
{
    reg_ops(addr, 1, data);
    return 0;
}

static void dsi_reg_write(unsigned int addr, unsigned int data)
{
    write_reg32(DSI_HOST_REG_BASE + addr, data);
}

static unsigned int dsi_reg_read(unsigned int addr)
{
    return read_reg32(DSI_HOST_REG_BASE + addr);
}

static int dsi_read(uint8_t data_type, uint8_t addr, uint8_t *data, uint32_t len)
{
    uint32_t  reg = 0;
    uint32_t  cnt = 0;
    uint32_t  align_len = AR_ALIGN4(len);
    uint32_t  left_len = len;
    uint32_t  copy_len;
    int       ret = 0;
    int       mode_cfg_bak = dsi_reg_read(DSI_HOST_REG_MODE_CFG);

    /* enter cmd mode */
    dsi_reg_write(DSI_HOST_REG_MODE_CFG,  0x1);

    /* set read size */
    dsi_reg_write(DSI_HOST_REG_GEN_HDR,  (align_len << 8) | 0x37);
    ar_delay(1);
    dsi_reg_write(DSI_HOST_REG_GEN_HDR,   (((uint32_t)addr) << 8) | data_type);
    reg = dsi_reg_read(DSI_HOST_REG_CMD_PKT_STATUS);
    ar_debug("wait ready reg=0x%x.", reg);
    while(reg & 0x50)
    {
        if (cnt++ >= 100)
        {
            ar_err("read timeout");
            ret = -1;
            goto End;
        }

        reg = dsi_reg_read(DSI_HOST_REG_CMD_PKT_STATUS);
        ar_debug("wait ready reg=0x%x.", reg);
        ar_delay(1);
    }

    for (int i=0; i < align_len/4; i++)
    {
        reg = dsi_reg_read(DSI_HOST_REG_GEN_PLD_DATA);
        ar_always("reg = 0x%x", reg);
        if (left_len >= 4)
            copy_len = 4;
        else
            copy_len = left_len;

        for (int j=0; j < copy_len; j++)
        {
            *data = (reg >> (j*8)) & 0xff;
            ar_debug("data: 0x%x", *data);
            data++;
        }

        left_len -= copy_len;
    }

End:
    /* enter video mode for dsi to recieve dphy's stop signal */
    dsi_reg_write(DSI_HOST_REG_MODE_CFG,  0x0);
    ar_delay(1);
    dsi_reg_write(DSI_HOST_REG_PHY_RSTZ,   0xd);
    ar_delay(1);
    dsi_reg_write(DSI_HOST_REG_PHY_RSTZ,   0xf);
    ar_delay(1);
    dsi_reg_write(DSI_HOST_REG_MODE_CFG,  mode_cfg_bak);
    ar_always("reset dphy and restore mode:%d", mode_cfg_bak);
    return ret;
}

int IPC_VO_read_panel_id(VO_DEV voDev){
    uint8_t test_data[4] = {0};

    if(dsi_read(0x14, 0x04, test_data, 0x3) == 0){
        int id = (test_data[3]<<24) | (test_data[2]<<16) | (test_data[1]<<8) | test_data[0];
        ar_always("panel_id : 0x%x \n", id);
        return id;
    }
    return -1;
}

int IPC_VO_Get_Display_Param(VO_PUB_ATTR_S *stPubAttr, LCD_INDEX_E eLcdId){
    switch (eLcdId)
    {
        case LCD_ID_7INCH_LCD:
            stPubAttr->stSyncInfo.u16Hpw  = 96;
            stPubAttr->stSyncInfo.u16Hbb  = 216;
            stPubAttr->stSyncInfo.u16Hact = 1024;
            stPubAttr->stSyncInfo.u16Hfb  = 216;
            stPubAttr->stSyncInfo.u16Vpw  = 10;
            stPubAttr->stSyncInfo.u16Vbb  = 23;
            stPubAttr->stSyncInfo.u16Vact = 600;
            stPubAttr->stSyncInfo.u16Vfb  = 12;
            break;
        case LCD_ID_8INCH_LCD:
            IPC_VO_8inch_Get_Display_Param(stPubAttr);
            break;
        case LCD_ID_DVP_OLED:
            APP_VO_DVP_OLED_Get_Display_Param(stPubAttr);
            break;
        case LCD_ID_ST7703_LCD:
            IPC_VO_8inch_Get_Display_Param(stPubAttr);
            break;
		case LCD_ID_5INCH_LCD:
            IPC_VO_5inch_Get_Display_Param(stPubAttr);
            break;
        default:
            PRINT_ERR("no matched panel id: %d!\n", eLcdId);
            break;
    }
    return 0;
}

int IPC_VO_Lcd_Init(VO_DEV VoDev, LCD_INDEX_E eLcdId){
    int ret = -1;
    PRINT_INFO("init panel, id: %d!\n", eLcdId);
    switch (eLcdId)
    {
        case LCD_ID_8INCH_LCD:
            PRINT_INFO("matched to 8 inch panel, id: %d!\n", eLcdId);
            ret = IPC_VO_8inch_Lcd_Init(VoDev);
            break;
        case LCD_ID_DVP_OLED:
            ret = APP_VO_DVP_OLED_Lcd_Init(VoDev);
            break;
	    case LCD_ID_7INCH_LCD:
		    ret = IPC_PF_VO_7INCH_LCD_Init(VoDev);
			break;
        case LCD_ID_ST7703_LCD:
            // ret = IPC_VO_8inch_Lcd_Init(VoDev);
			break;
        case LCD_ID_5INCH_LCD:
            ret = IPC_PF_VO_5INCH_LCD_Init(VoDev);
			break;
        default:
            PRINT_ERR("no matched panel id: %d!\n", eLcdId);
            break;
    }
    return ret;
}

