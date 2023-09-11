/*
 *  Copyright (c) 2015-2019 twinlife SA.
 *
 *  All Rights Reserved.
 *  
 *  Contributor: 
 *   Christian Jacquemot (Christian.Jacquemot@twinlife-systems.com)
 */

/*
 * Derived from:
 *  audio_track_jni.cc
 *  audio_record_jni.cc
 *
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_device/android/audio_streaming_jni.h"

#include <utility>

#include <android/log.h>

#include "modules/audio_device/android/audio_common.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/checks.h"

#include "rtc_base/format_macros.h"

#define TAG "AudioStreamingJni"
#define ALOGV(...) // __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define ALOGD(...) // __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

namespace webrtc {

// AudioStreamingJni::JavaAudioStreaming implementation.
AudioStreamingJni::JavaAudioStreaming::JavaAudioStreaming(
    NativeRegistration* native_reg,
    std::unique_ptr<GlobalRef> audio_streaming)
    : audio_streaming_(std::move(audio_streaming)),
      is_audio_streaming_mode_enabled_(native_reg->GetMethodId("isAudioStreamingModeEnabled", "()Z")),
      init_recording_(native_reg->GetMethodId("initRecording", "(II)I")),
      start_recording_(native_reg->GetMethodId("startRecording", "()Z")),
      stop_recording_(native_reg->GetMethodId("stopRecording", "()Z")) {}

AudioStreamingJni::JavaAudioStreaming::~JavaAudioStreaming() {}

bool AudioStreamingJni::JavaAudioStreaming::IsAudioStreamingModeEnabled() {
  return audio_streaming_->CallBooleanMethod(is_audio_streaming_mode_enabled_);
}
  
int AudioStreamingJni::JavaAudioStreaming::InitRecording(
    int sample_rate, size_t channels) {
  return audio_streaming_->CallIntMethod(init_recording_,
                                      static_cast<jint>(sample_rate),
                                      static_cast<jint>(channels));
}

bool AudioStreamingJni::JavaAudioStreaming::StartRecording() {
  return audio_streaming_->CallBooleanMethod(start_recording_);
}

bool AudioStreamingJni::JavaAudioStreaming::StopRecording() {
  return audio_streaming_->CallBooleanMethod(stop_recording_);
}

bool AudioStreamingJni::JavaAudioStreaming::EnableBuiltInAEC(bool enable) {
  return false;
}

bool AudioStreamingJni::JavaAudioStreaming::EnableBuiltInNS(bool enable) {
  return false;
}

// AudioStreamingJni implementation.
AudioStreamingJni::AudioStreamingJni(AudioManager* audio_manager)
    : j_environment_(JVM::GetInstance()->environment()),
      audio_manager_(audio_manager),
      audio_parameters_(audio_manager->GetRecordAudioParameters()),
      total_delay_in_milliseconds_(0),
      direct_buffer_address_(nullptr),
      direct_buffer_capacity_in_bytes_(0),
      frames_per_buffer_(0),
      initialized_(false),
      recording_(false),
      audio_device_buffer_(nullptr) {
  ALOGD("ctor%s", GetThreadInfo().c_str());
  RTC_DCHECK(audio_parameters_.is_valid());
  RTC_CHECK(j_environment_);
  JNINativeMethod native_methods[] = {
      {"nativeCacheDirectBufferAddress", "(Ljava/nio/ByteBuffer;J)V",
      reinterpret_cast<void*>(
          &webrtc::AudioStreamingJni::CacheDirectBufferAddress)},
      {"nativeDataIsRecorded", "(IJ)V",
      reinterpret_cast<void*>(&webrtc::AudioStreamingJni::DataIsRecorded)}};
  j_native_registration_ = j_environment_->RegisterNatives(
      "org/webrtc/voiceengine/WebRtcAudioStreaming", native_methods,
      arraysize(native_methods));
  j_audio_streaming_.reset(new JavaAudioStreaming(
      j_native_registration_.get(),
      j_native_registration_->NewObject(
          "<init>", "(J)V", PointerTojlong(this))));
  // Detach from this thread since we want to use the checker to verify calls
  // from the Java based audio thread.
  thread_checker_java_.Detach();
}

AudioStreamingJni::~AudioStreamingJni() {
  ALOGD("~dtor%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  Terminate();
}

int32_t AudioStreamingJni::Init() {
  ALOGD("Init%s", GetThreadInfo().c_str()); 
  RTC_DCHECK(thread_checker_.IsCurrent());
  return 0;
}

int32_t AudioStreamingJni::Terminate() {
  ALOGD("Terminate%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  StopRecording();
  return 0;
}

bool AudioStreamingJni::IsAudioStreamingModeEnabled() {
  ALOGD("IsAudioStreamingModeEnabled%s", GetThreadInfo().c_str());
  return j_audio_streaming_->IsAudioStreamingModeEnabled();
}

int32_t AudioStreamingJni::InitRecording() {
  ALOGD("InitRecording%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  RTC_DCHECK(!initialized_);
  RTC_DCHECK(!recording_);
  int frames_per_buffer = j_audio_streaming_->InitRecording(
      audio_parameters_.sample_rate(), audio_parameters_.channels());
  if (frames_per_buffer < 0) {
    ALOGE("InitRecording failed!");
    return -1;
  }
  frames_per_buffer_ = static_cast<size_t>(frames_per_buffer);
  ALOGD("frames_per_buffer: %" PRIuS, frames_per_buffer_);
  const size_t bytes_per_frame = audio_parameters_.channels() * sizeof(int16_t);
  RTC_CHECK_EQ(direct_buffer_capacity_in_bytes_,
               frames_per_buffer_ * bytes_per_frame);
  RTC_CHECK_EQ(frames_per_buffer_, audio_parameters_.frames_per_10ms_buffer());
  initialized_ = true;
  return 0;
}

int32_t AudioStreamingJni::StartRecording() {
  ALOGD("StartRecording%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  RTC_DCHECK(initialized_);
  RTC_DCHECK(!recording_);
  if (!j_audio_streaming_->StartRecording()) {
    ALOGE("StartRecording failed!");
    return -1;
  }
  recording_ = true;
  return 0;
}

int32_t AudioStreamingJni::StopRecording() {
  ALOGD("StopRecording%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  if (!initialized_ || !recording_) {
    return 0;
  }
  if (!j_audio_streaming_->StopRecording()) {
    ALOGE("StopRecording failed!");
    return -1;
  }
  // If we don't detach here, we will hit a RTC_DCHECK in OnDataIsRecorded()
  // next time StartRecording() is called since it will create a new Java
  // thread.
  thread_checker_java_.Detach();
  initialized_ = false;
  recording_ = false;
  direct_buffer_address_= nullptr;
  return 0;
}

void AudioStreamingJni::AttachAudioBuffer(AudioDeviceBuffer* audioBuffer) {
  ALOGD("AttachAudioBuffer");
  RTC_DCHECK(thread_checker_.IsCurrent());
  audio_device_buffer_ = audioBuffer;
}

int32_t AudioStreamingJni::EnableBuiltInAEC(bool enable) {
  ALOGD("EnableBuiltInAEC%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  return -1;
}

int32_t AudioStreamingJni::EnableBuiltInAGC(bool enable) {
  // TODO(henrika): possibly remove when no longer used by any client.
  RTC_CHECK_NOTREACHED();  
}

int32_t AudioStreamingJni::EnableBuiltInNS(bool enable) {
  ALOGD("EnableBuiltInNS%s", GetThreadInfo().c_str());
  RTC_DCHECK(thread_checker_.IsCurrent());
  return -1;
}

void JNICALL AudioStreamingJni::CacheDirectBufferAddress(
    JNIEnv* env, jobject obj, jobject byte_buffer, jlong native_audio_streaming) {
  webrtc::AudioStreamingJni* this_object =
      reinterpret_cast<webrtc::AudioStreamingJni*> (native_audio_streaming);
  this_object->OnCacheDirectBufferAddress(env, byte_buffer);
}

void AudioStreamingJni::OnCacheDirectBufferAddress(
    JNIEnv* env, jobject byte_buffer) {
  ALOGD("OnCacheDirectBufferAddress");
  RTC_DCHECK(thread_checker_.IsCurrent());
  RTC_DCHECK(!direct_buffer_address_);
  direct_buffer_address_ =
      env->GetDirectBufferAddress(byte_buffer);
  jlong capacity = env->GetDirectBufferCapacity(byte_buffer);
  ALOGD("direct buffer capacity: %lld", capacity);
  direct_buffer_capacity_in_bytes_ = static_cast<size_t>(capacity);
}

void JNICALL AudioStreamingJni::DataIsRecorded(
  JNIEnv* env, jobject obj, jint length, jlong native_audio_streaming) {
  webrtc::AudioStreamingJni* this_object =
      reinterpret_cast<webrtc::AudioStreamingJni*> (native_audio_streaming);
  this_object->OnDataIsRecorded(length);
}

// This method is called on a high-priority thread from Java. The name of
// the thread is 'AudioStreamingThread'.
void AudioStreamingJni::OnDataIsRecorded(int length) {
  RTC_DCHECK(thread_checker_java_.IsCurrent());
  if (!audio_device_buffer_) {
    ALOGE("AttachAudioBuffer has not been called!");
    return;
  }
  audio_device_buffer_->SetRecordedBuffer(direct_buffer_address_,
                                          frames_per_buffer_);
  // We provide one (combined) fixed delay estimate for the APM and use the
  // |playDelayMs| parameter only. Components like the AEC only sees the sum
  // of |playDelayMs| and |recDelayMs|, hence the distributions does not matter.
  audio_device_buffer_->SetVQEData(total_delay_in_milliseconds_,
                                   0);  // clockDrift
  if (audio_device_buffer_->DeliverRecordedData() == -1) {
    ALOGE("AudioDeviceBuffer::DeliverRecordedData failed!");
  }
}

int32_t AudioStreamingJni::PlayoutIsAvailable(bool& available) {
  return -1;
}
  
int32_t AudioStreamingJni::InitPlayout() {
  return -1;
}

bool AudioStreamingJni::PlayoutIsInitialized() const {
  return false;
}
  
int32_t AudioStreamingJni::StartPlayout() {
  return -1;
}

int32_t AudioStreamingJni::StopPlayout() {
  return -1;
}
  
bool AudioStreamingJni::Playing() const {
  return false;
}

int32_t AudioStreamingJni::SpeakerVolumeIsAvailable(bool& available) {
  return -1;
}
  
int32_t AudioStreamingJni::SetSpeakerVolume(uint32_t volume) {
  return -1;
}

int32_t AudioStreamingJni::SpeakerVolume(uint32_t& volume) const {
  return -1;
}

int32_t AudioStreamingJni::MaxSpeakerVolume(uint32_t& maxVolume) const {
  return -1;
}

int32_t AudioStreamingJni::MinSpeakerVolume(uint32_t& minVolume) const {
  return -1;
}
}  // namespace webrtc
