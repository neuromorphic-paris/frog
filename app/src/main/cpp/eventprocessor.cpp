#include <jni.h>
#include <string>
#include <android/log.h>
#include "eventprocessor.hpp"

extern "C" {

    JNIEXPORT jstring JNICALL
    Java_com_example_chronocam_atis_MainActivity_stringFromJNI(JNIEnv *env, jobject instance) {

        __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor stringFromJNI ", "IM HEREEEE");

        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

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