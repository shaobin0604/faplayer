
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := charset

LOCAL_CFLAGS += \
    -DHAVE_CONFIG_H \
    -DBUILDING_LIBCHARSET \
    -DIN_LIBRARY \
    -DLIBDIR=\"\"

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include

LOCAL_SRC_FILES := \
    lib/localcharset.c

include $(BUILD_STATIC_LIBRARY)

