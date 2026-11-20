#!/bin/bash
# Build the WebRTC static library for the iOS Simulator (configured by build-ios-sim.sh).

ninja -C out-ios-sim/Release obj/libwebrtc.a || exit 1

echo "Generate WebRTC library for the iOS Simulator..."
# Same as the iOS device build in build-release-64.sh: add all the objects to libwebrtc.a.
(cd out-ios-sim/Release/obj/sdk && ar cr ../libwebrtc.a `find .. -name '*.o'`) || exit 1
ls -l out-ios-sim/Release/obj/libwebrtc.a
