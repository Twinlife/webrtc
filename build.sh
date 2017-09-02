#!/bin/bash

. build/android/envsetup.sh

echo "Init WebRTC build for Android armv7..."
./build-32.sh

echo "Init WebRTC build for Android arm64..."
./build-64.sh
