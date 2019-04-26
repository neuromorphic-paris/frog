#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <cmath>
// #include <android/log.h>
// #include <iterator>

// This is an implementation of the gesture recognition pipeline
// using an ATIS as input. The output is an integer, that corresponds to
// the class of the gesture.

// -----------------------------------------------------------------------------
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
// // -----------------------------------------------------------------------------
// // This is a dummy event structure - needs to be adapted for android
// // Depends on how the event is passed to the EventProcessor
// typedef struct event {
//   uint64_t ts;  // timestamp
//   uint16_t x;   // x coordinate
//   uint16_t y;   // y coordinate
//   uint8_t pol;  // polarity
// } __event;

// -----------------------------------------------------------------------------
// The EventProcessor gets events as input (via processEvent()) and stores
// their output polarities internally. Once the whole gesture has been
// processed, the output vector can be passed to the recognition part.
class EventProcessor {
public:

  // Constructor
  uint64_t baseTime = 0;

  EventProcessor() { }

  void init(std::string l1ProtoPath, std::string l2ProtoPath, std::string gestureSigPath, bool denoise, bool bg_denoise, bool refrac, int gest_mode);

  void setCameraData(unsigned char *data, unsigned long size);
  void initAndroidKeycodes(); // defines androidKeyCodes for each gesture label

  // Get an event as input, and store its output polarity internally
  // This will process each event incoming from the camera
  void processEvent(uint64_t ts, uint16_t x, uint16_t y);

  // Returns the output polarity vector,
  // used to pass the output vector to the recognition part
  std::vector<float> getOutputPolVector();

  // Predict the current gesture class
  void predict(char *cpredict);
  std::vector<float> predict(); // for OFFLINE C++ main.cpp

  // Flush the input and output memories of the EventProcessor.
  // Must be called after each gesture sample
  void flush();

  uint64_t getPastDenoiseFilter();
  uint64_t getPastBackgroundRemover();
  uint64_t getPastRefrac();

  void setDoDenoise(bool b);

  void setDoBackgroundRemoval(bool b);

  void setDoRefractoryPeriod(bool b);

  std::vector<float> getSaved_signature();

  std::vector<float> getKnnProbaVector();

private:
	// we need an event struct here because the timestamp is cumulative
  // event memory (input surface) size 304 * 240
  std::vector<uint64_t> inputMemoryL1;
  // denoise memory filter
  std::vector<uint64_t> denoiseMemory;
  bool doDenoise;
  // background remover
  std::vector<float> backgroundMemory;
  std::vector<float> backgroundLUT;
  std::vector<uint64_t> backgroundLastUpdate;
  int decay_out = (int) 3 * GRID_TAU - 1;
  bool doRemoveBackground;
  // refractory period
  std::vector<uint64_t> refractoryMemory;
  bool doRefractory;

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
