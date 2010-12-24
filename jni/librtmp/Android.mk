LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := rtmp

LOCAL_CFLAGS += -DRTMPDUMP_VERSION=\"v2.3\" -DNO_CRYPTO

LOCAL_SRC_FILES := \
    rtmp.c \
    log.c \
    amf.c \
    hashswf.c \
    parseurl.c

include $(BUILD_SHARED_LIBRARY)

