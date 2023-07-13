/*
 *  Copyright (c) 2023 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributors:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import <Foundation/Foundation.h>

#import "RTCMacros.h"

NS_ASSUME_NONNULL_BEGIN

RTC_OBJC_EXPORT
@interface RTC_OBJC_TYPE (RTCHostname) : NSObject

/** Hostname. */
@property(nonatomic, readonly) NSString *hostname;

/** IPv4 represented as a string. */
@property(nonatomic, readonly) NSString *ipv4;

/** IPv6 represented as a string. */
@property(nonatomic, readonly) NSString *ipv6;

- (nonnull instancetype)init NS_UNAVAILABLE;

/**
 * Initialize an RTCHostname with its associated hostname, IPV4, IPv6.
 */
- (instancetype)initWithHostname:(NSString *)hostname
                            ipv4:(nullable NSString *)ipv4
                            ipv6:(nullable NSString *)ipv6
    NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END
