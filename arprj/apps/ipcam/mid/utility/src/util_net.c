#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "util_net.h"
#include "util_api.h"

int IPC_MID_UTIL_GetIP(const char *adapter_name, char *addr)
{
    struct ifreq ifr;
    const char *ifname = adapter_name;
    int skfd;
    struct sockaddr_in *ipad;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd < 0)
    {
        PRINT_ERR("socket create error\n");
        return -1;
    }
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0)
    {
        ipad = (struct sockaddr_in *)&ifr.ifr_addr;
        strcpy(addr,inet_ntoa(ipad->sin_addr));
    }
    else
    {
        close(skfd);
        PRINT_ERR("error!!:%s,device:%s\n",strerror(errno),adapter_name);
        return -1;
    }

    close(skfd);
    return 0;

}

int IPC_MID_UTIL_GetMac(const char *adapter_name, char *mac)
{
    struct ifreq ifr;
    const char *ifname = adapter_name;
    int skfd;
    int i;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd < 0)
    {
        PRINT_ERR("socket create error\n");
        return -1;
    }
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);

    if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(skfd);
        PRINT_ERR("get mac failed\n");
        PRINT_ERR("error!!:%s,device:%s\n",strerror(errno),adapter_name);
        return -1;
    }

    for(i = 0; i < 6; i++)
    {
        sprintf(mac+3*i,"%02x:",(unsigned char)ifr.ifr_hwaddr.sa_data[i]);
    }
    mac[17] = 0;
    //printf("get mac :%s \n",mac);

    close(skfd);

    return 0;

}

int IPC_MID_UTIL_GetGateway(const char *adapter_name, char *gateway)
{
    FILE *fp;
    char buf[1024];
    char iface[16];
    unsigned char tmp[100]={'\0'};
    unsigned int dest_addr=0, gate_addr=0;
    if(NULL == gateway)
    {
        PRINT_ERR("gateway is NULL \n");
        return -1;
    }
    fp = fopen(_PATH_PROCNET_ROUTE, "r");
    if(fp == NULL)
    {
        PRINT_ERR("fopen route error \n");
        return -1;
    }

    fgets(buf, sizeof(buf), fp);
    while(fgets(buf, sizeof(buf), fp))
    {
        if((sscanf(buf, "%s\t%X\t%X", iface, &dest_addr, &gate_addr) == 3)
            && (memcmp(adapter_name, iface, strlen(adapter_name)) == 0)
            && gate_addr != 0)
        {
                memcpy(tmp, (unsigned char *)&gate_addr, 4);
                sprintf(gateway, "%d.%d.%d.%d", (unsigned char)*tmp, (unsigned char)*(tmp+1), (unsigned char)*(tmp+2), (unsigned char)*(tmp+3));
                break;
        }
    }

    fclose(fp);
    return 0;
}

int IPC_MID_UTIL_GetNetmask(const char *adapter_name, char *netmask)
{
    struct ifreq ifr;
    const char *ifname = adapter_name;
    int skfd;
    struct sockaddr_in *mask;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd < 0)
    {
        PRINT_ERR("socket create error\n");
        return -1;
    }
    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, ifname);

    if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0){
        if (ioctl(skfd, SIOCGIFNETMASK, &ifr) >= 0)
        {
            mask = (struct sockaddr_in *)&ifr.ifr_netmask;
            strcpy(netmask,inet_ntoa(mask->sin_addr));
            //printf("netmask:%s\n",netmask);
        }
    }
    else
    {
        close(skfd);
        PRINT_ERR("error!!:%s,device:%s\n",strerror(errno),adapter_name);
        return -1;
    }

    close(skfd);
    return 0;
}

int IPC_MID_UTIL_GetDns(char *dns1, char *dns2)
{
	FILE *fp;

    char line_buf[1000];
	int i = 0;
	char name[16] = "nameserver";
	char *pret;

    fp = fopen(_PATH_DNS_CONT, "r");
    if (NULL == fp)
    {
        PRINT_ERR("fopen %s ERROR!\n", _PATH_DNS_CONT);
        return -1;
    }

    while(1)
    {
        memset(line_buf, 0, sizeof(line_buf));

        /* read flash_map.ini  file */
        pret = fgets(line_buf, sizeof(line_buf) - 1, fp);
		if(NULL == pret)
		{
			fclose(fp);
			return 0;
		}

		if (NULL != (pret = strstr(line_buf, name)))
		{
			if(i == 0)
			{
				sscanf(line_buf, "%*[^ ] %s", dns1);
			}
			else if (i == 1)
			{
				sscanf(line_buf, "%*[^ ] %s", dns2);
			}
			else if(i > 1)
			{
				break;
			}
			i++;
		}

    }
	fclose(fp);

    return 0;
}




// 效验算法
static unsigned short cal_chksum(unsigned short *addr, int len)
{
    int nleft=len;
    int sum=0;
    unsigned short *w=addr;
    unsigned short answer=0;

    while(nleft > 1)
    {
           sum += *w++;
        nleft -= 2;
    }

    if( nleft == 1)
    {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
           sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

// Ping函数
int IPC_MID_UTIL_Ping( char *ips, int timeout)
{
    struct timeval timeo;
    int sockfd;
    struct sockaddr_in addr;
    struct sockaddr_in from;

    struct timeval *tval;
    struct ip *iph;
    struct icmp *icmp;

    char sendpacket[PACKET_SIZE];
    char recvpacket[PACKET_SIZE];

    int n;
    pid_t pid;
    int maxfds = 0;
    fd_set readfds;

    // 设定Ip信息
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ips);

    // 取得socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        printf("ip:%s,socket error\n",ips);
        return -1;
    }

    // 设定TimeOut时间
    timeo.tv_sec = timeout / 1000;
    timeo.tv_usec = timeout % 1000;

    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    {
        printf("ip:%s,setsockopt error\n",ips);
        return -1;
    }

    // 设定Ping包
    memset(sendpacket, 0, sizeof(sendpacket));

    // 取得PID，作为Ping的Sequence ID
    pid=getpid();
    int packsize;
    icmp=(struct icmp*)sendpacket;
    icmp->icmp_type=ICMP_ECHO;
    icmp->icmp_code=0;
    icmp->icmp_cksum=0;
    icmp->icmp_seq=0;
    icmp->icmp_id=pid;
    packsize=8+56;
    tval= (struct timeval *)icmp->icmp_data;
    gettimeofday(tval,NULL);
    icmp->icmp_cksum=cal_chksum((unsigned short *)icmp,packsize);

    // 发包
    n = sendto(sockfd, (char *)&sendpacket, packsize, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 1)
    {
        printf("ip:%s,sendto error\n",ips);
        return -1;
    }

    // 接受
    // 由于可能接受到其他Ping的应答消息，所以这里要用循环
    while(1)
    {
        // 设定TimeOut时间，这次才是真正起作用的
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfds = sockfd + 1;
        n = select(maxfds, &readfds, NULL, NULL, &timeo);
        if (n <= 0)
        {
        //    printf("ip:%s,Time out error\n",ips);
            close(sockfd);
            return -1;
        }

        // 接受
        memset(recvpacket, 0, sizeof(recvpacket));
        int fromlen = sizeof(from);
        n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
        if (n < 1) {
            break;
        }

        // 判断是否是自己Ping的回复
        char *from_ip = (char *)inet_ntoa(from.sin_addr);
        printf("fomr ip:%s\n",from_ip);
         if (strcmp(from_ip,ips) != 0)
         {
            printf("ip:%s,Ip wang\n",ips);
            break;
         }

        iph = (struct ip *)recvpacket;

        icmp=(struct icmp *)(recvpacket + (iph->ip_hl<<2));

        printf("ip:%s,icmp->icmp_type:%d,icmp->icmp_id:%d\n",ips,icmp->icmp_type,icmp->icmp_id);
       // 判断Ping回复包的状态
        if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid)
        {
            // 正常就退出循环
            break;
        }
        else
        {
            // 否则继续等
            continue;
        }
    }

    // 关闭socket
    close(sockfd);

    printf("ip:%s,Success\n",ips);
    return 0;
}


int uti_wifi_cli_strtoint(char *str)
{
	char *cmd_cur = str;
    unsigned int buf = 0;


    while(*cmd_cur != '\0')
    {
        if(*cmd_cur <= '9' && *cmd_cur >= '0')
        {
            buf *= 10;
            buf += *cmd_cur - '0';
            cmd_cur++;
        }
        else
        {
            printf("not a number\n");
            return -1;
        }
    }

    return buf;
}


int uti_str_get_ssid(char *src, char *dst)
{
    #if 0
    printf("src_len:%d, src:%s\n", src_len, src);
    char buf[4000] = {0};
    if (src_len > sizeof(buf))
    {
        PRINT_ERR("param error\n");
        return -1;
    }
    printf("1111\n");
    memset(buf, 0, sizeof(buf));
    printf("222\n");
    strncpy(buf, src, src_len);

    printf("buf:%s\n", buf);

    char *pBuf = buf;
    #else
    char *pBuf = src;
    #endif
    char *pBegin = NULL;
    char *pEnd = NULL;

    pBegin = strstr(pBuf, "ESSID");
    if(!pBegin)
    {
        PRINT_ERR("session not find ESSID\n");
        return -1;
    }

    pBegin += strlen("ESSID") + 2;

    pBuf = pBegin;
    pEnd = strstr(pBuf, "\"");
    if(!pEnd)
    {
        PRINT_ERR("session not find ESSID\n");
        return -1;
    }

    int len = pEnd - pBegin;
    strncpy(dst, pBegin, len);

    return 0;
}


int uti_str_get_field(char *src, char *spr1, char *spr2, char *dst, int *dst_len)
{
    #if 0
    char session[1000] = {0};
    if (src_len > sizeof(session))
    {
        PRINT_ERR("param error\n");
        return -1;
    }

    memset(session, 0, sizeof(session));
    strncpy(session, src, src_len);

    char *pBuf = session;
    #else
    char *pBuf = src;
    #endif
    char *pBegin = NULL;
    char *pEnd = NULL;

    pBegin = strstr(pBuf, spr1);
    if(!pBegin)
    {
        PRINT_ERR("session not find %s\n", spr1);
        return -1;
    }
    pBegin += strlen(spr1);

    pBuf = pBegin;
    pEnd = strstr(pBuf, spr2);
    if(!pEnd)
    {
        PRINT_ERR("session not find %s\n", spr2);
        return -1;
    }
    int len = pEnd - pBegin;
    strncpy(dst, pBegin, len);

    if (dst_len)
    {
        *dst_len = len;
    }

    return 0;
}

int uti_str_get_field2(char *src, char *spr1, char *dst, int dst_len)
{
    #if 0
    char session[1000] = {0};
    if (src_len > sizeof(session))
    {
        PRINT_ERR("param error\n");
        return -1;
    }

    memset(session, 0, sizeof(session));
    strncpy(session, src, src_len);

    char *pBuf = session;
    #else
    char *pBuf = src;
    #endif
    char *pBegin = NULL;

    pBegin = strstr(pBuf, spr1);
    if(!pBegin)
    {
        PRINT_ERR("session not find %s\n", spr1);
        return -1;
    }
    pBegin += strlen(spr1);

    strncpy(dst, pBegin, dst_len);

    return 0;
}

int uti_str_get_security(char *session)
{
    #if 0
    char session[1000] = {0};
    if (src_len > sizeof(session))
    {
        PRINT_ERR("param error\n");
        return -1;
    }
    memset(session, 0, sizeof(session));
    strncpy(session, src, src_len);

    char *pBuf = session;
    #endif

    char *pBegin = NULL;
    int ECY_OFF,WPA,WPA2,AES,TKIP;
/*
1
    WPA Version +WPA2 Version =WPA
                 WPA2 Version =WPA2

2
    Group Cipher : CCMP  -->AES
    Group Cipher : TKIP  -->SKIP


*/
    ECY_OFF = WPA = WPA2 = AES = TKIP = 0;

    char *pBuf = session;
    pBegin = strstr(pBuf, "Encryption key:off");
    if(pBegin)
    {
        ECY_OFF = 1;
    }

    pBuf = session;
    pBegin = strstr(pBuf, "WPA Version");
    if(pBegin)
    {
        WPA = 1;
    }

    pBuf = session;
    pBegin = strstr(pBuf, "WPA2 Version");
    if(pBegin)
    {
        WPA2 = 1;
    }

    pBuf = session;
    pBegin = strstr(pBuf, "Group Cipher : CCMP");
    if(pBegin)
    {
        AES = 1;
    }

    pBuf = session;
    pBegin = strstr(pBuf, "Group Cipher : TKIP");
    if(pBegin)
    {
        TKIP = 1;
    }

/*
#define WPAECN_NONE     0
#define WEP_64_ASSCI    1
#define WEP_64_HEX      2
#define WEP_128_ASSCI   3
#define WEP_128_HEX     4
#define WPAPSK_TKIP     5
#define WPAPSK_AES      6
#define WPA2PSK_TKIP    7
#define WPA2PSK_AES     8

*/

    int security = 0;
    if(ECY_OFF)
    {
        security = WPAECN_NONE;
    }
    else if (WPA && TKIP)
    {
        security = WPAPSK_TKIP;
    }
    else if (WPA && AES)
    {
        security = WPAPSK_AES;
    }
    else if(WPA2 && TKIP)//WPA2PSK_TKIP
    {
        security = WPA2PSK_TKIP;
    }
    else if(WPA2 && AES)
    {

        security = WPA2PSK_AES;
    }
    else
    {
        security = WEP_64_ASSCI;
    }

    return security;
}

#if 1
//buff ̫С�᲻��
int IPC_MID_UTIL_WifiScanAP(const char* dev, NETCAM_WIFI_SCAN_LIST *plist)
{
    int ret = 0;
    char buff[50000] = {0};
    char cmd[128] = {0};

    //int len = 0;
    int bufLen ;
    int count = 0;
    WIFI_SCAN_LIST_t list[20];
    char tmpBuf[128] = {0};
    //char session[1000] = {0};

    memset(list,0,sizeof(list));

    //printf("------ scan ap list start ---\n");
	snprintf(cmd, sizeof(cmd),"%s %s scan",IWLIST_CMD, dev);

    memset(buff, 0, sizeof(buff));
    ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
	if(ret != 0)
    {
        printf("network_wifi_getStringBySystem failed\n");
		return ret;
	}
    //printf("------ scan ap list stop ----\n");

    bufLen = strlen(buff);
    if (bufLen <= sizeof(buff))
    {
        buff[bufLen - 1] = '\0';
    }
    else
    {
        buff[sizeof(buff) - 1] = '\0';
    }
    //printf("iwlist scan return bufLen:%d, buf:\n%s\n", bufLen, buff);

    char *psessBuf = buff;
    char *pBuf = buff;

    //char *pBegin = buff;
    //char *pEnd = buff;
    char *sessBegin = buff;
    char *sessEnd = buff;
    while('\0' != *pBuf)
    {
        sessBegin = strstr(psessBuf, "Cell");
        if(!sessBegin)
        {
            //printf("no more cell,find ssid number:%d\n", count);
            break;
        }
        sessBegin += strlen("Cell");
        psessBuf = sessBegin;

        sessEnd = strstr(psessBuf, "Cell");
        if(!sessEnd)
        {
            sessEnd = buff + bufLen -1;
            //printf("last cell ,sessEnd:%d,find ssid number:%d\n", sessEnd, count + 1);
        }
        psessBuf = sessEnd;


        char *session = sessBegin;


        /*1 essid*/
        uti_str_get_ssid(session, list[count].essid);
        if (strlen(list[count].essid) > 0)
        {
            list[count].vaild = 1;
        }
        else
        {
            list[count].vaild = 0;
            continue;
        }
        //printf("index:%d, essid:%s\n", count, list[count].essid);

        //2,channel
        memset(tmpBuf,0,sizeof(tmpBuf));
        uti_str_get_field(session, "Channel ", ")", tmpBuf, NULL);
        list[count].channel = atoi(tmpBuf);
        //printf("channel:%d\n", list[count].channel);

        //3,signal level
        memset(tmpBuf,0,sizeof(tmpBuf));
        uti_str_get_field(session, "Signal level=-", " ", tmpBuf, NULL);
        list[count].quality = atoi(tmpBuf);
        //printf("quality:%d\n", list[count].quality);

        //4,mac
        uti_str_get_field2(session, "Address: ", list[count].mac, 17);
        //printf("mac:%s\n", list[count].mac);

        //5,security
        list[count].security = uti_str_get_security(session);
        //printf("security:%d\n", list[count].security);

        count++;

        if (count >= 20)
        {
            PRINT_INFO("scan max is 20, so break\n");
            break;
        }
    }

    memcpy(plist->list, list, sizeof(list));
    plist->count = count;
	return 0;
}
#endif

void IPC_MID_UTIL_PrintAPList(NETCAM_WIFI_SCAN_LIST *plist)
{
#if 1
    printf("scan list, count=%d\n", plist->count);
    printf("index  valid    mac              channel   quality  security     ssid\n");
    int j;
    for (j = 0; j < 20; j ++)
    {
        printf("%2d   %d     %s      %2d      %3d      %2d      %s     \n",
            j,
            plist->list[j].vaild,
            plist->list[j].mac,
            plist->list[j].channel,
            plist->list[j].quality,
            plist->list[j].security,
            plist->list[j].essid);
    }
#endif
}

void IPC_MID_UTIL_SortAPList(NETCAM_WIFI_SCAN_LIST *plist)
{
    int i, j;

    for (i = 0; i < 20 -1 ; i++) {
        if(plist->list[i].vaild == 0) {
            continue;
        }

        //����Ǻ͵�һ�����ıȽϵ���
        for (j = i+1; j < 20; j++) {
            if(plist->list[j].vaild == 0) {
                continue;
            }

            if(plist->list[i].quality > plist->list[j].quality){
                //change
                WIFI_SCAN_LIST_t tmp;
                memcpy(&tmp, &(plist->list[i]), sizeof(WIFI_SCAN_LIST_t));
                memcpy(&(plist->list[i]), &(plist->list[j]), sizeof(WIFI_SCAN_LIST_t));
                memcpy(&(plist->list[j]), &tmp, sizeof(WIFI_SCAN_LIST_t));
            }
        }
    }
}

int uti_wifi_scan_ap(const char* dev, NETCAM_WIFI_SCAN_LIST *plist)
{
    char *path = "/tmp/list";
    char cmd[128] = {0};
    //printf("------ scan ap list start ---\n");
	snprintf(cmd, sizeof(cmd), "%s %s scan > %s", IWLIST_CMD, dev, path);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    //printf("------ scan ap list stop ----\n");

    char *buff = NULL;
    int bufLen = 0;
    buff = IPC_MID_UTIL_OpenFile(path, &bufLen);
    //printf("iwlist scan return bufLen:%d, buf:\n%s\n", bufLen, buff);

	snprintf(cmd, sizeof(cmd), "rm -rf %s", path);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    char *pBuf = buff;
    char *psessBuf = buff;

    char *sessBegin = NULL;
    char *sessEnd = NULL;

    int count = 0;
    WIFI_SCAN_LIST_t list[20];
    memset(list, 0, sizeof(list));
    char tmpBuf[128] = {0};

    while('\0' != *pBuf)
    {
        sessBegin = strstr(psessBuf, "Cell");
        if(!sessBegin)
        {
            //printf("no more cell,find ssid number:%d\n", count);
            break;
        }
        sessBegin += strlen("Cell");
        psessBuf = sessBegin;

        sessEnd = strstr(psessBuf, "Cell");
        if(!sessEnd)
        {
            sessEnd = buff + bufLen -1;
            //printf("last cell ,sessEnd:%d,find ssid number:%d\n", sessEnd, count + 1);
        }
        psessBuf = sessEnd;

        char *session = sessBegin;

        /*1 essid*/
        uti_str_get_ssid(session, list[count].essid);
        if (strlen(list[count].essid) > 0)
        {
            list[count].vaild = 1;
        }
        else
        {
            list[count].vaild = 0;
            continue;
        }
        //printf("index:%d, essid:%s\n", count, list[count].essid);

        //2,channel
        memset(tmpBuf,0,sizeof(tmpBuf));
        uti_str_get_field(session, "Channel ", ")", tmpBuf, NULL);
        list[count].channel = atoi(tmpBuf);
        //printf("channel:%d\n", list[count].channel);

        //3,signal level
        memset(tmpBuf,0,sizeof(tmpBuf));
        uti_str_get_field(session, "Signal level=-", " ", tmpBuf, NULL);
        list[count].quality = atoi(tmpBuf);
        //printf("quality:%d\n", list[count].quality);

        //4,mac
        uti_str_get_field2(session, "Address: ", list[count].mac, 17);
        //printf("mac:%s\n", list[count].mac);

        //5,security
        list[count].security = uti_str_get_security(session);
        //printf("security:%d\n", list[count].security);

        count++;

        if (count >= 20)
        {
            PRINT_INFO("scan max is 20, so break\n");
            break;
        }
    }

    if (buff)
    {
        free(buff);
    }

    memcpy(plist->list, list, sizeof(list));
    plist->count = count;
	return 0;
}

int IPC_MID_UTIL_WifiScan(const char *adapter_name, NETCAM_WIFI_SCAN_LIST *plist)
{
    int retry = 10;
    do
    {
        uti_wifi_scan_ap(adapter_name, plist);
        sleep(1);
        retry --;
        if(retry <= 0)
        {
            break;
        }
        PRINT_INFO("wifi scan, retry = %d\n", 9 - retry);
    } while(plist->count <= 0);

    return 0;
}

#if 0
int main(int argc ,char *arg[])
{

    ping(arg[1],atoi(arg[2]));
}
#endif

