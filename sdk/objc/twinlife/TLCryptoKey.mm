/*
 *  Copyright (c) 2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "TLCryptoKey.h"
#import "TLCryptoKey+Private.h"

#include "twinlife_crypto.h"
#import "base/RTCLogging.h"
#import "helpers/NSString+StdString.h"

RTC_OBJC_EXPORT
@implementation TLCryptoKey

+ (twinlife::CryptoKey::Kind)toCryptoKind:(TLCryptoKind)kind {

    switch (kind) {
        case TLCryptoKindECDSA:
            return twinlife::CryptoKey::Kind::ECDSA;

        case TLCryptoKindED25519:
            return twinlife::CryptoKey::Kind::ED25519;

        case TLCryptoKindX25519:
            return twinlife::CryptoKey::Kind::X25519;

        default:
            return twinlife::CryptoKey::Kind::ECDSA;
    }
}

- (nonnull instancetype)initWithCryptoKey:(twinlife::CryptoKey *)cryptoKey {

    self = [super init];
    if (self) {
        _cryptoKey = cryptoKey;
    }
    return self;
}

+ (nullable TLCryptoKey *)importPublicKey:(TLCryptoKind)kind pubKey:(nonnull NSData *)pubKey isBase64:(BOOL)isBase64 {

   twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
   twinlife::CryptoKey::Kind cKind = [TLCryptoKey toCryptoKind:kind];
   twinlife::CryptoKey* cryptoKey = twinlife::CryptoKey::importPublicKey<twinlife::CryptoKey>(cKind, format, [pubKey bytes], [pubKey length]);
   if (!cryptoKey) {
      RTC_LOG(LS_ERROR) << "invalid public key";
      return nil;
   }
   return [[TLCryptoKey alloc] initWithCryptoKey:cryptoKey];
}

+ (nullable TLCryptoKey *)importPrivateKey:(TLCryptoKind)kind privateKey:(nonnull NSData *)privateKey isBase64:(BOOL)isBase64 {

   twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
   twinlife::CryptoKey::Kind cKind = [TLCryptoKey toCryptoKind:kind];
   twinlife::CryptoKey* cryptoKey = twinlife::CryptoKey::importPrivateKey<twinlife::CryptoKey>(cKind, format, [privateKey bytes], [privateKey length]);
   if (!cryptoKey) {
      RTC_LOG(LS_ERROR) << "invalid private key";
      return nil;
   }
   return [[TLCryptoKey alloc] initWithCryptoKey:cryptoKey];
}

+ (nullable TLCryptoKey *)createWithKind:(TLCryptoKind)kind {

   twinlife::CryptoKey::Kind cKind = [TLCryptoKey toCryptoKind:kind];
   twinlife::CryptoKey* cryptoKey = twinlife::CryptoKey::create<twinlife::CryptoKey>(cKind);
   if (!cryptoKey) {
      RTC_LOG(LS_ERROR) << "cannot create private key";
      return nil;
   }
   return [[TLCryptoKey alloc] initWithCryptoKey:cryptoKey];
}

+ (nullable NSData *)extractAuthPublicKeyWithSignature:(nonnull NSString *)signature {

    std::string signatureStr = [signature stdString];
    unsigned char buf[TWINLIFE_MAX_SIZE];
    int len = twinlife::CryptoKey::extractAuthPublicKey(signatureStr.data(), buf, sizeof(buf));
    if (len <= 0) {
        return nil;
    }
    return [NSData initWithBytes:buf length:len];
}

- (nullable NSData *)publicKey:(BOOL)isBase64 {

    unsigned char buf[TWINLIFE_MAX_SIZE];
    twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
    int len = self.cryptoKey->exportPublicKey(format, buf, sizeof(buf));
    if (len <= 0) {
        RTC_LOG(LS_ERROR) << "cannot export public key error " << len;
        return nil;
    }
    return [NSData dataWithBytes:buf length:len];
}

- (nullable NSData *)privateKey:(BOOL)isBase64 {

    unsigned char buf[TWINLIFE_MAX_SIZE];
    twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
    int len = self.cryptoKey->exportPrivateKey(format, buf, sizeof(buf));
    if (len <= 0) {
        RTC_LOG(LS_ERROR) << "cannot export private key error " << len;
        return nil;
    }
    return [NSData dataWithBytes:buf length:len];
}

- (nullable NSData *)signWithData:(nonnull NSData *)data isBase64:(BOOL)isBase64 {

    unsigned char buf[TWINLIFE_MAX_SIZE];
    twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
    int len = self.cryptoKey->sign(format, [data bytes], [data length], buf, sizeof(buf));
    if (len <= 0) {
        RTC_LOG(LS_ERROR) << "cannot sign error " << len;
        return nil;
    }
    return [NSData dataWithBytes:buf length:len];
}

- (int)verifyWithData:(nonnull NSData *)data signature:(nonnull NSData *)signature isBase64:(BOOL)isBase64 {

    twinlife::CryptoKey::Format format = isBase64 ? twinlife::CryptoKey::Format::BASE64 : twinlife::CryptoKey::Format::BINARY;
    return self.cryptoKey->verify(format, [data bytes], [data length], [signature bytes] [signature length]);
}

- (nullable NSString *)signAuthWithKey:(nonnull TLCryptoKey *)peerPublicKey item:(nonnull NSString *)item peerItem:(nonnull NSString *)peerItem {

    std::string itemStr = [item stdString];
    std::string peerItemStr = [peerItem stdString];
    unsigned char buf[TWINLIFE_MAX_SIZE];
    int len = self.cryptoKey->signAuth(peerPublicKey.cryptoKey, itemStr.data(), peerItemStr.data(), buf, sizeof(buf));
    if (len <= 0) {
        RTC_LOG(LS_ERROR) << "cannot sign error " << len;
        return nil;
    }
    return [[NSString alloc] initWithBytes:buf length:len encoding:NSUTF8StringEncoding];
}

- (int)verifyAuthWithKey:(nonnull TLCryptoKey *)peerPublicKey item:(nullable NSString *)item peerItem:(nullable NSString *)peerItem signature:(nonnull NSString *)signature {

    std::string itemStr;
    std::string peerItemStr;
    std::string signatureStr = [signature stdString];
    const char* itemPtr = nullptr;
    const char* peerItemPtr = nullptr;
    if (item) {
        itemStr = [item stdString];
        itemPtr = itemStr.data();
    }
    if (peerItem) {
        peerItemStr = [peerItem stdString];
        peerItemPtr = peerItemStr.data();
    }
    return self.cryptoKey->verifyAuth(peerPublicKey.cryptoKey, itemPtr, peerItemPtr, signatureStr.data());
}

- (void)dealloc {

    delete self.cryptoKey;
}

@end
