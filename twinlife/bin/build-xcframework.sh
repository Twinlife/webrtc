#!/bin/bash
# Package the iOS device and simulator libraries with the public headers into
# out-xcframework/libwebrtc.xcframework, so that Xcode picks the right one for each destination.
# Both libraries are arm64, so they can't be merged with lipo.

DEVICE_LIB=out-64/Release/obj/libwebrtc.a
SIMULATOR_LIB=out-ios-sim/Release/obj/libwebrtc.a

for lib in ${DEVICE_LIB} ${SIMULATOR_LIB}; do
    if test ! -f ${lib}; then
        echo "Missing ${lib}: run build-release.sh ios and build-release.sh ios-sim first" 1>&2
        exit 1
    fi
done

./twinlife/bin/collect-objc.sh || exit 1

rm -rf out-xcframework/libwebrtc.xcframework
mkdir -p out-xcframework
xcodebuild -create-xcframework \
    -library ${DEVICE_LIB} -headers out-objc \
    -library ${SIMULATOR_LIB} -headers out-objc \
    -output out-xcframework/libwebrtc.xcframework || exit 1

ls -l out-xcframework/libwebrtc.xcframework
