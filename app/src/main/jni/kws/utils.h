// Created on 2017-06-07
// Author: Binbin Zhang


#ifndef UTILS_H_ 
#define UTILS_H_

#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>

#define DISALLOW_COPY_AND_ASSIGN(Type) \
    Type(const Type &); \
    Type& operator=(const Type &)

#define LOG(format, ...) \
    do { \
        fprintf(stderr, "LOG (%s: %s(): ,%d" format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ERROR(format, ...) \
    do { \
        fprintf(stderr, "ERROR (%s: %s(): ,%d) " format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
         exit(-1); \
    } while (0)

#endif


#define LOG_TAG ("Xiaogua")

#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__))

