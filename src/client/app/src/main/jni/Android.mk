LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := hello
LOCAL_SRC_FILES := com_moodwu_taijie_Hello.cpp

LOCAL_LDFLAGS :=
LOCAL_LDLIBS :=

include $(BUILD_SHARED_LIBRARY)
