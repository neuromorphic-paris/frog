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

class EventProcessor {
public:
    EventProcessor() = default;

    AndroidBitmapInfo _bitmap_info{0};

    void save_bitmap_info(JNIEnv *env, jobject *bitmap);

    void trigger_sepia(JNIEnv *env, std::string filepath, jobject *bitmap);

    void reset_bitmap(JNIEnv *pEnv, jobject *pJobject);
};

#endif //FROG_EVENTPROCESSOR_H
