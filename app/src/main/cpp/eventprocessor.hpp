//
// Created by gregorlenz on 26/11/18.
//

#ifndef FROG_EVENTPROCESSOR_H
#define FROG_EVENTPROCESSOR_H

#include <android/bitmap.h>
#include <android/log.h>
#include <string>
#include <iostream>
#include <cstdint>
#include <chrono>
#include "sepia/source/sepia.hpp"

class EventProcessor {
public:
    EventProcessor() = default;

    jobject _bitmap = nullptr;

    AndroidBitmapInfo _bitmap_info{0};

    void save_bitmap_info(JNIEnv *env);

    void trigger_sepia(JNIEnv *env, std::string filepath);

    void update_shared_bitmap(JNIEnv *pEnv);

    void reset_bitmap(JNIEnv *pEnv);

    void set_camera_data(JNIEnv *env, unsigned char *data, unsigned long size);

private:
    uint16_t _sensor_width = 304;
    uint16_t _sensor_height = 240;

    bool _bitarray[304 * 240];

    uint64_t _baseTime = 0;

    int _scaleX = 3; //_baseTime / _sensor_height; //1; //static_cast<int>(info.width/320);
    int _scaleY = 3; //static_cast<int>(info.height/240);
};

#endif //FROG_EVENTPROCESSOR_H
