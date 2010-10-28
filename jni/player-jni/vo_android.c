
#include <pthread.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <jni.h>

#include "vo.h"
#include "vo_android.h"
#include "player.h"

extern JavaVM* jvm;

static pthread_mutex_t* lock = 0;
static jobject holder = 0;
static jint width = 0, height = 0;
static jint* screen = 0;
static jobject colors = 0;
static jmethodID mid_SurfaceHolder_lockCanvas = 0;
static jmethodID mid_SurfaceHolder_unlockCanvasAndPost = 0;
static jmethodID mid_Canvas_drawBitmap = 0;

static void* buffer = 0;
static struct SwsContext* cvt = 0; 

static void vo_lock() {
    if (!lock) {
        lock = malloc(sizeof(pthread_mutex_t));
        if (!lock)
            return;
        pthread_mutex_init(lock, 0);
    }
    pthread_mutex_lock(lock);
}

static void vo_unlock() {
    if (lock)
        pthread_mutex_unlock(lock);
}

jint attach(JNIEnv* env, jobject thiz, jobject obj, jint w, jint h) {
    jint ret = -1;
    jclass clz;
    jintArray array;
    jint* element;

    debug("attach %p %d %d", obj, w, h);

    vo_lock();
    if (holder)
        (*env)->DeleteGlobalRef(env, holder);
    holder = (*env)->NewGlobalRef(env, obj);
    if (!holder)
        goto fail;
    clz = (*env)->FindClass(env, "android/view/SurfaceHolder");
    mid_SurfaceHolder_lockCanvas = (*env)->GetMethodID(env, clz, "lockCanvas", "()Landroid/graphics/Canvas;");
    mid_SurfaceHolder_unlockCanvasAndPost = (*env)->GetMethodID(env, clz, "unlockCanvasAndPost", "(Landroid/graphics/Canvas;)V");
    (*env)->DeleteLocalRef(env, clz);
    clz = (*env)->FindClass(env, "android/graphics/Canvas");
    mid_Canvas_drawBitmap = (*env)->GetMethodID(env, clz, "drawBitmap", "([IIIIIIIZLandroid/graphics/Paint;)V");
    (*env)->DeleteLocalRef(env, clz);
    if (colors) {
        element = (*env)->GetIntArrayElements(env, colors, 0);
        (*env)->ReleaseIntArrayElements(env, (jintArray) colors, element, 0);
        (*env)->DeleteGlobalRef(env, colors);
    }
    array = (*env)->NewIntArray(env, w * h);
    if (!array)
        goto fail;
    colors = (*env)->NewGlobalRef(env, array);
    if (!colors) {
        element = (*env)->GetIntArrayElements(env, array, 0);
        (*env)->ReleaseIntArrayElements(env, array, element, 0);
        goto fail;
    }
    width = w;
    height = h;
    screen = (*env)->GetIntArrayElements(env, array, 0);
    ret = 0;
fail:
    vo_unlock();

    return ret;
}

void detach(JNIEnv* env, jobject thiz) {
    jint* element;

    vo_lock();
    if (holder) {
        //(*env)->DeleteGlobalRef(env, holder);
        holder = 0;
    }
    if (colors) {
        element = (*env)->GetIntArrayElements(env, colors, 0);
        (*env)->ReleaseIntArrayElements(env, (jintArray) colors, element, 0);
        (*env)->DeleteGlobalRef(env, colors);
        colors = 0;
    }
    vo_unlock();
}

// scretch a line
static void copyrow4(uint32_t *src, int src_w, uint32_t *dst, int dst_w)
{
    int i;
    int pos, inc;
    uint32_t pixel = 0;

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

// scretch from a rect to another
// src width, height, x, y, w, h
// dst width, height, x, y, w, h
static void scretch4(uint32_t* src, int sw, int sh, int srx, int sry, int srw, int srh, uint32_t* dst, int dw, int dh, int drx, int dry, int drw, int drh) {
    int pos, inc;
    int dst_max_row;
    int src_row, dst_row;
    uint32_t *srcp, *dstp;

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
        copyrow4(srcp, srw, dstp, drw);
        pos += inc;
    }
}

static int vo_init_android() {
    return 0;
}

static int vo_display_android(Picture* pic, void* extra) {
    int ret = -1, mode;
    AVPicture dest;
    int sw, sh, psz;
    int x, y, i, j;
    int nw, nh, rw, rh;
    struct SwsContext* ctx;
    JNIEnv* env = (JNIEnv*) extra;
    jobject canvas;

    if (!pic)
        return 0;
    if (!buffer) {
        psz = pic->width * pic->height * (sizeof(uint32_t));
        buffer = av_malloc(psz);
        if (!buffer)
            return 0;
    }
    vo_lock();
    if (!holder || !colors)
        goto fail;
    if (colors) {
        sw = width;
        sh = height;
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
        memset(screen, 0, sw * sh * sizeof(uint32_t));
        cvt = sws_getCachedContext(cvt, pic->width, pic->height, pic->format, pic->width, pic->height, PIX_FMT_ARGB, SWS_POINT, 0, 0, 0);
        avpicture_fill(&dest, buffer, PIX_FMT_ARGB, pic->width, pic->height);
        sws_scale(cvt, (const uint8_t * const*) pic->picture.data, pic->picture.linesize, 0, pic->height, dest.data, dest.linesize);
        x = (sw - nw) / 2;
        y = (sh - nh) / 2;
        scretch4(buffer, pic->width, pic->height, 0, 0, pic->width, pic->height, screen, sw, sh, x, y, nw, nh);
        if (holder) {
            canvas = (*env)->CallObjectMethod(env, holder, mid_SurfaceHolder_lockCanvas);
            // JNI_TRUE for ARGB8888 and JNI_FALSE for RGB565
            (*env)->CallVoidMethod(env, canvas, mid_Canvas_drawBitmap, colors, 0, sw, 0, 0, sw, sh, JNI_TRUE, 0);
            (*env)->CallVoidMethod(env, holder, mid_SurfaceHolder_unlockCanvasAndPost, canvas);
            (*env)->DeleteLocalRef(env, canvas);
        }
    }
    ret = 0;
fail:
    vo_unlock();

    return ret;
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
}

vo_t vo_android = {
    .name = "android",
    .init = vo_init_android,
    .display = vo_display_android,
    .free = vo_free_android,
};

