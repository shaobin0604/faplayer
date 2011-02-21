#APP_ABI := armeabi armeabi-v7a
APP_OPTIM := release
APP_CFLAGS := $(APP_CFLAGS) -O3 -ftree-vectorize -ffast-math -fsingle-precision-constant
APP_CPPFLAGS := $(APP_CPPFLAGS) -O3 -ftree-vectorize -ffast-math -fsingle-precision-constant
