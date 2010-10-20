
#include <pthread.h>

#include <libavcodec/audioconvert.h>

#include "ao.h"
#include "queue.h"

extern int audio_track_create(int rate, int format, int channel);
extern void audio_track_play(const void* buffer, int length);
extern void audio_track_destroy();

static pthread_mutex_t lock;
static AVAudioConvert* cvt;

static int ao_init_android() {
    pthread_mutex_init(&lock, 0);
    return 0;
}

static int ao_play_android(Samples* sam) {
    int err, i, n, m;
    void* in[6];
    void* out[6];
    int in_stride[6];
    int out_stride[6];

    if (sam->format > SAMPLE_FMT_S16 || sam->channel > 2) {
        debug("audio format %d, channel %d, need convert\n", sam->format, sam->channel);
        if (!cvt)
            cvt = av_audio_convert_alloc(sam->format > SAMPLE_FMT_S16 ? SAMPLE_FMT_S16 : sam->format, sam->channel > 2 ? 2 : sam->channel, sam->format, sam->channel, 0, 0);
        if (!cvt)
            return -1;
        switch(sam->format) {
            case SAMPLE_FMT_S32:
                n = sizeof(int32_t);
                break;
            case SAMPLE_FMT_FLT:
                n = sizeof(float);
                break;
            case SAMPLE_FMT_DBL:
                n = sizeof(double);
                break;
            default:
                return -1;
        }
        in[0] = sam->samples;
        in_stride[0] = n;
        if (sam->channel > 1) {
            for (i = 1; i < sam->channel && i < 6; i++) {
                in[i] = (uint8_t*)(sam->samples) + n * i;
                in_stride[i] = n;
            }
        }
        m = (sam->format == SAMPLE_FMT_U8) ? sizeof(uint8_t) : sizeof(int16_t);
        out[0] = malloc(sam->size * m * (sam->channel > 1 ? 2 : sam->channel));
        if (!out[0])
            return -1;
        out[1] = out[2] = out[3] = out[4] = out[5] = 0;
        out_stride[0] = m;
        out_stride[1] = out_stride[2] = out_stride[3] = out_stride[4] = out_stride[5] = 0;
        if (sam->channel > 1) {
            out[1] = (uint8_t*)(out[0]) + sam->size * m / n / 2;
            if (!out[1]) {
                free(out[0]);
                return -1;
            }
            out_stride[1] = m;
        }
        err = av_audio_convert(cvt, out, out_stride, (const void * const*)in, in_stride, sam->size);
        if (err < 0) {
            free(out[0]);
            if (sam->channel > 1)
                free(out[1]);
            return -1;
        }
    }
    pthread_mutex_lock(&lock);
    err = audio_track_create(sam->rate, sam->format, sam->channel);
    if (!err && sam && sam->samples && sam->size)
        audio_track_play(sam->samples, sam->size);
    pthread_mutex_unlock(&lock);

    return err;
}

static void ao_free_android() {
    pthread_mutex_lock(&lock);
    audio_track_destroy();
    pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
    if (cvt) {
        av_audio_convert_free(cvt);
        cvt = 0;
    }
}

ao_t ao_android = {
    .name = "android",
    .init = ao_init_android,
    .free = ao_free_android,
    .play = ao_play_android,
};

