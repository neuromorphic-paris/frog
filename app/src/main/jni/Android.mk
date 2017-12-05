# File: Android.mk
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := eventprocessor

#LOCAL_SRC_FILES := eventprocessor_wrap.cpp eventprocessor.hpp eventprocessor.cpp

#LOCAL_SRC_FILES := eventprocessor_wrap.cpp eventprocessor2L.hpp eventprocessor2L.cpp

LOCAL_SRC_FILES := eventprocessor_wrap.cpp eventprocessor1L.hpp eventprocessor1L.cpp

LOCAL_CFLAGS    := -std=c++11 -fexceptions
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
