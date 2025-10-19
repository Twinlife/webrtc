**WebRTC is a free, open software project** that provides browsers and mobile
applications with Real-Time Communications (RTC) capabilities via simple APIs.
The WebRTC components have been optimized to best serve this purpose.

**Our mission:** To enable rich, high-quality RTC applications to be
developed for the browser, mobile platforms, and IoT devices, and allow them
all to communicate via a common set of protocols.

The WebRTC initiative is a project supported by Google, Mozilla and Opera,
amongst others.

### Twinlife notes

This repository contains a modified version of WebRTC available at https://webrtc.googlesource.com/src
and used by [twinme](https://www.twin.me) and [Skred](https://www.skred.app) private messaging applications.
Follow the [build instructions](twinlife/BUILDING.md) if you want to compile this project.

The changes that you will find here have a very long history, thanks to Christian Jacquemot how
initiated the project in 2014.  Since the beginning, our changes have been made in several git
repositories that were synchronized with the numerous git repositories used by the WebRTC project.
Our changes can be identified by `--twinlife--` comments.  In addition to changes in WebRTC source
code, our build integrates the [sqlcipher](https://github.com/Twinlife/sqlcipher)
and [libwebsockets](https://github.com/Twinlife/libwebsockets) components that we are using
for our applications.

We have made the following improvements and fixes for us:

- 2025-08-29: for 32-bit version use 16K alignment (tested OK on a 32-bit device).
- 2025-08-29: add ISRG Root X1 and ISG Root X2 in the ssl_roots.h certificate list.
- 2025-03-24: fixed data channel send method returns true when the message exceeds the limit (see https://issues.webrtc.org/issues/406012421)
- 2025-01-27: disable legacy `GetStats()`, disable AEC dump from compilation to reduce library size
- 2025-01-22: removed deprecated `onAddStream()`, `onRemoveStream()`, `createLocalMediaStream()`...
- 2025-01-21: remove deprecated `initializeFieldTrials()`
- 2024-06-11: added our CryptoBox and CrypoKey APIs
- 2023-07-11: provide hostname resolution in the peer connection factory (no DNS requests when using TURN)
- 2023-09-14: ignore IPv6 site local address (used by DuckDuckGo on Android) (see https://issues.webrtc.org/issues/42225831)
- 2023-09-12: restrict the ciphers for SSL and force TLS 1.3
- 2023-04-04: don't build the fake encoder/decoder (reduce library size)
- 2022-10-24: don't create the low_priority worker queue (not used for us).
- 2022-04-13: disable DTMF sender as per Threema improvement.
- 2022-04-13: don't include G711 and G722 because we don't use them
- 2020-01-29: fix multiple TCP port allocation with same configuration for a same peer connection (See https://issues.webrtc.org/issues/42221421)
- 2016-11-18: added zoom support in the Camera API for Android and iOS


### Development

See [here][native-dev] for instructions on how to get started
developing with the native code.

[Authoritative list](native-api.md) of directories that contain the
native API header files.

### More info

 * Official web site: http://www.webrtc.org
 * Master source code repo: https://webrtc.googlesource.com/src
 * Samples and reference apps: https://github.com/webrtc
 * Mailing list: http://groups.google.com/group/discuss-webrtc
 * Continuous build: https://ci.chromium.org/p/webrtc/g/ci/console
 * [Coding style guide](g3doc/style-guide.md)
 * [Code of conduct](CODE_OF_CONDUCT.md)
 * [Reporting bugs](docs/bug-reporting.md)
 * [Documentation](g3doc/sitemap.md)

[native-dev]: https://webrtc.googlesource.com/src/+/main/docs/native-code/
