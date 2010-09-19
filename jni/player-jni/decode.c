
#include "decode.h"

#include "player.h"
#include "queue.h"

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

    pkt = 0;
    pic = 0;
    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        if (picture_queue_size() > MAX_PICTURE) {
            usleep(25 * 1000);
            continue;
        }
        pkt = video_packet_queue_pop_tail();
        if (pkt) {
            pthread_mutex_lock(&gCtx->skip_mutex);
            if (!gCtx->skip_count) {
                gCtx->video_ctx->flags &= (~CODEC_FLAG_LOW_DELAY);
                gCtx->video_ctx->skip_idct = AVDISCARD_DEFAULT;
                gCtx->video_ctx->skip_frame = AVDISCARD_DEFAULT;
            }
            if (gCtx->skip_count > 0) {
                gCtx->skip_count--;
                gCtx->video_ctx->flags |= CODEC_FLAG_LOW_DELAY;
                gCtx->video_ctx->skip_idct = gCtx->skip_level;
                gCtx->video_ctx->skip_frame = gCtx->skip_level;
            }
            pthread_mutex_unlock(&gCtx->skip_mutex);
            err = avcodec_decode_video2(gCtx->video_ctx, gCtx->frame, &got, pkt);
            if (err < 0) {
                debug("avcodec_decode_video2 fail: %d\n", err);
                goto next;
            }
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
        priority = priority ? 60 : 0;
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

