
LOCAL_PATH := $(call my-dir)

# libaudio_format_neon_plugin-7.so

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := audio_format_neon_plugin-7

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"audio_format_neon_plugin-7\" \
    -Dasm=__asm__

LOCAL_CFLAGS += \
    -march=armv7-a \
    -mtune=cortex-a8 \
    -mfloat-abi=softfp \
    -mfpu=neon

LOCAL_LDFLAGS += \
    -Wl,--fix-cortex-a8

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    audio_format.c \
    s32_s16.S

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

