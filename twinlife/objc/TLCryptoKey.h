/*
 *  Copyright (c) 2024 twinlife SA.
 *  SPDX-License-Identifier: AGPL-3.0-only
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import <Foundation/Foundation.h>

#import "RTCMacros.h"

typedef enum {
  TLCryptoKindECDSA,
  TLCryptoKindED25519,
  TLCryptoKindX25519
} TLCryptoKind;

RTC_OBJC_EXPORT
@interface TLCryptoKey : NSObject

/// Import a public key of the specified type.  The public key is either encoded in binary or Base64url.
+ (nullable TLCryptoKey *)importPublicKey:(TLCryptoKind)kind pubKey:(nonnull NSData *)pubKey isBase64:(BOOL)isBase64;

/// Import a private/public keypair of the specified type.  The private key is either encoded in binary or Base64url.
+ (nullable TLCryptoKey *)importPrivateKey:(TLCryptoKind)kind privateKey:(nonnull NSData *)privateKey isBase64:(BOOL)isBase64;

/// Create and generate a new private/public keypair of the specified kind.
+ (nullable TLCryptoKey *)createWithKind:(TLCryptoKind)kind;

/// Helper function to extract from the signature the public key used.
/// Note: extraction is necessary because we have to retrieve our private key as
/// well as item and peerItem before calling verifyAuth().
+ (nullable NSData *)extractAuthPublicKeyWithSignature:(nonnull NSString *)signature;

/// Get the public key either in binary or encoded in base64url.
- (nullable NSData *)publicKey:(BOOL)isBase64;

/// Get the private key either in binary or encoded in base64url.
- (nullable NSData *)privateKey:(BOOL)isBase64;

/// Sign the content of the data buffer with the private key and encode the ECDSA signature in Base64
/// in the signature buffer.
- (nullable NSData *)signWithData:(nonnull NSData *)data isBase64:(BOOL)isBase64;

/// Verify with the public key that the data buffer corresponds to the signature.
- (int)verifyWithData:(nonnull NSData *)data signature:(nonnull NSData *)signature isBase64:(BOOL)isBase64;

/// Sign the two items to create an authenticate signature signed by our private key.
/// The output signature has the following format:
///   <sha256>.<pubKey>.<sign(<sha256>, privKey>)>
/// where the <sha256> is computed as follows:
///   SHA256(item) ^ SHA256(peerItem) ^ SHA256(pubKey-1) ^ SHA256(pubKey-2)
- (nullable NSString *)signAuthWithKey:(nonnull TLCryptoKey *)peerPublicKey item:(nonnull NSString *)item peerItem:(nonnull NSString *)peerItem;

/// Verify the authenticate signature signed by the peer private key.
- (int)verifyAuthWithKey:(nonnull TLCryptoKey *)peerPublicKey item:(nullable NSString *)item peerItem:(nullable NSString *)peerItem signature:(nonnull NSString *)signature;

@end
