#ifndef __ICC_PEER_H__
#define __ICC_PEER_H__

#include "IccList.h"
#include "IccPort.h"

struct icc_peer
{
	int core_id;
	int notify_addr;
	int irq;
	int (*notify)(struct icc_peer *peer);
	osMutexId_t mutex;
};

int xIccPeerInitCa(struct icc_peer *peer, int core_id);
int xIccPeerInitCm(struct icc_peer *peer, int core_id);
int xIccPeerInitCv(struct icc_peer *peer, int core_id);
#endif
