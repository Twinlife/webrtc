#!/bin/bash

case $# in
    0)
        if test -f buildtools/mac/gn; then
            TARGET=ios
            gn=buildtools/mac/gn
        else
            TARGET=android
            gn=buildtools/linux64/gn
        fi
        ;;
    1)
        TARGET=$1
        ;;
    *)
	echo "Usage: build-release.sh [ios|android|linux]" 1>&2
	exit 1
	;;
esac

case ${TARGET} in
    ios)
	echo "Build WebRTC for iOS arm64..."
	./twinlife/bin/build-release-64.sh ios
	;;

    android)
	echo "Build WebRTC for Android armv7..."
	./twinlife/bin/build-release-32.sh android

	echo "Build WebRTC for Android arm64..."
	./twinlife/bin/build-release-64.sh android
	;;

    linux)
	echo "Build WebRTC for Linux x86_64..."
	./twinlife/bin/build-release-64.sh linux
	;;

esac
