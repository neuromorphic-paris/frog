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
    /*
    uint16_t x = event.x;
    uint16_t y = event.y;
    bool p = event.is_increase;
    pixels = (char*)pixels + (info->width*y + x);
    auto* point = (char*) pixels;
    point = reinterpret_cast<char*>(15);
     */
    int x, y;
    for (y=0;y<info->height;y++) {
        auto * line = (uint32_t *)pixels;
        for (x=0;x<info->width;x++) {
            //set pixels Alpha, Blue, Green, Red (little Endian)
            if(y == event.y && x == event.x){
                if(event.is_increase){line[x] = 0xFF0000FF;}
                else{line[x] = 0xFFFF0000;}
            }
        }
        pixels = (char *)pixels + info->stride;
    }

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
Java_com_example_chronocam_atis_Eventprocessor_render_1preview(JNIEnv *env, jobject instance,
                                                               jobject handle) {
    auto *jniBitmap = (EventProcessor *) env->GetDirectBufferAddress(handle);
    if (jniBitmap == NULL || jniBitmap->_storedBitmapPixels == NULL)
        return;

    int width = jniBitmap->_bitmapInfo.width, height = jniBitmap->_bitmapInfo.height, stride = jniBitmap->_bitmapInfo.stride;

    int wheretoput = 0;
    int yy;
    for (yy = 0; yy < height; yy++) {
        uint32_t *line = (uint32_t *) jniBitmap->_storedBitmapPixels;

        for (int xx = 0; xx < width; xx++) {
            line[xx++] = 100;
        }
        jniBitmap->_storedBitmapPixels = (uint32_t *) jniBitmap->_storedBitmapPixels + stride;
    }
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