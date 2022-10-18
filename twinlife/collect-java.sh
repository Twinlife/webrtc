#!/bin/sh
ROOT=`pwd`/
rm -rf out-java
mkdir -p out-java/org/webrtc
cp -p ${ROOT}/sdk/android/src/java/org/webrtc/*.java out-java/org/webrtc/
cp -p ${ROOT}/sdk/android/api/org/webrtc/*.java out-java/org/webrtc/
cp -p ${ROOT}/rtc_base/java/src/org/webrtc/*.java out-java/org/webrtc/
mkdir -p out-java/org/webrtc/voiceengine
cp -p ${ROOT}/modules/audio_device/android/java/src/org/webrtc/voiceengine/*.java out-java/org/webrtc/voiceengine
mkdir -p out-java/org/webrtc/audio
cp -p ${ROOT}/sdk/android/api/org/webrtc/audio/*.java out-java/org/webrtc/audio




