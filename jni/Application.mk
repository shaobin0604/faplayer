#APP_ABI := armeabi armeabi-v7a
APP_OPTIM := release
OPT_CFLAGS := -O3 -ffast-math -fsingle-precision-constant
OPT_CPPFLAGS := $(OPT_CLFAGS)
APP_CFLAGS := $(APP_CFLAGS) $(OPT_CFLAGS)
APP_CPPFLAGS := $(APP_CPPFLAGS) $(OPT_CPPFLAGS)
