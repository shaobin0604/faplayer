/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 to allow running VLC as root (uid 0). */
/* #undef ALLOW_RUN_AS_ROOT */

/* Define to 1 if the operating system uses UTF-8 internally */
#define ASSUME_UTF8 1

/* Maximum supported data alignment */
#define ATTRIBUTE_ALIGNED_MAX 64

/* Define to 1 if 3D Now! inline assembly is available. */
/* #undef CAN_COMPILE_3DNOW */

/* Define to 1 if AltiVec inline assembly is available. */
/* #undef CAN_COMPILE_ALTIVEC */

/* Define to 1 if C AltiVec extensions are available. */
/* #undef CAN_COMPILE_C_ALTIVEC */

/* Define to 1 inline MMX assembly is available. */
/* #undef CAN_COMPILE_MMX */

/* Define to 1 if MMX EXT inline assembly is available. */
/* #undef CAN_COMPILE_MMXEXT */

/* Define to 1 if SSE inline assembly is available. */
/* #undef CAN_COMPILE_SSE */

/* Define to 1 if SSE2 inline assembly is available. */
/* #undef CAN_COMPILE_SSE2 */

/* Define to 1 if SSE3 inline assembly is available. */
/* #undef CAN_COMPILE_SSE3 */

/* Define to 1 if SSE4A inline assembly is available. */
/* #undef CAN_COMPILE_SSE4A */

/* Define to 1 if SSE4_1 inline assembly is available. */
/* #undef CAN_COMPILE_SSE4_1 */

/* Define to 1 if SSE4_2 inline assembly is available. */
/* #undef CAN_COMPILE_SSE4_2 */

/* Define to 1 if SSSE3 inline assembly is available. */
/* #undef CAN_COMPILE_SSSE3 */

/* The ./configure command line */
#define CONFIGURE_LINE "./configure  '--host=arm-linux-androideabi' '--prefix=/opt/google/standalone-android-toolchain/sysroot' '--without-contrib' '--disable-non-utf8' '--disable-dbus' '--disable-dbus-control' '--disable-telepathy' '--disable-sout' '--disable-lua' '--disable-httpd' '--disable-libproxy' '--disable-vlm' '--disable-growl' '--disable-notify' '--disable-taglib' '--disable-live555' '--disable-dc1394' '--disable-dv' '--disable-dvdread' '--disable-dvdnav' '--disable-dshow' '--disable-bda' '--disable-opencv' '--disable-smb' '--disable-sftp' '--disable-dvbpsi' '--disable-v4l' '--disable-libv4l' '--disable-v4l2' '--disable-libv4l2' '--disable-pvr' '--disable-gnomevfs' '--disable-vcdx' '--disable-vcd' '--disable-libcddb' '--disable-dvb' '--disable-screen' '--disable-gme' '--disable-ogg' '--disable-mux_ogg' '--disable-shout' '--disable-mkv' '--disable-mod' '--disable-mpc' '--disable-wma-fixed' '--disable-shine' '--disable-omxil' '--disable-mad' '--disable-id3tag' '--disable-merge-ffmpeg' '--enable-avcodec' '--disable-libva' '--disable-dxva2' '--disable-switcher' '--enable-avformat' '--enable-swscale' '--disable-postproc' '--disable-faad' '--disable-twolame' '--disable-quicktime' '--disable-real' '--disable-realrtsp' '--disable-libtar' '--disable-a52' '--disable-dca' '--disable-flac' '--disable-libmpeg2' '--disable-vorbis' '--disable-tremor' '--disable-speex' '--disable-theora' '--disable-dirac' '--disable-schroedinger' '--disable-png' '--disable-x264' '--disable-fluidsynth' '--disable-zvbi' '--disable-telx' '--disable-libass' '--disable-asademux' '--disable-kate' '--disable-tiger' '--without-x' '--disable-xcb' '--disable-xvideo' '--disable-glx' '--disable-opengl' '--disable-sdl' '--disable-sdl-image' '--disable-freetype' '--disable-fribidi' '--disable-fontconfig' '--disable-libxml2' '--disable-svg' '--disable-snapshot' '--disable-hd1000v' '--disable-directx' '--disable-wingdi' '--disable-fb' '--disable-omapfb' '--disable-svgalib' '--disable-directfb' '--disable-ggi' '--disable-aa' '--disable-caca' '--disable-oss' '--disable-pulse' '--disable-portaudio' '--disable-alsa' '--disable-waveout' '--disable-macosx-audio' '--disable-hd1000a' '--disable-jack' '--disable-cyberlink' '--disable-upnp' '--disable-skins2' '--disable-hildon' '--disable-qt4' '--disable-macosx' '--disable-macosx-vout' '--disable-macosx-dialog-provider' '--disable-macosx-qtcapture' '--disable-macosx-eyetv' '--disable-macosx-vlc-app' '--disable-ncurses' '--disable-fbosd' '--disable-visual' '--disable-goom' '--disable-projectm' '--disable-atmo' '--disable-bonjour' '--disable-udev' '--disable-mtp' '--disable-lirc' '--disable-libgcrypt' '--disable-gnutls' '--disable-remoteosd' '--disable-osso_screensaver' '--disable-update-check' '--disable-sqlite' '--disable-loader' '--disable-vlc' '--disable-activex' '--disable-mozilla' 'host_alias=arm-linux-androideabi'"

/* Copyright string */
#define COPYRIGHT_MESSAGE "Copyright © 1996-2011 the VideoLAN team"

/* The copyright years */
#define COPYRIGHT_YEARS "1996-2011"

/* Disables runtime check for buggy glibc. */
/* #undef DISABLE_BUGGY_GLIBC_CHECK */

/* Binary specific version */
/* #undef DISTRO_VERSION */

/* Define if you want the HTTP dameon support */
/* #undef ENABLE_HTTPD */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Define to 1 for stream output support. */
/* #undef ENABLE_SOUT */

/* Define if you want the VideoLAN manager support */
/* #undef ENABLE_VLM */

/* Define to 1 if you have the <a52dec/a52.h> header file. */
/* #undef HAVE_A52DEC_A52_H */

/* Define to 1 if you have the `accept4' function. */
/* #undef HAVE_ACCEPT4 */

/* Define to 1 if <netdb.h> defines `struct addrinfo'. */
#define HAVE_ADDRINFO 1

/* Define to 1 if you have the <altivec.h> header file. */
/* #undef HAVE_ALTIVEC_H */

/* Define to 1 if you have the <ApplicationServices/ApplicationServices.h>
   header file. */
/* #undef HAVE_APPLICATIONSERVICES_APPLICATIONSERVICES_H */

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <asm/arch-omap/omapfb.h> header file. */
/* #undef HAVE_ASM_ARCH_OMAP_OMAPFB_H */

/* Define to 1 if you have the `asprintf' function. */
#define HAVE_ASPRINTF 1

/* Define to 1 if you have the `atof' function. */
#define HAVE_ATOF 1

/* Define to 1 if you have the `atoll' function. */
#define HAVE_ATOLL 1

/* Support for __attribute__((packed)) for structs */
#define HAVE_ATTRIBUTE_PACKED 1

/* Define if avcodec has to be built with DxVA2 support. */
/* #undef HAVE_AVCODEC_DXVA2 */

/* Define if avcodec has to be built with VAAPI support. */
/* #undef HAVE_AVCODEC_VAAPI */

/* Define to 1 if you have the `backtrace' function. */
/* #undef HAVE_BACKTRACE */

/* Define to 1 if you have the <Carbon/Carbon.h> header file. */
/* #undef HAVE_CARBON_CARBON_H */

/* Define to 1 if you have the <cascade/graphics/CascadeBitmap.h> header file.
   */
/* #undef HAVE_CASCADE_GRAPHICS_CASCADEBITMAP_H */

/* Define to 1 if you have the <cascade/graphics/CascadeScreen.h> header file.
   */
/* #undef HAVE_CASCADE_GRAPHICS_CASCADESCREEN_H */

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have clock_nanosleep. */
/* #undef HAVE_CLOCK_NANOSLEEP */

/* Define to 1 if you have the `connect' function. */
#define HAVE_CONNECT 1

/* Define to 1 if you have the <CoreAudio/CoreAudio.h> header file. */
/* #undef HAVE_COREAUDIO_COREAUDIO_H */

/* Define to 1 if you have the `ctime_r' function. */
#define HAVE_CTIME_R 1

/* Define to 1 if you have the <cybergarage/upnp/MediaServer.h> header file.
   */
/* #undef HAVE_CYBERGARAGE_UPNP_MEDIASERVER_H */

/* Define to 1 if you have the <d3d9.h> header file. */
/* #undef HAVE_D3D9_H */

/* Define to 1 if you have the `daemon' function. */
#define HAVE_DAEMON 1

/* Define if you have the D-BUS library */
/* #undef HAVE_DBUS */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the <ddraw.h> header file. */
/* #undef HAVE_DDRAW_H */

/* Define if <time.h> defines nanosleep. */
#define HAVE_DECL_NANOSLEEP 1

/* Define to 1 if you have the
   <deschutes/libraries/hdmachinex225/PCMAudioPlayer.h> header file. */
/* #undef HAVE_DESCHUTES_LIBRARIES_HDMACHINEX225_PCMAUDIOPLAYER_H */

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the BeOS dl */
/* #undef HAVE_DL_BEOS */

/* Define if you have the dlopen API */
#define HAVE_DL_DLOPEN 1

/* Define to 1 if you have the <dl.h> header file. */
/* #undef HAVE_DL_H */

/* Define if you have the shl_load API */
/* #undef HAVE_DL_SHL_LOAD */

/* Define if you have Windows' LoadLibrary */
/* #undef HAVE_DL_WINDOWS */

/* Define to 1 if you have the <dshow.h> header file. */
/* #undef HAVE_DSHOW_H */

/* Define to 1 if you have the `dup3' function. */
/* #undef HAVE_DUP3 */

/* Define to 1 if you have the <dvbpsi/dr.h> header file. */
/* #undef HAVE_DVBPSI_DR_H */

/* Define if you have dvbpsi_GenSDTSections. */
/* #undef HAVE_DVBPSI_SDT */

/* Define if you have dvdnav_get_video_resolution. */
/* #undef HAVE_DVDNAV_GET_VIDEO_RESOLUTION */

/* Define to 1 if you have the <dvdread/dvd_reader.h> header file. */
/* #undef HAVE_DVDREAD_DVD_READER_H */

/* Define to 1 if you have the <dxva2api.h> header file. */
/* #undef HAVE_DXVA2API_H */

/* Define if dynamic plugins are supported */
#define HAVE_DYNAMIC_PLUGINS 1

/* Define to 1 if you have the <ebml/EbmlVersion.h> header file. */
/* #undef HAVE_EBML_EBMLVERSION_H */

/* Define to 1 if you have the `eventfd' function. */
/* #undef HAVE_EVENTFD */

/* Define to 1 if you have the <execinfo.h> header file. */
/* #undef HAVE_EXECINFO_H */

/* Define to 1 if you have the <faad.h> header file. */
/* #undef HAVE_FAAD_H */

/* Define to 1 if you have the `fcntl' function. */
#define HAVE_FCNTL 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fdatasync' function. */
/* #undef HAVE_FDATASYNC */

/* Define to 1 if you have the `fdopendir' function. */
#define HAVE_FDOPENDIR 1

/* Define to 1 if you have the <ffmpeg/avcodec.h> header file. */
/* #undef HAVE_FFMPEG_AVCODEC_H */

/* Define to 1 if you have the <ffmpeg/avformat.h> header file. */
/* #undef HAVE_FFMPEG_AVFORMAT_H */

/* Define to 1 if you have the <ffmpeg/avutil.h> header file. */
/* #undef HAVE_FFMPEG_AVUTIL_H */

/* Define to 1 if you have the <ffmpeg/swscale.h> header file. */
/* #undef HAVE_FFMPEG_SWSCALE_H */

/* Define to 1 if you have the <fontconfig/fontconfig.h> header file. */
/* #undef HAVE_FONTCONFIG_FONTCONFIG_H */

/* Define to 1 if you have the `fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the `fstatvfs' function. */
/* #undef HAVE_FSTATVFS */

/* Define to 1 if you have the `gai_strerror' function. */
#define HAVE_GAI_STRERROR 1

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the `getdelim' function. */
/* #undef HAVE_GETDELIM */

/* Define to 1 if you have the `getenv' function. */
#define HAVE_GETENV 1

/* Define to 1 if you have the `getnameinfo' function. */
#define HAVE_GETNAMEINFO 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getpid' function. */
#define HAVE_GETPID 1

/* Define to 1 if you have the `getpwuid_r' function. */
/* #undef HAVE_GETPWUID_R */

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define to 1 if you have the `gmtime_r' function. */
#define HAVE_GMTIME_R 1

/* Define to 1 if you have the <Growl/GrowlDefines.h> header file. */
/* #undef HAVE_GROWL_GROWLDEFINES_H */

/* Define if you have the iconv() function and it works. */
#define HAVE_ICONV 1

/* Define to 1 if you have the <id3tag.h> header file. */
/* #undef HAVE_ID3TAG_H */

/* Define to 1 if you have the <image.h> header file. */
/* #undef HAVE_IMAGE_H */

/* Define to 1 if you have the `inet_aton' function. */
#define HAVE_INET_ATON 1

/* Define to 1 if you have inet_ntop(). */
#define HAVE_INET_NTOP 1

/* Define to 1 if you have inet_pton(). */
#define HAVE_INET_PTON 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* For FreeBSD VCD support */
/* #undef HAVE_IOC_TOC_HEADER_IN_SYS_CDIO_H */

/* Define to 1 if you have the `isatty' function. */
#define HAVE_ISATTY 1

/* Define to 1 if you have the <kate/kate.h> header file. */
/* #undef HAVE_KATE_KATE_H */

/* Define to 1 if you have the <lauxlib.h> header file. */
/* #undef HAVE_LAUXLIB_H */

/* Define to 1 if you have the <libavcodec/avcodec.h> header file. */
#define HAVE_LIBAVCODEC_AVCODEC_H 1

/* Define to 1 if you have the <libavcodec/dxva2.h> header file. */
/* #undef HAVE_LIBAVCODEC_DXVA2_H */

/* Define to 1 if you have the <libavcodec/vaapi.h> header file. */
/* #undef HAVE_LIBAVCODEC_VAAPI_H */

/* Define to 1 if you have the <libavformat/avformat.h> header file. */
#define HAVE_LIBAVFORMAT_AVFORMAT_H 1

/* Define to 1 if you have the <libavformat/avio.h> header file. */
#define HAVE_LIBAVFORMAT_AVIO_H 1

/* Define to 1 if you have the <libavutil/avutil.h> header file. */
#define HAVE_LIBAVUTIL_AVUTIL_H 1

/* Define this if you have libcddb installed */
/* #undef HAVE_LIBCDDB */

/* Define to 1 if you have the <libdvdread/dvd_reader.h> header file. */
/* #undef HAVE_LIBDVDREAD_DVD_READER_H */

/* Define to 1 if you have the `mingw32' library (-lmingw32). */
/* #undef HAVE_LIBMINGW32 */

/* Define to 1 if you have the `ncurses' library (-lncurses). */
/* #undef HAVE_LIBNCURSES */

/* Define to 1 if you have the `ncursesw' library (-lncursesw). */
/* #undef HAVE_LIBNCURSESW */

/* Define if libproxy is available */
/* #undef HAVE_LIBPROXY */

/* Define to 1 if you have the <libsmbclient.h> header file. */
/* #undef HAVE_LIBSMBCLIENT_H */

/* Define to 1 if you have the <libssh2.h> header file. */
/* #undef HAVE_LIBSSH2_H */

/* Define to 1 if you have the <libswscale/swscale.h> header file. */
#define HAVE_LIBSWSCALE_SWSCALE_H 1

/* Define to 1 if you have the <libtar.h> header file. */
/* #undef HAVE_LIBTAR_H */

/* Define if libv4l is available */
/* #undef HAVE_LIBV4L1 */

/* Define if libv4l2 is available */
/* #undef HAVE_LIBV4L2 */

/* Define to 1 if you have the <linux/dccp.h> header file. */
#define HAVE_LINUX_DCCP_H 1

/* Define to 1 if you have the <linux/dvb/frontend.h> header file. */
/* #undef HAVE_LINUX_DVB_FRONTEND_H */

/* Define to 1 if you have the <linux/dvb/version.h> header file. */
/* #undef HAVE_LINUX_DVB_VERSION_H */

/* Define to 1 if you have the <linux/fb.h> header file. */
/* #undef HAVE_LINUX_FB_H */

/* Define to 1 if you have the <linux/magic.h> header file. */
#define HAVE_LINUX_MAGIC_H 1

/* Define to 1 if you have the <linux/version.h> header file. */
#define HAVE_LINUX_VERSION_H 1

/* Define to 1 if you have the <linux/videodev2.h> header file. */
/* #undef HAVE_LINUX_VIDEODEV2_H */

/* Define to 1 if you have the <linux/videodev.h> header file. */
/* #undef HAVE_LINUX_VIDEODEV_H */

/* Define to 1 if you have the <liveMedia.hh> header file. */
/* #undef HAVE_LIVEMEDIA_HH */

/* Define to 1 if you have the <liveMedia_version.hh> header file. */
/* #undef HAVE_LIVEMEDIA_VERSION_HH */

/* Define to 1 if you have the `lldiv' function. */
#define HAVE_LLDIV 1

/* Define to 1 if you have the `load_add_on' function. */
/* #undef HAVE_LOAD_ADD_ON */

/* Define to 1 if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define to 1 if you have the `localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if you have the lrintf function */
#define HAVE_LRINTF 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the <lualib.h> header file. */
/* #undef HAVE_LUALIB_H */

/* Define to 1 if you have the <lua.h> header file. */
/* #undef HAVE_LUA_H */

/* Define to 1 if you have the <machine/param.h> header file. */
/* #undef HAVE_MACHINE_PARAM_H */

/* Define to 1 if you have the <mad.h> header file. */
/* #undef HAVE_MAD_H */

/* Define to 1 if building for the Maemo platform. */
/* #undef HAVE_MAEMO */

/* Define to 1 if you have the <matroska/KaxAttachments.h> header file. */
/* #undef HAVE_MATROSKA_KAXATTACHMENTS_H */

/* Define to 1 if you have the <matroska/KaxVersion.h> header file. */
/* #undef HAVE_MATROSKA_KAXVERSION_H */

/* Define to 1 if you have the `memalign' function. */
#define HAVE_MEMALIGN 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if MMX intrinsics are available. */
/* #undef HAVE_MMX_INTRINSICS */

/* Define to 1 if you have the <mozilla-config.h> header file. */
/* #undef HAVE_MOZILLA_CONFIG_H */

/* Define to 1 if you have the <mpcdec/mpcdec.h> header file. */
/* #undef HAVE_MPCDEC_MPCDEC_H */

/* Define to 1 if you have the <mpc/mpcdec.h> header file. */
/* #undef HAVE_MPC_MPCDEC_H */

/* Define if nanosleep is available. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have libncursesw. */
/* #undef HAVE_NCURSESW */

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <netinet/udplite.h> header file. */
/* #undef HAVE_NETINET_UDPLITE_H */

/* Define to 1 if you have the <net/if.h> header file. */
#define HAVE_NET_IF_H 1

/* Define if new linux/videodev2.h present */
/* #undef HAVE_NEW_LINUX_VIDEODEV2_H */

/* Define to 1 if you have the <npapi.h> header file. */
/* #undef HAVE_NPAPI_H */

/* Define to 1 if you have the <npruntime.h> header file. */
/* #undef HAVE_NPRUNTIME_H */

/* Define to 1 if you have the `nrand48' function. */
#define HAVE_NRAND48 1

/* Define to 1 if you have the <objsafe.h> header file. */
/* #undef HAVE_OBJSAFE_H */

/* Define to 1 if you have the <ole2.h> header file. */
/* #undef HAVE_OLE2_H */

/* Define to 1 if you have the <olectl.h> header file. */
/* #undef HAVE_OLECTL_H */

/* Define to 1 if you have the `openat' function. */
#define HAVE_OPENAT 1

/* Define to 1 if you have the <OpenGL/gl.h> header file. */
/* #undef HAVE_OPENGL_GL_H */

/* Define to 1 if you have the <png.h> header file. */
/* #undef HAVE_PNG_H */

/* Define to 1 if the OS is usabl... err, has poll(). */
#define HAVE_POLL 1

/* Define to 1 if you have the `posix_fadvise' function. */
/* #undef HAVE_POSIX_FADVISE */

/* Define to 1 if you have the `posix_madvise' function. */
/* #undef HAVE_POSIX_MADVISE */

/* Define to 1 if you have the `posix_memalign' function. */
/* #undef HAVE_POSIX_MEMALIGN */

/* Define to 1 if you have the <postproc/postprocess.h> header file. */
/* #undef HAVE_POSTPROC_POSTPROCESS_H */

/* Define to 1 if using libprojectM 2.x */
/* #undef HAVE_PROJECTM2 */

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define to 1 if you have the `pthread_mutex_lock' function. */
#define HAVE_PTHREAD_MUTEX_LOCK 1

/* Define to 1 if you have the <QuickTime/QuickTime.h> header file. */
/* #undef HAVE_QUICKTIME_QUICKTIME_H */

/* Define to 1 if you have the `rewind' function. */
#define HAVE_REWIND 1

/* For NetBSD VCD support */
/* #undef HAVE_SCSIREQ_IN_SYS_SCSIIO_H */

/* Define to 1 if you have the <scsi/scsi.h> header file. */
/* #undef HAVE_SCSI_SCSI_H */

/* Define to 1 if you have the `send' function. */
#define HAVE_SEND 1

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define to 1 if you have the <soundcard.h> header file. */
/* #undef HAVE_SOUNDCARD_H */

/* Define to 1 if you have the <sqlite3.h> header file. */
/* #undef HAVE_SQLITE3_H */

/* Define to 1 if SSE2 intrinsics are available. */
/* #undef HAVE_SSE2_INTRINSICS */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strcasestr' function. */
#define HAVE_STRCASESTR 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `stricmp' function. */
/* #undef HAVE_STRICMP */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcpy' function. */
#define HAVE_STRLCPY 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strnicmp' function. */
/* #undef HAVE_STRNICMP */

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define to 1 if you have the `strtof' function. */
#define HAVE_STRTOF 1

/* Define to 1 if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define to 1 if you have the `strtoll' function. */
#define HAVE_STRTOLL 1

/* Define to 1 if the system has the type `struct addrinfo'. */
#define HAVE_STRUCT_ADDRINFO 1

/* Define if <sys/time.h> defines struct timespec. */
#define HAVE_STRUCT_TIMESPEC 1

/* Define to 1 if you have the `swab' function. */
/* #undef HAVE_SWAB */

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/dl.h> header file. */
/* #undef HAVE_SYS_DL_H */

/* Define to 1 if you have the <sys/eventfd.h> header file. */
/* #undef HAVE_SYS_EVENTFD_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/shm.h> header file. */
/* #undef HAVE_SYS_SHM_H */

/* Define to 1 if you have the <sys/soundcard.h> header file. */
/* #undef HAVE_SYS_SOUNDCARD_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/videoio.h> header file. */
/* #undef HAVE_SYS_VIDEOIO_H */

/* Define to 1 if you have the <taglib/mp4coverart.h> header file. */
/* #undef HAVE_TAGLIB_MP4COVERART_H */

/* Define to 1 if you have the `tdestroy' function. */
/* #undef HAVE_TDESTROY */

/* Define if libtiger is available. */
/* #undef HAVE_TIGER */

/* Define to 1 if you have the <tremor/ivorbiscodec.h> header file. */
/* #undef HAVE_TREMOR_IVORBISCODEC_H */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <unzip.h> header file. */
/* #undef HAVE_UNZIP_H */

/* Define to 1 if you have the `uselocale' function. */
/* #undef HAVE_USELOCALE */

/* Define to 1 if you have the <valgrind/valgrind.h> header file. */
/* #undef HAVE_VALGRIND_VALGRIND_H */

/* Define to 1 if you have the `vasprintf' function. */
#define HAVE_VASPRINTF 1

/* Define if <stdarg.h> defines va_copy. */
#define HAVE_VA_COPY 1

/* Define to 1 if you have the `vmsplice' function. */
/* #undef HAVE_VMSPLICE */

/* Define to 1 if you have the <X11/Xlib.h> header file. */
/* #undef HAVE_X11_XLIB_H */

/* Define to 1 if you have the <X11/xpm.h> header file. */
/* #undef HAVE_X11_XPM_H */

/* Define to 1 if you have the <xlocale.h> header file. */
/* #undef HAVE_XLOCALE_H */

/* Define to 1 if you have the <xosd.h> header file. */
/* #undef HAVE_XOSD_H */

/* Define to 1 if you have the <zlib.h> header file. */
#define HAVE_ZLIB_H 1

/* Define if <stdarg.h> defines __va_copy. */
#define HAVE___VA_COPY 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST 

/* Dynamic object extension */
#define LIBEXT ".so"

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to 1 if debug code should NOT be compiled */
#define NDEBUG 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Define if <sys/param.h> defines ntohl. */
/* #undef NTOHL_IN_SYS_PARAM_H */

/* Define if you want to optimize memory usage over performance */
/* #undef OPTIMIZE_MEMORY */

/* Name of package */
#define PACKAGE "vlc"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "vlc"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "vlc 1.1.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "vlc"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.1.7"

/* version minor number */
#define PACKAGE_VERSION_EXTRA ""

/* version major number */
#define PACKAGE_VERSION_MAJOR "1"

/* version minor number */
#define PACKAGE_VERSION_MINOR "1"

/* version minor number */
#define PACKAGE_VERSION_REVISION "7"

/* Define if you want to use SQLite module */
/* #undef SQLITE_MODULE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if <strings.h> defines strncasecmp. */
#define STRNCASECMP_IN_STRINGS_H 1

/* Define if you want to use the VLC update mechanism */
/* #undef UPDATE_CHECK */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "1.1.7"

/* Simple version string */
#define VERSION_MESSAGE "1.1.7 The Luggage"

/* Location of videodev2.h */
/* #undef VIDEODEV2_H_FILE */

/* compiler */
#define VLC_COMPILER "gcc version 4.4.3 (GCC) "

/* user who ran configure */
#define VLC_COMPILE_BY "luka"

/* host which ran configure */
#define VLC_COMPILE_HOST "luka-laptop"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Define if the zvbi module is built */
/* #undef ZVBI_COMPILED */

/* Define to '2' to get glibc warnings. */
#define _FORTIFY_SOURCE 2

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to '0x0500' for Windows 2000 APIs. */
/* #undef _WIN32_WINNT */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Alias fdatasync() to fsync() if missing. */
#define fdatasync fsync

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define to `sockaddr' if <sys/socket.h> does not define. */
/* #undef sockaddr_storage */

/* Define to `int' if <sys/socket.h> does not define. */
/* #undef socklen_t */

/* Define to `sa_family' if <sys/socket.h> does not define. */
/* #undef ss_family */

/* Define to `int' if <stddef.h> does not define. */
/* #undef ssize_t */

#include <vlc_fixups.h>
