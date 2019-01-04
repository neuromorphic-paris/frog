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
    void* _storedBitmapPixels;
    AndroidBitmapInfo _bitmapInfo;

    EventProcessor() {
        _storedBitmapPixels = NULL;
    }

    void triggerSepia(std::string filepath);
    void renderBitmapView(JNIEnv *env);
};

#endif //FROG_EVENTPROCESSOR_H
