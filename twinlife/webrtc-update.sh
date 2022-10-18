#!/bin/sh
WEBRTC_DST_DIR=/build/webrtc-library
WEBRTC_SRC_DIR=/ext/webrtc/src

if test $# -ne 1; then
   echo "Usage: webrtc-update.sh name"
   echo "Valid names:"
   (cd $WEBRTC_DST_DIR/third_party && ls)
   exit 2
fi
NAME=$1

if test ! -d $WEBRTC_DST_DIR/third_party/$NAME; then
   echo "Invalid $NAME: destination is missing"
   exit 1
fi

if test ! -d $WEBRTC_SRC_DIR/third_party/$NAME; then
   echo "Invalid $NAME: source is missing"
   exit 1
fi

cd $WEBRTC_DST_DIR/third_party/$NAME || exit 1
rm -rf *
cp -r $WEBRTC_SRC_DIR/third_party/$NAME/* .
git add .
git status

