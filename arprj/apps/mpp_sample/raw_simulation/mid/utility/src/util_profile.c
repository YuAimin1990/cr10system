#include "util_profile.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_PROFILE   32


typedef struct
{
    char strName[128];
    AR_U64 u64Tic;
    AR_U64 u64Toc;
} AR_PROFILE_S;

AR_U32 u32ProfileEnable;
AR_PROFILE_S astProfile[MAX_PROFILE];

AR_S32 EnableProfile()
{
    memset(astProfile, 0, sizeof(AR_PROFILE_S) * 10);
    u32ProfileEnable = 1;
    return 0;
}

AR_S32 DisbleProfile()
{
    u32ProfileEnable = 0;
    return 0;
}

AR_S32 ProfileStart(const AR_CHAR * pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        for(i = 0; i < MAX_PROFILE; i++)
        {
            if(strlen(astProfile[i].strName) == 0)
                break;
            if(0 == strcmp(astProfile[i].strName, pchName))
                break;
        }

        if(i == MAX_PROFILE)
        {
            printf("Profile has run out.\n");
            return 0;
        }

        //char dev_name[40] = { '\0' };
        sprintf(astProfile[i].strName, "%s", pchName);
        struct timeval tm;
        gettimeofday(&tm, NULL);
        astProfile[i].u64Tic = tm.tv_sec * 1000000 + tm.tv_usec;
    }

    return 0;
}

AR_S32 ProfileEnd(const AR_CHAR * pchName)
{
    AR_U32 i = 0;

    if(u32ProfileEnable)
    {
        struct timeval tm;
        gettimeofday(&tm, NULL);

        for(i = 0; i < MAX_PROFILE; i++)
        {
            if(0 == strcmp(astProfile[i].strName, pchName))
                break;
        }
        if(i == MAX_PROFILE)
        {
            printf("Profile has run out.\n");
            return 0;
        }

        astProfile[i].u64Toc = tm.tv_sec * 1000000 + tm.tv_usec;

        AR_FLOAT dTime = (astProfile[i].u64Toc - astProfile[i].u64Tic)*1.0/1000;
        printf("[IPCAM][PROF] %s takes %f ms\n", astProfile[i].strName, dTime);

        memset(astProfile[i].strName, 0, sizeof(astProfile[i].strName));
        astProfile[i].u64Tic = 0;
        astProfile[i].u64Toc = 0;
    }

    return 0;
}


