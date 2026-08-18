//#define ENABALE_ALL_LOG
#if !defined (__linux__)
#include "reg_access.h"
#endif
#include "osal.h"
#include "ar_hdmi_rx_app.h"
#include "hal_vin_log.h"

struct ar_hdmi_rx_notify_t ar_hdmi_rx_notify[HDMI_RX_MAX] = {
	{
		.notify_format = hdmi_rx_notify_format,
		.hdmi_notify_t = NOTIFY_EVENT_DISCONNECT,
		.drv = NULL,
	},
	{
		.notify_format = hdmi_rx_notify_format,
		.hdmi_notify_t = NOTIFY_EVENT_DISCONNECT,
		.drv = NULL,
	},

};

void ar_hdmi_rx_register_driver(struct ar_hdmi_rx_driver *device_driver)
{
	unsigned char name = device_driver->name;
	ar_hdmi_rx_notify[name].drv = device_driver;
}

void ar_hdmi_rx_register_notify(unsigned char name, struct ar_hdmi_rx_notify_t **notify)
{
	*notify = &ar_hdmi_rx_notify[name];
}

//call camera callback
void _hdmi_rx_notify_format(unsigned char name)
{
	void * pra = (void *)&ar_hdmi_rx_notify[name].hdmi_notify_pra;
	int size = sizeof(ar_hdmi_rx_notify[name].hdmi_notify_pra);
	void *usr_data = ar_hdmi_rx_notify[name].hdmi_format_notif_data;
	notify_t notify_code = 0;
	ar_always("%p", ar_hdmi_rx_notify[name].hdmi_format_notify);
	ar_always("%s", ar_hdmi_rx_notify[name].hdmi_notify_pra.video_format.is_interlace ? "INTERLACE" : "PROG");
	ar_always("%d", ar_hdmi_rx_notify[name].hdmi_notify_pra.audio_format.rate);
	ar_always("hdmi_src %d hight %d", ar_hdmi_rx_notify[name].hdmi_notify_pra.e_data_src, ar_hdmi_rx_notify[name].hdmi_notify_pra.video_format.height);
	ar_hdmi_rx_notify[name].hdmi_format_notify(notify_code, ar_hdmi_rx_notify[name].hdmi_notify_t, pra, size, usr_data); //send to camera-server info
}

static void hdmi_rx_pre_notify_data(unsigned char name, STRU_VIDEO_FORAMT_NOTITY_PRA_T *video_format, STRU_AUDIO_FORAMT_NOTITY_PRA_T *audio_format)
{
	struct video_format_info_t hdmi_video;
	struct audio_format_info_t hdmi_audio;
	struct ar_hdmi_rx_notify_t *hdmi_rx_notify = &ar_hdmi_rx_notify[name];
	hdmi_rx_notify->drv->ops->get_vformat(&hdmi_video);
	hdmi_rx_notify->drv->ops->get_aformat(&hdmi_audio);

	switch (hdmi_video.format) {
		case HDMI_RX_OUT_MODE_RGB444:
			video_format->format = STREAM_FORMAT_RAW_RGB_888;
			break;
		case HDMI_RX_OUT_MODE_YUV422:
			video_format->format = STREAM_FORMAT_YUV422_8BIT;
			break;
		case HDMI_RX_OUT_MODE_YUV444:
			video_format->format = STREAM_FORMAT_YUV444_8BIT;
			break;
		default:
			ar_err("get_vformat err\n");
			memset(video_format, 0, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
			memset(audio_format, 0, sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));
			return;
	}

	video_format->height = hdmi_video.height;
	video_format->width = hdmi_video.width;
	video_format->fps_num = hdmi_video.fps_num;
	video_format->fps_den = hdmi_video.fps_den;
	video_format->is_interlace = hdmi_video.is_interlace;
	audio_format->channels = hdmi_audio.channels;
	audio_format->rate = hdmi_audio.rate;
	audio_format->format = hdmi_audio.format;
	ar_always("rate %d format %d channels %d", audio_format->rate, audio_format->format, audio_format->channels);
	ar_always("h %d w %d fps %d %d %d", video_format->height, video_format->width, \
    video_format->fps_num,video_format->fps_den,video_format->is_interlace);
}




void hdmi_rx_notify_format(struct ar_hdmi_rx_notify_t *notify_t)
{
	ar_func_enter();
	STRU_VIDEO_FORAMT_NOTITY_PRA_T video_format_info;
	STRU_AUDIO_FORAMT_NOTITY_PRA_T audio_format_info;
	unsigned char name = notify_t->drv->name;
	struct ar_hdmi_rx_notify_t *hdmi_rx_notify = &ar_hdmi_rx_notify[name];
	uint8_t state = hdmi_rx_notify->drv->ops->check_av_state();

    ar_always("enter %d",state);
	if (NULL == hdmi_rx_notify->hdmi_format_notify) {
		ar_always("NULL");
		//return;
	}

	/* todo need handle video only case */
	if (HDMI_RX_UNSTABLE != state) {
		hdmi_rx_pre_notify_data(name, &video_format_info, &audio_format_info);
	} else {
		if (hdmi_rx_notify->hdmi_notify_t == NOTIFY_EVENT_DISCONNECT) {
			return;
		}
	}

	if (hdmi_rx_notify->hdmi_notify_t == NOTIFY_EVENT_DISCONNECT) {
		if (state == HDMI_RX_AVSTABLE) {
			hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_CONNECT;
			hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO_VIDEO;
			memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
			memcpy(&hdmi_rx_notify->hdmi_notify_pra.audio_format, &audio_format_info, sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));
			_hdmi_rx_notify_format(name);
		} else if (state == HDMI_RX_VIDEO_O) {
			hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_CONNECT;
			hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_VIDEO;
			memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
			memset(&hdmi_rx_notify->hdmi_notify_pra.audio_format, 0, sizeof(hdmi_rx_notify->hdmi_notify_pra.audio_format));

			_hdmi_rx_notify_format(name);
		}
	} else if (hdmi_rx_notify->hdmi_notify_t == NOTIFY_EVENT_CONNECT || hdmi_rx_notify->hdmi_notify_t == NOTIFY_EVENT_FORAMAT_CHANGE) {
		if (state == HDMI_RX_UNSTABLE) {
			hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_DISCONNECT;
			hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO_VIDEO;

			_hdmi_rx_notify_format(name);
		}
		if (state == HDMI_RX_AVSTABLE) {
			int video_change = 1, audio_change = 1;
			video_change = memcmp(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
			audio_change = memcmp(&hdmi_rx_notify->hdmi_notify_pra.audio_format, &audio_format_info, sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));
			/* if video format is change, assume audio video is all change */
            if (video_change == 0 && audio_change == 0) {
                ar_func_exit();
                return;
            }
			if (video_change != 0 && audio_change != 0)
				hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO_VIDEO;
			else if (video_change != 0)
				hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_VIDEO;
			else if (audio_change != 0)
				hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO;
			else
				hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO_VIDEO;
			/* vif issue, if hdmi not stable,  even video is not changed, assume all change */
			hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_FORAMAT_CHANGE;
			memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
			memcpy(&hdmi_rx_notify->hdmi_notify_pra.audio_format, &audio_format_info, sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));

			_hdmi_rx_notify_format(name);
		} else if (state == HDMI_RX_VIDEO_O) {
				int video_change = 1;
				video_change = memcmp(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
				/* if video format is change, assume audio video is all change */
				if (video_change != 0) {
					hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_VIDEO;
					hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_FORAMAT_CHANGE;
					memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
					memset(&hdmi_rx_notify->hdmi_notify_pra.audio_format, 0, sizeof(hdmi_rx_notify->hdmi_notify_pra.audio_format));

					_hdmi_rx_notify_format(name);
				}
		}
	}
	ar_func_exit();
}


int ___register_hdmi_notify(unsigned char name, void *data, ptf_notify_call_back cb)
{
	ar_func_enter();
	if (name >= HDMI_RX_MAX) {
		ar_err("register hdmi nottify error");
		return 1;
	}
	struct ar_hdmi_rx_notify_t *hdmi_rx_notify = &ar_hdmi_rx_notify[name];
	STRU_VIDEO_FORAMT_NOTITY_PRA_T video_format_info;
	STRU_AUDIO_FORAMT_NOTITY_PRA_T audio_format_info;

	if (hdmi_rx_notify->drv == NULL) {
		hdmi_rx_notify->hdmi_format_notify = cb;
		hdmi_rx_notify->hdmi_format_notif_data = data;
	} else {
		int state = hdmi_rx_notify->drv->ops->check_av_state();
		if (NULL == hdmi_rx_notify->hdmi_format_notify) {
			hdmi_rx_notify->hdmi_format_notify = cb; //camera register
			hdmi_rx_notify->hdmi_format_notif_data = data;
			if (HDMI_RX_UNSTABLE != state) {

				hdmi_rx_pre_notify_data(name, &video_format_info, &audio_format_info);
				hdmi_rx_notify->hdmi_notify_t = NOTIFY_EVENT_CONNECT;

				if (HDMI_RX_VIDEO_O == state) {
					hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_VIDEO;
					memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
					memcpy(&hdmi_rx_notify->hdmi_notify_pra.audio_format, &audio_format_info, sizeof(STRU_AUDIO_FORAMT_NOTITY_PRA_T));
				} else {
					hdmi_rx_notify->hdmi_notify_pra.e_data_src = DATA_HDMI_SRC_AUDIO_VIDEO;
					memcpy(&hdmi_rx_notify->hdmi_notify_pra.video_format, &video_format_info, sizeof(STRU_VIDEO_FORAMT_NOTITY_PRA_T));
					memset(&hdmi_rx_notify->hdmi_notify_pra.audio_format, 0, sizeof(hdmi_rx_notify->hdmi_notify_pra.audio_format));
				}
				_hdmi_rx_notify_format(name);
		    }
	    }
	}

 	ar_func_exit();
	return 0;
}

int register_hdmi_notify_dvp_in(void *data, ptf_notify_call_back cb)
{
	ar_func_enter();
	return ___register_hdmi_notify(HDMI_RX_ITE, data, cb);

}

int register_hdmi_notify(void *data, ptf_notify_call_back cb)
{
	ar_func_enter();
	return ___register_hdmi_notify(HDMI_RX_DW, data, cb);
}


