#!/bin/bash

rm -rf out-64/Release
os=`uname`
if [[ $os == 'Linux' ]];
then
    gn=buildtools/linux64/gn
elif [[ $os == 'Darwin' ]];
then
    gn=buildtools/mac/gn
fi
$gn gen out-64/Release --args='target_os="android" target_cpu="arm64" is_official_build=true rtc_build_examples=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_tools=false use_errorprone_java_compiler=false'


