// Created on 2017-06-07
// Author: Binbin Zhang


#ifndef UTILS_H_ 
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_ANDROID_LOG
#include <android/log.h>
#define LOG_TAG ("Xiaogua")
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__))

#define LOG(format, ...) \
    do { \
        __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "LOG (%s: %s(): %d) " format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ERROR(format, ...) \
    do { \
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "ERROR (%s: %s(): %d) " format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
         exit(-1); \
    } while (0)

#define CHECK(test) \
    do { \
        if (!(test)) { \
            __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "CHECK (%s: %s(): %d) %s \n", \
                    __FILE__, __func__, __LINE__, #test); \
                exit(-1); \
        } \
    } while (0)

#else

#define LOG(format, ...) \
    do { \
        fprintf(stderr, "LOG (%s: %s(): %d) " format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define ERROR(format, ...) \
    do { \
        fprintf(stderr, "ERROR (%s: %s(): %d) " format "\n", \
            __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
         exit(-1); \
    } while (0)

#define CHECK(test) \
    do { \
        if (!(test)) { \
            fprintf(stderr, "CHECK (%s: %s(): %d) %s \n", \
                    __FILE__, __func__, __LINE__, #test); \
                exit(-1); \
        } \
    } while (0)

#endif

#define DISALLOW_COPY_AND_ASSIGN(Type) \
    Type(const Type &); \
    Type& operator=(const Type &)



#endif
