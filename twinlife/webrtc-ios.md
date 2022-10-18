			WebRTC/iOS Update
			=================

1. Create webrtc dedicated repository
   $ mkdir webrtc_ios
   
2. Install depo-tools repository
   $ cd webrtc_ios
   $ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
   $ export PATH=.../webrtc_ios/depot_tools:$PATH

3. Getting webrtc code
   $ fetch --nohooks webrtc (be patient...)
   $ gclient sync (be patient...)
   $ du -sh .
	19G	.

4. Create webrtc dedicated repository
   $ mkdir .../webrtc
   $ cd .../webrtc
   $ repo init -u ssh://username@git.twinlife-systems.com:29418/device/ios/manifests -m webrtc-1.0.xml
   $ repo sync

5. Synchronize webrtc_ios with up-to-date branch (https://chromiumdash.appspot.com/schedule, https://chromiumdash.appspot.com/branches)
   $ cd .../webrtc_ios
   $ cd src
   $ git pull origin
   $ git checkout -b 4758/master branch-heads/4758 (WebRTC 98)
   $ export PATH=.../webrtc_ios/depot_tools:$PATH
   $ gclient sync
   $ gclient sync -D (optional)

6. SCz: Do the Android webrtc integration

7. SCz: Continue webrtc_ios integration
   $ cd .../webrtc/webrtc-library
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git pull
   $ git checkout ios/master
   $ git merge google/master

   $ cd ../..
   $ cd testing
   $ git checkout google/master
   $ git checkout twinlife-1.0/ios/master
   $ git merge google/master (if required)
   $ cd ..

   $ cd buildtools
   $ mv third_party ../..
   $ rm -rf *
   $ cp -r .../webrtc_android/src/buildtools/* .
   $ rm -rf third_party
   $ mv ../../third_party .
   $ git add . (if required)
   $ git commit (if required)
   $ cd third_party
   $ cd libc++
   $ rm -rf *
   $ cp -r .../webrtc_android/src/buildtools/third_party/libc++/* .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout -b twinlife-1.0/ios/master remotes/twinlife/twinlife-1.0/ios/master
   $ git merge google/master
   $ cd ..
   $ cd libc++abi
   $ git checkout -b google/master remotes/twinlife/google/master
   $ rm -rf *
   $ cp -r .../webrtc_android/src/buildtools/third_party/libc++abi/* .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout -b ios/master remotes/twinlife/twinlife-1.0/ios/master
   $ git merge google/master 

   $ cd .../third_party
   $ cd llvm-build
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/llvm-build/*
   $ git add . (if required)
   $ git commit (if required)

   $ cd .../build
   $ git checkout google/master
   $ git pull
   $ git checkout ios/master
   $ git merge google/master

8. Compile WebRTC
   $ ./build.sh
   $ ./build-release.sh
   // fix compilation errors

