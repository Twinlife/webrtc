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
	echo "Usage: build-release-64.sh [ios|android|linux]" 1>&2
	exit 1
	;;
esac

case ${TARGET} in
    ios)
	rm -f libs/arm64-v8a/libjingle_peerconnection_so.so
	ninja -C out-64/Release obj/libwebrtc.a
	echo "Generate WebRTC universal library for iOS..."
	if test -f out-64/Release/obj/libwebrtc.a; then
	    (cd out-64/Release/obj/sdk && ar cr ../libwebrtc.a `find .. -name '*.o'`)
	    ls -l out-64/Release/obj/libwebrtc.a
	else
	    echo "Webrtc library not found!"
	fi
	;;

    android)
	rm -f libs/arm64-v8a/libjingle_peerconnection_so.so

	ninja -k50 -C out-64/Release libjingle_peerconnection_so

	mkdir -p libs/arm64-v8a
	cp out-64/Release/libjingle_peerconnection_so.so libs/arm64-v8a

	;;

    linux)
	rm -f libs/arm64-v8a/libjingle_peerconnection_so.so
        export JAVA_HOME=`pwd`/third_party/jdk/current
        ninja -k50 -C out-64/Release libjingle_peerconnection_so
        ;;

esac


