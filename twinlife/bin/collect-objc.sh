#!/bin/sh
ROOT=`pwd`/
rm -rf out-objc
mkdir -p out-objc
cp -p ${ROOT}/sdk/objc/base/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/api/logging/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/api/peerconnection/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/api/video_codec/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/api/video_frame_buffer/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/api/video_frame_buffer/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/components/audio/RTC*.h out-objc
cp -p ${ROOT}/sdk/objc/components/capturer/RTC*.h out-objc
cp -p ${ROOT}/sdk/objc/components/renderer/metal/RTC*.h out-objc
cp -p ${ROOT}/sdk/objc/components/renderer/opengl/RTC*.h out-objc
cp -p ${ROOT}/sdk/objc/components/video_codec/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/components/video_frame_buffer/RTC*.h out-objc/
cp -p ${ROOT}/twinlife/objc/*.h out-objc
cp -p ${ROOT}/sdk/objc/helpers/RTC*.h out-objc/
cp -p ${ROOT}/sdk/objc/helpers/UIDevice*.h out-objc/

rm -rf out-objc/RTC*Private*
rm -rf out-objc/TL*Private*
rm -rf out-objc/RTC*Native*




