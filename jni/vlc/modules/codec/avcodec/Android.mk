
LOCAL_PATH := $(call my-dir)

# libavcodec-6_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := avcodec-6_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"avcodec-6\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/ffmpeg

LOCAL_SRC_FILES := \
    audio.c \
    avcodec.c \
    chroma.c \
    copy.c \
    deinterlace.c \
    dxva2.c \
    fourcc.c \
    subtitle.c \
    vaapi.c \
    video.c

LOCAL_SHARED_LIBRARIES += vlccore ffmpeg-6

include $(BUILD_SHARED_LIBRARY)

# libavcodec-7_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := avcodec-7_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"avcodec-7\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/ffmpeg

LOCAL_SRC_FILES := \
    audio.c \
    avcodec.c \
    chroma.c \
    copy.c \
    deinterlace.c \
    dxva2.c \
    fourcc.c \
    subtitle.c \
    vaapi.c \
    video.c

LOCAL_SHARED_LIBRARIES += vlccore ffmpeg-7

include $(BUILD_SHARED_LIBRARY)

