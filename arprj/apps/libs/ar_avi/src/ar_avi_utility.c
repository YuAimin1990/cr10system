#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ar_avi_api.h"
#include "ar_avi_utility.h"

int AR_AVI_WriteChar(FILE *file, AR_CHAR c)
{
    return fwrite(&c, sizeof(AR_CHAR), 1, file);
}
int AR_AVI_WriteInt32(FILE *file, int i)
{
    return fwrite(&i, sizeof(int), 1, file);
}
int AR_AVI_WriteFourcc(FILE *file, int fourcc)
{
    return fwrite(&fourcc, sizeof(AR_CHAR)*4, 1, file);
}
int AR_AVI_WriteData(FILE *file, AR_CHAR *data, int size)
{
    return fwrite(data, size, 1, file);
}

int AR_AVI_ReadChar(FILE *file, AR_CHAR *c)
{
    return fread(c, sizeof(AR_CHAR), 1, file);
}
int AR_AVI_ReadInt32(FILE *file, int *i)
{
    return fread(i, sizeof(int), 1, file);
}
int AR_AVI_ReadFourcc(FILE *file, int *fourcc)
{
    return fread(fourcc, sizeof(AR_CHAR)*4, 1, file);
}
int AR_AVI_ReadData(FILE *file, AR_UCHAR *data, int size)
{
    return fread(data, size, 1, file);
}


#if 1
void AR_AVI_Write32(void *pp, int w)
{
    AR_CHAR *p = pp;

    p[0] = ( w      )&0xff;
    p[1] = ( w >> 8 )&0xff;
    p[2] = ( w >> 16)&0xff;
    p[3] = ( w >> 24)&0xff;
}

int AR_AVI_Read32(void *pp)
{
    AR_CHAR *p = pp;
    int tmp32 = (int)((p[0])|(p[1])<<8|(p[2])<<16|(p[3])<<24);
    return tmp32;
}

#endif
void AR_AVI_ExtractSubstr(AR_CHAR *dest, AR_CHAR *src, int start, int end)
{
    int i=start;
    if(start>strlen(src))
        return;
    if(end>strlen(src))
        end=strlen(src);
    while(i<end)
    {
        dest[i-start]=src[i];
        i++;
    }
    dest[i-start]='\0';
    return;
}

int AR_AVI_FindLastIndex(AR_CHAR *str, AR_CHAR c)
{
    int n;
    n = strlen(str);

    while(n--)
    {
        if(c == str[n-1])
            return n-1 ;
    }
    return -1;
}

AR_U32 AR_AVI_GetChnFromName(AR_CHAR *file_name)
{
    AR_U32 tmp;
    tmp = (AR_U32)(file_name[0] - 48);
    return tmp;
}

AR_U64 AR_AVI_GetStartTimeFromName(AR_CHAR *dir, AR_CHAR *file_name)
{
    AR_CHAR tmp_str[20];
    AR_U64 tmp64;
    AR_AVI_ExtractSubstr(tmp_str, file_name, 2, 2+6);
    AR_CHAR time_str[20] = {0};
    sprintf(time_str, "%s%s", dir, tmp_str);
    tmp64 = (AR_U64)atoll(time_str);
    return tmp64;
}

AR_U64 AR_AVI_GetStopTimeFromName(AR_CHAR *dir, AR_CHAR *file_name)
{
    AR_CHAR tmp_str[20];
    AR_U64 tmp64;
    AR_AVI_ExtractSubstr(tmp_str, file_name, 9, 9+6);
    AR_CHAR time_str[20] = {0};
    sprintf(time_str, "%s%s", dir, tmp_str);
    tmp64 = (AR_U64)atoll(time_str);
    return tmp64;
}

void AR_AVI_GetDateAndTimeStr(AR_CHAR *str_date, AR_CHAR *str_time, AR_S32 preSeconds)
{
    struct tm *ptm;
    long ts;

    ts = time(NULL);
    struct tm tt = {0};
    ptm = localtime_r(&ts, &tt);

    time_t timep;
    timep = mktime(ptm);
    timep -= preSeconds;
    memset(&tt, 0, sizeof(struct tm));
    ptm = localtime_r(&timep, &tt);

    sprintf(str_date, "%04d%02d%02d", ptm->tm_year+1900,
    						 ptm->tm_mon+1,
    						 ptm->tm_mday);

    sprintf(str_time, "%02d%02d%02d", ptm->tm_hour,
    						 ptm->tm_min,
    						 ptm->tm_sec);

    return;
}

void get_nowhm_str(AR_CHAR *str)
{
    struct tm *ptm;
    long ts;

    ts = time(NULL);
    struct tm tt = {0};
    ptm = localtime_r(&ts, &tt);

	sprintf(str, "%02d%02d%02d", ptm->tm_hour,
								 ptm->tm_min,
								 ptm->tm_sec);

    return;
}

void tm_to_dmstime(AVI_DMS_TIME *dst, struct tm *src)
{
	dst->dwYear = src->tm_year;
	dst->dwMonth = src->tm_mon;
	dst->dwDay =  src->tm_mday;
	dst->dwHour = src->tm_hour;
	dst->dwMinute =src->tm_min;
	dst->dwSecond = src->tm_sec;
}

void tm_add_seconds(struct tm *dst, struct tm *src, int time_seconds)
{
	time_t timep;
    struct tm t1 = {0};
	struct tm *p = &t1;

	p->tm_year = src->tm_year - 1900;
	p->tm_mon = src->tm_mon - 1;
	p->tm_mday = src->tm_mday;
	p->tm_hour = src->tm_hour;
	p->tm_min = src->tm_min;
	p->tm_sec = src->tm_sec;


	timep = mktime(p);

	timep += time_seconds;
    memset(&t1, 0, sizeof(struct tm));
	p = localtime_r(&timep, &t1);

	dst->tm_year = p->tm_year + 1900;
	dst->tm_mon = p->tm_mon + 1;
	dst->tm_mday =  p->tm_mday;
	dst->tm_hour = p->tm_hour;
	dst->tm_min =p->tm_min;
	dst->tm_sec = p->tm_sec;

}

AR_U64 u64t_add_seconds(AR_U64 opr1, int seconds)
{
    AVI_DMS_TIME src;
    AR_AVI_U64ToTime(&src, opr1);

    time_t timep;
	struct tm *p;

	time(&timep);

    struct tm t1 = {0};
    p = localtime_r(&timep, &t1);

	p->tm_year = src.dwYear - 1900;
	p->tm_mon = src.dwMonth - 1;
	p->tm_mday = src.dwDay;
	p->tm_hour = src.dwHour;
	p->tm_min = src.dwMinute;
	p->tm_sec = src.dwSecond;

	timep = mktime(p);
	timep += seconds;
    memset(&t1, 0, sizeof(struct tm));
	p = localtime_r(&timep, &t1);

    AVI_DMS_TIME dst;
	dst.dwYear = p->tm_year + 1900;
	dst.dwMonth = p->tm_mon + 1;
	dst.dwDay =  p->tm_mday;
	dst.dwHour = p->tm_hour;
	dst.dwMinute = p->tm_min;
	dst.dwSecond = p->tm_sec;

    AR_U64 opr2 = AR_AVI_TimeToU64(&dst);

    return opr2;
}
int cal_time_sub(AR_U64 opr1, AR_U64 opr2)
{
    AVI_DMS_TIME src;
    AR_AVI_U64ToTime(&src, opr1);

    time_t timep1;
    time_t timep2;
	struct tm *p;

	time(&timep1);
    struct tm t1 = {0};
    p = localtime_r(&timep1, &t1);

	p->tm_year = src.dwYear - 1900;
	p->tm_mon = src.dwMonth - 1;
	p->tm_mday = src.dwDay;
	p->tm_hour = src.dwHour;
	p->tm_min = src.dwMinute;
	p->tm_sec = src.dwSecond;

	timep1 = mktime(p);

    memset(&src, 0, sizeof(AVI_DMS_TIME));
    AR_AVI_U64ToTime(&src, opr2);
	p->tm_year = src.dwYear - 1900;
	p->tm_mon = src.dwMonth - 1;
	p->tm_mday = src.dwDay;
	p->tm_hour = src.dwHour;
	p->tm_min = src.dwMinute;
	p->tm_sec = src.dwSecond;
    timep2 = mktime(p);

    return (timep1 - timep2);
}

AR_U64 AR_AVI_TimeToU64(AVI_DMS_TIME *p)
{
    AR_CHAR tmp[20];
    AR_U64 tmp64 = 0;
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%04d%02d%02d%02d%02d%02d", p->dwYear, p->dwMonth, p->dwDay,
                                            p->dwHour, p->dwMinute, p->dwSecond);
    tmp64 = atoll(tmp);

    return tmp64;
}

void AR_AVI_U64ToTime(AVI_DMS_TIME *ptime, AR_U64 tmp64)
{
    AR_CHAR tmp_str[20];
    AR_CHAR tmp[20];

    memset(tmp_str, 0, sizeof(tmp_str));
    sprintf(tmp_str, "%lld", tmp64);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 0, 0+4);
    ptime->dwYear = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 4, 4+2);
    ptime->dwMonth = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 6, 6+2);
    ptime->dwDay = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 8, 8+2);
    ptime->dwHour = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 10, 10+2);
    ptime->dwMinute = atoi(tmp);

    memset(tmp, 0, sizeof(tmp));
    AR_AVI_ExtractSubstr(tmp, tmp_str, 12, 12+2);
    ptime->dwSecond = atoi(tmp);

}

void AR_AVI_msleep(AR_U32 mSec)
{
    struct timeval tv;
    tv.tv_sec = mSec/1000;
    tv.tv_usec = (mSec % 1000) * 1000;
    int err;
    do {
       err = select(0, NULL, NULL, NULL, &tv);
    } while (err < 0 && errno == EINTR);
}


