			WebRTC/android Update
			=====================

1. Create webrtc_android dedicated repository
   $ mkdir .../webrtc_android
   
2. Install depo-tools repository
   $ cd .../webrtc_android
   $ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
   $ export PATH=.../webrtc_android/depot_tools:$PATH

3. Getting webrtc code
   $ fetch --nohooks webrtc (be patient...)
   $ gclient sync  (be patient...)
   $ du -sh .
	19G	.

4. Create webrtc dedicated repository
   $ mkdir .../webrtc
   $ cd .../webrtc
   $ repo init -u ssh://username@git.twinlife-systems.com:29418/device/android/manifests -m webrtc-1.0.xml
   $ repo sync
   $ du -sh .
	11G	.
   $ cd webrtc-library
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  google/master
	* twinlife-1.0/master
   $ cd base
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/android/master remotes/twinlife/twinlife-1.0/android/master
   $ git branch
	  google/master
	* twinlife-1.0/android/master
   $ cd third_party
   $ cd double_conversion
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd dynamic_annotations
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libevent
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd symbolize
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd xdg_mime
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd xdg_user_dirs
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ../..
   $ cd build
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/android/master remotes/twinlife/twinlife-1.0/android/master
   $ git branch
	  google/master
	* twinlife-1.0/android/master
   $ linux/debian_sid_amd64-sysroot
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ../../..
   $ cd buildtools
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd third_party
   $ cd libc++
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libc++abi
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ../../..
   $ cd testing
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/android/master remotes/twinlife/twinlife-1.0/android/master
   $ git branch
	  google/master
	* twinlife-1.0/android/master
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd tools
   $ cd android
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  google/master
	* twinlife-1.0/master
   $ cd ..
   $ cd clang
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd grit
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd ninja
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ../..
   $ cd third_party
   $ cd abseil-cpp
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd android_deps
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  google/master
	* twinlife-1.0/master
   $ cd ..
   $ cd android_ndk
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd android_sdk
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd androidx
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  google/master
	* twinlife-1.0/master
   $ cd ..
   $ cd boringssl
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd crc32c
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  google/master
	* twinlife-1.0/master
   $ cd ..
   $ cd expat
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd google_benchmark
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd googletest
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd ijar
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd jdk
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd json
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libaom
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libjpeg_turbo
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libsrtp
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libvpx
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd libwebsockets
   $ git checkout -b libwebsockets/master remotes/twinlife/libwebsockets/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  libwebsockets/master
	* twinlife-1.0/master
   $ cd libyuv
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd llvm-build
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd modp_b64
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd nasm
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd opus
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd pffft
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd rnnoise
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd sqlcipher
   $ git checkout -b sqlcipher/master remotes/twinlife/sqlcipher/master
   $ git checkout -b twinlife-1.0/master remotes/twinlife/twinlife-1.0/master
   $ git branch
	  sqlcipher/master
	* twinlife-1.0/master
   $ cd usrsctp
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master
   $ cd ..
   $ cd zlib
   $ git checkout -b google/master remotes/twinlife/google/master
   $ git branch
	* google/master

5. Compile WebRTC
   $ cd .../webrtc
   $ cd webrtc-library
   $ ./build.sh
   $ ./build-release.sh

6. Synchronize webrtc_android with up-to-date branch (https://chromiumdash.appspot.com/schedule, https://chromiumdash.appspot.com/branches)
   $ cd .../webrtc_android
   $ cd src
   $ git pull origin
   $ git checkout -b 4758/master branch-heads/4758 (WebRTC 98)
   $ export PATH=.../webrtc_android/depot_tools:$PATH
   $ gclient sync
   $ gclient sync -D (optional)

7. Update webrtc to WebRTC 98
   $ cd .../webrtc
   $ cd webrtc-library
   $ cd third_party
   $ cd zlib
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/zlib/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd usrsctp
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/usrsctp/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd rnnoise
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/rnnoise/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd pffft
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/pffft/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd opus
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/opus/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd nasm
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/nasm/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd modp_b64
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/modp_b64/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd llvm-build
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/llvm-build/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd libvpx
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/libvpx/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd libsrtp
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/libsrtp/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd libjpeg_turbo
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/libjpeg_turbo/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd libaom
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/libaom/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd jsoncpp
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/jsoncpp/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd jdk
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/jdk/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd ijar
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/ijar/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd googletest
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/googletest/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd google_benchmark
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/google_benchmark/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd expat
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/expat/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd crc32c
   $ git checkout google/master
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/crc32c/*
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/master
   $ git merge google/master (if required)
   $ cd ..
   $ cd boringssl
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/boringssl/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd androidx
   $ git checkout google/master
   $ rm -rf *
   // follow links
   $ cp -Lr .../webrtc_android/src/third_party/androidx/* .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/master
   $ git merge google/master (if required)
   $ cd ..
   $ cd android_sdk
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/android_sdk/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd android_ndk
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/android_ndk/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd android_deps
   $ git checkout google/master
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/android_deps/*
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/master
   $ git merge google/master (if required)
   $ cd ..
   $ cd abseil-cpp
   $ rm -rf *
   $ cp -r .../webrtc_android/src/third_party/abseil-cpp/*
   $ git add . (if required)
   $ git commit (if required)
   $ cd ../..
   $ cd tools
   $ cd android
   $ git checkout google/master
   $ rm -rf *
   $ cp -r .../webrtc_android/src/tools/android/* .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/master
   $ git merge google/master (if required)
   $ cd ..
   $ cd clang
   $ rm -rf *
   $ cp -r .../webrtc_android/src/tools/clang/* .
   $ git add . (if required)
   $ git commit (if required)
   $ cd ..
   $ cd grit
   $ rm -rf *
   $ cp -r .../webrtc_android/src/tools/grit/* .
   $ git add . (if required)
   $ git commit (if required)
   $ cd ../..
   $ cd testing
   $ git checkout google/master
   $ rm -rf *
   $ cp -r .../webrtc_android/src/testing/* .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/android/master
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
   $ cd ..
   $ cd libc++abi
   $ rm -rf *
   $ cp -r .../webrtc_android/src/buildtools/third_party/libc++abi/* .
   $ git add . (if required)
   $ git commit (if required)
   $ cd ../../..
   $ cd build
   $ git checkout google/master
   $ mv linux/debian_sid_amd64-sysroot ../..
   $ rm -rf *
   $ cp -r .../webrtc_android/src/build/* .
   $ rm -rf linux/debian_sid_amd64-sysroot
   $ mv ../../debian_sid_amd64-sysroot linux
   // do not add linux/debian_sid_i386-sysroot
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/android/master
   $ git merge google/master (if required)
   $ cd linux/debian_sid_amd64-sysroot
   $ rm -rf *
   $ cp -r .../webrtc_android/src/build/linux/debian_sid_amd64-sysroot/* .
   $ git add . (if required)
   $ git commit (if required)
   $ cd ../../..
   $ cd base
   $ git checkout google/master
   $ git pull twinlife <<< required due to a missing git push 
   $ mv third_party ../..
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/* .
   $ rm -rf third_party
   $ mv ../../third_party .
   $ git add . (if required)
   $ git commit (if required)
   $ git checkout twinlife-1.0/android/master
   $ git merge google/master (if required)
   $ cd third_party
   $ cd double_conversion
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/double_conversion/* .
   $ cd ..
   $ cd dynamic_annotations
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/dynamic_annotations/* .
   $ cd ..
   $ cd libevent
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/libevent/* .
   $ cd ..
   $ cd symbolize
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/symbolize/* .
   $ cd ..
   $ cd xdg_mime
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/xdg_mime/* .
   $ cd ..
   $ cd xdg_user_dirs
   $ rm -rf *
   $ cp -r .../webrtc_android/src/base/third_party/xdg_user_dirs/* .
   $ cd ../../..
   $ git checkout google/master
   $ git pull .../webrtc_android/src
   $ git checkout twinlife-1.0/master
   // add missing components
   $ cd buildtools/third_party
   $ cp -r .../webrtc_android/src/buildtools/third_party/libunwind .
   $ cd ../..
   $ cd third_party
   $ cp -r .../webrtc_android/src/third_party/dav1d .
   $ cd ../..
   // fix compilation errors
   >> third_party/sqlcipher/src/sqlite3.c
   // build
   $ ./build.sh
   $ ./build-release.sh

 webrtc/base/third_party/double_conversion

