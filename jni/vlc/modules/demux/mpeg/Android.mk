
LOCAL_PATH := $(call my-dir)

# libmpgv_plugin.so

include $(CLEAR_VARS)

#LOCAL_MODULE := mpgv_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"mpgv\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    mpgv.c \

LOCAL_SHARED_LIBRARIES += vlccore

#include $(BUILD_SHARED_LIBRARY)

