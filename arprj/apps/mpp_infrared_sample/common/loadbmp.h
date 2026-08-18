#ifndef     __LOAD_BMP_H__
#define     __LOAD_BMP_H__

#include "hal_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


/* the color format OSD supported */
typedef enum arOSD_COLOR_FMT_E
{
    OSD_COLOR_FMT_RGB444    = 0,
    OSD_COLOR_FMT_RGB4444   = 1,
    OSD_COLOR_FMT_RGB555    = 2,
    OSD_COLOR_FMT_RGB565    = 3,
    OSD_COLOR_FMT_RGB1555   = 4,
    OSD_COLOR_FMT_RGB888    = 6,
    OSD_COLOR_FMT_RGB8888   = 7,
    OSD_COLOR_FMT_BUTT
} OSD_COLOR_FMT_E;

typedef struct arOSD_RGB_S
{
    AR_U8   u8B;
    AR_U8   u8G;
    AR_U8   u8R;
    AR_U8   u8Reserved;
} OSD_RGB_S;

typedef struct arOSD_SURFACE_S
{
    OSD_COLOR_FMT_E enColorFmt;         /* color format */
    AR_U8*  pu8PhyAddr;               /* physical address */
    AR_U16  u16Height;                /* operation height */
    AR_U16  u16Width;                 /* operation width */
    AR_U16  u16Stride;                /* surface stride */
    AR_U16  u16Reserved;
} OSD_SURFACE_S;

typedef struct tag_OSD_Logo
{
    AR_U32    width;        /* out */
    AR_U32    height;       /* out */
    AR_U32    stride;       /* in */
    AR_U8*    pRGBBuffer;   /* in/out */
} OSD_LOGO_T;

typedef struct tag_OSD_BITMAPINFOHEADER
{
    AR_U16      biSize;
    AR_U32       biWidth;
    AR_S32       biHeight;
    AR_U16       biPlanes;
    AR_U16       biBitCount;
    AR_U32      biCompression;
    AR_U32      biSizeImage;
    AR_U32       biXPelsPerMeter;
    AR_U32       biYPelsPerMeter;
    AR_U32      biClrUsed;
    AR_U32      biClrImportant;
} OSD_BITMAPINFOHEADER;

typedef struct tag_OSD_BITMAPFILEHEADER
{
    AR_U32   bfSize;
    AR_U16    bfReserved1;
    AR_U16    bfReserved2;
    AR_U32   bfOffBits;
} OSD_BITMAPFILEHEADER;

typedef struct tag_OSD_RGBQUAD
{
    AR_U8    rgbBlue;
    AR_U8    rgbGreen;
    AR_U8    rgbRed;
    AR_U8    rgbReserved;
} OSD_RGBQUAD;

typedef struct tag_OSD_BITMAPINFO
{
    OSD_BITMAPINFOHEADER    bmiHeader;
    OSD_RGBQUAD                 bmiColors[1];
} OSD_BITMAPINFO;

typedef struct arOSD_COMPONENT_INFO_S
{
    int alen;
    int rlen;
    int glen;
    int blen;
} OSD_COMP_INFO;

AR_S32 LoadImage(const AR_CHAR* filename, OSD_LOGO_T* pVideoLogo);
AR_S32 LoadBitMap2Surface(const AR_CHAR* pszFileName, const OSD_SURFACE_S* pstSurface, AR_U8* pu8Virt);
AR_S32 CreateSurfaceByBitMap(const AR_CHAR* pszFileName, OSD_SURFACE_S* pstSurface, AR_U8* pu8Virt);
AR_S32 CreateSurfaceByCanvas(const AR_CHAR* pszFileName, OSD_SURFACE_S* pstSurface, AR_U8* pu8Virt, AR_U32 u32Width, AR_U32 u32Height, AR_U32 u32Stride);
AR_S32 GetBmpInfo(const AR_CHAR* filename, OSD_BITMAPFILEHEADER*  pBmpFileHeader, OSD_BITMAPINFO* pBmpInfo);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __LOAD_BMP_H__*/

