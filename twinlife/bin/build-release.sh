#!/bin/bash

if [ ! -f tools/ninja/ninja ]; then
    pushd tools/ninja
    ./configure.py --bootstrap
    popd    
fi

echo "Build WebRTC for Android armv7..."
./twinlife/bin/build-release-32.sh

echo "Build WebRTC for Android arm64..."
./twinlife/bin/build-release-64.sh
