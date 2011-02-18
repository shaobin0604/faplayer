
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_plugin.h>
#include <vlc_vout_display.h>
#include <vlc_picture_pool.h>

#ifndef __PLATFORM__
#error "android api level is not defined!"
#endif

#if __PLATFORM__ < 8
#include <ui/Surface.h>
#else
#include <surfaceflinger/Surface.h>
#endif

using namespace android;

extern "C" void LockSurface();
extern "C" void UnlockSurface();
extern "C" void *GetSurface();

static int Open (vlc_object_t *);
static void Close(vlc_object_t *);

vlc_module_begin()
    set_shortname("AndroidSurface")
    set_category(CAT_VIDEO)
    set_subcategory(SUBCAT_VIDEO_VOUT)
    set_description(N_("Android Surface video output"))
    set_capability("vout display", __PLATFORM__)
    add_shortcut("android")
    set_callbacks(Open, Close)
vlc_module_end()

static picture_pool_t *Pool(vout_display_t *, unsigned);
static void Display(vout_display_t *, picture_t *);
static int Control(vout_display_t *, int, va_list);
static void Manage(vout_display_t *);

static void picture_Strech2(vout_display_t *, picture_t *, picture_t *);

struct vout_display_sys_t {
    vout_display_place_t place;
    int format;
    int width;
    int height;
#if __PLATFORM__ > 4
    int stride;
#endif
    int w, h;
    picture_t *picture;
    picture_pool_t *pool;
};

static int Open(vlc_object_t *object) {
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;

    // get surface infomation
    LockSurface();
    Surface *surf = (Surface*) GetSurface();
    if (!surf) {
        UnlockSurface();
        msg_Err(vd, "android surface is not ready");
        return VLC_EGENERIC;
    }
    Surface::SurfaceInfo info;
    surf->lock(&info);
    surf->unlockAndPost();
    UnlockSurface();
    video_format_t fmt = vd->fmt;
    // TODO: what about the other formats?
    const char *chroma_format;
    switch (info.format) {
    case PIXEL_FORMAT_RGB_565:
        chroma_format = "RV16";
        break;
    default:
        msg_Err(vd, "unknown chroma format %d", info.format);
        return VLC_EGENERIC;
    }    
    vlc_fourcc_t chroma = vlc_fourcc_GetCodecFromString(VIDEO_ES, chroma_format);
    if (!chroma) {
        msg_Err(vd, "unsupported chroma format %s", chroma_format);
        return VLC_EGENERIC;
    }
    fmt.i_chroma = chroma;
    switch (chroma) {
    case VLC_CODEC_RGB16:
        fmt.i_rmask = 0xf800;
        fmt.i_gmask = 0x07e0;
        fmt.i_bmask = 0x001f;
        break;
    default:
        fmt.i_rmask = 0;
        fmt.i_gmask = 0;
        fmt.i_bmask = 0;
        break;
    }
    sys = (struct vout_display_sys_t*)malloc(sizeof(struct vout_display_sys_t));
    if (!sys)
        return VLC_ENOMEM;
    sys->format = info.format;
    sys->width = info.w;
    sys->height = info.h;
#if __PLATFORM__ > 4
    sys->stride = info.s;
#endif
    sys->w = fmt.i_width;
    sys->h = fmt.i_height;
#if __PLATFORM__ > 4
    msg_Dbg(object, "SurfaceInfo w = %d, h = %d, s = %d", sys->width, sys->height, sys->stride);
#else
    msg_Dbg(object, "SurfaceInfo w = %d, h = %d", sys->width, sys->height);
#endif
    sys->pool = NULL;
    vd->sys = sys;
    vout_display_cfg_t cfg = *vd->cfg;
    cfg.display.width = info.w;
    cfg.display.height = info.h;
    vout_display_PlacePicture(&sys->place, &vd->source, &cfg, true);
    vd->info.has_hide_mouse = true;
    //vd->info.is_slow = true;
    // uncomment to disable dr
    //vd->info.has_pictures_invalid = true;
    vd->fmt = fmt;
    vd->pool = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    vout_display_SendEventDisplaySize(vd, sys->width, sys->height, vd->cfg->is_fullscreen);
    return VLC_SUCCESS;
}

static void Close(vlc_object_t *object) {
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;

    if (sys->pool)
        picture_pool_Delete(sys->pool);
    free(sys);
}

static picture_pool_t *Pool(vout_display_t *vd, unsigned count) {
    vout_display_sys_t *sys = vd->sys;

    if (!sys->pool) {
        sys->pool = picture_pool_NewFromFormat(&vd->fmt, count);
    }
    return sys->pool;
}

static void Display(vout_display_t *vd, picture_t *picture) {
    vout_display_sys_t *sys = vd->sys;
    Surface *surf;
    Surface::SurfaceInfo info;

    LockSurface();
    surf = (Surface*)(GetSurface());
    if (surf) {
        surf->lock(&info);
        if (sys->format != info.format ||
            sys->width != info.w ||
#if __PLATFORM__ > 4
            sys->height != info.h ||
            sys->stride != info.s)
#else
            sys->height != info.h)
#endif
            goto bail;
        switch (info.format) {
        case PIXEL_FORMAT_RGB_565: {
                picture_t surface;

                memset(&surface, 0, sizeof(surface));
                surface.i_planes = 1;
                surface.p[0].p_pixels = (uint8_t*)(info.bits);
                surface.p[0].i_lines = info.h;
#if __PLATFORM__ > 4
                surface.p[0].i_pitch = info.s << 1;
#else
                surface.p[0].i_pitch = info.w << 1;
#endif
                surface.p[0].i_pixel_pitch = 2;
                surface.p[0].i_visible_lines = info.h;
                surface.p[0].i_visible_pitch = info.w << 1;
                picture_Strech2(vd, &surface, picture);
            }
        default:
            break;
        }
bail:
        surf->unlockAndPost();
    }
    UnlockSurface();

    picture_Release(picture);
}

static int Control(vout_display_t *vd, int query, va_list args) {
    return VLC_EGENERIC;
}

static void Manage(vout_display_t *vd) {
    VLC_UNUSED(vd);
}

static inline void copyrow2(uint16_t *src, int src_w, uint16_t *dst, int dst_w) {
    int i;
    int pos, inc;
    uint16_t pixel = 0;

    pos = 0x10000;
    inc = (src_w << 16) / dst_w;
    for (i = dst_w; i > 0; --i) {
        while (pos >= 0x10000) {
            pixel = *src++;
            pos -= 0x10000;
        }
        *dst++ = pixel;
        pos += inc;
    }
}

static void picture_Strech2(vout_display_t *vd, picture_t *dst_p, picture_t *src_p) {
    vout_display_sys_t *sys = vd->sys;
    vout_display_place_t *place = &sys->place;

    // TODO: shall i handle all the planes?
    if (src_p->i_planes != dst_p->i_planes) {
        msg_Dbg(VLC_OBJECT(vd), "FIXME: src_p->i_planes != dst_p->i_planes");
        return;
    }
    if (src_p->i_planes != 1 || dst_p->i_planes != 1)
        return;
    //for (int i = 0; i < src->i_planes; i++) {
    //    if (src->p[i].i_pixel_pitch != dst->p[i].i_pixel_pitch)
    //        return;
    //}
    if ((sys->width < place->x + place->width) || (sys->height < place->y + place->height)) {
        msg_Dbg(VLC_OBJECT(vd), "Surface %dx%d, place %d, %d %dx%d, out of region", sys->width, sys->height, place->x, place->y, place->width, place->height);
        return;
    }
    // TODO: shall i handle all the planes?
    plane_t *sp = &src_p->p[0];
    plane_t *dp = &dst_p->p[0];
    uint16_t *src, *dst;
    uint16_t *srcp, *dstp;
    int sw, sh, ss, dw, dh, ds;
    int srx, sry, srh, srw;
    int drx, dry, drh, drw;
    int pos, inc;
    int dst_max_row;
    int src_row, dst_row;

    src = (uint16_t*)sp->p_pixels;
    dst = (uint16_t*)dp->p_pixels;
    sw = sp->i_visible_pitch / sp->i_pixel_pitch;
    sh = sp->i_visible_lines;
    ss = sp->i_pitch / sp->i_pixel_pitch;
    srx = 0;
    sry = 0;
    srw = sp->i_visible_pitch / sp->i_pixel_pitch;
    srh = sp->i_visible_lines;
    dw = dp->i_visible_pitch / dp->i_pixel_pitch;
    dh = dp->i_visible_lines;
    ds = dp->i_pitch / dp->i_pixel_pitch;
    drx = place->x;
    dry = place->y;
    drw = place->width;
    drh = place->height;
    //msg_Dbg(VLC_OBJECT(vd), "%dx%d %d %d,%d %dx%d -> %dx%d %d %d,%d %dx%d", sw, sh, ss, srx, sry, srw, srh, dw, dh, ds, drx, dry, drw, drh);
    pos = 0x10000;
    inc = (srh << 16) / drh;
    src_row = sry;
    dst_row = dry;

    for (dst_max_row = dst_row + drh; dst_row < dst_max_row; ++dst_row ) {
        while (pos >= 0x10000) {
            srcp = src + src_row * ss + srx;
            ++src_row;
            pos -= 0x10000;
        }
        dstp = dst + dst_row * ds + drx;
        copyrow2(srcp, srw, dstp, drw);
        pos += inc;
    }
}

