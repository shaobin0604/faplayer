
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc/vlc.h>
#include <vlc_common.h>

#if HAVE_ANDROID

#include <jni.h>

#define CLASS org_videolan_VLC
#define KLASS "org/videolan/VLC"

#define NAME1(CLZ, FUN) Java_##CLZ##_##FUN
#define NAME2(CLZ, FUN) NAME1(CLZ, FUN)

#define NAME(FUN) NAME2(CLASS, FUN)

JavaVM *gJVM = NULL;

static jobject vout_android_ref = NULL;
static void *vout_android_surf = NULL;
static vlc_mutex_t vout_android_lock;

void LockSurface() {
    vlc_mutex_lock(&vout_android_lock);
}

void UnlockSurface() {
    vlc_mutex_unlock(&vout_android_lock);
}

void *GetSurface() {
    return vout_android_surf;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    gJVM = vm;
    vlc_mutex_init(&vout_android_lock);

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    vlc_mutex_destroy(&vout_android_lock);
}

JNIEXPORT int JNICALL NAME(setenv)(JNIEnv *env, jclass klz, jstring key, jstring val, jboolean overwrite) {
    const char *key_utf8 = (*env)->GetStringUTFChars(env, key, NULL);
    const char *val_utf8 = (*env)->GetStringUTFChars(env, val, NULL);
    int err = setenv(key_utf8, val_utf8, overwrite);
    (*env)->ReleaseStringUTFChars(env, key, key_utf8);
    (*env)->ReleaseStringUTFChars(env, val, val_utf8);
    return err;
}

JNIEXPORT void JNICALL NAME(attachVideoOutput)(JNIEnv *env, jclass klz, jobject surf) {
    jclass clz;
    jfieldID fid;
    jthrowable exp;

    vlc_mutex_lock(&vout_android_lock);
    //vout_android_ref = (*env)->NewGlobalRef(env, surf);
    clz = (*env)->GetObjectClass(env, surf);
    fid = (*env)->GetFieldID(env, clz, "mSurface", "I");
    if (fid == NULL) {
        exp = (*env)->ExceptionOccurred(env);
        if (exp) {
            (*env)->DeleteLocalRef(env, exp);
            (*env)->ExceptionClear(env);
        }
        fid = (*env)->GetFieldID(env, clz, "mNativeSurface", "I");
    }
    vout_android_surf = (void*)(*env)->GetIntField(env, surf, fid);
    (*env)->DeleteLocalRef(env, clz);
    vlc_mutex_unlock(&vout_android_lock);
}

JNIEXPORT void JNICALL NAME(detachVideoOutput)(JNIEnv *env, jclass klz) {
    vlc_mutex_lock(&vout_android_lock);
    //(*env)->DeleteGlobalRef(env, vout_android_ref);
    vout_android_surf = NULL;
    vlc_mutex_unlock(&vout_android_lock);
}

JNIEXPORT void JNICALL NAME(run)(JNIEnv *env, jobject thiz, jobject args) {
    jclass clz;
    jfieldID fid;
    jstring arg;
    int i, argc;
    const char **argv;
    libvlc_instance_t *vlc;

    if (!args)
        return;
    argc = (*env)->GetArrayLength(env, args);
    if (!argc)
        return;
    argv = (const char**) malloc(argc * sizeof(char*));
    if (!argv)
        return;
    for (i = 0; i < argc; i++) {
        arg = (*env)->GetObjectArrayElement(env, args, i);
        argv[i] = (*env)->GetStringUTFChars(env, arg, NULL);
    }
    vlc = libvlc_new(argc, argv);
    if (vlc) {
        libvlc_set_user_agent(vlc, "VLC media player", NULL);
        if (libvlc_add_intf(vlc, NULL) == 0) {
            libvlc_playlist_play(vlc, -1, 0, NULL);
            libvlc_wait(vlc);
        }
        libvlc_release(vlc);
    }
    for (i = 0; i < argc; i++) {
        arg = (*env)->GetObjectArrayElement(env, args, i);
        (*env)->ReleaseStringUTFChars(env, arg, argv[i]);
    }
    free(argv);
}

#endif

