#pragma once

#include "stdafx.h"

using namespace cv;

class skin_detection2
{
public: 
	
	IplImage *HSV_frame; // klatka po konwersji z BRG do HSV
	Mat mask; //uzyskana maska

private:
	vector<Mat> HSV_split;   //skladowe H S V
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame; //bierzaca po normalizacji;
	IplImage *maska;
	//MatND skin_Histogram; //histogram na pixeli uznanych za skore
	//MatND non_skin_Histogram; //histogram dla pixeli nie uznanych..

	
public:
	Mat detect_skin(Mat raw_frame);
	skin_detection2();
	~skin_detection2(void);
private:
	Mat toHSV(IplImage *image);
	void Detekcja(IplImage *obrazHSV, double progLambda, unsigned int progKanaluV, float *wektorMs, float *macierzKowariancji);
};

