#pragma once
#include "StdAfx.h"
class background_subtraction
{
public:
	background_subtraction(void);
	~background_subtraction(void);
	IplImage* subtract_background(IplImage *rawImage, int nframes);
	int setup(IplImage *rawImage); // Initial commands for setup processing
	IplImage* simple_substract(IplImage *rawImage);

	CvBGCodeBookModel* model;
	static const int NCHANNELS = 3;
	bool ch[NCHANNELS]; // This sets what channels should be adjusted for background bounds
	int class1;
	char filename1[1000];
	int cc;
	int nframesToLearnBG;
	IplImage *yuvImage; //yuvImage is for codebook method
    IplImage *ImaskCodeBook;
	IplImage *ImaskCodeBookCC;

private:
};

