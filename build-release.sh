#!/bin/bash

pushd webrtc_root
if [ ! -f tools/ninja/ninja ]; then
    pushd tools/ninja
    ./configure.py --bootstrap
    popd    
fi
popd

echo "Build WebRTC for Android armv7..."
./build-release-32.sh

echo "Build WebRTC for Android arm64..."
./build-release-64.sh
