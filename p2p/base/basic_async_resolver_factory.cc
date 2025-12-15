/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "p2p/base/basic_async_resolver_factory.h"

#include <memory>
#include <utility>

#include "absl/functional/any_invocable.h"
#include "api/async_dns_resolver.h"
#include "rtc_base/async_dns_resolver.h"
#include "rtc_base/socket_address.h"

namespace webrtc {

// --twinlife 2023-07-11: provide hostname resolution
void BasicAsyncDnsResolverFactory::setHostnames(const std::vector<webrtc::StaticHostname>& hostnames) {

  for (const webrtc::StaticHostname& host : hostnames) {
    hostnames_.push_back(host);
  }
}
// --twinlife 2023-07-11: provide hostname resolution

std::unique_ptr<AsyncDnsResolverInterface>
BasicAsyncDnsResolverFactory::Create() {
  // --twinlife 2023-07-11: provide hostname resolution
  return std::make_unique<AsyncDnsResolver>(&hostnames_);
  // --twinlife 2023-07-11: provide hostname resolution
}

std::unique_ptr<AsyncDnsResolverInterface>
BasicAsyncDnsResolverFactory::CreateAndResolve(
    const SocketAddress& addr,
    absl::AnyInvocable<void()> callback) {
  std::unique_ptr<AsyncDnsResolverInterface> resolver = Create();
  resolver->Start(addr, std::move(callback));
  return resolver;
}

std::unique_ptr<AsyncDnsResolverInterface>
BasicAsyncDnsResolverFactory::CreateAndResolve(
    const SocketAddress& addr,
    int family,
    absl::AnyInvocable<void()> callback) {
  std::unique_ptr<AsyncDnsResolverInterface> resolver = Create();
  resolver->Start(addr, family, std::move(callback));
  return resolver;
}

}  // namespace webrtc
