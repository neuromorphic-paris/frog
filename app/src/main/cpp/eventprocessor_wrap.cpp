#include "eventprocessor.hpp"
#include <jni.h>


#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_chronocam_atis_Eventprocessor_get_1JVM_1version(JNIEnv *env, jobject instance) {
    return env->GetVersion();
}

extern "C" {
JNIEXPORT void JNICALL
Java_com_example_chronocam_atis_Eventprocessor_trigger_1sepia(JNIEnv *env, jobject instance,
                                                              jlong objPtr, jobject byteBuf,
                                                              jstring path_) {
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
                                                               jobject handle) {
    EventProcessor* jniBitmap = (EventProcessor*) env->GetDirectBufferAddress(handle);
    if (jniBitmap == NULL || jniBitmap->_storedBitmapPixels == NULL)
        return;

    int width = jniBitmap->_bitmapInfo.width, height = jniBitmap->_bitmapInfo.height, stride = jniBitmap->_bitmapInfo.stride;

    int wheretoput = 0;
    int yy;
    for (yy = 0;  yy < height; yy++){
        uint32_t* line = (uint32_t*) jniBitmap->_storedBitmapPixels;

        for (int xx = 0; xx < width; xx++){
            line[xx++] = 100;
        }
        jniBitmap->_storedBitmapPixels = (uint32_t*)jniBitmap->_storedBitmapPixels + stride;
    }
}

JNIEXPORT jobject JNICALL
Java_com_example_chronocam_atis_Eventprocessor_set_1bitmap(JNIEnv *env, jobject instance,
                                                           jlong objPtr, jobject bitmap) {
    EventProcessor *eventProcessor = *(EventProcessor **) &objPtr;

    AndroidBitmapInfo bitmapInfo;
    uint32_t* storedBitmapPixels = NULL;
    //LOGD("reading bitmap info...");
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    //LOGD("width:%d height:%d stride:%d", bitmapInfo.width, bitmapInfo.height, bitmapInfo.stride);
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) { //ANDROID_BITMAP_FORMAT_A_8
        LOGE("Bitmap format is not RGBA_8888!");
        return NULL;
    }
    //read pixels of bitmap into native memory :
    //LOGD("reading bitmap pixels...");
    void* bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint32_t* src = (uint32_t*) bitmapPixels;
    storedBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
    int pixelsCount = bitmapInfo.height * bitmapInfo.width;
    memcpy(storedBitmapPixels, src, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, bitmap);
    eventProcessor->_bitmapInfo = bitmapInfo;
    eventProcessor->_storedBitmapPixels = storedBitmapPixels;
    return env->NewDirectByteBuffer(eventProcessor, 0);
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

