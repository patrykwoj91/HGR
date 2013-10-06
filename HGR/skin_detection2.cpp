#include "StdAfx.h"
#include "skin_detection2.h"


skin_detection2::skin_detection2()
{
}

skin_detection2::~skin_detection2(void)
{
}

Mat skin_detection2::toHSV(IplImage *image)
{
	HSV_frame = cvCreateImage(cvSize(image->width, image->height), 8, 3);
	cvCvtColor(image, HSV_frame, CV_BGR2HSV);
	return HSV_frame;
}

void skin_detection2::Detekcja(IplImage *obrazHSV, double progLambda, unsigned int progKanaluV, float *wektorMs, float *macierzKowariancji) {
	maska = cvCreateImage(cvSize(obrazHSV->width, obrazHSV->height), 8, 1);
	// macierze
	CvMat* Cs;
	CvMat* CsInv;
	CvMat* ms;
	// przygotowujemy macierze
	Cs = cvCreateMat(2, 2, CV_32FC1);
	cvInitMatHeader(Cs, 2, 2, CV_32FC1, macierzKowariancji);
	CsInv = cvCreateMat(2, 2, CV_32FC1);
	cvInvert(Cs, CsInv);
	ms = cvCreateMat(1, 2, CV_32FC1);
	cvInitMatHeader(ms, 1, 2, CV_32FC1, wektorMs);
	// liczymy
	int height = obrazHSV->height;
	int width = obrazHSV->width;
	int step = obrazHSV->widthStep;
	int channels = obrazHSV->nChannels;
	CvMat* X = cvCreateMat(1, 2, CV_32FC1);
	CvMat* Xms = cvCreateMat(1, 2, CV_32FC1);
	CvMat* XmsT = cvCreateMat(2, 1, CV_32FC1);
	CvMat* XmsCsInv = cvCreateMat(1, 2, CV_32FC1);
	CvMat* XmsCsInvXmsT = cvCreateMat(1, 1, CV_32FC1);
	uchar* data = (uchar *)obrazHSV->imageData;
	int stepMask = maska->widthStep;
	uchar* dataMask = (uchar *)maska->imageData;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {
			cvmSet(X, 0, 0, (double)data[i*step+j*channels+0]);
			cvmSet(X, 0, 1, (double)data[i*step+j*channels+1]);
			cvSub(X, ms, Xms);
			cvTranspose(Xms, XmsT);
			cvMatMul(Xms, CsInv, XmsCsInv);
			cvMatMul(XmsCsInv, XmsT, XmsCsInvXmsT);
			// sprawdzamy przynaleznosc
			double lambda = cvmGet(XmsCsInvXmsT, 0, 0);
			dataMask[i*stepMask+j] = ((data[i*step+j*channels+2] >= progKanaluV) && (lambda < progLambda)) ? 255 : 0;
		}
	cvReleaseMat(&XmsCsInvXmsT);
	cvReleaseMat(&XmsCsInv);
	cvReleaseMat(&XmsT);
	cvReleaseMat(&Xms);
	cvReleaseMat(&X);
	// zwalniamy macierze
	cvReleaseMat(&ms);
	cvReleaseMat(&CsInv);
	cvReleaseMat(&Cs);
}

Mat skin_detection2::detect_skin(Mat raw_frame)
{
	 raw_frame.copyTo(this->frame);
					
	IplImage* photo = new IplImage(cv::imread("rgb.jpg"));

	 toHSV(photo);
	 cvShowImage("HSV",HSV_frame);
	
	  float ms[] = { 114.8755,   56.5551 };
	  float macierz[] = {
			0.1792 * 1000,   -0.1985 * 1000,
			-0.1985 * 1000,    2.8325 * 1000};
	 Detekcja(HSV_frame, 2, 40, ms, macierz);
	 cvShowImage("Maska",maska);
	 return mask;
}