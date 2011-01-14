
LOCAL_PATH := $(call my-dir)

# libaccess_avio_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := access_avio_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"access_avio\"

LOCAL_C_INCLUDES += \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/compat \
    $(EXTROOT)/ffmpeg

LOCAL_SRC_FILES := \
    avio.c

LOCAL_SHARED_LIBRARIES += vlccore ffmpeg

include $(BUILD_SHARED_LIBRARY)

# libfilesystem_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := filesystem_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"filesystem\"

LOCAL_C_INCLUDES += \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/compat

LOCAL_SRC_FILES := \
    directory.c \
    file.c \
    fs.c

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

