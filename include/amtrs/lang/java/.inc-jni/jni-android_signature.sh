#!/bin/sh

NDK_ROOT=$(dirname $(which ndk-build.cmd))
SDK_ROOT=$(dirname $NDK_ROOT)

javah -d . -classpath $SDK_ROOT/platforms/android-28/android.jar $1
