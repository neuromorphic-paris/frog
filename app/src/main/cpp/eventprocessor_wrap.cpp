#include "eventprocessor.hpp"
#include <jni.h>

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_example_chronocam_atis_Eventprocessor_new_1Eventprocessor(JNIEnv *env, jobject instance) {
    jlong jresult = 0;
    EventProcessor *result = nullptr;
    result = new EventProcessor();
    *(EventProcessor **) &jresult = result;
    return jresult;
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_delete_1Eventprocessor(JNIEnv *env, jobject instance,
                                                                      jlong jniCPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    delete eventProcessor;
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_set_1bitmap(JNIEnv *env, jobject instance,
                                                           jlong objPtr, jobject bitmap) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    EventProcessor::_bitmap = env->NewGlobalRef(bitmap);
    eventProcessor->save_bitmap_info(env);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_reset_1bitmap(JNIEnv *env, jobject instance,
                                                             jlong objPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    if (eventProcessor->_bitmap_info.width == 0) {
        eventProcessor->save_bitmap_info(env);
    }
    eventProcessor->reset_bitmap(env);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_delete_1bitmap(JNIEnv *env, jobject instance,
                                                              jlong jniCPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    env->DeleteGlobalRef(EventProcessor::_bitmap);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_trigger_1sepia(JNIEnv *env, jobject instance,
                                                              jlong jniCPtr, jstring path_) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    const char *path = env->GetStringUTFChars(path_, nullptr);
    std::string esFilePath(path, 100);
    esFilePath.erase(std::find(esFilePath.begin(), esFilePath.end(), '\0'), esFilePath.end());
    eventProcessor->trigger_sepia(env, esFilePath);
    env->ReleaseStringUTFChars(path_, path);
}

JNIEXPORT jint JNICALL
Java_com_example_chronocam_atis_Eventprocessor_get_1JVM_1version(JNIEnv *env, jobject instance) {
    return env->GetVersion();
}

}