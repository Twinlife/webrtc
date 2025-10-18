#!/bin/sh
if test T$WEBRTC_DST_DIR = T; then
   echo "Missing WEBRTC_DST_DIR environment variable"
   exit 1;
fi
if ! test -d "$WEBRTC_DST_DIR"; then
   echo "WEBRTC_DST_DIR is not a directory"
   exit 1;
fi
if test T$WEBRTC_SRC_DIR = T; then
   echo "Missing WEBRTC_SRC_DIR environment variable"
   exit 1;
fi
if ! test -d "$WEBRTC_SRC_DIR"; then
   echo "WEBRTC_SRC_DIR is not a directory"
   exit 1;
fi

if test $# -lt 1; then
   echo "Usage: webrtc-update.sh name [commit-message]"
   echo "Valid names:"
   (cd $WEBRTC_DST_DIR/third_party && ls)
   exit 2
fi
NAME=$1

SRC_DIR="?"
DST_DIR="?"
case $NAME in
    build|base|testing|tools/grit|tools/clang|tools/android)
        SRC_DIR=$WEBRTC_SRC_DIR/$NAME
        DST_DIR=$WEBRTC_DST_DIR/$NAME
        ;;

    *)
        SRC_DIR=$WEBRTC_SRC_DIR/third_party/$NAME
        DST_DIR=$WEBRTC_DST_DIR/third_party/$NAME
        ;;
esac

if test ! -d $DST_DIR; then
   echo "Invalid $NAME: destination is missing"
   exit 1
fi

if test ! -d $SRC_DIR; then
   echo "Invalid $NAME: source is missing"
   exit 1
fi

cd $DST_DIR || exit 1
git checkout google/master
if test $? -ne 0; then
   echo "Switching to google/master branch in $NAME failed"
   exit 1
fi
rm -rf *
cp -r $SRC_DIR/* .
git add .
git status
if test $# -eq 2; then
   MESSAGE=$2
   git commit -m "$MESSAGE"
   git push
fi

