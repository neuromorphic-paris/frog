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

// INPUT DATA PARAMETERS
#define INPUT_X 304     // Maximum value of x coordinate
#define INPUT_Y 240     // Maximum value of y coordinate
// LAYERS PARAMETERS
#define LAYER1_CEN 8    // Number of centers for layer 1
#define LAYER_TAU 30000.0 // Time Constant (µs) for all layers
#define LAYER_RAD 2     // Halfwidth for all layers (radius == 2 * LAYER_RAD +1)
#define N_ITEMS (2 * LAYER_RAD + 1) * (2 * LAYER_RAD + 1)
#define MIN_POWER 4.0 // 2 * LAYER_RAD
// GESTURE DATASET PARAMETERS
#define GESTURE_LABELS 6     // Number of class (number of different gestures)
#define GESTURE_EXAMPLES_FULL 1621  // number of examples to load
#define GESTURE_EXAMPLES_SIMPLE 1103  // number of examples to load
// NOISE FILTER PARAMETERS
#define NOISE_FILTER_TIME 10000
#define NOISE_MIN_NEIGH 5
#define NOISE_RAD_NEIGH 2
// REFRACTORY PERIOD PARAMETERS
#define REFRAC_TIME 20000  // refractory time in microsec
// BACKGROUND REMOVER PARAMETERS
#define GRID_X 13
#define GRID_Y 10
#define GRID_INPUT_X 304.0     // Maximum value of x coordinate (float)
#define GRID_INPUT_Y 240.0     // Maximum value of y coordinate (float)
#define GRID_TAU 300.0 // time constant for background decay in microsec
#define GRID_MIN_THRESHOLD 4.0
#define GRID_THRES_FACTOR 1.5
// K-NN PARAMETERS
#define K_NEIGH_FULL 11
#define K_NEIGH_SIMPLE 7

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

    void set_camera_data(JNIEnv *env, unsigned char *data, unsigned long size, bool isRecorded);

    void gesture_init(std::string stdL1ProtoPath, std::string stdGestureSigPath, bool denoise, bool bgDenoise, bool refrac, int gest_mode);

    void initAndroidKeycodes(); // defines androidKeyCodes for each gesture label

    // Get an event as input, and store its output polarity internally
    // This will process each event incoming from the camera
    void processEvent(uint64_t ts, uint16_t x, uint16_t y);

    // Returns the output polarity vector,
    // used to pass the output vector to the recognition part
    std::vector<float> getOutputPolVector();

    // Predict the current gesture class
    void predict(char *cpredict);

    // Flush the input and output memories of the EventProcessor.
    // Must be called after each gesture sample
    void flush();

    std::vector<float> getKnnProbaVector();

private:
    // we need an event struct here because the timestamp is cumulative
    // event memory (input surface) size 304 * 240
    std::vector<uint64_t> inputMemoryL1;
    // denoise memory filter
    std::vector<uint64_t> denoiseMemory;
    // background remover
    std::vector<float> backgroundMemory;
    std::vector<float> backgroundLUT;
    std::vector<uint64_t> backgroundLastUpdate;
    int decay_out = static_cast<int>(3 * GRID_TAU - 1);
    bool doRemoveBackground{};
    // refractory period
    std::vector<uint64_t> refractoryMemory;
    bool doRefractory{};
    bool _doDenoise;

    // output polarities memory (size N+1 polarities)
    std::vector<float> outputMemory;

    // save computed signature in this vector (for learning)
    std::vector<float> saved_signature;


    // prototypes memory
    std::vector<std::vector<float>> prototypesL1;
    // class signatures & labels vectors (loaded from text files)
    std::vector<std::vector<float>> classSignatures;
    std::vector<int> classLabels;

    // vectors for kNN
    std::vector<float> distancesKnn;
    std::vector<int> closestLabelsKnn;
    // android keycodes
    std::vector<int> androidKeyCodes;
    // knn functions
    int getWinningLabel();
    void runKnn();

    // to load prototypes from external text/binary file
    std::vector<float> loadPrototypesFromFile(std::string _path);
    std::vector<float> loadClassSignatures(std::string _path);

    // called by flush(), flush corresponding memory for next gesture
    void flushInputs();
    void flushOutput();

    uint64_t pastDenoiseFilter = 0;
    uint64_t pastBackgroundRemover = 0;
    uint64_t pastRefrac = 0;

    // switch between full and simple navigation mode
    int gesture_mode = 6; // default mode is full mode
    int GESTUREEXEMPLES = 0;
    int KNEIGH = 0;
};

#endif //FROG_EVENTPROCESSOR_H
