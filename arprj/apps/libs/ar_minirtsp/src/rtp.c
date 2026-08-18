
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/prctl.h>

#include "rtspServ.h"
#include "utils.h"
#include "rtp.h"
#include "rtcp.h"
#include "h264.h"
#include "h265.h"
#include "netio.h"
#include "shmem.h"
#include "ar_g711.h"

static ssize_t udp_send_rtcp(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type);
static void print_time(int64_t ms, char *time_str, int len)
{
	time_t second = ms / 1000;
	uint32_t millisecond = ms % 1000;

	struct tm tm;
	localtime_r(&second, &tm);

	snprintf(time_str, len, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
			tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec, millisecond);
}

static ssize_t sendRtcpPkt(rtsp_session_t *sessp, uint8_t *buf, int len, int media_type);

/**
 * Sequence number self-increased by one, in network byte order.
 */
static void increase_seq_number(unsigned short *seqp)
{
	*seqp = htons(ntohs(*seqp) + 1);
	return;
}

#define NTP_OFFSET 2208988800ULL
#define NTP_OFFSET_US (NTP_OFFSET * 1000000ULL)

int64_t av_gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

static uint64_t get_ntp_time(void)
{
	return (av_gettime() / 1000) * 1000 + NTP_OFFSET_US;
}


static int rtcp_send_sr(rtsp_session_t *sessp, rtp_stat_t *stat,
		int64_t ntp_time, int media_type)
{
	uint8_t buf[128];
	memset(buf, 0, sizeof(buf));

	rtp_hdr_t *rtp_hdrp = &sessp->rtp[media_type].rtp_hdr;

	/*
	 * buidl rtcp sender report
	 */
	rtcp_sr_t *sr = (rtcp_sr_t *)buf;
	memset(sr, 0, sizeof(rtcp_sr_t));
	sr->ch.version = RTP_VERSION;
	sr->ch.rc = 0;
	sr->ch.packet_type = RTCP_SR;
	sr->ch.length = htons((sizeof(rtcp_sr_t) + 3) / 4 - 1);	/* length in words - 1 */
	sr->ssrc = htonl(rtp_hdrp->ssrc);
	sr->si.ntp_timestamp_msw = htonl(ntp_time / 1000000);
	sr->si.ntp_timestamp_lsw = htonl(((ntp_time % 1000000) << 32) / 1000000 + 1);
	sr->si.rtp_timestamp = rtp_hdrp->ts;
	sr->si.senders_packet_count = htonl(stat->packet_count);
	sr->si.senders_octet_count = htonl(stat->octet_count);

	/*
	 * build rtcp source description
	 */
	rtcp_sdes_t *ds = (rtcp_sdes_t *)(buf + sizeof(rtcp_sr_t));
	memset(ds, 0, sizeof(rtcp_sdes_t));
	ds->ch.version = RTP_VERSION;
	ds->ch.rc = 1;
	ds->ch.packet_type = RTCP_SDES;
	ds->ch.length = htons((sizeof(rtcp_sdes_t) + strlen(RTCP_SDES_CNAME) + 3) / 4 - 1);	/* length in words - 1 */
	ds->chunk.ssrc = rtp_hdrp->ssrc;
	ds->chunk.sdes.item_type = 1;
	ds->chunk.sdes.len = strlen(RTCP_SDES_CNAME);
	memcpy(ds->chunk.sdes.content, RTCP_SDES_CNAME, ds->chunk.sdes.len);

	int total_len = (ntohs(sr->ch.length) + 1) * 4 + (ntohs(ds->ch.length) + 1) * 4;

	if (sendRtcpPkt(sessp, buf, total_len, media_type) < 0)
		return -1;

	return 0;
}

static int rtcp_send_bye(rtsp_session_t *sessp, rtp_stat_t *stat,
		int64_t ntp_time, int media_type)
{
	uint8_t buf[128];
	memset(buf, 0, sizeof(buf));

	rtp_hdr_t *rtp_hdrp = &sessp->rtp[media_type].rtp_hdr;

	/*
	 * buidl rtcp sender report
	 */
	rtcp_sr_t *sr = (rtcp_sr_t *)buf;
	memset(sr, 0, sizeof(rtcp_sr_t));
	sr->ch.version = RTP_VERSION;
	sr->ch.rc = 0;
	sr->ch.packet_type = RTCP_SR;
	sr->ch.length = htons((sizeof(rtcp_sr_t) + 3) / 4 - 1);	/* length in words - 1 */
	sr->ssrc = rtp_hdrp->ssrc;
	sr->si.ntp_timestamp_msw = htonl(ntp_time / 1000000);
	sr->si.ntp_timestamp_lsw = htonl(((ntp_time % 1000000) << 32) / 1000000 + 1);
	sr->si.rtp_timestamp = rtp_hdrp->ts;
	sr->si.senders_packet_count = htonl(stat->packet_count);
	sr->si.senders_octet_count = htonl(stat->octet_count);

	rtcp_bye_t *bye = (rtcp_bye_t *)(buf + sizeof(rtcp_sr_t));
	memset(bye, 0, sizeof(rtcp_bye_t));
	bye->ch.version = RTP_VERSION;
	bye->ch.rc = 1;
	bye->ch.packet_type = RTCP_BYE;
	bye->ch.length = htons((sizeof(rtcp_bye_t) + 3) / 4 - 1);	/* length in words - 1 */
	bye->ssrc = rtp_hdrp->ssrc;

	int total_len = (ntohs(sr->ch.length) + 1) * 4 + (ntohs(bye->ch.length) + 1) * 4;
//	printf("sizeof(rtcp_sr_t) = %d, sizeof(rtcp_bye_t) = %d, total_len = %d\n", sizeof(rtcp_sr_t), sizeof(rtcp_bye_t), total_len);

	if (sendRtcpPkt(sessp, buf, total_len, media_type) < 0)
		return -1;

	return 0;
}

static int send_rtsp_interleaved_header(rtsp_session_t *sessp, int len, int channel)
{
	int n;
	uint8_t headers[4];
    unsigned int data_len = len;

	headers[0] = '$';
	headers[1] = channel;
	headers[2] = (uint8_t) ((data_len&0xFF00)>>8);
	headers[3] = (uint8_t) (data_len&0xFF);;

	n = rtsp_writen(sessp->rtsp_snd_sock, headers, 4);
	if (n < 0)
		return -1;

	return 0;
}

static int tcp_send_rtp(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	int len;
    char newbuf[4096];
	int channel = sessp->transport[media_type].interleaved_min;
	//if (send_rtsp_interleaved_header(sessp, size, channel) < 0)
	//	return -1;
	newbuf[0] = '$';
	newbuf[1] = channel;
	newbuf[2] = (uint8_t) ((size&0xFF00)>>8);
	newbuf[3] = (uint8_t) (size&0xFF);;
    memcpy(newbuf+4,buf,size);


	uint8_t *buf_ptr = (uint8_t *)newbuf;
	uint8_t *buf_end = buf_ptr + size + 4;

	while (!sessp->abort) {
		len = send(sessp->rtsp_snd_sock, buf_ptr, buf_end - buf_ptr, 0);
		if (len <= 0) {
			if (len < 0 && (errno == EINTR || errno == EAGAIN)) {
				perror("send");
				continue;
			}
			return -1;
		}

		buf_ptr += len;

		if (buf_ptr >= buf_end) {
			sessp->timeout = 0;	/* clear timeout */
			break;
		}

		if (fd_wait_writeable(sessp->rtsp_snd_sock, 5000) <= 0)
			return -1;
	}

	if (!sessp->abort) {
		rtp_stat_t *stat = &sessp->rtp[media_type].stat;
		stat->octet_count += len;
		stat->packet_count++;
	} else {
		return -1;
	}

	return 0;
}

static int udp_send_rtp(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	int len;

	while (!sessp->abort) {
		if (fd_wait_writeable(sessp->rtp[media_type].rtp_sock, 5000) <= 0)
			return -1;

		len = send(sessp->rtp[media_type].rtp_sock, buf, size, 0);
		if (len <= 0) {
			if (len < 0 && (errno == EINTR || errno == EAGAIN))
				continue;

			perror("send fail");
			return -1;
		}

		if (len == size)
			break;
	}

	if (!sessp->abort) {
		rtp_stat_t *stat = &sessp->rtp[media_type].stat;
		stat->octet_count += len;
		stat->packet_count++;
	}

	return 0;
}

static ssize_t rtp_send_packet(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	int ret = -1;

	switch (sessp->transport[media_type].lower_transport) {
	case RTSP_LOWER_TRANSPORT_TCP:
		pthread_mutex_lock(&sessp->rtsp_snd_lock);
		ret = tcp_send_rtp(sessp, buf, size, media_type);
		pthread_mutex_unlock(&sessp->rtsp_snd_lock);
		break;
	case RTSP_LOWER_TRANSPORT_UDP:
	case RTSP_LOWER_TRANSPORT_UDP_MULTICAST:
		ret = udp_send_rtp(sessp, buf, size, media_type);
		break;
	default:
		DPRINT("unknown lower transport type %d\n",
				sessp->transport[media_type].lower_transport);
		return -1;
	}

	return ret;
}
static int y_udp_send_rtp(rtsp_session_t *sessp, uint8_t *buf,
		int size, uint8_t *databuf,int dataLen,int media_type)
{
	int len = 0;
    char newBuf[4096];
    memcpy(newBuf, buf,size);
    memcpy(newBuf+size, databuf,dataLen);

    size +=dataLen;

	while (!sessp->abort) {
		if (fd_wait_writeable(sessp->rtp[media_type].rtp_sock, 5000) <= 0)
			return -1;

		len = send(sessp->rtp[media_type].rtp_sock, newBuf, size, 0);
		if (len <= 0) {
			if (len < 0 && (errno == EINTR || errno == EAGAIN))
				continue;

			perror("send fail");
			return -1;
		}

		if (len == size)
			break;
	}

	if (!sessp->abort) {
		rtp_stat_t *stat = &sessp->rtp[media_type].stat;
		stat->octet_count += len;
		stat->packet_count++;
	}

	return 0;
}


/*
    add by harric He(heyong). begin
*/
static void y_start_record_data(y_test_buf *y_buf)
{
    y_buf->g_y_buffer_len = 0;
    //memset(y_buf->g_y_buffer, 0, sizeof(y_buf->g_y_buffer));
}
static int y_end_record_data(y_test_buf *y_buf, rtsp_session_t *sessp, int media_type)
{
	int len;
    uint8_t *buf_ptr = y_buf->g_y_buffer;
    uint8_t *buf_end = y_buf->g_y_buffer + y_buf->g_y_buffer_len;

    if(!y_buf->g_y_buffer_len)
        return 0;
    switch (sessp->transport[media_type].lower_transport) {
        case RTSP_LOWER_TRANSPORT_TCP:
            pthread_mutex_lock(&sessp->rtsp_snd_lock);
        while (1) {
            len = send(sessp->rtsp_snd_sock, buf_ptr, buf_end - buf_ptr, 0);
            if (len <= 0) {
                if (len < 0 && (errno == EINTR || errno == EAGAIN)) {
                    perror("send");
                    continue;
                }
                pthread_mutex_unlock(&sessp->rtsp_snd_lock);
                return -1;
            }

            buf_ptr += len;

            if (buf_ptr >= buf_end) {
                sessp->timeout = 0;	/* clear timeout */
                pthread_mutex_unlock(&sessp->rtsp_snd_lock);
                break;
            }

            if (fd_wait_writeable(sessp->rtsp_snd_sock, 5000) <= 0)
            {
                pthread_mutex_unlock(&sessp->rtsp_snd_lock);
                return -1;
            }
        }
        break;
        case RTSP_LOWER_TRANSPORT_UDP:
            udp_send_rtp(sessp,  buf_ptr, buf_end - buf_ptr, media_type);
        break;
        default:
            break;
    }
    return 1;
}

#if 0
static int y_send_rtsp_interleaved_header(y_test_buf *y_buf, rtsp_session_t *sessp, int len, int channel)
{
	int n;
	uint8_t headers[4];
    unsigned int data_len = len;

	headers[0] = '$';
	headers[1] = channel;
	headers[2] = (uint8_t) ((data_len&0xFF00)>>8);
	headers[3] = (uint8_t) (data_len&0xFF);;

	//n = rtsp_writen(sessp->rtsp_snd_sock, headers, 4);
	memcpy(y_buf->g_y_buffer+y_buf->g_y_buffer_len, headers, 4);
	y_buf->g_y_buffer_len+=4;

	n = 4;
	if (n < 0)
		return -1;

	return 0;
}
#endif

static int y_tcp_send_rtp(y_test_buf *y_buf, rtsp_session_t *sessp, uint8_t *buf,
		int size, uint8_t *databuf,int dataLen,int media_type)
{
	int len = size;
	int channel = sessp->transport[media_type].interleaved_min;
	uint8_t headers[4];

	headers[0] = '$';
	headers[1] = channel;
	headers[2] = (uint8_t) (((size+dataLen)&0xFF00)>>8);
	headers[3] = (uint8_t) ((size+dataLen)&0xFF);;

	memcpy(y_buf->g_y_buffer+y_buf->g_y_buffer_len, headers, 4);
	y_buf->g_y_buffer_len+=4;

	memcpy(y_buf->g_y_buffer+y_buf->g_y_buffer_len, buf, size);
	y_buf->g_y_buffer_len+=size;

	memcpy(y_buf->g_y_buffer+y_buf->g_y_buffer_len, databuf, dataLen);
	y_buf->g_y_buffer_len +=dataLen;

    sessp->timeout = 0; /* clear timeout */

	if (!sessp->abort) {
		rtp_stat_t *stat = &sessp->rtp[media_type].stat;
		stat->octet_count += (len+dataLen);
		stat->packet_count++;
	} else {
		return -1;
	}

	return 0;
}

static ssize_t y_rtp_send_packet(y_test_buf *y_buf,rtsp_session_t *sessp, uint8_t *buf,
		int size, uint8_t *databuf,int dataLen,int media_type)
{
	int ret = -1;

	switch (sessp->transport[media_type].lower_transport) {
	case RTSP_LOWER_TRANSPORT_TCP:
		//pthread_mutex_lock(&sessp->rtsp_snd_lock);
		ret = y_tcp_send_rtp(y_buf, sessp, buf, size, databuf,dataLen,media_type);
		//pthread_mutex_unlock(&sessp->rtsp_snd_lock);
		break;
    case RTSP_LOWER_TRANSPORT_UDP:
    case RTSP_LOWER_TRANSPORT_UDP_MULTICAST:
        ret = y_udp_send_rtp(sessp, buf, size, databuf,dataLen,media_type);
		break;

	default:
		DPRINT(">>>hy: unknown lower transport type %d\n",
				sessp->transport[media_type].lower_transport);
		return -1;
	}

	return ret;
}

/*
    add end.
*/

static ssize_t tcp_send_rtcp(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	int len;

	int channel = sessp->transport[media_type].interleaved_max;
	if (send_rtsp_interleaved_header(sessp, size, channel) < 0)
		return -1;

	while (!sessp->abort) {
		if (fd_wait_writeable(sessp->rtsp_snd_sock, 5000) <= 0)
			return -1;

		len = send(sessp->rtsp_snd_sock, buf, size, 0);
		if (len < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		} else if (len == 0) {
			return -1;
		}

		if (len == size) {
			sessp->timeout = 0;	/* clear timeout */
			break;
		}
	}

	return 0;
}

static ssize_t udp_send_rtcp(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	if (send(sessp->rtp[media_type].rtcp_sock, buf, size, 0) < 0)
		return -1;

	return 0;
}

static ssize_t sendRtcpPkt(rtsp_session_t *sessp, uint8_t *buf,
		int size, int media_type)
{
	int ret = -1;

	switch (sessp->transport[media_type].lower_transport) {
	case RTSP_LOWER_TRANSPORT_TCP:
		pthread_mutex_lock(&sessp->rtsp_snd_lock);
		ret = tcp_send_rtcp(sessp, buf, size, media_type);
		pthread_mutex_unlock(&sessp->rtsp_snd_lock);
		break;
	case RTSP_LOWER_TRANSPORT_UDP:
	case RTSP_LOWER_TRANSPORT_UDP_MULTICAST:
		ret = udp_send_rtcp(sessp, buf, size, media_type);
		break;
	default:
		DPRINT("sendRtcpPkt failed!\n");
		return -1;
	}

	if (ret < 0) {
		perror("sendRtcpPkt error");
	}

	return ret;
}

typedef struct onvif_pkt_s {
	rtp_hdr_t rtp_hdr;
	onvif_exthdr_t ovf_hdr;
	uint8_t data[2048];
} onvif_pkt_t;

static int onvif_send_nal(rtsp_session_t *s, const uint8_t *buf, int size, int last, int reverse, int keyframe, int end_of_section, int cseq, int64_t time_ms)
{
	onvif_pkt_t pkt1, *pkt = &pkt1;

	pkt->rtp_hdr = s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;
	pkt->rtp_hdr.x = 1;	/* always set */

	pkt->ovf_hdr.id = htons(0xABAC);
	pkt->ovf_hdr.length = htons(3);

	uint64_t ntp_time = time_ms * 1000 + NTP_OFFSET_US;

	uint32_t ntp_sec  = ntp_time / 1000000;
	uint32_t ntp_usec = ntp_time % 1000000;
	uint32_t ntp_frac = ((int64_t)ntp_usec << 32) / 1000000 + 1;

	pkt->ovf_hdr.ntp_timestamp_msw = htonl(ntp_sec);
	pkt->ovf_hdr.ntp_timestamp_lsw = htonl(ntp_frac);

	pkt->ovf_hdr.c = keyframe ? 1 : 0;
	pkt->ovf_hdr.e = end_of_section ? 1 : 0;
	pkt->ovf_hdr.d = reverse ? (keyframe ? 1 : 0) : 0;

	pkt->ovf_hdr.mbz = 0;
	pkt->ovf_hdr.cseq = cseq;
	pkt->ovf_hdr.pad = 0;

	int max_payload_size = s->max_payload_size - sizeof(onvif_exthdr_t);

	if (size <= max_payload_size) {
		pkt->rtp_hdr.m = last;
		memcpy(pkt->data, buf, size);
		if (rtp_send_packet(s, (uint8_t *)pkt, sizeof(rtp_hdr_t) + sizeof(onvif_exthdr_t) + size, MEDIA_TYPE_VIDEO) < 0)
			return -1;
	} else {
		uint8_t type = buf[0] & 0x1F;
		uint8_t nri = buf[0] & 0x60;

		pkt->data[0] = 28;        /* FU Indicator; Type = 28 ---> FU-A */
		pkt->data[0] |= nri;

		pkt->data[1] = type;
		pkt->data[1] |= 1 << 7;

		buf += 1;
		size -= 1;

		while (size + 2 > max_payload_size) {
			pkt->rtp_hdr.m = 0;
			memcpy(&pkt->data[2], buf, max_payload_size - 2);

			int buf_size = sizeof(rtp_hdr_t) + sizeof(onvif_exthdr_t) + max_payload_size;
			if (rtp_send_packet(s, (uint8_t *)pkt, buf_size, MEDIA_TYPE_VIDEO) < 0)
				return -1;

			buf += max_payload_size - 2;
			size -= max_payload_size - 2;

			pkt->data[1] &= ~(1 << 7);

			increase_seq_number(&pkt->rtp_hdr.seq);
		}

		pkt->data[1] |= 1 << 6;
		pkt->rtp_hdr.m = last;
		memcpy(&pkt->data[2], buf, size);

		int buf_size = sizeof(rtp_hdr_t) + sizeof(onvif_exthdr_t) + size + 2;
		if (rtp_send_packet(s, (uint8_t *)pkt, buf_size, MEDIA_TYPE_VIDEO) < 0)
			return -1;
	}

	/* update sequence number */
	increase_seq_number(&pkt->rtp_hdr.seq);
	s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr.seq = pkt->rtp_hdr.seq;

	return 0;
}

typedef struct rtp_pkt_s {
	rtp_hdr_t rtp_hdr;
	uint8_t data[2048];
} rtp_pkt_t;

static int nal_send_h264(rtsp_session_t *s, const uint8_t *buf, int size, int last)
{
	rtp_pkt_t pkt1, *pkt = &pkt1;
	pkt->rtp_hdr = s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;

    y_test_buf *y_buf = s->y_buf;

	if (size <= s->max_payload_size) {
		pkt->rtp_hdr.m = last;
        if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t) ,(uint8_t *)buf, size, MEDIA_TYPE_VIDEO) < 0)
            return -1;
	} else {
		uint8_t type = buf[0] & 0x1F;
		uint8_t nri = buf[0] & 0xE0;

		pkt->data[0] = 28;        /* FU Indicator; Type = 28 ---> FU-A */
		pkt->data[0] |= nri;

		pkt->data[1] = type;
		pkt->data[1] |= 1 << 7;

		buf += 1;
		size -= 1;

		while (size + 2 > s->max_payload_size) {
			pkt->rtp_hdr.m = 0;
			if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t)+2,(uint8_t *)buf,s->max_payload_size-2, MEDIA_TYPE_VIDEO) < 0)
				return -1;

			buf += s->max_payload_size - 2;
			size -= s->max_payload_size - 2;

			pkt->data[1] &= ~(1 << 7);

			increase_seq_number(&pkt->rtp_hdr.seq);
		}

			pkt->data[1] |= 1 << 6;
			pkt->rtp_hdr.m = last;
			if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t)+2, (uint8_t *)buf,size, MEDIA_TYPE_VIDEO) < 0)
				return -1;
		}
	    if(last)
		{
	        y_end_record_data(y_buf, s, MEDIA_TYPE_VIDEO);
	    }
		increase_seq_number(&pkt->rtp_hdr.seq);
		s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr.seq = pkt->rtp_hdr.seq;

		return 0;
}


static int nal_send_h265(rtsp_session_t *s, const uint8_t *buf, int size, int last)
{
		rtp_pkt_t pkt1, *pkt = &pkt1;
		pkt->rtp_hdr = s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;
	    y_test_buf *y_buf = s->y_buf;

		if (size <= s->max_payload_size) {
			pkt->rtp_hdr.m = last;
	        if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t) ,(uint8_t *)buf, size, MEDIA_TYPE_VIDEO) < 0)
	            return -1;
		} else
		{
			uint8_t type = (buf[0] & 0x7E)>>1;

			pkt->data[0] = 49 <<1;        /* FU Indicator; Type = 28 ---> FU-A */
			pkt->data[1] = 1;
			pkt->data[2] = type;
			pkt->data[2] |= 1 << 7;

			buf += 2;
			size -= 2;

			while (size + 3 > s->max_payload_size) {
				pkt->rtp_hdr.m = 0;
				if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t)+3,(uint8_t *)buf,s->max_payload_size-3, MEDIA_TYPE_VIDEO) < 0)
					return -1;

				buf += s->max_payload_size - 3;
				size -= s->max_payload_size - 3;

				pkt->data[2] &= ~(1 << 7);

				increase_seq_number(&pkt->rtp_hdr.seq);
			}

			pkt->data[2] |= 1 << 6;
			pkt->rtp_hdr.m = last;
			if (y_rtp_send_packet(y_buf, s, (uint8_t *)pkt, sizeof(rtp_hdr_t)+3, (uint8_t *)buf,size, MEDIA_TYPE_VIDEO) < 0)
				return -1;
		}
	    if(last)
		{
	        y_end_record_data(y_buf, s, MEDIA_TYPE_VIDEO);
	    }
		increase_seq_number(&pkt->rtp_hdr.seq);
		s->rtp[MEDIA_TYPE_VIDEO].rtp_hdr.seq = pkt->rtp_hdr.seq;

		return 0;
}
#define RTSP_SEND_TIME_DEBUG 0

#if RTSP_SEND_TIME_DEBUG
static int calc_time_val(struct timeval now,struct timeval last)
{
    int ms = (now.tv_sec - last.tv_sec)*1000000;

    ms +=(now.tv_usec - last.tv_usec);
    return ms/1000;

}
#endif

#define SEC_TO_US(x) ((x) * 1000000)
static int rtp_send_h264(rtsp_session_t *sessp, const void *fb, int replay, int reverse, int keyframe, int end_of_section, int cseq, int64_t time_ms)
{
	const uint8_t *p;
	const uint8_t *end;
	const uint8_t *r, *r1;
	frame_head_t *f = (frame_head_t *)fb;
    rtsp_serv_t * pstServer = ArGetRtspServer();

	if (!sessp || !fb)
		return -1;

	#if RTSP_SEND_TIME_DEBUG
    struct timeval time1 ;
    struct timeval time2 ;
    struct timeval readtime ;
    static struct timeval lasttime2 ;
    gettimeofday(&time1, NULL);
	#endif

	p = fb + sizeof(frame_head_t);
	end = p + f->frame_size;


	/* update rtp timestamp */
	rtp_hdr_t *rtp_hdrp = &sessp->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;
	rtp_hdrp->ts = htonl((uint32_t)sessp->rtp[MEDIA_TYPE_VIDEO].pts);

    if((f->frame_size+10240) > sessp->y_buf->g_y_buffer_total)
    {
        if(sessp->y_buf->g_y_buffer)
        {
            free(sessp->y_buf->g_y_buffer);
        }
        sessp->y_buf->g_y_buffer_total = f->frame_size+30*1024; // add more 30KB
        sessp->y_buf->g_y_buffer = malloc(sessp->y_buf->g_y_buffer_total);
        if(sessp->y_buf->g_y_buffer == NULL)
        {
            printf("rtp_send_h264,malloc failed:len:%d\n",sessp->y_buf->g_y_buffer_total);
            return -1;
        }
    }
    y_start_record_data(sessp->y_buf);
	r = h264_find_startcode(p, end);
	while (r < end) {
		while (!*(r++));
		r1 = h264_find_startcode(r, end);
        //r1 = end;

		if (replay) {
			if (onvif_send_nal(sessp, r, r1 - r, r1 == end, reverse, keyframe, end_of_section, cseq, time_ms) < 0) {
				return -1;
			}
		} else {
			if (nal_send_h264(sessp, r, r1 - r, r1 == end) < 0)
				return -1;
		}

		r = r1;
	}
#if RTSP_SEND_TIME_DEBUG
    readtime.tv_sec = f->sec;
    readtime.tv_usec = f->usec;
    gettimeofday(&time2, NULL);
    if(f->video_reso > (1000<<16+1280))
    {
        printf("frame tyep:%d,send time:%d,frame size:%d,readInterval:%d,two send interval:%d\n",
            f->frame_type,calc_time_val(time2,time1),f->frame_size,calc_time_val(time1,readtime),calc_time_val(time1,lasttime2));


        lasttime2 = time1;
    }
#endif

	if (pstServer->enable_rtcp) {
		/* send rtcp packet as require */
		int64_t ntp_time = get_ntp_time();
		rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_VIDEO].stat;
		if (llabs(ntp_time - stat->last_rtcp_ntp_time) >
				SEC_TO_US(pstServer->rtcp_interval)) {
			if (rtcp_send_sr(sessp, stat, ntp_time, MEDIA_TYPE_VIDEO) < 0)
				return -1;
			stat->last_rtcp_ntp_time = ntp_time;
		}
	}

	return 0;
}

static int rtp_send_g711(rtsp_session_t *sessp, const void *fb)
{
	int size = 0;
	frame_head_t *f = (frame_head_t *)fb;
	const char *p = fb + sizeof(frame_head_t);
    rtsp_serv_t * pstServer = ArGetRtspServer();

	rtp_pkt_t pkt1, *pkt = &pkt1;

	/* update rtp timestamp */
	rtp_hdr_t *rtp_hdrp = &sessp->rtp[MEDIA_TYPE_AUDIO].rtp_hdr;
	rtp_hdrp->ts = htonl(sessp->rtp[MEDIA_TYPE_AUDIO].pts);

	if (f->frame_size > 2048) {
		DPRINT("Audio frame size[%d] is too large than 2048!\n", f->frame_size);
		return -1;
	}


	rtp_hdrp->m = 1;
	pkt->rtp_hdr = *rtp_hdrp;

	memcpy(pkt->data, p, f->frame_size);
	size = f->frame_size;

	if (rtp_send_packet(sessp, (uint8_t *)pkt, sizeof(rtp_hdr_t) + size, MEDIA_TYPE_AUDIO) < 0) {
		return -1;
	}

	increase_seq_number(&rtp_hdrp->seq);

	if (pstServer->enable_rtcp) {
		/* send rtcp packet as require */
		int64_t ntp_time = get_ntp_time();
		rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_AUDIO].stat;
		if (llabs(ntp_time - stat->last_rtcp_ntp_time) >
				SEC_TO_US(pstServer->rtcp_interval)) {
			if (rtcp_send_sr(sessp, stat, ntp_time, MEDIA_TYPE_AUDIO) < 0)
				return -1;
			stat->last_rtcp_ntp_time = ntp_time;
		}
	}

	return 0;
}

static int rtp_send_h264_p(rtsp_session_t *sessp, const void *fb, int replay, int reverse, int keyframe, int end_of_section, int cseq, int64_t time_ms,frame_head_t* rtsp_header)
{
	const uint8_t *p;
	const uint8_t *end;
	const uint8_t *r, *r1;
    rtsp_serv_t * pstServer = ArGetRtspServer();

	if (!sessp || !rtsp_header)
		return -1;

	frame_head_t *f = (frame_head_t *)rtsp_header;

	p = fb;
	end = p + f->frame_size;

	/* update rtp timestamp */
	rtp_hdr_t *rtp_hdrp = &sessp->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;
	rtp_hdrp->ts = htonl((uint32_t)sessp->rtp[MEDIA_TYPE_VIDEO].pts);

	#if RTSP_SEND_TIME_DEBUG
    struct timeval time1 ;
    struct timeval time2 ;
    struct timeval readtime ;
    static struct timeval lasttime2 ;
    gettimeofday(&time1, NULL);
	#endif

    if((f->frame_size+20480) > sessp->y_buf->g_y_buffer_total)
    {
        if(sessp->y_buf->g_y_buffer)
        {
            free(sessp->y_buf->g_y_buffer);
        }
        sessp->y_buf->g_y_buffer_total = f->frame_size+30*1024; // add more 30KB
        sessp->y_buf->g_y_buffer = malloc(sessp->y_buf->g_y_buffer_total);
        if(sessp->y_buf->g_y_buffer == NULL)
        {
            printf("rtp_send_h264,malloc failed:len:%d\n",sessp->y_buf->g_y_buffer_total);
            return -1;
        }
    }
    y_start_record_data(sessp->y_buf);

	r = h264_find_startcode(p, end);
	while (r < end) {
		while (!*(r++));
		r1 = h264_find_startcode(r, end);
        //r1 = end;

		if (replay) {
			if (onvif_send_nal(sessp, r, r1 - r, r1 == end, reverse, keyframe, end_of_section, cseq, time_ms) < 0) {
				return -1;
			}
		} else {
			if (nal_send_h264(sessp, r, r1 -r , r1 == end) < 0)
				return -1;
		}

		r = r1;
	}

	if (pstServer->enable_rtcp) {
		/* send rtcp packet as require */
		int64_t ntp_time = get_ntp_time();
		rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_VIDEO].stat;
		if (llabs(ntp_time - stat->last_rtcp_ntp_time) >
				SEC_TO_US(pstServer->rtcp_interval)) {
			if (rtcp_send_sr(sessp, stat, ntp_time, MEDIA_TYPE_VIDEO) < 0)
				return -1;
			stat->last_rtcp_ntp_time = ntp_time;
		}
	}

	return 0;
}


static int rtp_send_h265_p(rtsp_session_t *sessp, const void *fb, int replay, int reverse, int keyframe, int end_of_section, int cseq, int64_t time_ms,frame_head_t* rtsp_header)
{
	const uint8_t *p;
	const uint8_t *end;
	const uint8_t *r, *r1;
	rtsp_serv_t * pstServer = ArGetRtspServer();


	if (!sessp || !rtsp_header)
		return -1;

	frame_head_t *f = (frame_head_t *)rtsp_header;

	p = fb;
	end = p + f->frame_size;

	/* update rtp timestamp */
	rtp_hdr_t *rtp_hdrp = &sessp->rtp[MEDIA_TYPE_VIDEO].rtp_hdr;
	rtp_hdrp->ts = htonl((uint32_t)sessp->rtp[MEDIA_TYPE_VIDEO].pts);


    if((f->frame_size+20480) > sessp->y_buf->g_y_buffer_total)
    {
        if(sessp->y_buf->g_y_buffer)
        {
            free(sessp->y_buf->g_y_buffer);
        }
        sessp->y_buf->g_y_buffer_total = f->frame_size+30*1024; // add more 30KB
        sessp->y_buf->g_y_buffer = malloc(sessp->y_buf->g_y_buffer_total);
        if(sessp->y_buf->g_y_buffer == NULL)
        {
            printf("rtp_send_h264,malloc failed:len:%d\n",sessp->y_buf->g_y_buffer_total);
            return -1;
        }
    }
    y_start_record_data(sessp->y_buf);
    r = h265_find_startcode(p, end);


	while (r < end) {

		while (!*(r++));
		r1 = h265_find_startcode(r, end);

		if (replay) {
			if (onvif_send_nal(sessp, r, r1 - r, r1 == end, reverse, keyframe, end_of_section, cseq, time_ms) < 0) {
				return -1;
			}
		} else {
			int tmpret = nal_send_h265(sessp, r, r1 -r , r1 == end);
			if (tmpret < 0)
				return -1;
		}
		r = r1;
	}

	if (pstServer->enable_rtcp) {
		/* send rtcp packet as require */
		int64_t ntp_time = get_ntp_time();
		rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_VIDEO].stat;
		if (llabs(ntp_time - stat->last_rtcp_ntp_time) >
				SEC_TO_US(pstServer->rtcp_interval)) {
			if (rtcp_send_sr(sessp, stat, ntp_time, MEDIA_TYPE_VIDEO) < 0)
				return -1;
			stat->last_rtcp_ntp_time = ntp_time;
		}
	}

	return 0;
}

static int rtp_send_g711_p(rtsp_session_t *sessp, const void *fb,frame_head_t* rtsp_header)
{
	int size = 0;
	frame_head_t *f = (frame_head_t *)rtsp_header;
	const char *p = fb;
    rtsp_serv_t * pstServer = ArGetRtspServer();

	#if 1
    //static int audio_src_type = 0;

    char alaw_data[2048] = {0};
    int alaw_len = 0;

    //if(!audio_src_type){
    //    int i;
    //    for (i = 0; i < sessp->ndesc; i++) {
	//	    MediaDesc *desc = &sessp->mdescs[i];
	//	    if (desc->media == MEDIA_TYPE_AUDIO) {
    //            audio_src_type = desc->codec;
    //        }
    //    }
    //}
    //printf("--------- audio_src_type:%d\n", audio_src_type);
    //if(audio_src_type == MEDIA_CODEC_PCM){
    //    alaw_len = AR_G711_AlawEncode(alaw_data, fb, f->frame_size);
    //    p = alaw_data;
    //    f->frame_size = alaw_len;
    //    printf("rtsp pcm len:%d, alaw_len:%d\n", f->frame_size, alaw_len);
    //}
    memset(alaw_data, 0, sizeof(alaw_data));
	if(f->device_type == 0) //MONO
	{
    	alaw_len = AR_G711_AlawEncode((unsigned char *)alaw_data, (short *)fb, f->frame_size);
	}
	else    //STEREO
	{
		alaw_len = AR_G711_Alaw_Stereo_Encode((unsigned char *)alaw_data, (short*)(fb), (short*)((char*)fb+f->frame_size), f->frame_size);
	}
	p = alaw_data;
    //printf("rtsp pcm len:%d, alaw_len:%d\n", f->frame_size, alaw_len);
    f->frame_size = alaw_len;
	#endif

	rtp_pkt_t pkt1, *pkt = &pkt1;

	/* update rtp timestamp */
	rtp_hdr_t *rtp_hdrp = &sessp->rtp[MEDIA_TYPE_AUDIO].rtp_hdr;
	rtp_hdrp->ts = htonl(sessp->rtp[MEDIA_TYPE_AUDIO].pts);

	if (f->frame_size > 2048) {
		DPRINT("Audio frame size[%d] is too large than 2048!\n", f->frame_size);
		return -1;
	}


	rtp_hdrp->m = 1;
	pkt->rtp_hdr = *rtp_hdrp;

	memcpy(pkt->data, p, f->frame_size);
	size = f->frame_size;

	if (rtp_send_packet(sessp, (uint8_t *)pkt, sizeof(rtp_hdr_t) + size, MEDIA_TYPE_AUDIO) < 0) {
		return -1;
	}

	increase_seq_number(&rtp_hdrp->seq);

	if (pstServer->enable_rtcp) {
		/* send rtcp packet as require */
		int64_t ntp_time = get_ntp_time();
		rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_AUDIO].stat;
		if (llabs(ntp_time - stat->last_rtcp_ntp_time) >
				SEC_TO_US(pstServer->rtcp_interval)) {
			if (rtcp_send_sr(sessp, stat, ntp_time, MEDIA_TYPE_AUDIO) < 0)
				return -1;
			stat->last_rtcp_ntp_time = ntp_time;
		}
	}

	return 0;
}


static int64_t av_gettime_ms(void)
{
#if 1
	struct timespec tp;
	if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0)
		return -1;
	return (int64_t)tp.tv_sec * 1000 + (int64_t)tp.tv_nsec / 1000000;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_usec / 1000;
#endif
}

#define AV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

typedef struct FramePtr {
	uint8_t *ptr;
	int len;
} FramePtr;

static int rtsp_read_gop(rtsp_session_t *sessp, void *buffer, int size, FramePtr *fp, int nmemb)
{
	uint8_t *bufptr = buffer;
	uint8_t *bufend = buffer + size;

	int found = 0;

	int i;
	for (i = 0; i < 1024; i++) {
		int len = sessp->ops->read(sessp->handle, bufptr, bufend - bufptr, 1);
		if (len <= 0)
			return -1;

		frame_head_t *f = (frame_head_t *)bufptr;

		/* drop audio frame in reverse replay */
		if (f->frame_type == RTSP_A_FRAME) {
			i--;
			continue;
		}

		/* check malformed frame */
		if ((f->frame_size < 0) || (f->frame_size > MAX_FRAME_SIZE))
			return -1;

		/**
		 * check whether current frame is out of range
		 */
		int64_t time_ms = (int64_t)f->sec * 1000 + (int64_t)f->usec / 1000;
		//int64_t time_ms = (int64_t)f->usec / 1000;

		if (sessp->range_start > 0 && time_ms < sessp->range_start)
			return -1;	/* at the very begining, backward is not allowed */

		if (sessp->range_end > 0 && time_ms > sessp->range_end) {
			i--;
			continue;
		}

		fp[i].ptr = bufptr;
		fp[i].len = len;

		bufptr += len;

		if (f->frame_type == RTSP_I_FRAME)
			break;

		/*
		 * run out of memory, drop old frames and start it again
		 */
		if (bufend - bufptr == 0) {
			i = -1;
			bufptr = buffer;
			continue;
		}

		found = 1;
	}

	return found ? i + 1 : 0;
}

static int rtsp_replay_backward(rtsp_session_t *sessp, uint8_t *buffer, int bufsiz, RTSPMessageHeader *h, JShmem *shm)
{
	MediaDesc *desc;

	if (!sessp->rtp[MEDIA_TYPE_VIDEO].enable)
		return -1;

	if (h->range_start > 0) {
		/* seek to the right position */
		MediaDesc *desc = &sessp->mdescs[MEDIA_TYPE_VIDEO];
		int64_t offset_ms = h->range_start - desc->start_ms;
		if (offset_ms > 0) {
			if (sessp->ops->seek) {
				if (sessp->ops->seek(sessp->handle, offset_ms, SEEK_SET) < 0LL) {
					/* FIXME: abort ? */
					return -1;
				}
			}
		} else if (offset_ms < 0) {
			return -1;
		}

		h->range_start = 0;	/* XXX */
	}

	if (h->frames.interval) {
		if (sessp->ops->seek) {
			int interval = h->frames.interval * -1;		/* reverse replay */
			if (sessp->ops->seek(sessp->handle, interval, SEEK_CUR) < 0LL) {
				/* FIXME: abort ? */
				return -1;
			}
		}
	}

	/* skip to intra frame as require */
	if (h->frames.type == FRAMES_TYPE_INTRA) {
		if (sessp->ops->skip) {
			if (sessp->ops->skip(sessp->handle, 1, 1, 1) < 0) {
				return -1;
			}
		}
	}

	FramePtr fp[1024];
	int numframes;

	if (h->frames.type == FRAMES_TYPE_INTRA) {
		int len = sessp->ops->read(sessp->handle, buffer, bufsiz, 1);
		if (len <= 0)
			return -1;
		fp[0].ptr = buffer;
		fp[0].len = len;
		numframes = 1;
	} else {
		numframes = rtsp_read_gop(sessp, buffer, bufsiz, fp, NELEMS(fp));
		if (numframes <= 0)
			return -1;
	}

	if (sessp->rtp[MEDIA_TYPE_VIDEO].pts_base == AV_NOPTS_VALUE) {
		frame_head_t *f = (frame_head_t *)fp[0].ptr;
		sessp->rtp[MEDIA_TYPE_VIDEO].pts_base = f->pts;
		sessp->rtp[MEDIA_TYPE_VIDEO].timebase = av_gettime_ms();
	}

	int i;
	for (i = numframes - 1; i >= 0; i--) {
		frame_head_t *f = (frame_head_t *)fp[i].ptr;

		int64_t time_ms = (int64_t)f->sec * 1000 + (int64_t)f->usec / 1000;
        //int64_t time_ms = (int64_t)f->usec / 1000;

		if (f->frame_type != RTSP_I_FRAME && f->frame_type != RTSP_P_FRAME)
			continue;	/* video only */

		desc = &sessp->mdescs[MEDIA_TYPE_VIDEO];

		int64_t new_pts = f->pts;

		if (h->rate_control) {	/* enable rate control */
			int64_t now = av_gettime_ms();

			double ratio    = 1 / h->scale;
			double diff_pts = abs((f->pts - sessp->rtp[MEDIA_TYPE_VIDEO].pts_base) * ratio);
			double diff_ms  = diff_pts / desc->clockrate * 1000;
			double reltime  = sessp->rtp[MEDIA_TYPE_VIDEO].timebase + diff_ms;
			double waittime = reltime - (double)now;

			new_pts = sessp->rtp[MEDIA_TYPE_VIDEO].pts_base + diff_pts;

			if (f->frame_type == RTSP_I_FRAME) {
				if (waittime > 0 && waittime <= sessp->max_interval) {
					usleep(waittime * 1000);
				} else if (waittime > sessp->max_interval) {
					sessp->rtp[MEDIA_TYPE_VIDEO].pts_base = f->pts;
					sessp->rtp[MEDIA_TYPE_VIDEO].timebase = av_gettime_ms();
				}
			}
		}

		sessp->rtp[MEDIA_TYPE_VIDEO].pts = new_pts;

		int keyframe = f->frame_type == RTSP_I_FRAME ? 1 : 0;
		int end_of_section = i == 0 ? 1 : 0;	/* FIXME */
		if (shm) {
			/* FIXME */
			f->pts = new_pts;
			f->video_standard = h->seq;
			if (j_shmem_write(shm, fp[i].ptr, fp[i].len, NULL, NULL, 5000) < 0)
				return -1;
		} else {
			if (rtp_send_h264(sessp, fp[i].ptr, 1, 1, keyframe, end_of_section, h->seq, time_ms) < 0)
				return -1;
		}
	}

	return 0;
}

static int64_t get_pts_and_wait(rtsp_session_t *sessp, int media_type, double scale, int64_t pts)
{
	MediaDesc *desc = &sessp->mdescs[media_type];

	int64_t new_pts = pts;

	if (sessp->rtp[media_type].pts_base == AV_NOPTS_VALUE) {
		sessp->rtp[media_type].pts_base = pts;
		sessp->rtp[media_type].timebase = av_gettime_ms();
	}

	int64_t now = av_gettime_ms();

	double ratio    = 1 / scale;
	double diff_pts = abs((pts - sessp->rtp[media_type].pts_base) * ratio);
	double diff_ms  = diff_pts / desc->clockrate * 1000;
	double reltime  = sessp->rtp[media_type].timebase + diff_ms;
	double waittime = reltime - (double)now;

	new_pts = sessp->rtp[media_type].pts_base + diff_pts;

	if (waittime > 0 && waittime <= sessp->max_interval) {
		usleep(waittime * 1000);
	} else if (waittime > sessp->max_interval) {
		sessp->rtp[media_type].pts_base = pts;
		sessp->rtp[media_type].timebase = av_gettime_ms();
	}

	return new_pts;
}

static int rtsp_replay_forward(rtsp_session_t *sessp, uint8_t *buffer, int bufsiz, RTSPMessageHeader *h, JShmem *shm)
{
	if (h->range_start > 0) {
		/* seek to the right position */
		MediaDesc *desc = &sessp->mdescs[MEDIA_TYPE_VIDEO];
		int64_t offset_ms = h->range_start - desc->start_ms;
		if (offset_ms > 0) {
			if (sessp->ops->seek) {
				if (sessp->ops->seek(sessp->handle, offset_ms, SEEK_SET) < 0LL) {
					//printf("seek failed, offset_ms = %ld\n", offset_ms);
					/* FIXME: abort ? */
					return -1;
				}
			}
		} else if (offset_ms < 0) {
			//printf("offset %ld is out of range(%ld, %ld)\n",
			//		h->range_start, desc->start_ms, desc->end_ms);
			return -1;
		}

		h->range_start = 0;	/* XXX */
	}

	if (h->frames.interval) {
		if (sessp->ops->seek) {
			if (sessp->ops->seek(sessp->handle, h->frames.interval, SEEK_CUR) < 0LL) {
				printf("%s.%d: seek failed\n", __func__, __LINE__);
				/* FIXME: abort ? */
				return -1;
			}
		}
	}

	/* skip to intra frame as require */
	if (h->frames.type == FRAMES_TYPE_INTRA) {
		if (sessp->ops->skip) {
			if (sessp->ops->skip(sessp->handle, 1, 1, 0) < 0) {
				printf("%s.%d: skip failed\n", __func__, __LINE__);
				return -1;
			}
		}
	}

	int len = sessp->ops->read(sessp->handle, buffer, bufsiz, 0);
	if (len <= 0) {
		printf("%s.%d: read frame failed\n", __func__, __LINE__);
		return -1;
	}

	frame_head_t *f = (frame_head_t *)buffer;

	/* check frame size */
	if ((f->frame_size < 0) || (f->frame_size > MAX_FRAME_SIZE)) {
		printf("%s.%d: invalid frame size %d\n", __func__, __LINE__, f->frame_size);
		return -1;
	}

	/**
	 * check whether current frame is out of range
	 */
	int64_t time_ms = (int64_t)f->sec * 1000 + (int64_t)f->usec / 1000;
    //int64_t time_ms = (int64_t)f->usec / 1000;

	if (sessp->range_start > 0 && time_ms < sessp->range_start)
		return 0;

	if (sessp->range_end > 0 && time_ms > sessp->range_end) {
		char curr[128];
		print_time(time_ms, curr, 128);

		char end[128];
		print_time(sessp->range_end, end, 128);

		printf("%s: (curr %s, end %s) end of record, no more further\n", __func__, curr, end);
		return -1;	/* no more further */
	}

	int64_t new_pts;

	switch (f->frame_type) {
	case RTSP_I_FRAME:
	case RTSP_P_FRAME:
		if (!sessp->rtp[MEDIA_TYPE_VIDEO].enable)
			break;

		if (h->rate_control) {
			new_pts = get_pts_and_wait(sessp, MEDIA_TYPE_VIDEO, h->scale, f->pts);
//			printf("o_pts = %lld, n_pts = %lld\n", f->pts, new_pts);
		} else {
			new_pts = f->pts;
		}

		sessp->rtp[MEDIA_TYPE_VIDEO].pts = new_pts;

		int keyframe = f->frame_type == RTSP_I_FRAME ? 1 : 0;

		if (shm) {
			/* FIXME */
			f->pts = new_pts;
			f->video_standard = h->seq;
			if (j_shmem_write(shm, buffer, len, NULL, NULL, 5000) < 0) {
				return -1;
			}
		} else {
			if (rtp_send_h264(sessp, buffer, 1, 0, keyframe, 0, h->seq, time_ms) < 0) {
				return -1;
			}
		}

		break;
	case RTSP_A_FRAME:
		if (!sessp->rtp[MEDIA_TYPE_AUDIO].enable)
			break;

		if (h->rate_control)
			new_pts = get_pts_and_wait(sessp, MEDIA_TYPE_AUDIO, h->scale, f->pts);
		else
			new_pts = f->pts;

		sessp->rtp[MEDIA_TYPE_AUDIO].pts = new_pts;

		if (shm) {
			/* FIXME */
			f->pts = new_pts;
			f->video_standard = h->seq;
			if (j_shmem_write(shm, buffer, len, NULL, NULL, 5000) < 0) {
				printf("%s.%d: j_shmem_write failed!\n", __func__, __LINE__);
				return -1;
			}
		} else {
			if (rtp_send_g711(sessp, buffer) < 0)
				return -1;
		}

		break;
	default:
		DPRINT("Wrong frame type[%d]!\n", f->frame_type);
	}

	return 0;
}

void *rtsp_replay_thread(void *arg)
{
    prctl(PR_SET_NAME, "rtsp_replay", 0,0,0);
	uint8_t *buffer = NULL;
	RTSPMessageHeader *current = NULL;

	rtsp_session_t *sessp = (rtsp_session_t *)arg;
	if (!sessp)
		return NULL;

	JShmem *shm = NULL;
	RTSPTransportField *th = &sessp->transport[MEDIA_TYPE_VIDEO];
	if (th->lower_transport == RTSP_LOWER_TRANSPORT_SHMEM) {
		if (!strlen(th->shm_name) || !th->shm_size)
			goto fail;
		shm = j_shmem_connect(th->shm_name, th->shm_size);
		if (!shm)
			goto fail;
	}

	int bufsiz = 3 * MAX_FRAME_SIZE;

	buffer = malloc(bufsiz);
	if (!buffer)
		goto fail;

	while (!sessp->abort) {
		if (sessp->multicast) {
			if (sessp->mcast_refcount == 0) {
				msleep(200);
				continue;
			}
		} else {
			if (sessp->state != RTSP_STATE_PLAYING) {
				msleep(100);
				continue;
			}
		}

		/*
		 * new PLAY method came, reset the pts base and time base
		 */
		RTSPMessageHeader *h = j_queue_get(sessp->cmdq, 0);
		if (h) {
			if (current)
				free(current);
			current = h;

			sessp->rtp[MEDIA_TYPE_VIDEO].pts_base = AV_NOPTS_VALUE;
			sessp->rtp[MEDIA_TYPE_VIDEO].timebase = AV_NOPTS_VALUE;

			sessp->rtp[MEDIA_TYPE_AUDIO].pts_base = AV_NOPTS_VALUE;
			sessp->rtp[MEDIA_TYPE_AUDIO].timebase = AV_NOPTS_VALUE;
		}

		int backward = current->scale > 0 ? 0 : 1;

		if (backward) {
			if (rtsp_replay_backward(sessp, buffer, bufsiz, current, shm) < 0) {
				printf("%s.%d: rtsp_replay_backward failed!\n", __func__, __LINE__);
				goto fail;
			}
		} else {
			if (rtsp_replay_forward(sessp, buffer, bufsiz, current, shm) < 0) {
				printf("%s.%d: rtsp_replay_forward failed!\n", __func__, __LINE__);
				goto fail;
			}
		}
	}

fail:
	if (sessp->abort == 0) {
		if (sessp->rtp[MEDIA_TYPE_VIDEO].enable) {
			int64_t ntp_time = get_ntp_time();
			rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_VIDEO].stat;
			rtcp_send_bye(sessp, stat, ntp_time, MEDIA_TYPE_VIDEO);
		}

		if (sessp->rtp[MEDIA_TYPE_AUDIO].enable) {
			int64_t ntp_time = get_ntp_time();
			rtp_stat_t *stat = &sessp->rtp[MEDIA_TYPE_AUDIO].stat;
			rtcp_send_bye(sessp, stat, ntp_time, MEDIA_TYPE_AUDIO);
		}
	}

	if (shm)
		j_shmem_destroy(shm);
	if (buffer)
		free(buffer);
	if (current)
		free(current);
	sessp->abort = 1;	/* abort this session */
	return NULL;
}

static int rtsp_send_stream(rtsp_session_t *sessp)
{
	if (!sessp)
		return -1;

    frame_head_t rtsp_header;
	frame_head_t *hdr = &rtsp_header;

    uint8_t *buffer;
    int size = 0;
	int ret = 0;


	while (!sessp->abort) {
		if (sessp->multicast) {
			if (sessp->mcast_refcount == 0) {
				msleep(200);
				continue;
			}
		} else {
			if (sessp->state != RTSP_STATE_PLAYING) {
				msleep(100);
				continue;
			}
		}
        size = 0;
        buffer = 0;
		if (sessp->saved_keyframe) {
			if (sessp->saved_keyframe_length > MAX_FRAME_SIZE) {
				free(sessp->saved_keyframe);
				sessp->saved_keyframe = NULL;
				sessp->saved_keyframe_length = 0;
				continue;
			}

			buffer = sessp->saved_keyframe;
			size = sessp->saved_keyframe_length;
            memcpy(&rtsp_header,&sessp->saved_head,sizeof(frame_head_t));
		} else {

			ret = sessp->ops->read_p(sessp->handle, (void **)&buffer, (size_t *)&size, 0,&rtsp_header);
			if (ret <= 0)
				continue;
		}

		if (hdr->frame_type == RTSP_I_FRAME ||
				hdr->frame_type == RTSP_P_FRAME) {
			if (sessp->rtp[MEDIA_TYPE_VIDEO].enable) {
				//sessp->rtp[MEDIA_TYPE_VIDEO].pts = hdr->pts;
				sessp->rtp[MEDIA_TYPE_VIDEO].pts += 90000 /hdr->frame_rate;

				if (sessp->mdescs[0].codec == MEDIA_CODEC_H264)
				{
					if (rtp_send_h264_p(sessp, buffer, 0, 0, 0, 0, 0, 0,&rtsp_header) < 0) {
						ret = -1;
						goto fail;
					}
				}
				else if(sessp->mdescs[0].codec == MEDIA_CODEC_H265)
				{
					if (rtp_send_h265_p(sessp, buffer, 0, 0, 0, 0, 0, 0,&rtsp_header) < 0) {
						ret = -1;
						goto fail;
					}
				}
			}
		} else if (hdr->frame_type == RTSP_A_FRAME) {
			if (sessp->rtp[MEDIA_TYPE_AUDIO].enable) {
				//sessp->rtp[MEDIA_TYPE_AUDIO].pts = hdr->pts; 
				sessp->rtp[MEDIA_TYPE_AUDIO].pts += hdr->frame_size/2; //8000/hdr->frame_rate;//1024

				if (rtp_send_g711_p(sessp, buffer,&rtsp_header) < 0) {
                    ret = -1;
					goto fail;
				}
			}
		} else {
			DPRINT("Wrong frame type[%d]!\n", hdr->frame_type);
		}

        if(buffer == sessp->saved_keyframe)
        {
            if(buffer != NULL)
            {
    		    free(sessp->saved_keyframe);
    			sessp->saved_keyframe = NULL;
    			sessp->saved_keyframe_length = 0;
            }
        }
	}


	ret = 0;

fail:
    if(buffer == sessp->saved_keyframe)
    {
        if(buffer != NULL)
        {
		    free(sessp->saved_keyframe);
			sessp->saved_keyframe = NULL;
			sessp->saved_keyframe_length = 0;
        }
    }
	return ret;
}

static int shm_write_cb(void *buf, size_t size, void *ctx)
{
	rtsp_session_t *sessp = ctx;
	if (!sessp)
		return -1;

	int len = 0;

	if (sessp->saved_keyframe) {
		if (sessp->saved_keyframe_length > size) {
            len = sessp->ops->read(sessp->handle, buf, size, 0);
		} else {
			memcpy(buf, sessp->saved_keyframe, sessp->saved_keyframe_length);
			len = sessp->saved_keyframe_length;
		}

		free(sessp->saved_keyframe);
		sessp->saved_keyframe = NULL;
		sessp->saved_keyframe_length = 0;

		return len;
	}

    return sessp->ops->read(sessp->handle, buf, size, 0);
}

static int shmem_send_stream(rtsp_session_t *sessp)
{
	if (!sessp)
		return -1;

	JShmem *shm = NULL;

	RTSPTransportField *th = &sessp->transport[MEDIA_TYPE_VIDEO];
	if (!strlen(th->shm_name) || !th->shm_size)
		return -1;

	shm = j_shmem_connect(th->shm_name, th->shm_size);
	if (!shm)
		return -1;

	int timeout = 0;

	while (!sessp->abort) {
		if (sessp->multicast) {
			if (sessp->mcast_refcount == 0) {
				msleep(200);
				continue;
			}
		} else {
			if (sessp->state != RTSP_STATE_PLAYING) {
				msleep(100);
				continue;
			}
		}

		int len = j_shmem_write(shm, NULL, 0, shm_write_cb, sessp, 1000);
		if (len < 0) {
			if (errno == ETIMEDOUT) {
				if (!sessp->pause && ++timeout > 5) {
					printf("%s.%d: j_shmem_write timeout, abort this session!\n", __func__, __LINE__);
					goto fail;
				}
				continue;
			}
			goto fail;
		}
		timeout = 0;
	}

	j_shmem_destroy(shm);
	return 0;

fail:
	if (shm)
		j_shmem_destroy(shm);
	return -1;
}

void *rtsp_stream_thread(void *arg)
{
    prctl(PR_SET_NAME, "rtsp_str_thrd", 0,0,0);
	rtsp_session_t *sessp = arg;
	if (!sessp)
		return NULL;

	RTSPTransportField *th = &sessp->transport[MEDIA_TYPE_VIDEO];
	if (th->lower_transport == RTSP_LOWER_TRANSPORT_SHMEM) {
		if (shmem_send_stream(sessp) < 0)
			goto fail;
	} else {
		if (rtsp_send_stream(sessp) < 0)
			goto fail;
	}

fail:
	sessp->abort = 1;
	return NULL;
}
