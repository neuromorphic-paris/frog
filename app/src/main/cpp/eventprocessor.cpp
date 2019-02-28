#include "eventprocessor.hpp"

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

jobject EventProcessor::_bitmap;

void EventProcessor::save_bitmap_info(JNIEnv *env) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, EventProcessor::_bitmap, &info)) < 0) {
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

void EventProcessor::reset_bitmap(JNIEnv *pEnv) {
    void *pixels;
    int ret;
    if ((ret = AndroidBitmap_lockPixels(pEnv, EventProcessor::_bitmap, &pixels)) < 0) {
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
    AndroidBitmap_unlockPixels(pEnv, EventProcessor::_bitmap);
}

void EventProcessor::set_pixel(sepia::dvs_event event, void *pixels) {
    int x = event.x;
    const int scaleX = 3; //static_cast<int>(info.width/320);
    const int scaleY = 3; //static_cast<int>(info.height/240);
    pixels = (char *) pixels + event.y * scaleY * this->_bitmap_info.stride;
    auto *line = (char *) pixels; // select the desired scaled line of pixels
    if (event.is_increase) { // put for loops for each branch for efficiency reasons
        auto white = static_cast<unsigned char>(0xFF);
        for (int i = 0; i < scaleY; i++) { // write a square of pixels related to scale factors.
            for (int j = 0; j < scaleX; j++) {
                line[x * scaleX + j] = white;
            }
            line = (char *) pixels + this->_bitmap_info.stride;
        }
    } else {
        for (int i = 0; i < scaleY; i++){
            for (int j = 0; j < scaleX; j++) {
                line[x * scaleX + j] = 0x00;
            }
            line = (char*) pixels + this->_bitmap_info.stride;
        }
    }
}

void EventProcessor::trigger_sepia(JNIEnv *env, std::string filepath) {
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

                                                     void *pixels;
                                                     int ret;
                                                     if ((ret = AndroidBitmap_lockPixels(threadEnv,
                                                                                         EventProcessor::_bitmap,
                                                                                         &pixels)) <
                                                         0) {
                                                         LOGE("AndroidBitmap_lockPixels() failed ! error=%d",
                                                              ret);
                                                     }

                                                     set_pixel(dvs_event, pixels);

                                                     AndroidBitmap_unlockPixels(threadEnv,
                                                                                EventProcessor::_bitmap);

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

void EventProcessor::set_camera_data(JNIEnv *env, unsigned char *data, unsigned long size) {
    for (int i = 0; i < size;) {
        unsigned char a = data[i++];
        unsigned char b = data[i++];
        unsigned char c = data[i++];
        unsigned char d = data[i++];
        //	event.pol currently contains the type of event
        auto pol = (uint8_t) ((d & 0xf0) >> 4);
        if (pol == 8) {
            baseTime = (((a & 0xff) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0x0f) << 24))
                            << 11);
//        __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor ", "index=%u raw=%02X%02X%02X%02X, ts=%llu", i, a, b, c, d, (unsigned long long int) localBaseTime);
        } else if (baseTime != 0) {
            uint16_t y = 239 - (uint16_t) (a & 0xff);
            uint16_t x = (uint16_t) ((b & 0xff) | ((c & 0x01) << 8));
            uint64_t ts = baseTime + (((c & 0xff) >> 1) & 0x7f) | ((d & 0x0f) << 7);
//		__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "index=%u raw=%02X%02X%02X%02X, ts=%llu, pol=%u, x=%03u, y=%03u", i, a, b, c, d, (unsigned long long int) ts, pol, x, y);

            auto event = sepia::dvs_event{ts, x, y, static_cast<bool>(pol)};
            void *pixels;
            int ret;
            if ((ret = AndroidBitmap_lockPixels(env, EventProcessor::_bitmap, &pixels)) < 0) {
                LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
            }

            set_pixel(event, pixels);

            AndroidBitmap_unlockPixels(env, EventProcessor::_bitmap);
        }
    }
}