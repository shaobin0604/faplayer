
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := iconv

LOCAL_CFLAGS += \
    -DHAVE_CONFIG_H \
    -DBUILDING_LIBICONV \
    -DIN_LIBRARY

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/libcharset/include

LOCAL_SRC_FILES := \
    lib/iconv.c

LOCAL_STATIC_LIBRARIES += charset

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

include $(call all-makefiles-under,$(LOCAL_PATH))

