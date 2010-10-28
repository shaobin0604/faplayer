
#include <pthread.h>

#include <libavcodec/avcodec.h>

#include "ao.h"
#include "queue.h"

extern int audio_track_create(int rate, int format, int channel);
extern void audio_track_play(const void* buffer, int length);
extern void audio_track_destroy();

static ReSampleContext* cvt;

static int ao_init_android() {
    return 0;
}

static int ao_play_android(Samples* sam, void* extra) {
    int needed;
    int fmt, chl, size;
    void *smp, *in, *out;
    int err, i, in_size, out_size, count;

    fmt = sam->format;
    chl = sam->channel;
    smp = sam->samples;
    size = sam->size;
    needed = (sam->format > SAMPLE_FMT_S16) || (sam->channel > 2);
    if (needed) {
        fmt = sam->format > SAMPLE_FMT_S16 ? SAMPLE_FMT_S16 : sam->format;
        chl = sam->channel > 2 ? 2 : sam->channel;
        if (!cvt)
            cvt = av_audio_resample_init(chl, sam->channel, sam->rate, sam->rate, fmt, sam->format, 0, 0, 0, 0);
        if (!cvt)
            return -1;
        switch(sam->format) {
            case SAMPLE_FMT_U8:
                in_size = sizeof(uint8_t);
                break;
            case SAMPLE_FMT_S16:
                in_size = sizeof(int16_t);
                break;
            case SAMPLE_FMT_S32:
                in_size = sizeof(int32_t);
                break;
            case SAMPLE_FMT_FLT:
                in_size = sizeof(float);
                break;
            case SAMPLE_FMT_DBL:
                in_size = sizeof(double);
                break;
            default:
                debug("unknown sample format %d", sam->format);
                return -1;
        }
        in = sam->samples;
        out_size = (sam->format == SAMPLE_FMT_U8) ? sizeof(uint8_t) : sizeof(int16_t);
        count = sam->size / in_size / sam->channel;
        size = count * out_size * chl;
        out = av_mallocz(size);
        if (!out)
            return -1;
        err = audio_resample(cvt, out, in, count);
        if (err < 0) {
            free(out);
            return -1;
        }
        smp = out;
    }
    err = audio_track_create(sam->rate, fmt, chl);
    if (!err && smp && size)
        audio_track_play(smp, size);
    if (needed) {
        av_free(out);
    }

    return err;
}

static void ao_free_android() {
    audio_track_destroy();
    if (cvt) {
        audio_resample_close(cvt);
        cvt = 0;
    }
}

ao_t ao_android = {
    .name = "android",
    .init = ao_init_android,
    .free = ao_free_android,
    .play = ao_play_android,
};

