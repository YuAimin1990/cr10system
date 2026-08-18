
#ifndef __PARSER_H__
#define __PARSER_H__

#include "rtspServ.h"

int rtsp_get_media_type(char *req);
int rtsp_parse_request(rtsp_session_t *sessp);
RtspRoute *rtsp_get_route(const char *route);

#endif /* __PARSER_H__ */
