#!/bin/sh
ROOT=`pwd`/
rm -rf out-java
mkdir -p out-java/org/webrtc
mkdir -p out-java/org/twinlife/twinlife/crypto
cp -p ${ROOT}/sdk/android/src/java/org/webrtc/*.java out-java/org/webrtc/
cp -p ${ROOT}/sdk/android/api/org/webrtc/*.java out-java/org/webrtc/
cp -p ${ROOT}/sdk/android/api/org/webrtc/*.java out-java/org/webrtc/
cp -p ${ROOT}/sdk/android/api/org/twinlife/twinlife/crypto/*.java out-java/org/twinlife/twinlife/crypto/
mkdir -p out-java/org/webrtc/audio
cp -p ${ROOT}/sdk/android/api/org/webrtc/audio/*.java out-java/org/webrtc/audio
cp -p ${ROOT}/sdk/android/src/java/org/webrtc/audio/*.java out-java/org/webrtc/audio




