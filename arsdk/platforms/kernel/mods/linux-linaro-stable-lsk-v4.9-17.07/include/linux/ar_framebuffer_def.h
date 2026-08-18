#ifndef __AR_FRAMEBUFFER_DEF_H
#define __AR_FRAMEBUFFER_DEF_H

#include <linux/fb.h>

typedef struct
{
    unsigned int x;
    unsigned int y;
} arfb_cursor_pos_t;

#define AR_FRAMEBUFFER_IO_CURSOR_SET_ICON           _IOW('F', 0x81, __u32)
#define AR_FRAMEBUFFER_IO_CURSOR_DISPLAY_ENABLE     _IOW('F', 0x82, __u32)
#define AR_FRAMEBUFFER_IO_CMD_CURSOR_SET_POS        _IOW('F', 0x83, arfb_cursor_pos_t)


#endif