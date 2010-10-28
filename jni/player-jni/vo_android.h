
#ifndef VO_ANDROID_H
#define VO_ANDROID_H

#ifdef __cplusplus
extern "C" {
#endif

jint attach(JNIEnv* env, jobject thiz, jobject obj, jint, jint);
void detach(JNIEnv* env, jobject thiz);

#ifdef __cplusplus
}
#endif

#endif

