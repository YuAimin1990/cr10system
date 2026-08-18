/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2016 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2016 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/


#ifndef _VIV_DC_TYPE_H_
#define _VIV_DC_TYPE_H_

 /*
  * Frame buffer format
  */
 typedef enum _InputFormat_e {
	 Input_XRGB4444,
	 Input_ARGB4444,
	 Input_XRGB1555,
	 Input_ARGB1555,
	 Input_RGB565,	// 04
	 Input_XRGB8888,
	 Input_ARGB8888,
	 Input_YUY2, // 07
	 Input_UYVY,
	 Input_INDEX8,
	 Input_MONOCHROME, // 0A
	 Input_YV12 = 0xF,
	 Input_A8, // 0x10
	 Input_NV12,
	 Input_NV16,
	 Input_RG16,
	 Input_R8, // 0x14
	 Input_NV12_10BIT, //0x15
	 Input_ARGB2101010,
	 Input_NV16_10BIT,
	 Input_INDEX1,
	 Input_INDEX2, //0x19
	 Input_INDEX4, //0x1A
	 Input_P010, //0x1B
	 Input_NV12_10BIT_L1, // 0x1C  3P4B
	 Input_NV16_10BIT_L1, //0x1D  3P4B
	 Input_MAX
 }InputFormat_e;

 /*
  * output format type.
  */
typedef enum _viv_dpi_type {
    /* Unknown */
    //vivUNKNOWN,

    /* General */
    //vivARGB1555,
    //vivARGB4444,
    //vivRGB565,
    //vivARGB8888,
    //vivNV12,
    //vivYUY2,

    /* DBI */
    //vivD8R3G3B2,
    //vivD8R4G4B4,
    //vivD8R5G5B5,
    //vivD8R6G6B6,
    //vivD8R8G8B8,
    //vivD9R6G6B6,
    //vivD16R3G3B2,
    //vivD16R4G4B4,
    //vivD16R5G5B5,
    //vivD16R6G6B6OP1,
    //vivD16R6G6B6OP2,
    //vivD16R8G8B8OP1,
    //vivD16R8G8B8OP2,

    /* DPI */
    vivD16CFG1,
    vivD16CFG2,
    vivD16CFG3,
    vivD18CFG1,
    vivD18CFG2,
    vivD24    ,
    vivD30	  ,// ARGB2101010 ?????????????
}
viv_dpi_type;

 /*
  * Frame buffer mode.
  * Used in viv_conf_framebuffer_set_config()
  */
//typedef enum _viv_tiling_type {
//    vivLINEAR,
//    vivTILED,
//}
//viv_tiling_type;

 /*
  * YUV type.
  * Used in viv_conf_yuv_set_config()
  */
typedef enum _viv_yuv_type {
    vivBT601,
    vivBT709,
    vivBTUNKNOWN,
    vivBT2020,
}
viv_yuv_type;

 /*
  * Output type selection.
  * Used in viv_conf_output_select()
  */
//typedef enum _viv_output_type {
//    vivDBI,
//    vivDPI,
//}
//viv_output_type;

 /*
  * DBI type.
  * Used in viv_conf_output_dbi_set_config()
  */
//typedef enum _viv_dbi_type {
//    vivDBI_AFIXED,
//    vivDBI_ACLOCK,
//    vivDBI_B,
//}
//viv_dbi_type;

 /*
  * DBI command flag
  * Used in viv_conf_output_dbi_set_command()
  */
//typedef enum _viv_dbi_command_type {
//    vivDBI_COMMAND_ADDRESS,
//    vivDBI_COMMAND_MEM,
//    vivDBI_COMMAND_DATA,
//}
//viv_dbi_command_type;

 /*
  * Cursor type.
  * Used in viv_conf_cursor_set_type()
  */
typedef enum _viv_cursor_type {
    vivCURSOR_DISABLED,
    vivCURSOR_MASKED,
    vivCURSOR_ARGB8888,
}
viv_cursor_type;

/************************************************************************/
typedef enum _viv_status_type {
    vivSTATUS_HEAP_CORRUPTED = -7,
    vivSTATUS_OUT_OF_RESOURCES = -6,
    vivSTATUS_TIMEOUT = -5,
    vivSTATUS_NOT_SUPPORT = -4,
    vivSTATUS_OOM = -3,
    vivSTATUS_FAILED = -2,
    vivSTATUS_INVALID_ARGUMENTS = -1,

    vivSTATUS_OK = 0,
}
viv_status_type, vivSTATUS;

//typedef enum _viv_file_type {
//    vivFILE_BINARY = 0x001,

//    vivFILE_READ   = 0x010,
//    vivFILE_WRITE  = 0x100,

//    vivFILE_READB  = vivFILE_READ  | vivFILE_BINARY,
//    vivFILE_WRITEB = vivFILE_WRITE | vivFILE_BINARY,
//}
//viv_file_type;

typedef int                     gctBOOL;
typedef int                     gctINT;
typedef unsigned int            gctUINT;
typedef unsigned char           gctUINT8;
typedef signed short            gctINT16;
typedef unsigned short          gctUINT16;
typedef gctUINT16 *                gctUINT16_PTR;
typedef signed int                gctINT32;
typedef unsigned int            gctUINT32;
typedef unsigned long long      gctUINT64;
typedef gctUINT32 *                gctUINT32_PTR;
typedef unsigned long           gctSIZE_T;
typedef char                    gctCHAR;
typedef float                   gctFLOAT;
typedef double                  gctDOUBLE;

typedef void                    gctVOID;
typedef void *                  gctPOINTER;

#define vivFALSE                0
#define vivTRUE                 1

#ifdef __cplusplus
#   define vivNULL              0
#else
#   define vivNULL              ((void *) 0)
#endif

/************************************************************************/

/* For function enable/disable parameter */
#define SET_ENABLE                                  1
#define SET_DISABLE                                 0

/* For polarity parameter */
#define SET_POSITIVE                                0
#define SET_NEGATIVE                                1


//#define vivMAX(a, b) ((a) > (b) ? (a) : (b))
//#define vivMIN(a, b) ((a) < (b) ? (a) : (b))

//#define vivPOW     pow
//#define vivABS     abs
//#define vivATOI    atoi
//#define vivSIZEOF  sizeof

#define vivINFINITE ((gctUINT32)(~0U))

#define VIV_ALIGN(data, offset)                 ((data + offset - 1) & ~(offset - 1))

/*
#define dcONERROR(func) \
    do \
    { \
        status = func; \
        if (status < 0) \
        { \
            goto OnError; \
        } \
    } \
    while (0)*/

#define GAMMA_INDEX_MAX 256

typedef struct {
  unsigned int addr[3];
  void *priv;
}dis_fifo_item;

#define DIS_FIFO_LEN 8
typedef struct {
  dis_fifo_item fifo[DIS_FIFO_LEN];
  int head, tail, cur;
}dis_fifo;

typedef struct _dc_framebuffer {
    //gctUINT YAddress;
    //gctUINT UAddress;
    //gctUINT VAddress;

    dis_fifo_item fb_item;
    gctUINT fb_stride[3];

    int valid; // need set to 0

    //gctUINT fb_phys_addr[3];
    gctUINT fb_format;
    //gctUINT fb_tiling;
    gctUINT fb_yuv_type;
    //gctUINT fb_stride[3];
    //gctUINT fb_astride[3];

    gctUINT rotAngle;

    /* TODO. We don't support alpha blending at the moment. */
    //gctUINT alphaMode;   // ?????
    //gctUINT alphaValue;  // ?????

    gctUINT lut[256]; // ?????

    /* Original size in pixel before rotation and scale. */
    gctUINT width;
    gctUINT height;

    //gctUINT tileMode;
    gctUINT scale;
    gctUINT scaleFactorX; // ????? +++
    gctUINT scaleFactorY; // ????? +++
    gctUINT filterTap;
    gctUINT horizontalFilterTap;

    gctUINT horKernel[128]; // ????? +++
    gctUINT verKernel[128]; // ????? +++

    gctUINT swizzle;
    gctUINT uvSwizzle;

    gctUINT colorKey;
    gctUINT colorKeyHigh;
    gctUINT bgColor;
    gctUINT transparency;
    gctUINT clearFB;
    gctUINT clearValue;

    gctUINT initialOffsetX;
    gctUINT initialOffsetY;

    //gctUINT compressed;
    //gctUINT tileStatusAddress[3];

    //int nPlanes; // ????? +++
    //int bpp[3]; // ????? +++
    //int alignedWidth[3]; // ????? +++
    //int alignedHeight[3]; // ????? +++
    ////int planeHeight[3];
}
dc_framebuffer;

typedef struct _dc_overlay {
    //gctUINT mode; // useless?????
    //gctUINT srcAlphaValue; // useless?????
    gctUINT srcAlphaMode;
    gctUINT srcGlobalAlphaValue;
    gctUINT srcGlobalAlphaMode;
    gctUINT enable;
    gctUINT format;
    int yuv_standard;

    gctUINT address[3];
    //gctUINT tileStatusAddress[3];
    gctUINT stride[3];
    gctUINT tlX;
    gctUINT tlY;
    gctUINT brX;
    gctUINT brY;

    gctUINT rotAngle;
    gctUINT width;
    gctUINT height;
    //gctUINT alignedWidth[3]; // useless?????
    //gctUINT alignedHeight[3]; // useless?????

    gctUINT swizzle;
    gctUINT uvSwizzle;

    gctUINT colorKey;
    gctUINT colorKeyHigh;
    gctUINT transparency;

    gctUINT dstAlphaMode;
    //gctUINT dstAlphaValue; // useless?????

    gctUINT dstGlobalAlphaMode;
    gctUINT dstGlobalAlphaValue;

    int scrBlendingMode;
    int dstBlendingMode;
    int srcAlphaFactor;
    int dstAlphaFactor;

    gctUINT clearOverlay;
    gctUINT clearValue;

    //gctUINT tileMode;

    //gctUINT lut[256];

    //gctUINT compressed;

    //gctUINT scale;
    //gctUINT scaleFactorX;
    //gctUINT scaleFactorY;
    //gctUINT filterTap;
    //gctUINT horizontalFilterTap;
    //gctUINT horKernel[128];
    //gctUINT verKernel[128];

    //gctUINT initialOffsetX;
    //gctUINT initialOffsetY;


}
dc_overlay;


#define DC_OVERLAY_NUM 1

typedef struct _viv_dc_cursor {
    //gctBOOL enable;
    gctUINT phys_addr;
    gctUINT type;
    gctUINT x, y;
    gctUINT hot_x, hot_y;
    gctUINT bg_color, fg_color;
    gctBOOL dirty;
}
viv_dc_cursor;

typedef struct _viv_dc_display {
    /* display */
    gctUINT dpy_hline, dpy_htotal;
    gctUINT dpy_vline, dpy_vtotal;
    gctUINT dpy_hsync_start, dpy_hsync_end;
    gctUINT dpy_vsync_start, dpy_vsync_end;
    gctBOOL dpy_hsync_polarity;
    gctBOOL dpy_vsync_polarity;
    gctBOOL display_dirty;

    /* framebuffer */
    //gctUINT fb_phys_addr[3];
    //gctUINT fb_format;
    //gctUINT fb_tiling;
    //gctUINT fb_yuv_type;
    //gctUINT fb_stride[3];
    //gctUINT fb_astride[3];
    gctBOOL fb_dirty;

    /* panel */
    gctBOOL panel_de_en;
    gctBOOL panel_da_en;
    gctBOOL panel_clock_en;
    gctBOOL panel_dep_polarity;
    gctBOOL panel_dap_polarity;
    gctBOOL panel_clockp_polarity;
    gctBOOL panel_dirty;

    /* gamma correction */
    gctBOOL gamma_enable;
    gctUINT16 gamma[GAMMA_INDEX_MAX][3];
    gctBOOL gamma_dirty;

    /* dither */
    gctBOOL dither_enable;
    gctUINT dither_red_channel;
    gctUINT dither_green_channel;
    gctUINT dither_blue_channel;
    gctUINT dither_table_low;
    gctUINT dither_table_high;
    gctBOOL dither_dirty;

    /* output */
    gctBOOL output_enable;
    //gctUINT output_type;
    //gctUINT output_dbi_type;
    //gctUINT output_dbi_format;
    //gctUINT output_dbi_actime;
    //gctUINT output_dbi_period[2];
    //gctUINT output_dbi_eor_assert[2];
    //gctUINT output_dbi_cs_assert[2];
    //gctBOOL output_dbi_polarity;
    gctUINT output_dpi_format;
    gctBOOL output_dirty;

    /**/
    dc_framebuffer framebuffer;

    /* destination. */
    //dc_dest dest;
    //gctBOOL dest_dirty;

    /* Overlay. */
    dc_overlay overlay[DC_OVERLAY_NUM];
    gctBOOL overlay_dirty;
}
viv_dc_display;


/********************************************************************************
                DVP Out
********************************************************************************/
typedef struct _dvpo_para_s
{
    int dvpo_type; // 0---601, 1---656
    int dvpo_mode; // 0---sdr(16bit only), 1---ddr(8bit only)
    int display_input_width; // 0---8bit data input, 1---10bit data input
    int dvpo_lcd_mode;
    int dvpo_clockphase_ctrl;
}dvpo_para_s;

#endif

