#include "eventprocessor.hpp"
#include <jni.h>
#include "sepia/source/sepia.hpp"

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static jobject _bitmap;

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_chronocam_atis_Eventprocessor_get_1JVM_1version(JNIEnv *env, jobject instance) {
    return env->GetVersion();
}

void setPixel(sepia::dvs_event event, AndroidBitmapInfo* info, void* pixels){
    int x = event.x;
    int scaleX = 1; //static_cast<int>(info->width/320)
    int scaleY = 1; //static_cast<int>(info->height/240)
    pixels = (char *)pixels + event.y * scaleY * info->stride;
    auto * line = (char *)pixels;
    if(event.is_increase){line[x * scaleX] = static_cast<char>(0xFF);}
    else{line[x * scaleX] = 0x00;}
}

void triggerSepia(const std::string &filepath, JNIEnv *env) {
    int counter = 0;
    JavaVM* jvm;
    env->GetJavaVM(&jvm);
    JNIEnv *threadEnv = nullptr;
    bool isThreadAttached = false;

    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(filepath), [&](sepia::dvs_event dvs_event) {
                if(dvs_event.t > 5000) {
                    if (!isThreadAttached) {
                        jvm->AttachCurrentThread(&threadEnv, nullptr);
                        isThreadAttached = true;
                    }

                    AndroidBitmapInfo info;
                    void *pixels;
                    int ret;
                    if ((ret = AndroidBitmap_getInfo(threadEnv, _bitmap, &info)) < 0) {
                        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
                        return;
                    }

                    if ((ret = AndroidBitmap_lockPixels(threadEnv, _bitmap, &pixels)) < 0) {
                        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
                    }
                    setPixel(dvs_event, &info, pixels);
                    AndroidBitmap_unlockPixels(threadEnv, _bitmap);

                    if (counter < 10) {
                        LOGD("SEPIA: JNI bitmap width: %d, height: %d, stride: %d", info.width,
                             info.height, info.stride);
                    }

                    if (dvs_event.is_increase) {
                        if (counter < 10) {
                            LOGD("+");
                            counter++;
                        }
                    } else {
                        if (counter < 10) {
                            LOGD("-");
                            counter++;
                        }
                    }
                }
    }
    /*
    ,[&](std::exception_ptr exception_pointer) {
        try {
            std::rethrow_exception(exception_pointer);
        } catch(const sepia::end_of_file& exception) {
            jvm->DetachCurrentThread();
            // these lines are executed if something interrupted the event stream
            // call exception.what() to get the error message
        }
    }
    */);
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_MainActivity_trigger_1sepia(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, nullptr);
    std::string esFilePath(path, 100);
    esFilePath.erase(std::find(esFilePath.begin(), esFilePath.end(), '\0'), esFilePath.end());
    triggerSepia(esFilePath, env);
    env->ReleaseStringUTFChars(path_, path);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_set_1camera_1data_1Eventprocessor(
        JNIEnv *env, jobject instance, jlong jniCPtr, jobject eventprocessor, jbyteArray arg0_,
        jlong arg1) {
    jbyte *arg0 = env->GetByteArrayElements(arg0_, NULL);
    // TODO
    env->ReleaseByteArrayElements(arg0_, arg0, 0);
}

JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_CameraView_setBitmap(JNIEnv *env, jclass type, jobject bitmap) {
    _bitmap = env->NewGlobalRef(bitmap);

    AndroidBitmapInfo  info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, _bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
    LOGD("JNI bitmap width: %d, height: %d, stride: %d", info.width, info.height, info.stride);
}

}extern "C"
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_CameraView_deleteBitmap(JNIEnv *env, jclass type) {
    env->DeleteGlobalRef(_bitmap);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_CameraView_resetBitmap(JNIEnv *env, jclass type) {
    AndroidBitmapInfo info;
    void *pixels;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, _bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if ((ret = AndroidBitmap_lockPixels(env, _bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    int x, y;
    for (y=0;y<info.height;y++) {
        auto * line = (uint32_t *)pixels;
        for (x=0;x<info.width;x++) {
            line[x] = 0x00;
        }
        pixels = (char *)pixels + info.stride;
    }

    AndroidBitmap_unlockPixels(env, _bitmap);
}