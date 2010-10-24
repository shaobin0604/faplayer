
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
static struct SwsContext* cvt = 0; 

static void copyrow2(unsigned short *src, int src_w, unsigned short *dst, int dst_w)
{
    int i;
    int pos, inc;
    unsigned short pixel = 0;

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

static void scretch2(unsigned short* src, int sw, int sh, int srx, int sry, int srw, int srh, unsigned short* dst, int dw, int dh, int drx, int dry, int drw, int drh) {
    int pos, inc;
    int dst_max_row;
    int src_row, dst_row;
    unsigned short *srcp, *dstp;

    pos = 0x10000;
    inc = (srh << 16) / drh;
    src_row = sry;
    dst_row = dry;

    for (dst_max_row = dst_row + drh; dst_row < dst_max_row; ++dst_row ) {
        while (pos >= 0x10000) {
            srcp = src + src_row * sw + srx;
            ++src_row;
            pos -= 0x10000;
        }
        dstp = dst + dst_row * dw + drx;
        copyrow2(srcp, srw, dstp, drw);
        pos += inc;
    }
}

static int vo_init_android() {
    createSurfaceLock();
    return 0;
}

static int vo_display_android(Picture* pic) {
    AVPicture dest;
    void *screen;
    int sw, sh, ssz, psz;
    int x, y, i;
    int nw, nh;
    int rw, rh;
    struct SwsContext* ctx;
    int64_t bgn, end;

    if (!pic || !pic->width || !pic->height)
        return 0;
    lockSurface();
    sw = getSurfaceWidth();
    sh = getSurfaceHeight();
    screen = getSurfaceBuffer();
    if (!sw || !sh || !screen)
        goto fail;
    ssz = sw * sh * 2;
    psz = pic->width * pic->height * 2;
    if (!buffer) {
        buffer = av_malloc(ssz > psz ? ssz : psz);
        if (!buffer)
            goto succ;
    }
    if (gCtx->mode == 0 && pic->width <= sw && pic->height <= sh) {
        // convert only
        nw = pic->width;
        nh = pic->height;
    }
    else if (gCtx->mode == 2) {
        // fill the surface
        nw = sw;
        nh = sh;
    }
    else {
        // fit to the surface
        rw = (sw << 16) / pic->width;
        rh = (sh << 16) / pic->height;
        if (rw > rh) {
            nw = pic->width * sh / pic->height;
            nh = pic->height * sh / pic->height;
        }
        else {
            nw = pic->width * sw / pic->width;
            nh = pic->height * sw / pic->width;
        }
    }
    cvt = sws_getCachedContext(cvt, pic->width, pic->height, pic->format, pic->width, pic->height, PIX_FMT_RGB565, SWS_POINT, 0, 0, 0);
    avpicture_fill(&dest, buffer, PIX_FMT_RGB565, pic->width, pic->height);
    sws_scale(cvt, (const uint8_t * const*) pic->picture.data, pic->picture.linesize, 0, pic->height, dest.data, dest.linesize);
    memset(screen, 0, ssz);
    x = (sw - nw) / 2;
    y = (sh - nh) / 2;
    scretch2(buffer, pic->width, pic->height, 0, 0, pic->width, pic->height, screen, sw, sh, x, y, nw, nh);
succ:
    unlockSurface();
    return 0;
fail:
    unlockSurface();
    return -1;
}

static void vo_free_android() {
    if (cvt) {
        sws_freeContext(cvt);
        cvt = 0;
    }
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

