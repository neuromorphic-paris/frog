#include <jni.h>
#include <string>
#include <android/log.h>
#include <iostream>
#include "eventprocessor.hpp"
#include "sepia/source/sepia.hpp"

extern "C"
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_triggerSepia(JNIEnv *env, jobject instance,
                                                          jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    std::string stdL1ProtoPath(path, 100);
    stdL1ProtoPath.erase(std::find(stdL1ProtoPath.begin(), stdL1ProtoPath.end(), '\0'), stdL1ProtoPath.end());
    __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", "init");
    //__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", path_);
    //__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", path);
    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(stdL1ProtoPath), [&](sepia::dvs_event dvs_event) {
        if (dvs_event.is_increase) {
            __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", "+");
        } else {
            __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", "-");
        }
    });

    env->ReleaseStringUTFChars(path_, path);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_chronocam_atis_Eventprocessor_stringFromJNI(JNIEnv *env, jobject instance) {
    __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor stringFromJNI ", "IM HEREEEE");

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" {

    JNIEXPORT jlong JNICALL
    Java_com_example_chronocam_atis_Eventprocessor_new_1Eventprocessor(JNIEnv *env, jobject instance) {
        jlong jresult = 0;
        EventProcessor *result = 0;
        result = (EventProcessor * )
        new EventProcessor();
        *(EventProcessor * *) & jresult = result;
        return jresult;
    }

    JNIEXPORT jlong JNICALL
    Java_com_example_chronocam_atis_Eventprocessor_delete_1Eventprocessor(JNIEnv *env, jobject instance, jlong jniCPtr) {

        // TODO

    }

    JNIEXPORT void JNICALL
    Java_com_example_chronocam_atis_Eventprocessor_set_1camera_1data_1Eventprocessor(
    JNIEnv *env, jobject instance, jlong jniCPtr, jobject eventprocessor, jbyteArray arg0_, jlong arg1) {
        jbyte *arg0 = env->GetByteArrayElements(arg0_, NULL);

        // TODO

        env->ReleaseByteArrayElements(arg0_, arg0, 0);
    }
}