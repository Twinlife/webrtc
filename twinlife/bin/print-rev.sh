#!/bin/sh

ROOT=`pwd`

echo -n "   webrtc-library:                        "
git rev-parse HEAD

cd $ROOT/build
echo -n "    build:                                "
git rev-parse HEAD

cd $ROOT/buildtools
echo -n "    buildtools:                           "
git rev-parse HEAD

cd $ROOT/testing
echo -n "    testing:                              "
git rev-parse HEAD

cd $ROOT/tools
echo -n "    tools:                                "
git rev-parse HEAD

cd $ROOT/third_party
echo -n "    third_party:                          "
git rev-parse HEAD

echo "    twinlife:                        "

cd $ROOT/twinlife/libwebsockets
echo -n "     libwebsockets:                       "
git rev-parse HEAD

cd $ROOT/twinlife/sqlcipher
echo -n "     sqlcipher:                           "
git rev-parse HEAD
