//
// Created by binbzha on 7/13/2017.
//
#include <string.h>
#include <assert.h>

#include "kws/feature-pipeline.h"
#include "kws/kws.h"
#include "kws/utils.h"

#include "com_example_binbzha_xiaogua_Kws.h"

Kws *kws = NULL;
//// Must keep KwsConfig a global variable, it is refered in kws modules
KwsConfig kws_config;

JNIEXPORT jstring JNICALL Java_com_example_binbzha_xiaogua_Kws_Hello
  (JNIEnv *env, jobject jobj) {
    return env->NewStringUTF("Hello JNI!");
}

JNIEXPORT void JNICALL Java_com_example_binbzha_xiaogua_Kws_Init
  (JNIEnv *env, jobject jobj, jstring netFile, jstring cmvnFile,
  jstring fstFile, jstring fillerFile) {
    CHECK(kws == NULL);
    const char *net_file= env->GetStringUTFChars(netFile, NULL);
    const char *cmvn_file= env->GetStringUTFChars(cmvnFile, NULL);
    const char *fst_file= env->GetStringUTFChars(fstFile, NULL);
    const char *filler_file= env->GetStringUTFChars(fillerFile, NULL);
    
    FeaturePipelineConfig feature_config;
    feature_config.num_bins = 40;
    feature_config.frame_shift = 160;
    feature_config.frame_length = 400;
    feature_config.sample_rate = 16000;
    feature_config.left_context = 5;
    feature_config.right_context = 5;
    feature_config.cmvn_file = cmvn_file;

    kws_config.feature_config = feature_config;
    kws_config.net_file = net_file;
    kws_config.fst_file = fst_file;
    kws_config.filler_table_file = filler_file;
    kws_config.thresh = 0.0;
    kws_config.min_keyword_frames = 0;
    kws_config.min_frames_for_last_state = 0;
    kws = new Kws(kws_config);

    env->ReleaseStringUTFChars(netFile, net_file);
    env->ReleaseStringUTFChars(cmvnFile, cmvn_file);
    env->ReleaseStringUTFChars(fstFile, fst_file);
    env->ReleaseStringUTFChars(fillerFile, filler_file);
}
 
 
JNIEXPORT jboolean JNICALL Java_com_example_binbzha_xiaogua_Kws_DetectOnline
  (JNIEnv *env, jobject obj, jshortArray jarr, jboolean end) {
    CHECK(kws != NULL);
    jshort *array = env->GetShortArrayElements(jarr, NULL); 
    if (NULL == array) return false;
    jsize size =  env->GetArrayLength(jarr);
    std::vector<float> data(size, 0.0);
    for (int i = 0; i < size; i++) data[i] = array[i];
    float confidence = 0.0f;
    int32_t keyword = 0;
    bool legal = kws->DetectOnline(data, end, &confidence, &keyword);
    env->ReleaseShortArrayElements(jarr, array, 0);
    return legal;
}
 
JNIEXPORT void JNICALL Java_com_example_binbzha_xiaogua_Kws_Reset
  (JNIEnv *env, jobject obj) {
    CHECK(kws != NULL);
    kws->Reset();
}

JNIEXPORT void JNICALL Java_com_example_binbzha_xiaogua_Kws_SetThresh
  (JNIEnv *env, jobject obj, jfloat thresh) {
    CHECK(kws != NULL);
    kws->SetThresh(thresh);
}

