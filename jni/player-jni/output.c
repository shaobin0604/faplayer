
#include "output.h"
#include "ao.h"
#include "vo.h"
#include "player.h"
#include "queue.h"
#include "utility.h"
#include <jni.h>
#include <pthread.h>

extern JavaVM* jvm;

static ao_t* ao = 0;
static vo_t* vo = 0;

static int stop = 0;

static pthread_t aotid = 0;
static pthread_t votid = 0;
static pthread_t sytid = 0;

static void* audio_output_thread(void* para) {
    Samples* sam;
    int64_t cnt, total = 0;
    JNIEnv* env;

    set_thread_priority(8);

    (*jvm)->AttachCurrentThread(jvm, &env, 0);

    pthread_mutex_lock(&gCtx->start_mutex);
    while (!gCtx->start)
        pthread_cond_wait(&gCtx->start_condv, &gCtx->start_mutex);
    pthread_mutex_unlock(&gCtx->start_mutex);

    for (;;) {
        if (stop) {
            break;
        }
        if (gCtx->pause) {
            usleep(25 * 1000);
            continue;
        }
        sam = samples_queue_pop_tail();
        if (sam) {
            if (ao && ao->play)
                ao->play(sam, env);
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
    (*jvm)->DetachCurrentThread(jvm);
    pthread_exit(0);

    return 0;
}

static void* video_output_thread(void* para) {
    Picture* pic;
    int64_t bgn, end, left;
    double diff, factor;
    int err, count;
    int64_t vb, ve, vt;
    JNIEnv* env;

    set_thread_priority(8);

    (*jvm)->AttachCurrentThread(jvm, &env, 0);

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
            break;
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
                while (-1 && !stop) {
                    vb = av_gettime();
                    err = vo->display(pic, env);
                    ve = av_gettime();
                    if (!err)
                        break;
                }
                vt += (ve - vb);
                gCtx->avg_video_display_time = vt / count;
                debug("vo time %lld", gCtx->avg_video_display_time);
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
            //debug("cur a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts, gCtx->video_last_pts, diff);
            if (left > 0 && factor > 0) {
                usleep((int64_t)(left * factor));
            }
        }
    }
    (*jvm)->DetachCurrentThread(jvm);
    pthread_exit(0);

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

    if (!(gCtx->audio_enabled && (gCtx->video_enabled || gCtx->subtitle_enabled)))
        pthread_exit(0);

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
        //debug("etime %d dtime %d sched %d diff %.3f adif %.3f judge %.3f\n", etime, dtime, sched, diff, adif, judge);
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

int output_init() {
    char *audio, *video;

    if (ao || vo || aotid || votid || sytid)
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
    sytid = 0;

    if (gCtx->audio_enabled)
        pthread_create(&aotid, 0, audio_output_thread, 0);
    if (gCtx->video_enabled || gCtx->subtitle_enabled)
        pthread_create(&votid, 0, video_output_thread, 0);
    if (gCtx->audio_enabled || gCtx->video_enabled)
        pthread_create(&sytid, 0, synchronize_thread, 0);

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
    if (sytid) {
        pthread_join(sytid, 0);
        sytid = 0;
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

