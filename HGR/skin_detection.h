#pragma once

#include "stdafx.h"

using namespace cv;

class skin_detection
{
public: 
	vector<Mat> HSV_split;   //skladowe H S V
	Mat HSV_frame; // klatka po konwersji z BRG do HSV

private:
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame;

public:
	void toHSV(Mat Frame);
	MatND calc_2D_hist(const Mat& img, const Mat& mask, Scalar wchannels, Scalar bins, Scalar low, Scalar high);
	MatND calc_rg_hist (const Mat& img, const Mat& mask, const Scalar& bins = Scalar(250, 250), const Scalar& low = Scalar(0, 0), const Scalar& high = Scalar(1, 1));
	Mat get_bootstrap();
	void calc_histogram(Mat mask);
	skin_detection();
	~skin_detection(void);

};

