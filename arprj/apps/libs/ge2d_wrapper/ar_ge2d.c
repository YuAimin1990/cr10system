#include "ar_ge2d.h"
#include "ge2d_api.h"
#include "utils_debug.h"
#include "ar_mmap.h"

#include <fcntl.h>

#if defined LOG_TAG
#undef      LOG_TAG
#define     LOG_TAG  "ge2d"
#endif

#define GE2D_SEM "ge2d_sem"
//#define GE2D_DEBUG 1

typedef struct {
    ge2d_server_t *ge2d_server;
    ge2d_client_t *ge2d;
    frame_cmd_t *cmd;
    sem_t *sem;
} ge2d_obj_t;

static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
static ge2d_obj_t g_ge2d_obj = {0};
static inline ge2d_obj_t *get_ge2d_obj()
{
    return &g_ge2d_obj;
}

int AR_GE2D_Open()
{
    ge2d_obj_t* obj = get_ge2d_obj();
    static int initialized = 0;
    int ret = -1;

    pthread_mutex_lock(&global_mutex);
    if (!initialized) {
        ret = ar_mmap_init();
        if (ret) {
            pthread_mutex_unlock(&global_mutex);
            return -1;
        }

        ret = ar_dev_map_init();
        if (ret) {
            pthread_mutex_unlock(&global_mutex);
            return -1;
        }

        obj->ge2d_server = get_ge2d_server();

        obj->sem = sem_open(GE2D_SEM, O_CREAT|O_RDWR, 0666, 1);
        if (obj->sem == SEM_FAILED) {
            perror("sem_open");
            return -1;
        }

        initialized = 1;
    }
    pthread_mutex_unlock(&global_mutex);

    obj->ge2d = obj->ge2d_server->creat_client(obj->ge2d_server, "gui");

    return obj->ge2d == NULL ? -1 : 0;
}

int AR_GE2D_Close()
{
    ge2d_obj_t* obj = get_ge2d_obj();

    sem_close(obj->sem);
    sem_unlink(GE2D_SEM);

    return obj->ge2d_server->delete_client(obj->ge2d_server, obj->ge2d);
}

AR_GE2D_HANDLE AR_GE2D_BeginJob()
{
    ge2d_obj_t* obj = get_ge2d_obj();

    obj->cmd = obj->ge2d->malloc_frame(obj->ge2d);

    return obj;
}

int AR_GE2D_EndJob(AR_GE2D_HANDLE gid)
{
    ge2d_obj_t* obj = get_ge2d_obj();

    return obj->ge2d->free_frame(obj->ge2d, obj->cmd);
}

static void ge2d_format_convert(AR_GE2D_COLOR_FORMAT src_fmt, ge2d_format_t* dst_format)
{
    switch (src_fmt) {
    case AR_COLOR_FMT_ARGB8888:
        *dst_format = ARGB8888;
        break;
    case AR_COLOR_FMT_ARGB4444:
        *dst_format = ARGB4444;
        break;
    case AR_COLOR_FMT_ARGB1555:
        *dst_format = ARGB1555;
        break;
    case AR_COLOR_FMT_YV12:
        *dst_format = YV12;
        break;
    case AR_COLOR_FMT_NV12:
        *dst_format = NV12;
        break;
    case AR_COLOR_FMT_MONOCHROMA:
        *dst_format = MONOCHROMA;
        break;
    case AR_COLOR_FMT_GRAY1:
        *dst_format = GRAY1;
        break;
    case AR_COLOR_FMT_GRAY4:
        *dst_format = GRAY4;
        break;
    case AR_COLOR_FMT_GRAY8:
        *dst_format = GRAY8;
        break;
    default:
        break;
    }
}

int AR_GE2D_Bitblit(AR_GE2D_HANDLE gid, AR_GE2D_SURFACE* surface_src, AR_GE2D_RECT* rect_src,
                    AR_GE2D_SURFACE* surface_dst, AR_GE2D_RECT* rect_dst, AR_GE2D_OPT* opt)
{
    ge2d_format_t format;
    ge2d_obj_t* obj = (ge2d_obj_t*)gid;
    if (!obj || !surface_src || !rect_src || !surface_dst || !rect_dst || !opt) {
        log_tag_err("ge2d handle is invalid");
        return -1;
    }

    ge2d_format_convert(surface_src->format, &format);

    int y_stride = surface_dst->stride;
    int uv_stride = y_stride/2;
    int y_plane_size = surface_dst->w * surface_dst->h;
    int c_plane_size = y_plane_size/2;
    void* y_addr = surface_dst->pa;
    void* u_addr = surface_dst->pa + y_plane_size;
    void* v_addr = u_addr + c_plane_size;
#ifdef GE2D_DEBUG
    log_tag_info("AR_GE2D_Bitblit, stride = %d, w = %d, h = %d, x = %d, y = %d, rw = %d, rh = %d",
                  surface_src->stride, surface_src->w, surface_src->h,
                  rect_src->x, rect_src->y, rect_src->w, rect_src->h);
    log_tag_info("AR_GE2D_Bitblit, stride = %d, w = %d, h = %d, x = %d, y = %d, rw = %d, rh = %d",
                  surface_dst->stride, surface_dst->w, surface_dst->h,
                  rect_dst->x, rect_dst->y, rect_dst->w, rect_dst->h);
    log_tag_info("AR_GE2D_Bitblit, format = %d, alpha = %d, global = %d",
                  surface_src->format, opt->alpha, opt->use_global_alpha);
#endif

    // sem -1
    sem_wait(obj->sem);
    obj->ge2d->client_set_format(obj->ge2d, surface_dst->w, surface_dst->h, format, y_stride, uv_stride);
    obj->ge2d->frame_add_start(obj->ge2d, obj->cmd, y_addr, u_addr, v_addr);

    bmp_data_t data;
    data.use_global_alfa = opt->use_global_alpha; // TODO
    data.alfa = opt->alpha;
    data.dest_x = rect_dst->x;
    data.dest_y = rect_dst->y;
    data.format = format;
    data.x = rect_src->x;
    data.y = rect_src->y;
    data.w = rect_dst->w;
    data.h = rect_dst->h;
    data.src_w = rect_src->w;
    data.src_h = rect_src->h;
    data.stride = surface_src->stride;
    data.uv_stride = surface_src->stride/2;
    data.y_addr = surface_src->pa;
    data.u_addr = surface_src->pa + surface_src->w * surface_src->h;
    data.v_addr = data.u_addr + surface_src->w * surface_src->h/2;

    obj->ge2d->frame_add_bmp(obj->ge2d, obj->cmd, &data);

    obj->ge2d->frame_add_end(obj->ge2d, obj->cmd);

    int ret = obj->ge2d->process_frame(obj->ge2d, obj->cmd);
    sem_post(obj->sem); //sem +1

    return ret;
}

int AR_GE2D_SolidDraw(AR_GE2D_HANDLE gid, AR_GE2D_SURFACE* surface,
                      AR_GE2D_RECT* rect, int color, AR_GE2D_OPT* opt)
{
    ge2d_format_t format;
    ge2d_obj_t* obj = (ge2d_obj_t*)gid;
    if (!obj || !surface || !rect || !opt) {
        log_tag_err("ge2d handle is invalid");
        return -1;
    }

    ge2d_format_convert(surface->format, &format);

    int y_stride = surface->stride;
    int uv_stride = y_stride/2;
    int y_plane_size = surface->w * surface->h;
    int c_plane_size = y_plane_size/2;
    void* y_addr = surface->pa;
    void* u_addr = surface->pa + y_plane_size;
    void* v_addr = u_addr + c_plane_size;

#ifdef GE2D_DEBUG
    log_tag_info("AR_GE2D_SolidDraw, stride = %d, w = %d, h = %d, alpha = %d, global = %d",
                 surface->stride, surface->w, surface->h, opt->alpha, opt->use_global_alpha);
    log_tag_info("AR_GE2D_SolidDraw, x = %d, y = %d, w = %d, h = %d, color = %x",
                  rect->x, rect->y, rect->w, rect->h, color);
#endif

    // sem -1
    sem_wait(obj->sem);
    obj->ge2d->client_set_format(obj->ge2d, surface->w, surface->h, format, y_stride, uv_stride);
    obj->ge2d->frame_add_start(obj->ge2d, obj->cmd, y_addr, u_addr, v_addr);

    line_data_t data;
    data.alfa = color >> 24;//opt->alpha;
    data.rgb = color & 0x00ffffff;
    data.use_global_alfa = opt->use_global_alpha; // TODO
    obj->ge2d->frame_add_line(obj->ge2d, obj->cmd, &data);

    line_append_data_t append_data;
    append_data.alfa_en = 1;
    append_data.end_line = 1;
    append_data.end_x = rect->x + rect->w/2;
    append_data.end_y = rect->y + rect->h;
    append_data.start_x = rect->x + rect->w/2;
    append_data.start_y = rect->y;
    append_data.line_w = rect->w;

    obj->ge2d->frame_append_line(obj->ge2d, obj->cmd, &append_data);

    obj->ge2d->frame_add_end(obj->ge2d, obj->cmd);

    int ret = obj->ge2d->process_frame(obj->ge2d, obj->cmd);
    sem_post(obj->sem); //sem +1

    return ret;
}
