
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
    int idx;
    int64_t inc;

    idx = 0;
    inc = 0;
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
            idx++;
            if (ao && ao->play)
                ao->play(sam);
            if (sam->pts >= 0) {
                gCtx->audio_last_pts = sam->pts;
                inc = sam->pts / idx;
            }
            else
                gCtx->audio_last_pts += inc;
            free_Samples(sam);
        }
    }

    return 0;
}

static void* video_output_thread(void* para) {
    Picture* pic;
    int64_t bgn, end, left;
    double temp[16];
    double diff, total, avg, factor;
    int idx, fps, step, count;
    int64_t inc, vb, ve, vt;

    idx = 0;
    inc = 0;
    vt = 0;
    count = 0;
    fps = (int)(gCtx->fps);
    step = (fps >> 2);
    total = 0;
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
            idx++;
            if (vo && vo->display) {
                count++;
                vb = av_gettime();
                vo->display(pic);
                ve = av_gettime();
                vt += (ve - vb);
                gCtx->avg_video_display_time = vt / count;
            }
            if (pic->pts >= 0) {
                gCtx->video_last_pts = pic->pts;
                inc = pic->pts / idx;
            }
            else
                gCtx->video_last_pts += (int64_t)(1.0 / gCtx->fps / gCtx->video_time_base);
            free_Picture(pic);
        }
        end = av_gettime();
        left = (int64_t)(1000 * 1000 / gCtx->fps) - end + bgn;
        if (gCtx->audio_enabled) {
            if (idx >= fps) {
                avg = total / step;
                total -= temp[(idx - 1) % step];
                debug("avg a/v diff %.3f since last %d output\n", avg, step);
                if (avg > 0.2)
                    factor = 0;
                else if (avg > -0.2)
                    factor = 1.0;
                else
                    factor = 1.0 - avg / 2;
            }
            diff = gCtx->audio_last_pts * gCtx->audio_time_base - gCtx->video_last_pts * gCtx->video_time_base;
            if (idx >= (fps - step))
                total += diff;
            temp[(idx - 1) % step] = diff;
            debug("cur a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts * gCtx->audio_time_base, gCtx->video_last_pts * gCtx->video_time_base, diff);
            if (left > 0) {
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

