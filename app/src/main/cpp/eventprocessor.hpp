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
    EventProcessor() {}

    void triggerSepia(std::string filepath);
    void setBitmap(jobject bitmap);
    void renderBitmapView(JNIEnv *env);

private:
    jobject jbitmap;

};

#endif //FROG_EVENTPROCESSOR_H
