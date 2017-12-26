#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <android/log.h>
// #include <iterator>

// This is an implementation of the gesture recognition pipeline
// using an ATIS as input. The output is an integer, that corresponds to
// the class of the gesture.

// -----------------------------------------------------------------------------
// INPUT PARAMETERS
#define INPUT_X 304     // Maximum value of x coordinate
#define INPUT_Y 240     // Maximum value of y coordinate
// LAYERS PARAMETERS
#define LAYER1_CEN 8    // Number of centers for layer 1
#define LAYER_TAU 30000.0 // Time Constant (µs) for all layers
#define LAYER_RAD 2     // Halfwidth for all layers (radius == 2 * LAYER_RAD +1)
#define N_ITEMS (2 * LAYER_RAD + 1) * (2 * LAYER_RAD + 1)
#define LAYER1_PROTO_PATH "models/1LAYER/ald1ln8.prototypes" // path for layer 1 prototypes
#define MIN_POWER 4.0 // 2 * LAYER_RAD
// GESTURE PARAMETERS
#define GESTURE_N 7     // Number of class (number of different gestures)
#define GESTURE_KNOWN_SAMPLES 7 // number of known labelled samples
#define GESTURE_SIG_PATH "models/1LAYER/ald1ln8.signatures" // path to the signatures file
// NOISE FILTER PARAMETERS
#define NOISE_FILTER_TIME 10000.0
#define NOISE_MIN_NEIGH 3
#define NOISE_RAD_NEIGH 1

// -----------------------------------------------------------------------------
// This is a dummy event structure - needs to be adapted for android
// Depends on how the event is passed to the EventProcessor
typedef struct event {
  uint64_t ts;  // timestamp
  uint16_t x;   // x coordinate
  uint16_t y;   // y coordinate
  uint8_t pol;  // polarity
} __event;

// -----------------------------------------------------------------------------
// The EventProcessor gets events as input (via processEvent()) and stores
// their output polarities internally. Once the whole gesture has been
// processed, the output vector can be passed to the recognition part.
class EventProcessor {
public:

	// Constructor
	EventProcessor() { }

	void init(std::string l1ProtoPath, std::string l2ProtoPath, std::string gestureSigPath);

	void setCameraData(unsigned char *data, unsigned long size);

  // Get an event as input, and store its output polarity internally
  // This will process each event incoming from the camera
  void processEvent(__event _event);

  // Returns the output polarity vector,
  // used to pass the output vector to the recognition part
  std::vector<double> getOutputPolVector();

  // Predict the current gesture class
  int predict();

  // Flush the input and output memories of the EventProcessor.
  // Must be called after each gesture sample
  void flush();

	void cleanUp(void);

private:
	// we need an event struct here because the timestamp is cumulative
    __event event;
    uint64_t base_time;
  // event memory (input surface) size 304 * 240
  std::vector<uint64_t> inputMemoryL1;
  // denoise memory filter
  std::vector<uint64_t> denoiseMemory;

  // output polarities memory (size N+1 polarities)
  std::vector<double> outputMemory;

  // prototypes memory
  std::vector<std::vector<float>> prototypesL1;
  std::vector<std::vector<float>> prototypesL2;
  // class signatures memory
  std::vector<std::vector<float>> classSignatures;

  // to load prototypes from external text/binary file
  std::vector<float> loadPrototypesFromFile(std::string _path, int _layerID);
  std::vector<float> loadClassSignatures(std::string _path);

  // called by flush(), flush corresponding memory for next gesture
  void flushInputs();
  void flushOutput();

};
