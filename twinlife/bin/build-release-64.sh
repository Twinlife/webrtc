#!/bin/bash

rm -f libs/arm64-v8a/libjingle_peerconnection_so.so

ninja -C out-64/Release libjingle_peerconnection_so

mkdir -p libs/arm64-v8a
cp out-64/Release/libjingle_peerconnection_so.so libs/arm64-v8a


