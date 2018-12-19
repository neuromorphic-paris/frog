#include "eventprocessor.hpp"
#include <jni.h>


#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_trigger_1sepia(JNIEnv *env, jobject instance,
                                                              jlong objPtr, jstring path_) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;

    const char *path = env->GetStringUTFChars(path_, 0);
    std::string esFilePath(path, 100);
    esFilePath.erase(std::find(esFilePath.begin(), esFilePath.end(), '\0'), esFilePath.end());

    (eventProcessor)->triggerSepia(esFilePath);

    env->ReleaseStringUTFChars(path_, path);
}

JNIEXPORT jstring JNICALL
Java_com_example_chronocam_atis_Eventprocessor_string_1from_1JNI(JNIEnv *env, jobject instance) {
    __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor stringFromJNI ", "IM HEREEEE");

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_render_1preview(JNIEnv *env, jobject instance,
                                                               jlong objPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    (eventProcessor)->renderBitmapView(env);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_set_1bitmap(JNIEnv *env, jobject instance,
                                                           jlong objPtr, jobject bitmap) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    (eventProcessor)->setBitmap(bitmap);
}

JNIEXPORT jlong JNICALL
Java_com_example_chronocam_atis_Eventprocessor_new_1Eventprocessor(JNIEnv *env, jobject instance) {
    jlong jresult = 0;
    EventProcessor *result = 0;
    result = (EventProcessor *)
            new EventProcessor();
    *(EventProcessor **) &jresult = result;
    return jresult;
}

JNIEXPORT jlong JNICALL
Java_com_example_chronocam_atis_Eventprocessor_delete_1Eventprocessor(JNIEnv *env, jobject instance,
                                                                      jlong jniCPtr) {
    return 0;
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_set_1camera_1data_1Eventprocessor(
        JNIEnv *env, jobject instance, jlong jniCPtr, jobject eventprocessor, jbyteArray arg0_,
        jlong arg1) {
    jbyte *arg0 = env->GetByteArrayElements(arg0_, NULL);

    // TODO

    env->ReleaseByteArrayElements(arg0_, arg0, 0);
}
}