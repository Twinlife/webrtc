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
	echo "Usage: build.sh [ios|android|linux]" 1>&2
	exit 1
	;;
esac

case ${TARGET} in
    ios)
	echo "Init WebRTC build for iOS arm64..."
	./twinlife/bin/build-64.sh ${TARGET}

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
	./twinlife/bin/build-32.sh ${TARGET}

	echo "Init WebRTC build for Android arm64..."
	./twinlife/bin/build-64.sh ${TARGET}
	;;

    linux) 
	. build/android/envsetup.sh

	echo "Init WebRTC build for Linux x86_64..."
        export JAVA_HOME=`pwd`/third_party/jdk/current
	./twinlife/bin/build-64.sh ${TARGET}
       ;;

    *)
       echo "Target not recognized: ${TARGET}"
       exit 1
       ;;
esac
