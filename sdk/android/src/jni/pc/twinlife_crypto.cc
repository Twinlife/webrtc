/*
 *  Copyright (c) 2023-2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#include "sdk/android/src/jni/pc/twinlife_crypto.h"

#include <memory>
#include <utility>

#include "sdk/android/generated_peerconnection_jni/Crypto_jni.h"
#include "sdk/android/native_api/jni/java_types.h"
#include "sdk/android/src/jni/jni_helpers.h"
#include "sdk/android/src/jni/pc/rtp_parameters.h"

namespace webrtc {
namespace jni {

  // Export the public key in DER base64 in the given buffer and return the length of exported public key.
  ScopedJavaLocalRef<jbyteArray> Crypto::GetPublicKey(JNIEnv *env, jboolean useBase64) {
    unsigned char buffer[TWINLIFE_MAX_PUBKEY_LENGTH];

    Crypto::Format format = useBase64 ? Crypto::Format::BASE64 : Crypto::Format::BINARY;
    int length = exportPublicKey(format, buffer, sizeof(buffer));
    if (length <= 0) {
      return nullptr;
    }

    ScopedJavaLocalRef<jbyteArray> jarray(env, env->NewByteArray(length));
    int8_t* array_ptr = env->GetByteArrayElements(jarray.obj(), /*isCopy=*/nullptr);
    memcpy(array_ptr, buffer, length);
    env->ReleaseByteArrayElements(jarray.obj(), array_ptr, /*mode=*/0);
    return jarray;
  }

  // Export the private key in DER in the given buffer (no base64 encoding)
  // and return the length of exported private key.
  ScopedJavaLocalRef<jbyteArray> Crypto::GetPrivateKey(JNIEnv *env, jboolean useBase64) {
    unsigned char buffer[TWINLIFE_MAX_PUBKEY_LENGTH];

    Crypto::Format format = useBase64 ? Crypto::Format::BASE64 : Crypto::Format::BINARY;
    int length = exportPrivateKey(format, buffer, sizeof(buffer));
    if (length <= 0) {
      return nullptr;
    }

    ScopedJavaLocalRef<jbyteArray> jarray(env, env->NewByteArray(length));
    int8_t* array_ptr = env->GetByteArrayElements(jarray.obj(), /*isCopy=*/nullptr);
    memcpy(array_ptr, buffer, length);
    env->ReleaseByteArrayElements(jarray.obj(), array_ptr, /*mode=*/0);
    return jarray;
  }

    // Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
    // in the signature buffer.  Return the length of the signature or a negative error code.
  jint Crypto::SignECDSA(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature) {
    jbyte* buffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t length = env->GetArrayLength(data.obj());
    jbyte* signBuffer = env->GetByteArrayElements(signature.obj(), nullptr);
    size_t signLength = env->GetArrayLength(signature.obj());

    int result = signECDSA((const unsigned char*) buffer, length, (unsigned char*) signBuffer, signLength);
    env->ReleaseByteArrayElements(data.obj(), buffer, JNI_ABORT);
    env->ReleaseByteArrayElements(signature.obj(), signBuffer, 0);
    return result;
  }

  // Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
  // Returns 1 if the signature is verified, 0 if the data does not match or a negative error code.
  jint Crypto::VerifyECDSA(JNIEnv *env, const JavaParamRef<jbyteArray>& data, const JavaParamRef<jbyteArray>& signature) {
    jbyte* buffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t length = env->GetArrayLength(data.obj());
    jbyte* signBuffer = env->GetByteArrayElements(signature.obj(), nullptr);
    size_t signLength = env->GetArrayLength(signature.obj());

    int result = verifyECDSA((const unsigned char*) buffer, length, (const unsigned char*) signBuffer, signLength);
    env->ReleaseByteArrayElements(data.obj(), buffer, JNI_ABORT);
    env->ReleaseByteArrayElements(signature.obj(), signBuffer, JNI_ABORT);
    return result;
  }

  // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
  // and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
  // to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
  // to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
  // and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
  jint Crypto::Bind(JNIEnv *env, jlong peerPublicKey, const JavaParamRef<jbyteArray>& nonce, jint maxIncrement) {
    Crypto* peer = reinterpret_cast<Crypto*>(peerPublicKey);
    jbyte* nonceBuffer = env->GetByteArrayElements(nonce.obj(), nullptr);
    size_t nonceLength = env->GetArrayLength(nonce.obj());

    // CHECK_NATIVE_PTR(env, jcaller, native, "Bind", 0);
    int result;
    if (nonceLength != TWINLIFE_NONCE_LENGTH) {
      result = TWINLIFE_BAD_PARAM;
    } else {
      result = bind(peer, (const unsigned char*) nonceBuffer, maxIncrement);
    }
    env->ReleaseByteArrayElements(nonce.obj(), nonceBuffer, JNI_ABORT);
    return result;
  }

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
  jint Crypto::Unbind(JNIEnv *env) {
    unbind();
    return 0;
  }

  // Setup a new nonce for encryptAEAD().
  jint Crypto::NewNonce(JNIEnv *env, const JavaParamRef<jbyteArray>& nonce, jint maxIncrement) {
    jbyte* buffer = env->GetByteArrayElements(nonce.obj(), nullptr);
    size_t nonceLength = env->GetArrayLength(nonce.obj());

    int result;
    if (nonceLength != TWINLIFE_NONCE_LENGTH) {
      result = TWINLIFE_BAD_PARAM;
    } else {
      newNonce((const unsigned char*) buffer, maxIncrement);
      result = 0;
    }
    env->ReleaseByteArrayElements(nonce.obj(), buffer, JNI_ABORT);
    return result;
  }

  // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
  // Only the data buffer is encrypted.  The nonce buffer will be filled with a new nonce of 12 bytes.
  // Return the length of the output buffer or a negative error code.
  jint Crypto::EncryptAEAD(JNIEnv *env, const JavaParamRef<jbyteArray>& data,
                           const JavaParamRef<jbyteArray>& auth, const JavaParamRef<jbyteArray>& nonce,
                           const JavaParamRef<jbyteArray>& buffer) {
    jbyte* dataBuffer = env->GetByteArrayElements(data.obj(), nullptr);
    size_t dataLength = env->GetArrayLength(data.obj());
    jbyte* authBuffer = env->GetByteArrayElements(auth.obj(), nullptr);
    size_t authLength = env->GetArrayLength(auth.obj());
    jbyte* nonceBuffer = env->GetByteArrayElements(nonce.obj(), nullptr);
    size_t nonceLength = env->GetArrayLength(nonce.obj());
    jbyte* resultBuffer = env->GetByteArrayElements(buffer.obj(), nullptr);
    size_t resultLength = env->GetArrayLength(buffer.obj());

    int result;
    if (nonceLength != TWINLIFE_NONCE_LENGTH) {
      result = TWINLIFE_BAD_PARAM;
    } else {
      result = encryptAEAD((const unsigned char*) dataBuffer, dataLength, (const unsigned char *)authBuffer, authLength,
                           (unsigned char*) nonceBuffer, (unsigned char*) resultBuffer, resultLength);
    }
    env->ReleaseByteArrayElements(data.obj(), dataBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(auth.obj(), authBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(nonce.obj(), nonceBuffer, 0);
    env->ReleaseByteArrayElements(buffer.obj(), resultBuffer, 0);
    return result;
  }

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
  jint Crypto::DecryptAEAD(JNIEnv *env, const JavaParamRef<jbyteArray>& encryptedData,
                           const JavaParamRef<jbyteArray>& auth, const JavaParamRef<jbyteArray>& nonce,
                           const JavaParamRef<jbyteArray>& buffer) {
    jbyte* dataBuffer = env->GetByteArrayElements(encryptedData.obj(), nullptr);
    size_t dataLength = env->GetArrayLength(encryptedData.obj());
    jbyte* authBuffer = env->GetByteArrayElements(auth.obj(), nullptr);
    size_t authLength = env->GetArrayLength(auth.obj());
    jbyte* nonceBuffer = env->GetByteArrayElements(nonce.obj(), nullptr);
    size_t nonceLength = env->GetArrayLength(nonce.obj());
    jbyte* resultBuffer = env->GetByteArrayElements(buffer.obj(), nullptr);
    size_t resultLength = env->GetArrayLength(buffer.obj());

    int result;
    if (nonceLength != TWINLIFE_NONCE_LENGTH) {
      result = TWINLIFE_BAD_PARAM;
    } else {
      result = decryptAEAD((const unsigned char*) dataBuffer, dataLength, (const unsigned char *)authBuffer, authLength,
                           (const unsigned char*) nonceBuffer, (unsigned char*) resultBuffer, resultLength);      
    }
    env->ReleaseByteArrayElements(encryptedData.obj(), dataBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(auth.obj(), authBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(nonce.obj(), nonceBuffer, JNI_ABORT);
    env->ReleaseByteArrayElements(buffer.obj(), resultBuffer, 0);
    return result;
  }

  void Crypto::Dispose(JNIEnv *env) {
    delete this;
  }
  
static base::android::ScopedJavaLocalRef<jobject> JNI_Crypto_Create(JNIEnv* env) {

  EVP_PKEY *pkey = twinlife::Crypto::create();

  Crypto *crypto;
  if (pkey) {
    crypto = new Crypto(pkey);
  } else {
    crypto = nullptr;
  }
  return Java_Crypto_Constructor(env, NativeToJavaPointer(crypto));
}

static base::android::ScopedJavaLocalRef<jobject> JNI_Crypto_ImportPrivateKey(JNIEnv* env, const
                                                                              base::android::JavaParamRef<jbyteArray>& privateKey,
                                                                              jboolean isBase64) {
  jbyte* buffer = env->GetByteArrayElements(privateKey.obj(), nullptr);
  size_t length = env->GetArrayLength(privateKey.obj());

  Crypto::Format format = isBase64 ? Crypto::Format::BASE64 : Crypto::Format::BINARY;
  EVP_PKEY *pkey = twinlife::Crypto::importPrivateKey(format, (const unsigned char*) buffer, length);
  env->ReleaseByteArrayElements(privateKey.obj(), buffer, JNI_ABORT);  

  Crypto *crypto;
  if (pkey) {
    crypto = new Crypto(pkey);
  } else {
    crypto = nullptr;
  }
  return Java_Crypto_Constructor(env, NativeToJavaPointer(crypto));
}

static base::android::ScopedJavaLocalRef<jobject> JNI_Crypto_ImportPublicKey(JNIEnv* env, const
                                                                             base::android::JavaParamRef<jbyteArray>& privateKey,
                                                                             jboolean isBase64) {
  jbyte* buffer = env->GetByteArrayElements(privateKey.obj(), nullptr);
  size_t length = env->GetArrayLength(privateKey.obj());

  Crypto::Format format = isBase64 ? Crypto::Format::BASE64 : Crypto::Format::BINARY;
  EVP_PKEY *pkey = twinlife::Crypto::importPublicKey(format, (const unsigned char*) buffer, length);
  env->ReleaseByteArrayElements(privateKey.obj(), buffer, JNI_ABORT);

  Crypto *crypto;
  if (pkey) {
    crypto = new Crypto(pkey);
  } else {
    crypto = nullptr;
  }
  return Java_Crypto_Constructor(env, NativeToJavaPointer(crypto));
}
  
}  // namespace jni
}  // namespace webrtc
