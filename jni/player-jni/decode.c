
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

static void* audio_decode_thread(void* para) {
    AVPacket* pkt;
    int size, err;
    Samples* sam;
    int count;
    int64_t time, bgn, end;

    err = SetThreadPriority(8);
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
            sam->samples = size > 0 ? av_malloc(size) : 0;
            if (size)
                memcpy(sam->samples, gCtx->samples, size);
            sam->format = gCtx->audio_ctx->sample_fmt;
            sam->pts = pkt->pts;
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
    int latency;
    int skip_sched, skip_total, skip_count;
    int dt, et, left, show;

    err = SetThreadPriority(10);
    if (err < 0)
        debug("warning: failed to set video decode thread priority!\n");

    pkt = 0;
    pic = 0;
    count = 0;
    time = 0;
    fps = (int)(gCtx->fps);
    step = (fps >> 2);
    skip_sched = 0;
    skip_total = 0;
    skip_count = 0;
    dt = 0;
    et = (int)(1000 / gCtx->fps);
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
            latency = (int)((gCtx->audio_last_pts * gCtx->audio_time_base - gCtx->video_last_pts * gCtx->video_time_base) * fps);
            if (count > step) {
                dt = (gCtx->avg_video_decode_time + gCtx->avg_video_display_time) / 1000;
                skip_sched = (dt < et) ? 0 : (fps * (dt- et) / et + 1);
                skip_total += skip_sched;
            }
            if (!(count % step)) {
                skip_count += ((skip_total << 1) / step);
                skip_count = (latency > skip_count) ? latency : skip_count;
                skip_count = (latency < 0) ? skip_count >> 1 : skip_count;
                if (skip_count) {
                    left = picture_queue_size();
                    if (left)
                        skip_count /= left;
                }
                debug("scheduled skip %d in %d, total %d, latency %d\n", (skip_total / fps), step, skip_count, latency);
                skip_total = 0;
            }
            show = -1;
            /*if (skip_count) {
                gCtx->video_ctx->flags |= CODEC_FLAG_LOW_DELAY;
                gCtx->video_ctx->skip_frame = AVDISCARD_NONREF;
                gCtx->video_ctx->skip_idct = AVDISCARD_NONREF;
                gCtx->video_ctx->skip_loop_filter = AVDISCARD_NONREF;
                skip_count--;
            }
            else {*/
                gCtx->video_ctx->flags &= (~CODEC_FLAG_LOW_DELAY);
                gCtx->video_ctx->skip_frame = AVDISCARD_DEFAULT;
                gCtx->video_ctx->skip_idct = AVDISCARD_DEFAULT;
                gCtx->video_ctx->skip_loop_filter = AVDISCARD_DEFAULT;
            //}
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
                pic->pts = pkt->pts;
                picture_queue_push_head(pic);
            }
next:
            end = av_gettime();
            count++;
            if (count >= step) {
                gCtx->avg_video_decode_time = time / step;
                time -= temp[(count - 1) % step];
            }
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

    if (gCtx->audio_enabled) {
        pthread_attr_init(&attr);
        pthread_attr_getschedpolicy(&attr, &policy);
        if (policy != SCHED_RR) {
            policy = SCHED_RR;
            pthread_attr_setschedpolicy(&attr, policy);
        }
        priority = sched_get_priority_max(policy);
        priority = priority ? 85 : 0;
        para.sched_priority = priority;
        pthread_attr_setschedparam(&attr, &para);
        pthread_create(&adtid, 0, audio_decode_thread, 0);
        pthread_attr_destroy(&attr);
    }
    if (gCtx->video_enabled) {
        pthread_attr_init(&attr);
        pthread_attr_getschedpolicy(&attr, &policy);
        if (policy != SCHED_RR) {
            policy = SCHED_RR;
            pthread_attr_setschedpolicy(&attr, policy);
        }
        priority = sched_get_priority_max(policy);
        priority = priority ? 90 : 0;
        para.sched_priority = priority;
        pthread_attr_setschedparam(&attr, &para);
        pthread_create(&vdtid, &attr, video_decode_thread, 0);
        pthread_attr_destroy(&attr);
    }
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

