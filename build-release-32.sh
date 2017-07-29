#!/bin/bash

rm -f libs/armeabi-v7a/libjingle_peerconnection_so.so

tools/ninja/ninja -C out-32/Release libjingle_peerconnection_so

mkdir -p libs/armeabi-v7a
cp out-32/Release/libjingle_peerconnection_so.so libs/armeabi-v7a/

