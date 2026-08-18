#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SystemCommon.h"
#include "IccApi.h"
#include "Uart.h"
#include "IccPort.h"
#include "IccFileOperation.h"

#define DSP_FS_NAME_LEN (256)

#define DSP_FS_HAS_INIT (0x21)
#define DSP_FS_NOT_INIT (0x0)
static int data_fs_init_flag = DSP_FS_NOT_INIT;

void *fs_icc_client = NULL;
static char *pub_file_name = NULL;


typedef enum{
	INIT = 0,
	OPEN_FILE,
	GET_FILE_LEN,
	READ_FILE,
	WRITE_FILE,
	CLOSE_FILE,

}dsp_proxy_type_e;


static int xArProxyIccSend(uint32_t * msg)
{
	int ret = 0;
	if(DSP_FS_HAS_INIT != data_fs_init_flag)
	{
		xDebugPrint("[ERROR] please do fs init at first!\r\n");
		return -1;
	}
	msg[1] = xIccGetCoreId();
	ret = xArIccSend(fs_icc_client, msg,
				24,
				MSG_CEVA_WITH_LINUX_FOPS_0 + xGetSelfCoreId(),
				ICC_CORE_A_0);
	if (ret < 0)
	{
	    xDebugPrint("xArProxyIccSend error\r\n");
	}

	return ret;
}

static int xArProxyIccRecv(uint32_t * msg)
{
	int32_t ret = -1;
	if(DSP_FS_HAS_INIT != data_fs_init_flag)
	{
		xDebugPrint("[ERROR] please do fs init at first!\r\n");
		return -1;
	}
	ret = xArIccReceive(fs_icc_client, msg, 24, MSG_CEVA_WITH_LINUX_FOPS_0 + xGetSelfCoreId());
	if (ret < 0)
	{
		xDebugPrint("xArProxyIccRecv Timeout\r\n");
	}
	return ret;
}


int xArFsInit()
{
	int ret = -1;
	unsigned int msg[6] = {0};
	if(DSP_FS_NOT_INIT != data_fs_init_flag)
	{
		xDebugPrint("[ERROR] DON'T DO FS INIT AGAIN!\r\n");
		return -1;
	}
	data_fs_init_flag = DSP_FS_HAS_INIT;
	
    fs_icc_client = pvArIccClientCreate();
    AR_ASSERT(NULL != fs_icc_client);
    ret = xArIccRegisterMsgid(fs_icc_client, MSG_CEVA_WITH_LINUX_FOPS_0 + xGetSelfCoreId());
    AR_ASSERT(0 == ret);
	pub_file_name = malloc(DSP_FS_NAME_LEN);

	return 0;
}

int xArOpen(char * file_name,int flag)
{
	unsigned long msg[6] = {0};
	int fd = -1;
	int ret = 0;

	msg[0] = OPEN_FILE;
	memset(pub_file_name, 0x0, DSP_FS_NAME_LEN);
	memcpy(pub_file_name, file_name, strlen(file_name));
	msg[2] = (unsigned long)pub_file_name;
	msg[3] = flag;
	ret = xArProxyIccSend(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc send error, ret %d\r\n", ret);
		return ret;
	}

	ret = xArProxyIccRecv(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc recv error, ret %d\r\n", ret);
		return ret;
	}

	if(0 != msg[0])
	{
		fd = msg[2];
	}
	else
	{
		fd = msg[0];
	}
	return fd;
}

int xArRead(int fd, char *buf, int * read_size)
{
	unsigned long msg[6] = {0};
	int ret = -1;

	memset(buf, 0x0, *read_size);
	msg[0] = READ_FILE;
	msg[2] = fd;
	msg[3] = *read_size;
	msg[4] = buf;
	ret = xArProxyIccSend(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc send error, ret %d\r\n", ret);
		return ret;
	}

	ret = xArProxyIccRecv(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc recv error, ret %d\r\n", ret);
		return ret;
	}
	if(0 != msg[0])
	{
		*read_size = msg[2];
		ret = msg[0];
	}
	else
	{
		ret = msg[0];
	}
	return ret;
}

int xArGetFileLen(int fd)
{
	unsigned long msg[6] = {0};
	int ret = -1;
	msg[0] = GET_FILE_LEN;
	msg[2] = fd;

	ret = xArProxyIccSend(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc send error, ret %d\r\n", ret);
		return ret;
	}

	ret = xArProxyIccRecv(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc recv error, ret %d\r\n", ret);
		return ret;
	}

	if(0 != msg[0])
	{
		ret = msg[2];

	}
	else
	{
		ret = msg[0];
	}

	return ret;
}

int xArWrite(int fd, char *buf, int *write_size)
{
	unsigned long msg[6] = {0};
	int ret = -1;

	msg[0] = WRITE_FILE;
	msg[2] = fd;
	msg[3] = *write_size;
	msg[4] = buf;
	ret = xArProxyIccSend(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc send error, ret %d\r\n", ret);
		return ret;
	}

	ret = xArProxyIccRecv(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc recv error, ret %d\r\n", ret);
		return ret;
	}

	if(0 != msg[0])
	{
		*write_size = msg[2];
		ret = msg[0];
	}
	else
	{
		ret = msg[0];
	}
	return ret;
}

int xArClose(int fd)
{
	unsigned long msg[6] = {0};
	char * read_buf = NULL;
	int ret = -1;

	msg[0] = CLOSE_FILE;
	msg[2] = fd;

	ret = xArProxyIccSend(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc send error, ret %d\r\n", ret);
		return ret;
	}

	//debug_print("after send\r\n");
	ret = xArProxyIccRecv(msg);
	if(ret < 0)
	{
		xDebugPrint("Icc recv error, ret %d\r\n", ret);
		return ret;
	}

	if(0 != msg[0])
	{
		ret = msg[2];
	}
	else
	{
		ret = msg[0];
	}
	return ret;
}


void vWaitLinuxFsReady()
{
    int ret = 0;
    unsigned long msg[6] = {0};

	while(1)
	{
		ret = xArProxyIccRecv(msg);
		if(ret < 0)
		{
			xDebugPrint("Waitting Linux Fs Ready, ret %d\r\n", ret);
			continue;
		}
		if(msg[0]=='R')
		{
			break;
		}
	}
	return;
}

// just a test for reference
int xArFileOpsTest(char * path_name)
{
	int ret = 0;
	int len = 32;
	if(!path_name)
	{
		xDebugPrint("Null file name .\r\n");
		return -1;
	}

	vWaitLinuxFsReady();

	xDebugPrint("===Open file %s ===\r\n", path_name);
	int fd=xArOpen(path_name, O_CREAT|O_RDWR);
	if(fd < 0)
	{
		xDebugPrint("Open file %s error!\r\n", path_name);
		return -1;
	}

	char *buf = (char *)malloc(64);
	if(!buf)
	{
		xDebugPrint("malloc error!\r\n");
		return -1;
	}
	memset(buf, 0, 64);
	sprintf(buf, "%s %d\r\n", "Hello from dsp", xGetSelfCoreId());


	xDebugPrint("===Write file %s ===\r\n", path_name);
	ret=xArWrite(fd,buf,&len);
	if(ret < 0)
	{
		xDebugPrint("Write file %s error %d\r\n", path_name, ret);
		free(buf);
		return ret;
	}

	xDebugPrint("===Close file %s ===\r\n", path_name);
	ret=xArClose(fd);
	if(ret < 0)
	{
		xDebugPrint("Read file %s error %d\r\n", path_name, ret);
		free(buf);
		return ret;
	}

	vBlockInstructuon(500000);

	memset(buf, 0, 64);
	xDebugPrint("===Open file %s again===\r\n", path_name);
	fd=xArOpen(path_name, O_CREAT|O_RDWR);
	if(fd < 0)
	{
		xDebugPrint("Open file %s error!\r\n", path_name);
		free(buf);
		return -1;
	}
	xDebugPrint("===Read file %s ===\r\n", path_name);
	ret=xArRead(fd,buf,&len);
	if(ret < 0)
	{
		xDebugPrint("Read file %s error %d\r\n", path_name, ret);
		free(buf);
		return ret;
	}
	xDebugPrint("Read file %s : %s \r\n", path_name, buf);

	xDebugPrint("===Close file %s again===\r\n", path_name);
	ret=xArClose(fd);
	if(ret < 0)
	{
		xDebugPrint("Read file %s error %d\r\n", path_name, ret);
		free(buf);
		return ret;
	}
	vBlockInstructuon(500000);
	free(buf);
}
