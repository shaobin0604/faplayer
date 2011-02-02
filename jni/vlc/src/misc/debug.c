
#include <jni.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#ifdef ANDROID
#include <android/log.h>
#endif
#include "debug.h"

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
    __android_log_vprint(ANDROID_LOG_DEBUG, "faplayer", fmt, vlist);
#else
    vfprintf(stdout, fmt, vlist);
    fflush(stdout);
#endif
    pthread_mutex_unlock(&mutex);
}

void dump(const void* addr, int size) {
    int i, j;
    unsigned char* p = (unsigned char*) addr;
    char v, c;
    char buffer[512];

    if (!init) {
        init = -1;
        pthread_mutex_init(&mutex, 0);
    }
    j = 0;
    for (i = 0; i < size; i++) {
        if (i + 1 % 16 == 0 || i + 1 == size) {
            buffer[j] = 0;
            j = 0;
            pthread_mutex_lock(&mutex);
#ifdef ANDROID
            __android_log_write(ANDROID_LOG_DEBUG, "faplayer", buffer);
#else
            vfprintf(stdout, fmt, vlist);
            fflush(stdout);
#endif
            pthread_mutex_unlock(&mutex);
        }
        v = *((unsigned char*)(addr) + i);
        c = (v >> 4) | 0x30;
        c += (c > '9') ? 7 : 0;
        buffer[j++] = c;
        c = (v & 0x0f) | 0x30;
        c += (c > '9') ? 7 : 0;
        buffer[j++] = c;
        buffer[j++] = 0x20;
    }
}


