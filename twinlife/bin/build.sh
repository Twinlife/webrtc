#!/bin/bash

. build/android/envsetup.sh

echo "Init WebRTC build for Android armv7..."
./twinlife/bin/build-32.sh

echo "Init WebRTC build for Android arm64..."
./twinlife/bin/build-64.sh
