#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <strings.h>

#include "pf_type.h"

AR_S32 IPC_MID_UART_Open(const AR_CHAR *devicePath)
{
    AR_S32 SerialPortFd = open(devicePath, O_RDWR | O_NOCTTY | O_NDELAY);
    if (SerialPortFd < 0)
    {
        perror("open error.");
        return -1;
    }
    return SerialPortFd;
}

AR_S32 IPC_MID_UART_Config(AR_S32 SerialPortFd, AR_S32 nSpeed,AR_S32 nBits, AR_CHAR nEvent,AR_S32 nStop)
{
    AR_S32 rtn = 0;
    AR_S32 speed = 0;
    struct termios NewConfig,OldConfig;

    rtn = tcgetattr(SerialPortFd, &OldConfig);
    if(0 != rtn)
    {
        perror("Error on tcgetattr");
        return -1;
    }

    bzero(&NewConfig, sizeof(NewConfig));

    NewConfig.c_cflag |= CLOCAL | CREAD;

    NewConfig.c_cflag &= ~CSIZE;
    switch(nBits)
    {
        case 8: NewConfig.c_cflag |= CS8;break;
        case 7: NewConfig.c_cflag |= CS7;break;
        case 6: NewConfig.c_cflag |= CS6;break;
        case 5: NewConfig.c_cflag |= CS5;break;
        default:
            printf("Error on nBits\n");
            return -1;
    }

    switch(nEvent)
    {
        case 'o':
        case 'O':  //设置为奇校验
            NewConfig.c_cflag |=(PARODD|PARENB);
            NewConfig.c_iflag |= INPCK;
        break;
        case 'e':
        case 'E':  //设置为偶校验
            NewConfig.c_cflag |=PARENB;
            NewConfig.c_cflag &= ~PARODD;
            NewConfig.c_iflag |= INPCK;
        break;
        case 's':
        case 'S':   /*as no parity*/
            NewConfig.c_cflag &=~PARENB;
            NewConfig.c_cflag &= ~CSTOPB;
        break;
        case 'n':
        case 'N': //设置无奇偶校验位
            NewConfig.c_cflag &= ~PARENB;
            NewConfig.c_iflag &= ~INPCK;
        break;
        default:
            printf("Error on nEvent\n");
            return -1;
    }

    switch(nSpeed)
    {
        case 2400: speed = B2400; break;
        case 4800: speed = B4800; break;
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 115200: speed = B115200; break;
        default:
            printf("Error on nSpeed\n");
            return -1;
    }
    cfsetispeed(&NewConfig,speed);
    cfsetospeed(&NewConfig,speed);

    switch(nStop)
    {
        case 1: NewConfig.c_cflag &= ~CSTOPB;break;
        case 2: NewConfig.c_cflag |=  CSTOPB;break;
        default:
            printf("Error on nStop\n");
            return -1;
    }


    NewConfig.c_cc[VTIME] = 0;
    NewConfig.c_cc[VMIN] = 0;

    tcflush(SerialPortFd,TCIFLUSH);

    rtn = tcsetattr(SerialPortFd,TCSANOW,&NewConfig);
    if(0 != rtn)
    {
        perror("Error on tcsetattr");
        return -1;
    }

    return 0;
}

AR_S32 IPC_MID_UART_Receive(AR_S32 SerialPortFd, AR_UCHAR *buf,AR_S32 size)
{
    AR_S32 rtn ;

    rtn = read(SerialPortFd,buf,size);
//  printf("ReadLength = %d\n",rtn);
    return rtn;
}

AR_S32 IPC_MID_UART_Send(AR_S32 SerialPortFd, AR_UCHAR *buf,AR_S32 DataLength)
{
    AR_S32 rtn ;

#if 0
    AR_S32 remain = DataLength;
    AR_S32 offset = 0;
    AR_S32 sub = 0;
    while(remain > 0 )
    {
        sub = (remain >= 8? 8:remain);
        write(SerialPortFd,buf+offset,sub);
        tcflush(SerialPortFd,TCOFLUSH);
        remain -= 8;
        offset += 8;
    }
#else
    rtn = write(SerialPortFd, buf, DataLength);
#endif
    return rtn;
}

AR_S32 IPC_MID_UART_Close(AR_S32 SerialPortFd)
{
    close(SerialPortFd);
    return 0;
}

AR_S32 IPC_MID_UART_Init(AR_S32 dev_no, AR_S32 baudrate)
{
    AR_S32 ret = 0;
    AR_S32 fd = 0;
    char dev[20] = {0};
    sprintf(dev, "/dev/ttyS%d", dev_no);

    fd = IPC_MID_UART_Open(dev);
    if(fd < 0)
    {
        printf("IPC_MID_UART_Open error.\n");
        return -1;
    }

    //ret = IPC_MID_UART_Config(fd, 115200, 8, 'n', 1);
    ret = IPC_MID_UART_Config(fd, baudrate, 8, 'n', 1); // baudrate = 19200
    //ret = IPC_MID_UART_Config(fd, 9600, 8, 'n', 1);
    if(ret < 0)
    {
        printf("IPC_MID_UART_Config error.\n");
        return -1;
    }

    return fd;
}

