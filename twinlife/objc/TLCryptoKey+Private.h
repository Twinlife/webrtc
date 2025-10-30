/*
 *  Copyright (c) 2024 twinlife SA.
 *  SPDX-License-Identifier: AGPL-3.0-only
 *
 *  Contributor:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "TLCryptoKey.h"

#include "twinlife/twinlife_crypto.h"

RTC_OBJC_EXPORT
@interface TLCryptoKey ()

+ (twinlife::CryptoKey::Kind)toCryptoKind:(TLCryptoKind)kind;

@property (nonatomic, nullable) twinlife::CryptoKey *cryptoKey;

@end

