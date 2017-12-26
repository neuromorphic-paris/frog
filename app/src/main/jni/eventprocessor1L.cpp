#include "eventprocessor1L.hpp"

// #############################################################################

void EventProcessor::init(std::string l1ProtoPath, std::string l2ProtoPath, std::string gestureSigPath)
{
}

// #############################################################################

std::vector<float> EventProcessor::loadPrototypesFromFile(std::string _path, int _layerID)
{
}

// #############################################################################

std::vector<float> EventProcessor::loadClassSignatures(std::string _path)
{
}

// #############################################################################

void EventProcessor::setCameraData(unsigned char *data, unsigned long size)
{
}

// #############################################################################

void EventProcessor::processEvent(__event _event)
{
}

// #############################################################################

int EventProcessor::predict()
{
}

// #############################################################################

std::vector<double> EventProcessor::getOutputPolVector()
{
}

// #############################################################################

void EventProcessor::flush()
{
}

// #############################################################################

void EventProcessor::flushInputs()
{
}

// #############################################################################

void EventProcessor::flushOutput()
{
}

// #############################################################################
void EventProcessor :: cleanUp (void) {

}