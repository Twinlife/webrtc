/*
 *  Copyright (c) 2023 twinlife SA.
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

EVP_PKEY* Crypto::create()
{
  EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (!ec_key) {
    return nullptr;
  }

  EVP_PKEY* pkey = EVP_PKEY_new();
  if (!pkey || !EC_KEY_generate_key(ec_key) || !EVP_PKEY_assign_EC_KEY(pkey, ec_key)) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(ec_key);
    return nullptr;
  }

  return pkey;
}

EVP_PKEY* Crypto::importPublicKey(const unsigned char* pubKey, size_t pubKeyLength)
{
  unsigned char buffer[TWINLIFE_MAX_PUBKEY_LENGTH];
  size_t length;

  if (!pubKey || pubKeyLength <= 0 || pubKeyLength > TWINLIFE_MAX_SIZE) {
    return nullptr;
  }
  if (EVP_DecodeBase64(buffer, &length, sizeof(buffer), pubKey, pubKeyLength) != 1) {
    return nullptr;
  }

  CBS cbs;
  CBS_init(&cbs, buffer, length);

  EVP_PKEY* pkey = EVP_parse_public_key(&cbs);
  if (!pkey) {
    return nullptr;
  }

  if (CBS_len(&cbs) != 0) {
    EVP_PKEY_free(pkey);
    return nullptr;
  }

  return pkey;
}

EVP_PKEY* Crypto::importPrivateKey(const unsigned char* privateKey, size_t privateKeyLength)
{
  if (!privateKey || privateKeyLength <= 0 || privateKeyLength > TWINLIFE_MAX_SIZE) {
    return nullptr;
  }

  CBS cbs;
  CBS_init(&cbs, privateKey, privateKeyLength);

  EVP_PKEY* pkey = EVP_parse_private_key(&cbs);
  if (!pkey) {
    return nullptr;
  }

  if (CBS_len(&cbs) != 0) {
    EVP_PKEY_free(pkey);
    return nullptr;
  }

  return pkey;
}

int Crypto::exportPublicKey(unsigned char* buffer, size_t maxLength)
{
  CBB cbb;
  uint8_t *data;
  size_t size;

  if (!buffer || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }

  CBB_init(&cbb, TWINLIFE_MAX_SIZE);
  if (EVP_marshal_public_key(&cbb, pkey_) != 1) {
    CBB_cleanup(&cbb);
    return TWINLIFE_TOO_SMALL;
  }
  CBB_finish(&cbb, &data, &size);

  // Verify we have enough space for BASE64 (+5 is for /3 rounding + 1 for NUL).
  if (4 * (size / 3) + 5 >= maxLength) {
    OPENSSL_free(data);
    return TWINLIFE_TOO_SMALL;
  }

  int result = EVP_EncodeBlock(buffer, data, size);
  OPENSSL_free(data);
  return result;
}

int Crypto::exportPrivateKey(unsigned char* buffer, size_t maxLength)
{
  CBB cbb;
  uint8_t *data;
  size_t size;

  if (!buffer || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }
  CBB_init_fixed(&cbb, buffer, maxLength);
  if (EVP_marshal_private_key(&cbb, pkey_) != 1) {
    return TWINLIFE_TOO_SMALL;
  }
  CBB_finish(&cbb, &data, &size);

  return size;
}

int Crypto::signECDSA(const unsigned char* data, size_t len, unsigned char* signature, size_t maxLength)
{
  if (!data || !signature || len <= 0 || maxLength <= 0) {
    return TWINLIFE_BAD_PARAM;
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
  int sigLength = slen + rlen;
  if (rlen <= 0 || slen <= 0 || sigLength >= TWINLIFE_MAX_SIGLEN) {
    ECDSA_SIG_free(sig);
    return TWINLIFE_BAD_SIGNATURE;
  }

  unsigned char buf[TWINLIFE_MAX_SIGLEN];
  rlen = BN_bn2bin(r, buf);
  slen = BN_bn2bin(s, &buf[rlen]);

  ECDSA_SIG_free(sig);

  // Verify we have enough space for BASE64 (+5 is for /3 rounding + 1 for NUL).
  if ((size_t) (4 * (sigLength / 3) + 5) >= maxLength) {
    return TWINLIFE_TOO_SMALL;
  }

  size_t result = EVP_EncodeBlock(signature, buf, sigLength);
  return result;
}

int Crypto::verifyECDSA(const unsigned char* data, size_t len, const unsigned char* signature, size_t signatureLength)
{
  if (!data || !signature || len <= 0 || signatureLength <= 0) {
    return TWINLIFE_BAD_PARAM;
  }

  unsigned char digest[EVP_MAX_MD_SIZE];
  int digest_len = Crypto::digest(data, len, digest);
  if (digest_len <= 0) {
    return TWINLIFE_BAD_ALLOC;
  }

  const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey_);
  if (!ec_key) {
    return TWINLIFE_BAD_EC_KEY;
  }

  unsigned char buffer[TWINLIFE_MAX_SIGLEN];
  size_t length;
  if (EVP_DecodeBase64(buffer, &length, sizeof(buffer), signature, signatureLength) != 1) {
    return TWINLIFE_BAD_SIGNATURE;
  }

  int slen = length / 2;
  BIGNUM *r = BN_bin2bn(buffer, slen, 0);
  BIGNUM *s = BN_bin2bn(&buffer[slen], length - slen, 0);
  ECDSA_SIG *sig = ECDSA_SIG_new();

  ECDSA_SIG_set0(sig, r, s);
  int result = ECDSA_do_verify(digest, digest_len, sig, (EC_KEY *)ec_key);
  ECDSA_SIG_free(sig);

  return result;
}

int Crypto::createSharedSecret(const Crypto* peerPublicKey, unsigned char* key, size_t keyLength)
{
  if (!peerPublicKey || !key || keyLength <= 0) {
    return TWINLIFE_BAD_PARAM;
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

  const EVP_AEAD *aead = EVP_aead_aes_256_gcm();

  aead_ = EVP_AEAD_CTX_new(aead, key, keyLength, 0);
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
  maxIncrement_ = maxIncrement;
}

int Crypto::encryptAEAD(const unsigned char* data, size_t len, const unsigned char* auth, size_t authLength,
                        unsigned char* nonce, unsigned char* buffer, size_t maxLength)
{
  if (!aead_) {
    return TWINLIFE_BAD_PARAM;
  }

  int v = std::atomic_fetch_add<int>(&nonceVal_, 1);
  if (v >= maxIncrement_) {
    return TWINLIFE_NONCE_ERROR;
  }
  memcpy(nonce, nonce_, sizeof(nonce_));
  nonce[TWINLIFE_NONCE_LENGTH - 1] = v;

  size_t outLength;
  int result = EVP_AEAD_CTX_seal(aead_, buffer, &outLength, maxLength, nonce, sizeof(nonce_), data, len, auth, authLength);
  return result <= 0 ? TWINLIFE_AEAD_FAIL : outLength;
}

int Crypto::decryptAEAD(const unsigned char* encryptedData, size_t len,
                        const unsigned char* auth, size_t authLength,
                        const unsigned char* nonce, unsigned char* buffer, size_t maxLength)
{
  if (!aead_) {
    return TWINLIFE_BAD_PARAM;
  }

  size_t outLength;
  int result = EVP_AEAD_CTX_open(aead_, buffer, &outLength, maxLength, nonce, sizeof(nonce_), encryptedData, len, auth, authLength);
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
