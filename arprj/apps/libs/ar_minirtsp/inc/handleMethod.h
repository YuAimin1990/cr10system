#ifndef __HANDLEMETHOD_H__
#define __HANDLEMETHOD_H__

#include "rtspServ.h"

int initRtpHdr(rtp_hdr_t *hdrp, int media_type);

int rtsp_cmd_options(rtsp_session_t *sessp);
int rtsp_cmd_describe(rtsp_session_t *sessp);
int rtsp_cmd_setup(rtsp_session_t *sessp);
int rtsp_cmd_teardown(rtsp_session_t *sessp);
int rtsp_cmd_play(rtsp_session_t *sessp);
int rtsp_cmd_pause(rtsp_session_t *sessp);
int rtsp_cmd_get_parameter(rtsp_session_t *sessp);
int rtsp_cmd_set_parameter(rtsp_session_t *sessp);


#endif /* __HANDLEMETHOD_H__ */
