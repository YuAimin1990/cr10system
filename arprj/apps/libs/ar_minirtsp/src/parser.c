
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "rtspServ.h"
#include "utils.h"
#include "handleMethod.h"
#include "parser.h"

#define SPACE_CHARS " \t\r\n"

static void get_word_until_chars(char *buf, int buf_size,
		char *sep, char **pp)
{
	char *p;
	char *q;

	p = *pp;
	p += strspn(p, SPACE_CHARS);
	q = buf;
	while (!strchr(sep, *p) && *p != '\0') {
		if ((q - buf) < buf_size - 1)
			*q++ = *p;
		p++;
	}
	if (buf_size > 0)
		*q = '\0';
	*pp = p;
}

static void get_word_sep(char *buf, int buf_size, char *sep,
		char **pp)
{
	if (**pp == '/') (*pp)++;
	get_word_until_chars(buf, buf_size, sep, pp);
}

static void get_word(char *buf, int buf_size, char **pp)
{
	get_word_until_chars(buf, buf_size, SPACE_CHARS, pp);
}
/*!
******************************************
** Function      @get_line
** Description   @copy one line data to buf if this line is not empty
** Param[in]     @buf:save data point adrees
** Param[in]     @buf_size:buf size
** Param[in]     @p:data buffer
** Param[out]    @buf:data of one line,end of \n
**
** Return        @ 0
**- #
**- #
** Others        @
******************************************
*/
static int get_line(char *buf, int buf_size, char **pp)
{
	int len;
	char *p = *pp;
	char *p1, *p2;

	if (!buf || (buf_size <= 0) || !pp)
		return -EINVAL;

	if (buf_size == 1) {
		buf[0] = '\0';
		return 0;
	}

	p1 = strchr(p, '\n');
	if (p1 == NULL) {
		/* line terminated by NULL */
		len = buf_size - 1;     /* lefe one byte for '\0' */
		strncpy(buf, p, len);
		buf[len] = '\0';
		*pp = p + strlen(buf);
		return 0;
	}

	switch (p1 - p) {
		case 0: /* empty line terminated by '\n' */
			buf[0] = '\0';
			*pp += 1;
			break;
		case 1:
			if (*(p1-1) == '\r') { /* empty line terminated by "\r\n" */
				buf[0] = '\0';
			} else {
				buf[0] = p[0];
				buf[1] = '\0';
			}

			*pp += 2;
			break;
		default:
			if (*(p1-1) == '\r')
				p2 = p1 - 1;
			else
				p2 = p1;

			len = min(p2 - p, buf_size - 1);    /* lefe one byte for '\0' */
			memcpy(buf, p, len);
			buf[len] = '\0';

			*pp = p1 + 1;   /* skip to next line */
	}

	return 0;
}

static int stristart(char *str, char *pfx, char **ptr)
{
	while (*pfx && toupper((unsigned)*pfx) == toupper((unsigned)*str)) {
		pfx++;
		str++;
	}
	if (!*pfx && ptr)
		*ptr = str;
	return !*pfx;
}

/**
 * Handle the RTSP methods and the change RTSP state if necessary.
 */
static int rtsp_handle_method(rtsp_session_t *sessp)
{
	if (!sessp)
		return -1;

	short old_state = sessp->state;
	short new_state = sessp->state;

	if (old_state != RTSP_STATE_INITED &&
			old_state != RTSP_STATE_READY &&
			old_state != RTSP_STATE_PLAYING &&
			old_state != RTSP_STATE_RECORDING) {
		DPRINT("Unknown RTSP server state[%d]\n", old_state);
		return -1;
	}

	switch (sessp->method_id) {
	case RTSP_METHOD_OPTIONS:
		if (rtsp_cmd_options(sessp) < 0) {
			DPRINT("Handle method [OPTIONS] failed!\n");
			return -1;
		}
		break;
	case RTSP_METHOD_DESCRIBE:
		if (rtsp_cmd_describe(sessp) < 0) {
			DPRINT("Handle method [DESCRIBE] failed!\n");
			return -1;
		}
		break;
	case RTSP_METHOD_SETUP:
		if (rtsp_cmd_setup(sessp) < 0) {
			DPRINT("Handle method [SETUP] failed!\n");
			return -1;
		}
		if (old_state == RTSP_STATE_INITED || old_state == RTSP_STATE_READY)
			new_state = RTSP_STATE_READY;
		break;
	case RTSP_METHOD_TEARDOWN:
		if (rtsp_cmd_teardown(sessp) < 0) {
			DPRINT("Handle method [TEARDOWN] failed!\n");
			return -1;
		}
		new_state = RTSP_STATE_INITED;
		break;
	case RTSP_METHOD_PLAY:
		if (old_state != RTSP_STATE_READY && old_state != RTSP_STATE_PLAYING) {
			rtsp_reply_error(sessp, 455); /* Method Not Valid in This State. */
			DPRINT("Method [PLAY] Not Valid in This State[%d].\n", old_state);
			return -1;
		}

		if (rtsp_cmd_play(sessp) < 0) {
			DPRINT("Handle method [PLAY] failed!\n");
			return -1;
		}
		new_state = RTSP_STATE_PLAYING;
		break;
	case RTSP_METHOD_PAUSE:
		if (old_state != RTSP_STATE_PLAYING   &&
				old_state != RTSP_STATE_RECORDING &&
				old_state != RTSP_STATE_READY) {
			rtsp_reply_error(sessp, 455); /* Method Not Valid in This State. */
			DPRINT("Method [PAUSE] Not Valid in This State[%d].\n", old_state);
			return -1;
		}

		if (rtsp_cmd_pause(sessp) < 0) {
			DPRINT("Handle method [PAUSE] failed!\n");
			return -1;
		}
		new_state = RTSP_STATE_READY;
		break;
	case RTSP_METHOD_GET_PARAMETER:
		if (rtsp_cmd_get_parameter(sessp) < 0) {
			DPRINT("Handle method [GET_PARAMETER] failed!\n");
			return -1;
		}
		break;
	case RTSP_METHOD_SET_PARAMETER:
		if (rtsp_cmd_set_parameter(sessp) < 0) {
			DPRINT("Handle method [SET_PARAMETER] failed!\n");
			return -1;
		}
		break;
	default:
		DPRINT("unknown methid id %d\n", sessp->method_id);
		return -1;
	}

	sessp->state = new_state;
	return 0;
}

int rtsp_get_media_type(char *req)
{
	int media_type;

	char *track = strstr(req, "track");
	if (!track)
		return -1;

	if (sscanf(track, "track%d", &media_type) != 1)
		return -1;

	if (media_type == MEDIA_TYPE_VIDEO || media_type == MEDIA_TYPE_AUDIO)
		return media_type;

	return -1;
}

static int get_sockaddr(const char *buf, struct sockaddr_storage *sock)
{
	struct addrinfo hints = { 0 }, *ai = NULL;
	hints.ai_flags = AI_NUMERICHOST;
	if (getaddrinfo(buf, NULL, &hints, &ai))
		return -1;
	memcpy(sock, ai->ai_addr, min(sizeof(*sock), ai->ai_addrlen));
	freeaddrinfo(ai);
	return 0;
}

static void rtsp_parse_range(int *min_ptr, int *max_ptr, char **pp)
{
	char *q;
	char *p;
	int v;

	q = *pp;
	q += strspn(q, SPACE_CHARS);
	v = strtol(q, &p, 10);
	if (*p == '-') {
		p++;
		*min_ptr = v;
		v = strtol(p, &p, 10);
		*max_ptr = v;
	} else {
		*min_ptr = v;
		*max_ptr = v;
	}
	*pp = p;
}

static void rtsp_parse_transport(RTSPTransportField *th, char *p)
{
	char transport_protocol[16];
	char profile[16];
	char lower_transport[16];
	char parameter[16];
	char buf[256];

	p += strspn(p, SPACE_CHARS);
	if (*p == '\0')
		return;

	get_word_sep(transport_protocol, sizeof(transport_protocol),
			"/", &p);
	if (!strcasecmp (transport_protocol, "rtp")) {
		get_word_sep(profile, sizeof(profile), "/;,", &p);
		lower_transport[0] = '\0';
		/* rtp/avp/<protocol> */
		if (*p == '/') {
			get_word_sep(lower_transport, sizeof(lower_transport),
					";,", &p);
		}
		th->transport = RTSP_TRANSPORT_RTP;
	} else if (!strcasecmp (transport_protocol, "x-pn-tng") ||
			!strcasecmp (transport_protocol, "x-real-rdt")) {
		/* x-pn-tng/<protocol> */
		get_word_sep(lower_transport, sizeof(lower_transport), "/;,", &p);
		profile[0] = '\0';
		th->transport = RTSP_TRANSPORT_RDT;
	} else if (!strcasecmp(transport_protocol, "raw")) {
		get_word_sep(profile, sizeof(profile), "/;,", &p);
		lower_transport[0] = '\0';
		/* raw/raw/<protocol> */
		if (*p == '/') {
			get_word_sep(lower_transport, sizeof(lower_transport),
					";,", &p);
		}
		th->transport = RTSP_TRANSPORT_RAW;
	}
	if (!strcasecmp(lower_transport, "TCP"))
		th->lower_transport = RTSP_LOWER_TRANSPORT_TCP;
	else if (!strcasecmp(lower_transport, "SHMEM"))
		th->lower_transport = RTSP_LOWER_TRANSPORT_SHMEM;
	else
		th->lower_transport = RTSP_LOWER_TRANSPORT_UDP;

	if (*p == ';')
		p++;
	/* get each parameter */
	while (*p != '\0' && *p != ',') {
		get_word_sep(parameter, sizeof(parameter), "=;,", &p);
		if (!strcmp(parameter, "port")) {
			if (*p == '=') {
				p++;
				rtsp_parse_range(&th->port_min, &th->port_max, &p);
			}
		} else if (!strcmp(parameter, "client_port")) {
			if (*p == '=') {
				p++;
				rtsp_parse_range(&th->client_port_min,
						&th->client_port_max, &p);
			}
		} else if (!strcmp(parameter, "server_port")) {
			if (*p == '=') {
				p++;
				rtsp_parse_range(&th->server_port_min,
						&th->server_port_max, &p);
			}
		} else if (!strcmp(parameter, "interleaved")) {
			if (*p == '=') {
				p++;
				rtsp_parse_range(&th->interleaved_min,
						&th->interleaved_max, &p);
			}
		} else if (!strcmp(parameter, "multicast")) {
			if (th->lower_transport == RTSP_LOWER_TRANSPORT_UDP)
				th->lower_transport = RTSP_LOWER_TRANSPORT_UDP_MULTICAST;
		} else if (!strcmp(parameter, "ttl")) {
			if (*p == '=') {
				char *end;
				p++;
				th->ttl = strtol(p, &end, 10);
				p = end;
			}
		} else if (!strcmp(parameter, "destination")) {
			if (*p == '=') {
				p++;
				get_word_sep(buf, sizeof(buf), ";,", &p);
				get_sockaddr(buf, &th->destination);
			}
		} else if (!strcmp(parameter, "source")) {
			if (*p == '=') {
				p++;
				get_word_sep(buf, sizeof(buf), ";,", &p);
				strncpy(th->source, buf, sizeof(th->source) - 1);
			}
		} else if (!strcmp(parameter, "mode")) {
			if (*p == '=') {
				p++;
				get_word_sep(buf, sizeof(buf), ";, ", &p);
				if (!strcmp(buf, "record") ||
						!strcmp(buf, "receive"))
					th->mode_record = 1;
			}
		} else if (!strcmp(parameter, "shm_name")) {
			if (*p == '=') {
				p++;
				get_word_sep(buf, sizeof(buf), ";, ", &p);
				strncpy(th->shm_name, buf, sizeof(th->shm_name) - 1);
			}
		} else if (!strcmp(parameter, "shm_size")) {
			if (*p == '=') {
				p++;
				get_word_sep(buf, sizeof(buf), ";, ", &p);
				th->shm_size = atoi(buf);
			}
		}

		while (*p != ';' && *p != '\0' && *p != ',')
			p++;
		if (*p == ';')
			p++;
	}
	if (*p == ',')
		p++;
}

RtspRoute *rtsp_get_route(const char *route)
{
	JListNode *n, *n_next;
    rtsp_serv_t * pstServer = ArGetRtspServer();

	pthread_mutex_lock(&pstServer->route_list_lock);
	for (n = j_list_first(pstServer->allroutes); n; n = n_next) {
		n_next = j_list_next(pstServer->allroutes, n);
		RtspRoute *r = n->data;
		if(strlen(route)== strlen(r->route))
		{
			if (!strncmp(r->route, route, strlen(r->route))) {
				pthread_mutex_unlock(&pstServer->route_list_lock);
				return r;
			}
		}
	}
	pthread_mutex_unlock(&pstServer->route_list_lock);

	return NULL;
}

static int parse_request_line(char *line, rtsp_session_t *sessp)
{
	if (!line || !sessp)
		return -1;

	char *p = line;

	char method[64];
	char url[1024];
	char version[32];

	get_word(method, sizeof(method), &p);
	get_word(url, sizeof(url), &p);
	get_word(version, sizeof(version), &p);

	if (strlen(sessp->request_uri) == 0) {
		strncpy(sessp->request_uri, url, sizeof(sessp->request_uri) - 1);
	}

	/* Parse RTSP version. */
	if (strcmp(version, RTSP_VERSION)) {
		DPRINT("Wrong RTSP version!\n");
		return -1;
	}

	/* Parse method. */
	if (!strcmp(method, "OPTIONS")) {
		sessp->method_id = RTSP_METHOD_OPTIONS;
	} else if (!strcmp(method, "DESCRIBE")) {
		sessp->method_id = RTSP_METHOD_DESCRIBE;
	} else if (!strcmp(method, "SETUP")) {
		sessp->method_id = RTSP_METHOD_SETUP;
	} else if (!strcmp(method, "TEARDOWN")) {
		sessp->method_id = RTSP_METHOD_TEARDOWN;
	} else if (!strcmp(method, "PLAY")) {
		sessp->method_id = RTSP_METHOD_PLAY;
	} else if (!strcmp(method, "PAUSE")) {
		sessp->method_id = RTSP_METHOD_PAUSE;
	} else if (!strcmp(method, "GET_PARAMETER")) {
		sessp->method_id = RTSP_METHOD_GET_PARAMETER;
	} else if (!strcmp(method, "SET_PARAMETER")) {
		sessp->method_id = RTSP_METHOD_SET_PARAMETER;
	} else {
		DPRINT("Wrong RTSP request, method '%s' not supported!\n", method);
		rtsp_reply_error(sessp, 551); /* Method not supported. */
		return -1;
	}

	/* Parse URI. */
	if (!sessp->url) {
		sessp->url = UrlParse(url);
		if (!sessp->url) {
			UrlFree(sessp->url);
			sessp->url = NULL;
			rtsp_reply_error(sessp, 404); /* not found. */
			return -1;
		}

		if (!sessp->url->path) {
			UrlFree(sessp->url);
			sessp->url = NULL;
			return -1;
		}

		RtspRoute *r = rtsp_get_route(sessp->url->path);
		if (!r) {
			DPRINT("Wrong RTSP request, path '%s' not found!\n", sessp->url->path);
			rtsp_reply_error(sessp, 404); /* not found. */
			return -1;
		}

		sessp->ops = &r->ops;

		sessp->handle = sessp->ops->open(sessp->url->path, sessp->url->query);
		if (!sessp->handle) {
			rtsp_reply_error(sessp, 500); /* Internal Server Error */
			return -1;
		}

		if (sessp->url->query) {
			char *q = sessp->url->query;
			while (q && (*q != '\0')) {
				char query[128] = {0};
				get_word_until_chars(query, sizeof(query), "&", &q);
				if (strlen(query) && !strcmp(query, "transport=multicast")) {
					sessp->transport[0].lower_transport = RTSP_LOWER_TRANSPORT_UDP_MULTICAST;
					sessp->transport[1].lower_transport = RTSP_LOWER_TRANSPORT_UDP_MULTICAST;
					sessp->multicast = 1;
				}
			}
		}
	}

	return 0;
}

static void rtsp_parse_range_clock(char *p, int64_t *start_ms, int64_t *end_ms)
{
	char buf[256];
	int64_t start_us = 0;
	int64_t end_us   = 0;

	p += strspn(p, SPACE_CHARS);
	if (!stristart(p, "clock=", &p))
		return;

	get_word_sep(buf, sizeof(buf), "-", &p);
	parse_time(&start_us, buf, 0);
	if (*p == '-') {
		p++;
		get_word_sep(buf, sizeof(buf), "-", &p);
		parse_time(&end_us, buf, 0);
	}

	*start_ms = start_us / 1000;
	*end_ms   = end_us   / 1000;
}

static int rtsp_parse_line(char *line, rtsp_session_t *sessp)
{
	if (!line || !sessp)
		return -1;

	RTSPMessageHeader *h = &sessp->message_headers;

	char *p = line;

	if (stristart(p, "CSeq:", &p)) {
		sessp->cseq = strtol(p, NULL, 10);
		h->seq = sessp->cseq;
	} else if (stristart(p, "Transport:", &p)) {
		int media_type = rtsp_get_media_type(sessp->recv_buf);
		if (media_type < 0)
			return -1;
		rtsp_parse_transport(&sessp->transport[media_type], p);
	} else if (stristart(p, "Accept:", &p)) {
		/* Do nothing. */
	} else if (stristart(p, "Require:", &p)) {
		p += strspn(p, SPACE_CHARS);
		if (stristart(p, "onvif-replay", &p))
			h->onvif_replay = 1;
	} else if (stristart(p, "Scale:", &p)) {
		char Scale[32];
		get_word(Scale, sizeof(Scale), &p);
		h->scale = atof(Scale);
	} else if (stristart(p, "Max-Interval:", &p)) {
		char Max_Interval[32];
		get_word(Max_Interval, sizeof(Max_Interval), &p);
		h->max_interval = atof(Max_Interval);
		sessp->max_interval = h->max_interval;
	} else if (stristart(p, "Range:", &p)) {
		rtsp_parse_range_clock(p, &h->range_start, &h->range_end);
	} else if (stristart(p, "Rate-Control:", &p)) {
		p += strspn(p, SPACE_CHARS);
		if (stristart(p, "yes", &p))
			h->rate_control = 1;
		else if (stristart(p, "no", &p))
			h->rate_control = 0;
	} else if (stristart(p, "Frames:", &p)) {
		p += strspn(p, SPACE_CHARS);

		if (stristart(p, "all", &p)) {
			h->frames.type = 0;
		} else if (stristart(p, "intra", &p)) {
			h->frames.type = 1;
			if (*p == '/') {
				p++;
				h->frames.interval = atoi(p);
			}
		} else if (stristart(p, "predicted", &p)) {
			h->frames.type = 2;
		} else {
			return -1;
		}
	} else if (stristart(p, "Immediate:", &p)) {
		p += strspn(p, SPACE_CHARS);

		if (stristart(p, "yes", &p))
			h->immediate = 1;
	}

	return 0;
}

#if 0
static void dump_request(const char *req)
{
	printf("\n\n\033[1;33;41m[Begin]=============== Rtsp Client Request Message ==================[Begin]\033[0m\n"
			"\033[32m%s\033[0m"
			"\033[33m[End]==================================================================[End]\033[0m\n\n\n",
			req);
}
#endif

int rtsp_parse_request(rtsp_session_t *sessp)
{
	if (!sessp)
		return -1;

#if 0
	dump_request(sessp->recv_buf);
#endif

	char *p = sessp->recv_buf;

	char line[1024];
	if (get_line(line, sizeof(line), &p) < 0)
		return -1;

	/* FIXME */
	RTSPMessageHeader *h = &sessp->message_headers;
	memset(h, 0, sizeof(RTSPMessageHeader));
	h->rate_control = 1;	/* default */
	h->scale = sessp->scale;	/* default */
	h->max_interval = sessp->max_interval;

	if (parse_request_line(line, sessp) < 0)
		return -1;

	/* Parse header lines. */
	while (*p != '\0') {
		if (get_line(line, sizeof(line), &p) < 0)
			return -1;

		if (!strlen(line))
			continue;	/* skip empty line */

		if (rtsp_parse_line(line, sessp) < 0)
			return -1;
	}

//	sessp->scale = h->scale;

	/* Handle the method. */
	if (rtsp_handle_method(sessp) < 0)
		return -1;

	return 0;
}
