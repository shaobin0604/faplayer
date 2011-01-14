
LOCAL_PATH := $(call my-dir)

# libdummy_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := dummy_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"dummy\"

LOCAL_C_INCLUDES += \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/compat

LOCAL_SRC_FILES := \
    aout.c \
    decoder.c \
    dummy.c \
    encoder.c \
    input.c \
    interface.c \
    renderer.c \
    vout.c

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

