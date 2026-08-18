/*
 * wpa_supplicant/hostapd control interface library
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include "net_wpactl.h"

 static struct wpa_ctrl * _wpa_ctrl_open(const char *ctrl_path)
 {
     struct wpa_ctrl *ctrl;
     static int counter = 0;
     int ret;
     int tries = 0;
     ctrl = malloc(sizeof(*ctrl));
     if (ctrl == NULL)
         return NULL;
     memset(ctrl, 0, sizeof(*ctrl));

     ctrl->s = socket(PF_UNIX, SOCK_DGRAM, 0);
     if (ctrl->s < 0) {
         free(ctrl);
         return NULL;
     }

     ctrl->local.sun_family = AF_UNIX;
     counter++;
 try_again:
     ret = snprintf(ctrl->local.sun_path, sizeof(ctrl->local.sun_path),
               "/tmp/wpa_ctrl_%d-%d", getpid(), counter);
     if (ret < 0 || (unsigned int) ret >= sizeof(ctrl->local.sun_path)) {
         close(ctrl->s);
         free(ctrl);
         return NULL;
     }
     tries++;
     if (bind(ctrl->s, (struct sockaddr *) &ctrl->local,
             sizeof(ctrl->local)) < 0) {
         if (errno == EADDRINUSE && tries < 2) {
             /*
              * getpid() returns unique identifier for this instance
              * of wpa_ctrl, so the existing socket file must have
              * been left by unclean termination of an earlier run.
              * Remove the file and try again.
              */
             unlink(ctrl->local.sun_path);
             goto try_again;
         }
         close(ctrl->s);
         free(ctrl);
         return NULL;
     }

     ctrl->dest.sun_family = AF_UNIX;
     if (strlen(ctrl_path) >= sizeof(ctrl->dest.sun_path)) {
         close(ctrl->s);
         free(ctrl);
         return NULL;
     }
     memset(ctrl->dest.sun_path, 0, sizeof(ctrl->dest.sun_path));
     strncpy(ctrl->dest.sun_path, ctrl_path,
              sizeof(ctrl->dest.sun_path));
     if (connect(ctrl->s, (struct sockaddr *) &ctrl->dest,
             sizeof(ctrl->dest)) < 0) {
         close(ctrl->s);
         unlink(ctrl->local.sun_path);
         free(ctrl);
         return NULL;
     }
     return ctrl;
 }

struct wpa_ctrl * wpa_ctrl_open(const char *ifname, const char *ctrl_path)
{
    char ctl_path[128] = {0};
    struct wpa_ctrl *pstWpaCtl;
    if(isNull(ifname) || isNull(ctrl_path))
        return NULL;
    sprintf(ctl_path, "%s/%s", WPA_CTL_INTERFACE, ifname);
    pstWpaCtl = _wpa_ctrl_open(ctl_path);
    return pstWpaCtl;
}

 void wpa_ctrl_close(struct wpa_ctrl *ctrl)
 {
     unlink(ctrl->local.sun_path);
     close(ctrl->s);
     free(ctrl);
 }

 int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, unsigned int cmd_len,
              char *reply, unsigned int *reply_len,
              void (*msg_cb)(char *msg, unsigned int len))
 {
     struct timeval tv;
     int res;
     fd_set rfds;
     const char *_cmd;
     char *cmd_buf = NULL;
     unsigned int _cmd_len;

     {
         _cmd = cmd;
         _cmd_len = cmd_len;
     }

     if (send(ctrl->s, _cmd, _cmd_len, 0) < 0) {
         free(cmd_buf);
         return -1;
     }
     free(cmd_buf);

     for (;;) {
         tv.tv_sec = 2;
         tv.tv_usec = 0;
         FD_ZERO(&rfds);
         FD_SET(ctrl->s, &rfds);
         res = select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
         if (FD_ISSET(ctrl->s, &rfds)) {
             res = recv(ctrl->s, reply, *reply_len, 0);
             if (res < 0)
                 return res;
             if (res > 0 && reply[0] == '<') {
                 /* This is an unsolicited message from
                  * wpa_supplicant, not the reply to the
                  * request. Use msg_cb to report this to the
                  * caller. */
                 if (msg_cb) {
                     /* Make sure the message is nul
                      * terminated. */
                     if ((unsigned int) res == *reply_len)
                         res = (*reply_len) - 1;
                     reply[res] = '\0';
                     reply[0] = '>';
                     msg_cb(reply, res);
                 }
                 continue;
             }
             *reply_len = res;
             break;
         } else {
             return -2;
         }
     }
     return 0;
 }


 static int wpa_ctrl_attach_helper(struct wpa_ctrl *ctrl, int attach)
 {
     char buf[10];
     int ret;
     unsigned int len = 10;

     ret = wpa_ctrl_request(ctrl, attach ? "ATTACH" : "DETACH", 6,
                    buf, &len, NULL);
     if (ret < 0)
         return ret;
     if (len == 3 && memcmp(buf, "OK\n", 3) == 0)
         return 0;
     return -1;
 }


 int wpa_ctrl_attach(struct wpa_ctrl *ctrl)
 {
     return wpa_ctrl_attach_helper(ctrl, 1);
 }


 int wpa_ctrl_detach(struct wpa_ctrl *ctrl)
 {
     return wpa_ctrl_attach_helper(ctrl, 0);
 }


 int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, unsigned int *reply_len)
 {
     int res;

     res = recv(ctrl->s, reply, *reply_len, 0);
     if (res < 0)
         return res;
     *reply_len = res;
     return 0;
 }


 int wpa_ctrl_pending(struct wpa_ctrl *ctrl)
 {
     struct timeval tv;
     fd_set rfds;
     tv.tv_sec = 10;
     tv.tv_usec = 0;
     FD_ZERO(&rfds);
     FD_SET(ctrl->s, &rfds);
     select(ctrl->s + 1, &rfds, NULL, NULL, &tv);
     return FD_ISSET(ctrl->s, &rfds);
 }


 int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl)
 {
     return ctrl->s;
 }

int wpa_get_connect_status(struct wpa_ctrl *ctrl, char **pstatus, int *plen)
{
	char buf[2048];
	unsigned int len;
	int ret;
    char *substr = NULL;
    const char *cmd = "STATUS";
    const char *wpa_status = "wpa_state=";

	len = sizeof(buf) - 1;
    memset(buf, 0, sizeof(buf));
	ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
	if (ret == -2) {
		printf("'%s' command timed out.\n", cmd);
		return WIFI_TIMEOUT;
	} else if (ret < 0) {
		printf("'%s' command failed.\n", cmd);
		return WIFI_FAIL;
	}

    substr = strstr(buf, wpa_status);
    if(substr != NULL){
        int len = 0;
        substr += strlen(wpa_status);
        while(substr[len] == '_' || (substr[len] <= 'Z' && substr[len] >= 'A') || (substr[len] <= '9' && substr[len] >= '0')){
            len++;
        }
        *pstatus = malloc(len+1);
        if(pstatus == NULL) {
            printf("malloc failed.\n");
            return WIFI_FAIL;
        }
        *plen = len+1;
        memset(*pstatus, 0, len+1);
        memcpy(*pstatus, substr, len);
        return 0;
    }
    return WIFI_FAIL;
}

int wpa_ctrl_command(struct wpa_ctrl *ctrl, char *cmd)
{
	char buf[2048];
	unsigned int len;
	int ret;

	len = sizeof(buf) - 1;
	ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
	if (ret == -2) {
		printf("'%s' command timed out.\n", cmd);
		return WIFI_TIMEOUT;
	} else if (ret < 0) {
		printf("'%s' command failed.\n", cmd);
		return WIFI_FAIL;
	}

    if(len >= 2 && strncmp(buf, "OK", 2) == 0)
        return WIFI_OK;
    else
        return WIFI_FAIL;
}


