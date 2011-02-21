
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := vlccore

LOCAL_CFLAGS += \
    -std=c99 \
    -D__THROW= \
    -DHAVE_CONFIG_H \
    -DNDEBUG \
    -DMODULE_STRING=\"main\" \
    -D__LIBVLC__ \
    -D__arm__ \
    -D__linux__ \
    -DPACKAGENAME=\"org.stagex\" \
    -DLOCALEDIR=\"/data/data/\"PACKAGENAME\"/cache/locale\" \
    -DSYSCONFDIR=\"/data/data/\"PACKAGENAME\"/cache/etc\" \
    -DDATA_PATH=\"/data/data/\"PACKAGENAME\"/cache/share\" \
    -DPKGLIBDIR=\"/data/data/\"PACKAGENAME\"/cache/lib\"

LOCAL_CFLAGS += $(COMMON_OPT_CFLAGS)

LOCAL_C_INCLUDES += \
    $(VLCROOT)/compat \
    $(VLCROOT) \
    $(VLCROOT)/include \
    $(VLCROOT)/src \
    $(EXTROOT)/cpufeatures \
    $(EXTROOT)/iconv/include

LOCAL_SRC_FILES := \
    src/jni.c \
    src/libvlc.c \
    src/libvlc-module.c \
    src/missing.c \
    src/revision.c \
    src/version.c \
    src/audio_output/common.c \
    src/audio_output/dec.c \
    src/audio_output/filters.c \
    src/audio_output/input.c \
    src/audio_output/intf.c \
    src/audio_output/mixer.c \
    src/audio_output/output.c \
    src/config/chain.c \
    src/config/cmdline.c \
    src/config/core.c \
    src/config/dirs.c \
    src/config/dirs_android.c \
    src/config/file.c \
    src/config/getopt.c \
    src/config/intf.c \
    src/config/keys.c \
    src/control/audio.c \
    src/control/core.c \
    src/control/error.c \
    src/control/event.c \
    src/control/event_async.c \
    src/control/log.c \
    src/control/media.c \
    src/control/media_discoverer.c \
    src/control/media_library.c \
    src/control/media_list.c \
    src/control/media_list_player.c \
    src/control/media_player.c \
    src/control/playlist.c \
    src/control/video.c \
    src/extras/libc.c \
    src/extras/tdestroy.c \
    src/input/access.c \
    src/input/clock.c \
    src/input/control.c \
    src/input/decoder.c \
    src/input/decoder_synchro.c \
    src/input/demux.c \
    src/input/es_out.c \
    src/input/es_out_timeshift.c \
    src/input/event.c \
    src/input/input.c \
    src/input/item.c \
    src/input/meta.c \
    src/input/resource.c \
    src/input/stream.c \
    src/input/stream_demux.c \
    src/input/stream_filter.c \
    src/input/stream_memory.c \
    src/input/subtitles.c \
    src/input/var.c \
    src/interface/dialog.c \
    src/interface/interface.c \
    src/misc/action.c \
    src/misc/block.c \
    src/misc/cpu.c \
    src/misc/debug.c \
    src/misc/epg.c \
    src/misc/error.c \
    src/misc/es_format.c \
    src/misc/events.c \
    src/misc/filter.c \
    src/misc/filter_chain.c \
    src/misc/fourcc.c \
    src/misc/http_auth.c \
    src/misc/image.c \
    src/misc/linux_specific.c \
    src/misc/md5.c \
    src/misc/messages.c \
    src/misc/mtime.c \
    src/misc/objects.c \
    src/misc/picture_fifo.c \
    src/misc/picture_pool.c \
    src/misc/probe.c \
    src/misc/pthread.c \
    src/misc/rand.c \
    src/misc/sql.c \
    src/misc/stats.c \
    src/misc/threads.c \
    src/misc/update.c \
    src/misc/update_crypto.c \
    src/misc/variables.c \
    src/misc/xml.c \
    src/modules/cache.c \
    src/modules/entry.c \
    src/modules/modules.c \
    src/modules/os.c \
    src/modules/textdomain.c \
    src/network/acl.c \
    src/network/getaddrinfo.c \
    src/network/io.c \
    src/network/poll.c \
    src/network/rootbind.c \
    src/network/tcp.c \
    src/network/tls.c \
    src/network/udp.c \
    src/osd/osd.c \
    src/osd/osd_text.c \
    src/osd/osd_widgets.c \
    src/playlist/art.c \
    src/playlist/control.c \
    src/playlist/engine.c \
    src/playlist/fetcher.c \
    src/playlist/item.c \
    src/playlist/loadsave.c \
    src/playlist/preparser.c \
    src/playlist/search.c \
    src/playlist/services_discovery.c \
    src/playlist/sort.c \
    src/playlist/thread.c \
    src/playlist/tree.c \
    src/text/charset.c \
    src/text/filesystem.c \
    src/text/iso_lang.c \
    src/text/strings.c \
    src/text/unicode.c \
    src/video_output/display.c \
    src/video_output/inhibit.c \
    src/video_output/snapshot.c \
    src/video_output/video_epg.c \
    src/video_output/video_output.c \
    src/video_output/video_text.c \
    src/video_output/video_widgets.c \
    src/video_output/vout_intf.c \
    src/video_output/vout_pictures.c \
    src/video_output/vout_subpictures.c \
    src/video_output/window.c

LOCAL_LDLIBS += -llog

LOCAL_STATIC_LIBRARIES += compat cpufeatures 

# please try to use the latest NDK
# http://code.google.com/p/android/issues/detail?id=9439
LOCAL_WHOLE_STATIC_LIBRARIES += iconv charset freetype ass

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))

