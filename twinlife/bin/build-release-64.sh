#!/bin/bash

if test -f buildtools/mac/gn; then
    TARGET=ios
else
    TARGET=android
fi

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

	ninja -C out-64/Release libjingle_peerconnection_so

	mkdir -p libs/arm64-v8a
	cp out-64/Release/libjingle_peerconnection_so.so libs/arm64-v8a

	;;

esac


