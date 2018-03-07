LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := blas
LOCAL_SRC_FILES := libopenblas_armv7p-r0.2.20.dev.a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := kws
LOCAL_SRC_FILES := kws.cc fsm.cc net.cc feature-pipeline.cc fft.cc

GEMM := BLAS

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
$(warning "compile on armeabi-v7a")

ifeq ($(GEMM), GEMMLOWP)
$(warning "compile use gemmlowp")
LOCAL_ARM_NEON := true
LOCAL_CFLAGS += -mfpu=neon -mfloat-abi=softfp -D USE_GEMMLOWP -D USE_ANDROID_LOG
else #BLAS
$(warning "compile use blas")
LOCAL_CFLAGS += -mfpu=neon -D USE_BLAS -D USE_GEMMLOWP -D USE_ANDROID_LOG
TARGET_LDFLAGS += -mfloat-abi=softfp -D ARM_SOFTFP_ABI=1
TARGET_LDFLAGS += -Wl,--no-warn-mismatch 
LOCAL_STATIC_LIBRARIES := blas
endif

endif


LOCAL_CFLAGS += -std=gnu++11
LOCAL_LDLIBS += -llog
$(warning $(LOCAL_CFLAGS))

include $(BUILD_STATIC_LIBRARY)

