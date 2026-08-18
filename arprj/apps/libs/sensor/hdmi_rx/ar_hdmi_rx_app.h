#ifndef _AR_HDMI_RX_APP_H_
#define _AR_HDMI_RX_APP_H_

#include "hal_vin_type_def.h"

typedef int (*ptf_notify_call_back)(int camera_id,notify_t notify_code,void * pra,int size,void *usr_data);


enum HDMI_RX_NAME
{
	HDMI_RX_DW = 0,
	HDMI_RX_ITE,
	HDMI_RX_MAX
};

#define HDMI_RX_UNSTABLE (0)
#define HDMI_RX_VIDEO_O (1)
#define HDMI_RX_AVSTABLE (2)

#define HDMI_RX_OUT_MODE_RGB444 (0)
#define HDMI_RX_OUT_MODE_YUV422 (1)
#define HDMI_RX_OUT_MODE_YUV444 (2)

struct video_format_info_t {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t fps_num;
    uint32_t fps_den;
	uint32_t is_interlace;
};

struct audio_format_info_t {
    uint32_t format;
    uint32_t rate;
    uint32_t channels;
};

struct ar_hdmi_rx_notify_t {
	STRU_NOTIFY_EVENT_DATA_SRC_PRA_T hdmi_notify_pra;
	notify_t hdmi_notify_t;
	void *hdmi_format_notif_data;
	ptf_notify_call_back hdmi_format_notify;
	void (*notify_format)(struct ar_hdmi_rx_notify_t *notify_t);
	struct ar_hdmi_rx_driver *drv;
};


struct ar_hdmi_rx_device {
	struct video_format_info_t hdmi_video;
	struct audio_format_info_t hdmi_audio;
	void *pre;
};

struct hdmi_rx_device_operations {
	int (*check_av_state) (void);
	int (*get_vformat)(struct video_format_info_t *hdmi_video);
	int (*get_aformat)(struct audio_format_info_t *hdmi_audio);
};

struct ar_hdmi_rx_driver {
	signed char name;
	void *pre;
	struct hdmi_rx_device_operations *ops;
};


struct ar_hdmi_rx_device_driver {
	struct ar_hdmi_rx_device dev;
	struct ar_hdmi_rx_driver *drv;
};

struct _ar_hdmi_rx {
	struct ar_hdmi_rx_notify_t *hdmi_rx_notify;
	struct ar_hdmi_rx_device_driver hdmi_rx_device_driver;
};

void ar_hdmi_rx_register_driver(struct ar_hdmi_rx_driver *device_driver);
void ar_hdmi_rx_register_notify(unsigned char name, struct ar_hdmi_rx_notify_t **notify);
void hdmi_rx_notify_format(struct ar_hdmi_rx_notify_t *notify_t);;

int register_hdmi_notify(void *data, ptf_notify_call_back cb);

void * board_fdt_blob_setup(void);
int fdt_node_offset_by_compatible(const void *fdt, int startoffset,
				  const char *compatible);
int fdtdec_get_int_array(const void *blob, int node, const char *prop_name,
		unsigned int *array, int count);
int fdtdec_get_int_array_count(const void *blob, int node,
			       const char *prop_name, unsigned int *array, int count);
#endif

