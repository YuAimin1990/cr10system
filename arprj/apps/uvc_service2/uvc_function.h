#ifndef __UVC_FUNCTION_H__
#define __UVC_FUNCTION_H__

void uvc_events_init(struct uvc_device *dev);
int uvc_video_init(struct uvc_device *dev __attribute__((__unused__)));
void uvc_events_process(struct uvc_device *dev);
int uvc_video_process(struct uvc_device *dev);
int uvc_video_send(struct uvc_device *dev, void * data, unsigned int len, unsigned int pts, fd_set wfds, struct timeval tv);
int uvc_video_set_format(struct uvc_device *dev);

#endif
