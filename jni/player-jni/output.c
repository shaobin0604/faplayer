
#include <libswscale/swscale.h>
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

static ReSampleContext* audio_cvt_ctx = 0;
static struct SwsContext* video_cvt_ctx = 0;

static pthread_t aotid = 0;
static pthread_t votid = 0;
static pthread_t sytid = 0;
static pthread_t actid = 0;
static pthread_t vctid = 0;

static int get_audio_format_size(int format) {
    switch (format) {
        case SAMPLE_FMT_U8:
            return sizeof(uint8_t);
        case SAMPLE_FMT_S16:
            return sizeof(int16_t);
        case SAMPLE_FMT_S32:
            return sizeof(int32_t);
        case SAMPLE_FMT_FLT:
            return sizeof(float);
        case SAMPLE_FMT_DBL:
            return sizeof(double);
        default:
            return -1;
    }
}

static void* audio_output_thread(void* para) {
    Samples* sam;
    int cvt = -1;
    int64_t cnt, total = 0;
    JNIEnv* env;

    set_thread_priority(7);

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
        if (cvt) {
            sam = audio_frame_queue_pop_tail();
            cvt = sam->cvt;
        }
        else
            sam = samples_queue_pop_tail();
        if (sam) {
            if (ao && ao->play)
                ao->play(sam, 0);
            cnt = sam->size / sam->channel;
            cnt /= get_audio_format_size(sam->format);
            if (cnt > 0) {
                total += cnt;
                gCtx->audio_last_pts = (sam->pts != 0) ? sam->pts : ((double)total / (double)(sam->rate));
            }
            free_Samples(sam);
        }
    }

    debug("ao thread exit");

    return 0;
}

static void* video_output_thread(void* para) {
    Picture* pic;
    int64_t bgn, end, left;
    double diff, factor;
    int err, count;
    int64_t vb, ve, vt, tm;

    set_thread_priority(7);

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
        if (gCtx->pause && (gCtx->video_last_pts < gCtx->audio_last_pts)) {
            usleep(25 * 1000);
            continue;
        }
        bgn = av_gettime();
        pic = video_frame_queue_pop_tail();
        if (pic) {
            if (vo && vo->display) {
                count++;
                // wait until surface is ready
                while (!stop) {
                    vb = av_gettime();
                    err = vo->display(pic, 0);
                    ve = av_gettime();
                    if (!err)
                        break;
                }
                vt += (ve - vb);
                gCtx->avg_video_display_time = vt / count;
                tm = av_gettime();
                if (!gCtx->video_first_time)
                    gCtx->video_first_time = tm;
            }
            gCtx->video_last_pts = pic->pts;
            free_Picture(pic);
        }
        end = av_gettime();
        left = (int64_t)(1000 * 1000 / gCtx->fps) - end + bgn;
        factor = 1.0;
        diff = (gCtx->audio_enabled) ? (gCtx->audio_last_pts - gCtx->video_last_pts) : ((double)(tm - gCtx->video_first_time) / (double)(1000 * 1000) - gCtx->video_last_pts);
        factor = (diff > 0) ? 0 : (1.0 - diff / 2);
        //debug("cur a/v/d %.3f/%.3f/%.3f\n", gCtx->audio_last_pts, gCtx->video_last_pts, diff);
        if (left > 0 && factor > 0) {
            usleep((int64_t)(left * factor));
        }
    }

    debug("vo thread exit");

    return 0;
}

static void* synchronize_thread(void* para) {
    int idx, fps, step;
    int sched, etime, dtime;
    double temp[100];
    double diff, total, adif;
    double judge;

    idx = 0;
    fps = (int)(gCtx->fps);
    step = fps >> 2;
    etime = 1000 / fps;
    total = 0;

    pthread_mutex_lock(&gCtx->start_mutex);
    while ((gCtx->audio_enabled && (audio_frame_queue_size() + samples_queue_size() < step)) || (gCtx->video_enabled && (video_frame_queue_size() + picture_queue_size() < step)))
        usleep(5 * 1000);
    gCtx->start = -1;
    pthread_cond_broadcast(&gCtx->start_condv);
    pthread_mutex_unlock(&gCtx->start_mutex);

    if (!(gCtx->audio_enabled && (gCtx->video_enabled || gCtx->subtitle_enabled)))
        goto out;

    for (;;) {
        if (stop)
            break;
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

out:
    debug("sy thread exit");

    return 0;
}

static void* audio_convert_thread(void* para) {
    Samples* samples;
    int flag;
    int fmt, chl, sz;
    int err, is, os, cnt;
    void *cvt, *in, *out;

    set_thread_priority(8);

    for (;;) {
        if (stop)
            break;
        samples = samples_queue_pop_tail();
        if (!samples)
            continue;
        fmt = samples->format;
        chl = samples->channel;
        cvt = samples->samples;
        sz = samples->size;
        flag = (fmt > SAMPLE_FMT_S16) || (chl > 2);
        if (!flag) {
            audio_frame_queue_push_head(samples);
            break;
        }
        fmt = samples->format > SAMPLE_FMT_S16 ? SAMPLE_FMT_S16 : samples->format;
        chl = samples->channel > 2 ? 2 : samples->channel;
        if (!audio_cvt_ctx) {
            audio_cvt_ctx = av_audio_resample_init(chl, samples->channel, samples->rate, samples->rate, fmt, samples->format, 0, 0, 0, 0);
            if (!audio_cvt_ctx)
                break;
        }
        is = get_audio_format_size(samples->format);
        if (is < 0) {
            free_Samples(samples);
            continue;
        }
        in = samples->samples;
        os = (samples->format == SAMPLE_FMT_U8) ? sizeof(uint8_t) : sizeof(int16_t);
        cnt = samples->size / is / samples->channel;
        sz = cnt * os * chl;
        out = av_mallocz(sz);
        if (!out) {
            free_Samples(samples);
            continue;
        }
        err = audio_resample(audio_cvt_ctx, out, in, cnt);
        if (err < 0) {
            av_free(out);
            free_Samples(samples);
            continue;
        }
        samples->cvt = -1;
        av_free(samples->samples);
        samples->channel = chl;
        samples->format = fmt;
        samples->size = sz;
        samples->samples = out;
        audio_frame_queue_push_head(samples);        
    }

    debug("ac thread exit");

    return 0;
}

static void* video_convert_thread(void* para) {
    int err;
    AVPicture dest;
    void* buffer;
    Picture* picture;

    set_thread_priority(8);

    for (;;) {
        if (stop)
            break;
        picture = picture_queue_pop_tail();
        if (!picture)
            continue;
        video_cvt_ctx = sws_getCachedContext(video_cvt_ctx, picture->width, picture->height, picture->format, picture->width, picture->height, PIX_FMT_RGB565, SWS_POINT, 0, 0, 0);
        if (!video_cvt_ctx) {
            free_Picture(picture);
            continue;
        }
        err = avpicture_alloc(&dest, PIX_FMT_RGB565, picture->width, picture->height);
        if (err < 0) {
            free_Picture(picture);
            continue;
        }
        err = sws_scale(video_cvt_ctx, (const uint8_t * const*) picture->picture.data, picture->picture.linesize, 0, picture->height, dest.data, dest.linesize);
        if (err < 0) {
            avpicture_free(&dest);
            free_Picture(picture);
            continue;
        }
        picture->cvt = -1;
        avpicture_free(&picture->picture);
        picture->picture = dest;
        picture->format = PIX_FMT_RGB565;
        video_frame_queue_push_head(picture);
    }

    debug("vc thread exit");

    return 0;
}

int output_init() {
    char *audio, *video;

    if (ao || vo || aotid || votid || sytid || actid || vctid)
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
    actid = 0;
    vctid = 0;

    if (gCtx->audio_enabled) {
        pthread_create(&actid, 0, audio_convert_thread, 0);
        pthread_create(&aotid, 0, audio_output_thread, 0);
    }
    if (gCtx->video_enabled) {
        pthread_create(&vctid, 0, video_convert_thread, 0);
        pthread_create(&votid, 0, video_output_thread, 0);
    }
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
    if (actid) {
        audio_frame_queue_wake();
        pthread_join(actid, 0);
        actid = 0;
    }
    if (vctid) {
        video_frame_queue_wake();
        pthread_join(vctid, 0);
        vctid = 0;
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

