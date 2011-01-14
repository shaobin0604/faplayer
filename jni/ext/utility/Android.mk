LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := utility

LOCAL_SRC_FILES := \
    utility.c

include $(BUILD_STATIC_LIBRARY)

