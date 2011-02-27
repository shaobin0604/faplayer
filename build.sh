#!/bin/sh

rm -rf libs
ndk-build V=1 -j3 || exit 1
rm -rf assests/lib
ruby trivial.rb
rm -rf bin
ant release
test -f bin/faplayer-unsigned.apk || exit 1
java -jar signapk.jar testkey.x509.pem testkey.pk8 bin/faplayer-unsigned.apk bin/faplayer.apk
echo "Done"

