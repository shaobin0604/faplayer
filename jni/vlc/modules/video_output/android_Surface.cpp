
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

extern "C" void *getSurface();

static int Open (vlc_object_t *);
static void Close(vlc_object_t *);

vlc_module_begin()
    set_shortname("Android")
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
    void *surface;
    int format;
    int width;
    int height;
    int stride;
    void *bits;
    picture_t *picture;
    picture_pool_t *pool;
};

static int Open(vlc_object_t *object) {
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys;

    // get surface infomation
    Surface *surf = (Surface*) getSurface();
    if (!surf) {
        msg_Err(vd, "android surface is not ready");
        return VLC_EGENERIC;
    }
    Surface::SurfaceInfo info;
    surf->lock(&info);
    surf->unlockAndPost();
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
        fmt.i_rmask = 0x001f;
        fmt.i_gmask = 0x07e0;
        fmt.i_bmask = 0xf800;
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
    sys->surface = surf;
    sys->format = info.format;
    sys->width = info.w;
    sys->height = info.h;
#if __PLATFORM__ < 5
    sys->stride = 0;
#else
    sys->stride = info.s;
#endif
    sys->bits = info.bits;
    sys->picture = NULL;
    sys->pool = NULL;
    vd->sys = sys;
    vd->info.has_hide_mouse = true;
    vd->fmt = fmt;
    vd->pool = Pool;
    vd->prepare = NULL;
    vd->display = Display;
    vd->control = Control;
    vd->manage  = Manage;
    vout_display_SendEventFullscreen(vd, false);
    vout_display_SendEventDisplaySize(vd, sys->width, sys->height, false);
    return VLC_SUCCESS;
}

static void Close(vlc_object_t *object) {
    vout_display_t *vd = (vout_display_t *)object;
    vout_display_sys_t *sys = vd->sys;

    if (sys->picture)
        picture_Release(sys->picture);
    if (sys->pool)
        picture_pool_Delete(sys->pool);
    free(sys);
}

static picture_pool_t *Pool(vout_display_t *vd, unsigned count) {
    VLC_UNUSED(count);
    vout_display_sys_t *sys = vd->sys;
    picture_resource_t rsc;

    if (!sys->pool) {
        if (!sys->picture) {
            memset(&rsc, 0, sizeof(rsc));
            rsc.p[0].p_pixels = (uint8_t*)(sys->bits);
            rsc.p[0].i_pitch  = sys->stride ? sys->stride : sys->width;
            rsc.p[0].i_lines  = vd->fmt.i_height;
            sys->picture = picture_NewFromResource(&vd->fmt, &rsc);
            if (!sys->picture)
                return NULL;
        }
        sys->pool = picture_pool_NewFromFormat(&vd->fmt, count);
    }
    return sys->pool;
}

static void Display(vout_display_t *vd, picture_t *picture) {
    vout_display_sys_t *sys = vd->sys;
    Surface *surf = (Surface*)(sys->surface);
    Surface::SurfaceInfo info;

    surf->lock(&info);
    picture_Copy(sys->picture, picture);
    surf->unlockAndPost();
    picture_Release(picture);
}

static int Control(vout_display_t *vd, int query, va_list args) {
    VLC_UNUSED(vd);
    VLC_UNUSED(query);
    VLC_UNUSED(args);

    return VLC_EGENERIC;
}

static void Manage(vout_display_t *vd) {
    VLC_UNUSED(vd);
}

