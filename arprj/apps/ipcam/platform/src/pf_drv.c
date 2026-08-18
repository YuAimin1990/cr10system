#include <linux/rtc.h>
#include <linux/watchdog.h>

#include "hal_gpio.h"

#include "pf_type.h"
#include "pf_drv.h"
#include "util_common.h"
#include "util_api.h"

void * IPC_PF_Mmap(unsigned int base_addr, unsigned int size)
{
	void *addr;
    static int mmap_fd = -1;

	/* not mmaped yet */
	if(mmap_fd < 0)
	{
		/* dev not opened yet, so open it */
		mmap_fd = open ("/dev/mem", O_RDWR | O_SYNC);
		if (mmap_fd < 0)
		{
			PRINT_ERR("open(): mem error");
			return NULL;
		}
        //PRINT_INFO("%d\n", mmap_fd);
	}

	addr = mmap ((void *)0, size, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, base_addr);
	if (addr == MAP_FAILED)
	{
		PRINT_ERR("mmap(): 0x%08X, %d", base_addr, size);
		return NULL;
	}
    //PRINT_INFO("%p", addr);
	return addr;
}

int IPC_PF_Munmap(void *addr_mapped, unsigned int size)
{
    int ret;

    ret = munmap(addr_mapped, size);
    if (ret == -1)
    {
        PRINT_ERR("munmap(): %p", addr_mapped);
    }

    return ret;
}

/********************** SoC Reg **********************/

unsigned int IPC_PF_ReadReg(unsigned int base, unsigned int offset)
{
    unsigned int reg_val = 0;
    #if 0
    void *vaddr;

    vaddr = IPC_PF_Mmap(base, 0x10000);

    if (vaddr)
    {
        reg_val = *(volatile unsigned int *)(vaddr + offset);
    }

    IPC_PF_Munmap(vaddr, 0x10000);
    #else
    char cmd[128] = {0};
    unsigned int paddr = base + offset;
    snprintf(cmd, sizeof(cmd), "devmem 0x%x", paddr);
    //printf("cmd:%s\n", cmd);
    char buf[20] = {0};
    IPC_MID_UTIL_SysCall(cmd, buf, sizeof(buf));
    reg_val = strtoul(buf, NULL, 0);
    #endif

    return reg_val;
}

int IPC_PF_WriteReg(unsigned int base, unsigned int offset, unsigned int value)
{
    #if 0
    void *vaddr;

    vaddr = IPC_PF_Mmap(base, 0x10000);

    if (vaddr)
    {
        *(volatile unsigned int *)(vaddr + offset) = value;
    }

    IPC_PF_Munmap(vaddr, 0x10000);
    #else
    //char cmd[128] = {0};
    //unsigned int paddr = base + offset;
    //snprintf(cmd, sizeof(cmd), "armm 0x%x 0x%x", paddr, value);
    //printf("cmd:%s\n", cmd);
    //IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    #endif

    return 0;
}

void IPC_PF_SysShutdown()
{
    #if 0
    unsigned int reg_val;
    unsigned int addr = 0x120F0000;
    reg_val = IPC_PF_ReadReg(addr, 0);
    PRINT_INFO("1 0x120F0000 : %u\n", reg_val);

    reg_val |= (1 << 1);
    IPC_PF_WriteReg(addr, 0, reg_val);
    #else
    //char cmd[128] = {0};
    //snprintf(cmd, sizeof(cmd), "armm 0x120F0000 0x100162");
    PRINT_INFO("power shutdown!!!\n");
    //printf("cmd:%s\n", cmd);
    //IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    #endif
}

static int adc_fd = -1;
#define PF_ADC_DEVICE "/dev/ar_adc"
#define PF_ADC_DRIVER "/mod/ar_adc.ko"
//static int adc_ch = 1;

static int IPC_PF_OpenDev(char *dev)
{
    if (access(dev, F_OK) != 0) {
        PRINT_ERR("%s not exist\n", dev);
        return -1;
    }

    int fd = -1;

    int retry = 5;
    do {
        fd = open(dev, O_RDWR);;
        if (fd >= 0) {
            break;
        }

        usleep(5000);
    } while (retry--);

    if (retry == 0) {
        PRINT_ERR("open %s failed, fd = %d !\n", dev, fd);
        return -1;
    } else {
        PRINT_INFO("open %s success, fd = %d\n", dev, fd);
    }

    return fd;
}

int IPC_PF_ADC_Init(void)
{
    //int ret;
    adc_fd = IPC_PF_OpenDev(PF_ADC_DEVICE);
    if (adc_fd < 0) {
        PRINT_ERR("IPC_PF_OpenDev %s failed\n", PF_ADC_DEVICE);
        return -1;
    }

    //Init some registers


    return 0;
}

int IPC_PF_ADC_UnInit(void)
{
    if (adc_fd < 0)
        return -1;

    PRINT_INFO("adc, close: %d", adc_fd);
    close(adc_fd);
    adc_fd = -1;

    return 0;
}

int IPC_PF_ADC_Read(int *value)
{
    if (adc_fd < 0 ) {
        return -1;
    }

    *value = 1;

    return 0;
}


static int rtc_fd = -1;
#define PF_RTC_DEVICE "/dev/rtc0"

int IPC_PF_RTC_Init(void)
{
#if 0
    
    rtc_fd = IPC_PF_OpenDev(PF_RTC_DEVICE);
    if (rtc_fd < 0) {
        PRINT_ERR("IPC_PF_OpenDev %s failed\n", PF_RTC_DEVICE);
        return -1;
    }
#endif

    return 0;
}

int IPC_PF_RTC_UnInit(void)
{
#if 0
    if (rtc_fd < 0)
        return -1;

    PRINT_INFO("rtc, close: %d", rtc_fd);

    close(rtc_fd);
    rtc_fd = -1;
#endif
    return 0;
}

int IPC_PF_RTC_Read(PF_RTC_TIME_S *tm)
{
    if (NULL == tm)
    {
        PRINT_ERR("IPC_PF_RTC_Read Para is error\n");
        return -1;
    }
	
    rtc_fd = IPC_PF_OpenDev(PF_RTC_DEVICE);
    if (rtc_fd < 0) {
        PRINT_ERR("IPC_PF_OpenDev %s failed\n", PF_RTC_DEVICE);
        return -1;
    }

    struct rtc_time rtc_tm;
    int retval;
    retval = ioctl(rtc_fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1)
    {
        PRINT_ERR("RTC_RD_TIME ioctl %d\n", retval);
        return -1;
    }

    tm->year = rtc_tm.tm_year + 1900;
        tm->month = rtc_tm.tm_mon + 1;
        tm->date = rtc_tm.tm_mday;
        tm->hour = rtc_tm.tm_hour;
        tm->minute = rtc_tm.tm_min;
        tm->second = rtc_tm.tm_sec;
        tm->weekday = rtc_tm.tm_wday;

    PRINT_INFO("RTC_RD_TIME, %04d-%02d-%02d %02d:%02d:%02d (%d)\n",
          tm->year,tm->month,tm->date,
          tm->hour,tm->minute,tm->second,
          tm->weekday);

    close(rtc_fd);
    rtc_fd = -1;

    return 0;
}

int IPC_PF_RTC_Write(PF_RTC_TIME_S *tm)
{
    if (NULL == tm){
        PRINT_ERR("IPC_PF_RTC_Write Para is error\n");
        return -1;
    }
    
    rtc_fd = IPC_PF_OpenDev(PF_RTC_DEVICE);
    if (rtc_fd < 0) {
        PRINT_ERR("IPC_PF_OpenDev %s failed\n", PF_RTC_DEVICE);
        return -1;
    }

    PRINT_INFO("%d-%d-%d %d:%d:%d (%d)\n",
        tm->year ,tm->month ,tm->date ,tm->hour ,tm->minute ,tm->second ,tm->weekday);
    struct rtc_time rtc_tm;

    int retval;

    rtc_tm.tm_year = tm->year - 1900;
        rtc_tm.tm_mon = tm->month - 1;
        rtc_tm.tm_mday = tm->date;
        rtc_tm.tm_hour = tm->hour;
        rtc_tm.tm_min = tm->minute;
        rtc_tm.tm_sec = tm->second;

    retval = ioctl(rtc_fd,  RTC_SET_TIME, &rtc_tm);
    if (retval == -1)
    {
        PRINT_ERR("RTC_RD_TIME ioctl %d\n", retval);
	return -1;
    }

    close(rtc_fd);
    rtc_fd = -1;
    return 0;
}

int IPC_PF_RTC_UpdateLocalTime()
{
    PF_RTC_TIME_S st;
    memset(&st, 0, sizeof(st));
    IPC_PF_RTC_Read(&st);

    IPC_MID_UTIL_ShowTime();

    #if 0
    struct tm t;
    memset(&t, 0, sizeof(t));

    t.tm_year = st.year - 1900;
    t.tm_mon = st.month - 1;
    t.tm_mday = st.date;
    t.tm_hour = st.hour;
    t.tm_min = st.minute;
    t.tm_sec = st.second;

    time_t timep = mktime(&t);

    struct timeval set_tv;
    set_tv.tv_sec = timep;
    set_tv.tv_usec = 0;
//4
    if (settimeofday(&set_tv, (struct timezone *)0) < 0) {
        PRINT_ERR("set time error !\n");
        return -1;
    }

    IPC_MID_UTIL_ShowTime();

    #endif

    return 0;
}

int IPC_PF_RTC_UpdateRTCFromLocal()
{
    PF_RTC_TIME_S st;
    memset(&st, 0, sizeof(st));

    time_t timep;
    struct tm *p;
    time(&timep);
    struct tm tt = {0};
    //p = localtime_r(&timep, &tt);
    p = gmtime_r(&timep, &tt);

    st.year = p->tm_year + 1900;
    st.month = p->tm_mon + 1;
    st.date = p->tm_mday;
    st.hour = p->tm_hour;
    st.minute = p->tm_min;
    st.second = p->tm_sec;
    st.weekday = p->tm_wday;

    IPC_PF_RTC_Write(&st);

    return 0;
}

static int  wdt_fd = -1;
#define PF_WDT_DEVICE "/dev/watchdog"
#define PF_WDT_DRIVER "/ko/hi3516ev200_wdt.ko"


int IPC_PF_WDT_Enable(void)
{
    if (wdt_fd < 0)
        return -1;

    int new_status = WDIOS_ENABLECARD;
    if (ioctl(wdt_fd, WDIOC_SETOPTIONS, &new_status) != 0)
    {
        PRINT_ERR("ioctl: WDIOC_SETOPTIONS");
        return -2;
    }

    return 0;
}

int IPC_PF_WDT_Disable(void)
{
    if (wdt_fd < 0)
        return -1;

    int new_status = WDIOS_DISABLECARD;
    if (ioctl(wdt_fd, WDIOC_SETOPTIONS, &new_status) != 0)
    {
        PRINT_ERR("ioctl: WDIOC_SETOPTIONS");
        return -2;
    }

    return 0;
}

int IPC_PF_WDT_SetLiveTime(int livetime_sec)
{
    if (wdt_fd < 0)
        return -1;

    if (ioctl(wdt_fd, WDIOC_SETTIMEOUT, &livetime_sec) != 0)
    {
        PRINT_ERR("ioctl: WDIOC_SETTIMEOUT");
        return -2;
    }

    return 0;
}

int IPC_PF_WDT_GetLiveTime(int *livetime_sec)
{
    if (wdt_fd < 0)
        return -1;

    if (ioctl(wdt_fd, WDIOC_GETTIMEOUT, livetime_sec) != 0)
    {
        PRINT_ERR("ioctl: WDIOC_GETTIMEOUT");
        return -2;
    }

    return 0;
}

int IPC_PF_WDT_KeepAlive(void)
{
    if (wdt_fd < 0)
        return -1;

    if (ioctl(wdt_fd, WDIOC_KEEPALIVE, 0) != 0)
    {
        PRINT_ERR("ioctl: WDIOC_KEEPALIVE");
        return -2;
    }

    return 0;
}

int IPC_PF_WDT_Init(void)
{
    AR_S32 s32Ret;

    wdt_fd = IPC_PF_OpenDev(PF_WDT_DEVICE);
    if (wdt_fd < 0) {
        PRINT_ERR("IPC_PF_OpenDev %s failed\n", PF_WDT_DEVICE);
        return -1;
    }

    s32Ret = IPC_PF_WDT_Enable();
    if (s32Ret != 0) {
        PRINT_ERR("IPC_PF_WDT_Enable failed\n");
    }

    AR_S32 live = 0;
    s32Ret = IPC_PF_WDT_GetLiveTime(&live);
    if (s32Ret != 0) {
        PRINT_ERR("IPC_PF_WDT_GetLiveTime failed\n");
    }
    printf("1 wdt live:%d\n", live);

    s32Ret = IPC_PF_WDT_SetLiveTime(10); //10s
    if (s32Ret != 0) {
        PRINT_ERR("IPC_PF_WDT_SetLiveTime failed\n");
    }

    live = 0;
    s32Ret = IPC_PF_WDT_GetLiveTime(&live);
    if (s32Ret != 0) {
        PRINT_ERR("IPC_PF_WDT_GetLiveTime failed\n");
    }
    printf("2 wdt live:%d\n", live);
    return 0;
}

int IPC_PF_WDT_UnInit(void)
{
    if (wdt_fd < 0)
        return -1;

    int ret;
    IPC_PF_WDT_Disable();

    PRINT_INFO("wdt, close: %d", adc_fd);
    ret = close(wdt_fd);
    if (ret != 0) {
        PRINT_ERR("close wdt_fd(%d) failed\n", wdt_fd);
        //return -1;
    }

    wdt_fd = -1;

    return 0;
}

//static pthread_mutex_t g_GpioMutex = PTHREAD_MUTEX_INITIALIZER;

int IPC_PF_GPIO_SetValue(int pin, int value)
{
    char dir_tmp[128] = {0};
    char cmd[128] = {0};
    int ret = 0;

    sprintf(dir_tmp, "/sys/class/gpio/gpio%d", pin);
    if(access(dir_tmp, F_OK) != 0 ) {
        snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export", pin);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    }

    snprintf(cmd, sizeof(cmd), "cat /sys/class/gpio/gpio%d/direction", pin);
    char buff[30] = {0};
    ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
	if ((ret != 0) || (strlen(buff) == 0)) {
        PRINT_ERR("%s failed\n", cmd);
		return -1;
	}
    if (strncmp(buff, "in", 2) == 0) {
        snprintf(cmd, sizeof(cmd), "echo out > /sys/class/gpio/gpio%d/direction", pin);
        //printf("1 cmd:%s, buff:%s", cmd, buff);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    } else {
        //printf("1 buff:%s", buff);
    }

    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value", value, pin);
    //printf("1 cmd:%s\n", cmd);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    return 0;
}


int IPC_PF_GPIO_GetValue(int pin, int *value)
{
    char dir_tmp[128] = {0};
    char cmd[128] = {0};
    char buff[30] = {0};
    int ret = 0;

    sprintf(dir_tmp, "/sys/class/gpio/gpio%d", pin);
    if(access(dir_tmp, F_OK) != 0 ) {
        snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export", pin);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    }

    snprintf(cmd, sizeof(cmd), "cat /sys/class/gpio/gpio%d/direction", pin);
    ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
	if ((ret != 0) || (strlen(buff) == 0)) {
        PRINT_ERR("%s failed\n", cmd);
		return -1;
	}
    if (strncmp(buff, "out", 3) == 0) {
        snprintf(cmd, sizeof(cmd), "echo in > /sys/class/gpio/gpio%d/direction", pin);
        //printf("2 cmd:%s, buff:%s", cmd, buff);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    } else {
        //printf("2 buff:%s", buff);
    }

    snprintf(cmd, sizeof(cmd), "cat /sys/class/gpio/gpio%d/value", pin);
    ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
	if ((ret != 0) || (strlen(buff) == 0))
    {
        PRINT_ERR("%s failed\n", cmd);
		return -1;
	}
    *value = atoi(buff);

    return 0;
}

//for ar9301 platform, gpio name A1_0 to gpio num;
//for ar9311 platform, gpio name C_20 to gpio num;
AR_S32 AR_GPIO_Trans(AR_CHAR * strGpio, ENUM_GPIO_GROUP * pGrp, ENUM_GPIO_PORT * pPort, AR_U32 * pNum)
{
    AR_CHAR chGrp = 0, chPort = 0, chNum = 0;
    AR_U32 u32Len = 0;
    AR_S32 i = 0;

    if(!strGpio || !pGrp || !pPort || !pNum)
    {
        PRINT_ERR("GPIO trans null pointer.\n");
        return -1;
    }

    //remove space
    while(*strGpio == 0x20) {strGpio++;}

    //PRINT_INFO("GPIO %s\n", strGpio);
    u32Len = strlen(strGpio);
    //left should be like "A1_0"
    if(u32Len < 3)
    {
        PRINT_ERR("Invalid gpio: %s\n", strGpio);
        return -1;
    }

    chPort = toupper(strGpio[0]);
    if(chPort < 'A' || chPort > 'G')
    {
        PRINT_ERR("Invalid gpio : %s, the port should be from A to G\n", strGpio);
        return -1;
    }
    *pPort = PORT_A + chPort - 'A';
    //PRINT_INFO("GPIO %s\n", strGpio);

    chGrp = strGpio[1];
    if('0' <= chGrp && chGrp < '4')
    {
        *pGrp = GROUP_0 + chGrp - '0';        
    }
    else if(chGrp == '_')
    {
        *pGrp = GROUP_0;
    }
    else
    {
        PRINT_ERR("Invalid gpio : %s, the grp should be from 0 to 3\n", strGpio);
        return -1;
    }
    //PRINT_INFO("GPIO %s\n", strGpio);

    //skip the '_', get the pin num.
    for(i = 0; i < u32Len; i++)
    {
        if(strGpio[i] == '_')
            break;
    }

    if(i == u32Len)
    {
        PRINT_ERR("Invalid gpio: %s, format should be like A1_2 or C_20\n", strGpio);
        return -1;
    }

    chNum = atoi(&strGpio[i+1]);
    if(chNum < 0 || chNum > 32)
    {
        PRINT_ERR("Invalid gpio : %s, the num should be from 0 to 32\n", strGpio);
        return -1;
    }
    *pNum = chNum;

    return 0;
}

AR_S32 IPC_PF_GPIO_SetByName(AR_CHAR * strName, ENUM_GPIO_DIR enDir, AR_U32 u32Value)
{
    ENUM_GPIO_GROUP enGrp = 0;
    ENUM_GPIO_PORT enPort = 0;
    AR_U32 u32Num = 0;
    AR_U32 u32Pin = 0;

    AR_GPIO_Trans(strName, &enGrp, &enPort, &u32Num);

    u32Pin = ar_hal_gpio_name_to_num(enGrp, enPort, u32Num);
    ar_hal_gpio_export(u32Pin);
    ar_hal_gpio_set_dir(u32Pin, enDir);
    ar_hal_gpio_set_value(u32Pin, u32Value);
    return 0;
}

AR_S32 IPC_PF_GPIO_GetByName(AR_CHAR * strName, ENUM_GPIO_DIR * penDir, AR_U32 * pu32Value)
{
    ENUM_GPIO_GROUP enGrp = 0;
    ENUM_GPIO_PORT enPort = 0;
    AR_U32 u32Num = 0;
    AR_U32 u32Pin = 0;

    AR_GPIO_Trans(strName, &enGrp, &enPort, &u32Num);

    u32Pin = ar_hal_gpio_name_to_num(enGrp, enPort, u32Num);
    ar_hal_gpio_export(u32Pin);

    ar_hal_gpio_get_dir(u32Pin, penDir);
    ar_hal_gpio_get_value(u32Pin, pu32Value);
    return 0;
}

