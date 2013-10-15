#pragma once

#include "stdafx.h"


using namespace cv;

class skin_detection
{
public: 
 
	static const int alpha_slider_max = 255;
	int alpha_slider;
	int beta_slider;
	int alpha_slider_2;
	int beta_slider_2;
	int threshLambda;
	int threshValue;

	IplImage *mainMask;
	IplImage* covMaskI;
	IplImage* CrCbMaskI;
	IplImage* probMaskI;
	IplImage *hsvImage;
	IplImage *nrgbImage;
	IplImage* hsvMask;
	IplImage* nrgbMask;

private:
	vector<Mat> HSV_split;   //skladowe H S V
	Mat frame; // kopia bie¿acej klatki
	Mat nRGB_frame; //bierzaca po normalizacji;	

	CvScalar hsv_min; 
	CvScalar hsv_max;
	CvScalar nrgb_min;
	CvScalar nrgb_max;

	float macierz[4];
	float ms[2];

	// macierze
	CvMat* Cs;
	CvMat* CsInv;
	CvMat* Ms;

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
	void setup(IplImage * rawImage);
	IplImage * mask_skin(IplImage *rawImage);
	IplImage* getNRGB(IplImage* rawImage);

	skin_detection();
	~skin_detection(void);

private:
	void probMask(IplImage *rawImage);
	void covMask(IplImage *obrazHSV,  double threshLambda, unsigned int threshV); //wykrywanie na podstawie macierzy kowariacji wartosci srednich i odleglosci ma...
	void CrCbMask(IplImage * rawImage);
};

