#include "eventprocessor.hpp"
#include "sepia/source/sepia.hpp"

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


void EventProcessor::triggerSepia(std::string filepath) {
    int counter = 0;
    sepia::join_observable<sepia::type::dvs>(sepia::filename_to_ifstream(filepath), [&](sepia::dvs_event dvs_event) {
        if (dvs_event.is_increase) {
            if (counter < 10) {__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", "+"); counter++;}
        } else {
            if (counter < 10) {__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor triggerSepia ", "-"); counter++;}
        }
    });
}
/*
void EventProcessor::renderBitmapView(JNIEnv *env) {
    AndroidBitmapInfo  info;
    void*              pixels;
    int                ret;

    if ((ret = AndroidBitmap_getInfo(env, this->jbitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, this->jbitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    // Drawing rectangle
    int width = info.width, height = info.height, stride = info.stride;

    int yy;
    for (yy = 0;  yy < height; yy++){
        uint16_t* line = (uint16_t*) pixels;

        int xx;
        for (xx = 0; xx < width; xx++){
            line[xx] = 255;
        }
        pixels = (char*)pixels + stride;
    }

    AndroidBitmap_unlockPixels(env, this->jbitmap);
}
*/