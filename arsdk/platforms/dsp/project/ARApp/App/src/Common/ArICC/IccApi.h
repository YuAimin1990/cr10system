#ifndef __ICC_API_H__
#define __ICC_API_H__
#ifdef __cplusplus
extern "C"{
#endif
//#include "icc_port.h"

enum
{
	ICC_CORE_A_0,
	ICC_CORE_A_1,
	ICC_CORE_A_2,
	ICC_CORE_A_3,

	ICC_CORE_M_0,
	ICC_CORE_M_1,
	ICC_CORE_M_2,
	ICC_CORE_M_3,

	ICC_CORE_CV_0,
	ICC_CORE_CV_1,
	ICC_CORE_CV_2,
	ICC_CORE_CV_3,

	ICC_CORE_TOTAL
};

int xArIccInit(unsigned char * shm, unsigned int shm_len);
unsigned short xArIccFtok(const char *path);
void pArIccDumpTokenId(void);
void *pvArIccClientCreate(void);
void xArIccClientDestroy(void *client);
int xArIccRegisterMsgid(void *client, unsigned int msg_id);
int xArIccUnregisterMsgid(void *client, unsigned int msg_id);
int xArIccSend(void *client, unsigned char *buf, unsigned int size, unsigned int msg_id, unsigned int core_id);
int xArIccReceive(void *client, unsigned char *buf, unsigned int size, unsigned int msg_id);

void vIccIsr(void);
unsigned int xArIccIsrCnt(void);
#ifdef __cplusplus
}
#endif
#endif
