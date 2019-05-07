#include "eventprocessor.hpp"
#include <jni.h>
#include <zconf.h>
#include <unistd.h>

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
Java_com_paris_neuromorphic_Eventprocessor_set_1bitmap(JNIEnv *env, jclass instance,
                                                       jlong objPtr, jobject bitmap) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    eventProcessor->_bitmap = env->NewGlobalRef(bitmap);
    eventProcessor->save_bitmap_info(env);
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
Java_com_paris_neuromorphic_Eventprocessor_delete_1bitmap(JNIEnv *env, jclass instance,
                                                          jlong jniCPtr) {
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
                                                             jlong camera_data_length,
                                                             jboolean is_recorded) {
    std::chrono::system_clock::time_point start_copying, end_copying;

    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;

    start_copying = std::chrono::system_clock::now();
    jbyte *camera_data = env->GetByteArrayElements(camera_data_, nullptr);
    (eventProcessor)->set_camera_data(env, (unsigned char *) camera_data,
                                      (unsigned long) camera_data_length, is_recorded);
    env->ReleaseByteArrayElements(camera_data_, camera_data, 0);
    end_copying = std::chrono::system_clock::now();

    std::chrono::duration<double, std::milli> time_copying = end_copying - start_copying;
    //LOGD("within wrap .cpp, locking and parsing takes %fms", time_copying.count());
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_gestures_1init(JNIEnv *env, jclass type, jlong objPtr,
                                                          jstring arg0_, jstring arg1_,
                                                          jboolean denoise, jboolean bgDenoise,
                                                          jboolean refrac, jint gest_mode) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;
    const char *l1ProtoPath = env->GetStringUTFChars(arg0_, 0);
    const char *gestureSigPath = env->GetStringUTFChars(arg1_, 0);

    std::string stdL1ProtoPath(l1ProtoPath, 100);
    stdL1ProtoPath.erase(std::find(stdL1ProtoPath.begin(), stdL1ProtoPath.end(), '\0'),
                         stdL1ProtoPath.end());
    std::string stdGestureSigPath(gestureSigPath, 100);
    stdGestureSigPath.erase(std::find(stdGestureSigPath.begin(), stdGestureSigPath.end(), '\0'),
                            stdGestureSigPath.end());
    (eventProcessor)->gesture_init(stdL1ProtoPath, stdGestureSigPath, denoise, bgDenoise, refrac,
                                   gest_mode);

    env->ReleaseStringUTFChars(arg0_, l1ProtoPath);
    env->ReleaseStringUTFChars(arg1_, gestureSigPath);
}

JNIEXPORT jint JNICALL
Java_com_paris_neuromorphic_Eventprocessor_get_1JVM_1version(JNIEnv *env, jclass instance) {
    return env->GetVersion();
}

JNIEXPORT jstring JNICALL
Java_com_paris_neuromorphic_Eventprocessor_predict(JNIEnv *env, jclass type, jlong jniCPtr) {
    //	DOWN, HOME, LEFT, RIGHT, SELECT, UP
    jstring jpredict = env->NewStringUTF("0.000000,0.000000,0.000000,0.000000,0.000000,0.000000");
    char *cpredict = (char *) env->GetStringUTFChars(jpredict, nullptr);
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    (eventProcessor)->predict(cpredict);
    jpredict = env->NewStringUTF(cpredict);
    env->ReleaseStringUTFChars(jpredict, cpredict);

    return jpredict;
}

void *threadFunction(EventProcessor *eventProcessor) {
    JNIEnv *threadEnv = nullptr;
    eventProcessor->jvm->AttachCurrentThread(&threadEnv,
                                             nullptr);
    while (eventProcessor->is_processing) {
        sepia::dvs_event event = {};
        if (eventProcessor->_fifo.pull(event)) {
            LOGD("t: %u", event.t);
        };
    }

    eventProcessor->jvm->DetachCurrentThread();
    return nullptr;
}

JNIEXPORT void JNICALL
Java_com_paris_neuromorphic_Eventprocessor_create_1thread(JNIEnv *env, jclass type, jlong jniCPtr) {
    EventProcessor *eventProcessor = *(EventProcessor **) &jniCPtr;
    env->GetJavaVM(&eventProcessor->jvm);

    eventProcessor->_fifo.push(sepia::dvs_event{1, 2, 2, false});
    eventProcessor->_fifo.push(sepia::dvs_event{2, 3, 3, false});
    eventProcessor->_fifo.push(sepia::dvs_event{5, 4, 4, false});

    std::thread my_thread(threadFunction, eventProcessor);

    LOGD("before join");
    //my_thread.detach();
    LOGD("after join");
    //sleep(1);

    //eventProcessor->is_processing = false;
    my_thread.join();

}

}