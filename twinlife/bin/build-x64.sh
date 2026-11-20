#!/bin/bash

rm -rf out-x64/Release
os=`uname`
if [[ $os == 'Linux' ]];
then
    gn=buildtools/linux64/gn
elif [[ $os == 'Darwin' ]];
then
    gn=buildtools/mac/gn
fi
$gn gen out-x64/Release --args='target_os="android" target_cpu="x64" target_environment="device" enable_libaom=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_examples=false rtc_include_internal_audio_device=false rtc_include_dav1d_in_internal_decoder_factory=false rtc_enable_google_benchmarks=false enable_rust=false rtc_rust=false rtc_build_tools=false is_official_build=true libyuv_disable_jpeg=true use_errorprone_java_compiler=false is_debug=false'
