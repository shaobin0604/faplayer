
LOCAL_PATH := $(call my-dir)

# liboldhttp_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := oldhttp_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"oldhttp\"

LOCAL_C_INCLUDES += \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/compat

LOCAL_SRC_FILES := \
    http.c \
    macro.c \
    mvar.c \
    rpn.c \
    util.c

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

