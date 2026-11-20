#!/bin/bash

rm -f libs/x86_64/libjingle_peerconnection_so.so

ninja -C out-x64/Release libjingle_peerconnection_so || exit 1

mkdir -p libs/x86_64
cp out-x64/Release/libjingle_peerconnection_so.so libs/x86_64/ || exit 1
