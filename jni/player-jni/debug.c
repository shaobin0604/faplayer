
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#ifdef ANDROID
#include <android/log.h>
#endif

static int init = 0;
static pthread_mutex_t mutex;
static char buffer[1024];

void debug(const char* fmt, ...) {
    va_list vlist;
    va_start(vlist, fmt);
    if (!init) {
        init = -1;
        pthread_mutex_init(&mutex, 0);
    }
    pthread_mutex_lock(&mutex);
#ifdef ANDROID
    __android_log_vprint(ANDROID_LOG_DEBUG, "player-4", fmt, vlist);
#else
    vfprintf(stdout, fmt, vlist);
    fflush(stdout);
#endif
    pthread_mutex_unlock(&mutex);
}

