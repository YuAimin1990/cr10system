#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ar_rtsp_server.h"
#include "rtspServ.h"
#include "utils.h"

#define RTSPSERV_BUILD_VERSION "v.0.2"     /* Version format: MAJOR.MINOR.BUGFIXED */
#define RTSPSERV_BUILD_DATE "2020-07-31"

/**
 * Print RTSP library version in pretty format.
 */
static void dump_version(void)
{
	printf ("\n    \033[32m**********************************************************\033[0m\n"
		"    \033[1;32m*   rtspServLib build information: [%s, %s]    *\033[0m\n"
		"    \033[32m**********************************************************\033[0m\n\n\n",
		RTSPSERV_BUILD_VERSION, RTSPSERV_BUILD_DATE);
	return;
}

AR_S32 AR_RTSP_Init(void)
{
	dump_version();

    if(ArRtspServerInitialized())
    {
        return 0;
    }

    rtsp_serv_t * pstServer = ArGetRtspServer();

	memset(pstServer, 0, sizeof(rtsp_serv_t));

	pstServer->allsessions   = j_list_alloc();
	pstServer->allmulticasts = j_list_alloc();
	pstServer->allroutes     = j_list_alloc();

	pthread_mutex_init(&pstServer->sess_list_lock, NULL);
	pthread_mutex_init(&pstServer->mcast_list_lock, NULL);
	pthread_mutex_init(&pstServer->route_list_lock, NULL);

	pstServer->rtspsock = -1;
	pstServer->httpsock = -1;

	pstServer->rtsp_port = DFL_RTSP_PORT;
	pstServer->http_port = DFL_HTTP_PORT;

	pstServer->enable_rtcp   = 1;
	pstServer->rtcp_interval = 5;	/* 5 second */

	return 0;
}

AR_S32 AR_RTSP_UnInit(void)
{
    rtsp_serv_t * pstServer = ArGetRtspServer();

	pthread_mutex_destroy(&pstServer->sess_list_lock);
	pthread_mutex_destroy(&pstServer->mcast_list_lock);
	pthread_mutex_destroy(&pstServer->route_list_lock);

	j_list_free(pstServer->allsessions);
	j_list_free(pstServer->allmulticasts);

	if (pstServer->allroutes) {
		JListNode *n;
		while ((n = j_list_first(pstServer->allroutes)) != NULL) {
			RtspRoute *r = n->data;
			j_list_remove(pstServer->allroutes, r);
			free(r);
		}

		j_list_free(pstServer->allroutes);
	}

    ArRtspServerSetUnInit();
	return 0;
}

AR_S32 AR_RTSP_AddRoute(const AR_CHAR *route, RtspOps *ops)
{
	if (!ops || !ops->read || !ops->describe)
		return -1;

    rtsp_serv_t * pstServer = ArGetRtspServer();

	RtspRoute *r = malloc(sizeof(RtspRoute));
	if (!r)
		return -1;
	memset(r, 0, sizeof(RtspRoute));

	strncpy(r->route, route, sizeof(r->route) - 1);
	memcpy(&r->ops, ops, sizeof(RtspOps));

	pthread_mutex_lock(&pstServer->route_list_lock);
	j_list_append(pstServer->allroutes, r);
	pthread_mutex_unlock(&pstServer->route_list_lock);

	return 0;
}

AR_S32 AR_RTSP_StartService(const AR_CHAR *ip, AR_S32 rtspport, AR_S32 httpport)
{
    rtsp_serv_t * pstServer = ArGetRtspServer();

	pstServer->rtsp_port = rtspport;
	pstServer->http_port = httpport;

	return rtsp_startRtspServ();
}

AR_S32 AR_RTSP_StopService(void)
{
	rtsp_stopRtspServ();
	return 0;
}

AR_S32 AR_RTSP_Setup(AR_S32 enable, AR_S32 interval_s)
{
    rtsp_serv_t * pstServer = ArGetRtspServer();

	pstServer->enable_rtcp = enable;
	pstServer->rtcp_interval = interval_s;
	return 0;
}

AR_S32 AR_RTSP_GetVersion(AR_CHAR *version, AR_CHAR *buildDate)
{
	strcpy(version, RTSPSERV_BUILD_VERSION);
	sprintf(buildDate, "%s %s", __DATE__, __TIME__);
	return 0;
}

AR_S32 AR_RTSP_StartMulticastStream(AR_CHAR *route)
{
	rtsp_session_t *sessp = rtsp_find_multicast_session(route);
	if (!sessp) {
		AR_CHAR uri[1024];
		snprintf(uri, sizeof(uri), "rtsp://127.0.0.1%s", route);

		sessp = rtsp_create_multicast_session(uri);
		if (!sessp)
			return -1;
	}

	sessp->mcast_refcount++;

	return 0;
}

AR_S32 AR_RTSP_StopMulticastStream(AR_CHAR *route)
{
	rtsp_session_t *sessp = rtsp_find_multicast_session(route);
	if (!sessp)
		return -1;

    rtsp_serv_t * pstServer = ArGetRtspServer();

	sessp->mcast_refcount--;
	if (sessp->mcast_refcount == 0) {
		j_list_remove(pstServer->allmulticasts, sessp);
		rtsp_closeRtspMcastSession(sessp);
	}

	return 0;
}
