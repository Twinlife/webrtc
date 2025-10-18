/*
 *  Copyright (c) 2024 twinlife SA.
 *  SPDX-License-Identifier: AGPL-3.0-only
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "TLCryptoKey.h"

typedef enum {
  TLCryptoBoxKindAES_GCM,
  TLCryptoBoxKindChaCha20_Poly1305
} TLCryptoBoxKind;

#define TL_NONCE_LENGTH 12
#define TL_KEY_LENGTH   32

RTC_OBJC_EXPORT
@interface TLCryptoBox : NSObject

/// Create a crypto box to encrypt/decrypt with the given AEAD operation.
+ (nullable TLCryptoBox *)createWithKind:(TLCryptoBoxKind)kind;

/// Prepare for use of AEAD with the peer's public key.  Derive a shared secret based on the private key
/// and peer's public key, compute the HMAC(sharedSecret, {salt || pubKeyA || pubKeyB}) digest,
/// setup the AEAD internal context to be ready to use `encryptAEAD` or `decryptAEAD`.
/// The `bind` is a costly operation compared to encryption and decryption.
/// Returns 1 when the operation succeeds or an error code.
- (int)bindWithKey:(nonnull TLCryptoKey *)privateKey peerPublicKey:(nonnull TLCryptoKey *)publicKey encrypt:(BOOL)encrypt salt:(nonnull NSData *)salt;

/// Prepare for use of AEAD with the secret key given in key and with the given length.
/// Returns 1 when the operation succeeds or an error code.
- (int)bindWithKey:(nonnull NSData *)key;

/// Unbind with peer's public key and release the AEAD context.  This operation must be called when
/// encryption and decryption are not necessary any more.
- (void)unbind;

/// Encrypt and sign with AES256-GCM or ChaCha20-Poly1305 the data buffer and auth buffer with a new nonce sequence.
/// Only the data buffer is encrypted.  The result buffer has the following format:
/// +-------------------------+----------------+
/// | auth data [auth_length] | encrypted data |
/// +-------------------------+----------------+    
- (int)encryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data auth:(nonnull NSData *)auth output:(nonnull NSMutableData *)output;

/// Decrypt and verify the data with AES256-GCM or ChaCha20-Poly1305.  The data buffer is assumed
/// to use the following format:
/// +-------------------------+----------------+
/// | data [auth_length]      | data encrypted |
/// +-------------------------+----------------+    
- (int)decryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data authLength:(int)authLength output:(nonnull NSMutableData *)output;

@end
