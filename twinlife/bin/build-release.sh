#!/bin/bash

if test -f buildtools/mac/gn; then
    TARGET=ios
else
    TARGET=android
fi

case ${TARGET} in
    ios)
	echo "Build WebRTC for iOS arm64..."
	./twinlife/bin/build-release-64.sh
	;;

    android)
	echo "Build WebRTC for Android armv7..."
	./twinlife/bin/build-release-32.sh

	echo "Build WebRTC for Android arm64..."
	./twinlife/bin/build-release-64.sh
	;;
esac
