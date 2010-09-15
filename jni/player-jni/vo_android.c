
#include <pthread.h>
#include <libswscale/swscale.h>

#include "vo.h"
#include "player.h"

extern void createSurfaceLock();
extern void destroySurfaceLock();
extern void lockSurface();
extern void unlockSurface();
extern int getSurfaceWidth();
extern int getSurfaceHeight();
extern void* getSurfaceBuffer();

static void* buffer = 0;
static struct SwsContext* convt = 0;
static struct SwsContext* scale_fit = 0;
static struct SwsContext* scale_fil = 0;

static int vo_init_android() {
    createSurfaceLock();
    return 0;
}

static int vo_display_android(Picture* pic) {
    AVPicture dest;
    void* screen;
    int sw, sh, ssz, psz;
    int left, top, i;
    int nw, nh;
    double rw, rh;
    struct SwsContext* ctx;
    int64_t bgn, end;

    if (!pic->width || !pic->height)
        return 0;
    lockSurface();
    sw = getSurfaceWidth();
    sh = getSurfaceHeight();
    screen = getSurfaceBuffer();
    if (sw && sh && screen) {
        ssz = sw * sh;
        psz = pic->width * pic->height;
        if (!buffer) {
            buffer = av_malloc(ssz > psz ? ssz * 2 : psz * 2);
            if (!buffer) {
                unlockSurface();
                return 0;
            }
        }
        if (gCtx->mode == 0 && pic->width <= sw && pic->height <= sh) {
            // convert to rgb565 only
            nw = pic->width;
            nh = pic->height;
            convt = sws_getCachedContext(convt, pic->width, pic->height, pic->format, pic->width, pic->height, PIX_FMT_RGB565, SWS_FAST_BILINEAR, 0, 0, 0);
            ctx = convt;
        }
        else if (gCtx->mode == 2) {
            // fill the surface
            nw = sw;
            nh = sh;
            scale_fil = sws_getCachedContext(scale_fil, pic->width, pic->height, pic->format, nw, nh, PIX_FMT_RGB565, SWS_FAST_BILINEAR, 0, 0, 0);
            ctx = scale_fil;
        }
        else {
            // fit to the surface
            rw = ((double)sw / (double)pic->width);
            rh = ((double)sh / (double)pic->height);
            if (rw > rh) {
                nw = pic->width * sh / pic->height;
                nh = pic->height * sh / pic->height;
            }
            else {
                nw = pic->width * sw / pic->width;
                nh = pic->height * sw / pic->width;
            }
            scale_fit = sws_getCachedContext(scale_fit, pic->width, pic->height, pic->format, nw, nh, PIX_FMT_RGB565, SWS_FAST_BILINEAR, 0, 0, 0);
            ctx = scale_fit;
        }
        bgn = av_gettime();
        avpicture_fill(&dest, buffer, PIX_FMT_RGB565, nw, nh);
        sws_scale(ctx, (const uint8_t * const*) pic->picture.data, pic->picture.linesize, 0, pic->height, dest.data, dest.linesize);
        end = av_gettime();
        debug("sws_scale takes %lld us\n", end - bgn);
        left = (sw - nw) / 2;
        top = (sh - nh) /2;
        if (top) {
            memset(screen, 0, top * sw * 2);
            memset(screen + (sh - top) * sw, 0, top * sw * 2);
        }
        if (left) {
            for (i = top; i < sh - top; i++) {
                memset(screen + i * sw * 2, 0, left * 2);
                memcpy(screen + i * sw * 2 + left * 2, buffer + (i - top) * nw * 2, nw * 2);
                memset(screen + i * sw * 2 + left * 2 + nw * 2, 0, left * 2);
            }
        }
        else
            memcpy(screen + top * sw * 2, buffer, nw * nh * 2);
    }
    else {
        unlockSurface();
        return -1;
    }
    unlockSurface();
    return 0;
}

static void vo_free_android() {
    sws_freeContext(convt);
    convt = 0;
    sws_freeContext(scale_fit);
    scale_fit = 0;
    sws_freeContext(scale_fil);
    scale_fil = 0;
    if (buffer) {
        av_free(buffer);
        buffer = 0;
    }
    destroySurfaceLock();
}

vo_t vo_android = {
    .name = "android",
    .init = vo_init_android,
    .display = vo_display_android,
    .free = vo_free_android,
};

