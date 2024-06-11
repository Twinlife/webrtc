/*
 *  Copyright (c) 2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "TLCryptoKey.h"

#include "rtc_base/twinlife_crypto.h"

RTC_OBJC_EXPORT
@interface TLCryptoKey ()

+ (twinlife::CryptoKey::Kind)toCryptoKind:(TLCryptoKind)kind;

@property (nonatomic, nullable) twinlife::CryptoKey *cryptoKey;

@end

