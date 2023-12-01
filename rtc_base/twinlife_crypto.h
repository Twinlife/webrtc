/*
 *  Copyright (c) 2023 twinlife SA.
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

#define TWINLIFE_MAX_SIGLEN        128 // 64-bytes in real
#define TWINLIFE_MAX_PUBKEY_LENGTH 128 // 91-bytes in real
#define TWINLIFE_MAX_SIZE          256 // Max size for buffers (enough for pubkey in BASE64)
#define TWINLIFE_MAX_SECRET_SIZE   32  // ECDH shared keys are 256-bits == 32-bytes
#define TWINLIFE_NONCE_LENGTH      12  // Length of the nonce

#define TWINLIFE_BAD_PARAM     (-1)
#define TWINLIFE_TOO_SMALL     (-2)
#define TWINLIFE_BAD_ALLOC     (-3)
#define TWINLIFE_BAD_EC_KEY    (-4)
#define TWINLIFE_SIGN_ERROR    (-5)
#define TWINLIFE_BAD_SIGNATURE (-6)
#define TWINLIFE_AEAD_FAIL     (-7)
#define TWINLIFE_NONCE_ERROR   (-8)

namespace twinlife {
  class Crypto {
  public:
    // Create a private/public keypair for ECDSA (use prime256v1 EC).
    static EVP_PKEY* create();

    // Create the instance by importing a DER BASE64 public key.  Returns null if the format is invalid.
    static EVP_PKEY* importPublicKey(const unsigned char* pubKey, size_t pubKeyLength);

    // Create the instance by importing a DER binary private key.  Returns null if the format is invalid.
    static EVP_PKEY* importPrivateKey(const unsigned char* privateKey, size_t privateKeyLength);

    // Export the public key in DER base64 in the given buffer and return the length of exported public key.
    int exportPublicKey(unsigned char* buffer, size_t maxLength);

    // Export the priviate key in DER in the given buffer (no base64 encoding)
    // and return the length of exported private key.
    int exportPrivateKey(unsigned char* buffer, size_t maxLength);

    // Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
    // in the signature buffer.  Return the length of the signature or a negative error code.
    int signECDSA(const unsigned char* data, size_t len, unsigned char* signature, size_t maxLength);

    // Verify with the public key that the data buffer corresponds to the Base64 ECDSA signature.
    // Returns 1 if the signature is verified, 0 if the data does not match or a negative error code.
    int verifyECDSA(const unsigned char* data, size_t len, const unsigned char* signature, size_t signatureLength);

    // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
    // and peer's public key, compute the SHA256 digest of that secret, setup the AEAD internal context
    // to be ready to use `encryptAEAD` or `decryptAEAD`.  The `bind` is a costly operation compared
    // to encryption and decryption.  The encryption nonce is pre-initialized with the given buffer
    // and will be incremented before each encryptAEAD() a maximum of `maxIncrement` times.
    int bind(const Crypto *peerPublicKey, const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement);

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
    void unbind();

    // Setup a new nonce for encryptAEAD().
    void newNonce(const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement);

    // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
    // Only the data buffer is encrypted.  The nonce buffer will be filled with a new nonce of 12 bytes.
    // Return the length of the output buffer or a negative error code.
    int encryptAEAD(const unsigned char* data, size_t len,
                    const unsigned char* auth, size_t auth_length,
                    unsigned char* nonce, unsigned char* buffer, size_t maxLength);

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
    int decryptAEAD(const unsigned char* encryptedData, size_t len,
                    const unsigned char *auth, size_t auth_length, const unsigned char* nonce,
                    unsigned char* buffer, size_t maxLength);

    Crypto(EVP_PKEY *pkey) {
      pkey_ = pkey;
      aead_ = nullptr;
    }
    ~Crypto();

    // Forbid copy and assignment.
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;

  private:
    EVP_PKEY* pkey_;
    EVP_AEAD_CTX *aead_;
    unsigned char nonce_[TWINLIFE_NONCE_LENGTH];
    int maxIncrement_;
    std::atomic<int> nonceVal_;

    static int digest(const unsigned char* data, int len, unsigned char digest[EVP_MAX_MD_SIZE]);

    // Derive a shared secret based on the private key and peer's public key, compute the SHA256 digest of that
    // secret and return it in the `key` buffer.  The `key` buffer must be allocated by using OPENSSL_malloc()
    // for security constraints (key will be cleared when buffer is released).
    int createSharedSecret(const Crypto* peerPublicKey, unsigned char* key, size_t keyLength);
  };
}

#endif
