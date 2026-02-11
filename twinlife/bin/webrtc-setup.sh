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
ln -s ${WEBRTC_SRC_DIR}/tools
ln -s ${WEBRTC_SRC_DIR}/buildtools
ln -s ${WEBRTC_SRC_DIR}/testing
ln -s ${WEBRTC_SRC_DIR}/build

# Create symbolic link to each third_party directory
# For some of these third_party, we may do fixes and adaptations.
# For these limited cases, we copy the WebRTC official directory and override it
# with our own adaptations that we save in twinlife/<override> directory.
# It is not perfect but enough and easier to do it that way.
COPY_THIRD_PARTY_LIBS="jni_zero"

mkdir third_party && (cd third_party &&
  for i in ${WEBRTC_SRC_DIR}/third_party/*; do
     ln -s $i
  done
  for i in ${COPY_THIRD_PARTY_LIBS}; do
     rm -rf $i
     cp -r ${WEBRTC_SRC_DIR}/third_party/$i $i
  done        
)

# Apply our own override:
cp twinlife/jni_zero/jni_zero.cc third_party/jni_zero/

