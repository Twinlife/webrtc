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
#import "helpers/NSString+StdString.h"

#include "rtc_base/twinlife_crypto.h"
#include "rtc_base/logging.h"

RTC_OBJC_EXPORT
@interface TLCryptoBox ()

@property (nonatomic, nullable) twinlife::CryptoBox *cryptoBox;

- (nonnull instancetype)initWithCryptoBox:(twinlife::CryptoBox *)cryptoBox;

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

   return self.cryptoBox->bind(encrypt ? true : false, privateKey.cryptoKey, publicKey.cryptoKey, (const unsigned char*)[salt bytes], (size_t)[salt length]);
}

- (int)bindWithKey:(nonnull NSData *)key {

   return self.cryptoBox->bind((const unsigned char*)[key bytes], (size_t)[key length]);
}

- (void)unbind {

    self.cryptoBox->unbind();
}

- (int)encryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data auth:(nonnull NSData *)auth output:(nonnull NSMutableData *)output {

    return self.cryptoBox->encryptAEAD((const unsigned char*)[data bytes], (size_t)[data length], (const unsigned char*)[auth bytes], (size_t)[auth length], nonceSequence, (unsigned char*)[output bytes], (size_t)[output length]);
}

- (int)decryptAEAD:(int64_t)nonceSequence data:(nonnull NSData *)data authLength:(int)authLength output:(nonnull NSMutableData *)output {

    return self.cryptoBox->decryptAEAD((const unsigned char *)[data bytes], (size_t)[data length], (size_t)authLength, nonceSequence, (unsigned char *)[output bytes], (size_t)[output length]);
}

- (void)dealloc {

    delete self.cryptoBox;
}

@end
