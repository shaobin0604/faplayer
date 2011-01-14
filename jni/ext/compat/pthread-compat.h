
#ifndef PTHREAD_COMPAT_H
#define PTHREAD_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <android/api-level.h>

#if __ANDROID_API__ < 9

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_getpshared(pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t* attr, int pshared);
int pthread_condattr_destroy(pthread_condattr_t *attr);

typedef int pthread_rwlockattr_t;

typedef struct {
    pthread_mutex_t  lock;
    pthread_cond_t   cond;
    int              numLocks;
    int              writerThreadId;
    int              pendingReaders;
    int              pendingWriters;
    void*            reserved[4];  /* for future extensibility */
} pthread_rwlock_t;

#define PTHREAD_RWLOCK_INITIALIZER  { PTHREAD_MUTEX_INITIALIZER, 0, NULL, 0, 0 }

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int  pshared);
int pthread_rwlockattr_getpshared(pthread_rwlockattr_t *attr, int *pshared);

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);

int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
#endif

#if __ANDROID_API__ < 8

#endif

#if __ANDROID_API__ < 5

#endif

enum {
    PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE   PTHREAD_CANCEL_ENABLE
    PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE  PTHREAD_CANCEL_DISABLE
};

enum {
    PTHREAD_CANCEL_DEFERRED,
#define PTHREAD_CANCEL_DEFERRED PTHREAD_CANCEL_DEFERRED
    PTHREAD_CANCEL_ASYNCHRONOUS
#define PTHREAD_CANCEL_ASYNCHRONOUS PTHREAD_CANCEL_ASYNCHRONOUS
};

#define PTHREAD_CANCELED ((void *) -1)

void pthread_register(pthread_t th);
int pthread_cancel(pthread_t th);
void pthread_testcancel();
int pthread_setcanceltype(int type, int *oldtype);
int pthread_setcancelstate(int state, int *oldstate);

#ifdef __cplusplus
}
#endif

#endif

