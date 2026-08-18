#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net_wpactl.h"

#define CONFIG_CTRL_IFACE_DIR "/var/run/hostapd"

static const char *ctrl_iface_dir = CONFIG_CTRL_IFACE_DIR;


static char *ctrl_ifname = NULL;

static struct wpa_ctrl *ctrl_conn;

static int hostapd_cli_attached = 0;


static void z_wpa_cli_msg_cb(char *msg, unsigned int len)
{
	printf("---cb:%s\n", msg);
}

int ar1021x_have_client = 0;

int z_wpa_ctrl_command_sta(struct wpa_ctrl *ctrl, char *cmd,
				char *addr, size_t addr_len)
{
	char buf[4096], *pos;
	unsigned int len;
	int ret;

	if (ctrl_conn == NULL) {
		printf("Not connected to hostapd - command dropped.\n");
		return -1;
	}
	len = sizeof(buf) - 1;
    //printf("cmd:%s\n", cmd);
	ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len,
			       z_wpa_cli_msg_cb);
    //printf("ret:%d, len:%d, buf:%s\n", ret, len, buf);
	if (ret == -2) {
		printf("'%s' command timed out.\n", cmd);
		return -2;
	} else if (ret < 0) {
		printf("'%s' command failed.\n", cmd);
		return -1;
	}

	buf[len] = '\0';
    if (memcmp(buf, "FAIL", 4) == 0) {
        ar1021x_have_client = 0;
        return -1;
    } else {
        //printf("z_wpa_ctrl_command_sta, recv, len:%d, buf:%s", len, buf);
        ar1021x_have_client = 1;
    }

	pos = buf;
	while (*pos != '\0' && *pos != '\n')
		pos++;
	*pos = '\0';
	//os_strlcpy(addr, buf, addr_len);
    memcpy(addr, buf, addr_len); //addr 是mac 地址，获取下一个mac地址用
	return 0;
}


int z_wpa_cli_cmd_all_sta(struct wpa_ctrl *ctrl)
{
	char addr[32];

	if (z_wpa_ctrl_command_sta(ctrl, "STA-FIRST", addr, sizeof(addr)))
		return 0;

    #if 0
    char cmd[64];
    //不需要发STA-NEXT获取下一个信息
	do {
		snprintf(cmd, sizeof(cmd), "STA-NEXT %s", addr);
	} while (z_wpa_ctrl_command_sta(ctrl, cmd, addr, sizeof(addr)) == 0);
    #endif
    
	return -1;
}

static struct wpa_ctrl * z_wpa_cli_open_connection(const char *ifname)
{
	char *cfile;
	int flen;

	if (ifname == NULL)
		return NULL;

	flen = strlen(ctrl_iface_dir) + strlen(ifname) + 2;
	cfile = malloc(flen);
	if (cfile == NULL)
		return NULL;
	snprintf(cfile, flen, "%s/%s", ctrl_iface_dir, ifname);

	ctrl_conn = wpa_ctrl_open(ifname, cfile);
	free(cfile);
	return ctrl_conn;
}

static void z_wpa_cli_close_connection(void)
{
	if (ctrl_conn == NULL)
		return;

	if (hostapd_cli_attached) {
		wpa_ctrl_detach(ctrl_conn);
		hostapd_cli_attached = 0;
	}
	wpa_ctrl_close(ctrl_conn);
	ctrl_conn = NULL;
}


int z_wpa_get_all_sta_mac()
{
    //int warning_displayed = 0;

    ctrl_ifname = "wlan0";
    
    ctrl_conn = z_wpa_cli_open_connection(ctrl_ifname);
    if (!ctrl_conn) {
        printf("ERROR: Connection not established.\n");
        return -1;
    }

    #if 0
    if (wpa_ctrl_attach(ctrl_conn) == 0) {
        hostapd_cli_attached = 1;
    } else {
        printf("Warning: Failed to attach to hostapd.\n");
        return -1;
    }
    #endif

    z_wpa_cli_cmd_all_sta(ctrl_conn);

    z_wpa_cli_close_connection();

    return 0;
}


