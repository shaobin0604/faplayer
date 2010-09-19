
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
    double diff, total, avg, factor;
    int idx, fps;
    int64_t inc;
    int aq, vq, ad, vd;

    idx = 0;
    inc = 0;
    fps = (int)(gCtx->fps);
    total = 0;
    factor = 1;
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
                vo->display(pic);
            }
            if (pic->pts >= 0) {
                gCtx->video_last_pts = pic->pts;
                inc = pic->pts / idx;
            }
            else
                gCtx->video_last_pts += inc;
            free_Picture(pic);
        }
        end = av_gettime();
        left = (int64_t)(1000 * 1000 / gCtx->fps) - end + bgn;
        if (gCtx->audio_enabled) {
            diff = gCtx->audio_last_pts * gCtx->audio_time_base - gCtx->video_last_pts * gCtx->video_time_base;
            debug("a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts * gCtx->audio_time_base, gCtx->video_last_pts * gCtx->video_time_base, diff);
            total += diff;
            if (idx % (fps >> 2) == 0) {
                avg = total / (fps >> 2);
                total = 0;
                debug("avg diff %.3f in %d\n", avg, (fps >> 2));
                pthread_mutex_lock(&gCtx->skip_mutex);
                if (avg > 0 && (idx > (fps >> 1))) {
                    factor = 0;
                    if (avg < 0.25) {
                        gCtx->skip_count = 0;
                        gCtx->skip_level = AVDISCARD_DEFAULT;
                    }
                    else if (avg < 0.5) {
                        gCtx->skip_count = (fps >> 1);
                        gCtx->skip_level = AVDISCARD_NONREF;
                    }
                    else if (avg < 1.0) {
                        gCtx->skip_count = fps;
                        gCtx->skip_level = AVDISCARD_NONREF;
                    }
                    else if (avg < 2.0) {
                        gCtx->skip_count = (fps << 1);
                        gCtx->skip_level = AVDISCARD_NONREF;
                        debug("a/v out of sync! avg diff %.3f in %d\n", avg, (fps >> 2));
                    }
                    else {
                        gCtx->skip_count = (fps << 2);
                        gCtx->skip_level = AVDISCARD_NONREF;
                        debug("a/v out of sync! avg diff %.3f in %d\n", avg, (fps >> 2));
                    }
                }
                else {
                    factor = 1.0;
                    gCtx->skip_count = 0;
                    gCtx->skip_level = AVDISCARD_DEFAULT;
                    if (avg < -0.5)
                        factor = 1.5;
                    if (avg < -1.0)
                        factor = 3.0;
                }
                pthread_mutex_unlock(&gCtx->skip_mutex);
            }
            if (left > 0) {
                usleep((int)(left * factor));
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

