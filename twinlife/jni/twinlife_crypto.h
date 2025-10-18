/*
 *  Copyright (c) 2023-2024 twinlife SA.
 *  SPDX-License-Identifier: AGPL-3.0-only
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#ifndef SDK_ANDROID_SRC_JNI_PC_TWINLIFE_CRYPTO_H_
#define SDK_ANDROID_SRC_JNI_PC_TWINLIFE_CRYPTO_H_

#include <jni.h>

#include "twinlife/twinlife_crypto.h"
#include "sdk/android/native_api/jni/scoped_java_ref.h"

using twinlife::CryptoKey;
using twinlife::CryptoBox;

namespace webrtc {
namespace jni {

class CryptoKey : public twinlife::CryptoKey {
public:
  CryptoKey(twinlife::CryptoKey::Kind kind, EVP_PKEY *pkey) : twinlife::CryptoKey(kind, pkey) {}
  ~CryptoKey() {}

  // Export the public key in DER base64 in the given buffer and return the length of exported public key.
  ScopedJavaLocalRef<jbyteArray> GetPublicKey(JNIEnv *env, jboolean useBase64);

  // Export the priviate key in DER in the given buffer (no base64 encoding)
  // and return the length of exported private key.
  ScopedJavaLocalRef<jbyteArray> GetPrivateKey(JNIEnv *env, jboolean useBase64);

  // Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
  // in the signature buffer.  Return the length of the signature or a negative error code.
  jint Sign(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature, jboolean useBase64);

    // Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
    // Returns 1 if the signature is verified, 0 if the data does not match or a negative error code.
  jint Verify(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature, jboolean useBase64);
 
  ScopedJavaLocalRef<jstring> SignAuth(JNIEnv *env, jlong peerPublicKey,
                                 const JavaParamRef<jstring>& item,
                                 const JavaParamRef<jstring>& peerItem);

  jint VerifyAuth(JNIEnv *env, jlong peerPublicKey,
                  const JavaParamRef<jstring>& item,
                  const JavaParamRef<jstring>& peerItem,
                  const JavaParamRef<jstring>& signature);

  jint DeriveKeyPBKDF2HMACSHA256(JNIEnv *env, jstring password, const JavaParamRef<jbyteArray> &salt,
                   const jint iterations, const JavaParamRef<jbyteArray> &outKey);

  void Dispose(JNIEnv *env);
};

class CryptoBox : public twinlife::CryptoBox {
public:
  CryptoBox(twinlife::CryptoBox::Kind kind) : twinlife::CryptoBox(kind) {}
  ~CryptoBox() {}

    // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
    // and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
    // to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
    // to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
    // and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
  jint Bind(JNIEnv *env, jboolean encrypt, jlong privateKey, jlong peerPublicKey, const JavaParamRef<jbyteArray>& salt);

  jint BindSecret(JNIEnv *env, const JavaParamRef<jbyteArray>& key);

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
  jint Unbind(JNIEnv *env);

    // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
    // Only the data buffer is encrypted.  The nonce buffer will be filled with a new nonce of 12 bytes.
    // Return the length of the output buffer or a negative error code.
  jint EncryptAEAD(JNIEnv *env, jlong nonceSequence, const JavaParamRef<jbyteArray>& data, jint dataLength,
                   const JavaParamRef<jbyteArray>& auth, const JavaParamRef<jbyteArray>& buffer);

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
  jint DecryptAEAD(JNIEnv *env, jlong nonceSequence, const JavaParamRef<jbyteArray>& data, const jint authLength,
                   const JavaParamRef<jbyteArray>& buffer);

  void Dispose(JNIEnv *env);
};
  
ScopedJavaLocalRef<jobject> NativeToJavaCryptoKey(
    JNIEnv* env,
    twinlife::CryptoKey *crypto);

ScopedJavaLocalRef<jobject> NativeToJavaCryptoBox(
    JNIEnv* env,
    twinlife::CryptoBox *crypto);

}  // namespace jni
}  // namespace webrtc

#endif  // SDK_ANDROID_SRC_JNI_PC_TWINLIFE_CRYPTO_H_
