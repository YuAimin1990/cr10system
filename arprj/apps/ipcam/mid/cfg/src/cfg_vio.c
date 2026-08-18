#include "cfg_vio.h"

IPC_CFG_VIO_S g_stRunVioCfg;
IPC_VI_MAX_RESOURCE_S g_stViMaxResorce = {0};

CFG_MAP ViMap[IPC_MAX_PIPELINE_NUM][30] =
{
    //sensor 0
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[0].mipi_id),   CFG_DATA_TYPE_U32, "0", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[0].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[0].i2c_bus), CFG_DATA_TYPE_U32, "1",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[0].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[0].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[0].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[0].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[0].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[0].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[0].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[0].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[0].power_gpio), CFG_DATA_TYPE_STRING, "C1_0",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[0].reset_gpio), CFG_DATA_TYPE_STRING, "C1_1",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[0].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[0].channel_0_enable), CFG_DATA_TYPE_U32, "1",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[0].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[0].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[0].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[0].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[0].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[0].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[0].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[0].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 1
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[1].mipi_id),   CFG_DATA_TYPE_U32, "1", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[1].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[1].i2c_bus), CFG_DATA_TYPE_U32, "2",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[1].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[1].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[1].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[1].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[1].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[1].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[1].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[1].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[1].power_gpio), CFG_DATA_TYPE_STRING, "C1_0",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[1].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[1].reset_gpio), CFG_DATA_TYPE_STRING, "C1_3",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[1].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[1].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[1].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[1].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[1].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[1].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[1].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[1].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[1].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,	"Enable raw compress"}, 
        {NULL,}
    },
    //sensor 2
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[2].mipi_id),   CFG_DATA_TYPE_U32, "2", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[2].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[2].i2c_bus), CFG_DATA_TYPE_U32, "3",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[2].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[2].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[2].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[2].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[2].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[2].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[2].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[2].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[2].power_gpio), CFG_DATA_TYPE_STRING, "D1_0",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[2].reset_gpio), CFG_DATA_TYPE_STRING, "D1_1",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[2].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[2].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[2].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[2].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[2].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[2].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[2].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[2].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[2].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[2].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 3
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[3].mipi_id),   CFG_DATA_TYPE_U32, "3", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[3].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[3].i2c_bus), CFG_DATA_TYPE_U32, "0",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[3].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[3].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[3].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[3].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[3].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[3].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[3].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[3].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[3].power_gpio), CFG_DATA_TYPE_STRING, "D1_0",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[3].reset_gpio), CFG_DATA_TYPE_STRING, "D1_3",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[3].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[3].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[3].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[3].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[3].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[3].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[3].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[3].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[3].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[3].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 4
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[4].mipi_id),   CFG_DATA_TYPE_U32, "0", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[4].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[4].i2c_bus), CFG_DATA_TYPE_U32, "0",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[4].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[4].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[4].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[4].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[4].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[4].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[4].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[4].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[4].power_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[4].reset_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[4].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[4].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[4].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[4].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[4].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[4].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[4].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[4].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[4].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[4].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 5
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[5].mipi_id),   CFG_DATA_TYPE_U32, "0", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[5].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[5].i2c_bus), CFG_DATA_TYPE_U32, "0",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[5].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[5].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[5].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[5].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[5].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[5].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[5].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[5].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[5].power_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[5].reset_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[5].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[5].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[5].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[5].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[5].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[5].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[5].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[5].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[5].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[5].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 6
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[6].mipi_id),   CFG_DATA_TYPE_U32, "0", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[6].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[6].i2c_bus), CFG_DATA_TYPE_U32, "0",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[6].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[6].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[6].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[6].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[6].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[6].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[6].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[6].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[6].power_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[6].reset_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[6].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[6].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[6].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[6].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[6].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[6].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[6].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[6].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[6].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[6].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    },
    //sensor 7
    {
        {"mipi_id",  &(g_stRunVioCfg.vi[7].mipi_id),   CFG_DATA_TYPE_U32, "0", "rw", 0, 8,  "sensor mipi_id"},
        {"sensor",  &(g_stRunVioCfg.vi[7].sensor),   CFG_DATA_TYPE_STRING, "imx307", "rw", 1, 32,  "sensor name: null means no sensor"},
        {"i2c_bus", &(g_stRunVioCfg.vi[7].i2c_bus), CFG_DATA_TYPE_U32, "0",  "rw", 0, 7,  NULL},
        {"width",  &(g_stRunVioCfg.vi[7].width),   CFG_DATA_TYPE_U32, "1920", "rw", 0, 5000,  NULL},
        {"height",  &(g_stRunVioCfg.vi[7].height),   CFG_DATA_TYPE_U32, "1080", "rw", 0, 5000,  NULL},
        {"stride",  &(g_stRunVioCfg.vi[7].stride),   CFG_DATA_TYPE_U32, "2048", "rw", 0, 5000,  NULL},
        {"fps",         &(g_stRunVioCfg.vi[7].fps), CFG_DATA_TYPE_U32, "25",  "rw", 1, 60,  NULL},
        {"flip",         &(g_stRunVioCfg.vi[7].flip), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"mirror",         &(g_stRunVioCfg.vi[7].mirror), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"HDR_enable",         &(g_stRunVioCfg.vi[7].HDR_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"vfe_mode",         &(g_stRunVioCfg.vi[7].vfe_mode), CFG_DATA_TYPE_U32, "0",  "rw", 0, 5,  "enable AiISP or ceva-HDR"},
        {"power_gpio",         &(g_stRunVioCfg.vi[7].power_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"reset_gpio",         &(g_stRunVioCfg.vi[7].reset_gpio), CFG_DATA_TYPE_STRING, "Cn_m",  "rw", 3, 8,  NULL},
        {"comm_gpio",         &(g_stRunVioCfg.vi[7].comm_gpio), CFG_DATA_TYPE_STRING, "",  "rw", 3, 8,  NULL},
        {"channel_0_enable",   &(g_stRunVioCfg.vi[7].channel_0_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_0_fps",      &(g_stRunVioCfg.vi[7].channel_0_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_1_enable",   &(g_stRunVioCfg.vi[7].channel_1_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  NULL},
        {"channel_1_width",   &(g_stRunVioCfg.vi[7].channel_1_width), CFG_DATA_TYPE_U32, "640",  "rw", 0, 5000,  NULL},
        {"channel_1_height",   &(g_stRunVioCfg.vi[7].channel_1_height), CFG_DATA_TYPE_U32, "480",  "rw", 0, 5000,  NULL},
        {"channel_1_stride",   &(g_stRunVioCfg.vi[7].channel_1_stride), CFG_DATA_TYPE_U32, "1024",  "rw", 0, 5000,  NULL},
        {"channel_1_fps",   &(g_stRunVioCfg.vi[7].channel_1_fps), CFG_DATA_TYPE_U32, "25",  "rw", 0, 60,  NULL},
        {"channel_2_raw_enable",   &(g_stRunVioCfg.vi[7].channel_2_raw_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw"},
        {"raw_compress_enable",   &(g_stRunVioCfg.vi[7].raw_compress_enable), CFG_DATA_TYPE_U32, "0",  "rw", 0, 1,  "Enable raw compress"}, 
        {NULL,}
    }

};

CFG_MAP VoMap[] =
{
    {"enable",  &(g_stRunVioCfg.vo.enable),   CFG_DATA_TYPE_U32, "1", "rw", 0, 1,  NULL},
    {"type",  &(g_stRunVioCfg.vo.type),   CFG_DATA_TYPE_STRING, "hdmi", "rw", 0, 32,  NULL},
    {"lcd_id", &(g_stRunVioCfg.vo.lcd_id), CFG_DATA_TYPE_U32, "0",  "rw", 0, 60,  NULL},
    {"width",  &(g_stRunVioCfg.vo.width),   CFG_DATA_TYPE_U32, "640", "rw", 0, 5000,  NULL},
    {"height",  &(g_stRunVioCfg.vo.height),   CFG_DATA_TYPE_U32, "480", "rw", 0, 5000,  NULL},
    {"stride",  &(g_stRunVioCfg.vo.stride),   CFG_DATA_TYPE_U32, "640", "rw", 0, 5000,  NULL},
    {"fps",         &(g_stRunVioCfg.vo.fps), CFG_DATA_TYPE_U32, "60",  "rw", 1, 60,  NULL},
    {"square",         &(g_stRunVioCfg.vo.square), CFG_DATA_TYPE_U32, "1",  "rw", 1, 3,  NULL},
	{NULL,}
};

CFG_MAP ViCfgMap[] =
{
    {"cam_mode",  &(g_stRunVioCfg.vi_cfg.cam_mode),   CFG_DATA_TYPE_U32, "0", "rw", 0, 4,  NULL},
    {"vb_blk_cnt",  &(g_stRunVioCfg.vi_cfg.vb_blk_cnt),   CFG_DATA_TYPE_U32, "6", "rw", 0, 20,  NULL},
    {"hdr_freq",  &(g_stRunVioCfg.vi_cfg.hdr_freq),   CFG_DATA_TYPE_U32, "150000000", "rw", 75000000, 700000000,  NULL},
    {"isp_freq",  &(g_stRunVioCfg.vi_cfg.isp_freq),   CFG_DATA_TYPE_U32, "150000000", "rw", 75000000, 700000000,  NULL},
    {"vif_freq",  &(g_stRunVioCfg.vi_cfg.vif_freq),   CFG_DATA_TYPE_U32, "400000000", "rw", 75000000, 700000000,  NULL},
    {"mipi_freq", &(g_stRunVioCfg.vi_cfg.mipi_freq),   CFG_DATA_TYPE_U32, "100000000", "rw", 75000000, 700000000,  NULL},
    {NULL,}
};


void IPC_CFG_VIO_Print()
{
    int i = 0;
    printf("*************** vio info **************\n");
    printf("vi\n");
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        CfgPrintMap(ViMap[i]);
    }
    printf("\n");
    printf("vi_cfg");
    CfgPrintMap(ViCfgMap);
    printf("\n");
    printf("vo\n");
    CfgPrintMap(VoMap);
    printf("\n");

    printf("\n");
    printf("*************** vio info **************\n\n");
}

AR_S32 IPC_CFG_VIO_Save()
{
    cJSON *root;
    char *out;
    int i = 0;

    root = cJSON_CreateObject();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        char tmp[32] = {0};
        sprintf(tmp, "vi_%d", i);

        CfgAddCjson(root, tmp, ViMap[i]);
    }
    CfgAddCjson(root, "vi_cfg", ViCfgMap);
    CfgAddCjson(root, "vo", VoMap);

    out = cJSON_Print(root);

    AR_S32 ret = CfgWriteToFile(VIO_CFG_FILE, out);
    if (ret != 0)
    {
        PRINT_ERR("CfgWriteToFile %s error.", VIO_CFG_FILE);
        return -1;
    }

    free(out);
    cJSON_Delete(root);

    return 0;
}

static void IPC_CFG_VIO_SetSingle307_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
}

//The sensor board only connect to mipi1 and mipi2
static void IPC_CFG_VIO_SetSingle230_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    strcpy(pVioCfg->vi[0].sensor, "sc230ai");
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].mipi_id = 1;
    pVioCfg->vi[0].i2c_bus = 2;
    strcpy(pVioCfg->vi[0].power_gpio, "C1_2");
    strcpy(pVioCfg->vi[0].reset_gpio, "C1_3");    
}

static void IPC_CFG_VIO_SetDual230_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    strcpy(pVioCfg->vi[0].sensor, "sc230ai");
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].mipi_id = 1;
    pVioCfg->vi[0].i2c_bus = 2;
    strcpy(pVioCfg->vi[0].power_gpio, "C1_2");
    strcpy(pVioCfg->vi[0].reset_gpio, "C1_3");
    
    strcpy(pVioCfg->vi[1].sensor, "sc230ai");
    pVioCfg->vi[1].fps = 30;
    pVioCfg->vi[1].channel_0_fps = 30;
    pVioCfg->vi[1].channel_1_fps = 30;
    pVioCfg->vi[1].channel_0_enable = 1;
    pVioCfg->vi[1].channel_1_enable = 1;
    pVioCfg->vi[1].mipi_id = 2;
    pVioCfg->vi[1].i2c_bus = 3;
    strcpy(pVioCfg->vi[1].power_gpio, "D1_0");
    strcpy(pVioCfg->vi[1].reset_gpio, "D1_1");
    
    //vo split to 4 windows
    pVioCfg->vo.square = 2;

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 200000000;
}

static void IPC_CFG_VIO_SetDual307_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[1].channel_0_enable = 1;
    pVioCfg->vi[1].channel_1_enable = 1;
    //vo split to 4 windows
    pVioCfg->vo.square = 2;

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 200000000;
}

static void IPC_CFG_VIO_SetFour307_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[1].channel_0_enable = 1;
    pVioCfg->vi[1].channel_1_enable = 1;
    pVioCfg->vi[2].channel_0_enable = 1;
    pVioCfg->vi[2].channel_1_enable = 1;
    pVioCfg->vi[3].channel_0_enable = 1;
    pVioCfg->vi[3].channel_1_enable = 1;
    //vo split to 4 windows
    pVioCfg->vo.square = 2;

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 400000000;
}

static void IPC_CFG_VIO_SetSingle415_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx415");
    pVioCfg->vi[0].width = 3840;
    pVioCfg->vi[0].height = 2160;
    pVioCfg->vi[0].stride = 4096;
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

    pVioCfg->vi_cfg.isp_freq = 400000000; //for 30fps

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle464_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].channel_1_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle347()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx347");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].channel_1_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle464Hdr_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].HDR_enable = 1;
    pVioCfg->vi[0].vfe_mode = 0;

    pVioCfg->vi[0].channel_1_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.vif_freq = 500000000;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle307Hdr_LowBw()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].HDR_enable = 1;
    pVioCfg->vi_cfg.cam_mode = 1;
}

static void IPC_CFG_VIO_SetDvp8x1080P_25_LowBw()
{
    int i = 0;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        strcpy(pVioCfg->vi[i].sensor, "tp9930");
        pVioCfg->vi[i].i2c_bus = 3;
        pVioCfg->vi[i].channel_0_enable = 1;
        pVioCfg->vi[i].channel_1_enable = 1;
        if(i < 4)
        {
            //dvp 0
            pVioCfg->vi[i].mipi_id = 0;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_5");
        }
        else
        {
            pVioCfg->vi[i].mipi_id = 2;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_4");
        }
    }

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.vb_blk_cnt = 5;
    pVioCfg->vi_cfg.isp_freq = 600000000;
    pVioCfg->vo.square = 3;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetDvp8x1080P_30_LowBw()
{
    int i = 0;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        strcpy(pVioCfg->vi[i].sensor, "tp9930");
        pVioCfg->vi[i].i2c_bus = 3;
        pVioCfg->vi[i].fps = 30;
        pVioCfg->vi[i].channel_0_enable = 1;
        pVioCfg->vi[i].channel_0_fps = 30;
        pVioCfg->vi[i].channel_1_enable = 1;
        pVioCfg->vi[i].channel_1_fps = 30;
        if(i < 4)
        {
            //dvp 0
            pVioCfg->vi[i].mipi_id = 0;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_5");
        }
        else
        {
            pVioCfg->vi[i].mipi_id = 2;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_4");
        }
    }

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.vb_blk_cnt = 5;
    pVioCfg->vi_cfg.isp_freq = 600000000;
    pVioCfg->vo.square = 3;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetDual307()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 1
    pVioCfg->vi[1].channel_0_enable = 1;
    //vo split to 4 windows
    pVioCfg->vo.square = 2;

    pVioCfg->vi_cfg.cam_mode = 2;
}

static void IPC_CFG_VIO_SetFour307()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[1].channel_0_enable = 1;
    pVioCfg->vi[2].channel_0_enable = 1;
    pVioCfg->vi[3].channel_0_enable = 1;
    //vo split to 4 windows
    pVioCfg->vo.square = 2;

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 500000000;
}

static void IPC_CFG_VIO_SetSingle415()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx415");
    pVioCfg->vi[0].width = 3840;
    pVioCfg->vi[0].height = 2160;
    pVioCfg->vi[0].stride = 4096;
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;

    pVioCfg->vi_cfg.isp_freq = 600000000;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle464()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle464Hdr()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].HDR_enable = 1;

    pVioCfg->vi_cfg.cam_mode = 1;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle307Hdr()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].HDR_enable = 1;
    pVioCfg->vi_cfg.cam_mode = 1;
}

static void IPC_CFG_VIO_SetDvp8x1080P_25()
{
    int i = 0;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        strcpy(pVioCfg->vi[i].sensor, "tp9930");
        pVioCfg->vi[i].i2c_bus = 3;
        pVioCfg->vi[i].channel_0_enable = 1;
        if(i < 4)
        {
            //dvp 0
            pVioCfg->vi[i].mipi_id = 0;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_5");
        }
        else
        {
            pVioCfg->vi[i].mipi_id = 2;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_4");
        }
    }

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 600000000;
    pVioCfg->vo.square = 3;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetDvp8x1080P_30()
{
    int i = 0;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        strcpy(pVioCfg->vi[i].sensor, "tp9930");
        pVioCfg->vi[i].i2c_bus = 3;
        pVioCfg->vi[i].fps = 30;
        pVioCfg->vi[i].channel_0_enable = 1;
        pVioCfg->vi[i].channel_0_fps = 30;
        if(i < 4)
        {
            //dvp 0
            pVioCfg->vi[i].mipi_id = 0;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_5");
        }
        else
        {
            pVioCfg->vi[i].mipi_id = 2;
            strcpy(pVioCfg->vi[i].power_gpio,"NULL");
            strcpy(pVioCfg->vi[i].reset_gpio,"B3_4");
        }
    }

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 600000000;
    pVioCfg->vo.square = 3;

    //set vo to 1080P
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle307_ArIpc()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
	pVioCfg->vi[0].i2c_bus = 4;
	pVioCfg->vi_cfg.isp_freq = 75000000;
	strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
	strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");

    pVioCfg->vi_cfg.vb_blk_cnt = 4;
	pVioCfg->vo.enable = 0;
}

static void IPC_CFG_VIO_SetSingle415_ArIpc()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

	//sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx415");
	pVioCfg->vi[0].i2c_bus = 4;
    pVioCfg->vi[0].width = 3840;
    pVioCfg->vi[0].height = 2160;
    pVioCfg->vi[0].stride = 4096;
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

	//set ar_ipc gpio
	strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
	strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");

    //bug on 20220622, gdc share isp frequency, set to 500M for gdc 
    pVioCfg->vi_cfg.isp_freq = 500000000; //for 30fps
    pVioCfg->vi_cfg.vb_blk_cnt = 4;

    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle464_ArIpc()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

	//sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
	pVioCfg->vi[0].i2c_bus = 4;
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].channel_1_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

	//set ar_ipc gpio
	strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
	strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");

	pVioCfg->vi_cfg.vb_blk_cnt = 4;
    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;

}

static void IPC_CFG_VIO_SetSingle307_ArIpcHdr()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

	pVioCfg->vi[0].i2c_bus = 4;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].HDR_enable = 1;
    pVioCfg->vi_cfg.cam_mode = 1;
	pVioCfg->vi_cfg.isp_freq = 75000000;
	//set ar_ipc gpio
	strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
	strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");

    pVioCfg->vi_cfg.vb_blk_cnt = 4;
	pVioCfg->vo.enable = 0;
}

static void IPC_CFG_VIO_SetSingle464_ArIpcHdr()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "imx464");
	pVioCfg->vi[0].i2c_bus = 4;
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1520;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].HDR_enable = 1;
    //use ceva hdr temporarily
    pVioCfg->vi[0].vfe_mode = 1;

    pVioCfg->vi[0].channel_1_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;

	//set ar_ipc gpio
	strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
	strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");

    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.vb_blk_cnt = 4;

    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
}

static void IPC_CFG_VIO_SetSingle530ai_30()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "sc530ai");
    pVioCfg->vi[0].width = 2880;
    pVioCfg->vi[0].height = 1616;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].fps = 30;
    pVioCfg->vi[0].channel_0_fps = 30;

	pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
	pVioCfg->vi[0].channel_1_fps = 30;

	pVioCfg->vi[0].channel_2_raw_enable = 1;

    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;

	pVioCfg->vi_cfg.cam_mode = 0;
	pVioCfg->vi_cfg.isp_freq = 300000000;
	pVioCfg->vi_cfg.vif_freq = 200000000;
	//pVioCfg->vi_cfg.mipi_freq = 200000000;
}

static void IPC_CFG_VIO_SetSingleOV04A10()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
    
    pVioCfg->vi_cfg.cam_mode = 0;
    pVioCfg->vi_cfg.isp_freq = 300000000;
}

static void IPC_CFG_VIO_SetSingleOV04A10Hdr()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].HDR_enable = 1;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 60;
    
    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000; 
    pVioCfg->vi_cfg.hdr_freq = 300000000;
}

static void IPC_CFG_VIO_SetSingleOV04A10_ArIpc()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vi_cfg.vb_blk_cnt = 4;
    pVioCfg->vi[0].i2c_bus = 4;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
    strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");
    
    pVioCfg->vi_cfg.cam_mode = 0;
    pVioCfg->vi_cfg.isp_freq = 300000000;
}

static void IPC_CFG_VIO_SetSingleOV04A10Hdr_ArIpc()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].HDR_enable = 1;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vi_cfg.vb_blk_cnt = 4;
    pVioCfg->vi[0].i2c_bus = 4;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "B3_4");
    strcpy(pVioCfg->vi[0].reset_gpio, "D3_2");
    
    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000; 
    pVioCfg->vi_cfg.hdr_freq = 300000000;

}

static void IPC_CFG_VIO_SetSingle307_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].raw_compress_enable = 1;
    pVioCfg->vo.fps = 30;
    
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    pVioCfg->vi_cfg.isp_freq = 150000000; 
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 100000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000;
}

static void IPC_CFG_VIO_SetSingle307Hdr_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].HDR_enable = 1;
    pVioCfg->vi[0].raw_compress_enable = 1;    
    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vo.fps = 30;
    
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");

    pVioCfg->vi_cfg.isp_freq = 100000000; 
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 100000000;
    pVioCfg->vi_cfg.mipi_freq = 166000000;
}

static void IPC_CFG_VIO_SetSingleOV04A10_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
    
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    
    pVioCfg->vi_cfg.cam_mode = 0;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 100000000;
    pVioCfg->vi_cfg.mipi_freq = 166000000;

    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
}

static void IPC_CFG_VIO_SetSingleOV04A10Hdr_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].HDR_enable = 1;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    
    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.hdr_freq = 300000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000; //pcs

    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
}

static void IPC_CFG_VIO_SetTimo256_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "timo256");

    //set vo w/h
    pVioCfg->vo.fps = 30;
}

static void IPC_CFG_VIO_SetGst417w_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ar_gst417w");
    pVioCfg->vi[0].width = 408;
    pVioCfg->vi[0].height = 300;
    pVioCfg->vi[0].stride = 512;
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].i2c_bus = 2;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 408;
    pVioCfg->vo.height = 300;
    pVioCfg->vo.stride = 512;
    pVioCfg->vo.fps = 30;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 75000000; 
    pVioCfg->vi_cfg.hdr_freq = 75000000;
    pVioCfg->vi_cfg.vif_freq = 75000000;
    //set ar_ipc gpio
    //strcpy(pVioCfg->vi[0].power_gpio, "A_19");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_0");
}

static void IPC_CFG_VIO_SetGst212w4_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ar_gst212w4");
    pVioCfg->vi[0].width = 256;
    pVioCfg->vi[0].height = 192;
    pVioCfg->vi[0].stride = 256;
    
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].i2c_bus = 2;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 256;
    pVioCfg->vo.height = 192;
    pVioCfg->vo.stride = 256;
    pVioCfg->vo.fps = 30;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 75000000; 
    pVioCfg->vi_cfg.hdr_freq = 75000000;
    pVioCfg->vi_cfg.vif_freq = 75000000;
    //set ar_ipc gpio
    //strcpy(pVioCfg->vi[0].power_gpio, "A_19");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_0");
}

static void IPC_CFG_VIO_SetFussion_307_timo256()
{
}

static void IPC_CFG_VIO_SetDual307_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].raw_compress_enable = 1;
    pVioCfg->vi[1].channel_0_enable = 1;
    pVioCfg->vi[1].channel_1_enable = 1;
    pVioCfg->vi[1].i2c_bus = 2; 
    pVioCfg->vi[1].raw_compress_enable = 1;

    //vo split to 4 windows
    pVioCfg->vo.fps = 30;
    pVioCfg->vo.square = 2;
    pVioCfg->vo.width = 1280;
    pVioCfg->vo.height = 720;
    pVioCfg->vo.stride = 1280;

    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 150000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000;
        
    //set sensor gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    //set sensor gpio
    strcpy(pVioCfg->vi[1].power_gpio, "G_10");
    strcpy(pVioCfg->vi[1].reset_gpio, "G_14");
}

static void IPC_CFG_VIO_SetFusion_OV04A10_Gst417w()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    //sensor 1
    strcpy(pVioCfg->vi[1].sensor, "ar_gst417w");
    pVioCfg->vi[1].width = 408;
    pVioCfg->vi[1].height = 300;
    pVioCfg->vi[1].stride = 512;
    pVioCfg->vi[1].fps = 25;
    pVioCfg->vi[1].i2c_bus = 2;
    
    pVioCfg->vi[1].channel_0_fps = 25;
    pVioCfg->vi[1].channel_2_raw_enable = 0;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[1].power_gpio, "");
    strcpy(pVioCfg->vi[1].reset_gpio, "G_0");
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    pVioCfg->vo.square = 2;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 166000000;
    
}

static void IPC_CFG_VIO_SetFusion_OV04A10_Gst212w()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov04a10");
    pVioCfg->vi[0].width = 2560;
    pVioCfg->vi[0].height = 1440;
    pVioCfg->vi[0].stride = 2560;
    pVioCfg->vi[0].fps = 25;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 25;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
    
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    //sensor 1
    strcpy(pVioCfg->vi[1].sensor, "ar_gst212w4");
    pVioCfg->vi[1].width = 256;
    pVioCfg->vi[1].height = 192;
    pVioCfg->vi[1].stride = 256;
    pVioCfg->vi[1].fps = 25;
    pVioCfg->vi[1].i2c_bus = 2;
    
    pVioCfg->vi[1].channel_0_fps = 25;
    pVioCfg->vi[1].channel_2_raw_enable = 0;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[1].power_gpio, "");
    strcpy(pVioCfg->vi[1].reset_gpio, "G_0");
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    pVioCfg->vo.square = 2;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 200000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 166000000;
    
}

static void IPC_CFG_VIO_SetSingleOV05A20_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov05a20");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1944;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].fps = 30;
    
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 30;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
	
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;

	//set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    pVioCfg->vi_cfg.cam_mode = 1;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000;
}

static void IPC_CFG_VIO_SetFusion_OV05A20_Gst212w()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();

    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "ov05a20");
    pVioCfg->vi[0].width = 2688;
    pVioCfg->vi[0].height = 1944;
    pVioCfg->vi[0].stride = 3072;
    pVioCfg->vi[0].fps = 30;
    
    pVioCfg->vi[0].channel_0_fps = 30;
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_1_width = 1920;
    pVioCfg->vi[0].channel_1_height = 1080;
    pVioCfg->vi[0].channel_1_stride = 2048;
    pVioCfg->vi[0].channel_1_fps = 30;
    
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
    
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    //sensor 1
    strcpy(pVioCfg->vi[1].sensor, "ar_gst212w4");
    pVioCfg->vi[1].width = 256;
    pVioCfg->vi[1].height = 192;
    pVioCfg->vi[1].stride = 256;
    pVioCfg->vi[1].fps = 25;
    pVioCfg->vi[1].i2c_bus = 2;
    
    pVioCfg->vi[1].channel_0_fps = 25;
    pVioCfg->vi[1].channel_2_raw_enable = 0;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[1].power_gpio, "");
    strcpy(pVioCfg->vi[1].reset_gpio, "G_0");
    
    //set vo to 1080P
    pVioCfg->vo.enable = 0;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    pVioCfg->vo.square = 2;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 300000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000;
    
}

static void IPC_CFG_VIO_SetH3812c1sg_ARS31()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    strcpy(pVioCfg->vi[0].sensor, "h3812c1sg");
    pVioCfg->vi[0].width = 384;
    pVioCfg->vi[0].height = 288;
    pVioCfg->vi[0].stride = 512;
    pVioCfg->vi[0].fps = 25;
    pVioCfg->vi[0].i2c_bus = 2;
    
    pVioCfg->vi[0].channel_0_fps = 25;
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 384;
    pVioCfg->vo.height = 288;
    pVioCfg->vo.stride = 512;
    pVioCfg->vo.fps = 30;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 75000000; 
    pVioCfg->vi_cfg.hdr_freq = 75000000;
    pVioCfg->vi_cfg.vif_freq = 75000000;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "A_17");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_0");
}


static void IPC_CFG_VIO_SetFusion_307_H3812c1sg()
{
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    //sensor 0
    pVioCfg->vi[0].channel_1_enable = 1;
    pVioCfg->vi[0].channel_2_raw_enable = 0;
    pVioCfg->vi[0].raw_compress_enable = 1;
    
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[0].power_gpio, "G_3");
    strcpy(pVioCfg->vi[0].reset_gpio, "G_15");
    
    //sensor 1
    strcpy(pVioCfg->vi[1].sensor, "h3812c1sg");
    pVioCfg->vi[1].width = 384;
    pVioCfg->vi[1].height = 288;
    pVioCfg->vi[1].stride = 512;
    pVioCfg->vi[1].fps = 25;
    pVioCfg->vi[1].i2c_bus = 2;
    
    pVioCfg->vi[1].channel_0_fps = 25;
    pVioCfg->vi[1].channel_2_raw_enable = 0;
    //set ar_ipc gpio
    strcpy(pVioCfg->vi[1].power_gpio, "A_17");
    strcpy(pVioCfg->vi[1].reset_gpio, "G_0");
    
    //set vo to 1080P
    pVioCfg->vo.enable = 1;
    pVioCfg->vo.width = 1920;
    pVioCfg->vo.height = 1080;
    pVioCfg->vo.stride = 1920;
    pVioCfg->vo.fps = 30;
    pVioCfg->vo.square = 2;
    
    pVioCfg->vi_cfg.cam_mode = 2;
    pVioCfg->vi_cfg.isp_freq = 200000000;
    pVioCfg->vi_cfg.hdr_freq = 100000000;
    pVioCfg->vi_cfg.vif_freq = 300000000;
    pVioCfg->vi_cfg.mipi_freq = 100000000;
}

AR_S32 IPC_CFG_VIO_LoadMaxResource()
{
    AR_S32 i = 0;
    IPC_CFG_VIO_S * pVioCfg = IPC_CFG_VIO_GetParam();
    
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        g_stViMaxResorce.stViMaxResource[i].channel_0_width = pVioCfg->vi[i].width;
        g_stViMaxResorce.stViMaxResource[i].channel_0_height = pVioCfg->vi[i].height;
        g_stViMaxResorce.stViMaxResource[i].channel_1_width = pVioCfg->vi[i].channel_1_width;
        g_stViMaxResorce.stViMaxResource[i].channel_1_height = pVioCfg->vi[i].channel_1_height;
    }
    
    return 0;
}

AR_S32 IPC_CFG_VIO_LoadDefault(CFG_RUN_MODE_E enMode)
{
    int i = 0;
    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        CfgLoadDefValue(ViMap[i]);
    }
    CfgLoadDefValue(ViCfgMap);
    CfgLoadDefValue(VoMap);

    switch(enMode)
    {
#if 0
        case CFG_RUN_MODE_DUAL_307:
        	IPC_CFG_VIO_SetDual307();
            break;
        case CFG_RUN_MODE_SINGLE_464:
        	IPC_CFG_VIO_SetSingle464();
            break;
        case CFG_RUN_MODE_SINGLE_415:
        	IPC_CFG_VIO_SetSingle415();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_25:
        	IPC_CFG_VIO_SetDvp8x1080P_25();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_30:
        	IPC_CFG_VIO_SetDvp8x1080P_30();
            break;
        case CFG_RUN_MODE_SINGLE_307_HDR:
        	IPC_CFG_VIO_SetSingle307Hdr();
            break;
        case CFG_RUN_MODE_SINGLE_464_HDR:
        	IPC_CFG_VIO_SetSingle464Hdr();
            break;
        case CFG_RUN_MODE_FOUR_307:
        	IPC_CFG_VIO_SetFour307();
            break;
#endif 
        case CFG_RUN_MODE_SINGLE_SC230AI_LOW_BW:
            IPC_CFG_VIO_SetSingle230_LowBw();
            break;
        case CFG_RUN_MODE_DUAL_SC230AI_LOW_BW:
            IPC_CFG_VIO_SetDual230_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_307_LOW_BW:
            IPC_CFG_VIO_SetSingle307_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_307_HDR_LOW_BW:
            IPC_CFG_VIO_SetSingle307Hdr_LowBw();
            break;
        case CFG_RUN_MODE_DUAL_307_LOW_BW:
            IPC_CFG_VIO_SetDual307_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_415_LOW_BW:
            IPC_CFG_VIO_SetSingle415_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_464_LOW_BW:
            IPC_CFG_VIO_SetSingle464_LowBw();
            break;
        case CFG_RUN_MODE_SINGLE_464_HDR_LOW_BW:
            IPC_CFG_VIO_SetSingle464Hdr_LowBw();
            break;
        case CFG_RUN_MODE_FOUR_307_LOW_BW:
            IPC_CFG_VIO_SetFour307_LowBw();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_25_LOW_BW:
            IPC_CFG_VIO_SetDvp8x1080P_25_LowBw();
            break;
        case CFG_RUN_MODE_DVP_8X1080P_30_LOW_BW:
            IPC_CFG_VIO_SetDvp8x1080P_30_LowBw();
            break;
		case CFG_RUN_MODE_SINGLE_307_ARIPC:
			IPC_CFG_VIO_SetSingle307_ArIpc();
			break;
		case CFG_RUN_MODE_SINGLE_415_ARIPC:
			IPC_CFG_VIO_SetSingle415_ArIpc();
			break;
		case CFG_RUN_MODE_SINGLE_464_ARIPC:
			IPC_CFG_VIO_SetSingle464_ArIpc();
			break;
		case CFG_RUN_MODE_SINGLE_307_HDR_ARIPC:
			IPC_CFG_VIO_SetSingle307_ArIpcHdr();
			break;
		case CFG_RUN_MODE_SINGLE_464_HDR_ARIPC:
			IPC_CFG_VIO_SetSingle464_ArIpcHdr();
			break;
        case CFG_RUN_MODE_SINGLE_530AI_30:
            IPC_CFG_VIO_SetSingle530ai_30();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25:
            IPC_CFG_VIO_SetSingleOV04A10();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR:
            IPC_CFG_VIO_SetSingleOV04A10Hdr();
            break;
        case CFG_RUN_MODE_SINGLE_347:
            IPC_CFG_VIO_SetSingle347();
        case CFG_RUN_MODE_SINGLE_307_ARS31:
            IPC_CFG_VIO_SetSingle307_ARS31();
            break;
        case CFG_RUN_MODE_SINGLE_307_HDR_ARS31:
            IPC_CFG_VIO_SetSingle307Hdr_ARS31();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_ARS31:
            IPC_CFG_VIO_SetSingleOV04A10_ARS31();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_HDR_ARS31:
            IPC_CFG_VIO_SetSingleOV04A10Hdr_ARS31();
            break;
        case CFG_RUN_MODE_SINGLE_TIMO256_ARS31:
            IPC_CFG_VIO_SetTimo256_ARS31();
            break;
        case CFG_RUN_MODE_SINGLE_GST417W_ARS31:
            IPC_CFG_VIO_SetGst417w_ARS31();
            break;
        case CFG_RUN_MODE_307_timo256_ARS31:
            IPC_CFG_VIO_SetFussion_307_timo256();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25_ARIPC:
            IPC_CFG_VIO_SetSingleOV04A10_ArIpc();
            break;
        case CFG_RUN_MODE_SINGLE_OV04A10_25_HDR_ARIPC:
            IPC_CFG_VIO_SetSingleOV04A10Hdr_ArIpc();
            break;
        case CFG_RUN_MODE_DUAL_307_ARS31:
            IPC_CFG_VIO_SetDual307_ARS31();
            break;
        case CFG_RUN_MODE_OV04A10_GST417W_ARS31:
            IPC_CFG_VIO_SetFusion_OV04A10_Gst417w();
            break;
        case CFG_RUN_MODE_SINGLE_GST212W4_ARS31:
            IPC_CFG_VIO_SetGst212w4_ARS31();
            break;    
        case CFG_RUN_MODE_OV04A10_GST212W_ARS31:
            IPC_CFG_VIO_SetFusion_OV04A10_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_OV05A20_ARS31:
            IPC_CFG_VIO_SetSingleOV05A20_ARS31();
            break; 
        case CFG_RUN_MODE_OV05A20_GST212W_ARS31:
            IPC_CFG_VIO_SetFusion_OV05A20_Gst212w();
            break;
        case CFG_RUN_MODE_SINGLE_H3812C1SG_ARS31:
            IPC_CFG_VIO_SetH3812c1sg_ARS31();
            break;
        case CFG_RUN_MODE_307_H3812C1SG_ARS31:
            IPC_CFG_VIO_SetFusion_307_H3812c1sg();
            break;
        default: //single 307 normal
            break;
    }

    return 0;
}

AR_S32 IPC_CFG_VIO_Load(CFG_RUN_MODE_E enMode)
{
    int i = 0;
    char *data = NULL;
    data = CfgReadFromFile(VIO_CFG_FILE);
    if (data == NULL)
    {
        PRINT_INFO("load %s error, so to load default cfg param.\n", VIO_CFG_FILE);
        goto err;
    }

    cJSON *json = NULL;
    json = cJSON_Parse(data);
    if (!json)
    {
        PRINT_ERR("Error before: [%s]\n", cJSON_GetErrorPtr());
        free(data);
        goto err;
    }

    for(i = 0; i < IPC_MAX_PIPELINE_NUM; i++)
    {
        char tmp[32] = {0};
        sprintf(tmp, "vi_%d", i);

        CfgParseCjson(json, tmp, ViMap[i]);
    }
    CfgParseCjson(json, "vi_cfg", ViCfgMap);
    CfgParseCjson(json, "vo", VoMap);

    cJSON_Delete(json);
    free(data);
	
    IPC_CFG_VIO_LoadMaxResource();
    return 0;

err:
    IPC_CFG_VIO_LoadDefault(enMode);
    IPC_CFG_VIO_Save();
    IPC_CFG_VIO_LoadMaxResource();
    return 0;
}

IPC_CFG_VIO_S * IPC_CFG_VIO_GetParam()
{
    return &g_stRunVioCfg;
}

IPC_VI_MAX_RESOURCE_S * IPC_CFG_VIO_GetMaxResource()
{
    return &g_stViMaxResorce;
}
