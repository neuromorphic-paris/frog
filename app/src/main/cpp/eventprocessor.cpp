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

void EventProcessor::set_pixel(sepia::dvs_event event, void *pixels) {
    int x = event.x;
    pixels = (char *) pixels + event.y * this->_bitmap_info.stride;
    auto *line = (char *) pixels;
    if (event.is_increase) {
        line[x] = static_cast<char>(0xFF); //white
    } else {
        line[x] = 0x00;
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
                                                                                         this->_bitmap,
                                                                                         &pixels)) <
                                                         0) {
                                                         LOGE("AndroidBitmap_lockPixels() failed ! error=%d",
                                                              ret);
                                                     }

                                                     set_pixel(dvs_event, pixels);

                                                     AndroidBitmap_unlockPixels(threadEnv,
                                                                                this->_bitmap);

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

void EventProcessor::set_camera_data(JNIEnv *env, unsigned char *data, unsigned long size,
                                     bool is_recorded) {
    std::chrono::system_clock::time_point start_method = std::chrono::system_clock::now();

    std::chrono::system_clock::time_point start_coordinates;
    std::chrono::system_clock::time_point start_locking;
    std::chrono::system_clock::time_point start_set_pixel, end_set_pixel;

    std::vector<sepia::dvs_event> all_events;
    all_events.reserve(size / 4);

    start_coordinates = std::chrono::system_clock::now();
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
                __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor",
                                    "Mirroring for y-axis does not work for y=%d", y);
            }
            auto x = (unsigned short) ((b & 0xffu) | ((c & 0x01u) << 8u));
            uint64_t ts = this->_baseTime + (((c & 0xffu) >> 1u) & 0x7fu) | ((d & 0x0fu) << 7u);
            //__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "index=%u raw=%02X%02X%02X%02X, ts=%llu, pol=%u, x=%03u, y=%03u", i, a, b, c, d, (unsigned long long int) ts, pol, x, y);
            auto event = sepia::dvs_event{ts, x, y, static_cast<bool>(pol)};
            all_events.push_back(event);
            if (is_recorded) {
                this->_fifo.push(event);
            }
        }
    }
    std::chrono::duration<double, std::milli> time_coordinates =
            std::chrono::system_clock::now() - start_coordinates;

    void *pixels;
    int ret;

    start_locking = std::chrono::system_clock::now();
    if ((ret = AndroidBitmap_lockPixels(env, this->_bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    start_set_pixel = std::chrono::system_clock::now();
    for (auto event : all_events) {
        set_pixel(event, pixels);
        this->_event_counter++;
    }
    end_set_pixel = std::chrono::system_clock::now();
    AndroidBitmap_unlockPixels(env, this->_bitmap);

    std::chrono::duration<double, std::milli> time_locking = (std::chrono::system_clock::now() -
                                                              start_locking);
    std::chrono::duration<double, std::milli> time_set_pixel = (end_set_pixel - start_set_pixel);
    std::chrono::duration<double, std::milli> end = std::chrono::system_clock::now() - start_method;
    //LOGD("Total parsing time for %lu events %fms, setting the pixel %fms and locking overall %fms. Method execution time %fms", size, time_coordinates.count(), time_set_pixel.count(), time_locking.count(), end.count());
}