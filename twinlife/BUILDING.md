Building WebRTC can be difficult (it is a nightmare compared to some other OpenSource projects)
and over time we have used different approches.
To build this GitHub project we have made the choice to rely on an external manual
and full extraction of WebRTC.

https://webrtc.github.io/webrtc-org/native-code/development/

## Get WebRTC official sources

1. Create webrtc_google dedicated repository
   ```bash
   $ mkdir .../webrtc_google
   $ cd .../webrtc_google
   ```

2. Install depo-tools repository
   ```bash
   $ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
   $ export PATH=.../webrtc_google/depot_tools:$PATH
   ```

3. Getting webrtc code
   If you want to build for Android, use (be patient):

   ```bash
   $ fetch --nohooks webrtc_android
   ```

   If you want to build for iOS, use (be patient:

   ```bash
   $ fetch --nohooks webrtc_ios
   ```

   Then, following the process to get webrtc code:

   ```bash
   $ gclient sync
   $ du -sh .
	19G	.
   ```

4. Switch to the same WebRTC branch as our integration: WebRTC 144
   ```bash
   $ cd src
   $ git checkout -b 7559/master branch-heads/7559
   $ git pull origin
   $ gclient sync
   $ gclient sync -D
   ```
   The second call `gclient sync -D` is optional but it seems necessary.
   It does not harm to run it.

## Prepare source tree

Our preparation script will setup various symbolic link to WebRTC 144
tools, third_party, buildtools, base, testing and build directories.
Setup the `WEBRTC_SRC_DIR` environment variable to refer to the WebRTC
extraction and run our `webrtc-setup.sh` script.

   ```bash
   $ export WEBRTC_SRC_DIR=.../webrtc_google/src
   $ bash ./twinlife/bin/webrtc-setup.sh
   ```

## Generating Ninja project files

Run the `build.sh` script at the top of our WebRTC source tree.
On Linux, the script will configure to build WebRTC for Android and
on MacOS, it will configure for iOS target.
This generates the Ninja project files by using `gn` and our specific
configuration:

```bash
./twinlife/bin/build.sh
```

## Compiling

Run the `build-release.sh` script to build the 32-bit and 64-bit Android or iOS 64-bit libraries.

```bash
./twinlife/bin/build-release.sh
```

