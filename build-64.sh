#!/bin/bash

rm -rf out-64/Release
buildtools/linux64/gn gen out-64/Release --args='target_os="android" target_cpu="arm64" is_official_build=true rtc_enable_protobuf=false rtc_include_tests=false use_errorprone_java_compiler=false'


