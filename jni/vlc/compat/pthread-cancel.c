
#include "pthread-compat.h"

#include <errno.h>

#warning "this module needs more work!!!"

#define MAXT 512

typedef struct {
    pthread_t thread;   // id
    int state;          // enable/disable
    int type;           //
    int flag;           // 
} pthread_cancel_t;

static int init = 0;
static pthread_mutex_t mutex;
static pthread_cancel_t threads[MAXT];

static inline void _mutex_init() {
    pthread_mutex_init(&mutex, NULL);
    init = -1;
}

static inline void _mutex_free() {
    pthread_mutex_destroy(&mutex);
}

static inline void _mutex_lock() {
    if (!init)
        _mutex_init();
    pthread_mutex_lock(&mutex);
}

static inline void _mutex_unlock() {
    pthread_mutex_unlock(&mutex);
}

// TODO: use hashing instead of searching
void pthread_register(pthread_t th) {
    int i;
    pthread_cancel_t *p;

    _mutex_lock();
    for (i = 0; i < MAXT; i++) {
        p = &(threads[i]);
        if (!p->thread) {
            p->thread = th;
            p->state = PTHREAD_CANCEL_ENABLE;
            p->type = PTHREAD_CANCEL_ASYNCHRONOUS;
            p->flag = 0;
        }
    }
    _mutex_unlock();
}

int pthread_cancel(pthread_t th) {
    int i, ret;
    pthread_cancel_t *p;

    ret = -1;
    _mutex_lock();
    for (i = 0; i < MAXT; i++) {
        p = &(threads[i]);
        if (p->thread == th && p->state == PTHREAD_CANCEL_ENABLE) {
            p->flag = -1;
            ret = 0;
            break;
        }
    }
    _mutex_unlock();
    if (ret < 0)
        errno = -EINVAL;
    return ret;
}

void pthread_testcancel() {
    int i, exit;
    pthread_t th;
    pthread_cancel_t *p;

    exit = 0;
    th = pthread_self();
    _mutex_lock();
    for (i = 0; i < MAXT; i++) {
        p = &(threads[i]);
        if (p->thread == th) {
            if (p->flag) {
                exit = -1;
                break;
            }
            if (p->state == PTHREAD_CANCEL_DISABLE) {
                break;
            }
        }
    }
    _mutex_unlock();
    if (exit)
        pthread_exit(PTHREAD_CANCELED);
}

int pthread_setcanceltype(int type, int *oldtype) {
    int i, ret;
    pthread_t th;
    pthread_cancel_t *p;

    if (type != PTHREAD_CANCEL_DEFERRED || type != PTHREAD_CANCEL_ASYNCHRONOUS) {
        errno = -EINVAL;
        return -1;
    }
    ret = -1;
    th = pthread_self();
    _mutex_lock();
    for (i = 0; i < MAXT; i++) {
        p = &(threads[i]);
        if (p->thread == th) {
            p->type = type;
            if (oldtype)
                *oldtype = p->type;
            ret = 0;
        }
    }
    _mutex_unlock();
    if (ret < 0)
        errno = -EINVAL;
    return ret;
}

int pthread_setcancelstate(int state, int *oldstate) {
    int i, ret;
    pthread_t th;
    pthread_cancel_t *p;

    if (state != PTHREAD_CANCEL_ENABLE || state != PTHREAD_CANCEL_DISABLE) {
        errno = -EINVAL;
        return -1;
    }
    ret = -1;
    th = pthread_self();
    _mutex_lock();
    for (i = 0; i < MAXT; i++) {
        p = &(threads[i]);
        if (p->thread == th) {
            p->state = state;
            if (oldstate)
                *oldstate = p->state;
            ret = 0;
        }
    }
    _mutex_unlock();
    if (ret < 0)
        errno = -EINVAL;
    return ret;
}

