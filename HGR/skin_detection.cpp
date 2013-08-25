#include "StdAfx.h"
#include "skin_detection.h"


skin_detection::skin_detection()
{
}


skin_detection::~skin_detection(void)
{
}

void skin_detection::toHSV(Mat frame)
{
	frame.copyTo(this->frame); // kopiuj klatke
	cvtColor(this->frame,HSV_frame,COLOR_BGR2HSV); //konwersja przestrzeni barwnej
	split(HSV_frame, HSV_split); // rozdziel na kanaly 
}

Mat skin_detection::get_bootstrap()
{
	Mat bootstrap;
	//get HSV
	Mat HSV_frame;
	this->HSV_frame.copyTo(HSV_frame);
	//Normalize RGB 

	nRGB_frame = NormalizeRGB(this->frame);

	//take the pixels that are inside the ranges in both colorspaces to create masks
	Mat HSV_mask, nRGB_mask;
	inRange(HSV_frame, Scalar(0,0.2*255.0,0.35*255.0), Scalar(50.0/2.0,0.68*255.0,1.0*255.0), HSV_mask); //wartosci przedzialu znalezione w internecie
	inRange(nRGB_frame, Scalar(0,0.28,0.36), Scalar(1.0,0.363,0.465), nRGB_mask); //j.w

	//combine the masks
	bootstrap = HSV_mask & nRGB_mask;
	return bootstrap;
}

void skin_detection::calc_histogram(Mat mask) 
{
	MatND skin_Histogram;
	MatND non_skin_Histogram;

	skin_Histogram = calc_rg_hist(nRGB_frame,mask);
	non_skin_Histogram = calc_rg_hist(nRGB_frame,~mask);

	//create a probabilty density function
	float skin_pixels = countNonZero(mask), non_skin_pixels = countNonZero(~mask);
	for (int ubin=0; ubin < 250; ubin++) {
		for (int vbin = 0; vbin < 250; vbin++) {
			if (skin_Histogram.at<float>(ubin,vbin) > 0) {
				skin_Histogram.at<float>(ubin,vbin) /= skin_pixels;
			}
			if (non_skin_Histogram.at<float>(ubin,vbin) > 0) {
				non_skin_Histogram.at<float>(ubin,vbin) /= non_skin_pixels;
			}
		}
	}
}

MatND skin_detection::calc_rg_hist(const Mat& img, const Mat& mask, const Scalar& bins, const Scalar& low, const Scalar& high) 
{
	Scalar channels(1, 2);
	return this->calc_2D_hist(img,mask,channels,bins,low,high);		
}

MatND skin_detection::calc_2D_hist(const Mat& img, const Mat& mask, Scalar wchannels, Scalar bins, Scalar low, Scalar high) 
{
	MatND hist;
	int histSize[] = { bins[0], bins[1] };
	float uranges[] = { low[0], high[0] };
	float vranges[] = { low[1], high[1] };
	const float *ranges[] = { uranges, vranges };
	int channels[] = {wchannels[0], wchannels[1]};

	calcHist( &img, 1, channels, mask, hist, 2, histSize, ranges, true, false );

	return hist;
}