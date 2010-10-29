
#include <jni.h>

#include "player.h"
#include "utility.h"

JavaVM* jvm = 0;

#ifndef CLASS
#error "?"
#endif

#define NAME1(CLZ, FUN) Java_##CLZ##_##FUN
#define NAME2(CLZ, FUN) NAME1(CLZ, FUN)

#define NAME(FUN) NAME2(CLASS, FUN)

JNIEXPORT jint JNICALL NAME(open)(JNIEnv *env, jobject thiz, jstring file) {
    jint result;
    const char *f;
    jboolean copy;

    f = (*env)->GetStringUTFChars(env, file, &copy);
    result = player_open(f);
    (*env)->ReleaseStringUTFChars(env, file, f);

    return result;
}

JNIEXPORT void JNICALL NAME(close)(JNIEnv *env, jobject thiz) {
    player_close();
}

JNIEXPORT jint JNICALL NAME(play)(JNIEnv *env, jobject thiz, jdouble start, jint ast, jint vst, jint sst) {
    return player_play(start, ast, vst, sst);
}

JNIEXPORT void JNICALL NAME(pause)(JNIEnv *env, jobject thiz) {
    player_pause();
}

JNIEXPORT void JNICALL NAME(resume)(JNIEnv *env, jobject thiz) {
    player_resume();
}

JNIEXPORT int JNICALL NAME(seek)(JNIEnv *env, jobject thiz, jdouble time) {
    return player_seek(time);
}

JNIEXPORT jint JNICALL NAME(getDuration)(JNIEnv *env, jobject thiz) {
    return player_get_duration();
}

JNIEXPORT jint JNICALL NAME(getCurrentTime)(JNIEnv *env, jobject thiz) {
    return player_get_current_time();
}

JNIEXPORT jint JNICALL NAME(setVideoMode)(JNIEnv *env, jobject thiz, jint mode) {
    if (mode < 0 || mode > 2)
        return -1;
    player_set_video_mode(mode);
}

JNIEXPORT jint JNICALL NAME(getVideoWidth)(JNIEnv *env, jobject thiz) {
    return player_get_video_width();
}

JNIEXPORT jint JNICALL NAME(getVideoHeight)(JNIEnv *env, jobject thiz) {
    return player_get_video_height();
}

JNIEXPORT jint JNICALL NAME(getAudioStreamCount)(JNIEnv *env, jobject thiz) {
    return player_get_audio_stream_count();
}

JNIEXPORT jint JNICALL NAME(getVideoStreamCount)(JNIEnv *env, jobject thiz) {
    return player_get_video_stream_count();
}

JNIEXPORT jint JNICALL NAME(getSubtitleStreamCount)(JNIEnv *env, jobject thiz) {
    return player_get_subtitle_stream_count();
}

JNIEXPORT jboolean JNICALL NAME(isPlaying)(JNIEnv *env, jobject thiz) {
    return player_is_playing() == 0 ? 1 : 0;
}

JNIEXPORT jint JNICALL NAME(attach)(JNIEnv *env, jobject thiz, jobject obj) {
    return attach(env, thiz, obj);
}

JNIEXPORT void JNICALL NAME(detach)(JNIEnv *env, jobject thiz) {
    detach(env, thiz);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jvm = vm;

    return JNI_VERSION_1_4;
}

