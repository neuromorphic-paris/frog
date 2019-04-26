#include "eventprocessor1L.hpp"

// #############################################################################

void EventProcessor::init(std::string l1ProtoPath, std::string l2ProtoPath, std::string gestureSigPath, bool denoise, bool bg_denoise, bool refrac, int gest_mode)
{
  // The constructor sets all vectors,
  // loads prototypes for both layer 1 from the corresponding text file
  // loads known class signature from the signature text file.

//  __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor init L1 ", "%s", l1ProtoPath.c_str());
//  __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor init L2 ", "%s", l2ProtoPath.c_str());
//  __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor init SIG ", "%s", gestureSigPath.c_str());

  // gesture mode (simple or full ?)
  this->gesture_mode = gest_mode;
  if (this->gesture_mode == 4) { // simple mode
    this->KNEIGH = K_NEIGH_SIMPLE;
    this->GESTUREEXEMPLES = GESTURE_EXAMPLES_SIMPLE;
  }
  if (this->gesture_mode == 6) { // full mode
    this->KNEIGH = K_NEIGH_FULL;
    this->GESTUREEXEMPLES = GESTURE_EXAMPLES_FULL;
  }

  // init android keycodes
  this->initAndroidKeycodes();

  // resize input/output/denoise/knn memories/vectors
  this->inputMemoryL1.resize(INPUT_X * INPUT_Y);

  this->denoiseMemory.resize(INPUT_X * INPUT_Y);
  std::fill(denoiseMemory.begin(), denoiseMemory.end(), -NOISE_FILTER_TIME - 1);
  this->doDenoise = denoise;

  this->outputMemory.resize(LAYER1_CEN);
  this->distancesKnn.resize(this->KNEIGH);
  std::fill(this->distancesKnn.begin(), this->distancesKnn.end(), std::numeric_limits<float>::infinity());
  this->closestLabelsKnn.resize(this->KNEIGH);

  // refractory period
  this->refractoryMemory.resize(INPUT_X * INPUT_Y);
  this->doRefractory = refrac;
  // background arrays
  this->doRemoveBackground = bg_denoise;
  this->backgroundMemory.resize(GRID_X * GRID_Y);
  this->backgroundLastUpdate.resize(GRID_X * GRID_Y);
  this->backgroundLUT.resize(3 * GRID_TAU);
  float t = 0.0;
  for (auto & l : this->backgroundLUT) {
    l = std::exp(t / GRID_TAU);
    t = t - 1;
  }
  this->backgroundLUT.at(3 * GRID_TAU - 1) = 0.0;

  // ------------------------------------
  // resize prototypes arrays
  this->prototypesL1.resize(LAYER1_CEN);
  for(auto & p: this->prototypesL1) {
    p.reserve(N_ITEMS);
  }

  // ------------------------------------
  // std::cout << "loading_small prototypes" << std::endl;
  // load prototypes values from file...
  // ... for layer 1...
  std::vector<float> loadedValues = this->loadPrototypesFromFile(l1ProtoPath);
  int i = 0;  // silent variable
  int j = 0;  // current prototype ID
  for(auto & value: loadedValues) {
    prototypesL1[j].push_back(value);
    i++;
    if (i == N_ITEMS) {
      // std::cout << i << std::endl;
      i = 0;
      j++;
    }
  }
  std::cout << "Prototypes loaded." << std::endl;
  // // START DEBUG
  // for (auto & pv: prototypesL1) { // DEBUG
  //   std::cout << "Protoype:" <<std::endl;
  //   for (auto & p: pv) {
  //     std::cout << p << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // // END DEBUG

  // ------------------------------------
  // std::cout << "loading_small signatures" << std::endl;
  // loading_small class signatures
  // resize class signature array
  this->classSignatures.resize(this->GESTUREEXEMPLES);
  for(auto & c: this->classSignatures) {c.reserve(LAYER1_CEN);}
  this->classLabels.reserve(this->GESTUREEXEMPLES);

  // load known signatures from file
  std::vector<float> loadedSignatures = this->loadClassSignatures(gestureSigPath);

  i = 0;  // silent variable
  j = 0;  // gesture example ID
  bool nextIsLabel = false;
  for(auto & value: loadedSignatures) {
    if (nextIsLabel) {
      classLabels.push_back(value);
      nextIsLabel = false;
    } else {
      classSignatures[j].push_back(value);
      i++;
      if (i == LAYER1_CEN) {
        i = 0;
        j++;
        nextIsLabel = true;
      }
    }
  }
  std::cout << "Signatures loaded." << std::endl;
  // // START DEBUG
  // i = 0;
  // for (auto & pv: classSignatures) { // DEBUG
  //   std::cout << "Sig:" <<std::endl;
  //   for (auto & p: pv) {
  //     std::cout << p << " ";
  //   }
  //   std::cout << "label is: " << classLabels[i] << std::endl;
  //   i++;
  // }
  // // END DEBUG
}

// #############################################################################
void EventProcessor::setDoDenoise(bool b)
{
  if (b) {
    std::cout << "Denoiser enabled." << std::endl;
  } else { std::cout << "Denoiser disabled." << std::endl;}
  this->doDenoise = b;
}

// #############################################################################
void EventProcessor::setDoBackgroundRemoval(bool b)
{
  if (b) {
    std::cout << "Background remover enabled." << std::endl;
  } else { std::cout << "Background remover disabled." << std::endl;}
  this->doRemoveBackground = b;
}
// #############################################################################
void EventProcessor::setDoRefractoryPeriod(bool b)
{
  if (b) {
    std::cout << "Refractory period enabled." << std::endl;
  } else { std::cout << "Refractory period disabled." << std::endl;}
  this->doRefractory = b;
}
// #############################################################################

void EventProcessor::initAndroidKeycodes()
{
  // BEWARE : DO NOT CHANGE THE push_back ORDER !

  // The navigation codes used by Android
  // the return code has to be one of these
  // KEYCODE_DPAD_UP = 19
  // KEYCODE_DPAD_DOWN = 20
  // KEYCODE_DPAD_LEFT = 21
  // KEYCODE_DPAD_RIGHT = 22
  // KEYCODE_ENTER = 66
  // KEYCODE_B (BACK) = 30
  // KEYCODE_H (HOME) = 36
  // KEYCODE_E (ERROR) = 33
  // KEYCODE_UNKNOWN (IGNORE) = 0

  this->androidKeyCodes.reserve(GESTURE_LABELS); // Number of gestures
  this->androidKeyCodes.push_back(0); // IGNORE / ERROR
  this->androidKeyCodes.push_back(19); // DOWN
  this->androidKeyCodes.push_back(36); // HOME
  this->androidKeyCodes.push_back(22); // LEFT
  this->androidKeyCodes.push_back(21); // RIGHT
  this->androidKeyCodes.push_back(66); // SELECT
  this->androidKeyCodes.push_back(20); // UP
}

// #############################################################################


std::vector<float> EventProcessor::loadPrototypesFromFile(std::string _path)
{
  std::vector<float> v;
  v.reserve(N_ITEMS * LAYER1_CEN);

  // open file (_path)
  std::ifstream ifs(_path.c_str(), std::ifstream::in);

  if(ifs.good() == false) {
    std::cout << "error: while trying to open " << _path.c_str() << ": failed" << std::endl;
    throw;
  }

  std::string line;
  while ( getline (ifs,line) ) {

    std::stringstream linestr(line);
    char c;
    linestr >> c;
    if ( (!linestr) || (c == '?') || (c == '%')) {
      continue; // header line
    }

    std::stringstream linestrtmp(line);
    float r;
    for (int ii = 0; ii < N_ITEMS; ++ii) {
      linestrtmp >> r;
      v.push_back(r); // fill vector
//      __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor ", "r=%.7f", r);
    }
//	  __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "END OF PROTO");
  }
//	__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "vsize=%lu", v.size());
	// std::cout << "v size " << v.size() << std::endl; // DEBUG
  return v;
}

// #############################################################################

std::vector<float> EventProcessor::loadClassSignatures(std::string _path)
{
  std::vector<float> v;
  v.reserve(this->GESTUREEXEMPLES * (LAYER1_CEN + 1)); // signature is LAYER1_CEN long, + 1 for the label

  // open file (_path)
  std::ifstream ifs(_path.c_str(), std::ifstream::in);

  if(ifs.good() == false) {
    std::cout << "error: while trying to open " << _path.c_str() << ": failed" << std::endl;
    throw;
  }

  std::string line;
  while ( getline (ifs,line) ) {

    std::stringstream linestr(line);
    char c;
    linestr >> c;
    if ( (!linestr) || (c == '?') || (c == '%')) {
      continue; // header line
    }

    std::stringstream linestrtmp(line);
    float r;
    for (int ii = 0; ii < (LAYER1_CEN + 1); ++ii) {
      linestrtmp >> r;
      v.push_back(r); // fill vector
//		__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor ", "r=%.12f", r);
	}
//	  __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "END OF SIG");
  }
//	__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "vsize=%lu", v.size());
  return v;
}

// #############################################################################
void EventProcessor::setCameraData(unsigned char *data, unsigned long size)
{
  for (int i = 0; i < size;) {
    unsigned char a = data[i++];
    unsigned char b = data[i++];
    unsigned char c = data[i++];
    unsigned char d = data[i++];
    //	event.pol currently contains the type of event
	uint8_t pol = (uint8_t) ((d & 0xf0) >> 4);
    if (pol == 8) {
		baseTime = (uint64_t) (((a & 0xff) | ((b & 0xff) << 8) | ((c & 0xff) << 16) | ((d & 0x0f) << 24)) << 11);
//        __android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor ", "index=%u raw=%02X%02X%02X%02X, ts=%llu", i, a, b, c, d, (unsigned long long int) localBaseTime);
    } else if (baseTime != 0) {
		uint16_t y = (uint16_t) (a & 0xff);
		uint16_t x = (uint16_t) ((b & 0xff) | ((c & 0x01) << 8));
		uint64_t ts = baseTime + (((c & 0xff) >> 1) & 0x7f) | ((d & 0x0f) << 7);
//		__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "index=%u raw=%02X%02X%02X%02X, ts=%llu, pol=%u, x=%03u, y=%03u", i, a, b, c, d, (unsigned long long int) ts, pol, x, y);
      processEvent(ts, x, y);
    }
  }
}


// #############################################################################

void EventProcessor::processEvent(uint64_t ts, uint16_t x, uint16_t y)
{
//		__android_log_print(ANDROID_LOG_DEBUG, "C++ EventProcessor", "ts=%llu, x=%03u, y=%03u", (unsigned long long int) ts, x, y);

  // REFRACTORY PERIOD
  if (this->doRefractory) {
    uint64_t dur = ts - this->refractoryMemory.at(x + ( y * INPUT_Y ));
    if (dur < REFRAC_TIME) {return;}
    else {
      this->refractoryMemory.at(x + ( y * INPUT_Y )) = ts;
      this->pastRefrac++;
    }
  }

	// DENOISER
	if (this->doDenoise) {
		// denoising
		int neighboor = 0;
		// store event in denoise memory
		this->denoiseMemory.at(x + ( y * INPUT_Y )) = ts;
		// std::cout << "denoise " << std::endl;
		if ( (x >= NOISE_RAD_NEIGH) && (x <= (INPUT_X - NOISE_RAD_NEIGH)) && (y >= NOISE_RAD_NEIGH) && (y <= (INPUT_Y - NOISE_RAD_NEIGH)) ) {

			for (int i = -NOISE_RAD_NEIGH; i <= NOISE_RAD_NEIGH; ++i) {
				for (int j = -NOISE_RAD_NEIGH; j <= NOISE_RAD_NEIGH; ++j) {

					// std::cout << x + j << " " << y + i << std::endl;
					int coord = (x + j) + ( (y + i) * INPUT_Y );
					// std::cout << "event ts: " << ts << std::endl;

					// std::cout << "dmem " << this->denoiseMemory.at(coord) << std::endl;

					uint64_t deltats =  ts - this->denoiseMemory.at(coord);

					// std::cout << "delta ts : " << deltats << std::endl;

					if ( deltats < NOISE_FILTER_TIME ) {
						neighboor++;
						// std::cout << "h" << neighboor << std::endl;
					}
				}
			}
			// if event is considered to be noise, do not add it to the inputMemoryL1 and wait for next call
			if (neighboor < NOISE_MIN_NEIGH) { return; }
			// else processing can go on
			this->pastDenoiseFilter++;
		}
	} else {
		this->pastDenoiseFilter++;
	}
  // END OF DENOISER

  // BACKGROUND REMOVER
  if (this->doRemoveBackground) {
    // get grid coordinates from event position
    int gx = (int) (x / GRID_INPUT_X * GRID_X);
    int gy = (int) (y / GRID_INPUT_Y * GRID_Y);
    // std::cout << "gx,gy : " << gx << " " << gy << std::endl;
    // apply some decay
    int loc = gx + (GRID_X * gy); // get location in 1D vector
    int dtemp = (int) ts - backgroundLastUpdate[loc]; // delta t between last update and current timestamp
    int s = (int) 3 * GRID_TAU - 1; // max size of the bg LUT
    int dt = std::min(dtemp, s); // don't go over the size of bg LUT
    this->backgroundMemory[loc] *= this->backgroundLUT[dt]; // apply decay
    this->backgroundLastUpdate[loc] = ts; // store ts as new last update
    // handle arbitrer bug (events with x > 254 spike two times!)
    if (x < 255) {this->backgroundMemory[loc] += 1.0;}
    else {this->backgroundMemory[loc] += 0.5;}
    // get threshold that will discriminate between event and bg event
	auto grid_mean = std::accumulate(this->backgroundMemory.begin(), this->backgroundMemory.end(), 0.0) / this->backgroundMemory.size();
    float threshold = std::max(GRID_MIN_THRESHOLD, grid_mean * GRID_THRES_FACTOR);
    // std::cout << "threshold bg: " << threshold << std::endl;
    if ( this->backgroundMemory[loc] < threshold ) { return; } // remove event
    // else event can go on
    this->pastBackgroundRemover++;
  } else { // no bg remover, event can go on
    this->pastBackgroundRemover++;
  }
  // END OF BACKGROUND REMOVER

	// store event in L1 input memory
	this->inputMemoryL1.at(x + ( y * INPUT_Y )) = ts;

	// if the event spatial location isnt to close to the border (ie the TC won't get out of the inputMemory size) we can go on, otherwise no TS computation for this event.
	bool DO_NEXT = false; // the bool that tells us if processing must go on
	if ( (x >= LAYER_RAD) && (x <= (INPUT_X - LAYER_RAD)) && (y >= LAYER_RAD) && (y <= (INPUT_Y - LAYER_RAD)) ) {
		DO_NEXT = true;
	}

	// store TS power
	float TS_power = 0;

	if (DO_NEXT) {

		// LAYER 1 ---------------------------------------------------------------
		std::vector<uint64_t> TC; // to store the time-context of Layer 1
		// extract time context around the event
		TC.reserve(N_ITEMS);
		for (int i = -LAYER_RAD; i <= LAYER_RAD; ++i) {
			for (int j = -LAYER_RAD; j <= LAYER_RAD; ++j) {
				TC.push_back(this->inputMemoryL1.at(x + j + ( (y + i) * INPUT_Y )));
			}
		}
		// compute time surface
		//uint64_t max_ts = *std::max_element(TC.begin(), TC.end()); // get highest
		uint64_t max_ts = ts; // get highest timestamp
		std::vector<float> TS; // time surface of layer 1
		TS.reserve(TC.size());
		for (auto & val: TC) {
			float t = (float) (max_ts - val);
			if (t < LAYER_TAU) { TS.push_back( 1 - (t / LAYER_TAU) ); }
			else { TS.push_back(0.0); }
		}
		TS_power = std::accumulate(TS.begin(), TS.end(), 0.0) ;

		// for (auto & val: TS) { std::cout << val << std::endl; }

		// check that TS is sufficiently populated
		// std::cout << "TSpower: "<< TS_power << std::endl;
		if (TS_power < MIN_POWER) {
			// std::cout << "not enough power (L1)" << std::endl;
			return;
		}
		// std::cout << "go on" << std::endl;

		// compute closest prototype for the first layer
		int closestL1 = -1;
		float min_dist = std::numeric_limits<float>::infinity();
		int curr_p = 0;
		for (auto & p: this->prototypesL1) {
			std::vector<float> result;
			result.reserve(p.size());
			std::transform(p.begin(), p.end(), TS.begin(), std::back_inserter(result), [&](float a, float b) {return (a-b) * (a-b);});
			float dist = std::accumulate(result.begin(), result.end(), 0.0);
			if (dist < min_dist) {
				min_dist = dist;
				closestL1 = curr_p; // closestL1 is polarity for layer 2
			}
			curr_p++;
		}

		// add L1 output polarity to output memory (gesture signature)
		this->outputMemory[closestL1]++;
	}
}

// #############################################################################

void EventProcessor::predict(char *cpredict)
{
  // normalize signature (which is stored in outputMemory)
  float sumO = std::accumulate(this->outputMemory.begin(), this->outputMemory.end(), 0.0);
  for (auto & count: this->outputMemory) {
    count = count / sumO;
  }

  // std::cout << sumO << std::endl;
  // std::cout << "Signature: ";
  // for (auto &count: outputMemory) {std::cout << count << " ";}
  // std::cout << std::endl;

  // ### K-nearest neighboor (K-NN)
  this->runKnn();

  // // ------ START OF OLD VERSION : WINNER TAKES ALL --------
  // // ### majority vote between closestLabels
  // int winningLabel = this->getWinningLabel();
  // // ------ END OF OLD VERSION : WINNER TAKES ALL --------
  std::vector<float> knnProbaVector = this->getKnnProbaVector();

  // save signature into saved_signature
  this->saved_signature.resize(LAYER1_CEN);
  uint i = 0;
  for (auto & val : this->outputMemory) {
    this->saved_signature[i] = val;
    i++;
  }

  // flush all memories for the next gesture...
  this->flush();

  // // ------ START OF OLD VERSION : WINNER TAKES ALL --------
  // // and finally return android keycode value
  // // std::cout << "winningLabel: " << winningLabel << std::endl;
  // if (winningLabel < 0) { return 0; } // ERROR - RETURNS 0
  // if (winningLabel > GESTURE_LABELS) { return 0; } // ERROR - RETURNS 0
  // return this->androidKeyCodes[winningLabel]; // GESTURE KEYCODE
  // // ------ END OF OLD VERSION : WINNER TAKES ALL --------
//	unfortunately to return something JNI can understand I have needed to add this code
	//	Added this for cleanliness Andrew

	baseTime = 0;
	sprintf(cpredict, "%f,%f,%f,%f,%f,%f", knnProbaVector[0], knnProbaVector[1], knnProbaVector[2], knnProbaVector[3], knnProbaVector[4], knnProbaVector[5]);


}

// #############################################################################

// this one is for OFFLINE C++ main.cpp
// please check the one just above for the android version

std::vector<float> EventProcessor::predict()
{
  // normalize signature (which is stored in outputMemory)
  float sumO = std::accumulate(this->outputMemory.begin(), this->outputMemory.end(), 0.0);
  for (auto & count: this->outputMemory) {
    count = count / sumO;
  }

  // std::cout << sumO << std::endl;
  // std::cout << "Signature: ";
  // for (auto &count: outputMemory) {std::cout << count << " ";}
  // std::cout << std::endl;

  // ### K-nearest neighboor (K-NN)
  this->runKnn();

  // // ------ START OF OLD VERSION : WINNER TAKES ALL --------
  // // ### majority vote between closestLabels
  // int winningLabel = this->getWinningLabel();
  // // ------ END OF OLD VERSION : WINNER TAKES ALL --------
  std::vector<float> knnProbaVector = this->getKnnProbaVector();

  // save signature into saved_signature
  this->saved_signature.resize(LAYER1_CEN);
  uint i = 0;
  for (auto & val : this->outputMemory) {
    this->saved_signature[i] = val;
    i++;
  }

  // flush all memories for the next gesture...
  this->flush();

  // // ------ START OF OLD VERSION : WINNER TAKES ALL --------
  // // and finally return android keycode value
  // // std::cout << "winningLabel: " << winningLabel << std::endl;
  // if (winningLabel < 0) { return 0; } // ERROR - RETURNS 0
  // if (winningLabel > GESTURE_LABELS) { return 0; } // ERROR - RETURNS 0
  // return this->androidKeyCodes[winningLabel]; // GESTURE KEYCODE
  // // ------ END OF OLD VERSION : WINNER TAKES ALL --------
  return knnProbaVector;
}


// #############################################################################

void EventProcessor::runKnn()
{
  int curr_sig_index = 0;
  for (auto & sig : this->classSignatures) {
    // for (auto & s : sig) {
    //   std::cout << s << std::endl;
    // }
    //
    // std::cout << "current closest labels: ";
    // for (auto & lab : this->closestLabelsKnn) {
    //   std::cout << lab << " ";
    // }
    // std::cout << std::endl;
    //
    // std::cout << "current sig index: " << curr_sig_index << std::endl;

    // 1) compute distance from sig to sig_to_classify
    std::vector<float> result;
    result.reserve(sig.size());
    std::transform(sig.begin(), sig.end(), this->outputMemory.begin(), std::back_inserter(result), [&](float a, float b) {return (a-b) * (a-b);});
    // distance from current sig is stored in current_sig_dist:
    float current_sig_dist = std::accumulate(result.begin(), result.end(), 0.0);
    // std::cout << "dist: " << current_sig_dist << std::endl;
    // 2) now do we add it to the "closest" list?
    // 2.1) first get the biggest distance value and index
    float biggestDistance = 0.0;
    uint biggestDistanceIndex = 0;
    uint i = 0;
    for (auto & d : this->distancesKnn) {
      if (d > biggestDistance) {
        biggestDistance = d;
        biggestDistanceIndex = i;
      }
      i++;
    }
    // std::cout << "big : " << biggestDistance << std::endl;
    // 2.2) now if the current distance is smaller than the biggestDistance
    // we store it instead of the biggestDistance
    if (current_sig_dist < biggestDistance) {
      // std::cout << "yeah" << std::endl;
      this->distancesKnn[biggestDistanceIndex] = current_sig_dist; // store new distance
      this->closestLabelsKnn[biggestDistanceIndex] = this->classLabels[curr_sig_index]; // store new label
    }
    curr_sig_index++; // increment current signature index
  }
}

// #############################################################################

int EventProcessor::getWinningLabel()
{
  // 1) get labels' max value
  int maxlabel = 0;
  for (auto & l : this->closestLabelsKnn) {
    if (l > maxlabel) {
      maxlabel = l;
    }
  }
  // 2) create histogram
  std::vector<int> labelHistogram;
  labelHistogram.resize(maxlabel+1);
  for (auto & l : this->closestLabelsKnn) {
    labelHistogram[l]++;
  }
  // 3) get max
  int winningLabel = 0;
  int winningValue = 0;
  uint i = 0;
  for (auto & l : labelHistogram) {
    if (l > winningValue) {
      winningValue = l;
      winningLabel = i;
    }
    i++;
  }
  return winningLabel;
}

// #############################################################################

std::vector<float> EventProcessor::getKnnProbaVector()
{
  // Returns a vector of floats
  // each value is between 0 and 1, sum of vector is 1
  // they are the probability of each class (the highest the better)
  // the vector is ordered that way:
  // [DOWN, HOME, LEFT, RIGHT, SELECT, UP]

  std::vector<float> labelProba;
  labelProba.resize(GESTURE_LABELS);
  float w = (1.0 / this->KNEIGH); // weight of one neighboor
  for (auto & l : this->closestLabelsKnn) {
    labelProba[l-1] += w; // l-1 because labels are from 1 to GESTURE_LABELS
  }
  return labelProba;
}

// #############################################################################

std::vector<float> EventProcessor::getOutputPolVector()
{
  return this->outputMemory;
}

// #############################################################################

std::vector<float> EventProcessor::getSaved_signature()
{
  return this->saved_signature;
}

// #############################################################################

void EventProcessor::flush()
{
  this->flushInputs();
  this->flushOutput();
  std::cout << "Flushed arrays for next gesture." << std::endl;
}

// #############################################################################

void EventProcessor::flushInputs()
{
  std::fill(this->inputMemoryL1.begin(), this->inputMemoryL1.end(), 0);
  std::fill(this->denoiseMemory.begin(), this->denoiseMemory.end(), -NOISE_FILTER_TIME - 1000.0);
  std::fill(this->distancesKnn.begin(), this->distancesKnn.end(), std::numeric_limits<float>::infinity());
  std::fill(this->closestLabelsKnn.begin(), this->closestLabelsKnn.end(), -1);
  this->pastDenoiseFilter = 0;

  // flush background remover
  this->pastBackgroundRemover = 0;
  std::fill(this->backgroundMemory.begin(), this->backgroundMemory.end(), 0.0);
  std::fill(this->backgroundLastUpdate.begin(), this->backgroundLastUpdate.end(), 0);
  // flush refractory period
  this->pastRefrac = 0;
  std::fill(this->refractoryMemory.begin(), this->refractoryMemory.end(), 0);
}

// #############################################################################

void EventProcessor::flushOutput()
{
  std::fill(this->outputMemory.begin(), this->outputMemory.end(), 0.0);
}

// #############################################################################

uint64_t EventProcessor::getPastDenoiseFilter()
{
  // returns the number of event that got past the denoise filter
  return this->pastDenoiseFilter;
}

// #############################################################################

uint64_t EventProcessor::getPastRefrac()
{
  // returns the number of event that got past the refractory period
  return this->pastRefrac;
}

// #############################################################################

uint64_t EventProcessor::getPastBackgroundRemover()
{
  // returns the number of event that got past the bg remover
  return this->pastBackgroundRemover;
}
