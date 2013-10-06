
#pragma once
#include "stdafx.h"
using namespace cv;

class AbstractAlgorithm {
protected:
	bool initialized;

public:
	AbstractAlgorithm() { initialized = false; }
	
	bool isInitialized() { return initialized; }

};