
NDK=/opt/android-ndk-r4b
AND=4

./configure \
--enable-cross-compile \
--cross-prefix=arm-none-linux-gnueabi- \
--sysinclude=$NDK/build/platforms/android-$AND/arch-arm/usr/include \
--arch=armv6j \
--cpu=arm1136jf-s \
--target-os=linux \
--enable-pic
