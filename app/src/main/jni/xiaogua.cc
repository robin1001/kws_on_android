//
// Created by binbzha on 7/13/2017.
//
#include <string.h>
#include <assert.h>

#include "kws/feature-pipeline.h"
#include "kws/kws.h"

#include "com_example_binbzha_xiaogua_Kws.h"

Kws *kws = NULL;

JNIEXPORT jstring JNICALL Java_com_example_binbzha_xiaogua_Kws_Hello
  (JNIEnv *env, jobject jobj) {
    return env->NewStringUTF("Hello JNI!");
}

JNIEXPORT void JNICALL Java_com_example_binbzha_xiaogua_Kws_Init
  (JNIEnv *env, jobject jobj, jstring netFile, jstring cmvnFile,
  jstring fsmFile) {
    assert(kws == NULL);
    const char *net_file= env->GetStringUTFChars(netFile, NULL);
    const char *cmvn_file= env->GetStringUTFChars(cmvnFile, NULL);
    const char *fsm_file= env->GetStringUTFChars(fsmFile, NULL);
    
    FeaturePipelineConfig feature_config;
    feature_config.num_bins = 40;
    feature_config.frame_shift = 160;
    feature_config.frame_length = 400;
    feature_config.sample_rate = 16000;
    feature_config.left_context = 5;
    feature_config.right_context = 5;
    feature_config.cmvn_file = cmvn_file;

    KwsConfig kws_config;
    kws_config.feature_config = feature_config;
    kws_config.net_file = net_file;
    kws_config.fsm_file = fsm_file;
    kws_config.thresh = 0.8;
    kws = new Kws(kws_config);

    env->ReleaseStringUTFChars(netFile, net_file);
    env->ReleaseStringUTFChars(cmvnFile, cmvn_file);
    env->ReleaseStringUTFChars(fsmFile, fsm_file);
}


JNIEXPORT jboolean JNICALL Java_com_example_binbzha_xiaogua_Kws_DetectOnline
  (JNIEnv *env, jobject obj, jshortArray jarr, jboolean end) {
    assert(kws != NULL);
    jshort *array = env->GetShortArrayElements(jarr, NULL); 
    if (NULL == array) return false;
    jsize size =  env->GetArrayLength(jarr);
    std::vector<float> data(size, 0.0);
    for (int i = 0; i < size; i++) data[i] = array[i];
    bool detected = kws->DetectOnline(data, end);
    env->ReleaseShortArrayElements(jarr, array, 0);
    return detected;
}

JNIEXPORT void JNICALL Java_com_example_binbzha_xiaogua_Kws_Reset
  (JNIEnv *env, jobject obj) {
    assert(kws != NULL);
    kws->Reset();
}

