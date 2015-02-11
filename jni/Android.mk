# $Id$

LOCAL_PATH	:= $(call my-dir)
include $(CLEAR_VARS)

# Get PJ build settings
include ../../../build.mak
include $(PJDIR)/build/common.mak

# Android build settings
LOCAL_MODULE    := libssapp
LOCAL_CFLAGS    := -Werror $(APP_CFLAGS) -frtti
LOCAL_LDFLAGS   := $(APP_LDFLAGS)
LOCAL_LDLIBS    := $(APP_LDLIBS) -llog
LOCAL_SRC_FILES := ss_app.c ss_user.c com_smart_smartsip_SmartSipApp.cpp

include $(BUILD_SHARED_LIBRARY)
