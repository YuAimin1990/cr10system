#ifndef _AR_GE2D_H_
#define _AR_GE2D_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* AR_GE2D_HANDLE;

typedef enum
{
   AR_COLOR_FMT_ARGB8888 = 0,
   AR_COLOR_FMT_ARGB4444,
   AR_COLOR_FMT_ARGB1555,
   AR_COLOR_FMT_YV12,
   AR_COLOR_FMT_NV12,
   AR_COLOR_FMT_MONOCHROMA,
   AR_COLOR_FMT_GRAY1,
   AR_COLOR_FMT_GRAY4,
   AR_COLOR_FMT_GRAY8,
} AR_GE2D_COLOR_FORMAT;

typedef struct {
    void* pa;
    int w;
    int h;
    int stride;
    AR_GE2D_COLOR_FORMAT format;

    // TODO
} AR_GE2D_SURFACE;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} AR_GE2D_RECT;

typedef struct {
    int use_global_alpha;
    int alpha;

    unsigned int colorkey_src;
    unsigned int colorkey_dst;

    AR_GE2D_RECT clip_rect;
    // TODO
} AR_GE2D_OPT;

int AR_GE2D_Open(void);
int AR_GE2D_Close(void);
AR_GE2D_HANDLE AR_GE2D_BeginJob(void);
int AR_GE2D_EndJob(AR_GE2D_HANDLE gid);
int AR_GE2D_Bitblit(AR_GE2D_HANDLE gid, AR_GE2D_SURFACE* src, AR_GE2D_RECT* rect_src, AR_GE2D_SURFACE* dst, AR_GE2D_RECT* rect_dst, AR_GE2D_OPT* opt);
int AR_GE2D_SolidDraw(AR_GE2D_HANDLE gid, AR_GE2D_SURFACE* surface, AR_GE2D_RECT* rect, int color, AR_GE2D_OPT* opt);

#ifdef __cplusplus
}
#endif

#endif //_AR_GE2D_H_
