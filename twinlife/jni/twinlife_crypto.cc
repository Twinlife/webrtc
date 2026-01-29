/*
 *  Copyright (c) 2023-2026 twinlife SA.
 *  SPDX-License-Identifier: AGPL-3.0-only
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#include "twinlife/jni/twinlife_crypto.h"

#include <memory>
#include <utility>

#include "sdk/android/generated_peerconnection_jni/CryptoKey_jni.h"
#include "sdk/android/generated_peerconnection_jni/CryptoBox_jni.h"
// #include "sdk/android/generated_peerconnection_jni/Config_jni.h"
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/pc/rtp_parameters.h"

namespace webrtc {
namespace jni {

  // Obfuscate or deobfuscate the data:
  // - obfuscate data + optional Base64 encoding
  // - optional Base64 decoding + deobfuscate data
  ScopedJavaLocalRef<jbyteArray> JNI_Config_Get(JNIEnv* env, jint mode,
						const JavaParamRef<jbyteArray>& data) {
     return nullptr;
  }

  // Export the public key in DER base64 in the given buffer and return the length of exported public key.
  ScopedJavaLocalRef<jbyteArray> CryptoKey::GetPublicKey(JNIEnv *env, jboolean useBase64) {
    unsigned char buffer[TWINLIFE_MAX_PUBKEY_LENGTH];

    CryptoKey::Format format = useBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;
    int length = exportPublicKey(format, buffer, sizeof(buffer));
    if (length <= 0) {
      return nullptr;
    }

    ScopedJavaLocalRef<jbyteArray> jarray =
      ScopedJavaLocalRef<jbyteArray>::Adopt(
          env, env->NewByteArray(length));
    env->SetByteArrayRegion(
      jarray.obj(), 0, length,
      reinterpret_cast<const jbyte*>(buffer));
    int8_t* array_ptr = env->GetByteArrayElements(jarray.obj(), /*isCopy=*/nullptr);
    //memcpy(array_ptr, buffer, length);
    env->ReleaseByteArrayElements(jarray.obj(), array_ptr, /*mode=*/0);
    return jarray;
  }

  // Export the private key in DER in the given buffer (no base64 encoding)
  // and return the length of exported private key.
  ScopedJavaLocalRef<jbyteArray> CryptoKey::GetPrivateKey(JNIEnv *env, jboolean useBase64) {
    unsigned char buffer[TWINLIFE_MAX_SIZE];

    CryptoKey::Format format = useBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;
    int length = exportPrivateKey(format, buffer, sizeof(buffer));
    if (length <= 0) {
      return nullptr;
    }

    ScopedJavaLocalRef<jbyteArray> jarray =
      ScopedJavaLocalRef<jbyteArray>::Adopt(
          env, env->NewByteArray(length));
    env->SetByteArrayRegion(
      jarray.obj(), 0, length,
      reinterpret_cast<const jbyte*>(buffer));
    int8_t* array_ptr = env->GetByteArrayElements(jarray.obj(), /*isCopy=*/nullptr);
    // memcpy(array_ptr, buffer, length);
    env->ReleaseByteArrayElements(jarray.obj(), array_ptr, /*mode=*/0);
    return jarray;
  }

    // Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
    // in the signature buffer.  Return the length of the signature or a negative error code.
  jint CryptoKey::Sign(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature, jboolean useBase64) {
    jbyte* buffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t length = env->GetArrayLength(data.obj());
    jbyte* signBuffer = env->GetByteArrayElements(signature.obj(), nullptr);
    size_t signLength = env->GetArrayLength(signature.obj());
    CryptoKey::Format format = useBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;

    int result = sign(format, (const unsigned char*) buffer, length, (unsigned char*) signBuffer, signLength);
    env->ReleaseByteArrayElements(data.obj(), buffer, JNI_ABORT);
    env->ReleaseByteArrayElements(signature.obj(), signBuffer, 0);
    return result;
  }

  // Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
  // Returns 1 if the signature is verified, 0 if the data does not match or a negative error code.
  jint CryptoKey::Verify(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature, jboolean useBase64) {
    jbyte* buffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t length = env->GetArrayLength(data.obj());
    jbyte* signBuffer = env->GetByteArrayElements(signature.obj(), nullptr);
    size_t signLength = env->GetArrayLength(signature.obj());
    CryptoKey::Format format = useBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;

    int result = verify(format, (const unsigned char*) buffer, length, (const unsigned char*) signBuffer, signLength);
    env->ReleaseByteArrayElements(data.obj(), buffer, JNI_ABORT);
    env->ReleaseByteArrayElements(signature.obj(), signBuffer, JNI_ABORT);
    return result;
  }

  ScopedJavaLocalRef<jstring> CryptoKey::SignAuth(JNIEnv *env, jlong peerPublicKey,
                                                  const JavaParamRef<jstring>& item,
                                                  const JavaParamRef<jstring>& peerItem) {
    std::string item1 = JavaToNativeString(env, item);
    std::string item2 = JavaToNativeString(env, peerItem);
    CryptoKey* pubKey = reinterpret_cast<CryptoKey*>(peerPublicKey);
    unsigned char buf[TWINLIFE_MAX_SIZE];

    int len = signAuth(pubKey, item1.data(), item2.data(), buf, sizeof(buf));
    if (len <= 0) {
      return NativeToJavaString(env, "");
    }

    return NativeToJavaString(env, (const char*)buf);
  }

  jint CryptoKey::VerifyAuth(JNIEnv *env, jlong peerPublicKey,
                             const JavaParamRef<jstring>& item,
                             const JavaParamRef<jstring>& peerItem,
                             const JavaParamRef<jstring>& signature) {
    std::string item1 = JavaToNativeString(env, item);
    std::string item2 = JavaToNativeString(env, peerItem);
    std::string sig = JavaToNativeString(env, signature);
    CryptoKey* pubKey = reinterpret_cast<CryptoKey*>(peerPublicKey);

    return (jint)verifyAuth(pubKey, item1.data(), item2.data(), sig.data());
  }

  jint CryptoKey::DeriveKeyPBKDF2HMACSHA256(JNIEnv *env, const JavaParamRef<jstring>& password, const JavaParamRef<jbyteArray> &salt,
                                            const jint iterations,  const JavaParamRef<jbyteArray> &outKey) {
    const char* p = env->GetStringUTFChars(password.obj(), nullptr);
    int password_len = env->GetStringUTFLength(password.obj());

    jbyte* s = env->GetByteArrayElements(salt.obj(), nullptr);
    int salt_len = env->GetArrayLength(salt.obj());

    jbyte* o = env->GetByteArrayElements(outKey.obj(), nullptr);
    size_t keyLen = env->GetArrayLength(outKey.obj());

    int result = deriveKeyPBKDF2HMACSHA256(p, password_len, (const unsigned char *)s, salt_len, (uint32_t)iterations, (int)keyLen, (unsigned char *)o);

    env->ReleaseStringUTFChars(password.obj(), p);
    env->ReleaseByteArrayElements(salt.obj(), s, JNI_ABORT);
    env->ReleaseByteArrayElements(outKey.obj(), o, 0);

    return result;
  }

  // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
  // and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
  // to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
  // to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
  // and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
  jint CryptoBox::Bind(JNIEnv *env, jboolean direction, jlong privateKey, jlong peerPublicKey, const JavaParamRef<jbyteArray>& salt) {
    CryptoKey* key = reinterpret_cast<CryptoKey*>(privateKey);
    CryptoKey* peer = reinterpret_cast<CryptoKey*>(peerPublicKey);
    jbyte* saltBuffer = env->GetByteArrayElements(salt.obj(), nullptr);
    size_t saltLength = env->GetArrayLength(salt.obj());

    // CHECK_NATIVE_PTR(env, jcaller, native, "Bind", 0);
    int result = bind(direction, key, peer, (const unsigned char*) saltBuffer, saltLength);
    env->ReleaseByteArrayElements(salt.obj(), saltBuffer, JNI_ABORT);
    return result;
  }

  jint CryptoBox::BindSecret(JNIEnv *env, const JavaParamRef<jbyteArray>& key) {
    jbyte* keyBuffer = env->GetByteArrayElements(key.obj(), nullptr);
    size_t keyLength = env->GetArrayLength(key.obj());

    int result = bind((const unsigned char *)keyBuffer, keyLength);
    env->ReleaseByteArrayElements(key.obj(), keyBuffer, JNI_ABORT);
    return result;    
  }

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
  jint CryptoBox::Unbind(JNIEnv *env) {
    unbind();
    return 0;
  }

  // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
  // Only the data buffer is encrypted.  The nonce buffer will be filled with a new nonce of 12 bytes.
  // Return the length of the output buffer or a negative error code.
  jint CryptoBox::EncryptAEAD(JNIEnv *env, jlong nonceSequence, const JavaParamRef<jbyteArray>& data,
                              jint dataLength, const JavaParamRef<jbyteArray>& auth,
                              const JavaParamRef<jbyteArray>& buffer) {
    jbyte* dataBuffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t dataBufferLength = env->GetArrayLength(data.obj());
    jbyte* authBuffer = env->GetByteArrayElements(auth.obj(), nullptr);
    size_t authLength = env->GetArrayLength(auth.obj());
    jbyte* resultBuffer = env->GetByteArrayElements(buffer.obj(), nullptr);
    size_t resultLength = env->GetArrayLength(buffer.obj());

    int result;
    if ((size_t) dataLength > dataBufferLength) {
      result = TWINLIFE_TOO_BIG;
    } else {
      result = encryptAEAD((const unsigned char*) dataBuffer, dataLength, (const unsigned char *)authBuffer, authLength,
                           nonceSequence, (unsigned char*) resultBuffer, resultLength);
    }
    env->ReleaseByteArrayElements(data.obj(), dataBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(auth.obj(), authBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(buffer.obj(), resultBuffer, 0);
    return result;
  }

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
  jint CryptoBox::DecryptAEAD(JNIEnv *env, jlong nonceSequence, const JavaParamRef<jbyteArray>& encryptedData,
                              const jint authLength, const JavaParamRef<jbyteArray>& buffer) {
    jbyte* dataBuffer = env->GetByteArrayElements(encryptedData.obj(), nullptr);
    size_t dataLength = env->GetArrayLength(encryptedData.obj());
    jbyte* resultBuffer = env->GetByteArrayElements(buffer.obj(), nullptr);
    size_t resultLength = env->GetArrayLength(buffer.obj());

    int result = decryptAEAD((const unsigned char*) dataBuffer, dataLength, authLength, nonceSequence,
                             (unsigned char*) resultBuffer, resultLength);      
    env->ReleaseByteArrayElements(encryptedData.obj(), dataBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(buffer.obj(), resultBuffer, 0);
    return result;
  }

  void CryptoKey::Dispose(JNIEnv *env) {
    delete this;
  }
  
  void CryptoBox::Dispose(JNIEnv *env) {
    delete this;
  }
  
  static ScopedJavaLocalRef<jobject> JNI_CryptoKey_Create(JNIEnv* env, jint kind) {

    CryptoKey *crypto = CryptoKey::create<CryptoKey>((CryptoKey::Kind)kind);
    return Java_CryptoKey_Constructor(env, NativeToJavaPointer(crypto));
  }

  static ScopedJavaLocalRef<jobject> JNI_CryptoBox_Create(JNIEnv* env, jint kind) {

    CryptoBox *crypto = CryptoBox::create<CryptoBox>((CryptoBox::Kind)kind);
    return Java_CryptoBox_Constructor(env, NativeToJavaPointer(crypto));
  }

static ScopedJavaLocalRef<jobject> JNI_CryptoKey_ImportPrivateKey(JNIEnv* env,
								  jint kind,
								  const JavaParamRef<jbyteArray>& privateKey,
								  jboolean isBase64) {
  jbyte* buffer = env->GetByteArrayElements(privateKey.obj(), nullptr);
  size_t length = env->GetArrayLength(privateKey.obj());

  CryptoKey::Format format = isBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;
  CryptoKey *crypto = twinlife::CryptoKey::importPrivateKey<CryptoKey>(format, (CryptoKey::Kind)kind, (unsigned char*) buffer, length);
  env->ReleaseByteArrayElements(privateKey.obj(), buffer, JNI_ABORT);  

  return Java_CryptoKey_Constructor(env, NativeToJavaPointer(crypto));
}

static ScopedJavaLocalRef<jobject> JNI_CryptoKey_ImportPublicKey(JNIEnv* env,
								 jint kind,
								 const JavaParamRef<jbyteArray>& publicKey,
								 jboolean isBase64) {
  jbyte* buffer = env->GetByteArrayElements(publicKey.obj(), nullptr);
  size_t length = env->GetArrayLength(publicKey.obj());

  CryptoKey::Format format = isBase64 ? CryptoKey::Format::BASE64 : CryptoKey::Format::BINARY;
  CryptoKey *crypto = twinlife::CryptoKey::importPublicKey<CryptoKey>(format, (CryptoKey::Kind)kind, (unsigned char*) buffer, length);
  env->ReleaseByteArrayElements(publicKey.obj(), buffer, JNI_ABORT);

  return Java_CryptoKey_Constructor(env, NativeToJavaPointer(crypto));
}

static ScopedJavaLocalRef<jbyteArray> JNI_CryptoKey_ExtractAuthPublicKey(JNIEnv* env,
                                                                         const JavaParamRef<jstring>& signature) {
  std::string sig = JavaToNativeString(env, signature);
  unsigned char pubKey[TWINLIFE_MAX_SIZE];

  int len = twinlife::CryptoKey::extractAuthPublicKey(sig.data(), pubKey, sizeof(pubKey));

  ScopedJavaLocalRef<jbyteArray> result =
      ScopedJavaLocalRef<jbyteArray>::Adopt(env, env->NewByteArray(len));
  env->SetByteArrayRegion(result.obj(), 0, len, (const jbyte *)pubKey);

  return result;
}

}  // namespace jni
}  // namespace webrtc
