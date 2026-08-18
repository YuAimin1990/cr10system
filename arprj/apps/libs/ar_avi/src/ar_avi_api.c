#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include "ar_avi_api.h"
#include "ar_avi_utility.h"

/**********************************************************************
函数描述：计算结束时间
入口参数：char *time_stop: 计算后得到的结束时间
          char *file_path: avi文件路径名
          int frame_count: avi文件中存的总帧数
          int fps: 每秒的帧数
返回值：  无
**********************************************************************/
void AR_AVI_CalcStopTime(char *time_stop, char *file_path, int video_count, int fps)
{
    char str_tmp[10];
    int    y1,m1,d1,h1,n1,s1;  //start
    int duration;
    time_t timep;
    struct tm *p;

    int i;
    char file_name[128];

    memset(file_name, 0 ,sizeof(file_name));
    i = AR_AVI_FindLastIndex(file_path, '/');
    strcpy(file_name, file_path + i + 1);

    //PRINT_INFO("file_path:%s, file_name:%s\n", file_path, file_name);

    AR_AVI_ExtractSubstr(str_tmp, file_name, 2, 2+2);
    h1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 4, 4+2);
    n1 = atoi(str_tmp);
    AR_AVI_ExtractSubstr(str_tmp, file_name, 6, 16+2);
    s1 = atoi(str_tmp);

    #if 0
    duration = (int) (video_count + fps - 1) / fps;
    #else
    duration = (int) video_count / fps;
    #endif

    time(&timep);
    struct tm tt = {0};
    p = localtime_r(&timep, &tt);

    y1 = 1970;
    m1 = 1;
    d1 = 1;

    p->tm_year = y1 - 1900;
    p->tm_mon = m1 - 1;
    p->tm_mday = d1;
    p->tm_hour = h1;
    p->tm_min = n1;
    p->tm_sec = s1;

    //PRINT_INFO ("%d-%d-%d ", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
    //PRINT_INFO(" %d:%d:%d\n", p->tm_hour, p->tm_min, p->tm_sec);

    timep = mktime(p);

    timep += duration;
    memset(&tt, 0, sizeof(struct tm));
    p = localtime_r(&timep, &tt);

    //PRINT_INFO ("%d-%d-%d ", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
    //PRINT_INFO(" %d:%d:%d\n", p->tm_hour, p->tm_min, p->tm_sec);

    #if 0
    sprintf(time_stop, "%04d%02d%02d%02d%02d%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
                                                   p->tm_hour, p->tm_min, p->tm_sec);
    #else
    sprintf(time_stop, "%02d%02d%02d", p->tm_hour, p->tm_min, p->tm_sec);
    #endif
}


