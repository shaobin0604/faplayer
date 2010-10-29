
#include <pthread.h>
#include <libavcodec/avcodec.h>
#include "ao.h"
#include "queue.h"

extern int audio_track_create(int rate, int format, int channel);
extern void audio_track_play(const void* buffer, int length);
extern void audio_track_destroy();

static int ao_init_android() {
    return 0;
}

static int ao_play_android(Samples* sam, void* extra) {
    int err;

    err = audio_track_create(sam->rate, sam->format, sam->channel);
    if (!err && sam->samples && sam->size)
        audio_track_play(sam->samples, sam->size);

    return err;
}

static void ao_free_android() {
    audio_track_destroy();
}

ao_t ao_android = {
    .name = "android",
    .init = ao_init_android,
    .free = ao_free_android,
    .play = ao_play_android,
};

