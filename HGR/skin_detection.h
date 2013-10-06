#pragma once

#include "stdafx.h"

using namespace cv;

class skin_detection
{
public: 
	IplImage *maskImage; //yuvImage is for codebook method

private:
	vector<Mat> HSV_split;   //skladowe H S V
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame; //bierzaca po normalizacji;	

	CvScalar hsv_min; 
	CvScalar hsv_max;
	CvScalar nrgb_min;
	CvScalar nrgb_max;

	IplImage *hsvImage;
	IplImage *nrgbImage;
	IplImage* hsvMask;
	IplImage* nrgbMask;

	float macierz[4];
	float mw[2];

	// macierze
	CvMat* Cs;
	CvMat* CsInv;
	CvMat* ms;

	CvMat* X;
	CvMat* Xms;
	CvMat* XmsT;
	CvMat* XmsCsInv;
	CvMat* XmsCsInvXmsT;

	int height; 
	int width; 
	int step; 
	int channels;

public:
	void detect_skin(IplImage *rawImage);
	void setup(IplImage * rawImage);
	void detection(IplImage *obrazHSV, double progLambda, unsigned int progKanaluV, float *wektorMs, float *macierzKowariancji);
	skin_detection();
	~skin_detection(void);
private:
	void get_mask(IplImage *rawImage);
	IplImage* getNRGB(IplImage* rawImage);
};

