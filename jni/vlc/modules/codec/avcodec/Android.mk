
LOCAL_PATH := $(call my-dir)

# libavcodec_plugin-6.so

include $(CLEAR_VARS)

LOCAL_MODULE := avcodec_plugin-6

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

# libavcodec_plugin-7.so

include $(CLEAR_VARS)

LOCAL_MODULE := avcodec_plugin-7

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

