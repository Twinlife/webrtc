#!/bin/bash

if test -f buildtools/mac/gn; then
    TARGET=ios
else
    TARGET=android
fi

case ${TARGET} in
    ios)
	echo "Init WebRTC build for iOS arm64..."
	./twinlife/bin/build-64.sh

	#echo "Fixing out-64/Release/obj/buildtools/third_party/libc++/libc++_static.ninja"
	#(head -5 out-64/Release/obj/buildtools/third_party/libc++/libc++.ninja &&
	#     tail -n +6 out-64/Release/obj/buildtools/third_party/libc++/libc++_static.ninja ) > libc++_static-64.ninja
	#mv libc++_static-64.ninja out-64/Release/obj/buildtools/third_party/libc++/libc++_static.ninja

	#echo "Fixing out-64/Release/obj/buildtools/third_party/libc++abi/libc++abi_static.ninja"
	#(head -5 out-64/Release/obj/buildtools/third_party/libc++abi/libc++abi.ninja &&
	#     tail -n +6 out-64/Release/obj/buildtools/third_party/libc++abi/libc++abi_static.ninja ) > libc++abi_static-64.ninja
	#mv libc++abi_static-64.ninja out-64/Release/obj/buildtools/third_party/libc++abi/libc++abi_static.ninja
	;;

    android)
	. build/android/envsetup.sh

	echo "Init WebRTC build for Android armv7..."
	./twinlife/bin/build-32.sh

	echo "Init WebRTC build for Android arm64..."
	./twinlife/bin/build-64.sh
	;;
esac
