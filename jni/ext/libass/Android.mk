
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := ass

LOCAL_CFLAGS += \
    -DHAVE_CONFIG_H

LOCAL_C_INCLUDES += \
    $(EXTROOT)/iconv/include \
    $(EXTROOT)/freetype/include \
    $(LOCAL_PATH)

LOCAL_SRC_FILES := \
    libass/ass.c \
    libass/ass_bitmap.c \
    libass/ass_cache.c \
    libass/ass_drawing.c \
    libass/ass_font.c \
    libass/ass_fontconfig.c \
    libass/ass_library.c \
    libass/ass_parse.c \
    libass/ass_render.c \
    libass/ass_render_api.c \
    libass/ass_strtod.c \
    libass/ass_utils.c

include $(BUILD_STATIC_LIBRARY)

