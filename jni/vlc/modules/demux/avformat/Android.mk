
LOCAL_PATH := $(call my-dir)

# libavformat_plugin-6.so

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := avformat_plugin-6

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"avformat\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/ffmpeg

LOCAL_SRC_FILES := \
    avformat.c \
    demux.c \
    chroma.c \
    fourcc.c

LOCAL_SHARED_LIBRARIES += vlccore ffmpeg-6

include $(BUILD_SHARED_LIBRARY)

# libavformat_plugin-7.so

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := avformat_plugin-7

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"avformat\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/ffmpeg

LOCAL_SRC_FILES := \
    avformat.c \
    demux.c \
    chroma.c \
    fourcc.c

LOCAL_SHARED_LIBRARIES += vlccore ffmpeg-7

include $(BUILD_SHARED_LIBRARY)

