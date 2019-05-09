#include "eventprocessor.hpp"
#include <jni.h>

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_paris_neuromorphic_Eventprocessor_new_1Eventprocessor(JNIEnv *env, jclass type) {
    jlong jresult = 0;
    EventProcessor *result = nullptr;
    result = new EventProcessor();
    if (result->_bitmap != nullptr) {
        result->save_bitmap_info(env);
    }
    *(EventProcessor **) &jresult = result;
    return jresult;
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_delete_1Eventprocessor(JNIEnv *env, jclass instance,
                                                                  jlong jniCPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    delete eventProcessor;
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_set_1shared_1bitmap(JNIEnv *env, jclass instance,
                                                       jlong objPtr, jobject bitmap) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    eventProcessor->_bitmap = env->NewGlobalRef(bitmap);
    eventProcessor->save_bitmap_info(env);
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_update_1shared_1bitmap(JNIEnv *env, jclass type,
                                                                  jlong objPtr) {

    // TODO

}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_reset_1bitmap(JNIEnv *env, jclass instance,
                                                         jlong objPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    if (eventProcessor->_bitmap != nullptr) {
        eventProcessor->reset_bitmap(env);
    }
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_delete_1bitmap(JNIEnv *env, jclass instance, jlong jniCPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    if (eventProcessor->_bitmap != nullptr) {
        env->DeleteGlobalRef(eventProcessor->_bitmap);
    }
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_trigger_1sepia(JNIEnv *env, jclass instance,
                                                          jlong jniCPtr, jstring path_) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    const char *path = env->GetStringUTFChars(path_, nullptr);
    std::string esFilePath(path, 100);
    esFilePath.erase(std::find(esFilePath.begin(), esFilePath.end(), '\0'), esFilePath.end());
    eventProcessor->trigger_sepia(env, esFilePath);
    env->ReleaseStringUTFChars(path_, path);
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_set_1camera_1data(JNIEnv *env, jclass instance,
                                                             jlong objPtr, jbyteArray camera_data_,
                                                             jlong camera_data_length) {
    std::chrono::system_clock::time_point start_copying, end_copying;

    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;

    start_copying = std::chrono::system_clock::now();
    jbyte *camera_data = env->GetByteArrayElements(camera_data_, nullptr);
    (eventProcessor)->set_camera_data(env, (unsigned char *) camera_data, (unsigned long) camera_data_length);
    env->ReleaseByteArrayElements(camera_data_, camera_data, 0);
    end_copying = std::chrono::system_clock::now();

    std::chrono::duration<double, std::milli> time_copying = end_copying - start_copying;
    //LOGD("within wrap .cpp, locking and parsing takes %fms", time_copying.count());
}

JNIEXPORT jint JNICALL
Java_com_paris_neuromorphic_Eventprocessor_get_1JVM_1version(JNIEnv *env, jclass instance) {
    return env->GetVersion();
}

}