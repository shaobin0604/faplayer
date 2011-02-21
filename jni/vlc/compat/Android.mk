LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := compat

LOCAL_CFLAGS += \
    -Drestrict=__restrict \
    -DANDROID_SMP=0 \
    -D__LIBC_HIDDEN__="__attribute__ ((visibility (\"hidden\")))" \
    -D__THROW= \
    -D__USE_GNU \
    -Dinternal_function= \
    -DCLONE_SETTLS=0x00080000 \
    -DCLONE_PARENT_SETTID=0x00100000 \
    -DCLONE_CHILD_SETTID=0x01000000

LOCAL_CFLAGS += $(COMMON_OPT_CFLAGS)

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src

LOCAL_SRC_FILES := \
    bionic_clone.c \
    clone.S \
    pthread.c \
    pthread-atfork.c \
    pthread-cancel.c \
    pthread-rwlocks.c \
    pthread-timers.c \
    atomics_arm.S

LOCAL_SRC_FILES += \
    hsearch.c \
    hsearch_r.c \
    tsearch.c

LOCAL_SRC_FILES += \
    getdelim.c

include $(BUILD_STATIC_LIBRARY)

