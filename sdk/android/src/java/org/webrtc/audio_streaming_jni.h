/*
 *  Copyright (c) 2015-2019 twinlife SA.
 *
 *  All Rights Reserved.
 *  
 *  Contributor: 
 *   Christian Jacquemot (Christian.Jacquemot@twinlife-systems.com)
 */

/*
 * Derived from : 
 *  audio_track_jni.h
 *  audio_record_jni.h
 *
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_MODULES_AUDIO_DEVICE_ANDROID_AUDIO_STREAMING_JNI_H_
#define WEBRTC_MODULES_AUDIO_DEVICE_ANDROID_AUDIO_STREAMING_JNI_H_

#include <memory>

#include <jni.h>

#include "api/sequence_checker.h"
#include "modules/audio_device/android/audio_common.h"
#include "modules/audio_device/android/audio_manager.h"
#include "modules/audio_device/include/audio_device_defines.h"
#include "modules/audio_device/audio_device_generic.h"
#include "modules/utility/include/helpers_android.h"
#include "modules/utility/include/jvm_android.h"

namespace webrtc {

class AudioStreamingJni {
 public:
  // Wraps the Java specific parts of the AudioStreamingJni into one helper class.
  class JavaAudioStreaming {
   public:
    JavaAudioStreaming(NativeRegistration* native_registration,
                   std::unique_ptr<GlobalRef> audio_track);
    ~JavaAudioStreaming();

    bool IsAudioStreamingModeEnabled();
    int InitRecording(int sample_rate, size_t channels);
    bool StartRecording();
    bool StopRecording();
    bool EnableBuiltInAEC(bool enable);
    bool EnableBuiltInNS(bool enable);

   private:
    std::unique_ptr<GlobalRef> audio_streaming_;
    jmethodID is_audio_streaming_mode_enabled_ ;    
    jmethodID init_recording_;
    jmethodID start_recording_;
    jmethodID stop_recording_;
  };

  explicit AudioStreamingJni(AudioManager* audio_manager);
  ~AudioStreamingJni();

  int32_t Init();
  int32_t Terminate();

  bool IsAudioStreamingModeEnabled();
    
  int32_t PlayoutIsAvailable(bool& available);  
  int32_t InitPlayout();
  bool PlayoutIsInitialized() const;
  int32_t InitRecording();
  bool RecordingIsInitialized() const { return initialized_; }

  int32_t StartPlayout();
  int32_t StopPlayout();
  bool Playing() const;
  int32_t StartRecording();
  int32_t StopRecording();
  bool Recording() const { return recording_; }

  int32_t SpeakerVolumeIsAvailable(bool& available);
  int32_t SetSpeakerVolume(uint32_t volume);
  int32_t SpeakerVolume(uint32_t& volume) const;
  int32_t MaxSpeakerVolume(uint32_t& maxVolume) const;
  int32_t MinSpeakerVolume(uint32_t& minVolume) const;
  int32_t SpeakerVolumeStepSize(uint16_t& stepSize) const;
  
  int32_t SpeakerMuteIsAvailable(bool& available);
  int32_t SetSpeakerMute(bool enable);
  int32_t SpeakerMute(bool& enabled) const;

  int32_t EnableBuiltInAEC(bool enable);
  int32_t EnableBuiltInAGC(bool enable);
  int32_t EnableBuiltInNS(bool enable);

  void AttachAudioBuffer(AudioDeviceBuffer* audioBuffer);

 private:
  // Called from Java side so we can cache the address of the Java-manged
  // |byte_buffer| in |direct_buffer_address_|. The size of the buffer
  // is also stored in |direct_buffer_capacity_in_bytes_|.
  // This method will be called by the WebRtcAudioStreaming constructor, i.e.,
  // on the same thread that this object is created on.
  static void JNICALL CacheDirectBufferAddress(
    JNIEnv* env, jobject obj, jobject byte_buffer, jlong native_audio_streaming);
  void OnCacheDirectBufferAddress(JNIEnv* env, jobject byte_buffer);

  // Called periodically by the Java based WebRtcAudioStreaming object when
  // recording has started. Each call indicates that there are |length| new
  // bytes recorded in the memory area |direct_buffer_address_| and it is
  // now time to send these to the consumer.
  // This method is called on a high-priority thread from Java. The name of
  // the thread is 'AudioStreamingThread'.
  static void JNICALL DataIsRecorded(
    JNIEnv* env, jobject obj, jint length, jlong native_audio_streaming);
  void OnDataIsRecorded(int length);

  // Stores thread ID in constructor.
  SequenceChecker thread_checker_;

  // Stores thread ID in first call to OnDataIsRecorded() from high-priority
  // thread in Java. Detached during construction of this object.
  SequenceChecker thread_checker_java_;

  // Calls AttachCurrentThread() if this thread is not attached at construction.
  // Also ensures that DetachCurrentThread() is called at destruction.
  JvmThreadConnector attach_thread_if_needed_;

  // Wraps the JNI interface pointer and methods associated with it.
  std::unique_ptr<JNIEnvironment> j_environment_;

  // Contains factory method for creating the Java object.
  std::unique_ptr<NativeRegistration> j_native_registration_;

  // Wraps the Java specific parts of the AudioStreamingJni class.
  std::unique_ptr<AudioStreamingJni::JavaAudioStreaming> j_audio_streaming_;

  // Raw pointer to the audio manger.
  const AudioManager* audio_manager_;

  // Contains audio parameters provided to this class at construction by the
  // AudioManager.
  const AudioParameters audio_parameters_;

  // Delay estimate of the total round-trip delay (input + output).
  // Fixed value set once in AttachAudioBuffer() and it can take one out of two
  // possible values. See audio_common.h for details.
  int total_delay_in_milliseconds_;

  // Cached copy of address to direct audio buffer owned by |j_audio_record_|.
  void* direct_buffer_address_;

  // Number of bytes in the direct audio buffer owned by |j_audio_record_|.
  size_t direct_buffer_capacity_in_bytes_;

  // Number audio frames per audio buffer. Each audio frame corresponds to
  // one sample of PCM mono data at 16 bits per sample. Hence, each audio
  // frame contains 2 bytes (given that the Java layer only supports mono).
  // Example: 480 for 48000 Hz or 441 for 44100 Hz.
  size_t frames_per_buffer_;

  bool initialized_;

  bool recording_;

  // Raw pointer handle provided to us in AttachAudioBuffer(). Owned by the
  // AudioDeviceModuleImpl class and called by AudioDeviceModule::Create().
  AudioDeviceBuffer* audio_device_buffer_;
};

}  // namespace webrtc

#endif  // WEBRTC_MODULES_AUDIO_DEVICE_ANDROID_AUDIO_STREAMING_JNI_H_
