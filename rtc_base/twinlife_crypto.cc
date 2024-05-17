/*
 *  Copyright (c) 2023-2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */
#include "twinlife_crypto.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/curve25519.h>
#include <openssl/bytestring.h>
#include <openssl/mem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define nullptr 0
#define NAMESPACE namespace twinlife {

NAMESPACE

int Crypto::digest(const unsigned char* data, int len, unsigned char digest[EVP_MAX_MD_SIZE])
{
  unsigned int digest_len = EVP_MAX_MD_SIZE;
  EVP_MD_CTX* ctx = EVP_MD_CTX_new();
  const EVP_MD* md = EVP_sha256();

  EVP_MD_CTX_init(ctx);
  EVP_DigestInit_ex(ctx, md, 0);
  EVP_DigestUpdate(ctx, data, len);
  EVP_DigestFinal(ctx, digest, &digest_len);
  EVP_MD_CTX_destroy(ctx);
  return digest_len;
}

EVP_PKEY* Crypto::create(enum Kind kind)
{
  EVP_PKEY *pkey;
  switch (kind) {
  case ECDSA: {
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ec_key) {
      return nullptr;
    }

    pkey = EVP_PKEY_new();
    if (!pkey || !EC_KEY_generate_key(ec_key) || !EVP_PKEY_assign_EC_KEY(pkey, ec_key)) {
      EVP_PKEY_free(pkey);
      EC_KEY_free(ec_key);
      return nullptr;
    }
    break;
  }

  case ED25519: {
    uint8_t pubKey[ED25519_PUBLIC_KEY_LEN];
    uint8_t privKey[ED25519_PRIVATE_KEY_LEN];

    ED25519_keypair(pubKey, privKey);
    pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, privKey, 32);
    break;
  }

  case X25519_AES_GCM:
  case X25519_CHACHA20_POLY1305: {
    uint8_t pubKey[X25519_PUBLIC_VALUE_LEN];
    uint8_t privKey[X25519_PRIVATE_KEY_LEN];

    X25519_keypair(pubKey, privKey);
    pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, privKey, 32);
    break;
  }

  default:
    return nullptr;
  }

  return pkey;
}

int Crypto::decodeBase64(unsigned char* key, size_t length, unsigned char buffer[TWINLIFE_MAX_SIZE]) {

  // Switch Base64URL to Base64
  for (size_t i = 0; i < length; i++) {
    if (key[i] == '_') {
      key[i] = '/';
    } else if (key[i] == '-') {
      key[i] = '+';
    }
  }

  size_t decodedLength;
  if (EVP_DecodeBase64(buffer, &decodedLength, TWINLIFE_MAX_SIZE, key, length) != 1) {
    return TWINLIFE_BAD_PARAM;
  }
  return decodedLength;
}

int Crypto::encodeBase64(const unsigned char* data, size_t length, unsigned char *buffer, size_t maxLength)
{
  // Verify we have enough space for BASE64 (+5 is for /3 rounding + 1 for NUL).
  if (4 * (length / 3) + 5 >= maxLength) {
    return TWINLIFE_TOO_SMALL;
  }

  int result = EVP_EncodeBlock(buffer, data, length);
  if (result <= 0) {
    return TWINLIFE_BAD_PARAM;
  }

  // Switch Base64URL to Base64
  for (int i = 0; i < result; i++) {
    if (buffer[i] == '/') {
      buffer[i] = '_';
    } else if (buffer[i] == '+') {
      buffer[i] = '-';
    }
  }

  return result;
}

EVP_PKEY* Crypto::importPublicKey(enum Format format, enum Kind kind,
                                  unsigned char* pubKey, size_t pubKeyLength)
{
  if (!pubKey || pubKeyLength <= 0 || pubKeyLength > TWINLIFE_MAX_SIZE) {
    return nullptr;
  }

  unsigned char buffer[TWINLIFE_MAX_SIZE];
  size_t length;
  const unsigned char* p;
  if (format == Format::BASE64) {
    length = Crypto::decodeBase64(pubKey, pubKeyLength, buffer);
    if (length <= 0) {
      return nullptr;
    }
    p = buffer;
  } else {
    p = pubKey;
    length = pubKeyLength;
  }

  EVP_PKEY *pkey;
  switch (kind) {
  case ECDSA: {
    CBS cbs;

    CBS_init(&cbs, p, length);
    pkey = EVP_parse_public_key(&cbs);
    if (CBS_len(&cbs) != 0) {
      EVP_PKEY_free(pkey);
      return nullptr;
    }
    break;
  }

  case ED25519:
    if (length != 32) {
      return nullptr;
    }
    pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, p, length);
    break;

  case X25519_AES_GCM:
  case X25519_CHACHA20_POLY1305:
    if (length != 32) {
      return nullptr;
    }
    pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, p, length);
    break;
    
  default:
    return nullptr;
  }

  return pkey;
}

EVP_PKEY* Crypto::importPrivateKey(enum Format format, enum Kind kind,
                                   unsigned char* privateKey, size_t privateKeyLength)
{
  if (!privateKey || privateKeyLength <= 0 || privateKeyLength > TWINLIFE_MAX_SIZE) {
    return nullptr;
  }

  unsigned char buffer[TWINLIFE_MAX_SIZE];
  size_t length;
  const unsigned char* p;
  if (format == Format::BASE64) {
    length = Crypto::decodeBase64(privateKey, privateKeyLength, buffer);
    if (length <= 0) {
      return nullptr;
    }
    p = buffer;
  } else {
    p = privateKey;
    length = privateKeyLength;
  }

  EVP_PKEY *pkey;
  switch (kind) {
  case ECDSA: {
    CBS cbs;

    CBS_init(&cbs, p, length);
    pkey = EVP_parse_private_key(&cbs);
    if (CBS_len(&cbs) != 0) {
      EVP_PKEY_free(pkey);
      return nullptr;
    }
    break;
  }

  case ED25519: {
    if (length != 32) {
      return nullptr;
    }
    pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, p, length);
    break;
  }

  case X25519_AES_GCM:
  case X25519_CHACHA20_POLY1305:
    if (length != 32) {
      return nullptr;
    }
    pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, p, length);
    break;
    
  default:
    return nullptr;
  }

  return pkey;
}

int Crypto::exportPublicKey(enum Format format, unsigned char* buffer, size_t maxLength)
{
  if (!buffer || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unsigned char tmp[TWINLIFE_MAX_SIZE];
  unsigned char* p;
  size_t size;
  if (format == Format::BASE64) {
    p = tmp;
    size = sizeof(tmp);
  } else {
    p = buffer;
    size = maxLength;
  }

  switch (EVP_PKEY_id(pkey_)) {
  case NID_ED25519:
  case NID_X25519: {
    if (EVP_PKEY_get_raw_public_key(pkey_, p, &size) != 1) {
      return TWINLIFE_BAD_SIGNATURE;
    }
    break;
  }

  case NID_X9_62_id_ecPublicKey: {
    CBB cbb;
    uint8_t *data;
    CBB_init_fixed(&cbb, p, size);
    if (EVP_marshal_public_key(&cbb, pkey_) != 1) {
      return TWINLIFE_TOO_SMALL;
    }
    CBB_finish(&cbb, &data, &size);
    break;
  }

  default:
    break;
  }
  if (format == Format::BINARY) {
    return size;
  }

  return encodeBase64(tmp, size, buffer, maxLength);
}

int Crypto::exportPrivateKey(enum Format format, unsigned char* buffer, size_t maxLength)
{
  CBB cbb;
  uint8_t *data;
  size_t size;

  if (!buffer || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  if (format == Format::BASE64) {
    CBB_init(&cbb, TWINLIFE_MAX_SIZE);
    if (EVP_marshal_private_key(&cbb, pkey_) != 1) {
      CBB_cleanup(&cbb);
      return TWINLIFE_TOO_SMALL;
    }
    CBB_finish(&cbb, &data, &size);

    int result = encodeBase64(data, size, buffer, maxLength);
    OPENSSL_free(data);
    return result;
  } else {
    CBB_init_fixed(&cbb, buffer, maxLength);
    if (EVP_marshal_private_key(&cbb, pkey_) != 1) {
      return TWINLIFE_TOO_SMALL;
    }
    CBB_finish(&cbb, &data, &size);
    return size;
  }
}

int Crypto::signECDSA(enum Format format, const unsigned char* data, size_t len,
                      unsigned char* signature, size_t maxLength)
{
  if (!data || !signature || len <= 0 || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unsigned char digest[EVP_MAX_MD_SIZE];
  memset(digest, 0, sizeof(digest));
  int digest_len = Crypto::digest(data, len, digest);
  if (digest_len <= 0) {
    return TWINLIFE_BAD_ALLOC;
  }

  const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey_);
  if (!ec_key) {
    return TWINLIFE_BAD_EC_KEY;
  }

  ECDSA_SIG *sig = ECDSA_do_sign(digest, digest_len, (EC_KEY *)ec_key);
  if (!sig) {
    return TWINLIFE_SIGN_ERROR;
  }

  const BIGNUM *r = ECDSA_SIG_get0_r(sig);
  const BIGNUM *s = ECDSA_SIG_get0_s(sig);
  if (!r || !s) {
    ECDSA_SIG_free(sig);
    return TWINLIFE_BAD_SIGNATURE;
  }

  int rlen = BN_num_bytes(r);
  int slen = BN_num_bytes(s);
  size_t sigLength = slen + rlen;
  if (rlen <= 0 || slen <= 0 || sigLength >= TWINLIFE_MAX_SIGLEN) {
    ECDSA_SIG_free(sig);
    return TWINLIFE_BAD_SIGNATURE;
  }

  // Need at least enough room to store the binary signature.
  if (sigLength > maxLength) {
    return TWINLIFE_TOO_SMALL;
  }

  unsigned char buf[TWINLIFE_MAX_SIGLEN];
  unsigned char* p = format == Format::BINARY ? signature : buf;
  rlen = BN_bn2bin(r, p);
  slen = BN_bn2bin(s, &p[rlen]);

  ECDSA_SIG_free(sig);
  if (format == Format::BINARY) {
    return sigLength;
  } else {
    return encodeBase64(buf, sigLength, signature, maxLength);
  }
}

int Crypto::sign(enum Format format, const unsigned char* data, size_t len,
                 unsigned char* signature, size_t maxLength)
{
  switch (kind_) {
  case ED25519:
    return signED25519(format, data, len, signature, maxLength);

  case ECDSA:
    return signECDSA(format, data, len, signature, maxLength);

  default:
    return TWINLIFE_BAD_EC_KEY;
  }
}

int Crypto::signED25519(enum Format format, const unsigned char* data, size_t len,
                        unsigned char* signature, size_t maxLength)
{
  if (!data || !signature || len <= 0 || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
  if (!mdctx) {
    return TWINLIFE_BAD_ALLOC;
  }
 
  /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
  unsigned char binarySignature[64];
  size_t sigLength = sizeof(binarySignature);
  if (EVP_DigestSignInit(mdctx, NULL, NULL, NULL, pkey_) == 1
      && EVP_DigestSign(mdctx, binarySignature, &sigLength, data, len) == 1) {
  } else {
    sigLength = 0;
  }
  EVP_MD_CTX_destroy(mdctx);
  if (sigLength <= 0) {
    return TWINLIFE_SIGN_ERROR;
  }
  if (format == Format::BINARY) {
    if (sigLength > maxLength) {
      return TWINLIFE_TOO_SMALL;
    }
    memcpy(signature, binarySignature, sigLength);
    return sigLength;

  } else {
    return encodeBase64(binarySignature, sigLength, signature, maxLength);
  }
}

int Crypto::verify(enum Format format, const unsigned char* data, size_t len,
                   const unsigned char* signature, size_t signatureLength)
{
  switch (kind_) {
  case ED25519:
    return verifyED25519(format, data, len, signature, signatureLength);

  case ECDSA:
    return verifyECDSA(format, data, len, signature, signatureLength);

  default:
    return TWINLIFE_BAD_EC_KEY;
  }
}

int Crypto::verifyECDSA(enum Format format, const unsigned char* data, size_t len,
                        const unsigned char* signature, size_t signatureLength)
{
  if (!data || !signature || len <= 0 || signatureLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }

  unsigned char digest[EVP_MAX_MD_SIZE];
  int digest_len = Crypto::digest(data, len, digest);
  if (digest_len <= 0) {
    return TWINLIFE_BAD_ALLOC;
  }

  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }
  const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey_);
  if (!ec_key) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unsigned char buffer[TWINLIFE_MAX_SIGLEN];
  size_t length;
  const unsigned char* p;
  if (format == Format::BASE64) {
    if (EVP_DecodeBase64(buffer, &length, sizeof(buffer), signature, signatureLength) != 1) {
      return TWINLIFE_BAD_SIGNATURE;
    }
    p = buffer;
  } else {
    p = signature;
    length = signatureLength;
  }

  int slen = length / 2;
  BIGNUM *r = BN_bin2bn(p, slen, 0);
  BIGNUM *s = BN_bin2bn(&p[slen], length - slen, 0);
  ECDSA_SIG *sig = ECDSA_SIG_new();

  ECDSA_SIG_set0(sig, r, s);
  int result = ECDSA_do_verify(digest, digest_len, sig, (EC_KEY *)ec_key);
  ECDSA_SIG_free(sig);

  return result;
}

int Crypto::verifyED25519(enum Format format, const unsigned char* data, size_t len,
                          const unsigned char* signature, size_t signatureLength)
{
  if (!data || !signature || len <= 0 || signatureLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }

  if (!pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unsigned char buffer[TWINLIFE_MAX_SIGLEN];
  size_t length;
  const unsigned char *p;
  if (format == Format::BASE64) {
    if (EVP_DecodeBase64(buffer, &length, sizeof(buffer), signature, signatureLength) != 1) {
      return TWINLIFE_BAD_SIGNATURE;
    }
    p = buffer;
  } else {
    p = signature;
    length = signatureLength;
  }

  EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
  if (!mdctx) {
    return TWINLIFE_BAD_ALLOC;
  }

  /* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
  int result = EVP_DigestVerifyInit(mdctx, NULL, NULL, NULL, pkey_) == 1
    && EVP_DigestVerify(mdctx, p, length, data, len) == 1;
  EVP_MD_CTX_destroy(mdctx);

  return result;
}

int Crypto::createSharedSecret(const Crypto* peerPublicKey, unsigned char* key, size_t keyLength)
{
  if (!peerPublicKey || !key || keyLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  if (!pkey_ || !peerPublicKey->pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  EVP_PKEY_CTX *keyCtx = EVP_PKEY_CTX_new(pkey_, nullptr);
  if (!keyCtx) {
    return TWINLIFE_BAD_ALLOC;
  }

  if (EVP_PKEY_derive_init(keyCtx) <= 0) {
    EVP_PKEY_CTX_free(keyCtx);
    return TWINLIFE_BAD_ALLOC;
  }

  if (EVP_PKEY_derive_set_peer(keyCtx, peerPublicKey->pkey_) <= 0) {
    EVP_PKEY_CTX_free(keyCtx);
    return TWINLIFE_BAD_PARAM;
  }

  if (EVP_PKEY_derive(keyCtx, key, &keyLength) <= 0) {
    EVP_PKEY_CTX_free(keyCtx);
    return TWINLIFE_BAD_PARAM;
  }
  EVP_PKEY_CTX_free(keyCtx);

  return Crypto::digest(key, keyLength, key);
}

int Crypto::bind(const Crypto *peerPublicKey, const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement) {

  if (!pkey_ || !peerPublicKey->pkey_) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unbind();
  unsigned char* key = (unsigned char*)OPENSSL_malloc(TWINLIFE_MAX_SECRET_SIZE);
  if (!key) {
    return TWINLIFE_BAD_ALLOC;
  }

  int keyLength = createSharedSecret(peerPublicKey, key, TWINLIFE_MAX_SECRET_SIZE);
  if (keyLength <= 0) {
    OPENSSL_free(key);
    return TWINLIFE_BAD_PARAM;
  }
  newNonce(nonce, maxIncrement);

  const EVP_AEAD *aead;
  switch (kind_) {
  case ECDSA:
  case X25519_AES_GCM:
    aead = EVP_aead_aes_256_gcm();
    break;

  case X25519_CHACHA20_POLY1305:
    aead = EVP_aead_chacha20_poly1305();
    break;

  case ED25519:
    return TWINLIFE_BAD_EC_KEY;
  }

  aead_ = EVP_AEAD_CTX_new(aead, key, keyLength, EVP_AEAD_DEFAULT_TAG_LENGTH);
  OPENSSL_free(key);
  return aead_ ? 1 : TWINLIFE_BAD_ALLOC;
}

void Crypto::unbind() {
  if (aead_) {
    EVP_AEAD_CTX_free(aead_);
    aead_ = nullptr;
  }
}

void Crypto::newNonce(const unsigned char nonce[TWINLIFE_NONCE_LENGTH], int maxIncrement)
{
  memcpy(nonce_, nonce, TWINLIFE_NONCE_LENGTH);

  // Limit maxIncrement to 128 max to force a call to newNonce().
  if (maxIncrement > 128) {
    maxIncrement = 128;
  }
  incrementMask_ = 0;
  for (int i = 0; i <= 7; i++) {
    incrementMask_ |= (1 << i);
    if (maxIncrement < (1 << i)) {
      break;
    }
  }
  maxIncrement_ = maxIncrement;
  nonceVal_ = 0;
}

int Crypto::encryptAEAD(const unsigned char* data, size_t len, const unsigned char* auth, size_t authLength,
                        unsigned char* buffer, size_t maxLength)
{
  if (!aead_) {
    return TWINLIFE_BAD_PARAM;
  }

  unsigned int v = std::atomic_fetch_add<unsigned int>(&nonceVal_, 1);
  if (v >= maxIncrement_ || (v & ~incrementMask_) != 0) {
    return TWINLIFE_NONCE_ERROR;
  }
  if (maxLength <= authLength + sizeof(nonce_)) {
    return TWINLIFE_TOO_SMALL;
  }
  memcpy(buffer, auth, authLength);
  memcpy(&buffer[authLength], nonce_, sizeof(nonce_));
  buffer[authLength + TWINLIFE_NONCE_LENGTH - 1] ^= (unsigned char) (v & incrementMask_);

  size_t outLength;
  int result = EVP_AEAD_CTX_seal(aead_, &buffer[authLength + sizeof(nonce_)], &outLength,
                                 maxLength - authLength - sizeof(nonce_),
                                 &buffer[authLength], sizeof(nonce_),
                                 data, len, auth, authLength);
  return result <= 0 ? TWINLIFE_AEAD_FAIL : authLength + sizeof(nonce_) + outLength;
}

int Crypto::decryptAEAD(const unsigned char* data, size_t len, size_t authLength,
                        unsigned char* buffer, size_t maxLength)
{
  if (!aead_) {
    return TWINLIFE_BAD_PARAM;
  }
  if (len <= authLength + sizeof(nonce_)) {
    return TWINLIFE_TOO_SMALL;
  }

  size_t outLength;
  int result = EVP_AEAD_CTX_open(aead_, buffer, &outLength, maxLength, &data[authLength], sizeof(nonce_),
                                 &data[authLength + sizeof(nonce_)], len - authLength - sizeof(nonce_), data, authLength);
  return result <= 0 ? TWINLIFE_AEAD_FAIL : outLength;
}

Crypto::~Crypto()
{
   EVP_PKEY_free(pkey_);
   if (aead_) {
     EVP_AEAD_CTX_free(aead_);
   }
}

}
