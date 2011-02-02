
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

struct vout_display_sys_t {
    int format;
    int width;
    int height;
#if __PLATFORM__ > 4
    int stride;
#endif
    int mode;
    int x, y;
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
    fmt.i_width  = info.w;
    fmt.i_height = info.h;
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
    sys->pool = NULL;
    vd->sys = sys;
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
    //vout_display_SendEventFullscreen(vd, true);
    vout_display_SendEventDisplaySize(vd, sys->width, sys->height, true);
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
    VLC_UNUSED(count);
    vout_display_sys_t *sys = vd->sys;
    picture_resource_t rsc;

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
                surface.p[0].i_pixel_pitch = 1;
                surface.p[0].i_visible_lines = info.h;
                surface.p[0].i_visible_lines = info.w << 1;
                picture_CopyPixels(&surface, picture);
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

