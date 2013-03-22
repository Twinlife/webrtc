LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../../../../android-webrtc.mk

LOCAL_MODULE := libwebrtc_opus

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES = \
    opus_interface.c

LOCAL_CFLAGS := \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../.. \
    $(LOCAL_PATH)/../../../../../../opus \
    $(LOCAL_PATH)/../../../../../../.. \
    $(LOCAL_PATH)/../../../../../../webrtc

include $(BUILD_STATIC_LIBRARY)
