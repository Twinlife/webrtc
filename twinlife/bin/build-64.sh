#!/bin/bash

rm -rf out-64/Release
if test -f buildtools/mac/gn; then
    TARGET=ios
    gn=buildtools/mac/gn
else
    TARGET=android
    gn=buildtools/linux64/gn
fi

ARGS='target_environment="device" enable_libaom=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_examples=false rtc_include_internal_audio_device=false rtc_include_dav1d_in_internal_decoder_factory=false rtc_enable_google_benchmarks=false '
case ${TARGET} in
    ios)
	ARGS="${ARGS} symbol_level=0 rtc_build_tools=false rtc_build_json=false chrome_pgo_phase=0 libyuv_include_tests=false"
	$gn gen out-64/Release --args="target_os=\"ios\" target_cpu=\"arm64\" ${ARGS} is_component_build=false is_debug=false ios_deployment_target=\"12.0\" ios_enable_code_signing=false"
	;;

    android)
	$gn gen out-64/Release --args="target_os=\"android\" target_cpu=\"arm64\" ${ARGS} is_official_build=true libyuv_disable_jpeg=true rtc_build_tools=false use_errorprone_java_compiler=false"
	;;

esac


