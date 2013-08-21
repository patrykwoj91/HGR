#pragma once

#include "stdafx.h"

using namespace cv;

enum Kanal { H = 0, S = 1, V = 2 };

class HSV_Converter
{
public: 
	vector<Mat> HSV_split;   //skladowe H S V
	Mat HSV_frame; // klatka po konwersji z BRG do HSV

private:
	Mat frame; // kopia bie¿acej klatki

public:
	void Convert(Mat Frame);
	HSV_Converter();
	~HSV_Converter(void);
	
};

