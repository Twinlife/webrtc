/*
 *  Copyright (c) 2023-2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */
#ifndef TWINLIFE_SIGN_H
#define TWINLIFE_SIGN_H

#include <openssl/evp.h>
#include <atomic>

#define TWINLIFE_MAX_SIGLEN         128 // 64-bytes in real
#define TWINLIFE_MAX_PUBKEY_LENGTH  128 // Max is 124-bytes for Base64 ECDSA
#define TWINLIFE_MAX_PRIVKEY_LENGTH 256 // Max is 184-bytes for Base64 ECDSA
#define TWINLIFE_MAX_SIZE           256 // Max size for buffers (enough for private key in BASE64)
#define TWINLIFE_MAX_SECRET_SIZE    32  // ECDH shared keys are 256-bits == 32-bytes
#define TWINLIFE_NONCE_LENGTH       12  // Length of the nonce

#define TWINLIFE_BAD_PARAM     (-1)
#define TWINLIFE_TOO_SMALL     (-2)
#define TWINLIFE_BAD_ALLOC     (-3)
#define TWINLIFE_BAD_EC_KEY    (-4)
#define TWINLIFE_SIGN_ERROR    (-5)
#define TWINLIFE_BAD_SIGNATURE (-6)
#define TWINLIFE_AEAD_FAIL     (-7)
#define TWINLIFE_NONCE_ERROR   (-8)

namespace twinlife {
  class Obfuscate {
    static unsigned char* obfuscate(bool base64);
    static unsigned char* deobfuscate(bool base64);
  };

  class Crypto {
  public:
    enum Format {
      BINARY,
      BASE64
    };
    enum Kind {
      ECDSA,
      ED25519,
      X25519_AES_GCM,
      X25519_CHACHA20_POLY1305
    };
    // Create a private/public keypair for:
    // - ECDSA (use prime256v1 EC).
    // - ED25519
    // - X25519
    template <typename T>
    static T* create(enum Kind kind) {
      EVP_PKEY *pkey = Crypto::create(kind);
      if (pkey) {
        return new T(kind, pkey);
      } else {
        return 0;
      }
    }

    // Create the instance by importing a {DER|RAW} binary or BASE64 public key.
    // Returns null if the format is invalid.
    template <typename T>
    static T* importPublicKey(enum Format format, enum Kind kind,
                              unsigned char* pubKey, size_t pubKeyLength) {
      EVP_PKEY *pkey = Crypto::importPublicKey(format, kind, pubKey, pubKeyLength);
      if (pkey) {
        return new T(kind, pkey);
      } else {
        return 0;
      }
    }

    // Create the instance by importing a {DER|RAW} binary or BASE64 private key.
    // Returns null if the format is invalid.
    template <typename T>
    static T* importPrivateKey(enum Format format, enum Kind kind,
                               unsigned char* privateKey, size_t privateKeyLength) {
      EVP_PKEY *pkey = Crypto::importPrivateKey(format, kind, privateKey, privateKeyLength);
      if (pkey) {
        return new T(kind, pkey);
      } else {
        return 0;
      }
    }

    // Export the public key in DER base64 in the given buffer and return the length of exported public key.
    int exportPublicKey(enum Format format, unsigned char* buffer, size_t maxLength);

    // Export the priviate key in DER in the given buffer (no base64 encoding)
    // and return the length of exported private key.
    int exportPrivateKey(enum Format format, unsigned char* buffer, size_t maxLength);

    // Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
    // in the signature buffer.  Return the length of the signature or a negative error code.
    int sign(enum Format format, const unsigned char* data, size_t len, unsigned char* signature, size_t maxLength);
    int signECDSA(enum Format format, const unsigned char* data, size_t len, unsigned char* signature, size_t maxLength);
    int signED25519(enum Format format, const unsigned char* data, size_t len, unsigned char* signature, size_t maxLength);

    // Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
    // Returns 1 if the signature is verified, 0 if the data does not match or a negative error code.
    int verify(enum Format format, const unsigned char* data, size_t len,
               const unsigned char* signature, size_t signatureLength);
    int verifyECDSA(enum Format format, const unsigned char* data, size_t len,
                    const unsigned char* signature, size_t signatureLength);
    int verifyED25519(enum Format format, const unsigned char* data, size_t len,
                      const unsigned char* signature, size_t signatureLength);

    // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
    // and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
    // to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
    // to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
    // and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
    // Returns 1 when the operation succeeds or an error code.
    int bind(const Crypto *peerPublicKey, const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement);

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
    void unbind();

    // Setup a new nonce for encryptAEAD().
    void newNonce(const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement);

    // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
    // Only the data buffer is encrypted.  The result buffer has the following format:
    // +-------------------------+----------------+----------------+
    // | auth data [auth_length] | 12-bytes nonce | encrypted data |
    // +-------------------------+----------------+----------------+    
    // Return the length of the output buffer or a negative error code.
    int encryptAEAD(const unsigned char* data, size_t len,
                    const unsigned char* auth, size_t auth_length,
                    unsigned char* buffer, size_t maxLength);

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
    // The data buffer is assumed to use the following format:
    // +-------------------------+----------------+----------------+
    // | auth data [auth_length] | 12-bytes nonce | encrypted data |
    // +-------------------------+----------------+----------------+    
    int decryptAEAD(const unsigned char* data, size_t len, size_t auth_length,
                    unsigned char* buffer, size_t maxLength);

    ~Crypto();

    // Forbid copy and assignment.
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;

  protected:
    // Creation allowed only from create(), importPublicKey() or importPrivateKey().
    Crypto(enum Kind kind, EVP_PKEY *pkey) : kind_(kind) {
      pkey_ = pkey;
      aead_ = nullptr;
      maxIncrement_ = 0;
      nonceVal_ = 0;
      incrementMask_ = 0;
    }

  private:
    static EVP_PKEY* create(enum Kind kind);
    static EVP_PKEY* importPublicKey(enum Format format, enum Kind kind,
                                     unsigned char* pubKey, size_t pubKeyLength);
    static EVP_PKEY* importPrivateKey(enum Format format, enum Kind kind,
                                      unsigned char* privateKey, size_t privateKeyLength);

    const Kind kind_;
    EVP_PKEY* pkey_;
    EVP_AEAD_CTX *aead_;
    unsigned char nonce_[TWINLIFE_NONCE_LENGTH];
    unsigned int maxIncrement_;
    std::atomic<unsigned int> nonceVal_;
    unsigned int incrementMask_;

    static int digest(const unsigned char* data, int len, unsigned char digest[EVP_MAX_MD_SIZE]);

    // Convert in place the Base64URL `key` into Base64 alphabet and decode the Base64 result in `buffer`.
    // Return the length of the decoded data.
    static int decodeBase64(unsigned char* key, size_t length, unsigned char buffer[TWINLIFE_MAX_SIZE]);

    // Encode the data in Base64 URL in the target buffer.
    // Return the length of the encoded data or a negative error code.
    static int encodeBase64(const unsigned char* data, size_t length, unsigned char *buffer, size_t maxLength);

    // Derive a shared secret based on the private key and peer's public key, compute the SHA256 digest of that
    // secret and return it in the `key` buffer.  The `key` buffer must be allocated by using OPENSSL_malloc()
    // for security constraints (key will be cleared when buffer is released).
    int createSharedSecret(const Crypto* peerPublicKey, unsigned char* key, size_t keyLength);
  };
}

#endif
