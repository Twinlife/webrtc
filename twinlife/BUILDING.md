Building WebRTC can be difficult and over time we have used different approches.
To build this GitHub project we have made the choice to rely on an external manual
and full extraction of WebRTC.

https://webrtc.github.io/webrtc-org/native-code/development/

## Get WebRTC official sources

1. Create webrtc_android dedicated repository
   ```
   $ mkdir .../webrtc_android
   ```

2. Install depo-tools repository
   ```
   $ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
   $ export PATH=.../webrtc_android/depot_tools:$PATH
   ```

3. Getting webrtc code
   ```
   $ fetch --nohooks webrtc (be patient...)
   $ gclient sync  (be patient...)
   $ du -sh .
	19G	.
   ```

4. Switch to the same WebRTC branch as our integration: WebRTC 134
   ```
   $ cd src
   $ git pull origin
   $ git checkout -b 6998/master branch-heads/6998
   $ gclient sync
   ```

## Prepare source tree

Our preparation script will setup various symbolic link to WebRTC 134
tools, third_party, buildtools, base, testing and build directories.
Setup the `WEBRTC_SRC_DIR` environment variable to refer to the WebRTC
extraction and run our `webrtc-setup.sh` script.

   ```
   $ export WEBRTC_SRC_DIR=.../webrtc_android/src
   $ bash ./twinlife/bin/webrtc-setup.sh
   ```

## Generating Ninja project files

Run the `build.sh` script at the top of our WebRTC source tree.
This generates the Ninja project files by using `gn` and our specific
configuration:

```
./twinlife/bin/build.sh
```

## Compiling

Run the `build-release.sh` script to build the 32-bit and 64-bit Android libraries.

```
./twinlife/bin/build-release.sh
```

