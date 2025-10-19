/*
 *  Copyright (c) 2023 - 2024 twinlife SA.
 *
 *  All Rights Reserved.
 *
 *  Contributors:
 *   Stephane Carrez (Stephane.Carrez@skyrock.com)
 */

#import "RTCHostname.h"

#import "helpers/NSString+StdString.h"

@implementation RTC_OBJC_TYPE (RTCHostname)

@synthesize hostname = _hostname;
@synthesize ipv4 = _ipv4;
@synthesize ipv6 = _ipv6;

- (instancetype)initWithHostname:(NSString *)hostname
                            ipv4:(nullable NSString *)ipv4
                            ipv6:(nullable NSString *)ipv6 {
  self = [super init];
  if (self) {
    _hostname = [hostname copy];
    _ipv4 = [ipv4 copy];
    _ipv6 = [ipv6 copy];
  }
  return self;
}

- (NSString *)description {
  return [NSString stringWithFormat:@"RTC_OBJC_TYPE(RTCHostname):\n%@\n%@\n%@",
                                    _hostname,
                                    _ipv4,
                                    _ipv6];
}

#pragma mark - Private

@end
