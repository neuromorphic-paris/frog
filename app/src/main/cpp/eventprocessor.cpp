#include "eventprocessor.hpp"

#define LOG_TAG "eventprocessor"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

void EventProcessor::save_bitmap_info(JNIEnv *env) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, this->_bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }
    LOGD("JNI bitmap width: %d, height: %d, stride: %d", info.width, info.height, info.stride);
    this->_bitmap_info = info;

    for(int x = 0; x < 304; x++){
        for(int y = 0; y < 240; y++){
            this->_bitarray[x*y] = false;
        }
    }
}

void EventProcessor::reset_bitmap(JNIEnv *pEnv) {
    void *pixels;
    int ret;
    if ((ret = AndroidBitmap_lockPixels(pEnv, this->_bitmap, &pixels)) < 0) {
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
    AndroidBitmap_unlockPixels(pEnv, this->_bitmap);
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

                                                     this->_bitarray[dvs_event.x*dvs_event.y] = dvs_event.is_increase;

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
    std::vector<sepia::dvs_event> all_events;
    all_events.reserve(size/4);

    for (int i = 0; i < size;) {
        unsigned char a = data[i++];
        unsigned char b = data[i++];
        unsigned char c = data[i++];
        unsigned char d = data[i++];
        auto pol = (unsigned char) ((d & 0xf0u) >> 4u);
        if (pol == 8) {
            this->_baseTime = (((a & 0xffu) | ((b & 0xffu) << 8u) | ((c & 0xffu) << 16u)
                    | ((d & 0x0fu) << 24u)) << 11u);
            //__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor ", "index=%u raw=%02X%02X%02X%02X, ts=%llu", i, a, b, c, d, (unsigned long long int) localBaseTime);
        } else if (this->_baseTime != 0) {
            auto y = (unsigned short) (a & 0xffu);
            if (y <= 239) {
                y = static_cast<uint16_t>(239u - y);
            } else {
                __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "Mirroring for y-axis does not work for y=%d", y);
            }
            auto x = (unsigned short) ((b & 0xffu) | ((c & 0x01u) << 8u));
            uint64_t ts = this->_baseTime + (((c & 0xffu) >> 1u) & 0x7fu) | ((d & 0x0fu) << 7u);
            //__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "index=%u raw=%02X%02X%02X%02X, ts=%llu, pol=%u, x=%03u, y=%03u", i, a, b, c, d, (unsigned long long int) ts, pol, x, y);
            //auto event = sepia::dvs_event{ts, x, y, static_cast<bool>(pol)};

            //write event to internal bitarray that lives in the JNI
            this->_bitarray[x*y] = static_cast<bool>(pol);
        }
    }
}

void EventProcessor::update_shared_bitmap(JNIEnv *pEnv) {
    void *pixels;
    int ret;

    if ((ret = AndroidBitmap_lockPixels(pEnv, this->_bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    for (int x = 0; x < this->_sensor_width; x++) {
        for (int y = 0; y < this->_sensor_height; y++) {
            pixels = (char *) pixels + y * this->_scaleY * this->_bitmap_info.stride;
            auto *line = (char *) pixels;
            if (this->_bitarray[x*y]) {
                line[x * this->_scaleX] = static_cast<char>(0xFF); //white
            } else {
                line[x * this->_scaleX] = 0x00;
            }
        }
    }

    AndroidBitmap_unlockPixels(pEnv, this->_bitmap);
}
