
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
            gCtx->audio_last_pts = sam->pts;
            free_Samples(sam);
        }
    }

    return 0;
}

static void* video_output_thread(void* para) {
    Picture* pic;
    int64_t bgn, end, left;
    double diff;
    int aq, vq, ad, vd;

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
                vo->display(pic);
            }
            gCtx->video_last_pts = pic->pts;
            free_Picture(pic);
        }
        end = av_gettime();
        left = (int64_t)(1000 * 1000 / gCtx->fps) - end + bgn;
        if (gCtx->audio_enabled) {
            diff = gCtx->audio_last_pts * gCtx->audio_time_base - gCtx->video_last_pts * gCtx->video_time_base;
            debug("a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts * gCtx->audio_time_base, gCtx->video_last_pts * gCtx->video_time_base, diff);
            if (diff >= 0) {
                left = 0;
                if (diff < 0.25) {
                    gCtx->skip_count += 2;
                    gCtx->skip_level = AVDISCARD_NONREF;
                }
                else if (diff < 0.5) {
                    gCtx->skip_count += 4;
                    gCtx->skip_level = AVDISCARD_NONREF;
                }
                else if (diff < 1.0) {
                    if (gCtx->skip_count < 2)
                        gCtx->skip_count = 2;
                    gCtx->skip_level = AVDISCARD_NONKEY;
                }
                else {
                    if (gCtx->skip_count < 4)
                        gCtx->skip_count = 4;
                    gCtx->skip_level = AVDISCARD_NONKEY;
                }
            }
            else {
                if (diff < -0.25) {
                    left += left >> 2;
                }
                if (diff < -0.5)
                    left += left >> 1;
                if (diff < -1.0)
                    left += left;
                else
                    left += left << 1;
            }
        }
        if (left > 0) {
            usleep(left >> 1);
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

