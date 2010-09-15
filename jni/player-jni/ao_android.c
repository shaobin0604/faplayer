
#include <pthread.h>

#include "ao.h"
#include "queue.h"

extern int audio_track_create(int rate, int format, int channel);
extern void audio_track_play(const void* buffer, int length);
extern void audio_track_destroy();

static pthread_mutex_t lock;

static int ao_init_android() {
    pthread_mutex_init(&lock, 0);
    return 0;
}

static int ao_play_android(Samples* sam) {
    int err;

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
}

ao_t ao_android = {
    .name = "android",
    .init = ao_init_android,
    .free = ao_free_android,
    .play = ao_play_android,
};

