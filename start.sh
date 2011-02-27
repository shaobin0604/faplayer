#!/bin/sh

adb uninstall org.stagex
adb install bin/faplayer.apk
adb shell 'am start -a android.intent.action.MAIN -n org.stagex/.activity.FileListActivity'

