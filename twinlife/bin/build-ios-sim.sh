#!/bin/bash
# Configure the WebRTC build for the iOS Simulator on Apple Silicon (arm64), in out-ios-sim/Release.
# Same arguments as the iOS device build in build-64.sh, except target_environment.

rm -rf out-ios-sim/Release

ARGS='target_environment="simulator" enable_libaom=false rtc_enable_protobuf=false rtc_include_tests=false rtc_build_examples=false rtc_include_internal_audio_device=false rtc_include_dav1d_in_internal_decoder_factory=false rtc_enable_google_benchmarks=false enable_rust=false rtc_rust=false'
ARGS="${ARGS} rtc_build_tools=false "
ARGS="${ARGS} symbol_level=0 rtc_build_json=false chrome_pgo_phase=0 libyuv_include_tests=false"

buildtools/mac/gn gen out-ios-sim/Release --args="target_os=\"ios\" target_cpu=\"arm64\" ${ARGS} is_component_build=false is_debug=false ios_deployment_target=\"12.0\" ios_enable_code_signing=false"
