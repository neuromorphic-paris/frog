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

    uint64_t _baseTime = 0;

    jobject _bitmap = nullptr;

    AndroidBitmapInfo _bitmap_info{0};
    const int _scaleX = 1; //static_cast<int>(info.width/320);
    const int _scaleY = 1; //static_cast<int>(info.height/240);

    void save_bitmap_info(JNIEnv *env);

    void trigger_sepia(JNIEnv *env, std::string filepath);

    void reset_bitmap(JNIEnv *pEnv);

    void set_pixel(sepia::dvs_event, void *pixels);

    void set_camera_data(JNIEnv *env, unsigned char *data, unsigned long size);
};

#endif //FROG_EVENTPROCESSOR_H
