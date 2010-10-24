
#include "decode.h"

#include "player.h"
#include "queue.h"

#include <pthread.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <math.h>

#define MAX_PICTURE 30
#define MAX_SAMPLES 30

extern int SetThreadPriority(int);

static int stop = 0;

static pthread_t adtid = 0;
static pthread_t vdtid = 0;
static pthread_t sdtid = 0;
static pthread_t sytid = 0;

static void* audio_decode_thread(void* para) {
    AVPacket* pkt;
    int size, err;
    Samples* sam;
    int count;
    int64_t time, bgn, end;

    err = SetThreadPriority(9);
    if (err < 0)
        debug("warning: failed to set audio decode thread priority!\n");

    count = 0;
    time = 0;
    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        if (samples_queue_size() >= MAX_SAMPLES) {
            usleep(25 * 1000);
            continue;
        }
        pkt = audio_packet_queue_pop_tail();
        if (pkt) {
            size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
            bgn = av_gettime();
            err = avcodec_decode_audio3(gCtx->audio_ctx, gCtx->samples, &size, pkt);
            if (err < 0) {
                debug("avcodec_decode_audio3 fail: %d\n", err);
                size = 0;
            }
            sam = av_malloc(sizeof(Samples));
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
    }

    return 0;
}

static void* video_decode_thread(void* para) {
    int err, got, has;
    AVPacket* pkt;
    Picture *pic;
    int count;
    int fps, step;
    int64_t time, bgn, end;
    int64_t temp[16];
    int show;

    err = SetThreadPriority(10);
    if (err < 0)
        debug("warning: failed to set video decode thread priority!\n");

    pkt = 0;
    pic = 0;
    count = 0;
    time = 0;
    fps = (int)(gCtx->fps);
    step = (fps >> 2);
    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        if (picture_queue_size() > MAX_PICTURE) {
            usleep(25 * 1000);
            continue;
        }
        bgn = av_gettime();
        pkt = video_packet_queue_pop_tail();
        if (pkt) {
            show = -1;
            pthread_mutex_lock(&gCtx->skip_mutex);
            if (gCtx->skip_count > 0) {
                gCtx->video_ctx->flags |= CODEC_FLAG_LOW_DELAY;
                gCtx->video_ctx->skip_frame = gCtx->skip_level;
                gCtx->video_ctx->skip_idct = gCtx->skip_level;
                gCtx->video_ctx->skip_loop_filter = AVDISCARD_ALL;
                gCtx->skip_count--;
                if (gCtx->skip_level > AVDISCARD_NONREF)
                    show = 0;
            }
            else {
                gCtx->video_ctx->flags &= (~CODEC_FLAG_LOW_DELAY);
                gCtx->video_ctx->skip_frame = AVDISCARD_DEFAULT;
                gCtx->video_ctx->skip_idct = AVDISCARD_DEFAULT;
                gCtx->video_ctx->skip_loop_filter = AVDISCARD_DEFAULT;
                gCtx->skip_level = AVDISCARD_DEFAULT;
                gCtx->skip_count = 0;
            }
            debug("skip level %d skip count %d\n", gCtx->skip_level, gCtx->skip_count);
            pthread_mutex_unlock(&gCtx->skip_mutex);
            err = avcodec_decode_video2(gCtx->video_ctx, gCtx->frame, &got, pkt);
            if (err < 0) {
                debug("avcodec_decode_video2 fail: %d\n", err);
                goto next;
            }
            if (!show)
                goto next;
            if (got) {
                pic = av_malloc(sizeof(Picture));
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
                if (pkt->pts != AV_NOPTS_VALUE) {
                    pic->pts = pkt->pts * gCtx->video_time_base;
                } else if (pkt->dts != AV_NOPTS_VALUE) {
                    pic->pts = pkt->dts * gCtx->video_time_base;
                } else {
                    pic->pts = 0;
                }
                picture_queue_push_head(pic);
            }
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

    return 0;
}

static void* subtitle_decode_thread(void* para) {
    AVPacket* pkt;

    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        pkt = subtitle_packet_queue_pop_tail();
        if (pkt) {
            av_free_packet(pkt);
            av_free(pkt);
        }
    }

    return 0;
}

static void* synchronize_thread(void* para) {
    int idx, fps, step;
    int sched, etime, dtime;
    double temp[16];
    double diff, total, adif;
    double judge;

    idx = 0;
    fps = (int)(gCtx->fps);
    step = fps >> 2;
    etime = 1000 / fps;
    total = 0;

    pthread_mutex_lock(&gCtx->start_mutex);
    while ((gCtx->audio_enabled && (samples_queue_size() < step)) || (gCtx->video_enabled && (picture_queue_size() < step)))
        usleep(5 * 1000);
    gCtx->start = -1;
    pthread_cond_broadcast(&gCtx->start_condv);
    pthread_mutex_unlock(&gCtx->start_mutex);

    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        idx++;
        //
        dtime = (gCtx->avg_video_decode_time + gCtx->avg_video_display_time) / 1000;
        sched = (dtime > etime) ? (fps - 1000 / dtime) : 0;
        //
        diff = gCtx->audio_last_pts - gCtx->video_last_pts;
        if (idx >= step) {
            adif = total / step;
            total -= temp[(idx - 1) % step];
        }
        total += diff;
        temp[(idx - 1) % step] = diff;
        judge = (adif + diff * 4.0) / 5.0 + (double)(sched << 1) / (double)(fps);
        debug("etime %d dtime %d sched %d diff %.3f adif %.3f judge %.3f\n", etime, dtime, sched, diff, adif, judge);
        pthread_mutex_lock(&gCtx->skip_mutex);
        if (floor(judge) > 0) {
            if (!gCtx->skip_count) {
                gCtx->skip_level = AVDISCARD_NONREF;
                gCtx->skip_count = ceil(judge);
            }
        }
        else {
            gCtx->skip_level = AVDISCARD_DEFAULT;
            gCtx->skip_count = 0;
        }        
        pthread_mutex_unlock(&gCtx->skip_mutex);
        usleep(1000 * 1000 / fps);
    }

    return 0;
}

int decode_init() {
    int policy, priority;
    pthread_attr_t attr;
    struct sched_param para;

    if (!gCtx || adtid || vdtid || sdtid)
        return -1;

    stop = 0;

    adtid = 0;
    vdtid = 0;
    sdtid = 0;
    sytid = 0;

    if (gCtx->audio_enabled) {
        pthread_create(&adtid, 0, audio_decode_thread, 0);
    }
    if (gCtx->video_enabled) {
        pthread_create(&vdtid, 0, video_decode_thread, 0);
    }
    if (gCtx->subtitle_enabled)
        pthread_create(&sdtid, 0, subtitle_decode_thread, 0);
    if (gCtx->audio_enabled && gCtx->video_enabled)
        pthread_create(&sytid, 0, synchronize_thread, 0);

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
    if (sytid) {
        pthread_join(sytid, 0);
        sytid = 0;
    }
}

