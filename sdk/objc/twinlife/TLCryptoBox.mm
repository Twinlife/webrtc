/*
 *  Copyright (c) 2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "TLCryptoBox.h"
#import "TLCryptoKey+Private.h"

#include "twinlife_crypto.h"
#import "base/RTCLogging.h"
#import "helpers/NSString+StdString.h"

RTC_OBJC_EXPORT
@interface TLCryptoBox ()

+ (twinlife::CryptoKey::Kind)toCryptoKind:(TLCryptoKind)kind;

@property (nonatomic, nullable) twinlife::CryptoBox *cryptoBox;

@end

RTC_OBJC_EXPORT
@implementation TLCryptoBox

- (nonnull instancetype)initWithCryptoBox:(twinlife::CryptoBox *)cryptoBox {

    self = [super init];
    if (self) {
        _cryptoBox = cryptoBox;
    }
    return self;
}

+ (nullable TLCryptoBox *)createWithKind:(TLCryptoBoxKind)kind {

    twinlife::CryptoBox::Kind cKind = kind == TLCryptoBoxKindAES_GCM ? twinlife::CryptoBox::Kind::AES_GCM : twinlife::CryptoBox::Kind::CHACHA20_POLY1305;
    twinlife::CryptoBox *cryptoBox = twinlife::CryptoBox::create<twinlife::CryptoBox>(cKind);
    return [[TLCryptoBox alloc] initWithCryptoBox:cryptoBox];
}

- (int)bindWithKey:(nonnull TLCryptoKey *)privateKey peerPublicKey:(nonnull TLCryptoKey *)publicKey encrypt:(BOOL)encrypt salt:(nonnull NSData *)salt {

   return self.cryptoBox->bind(encrypt ? true : false, privateKey.cryptoKey, publicKey.cryptoKey, [salt bytes], [salt length]);
}

- (int)bindWithKey:(nonnull NSData *)key {

   return self.cryptoBox->bind([key bytes], [key length]);
}

- (int)unbind {

    return self.cryptoBox->unbind();
}

- (int)encryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data auth:(nonnull NSData *)auth output:(nonnull NSMutableData *)output {

    return self.cryptoBox->encryptAEAD([data bytes], [data length], [auth bytes], [auth length], nonceSequence, [output bytes], [output length]);
}

- (int)decryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data authLength:(int)authLength output:(nonnull NSMutableData *)output {

    return self.cryptoBox->decryptAEAD([data bytes], [data length], authLength, nonceSequence, [output bytes], [output length]);
}

- (void)dealloc {

    delete self.cryptoBox;
}

@end
