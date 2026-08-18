
#include "recmng_rec.h"
#include "recmng_sd.h"

static char s_achSDPathName[128] = {0};
static char s_achSDPartitionPathName[128] = {0};
//int sd_is_formated = 0;

#if 1
static int sd_is_partition_exist(void)
{
    if(access(IPC_SD_MOUNT_POINT, X_OK) == 0)
    {
        //PRINT_INFO("%s is exist\n", s_achSDPartitionPathName);
        return 1;
    }
    //PRINT_INFO("%s is not exist\n", s_achSDPartitionPathName);
    return 0;
}
#endif

int IPC_MID_RECMNG_GetSDFreeSize(void)
{
    return 100;
#if 0
    struct statfs statFS;
    AR_U64 freeBytes = 0;

    if (statfs(IPC_SD_MOUNT_POINT, &statFS) == -1)
    {
        //PRINT_INFO("statfs failed, path -> [%s]\n", IPC_SD_MOUNT_POINT);
        return 0;
    }

    if (statFS.f_type != 0x00004d44)
        return 0;

    freeBytes = (AR_U64)statFS.f_bfree * (AR_U64)statFS.f_frsize;

    return    (int)(freeBytes >> 20) ;
#endif
}

#if 0
static int str_get_substr(char *src, char *spr1, char *spr2, char *dst, int *dst_len)
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
#endif
//if rw, return 1
int IPC_MID_RECMNG_CheckSDRW(void)
{
    return 1;
#if 0
    int ret = 0;
    char buff[50000] = {0};
    char cmd[128] = {0};

	snprintf(cmd, sizeof(cmd), "mount");
    memset(buff, 0, sizeof(buff));
    ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
	if(ret != 0)
    {
        PRINT_ERR("call %s error\n", cmd);
		return ret;
	}

    int bufLen;
    bufLen = strlen(buff);
    if (bufLen <= sizeof(buff))
    {
        buff[bufLen - 1] = '\0';
    }
    else
    {
        buff[sizeof(buff) - 1] = '\0';
    }

    char *psessBuf = buff;
    char *pBuf = buff;

    char *sessBegin = buff;
    char *sessEnd = buff;

    int is_rw = 0;
    char *spr0 = "\/dev\/mmcblk";
    char tmpBuf[128] = {0};
    int n = 0;
    while('\0' != *pBuf)
    {
        sessBegin = strstr(psessBuf, spr0);
        if(!sessBegin)
        {
            //PRINT_INFO("no %s, to break, n = %d\n", spr0, n);
            break;
        }
        n++;
        sessBegin += strlen(spr0);
        //PRINT_INFO("len=%d, n = %d\n", strlen(spr0), n);
        psessBuf = sessBegin;

        sessEnd = strstr(psessBuf, spr0);
        if(!sessEnd)
        {
            //PRINT_INFO("222 n = %d\n", n);
            sessEnd = buff + bufLen -1;
        }
        psessBuf = sessEnd;
        char *session = sessBegin;

        memset(tmpBuf,0,sizeof(tmpBuf));
        str_get_substr(session, "/mnt/sd_card", "relatime", tmpBuf, NULL);
        char *ptmp = strstr(tmpBuf, "rw");
        if(ptmp)
        {
            //PRINT_INFO("tmpBuf:%s\n", tmpBuf);
            is_rw = 1;
            break;
        }
    }

    //PRINT_INFO("rw:%d\n", is_rw);

	return is_rw;
#endif
}

int IPC_MID_RECMNG_GetSDSize(void)
{
    //test
    return 100;
#if 0
    struct statfs statFS;

    if (statfs(IPC_SD_MOUNT_POINT, &statFS) == -1)
    {
        //PRINT_INFO("statfs failed, path -> [%s]\n", IPC_SD_MOUNT_POINT);
        return 0;
    }

    if (statFS.f_type != 0x00004d44)
        return 0;

    unsigned long long blocksize = statFS.f_bsize;
    unsigned long long totalsize = blocksize * statFS.f_blocks;

    //printf("Total_size = %lluB, = %lluKB, = %lluMB, = %lluGB,\n",
    //totalsize, totalsize >> 10, totalsize >> 20, totalsize >> 30);

    return (int)(totalsize >> 20) ;
#endif
}

int IPC_MID_RECMNG_IsSDMount(void)
{
#if 1
    struct statfs statFS;

    if (statfs(IPC_SD_MOUNT_POINT, &statFS) == -1)
    {
        //PRINT_INFO("statfs failed, path -> [%s]\n", IPC_SD_MOUNT_POINT);
        return -1;
    }

    if (statFS.f_type == 0x00004d44 && (sd_is_partition_exist() == 1))
    {
        //PRINT_INFO("%s is mounted\n", IPC_SD_MOUNT_POINT);
        return 0;
    }
    else
    {
        //PRINT_INFO("%s is not mounted, f_type:0x%x\n", IPC_SD_MOUNT_POINT, statFS.f_type);
        return -1;
    }

	return 0;
#endif
}

int IPC_MID_RECMNG_MountSD(void)
{
	int ret = 0;
#if 1
    if(access(IPC_SD_MOUNT_POINT, F_OK) != 0 )
    {
        if((mkdir(IPC_SD_MOUNT_POINT, 0777)) < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_MOUNT_POINT);
            return -1;
        }
    }

    ret = mount(s_achSDPathName, IPC_SD_MOUNT_POINT, "vfat", 0, NULL);
    if(ret != 0)
    {
        ret = mount(s_achSDPartitionPathName, IPC_SD_MOUNT_POINT, "vfat", 0, NULL);
        if(ret != 0)
        {
            PRINT_ERR("mount failed: %s\n",strerror(errno));
            return -1;
        }
        PRINT_INFO("mount %s successed\n", s_achSDPartitionPathName);
        return 0;
    }
    
    PRINT_INFO("mount %s successed\n", s_achSDPathName);
    return 0;
#endif
}

int IPC_MID_RECMNG_RemountSD(void)
{
	int ret = 0;
    if(access(IPC_SD_MOUNT_POINT, F_OK) != 0 )
    {
        if((mkdir(IPC_SD_MOUNT_POINT, 0777)) < 0)
        {
            PRINT_ERR("mkdir %s failed\n", IPC_SD_MOUNT_POINT);
            return -1;
        }
    }

	ret = mount(s_achSDPathName, IPC_SD_MOUNT_POINT, "vfat", MS_REMOUNT, NULL);
	if(ret != 0)
        PRINT_ERR("remount sd card failed: %s\n",strerror(errno));


	return ret;
}

int IPC_MID_RECMNG_UmountSD(void)
{
    int ret = 0;

#if 1
    ret = umount2(IPC_SD_MOUNT_POINT, MNT_DETACH|MNT_FORCE);
    if(ret != 0)
        PRINT_ERR("unmount sd card failed: %s\n",strerror(errno));
    unlink(IPC_SD_MOUNT_POINT);
#endif

    return ret;
}

static int sd_fdisk_and_mkdosfs(void)
{
#if 1
	FILE *fp = NULL;
	char cmd[128] = {0};
	memset(cmd, 0, sizeof(cmd));

	/*partition and format disc*/
	snprintf(cmd, sizeof(cmd), "fdisk %s", s_achSDPathName);

	fp = popen(cmd, "w");
	if(fp == NULL)
	{
		PRINT_INFO("MMC popen fail\n");
		return -1;
	}

	/*del all parmary partitions and extension partitons*/
	fprintf(fp, "d\n");
	fprintf(fp, "1\n");

	fprintf(fp, "d\n");
	fprintf(fp, "2\n");

	fprintf(fp, "d\n");
	fprintf(fp, "3\n");

	fprintf(fp, "d\n");
	fprintf(fp, "4\n");

	/*one parimary partiton*/
	fprintf(fp, "n\n");
	fprintf(fp, "p\n");
	fprintf(fp, "1\n");
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	/*changed fs ID for FAT32*/
	fprintf(fp, "t\n");
	fprintf(fp, "c\n");

	/*save*/
	fprintf(fp, "w\n");

	pclose(fp);


	//IPC_MID_UTIL_SysCall("mdev -s", NULL, 0);

	return 0;
#else
    return 0;
#endif
}

static int is_digit(char *str)
{
    int n, i;
    n = strlen(str);
    //int ret = 1;

    for(i = 0; i < n; i++)
    {
          if( str[i] < '0' || str[i] > '9')
            return 0;
    }
    return 1;
}

int is_fix_format_file(char *str)
{
    int n;

    char tmp[20];

    //ch0_20160101020304
    //0_020304
    //0_000931.part
    n = strlen(str);
    if(n != 13)
        return 0;

    if( str[0] < '0' || str[0] > '9')
        return 0;

    if(str[1] != '_')
        return 0;

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 2, 2+6);

    if (0 == is_digit(tmp))
        return 0;

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 8, 8+5);

    if (strcmp(tmp, ".part") != 0)
        return 0;

    return 1;
}

int is_fix_format_path(char *file_path)
{
    int i;
    char str[128];

    i = AR_AVI_FindLastIndex(file_path, '/');
    if(i < 0)
        return 0;
    memset(str, 0 ,sizeof(str));
    strcpy(str, file_path + i + 1);
//    PRINT_INFO("check %s is fix format or not.\n", str);

    int ret = is_fix_format_file(str);
    return ret;
}

int is_date_dir_format(char *str)
{
    int i, n;
    char tmp[30];
    int tmp32;

    n = strlen(str);

    if(n != 8)
        return 0;

    for(i = 0; i < n; i++)
    {
        if( str[i] < '0' || str[i] > '9')
            return 0;
    }

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 4, 4+2);
    tmp32 = atoi(tmp);
    if(tmp32 <=0 || tmp32 > 12)
        return 0;

    memset(tmp, 0, sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 6, 6+2);
    tmp32 = atoi(tmp);
    if(tmp32 <=0 || tmp32 > 31)
        return 0;

    return 1;
}

int is_video_file_format(char *str, int type)
{
    int n;
    char tmp[30];

    //0_190101_190202.avi
    n = strlen(str);

    if(n != 19)
        return 0;

    if( str[0] < '0' || str[0] > '9')
        return 0;

    if(str[1] != '_' || str[8] != '_' || str[15] != '.' )
        return 0;

    if (type == 0)
    {
        if(str[16] != 'a' || str[17] != 'v' || str[18] != 'i' )
            return 0;
    }
    else if (type == 1)
    {
        if(str[16] != 'm' || str[17] != 'o' || str[18] != 'v' )
            return 0;
    }

    memset(tmp, 0 ,sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 2, 2+6);

    if (0 == is_digit(tmp))
        return 0;

    memset(tmp, 0 ,sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 9, 9+6);

    if (0 == is_digit(tmp))
        return 0;

    return 1;
}

int is_avi_file_format(char *str)
{
    return is_video_file_format(str, 0);
}

int is_mov_file_format(char *str)
{
    return is_video_file_format(str, 1);
}

int is_snap_file_format(char *str)
{
    int n;
    char tmp[30];

    //0_190101_190202.avi
    //0_190101.jpg
    n = strlen(str);

    if(n != 12)
        return 0;

    if( str[0] < '0' || str[0] > '9')
        return 0;

    if(str[1] != '_' || str[8] != '.')
        return 0;

    if(str[9] != 'j' || str[10] != 'p' || str[11] != 'g' )
        return 0;

    memset(tmp, 0 ,sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, str, 2, 2+6);

    if (0 == is_digit(tmp))
        return 0;

    return 1;
}

int is_avi_file_path_format(char *file_path)
{
    char str[128];
    int i, ret;


    i = AR_AVI_FindLastIndex(file_path, '/');
    if(i < 0)
        return 0;
    //PRINT_INFO("i : %d\n", i);
    memset(str, 0 ,sizeof(str));
    strcpy(str, file_path + i + 1);

    ret = is_avi_file_format(str);
    return ret;
}

static void find_oldest_file(char *file_name, long long *min, char *min_filename)
{
    char tmp[128];
    long long tmp32;

    memset(tmp, 0 ,sizeof(tmp));
    IPC_MID_UTIL_ExtractSubstr(tmp, file_name, 2, 2+6);
    tmp32 = atoll(tmp);
    if(tmp32 < *min)
    {
        *min = tmp32;
        strcpy(min_filename, file_name);
    }
}

static int find_oldest_dir(char *min_dir)
{
    DIR *pDir;
    struct dirent *ent;
    int min = 0;
    int tmp = 0;

    min = 99999999;

    pDir = opendir(IPC_SD_VIDEO_PATH);

    while((ent = readdir(pDir)) != NULL)
    {
        if(ent->d_type & DT_DIR)
        {
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0)
                continue;

            if (0 == is_date_dir_format(ent->d_name))
                continue;

            tmp = atoi(ent->d_name);
            if(tmp < min)
                min = tmp;
        }
    }
	closedir(pDir);
    if(99999999 == min)
        return 0;  //can't find dir
    sprintf(min_dir, "%s/%d", IPC_SD_VIDEO_PATH, min);
    return 1;  //find dir
}

static int del_oldest_file(char *min_dir)
{
    DIR *pDir;
    struct dirent *ent;
    long long min;
    char min_filename[128];
    char del_file[128];
    int ret = 0;

    //min = 99999999999999ull;
    min = 999999ull;
    //PRINT_INFO("before min = %lld\n", min);
    memset(min_filename, 0, sizeof(min_filename));

    pDir = opendir(min_dir);
    while((ent = readdir(pDir)) != NULL)
    {

        if(ent->d_type & DT_REG)
        {
            if (0 == is_avi_file_format(ent->d_name))
                continue;

            //PRINT_INFO("dir = %s, filename = %s\n", min_dir, ent->d_name);
            find_oldest_file(ent->d_name, &min, min_filename);
        }
    }
	closedir(pDir);
    //PRINT_INFO("find result: min = %lld, file name = %s\n", min, min_filename);
    //if(min != 99999999999999ull)
    if(min != 999999ull)
    {
        //PRINT_INFO("find !!!!!!!!!!!\n");
        memset(del_file, 0, sizeof(del_file));
        sprintf(del_file, "%s/%s", min_dir, min_filename);
        PRINT_INFO("sd card doesn't have enough space, delete file: %s\n", del_file);
        ret = remove(del_file);
        if(ret != 0)
        {
            perror("remove error : ");
            return -1;
        }
    }
    else
    {
        PRINT_INFO("not find !!!!!!\n");
        PRINT_INFO("del dir = %s\n", min_dir);

        char cmd[128];
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "rm -rf %s", min_dir);
        IPC_MID_UTIL_SysCall(cmd, NULL, 0);
    }
    return 0;
}

int IPC_MID_RECMNG_DelOldestDir(void)
{
    char min_dir[128];
    int ret;
    memset(min_dir, 0, sizeof(min_dir));
    ret = find_oldest_dir(min_dir);
    if(ret == 0)
    {
        PRINT_INFO("no dir to del.\n");
        return 0;
    }
    del_oldest_file(min_dir);

    return 1;
}

static int sd_fileopt_getline(char *hLine, char **hStr)
{
	assert(hLine != NULL);
	assert(hStr != NULL);

	char *p = NULL;
	p = *hStr;

	while(*p == ' ' || *p == '\t' || *p == '\n')
	{
		p++;
	}

	if(*p == '\0')
	{
		return -1;
	}
	while(((*p) != '\0') && ((*p) != '\n') && ((*p) != '\r'))
	{
		*hLine++ = *p++;
	}

	*hLine = '\0';
	*hStr = p;

	return 0;
}

#if 0
IPC_SD_FSTYPE sd_disk_get_partition_format_type(char *hPartitionPathName, IPC_SD_DISKTYPE diskType)
{
	assert(hPartitionPathName != NULL);

	int ret = 0;
	char cmd[128];
	char message[128];
	char diskPathName[32];

	memset(cmd, 0, sizeof(cmd));
	memset(message, 0, sizeof(message));
	memset(diskPathName, 0, sizeof(diskPathName));

	if(diskType == IPC_SD_DISKTYPE_MMC)		//如果设备是MMC/SD卡
	{
		strncpy(diskPathName, hPartitionPathName, strlen(s_achSDPathName));
        diskPathName[strlen(s_achSDPathName)] = '\0';
	}
	else	//如果为SDA硬盘
	{
		strncpy(diskPathName, hPartitionPathName, strlen("/dev/sda"));
        diskPathName[strlen("/dev/sda")] = '\0';
	}

	PRINT_INFO("pathName: %s, devPathName: %s\n", hPartitionPathName, diskPathName);

	snprintf(cmd, sizeof(cmd), "fdisk -l %s > /tmp/partitionType", diskPathName);
	system(cmd);

	int fd = 0;
	fd = open("/tmp/partitionType", O_RDONLY);
	if(fd < 0)
	{
		PRINT_INFO("open </tmp/partitionType> fail\n");
		system("rm -rf /tmp/partitionType");
		return -1;
	}

	char *buffer = NULL;
	buffer = (char *)malloc(1024 * 8);
	if(buffer == NULL)
	{
		PRINT_INFO("malloc buffer fail\n");
		close(fd);
		system("rm -rf /tmp/partitionType");
		return -1;
	}

	memset(buffer, 0, 1024 * 8);

	ret = read(fd, buffer, 1024 * 8);
	if(ret <= 0)
	{
		PRINT_INFO("read fail\n");
		free(buffer);
		buffer = NULL;
		close(fd);
		system("rm -rf /tmp/partitionType");
		return -1;
	}

	close(fd);
	system("rm -rf /tmp/partitionType");


	/*parse*/
	char line[128];
	char *str = NULL;

	memset(line, 0, sizeof(line));

	str = buffer;

	while(sd_fileopt_getline(line, &str) == 0)
	{
		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "FAT32") != NULL))
		{
			PRINT_INFO("partition format type is FAT32, need not to be formated.\n");
			free(buffer);
			return IPC_SD_FSTYPE_FAT32;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "NTFS") != NULL))
		{
			PRINT_INFO("partition format type is NTFS.\n");
			free(buffer);
			return IPC_SD_FSTYPE_NTFS;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "PartitionMagic") != NULL))
		{
			PRINT_INFO("partition format type is PartitionMagic.\n");
			free(buffer);
			return IPC_SD_FSTYPE_REISERFS;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "Extended") != NULL))
		{
			PRINT_INFO("partition format type is Extended.\n");
			free(buffer);
			return IPC_SD_FSTYPE_EXTENDED;
		}
	}

	PRINT_INFO("partition format type unknown\n");
	free(buffer);
	buffer = NULL;

	return IPC_SD_FSTYPE_UNKNOWN;
}
#else
//exaple" sd_disk_get_partition_format_type(s_achSDPartitionPathName, IPC_SD_DISKTYPE_MMC);
IPC_SD_FSTYPE sd_disk_get_partition_format_type(char *hPartitionPathName, IPC_SD_DISKTYPE diskType)
{
	assert(hPartitionPathName != NULL);

	//int ret = 0;
	char cmd[128] = {0};
	char message[128] = {0};
	char diskPathName[32] = {0};

	memset(cmd, 0, sizeof(cmd));
	memset(message, 0, sizeof(message));
	memset(diskPathName, 0, sizeof(diskPathName));

	if(diskType == IPC_SD_DISKTYPE_MMC)
	{
		strncpy(diskPathName, hPartitionPathName, strlen(s_achSDPathName));
        diskPathName[strlen(s_achSDPathName)] = '\0';
	}
	else
	{
		strncpy(diskPathName, hPartitionPathName, strlen("/dev/sda"));
        diskPathName[strlen("/dev/sda")] = '\0';
	}

	PRINT_INFO("pathName: %s, devPathName: %s\n", hPartitionPathName, diskPathName);

	snprintf(cmd, sizeof(cmd), "fdisk -l %s", diskPathName);
    char buff[8192] = {0};
    memset(buff, 0, sizeof(buff));
    IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));


	/*parse*/
	char line[128] = {0};
	char *str = NULL;

	memset(line, 0, sizeof(line));
	str = buff;

	while(sd_fileopt_getline(line, &str) == 0)
	{
		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "FAT32") != NULL))
		{
			PRINT_INFO("partition format type is FAT32, need not to be formated.\n");
			return IPC_SD_FSTYPE_FAT32;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "NTFS") != NULL))
		{
			PRINT_INFO("partition format type is NTFS.\n");
			return IPC_SD_FSTYPE_NTFS;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "PartitionMagic") != NULL))
		{
			PRINT_INFO("partition format type is PartitionMagic.\n");
			return IPC_SD_FSTYPE_REISERFS;
		}

		if((strstr(line, hPartitionPathName) != NULL) && (strstr(line, "Extended") != NULL))
		{
			PRINT_INFO("partition format type is Extended.\n");
			return IPC_SD_FSTYPE_EXTENDED;
		}

        memset(line, 0, sizeof(line));
	}

	PRINT_INFO("partition format type unknown\n");

	return IPC_SD_FSTYPE_UNKNOWN;
}
#endif

IPC_SD_FSTYPE mmc_get_fs_type()
{
    if ((strlen(s_achSDPathName) == 0) || (strlen(s_achSDPartitionPathName) == 0)) {
        PRINT_ERR("s_achSDPathName is null\n");
        return IPC_SD_FSTYPE_UNKNOWN;
    }

    char cmd[128] = {0};
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "fdisk -l %s", s_achSDPathName);

    char buff[8192] = {0};
    memset(buff, 0, sizeof(buff));
    IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));


	/*parse*/
	char line[128] = {0};
	char *str = NULL;

	memset(line, 0, sizeof(line));
	str = buff;

	while(sd_fileopt_getline(line, &str) == 0)
	{
		if((strstr(line, s_achSDPartitionPathName) != NULL) && (strstr(line, "FAT32") != NULL))
		{
			PRINT_INFO("partition format type is FAT32, need not to be formated.\n");
			return IPC_SD_FSTYPE_FAT32;
		}

		if((strstr(line, s_achSDPartitionPathName) != NULL) && (strstr(line, "NTFS") != NULL))
		{
			PRINT_INFO("partition format type is NTFS.\n");
			return IPC_SD_FSTYPE_NTFS;
		}

		if((strstr(line, s_achSDPartitionPathName) != NULL) && (strstr(line, "PartitionMagic") != NULL))
		{
			PRINT_INFO("partition format type is PartitionMagic.\n");
			return IPC_SD_FSTYPE_REISERFS;
		}

		if((strstr(line, s_achSDPartitionPathName) != NULL) && (strstr(line, "Extended") != NULL))
		{
			PRINT_INFO("partition format type is Extended.\n");
			return IPC_SD_FSTYPE_EXTENDED;
		}

        memset(line, 0, sizeof(line));
	}

	PRINT_INFO("partition format type unknown\n");

	return IPC_SD_FSTYPE_UNKNOWN;
}

#if 0
static int sd_is_file_exist(char *file_path_name)
{
    // 刷新设备文件系统
    //system("mdev -s");
    //sleep(1);

    char cmd[256];
	memset(cmd, 0, sizeof(cmd));

	snprintf(cmd, sizeof(cmd), "ls -l %s 2>/dev/null | wc -l > /tmp/ls_count", file_path_name);

	system(cmd);

    FILE *fp = NULL;
    char line[10];
    fp = fopen("/tmp/ls_count", "r");
    if(fp == NULL)
    {
        PRINT_INFO("open mmc file fail\n");
        system("rm -rf /tmp/ls_count"); //删除临时文件
        return -1;
    }

    memset(line, 0, sizeof(line));
    if(fgets(line, sizeof(line), fp) == NULL)
    {
        PRINT_INFO("fgets mmc file fail\n");
        fclose(fp);
        system("rm -rf /tmp/ls_count"); //删除临时文件
    }

    fclose(fp);
    system("rm -rf /tmp/ls_count"); //删除临时文件

    if(0 == strncmp(line, "0", 1))
    {
        PRINT_ERR("%s not exist\n", file_path_name);
        return 0;
    }
    else if(0 == strncmp(line, "1", 1))
    {
        //PRINT_INFO("%s exist\n", file_path_name);
        return 1;
    }
    else
    {
        PRINT_INFO("check %s exist error\n", file_path_name);
        return -1;
    }
}
#else
static int sd_is_file_exist(char *file_path_name)
{
    //system("mdev -s");
    //sleep(1);

    char cmd[256] = {0};
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, sizeof(cmd), "ls -l %s| wc -l", file_path_name);

    char buff[128] = {0};
    memset(buff, 0, sizeof(buff));
    int ret = IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
    if (ret != 0) {
        PRINT_ERR();
        return -1;
    }
    int tmp = atoi(buff);

    if (tmp == 0) {
        PRINT_ERR("%s not exist\n", file_path_name);
        return 0;
    }
    else if(tmp == 1)
    {
        //PRINT_INFO("%s exist\n", file_path_name);
        return 1;
    }
    else
    {
        PRINT_INFO("check %s exist error\n", file_path_name);
        return -1;
    }
}

#endif
int IPC_MID_RECMNG_IsSDExist(void)
{
    int ret;
    ret = sd_is_file_exist(IPC_SD_PATHNAME);
    return ret;
}

#if 0
int IPC_MID_RECMNG_SetSDName(void)
{
    // 刷新设备文件系统
    //system("mdev -s");
    //sleep(1);

    char cmd[256];
	memset(cmd, 0, sizeof(cmd));

    //	ls /dev/mmcblk[0-9] > /tmp/mmc_device_name
	snprintf(cmd, sizeof(cmd), "ls %s 2>/dev/null > /tmp/mmc_device_name", IPC_SD_PATHNAME);
	system(cmd);

    // ls /dev/mmcblk[0-9]p[0-9] >> /tmp/mmc_device_name
	snprintf(cmd, sizeof(cmd), "ls %s 2>/dev/null >> /tmp/mmc_device_name", IPC_SD_PARTITION_PATHNAME);
	system(cmd);

    FILE *fp = NULL;
    char line[1024];
    fp = fopen("/tmp/mmc_device_name", "r");
    if(fp == NULL)
    {
        PRINT_ERR("open mmc file fail\n");
        system("rm -rf /tmp/mmc_device_name"); //删除临时文件
        return -1;
    }

    memset(s_achSDPathName, 0, sizeof(s_achSDPathName));
    memset(s_achSDPartitionPathName, 0, sizeof(s_achSDPartitionPathName));

    memset(line, 0, sizeof(line));
    fgets(line, sizeof(line), fp);
    strncpy(s_achSDPathName, line, strlen(IPC_SD_PATHNAME_0));

    memset(line, 0, sizeof(line));
    fgets(line, sizeof(line), fp);
    strncpy(s_achSDPartitionPathName, line, strlen(IPC_SD_PARTITION_PATHNAME_0));

    fclose(fp);
    system("rm -rf /tmp/mmc_device_name"); //删除临时文件

    return 0;
}
#else
int IPC_MID_RECMNG_SetSDName(void)
{
    //system("mdev -s");
    //sleep(1);

    char cmd[256];
	memset(cmd, 0, sizeof(cmd));
    char buff[128] = {0};

    //	ls /dev/mmcblk[0-9] > /tmp/mmc_device_name
	snprintf(cmd, sizeof(cmd), "ls %s", IPC_SD_PATHNAME);
    IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
    memset(s_achSDPathName, 0, sizeof(s_achSDPathName));
    strncpy(s_achSDPathName, buff, strlen(IPC_SD_PATHNAME_0));

	snprintf(cmd, sizeof(cmd), "ls %s", IPC_SD_PARTITION_PATHNAME);
    IPC_MID_UTIL_SysCall(cmd, buff, sizeof(buff));
    memset(s_achSDPartitionPathName, 0, sizeof(s_achSDPartitionPathName));
    strncpy(s_achSDPartitionPathName, buff, strlen(IPC_SD_PARTITION_PATHNAME_0));

    PRINT_INFO("SD: %s - %s\n", s_achSDPathName, s_achSDPartitionPathName);

    return 0;
}
#endif

static int sd_is_file_exist_ext(char *file_path_name)
{

    int i = 0;
    char cmd[256];
	memset(cmd, 0, sizeof(cmd));

    for(i = 1; i < 10;i++)
    {
        sprintf(cmd,"%s%d",file_path_name,i);
        if ( access(cmd,F_OK) == 0)
        {
            return i;
        }
    }
    return -1;
}

int sd_check_device_node(void)
{
    //system("mdev -s");
    //sleep(1);
	int ret = 0;
    char cmd[256];
	memset(cmd, 0, sizeof(cmd));
	int blki = 0;
    int blkp = 0;

    if (access(s_achSDPathName, F_OK) == 0)
    {
        //PRINT_INFO("s_achSDPartitionPathName:%s exist\n", s_achSDPartitionPathName);
        return 0;
    }

    blki = sd_is_file_exist_ext(IPC_SD_PATHNAME_HEADER);
    if(blki == -1)
    {
        return -1;
    }
	#if 0
    snprintf(cmd, sizeof(cmd), "%s%dp", IPC_SD_PATHNAME_HEADER,blki);
    blkp = sd_is_file_exist_ext(cmd);
    if(blkp == -1)
    {
        blkp = 1;
        ret = 0;
    }

    snprintf(s_achSDPartitionPathName,sizeof(s_achSDPartitionPathName),"%s%dp%d",IPC_SD_PATHNAME_HEADER,blki,blkp);

    PRINT_INFO("s_achSDPathName:%s", s_achSDPathName);
    PRINT_INFO("s_achSDPartitionPathName:%s\n", s_achSDPartitionPathName);
    #endif	
    //snprintf(s_achSDPathName,sizeof(s_achSDPathName),"%s%d",IPC_SD_PATHNAME_HEADER,blki);
    IPC_MID_RECMNG_SetSDName();
    return 0;
}


int IPC_MID_RECMNG_InitSD(void)
{
    int ret = 0;
    #if 1
	ret = sd_check_device_node();
	if(ret < 0)
	{
		if(IPC_MID_RECMNG_IsSDMount() == 0)
		{
		   IPC_MID_RECMNG_UmountSD();
		}
		//PRINT_ERR("%s error %d\n", __func__, __LINE__);
		return -1;
	}

    return 0;
    #else

    if(sd_is_formated)
        return 0;

	int ret = 0;
	//IPC_SD_DISKTYPE diskType = IPC_SD_DISKTYPE_MMC;
	//int formatType = IPC_SD_FSTYPE_FAT32;

	/*检测/dev/mmcblk[0-9]是否存在*/
	if(IPC_MID_RECMNG_IsSDExist() == 0)
	{
        PRINT_ERR("system has not a MMC device\n");
		return -1;
	}

	/*确定/dev/mmcblk[0-9] 是 /dev/mmcblk0 还是 /dev/mmcblk1 */
    if(IPC_MID_RECMNG_SetSDName() < 0)
        return -1;

	/*检测/dev/mmcblk0p1或/dev/mmcblk1p1是否存在*/
	if(sd_is_partition_exist() == 0)	//如果/dev/mmcblk0p1不存在
	{
		PRINT_ERR("system no < %s >, goto partiton first\n", s_achSDPartitionPathName);

		ret = gk_sd_format();
		if(ret != 0)
		{
			PRINT_ERR("IPC_MID_RECMNG_FormatSD < %s > fail\n", s_achSDPathName);
			return -1;
		}
	}
	/*检测/dev/mmcblk0p1分区类型*/
	ret = sd_disk_get_partition_format_type(s_achSDPartitionPathName, IPC_SD_DISKTYPE_MMC);
	if(ret != IPC_SD_FSTYPE_FAT32)
	{
		PRINT_ERR("Check Primary </dev/mmcblk0p1> not FAT32, format partition first\n");
		ret = gk_sd_format();
		if(ret != 0)
		{
			PRINT_ERR("IPC_MID_RECMNG_FormatSD < %s > fail\n", s_achSDPathName);
			return -1;
		}
	}

    #if 0
    //PRINT_INFO("MMC device is valid, mount\n");
	ret = IPC_MID_RECMNG_UmountSD();	//卸载MMC/SD设备
	ret = IPC_MID_RECMNG_MountSD();		//挂载MMC/SD设备
	if(ret != 0)
	{
		PRINT_INFO("mount sd card failed.\n");
		return -1;
	}
    #endif
	//PRINT_INFO("init sd card success.\n");
	sd_is_formated = 1;

    //fix_all_avi_file();

	return 0;
    #endif
}

static int is_format_running = 0;
int format_process = 0;

static void * IPC_MID_RECMNG_SDFormatThread(void *param)
{
    prctl(PR_SET_NAME, "SdThread", 0, 0, 0);

    if (is_format_running)
        return NULL;

    IPC_MID_RECMNG_StopRecAll();
    sleep(1);

    is_format_running = 1;
    format_process = 10;

    if(IPC_MID_RECMNG_SetSDName() < 0)
    {
        PRINT_ERR();
        return NULL;
    }

    sd_fdisk_and_mkdosfs();
    format_process = 20;

    int is_sd_mount = IPC_MID_RECMNG_IsSDMount();
    if (is_sd_mount)
    {
        IPC_MID_RECMNG_UmountSD();
    }
    format_process = 30;

    char cmd[64];
    memset(cmd, 0, sizeof(cmd));
    //mkdosfs -v -F32 -s32 /dev/mmcblk0p1
    snprintf(cmd, sizeof(cmd), "mkdosfs -v -F32 -s32 %s", s_achSDPartitionPathName);
    IPC_MID_UTIL_SysCall(cmd, NULL, 0);

    format_process = 60;

    //system("mdev -s");
    //IPC_MID_UTIL_Sleep(5 * 1000);
    sleep(2);
    IPC_MID_UTIL_SysCall("sync", NULL, 0);
    IPC_MID_RECMNG_MountSD();
    format_process = 80;
    IPC_MID_UTIL_SysCall("df -h", NULL, 0);

    format_process = 100;
    is_format_running = 0;

    IPC_MID_RECMNG_StartRecAll();

    return NULL;
}

int IPC_MID_RECMNG_GetFormatProgress()
{
    return format_process;
}

int IPC_MID_RECMNG_FormatSD(void)
{
    IPC_MID_UTIL_CreateDetachThread(IPC_MID_RECMNG_SDFormatThread, NULL, NULL);

    return 0;
    #if 0
    void *para = NULL;
    pthread_t *pid = NULL;

    pthread_t ThreadId;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);//绑定
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//分离
    if(pthread_create(&ThreadId, &attr, GkSdFormatThread, para) == 0) { //创建线程
        pthread_attr_destroy(&attr);
		if(pid)
			*pid = ThreadId;
		return 0;
    }
    pthread_attr_destroy(&attr);
    return 0;
    #endif
}

static unsigned int g_sd_status = 0;
static pthread_mutex_t g_sd_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static int REC_LIMIT_SIZE = 10;
static int sd_check_init = 0;

void IPC_MID_RECMNG_UpdateSDStatus()
{
    int ret = 0;
    ret = IPC_MID_RECMNG_InitSD();
    if(ret < 0)
    {
    	return -1;
    }

    ret = IPC_MID_RECMNG_IsSDMount();
    if(ret < 0)
    {
        ret = IPC_MID_RECMNG_MountSD();
        if(ret < 0)
        {
        	PRINT_ERR("%s mount sdcard failed %d", __func__, __LINE__);
			return -1;
        }
    }
    if (ret == 0) {
        IPC_MID_UTIL_SetBit(&g_sd_status, SD_STATUS_IS_INSERT_OFFSET, &g_sd_status_mutex);

        int free_size = IPC_MID_RECMNG_GetSDFreeSize();
        if (free_size < REC_LIMIT_SIZE) {
            IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_WRITABLE_OFFSET, &g_sd_status_mutex);
            IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_FREE_OFFSET, &g_sd_status_mutex);
            IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_REC_OFFSET, &g_sd_status_mutex);
            IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_SNAP_OFFSET, &g_sd_status_mutex);
        } else {
            IPC_MID_UTIL_SetBit(&g_sd_status, SD_STATUS_IS_FREE_OFFSET, &g_sd_status_mutex);

            if (IPC_MID_RECMNG_CheckSDRW() == 1) { //rw
                IPC_MID_UTIL_SetBit(&g_sd_status, SD_STATUS_IS_WRITABLE_OFFSET, &g_sd_status_mutex);
            } else { //ro
                PRINT_ERR("sd insert, but read only\n");
                IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_WRITABLE_OFFSET, &g_sd_status_mutex);
            }
        }
        //printf("sd is mount, status=%d\n", g_sd_status);

    } else {

    IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_INSERT_OFFSET, &g_sd_status_mutex);
    IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_WRITABLE_OFFSET, &g_sd_status_mutex);
    IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_FREE_OFFSET, &g_sd_status_mutex);
    IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_REC_OFFSET, &g_sd_status_mutex);
    IPC_MID_UTIL_ClearBit(&g_sd_status, SD_STATUS_IS_SNAP_OFFSET, &g_sd_status_mutex);
    //printf("sd is umount, status=%d\n", g_sd_status);
    }

    sd_check_init = 1;
}

AR_S32 IPC_MID_RECMNG_GetSDStatus()
{
    if (sd_check_init == 0)
        IPC_MID_RECMNG_UpdateSDStatus();

    int tmp;
    pthread_mutex_lock(&g_sd_status_mutex);
    tmp = g_sd_status;
    pthread_mutex_unlock(&g_sd_status_mutex);
    return tmp;
}

/* 1:yes, 0:no */
AR_S32 IPC_MID_RECMNG_SDStatusIs(E_SD_STATUS_OFFSET offset)
{
    return IPC_MID_UTIL_CheckBit(g_sd_status, offset, &g_sd_status_mutex);
}

void IPC_MID_RECMNG_SDStatusSet(E_SD_STATUS_OFFSET offset)
{
    IPC_MID_UTIL_SetBit(&g_sd_status, offset, &g_sd_status_mutex);
}

void IPC_MID_RECMNG_SDStatusClr(E_SD_STATUS_OFFSET offset)
{
    IPC_MID_UTIL_ClearBit(&g_sd_status, offset, &g_sd_status_mutex);
}


