LOCAL_PATH := $(call my-dir)
MY_LOCAL_PATH := $(LOCAL_PATH)
$(warning $(MY_LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_MODULE := xiaogua
LOCAL_SRC_FILES := xiaogua.cc 

LOCAL_STATIC_LIBRARIES := kws

LOCAL_CFLAGS += -std=gnu++11
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)

include $(MY_LOCAL_PATH)/kws/Android.mk

