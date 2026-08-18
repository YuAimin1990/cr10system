/*
 * Artosyn icc driver, peer function
 */
#include "IccPeer.h"
#include "IccPort.h"
#include "IccShm.h"


/* Linux defined SGI */
enum
{
	IPI_WAKEUP,
	IPI_TIMER,
	IPI_RESCHEDULE,
	IPI_CALL_FUNC,
	IPI_STOP,
	IPI_WORK,
	IPI_COMPLETE,
	IPI_CPU_BACKTRACE,
};

/* Notify function */
static int xIccNotifyCa(struct icc_peer *peer)
{
	/*
	 * to A7 rtos, use the register bank#2
	 * to A7 Linux, use the register bank#0
	 * although register bank shared by cevas, not need lock when trigger intr to A7.
        * because A7 will interate each msg in irq handler
	*/
	int core_id = 0;//(peer->core_id == ICC_CORE_A_3) ? 2 : 0;

	vEnableToArmInterrupt0(core_id);
	vTriggerToArmInterrupt0(core_id);
	return 0;
}

static int xIccNotifyCm(struct icc_peer *peer)
{
	return 0;
}

static int xIccNotifyCv(struct icc_peer *peer)
{
	__asm__("trap{t0}");

	return 0;
}

/* peer init */
int xIccPeerInitCa(struct icc_peer *peer, int core_id)
{
	peer->notify = xIccNotifyCa;
	peer->core_id = core_id;
	//register_interrupt(IPI_WORK, icc_isr, NULL);

	return 0;
}

int xIccPeerInitCm(struct icc_peer *peer, int core_id)
{
	return 0;
}

int xIccPeerInitCv(struct icc_peer *peer, int core_id)
{
	peer->notify = xIccNotifyCv;
	peer->core_id = core_id;

	return 0;
}
