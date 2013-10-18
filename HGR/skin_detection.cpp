#include "StdAfx.h"
#include "skin_detection.h"

skin_detection::skin_detection()
{	
	//range for nrgb and hsv masks // TODO: suwaki do zmiany zakresow, lub lepsze zakresy
	hsv_min = cvScalar(0, 0.2*255.0, 0.35*255.0);
	hsv_max = cvScalar(50.0/2.0, 0.68*255.0, 1.0*255.0);
	nrgb_min = cvScalar(0, 0.28*255.0, 0.36*255.0);
	nrgb_max = cvScalar(1.0, 0.363*255.0, 0.465*255.0);

	ms[0] = 114.8755; // average values matrix
	ms[1] = 56.5551;

	macierz[0] = 0.1792 * 1000; //covariation matrix
	macierz[1] = -0.1985 * 1000;
	macierz[2] = -0.1985 * 1000;
	macierz[3] = 2.8325 * 1000;

	// przygotowujemy macierze
	Cs = cvCreateMat(2, 2, CV_32FC1);
	cvInitMatHeader(Cs, 2, 2, CV_32FC1, macierz);
	CsInv = cvCreateMat(2, 2, CV_32FC1);
	cvInvert(Cs, CsInv);
	Ms = cvCreateMat(1, 2, CV_32FC1);
	cvInitMatHeader(Ms, 1, 2, CV_32FC1, ms);

	X = cvCreateMat(1, 2, CV_32FC1);
	Xms = cvCreateMat(1, 2, CV_32FC1);
	XmsT = cvCreateMat(2, 1, CV_32FC1);
	XmsCsInv = cvCreateMat(1, 2, CV_32FC1);
	XmsCsInvXmsT = cvCreateMat(1, 1, CV_32FC1);

	alpha_slider = 128;
	beta_slider = 164;
	alpha_slider_2 = 91;
	beta_slider_2 = 127;
	threshLambda = 2;
	threshValue = 40;
}

skin_detection::~skin_detection(void)
{
}

void skin_detection::setup(IplImage * rawImage)
{
	hsvImage = cvCreateImage(cvGetSize(rawImage),8,3);
	mainMask = cvCreateImage(cvGetSize(rawImage),8,1);


	CrCbMask = cvCreateImage(cvGetSize(rawImage), 8, 1);


	height = hsvImage->height;
	width = hsvImage->width;
	step = hsvImage->widthStep;
	channels = hsvImage->nChannels;

	createTrackbar( "Cr_min", "CrCb_mask", &alpha_slider, alpha_slider_max, NULL );
	createTrackbar( "Cr_max", "CrCb_mask", &beta_slider, alpha_slider_max, NULL );
	createTrackbar( "Cb_min", "CrCb_mask", &alpha_slider_2, alpha_slider_max, NULL );
	createTrackbar( "Cb_max", "CrCb_mask", &beta_slider_2, alpha_slider_max, NULL );
	createTrackbar( "threshV", "cov_mask", &threshValue, 100, NULL );
	createTrackbar( "threshLambda", "cov_mask", &threshLambda, 50, NULL );
}

IplImage* skin_detection::getNRGB(IplImage* rawImage)
{
	IplImage* rgbImage = cvCreateImage(cvGetSize(rawImage), 8, 3);
	rgbImage = rawImage;
	IplImage* imgavg = cvCreateImage(cvGetSize(rawImage), 8, 3);

	for(int x=0; x < rawImage->width ;x++)
	{
		for(int y=0;y < rawImage->height ;y++)
		{
			int redValue = (int)((unsigned char*)(rgbImage->imageData + rgbImage->widthStep*(y)))[(x)*3+0]; 
			int greenValue = (int)((unsigned char*)(rgbImage->imageData + rgbImage->widthStep*(y)))[(x)*3+1];
			int blueValue = (int)((unsigned char*)(rgbImage->imageData + rgbImage->widthStep*(y)))[(x)*3+2];

			double sum = redValue+greenValue+blueValue;

			((unsigned char*)(imgavg->imageData + imgavg->widthStep*(y)))[(x)*3+0] = redValue/sum*255;
			((unsigned char*)(imgavg->imageData + imgavg->widthStep*(y)))[(x)*3+1] = greenValue/sum*255;
			((unsigned char*)(imgavg->imageData + imgavg->widthStep*(y)))[(x)*3+2] = blueValue/sum*255;
		}
	}
	return imgavg;
}

void skin_detection::covariationMask(IplImage *rawImage, double threshL, unsigned int threshV) {

	if (this->covMask) {
		cvReleaseImage(&covMask);
	}

	cvCvtColor(rawImage,hsvImage,COLOR_RGB2HSV); //convert to HSV

	covMask = cvCreateImage(cvGetSize(rawImage), 8, 1);
	uchar* data = (uchar *)hsvImage->imageData; //odwolanie do wartosci pixela
	int stepMask = covMask->widthStep; 
	uchar* dataMask = (uchar *)covMask->imageData; //odwolanie do wartosci pixela maski

	double temp[2];

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {

			cvmSet(Xms,0,0,(double)data[i*step+j*channels+0] - cvmGet(Ms,0,0));
			cvmSet(Xms,0,1,(double)data[i*step+j*channels+1] - cvmGet(Ms,0,1));

			temp[0] = cvmGet(Xms,0,0);
			temp[1] = cvmGet(Xms,0,1);

			cvmSet(XmsT,0,0,cvmGet(Xms,0,0));
			cvmSet(XmsT,1,0,cvmGet(Xms,0,1));

			cvmSet(XmsCsInv,0,0,(cvmGet(Xms,0,0) * cvmGet(CsInv,0,0)) + (cvmGet(Xms,0,1) * cvmGet(CsInv,1,0)));
			cvmSet(XmsCsInv,0,1,(cvmGet(Xms,0,0) * cvmGet(CsInv,0,1)) + (cvmGet(Xms,0,1) * cvmGet(CsInv,1,1)));

			cvmSet(XmsCsInvXmsT,0,0,(cvmGet(XmsCsInv,0,0) * cvmGet(XmsT,0,0)) + (cvmGet(XmsCsInv,0,1) * cvmGet(XmsT,1,0)));

			// sprawdzamy przynaleznosc
			double lambda = cvmGet(XmsCsInvXmsT, 0, 0);

			dataMask[i*stepMask+j] = ((data[i*step+j*channels+2] >= threshV) && (lambda < threshL)) ? 255 : 0;

		}
}

void skin_detection::CrCbthreshholdMask(IplImage *rawImage)
{

	IplImage *YCrCb = cvCreateImage(cvGetSize(rawImage),IPL_DEPTH_8U,3);

	int x = 0 , y = 0;
	int Cr = 0, Cb = 0,w=0,h=0;

	cvCvtColor(rawImage,YCrCb,CV_BGR2YCrCb);
	w = YCrCb->width, h = YCrCb->height;

	for (y = 0; y < h ; y++)
		for (x = 0; x < w ; x++)
		{

			Cr= (int)((unsigned char*)(YCrCb->imageData + YCrCb->widthStep*(y)))[(x)*3+1];
			Cb =(int)((unsigned char*)(YCrCb->imageData + YCrCb->widthStep*(y)))[(x)*3+2];

			if ( (Cr>alpha_slider && Cr<beta_slider ) && (Cb>alpha_slider_2 && Cb<beta_slider_2))
				((unsigned char*)(CrCbMask->imageData + CrCbMask->widthStep*(y)))[(x)] = 255;
			else
				((unsigned char*)(CrCbMask->imageData + CrCbMask->widthStep*(y)))[(x)] = 0;
		}


		cvWaitKey(10);
}

IplImage* skin_detection::mask_skin(IplImage *rawImage)
{
	IplImage* temp = cvCreateImage(cvGetSize(rawImage),8,1);
	covariationMask(rawImage, (int)threshLambda, (int)threshValue); //metoda macierzy kowariacji
	CrCbthreshholdMask(rawImage); // metoda progowania barw z YcrCb
	cvOr(CrCbMask,covMask,temp);
	return temp;
}

