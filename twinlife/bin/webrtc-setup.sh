#!/bin/sh
if test ! -d "${WEBRTC_SRC_DIR}"; then
   echo "Missing WEBRTC_SRC_DIR environment variable"
   exit 1
fi

if test ! -d "${WEBRTC_SRC_DIR}/rtc_base"; then
   echo "Missing 'rtc_base' directory in ${WEBRTC_SRC_DIR}"
   echo "WEBRTC_SRC_DIR environment variable should point to WebRTC source tree"
   exit 1
fi

if test ! -f twinlife/bin/webrtc-setup.sh; then
   echo "You should run the script from a Twinlife/webrtc.git cloned repository"
   echo 1
fi

rm -rf third_party tools base buildtools testing build
ln -s ${WEBRTC_SRC_DIR}/third_party
ln -s ${WEBRTC_SRC_DIR}/tools
ln -s ${WEBRTC_SRC_DIR}/base
ln -s ${WEBRTC_SRC_DIR}/buildtools
ln -s ${WEBRTC_SRC_DIR}/testing
ln -s ${WEBRTC_SRC_DIR}/build
