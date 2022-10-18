#!/bin/sh

ROOT=`pwd`

echo -n "   webrtc-library:                        "
git rev-parse HEAD

cd $ROOT/base
echo -n "    base:                                 "
git rev-parse HEAD

cd $ROOT/build
echo -n "    build:                                "
git rev-parse HEAD

cd $ROOT/buildtools
echo -n "    buildtools:                           "
git rev-parse HEAD

echo    "     third_party"

cd $ROOT/buildtools/third_party/libc++
echo -n "      libc++:                             "
git rev-parse HEAD

cd $ROOT/buildtools/third_party/libc++abi
echo -n "      libc++abi:                          "
git rev-parse HEAD

cd $ROOT/testing
echo -n "    testing:                              "
git rev-parse HEAD

echo    "    tools"
cd $ROOT/tools/android
echo -n "     android:                             "
git rev-parse HEAD

cd $ROOT/tools/clang
echo -n "     clang:                               "
git rev-parse HEAD

cd $ROOT/tools/grit
echo -n "     grit:                                "
git rev-parse HEAD

cd $ROOT/tools/ninja
echo -n "     ninja:                               "
git rev-parse HEAD

echo    "    third_party"

cd $ROOT/third_party/abseil-cpp
echo -n "     abseil-cpp:                          "
git rev-parse HEAD

cd $ROOT/third_party/android_deps
echo -n "     android_deps:                        "
git rev-parse HEAD

cd $ROOT/third_party/android_ndk
echo -n "     android_ndk:                         "
git rev-parse HEAD

cd $ROOT/third_party/android_sdk
echo -n "     android_sdk:                         "
git rev-parse HEAD

cd $ROOT/third_party/androidx
echo -n "     androidx:                            "
git rev-parse HEAD

cd $ROOT/third_party/boringssl
echo -n "     boringssl:                           "
git rev-parse HEAD

cd $ROOT/third_party/crc32c
echo -n "     crc32c:                              "
git rev-parse HEAD

cd $ROOT/third_party/expat
echo -n "     expat:                               "
git rev-parse HEAD

cd $ROOT/third_party/ijar
echo -n "     ijar:                                "
git rev-parse HEAD

cd $ROOT/third_party/jdk
echo -n "     jdk:                                 "
git rev-parse HEAD

cd $ROOT/third_party/jsoncpp
echo -n "     jsoncpp:                             "
git rev-parse HEAD

cd $ROOT/third_party/libaom
echo -n "     libaom:                              "
git rev-parse HEAD

cd $ROOT/third_party/libjpeg_turbo
echo -n "     libjpeg_turbo:                       "
git rev-parse HEAD

cd $ROOT/third_party/libsrtp
echo -n "     libsrtp:                             "
git rev-parse HEAD

cd $ROOT/third_party/libvpx
echo -n "     libvpx:                              "
git rev-parse HEAD

cd $ROOT/third_party/libwebsockets
echo -n "     libwebsockets:                       "
git rev-parse HEAD

cd $ROOT/third_party/libyuv
echo -n "     libyuv:                              "
git rev-parse HEAD

cd $ROOT/third_party/llvm-build
echo -n "     llvm-build:                          "
git rev-parse HEAD

cd $ROOT/third_party/modp_b64
echo -n "     modp_b64:                            "
git rev-parse HEAD

cd $ROOT/third_party/nasm
echo -n "     nasm:                                "
git rev-parse HEAD

cd $ROOT/third_party/opus
echo -n "     opus:                                "
git rev-parse HEAD

cd $ROOT/third_party/pffft
echo -n "     pffft:                               "
git rev-parse HEAD

cd $ROOT/third_party/rnnoise
echo -n "     rnnoise:                             "
git rev-parse HEAD

cd $ROOT/third_party/sqlcipher
echo -n "     sqlcipher:                           "
git rev-parse HEAD

cd $ROOT/third_party/zlib
echo -n "     zlib:                                "
git rev-parse HEAD
