
#include "decode.h"
#include "player.h"
#include "queue.h"
#include "utility.h"

#include <pthread.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <math.h>

#define MAX_PICTURE 30
#define MAX_SAMPLES 30

static int stop = 0;

static pthread_t adtid = 0;
static pthread_t vdtid = 0;
static pthread_t sdtid = 0;

static void* audio_decode_thread(void* para) {
    AVPacket* pkt;
    int size, err;
    Samples* sam;
    int count;
    int64_t time, bgn, end;

    set_thread_priority(9);

    count = 0;
    time = 0;
    for (;;) {
        if (stop)
            break;
        if (samples_queue_size() + audio_frame_queue_size() >= MAX_SAMPLES) {
            usleep(25 * 1000);
            continue;
        }
        pkt = audio_packet_queue_pop_tail();
        if (!pkt)
            continue;
        size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
        bgn = av_gettime();
        err = avcodec_decode_audio3(gCtx->audio_ctx, gCtx->samples, &size, pkt);
        if (err < 0) {
            debug("avcodec_decode_audio3 fail: %d\n", err);
            size = 0;
        }
        sam = av_mallocz(sizeof(Samples));
        sam->rate = gCtx->audio_ctx->sample_rate;
        sam->channel = gCtx->audio_ctx->channels;
        sam->size = size;
        sam->samples = av_malloc(size);
        if (!sam->samples) {
            av_free(sam);
            goto next;
        }
        memcpy(sam->samples, gCtx->samples, size);
        sam->format = gCtx->audio_ctx->sample_fmt;
        if (pkt->pts != AV_NOPTS_VALUE)
            sam->pts = pkt->pts * gCtx->audio_time_base;
        else if (pkt->dts != AV_NOPTS_VALUE)
            sam->pts = pkt->dts * gCtx->audio_time_base;
        else
            sam->pts = 0;
        samples_queue_push_head(sam);
        end = av_gettime();
        count++;
        time += (end - bgn);
        gCtx->avg_audio_decode_time = time / count;
next:
        free_AVPacket(pkt);
    }

    debug("ad thread exit");

    return 0;
}

static void* video_decode_thread(void* para) {
    int err, got;
    AVPacket* pkt;
    Picture *pic;
    int count;
    int fps, step;
    int64_t time, bgn, end;
    int64_t temp[100];
    double pts;

    set_thread_priority(10);

    pkt = 0;
    pic = 0;
    count = 0;
    time = 0;
    fps = (int)(gCtx->fps);
    step = (fps >> 2);
    for (;;) {
        if (stop)
            break;
        if (picture_queue_size() + video_frame_queue_size() > MAX_PICTURE) {
            usleep(25 * 1000);
            continue;
        }
        pkt = video_packet_queue_pop_tail();
        if (!pkt)
            continue;
        bgn = av_gettime();
        gCtx->video_ctx->skip_frame = gCtx->frame_skip ? AVDISCARD_NONREF : AVDISCARD_DEFAULT;
        gCtx->video_ctx->skip_idct = gCtx->frame_skip ? AVDISCARD_NONREF : AVDISCARD_DEFAULT;
        gCtx->video_ctx->skip_loop_filter = gCtx->frame_skip ? AVDISCARD_ALL : AVDISCARD_DEFAULT;
        err = avcodec_decode_video2(gCtx->video_ctx, gCtx->frame, &got, pkt);
        if (err < 0) {
            debug("avcodec_decode_video2 fail: %d\n", err);
            goto next;
        }
        if (pkt->pts != AV_NOPTS_VALUE) {
            pts = pkt->pts * gCtx->video_time_base;
        } else if (pkt->dts != AV_NOPTS_VALUE) {
            pts = pkt->dts * gCtx->video_time_base;
        } else {
            pts = 0;
        }
        if (got) {
            // not always drop :)
            if (gCtx->frame_drop && count % gCtx->frame_drop == 0) {
                if (picture_queue_size() + video_frame_queue_size() == 0)
                    gCtx->video_last_pts = pts;
                goto next;
            }
            pic = av_mallocz(sizeof(Picture));
            if (!pic)
                goto next;
            err = avpicture_alloc(&pic->picture, gCtx->video_ctx->pix_fmt, gCtx->video_ctx->width, gCtx->video_ctx->height);
            if (err < 0) {
                av_free(pic);
                goto next;
            }
            av_picture_copy(&pic->picture, (AVPicture*) gCtx->frame, gCtx->video_ctx->pix_fmt, gCtx->video_ctx->width, gCtx->video_ctx->height);
            pic->width = gCtx->video_ctx->width;
            pic->height = gCtx->video_ctx->height;
            pic->format = gCtx->video_ctx->pix_fmt;
            pic->pts = pts;
            picture_queue_push_head(pic);
next:
            end = av_gettime();
            count++;
            if (count >= step) {
                gCtx->avg_video_decode_time = time / step;
                time -= temp[(count - 1) % step];
            }
            else
                gCtx->avg_video_decode_time = time / count;
            time += (end - bgn);
            temp[(count - 1) % step] = (end - bgn);
            free_AVPacket(pkt);
        }
    }

    debug("vd thread exit");

    return 0;
}

static void* subtitle_decode_thread(void* para) {
    AVPacket* pkt;

    for (;;) {
        if (stop)
            break;
        pkt = subtitle_packet_queue_pop_tail();
        if (pkt) {
            av_free_packet(pkt);
            av_free(pkt);
        }
    }

    debug("sd thread exit");

    return 0;
}

int decode_init() {
    if (!gCtx || adtid || vdtid || sdtid)
        return -1;

    stop = 0;

    adtid = 0;
    vdtid = 0;
    sdtid = 0;

    if (gCtx->audio_enabled)
        pthread_create(&adtid, 0, audio_decode_thread, 0);
    if (gCtx->video_enabled)
        pthread_create(&vdtid, 0, video_decode_thread, 0);
    if (gCtx->subtitle_enabled)
        pthread_create(&sdtid, 0, subtitle_decode_thread, 0);

	return 0;
}

void decode_free() {
    stop = -1;
    if (adtid) {
        audio_packet_queue_wake();
        pthread_join(adtid, 0);
        adtid = 0;
    }
    if (vdtid) {
        video_packet_queue_wake();
        pthread_join(vdtid, 0);
        vdtid = 0;
    }
    if (sdtid) {
        subtitle_packet_queue_wake();
        pthread_join(sdtid, 0);
        sdtid = 0;
    }
}

