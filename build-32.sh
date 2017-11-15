#!/bin/bash

rm -rf out-32/Release
buildtools/linux64/gn gen out-32/Release --args='target_os="android" target_cpu="arm" is_official_build=true rtc_enable_protobuf=false rtc_include_tests=false use_errorprone_java_compiler=false'


