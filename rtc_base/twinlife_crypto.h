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
#define TWINLIFE_TOO_BIG       (-9)

namespace twinlife {
  class Obfuscate {
    static unsigned char* obfuscate(bool base64);
    static unsigned char* deobfuscate(bool base64);
  };

  class CryptoKey {
  public:
    enum Format {
      BINARY = 0,
      BASE64 = 1
    };
    enum Kind {
      ECDSA = 0,
      ED25519 = 1,
      X25519 = 2
    };
    // Create a private/public keypair for:
    // - ECDSA (use prime256v1 EC).
    // - ED25519
    // - X25519
    template <typename T>
    static T* create(enum Kind kind) {
      EVP_PKEY *pkey = CryptoKey::create(kind);
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
                              const unsigned char* pubKey, size_t pubKeyLength) {
      EVP_PKEY *pkey = CryptoKey::importPublicKey(format, kind, pubKey, pubKeyLength);
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
                               const unsigned char* privateKey, size_t privateKeyLength) {
      EVP_PKEY *pkey = CryptoKey::importPrivateKey(format, kind, privateKey, privateKeyLength);
      if (pkey) {
        return new T(kind, pkey);
      } else {
        return 0;
      }
    }

    // Export the public key in binary or base64url in the given buffer and return the length of exported public key.
    int exportPublicKey(enum Format format, unsigned char* buffer, size_t maxLength) const;

    // Export the priviate key in binary or base64url in the given buffer
    // and return the length of exported private key.
    int exportPrivateKey(enum Format format, unsigned char* buffer, size_t maxLength) const;

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

    // Sign the two items to create an authenticate signature signed by our private key.
    // The output signature has the following format:
    //   <sha256>.<pubKey>.<sign(<sha256>, privKey>)>
    // where the <sha256> is computed as follows:
    //   SHA256(item) ^ SHA256(peerItem) ^ SHA256(pubKey-1) ^ SHA256(pubKey-2)
    int signAuth(const CryptoKey* peerPublicKey, const char* item, const char* peerItem,
                 unsigned char* signature, size_t maxLength);

    // Verify the auth signature produced by signAuth.
    // Return 1 if the signature is verified, 0 if there is a wrong signature or a negative error code.
    int verifyAuth(const CryptoKey* peerPublicKey, const char* item, const char* peerItem,
                   const char* signature);

    // Helper function to extract from the signature the public key used.
    // Note: extraction is necessary because we have to retrieve our private key as
    // well as item and peerItem before calling verifyAuth().
    static int extractAuthPublicKey(const char* signature, unsigned char* pubKey, size_t maxLength);

    ~CryptoKey();

    // Forbid copy and assignment.
    CryptoKey(const CryptoKey&) = delete;
    CryptoKey& operator=(const CryptoKey&) = delete;

  protected:
    // Creation allowed only from create(), importPublicKey() or importPrivateKey().
    CryptoKey(enum Kind kind, EVP_PKEY *pkey) : kind_(kind) {
      pkey_ = pkey;
    }

  private:
    friend class CryptoBox;

    int hashAuth(const CryptoKey* peerPublicKey, const char* item, const char* peerItem, unsigned char* sha256);

    static EVP_PKEY* create(enum Kind kind);
    static EVP_PKEY* importPublicKey(enum Format format, enum Kind kind,
                                     const unsigned char* pubKey, size_t pubKeyLength);
    static EVP_PKEY* importPrivateKey(enum Format format, enum Kind kind,
                                      const unsigned char* privateKey, size_t privateKeyLength);

    const Kind kind_;
    EVP_PKEY* pkey_;

    static int digest(const unsigned char* data, int len, unsigned char digest[EVP_MAX_MD_SIZE]);

    // Convert the Base64URL `data` into Base64 alphabet and decode the Base64 result in `buffer`.
    // Add necessary '=' that have been stripped.
    // Return the length of the decoded data.
    static int decodeBase64(const unsigned char* data, size_t length,
                            unsigned char* buffer, size_t maxLength);

    // Encode the data in Base64 URL in the target buffer.  The trailing '=' are removed.
    // Return the length of the encoded data or a negative error code.
    static int encodeBase64(const unsigned char* data, size_t length, unsigned char* buffer, size_t maxLength);

    static void xorBuffer(unsigned char* data, const unsigned char* src, size_t len);
  };

  class CryptoBox {
  public:
    enum Kind {
      AES_GCM = 0,
      CHACHA20_POLY1305 = 1
    };

    // Create the encrypt/decrypt box.
    template <typename T>
    static T* create(enum Kind kind) {
      return new T(kind);
    }

    // Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
    // and peer's public key, compute the HMAC(sharedSecret, {salt || pubKeyA || pubKeyB}) digest,
    // setup the AEAD internal context to be ready to use `encryptAEAD` or `decryptAEAD`.
    // The `bind` is a costly operation compared to encryption and decryption.
    // Returns 1 when the operation succeeds or an error code.
    int bind(bool direction, const CryptoKey *privateKey, const CryptoKey *peerPublicKey,
             const unsigned char *salt, size_t saltLength);

    // Prepare for use of AEAD with the secret key given in key and with the given length.
    // Returns 1 when the operation succeeds or an error code.
    int bind(const unsigned char *key, size_t keyLength);

    // Unbind with peer's public key and release the AEAD context.  This operation must be called when
    // encryption and decryption are not necessary any more.
    void unbind();

    // Encrypt and sign with AES256-GCM the data buffer and auth buffer with a new nonce.
    // Only the data buffer is encrypted.  The result buffer has the following format:
    // +-------------------------+----------------+
    // | auth data [auth_length] | encrypted data |
    // +-------------------------+----------------+    
    // Return the length of the output buffer or a negative error code.
    int encryptAEAD(const unsigned char* data, size_t len,
                    const unsigned char* auth, size_t auth_length,
                    uint64_t nonce, unsigned char* buffer, size_t maxLength);

    // Decrypt and verify the data with AES256-GCM.  Only the encryptedData buffer is decrypted.
    // The data buffer is assumed to use the following format:
    // +-------------------------+----------------+
    // | auth data [auth_length] | encrypted data |
    // +-------------------------+----------------+    
    int decryptAEAD(const unsigned char* data, size_t len, size_t auth_length,
                    uint64_t nonce, unsigned char* buffer, size_t maxLength);

    ~CryptoBox();

    // Forbid copy and assignment.
    CryptoBox(const CryptoBox&) = delete;
    CryptoBox& operator=(const CryptoBox&) = delete;

  protected:
    CryptoBox(enum Kind kind) : kind_(kind) {
      aead_ = 0;
    }

  private:
    const Kind kind_;
    EVP_AEAD_CTX *aead_;

    // Derive a shared secret based on the private key and peer's public key, compute the
    // HMAC(sharedSecret, {salt || pubKeyA || pubKeyB}) digest and return it in the `key` buffer.
    // The `key` buffer must be allocated by using OPENSSL_malloc()
    // for security constraints (key will be cleared when buffer is released).
    int createSharedSecret(bool direction, const CryptoKey *privateKey, const CryptoKey* peerPublicKey,
                           const unsigned char* salt, size_t saltLength,
                           unsigned char* key, size_t keyLength);

    static int HKDF(unsigned char* buffer, size_t sharedKeyLength,
                    const CryptoKey* firstKey, const CryptoKey* secondKey,
                    const unsigned char* salt, size_t saltLength,
                    unsigned char* key, size_t keyLength);
    static void makeNonce(unsigned char *nonce, size_t nonceLength, uint64_t nonceSequence);
  };
}

#endif
