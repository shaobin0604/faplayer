
#include <pthread.h>
#include <libswscale/swscale.h>

#include "vo.h"
#include "player.h"

extern void createSurfaceLock();
extern void destroySurfaceLock();
extern void lockSurface();
extern void unlockSurface();
extern void getSurfaceInfo(int* w, int* h, int* s, void** p);

static void copyrow2(unsigned short *src, int src_w, unsigned short *dst, int dst_w) {
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

static void scretch2(unsigned short* src, int sw, int sh, int sstr, int srx, int sry, int srw, int srh, unsigned short* dst, int dw, int dh, int dstr, int drx, int dry, int drw, int drh) {
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
            srcp = src + src_row * sstr + srx;
            ++src_row;
            pos -= 0x10000;
        }
        dstp = dst + dst_row * dstr + drx;
        copyrow2(srcp, srw, dstp, drw);
        pos += inc;
    }
}

static int vo_init_android() {
    createSurfaceLock();
    return 0;
}

static int vo_display_android(Picture* pic, void* extra) {
    int mode, ret = -1;
    AVPicture dest;
    void *screen = 0;
    int sw = 0, sh = 0, ss;
    int x, y;
    int nw, nh;
    int rw, rh;
    int64_t bgn, end;

    if (!pic || !pic->width || !pic->height || pic->format != PIX_FMT_RGB565)
        return 0;
    lockSurface();
    getSurfaceInfo(&sw, &sh, &ss, &screen);
    if (!sw || !sh || !screen)
        goto fail;
    if (ss == 2)
        ss = sw;
    mode = (gCtx->mode == 0 && pic->width <= sw && pic->height <= sh) ? 0 : gCtx->mode;
    mode = (mode < 0 || mode > 2) ? 2 : mode;
    switch (mode) {
        case 0:
            nw = pic->width;
            nh = pic->height;
            break;
        case 1:
            nw = sw;
            nh = sh;
            break;
        default:
            rw = (sw << 16) / pic->width;
            rh = (sh << 16) / pic->height;
            nw = (rw > rh) ? (pic->width * sh / pic->height) : (pic->width * sw / pic->width);
            nh = (rw > rh) ? (pic->height * sh / pic->height) : (pic->height * sw / pic->width);
            break;
    }
    x = (sw - nw) / 2;
    y = (sh - nh) / 2;
    memset(screen, 0, sw * sh * 2);
    scretch2((uint16_t*) pic->picture.data[0], pic->width, pic->height, pic->width, 0, 0, pic->width, pic->height, screen, sw, sh, ss, x, y, nw, nh);
    ret = 0;
fail:
    unlockSurface();
    return ret;
}

static void vo_free_android() {
    destroySurfaceLock();
}

vo_t vo_android = {
    .name = "android",
    .init = vo_init_android,
    .display = vo_display_android,
    .free = vo_free_android,
};

