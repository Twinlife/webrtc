#!/bin/bash

if test -f buildtools/mac/gn; then
    TARGET=ios
    gn=buildtools/mac/gn
else
    TARGET=android
    gn=buildtools/linux64/gn
fi
case $# in
    0)
        ;;
    1)
        TARGET=$1
        ;;
    *)
	echo "Usage: build-64.sh [ios|android|linux]" 1>&2
	exit 1
	;;
esac
rm -rf out-64/Release

ARGS='target_environment="device" enable_libaom=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_examples=false rtc_include_internal_audio_device=false rtc_include_dav1d_in_internal_decoder_factory=false rtc_enable_google_benchmarks=false enable_rust=false'
ARGS="${ARGS} rtc_build_tools=false "
case ${TARGET} in
    ios)
	ARGS="${ARGS} symbol_level=0 rtc_build_json=false chrome_pgo_phase=0 libyuv_include_tests=false"
	$gn gen out-64/Release --args="target_os=\"ios\" target_cpu=\"arm64\" ${ARGS} is_component_build=false is_debug=false ios_deployment_target=\"12.0\" ios_enable_code_signing=false"
	;;

    android)
	$gn gen out-64/Release --args="target_os=\"android\" target_cpu=\"arm64\" ${ARGS} is_official_build=true libyuv_disable_jpeg=true use_errorprone_java_compiler=false is_debug=false"
	;;

    linux)
        $gn gen out-64/Release --args="target_os=\"linux\" target_cpu=\"x64\" ${ARGS} rtc_has_java=true is_official_build=true rtc_include_builtin_audio_codecs=false rtc_use_pipewire=false optimize_for_size=true java_home=\"$JAVA_HOME\""
        ;;

esac


