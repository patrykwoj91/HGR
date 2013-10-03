#pragma once

#include "stdafx.h"

using namespace cv;

class skin_detection
{
public: 
	
	Mat HSV_frame; // klatka po konwersji z BRG do HSV
	Mat mask; //maska stworzona na podstawie bootstrapu i histogramow dla obszarow skin i non skin

private:
	vector<Mat> HSV_split;   //skladowe H S V
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame; //bierzaca po normalizacji;
	MatND skin_Histogram; //histogram na pixeli uznanych za skore
	MatND non_skin_Histogram; //histogram dla pixeli nie uznanych..

	float theta_thresh;
    Scalar hist_bins;
    Scalar low_range;
    Scalar high_range;
    Scalar range_dist;
public:
	Mat toHSV(Mat frame);
	Mat get_bootstrap();
	void calc_hist();
	Mat train();
	skin_detection();
	~skin_detection(void);
	void set_frame(Mat frame) { frame.copyTo(this->frame);}
};

