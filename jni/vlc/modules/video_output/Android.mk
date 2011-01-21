
LOCAL_PATH := $(call my-dir)

#libvout_android-3_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_android-3_plugin

LOCAL_CFLAGS += \
    -D__PLATFORM__=3 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_android-3\"

LOCAL_C_INCLUDES += \
    $(DEPROOT)/android-3/include \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    android_Surface.cpp

LOCAL_LDFLAGS += -L$(DEPROOT)/android-3/lib
LOCAL_LDLIBS += -lui

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

#libvout_android-4_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_android-4_plugin

LOCAL_CFLAGS += \
    -D__PLATFORM__=4 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_android-4\"

LOCAL_C_INCLUDES += \
    $(DEPROOT)/android-4/include \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    android_Surface.cpp

LOCAL_LDFLAGS += -L$(DEPROOT)/android-4/lib
LOCAL_LDLIBS += -lui

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

#libvout_android-5_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_android-5_plugin

LOCAL_CFLAGS += \
    -D__PLATFORM__=5 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_android-5\"

LOCAL_C_INCLUDES += \
    $(DEPROOT)/android-5/include \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    android_Surface.cpp

LOCAL_LDFLAGS += -L$(DEPROOT)/android-5/lib
LOCAL_LDLIBS += -lui

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

#libvout_android-8_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_android-8_plugin

LOCAL_CFLAGS += \
    -D__PLATFORM__=8 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_android-8\"

LOCAL_C_INCLUDES += \
    $(DEPROOT)/android-8/include \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    android_Surface.cpp

LOCAL_LDFLAGS += -L$(DEPROOT)/android-8/lib
LOCAL_LDLIBS += -lsurfaceflinger_client

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

#libvout_android-9_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_android-9_plugin

LOCAL_CFLAGS += \
    -D__PLATFORM__=9 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_android-9\"

LOCAL_C_INCLUDES += \
    $(DEPROOT)/android-9/include \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    android_Surface.cpp

LOCAL_LDFLAGS += -L$(DEPROOT)/android-9/lib
LOCAL_LDLIBS += -lsurfaceflinger_client

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

#libvout_wrapper_plugin.so

include $(CLEAR_VARS)

LOCAL_MODULE := vout_wrapper_plugin

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -D__PLUGIN__ \
    -DMODULE_STRING=\"vout_wrapper\"

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    wrapper.c

LOCAL_SHARED_LIBRARIES += vlccore

include $(BUILD_SHARED_LIBRARY)

