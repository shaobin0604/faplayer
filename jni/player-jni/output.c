
#include "output.h"

#include "ao.h"
#include "vo.h"
#include "player.h"
#include "queue.h"

#include <pthread.h>

static ao_t* ao = 0;
static vo_t* vo = 0;

static int stop = 0;

static pthread_t aotid = 0;
static pthread_t votid = 0;

static void* audio_output_thread(void* para) {
    Samples* sam;
    int64_t cnt, total = 0;

    if (gCtx->video_enabled) {
        pthread_mutex_lock(&gCtx->start_mutex);
        while (!gCtx->start)
            pthread_cond_wait(&gCtx->start_condv, &gCtx->start_mutex);
        pthread_mutex_unlock(&gCtx->start_mutex);
    }

    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        if (gCtx->pause) {
            usleep(25 * 1000);
            continue;
        }
        sam = samples_queue_pop_tail();
        if (sam) {
            if (ao && ao->play)
                ao->play(sam);
            cnt = sam->size / sam->channel;
            switch (sam->format) {
                case SAMPLE_FMT_U8:
                    cnt = cnt / sizeof(uint8_t);
                    break;
                case SAMPLE_FMT_S16:
                    cnt = cnt / sizeof(int16_t);
                    break;
                case SAMPLE_FMT_S32:
                    cnt = cnt / sizeof(int32_t);
                    break;
                case SAMPLE_FMT_FLT:
                    cnt = cnt / sizeof(float);
                    break;
                case SAMPLE_FMT_DBL:
                    cnt = cnt / sizeof(double);
                    break;
                default:
                    cnt = 0;
                    break;
            }
            total += cnt;
            gCtx->audio_last_pts = (sam->pts != 0) ? sam->pts : ((double)total / (double)(sam->rate));
            free_Samples(sam);
        }
    }

    return 0;
}

static void* video_output_thread(void* para) {
    Picture* pic;
    int64_t bgn, end, left;
    double diff, factor;
    int err, count;
    int64_t vb, ve, vt;

    pthread_mutex_lock(&gCtx->start_mutex);
    while (!gCtx->start)
        pthread_cond_wait(&gCtx->start_condv, &gCtx->start_mutex);
    pthread_mutex_unlock(&gCtx->start_mutex);

    vt = 0;
    count = 0;
    diff = 0;
    factor = 1.0;
    for (;;) {
        if (stop) {
            pthread_exit(0);
        }
        if (gCtx->pause) {
            usleep(25 * 1000);
            continue;
        }
        bgn = av_gettime();
        pic = picture_queue_pop_tail();
        if (pic) {
            if (vo && vo->display) {
                count++;
                // wait until surface is ready
                while (-1) {
                    vb = av_gettime();
                    err = vo->display(pic);
                    ve = av_gettime();
                    if (!err)
                        break;
                }
                vt += (ve - vb);
                gCtx->avg_video_display_time = vt / count;
            }
            gCtx->video_last_pts = pic->pts;
            free_Picture(pic);
        }
        end = av_gettime();
        left = (int64_t)(1000 * 1000 / gCtx->fps) - end + bgn;
        if (gCtx->audio_enabled) {
            diff = gCtx->audio_last_pts - gCtx->video_last_pts;
            factor = 1.0;
            if (diff > 0)
                factor = 0;
            else
                factor = 1.0 - diff / 2;
            debug("cur a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts, gCtx->video_last_pts, diff);
            if (left > 0 && factor > 0) {
                usleep((int64_t)(left * factor));
            }
        }
    }

    return 0;
}

int output_init() {
    char *audio, *video;

    if (ao || vo || aotid || votid)
        return -1;
    if (gCtx->audio_enabled) {
        ao_register_all();
        ao = ao_find_by_name("android");
        if (!ao || ao->init() < 0) {
            output_free();
            return -1;
        }
    }
    if (gCtx->video_enabled || gCtx->subtitle_enabled) {
        vo_register_all();
        vo = vo_find_by_name("android");
        if (!vo || vo->init() < 0) {
            output_free();
            return -1;
        }
    }

    stop = 0;

    aotid = 0;
    votid = 0;

    if (gCtx->audio_enabled)
        pthread_create(&aotid, 0, audio_output_thread, 0);
    if (gCtx->video_enabled || gCtx->subtitle_enabled)
        pthread_create(&votid, 0, video_output_thread, 0);

    return 0;
}

void output_free() {
    stop = -1;
    if (aotid) {
        samples_queue_wake();
        pthread_join(aotid, 0);
        aotid = 0;
    }
    if (votid) {
        picture_queue_wake();
        pthread_join(votid, 0);
        votid = 0;
    }
    if (ao) {
        ao->free();
        ao = 0;
    }
    if (vo) {
        vo->free();
        vo = 0;
    }
}

