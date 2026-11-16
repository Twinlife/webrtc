# Host test of twinlife/twinlife_crypto.cc with the BoringSSL of the WebRTC tree.
#
#   make check              build and run the test
#   make check SANITIZE=1   same with the address and undefined behavior sanitizers
#   make clean
#
# BoringSSL is built for the host with cmake and ninja in $(OUT), once per
# variant.  ninja is run each time so that the library follows the tree
# after a WebRTC update.

ROOT      := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
TWINLIFE  := $(ROOT)/twinlife
BORINGSSL := $(ROOT)/third_party/boringssl/src
OUT       := $(ROOT)/out-test
CC        := clang
CXX       := clang++

ifdef SANITIZE
VARIANT   := sanitize
SANFLAGS  := -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
else
VARIANT   := release
SANFLAGS  :=
endif

BUILD     := $(OUT)/$(VARIANT)
LIBCRYPTO := $(BUILD)/boringssl/libcrypto.a
CXXFLAGS  := -g -Wall -fPIE $(SANFLAGS) -I$(BORINGSSL)/include -I$(TWINLIFE)
OBJS      := $(BUILD)/sign_test.o $(BUILD)/twinlife_crypto.o

.PHONY: all check clean FORCE

all: $(BUILD)/sign_test

check: $(BUILD)/sign_test
	$(BUILD)/sign_test

clean:
	rm -rf $(OUT)

$(BUILD)/sign_test: $(OBJS) $(LIBCRYPTO)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBCRYPTO) -lpthread

$(BUILD)/sign_test.o: $(TWINLIFE)/test/sign_test.cc $(TWINLIFE)/twinlife_crypto.h
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/twinlife_crypto.o: $(TWINLIFE)/twinlife_crypto.cc $(TWINLIFE)/twinlife_crypto.h
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBCRYPTO): FORCE
	@test -f $(BUILD)/boringssl/build.ninja || cmake -GNinja -S $(BORINGSSL) -B $(BUILD)/boringssl \
		-DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) \
		-DCMAKE_C_FLAGS="$(SANFLAGS)" -DCMAKE_CXX_FLAGS="$(SANFLAGS)"
	cmake --build $(BUILD)/boringssl --target crypto

FORCE:
