#include "eventprocessor.hpp"
#include "sepia/source/sepia.hpp"

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

void EventProcessor::save_bitmap_info(JNIEnv *env, jobject *_bitmap) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, *_bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
    LOGD("JNI bitmap width: %d, height: %d, stride: %d", info.width, info.height, info.stride);
    this->_bitmap_info = info;
}

void EventProcessor::reset_bitmap(JNIEnv *pEnv, jobject *pBitmap) {
    void *pixels;
    int ret;

    if ((ret = AndroidBitmap_lockPixels(pEnv, *pBitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    int x, y;
    for (y = 0; y < this->_bitmap_info.height; y++) {
        auto *line = (uint32_t *) pixels;
        for (x = 0; x < this->_bitmap_info.width; x++) {
            line[x] = 0x00;
        }
        pixels = (char *) pixels + this->_bitmap_info.stride;
    }

    AndroidBitmap_unlockPixels(pEnv, *pBitmap);
}

void setPixel(sepia::dvs_event event, AndroidBitmapInfo *info, void *pixels) {
    int x = event.x;
    int scaleX = 1; //static_cast<int>(info->width/320)
    int scaleY = 1; //static_cast<int>(info->height/240)
    pixels = (char *) pixels + event.y * scaleY * info->stride;
    auto *line = (char *) pixels;
    if (event.is_increase) { line[x * scaleX] = static_cast<char>(0xFF); }
    else { line[x * scaleX] = 0x00; }
}

void EventProcessor::trigger_sepia(JNIEnv *env, std::string filepath, jobject *_bitmap) {
    int counter = 0;
    JavaVM *jvm;
    env->GetJavaVM(&jvm);
    JNIEnv *threadEnv = nullptr;
    bool isThreadAttached = false;

    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(filepath),
                                             [&](sepia::dvs_event dvs_event) {
                                                 if (dvs_event.t > 5000) {
                                                     if (!isThreadAttached) {
                                                         jvm->AttachCurrentThread(&threadEnv,
                                                                                  nullptr);
                                                         isThreadAttached = true;
                                                     }

                                                     AndroidBitmapInfo info;
                                                     void *pixels;
                                                     int ret;
                                                     if ((ret = AndroidBitmap_getInfo(threadEnv,
                                                                                      *_bitmap,
                                                                                      &info)) < 0) {
                                                         LOGE("AndroidBitmap_getInfo() failed ! error=%d",
                                                              ret);
                                                         return;
                                                     }

                                                     if ((ret = AndroidBitmap_lockPixels(threadEnv,
                                                                                         *_bitmap,
                                                                                         &pixels)) <
                                                         0) {
                                                         LOGE("AndroidBitmap_lockPixels() failed ! error=%d",
                                                              ret);
                                                     }
                                                     setPixel(dvs_event, &info, pixels);
                                                     AndroidBitmap_unlockPixels(threadEnv,
                                                                                *_bitmap);

                                                     if (counter < 10) {
                                                         LOGD("SEPIA: JNI bitmap width: %d, height: %d, stride: %d",
                                                              info.width,
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
    );
}


