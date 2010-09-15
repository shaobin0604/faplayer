LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := player-4

AOSP := /home/luka/Source/aosp-donut

LOCAL_C_INCLUDES += \
    $(AOSP)/hardware/libhardware/include \
    $(AOSP)/system/core/include \
    $(AOSP)/external/skia/include \
    $(AOSP)/frameworks/base/include \
    $(LOCAL_PATH)/../ffmpeg

LOCAL_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCLASS=org_luka_player_NativePlayer4 -DPLATFORM=4

LOCAL_SRC_FILES := \
    debug.c \
    player.c \
    input.c \
    output.c \
    decode.c \
    queue.c \
    ao.c \
    vo.c \
    ao_android.c \
    ao_android_wrapper.cpp \
    vo_android.c \
    vo_android_wrapper.cpp \
    jni.cpp

LOCAL_LDLIBS += -llog -lutils -lui -lmedia -lffmpeg

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := player-8

AOSP := /home/luka/Source/aosp-froyo

LOCAL_C_INCLUDES += \
    $(AOSP)/hardware/libhardware/include \
    $(AOSP)/system/core/include \
    $(AOSP)/external/skia/include \
    $(AOSP)/frameworks/base/include \
    $(LOCAL_PATH)/../ffmpeg

LOCAL_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -DCLASS=org_luka_player_NativePlayer8 -DPLATFORM=8

LOCAL_SRC_FILES := \
    debug.c \
    player.c \
    input.c \
    output.c \
    decode.c \
    queue.c \
    ao.c \
    vo.c \
    ao_android.c \
    ao_android_wrapper.cpp \
    vo_android.c \
    vo_android_wrapper.cpp \
    jni.cpp

LOCAL_LDLIBS += -llog -lutils -lsurfaceflinger_client -lmedia -lffmpeg

include $(BUILD_SHARED_LIBRARY)

