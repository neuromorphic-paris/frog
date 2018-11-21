#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_chronocam_atis_MainActivity_stringFromJNI (JNIEnv *env, jobject instance) {

    __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor init L1 ", "IM HEREEEE");

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

